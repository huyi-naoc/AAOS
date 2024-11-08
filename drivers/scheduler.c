//
//  scheduler.c
//  AAOS
//
//  Created by huyi on 2024/07/18.
//  Copyright © 2024 National Astronomical Observatories, Chinese Academy of Sciences. All rights reserved.
//

#include "adt.h"
#include "def.h"
#include "scheduler.h"
#include "scheduler_r.h"
#include "wrapper.h"

#include <cjson/cJSON.h>
#include <mysql.h>

struct SchedulerInfo {
    char *addr;
    char *port;
    char *name;
    void *scheduler;
};

struct TaskInfo {
    uint64_t identifier;
    uint64_t nside;
    uint64_t targ_id;
    int status;
    char *description; /* JSON */
};

struct TelescopeInfo {
    uint64_t identifier;
    uint64_t site_id;
    int status;
    char *name;
    char *description; /* JSON */
    void *rpc;
};

struct TargetInfo {
    uint32_t nside;
    uint64_t identifier;
    double ra;
    double dec;
    int status;
    char *name;
    char *description; /* JSON */
    void *rpc;
};

struct SiteInfo {
    uint64_t identifier;
    int status;
    char *name;
    char *description; /* JSON */
    void *rpc;
};

typedef int (*database_cb_t)(struct __Scheduler *, MYSQL_RES *);

/*
 * Scheduler Class.
 */

static void 
_scheduler_create_sql(int command, uint64_t identifier, const char *table, char *sql, size_t size, ...)
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
        int status = SCHEDULER_STATUS_OK, priority = 0;
        double ra, dec;

        name = va_arg(ap, const char *);
        targ_id = va_arg(ap, uint64_t);
        nside = va_arg(ap, uint32_t);
        ra = va_arg(ap, double);
        dec = va_arg(ap, double);
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
        double obstime;
        task_id = va_arg(ap, uint64_t);
        targ_id = va_arg(ap, uint64_t);
        nside = va_arg(ap, uint32_t);
        tel_id = va_arg(ap, uint64_t);
        site_id = va_arg(ap, uint64_t);
        status = va_arg(ap, int);
        description = va_arg(ap, const char *);
        obstime = va_arg(ap, double);
        timestamp = va_arg(ap, double);
        snprintf(sql, size, "INSERT INTO %s (task_id, targ_id, nside, tel_id, site_id, status, task_des, obstime, timestamp) VALUES (%ld, %ld, %u, %ld, %ld, %d, \"%s\", %lf, %lf)", table, task_id, targ_id, nside, tel_id, site_id, status, description, obstime, timestamp);
    } else if (command == SCHEDULER_UPDATE_TASK_STATUS) {
        int status = va_arg(ap, int);
        timestamp = va_arg(ap, double);
        snprintf(sql, size, "UPDATE %s SET status=%d, timestamp=%lf WHERE task_id=%ld", table, status, timestamp, identifier); 
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

static char *
__Scheduler_create_request_json_string(unsigned int command, ...)
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
 Scheduler_database_query(struct __Scheduler *self, const char *stmt_str, database_cb_t cb)
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

    return AAOS_OK;
 }


int
__scheduler_get_task_by_telescope_id(void *_self, unsigned  identifier, char *result, size_t size, size_t *length, unsigned int *type)
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
__Scheduler_get_task_by_telescope_id(void *_self, unsigned int identifier , char *result, size_t size, size_t *length, unsigned int *type)
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
            json_string = __Scheduler_create_request_json_string(SCHEDULER_GET_TASK_BY_TELESCOPE_ID, identifier);
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
            json_string = __Scheduler_create_request_json_string(SCHEDULER_GET_TASK_BY_TELESCOPE_NAME, name);
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
        json_string = __Scheduler_create_request_json_string(SCHEDULER_POP_TASK_BLOCK);
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
            json_string =  __Scheduler_create_request_json_string(SCHEDULER_TASK_BLOCK_ACK);
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
__Scheduler_update_status(void *_self, const char *string, unsigned int type)
{
    struct __Scheduler *self = cast(__Scheduler(), _self);

    int ret;
    uint32_t length;
    char buf[BUFSIZE];

    if (type == SCHEDULER_FORMAT_JSON) {
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
    } else {
        return AAOS_ENOTSUP;
    }
    
    return ret;
}



int
__scheduler_delete_site_by_id(void *_self, unsigned int identifier)
{
    const struct __SchedulerClass *class = (const struct __SchedulerClass *) classOf(_self);
    
    if (isOf(class, __SchedulerClass()) && class->delete_site_by_id.method) {
        return ((int (*)(void *, unsigned int)) class->delete_site_by_id.method)(_self, identifier);
    } else {
        int result;
        forward(_self, &result, (Method) __scheduler_delete_site_by_id, "delete_site_by_id", _self, identifier);
        return result;
    }
}

static int
__Scheduler_delete_site_by_id(void *_self, unsigned int identifier)
{
    struct __Scheduler *self = cast(__Scheduler(), _self);

    char *json_string;
    int ret;

    if (self->type != SCHEDULER_TYPE_GLOBAL) {
        return AAOS_ENOTSUP;
    }

    json_string = __Scheduler_create_request_json_string(SCHEDULER_DELETE_SITE_BY_ID, identifier);
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
    unsigned int identifier;
    
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
__scheduler_mask_site_by_id(void *_self, unsigned int identifier)
{
    const struct __SchedulerClass *class = (const struct __SchedulerClass *) classOf(_self);
    
    if (isOf(class, __SchedulerClass()) && class->mask_site_by_id.method) {
        return ((int (*)(void *, unsigned int)) class->mask_site_by_id.method)(_self, identifier);
    } else {
        int result;
        forward(_self, &result, (Method) __scheduler_mask_site_by_id, "mask_site_by_id", _self, identifier);
        return result;
    }
}

static int
__Scheduler_mask_site_by_id(void *_self, unsigned int identifier)
{
    struct __Scheduler *self = cast(__Scheduler(), _self);

    char *json_string;
    int ret;

    if (self->type != SCHEDULER_TYPE_GLOBAL) {
        return AAOS_ENOTSUP;
    }

    json_string = __Scheduler_create_request_json_string(SCHEDULER_MASK_SITE_BY_ID, identifier);
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
    unsigned int identifier;
    
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
__scheduler_unmask_site_by_id(void *_self, unsigned int identifier)
{
    const struct __SchedulerClass *class = (const struct __SchedulerClass *) classOf(_self);
    
    if (isOf(class, __SchedulerClass()) && class->unmask_site_by_id.method) {
        return ((int (*)(void *, unsigned int)) class->unmask_site_by_id.method)(_self, identifier);
    } else {
        int result;
        forward(_self, &result, (Method) __scheduler_unmask_site_by_id, "unmask_site_by_id", _self, identifier);
        return result;
    }
}

static int
__Scheduler_unmask_site_by_id(void *_self, unsigned int identifier)
{
    struct __Scheduler *self = cast(__Scheduler(), _self);

    char *json_string;
    int ret;

    if (self->type != SCHEDULER_TYPE_GLOBAL) {
        return AAOS_ENOTSUP;
    }

    json_string = __Scheduler_create_request_json_string(SCHEDULER_UNMASK_SITE_BY_ID, identifier);
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
    unsigned int identifier;
    
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
__scheduler_add_telescope(void *_self, const char *info)
{
    const struct __SchedulerClass *class = (const struct __SchedulerClass *) classOf(_self);
    
    if (isOf(class, __SchedulerClass()) && class->add_telescope.method) {
        return ((int (*)(void *, const char *)) class->add_telescope.method)(_self, info);
    } else {
        int result;
        forward(_self, &result, (Method) __scheduler_add_telescope, "add_telescope", _self, info);
        return result;
    }
}

static int
__Scheduler_add_telescope(void *_self, const char *info)
{
    struct __Scheduler *self = cast(__Scheduler(), _self);
    cJSON *telescope_json, *value_json;
    int ret = AAOS_OK;
    uint64_t site_id, tel_id, status;
    double timestamp;
    struct timespec tp;

    if (self->type == SCHEDULER_TYPE_UNIT) {
        return AAOS_ENOTSUP;
    }

    Clock_gettime(CLOCK_REALTIME, &tp);
    timestamp = tp.tv_sec + tp.tv_nsec / 1000000000.;

    telescope_json = cJSON_Parse(info);

    if (self->type == SCHEDULER_TYPE_SITE) {
        void *site;
        value_json = cJSON_GetObjectItemCaseSensitive(telescope_json, "site_id");
        if (value_json != NULL && cJSON_IsString(value_json)) {
            site_id = value_json->valueint;
        } else {
            ret = AAOS_EBADCMD;
            goto end;
        }
        value_json = cJSON_GetObjectItemCaseSensitive(telescope_json, "tel_id");
        if (value_json != NULL && cJSON_IsString(value_json)) {
            tel_id = value_json->valueint;
        } else {
            ret = AAOS_EBADCMD;
            goto end;
        }
        status = 0;
        if ((site = threadsafe_list_find_first_if(self->site_list, site_by_id, site_id)) == NULL) {
            ret = AAOS_ENOTFOUND;
            goto end;
        }
    } else {
        struct SiteInfo *site = self->site;
        value_json = cJSON_GetObjectItemCaseSensitive(telescope_json, "site_id");
        if (value_json != NULL && cJSON_IsString(value_json)) {
            site_id = value_json->valueint;
        } else {
            site_id = site->identifier;
            cJSON_AddNumberToObject(telescope_json, "site_id", site_id);
        }
        tel_id = self->max_telescope_id + 1;
        self->max_telescope_id++;
        tel_id &= (site_id<<16);
        cJSON_AddNumberToObject(telescope_json, "tel_id", tel_id);
        status = 0;
    }

    value_json = cJSON_GetObjectItemCaseSensitive(telescope_json, "telescop");
    if (value_json != NULL && cJSON_IsString(value_json)) {
        telescope->name = (char *) Malloc(strlen(value_json->valuestring) + 1);
        snprintf(telescope->name, strlen(value_json->valuestring) + 1, "%s", value_json->valuestring);
    }

    value_json = cJSON_GetObjectItemCaseSensitive(telescope_json, "site_id");
    if (value_json != NULL && cJSON_IsString(value_json)) {
        uint64_t site_id = value_json->valueint;
        tel_id = self->max_telescope_id + 1;
        self->max_telescope_id++;
        tel_id &= (site_id<<16);
    } else {
        if (self->type == SCHEDULER_TYPE_GLOBAL) {
            free(telescope);
            return AAOS_EBADCMD;
        } else {
            tel_id = self->max_telescope_id + 1;
            self->max_telescope_id++;
        }
    }
    cJSON_AddNumberToObject(telescope_json, "tel_id", tel_id);

    if (self->type == SCHEDULER_TYPE_SITE) {
        threadsafe_list_push_front(self->telescope_list, telescope);
    } else {
        free(telescope);
    }
end: 
    cJSON_Delete(telescope_json);
    return ret;
}

int
__scheduler_delete_telescope_by_id(void *_self, unsigned int identifier)
{
    const struct __SchedulerClass *class = (const struct __SchedulerClass *) classOf(_self);
    
    if (isOf(class, __SchedulerClass()) && class->delete_telescope_by_id.method) {
        return ((int (*)(void *, unsigned int)) class->delete_telescope_by_id.method)(_self, identifier);
    } else {
        int result;
        forward(_self, &result, (Method) __scheduler_delete_telescope_by_id, "delete_telescope_by_id", _self, identifier);
        return result;
    }
}

static int
__Scheduler_delete_telescope_by_id(void *_self, unsigned int identifier)
{
    struct __Scheduler *self = cast(__Scheduler(), _self);

    char *json_string;
    int ret;

    if (self->type == SCHEDULER_TYPE_UNIT) {
        return AAOS_ENOTSUP;
    }

    json_string = __Scheduler_create_request_json_string(SCHEDULER_DELETE_TARGET_BY_ID, identifier);
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

    struct SiteInfo *telescope;
    unsigned int identifier;
    
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
__scheduler_mask_telescope_by_id(void *_self, unsigned int identifier)
{
    const struct __SchedulerClass *class = (const struct __SchedulerClass *) classOf(_self);
    
    if (isOf(class, __SchedulerClass()) && class->mask_telescope_by_id.method) {
        return ((int (*)(void *, unsigned int)) class->mask_telescope_by_id.method)(_self, identifier);
    } else {
        int result;
        forward(_self, &result, (Method) __scheduler_mask_telescope_by_id, "mask_telescope_by_id", _self, identifier);
        return result;
    }
}

static int
__Scheduler_mask_telescope_by_id(void *_self, unsigned int identifier)
{
    struct __Scheduler *self = cast(__Scheduler(), _self);

    char *json_string;
    int ret;

    if (self->type != SCHEDULER_TYPE_GLOBAL) {
        return AAOS_ENOTSUP;
    }

    json_string = __Scheduler_create_request_json_string(SCHEDULER_MASK_TARGET_BY_ID, identifier);
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

    struct SiteInfo *telescope;
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
__scheduler_unmask_telescope_by_id(void *_self, unsigned int identifier)
{
    const struct __SchedulerClass *class = (const struct __SchedulerClass *) classOf(_self);
    
    if (isOf(class, __SchedulerClass()) && class->unmask_telescope_by_id.method) {
        return ((int (*)(void *, unsigned int)) class->unmask_telescope_by_id.method)(_self, identifier);
    } else {
        int result;
        forward(_self, &result, (Method) __scheduler_unmask_telescope_by_id, "unmask_telescope_by_id", _self, identifier);
        return result;
    }
}

static int
__Scheduler_unmask_telescope_by_id(void *_self, unsigned int identifier)
{
    struct __Scheduler *self = cast(__Scheduler(), _self);

    char *json_string;
    int ret;

    if (self->type != SCHEDULER_TYPE_GLOBAL) {
        return AAOS_ENOTSUP;
    }

    json_string = __Scheduler_create_request_json_string(SCHEDULER_UNMASK_TARGET_BY_ID, identifier);
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

    struct SiteInfo *telescope;
    unsigned int identifier;
    
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

    struct SiteInfo *target;
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
__scheduler_mask_target_by_id(void *_self, unsigned int identifier)
{
    const struct __SchedulerClass *class = (const struct __SchedulerClass *) classOf(_self);
    
    if (isOf(class, __SchedulerClass()) && class->mask_target_by_id.method) {
        return ((int (*)(void *, unsigned int)) class->mask_target_by_id.method)(_self, identifier);
    } else {
        int result;
        forward(_self, &result, (Method) __scheduler_mask_target_by_id, "mask_target_by_id", _self, identifier);
        return result;
    }
}

static int
__Scheduler_mask_target_by_id(void *_self, unsigned int identifier)
{
    struct __Scheduler *self = cast(__Scheduler(), _self);

    char *json_string;
    int ret;

    if (self->type != SCHEDULER_TYPE_GLOBAL) {
        return AAOS_ENOTSUP;
    }

    json_string = __Scheduler_create_request_json_string(SCHEDULER_MASK_TARGET_BY_ID, identifier);
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

    struct SiteInfo *target;
    unsigned int identifier;
    
    if (self->type != SCHEDULER_TYPE_GLOBAL) {
        return AAOS_ENOTSUP;
    }

    target = threadsafe_list_find_first_if(self->target_list, target_by_name, name);
    if (target == NULL) {
        return AAOS_ENOTFOUND;
    }
    identifier = target->identifier;

    return __Scheduler_mask_target_by_id(self, identifier);
}

int
__scheduler_unmask_target_by_id(void *_self, unsigned int identifier)
{
    const struct __SchedulerClass *class = (const struct __SchedulerClass *) classOf(_self);
    
    if (isOf(class, __SchedulerClass()) && class->unmask_target_by_id.method) {
        return ((int (*)(void *, unsigned int)) class->unmask_target_by_id.method)(_self, identifier);
    } else {
        int result;
        forward(_self, &result, (Method) __scheduler_unmask_target_by_id, "unmask_target_by_id", _self, identifier);
        return result;
    }
}

static int
__Scheduler_unmask_target_by_id(void *_self, unsigned int identifier)
{
    struct __Scheduler *self = cast(__Scheduler(), _self);

    char *json_string;
    int ret;

    if (self->type != SCHEDULER_TYPE_GLOBAL) {
        return AAOS_ENOTSUP;
    }

    json_string = __Scheduler_create_request_json_string(SCHEDULER_UNMASK_TARGET_BY_ID, identifier);
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

    struct SiteInfo *target;
    unsigned int identifier;
    
    if (self->type != SCHEDULER_TYPE_GLOBAL) {
        return AAOS_ENOTSUP;
    }

    target = threadsafe_list_find_first_if(self->target_list, target_by_name, name);
    if (target == NULL) {
        return AAOS_ENOTFOUND;
    }
    identifier = target->identifier;

    return __Scheduler_unmask_target_by_id(self, identifier);
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

    return AAOS_OK;
}

/*
 * Compiler-dependant initializer.
 */
#ifdef _USE_COMPILER_ATTRIBUTION_
static void __destructor__(void) __attribute__ ((destructor(_SERIAL_RPC_PRIORITY_)));

static void
__destructor__(void)
{
    scheduler_virtual_table_initialize();
    SchedulerClass_initialize();
    Scheduler_initialize();
    scheduler_server_virtual_table_initialize();
    SchedulerServerClass_initialize();
    SchedulerServer_initialize();
    scheduler_client_virtual_table_initialize();
    SchedulerClientClass_initialize();
    SchedulerClient_initialize();
}

static void __constructor__(void) __attribute__ ((constructor(_SERIAL_RPC_PRIORITY_)));

static void
__constructor__(void)
{
    SchedulerClient_destroy();
    SchedulerClientClass_destroy();
    scheduler_client_virtual_table_destroy();
    SchedulerServer_destroy();
    SchedulerServerClass_destroy();
    scheduler_server_virtual_table_destroy();
    Scheduler_destroy();
    SchedulerClass_destroy();
    scheduler_virtual_table_destroy();
}
#endif
