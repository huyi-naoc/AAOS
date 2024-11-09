//
//  scheduler.c
//  AAOS
//
//  Created by huyi on 2024/07/18.
//  Copyright © 2024 National Astronomical Observatories, Chinese Academy of Sciences. All rights reserved.
//

#include "adt.h"
#include "def.h"
#include "scheduler_def.h"
#include "scheduler.h"
#include "scheduler_r.h"
#include "wrapper.h"

#include <chealpix.h>
#include <cjson/cJSON.h>
#include <mysql.h>

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
    void *rpc;
};

typedef int (*database_cb_t)(struct __Scheduler *, MYSQL_RES *);

static int
__scheduler_site_init_cb(struct __Scheduler *self, MYSQL_RES *res)
{

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

    if (command == SCHEDULER_DELETE_TELESCOPE_BY_ID) {
        timestamp = va_arg(ap, double);
        snprintf(sql, size, "UPDATE %s SET status=%d, timestamp=%lf WHERE tel_id=%ld", table, SCHEDULER_STATUS_DELETE, timestamp, identifier);
    } else if (command == SCHEDULER_MASK_TELESCOPE_BY_ID) {
        snprintf(sql, size, "UPDATE %s SET status=%d, timestamp=%lf WHERE tel_id=%ld", table, SCHEDULER_STATUS_MASKED, timestamp, identifier);
    } else if (command == SCHEDULER_UNMASK_TELESCOPE_BY_ID) {
        snprintf(sql, size, "UPDATE %s SET status=%d, timestamp=%lf WHERE tel_id=%ld", table, SCHEDULER_STATUS_OK, timestamp, identifier);   
    } else if (command == SCHEDULER_ADD_TELESCOPE) {
        uint64_t tel_id, site_id;
        const char *name, *description;
        int status = SCHEDULER_STATUS_OK;
        name = va_arg(ap, const char *);
        tel_id = va_arg(ap, uint64_t);
        site_id = va_arg(ap, uint64_t);
        description = va_arg(ap, const char *);
        timestamp = va_arg(ap, double);
        snprintf(sql, size, "INSERT INTO %s (telescop, tel_id, site_id, tel_des, status, timestamp) VALUES (\"%s\", %ld, %ld, \"%s\", %d, %lf)", table, name, tel_id, site_id, description, status, timestamp);
    } else if (command == SCHEDULER_DELETE_SITE_BY_ID) {
        timestamp = va_arg(ap, double);
        snprintf(sql, size, "UPDATE %s SET status=%d, timestamp=%lf WHERE site_id=%ld", table, SCHEDULER_STATUS_DELETE, timestamp, identifier);
    } else if (command == SCHEDULER_MASK_SITE_BY_ID) {
        timestamp = va_arg(ap, double);
        snprintf(sql, size, "UPDATE %s SET status=%d, timestamp=%lf WHERE site_id=%ld", table, SCHEDULER_STATUS_MASKED, timestamp, identifier);
    } else if (command == SCHEDULER_UNMASK_SITE_BY_ID) {
        timestamp = va_arg(ap, double);
        snprintf(sql, size, "UPDATE %s SET status=%d, timestamp=%lf WHERE site_id=%ld", table, SCHEDULER_STATUS_OK, timestamp, identifier);   
    } else if (command == SCHEDULER_ADD_SITE) {
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
        snprintf(sql, size, "INSERT INTO %s (sitename, site_id, status, site_lat, site_lon, site_alt, timestamp) VALUES (\"%s\", %ld, %d, %lf, %lf, %lf, %lf)", table, name, site_id, status, site_lon, site_lat, site_alt, timestamp);
    } if (command == SCHEDULER_DELETE_TARGET_BY_ID) {
        uint32_t nside = va_arg(ap, uint32_t);
        timestamp = va_arg(ap, double);
        snprintf(sql, size, "UPDATE %s SET status=%d, timestamp=%lf WHERE targ_id=%ld AND nside=%u", table, SCHEDULER_STATUS_DELETE, timestamp, identifier, nside);
    } else if (command == SCHEDULER_MASK_TARGET_BY_ID) {
        snprintf(sql, size, "UPDATE %s SET status=%d, timestamp=%lf WHERE targ_id=%ld AND nside=%u", table, SCHEDULER_STATUS_MASKED, timestamp, identifier, nside);
    } else if (command == SCHEDULER_UNMASK_TARGET_BY_ID) {
        snprintf(sql, size, "UPDATE %s SET status=%d, timestamp=%lf WHERE targ_id=%ld AND nside=%u", table, SCHEDULER_STATUS_OK, timestamp, identifier, nside);   
    } else if (command == SCHEDULER_ADD_TARGET) {
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
            snprintf(sql, size, "INSERT INTO %s (targname, targ_id, nside, ra_tag, dec_tag, status, priority, timestamp) VALUES (\"%s\", %ld, %u, %lf, %lf, %d, %d, %lf)", table, name, targ_id, nside, ra, dec, status, priority, timestamp);
        } else {
            snprintf(sql, size, "INSERT INTO %s (targ_id, nside, ra_tag, dec_tag, status, priority, timestamp) VALUES (%ld, %u, %lf, %lf, %d, %d, %lf)", table, targ_id, nside, ra, dec, status, priority, timestamp);
        }
    } else if (command == SCHEDULER_ADD_TASK_RECORD) {
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
        snprintf(sql, size, "INSERT INTO %s (task_id, targ_id, nside, tel_id, site_id, status, task_des, obstime, timestamp) VALUES (%ld, %ld, %u, %ld, %ld, %d, \"%s\", %lf, %lf)", table, task_id, targ_id, nside, tel_id, site_id, status, description, obstime, timestamp);
    } else if (command == SCHEDULER_UPDATE_TASK_STATUS) {
        int status = va_arg(ap, int);
        timestamp = va_arg(ap, double);
        snprintf(sql, size, "UPDATE %s SET status=%d, timestamp=%lf WHERE task_id=%ld", table, status, timestamp, identifier); 
    } else if (command == SCHEDULER_SITE_INIT) {
        snprintf(sql, size, "SELECT sitename,site_id,status,site_lon,site_lat,site_alt FROM %s", table); 
    } else if (command == SCHEDULER_TELESCOPE_INIT) {
        snprintf(sql, size, "SELECT telescop,tel_id,site_id,status,tel_des FROM %s", table); 
    } else if (command == SCHEDULER_TARGET_INIT) {
        snprintf(sql, size, "SELECT targname,targ_id,nside,status,ra_targ,dec_targ,priority FROM %s", table); 
    }

    va_end(ap);
}

static predict 
telescope_by_id(struct TelescopeInfo *telescope_info, va_list *app)
{
    uint64_t identifier = va_arg(*app, uint64_t);

    if (telescope_info->identifier == identifier) {
        return true;
    } else {
        return false;
    }
}

static predict 
telescope_by_name(struct TelescopeInfo *telescope_info, va_list *app)
{
    const char *name = va_arg(*app, const char *);

    if (telescope_info->name == name) {
        return true;
    } else {
        return false;
    }
}

static predict 
site_by_id(struct SiteInfo *site_info, va_list *app)
{
    uint64_t identifier = va_arg(*app, uint64_t);

    if (site_info->identifier == identifier) {
        return true;
    } else {
        return false;
    }
}

static predict 
site_by_name(struct SiteInfo *site_info, va_list *app)
{
    const char *name;

    name = va_arg(*app,  const char *);

    if (strcmp(site_info->name, name) == 0) {
        return true;
    } else {
        return false;
    }
}

static predict 
target_by_id(struct TargetInfo *target_info, va_list *app)
{
    uint64_t identifier = va_arg(*app, uint64_t);
    uint32_t nside = va_arg(*app, uint32_t);

    identifier = va_arg(*app, unsigned int);

    if (target_info->identifier == identifier && target_info->nside == nside) {
        return true;
    } else {
        return false;
    }
}

static predict 
target_by_name(struct Target *target_info, va_list *app)
{
    const char *name;

    name = va_arg(*app,  const char *);

    if (strcmp(target_info->name, name) == 0) {
        return true;
    } else {
        return false;
    }
}

static disposition
print_site_list(struct SiteInfo *site_info, va_list *app)
{
    FILE *fp = va_arg(*app, FILE *);

    cJSON *root_json, *value_json;
    root_json = cJSON_CreateObject();
    char *string;

    if (site_info->name != NULL) {
        value_json = cJSON_CreateString(site_info->name);
        cJSON_AddItemToObject(root_json, "sitename", value_json);
    }
    value_json = cJSON_CreateNumber(site_info->identifier);
    cJSON_AddItemToObject(root_json, "site_id", value_json);
    value_json = cJSON_CreateNumber(site_info->status);
    cJSON_AddItemToObject(root_json, "status", value_json);
    value_json = cJSON_CreateNumber(site_info->site_lon);
    cJSON_AddItemToObject(root_json, "site_lon", value_json);
    value_json = cJSON_CreateNumber(site_info->site_lat);
    cJSON_AddItemToObject(root_json, "site_lat", value_json);
    value_json = cJSON_CreateNumber(site_info->site_alt);
    cJSON_AddItemToObject(root_json, "site_alt", value_json);

    string = cJSON_Print(root_json);
    cJSON_Delete(root_json);

    fprintf(fp, "%s", string);
    free(string);
}

static disposition
print_telescope_list(struct TelescopeInfo *telescope_info, va_list *app)
{
    FILE *fp = va_arg(*app, FILE *);
    
    cJSON *root_json, *value_json, *test_json;
    char *string;
    
    if (telescope_info->description != NULL) {
        root_json = cJSON_Parse(telescope_info->description);
    } else {
        root_json = cJSON_CreateObject();
    }

    if (telescope_info->name != NULL && cJSON_GetObjectItemCaseSensitive(root_json, "telescop") == NULL) {
        value_json = cJSON_CreateString(telescope_info->name);
        cJSON_AddItemToObject(root_json, "telescop", value_json);
    }
    if (cJSON_GetObjectItemCaseSensitive(root_json, "tel_id") == NULL) {
        value_json = cJSON_CreateString(telescope_info->identifier);
        cJSON_AddItemToObject(root_json, "tel_id", value_json);
    }
    if (cJSON_GetObjectItemCaseSensitive(root_json, "site_id") == NULL) {
        value_json = cJSON_CreateString(telescope_info->site_id);
        cJSON_AddItemToObject(root_json, "site_id", value_json);
    }
    if ((value_json = cJSON_GetObjectItemCaseSensitive(root_json, "status")) == NULL) {
        value_json = cJSON_CreateString(telescope_info->status);
        cJSON_AddItemToObject(root_json, "status", value_json);
    } else {
        cJSON_SetIntValue(value_json, telescope_info->status);
    }

    string = cJSON_Print(root_json);
    cJSON_Delete(root_json);

    fprintf(fp, "%s", string);
    free(string);
}

static disposition
print_target_list(struct TargetInfo *target_info, va_list *app)
{
    FILE *fp = va_arg(*app, FILE *);
    
    cJSON *root_json, *value_json, *test_json;
    char *string;
    
    if (target_info->description != NULL) {
        root_json = cJSON_Parse(target_info->description);
    } else {
        root_json = cJSON_CreateObject();
    }

    if (target_info->name != NULL && cJSON_GetObjectItemCaseSensitive(root_json, "targname") == NULL) {
        value_json = cJSON_CreateString(target_info->name);
        cJSON_AddItemToObject(root_json, "targname", value_json);
    }
    if ((value_json = cJSON_GetObjectItemCaseSensitive(root_json, "targ_id")) == NULL) {
        value_json = cJSON_CreateString(target_info->identifier);
        cJSON_AddItemToObject(root_json, "targ_id", value_json);
    } else {
        cJSON_SetIntValue(value_json, target_info->targ_id);
    }
    if ((value_json = cJSON_GetObjectItemCaseSensitive(root_json, "nside")) == NULL) {
        value_json = cJSON_CreateString(target_info->nside);
        cJSON_AddItemToObject(root_json, "nside", value_json);
    } else {
        cJSON_SetIntValue(value_json, target_info->nside);
    }
    if ((value_json = cJSON_GetObjectItemCaseSensitive(root_json, "targ_ra")) == NULL) {
        value_json = cJSON_CreateString(target_info->ra);
        cJSON_AddItemToObject(root_json, "targ_ra", value_json);
    } else {
        cJSON_SetIntValue(value_json, target_info->targ_ra);
    }
    if ((value_json = cJSON_GetObjectItemCaseSensitive(root_json, "targ_dec")) == NULL) {
        value_json = cJSON_CreateString(target_info->dec);
        cJSON_AddItemToObject(root_json, "targ_dec", value_json);
    } else {
        cJSON_SetIntValue(value_json, target_info->targ_dec);
    }
    if ((value_json = cJSON_GetObjectItemCaseSensitive(root_json, "status")) == NULL) {
        value_json = cJSON_CreateString(target_info->status);
        cJSON_AddItemToObject(root_json, "status", value_json);
    } else {
        cJSON_SetIntValue(value_json, targ_info->status);
    }
    if ((value_json = cJSON_GetObjectItemCaseSensitive(root_json, "priority")) == NULL) {
        value_json = cJSON_CreateString(target_info->priority);
        cJSON_AddItemToObject(root_json, "priority", value_json);
    } else {
        cJSON_SetIntValue(value_json, target_info->priority);
    }
    
    string = cJSON_Print(root_json);
    cJSON_Delete(root_json);

    fprintf(fp, "%s", string);
    free(string);
}


static char *
__scheduler_create_request_json_string(unsigned int command, ...)
{
    struct timespec tp;
    double timestamp;
    cJSON *root_json, *general_json, *site_json, *telescope_json, *value_json;
    char *json_string;
    va_list ap;

    Clock_gettime(CLOCK_REALTIME, &tp);
    timestamp = tp.tv_sec + tp.tv_nsec / 1000000000.;

    va_start(ap, command);
    root_json = cJSON_CreateObject();
    if (command == SCHEDULER_GET_TASK_BY_TELESCOPE_ID) {
        unsigned int identifier = va_arg(ap, uint64_t);
        general_json = cJSON_CreateObject();
        cJSON_AddItemToObject(root_json, "GENERAL-INFO", general_json);
        value_json = cJSON_CreateString("request");
        cJSON_AddItemToObject(general_json, "operate", value_json);
        value_json = cJSON_CreateNumber(timestamp);
        cJSON_AddItemToObject(general_json, "timestam", value_json);
        telescope_json = cJSON_CreateObject();
        cJSON_AddItemToObject(root_json, "TELESCOPE-INFO", telescope_json);
        value_json = cJSON_CreateNumber(identifier);
        cJSON_AddItemToObject(telescope_json, "tel_id", value_json);
        json_string = cJSON_Print(root_json);
    } else if (command == SCHEDULER_GET_TASK_BY_TELESCOPE_NAME) {
        const char  *name = va_arg(ap, const char *);
        general_json = cJSON_CreateObject();
        cJSON_AddItemToObject(root_json, "GENERAL-INFO", general_json);
        value_json = cJSON_CreateString("request");
        cJSON_AddItemToObject(general_json, "operate", value_json);
        value_json = cJSON_CreateNumber(timestamp);
        cJSON_AddItemToObject(general_json, "timestam", value_json);
        telescope_json = cJSON_CreateObject();
        cJSON_AddItemToObject(root_json, "TELESCOPE-INFO", telescope_json);
        value_json = cJSON_CreateString(name);
        cJSON_AddItemToObject(telescope_json, "telescop", value_json);
        json_string = cJSON_Print(root_json);
    } else if (command == SCHEDULER_POP_TASK_BLOCK) {
        general_json = cJSON_CreateObject();
        cJSON_AddItemToObject(root_json, "GENERAL-INFO", general_json);
        value_json = cJSON_CreateString("request");
        cJSON_AddItemToObject(general_json, "operate", value_json);
        value_json = cJSON_CreateNumber(timestamp);
        cJSON_AddItemToObject(general_json, "timestam", value_json);
        cJSON_Delete(root_json);
        json_string = cJSON_Print(root_json);
    } else if (command == SCHEDULER_TASK_BLOCK_ACK) {
        general_json = cJSON_CreateObject();
        cJSON_AddItemToObject(root_json, "GENERAL-INFO", general_json);
        value_json = cJSON_CreateString("acknowledge");
        cJSON_AddItemToObject(general_json, "operate", value_json);
        value_json = cJSON_CreateNumber(timestamp);
        cJSON_AddItemToObject(general_json, "timestam", value_json);
        cJSON_Delete(root_json);
        json_string = cJSON_Print(root_json);
    } else if (command == SCHEDULER_PUSH_TASK_BLOCK) {
        general_json = cJSON_CreateObject();
        cJSON_AddItemToObject(root_json, "GENERAL-INFO", general_json);
        value_json = cJSON_CreateString("request");
        cJSON_AddItemToObject(general_json, "operate", value_json);
        value_json = cJSON_CreateNumber(timestamp);
        cJSON_AddItemToObject(general_json, "timestam", value_json);
        cJSON_Delete(root_json);
        json_string = cJSON_Print(root_json);
    } else if (command == SCHEDULER_DELETE_SITE_BY_ID) {
        unsigned int identifier, status;
        identifier = va_arg(ap, uint64_t);
        status = SCHEDULER_STATUS_DELETE;
        general_json = cJSON_CreateObject();
        cJSON_AddItemToObject(root_json, "GENERAL-INFO", general_json);
        value_json = cJSON_CreateString("update");
        cJSON_AddItemToObject(general_json, "operate", value_json);
        value_json = cJSON_CreateNumber(timestamp);
        cJSON_AddItemToObject(general_json, "timestam", value_json);
        site_json = cJSON_CreateObject();
        cJSON_AddItemToObject(root_json, "SITE-INFO", site_json);
        value_json = cJSON_CreateNumber(identifier);
        cJSON_AddItemToObject(site_json, "site_id", value_json);
        value_json = cJSON_CreateNumber(status);
        cJSON_AddItemToObject(site_json, "status", value_json);
        cJSON_Delete(root_json);
        json_string = cJSON_Print(root_json);
    } else if (command == SCHEDULER_MASK_SITE_BY_ID) {
        unsigned int identifier, status;
        identifier = va_arg(ap, uint64_t);
        status = SCHEDULER_STATUS_MASK;
        general_json = cJSON_CreateObject();
        cJSON_AddItemToObject(root_json, "GENERAL-INFO", general_json);
        value_json = cJSON_CreateString("update");
        cJSON_AddItemToObject(general_json, "operate", value_json);
        value_json = cJSON_CreateNumber(timestamp);
        cJSON_AddItemToObject(general_json, "timestam", value_json);
        site_json = cJSON_CreateObject();
        cJSON_AddItemToObject(root_json, "SITE-INFO", site_json);
        value_json = cJSON_CreateNumber(identifier);
        cJSON_AddItemToObject(site_json, "site_id", value_json);
        value_json = cJSON_CreateNumber(status);
        cJSON_AddItemToObject(site_json, "status", value_json);
        cJSON_Delete(root_json);
        json_string = cJSON_Print(root_json);
    } else if (command == SCHEDULER_UNMASK_SITE_BY_ID) {
        unsigned int identifier, status;
        identifier = va_arg(ap, uint64_t);
        status = SCHEDULER_STATUS_OK;
        general_json = cJSON_CreateObject();
        cJSON_AddItemToObject(root_json, "GENERAL-INFO", general_json);
        value_json = cJSON_CreateString("update");
        cJSON_AddItemToObject(general_json, "operate", value_json);
        value_json = cJSON_CreateNumber(timestamp);
        cJSON_AddItemToObject(general_json, "timestam", value_json);
        site_json = cJSON_CreateObject();
        cJSON_AddItemToObject(root_json, "SITE-INFO", site_json);
        value_json = cJSON_CreateNumber(identifier);
        cJSON_AddItemToObject(site_json, "site_id", value_json);
        value_json = cJSON_CreateNumber(status);
        cJSON_AddItemToObject(site_json, "status", value_json);
        cJSON_Delete(root_json);
        json_string = cJSON_Print(root_json);
    }

    cJSON_Delete(root_json);
    va_end(ap);
    return json_string;
}

int
__Scheduler_ipc_write(struct __Scheduler *self, const char *string)
{
    int sockfd, cfd;
    uint32_t length;
    ssize_t nwrite;
    char buf[BUFSIZE];
    struct sockaddr_un addr;
   
    memset(&addr, '\0', sizeof(struct sockaddr_un));
    addr.sun_family  = AF_UNIX;
    snprintf(addr.sun_path, PATHSIZE, "%s", self->socket_file);

    if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        return AAOS_ERROR;
    }

    if ((cfd = connect(sockfd, (struct sockadd *)&addr, sizeof(struct sockadd_un))) < 0) {
        close(sockfd);
        return AAOS_ERROR;
    }

    length = strlen(string);
    memcpy(buf, &length, sizeof(uint32_t));
    snprintf(buf + sizeof(uint32_t), BUFSIZE - sizeof(uint32_t), "%s", string);

    if ((nwrite = Writen(cfd)) < 0) {
        close(cfd);
        close(sockfd);
    }

    close(cfd);
    close(sockfd);    
    return AAOS_OK;
}

static int
__Scheduler_ipc_write_and_read(struct __Scheduler *self, const char *string, char *res, size_t size, size_t *len)
{
    int sockfd, cfd; 
    uint32_t length;
    ssize_t nread, nwrite;
    char buf[BUFSIZE];
    struct sockaddr_un addr;

    memset(&addr, '\0', sizeof(struct sockaddr_un));
    addr.sun_family  = AF_UNIX;
    snprintf(addr.sun_path, PATHSIZE, "%s", self->socket_file);

    if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        return AAOS_ERROR;
    }

    if ((cfd = connect(sockfd, (struct sockadd *)&addr, sizeof(struct sockadd_un))) < 0) {
        close(sockfd);
        return AAOS_ERROR;
    }

    length = strlen(string);
    memcpy(buf, &length, sizeof(uint32_t));
    snprintf(buf + sizeof(uint32_t), BUFSIZE - sizeof(uint32_t), "%s", string);

    if ((nwrite = Writen(cfd, buf, length + sizeof(uint32_t))) < 0) {
        close(cfd);
        close(sockfd);
    }

    if ((nread = Readn(cfd, &length, sizeof(uint32_t))) < 0) {
        close(cfd);
        close(sockfd);
    }
    if ((nread = Readn(cfd, res, length)) < 0) {
        close(cfd);
        close(sockfd);
    }

    if (len != NULL) {
        *len = length;
    }

    close(cfd);
    close(sockfd);    
    return AAOS_OK;
}

static void 
cleanup_task_info(void *task)
{
    struct TaskInfo *task_ = (struct TaskInfo *) task;

    free(task_->description);
    free(task_);
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
    free(telescope);
}

static void 
cleanup_site_info(void *site)
{
    struct SiteInfo *site_ = (struct SiteInfo *) site;

    free(site_->name);
    free(site_->description);

    if (site_->rpc != NULL) {
        delete(site_->rpc);
    }
    free(site);
}

static int
Scheduler_database_initialize_site_list(struct __Scheduler *self, MYSQL_RES *res)
{
    MYSQL_ROW row;
    self->site_list = new(ThreadsafeList(), cleanup_site_info);

    while ((row = mysql_fetch_row(res)) != NULL) {

    }

    return AAOS_OK;
}

 static int
 __Scheduler_database_query(struct __Scheduler *self, const char *stmt_str, database_cb_t cb)
 {
    MYSQL *mysql;
    
    if ((mysql = mysql_init(NULL)) == NULL) {
        return AAOS_ENOMEM;
    }

    if ((mysql_real_connect(mysql, self->db_host, self->db_user, self->db_passwd, self->db_name, 0, NULL, 0)) == NULL) {
        mysql_close(mysql);
        return AAOS_ERROR;
    }

    if (mysql_query(mysql, stmt_str) != 0) {
        mysql_close(mysql);
        return AAOS_ERROR;
    }

    if (cb != NULL) {
        MYSQL_RES *res = mysql_store_result(mysql);
        if (res == NULL) {
            if (mysql_errno(mysql) != 0) {
                return AAOS_ERROR;
            } else {
                return AAOS_OK;
            }
        }
        return cb(self, res);
    }

    mysql_close(mysql);
    return AAOS_OK;
 }

int
__scheduler_get_task_by_telescope_id(void *_self, uint64_t identifier, char *result, size_t size, size_t *length, unsigned int *type)
{
    const struct __SchedulerClass *class = (const struct __SchedulerClass *) classOf(_self);
    
    if (isOf(class, __SchedulerClass()) && class->get_task_by_telescope_id.method) {
        return ((int (*)(void *, unsigned int, char *, size_t, size_t *, unsigned  *)) class->get_task_by_telescope_id.method)(_self, identifier, result, size, length, type);
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
    uint32_t length;
    char *json_string;
    int ret;

    if (self->type == SCHEDULER_TYPE_SITE) {
        telescope = threadsafe_list_find_first_if(self->telescope_list, telescope_by_id, identifier);
        if (telescope == NULL) {
            return AAOS_ENOTFOUND;
        }
        if (strcmp(self->ipc_model, "unix_stream") == 0) {
            json_string = __scheduler_create_request_json_string(SCHEDULER_GET_TASK_BY_TELESCOPE_ID, identifier);
            ret = __Scheduler_ipc_write_and_read(self, json_string, result, size, length);
            free(json_string);
            if (type != NULL) {
                *type = SCHEDULER_FORMAT_JSON;
            }
            
            return ret;
        }
    } else {
        return AAOS_ENOTSUP;
    }
}

int
__scheduler_get_task_by_telescope_name(void *_self, const char *name, char *result, size_t size, size_t *length, unsigned int *type)
{
    const struct __SchedulerClass *class = (const struct __SchedulerClass *) classOf(_self);
    
    if (isOf(class, __SchedulerClass()) && class->get_task_by_telescope_name.method) {
        return ((int (*)(void *, const char *, char *, size_t, size_t *, unsigned  *)) class->get_task_by_telescope_id.method)(_self, name, result, size, length, type);
    } else {
        int result;
        forward(_self, &result, (Method) __scheduler_get_task_by_telescope_name, "get_task_by_telescope_name", _self, name, result, size, type);
        return result;
    }
}

static int
__Scheduler_get_task_by_telescope_name(void *_self, unsigned int name, char *result, size_t size, size_t *length, unsigned int *type)
{
    struct __Scheduler *self = cast(__Scheduler(), _self);

    struct TelescopeInfo *telescope;
    uint32_t length;
    char *json_string;
    int ret;

    if (self->type == SCHEDULER_TYPE_SITE) {
        telescope = threadsafe_list_find_first_if(self->telescope_list, telescope_by_name, name);
        if (telescope == NULL) {
            return AAOS_ENOTFOUND;
        }
        if (strcmp(self->ipc_model, "unix_stream") == 0) {
            json_string = __scheduler_create_request_json_string(SCHEDULER_GET_TASK_BY_TELESCOPE_NAME, name);
            ret = __Scheduler_ipc_write_and_read(self, json_string, result, size, length);
            free(json_string);
            if (type != NULL) {
                *type = SCHEDULER_FORMAT_JSON;
            }
            return ret;
        }
    } else {
        return AAOS_ENOTSUP;
    }
}

int
__scheduler_pop_task_block(void *_self)
{
    const struct __SchedulerClass *class = (const struct __SchedulerClass *) classOf(_self);
    
    if (isOf(class, __SchedulerClass()) && class->pop_task_block.method) {
        return (int (*)(void *) class->pop_task_block.method)(_self);
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

    int sockfd, cfd;
    uint32_t length;
    ssize_t nread, nwrite;
    char buf[BUFSIZE], *block_buf;
    struct sockaddr_un addr;
    char *json_string;

    if (self->type != SCHEDULER_TYPE_GLOBAL) {
        return AAOS_ENOTSUP;
    }

    block_buf = (char *) Malloc(BUFSIZE * self->max_task_in_block);
    for (;;) {
        memset(&addr, '\0', sizeof(struct sockaddr_un));
        addr.sun_family  = AF_UNIX;
        snprintf(addr.sun_path, PATHSIZE, "%s", self->socket_file);
        if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
            continue;
        }
        if ((cfd = connect(sockfd, (struct sockadd *)&addr, sizeof(struct sockadd_un))) < 0) {
            close(sockfd);
            continue;
        }
        json_string = __scheduler_create_request_json_string(SCHEDULER_POP_TASK_BLOCK);
        length = strlen(json_string) + 1;
        memcpy(buf, &length, sizeof(uint32_t));
        snprintf(buf + sizeof(uint32_t), BUFSIZE - sizeof(uint32_t), "%s", json_string);
        free(json_string);
        if ((nwrite = Writen(cfd, buf, length + sizeof(uint32_t))) < 0) {
            close(cfd);
            close(sockfd);
            continue;
        }
        for (;;) {
            if ((nread = Readn(cfd, &length, sizeof(uint32_t))) < 0) {
                close(cfd);
                close(sockfd);
                break;
            }
            if ((nread = Readn(cfd, block_buf, length)) < 0) {
                close(cfd);
                close(sockfd);
                break;
            }
            json_string =  __scheduler_create_request_json_string(SCHEDULER_TASK_BLOCK_ACK);
            length = strlen(json_string) + 1;
            memcpy(buf, &length, sizeof(uint32_t));
            snprintf(buf + sizeof(uint32_t), BUFSIZE - sizeof(uint32_t), "%s", json_string);
            free(json_string);
            if ((nrwite = Writen(cfd, buf, length)) < 0) {
                close(cfd);
                close(sockfd);
                break;
            }
            /*
             * Parse block_buf, and push task block to 
             */
        }
        continue;
    }
    free(block_buf);

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
    char *buf;

    if (self->type != SCHEDULER_TYPE_GLOBAL) {
        return AAOS_ENOTSUP;
    }

    if (type == SCHEDULER_FORMAT_JSON) {
        length = strlen(block_buf) + 1; 
        buf = (char *) Malloc(length + sizeof(uint32_t));
        memcpy(buf, &length, sizeof(uint32_t));
        snprintf(buf + sizeof(uint32_t), length, "%s", block_buf);
        ret = __Scheduler_ipc_write(self, block_buf);
        free(buf);
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
    int ret;
    uint32_t length;
    char buf[BUFSIZE];

    
    cJSON *root_json;
    const cJSON *sites_json, *telescopes_json, *targets_json, *tasks_json, *site_json, *telescope_json, *target_json, *task_json;

    if ((root_json = cJSON_Parse(string)) == NULL) {
        return AAOS_EBADCMD;
    }
    length = strlen(string) + 1;
    memcpy(buf, &length, sizeof(uint32_t));
    snprintf(buf + sizeof(uint32_t), length, "%s", string);
    if (self->type == SCHEDULER_TYPE_GLOBAL && (sites_json = cJSON_GetObjectItemCaseSensitive("SITE-INFO", root_json)) != NULL) {
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
                }
            }
        }
    }
    if (self->type != SCHEDULER_TYPE_UNIT && (telescopes_json = cJSON_GetObjectItemCaseSensitive("TELESCOPE-INFO", root_json)) != NULL) {
        struct TelescopeIfo *telescope;
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
                    }
                }
            }
        } else {
            telescope_json = telescopes_json;
            id = cJSON_GetObjectItemCaseSensitive(telescope_json, "site_id");
            status = cJSON_GetObjectItemCaseSensitive(telescope_json, "status");
            if (cJSON_IsNumber(id) && cJSON_IsNumber(status)) {
                telescope = threadsafe_list_find_first_if(self->telescope_list, telescope_by_id, id->valueint);
                if (telescope = NULL) {
                    telescope->status = status->valueint;
                }
            }
        }
    }
    if (self->type != SCHEDULER_TYPE_UNIT && (targets_json = cJSON_GetObjectItemCaseSensitive("TARGET-INFO", root_json)) != NULL) {
        struct TargetInfo *target;
        const cJSON *id, *status;
        if (cJSON_IsArray(targets_json)) {
            cJSON_ArrayForEach(target_json, targets_json) {
                id = cJSON_GetObjectItemCaseSensitive(target_json, "targ_id");
                status = cJSON_GetObjectItemCaseSensitive(target_json, "status");
                if (cJSON_IsNumber(id) && cJSON_IsNumber(status)) {
                    telescope = threadsafe_list_find_first_if(self->target_list, target_by_id, id->valueint);
                    if (telescope != NULL) {
                        /*
                         * TODO, lock for threadsafe update!
                         */
                        telescope->status = status->valueint;
                    }
                }
            }
        } else {
            target_json = targets_json;
            id = cJSON_GetObjectItemCaseSensitive(target_json, "site_id");
            status = cJSON_GetObjectItemCaseSensitive(target_json, "status");
            if (cJSON_IsNumber(id) && cJSON_IsNumber(status)) {
                target = threadsafe_list_find_first_if(self->target_list, target_by_id, id->valueint);
                if (target = NULL) {
                    target->status = status->valueint;
                }
            }
        }
    }
    if (self->type != SCHEDULER_TYPE_UNIT && (tasks_json = cJSON_GetObjectItemCaseSensitive("TASK-INFO", root_json)) != NULL) {
        const cJSON *id, *status;
        if (cJSON_IsArray(targets_json)) {
            cJSON_ArrayForEach(target_json, targets_json) {
                id = cJSON_GetObjectItemCaseSensitive(target_json, "targ_id");
                status = cJSON_GetObjectItemCaseSensitive(target_json, "status");
                if (cJSON_IsNumber(id) && cJSON_IsNumber(status)) {
                    /*
                     * update database here. 
                     */
                }
            }
        } else {
            target_json = targets_json;
            id = cJSON_GetObjectItemCaseSensitive(target_json, "site_id");
            status = cJSON_GetObjectItemCaseSensitive(target_json, "status");
            if (cJSON_IsNumber(id) && cJSON_IsNumber(status)) {
                /*
                 * update database here. 
                 */
            }
        }
    }
    cJSON_Delete(root_json);
   
    return ret;
}

static int
__Scheduler_update_status(void *_self, const char *string, unsigned int type)
{
    struct __Scheduler *self = cast(__Scheduler(), _self);

    if (type == SCHEDULER_FORMAT_JSON) {
        return __Scheduler_update_status_json(self, string)
    } else {
        return AAOS_EFMTNOTSUP;
    }
    
    return ret;
}

int 
__scheduler_list_site(void *_self, char *buf, size_t size, unsigned int *type)
{
    const struct __SchedulerClass *class = (const struct __SchedulerClass *) classOf(_self);
    
    if (isOf(class, __SchedulerClass()) && class->list_site.method) {
        return ((int (*)(void *, char *, size_t, unsigned int *)) class->list_site.method)(_self, buf, size, type);
    } else {
        int result;
        forward(_self, &result, (Method) __scheduler_list_site, "list_site", _self, info, type);
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
        type = SCHEDULER_FORMAT_JSON;
    }
    return AAOS_OK;
}

static int
__Scheduler_list_telescope(void *_self, char *buf, size_t size, unsigned int *type)
{
    struct __Scheduler *self = cast(__Scheduler(), _self);

    FILE *fp;

    if (self->type == SCHEDULER_TYPE_UNIT) {
        return AAOS_ENOTSUP;
    }

    fp = fmemopen(buf, size, "w");
    threadsafe_list_foreach(self->telescope_list, print_telescope_list, fp);
    fclose(fp);

    return AAOS_OK;
}

static int
__Scheduler_list_target(void *_self, char *buf, size_t size, unsigned int *type)
{
    struct __Scheduler *self = cast(__Scheduler(), _self);

    FILE *fp;

    if (self->type != SCHEDULER_TYPE_UNIT) {
        return AAOS_ENOTSUP;
    }

    fp = fmemopen(buf, size, "w");
    threadsafe_list_foreach(self->target_list, print_target_list, fp);
    fclose(fp);

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
    double timestamp, site_lon, site_lat, site_alt;
    struct timespec tp;
    struct SiteInfo *site;
    char *sitename;
    char sql[BUFSIZE];
    
    Clock_gettime(CLOCK_REALTIME, &tp);
    timestamp = tp.tv_sec + tp.tv_nsec / 1000000000.;
    self->max_site_id++;
    site_id = self->max_site_id;

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
        return AAOS_EINVAL;
    }

    __scheduler_create_sql(SCHEDULER_ADD_SITE, 0, self->site_db_table, sql, BUFSIZE, sitename, site_id, site_lon, site_lat, site_alt, timestamp);
    __Scheduler_database_query(self, sql, NULL);

    site = (struct SiteInfo *) Malloc(sizeof(struct SiteInfo));
    memset(site, '\0', sizeof(struct SiteInfo));
    site->name = (char *) Malloc(strlen(sitename) + 1);
    snprintf(site->name, strlen(sitename) + 1, "%s", sitename);
    site->status = 0;

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
        return ((int (*)(void *, uuint64_t)) class->delete_site_by_id.method)(_self, identifier);
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
        forward(_self, &result, (Method) __scheduler_list_telescope, "list_telescope", _self, info, type);
        return result;
    }
}

static int
__Scheduler_list_telescope(void *_self, char *buf, size_t size, unsigned int *type)
{
    struct __Scheduler *self = cast(__Scheduler(), _self);

    FILE *fp;

    if (self->type != SCHEDULER_TYPE_GLOBAL) {
        return AAOS_ENOTSUP;
    }

    fp = fmemopen(buf, size, "w");
    threadsafe_list_foreach(self->site_list, print_telescope_list, fp);
    fclose(fp);

    if (type != NULL) {
        type = SCHEDULER_FORMAT_JSON;
    }
    return AAOS_OK;
}

/*
 * Available for site and global scheduler. 
 * If the scheduler is site scheduler, it will also asked the global scheduler to add a telescope. 
 */
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

static int
__Scheduler_add_telescope_json(struct __Scheduler *self, const char *info)
{
    cJSON *telescope_json, *value_json;
    int ret = AAOS_OK;
    uint64_t site_id, tel_id;
    int status;
    double timestamp;
    struct timespec tp;
    char *description = NULL, *telescop;
    char sql[BUFSIZE];

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
            site_id = site->identifier;
        }
        cJSON_AddNumberToObject(telescope_json, "site_id", site_id);
        tel_id = self->max_telescope_id + 1;
        self->max_telescope_id++;
        tel_id |= (site_id<<16);
        cJSON_AddNumberToObject(telescope_json, "tel_id", tel_id);
    }

    value_json = cJSON_GetObjectItemCaseSensitive(telescope_json, "telescop");
    if (value_json != NULL && cJSON_IsString(value_json)) {
        tel_id = value_json->valuestring;
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
        description = cJSON_Print(telescope_json);
        telescope->description = (char *) Malloc(strlen(description) + 1);
        snprintf(telescope->description, strlen(description) + 1, "%s", description);
        telescope->name = (char *) Malloc(strlen(telescop) + 1);
        snprintf(telescope->name, strlen(telescop) + 1, "%s", telescop);
        __scheduler_create_sql(SCHEDULER_ADD_SITE, 0, self->telescope_db_table, sql, BUFSIZE, telescop, tel_id, site_id, description, timestamp);
        free(description);
    } else {
        __scheduler_create_sql(SCHEDULER_ADD_SITE, 0, self->telescope_db_table, sql, BUFSIZE, telescop, tel_id, site_id, info, timestamp);
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
    
    if (self->type != SCHEDULER_TYPE_GLOBAL) {
        return AAOS_ENOTSUP;
    }

    telescope = threadsafe_list_find_first_if(self->telescope_list, telescope_by_name, name);
    if (telescope == NULL) {
        return AAOS_ENOTFOUND;
    }
    identifier = telescope->identifier;

    return __Scheduler_delete_telescope_by_id(self, identifier);
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

    if (self->type != SCHEDULER_TYPE_GLOBAL) {
        return AAOS_ENOTSUP;
    }

    json_string = __scheduler_create_request_json_string(SCHEDULER_MASK_TARGET_BY_ID, identifier);
    ret = __Scheduler_update_status(self, json_string, SCHEDULER_FORMAT_JSON);
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
    unsigned int identifier;
    
    if (self->type != SCHEDULER_TYPE_GLOBAL) {
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

static int
__Scheduler_unmask_telescope_by_id(void *_self, uint64_t identifier)
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
    
    if (isOf(class, __SchedulerClass()) && class->list_telescope.method) {
        return ((int (*)(void *, char *, size_t, unsigned int *)) class->list_target.method)(_self, buf, size, type);
    } else {
        int result;
        forward(_self, &result, (Method) __scheduler_list_target, "list_target", _self, info, type);
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
    threadsafe_list_foreach(self->site_list, print_target_list, fp);
    fclose(fp);

    if (type != NULL) {
        type = SCHEDULER_FORMAT_JSON;
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
    double ra, dec, timestamp, theta, phi;
    struct timespec tp;
    char *description = NULL, *targname = NULL;
    char sql[BUFSIZE];
    long hp_nside, hp_iring;
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
        name = value_json->valuestring;
    }

    if (ra < 0. || ra > 360. || dec > 90. || dec < -90.) {
        ret = AAOS_EINVAL;
        goto end;
    }

    hp_nside = nside;
    theta = ra * PI / 180.;
    phi = (90. - dec) * PI / 180.;
    ang2pix_ring(hp_nside, theta, phi, &hp_iring);
    targ_id = hp_iring;

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
    cJSON_Delete(telescope_json);

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
        return __Scheduler_add_telescope_json(self, info);
    } else {
        return AAOS_EFMTNOTSUP;
    }
}


int
__scheduler_delete_target_by_id(void *_self, unint64_t identifier, uint32_t nside)
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
__Scheduler_delete_target_by_id(void *_self, unint64_t identifier, uint32_t nside)
{
    struct __Scheduler *self = cast(__Scheduler(), _self);

    int ret = AAOS_OK;

    if (self->type == SCHEDULER_TYPE_UNIT) {
        return AAOS_ENOTSUP;
    }
    /*
     * update 
     */

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
    unsigned int identifier;
    
    if (self->type != SCHEDULER_TYPE_GLOBAL) {
        return AAOS_ENOTSUP;
    }

    target = threadsafe_list_find_first_if(self->target_list, target_by_name, name);
    if (target == NULL) {
        return AAOS_ENOTFOUND;
    }
    identifier = target->identifier;

    return __Scheduler_delete_target_by_id(self, identifier);
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
__Scheduler_mask_target_by_id(void *_self, uint64_t identifier, unint32_t nside)
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
    uint64_t identifier;
    uint32_t nside;
    
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
        forward(_self, &result, (Method) __Scheduler_add_task_record, "add_task_record", _self, status, info, type);
        return result;
    }
}

static int 
__Scheduler_add_task_record_json(struct __Scheduler *self, int status, const char *info)
{
    cJSON *root_json, *site_json, *telescope_json, *target_json, *value_json, *value2_json;
    int ret = AAOS_OK;
    uint64_t site_id, tel_id, targ_id, task_id;
    uint32_t nside;
    float timestamp;
    
    char sql[BUFSIZE];
    
    Clock_gettime(CLOCK_REALTIME, &tp);
    timestamp = tp.tv_sec + tp.tv_nsec / 1000000000.;

    root_json = cJSON_Parse(info);

    if (root_json != NULL) {
        if ((target_json = cJSON_GetObjectItemCaseSensitive(root_json, "TARGET-INFO")) != NULL && (value_json = cJSON_GetObjectItemCaseSensitive(target_json, "targ_id")) != NULL && cJSON_IsNumber(value_json) && (value2_json = cJSON_GetObjectItemCaseSensitive(target_json, "nside")) != NULL && cJSON_IsNumber(value2_json)) {
            targ_id = value_json->valueint;
            nside = value2_json->valueint;
        } else {
            ret = AAOS_EBADCMD;
            goto end;
        }
        if ((telescope_json = cJSON_GetObjectItemCaseSensitive(root_json, "TELESCOPE-INFO")) != NULL && (value_json = cJSON_GetObjectItemCaseSensitive(telescope_json, "tel_id")) != NULL && cJSON_IsNumber(value_json)) {
            tel_id = value_json->valueint;
        } else {
            ret = AAOS_EBADCMD;
            goto end;
        }
        if ((site_json = cJSON_GetObjectItemCaseSensitive(root_json, "SITE-INFO")) != NULL && (value_json = cJSON_GetObjectItemCaseSensitive(site_json, "site_id")) != NULL && cJSON_IsNumber(value_json)) {
            site_id = value_json->valueint;
        } else {
            ret = AAOS_EBADCMD;
            goto end;
        }
    } else {
        return AAOS_EINVAL;
    }

    __scheduler_create_sql(SCHEDULER_ADD_TASK_RECORD, 0, self->site_db_table, sql, BUFSIZE, task_id, targ_id, nside, tel_id, site_id, info,  timestamp);
    __Scheduler_database_query(self, sql, NULL);


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

int
__scheduler_update_task_record(void *_self, uint64_t identifier, const char *info)
{
    const struct __SchedulerClass *class = (const struct __SchedulerClass *) classOf(_self);
    
    if (isOf(class, __SchedulerClass()) && class->update_task_record.method) {
        return ((int (*)(void *, uint64_t, const char *)) class->add_task_record.method)(_self, identifier, info);
    } else {
        int result;
        forward(_self, &result, (Method) __Scheduler_add_task_record, "add_task_record", _self, identifier, info);
        return result;
    }
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

    if (self->type == SCHEDULER_TYPE_GLOBAL) {
        snprintf(sql, BUFSIZE, "SELECT sitename,site_id,status,site_lon,site_lat,site_alt FROM %s", self->site_db_table);
        mysql_query(mysql, sql);
    }

    return AAOS_OK;
}

static void *
__Scheduler_ctor(void *_self, va_list *app)
{
    struct __Scheduler *self = super_ctor(__Scheduler(), _self, app);
    
    const char *s, *key, *value;

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
    }

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

                    //__scheduler_add_task_record, "add_task_record", __Scheduler_add_task_record,
                    //__scheduler_update_task_record, "update_task_record", __Scheduler_update_task_record,

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
