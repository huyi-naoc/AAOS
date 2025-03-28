//
//  scheduler.c
//  AAOS
//
//  Created by huyi on 2024/07/18.
//  Copyright © 2024 National Astronomical Observatories, Chinese Academy of Sciences. All rights reserved.
//

#include "adt.h"
#include "def.h"
#include "rpc.h"
#include "scheduler_def.h"
#include "scheduler.h"
#include "scheduler_r.h"
#include "scheduler_rpc.h"
#include "utils.h"
#include "wrapper.h"

#include <chealpix.h>
#include <cjson/cJSON.h>
#include <mysql/mysql.h>

typedef int (*database_cb_t)(struct __Scheduler *, MYSQL_RES *);

struct TaskInfo {
    uint64_t identifier;
    uint32_t nside;
    uint64_t targ_id;
    int status;
    char *description; /* JSON */
};

struct TargetInfo {
    uint32_t nside;
    uint64_t identifier;
    double ra;
    double dec;
    int status;
    int priority;
    char *name;
    char *description; /* JSON */
};

struct TelescopeInfo {
    uint64_t identifier;
    uint64_t site_id;
    int status;
    double ra;
    double dec;
    char *name;
    char *description; /* JSON */
    void *rpc;
};

struct SiteInfo {
    uint64_t identifier;
    int status;
    char *name;
    double site_lon;
    double site_lat;
    double site_alt;
    double site_ws;
    double site_wd;
    double site_dp;
    double sitetemp;
    double site_rh;
    double site_see;
    double site_bkg;
    pthread_mutex_t mtx;
    pthread_cond_t cond;
    void *rpc;              /*use for communicattion */
};

void *scheduler;

static void
global_register_thread(void *arg, va_list *app)
{
    struct SiteInfo *site = arg;

    uint64_t identifier = va_arg(*app, uint64_t);
    void *rpc = va_arg(*app, void *);

    if (site->rpc != NULL) {
        delete(site->rpc);
    }
    
#ifdef DEBUG
    fprintf(stderr, "%s %s %d: register a site thread to the global thread.\n", __FILE__, __func__, __LINE__);
#endif
    
    site->rpc = rpc;
}

static void
global_push_task_block_to_site(void *arg, va_list *app)
{
    struct SiteInfo *site = arg;

    uint64_t identifier = va_arg(*app, uint64_t);
    const char *block_buf = va_arg(*app, const char *);
    unsigned int type = va_arg(*app, unsigned int);
    uint16_t option;
    int ret;

    option = type;
    if (site->rpc != NULL) {
        protobuf_set(site->rpc, PACKET_COMMAND, SCHEDULER_POP_TASK_BLOCK);
        protobuf_set(site->rpc, PACKET_BUF, block_buf, strlen(block_buf) + 1);
        protobuf_set(site->rpc, PACKET_OPTION, option);
        if ((ret = rpc_call(site->rpc)) != AAOS_OK) {
            delete(site->rpc);
            site->rpc = NULL;
        }
    }
}

static void
site_register_thread(void *arg, va_list *app)
{
    struct TelescopeInfo *telescope = arg;

    uint64_t identifier = va_arg(*app, uint64_t);
    void *rpc = va_arg(*app, void *);

    if (telescope->rpc != NULL) {
        delete(telescope->rpc);
    }

    telescope->rpc = rpc;
}

static uint64_t 
__Scheduler_generate_unique_site_id(struct __Scheduler *self)
{
    uint64_t site_id;

    Pthread_mutex_lock(&self->cnt_mtx);
    site_id = ++self->max_site_id;
    Pthread_mutex_unlock(&self->cnt_mtx);

    return site_id;
}

static uint64_t 
__Scheduler_generate_unique_telescope_id(struct __Scheduler *self, uint64_t site_id)
{
    uint64_t tel_id;

    Pthread_mutex_lock(&self->cnt_mtx);
    tel_id = ++self->max_telescope_id;
    Pthread_mutex_unlock(&self->cnt_mtx);

    return tel_id |= site_id<<16;
}

static uint64_t 
__Scheduler_generate_unique_task_id(struct __Scheduler *self, uint64_t site_id, uint64_t tel_id)
{
    uint64_t task_id;
    
    Pthread_mutex_lock(&self->cnt_mtx);
    task_id = ++self->max_task_id;
    Pthread_mutex_unlock(&self->cnt_mtx);
    tel_id&= ~(0xFFFFULL<<16)&site_id;

    return task_id|(site_id<<48)|(tel_id<<32);
}

static uint64_t
__scheduler_generate_unique_target_id(uint32_t nside, double ra, double dec)
{
    long hp_nside, hp_iring;
    uint64_t identifier;
    double theta, phi;

    hp_nside = nside;
    theta = ra * PI / 180.;
    phi = (90. - dec) * PI / 180.;
    ang2pix_ring(hp_nside, theta, phi, &hp_iring);
    identifier = hp_iring;

    return identifier;
}

/*
 * Scheduler Class.
 */
static void 
__scheduler_create_sql(int command, uint64_t identifier, const char *table, char *sql, size_t size, ...)
{
    va_list ap;
    va_start(ap, size);
    
    double timestamp;
    
    switch (command) {
        case SCHEDULER_DELETE_SITE_BY_ID:
        {
            timestamp = va_arg(ap, double);
#ifdef LINUX
            snprintf(sql, size, "UPDATE %s SET status=%d, timestam=%lf WHERE site_id=%lu", table, SCHEDULER_STATUS_DELETE, timestamp, identifier);
#else
            snprintf(sql, size, "UPDATE %s SET status=%d, timestam=%lf WHERE site_id=%llu", table, SCHEDULER_STATUS_DELETE, timestamp, identifier);
#endif
        }
            break;
        case SCHEDULER_MASK_SITE_BY_ID:
        {
            timestamp = va_arg(ap, double);
#ifdef LINUX
            snprintf(sql, size, "UPDATE %s SET status=%d, timestam=%lf WHERE site_id=%lu", table, SCHEDULER_STATUS_MASKED, timestamp, identifier);
#else
            snprintf(sql, size, "UPDATE %s SET status=%d, timestam=%lf WHERE site_id=%llu", table, SCHEDULER_STATUS_MASKED, timestamp, identifier);
#endif
        }
            break;
        case SCHEDULER_UNMASK_SITE_BY_ID:
        {
            timestamp = va_arg(ap, double);
#ifdef LINUX
            snprintf(sql, size, "UPDATE %s SET status=%d, timestam=%lf WHERE site_id=%lu", table, SCHEDULER_STATUS_OK, timestamp, identifier);
#else
            snprintf(sql, size, "UPDATE %s SET status=%d, timestam=%lf WHERE site_id=%llu", table, SCHEDULER_STATUS_OK, timestamp, identifier);
#endif
        }
            break;
        case SCHEDULER_ADD_SITE:
        {
            uint64_t site_id;
            const char *name;
            int status = SCHEDULER_STATUS_OK;
            double site_lon, site_lat, site_alt, timestamp;
            name = va_arg(ap, const char *);
            site_id = va_arg(ap, uint64_t);
            site_lon = va_arg(ap, double);
            site_lat = va_arg(ap, double);
            site_alt = va_arg(ap, double);
            timestamp = va_arg(ap, double);
#ifdef LINUX
            snprintf(sql, size, "INSERT INTO %s (sitename, site_id, status, site_lat, site_lon, site_alt, timestam) VALUES (\"%s\", %lu, %d, %lf, %lf, %lf, %lf)", table, name, site_id, status, site_lon, site_lat, site_alt, timestamp);
#else
            snprintf(sql, size, "INSERT INTO %s (sitename, site_id, status, site_lat, site_lon, site_alt, timestam) VALUES (\"%s\", %llu, %d, %lf, %lf, %lf, %lf)", table, name, site_id, status, site_lon, site_lat, site_alt, timestamp);
#endif
        }
            break;
        case SCHEDULER_DELETE_TELESCOPE_BY_ID:
            timestamp = va_arg(ap, double);
#ifdef LINUX
            snprintf(sql, size, "UPDATE %s SET status=%d, timestam=%lf WHERE tel_id=%lu", table, SCHEDULER_STATUS_DELETE, timestamp, identifier);
#else
            snprintf(sql, size, "UPDATE %s SET status=%d, timestam=%lf WHERE tel_id=%llu", table, SCHEDULER_STATUS_DELETE, timestamp, identifier);
#endif
            break;
        case SCHEDULER_MASK_TELESCOPE_BY_ID:
            timestamp = va_arg(ap, double);
#ifdef LINUX
            snprintf(sql, size, "UPDATE %s SET status=%d, timestam=%lf WHERE tel_id=%lu", table, SCHEDULER_STATUS_MASKED, timestamp, identifier);
#else
            snprintf(sql, size, "UPDATE %s SET status=%d, timestam=%lf WHERE tel_id=%llu", table, SCHEDULER_STATUS_MASKED, timestamp, identifier);
#endif
            break;
        case SCHEDULER_UNMASK_TELESCOPE_BY_ID:
            timestamp = va_arg(ap, double);
#ifdef LINUX
            snprintf(sql, size, "UPDATE %s SET status=%d, timestam=%lf WHERE tel_id=%lu", table, SCHEDULER_STATUS_OK, timestamp, identifier);
#else
            snprintf(sql, size, "UPDATE %s SET status=%d, timestam=%lf WHERE tel_id=%llu", table, SCHEDULER_STATUS_OK, timestamp, identifier);
#endif
            break;
        case SCHEDULER_ADD_TELESCOPE:
        {
            uint64_t tel_id, site_id;
            const char *name, *description;
            int status = SCHEDULER_STATUS_OK;
            name = va_arg(ap, const char *);
            tel_id = va_arg(ap, uint64_t);
            site_id = va_arg(ap, uint64_t);
            description = va_arg(ap, const char *);
            timestamp = va_arg(ap, double);
#ifdef LINUX
            snprintf(sql, size, "INSERT INTO %s (telescop, tel_id, site_id, tel_des, status, timestam) VALUES (\"%s\", %lu, %lu, \'%s\', %d, %lf)", table, name, tel_id, site_id, description, status, timestamp);
#else
            snprintf(sql, size, "INSERT INTO %s (telescop, tel_id, site_id, tel_des, status, timestam) VALUES (\"%s\", %llu, %llu, \'%s\', %d, %lf)", table, name, tel_id, site_id, description, status, timestamp);
#endif
        }
            break;
        case SCHEDULER_DELETE_TARGET_BY_ID:
        {
            uint32_t nside = va_arg(ap, uint32_t);
            timestamp = va_arg(ap, double);
#ifdef LINUX
            snprintf(sql, size, "UPDATE %s SET status=%d, timestam=%lf WHERE targ_id=%lu AND nside=%u", table, SCHEDULER_STATUS_DELETE, timestamp, identifier, nside);
#else
            snprintf(sql, size, "UPDATE %s SET status=%d, timestam=%lf WHERE targ_id=%llu AND nside=%u", table, SCHEDULER_STATUS_DELETE, timestamp, identifier, nside);
#endif
        }
            break;
        case SCHEDULER_MASK_TARGET_BY_ID:
        {
            uint32_t nside = va_arg(ap, uint32_t);
            timestamp = va_arg(ap, double);
#ifdef LINUX
            snprintf(sql, size, "UPDATE %s SET status=%d, timestam=%lf WHERE targ_id=%lu AND nside=%u", table, SCHEDULER_STATUS_MASKED, timestamp, identifier, nside);
#else
            snprintf(sql, size, "UPDATE %s SET status=%d, timestam=%lf WHERE targ_id=%llu AND nside=%u", table, SCHEDULER_STATUS_MASKED, timestamp, identifier, nside);
#endif
        }
            break;
        case SCHEDULER_UNMASK_TARGET_BY_ID:
        {
            uint32_t nside = va_arg(ap, uint32_t);
            timestamp = va_arg(ap, double);
#ifdef LINUX
            snprintf(sql, size, "UPDATE %s SET status=%d, timestam=%lf WHERE targ_id=%lu AND nside=%u", table, SCHEDULER_STATUS_MASKED, timestamp, identifier, nside);
#else
            snprintf(sql, size, "UPDATE %s SET status=%d, timestam=%lf WHERE targ_id=%llu AND nside=%u", table, SCHEDULER_STATUS_MASKED, timestamp, identifier, nside);
#endif
        }
            break;
        case SCHEDULER_ADD_TARGET:
        {
            uint32_t nside;
            uint64_t targ_id;
            const char *name, *description;
            int status = SCHEDULER_STATUS_OK, priority;
            double ra, dec;
            
            name = va_arg(ap, const char *);
            targ_id = va_arg(ap, uint64_t);
            nside = va_arg(ap, uint32_t);
            ra = va_arg(ap, double);
            dec = va_arg(ap, double);
            priority = va_arg(ap, int);
            timestamp = va_arg(ap, double);
            if (name != NULL) {
#ifdef LINUX
                snprintf(sql, size, "INSERT INTO %s (targname, targ_id, nside, ra_tag, dec_tag, status, priority, timestam) VALUES (\"%s\", %lu, %u, %lf, %lf, %d, %d, %lf)", table, name, targ_id, nside, ra, dec, status, priority, timestamp);
#else
                snprintf(sql, size, "INSERT INTO %s (targname, targ_id, nside, ra_tag, dec_tag, status, priority, timestam) VALUES (\"%s\", %llu, %u, %lf, %lf, %d, %d, %lf)", table, name, targ_id, nside, ra, dec, status, priority, timestamp);
#endif
            } else {
#ifdef LINUX
                snprintf(sql, size, "INSERT INTO %s (targ_id, nside, ra_tag, dec_tag, status, priority, timestam) VALUES (%lu, %u, %lf, %lf, %d, %d, %lf)", table, targ_id, nside, ra, dec, status, priority, timestamp);
#else
                snprintf(sql, size, "INSERT INTO %s (targ_id, nside, ra_tag, dec_tag, status, priority, timestam) VALUES (%llu, %u, %lf, %lf, %d, %d, %lf)", table, targ_id, nside, ra, dec, status, priority, timestamp);
#endif
            }
        }
            break;
        case SCHEDULER_ADD_TASK_RECORD:
            {
                uint64_t site_id, tel_id, targ_id, task_id;
                uint32_t nside;
                int status;
                const char *description;
                double obstime = -1.;
                task_id = va_arg(ap, uint64_t);
                targ_id = va_arg(ap, uint64_t);
                nside = va_arg(ap, uint32_t);
                tel_id = va_arg(ap, uint64_t);
                site_id = va_arg(ap, uint64_t);
                status = va_arg(ap, int);
                description = va_arg(ap, const char *);
                timestamp = va_arg(ap, double);
#ifdef LINUX
                snprintf(sql, size, "INSERT INTO %s (task_id, targ_id, nside, tel_id, site_id, status, task_des, obstime, timestam) VALUES (%lu, %lu, %u, %lu, %lu, %d, \'%s\', %lf, %lf)", table, task_id, targ_id, nside, tel_id, site_id, status, description, obstime, timestamp);
#else
                snprintf(sql, size, "INSERT INTO %s (task_id, targ_id, nside, tel_id, site_id, status, task_des, obstime, timestam) VALUES (%llu, %llu, %u, %llu, %llu, %d, \'%s\', %lf, %lf)", table, task_id, targ_id, nside, tel_id, site_id, status, description, obstime, timestamp);
#endif
            }
            break;
        case SCHEDULER_UPDATE_TASK_RECORD:
        {
            const char *description = va_arg(ap, const char *);
            double timestamp = va_arg(ap, double);
#ifdef LINUX
            snprintf(sql, size, "UPDATE %s SET description='%s',timestam=%lf WHERE task_id=%lu", table, description, timestamp, identifier);
#else
            snprintf(sql, size, "UPDATE %s SET description='%s',timestam=%lf WHERE task_id=%llu", table, description, timestamp, identifier);
#endif
        }
            break;
        case SCHEDULER_UPDATE_SITE_STATUS:
            {
                int status = va_arg(ap, int);
                timestamp = va_arg(ap, double);
#ifdef LINUX
                snprintf(sql, size, "UPDATE %s SET status=%d,timestam=%lf WHERE site_id=%lu", table, status, timestamp, identifier);
#else
                snprintf(sql, size, "UPDATE %s SET status=%d,timestam=%lf WHERE site_id=%llu", table, status, timestamp, identifier);
#endif
            }
            break;
        case SCHEDULER_UPDATE_TELESCOPE_STATUS:
            {
                int status = va_arg(ap, int);
                timestamp = va_arg(ap, double);
#ifdef LINUX
                snprintf(sql, size, "UPDATE %s SET status=%d,timestam=%lf WHERE tel_id=%lu", table, status, timestamp, identifier);
#else
                snprintf(sql, size, "UPDATE %s SET status=%d,timestam=%lf WHERE tel_id=%llu", table, status, timestamp, identifier);
#endif
            }
            break;
        case SCHEDULER_UPDATE_TARGET_STATUS:
            {
                int status = va_arg(ap, int);
                uint32_t nside = va_arg(ap, uint32_t);
                timestamp = va_arg(ap, double);
#ifdef LINUX
                snprintf(sql, size, "UPDATE %s SET status=%d,timestam=%lf WHERE task_id=%lu AND nside=%u", table, status, timestamp, identifier, nside);
#else
                snprintf(sql, size, "UPDATE %s SET status=%d,timestam=%lf WHERE task_id=%llu AND nside=%u", table, status, timestamp, identifier, nside);
#endif
            }
            break;
        case SCHEDULER_UPDATE_TASK_STATUS:
            {
                int status = va_arg(ap, int);
                timestamp = va_arg(ap, double);
#ifdef LINUX
                snprintf(sql, size, "UPDATE %s SET status=%d,timestam=%lf WHERE task_id=%lu", table, status, timestamp, identifier);
#else
                snprintf(sql, size, "UPDATE %s SET status=%d,timestam=%lf WHERE task_id=%llu", table, status, timestamp, identifier);
#endif
            }
            break;
        case SCHEDULER_SITE_INIT:
            snprintf(sql, size, "SELECT sitename,site_id,status,site_lon,site_lat,site_alt FROM %s", table);
            break;
        case SCHEDULER_TELESCOPE_INIT:
            snprintf(sql, size, "SELECT telescop,tel_id,site_id,status,tel_des FROM %s", table);
            break;
        case SCHEDULER_TARGET_INIT:
            snprintf(sql, size, "SELECT targname,targ_id,nside,status,ra_targ,dec_targ,priority FROM %s", table);
            break;
        case SCHEDULER_TASK_RECORD_INIT:
            snprintf(sql, size, "SELECT task_id FROM %s", table);
            break;
        default:
            break;
    }
    va_end(ap);
}

static bool
telescope_by_id(void *arg, va_list *app)
{
    struct TelescopeInfo *telescope_info = arg;
    uint64_t identifier = va_arg(*app, uint64_t);

    if (telescope_info->identifier == identifier) {
        return true;
    } else {
        return false;
    }
}

static bool
telescope_by_name(void *arg, va_list *app)
{
    struct TelescopeInfo *telescope_info = arg;

    const char *name = va_arg(*app, const char *);

    if (strcmp(telescope_info->name, name) == 0) {
        return true;
    } else {
        return false;
    }
}

static bool 
site_by_id(void *arg, va_list *app)
{
    struct SiteInfo *site_info = arg;
    uint64_t identifier = va_arg(*app, uint64_t);

    if (site_info->identifier == identifier) {
        return true;
    } else {
        return false;
    }
}

static bool
site_by_name(void *arg, va_list *app)
{
    struct SiteInfo *site_info = arg;
    const char *name;

    name = va_arg(*app,  const char *);

    if (strcmp(site_info->name, name) == 0) {
        return true;
    } else {
        return false;
    }
}

static bool 
target_by_id(void *arg, va_list *app)
{
    struct TargetInfo *target_info = arg;
    uint64_t identifier = va_arg(*app, uint64_t);
    uint32_t nside = va_arg(*app, uint32_t);

    identifier = va_arg(*app, unsigned int);

    if (target_info->identifier == identifier && target_info->nside == nside) {
        return true;
    } else {
        return false;
    }
}

static bool 
target_by_name(void *arg, va_list *app)
{
    struct TargetInfo *target_info = arg;
    const char *name;

    name = va_arg(*app,  const char *);

    if (strcmp(target_info->name, name) == 0) {
        return true;
    } else {
        return false;
    }
}

static void
print_site_list(void *arg, va_list *app)
{
    struct SiteInfo *site_info = arg;
    FILE *fp = va_arg(*app, FILE *);

    cJSON *root_json;
    root_json = cJSON_CreateObject();
    char *string;
    
    if (site_info->name != NULL) {
        cJSON_AddStringToObject(root_json, "sitename", site_info->name);
    }
    
    cJSON_AddNumberToObject(root_json, "site_id", site_info->identifier);
    cJSON_AddNumberToObject(root_json, "status", site_info->status);
    cJSON_AddNumberToObject(root_json, "site_lon", site_info->site_lon);
    cJSON_AddNumberToObject(root_json, "site_lat", site_info->site_lat);
    cJSON_AddNumberToObject(root_json, "site_alt", site_info->site_alt);

    string = cJSON_Print(root_json);
    cJSON_Delete(root_json);

    fprintf(fp, "%s\n", string);
    free(string);
}

static void
print_telescope_list(void *arg, va_list *app)
{
    struct TelescopeInfo *telescope_info = arg;
    FILE *fp = va_arg(*app, FILE *);
    
    cJSON *root_json, *value_json;
    char *string;
    
    if (telescope_info->description != NULL) {
        root_json = cJSON_Parse(telescope_info->description);
    } else {
        root_json = cJSON_CreateObject();
    }
    
    if (telescope_info->name != NULL && cJSON_GetObjectItemCaseSensitive(root_json, "telescop") == NULL) {
        cJSON_AddStringToObject(root_json, "telescop", telescope_info->name);
    }
    /*
     * If `description` field contain following field,
     * set them to the current one;
     */
    if ((value_json = cJSON_GetObjectItemCaseSensitive(root_json, "tel_id")) == NULL) {
        cJSON_AddNumberToObject(root_json, "tel_id", telescope_info->identifier);
    } else {
        cJSON_SetIntValue(value_json, telescope_info->identifier);
    }
    if ((value_json = cJSON_GetObjectItemCaseSensitive(root_json, "site_id")) == NULL) {
        cJSON_AddNumberToObject(root_json, "site_id", telescope_info->site_id);
    } else {
        cJSON_SetIntValue(value_json, telescope_info->site_id);
    }
    if ((value_json = cJSON_GetObjectItemCaseSensitive(root_json, "status")) == NULL) {
        cJSON_AddNumberToObject(root_json, "status", telescope_info->status);
    } else {
        cJSON_SetIntValue(value_json, telescope_info->status);
    }

    string = cJSON_Print(root_json);
    cJSON_Delete(root_json);

    fprintf(fp, "%s\n", string);
    free(string);
}

static void
print_target_list(void *arg, va_list *app)
{
    struct TargetInfo *target_info = arg;

    FILE *fp = va_arg(*app, FILE *);

    cJSON *root_json, *value_json;
    char *string;
    
    if (target_info->description != NULL) {
        root_json = cJSON_Parse(target_info->description);
    } else {
        root_json = cJSON_CreateObject();
    }
    
    if (target_info->name != NULL && cJSON_GetObjectItemCaseSensitive(root_json, "targname") == NULL) {
        cJSON_AddStringToObject(root_json, "targname", target_info->name);
    }
    if ((value_json = cJSON_GetObjectItemCaseSensitive(root_json, "targ_id")) == NULL) {
        cJSON_AddNumberToObject(root_json, "targ_id", target_info->identifier);
    } else {
        cJSON_SetIntValue(value_json, target_info->identifier);
    }
    if ((value_json = cJSON_GetObjectItemCaseSensitive(root_json, "nside")) == NULL) {
        cJSON_AddNumberToObject(root_json, "nside", target_info->nside);
    } else {
        cJSON_SetIntValue(value_json, target_info->nside);
    }
    if ((value_json = cJSON_GetObjectItemCaseSensitive(root_json, "targ_ra")) == NULL) {
        cJSON_AddNumberToObject(root_json, "targ_ra", target_info->ra);
    } else {
        cJSON_SetIntValue(value_json, target_info->ra);
    }
    if ((value_json = cJSON_GetObjectItemCaseSensitive(root_json, "targ_dec")) == NULL) {
        cJSON_AddNumberToObject(root_json, "targ_dec", target_info->dec);
    } else {
        cJSON_SetIntValue(value_json, target_info->dec);
    }
    if ((value_json = cJSON_GetObjectItemCaseSensitive(root_json, "status")) == NULL) {
        cJSON_AddNumberToObject(root_json, "status", target_info->status);
    } else {
        cJSON_SetIntValue(value_json, target_info->status);
    }
    if ((value_json = cJSON_GetObjectItemCaseSensitive(root_json, "priority")) == NULL) {
        cJSON_AddNumberToObject(root_json, "priority", target_info->priority);
    } else {
        cJSON_SetIntValue(value_json, target_info->priority);
    }
    
    string = cJSON_Print(root_json);
    cJSON_Delete(root_json);
    

    fprintf(fp, "%s\n", string);
    free(string);
}

char *
__scheduler_create_request_json_string(unsigned int command, ...)
{
    char timestamp[TIMESTAMPSIZE];
    cJSON *root_json, *general_json, *site_json, *telescope_json, *value_json;
    char *json_string = NULL;
    va_list ap;
    

    ct_to_iso_str(timestamp, TIMESTAMPSIZE);
    
    va_start(ap, command);
    root_json = cJSON_CreateObject();
    if (command == SCHEDULER_GET_TASK_BY_TELESCOPE_ID) {
        uint64_t identifier = va_arg(ap, uint64_t);
        general_json = cJSON_CreateObject();
        cJSON_AddItemToObject(root_json, "GENERAL-INFO", general_json);
        cJSON_AddStringToObject(general_json, "operate", "request");
        cJSON_AddStringToObject(general_json, "timestam", timestamp);
        telescope_json = cJSON_CreateObject();
        cJSON_AddItemToObject(root_json, "TELESCOPE-INFO", telescope_json);
        cJSON_AddNumberToObject(telescope_json, "tel_id", identifier);
        json_string = cJSON_Print(root_json);
    } else if (command == SCHEDULER_GET_TASK_BY_TELESCOPE_NAME) {
        const char  *name = va_arg(ap, const char *);
        general_json = cJSON_CreateObject();
        cJSON_AddItemToObject(root_json, "GENERAL-INFO", general_json);
        cJSON_AddItemToObject(root_json, "GENERAL-INFO", general_json);
        cJSON_AddStringToObject(general_json, "operate", "request");
        cJSON_AddStringToObject(general_json, "timestam", timestamp);
        telescope_json = cJSON_CreateObject();
        cJSON_AddItemToObject(root_json, "TELESCOPE-INFO", telescope_json);
        cJSON_AddStringToObject(telescope_json, "telescop", name);
        json_string = cJSON_Print(root_json);
    } else if (command == SCHEDULER_POP_TASK_BLOCK) {
        general_json = cJSON_CreateObject();
        cJSON_AddItemToObject(root_json, "GENERAL-INFO", general_json);
        cJSON_AddStringToObject(general_json, "operate", "request");
        cJSON_AddStringToObject(general_json, "timestam", timestamp);
        json_string = cJSON_Print(root_json);
    } else if (command == SCHEDULER_TASK_BLOCK_ACK) {
        general_json = cJSON_CreateObject();
        cJSON_AddItemToObject(root_json, "GENERAL-INFO", general_json);
        cJSON_AddStringToObject(general_json, "operate", "acknowledge");
        cJSON_AddStringToObject(general_json, "timestam", timestamp);
        json_string = cJSON_Print(root_json);
    } else if (command == SCHEDULER_PUSH_TASK_BLOCK) {
        general_json = cJSON_CreateObject();
        cJSON_AddItemToObject(root_json, "GENERAL-INFO", general_json);
        value_json = cJSON_CreateString("request");
        cJSON_AddStringToObject(general_json, "operate", "request");
        cJSON_AddStringToObject(general_json, "timestam", timestamp);
        json_string = cJSON_Print(root_json);
    } else if (command == SCHEDULER_DELETE_SITE_BY_ID) {
        uint64_t identifier = va_arg(ap, uint64_t);;
        unsigned int status;
        status = SCHEDULER_STATUS_DELETE;
        general_json = cJSON_CreateObject();
        cJSON_AddItemToObject(root_json, "GENERAL-INFO", general_json);
        cJSON_AddStringToObject(general_json, "operate", "update");
        cJSON_AddStringToObject(general_json, "timestam", timestamp);
        site_json = cJSON_CreateObject();
        cJSON_AddItemToObject(root_json, "SITE-INFO", site_json);
        cJSON_AddNumberToObject(site_json, "site_id", identifier);
        cJSON_AddNumberToObject(site_json, "status", status);
        json_string = cJSON_Print(root_json);
    } else if (command == SCHEDULER_MASK_SITE_BY_ID) {
        uint64_t identifier;
        unsigned int status;
        identifier = va_arg(ap, uint64_t);
        status = SCHEDULER_STATUS_MASKED;
        general_json = cJSON_CreateObject();
        cJSON_AddItemToObject(root_json, "GENERAL-INFO", general_json);
        cJSON_AddStringToObject(general_json, "operate", "update");
        cJSON_AddStringToObject(general_json, "timestam", timestamp);
        site_json = cJSON_CreateObject();
        cJSON_AddItemToObject(root_json, "SITE-INFO", site_json);
        cJSON_AddNumberToObject(site_json, "site_id", identifier);
        cJSON_AddNumberToObject(site_json, "status", status);
        json_string = cJSON_Print(root_json);
    } else if (command == SCHEDULER_UNMASK_SITE_BY_ID) {
        uint64_t identifier;
        unsigned int status;
        identifier = va_arg(ap, uint64_t);
        status = SCHEDULER_STATUS_OK;
        general_json = cJSON_CreateObject();
        cJSON_AddItemToObject(root_json, "GENERAL-INFO", general_json);
        cJSON_AddStringToObject(general_json, "operate", "update");
        cJSON_AddStringToObject(general_json, "timestam", timestamp);
        site_json = cJSON_CreateObject();
        cJSON_AddItemToObject(root_json, "SITE-INFO", site_json);
        cJSON_AddNumberToObject(site_json, "site_id", identifier);
        cJSON_AddNumberToObject(site_json, "status", status);
        json_string = cJSON_Print(root_json);
    }

    cJSON_Delete(root_json);
    va_end(ap);
    return json_string;
}

int
__Scheduler_ipc_write(struct __Scheduler *self, void *buf, uint32_t length)
{
    int cfd;
    ssize_t nwrite;
    
    if ((cfd = Un_stream_connect(self->sock_file)) < 0) {
        switch (errno) {
            case EACCES:
                return AAOS_EACCES;
                break;
            case ENOENT:
                return AAOS_ENOENT;
                break;
            case ECONNREFUSED:
                return AAOS_ECONNREFUSED;
                break;
            default:
                return AAOS_ERROR;
                break;
        }
    }
    
    if ((nwrite = Writen(cfd, buf, length)) < 0) {
        Close(cfd);
        return AAOS_ERROR;
    }

    Close(cfd);  
    return AAOS_OK;
}

static int
__Scheduler_ipc_write_and_read(struct __Scheduler *self, const char *string, char *res, size_t size, size_t *len)
{
    int cfd; 
    uint32_t length;
    ssize_t nread, nwrite;
    char buf[BUFSIZE];
    
    if ((cfd = Un_stream_connect(self->sock_file)) < 0) {
        return AAOS_ERROR;
    }

    length = (uint32_t) strlen(string);
    memcpy(buf, &length, sizeof(uint32_t));
    snprintf(buf + sizeof(uint32_t), BUFSIZE - sizeof(uint32_t), "%s", string);

    if ((nwrite = Writen(cfd, buf, length + sizeof(uint32_t))) < 0) {
        Close(cfd);
        return AAOS_ERROR;
    }

    if ((nread = Readn(cfd, &length, sizeof(uint32_t))) < 0) {
        Close(cfd);
        return AAOS_ERROR;
        
    }
    if ((nread = Readn(cfd, res, length)) < 0) {
        Close(cfd);
        return AAOS_ERROR;
        
    }

    if (len != NULL) {
        *len = length;
    }

    Close(cfd);
      
    return AAOS_OK;
}

static void 
cleanup_site_info(void *site)
{
    struct SiteInfo *site_ = (struct SiteInfo *) site;

    free(site_->name);
    
    if (site_->rpc != NULL) {
        delete(site_->rpc);
    }
    Pthread_mutex_destroy(&site_->mtx);
    Pthread_cond_destroy(&site_->cond);
    free(site);
}

static void 
cleanup_telescope_info(void *telescope)
{
    struct TelescopeInfo *telescope_ = (struct TelescopeInfo *) telescope;

    free(telescope_->name);
    free(telescope_->description);

    if (telescope_->rpc != NULL) {
        delete(telescope_->rpc);
    }
    free(telescope_);
}

static void
cleanup_target_info(void *target)
{
    struct TargetInfo *target_ = target;

    free(target_->name);
    free(target_->description);

    free(target_);
}

static void 
cleanup_task_info(void *task)
{
    struct TaskInfo *task_ = (struct TaskInfo *) task;

    free(task_->description);
    free(task_);
}

static int
__Scheduler_site_init_cb(struct __Scheduler *self, MYSQL_RES *res)
{
    struct SiteInfo *site;
    MYSQL_ROW row;
    unsigned long *lengths;
    unsigned long long n_rows;
    size_t i, cnt = 0; 
    uint64_t *identifiers;

    self->site_list = new(ThreadsafeList(), cleanup_site_info);

    n_rows = mysql_num_rows(res);
    identifiers = (uint64_t *) Malloc(sizeof(uint64_t) * n_rows);

    while ((row = mysql_fetch_row(res))) {
        site = (struct SiteInfo *) Malloc(sizeof(struct SiteInfo));
        Pthread_mutex_init(&site->mtx, NULL);
        Pthread_cond_init(&site->cond, NULL);
        memset(site, '\0', sizeof(struct SiteInfo));
        lengths = mysql_fetch_lengths(res);
        if (lengths[0] != 0) {
            site->name = (char *) Malloc(lengths[0] + 1);
            snprintf(site->name, lengths[0] + 1, "%s", row[0]);
        }
        if (lengths[1] != 0) {
            site->identifier = strtoull(row[1], NULL, 0);
            identifiers[cnt] = site->identifier;
        }
        if (lengths[2] != 0) {
            site->status = atoi(row[2]);
        }
        if (lengths[3] != 0) {
            site->site_lon = atof(row[3]);
        }
        if (lengths[4] != 0) {
            site->site_lat = atof(row[4]);
        }
        if (lengths[5] != 0) {
            site->site_alt = atof(row[5]);
        }
        cnt++;
        threadsafe_list_push_front(self->site_list, site);
    }
    if (cnt > 0) {
        self->max_site_id = identifiers[0];
        for (i = 1; i < cnt; i++) {
            if (self->max_site_id < identifiers[i]) {
                self->max_site_id = identifiers[i];
            }
        }
    }
    free(identifiers);
    return AAOS_OK;
} 

static int
__Scheduler_telescope_init_cb(struct __Scheduler *self, MYSQL_RES *res)
{
    struct TelescopeInfo *telescope;
    MYSQL_ROW row;
    unsigned long *lengths;
    uint64_t *identifiers;
    unsigned long long n_rows; 
    size_t i, cnt = 0;

    self->telescope_list = new(ThreadsafeList(), cleanup_telescope_info);

    n_rows = mysql_num_rows(res);
    identifiers = (uint64_t *) Malloc(sizeof(uint64_t) * n_rows);

    while ((row = mysql_fetch_row(res))) {
        telescope = (struct TelescopeInfo *) Malloc(sizeof(struct TelescopeInfo));
        memset(telescope, '\0', sizeof(struct TelescopeInfo));
        lengths = mysql_fetch_lengths(res);
        if (lengths[0] != 0) {
            telescope->name = (char *) Malloc(lengths[0] + 1);
            snprintf(telescope->name, lengths[0] + 1, "%s", row[0]);
        }
        if (lengths[1] != 0) {
            telescope->identifier = strtoull(row[1], NULL, 0);
            identifiers[cnt] = telescope->identifier;
        }
        if (lengths[2] != 0) {
            telescope->site_id = strtoull(row[2], NULL, 0);
        }
        if (lengths[3] != 0) {
            telescope->status = atoi(row[3]);
        }       
        if (lengths[4] != 0) {
            telescope->description = (char *) Malloc(lengths[4] + 1);
            snprintf(telescope->description, lengths[4] + 1, "%s", row[4]);
        }
        threadsafe_list_push_front(self->telescope_list, telescope);
        cnt++;
    }

    if (cnt > 0 && self->type == SCHEDULER_TYPE_SITE) {
        self->max_telescope_id = identifiers[0]&(~(0xFFFFULL<<16));
        for (i = 1; i < cnt; i++) {
            if (self->max_telescope_id < (identifiers[i]&(~(0xFFFFULL<<16)))) {
                self->max_telescope_id = identifiers[i]&(~(0xFFFFULL<<16));
            }
        }
    }

    free(identifiers);

    return AAOS_OK;
}

static int
__Scheduler_target_init_cb(struct __Scheduler *self, MYSQL_RES *res)
{
    struct TargetInfo *target;
    MYSQL_ROW row;
    unsigned long *lengths;

    self->target_list = new(ThreadsafeList(), cleanup_target_info);

    while ((row = mysql_fetch_row(res))) {
        target = (struct TargetInfo *) Malloc(sizeof(struct TargetInfo));
        memset(target, '\0', sizeof(struct TargetInfo));
        lengths = mysql_fetch_lengths(res);
        if (lengths[0] != 0) {
            target->name = (char *) Malloc(lengths[0] + 1);
            snprintf(target->name, lengths[0] + 1, "%s", row[0]);
        }
        if (lengths[1] != 0) {
            target->identifier = strtoull(row[1], NULL, 0);
        }
        if (lengths[2] != 0) {
            target->nside = (uint32_t) strtoul(row[2], NULL, 0);
        }
        if (lengths[3] != 0) {
            target->status = atoi(row[3]);
        }     
        if (lengths[4] != 0) {
            target->ra = atof(row[4]);
        }  
        if (lengths[5] != 0) {
            target->ra = atof(row[5]);
        }
        if (lengths[6] != 0) {
            target->priority = atoi(row[6]);
        }
        threadsafe_list_push_front(self->target_list, target);
    }

    return AAOS_OK;
}

static int
__Scheduler_task_init_cb(struct __Scheduler *self, MYSQL_RES *res)
{
    MYSQL_ROW row;
    uint64_t *identifiers;
    unsigned long long n_rows;
    size_t i, cnt = 0;
    unsigned long *lengths;

    n_rows = mysql_num_rows(res);
    identifiers = (uint64_t *) Malloc(sizeof(uint64_t) * n_rows);

    while ((row = mysql_fetch_row(res))) {
        lengths = mysql_fetch_lengths(res);
        if (lengths[0] != 0) {
            identifiers[cnt] = strtoull(row[0], NULL, 0);
            identifiers[cnt] = identifiers[cnt]&(~(0xFFFFFFFFULL<<32));
            cnt++;
        }
    }
    self->max_task_id = identifiers[0];
    for (i = 1; i < cnt; i++) {
        if (self->max_site_id < identifiers[i]) {
            self->max_site_id = identifiers[i];
        }
    }

    free(identifiers);

    return AAOS_OK;
} 


 static int
 __Scheduler_database_query(struct __Scheduler *self, const char *stmt_str, database_cb_t cb)
 {
    MYSQL mysql;
    int ret = AAOS_OK;
    
    if (mysql_init(&mysql) == NULL) {
#ifdef DEBUG
        fprintf(stderr, "%s %s %d: mysql initialization failed.\n", __FILE__, __func__, __LINE__);
#endif
        return AAOS_ENOMEM;
    }
    if ((mysql_real_connect(&mysql, self->db_host, self->db_user, self->db_passwd, self->db_name, 0, NULL, 0)) == NULL) {
#ifdef DEBUG
        fprintf(stderr, "%s %s %d: mysql connection failed.\n", __FILE__, __func__, __LINE__);
#endif
        mysql_close(&mysql);
        return AAOS_ERROR;
    }

    if ((ret = mysql_real_query(&mysql, stmt_str, strlen(stmt_str))) != 0) {
#ifdef DEBUG
        fprintf(stderr, "%s %s %d: mysql query `%s` failed.\n", __FILE__, __func__, __LINE__, stmt_str);
#endif
        mysql_close(&mysql);
        return AAOS_ERROR;
    }

    if (cb != NULL) {
        MYSQL_RES *res = mysql_store_result(&mysql);
        if (res == NULL) {
            mysql_close(&mysql);
            if (mysql_errno(&mysql) != 0) {
                return AAOS_ERROR;
            } else {
                return AAOS_OK;
            }
        }
        ret = cb(self, res);
        mysql_free_result(res);
    }

    mysql_close(&mysql);

    return ret;
 }

static void
__Scheduler_get_task_post_process(struct __Scheduler *self, char *buf, size_t size)
{
    char *json_string;
    cJSON *root_json, *target_json, *site_json;
    
    if ((root_json = cJSON_Parse(buf)) == NULL) {
        return;
    }
    
    if ((target_json = cJSON_GetObjectItemCaseSensitive(root_json, "TARGET-INFO")) != NULL) {
        
    }
    
    if ((site_json = cJSON_GetObjectItemCaseSensitive(root_json, "SITE-INFO")) != NULL) {
        
    } else {
        site_json = cJSON_CreateObject();
        cJSON_AddItemToObject(root_json, "SITE-INFO", site_json);
        struct SiteInfo *site = self->site;
        if (site != NULL) {
            cJSON_AddStringToObject(site_json, "sitename", site->name);
            cJSON_AddNumberToObject(site_json, "site_id", site->identifier);
            cJSON_AddNumberToObject(site_json, "site_lon", site->site_lon);
            cJSON_AddNumberToObject(site_json, "site_lat", site->site_lat);
            cJSON_AddNumberToObject(site_json, "site_alt", site->site_alt);
            Pthread_mutex_lock(&site->mtx);
            cJSON_AddNumberToObject(site_json, "sitetemp", site->sitetemp);
            cJSON_AddNumberToObject(site_json, "site_ws", site->site_ws);
            cJSON_AddNumberToObject(site_json, "site_wd", site->site_wd);
            cJSON_AddNumberToObject(site_json, "site_dp", site->site_dp);
            cJSON_AddNumberToObject(site_json, "site_see", site->site_see);
            cJSON_AddNumberToObject(site_json, "site_bkg", site->site_bkg);
            Pthread_mutex_unlock(&site->mtx);
        }
    }
}

int
__scheduler_get_task_by_telescope_id(void *_self, uint64_t identifier, char *result, size_t size, size_t *length, unsigned int *type)
{
    const struct __SchedulerClass *class = (const struct __SchedulerClass *) classOf(_self);
    
    if (isOf(class, __SchedulerClass()) && class->get_task_by_telescope_id.method) {
        return ((int (*)(void *, uint64_t, char *, size_t, size_t *, unsigned  *)) class->get_task_by_telescope_id.method)(_self, identifier, result, size, length, type);
    } else {
        int result;
        forward(_self, &result, (Method) __scheduler_get_task_by_telescope_id, "get_task_by_telescope_id", _self, identifier, result, size, type);
        return result;
    }
}

static int
__Scheduler_get_task_by_telescope_id(void *_self, uint64_t identifier, char *result, size_t size, size_t *length, unsigned int *type)
{
    struct __Scheduler *self = cast(__Scheduler(), _self);

    struct TelescopeInfo *telescope;
    char *json_string;
    int ret = AAOS_ERROR;
    uint64_t task_id;
    cJSON *root_json, *general_json;
    struct SiteInfo *site = self->site;
	    
    if (self->type == SCHEDULER_TYPE_SITE) {
        telescope = threadsafe_list_find_first_if(self->telescope_list, telescope_by_id, identifier);
        if (telescope == NULL) {
            return AAOS_ENOTFOUND;
        }
        if (strcmp(self->ipc_model, "unix_stream") == 0) {
            json_string = __scheduler_create_request_json_string(SCHEDULER_GET_TASK_BY_TELESCOPE_ID, identifier);
            ret = __Scheduler_ipc_write_and_read(self, json_string, result, size, length);
            free(json_string);
            if (ret != AAOS_OK) {
                return ret;
            }
            if ((root_json = cJSON_Parse(result)) != NULL && (general_json = cJSON_GetObjectItemCaseSensitive(root_json, "GENERAL-INFO")) != NULL) {
                task_id = __Scheduler_generate_unique_task_id(self, site->identifier, identifier);
                cJSON_AddNumberToObject(general_json, "task_id", task_id);
                json_string = cJSON_Print(root_json);
                snprintf(result, size, "%s", json_string);
                free(json_string);
                if (length != NULL) {
                    *length = strlen(result) + 1;
                }
                __scheduler_add_task_record(self, SCHEDULER_STATUS_GENERATE, result, SCHEDULER_FORMAT_JSON);
            }
            if (type != NULL) {
                *type = SCHEDULER_FORMAT_JSON;
            }
        }
    } else {
        return AAOS_ENOTSUP;
    }
    
    return ret;
}

int
__scheduler_get_task_by_telescope_name(void *_self, const char *name, char *result, size_t size, size_t *length, unsigned int *type)
{
    const struct __SchedulerClass *class = (const struct __SchedulerClass *) classOf(_self);
    
    if (isOf(class, __SchedulerClass()) && class->get_task_by_telescope_name.method) {
        return ((int (*)(void *, const char *, char *, size_t, size_t *, unsigned  *)) class->get_task_by_telescope_name.method)(_self, name, result, size, length, type);
    } else {
        int result;
        forward(_self, &result, (Method) __scheduler_get_task_by_telescope_name, "get_task_by_telescope_name", _self, name, result, size, type);
        return result;
    }
}

static int
__Scheduler_get_task_by_telescope_name(void *_self, const char *name, char *result, size_t size, size_t *length, unsigned int *type)
{
    struct __Scheduler *self = cast(__Scheduler(), _self);

    struct TelescopeInfo *telescope;
    char *json_string;
    int ret = AAOS_ERROR;
    uint64_t identifier, task_id;
    cJSON *root_json, *general_json;
    struct SiteInfo *site = self->site;

    if (self->type == SCHEDULER_TYPE_SITE) {
        telescope = threadsafe_list_find_first_if(self->telescope_list, telescope_by_name, name);
        if (telescope == NULL) {
            return AAOS_ENOTFOUND;
        }
        identifier = telescope->identifier;
        if (strcmp(self->ipc_model, "unix_stream") == 0) {
            json_string = __scheduler_create_request_json_string(SCHEDULER_GET_TASK_BY_TELESCOPE_ID, identifier);
            ret = __Scheduler_ipc_write_and_read(self, json_string, result, size, length);
            free(json_string);
            if ((root_json = cJSON_Parse(result)) != NULL && (general_json = cJSON_GetObjectItemCaseSensitive(root_json, "GENERAL-INFO")) != NULL) {
                task_id = __Scheduler_generate_unique_task_id(self, site->identifier, identifier);
                cJSON_AddNumberToObject(general_json, "task_id", task_id);
                json_string = cJSON_Print(root_json);
                snprintf(result, size, "%s", json_string);
                free(json_string);
                if (length != NULL) {
                    *length = strlen(result) + 1;
                }
                __scheduler_add_task_record(self, SCHEDULER_STATUS_GENERATE, result, SCHEDULER_FORMAT_JSON);
            }
            if (type != NULL) {
                *type = SCHEDULER_FORMAT_JSON;
            }
        }
    } else {
        return AAOS_ENOTSUP;
    }

    return ret;
}

int
__scheduler_pop_task_block(void *_self)
{
    const struct __SchedulerClass *class = (const struct __SchedulerClass *) classOf(_self);
    
    if (isOf(class, __SchedulerClass()) && class->pop_task_block.method) {
        return ((int (*)(void *)) class->pop_task_block.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) __scheduler_pop_task_block, "pop_task_block", _self);
        return result;
    }
}

static int
__Scheduler_pop_task_block(void *_self)
{
    struct __Scheduler *self = cast(__Scheduler(), _self);

    int cfd;
    uint32_t length;
    ssize_t nread, nwrite;
    char buf[BUFSIZE], *block_buf;
    char *json_string;
    int ret;

    if (self->type == SCHEDULER_TYPE_UNIT) {
        return AAOS_ENOTSUP;
    } else if (self->type == SCHEDULER_TYPE_SITE) {
        /*
         * For site scheduler, recieve task block from global scheduler,
         * and then push the block to the site scheduler module.
         */
        struct SiteInfo *site = self->site;
        unsigned int type;
        uint16_t option;
        for (;;) {
            /*
             * Read task block from global scheduler, and push task block to site algorithm.
             */
            if (site->rpc == NULL) {
                __scheduler_set_member(self, "connect_global");
                Nanosleep(5.0);
                continue;
            }
            if ((ret = rpc_process(site->rpc)) == AAOS_OK) {
                protobuf_get(site->rpc, PACKET_OPTION, &option);
                type = option;
                protobuf_get(site->rpc, PACKET_BUF, &block_buf, NULL);
#ifdef DEBUG
                fprintf(stderr, "%s %s %d: Push task block to site scheduling algorithm:\n", __FILE__, __func__, __LINE__);
                fprintf(stderr, "%s\n", block_buf);
#endif
            } else {
#ifdef DEBUG
                if (ret != AAOS_OK) {
                    fprintf(stderr, "%s %s %d: connection to `%s` failed.\n", __FILE__, __func__, __LINE__, self->sock_file);
                }
#endif
                if (ret == AAOS_ECONNREFUSED) {
                    FILE *fp;
                    if ((fp = popen(self->algorithm, "r")) != NULL) {
                        ret = pclose(fp);
#ifdef DEBUG
                        if (ret > 0) {
                            fprintf(stderr, "%s %s %d: execution `%s` failed.\n", __FILE__, __func__, __LINE__, self->algorithm);
                        }
#endif
                        sleep(2);
                    }
                } else if (ret < 0) {
                    __scheduler_set_member(self, "connect_global");
                }
            }
        }
    } else if (self->type == SCHEDULER_TYPE_GLOBAL) {
        block_buf = (char *) Malloc(BUFSIZE * self->max_task_in_block);
        for (; ;) {
            if ((cfd = Un_stream_connect(self->sock_file)) < 0) {
                /*
                 * Invoking scheduler module.
                 */
#ifdef DEBUG
                fprintf(stderr, "%s %s %d: connection to `%s` failed.\n", __FILE__, __func__, __LINE__, self->sock_file);
#endif
                FILE *fp;
                if ((fp = popen(self->algorithm, "r")) != NULL) {
                    ret = pclose(fp);
#ifdef DEBUG
                    if (ret > 0) {
                        fprintf(stderr, "%s %s %d: execution `%s` failed.\n", __FILE__, __func__, __LINE__, self->algorithm);
                    }
#endif
                    sleep(2);
                } else {
#ifdef DEBUG
                    fprintf(stderr, "%s %s %d: popen fialed.\n", __FILE__, __func__, __LINE__);
#endif
                }
                continue;
            }
            json_string = __scheduler_create_request_json_string(SCHEDULER_POP_TASK_BLOCK);
            length = (uint32_t) strlen(json_string) + 1;
            memcpy(buf, &length, sizeof(uint32_t));
            snprintf(buf + sizeof(uint32_t), BUFSIZE - sizeof(uint32_t), "%s", json_string);
            free(json_string);
            if ((nwrite = Writen(cfd, buf, length + sizeof(uint32_t))) < 0) {
                Close(cfd);
                continue;
            }
#ifdef DEBUG
            fprintf(stderr, "%s %s %d: start poping task block.\n", __FILE__, __func__, __LINE__);
#endif
            for (;;) {
                cJSON *root_json, *value_json;
                uint64_t identifier;
                if ((nread = Readn(cfd, &length, sizeof(uint32_t))) < 0) {
                    Close(cfd);
                    break;
                }
                if ((nread = Readn(cfd, block_buf, length)) < 0) {
                    Close(cfd);
                    break;
                }
                json_string =  __scheduler_create_request_json_string(SCHEDULER_TASK_BLOCK_ACK);
                length = (uint32_t) strlen(json_string) + 1;
                memcpy(buf, &length, sizeof(uint32_t));
                snprintf(buf + sizeof(uint32_t), BUFSIZE - sizeof(uint32_t), "%s", json_string);
                free(json_string);
                if ((nwrite = Writen(cfd, buf, sizeof(uint32_t) + length)) < 0) {
                    Close(cfd);
                    break;
                }
                /*
                 * Parse block_buf, and push task block to
                 */
                root_json = cJSON_Parse(block_buf);
#ifdef DEBUG
                fprintf(stderr, "Pop task block from global scheduling algorithm: \n");
                fprintf(stderr, "%s\n", block_buf);
#endif
                if ((value_json = cJSON_GetObjectItemCaseSensitive(root_json, "site_id")) != NULL) {
                    identifier = value_json->valueint;
                    threadsafe_list_operate_first_if(self->site_list, site_by_id, global_push_task_block_to_site, identifier, block_buf, SCHEDULER_FORMAT_JSON);
                } else {
#ifdef DEBUG
                    fprintf(stderr, "`site_id` field is not found in task block.\n");
#endif
                }
                cJSON_Delete(root_json);
            }
            continue;
        }
        free(block_buf);
    } else {
        
    }
    return AAOS_OK;
}

int
__scheduler_push_task_block(void *_self, const char *buf, unsigned int type)
{
    const struct __SchedulerClass *class = (const struct __SchedulerClass *) classOf(_self);
    
    if (isOf(class, __SchedulerClass()) && class->push_task_block.method) {
        return ((int (*)(void *, const char *, unsigned int)) class->push_task_block.method)(_self, buf, type);
    } else {
        int result;
        forward(_self, &result, (Method) __scheduler_push_task_block, "push_task_block", _self, result, type);
        return result;
    }
}

static int
__Scheduler_push_task_block(void *_self, const char *block_buf, unsigned int type)
{
    struct __Scheduler *self = cast(__Scheduler(), _self);

    int ret;
    uint32_t length;
    char *buf, *json_string = NULL, *s;
    cJSON *root_json, *general_json;
    struct timespec tp;
    struct tm time_buf;
    char timestamp[TIMESTAMPSIZE];
    
    if (self->type != SCHEDULER_TYPE_SITE) {
        return AAOS_ENOTSUP;
    }

    if (type == SCHEDULER_FORMAT_JSON) {
        root_json = cJSON_Parse(block_buf);
        if (root_json != NULL && (general_json = cJSON_GetObjectItem(root_json, "GENERAL-INFO")) == NULL) {
            general_json = cJSON_CreateObject();
            cJSON_AddStringToObject(general_json, "operate", "push");
            Clock_gettime(CLOCK_REALTIME, &tp);
            gmtime_r(&tp.tv_sec, &time_buf);
            strftime(timestamp, TIMESTAMPSIZE, "%Y-%m-%dT%H:%M:%S", &time_buf);
            s = timestamp + strlen(timestamp);
            snprintf(s, TIMESTAMPSIZE - strlen(timestamp), ".%06ldZ", tp.tv_nsec / 1000);
            cJSON_AddStringToObject(general_json, "timestam", timestamp);
            cJSON_AddItemToObject(root_json, "GENERAL-INFO", general_json);
            json_string = cJSON_PrintUnformatted(root_json);
            length = (uint32_t) strlen(json_string) + 1;
            buf = (char *) Malloc(length + sizeof(uint32_t));
            memcpy(buf, &length, sizeof(uint32_t));
            snprintf(buf + sizeof(uint32_t), length, "%s", json_string);
            free(json_string);
        } else {
            length = (uint32_t) strlen(block_buf) + 1;
            buf = (char *) Malloc(length + sizeof(uint32_t));
            memcpy(buf, &length, sizeof(uint32_t));
            snprintf(buf + sizeof(uint32_t), length, "%s", block_buf);
        }
        ret = __Scheduler_ipc_write(self, buf, length + sizeof(uint32_t));
        free(buf);
        cJSON_Delete(root_json);
    } else {
        return AAOS_ENOTSUP;
    }
    
    return ret;
}

int
__scheduler_update_status(void *_self, const char *buf, unsigned int type)
{
    const struct __SchedulerClass *class = (const struct __SchedulerClass *) classOf(_self);
    
    if (isOf(class, __SchedulerClass()) && class->update_status.method) {
        return ((int (*)(void *, const char *, unsigned int)) class->update_status.method)(_self, buf, type);
    } else {
        int result;
        forward(_self, &result, (Method) __scheduler_update_status, "update_status", _self, result, type);
        return result;
    }
}

static int
__Scheduler_update_status_json(struct __Scheduler *self, const char *string)
{
    int ret = AAOS_ERROR;
    uint32_t length;
    char buf[BUFSIZE], sql[BUFSIZE];
    MYSQL mysql;
    struct timespec tp;
    double timestamp;
    
    Clock_gettime(CLOCK_REALTIME, &tp);
    timestamp = tp.tv_sec + tp.tv_nsec / 1000000000.;
    
    
    cJSON *root_json;
    const cJSON *sites_json, *telescopes_json, *targets_json, *tasks_json, *site_json, *telescope_json, *target_json, *task_json;

    if ((root_json = cJSON_Parse(string)) == NULL) {
        return AAOS_EBADCMD;
    }
    length = (uint32_t) strlen(string) + 1;
    memcpy(buf, &length, sizeof(uint32_t));
    snprintf(buf + sizeof(uint32_t), length, "%s", string);
    if (mysql_init(&mysql) == NULL) {
        return AAOS_ERROR;
    }
    if (mysql_real_connect(&mysql, self->db_host, self->db_user, self->db_passwd, self->db_name, 0, NULL, 0) != 0) {
        return AAOS_ERROR;
    }
    if (self->type == SCHEDULER_TYPE_GLOBAL && (sites_json = cJSON_GetObjectItemCaseSensitive(root_json, "SITE-INFO")) != NULL) {
        struct SiteInfo *site;
        const cJSON *id, *status;
        if (cJSON_IsArray(sites_json)) {
            cJSON_ArrayForEach(site_json, sites_json) {
                id = cJSON_GetObjectItemCaseSensitive(site_json, "site_id");
                status = cJSON_GetObjectItemCaseSensitive(site_json, "status");
                if (cJSON_IsNumber(id) && cJSON_IsNumber(status)) {
                    site = threadsafe_list_find_first_if(self->site_list, site_by_id, id->valueint);
                    if (site != NULL) {
                        /*
                         * TODO, lock for threadsafe update!
                         */
                        site->status = status->valueint;
                        __scheduler_create_sql(SCHEDULER_UPDATE_SITE_STATUS, id->valueint, self->site_db_table, sql, BUFSIZE, site->status, timestamp);
                        mysql_real_query(&mysql, sql, strlen(sql));
                        
                    }
                }
            }
        } else {
            site_json = sites_json;
            id = cJSON_GetObjectItemCaseSensitive(site_json, "site_id");
            status = cJSON_GetObjectItemCaseSensitive(site_json, "status");
            if (cJSON_IsNumber(id) && cJSON_IsNumber(status)) {
                site = threadsafe_list_find_first_if(self->site_list, site_by_id, id->valueint);
                if (site != NULL) {
                    site->status = status->valueint;
                    __scheduler_create_sql(SCHEDULER_UPDATE_SITE_STATUS, id->valueint, self->site_db_table, sql, BUFSIZE, site->status, timestamp);
                    mysql_real_query(&mysql, sql, strlen(sql));
                }
            }
        }
    }
    if (self->type != SCHEDULER_TYPE_UNIT && (telescopes_json = cJSON_GetObjectItemCaseSensitive(root_json, "TELESCOPE-INFO")) != NULL) {
        struct TelescopeInfo *telescope;
        const cJSON *id, *status;
        if (cJSON_IsArray(telescopes_json)) {
            cJSON_ArrayForEach(telescope_json, telescopes_json) {
                id = cJSON_GetObjectItemCaseSensitive(telescope_json, "tel_id");
                status = cJSON_GetObjectItemCaseSensitive(telescope_json, "status");
                if (cJSON_IsNumber(id) && cJSON_IsNumber(status)) {
                    telescope = threadsafe_list_find_first_if(self->telescope_list, telescope_by_id, id->valueint);
                    if (telescope != NULL) {
                        /*
                         * TODO, lock for threadsafe update!
                         */
                        telescope->status = status->valueint;
                        __scheduler_create_sql(SCHEDULER_UPDATE_TELESCOPE_STATUS, id->valueint, self->telescope_db_table, sql, BUFSIZE, telescope->status, timestamp);
                        mysql_real_query(&mysql, sql, strlen(sql));
                    }
                }
            }
        } else {
            telescope_json = telescopes_json;
            id = cJSON_GetObjectItemCaseSensitive(telescope_json, "tel_id");
            status = cJSON_GetObjectItemCaseSensitive(telescope_json, "status");
            if (cJSON_IsNumber(id) && cJSON_IsNumber(status)) {
                telescope = threadsafe_list_find_first_if(self->telescope_list, telescope_by_id, id->valueint);
                if (telescope != NULL) {
                    telescope->status = status->valueint;
                    __scheduler_create_sql(SCHEDULER_UPDATE_TELESCOPE_STATUS, id->valueint, self->telescope_db_table, sql, BUFSIZE, telescope->status, timestamp);
                    mysql_real_query(&mysql, sql, strlen(sql));
                }
            }
        }
    }
    if (self->type != SCHEDULER_TYPE_UNIT && (targets_json = cJSON_GetObjectItemCaseSensitive(root_json, "TARGET-INFO")) != NULL) {
        struct TargetInfo *target;
        const cJSON *id, *nside, *status;
        if (cJSON_IsArray(targets_json)) {
            cJSON_ArrayForEach(target_json, targets_json) {
                id = cJSON_GetObjectItemCaseSensitive(target_json, "targ_id");
                nside = cJSON_GetObjectItemCaseSensitive(target_json, "nside");
                status = cJSON_GetObjectItemCaseSensitive(target_json, "status");
                if (cJSON_IsNumber(id) && cJSON_IsNumber(status)) {
                    target = threadsafe_list_find_first_if(self->target_list, target_by_id, id->valueint, nside->valueint);
                    if (target != NULL) {
                        /*
                         * TODO, lock for threadsafe update!
                         */
                        target->status = status->valueint;
                        __scheduler_create_sql(SCHEDULER_UPDATE_TARGET_STATUS, id->valueint, self->target_db_table, sql, BUFSIZE, target->status, timestamp);
                        mysql_real_query(&mysql, sql, strlen(sql));
                    }
                }
            }
        } else {
            target_json = targets_json;
            id = cJSON_GetObjectItemCaseSensitive(target_json, "site_id");
            status = cJSON_GetObjectItemCaseSensitive(target_json, "status");
            if (cJSON_IsNumber(id) && cJSON_IsNumber(status)) {
                target = threadsafe_list_find_first_if(self->target_list, target_by_id, id->valueint);
                if (target != NULL) {
                    target->status = status->valueint;
                    __scheduler_create_sql(SCHEDULER_UPDATE_TARGET_STATUS, id->valueint, self->target_db_table, sql, BUFSIZE, target->status, timestamp);
                    mysql_real_query(&mysql, sql, strlen(sql));
                }
            }
        }
    }
    if (self->type != SCHEDULER_TYPE_UNIT && (tasks_json = cJSON_GetObjectItemCaseSensitive(root_json, "TASK-INFO")) != NULL) {
        const cJSON *id, *status;
        if (cJSON_IsArray(tasks_json)) {
            cJSON_ArrayForEach(task_json, tasks_json) {
                id = cJSON_GetObjectItemCaseSensitive(target_json, "targ_id");
                status = cJSON_GetObjectItemCaseSensitive(target_json, "status");
                if (cJSON_IsNumber(id) && cJSON_IsNumber(status)) {
                    /*
                     * update database here. 
                     */
                    __scheduler_create_sql(SCHEDULER_UPDATE_TARGET_STATUS, id->valueint, self->task_db_table, sql, BUFSIZE, status->valueint, timestamp);
                    mysql_real_query(&mysql, sql, strlen(sql));
                }
            }
        } else {
            task_json = tasks_json;
            id = cJSON_GetObjectItemCaseSensitive(target_json, "site_id");
            status = cJSON_GetObjectItemCaseSensitive(target_json, "status");
            if (cJSON_IsNumber(id) && cJSON_IsNumber(status)) {
                /*
                 * update database here. 
                 */
                __scheduler_create_sql(SCHEDULER_UPDATE_TARGET_STATUS, id->valueint, self->task_db_table, sql, BUFSIZE, status->valueint, timestamp);
                mysql_real_query(&mysql, sql, strlen(sql));
            }
        }
    }
    mysql_close(&mysql);
    cJSON_Delete(root_json);
   
    return ret;
}

static int
__Scheduler_update_status(void *_self, const char *string, unsigned int type)
{
    struct __Scheduler *self = cast(__Scheduler(), _self);

    if (type == SCHEDULER_FORMAT_JSON) {
        return __Scheduler_update_status_json(self, string);
    } else {
        return AAOS_EFMTNOTSUP;
    }
}

int 
__scheduler_list_site(void *_self, char *buf, size_t size, unsigned int *type)
{
    const struct __SchedulerClass *class = (const struct __SchedulerClass *) classOf(_self);
    
    if (isOf(class, __SchedulerClass()) && class->list_site.method) {
        return ((int (*)(void *, char *, size_t, unsigned int *)) class->list_site.method)(_self, buf, size, type);
    } else {
        int result;
        forward(_self, &result, (Method) __scheduler_list_site, "list_site", _self, buf, type);
        return result;
    }
}

static int
__Scheduler_list_site(void *_self, char *buf, size_t size, unsigned int *type)
{
    struct __Scheduler *self = cast(__Scheduler(), _self);

    FILE *fp;

    if (self->type != SCHEDULER_TYPE_GLOBAL) {
        return AAOS_ENOTSUP;
    }

    fp = fmemopen(buf, size, "w");
    threadsafe_list_foreach(self->site_list, print_site_list, fp);
    fclose(fp);

    if (type != NULL) {
        *type = SCHEDULER_FORMAT_JSON;
    }
    return AAOS_OK;
}

int
__scheduler_add_site(void *_self, const char *info, unsigned int type)
{
    const struct __SchedulerClass *class = (const struct __SchedulerClass *) classOf(_self);
    
    if (isOf(class, __SchedulerClass()) && class->add_site.method) {
        return ((int (*)(void *, const char *, unsigned int)) class->add_site.method)(_self, info, type);
    } else {
        int result;
        forward(_self, &result, (Method) __scheduler_add_site, "add_site", _self, info, type);
        return result;
    }
}

static int 
__Scheduler_add_site_json(struct __Scheduler *self, const char *info)
{
    cJSON *site_json, *value_json;
    int ret = AAOS_OK;
    uint64_t site_id;
    double site_lon, site_lat, site_alt;
    struct timespec tp;
    struct SiteInfo *site;
    char *sitename;
    char sql[BUFSIZE];
    double timestamp;

    site_id = __Scheduler_generate_unique_site_id(self);

    site_json = cJSON_Parse(info);
    if (site_json != NULL) {
        value_json = cJSON_GetObjectItemCaseSensitive(site_json, "site_lon");
        if (value_json != NULL && cJSON_IsNumber(value_json)) {
            site_lon = value_json->valuedouble;
        } else {
            ret = AAOS_EBADCMD;
            goto end;
        }
        value_json = cJSON_GetObjectItemCaseSensitive(site_json, "site_lat");
        if (value_json != NULL && cJSON_IsNumber(value_json)) {
            site_lat = value_json->valuedouble;
        } else {
            ret = AAOS_EBADCMD;
            goto end;
        }
        value_json = cJSON_GetObjectItemCaseSensitive(site_json, "site_alt");
        if (value_json != NULL && cJSON_IsNumber(value_json)) {
            site_alt = value_json->valuedouble;
        } else {
            ret = AAOS_EBADCMD;
            goto end;
        }
        value_json = cJSON_GetObjectItemCaseSensitive(site_json, "sitename");
        if (value_json != NULL && cJSON_IsString(value_json)) {
            sitename = value_json->valuestring;
        } else {
            ret = AAOS_EBADCMD;
            goto end;
        }
    } else {
#ifdef DEBUG
        fprintf(stderr, "%s %s %d: input `%s` is not a legal JSON string.\n", __FILE__, __func__, __LINE__, info);
#endif
        return AAOS_EINVAL;
    }

    Clock_gettime(CLOCK_REALTIME, &tp);
    timestamp = tp.tv_sec + tp.tv_nsec / 1000000000.;
    __scheduler_create_sql(SCHEDULER_ADD_SITE, 0, self->site_db_table, sql, BUFSIZE, sitename, site_id, site_lon, site_lat, site_alt, timestamp);
    __Scheduler_database_query(self, sql, NULL);
    
    site = (struct SiteInfo *) Malloc(sizeof(struct SiteInfo));
    memset(site, '\0', sizeof(struct SiteInfo));
    site->name = (char *) Malloc(strlen(sitename) + 1);
    snprintf(site->name, strlen(sitename) + 1, "%s", sitename);
    site->status = 0;
    site->identifier = site_id;
    site->site_lon = site_lon;
    site->site_lat = site_lat;
    site->site_alt = site_alt;
    Pthread_mutex_init(&site->mtx, NULL);
    Pthread_cond_init(&site->cond, NULL);
    
    threadsafe_list_push_front(self->site_list, site);

end:
    cJSON_Delete(site_json);
    return ret;
}

static int
__Scheduler_add_site(void *_self, const char *info, unsigned int type)
{
    struct __Scheduler *self = cast(__Scheduler(), _self);
    
    if (self->type != SCHEDULER_TYPE_GLOBAL) {
        return AAOS_ENOTSUP;
    }

    if (type == SCHEDULER_FORMAT_JSON) {
        return __Scheduler_add_site_json(self, info);
    } else {
        return AAOS_EFMTNOTSUP;
    }
}

int
__scheduler_delete_site_by_id(void *_self, uint64_t identifier)
{
    const struct __SchedulerClass *class = (const struct __SchedulerClass *) classOf(_self);
    
    if (isOf(class, __SchedulerClass()) && class->delete_site_by_id.method) {
        return ((int (*)(void *, uint64_t)) class->delete_site_by_id.method)(_self, identifier);
    } else {
        int result;
        forward(_self, &result, (Method) __scheduler_delete_site_by_id, "delete_site_by_id", _self, identifier);
        return result;
    }
}

static int
__Scheduler_delete_site_by_id(void *_self, uint64_t identifier)
{
    struct __Scheduler *self = cast(__Scheduler(), _self);

    char *json_string;
    int ret;

    if (self->type != SCHEDULER_TYPE_GLOBAL) {
        return AAOS_ENOTSUP;
    }

    json_string = __scheduler_create_request_json_string(SCHEDULER_DELETE_SITE_BY_ID, identifier);
    ret = __Scheduler_update_status(self, json_string, SCHEDULER_FORMAT_JSON);
    free(json_string);

    return ret;
}

int
__scheduler_delete_site_by_name(void *_self, const char *name)
{
    const struct __SchedulerClass *class = (const struct __SchedulerClass *) classOf(_self);
    
    if (isOf(class, __SchedulerClass()) && class->delete_site_by_name.method) {
        return ((int (*)(void *, const char *)) class->delete_site_by_name.method)(_self, name);
    } else {
        int result;
        forward(_self, &result, (Method) __scheduler_delete_site_by_name, "delete_site_by_name", _self, name);
        return result;
    }
}

static int
__Scheduler_delete_site_by_name(void *_self, const char *name)
{
    struct __Scheduler *self = cast(__Scheduler(), _self);

    struct SiteInfo *site;
    uint64_t identifier;
    
    if (self->type != SCHEDULER_TYPE_GLOBAL) {
        return AAOS_ENOTSUP;
    }

    site = threadsafe_list_find_first_if(self->site_list, site_by_name, name);
    if (site == NULL) {
        return AAOS_ENOTFOUND;
    }
    identifier = site->identifier;

    return __Scheduler_delete_site_by_id(self, identifier);
}

int
__scheduler_mask_site_by_id(void *_self, uint64_t identifier)
{
    const struct __SchedulerClass *class = (const struct __SchedulerClass *) classOf(_self);
    
    if (isOf(class, __SchedulerClass()) && class->mask_site_by_id.method) {
        return ((int (*)(void *, uint64_t)) class->mask_site_by_id.method)(_self, identifier);
    } else {
        int result;
        forward(_self, &result, (Method) __scheduler_mask_site_by_id, "mask_site_by_id", _self, identifier);
        return result;
    }
}

static int
__Scheduler_mask_site_by_id(void *_self, uint64_t identifier)
{
    struct __Scheduler *self = cast(__Scheduler(), _self);

    char *json_string;
    int ret;

    if (self->type != SCHEDULER_TYPE_GLOBAL) {
        return AAOS_ENOTSUP;
    }

    json_string = __scheduler_create_request_json_string(SCHEDULER_MASK_SITE_BY_ID, identifier);
    ret = __Scheduler_update_status(self, json_string, SCHEDULER_FORMAT_JSON);
    free(json_string);

    return ret;
}

int
__scheduler_mask_site_by_name(void *_self, const char *name)
{
    const struct __SchedulerClass *class = (const struct __SchedulerClass *) classOf(_self);
    
    if (isOf(class, __SchedulerClass()) && class->mask_site_by_name.method) {
        return ((int (*)(void *, const char *)) class->mask_site_by_name.method)(_self, name);
    } else {
        int result;
        forward(_self, &result, (Method) __scheduler_mask_site_by_name, "mask_site_by_name", _self, name);
        return result;
    }
}

static int
__Scheduler_mask_site_by_name(void *_self, const char *name)
{
    struct __Scheduler *self = cast(__Scheduler(), _self);

    struct SiteInfo *site;
    uint64_t identifier;
    
    if (self->type != SCHEDULER_TYPE_GLOBAL) {
        return AAOS_ENOTSUP;
    }

    site = threadsafe_list_find_first_if(self->site_list, site_by_name, name);
    if (site == NULL) {
        return AAOS_ENOTFOUND;
    }
    identifier = site->identifier;

    return __Scheduler_mask_site_by_id(self, identifier);
}

int
__scheduler_unmask_site_by_id(void *_self, uint64_t identifier)
{
    const struct __SchedulerClass *class = (const struct __SchedulerClass *) classOf(_self);
    
    if (isOf(class, __SchedulerClass()) && class->unmask_site_by_id.method) {
        return ((int (*)(void *, uint64_t)) class->unmask_site_by_id.method)(_self, identifier);
    } else {
        int result;
        forward(_self, &result, (Method) __scheduler_unmask_site_by_id, "unmask_site_by_id", _self, identifier);
        return result;
    }
}

static int
__Scheduler_unmask_site_by_id(void *_self, uint64_t identifier)
{
    struct __Scheduler *self = cast(__Scheduler(), _self);

    char *json_string;
    int ret;

    if (self->type != SCHEDULER_TYPE_GLOBAL) {
        return AAOS_ENOTSUP;
    }

    json_string = __scheduler_create_request_json_string(SCHEDULER_UNMASK_SITE_BY_ID, identifier);
    ret = __Scheduler_update_status(self, json_string, SCHEDULER_FORMAT_JSON);
    free(json_string);

    return ret;
}

int
__scheduler_unmask_site_by_name(void *_self, const char *name)
{
    const struct __SchedulerClass *class = (const struct __SchedulerClass *) classOf(_self);
    
    if (isOf(class, __SchedulerClass()) && class->unmask_site_by_name.method) {
        return ((int (*)(void *, const char *)) class->unmask_site_by_name.method)(_self, name);
    } else {
        int result;
        forward(_self, &result, (Method) __scheduler_unmask_site_by_name, "unmask_site_by_name", _self, name);
        return result;
    }
}

static int
__Scheduler_unmask_site_by_name(void *_self, const char *name)
{
    struct __Scheduler *self = cast(__Scheduler(), _self);

    struct SiteInfo *site;
    uint64_t identifier;
    
    if (self->type != SCHEDULER_TYPE_GLOBAL) {
        return AAOS_ENOTSUP;
    }

    site = threadsafe_list_find_first_if(self->site_list, site_by_name, name);
    if (site == NULL) {
        return AAOS_ENOTFOUND;
    }
    identifier = site->identifier;

    return __Scheduler_unmask_site_by_id(self, identifier);
}


int 
__scheduler_list_telescope(void *_self, char *buf, size_t size, unsigned int *type)
{
    const struct __SchedulerClass *class = (const struct __SchedulerClass *) classOf(_self);
    
    if (isOf(class, __SchedulerClass()) && class->list_telescope.method) {
        return ((int (*)(void *, char *, size_t, unsigned int *)) class->list_telescope.method)(_self, buf, size, type);
    } else {
        int result;
        forward(_self, &result, (Method) __scheduler_list_telescope, "list_telescope", _self, buf, type);
        return result;
    }
}

static int
__Scheduler_list_telescope(void *_self, char *buf, size_t size, unsigned int *type)
{
    struct __Scheduler *self = cast(__Scheduler(), _self);

    FILE *fp;

    if (self->type != SCHEDULER_TYPE_GLOBAL && self->type != SCHEDULER_TYPE_SITE) {
        return AAOS_ENOTSUP;
    }

    fp = fmemopen(buf, size, "w");
    threadsafe_list_foreach(self->telescope_list, print_telescope_list, fp);
    fclose(fp);

    if (type != NULL) {
        *type = SCHEDULER_FORMAT_JSON;
    }

    return AAOS_OK;
}

/*
 * Available for site and global scheduler. 
 * If the scheduler is site scheduler, it will also asked the global scheduler to add a telescope. 
 */

struct AddTelescope {
    struct __Scheduler *scheduler;
    char *record;
};

static void *
__Scheduler_add_telescope_thr(void *arg)
{
    pthread_detach(pthread_self());
    
    struct AddTelescope *telescope_record = (struct AddTelescope *) arg;
    struct __Scheduler *self = telescope_record->scheduler;
    void *client, *scheduler_local;
    int ret;
    
    client = new(SchedulerClient(), self->global_addr, self->global_port);
    if ((ret = rpc_client_connect(client, &scheduler_local)) != AAOS_OK) {
        goto end;
    }
    scheduler_add_telescope(scheduler_local, telescope_record->record, SCHEDULER_FORMAT_JSON);
    delete(scheduler_local);
    
end:
    free(telescope_record->record);
    delete(client);
    free(arg);
    return NULL;
}


int
__scheduler_add_telescope(void *_self, const char *info, unsigned int type)
{
    const struct __SchedulerClass *class = (const struct __SchedulerClass *) classOf(_self);
    
    if (isOf(class, __SchedulerClass()) && class->add_telescope.method) {
        return ((int (*)(void *, const char *, unsigned int)) class->add_telescope.method)(_self, info, type);
    } else {
        int result;
        forward(_self, &result, (Method) __scheduler_add_telescope, "add_telescope", _self, info, type);
        return result;
    }
}

/*
 * {
 *   site_id: xxx,
 *   tel_id: xxx,
 *   telescop: 'xxxxxx',
 *   description: 'xxxxxxxx',
 * }
 */

static int
__Scheduler_add_telescope_json(struct __Scheduler *self, const char *info)
{
    cJSON *telescope_json, *value_json;
    int ret = AAOS_OK;
    uint64_t site_id, tel_id;
    int status;
    double timestamp;
    struct timespec tp;
    char *description = NULL, *telescop = NULL;
    char sql[BUFSIZE];
    void *client, *scheduler_local;

    Clock_gettime(CLOCK_REALTIME, &tp);
    timestamp = tp.tv_sec + tp.tv_nsec / 1000000000.;

    telescope_json = cJSON_Parse(info);
    if (telescope_json == NULL) {
        return AAOS_EINVAL;
    }
    
    if (self->type == SCHEDULER_TYPE_GLOBAL) {
        void *site;
        value_json = cJSON_GetObjectItemCaseSensitive(telescope_json, "site_id");
        if (value_json != NULL && cJSON_IsNumber(value_json)) {
            site_id = value_json->valueint;
        } else {
            ret = AAOS_EBADCMD;
            goto end;
        }
        value_json = cJSON_GetObjectItemCaseSensitive(telescope_json, "tel_id");
        if (value_json != NULL && cJSON_IsNumber(value_json)) {
            tel_id = value_json->valueint;
        } else {
            ret = AAOS_EBADCMD;
            goto end;
        }
    } else {
        struct SiteInfo *site = self->site;
        value_json = cJSON_GetObjectItemCaseSensitive(telescope_json, "site_id");
        if (value_json != NULL && cJSON_IsString(value_json)) {
            site_id = value_json->valueint;
        } else {
            cJSON_AddNumberToObject(telescope_json, "site_id", site->identifier);
            site_id = site->identifier;
        }
        cJSON_AddNumberToObject(telescope_json, "site_id", site_id);
        tel_id = __Scheduler_generate_unique_telescope_id(self, site_id);
        cJSON_AddNumberToObject(telescope_json, "tel_id", tel_id);
        
    }
    
    value_json = cJSON_GetObjectItemCaseSensitive(telescope_json, "telescop");
    if (value_json != NULL && cJSON_IsString(value_json)) {
        telescop = value_json->valuestring;
    } else {
        ret = AAOS_EBADCMD;
        goto end;
    }

    if (self->type == SCHEDULER_TYPE_SITE) {
        struct TelescopeInfo *telescope;
        telescope = (struct TelescopeInfo *) Malloc(sizeof(struct TelescopeInfo));
        memset(telescope, '\0', sizeof(struct TelescopeInfo));
        telescope->ra = 361.;
        telescope->dec = 91.;
        description = cJSON_PrintUnformatted(telescope_json);
        telescope->description = (char *) Malloc(strlen(description) + 1);
        snprintf(telescope->description, strlen(description) + 1, "%s", description);
        telescope->name = (char *) Malloc(strlen(telescop) + 1);
        snprintf(telescope->name, strlen(telescop) + 1, "%s", telescop);
        __scheduler_create_sql(SCHEDULER_ADD_TELESCOPE, 0, self->telescope_db_table, sql, BUFSIZE, telescop, tel_id, site_id, description, timestamp);
        struct AddTelescope *telescope_record = (struct AddTelescope *) Malloc(sizeof(struct AddTelescope));
        pthread_t tid;
        telescope_record->record = (char *) Malloc(strlen(description) + 1);
        snprintf(telescope_record->record, strlen(description) + 1, "%s", description);
        telescope_record->scheduler = self;
        Pthread_create(&tid, NULL, __Scheduler_add_telescope_thr, telescope_record);
        free(description);
    } else {
        __scheduler_create_sql(SCHEDULER_ADD_TELESCOPE, 0, self->telescope_db_table, sql, BUFSIZE, telescop, tel_id, site_id, info, timestamp);
    }
    __Scheduler_database_query(self, sql, NULL);

end:
    cJSON_Delete(telescope_json);
    

    return ret;
}

static int
__Scheduler_add_telescope(void *_self, const char *info, unsigned int type)
{
    struct __Scheduler *self = cast(__Scheduler(), _self);
    
    if (self->type == SCHEDULER_TYPE_UNIT) {
        return AAOS_ENOTSUP;
    }

    if (type == SCHEDULER_FORMAT_JSON) {
        return __Scheduler_add_telescope_json(self, info);
    } else {
        return AAOS_EFMTNOTSUP;
    }
}

struct TelescopeStatus {
    struct __Scheduler *scheduler;
    uint64_t identifier;
};

static void *
__Scheduler_delete_telescope_thr(void *arg)
{
    pthread_detach(pthread_self());
    
    struct TelescopeStatus *telescope_staus = (struct TelescopeStatus *) arg;
    struct __Scheduler *self = telescope_staus->scheduler;
    void *client, *scheduler_local;
    int ret;
    
    client = new(SchedulerClient(), self->global_addr, self->global_port);
    if ((ret = rpc_client_connect(client, &scheduler_local)) != AAOS_OK) {
        goto end;
    }
    scheduler_delete_telescope_by_id(scheduler_local, telescope_staus->identifier);
    delete(scheduler_local);
    
end:
    delete(client);
    free(arg);
    return NULL;
}

int
__scheduler_delete_telescope_by_id(void *_self, uint64_t identifier)
{
    const struct __SchedulerClass *class = (const struct __SchedulerClass *) classOf(_self);
    
    if (isOf(class, __SchedulerClass()) && class->delete_telescope_by_id.method) {
        return ((int (*)(void *, uint64_t)) class->delete_telescope_by_id.method)(_self, identifier);
    } else {
        int result;
        forward(_self, &result, (Method) __scheduler_delete_telescope_by_id, "delete_telescope_by_id", _self, identifier);
        return result;
    }
}

static int
__Scheduler_delete_telescope_by_id(void *_self, uint64_t identifier)
{
    struct __Scheduler *self = cast(__Scheduler(), _self);

    char *json_string;
    int ret;
    
    if (self->type == SCHEDULER_TYPE_UNIT) {
        return AAOS_ENOTSUP;
    }
    
    json_string = __scheduler_create_request_json_string(SCHEDULER_DELETE_TARGET_BY_ID, identifier);
    ret = __Scheduler_update_status(self, json_string, SCHEDULER_FORMAT_JSON);
    free(json_string);
    
    if (self->type == SCHEDULER_TYPE_SITE) {
        struct TelescopeStatus *telescope_status = (struct TelescopeStatus *) Malloc(sizeof(struct TelescopeStatus));
        pthread_t tid;
        telescope_status->scheduler = self;
        telescope_status->identifier = identifier;
        Pthread_create(&tid, NULL, __Scheduler_delete_telescope_thr, telescope_status);
    }

    return ret;
}

int
__scheduler_delete_telescope_by_name(void *_self, const char *name)
{
    const struct __SchedulerClass *class = (const struct __SchedulerClass *) classOf(_self);
    
    if (isOf(class, __SchedulerClass()) && class->delete_telescope_by_name.method) {
        return ((int (*)(void *, const char *)) class->delete_telescope_by_name.method)(_self, name);
    } else {
        int result;
        forward(_self, &result, (Method) __scheduler_delete_telescope_by_name, "delete_telescope_by_name", _self, name);
        return result;
    }
}

static int
__Scheduler_delete_telescope_by_name(void *_self, const char *name)
{
    struct __Scheduler *self = cast(__Scheduler(), _self);

    struct TelescopeInfo *telescope;
    uint64_t identifier;
    
    if (name == NULL) {
        return AAOS_EINVAL;
    }
    
    if (self->type != SCHEDULER_TYPE_SITE) {
        return AAOS_ENOTSUP;
    }
    
    telescope = threadsafe_list_find_first_if(self->telescope_list, telescope_by_name, name);
    if (telescope == NULL) {
        return AAOS_ENOTFOUND;
    }

    identifier = telescope->identifier;

    return __Scheduler_delete_telescope_by_id(self, identifier);
}

static void *
__Scheduler_mask_telescope_thr(void *arg)
{
    pthread_detach(pthread_self());
    
    struct TelescopeStatus *telescope_staus = (struct TelescopeStatus *) arg;
    struct __Scheduler *self = telescope_staus->scheduler;
    void *client, *scheduler_local;
    int ret;
    
    client = new(SchedulerClient(), self->global_addr, self->global_port);
    if ((ret = rpc_client_connect(client, &scheduler_local)) != AAOS_OK) {
        goto end;
    }
    scheduler_mask_telescope_by_id(scheduler_local, telescope_staus->identifier);
    delete(scheduler_local);
    
end:
    delete(client);
    free(arg);
    return NULL;
}

int
__scheduler_mask_telescope_by_id(void *_self, uint64_t identifier)
{
    const struct __SchedulerClass *class = (const struct __SchedulerClass *) classOf(_self);
    
    if (isOf(class, __SchedulerClass()) && class->mask_telescope_by_id.method) {
        return ((int (*)(void *, uint64_t)) class->mask_telescope_by_id.method)(_self, identifier);
    } else {
        int result;
        forward(_self, &result, (Method) __scheduler_mask_telescope_by_id, "mask_telescope_by_id", _self, identifier);
        return result;
    }
}

static int
__Scheduler_mask_telescope_by_id(void *_self, uint64_t identifier)
{
    struct __Scheduler *self = cast(__Scheduler(), _self);

    char *json_string;
    int ret;

    if (self->type == SCHEDULER_TYPE_UNIT) {
        return AAOS_ENOTSUP;
    }

    json_string = __scheduler_create_request_json_string(SCHEDULER_MASK_TARGET_BY_ID, identifier);
    ret = __Scheduler_update_status(self, json_string, SCHEDULER_FORMAT_JSON);
    if (self->type == SCHEDULER_TYPE_SITE) {
        struct TelescopeStatus *telescope_status = (struct TelescopeStatus *) Malloc(sizeof(struct TelescopeStatus));
        pthread_t tid;
        telescope_status->scheduler = self;
        telescope_status->identifier = identifier;
        Pthread_create(&tid, NULL, __Scheduler_mask_telescope_thr, telescope_status);
    }
    free(json_string);

    return ret;
}

int
__scheduler_mask_telescope_by_name(void *_self, const char *name)
{
    const struct __SchedulerClass *class = (const struct __SchedulerClass *) classOf(_self);
    
    if (isOf(class, __SchedulerClass()) && class->mask_telescope_by_name.method) {
        return ((int (*)(void *, const char *)) class->mask_telescope_by_name.method)(_self, name);
    } else {
        int result;
        forward(_self, &result, (Method) __scheduler_mask_telescope_by_name, "mask_telescope_by_name", _self, name);
        return result;
    }
}

static int
__Scheduler_mask_telescope_by_name(void *_self, const char *name)
{
    struct __Scheduler *self = cast(__Scheduler(), _self);

    struct TelescopeInfo *telescope;
    uint64_t identifier;
    
    if (name == NULL) {
        return AAOS_EINVAL;
    }
    
    if (self->type != SCHEDULER_TYPE_SITE) {
        return AAOS_ENOTSUP;
    }

    telescope = threadsafe_list_find_first_if(self->telescope_list, telescope_by_name, name);
    if (telescope == NULL) {
        return AAOS_ENOTFOUND;
    }
    identifier = telescope->identifier;
    
    return __Scheduler_mask_telescope_by_id(self, identifier);
}

int
__scheduler_unmask_telescope_by_id(void *_self, uint64_t identifier)
{
    const struct __SchedulerClass *class = (const struct __SchedulerClass *) classOf(_self);
    
    if (isOf(class, __SchedulerClass()) && class->unmask_telescope_by_id.method) {
        return ((int (*)(void *, uint64_t)) class->unmask_telescope_by_id.method)(_self, identifier);
    } else {
        int result;
        forward(_self, &result, (Method) __scheduler_unmask_telescope_by_id, "unmask_telescope_by_id", _self, identifier);
        return result;
    }
}

static void *
__Scheduler_unmask_telescope_thr(void *arg)
{
    pthread_detach(pthread_self());
    
    struct TelescopeStatus *telescope_staus = (struct TelescopeStatus *) arg;
    struct __Scheduler *self = telescope_staus->scheduler;
    void *client, *scheduler_local;
    int ret;
    
    client = new(SchedulerClient(), self->global_addr, self->global_port);
    if ((ret = rpc_client_connect(client, &scheduler_local)) != AAOS_OK) {
        goto end;
    }
    scheduler_unmask_telescope_by_id(scheduler_local, telescope_staus->identifier);
    delete(scheduler_local);
    
end:
    delete(client);
    free(arg);
    return NULL;
}

static int
__Scheduler_unmask_telescope_by_id(void *_self, uint64_t identifier)
{
    struct __Scheduler *self = cast(__Scheduler(), _self);

    char *json_string;
    int ret;
    void *client, *scheduler_local;

    if (self->type != SCHEDULER_TYPE_GLOBAL) {
        return AAOS_ENOTSUP;
    }

    json_string = __scheduler_create_request_json_string(SCHEDULER_UNMASK_TARGET_BY_ID, identifier);
    ret = __Scheduler_update_status(self, json_string, SCHEDULER_FORMAT_JSON);
    if (self->type == SCHEDULER_TYPE_SITE && self->site != NULL) {
        struct TelescopeStatus *telescope_status = (struct TelescopeStatus *) Malloc(sizeof(struct TelescopeStatus));
        pthread_t tid;
        telescope_status->scheduler = self;
        telescope_status->identifier = identifier;
        Pthread_create(&tid, NULL, __Scheduler_unmask_telescope_thr, telescope_status);
    }
    free(json_string);

    return ret;
}

int
__scheduler_unmask_telescope_by_name(void *_self, const char *name)
{
    const struct __SchedulerClass *class = (const struct __SchedulerClass *) classOf(_self);
    
    if (isOf(class, __SchedulerClass()) && class->unmask_telescope_by_name.method) {
        return ((int (*)(void *, const char *)) class->unmask_telescope_by_name.method)(_self, name);
    } else {
        int result;
        forward(_self, &result, (Method) __scheduler_unmask_telescope_by_name, "unmask_telescope_by_name", _self, name);
        return result;
    }
}

static int
__Scheduler_unmask_telescope_by_name(void *_self, const char *name)
{
    struct __Scheduler *self = cast(__Scheduler(), _self);

    struct TelescopeInfo *telescope;
    uint64_t identifier;
    
    if (name == NULL) {
        return AAOS_EINVAL;
    }
    
    if (self->type != SCHEDULER_TYPE_GLOBAL) {
        return AAOS_ENOTSUP;
    }

    telescope = threadsafe_list_find_first_if(self->telescope_list, telescope_by_name, name);
    if (telescope == NULL) {
        return AAOS_ENOTFOUND;
    }
    identifier = telescope->identifier;

    return __Scheduler_unmask_telescope_by_id(self, identifier);
}

int 
__scheduler_list_target(void *_self, char *buf, size_t size, unsigned int *type)
{
    const struct __SchedulerClass *class = (const struct __SchedulerClass *) classOf(_self);
    
    if (isOf(class, __SchedulerClass()) && class->list_target.method) {
        return ((int (*)(void *, char *, size_t, unsigned int *)) class->list_target.method)(_self, buf, size, type);
    } else {
        int result;
        forward(_self, &result, (Method) __scheduler_list_target, "list_target", _self, buf, type);
        return result;
    }
}

static int
__Scheduler_list_target(void *_self, char *buf, size_t size, unsigned int *type)
{
    struct __Scheduler *self = cast(__Scheduler(), _self);

    FILE *fp;

    if (self->type != SCHEDULER_TYPE_GLOBAL) {
        return AAOS_ENOTSUP;
    }

    fp = fmemopen(buf, size, "w");
    threadsafe_list_foreach(self->target_list, print_target_list, fp);
    fclose(fp);

    if (type != NULL) {
        *type = SCHEDULER_FORMAT_JSON;
    }
    return AAOS_OK;
}

int
__scheduler_add_target(void *_self, const char *info, unsigned int type)
{
    const struct __SchedulerClass *class = (const struct __SchedulerClass *) classOf(_self);
    
    if (isOf(class, __SchedulerClass()) && class->add_target.method) {
        return ((int (*)(void *, const char *, unsigned int)) class->add_target.method)(_self, info, type);
    } else {
        int result;
        forward(_self, &result, (Method) __scheduler_add_target, "add_target", _self, info, type);
        return result;
    }
}

static int
__Scheduler_add_target_json(struct __Scheduler *self, const char *info)
{
    cJSON *target_json, *value_json;
    int ret = AAOS_OK;
    uint64_t targ_id;
    uint32_t nside;
    int priority = 0;
    double ra, dec, timestamp;
    struct timespec tp;
    char *description = NULL, *targname = NULL;
    char sql[BUFSIZE];
    struct TargetInfo *target;

    Clock_gettime(CLOCK_REALTIME, &tp);
    timestamp = tp.tv_sec + tp.tv_nsec / 1000000000.;

    target_json = cJSON_Parse(info);
    if (target_json) {
        return AAOS_EINVAL;
    }
    
    value_json = cJSON_GetObjectItemCaseSensitive(target_json, "nside");
    if (value_json != NULL && cJSON_IsNumber(value_json)) {
        nside = value_json->valueint;
    } else {
        ret = AAOS_EBADCMD;
        goto end;
    }
    value_json = cJSON_GetObjectItemCaseSensitive(target_json, "ra_targ");
    if (value_json != NULL && cJSON_IsNumber(value_json)) {
        ra = value_json->valuedouble;
    } else {
        ret = AAOS_EBADCMD;
        goto end;
    }
    value_json = cJSON_GetObjectItemCaseSensitive(target_json, "dec_targ");
    if (value_json != NULL && cJSON_IsNumber(value_json)) {
        dec = value_json->valuedouble;
    } else {
        ret = AAOS_EBADCMD;
        goto end;
    }
    value_json = cJSON_GetObjectItemCaseSensitive(target_json, "priority");
    if (value_json != NULL && cJSON_IsNumber(value_json)) {
        priority = value_json->valueint;
    }
    value_json = cJSON_GetObjectItemCaseSensitive(target_json, "targname");
    if (value_json != NULL && cJSON_IsString(value_json)) {
        targname = value_json->valuestring;
    }

    if (ra < 0. || ra > 360. || dec > 90. || dec < -90.) {
        ret = AAOS_EINVAL;
        goto end;
    }

    targ_id = __scheduler_generate_unique_target_id(nside, ra, dec);

    __scheduler_create_sql(SCHEDULER_ADD_TARGET, 0, self->target_db_table, sql, BUFSIZE, targname, targ_id, nside, ra, dec, priority);
    __Scheduler_database_query(self, sql, NULL);

    target = (struct TargetInfo *) Malloc(sizeof(struct TargetInfo));
    memset(target, '\0', sizeof(struct TaskInfo));
    target->identifier = targ_id;
    target->nside = nside;
    target->name = targname;
    target->ra = ra;
    target->dec = dec;
    threadsafe_list_push_front(self->target_list, target);

end: 
    cJSON_Delete(target_json);

    return ret;
}

static int
__Scheduler_add_target(void *_self, const char *info, unsigned int type)
{
    struct __Scheduler *self = cast(__Scheduler(), _self);
    
    if (self->type != SCHEDULER_TYPE_GLOBAL) {
        return AAOS_ENOTSUP;
    }

    if (type == SCHEDULER_FORMAT_JSON) {
        return __Scheduler_add_target_json(self, info);
    } else {
        return AAOS_EFMTNOTSUP;
    }
}

int
__scheduler_delete_target_by_id(void *_self, uint64_t identifier, uint32_t nside)
{
    const struct __SchedulerClass *class = (const struct __SchedulerClass *) classOf(_self);
    
    if (isOf(class, __SchedulerClass()) && class->delete_target_by_id.method) {
        return ((int (*)(void *, uint64_t, uint32_t)) class->delete_target_by_id.method)(_self, identifier, nside);
    } else {
        int result;
        forward(_self, &result, (Method) __scheduler_delete_target_by_id, "delete_target_by_id", _self, identifier, nside);
        return result;
    }
}

static int
__Scheduler_delete_target_by_id(void *_self, uint64_t identifier, uint32_t nside)
{
    struct __Scheduler *self = cast(__Scheduler(), _self);
    
    char *json_string;
    int ret = AAOS_OK;

    if (self->type == SCHEDULER_TYPE_UNIT) {
        return AAOS_ENOTSUP;
    }
    json_string = __scheduler_create_request_json_string(SCHEDULER_DELETE_TARGET_BY_ID, identifier);
    ret = __Scheduler_update_status(self, json_string, SCHEDULER_FORMAT_JSON);
    free(json_string);
    
    return ret;
}

int
__scheduler_delete_target_by_name(void *_self, const char *name)
{
    const struct __SchedulerClass *class = (const struct __SchedulerClass *) classOf(_self);
    
    if (isOf(class, __SchedulerClass()) && class->delete_target_by_name.method) {
        return ((int (*)(void *, const char *)) class->delete_target_by_name.method)(_self, name);
    } else {
        int result;
        forward(_self, &result, (Method) __scheduler_delete_target_by_name, "delete_target_by_name", _self, name);
        return result;
    }
}

static int
__Scheduler_delete_target_by_name(void *_self, const char *name)
{
    struct __Scheduler *self = cast(__Scheduler(), _self);

    struct TargetInfo *target;
    uint64_t identifier;
    uint32_t nside;
    
    if (self->type != SCHEDULER_TYPE_GLOBAL) {
        return AAOS_ENOTSUP;
    }

    target = threadsafe_list_find_first_if(self->target_list, target_by_name, name);
    if (target == NULL) {
        return AAOS_ENOTFOUND;
    }
    identifier = target->identifier;
    nside = target->nside;

    return __Scheduler_delete_target_by_id(self, identifier, nside);
}

int
__scheduler_mask_target_by_id(void *_self, uint64_t identifier, uint32_t nside)
{
    const struct __SchedulerClass *class = (const struct __SchedulerClass *) classOf(_self);
    
    if (isOf(class, __SchedulerClass()) && class->mask_target_by_id.method) {
        return ((int (*)(void *, uint64_t, uint32_t)) class->mask_target_by_id.method)(_self, identifier, nside);
    } else {
        int result;
        forward(_self, &result, (Method) __scheduler_mask_target_by_id, "mask_target_by_id", _self, identifier, nside);
        return result;
    }
}

static int
__Scheduler_mask_target_by_id(void *_self, uint64_t identifier, uint32_t nside)
{
    struct __Scheduler *self = cast(__Scheduler(), _self);

    char *json_string;
    int ret;

    if (self->type != SCHEDULER_TYPE_GLOBAL) {
        return AAOS_ENOTSUP;
    }

    json_string = __scheduler_create_request_json_string(SCHEDULER_MASK_TARGET_BY_ID, identifier);
    ret = __Scheduler_update_status(self, json_string, SCHEDULER_FORMAT_JSON);
    free(json_string);

    return ret;
}

int
__scheduler_mask_target_by_name(void *_self, const char *name)
{
    const struct __SchedulerClass *class = (const struct __SchedulerClass *) classOf(_self);
    
    if (isOf(class, __SchedulerClass()) && class->mask_target_by_name.method) {
        return ((int (*)(void *, const char *)) class->mask_target_by_name.method)(_self, name);
    } else {
        int result;
        forward(_self, &result, (Method) __scheduler_mask_target_by_name, "mask_target_by_name", _self, name);
        return result;
    }
}

static int
__Scheduler_mask_target_by_name(void *_self, const char *name)
{
    struct __Scheduler *self = cast(__Scheduler(), _self);

    struct TargetInfo *target;
    uint64_t identifier;
    uint32_t nside;
    
    if (self->type != SCHEDULER_TYPE_GLOBAL) {
        return AAOS_ENOTSUP;
    }

    target = threadsafe_list_find_first_if(self->target_list, target_by_name, name);
    if (target == NULL) {
        return AAOS_ENOTFOUND;
    }
    identifier = target->identifier;
    nside = target->nside;

    return __Scheduler_mask_target_by_id(self, identifier, nside);
}

int
__scheduler_unmask_target_by_id(void *_self, uint64_t identifier, uint32_t nside)
{
    const struct __SchedulerClass *class = (const struct __SchedulerClass *) classOf(_self);
    
    if (isOf(class, __SchedulerClass()) && class->unmask_target_by_id.method) {
        return ((int (*)(void *, uint64_t, uint32_t)) class->unmask_target_by_id.method)(_self, identifier, nside);
    } else {
        int result;
        forward(_self, &result, (Method) __scheduler_unmask_target_by_id, "unmask_target_by_id", _self, identifier, nside);
        return result;
    }
}

static int
__Scheduler_unmask_target_by_id(void *_self, uint64_t identifier, uint32_t nside)
{
    struct __Scheduler *self = cast(__Scheduler(), _self);

    char *json_string;
    int ret;

    if (self->type != SCHEDULER_TYPE_GLOBAL) {
        return AAOS_ENOTSUP;
    }
    
    json_string = __scheduler_create_request_json_string(SCHEDULER_UNMASK_TARGET_BY_ID, identifier);
    ret = __Scheduler_update_status(self, json_string, SCHEDULER_FORMAT_JSON);
    free(json_string);
    
    return ret;
}

int
__scheduler_unmask_target_by_name(void *_self, const char *name)
{
    const struct __SchedulerClass *class = (const struct __SchedulerClass *) classOf(_self);
    
    if (isOf(class, __SchedulerClass()) && class->unmask_target_by_name.method) {
        return ((int (*)(void *, const char *)) class->unmask_target_by_name.method)(_self, name);
    } else {
        int result;
        forward(_self, &result, (Method) __scheduler_unmask_target_by_name, "unmask_target_by_name", _self, name);
        return result;
    }
}

static int
__Scheduler_unmask_target_by_name(void *_self, const char *name)
{
    struct __Scheduler *self = cast(__Scheduler(), _self);

    struct TargetInfo *target;
    uint64_t identifier = 0;
    uint32_t nside = 0;
    
    target = threadsafe_list_find_first_if(self->target_list, target_by_name, name);
    identifier = target->identifier;
    nside = target->nside;
    
    return __Scheduler_unmask_target_by_id(self, identifier, nside);
}

int
__scheduler_add_task_record(void *_self, int status, const char *info, unsigned int type)
{
    const struct __SchedulerClass *class = (const struct __SchedulerClass *) classOf(_self);
    
    if (isOf(class, __SchedulerClass()) && class->add_task_record.method) {
        return ((int (*)(void *, int, const char *, unsigned int)) class->add_task_record.method)(_self, status, info, type);
    } else {
        int result;
        forward(_self, &result, (Method) __scheduler_add_task_record, "add_task_record", _self, status, info, type);
        return result;
    }
}

struct AddTaskRecord {
    struct __Scheduler *scheduler;
    const char *record;
    char *record2;
};

static void *
__Scheduler_add_task_record_thr(void *arg)
{
    Pthread_detach(pthread_self());
    
    struct AddTaskRecord *task_record = (struct AddTaskRecord *) arg;
    struct __Scheduler *self = (struct __Scheduler *) task_record->scheduler;
    void *client, *scheduler_local;
    int ret;
    
    client = new(SchedulerClient(), self->global_addr, self->global_port);
    if ((ret = rpc_client_connect(client, &scheduler_local)) != AAOS_OK) {
        goto end;
    }
    if (task_record->record2 != NULL) {
        scheduler_add_task_record(scheduler_local, task_record->record2, SCHEDULER_FORMAT_JSON);
    } else {
        scheduler_add_task_record(scheduler_local, task_record->record, SCHEDULER_FORMAT_JSON);
    }
    delete(scheduler_local);
end:
    free(task_record->record2);
    delete(client);
    free(arg);
    return NULL;
}

static int 
__Scheduler_add_task_record_json(struct __Scheduler *self, int status, const char *info)
{
    cJSON *root_json, *general_json, *site_json, *telescope_json, *target_json, *value_json, *value2_json;
    int ret = AAOS_OK, ret2;
    uint64_t site_id, tel_id, targ_id, task_id;
    uint32_t nside;
    float timestamp;
    char sql[BUFSIZE];
    struct SiteInfo *site;
    char *info2 = NULL;
    void *client, *scheduler_local;
    
    struct timespec tp;
    Clock_gettime(CLOCK_REALTIME, &tp);
    timestamp = tp.tv_sec + tp.tv_nsec / 1000000000.;
    
    root_json = cJSON_Parse(info);
    if (root_json != NULL) {
        if ((general_json = cJSON_GetObjectItemCaseSensitive(root_json, "GENERAL-INFO")) != NULL && (value_json = cJSON_GetObjectItemCaseSensitive(general_json, "task_id")) != NULL && cJSON_IsNumber(value_json)) {
            task_id = value_json->valueint;
        } else {
#ifdef DEBUG
            fprintf(stderr, "%s %s %d: `task_id field is absent in GENERAL-INFO.\n", __FILE__, __func__, __LINE__);
#endif
            ret = AAOS_EBADCMD;
            goto end;
        }
        if ((target_json = cJSON_GetObjectItemCaseSensitive(root_json, "TARGET-INFO")) != NULL && (value_json = cJSON_GetObjectItemCaseSensitive(target_json, "targ_id")) != NULL && cJSON_IsNumber(value_json) && (value2_json = cJSON_GetObjectItemCaseSensitive(target_json, "nside")) != NULL && cJSON_IsNumber(value2_json)) {
            targ_id = value_json->valueint;
            nside = value2_json->valueint;
        } else {
#ifdef DEBUG
            fprintf(stderr, "%s %s %d: `targ_id` or `nside` field is absent in TARGET-INFO.\n", __FILE__, __func__, __LINE__);
#endif
            ret = AAOS_EBADCMD;
            goto end;
        }
        if ((telescope_json = cJSON_GetObjectItemCaseSensitive(root_json, "TELESCOPE-INFO")) != NULL && (value_json = cJSON_GetObjectItemCaseSensitive(telescope_json, "tel_id")) != NULL && cJSON_IsNumber(value_json)) {
#ifdef DEBUG
            fprintf(stderr, "%s %s %d: `tel_id field is absent in TELESCOPE-INFO.\n", __FILE__, __func__, __LINE__);
#endif
            tel_id = value_json->valueint;
        } else {
            ret = AAOS_EBADCMD;
            goto end;
        }
        if ((site_json = cJSON_GetObjectItemCaseSensitive(root_json, "SITE-INFO")) != NULL && (value_json = cJSON_GetObjectItemCaseSensitive(root_json, "site_id")) != NULL && cJSON_IsNumber(value_json)) {
            site_id = value_json->valueint;
        } else {
            if (self->type == SCHEDULER_TYPE_SITE && self->site != NULL) {
                site = self->site;
                site_id = site->identifier;
                if (site_json == NULL) {
                    site_json = cJSON_CreateObject();
                    cJSON_AddNumberToObject(site_json, "site_id", site_id);
                    cJSON_AddItemToObject(root_json, "SITE-INFO", site_json);
                    info2 = cJSON_Print(root_json);
                } else {
                    cJSON_AddNumberToObject(site_json, "site_id", site_id);
                    info2 = cJSON_Print(root_json);
                }
            } else {
#ifdef DEBUG
                fprintf(stderr, "%s %s %d: `site_id field is absent in SITE-INFO.\n", __FILE__, __func__, __LINE__);
#endif
                goto end;
            }
        }
    } else {
#ifdef DEBUG
        fprintf(stderr, "%s %s %d: `%s` illegal format.\n", __FILE__, __func__, __LINE__, info);
#endif
        return AAOS_EINVAL;
    }
    
    __scheduler_create_sql(SCHEDULER_ADD_TASK_RECORD, 0, self->task_db_table, sql, BUFSIZE, task_id, targ_id, nside, tel_id, site_id, status, info, timestamp);
    __Scheduler_database_query(self, sql, NULL);

    if (self->type == SCHEDULER_TYPE_SITE) {
        struct AddTaskRecord *task_record = (struct AddTaskRecord *) Malloc(sizeof(struct AddTaskRecord));
        pthread_t tid;
        task_record->scheduler = self;
        task_record->record = info;
        task_record->record2 = info2;
        Pthread_create(&tid, NULL, __Scheduler_add_task_record_thr, task_record);
    }
end:
    cJSON_Delete(root_json);
    return ret;
}

static int
__Scheduler_add_task_record(void *_self, int status, const char *info, unsigned int type)
{
    struct __Scheduler *self = cast(__Scheduler(), _self);
    
    if (self->type == SCHEDULER_TYPE_UNIT) {
        return AAOS_ENOTSUP;
    }

    if (type == SCHEDULER_FORMAT_JSON) {
        return __Scheduler_add_task_record_json(self, status, info);
    } else {
        return AAOS_EFMTNOTSUP;
    }
}

struct UpdateTaskRecord {
    struct __Scheduler *scheduler;
    uint64_t identifier;
    char *record;
};

static void *
__Scheduler_update_task_record_thr(void *arg)
{
    pthread_detach(pthread_self());
    
    struct UpdateTaskRecord *task_record = (struct UpdateTaskRecord *) arg;
    struct __Scheduler *self = task_record->scheduler;
    void *client, *scheduler_local;
    int ret;
    
    client = new(SchedulerClient(), self->global_addr, self->global_port);
    if ((ret = rpc_client_connect(client, &scheduler_local)) != AAOS_OK) {
        goto end;
    }
    scheduler_update_task_record(scheduler_local, task_record->identifier, task_record->record, SCHEDULER_FORMAT_JSON);

    delete(scheduler_local);
end:
    free(task_record->record);
    delete(client);
    free(arg);
    return NULL;
}


int
__scheduler_update_task_record(void *_self, uint64_t identifier, const char *info)
{
    const struct __SchedulerClass *class = (const struct __SchedulerClass *) classOf(_self);
    
    if (isOf(class, __SchedulerClass()) && class->update_task_record.method) {
        return ((int (*)(void *, uint64_t, const char *)) class->update_task_record.method)(_self, identifier, info);
    } else {
        int result;
        forward(_self, &result, (Method) __scheduler_update_task_record, "update_task_record", _self, identifier, info);
        return result;
    }
}

static int 
__Scheduler_update_task_record(void *_self, uint64_t identifier, const char *info)
{
    struct __Scheduler *self = cast(__Scheduler(), _self);
    
    double timestamp;
    char sql[BUFSIZE];
    struct timespec tp;
    
    Clock_gettime(CLOCK_REALTIME, &tp);
    timestamp = tp.tv_sec + tp.tv_nsec / 1000000000.;

    __scheduler_create_sql(SCHEDULER_UPDATE_TASK_RECORD, identifier, self->task_db_table, sql, BUFSIZE, identifier, info);
    __Scheduler_database_query(self, sql, NULL);
    
    if (self->type == SCHEDULER_TYPE_SITE) {
        struct UpdateTaskRecord *task_record = (struct UpdateTaskRecord *) Malloc(sizeof(struct UpdateTaskRecord));
        pthread_t tid;
        task_record->identifier = identifier;
        task_record->scheduler = self;
        task_record->record = (char *) Malloc(strlen(info) + 1);
        snprintf(task_record->record, strlen(info) + 1, "%s", info);
        Pthread_create(&tid, NULL, __Scheduler_update_task_record_thr, task_record);
    }
    
    return AAOS_OK;
}

struct UpdateTaskStatus {
    struct __Scheduler *scheduler;
    uint64_t identifier;
    int status;
};

static void *
__Scheduler_update_task_status_thr(void *arg)
{
    pthread_detach(pthread_self());
    
    struct UpdateTaskStatus *task_status = (struct UpdateTaskStatus *) arg;
    struct __Scheduler *self = task_status->scheduler;
    void *client, *scheduler_local;
    int ret;
    
    client = new(SchedulerClient(), self->global_addr, self->global_port);
    if ((ret = rpc_client_connect(client, &scheduler_local)) != AAOS_OK) {
        goto end;
    }
    scheduler_update_task_status(scheduler_local, task_status->identifier, task_status->status);

    delete(scheduler_local);
end:
    delete(client);
    free(arg);
    return NULL;
}

int
__scheduler_update_task_status(void *_self, uint64_t identifier, int status)
{
    const struct __SchedulerClass *class = (const struct __SchedulerClass *) classOf(_self);
    
    if (isOf(class, __SchedulerClass()) && class->update_task_status.method) {
        return ((int (*)(void *, uint64_t, int)) class->update_task_status.method)(_self, identifier, status);
    } else {
        int result;
        forward(_self, &result, (Method) __scheduler_update_task_record, "update_task_status", _self, identifier, status);
        return result;
    }
}

static int
__Scheduler_update_task_status(void *_self, uint64_t identifier, int status)
{
    struct __Scheduler *self = cast(__Scheduler(), _self);
    
    double timestamp;
    char sql[BUFSIZE];
    struct timespec tp;
   
    Clock_gettime(CLOCK_REALTIME, &tp);
    timestamp = tp.tv_sec + tp.tv_nsec / 1000000000.;

    __scheduler_create_sql(SCHEDULER_UPDATE_TASK_STATUS, identifier, self->task_db_table, sql, BUFSIZE, status, timestamp);
    __Scheduler_database_query(self, sql, NULL);
    if (self->type == SCHEDULER_TYPE_SITE) {
        struct UpdateTaskStatus *task_status = (struct UpdateTaskStatus *) Malloc(sizeof(struct UpdateTaskStatus));
        pthread_t tid;
        task_status->scheduler = self;
        task_status->identifier = identifier;
        task_status->status = status;
        Pthread_create(&tid, NULL, __Scheduler_update_task_status_thr, task_status);
    }
    return AAOS_OK;
}

void *
__scheduler_site_manage_thr(void *_self)
{
    const struct __SchedulerClass *class = (const struct __SchedulerClass *) classOf(_self);
    
    if (isOf(class, __SchedulerClass()) && class->site_manage_thr.method) {
        return ((void * (*)(void *)) class->site_manage_thr.method)(_self);
    } else {
        void *result;
        forward(_self, &result, (Method) __scheduler_site_manage_thr, "site_manage_thr", _self);
        return result;
    }
}

static void *
__Scheduler_site_manage_thr(void *_self)
{
    struct __Scheduler *self = cast(__Scheduler(), _self);

    Pthread_detach(pthread_self());

    __Scheduler_pop_task_block(self);

    return NULL;
}

void *
__scheduler_telescope_manage_thr(void *_self)
{
    const struct __SchedulerClass *class = (const struct __SchedulerClass *) classOf(_self);
    
    if (isOf(class, __SchedulerClass()) && class->telescope_manage_thr.method) {
        return ((void * (*)(void *)) class->telescope_manage_thr.method)(_self);
    } else {
        void *result;
        forward(_self, &result, (Method) __scheduler_telescope_manage_thr, "telescope_manage_thr", _self);
        return result;
    }
}

static void *
__Scheduler_telescope_manage_thr(void *_self)
{
    struct __Scheduler *self = cast(__Scheduler(), _self);

    Pthread_detach(pthread_self());

    __Scheduler_pop_task_block(self);

    return NULL;
}

void
__scheduler_set_member(void *_self, const char *name, ...)
{
    const struct __SchedulerClass *class = (const struct __SchedulerClass *) classOf(_self);
    
    va_list ap;
    va_start(ap, name);
    if (isOf(class, __SchedulerClass()) && class->set_member.method) {
        ((void (*)(void *, const char *, va_list *)) class->set_member.method)(_self, name, &ap);
    } else {
        forward(_self, (void *) 0, (Method) __scheduler_set_member, "set_member", _self, name, &ap);
    }
    va_end(ap);
}

static void
__Scheduler_set_member(void *_self, const char *name, va_list *app)
{
    struct __Scheduler *self = cast(__Scheduler(), _self);

    if (strcmp(name, "site") == 0) {
        self->site = va_arg(*app, void *);
    } else if (strcmp(name, "global_addr") == 0) {
        const char *value = va_arg(*app, const char *);
        if (value != NULL) {
            self->global_addr = Realloc(self->global_addr, strlen(value) + 1);
            snprintf(self->global_addr, strlen(value) + 1, "%s", value);
        }
    } else if (strcmp(name, "global_port") == 0) {
        const char *value = va_arg(*app, const char *);
        if (value != NULL) {
            self->global_port = Realloc(self->global_port, strlen(value) + 1);
            snprintf(self->global_port, strlen(value) + 1, "%s", value);
        }
    } else if (strcmp(name, "telescope") == 0) {
        self->telescope = va_arg(*app, void *);
    } else if (strcmp(name, "ipc_model") == 0) {
        const char *value = va_arg(*app, const char *);
        if (value != NULL) {
            self->ipc_model = Realloc(self->ipc_model, strlen(value) + 1);
            snprintf(self->ipc_model, strlen(value) + 1, "%s", value);
        }
    } else if (strcmp(name, "algorithm") == 0) {
        const char *value = va_arg(*app, const char *);
        if (value != NULL) {
            self->algorithm = Realloc(self->algorithm, strlen(value) + 1);
            snprintf(self->algorithm, strlen(value) + 1, "%s", value);
        }
    } else if (strcmp(name, "sock_file") == 0) {
        const char *value = va_arg(*app, const char *);
        if (value != NULL) {
            self->sock_file = Realloc(self->sock_file, strlen(value) + 1);
            snprintf(self->sock_file, strlen(value) + 1, "%s", value);
        }
    } else if (strcmp(name, "site_addr") == 0) {
        const char *value = va_arg(*app, const char *);
        if (value != NULL) {
            self->site_addr = Realloc(self->site_addr, strlen(value) + 1);
            snprintf(self->site_addr, strlen(value) + 1, "%s", value);
        }
    } else if (strcmp(name, "site_port") == 0) {
        const char *value = va_arg(*app, const char *);
        if (value != NULL) {
            self->site_port = Realloc(self->site_port, strlen(value) + 1);
            snprintf(self->site_port, strlen(value) + 1, "%s", value);
        }
    } else if (strcmp(name, "site_name") == 0) {
        struct SiteInfo *site = self->site;
        const char *value = va_arg(*app, const char *);
        if (site != NULL && value != NULL) {
            site->name = Realloc(site->name, strlen(value) + 1);
            snprintf(site->name, strlen(value) + 1, "%s", value);
        }
    } else if (strcmp(name, "site_id") == 0) {
        struct SiteInfo *site = self->site;
        uint64_t value = va_arg(*app, uint64_t);
        if (site == NULL) {
            site = (struct SiteInfo *) Malloc(sizeof(struct SiteInfo));
            memset(site, '\0', sizeof(struct SiteInfo));
            self->site = site;
            Pthread_mutex_init(&site->mtx, NULL);
            Pthread_cond_init(&site->cond, NULL);
        }
        site->identifier = value;
    } else if (strcmp(name, "site_lon") == 0) {
        double value = va_arg(*app, double);
        if (self->site != NULL) {
            ((struct SiteInfo *) self->site)->site_lon = value;
        }
    } else if (strcmp(name, "site_lat") == 0) {
        double value = va_arg(*app, double);
        if (self->site != NULL) {
            ((struct SiteInfo *) self->site)->site_lat = value;
        }
    } else if (strcmp(name, "site_alt") == 0) {
        double value = va_arg(*app, double);
        if (self->site != NULL) {
            ((struct SiteInfo *) self->site)->site_alt = value;
        }
    } else if (strcmp(name, "max_task_in_block") == 0) {
        self->max_task_in_block = va_arg(*app, size_t);
    } else if (strcmp(name, "connect_global") == 0) {
        struct SiteInfo *site = self->site;
        if (self->global_addr != NULL && self->global_addr != NULL && site != NULL) {
            int ret;
            Pthread_mutex_lock(&site->mtx);
            if (site->rpc != NULL) {
                delete(site->rpc);
                site->rpc = NULL;
            }
            void *client = new(SchedulerClient(), self->global_addr, self->global_port);
            ret = rpc_client_connect(client, &site->rpc);
            if (ret == AAOS_OK) {
                ret = scheduler_register_thread(site->rpc, site->identifier);
                if (ret != AAOS_OK) {
                    delete(site->rpc);
                    site->rpc = NULL;
#ifdef DEBUG
                    fprintf(stderr, "%s %s %d: register site thread failed.\n", __FILE__, __func__, __LINE__);
#endif
                }
                
            } else {
#ifdef DEBUG
                fprintf(stderr, "%s %s %d: site scheduler connect global scheduler faild.\n", __FILE__, __func__, __LINE__);
#endif
            }
            Pthread_mutex_unlock(&site->mtx);
            delete(client);
        } else {
#ifdef DEBUG
            if (self->global_addr == NULL) {
                fprintf(stderr, "%s %s %d: global scheduler address has not been initialized .\n", __FILE__, __func__, __LINE__);
            }
            if (self->global_addr == NULL) {
                fprintf(stderr, "%s %s %d: global scheduler port has not been initialized .\n", __FILE__, __func__, __LINE__);
            }
            if (self->site == NULL) {
                fprintf(stderr, "%s %s %d: global scheduler object has not been initialized .\n", __FILE__, __func__, __LINE__);
            }
#endif
        }
    } else if (strcmp(name, "connect_site") == 0) {
        struct TelescopeInfo *telescope = self->telescope;
        if (self->site_addr != NULL && self->site_port != NULL && telescope != NULL) {
            int ret;
	    if (telescope->rpc != NULL) {
	        delete(telescope->rpc);
	        telescope->rpc = NULL;
	    }
            void *client = new(SchedulerClient(), self->site_addr, self->site_port);
            ret = rpc_client_connect(client, &telescope->rpc);
            if (ret == AAOS_OK) {
                scheduler_register_thread(telescope->rpc, telescope->identifier);
            } else {
#ifdef DEBUG
                fprintf(stderr, "%s %s %d: unit scheduler connect site scheduler faild.\n", __FILE__, __func__, __LINE__);
#endif
            }
            delete(client);
        }
    } else if (strcmp(name, "tel_id") == 0) {
        struct TelescopeInfo *telescope= self->telescope;
        uint64_t value = va_arg(*app, uint64_t);
        uint64_t site_id = va_arg(*app, uint64_t);
        if (telescope == NULL) {
            telescope = (struct TelescopeInfo *) Malloc(sizeof(struct TelescopeInfo));
            memset(telescope, '\0', sizeof(struct TelescopeInfo));
            self->telescope = telescope;
        }
        telescope->identifier = value;
        telescope->site_id = site_id;
    } else if (strcmp(name, "telescop") == 0) {
        struct TelescopeInfo *telescope= self->telescope;
        const char *value = va_arg(*app, const char *);
        if (telescope != NULL && value != NULL) {
            telescope->name = Realloc(telescope->name, strlen(value) + 1);
            snprintf(telescope->name, strlen(value) + 1, "%s", value);
        }
    } else if (strcmp(name, "tel_des") == 0) {
        struct TelescopeInfo *telescope= self->telescope;
        const char *value = va_arg(*app, const char *);
        if (telescope != NULL && value != NULL) {
            telescope->name = Realloc(telescope->description, strlen(value) + 1);
            snprintf(telescope->description, strlen(value) + 1, "%s", value);
        }
    } else if (strcmp(name, "db_host") == 0) {
        const char *value = va_arg(*app, const char *);
        if (value != NULL) {
            self->db_host = Realloc(self->db_host, strlen(value) + 1);
            snprintf(self->db_host, strlen(value) + 1, "%s", value);
        }
    } else if (strcmp(name, "db_user") == 0) {
        const char *value = va_arg(*app, const char *);
        if (value != NULL) {
            self->db_user = Realloc(self->db_user, strlen(value) + 1);
            snprintf(self->db_user, strlen(value) + 1, "%s", value);
        }
    } else if (strcmp(name, "db_passwd") == 0) {
        const char *value = va_arg(*app, const char *);
        if (value != NULL) {
            self->db_passwd = Realloc(self->db_passwd, strlen(value) + 1);
            snprintf(self->db_passwd, strlen(value) + 1, "%s", value);
        }
    } else if (strcmp(name, "db_name") == 0) {
        const char *value = va_arg(*app, const char *);
        if (value != NULL) {
            self->db_name = Realloc(self->db_name, strlen(value) + 1);
            snprintf(self->db_name, strlen(value) + 1, "%s", value);
        }
    } else if (strcmp(name, "site_db_table") == 0) {
        const char *value = va_arg(*app, const char *);
        if (value != NULL) {
            self->site_db_table = Realloc(self->site_db_table, strlen(value) + 1);
            snprintf(self->site_db_table, strlen(value) + 1, "%s", value);
        }
    } else if (strcmp(name, "telescope_db_table") == 0) {
        const char *value = va_arg(*app, const char *);
        if (value != NULL) {
            self->telescope_db_table = Realloc(self->telescope_db_table, strlen(value) + 1);
            snprintf(self->telescope_db_table, strlen(value) + 1, "%s", value);
        }
    } else if (strcmp(name, "target_db_table") == 0) {
        const char *value = va_arg(*app, const char *);
        if (value != NULL) {
            self->target_db_table = Realloc(self->target_db_table, strlen(value) + 1);
            snprintf(self->target_db_table, strlen(value) + 1, "%s", value);
        }
    } else if (strcmp(name, "task_db_table") == 0) {
        const char *value = va_arg(*app, const char *);
        if (value != NULL) {
            self->task_db_table = Realloc(self->task_db_table, strlen(value) + 1);
            snprintf(self->task_db_table, strlen(value) + 1, "%s", value);
        }
    } else if (strcmp(name, "add_rpc") == 0) {
        uint64_t identifier = va_arg(*app, uint64_t);
        void **value = va_arg(*app, void **);
        if (self->type == SCHEDULER_TYPE_GLOBAL) {
            struct SiteInfo *site; 
            if ((site = threadsafe_list_find_first_if(self->site_list, site_by_id, identifier)) != NULL && value != NULL) {
                site->rpc = *value;
            }
        } else if (self->type == SCHEDULER_TYPE_SITE) {
            struct TelescopeInfo *telescope; 
            if ((telescope = threadsafe_list_find_first_if(self->site_list, telescope_by_id, identifier)) != NULL && value != NULL) {
                telescope->rpc = *value;
            }
        } else {
            if (value != NULL && *value != NULL) {
                delete(*value);
                *value = NULL;
            }
        }
    }
}

int
__scheduler_register_thread(void *_self, uint64_t identifier, void *thread)
{
    const struct __SchedulerClass *class = (const struct __SchedulerClass *) classOf(_self);
    
    if (isOf(class, __SchedulerClass()) && class->register_thread.method) {
        return ((int (*)(void *, uint64_t, void *)) class->register_thread.method)(_self, identifier, thread);
    } else {
        int result;
        forward(_self, &result, (Method) __scheduler_register_thread, "register_thread", _self, identifier, thread);
        return result;
    }
}

static int
__Scheduler_register_thread(void *_self, uint64_t identifier, void *thread)
{
    struct __Scheduler *self = cast(__Scheduler(), _self);

    int ret;
    if (self->type == SCHEDULER_TYPE_UNIT) {
        return AAOS_ENOTSUP;
    } else if (self->type == SCHEDULER_TYPE_SITE) {
        
        threadsafe_list_operate_first_if(self->telescope_list, telescope_by_id , site_register_thread, identifier, thread);
    } else if (self->type == SCHEDULER_TYPE_GLOBAL) {
        struct SiteInfo *site;
        site = threadsafe_list_find_first_if(self->site_list, site_by_id, identifier);
        if (site != NULL) {
            Pthread_mutex_lock(&site->mtx);
            if (site->rpc != NULL) {
                delete(site->rpc);
            }
            site->rpc = thread;
            Pthread_mutex_unlock(&site->mtx);
        } else {
#ifdef DEBUG
#ifdef LINUX
            fprintf(stderr, "%s %s %d: site %lu is not found in site list.\n", __FILE__, __func__, __LINE__, identifier);
#else
            fprintf(stderr, "%s %s %d: site %llu is not found in site list.\n", __FILE__, __func__, __LINE__, identifier);
#endif
#endif
            return AAOS_ENOTFOUND;
        }
        //threadsafe_list_operate_first_if(self->site_list, site_by_id, global_register_thread, identifier, thread);
    }

    return AAOS_OK;
}

int
__scheduler_init(void *_self)
{
    const struct __SchedulerClass *class = (const struct __SchedulerClass *) classOf(_self);
    
    if (isOf(class, __SchedulerClass()) && class->init.method) {
        return ((int (*)(void *)) class->init.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) __scheduler_init, "init", _self);
        return result;
    }
}

static int
__Scheduler_init(void *_self)
{
    struct __Scheduler *self = cast(__Scheduler(), _self);

    MYSQL *mysql;
    MYSQL_RES res;
    char sql[BUFSIZE];
    pthread_t tid;
    FILE *fp;

    if (self->type == SCHEDULER_TYPE_GLOBAL) {
        /*
         * For global scheduler, read and initialize site_list, telescope_list, and target_list
         */
        __scheduler_create_sql(SCHEDULER_SITE_INIT, 0, self->site_db_table, sql, BUFSIZE);
        __Scheduler_database_query(self, sql, __Scheduler_site_init_cb);
        __scheduler_create_sql(SCHEDULER_TELESCOPE_INIT, 0, self->telescope_db_table, sql, BUFSIZE);
        __Scheduler_database_query(self, sql, __Scheduler_telescope_init_cb);
        __scheduler_create_sql(SCHEDULER_TARGET_INIT, 0, self->target_db_table, sql, BUFSIZE);
        __Scheduler_database_query(self, sql, __Scheduler_target_init_cb);
        if ((fp = popen(self->algorithm, "r")) != NULL ) {
            pclose(fp);
        }
        Pthread_create(&tid, NULL, __scheduler_site_manage_thr, self);
    } else if (self->type == SCHEDULER_TYPE_SITE) {
        /*
         * For site scheduler, read and initialize telescope_list, and target_list,
         * and then connect to global scheduling system.
         */
        __scheduler_create_sql(SCHEDULER_TELESCOPE_INIT, 0, self->telescope_db_table, sql, BUFSIZE);
        __Scheduler_database_query(self, sql, __Scheduler_telescope_init_cb);
        __scheduler_create_sql(SCHEDULER_TARGET_INIT, 0, self->target_db_table, sql, BUFSIZE);
        __Scheduler_database_query(self, sql, __Scheduler_target_init_cb);
        __scheduler_set_member(self, "connect_global");
        if ((fp = popen(self->algorithm, "r")) != NULL ) {
            pclose(fp);
        }
        Pthread_create(&tid, NULL, __scheduler_telescope_manage_thr, self);
    } else if (self->type == SCHEDULER_TYPE_UNIT) {
        /*
         * For unit scheduler, connect to site scheduling system.
         */
        __scheduler_set_member(self, "connect_site");
    }
    
    return AAOS_OK;
}

static void *
__Scheduler_ctor(void *_self, va_list *app)
{
    struct __Scheduler *self = super_ctor(__Scheduler(), _self, app);
    
    const char *s, *key, *value;

    self->max_task_in_block = SCHEDULER_MAX_TASK_IN_BLOCK;
    
    self->type = va_arg(*app, unsigned int);
    while ((key = va_arg(*app, const char *))) {
        if (strcmp(key, "description") == 0) {
            value = va_arg(*app, const char *);
            if (value) {
                self->description = (char *) Malloc(strlen(value) + 1);
                snprintf(self->description, strlen(value) + 1, "%s", value);
            }
            continue;
        }  
        if (strcmp(key, "db_host") == 0) {
            value = va_arg(*app, const char *);
            if (value) {
                self->db_host = (char *) Malloc(strlen(value) + 1);
                snprintf(self->db_host, strlen(value) + 1, "%s", value);
            }
            continue;
        }
        if (strcmp(key, "db_user") == 0) {
            value = va_arg(*app, const char *);
            if (value) {
                self->db_user = (char *) Malloc(strlen(value) + 1);
                snprintf(self->db_user, strlen(value) + 1, "%s", value);
            }
            continue;
        }
        if (strcmp(key, "db_passwd") == 0) {
            value = va_arg(*app, const char *);
            if (value) {
                self->db_passwd = (char *) Malloc(strlen(value) + 1);
                snprintf(self->db_passwd, strlen(value) + 1, "%s", value);
            }
            continue;
        }
        if (strcmp(key, "db_name") == 0) {
            value = va_arg(*app, const char *);
            if (value) {
                self->db_name = (char *) Malloc(strlen(value) + 1);
                snprintf(self->db_name, strlen(value) + 1, "%s", value);
            }
            continue;
        }
        if (strcmp(key, "site_db_table") == 0) {
            value = va_arg(*app, const char *);
            if (value) {
                self->site_db_table = (char *) Malloc(strlen(value) + 1);
                snprintf(self->site_db_table, strlen(value) + 1, "%s", value);
            }
            continue;
        }
        if (strcmp(key, "telescope_db_table") == 0) {
            value = va_arg(*app, const char *);
            if (value) {
                self->telescope_db_table = (char *) Malloc(strlen(value) + 1);
                snprintf(self->telescope_db_table, strlen(value) + 1, "%s", value);
            }
            continue;
        }
        if (strcmp(key, "target_db_table") == 0) {
            value = va_arg(*app, const char *);
            if (value) {
                self->target_db_table = (char *) Malloc(strlen(value) + 1);
                snprintf(self->target_db_table, strlen(value) + 1, "%s", value);
            }
            continue;
        }
        if (strcmp(key, "task_db_table") == 0) {
            value = va_arg(*app, const char *);
            if (value) {
                self->task_db_table = (char *) Malloc(strlen(value) + 1);
                snprintf(self->task_db_table, strlen(value) + 1, "%s", value);
            }
            continue;
        }
        if (strcmp(key, "global_addr") == 0) {
            value = va_arg(*app, const char *);
            if (value) {
                self->global_addr = (char *) Malloc(strlen(value) + 1);
                snprintf(self->global_addr, strlen(value) + 1, "%s", value);
            }
            continue;
        }
        if (strcmp(key, "global_port") == 0) {
            value = va_arg(*app, const char *);
            if (value) {
                self->global_port = (char *) Malloc(strlen(value) + 1);
                snprintf(self->global_port, strlen(value) + 1, "%s", value);
            }
            continue;
        }
        if (strcmp(key, "site_addr") == 0) {
            value = va_arg(*app, const char *);
            if (value) {
                self->site_addr = (char *) Malloc(strlen(value) + 1);
                snprintf(self->site_addr, strlen(value) + 1, "%s", value);
            }
            continue;
        }
        if (strcmp(key, "site_port") == 0) {
            value = va_arg(*app, const char *);
            if (value) {
                self->site_port = (char *) Malloc(strlen(value) + 1);
                snprintf(self->site_port, strlen(value) + 1, "%s", value);
            }
            continue;
        }
        if (strcmp(key, "ipc_model") == 0) {
            value = va_arg(*app, const char *);
            if (value) {
                self->ipc_model = (char *) Malloc(strlen(value) + 1);
                snprintf(self->ipc_model, strlen(value) + 1, "%s", value);
            }
            continue;
        }
        if (strcmp(key, "algorithm") == 0) {
            value = va_arg(*app, const char *);
            if (value) {
                self->algorithm = (char *) Malloc(strlen(value) + 1);
                snprintf(self->algorithm, strlen(value) + 1, "%s", value);
            }
            continue;
        }
        if (strcmp(key, "sock_file") == 0) {
            value = va_arg(*app, const char *);
            if (value) {
                self->sock_file = (char *) Malloc(strlen(value) + 1);
                snprintf(self->sock_file, strlen(value) + 1, "%s", value);
            }
            continue;
        }
        if (strcmp(key, "max_task_in_block") == 0) {
            self->max_task_in_block = va_arg(*app, size_t);
            continue;
        }
    }
    
    if (self->type == SCHEDULER_TYPE_SITE) {
        struct SiteInfo *site;
        site = (struct SiteInfo *) Malloc(sizeof(struct SiteInfo));
        Pthread_mutex_init(&site->mtx, NULL);
        Pthread_cond_init(&site->cond, NULL);
        self->site = site;
    } else if (self->type == SCHEDULER_TYPE_UNIT) {
        struct TelescopeInfo *telescope;
        telescope = (struct TelescopeInfo *) Malloc(sizeof(struct TelescopeInfo));
        self->telescope = telescope;
    }
    Pthread_mutex_init(&self->cnt_mtx, NULL);

    return (void *) self;
}

static void *
__Scheduler_dtor(void *_self)
{
    struct __Scheduler *self = cast(__Scheduler(), _self);

    free(self->description);

    free(self->db_host);
    free(self->db_user);
    free(self->db_passwd);
    free(self->db_name);
    free(self->site_db_table);
    free(self->telescope_db_table);
    free(self->target_db_table);
    free(self->task_db_table);

    if (self->site_list != NULL) {
        delete(self->site_list);
    }
    if (self->telescope_list != NULL) {
        delete(self->telescope_list);
    }
    if (self->target_list != NULL) {
        delete(self->target_list);
    }

    free(self->global_addr);
    free(self->global_port);
    
    free(self->site_addr);
    free(self->site_port);

    free(self->ipc_model);
    free(self->algorithm);
    free(self->sock_file);
    if (self->site != NULL) {
        cleanup_site_info(self->site);
    }
    if (self->telescope != NULL) {
        cleanup_site_info(self->telescope);
    }

    Pthread_mutex_destroy(&self->cnt_mtx);

    return super_dtor(__Scheduler(), _self);
}

static void *
__SchedulerClass_ctor(void *_self, va_list *app)
{
    struct __SchedulerClass *self = super_ctor(__SchedulerClass(), _self, app);
    Method selector;

#ifdef va_copy
    va_list ap;
    va_copy(ap, *app);
#else
    va_list ap = *app;
#endif

        while ((selector = va_arg(ap, Method))) {
        const char *tag = va_arg(ap, const char *);
        Method method = va_arg(ap, Method);
        if (selector == (Method) __scheduler_init) {
            if (tag) {
                self->init.tag = tag;
                self->init.selector = selector;
            }
            self->init.method = method;
            continue;
        }
        if (selector == (Method) __scheduler_set_member) {
            if (tag) {
                self->set_member.tag = tag;
                self->set_member.selector = selector;
            }
            self->set_member.method = method;
            continue;
        }
        if (selector == (Method) __scheduler_get_task_by_telescope_id) {
            if (tag) {
                self->get_task_by_telescope_id.tag = tag;
                self->get_task_by_telescope_id.selector = selector;
            }
            self->get_task_by_telescope_id.method = method;
            continue;
        }
        if (selector == (Method) __scheduler_get_task_by_telescope_name) {
            if (tag) {
                self->get_task_by_telescope_name.tag = tag;
                self->get_task_by_telescope_name.selector = selector;
            }
            self->get_task_by_telescope_name.method = method;
            continue;
        }
        if (selector == (Method) __scheduler_pop_task_block) {
            if (tag) {
                self->pop_task_block.tag = tag;
                self->pop_task_block.selector = selector;
            }
            self->pop_task_block.method = method;
            continue;
        }
        if (selector == (Method) __scheduler_push_task_block) {
            if (tag) {
                self->push_task_block.tag = tag;
                self->push_task_block.selector = selector;
            }
            self->push_task_block.method = method;
            continue;
        }

        if (selector == (Method) __scheduler_update_status) {
            if (tag) {
                self->update_status.tag = tag;
                self->update_status.selector = selector;
            }
            self->update_status.method = method;
            continue;
        }

        if (selector == (Method) __scheduler_list_site) {
            if (tag) {
                self->list_site.tag = tag;
                self->list_site.selector = selector;
            }
            self->list_site.method = method;
            continue;
        }
        if (selector == (Method) __scheduler_add_site) {
            if (tag) {
                self->add_site.tag = tag;
                self->add_site.selector = selector;
            }
            self->add_site.method = method;
            continue;
        }
        if (selector == (Method) __scheduler_delete_site_by_id) {
            if (tag) {
                self->delete_site_by_id.tag = tag;
                self->delete_site_by_id.selector = selector;
            }
            self->delete_site_by_id.method = method;
            continue;
        }
        if (selector == (Method) __scheduler_delete_site_by_name) {
            if (tag) {
                self->delete_site_by_name.tag = tag;
                self->delete_site_by_name.selector = selector;
            }
            self->delete_site_by_name.method = method;
            continue;
        }
        if (selector == (Method) __scheduler_mask_site_by_id) {
            if (tag) {
                self->mask_site_by_id.tag = tag;
                self->mask_site_by_id.selector = selector;
            }
            self->mask_site_by_id.method = method;
            continue;
        }
        if (selector == (Method) __scheduler_mask_site_by_name) {
            if (tag) {
                self->mask_site_by_name.tag = tag;
                self->mask_site_by_name.selector = selector;
            }
            self->mask_site_by_name.method = method;
            continue;
        }
        if (selector == (Method) __scheduler_unmask_site_by_id) {
            if (tag) {
                self->unmask_site_by_id.tag = tag;
                self->unmask_site_by_id.selector = selector;
            }
            self->unmask_site_by_id.method = method;
            continue;
        }
        if (selector == (Method) __scheduler_unmask_site_by_name) {
            if (tag) {
                self->unmask_site_by_name.tag = tag;
                self->unmask_site_by_name.selector = selector;
            }
            self->unmask_site_by_name.method = method;
            continue;
        }
        if (selector == (Method) __scheduler_list_telescope) {
            if (tag) {
                self->list_telescope.tag = tag;
                self->list_telescope.selector = selector;
            }
            self->list_telescope.method = method;
            continue;
        }
        if (selector == (Method) __scheduler_add_telescope) {
            if (tag) {
                self->add_telescope.tag = tag;
                self->add_telescope.selector = selector;
            }
            self->add_telescope.method = method;
            continue;
        }
        if (selector == (Method) __scheduler_delete_telescope_by_id) {
            if (tag) {
                self->delete_telescope_by_id.tag = tag;
                self->delete_telescope_by_id.selector = selector;
            }
            self->delete_telescope_by_id.method = method;
            continue;
        }
        if (selector == (Method) __scheduler_delete_telescope_by_name) {
            if (tag) {
                self->delete_telescope_by_name.tag = tag;
                self->delete_telescope_by_name.selector = selector;
            }
            self->delete_telescope_by_name.method = method;
            continue;
        }
        if (selector == (Method) __scheduler_mask_telescope_by_id) {
            if (tag) {
                self->mask_telescope_by_id.tag = tag;
                self->mask_telescope_by_id.selector = selector;
            }
            self->mask_telescope_by_id.method = method;
            continue;
        }
        if (selector == (Method) __scheduler_mask_telescope_by_name) {
            if (tag) {
                self->mask_telescope_by_name.tag = tag;
                self->mask_telescope_by_name.selector = selector;
            }
            self->mask_telescope_by_name.method = method;
            continue;
        }
        if (selector == (Method) __scheduler_unmask_telescope_by_id) {
            if (tag) {
                self->unmask_telescope_by_id.tag = tag;
                self->unmask_telescope_by_id.selector = selector;
            }
            self->unmask_telescope_by_id.method = method;
            continue;
        }
        if (selector == (Method) __scheduler_unmask_telescope_by_name) {
            if (tag) {
                self->unmask_telescope_by_name.tag = tag;
                self->unmask_telescope_by_name.selector = selector;
            }
            self->unmask_telescope_by_name.method = method;
            continue;
        }

        if (selector == (Method) __scheduler_list_target) {
            if (tag) {
                self->list_target.tag = tag;
                self->list_target.selector = selector;
            }
            self->list_target.method = method;
            continue;
        }
        if (selector == (Method) __scheduler_add_target) {
            if (tag) {
                self->add_target.tag = tag;
                self->add_target.selector = selector;
            }
            self->add_target.method = method;
            continue;
        }
        if (selector == (Method) __scheduler_delete_target_by_id) {
            if (tag) {
                self->delete_target_by_id.tag = tag;
                self->delete_target_by_id.selector = selector;
            }
            self->delete_target_by_id.method = method;
            continue;
        }
        if (selector == (Method) __scheduler_delete_target_by_name) {
            if (tag) {
                self->delete_target_by_name.tag = tag;
                self->delete_target_by_name.selector = selector;
            }
            self->delete_target_by_name.method = method;
            continue;
        }
        if (selector == (Method) __scheduler_mask_target_by_id) {
            if (tag) {
                self->mask_target_by_id.tag = tag;
                self->mask_target_by_id.selector = selector;
            }
            self->mask_target_by_id.method = method;
            continue;
        }
        if (selector == (Method) __scheduler_mask_target_by_name) {
            if (tag) {
                self->mask_target_by_name.tag = tag;
                self->mask_target_by_name.selector = selector;
            }
            self->mask_target_by_name.method = method;
            continue;
        }
        if (selector == (Method) __scheduler_unmask_target_by_id) {
            if (tag) {
                self->unmask_target_by_id.tag = tag;
                self->unmask_target_by_id.selector = selector;
            }
            self->unmask_target_by_id.method = method;
            continue;
        }
        if (selector == (Method) __scheduler_unmask_target_by_name) {
            if (tag) {
                self->unmask_target_by_name.tag = tag;
                self->unmask_target_by_name.selector = selector;
            }
            self->unmask_target_by_name.method = method;
            continue;
        }

        if (selector == (Method) __scheduler_add_task_record) {
            if (tag) {
                self->add_task_record.tag = tag;
                self->add_task_record.selector = selector;
            }
            self->add_task_record.method = method;
            continue;
        }
        if (selector == (Method) __scheduler_update_task_record) {
            if (tag) {
                self->update_task_record.tag = tag;
                self->update_task_record.selector = selector;
            }
            self->update_task_record.method = method;
            continue;
        }
        if (selector == (Method) __scheduler_update_task_status) {
            if (tag) {
                self->update_task_status.tag = tag;
                self->update_task_status.selector = selector;
            }
            self->update_task_status.method = method;
            continue;
        }
        if (selector == (Method) __scheduler_register_thread) {
            if (tag) {
                self->register_thread.tag = tag;
                self->register_thread.selector = selector;
            }
            self->register_thread.method = method;
            continue;
        }
        if (selector == (Method) __scheduler_site_manage_thr) {
            if (tag) {
                self->site_manage_thr.tag = tag;
                self->site_manage_thr.selector = selector;
            }
            self->site_manage_thr.method = method;
            continue;
        }
        if (selector == (Method) __scheduler_telescope_manage_thr) {
            if (tag) {
                self->telescope_manage_thr.tag = tag;
                self->telescope_manage_thr.selector = selector;
            }
            self->telescope_manage_thr.method = method;
            continue;
        }

    }
    
#ifdef va_copy
    va_end(ap);
#endif
    
    return (void *) self;
}

static const void *___SchedulerClass;

static void
__SchedulerClass_destroy(void)
{
    free((void *) ___SchedulerClass);
}

static void
__SchedulerClass_initialize(void)
{
    ___SchedulerClass = new(Class(), "__SchedulerClass", Class(), sizeof(struct __SchedulerClass),
                        ctor, "ctor", __SchedulerClass_ctor,
                        (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(__SchedulerClass_destroy);
#endif
}

const void *
__SchedulerClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, __SchedulerClass_initialize);
#endif
    
    return ___SchedulerClass;
}

static const void *___Scheduler;

static void
__Scheduler_destroy(void)
{
    free((void *)___Scheduler);
}

static void
__Scheduler_initialize(void)
{
    ___Scheduler = new(__SchedulerClass(), "__Scheduler", Object(), sizeof(struct __Scheduler),
                       ctor, "ctor", __Scheduler_ctor,
                       dtor, "dtor", __Scheduler_dtor,
                       
                       __scheduler_init, "init", __Scheduler_init,
                       __scheduler_set_member, "set_member", __Scheduler_set_member,

                       __scheduler_get_task_by_telescope_id, "get_task_by_telescope_id", __Scheduler_get_task_by_telescope_id,
                       __scheduler_get_task_by_telescope_name, "get_task_by_telescope_name", __Scheduler_get_task_by_telescope_name,
                       __scheduler_pop_task_block, "pop_task_block", __Scheduler_pop_task_block,
                       __scheduler_push_task_block, "push_task_block", __Scheduler_push_task_block,

                       __scheduler_update_status, "update_status", __Scheduler_update_status,
                       
                       __scheduler_list_site, "list_site", __Scheduler_list_site,
                       __scheduler_add_site, "add_site", __Scheduler_add_site,
                       __scheduler_delete_site_by_id, "delete_site_by_id", __Scheduler_delete_site_by_id,
                       __scheduler_delete_site_by_name, "delete_site_by_name", __Scheduler_delete_site_by_name,
                       __scheduler_mask_site_by_id, "mask_site_by_id", __Scheduler_mask_site_by_id,
                    
                       __scheduler_mask_site_by_name, "mask_site_by_name", __Scheduler_mask_site_by_name,
                       __scheduler_unmask_site_by_id, "unmask_site_by_id", __Scheduler_unmask_site_by_id,
                       __scheduler_unmask_site_by_name, "unmask_site_by_name", __Scheduler_unmask_site_by_name,

                       __scheduler_list_telescope, "list_telescope", __Scheduler_list_telescope,
                       __scheduler_add_telescope, "add_telescope", __Scheduler_add_telescope,
                       __scheduler_delete_telescope_by_id, "delete_telescope_by_id", __Scheduler_delete_telescope_by_id,
                       __scheduler_delete_telescope_by_name, "delete_telescope_by_name", __Scheduler_delete_telescope_by_name,
                       __scheduler_mask_telescope_by_id, "mask_telescope_by_id", __Scheduler_mask_telescope_by_id,
                       __scheduler_mask_telescope_by_name, "mask_telescope_by_name", __Scheduler_mask_telescope_by_name,
                       __scheduler_unmask_telescope_by_id, "unmask_telescope_by_id", __Scheduler_unmask_telescope_by_id,
                       __scheduler_unmask_telescope_by_name, "unmask_telescope_by_name", __Scheduler_unmask_telescope_by_name,

                       __scheduler_list_target, "list_target", __Scheduler_list_target,
                       __scheduler_add_target, "add_target", __Scheduler_add_target,
                       __scheduler_delete_target_by_id, "delete_target_by_id", __Scheduler_delete_target_by_id,
                       __scheduler_delete_target_by_name, "delete_target_by_name", __Scheduler_delete_target_by_name,
                       __scheduler_mask_target_by_id, "mask_target_by_id", __Scheduler_mask_target_by_id,
                       __scheduler_mask_target_by_name, "mask_target_by_name", __Scheduler_mask_target_by_name,
                       __scheduler_unmask_target_by_id, "unmask_target_by_id", __Scheduler_unmask_target_by_id,
                       __scheduler_unmask_target_by_name, "unmask_target_by_name", __Scheduler_unmask_target_by_name,

                       __scheduler_add_task_record, "add_task_record", __Scheduler_add_task_record,
                       __scheduler_update_task_record, "update_task_record", __Scheduler_update_task_record,
                       __scheduler_update_task_status, "update_task_status", __Scheduler_update_task_status,
                       
                       __scheduler_register_thread, "register_thread", __Scheduler_register_thread,
                       __scheduler_site_manage_thr, "site_manage_thr", __Scheduler_site_manage_thr,
                       __scheduler_telescope_manage_thr, "telescope_manage_thr", __Scheduler_telescope_manage_thr,
                       (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(__Scheduler_destroy);
#endif
}

const void *
__Scheduler(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, __Scheduler_initialize);
#endif
    
    return ___Scheduler;
}

/*
 * Compiler-dependant initializer.
 */
#ifdef _USE_COMPILER_ATTRIBUTION_
static void __destructor__(void) __attribute__ ((destructor(_SERIAL_RPC_PRIORITY_)));

static void
__destructor__(void)
{
    __SchedulerClass_destroy();
    __Scheduler_destroy();
}

static void __constructor__(void) __attribute__ ((constructor(_SERIAL_RPC_PRIORITY_)));

static void
__constructor__(void)
{
    __SchedulerClass_initialize();
    __Scheduler_initialize();
}
#endif
