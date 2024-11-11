//
//  scheduler_rpc.c
//  AAOS
//
//  Created by Hu Yi on 2021/11/26.
//  Copyright © 2021 NAOC. All rights reserved.
//

#include "def.h"
#include "protocol.h"
#include "rpc.h"
#include "scheduler_def.h"
#include "scheduler_rpc.h"
#include "scheduler_rpc_r.h"
#include "scheduler.h"

#include "wrapper.h"


int 
scheduler_update_status(void *_self, const char *info, unsigned int type)
{
    const struct SchedulerClass *class = (const struct SchedulerClass *) classOf(_self);
    
    if (isOf(class, SchedulerClass()) && class->update_status.method) {
        return ((int (*)(void *, const char *, unsigned int)) class->add_task_record.method)(_self, info, type);
    } else {
        int result;
        forward(_self, &result, (Method) scheduler_update_status, "update_status", _self, info, type);
        return result;
    }
}

static int
Scheduler_update_status(void *_self, const char *info, unsigned int type)
{
    struct Scheduler *self = cast(Scheduler(), _self);
   
    uint16_t errorcode, option = type;
    char *buf;
    int ret = AAOS_OK;

    protobuf_set(self, PACKET_COMMAND, SCHEDULER_UPDATE_STATUS);
    protobuf_set(self, PACKET_OPTION, option);    
    protobuf_get(self, PACKET_BUF, &buf, NULL);
    if (info != buf) {
        protobuf_set(self, PACKET_BUF, info, strlen(info) + 1);
    }
    
    if ((ret = rpc_call(self)) == AAOS_OK) {
        protobuf_get(self, PACKET_ERRORCODE, &errorcode);
        if (errorcode != AAOS_OK) {
            return errorcode;
        }
    } else {
        return -1 * ret; /*Networking error.*/
    }

    return ret;
}


int
scheduler_get_task_by_telescope_id(void *_self, uint64_t identifier, char *result, size_t size, size_t *length, unsigned int *type)
{
    const struct SchedulerClass *class = (const struct SchedulerClass *) classOf(_self);
    
    if (isOf(class, SchedulerClass()) && class->get_task_by_telescope_id.method) {
        return ((int (*)(void *, uint64_t, char *, size_t, size_t *, unsigned int *)) class->get_task_by_telescope_id.method)(_self, identifier, result, size, length, type);
    } else {
        int result;
        forward(_self, &result, (Method) scheduler_get_task_by_telescope_id, "get_task_by_telescope_id", _self, identifier, result, size, length, type);
        return result;
    }
}

static int
Scheduler_get_task_by_telescope_id(void *_self, uint64_t identifier , char *result, size_t size, size_t *length, unsigned int *type)
{
    struct Scheduler *self = cast(Scheduler(), _self);
    char *res = NULL;
    
    uint32_t len;
    uint16_t errorcode, option;
    int ret = AAOS_OK;

    protobuf_set(self, PACKET_COMMAND, SCHEDULER_GET_TASK_BY_TELESCOPE_ID);
    protobuf_set(self, PACKET_U64F0, identifier);
    
    if ((ret = rpc_call(self)) == AAOS_OK) {
        protobuf_get(self, PACKET_ERRORCODE, &errorcode);
        if (errorcode != AAOS_OK) {
            return errorcode;
        }
        if (type != NULL) {
            protobuf_get(self, PACKET_OPTION, &option);
            *type = option;
        }
        protobuf_get(self, PACKET_BUF, &res, &len);
        if (result != res) {
            snprintf(result, size, "%s", res);
        }
        if (length != NULL) {
            *length = len;
        }
    } else {
        return -1 * ret; /*Networking error.*/
    }

    return ret;
}

int
scheduler_get_task_by_telescope_name(void *_self, const char *name, char *result, size_t size, size_t *length, unsigned int *type)
{
    const struct SchedulerClass *class = (const struct SchedulerClass *) classOf(_self);
    
    if (isOf(class, SchedulerClass()) && class->get_task_by_telescope_name.method) {
        return ((int (*)(void *, const char *, char *, size_t, size_t *, unsigned int *)) class->get_task_by_telescope_name.method)(_self, name, result, size, length, type);
    } else {
        int result;
        forward(_self, &result, (Method) scheduler_get_task_by_telescope_name, "get_task_by_telescope_name", _self, name, result, size, length, type);
        return result;
    }
}

static int
Scheduler_get_task_by_telescope_name(void *_self, const char *name , char *result, size_t size, size_t *length, unsigned int *type)
{
    struct Scheduler *self = cast(Scheduler(), _self);
  
    uint32_t len;
    uint16_t errorcode, option;
    char *res = NULL;
    int ret = AAOS_OK;


    protobuf_set(self, PACKET_COMMAND, SCHEDULER_GET_TASK_BY_TELESCOPE_NAME);

    if (strlen(name) < PACKETPARAMETERSIZE - 1) {
        protobuf_set(self, PACKET_STR, name);
        protobuf_set(self, PACKET_LENGTH, 0);
    } else {
        protobuf_set(self, PACKET_BUF, name, strlen(name) + 1);
        protobuf_set(self, PACKET_LENGTH, strlen(name) + 1);
    }

    if ((ret = rpc_call(self)) == AAOS_OK) {
        protobuf_get(self, PACKET_ERRORCODE, &errorcode);
        if (errorcode != AAOS_OK) {
            return errorcode;
        }
        if (type != NULL) {
            protobuf_get(self, PACKET_OPTION, &option);
            *type = option;
        }
        protobuf_get(self, PACKET_BUF, &res, &len);
        if (result != res) {
            snprintf(result, size, "%s", res);
        }
        if (length != NULL) {
            *length = len;
        }
    } else {
        return -1 * ret; /*Networking error.*/
    }

    return ret;
}

int
scheduler_pop_task_block(void *_self, const char *block_buf, unsigned int type)
{
    const struct SchedulerClass *class = (const struct SchedulerClass *) classOf(_self);

    int result;
    
    if (isOf(class, SchedulerClass()) && class->add_site.method) {
        result = ((int (*)(void *, const char *, unsigned int)) class->add_site.method)(_self, block_buf, type);
    } else {
        forward(_self, &result, (Method) scheduler_add_site, "add_site", _self, block_buf, type);
        
    }
    return result;
}

static int
Scheduler_pop_task_block(void *_self, const char *block_buf, unsigned int type)
{
    struct Scheduler *self = cast(Scheduler(), _self);

    uint16_t option = type, errorcode;
    char *buf;
    int ret = AAOS_OK;

    protobuf_set(self, PACKET_COMMAND, SCHEDULER_POP_TASK_BLOCK);
    protobuf_set(self, PACKET_OPTION, option);
    protobuf_get(self, PACKET_BUF, &buf, NULL);
    if (block_buf != buf) {
        protobuf_set(self, PACKET_BUF, block_buf, strlen(block_buf) + 1);
    }

    if ((ret = rpc_call(self)) == AAOS_OK) {
        protobuf_get(self, PACKET_ERRORCODE, &errorcode);
        if (errorcode != AAOS_OK) {
            return errorcode;
        }
    } else {
        return -1 * ret; /*Networking error.*/
    }

    return ret;
}

int
scheduler_list_site(void *_self, char *result, size_t size, size_t *length, unsigned int *type)
{
    const struct SchedulerClass *class = (const struct SchedulerClass *) classOf(_self);
    
    if (isOf(class, SchedulerClass()) && class->list_site.method) {
        return ((int (*)(void *, char *, size_t, size_t *, unsigned int *)) class->list_site.method)(_self, result, size, length, type);
    } else {
        int result;
        forward(_self, &result, (Method) scheduler_list_site, "list_site", _self, result, size, length, type);
        return result;
    }
}

static int
Scheduler_list_site(void *_self, char *result, size_t size, size_t *length, unsigned int *type)
{
    struct Scheduler *self = cast(Scheduler(), _self);
    char *res = NULL;
    uint32_t len;
    uint16_t errorcode, option;

    int ret = AAOS_OK;

    protobuf_set(self, PACKET_COMMAND, SCHEDULER_LIST_SITE);
    protobuf_set(self, PACKET_LENGTH, 0);

    if ((ret = rpc_call(self)) == AAOS_OK) {
        protobuf_get(self, PACKET_ERRORCODE, &errorcode);
        if (errorcode != AAOS_OK) {
            return errorcode;
        }
        if (type != NULL) {
            protobuf_get(self, PACKET_OPTION, &option);
            *type = option;
        }
        protobuf_get(self, PACKET_BUF, &res, &len);
        if (result != res) {
            snprintf(result, size, "%s", res);
        }
        if (length != NULL) {
            *length = strlen(result);
        }
    } else {
        return -1 * ret; /*Networking error.*/
    }

    return ret;
}

int
scheduler_add_site(void *_self, const char *info, unsigned int type)
{
    const struct SchedulerClass *class = (const struct SchedulerClass *) classOf(_self);

    int result;
    
    if (isOf(class, SchedulerClass()) && class->add_site.method) {
        result = ((int (*)(void *, const char *, unsigned int)) class->add_site.method)(_self, info, type);
    } else {
        forward(_self, &result, (Method) scheduler_add_site, "add_site", _self, info, type);
        
    }
    return result;
}

static int
Scheduler_add_site(void *_self, const char *info, unsigned int type)
{
    struct Scheduler *self = cast(Scheduler(), _self);

    uint16_t option = type, errorcode;
    char *buf;
    int ret = AAOS_OK;

    protobuf_set(self, PACKET_COMMAND, SCHEDULER_ADD_SITE);
    protobuf_set(self, PACKET_OPTION, option);
    protobuf_get(self, PACKET_BUF, &buf, NULL);
    if (info != buf) {
        protobuf_set(self, PACKET_BUF, info, strlen(info) + 1);
    }

    if ((ret = rpc_call(self)) == AAOS_OK) {
        protobuf_get(self, PACKET_ERRORCODE, &errorcode);
        if (errorcode != AAOS_OK) {
            return errorcode;
        }
    } else {
        return -1 * ret; /*Networking error.*/
    }

    return ret;
}

int
scheduler_delete_site_by_id(void *_self, uint64_t identifier)
{
    const struct SchedulerClass *class = (const struct SchedulerClass *) classOf(_self);
    
    if (isOf(class, SchedulerClass()) && class->delete_site_by_id.method) {
        return ((int (*)(void *, uint64_t)) class->delete_site_by_id.method)(_self, identifier);
    } else {
        int result;
        forward(_self, &result, (Method) scheduler_delete_site_by_id, "delete_site_by_id", _self, identifier);
        return result;
    }
}

int
Scheduler_delete_site_by_id(void *_self, uint64_t identifier)
{
    struct Scheduler *self = cast(Scheduler(), _self);
   
    uint16_t errorcode;
    
    int ret = AAOS_OK;

    protobuf_set(self, PACKET_COMMAND, SCHEDULER_DELETE_SITE_BY_ID);
    protobuf_set(self, PACKET_U64F0, identifier);
    protobuf_set(self, PACKET_LENGTH, 0);

    if ((ret = rpc_call(self)) == AAOS_OK) {
        protobuf_get(self, PACKET_ERRORCODE, &errorcode);
        if (errorcode != AAOS_OK) {
            return errorcode;
        }
    } else {
        return -1 * ret; /*Networking error.*/
    }

    return ret;
}

int
scheduler_delete_site_by_name(void *_self, const char *name)
{
    const struct SchedulerClass *class = (const struct SchedulerClass *) classOf(_self);
    
    if (isOf(class, SchedulerClass()) && class->delete_site_by_name.method) {
        return ((int (*)(void *, const char *)) class->delete_site_by_name.method)(_self, name);
    } else {
        int result;
        forward(_self, &result, (Method) scheduler_delete_site_by_name, "delete_site_by_name", _self, name);
        return result;
    }
}

int
Scheduler_delete_site_by_name(void *_self, const char *name)
{
    struct Scheduler *self = cast(Scheduler(), _self);
   
    uint16_t errorcode;
    
    int ret = AAOS_OK;

    protobuf_set(self, PACKET_COMMAND, SCHEDULER_DELETE_SITE_BY_NAME);
    if (strlen(name) < PACKETPARAMETERSIZE - 1) {
        protobuf_set(self, PACKET_STR, name);
        protobuf_set(self, PACKET_LENGTH, 0);
    } else {
        char *buf;
        protobuf_get(self, PACKET_BUF, &buf, NULL);
        if (name != buf) {
            protobuf_set(self, PACKET_BUF, name, strlen(name) + 1);
        }
    }

    if ((ret = rpc_call(self)) == AAOS_OK) {
        protobuf_get(self, PACKET_ERRORCODE, &errorcode);
        if (errorcode != AAOS_OK) {
            return errorcode;
        }
    } else {
        return -1 * ret; /*Networking error.*/
    }

    return ret;
}

int
scheduler_mask_site_by_id(void *_self, uint64_t identifier)
{
    const struct SchedulerClass *class = (const struct SchedulerClass *) classOf(_self);
    
    if (isOf(class, SchedulerClass()) && class->mask_site_by_id.method) {
        return ((int (*)(void *, uint64_t)) class->mask_site_by_id.method)(_self, identifier);
    } else {
        int result;
        forward(_self, &result, (Method) scheduler_mask_site_by_id, "mask_site_by_id", _self, identifier);
        return result;
    }
}

int
Scheduler_mask_site_by_id(void *_self, uint64_t identifier)
{
    struct Scheduler *self = cast(Scheduler(), _self);
   
    uint16_t errorcode;
    
    int ret = AAOS_OK;

    protobuf_set(self, PACKET_COMMAND, SCHEDULER_MASK_SITE_BY_ID);
    protobuf_set(self, PACKET_U64F0, identifier);
    protobuf_set(self, PACKET_LENGTH, 0);

    if ((ret = rpc_call(self)) == AAOS_OK) {
        protobuf_get(self, PACKET_ERRORCODE, &errorcode);
        if (errorcode != AAOS_OK) {
            return errorcode;
        }
    } else {
        return -1 * ret; /*Networking error.*/
    }

    return ret;
}

int
scheduler_mask_site_by_name(void *_self, const char *name)
{
    const struct SchedulerClass *class = (const struct SchedulerClass *) classOf(_self);
    
    if (isOf(class, SchedulerClass()) && class->mask_site_by_name.method) {
        return ((int (*)(void *, const char *)) class->mask_site_by_name.method)(_self, name);
    } else {
        int result;
        forward(_self, &result, (Method) scheduler_mask_site_by_name, "mask_site_by_name", _self, name);
        return result;
    }
}

int
Scheduler_mask_site_by_name(void *_self, const char *name)
{
    struct Scheduler *self = cast(Scheduler(), _self);
   
    uint16_t errorcode;
    
    int ret = AAOS_OK;

    protobuf_set(self, PACKET_COMMAND, SCHEDULER_MASK_SITE_BY_NAME);
    if (strlen(name) < PACKETPARAMETERSIZE - 1) {
        protobuf_set(self, PACKET_STR, name);
        protobuf_set(self, PACKET_LENGTH, 0);
    } else {
        char *buf;
        protobuf_get(self, PACKET_BUF, &buf, NULL);
        if (name != buf) {
            protobuf_set(self, PACKET_BUF, name, strlen(name) + 1);
        }
    }

    if ((ret = rpc_call(self)) == AAOS_OK) {
        protobuf_get(self, PACKET_ERRORCODE, &errorcode);
        if (errorcode != AAOS_OK) {
            return errorcode;
        }
    } else {
        return -1 * ret; /*Networking error.*/
    }

    return ret;
}

int
scheduler_unmask_site_by_id(void *_self, uint64_t identifier)
{
    const struct SchedulerClass *class = (const struct SchedulerClass *) classOf(_self);
    
    if (isOf(class, SchedulerClass()) && class->unmask_site_by_id.method) {
        return ((int (*)(void *, uint64_t)) class->unmask_site_by_id.method)(_self, identifier);
    } else {
        int result;
        forward(_self, &result, (Method) scheduler_unmask_site_by_id, "unmask_site_by_id", _self, identifier);
        return result;
    }
}

int
Scheduler_unmask_site_by_id(void *_self, uint64_t identifier)
{
    struct Scheduler *self = cast(Scheduler(), _self);
   
    uint16_t errorcode;
    
    int ret = AAOS_OK;

    protobuf_set(self, PACKET_COMMAND, SCHEDULER_UNMASK_SITE_BY_ID);
    protobuf_set(self, PACKET_U64F0, identifier);
    protobuf_set(self, PACKET_LENGTH, 0);

    if ((ret = rpc_call(self)) == AAOS_OK) {
        protobuf_get(self, PACKET_ERRORCODE, &errorcode);
        if (errorcode != AAOS_OK) {
            return errorcode;
        }
    } else {
        return -1 * ret; /*Networking error.*/
    }

    return ret;
}

int
scheduler_unmask_site_by_name(void *_self, const char *name)
{
    const struct SchedulerClass *class = (const struct SchedulerClass *) classOf(_self);
    
    if (isOf(class, SchedulerClass()) && class->unmask_site_by_name.method) {
        return ((int (*)(void *, const char *)) class->unmask_site_by_name.method)(_self, name);
    } else {
        int result;
        forward(_self, &result, (Method) scheduler_unmask_site_by_name, "mask_unsite_by_name", _self, name);
        return result;
    }
}

int
Scheduler_unmask_site_by_name(void *_self, const char *name)
{
    struct Scheduler *self = cast(Scheduler(), _self);
   
    uint16_t errorcode;
    
    int ret = AAOS_OK;

    protobuf_set(self, PACKET_COMMAND, SCHEDULER_UNMASK_SITE_BY_NAME);
    if (strlen(name) < PACKETPARAMETERSIZE - 1) {
        protobuf_set(self, PACKET_STR, name);
        protobuf_set(self, PACKET_LENGTH, 0);
    } else {
        char *buf;
        protobuf_get(self, PACKET_BUF, &buf, NULL);
        if (name != buf) {
            protobuf_set(self, PACKET_BUF, name, strlen(name) + 1);
        }
    }

    if ((ret = rpc_call(self)) == AAOS_OK) {
        protobuf_get(self, PACKET_ERRORCODE, &errorcode);
        if (errorcode != AAOS_OK) {
            return errorcode;
        }
    } else {
        return -1 * ret; /*Networking error.*/
    }

    return ret;
}

int
scheduler_list_telescope(void *_self, char *result, size_t size, size_t *length, unsigned int *type)
{
    const struct SchedulerClass *class = (const struct SchedulerClass *) classOf(_self);
    
    if (isOf(class, SchedulerClass()) && class->list_telescope.method) {
        return ((int (*)(void *, char *, size_t, size_t *, unsigned int *)) class->list_telescope.method)(_self, result, size, length, type);
    } else {
        int result;
        forward(_self, &result, (Method) scheduler_list_telescope, "list_telescope", _self, result, size, type);
        return result;
    }
}

static int
Scheduler_list_telescope(void *_self, char *result, size_t size, size_t *length, unsigned int *type)
{
    struct Scheduler *self = cast(Scheduler(), _self);
    char *res = NULL;
    uint32_t len;
    uint16_t errorcode, option;

    int ret = AAOS_OK;

    protobuf_set(self, PACKET_COMMAND, SCHEDULER_LIST_TELESCOPE);
    protobuf_set(self, PACKET_LENGTH, 0);

    if ((ret = rpc_call(self)) == AAOS_OK) {
        protobuf_get(self, PACKET_ERRORCODE, &errorcode);
        if (errorcode != AAOS_OK) {
            return errorcode;
        }
        if (type != NULL) {
            protobuf_get(self, PACKET_OPTION, &option);
            *type = option;
        }
        protobuf_get(self, PACKET_BUF, &res, &len);
        if (result != res) {
            snprintf(result, size, "%s", res);
        }
        if (length != NULL) {
            *length = strlen(result);
        }
    } else {
        return -1 * ret; /*Networking error.*/
    }

    return ret;
}

int
scheduler_add_telescope(void *_self, const char *info, unsigned int type)
{
    const struct SchedulerClass *class = (const struct SchedulerClass *) classOf(_self);

    int result;
    
    if (isOf(class, SchedulerClass()) && class->add_telescope.method) {
        result = ((int (*)(void *, const char *, unsigned int)) class->add_telescope.method)(_self, info, type);
    } else {
        forward(_self, &result, (Method) scheduler_add_telescope, "add_telescope", _self, info, type);
        
    }
    return result;
}

static int
Scheduler_add_telescope(void *_self, const char *info, unsigned int type)
{
    struct Scheduler *self = cast(Scheduler(), _self);

    uint16_t option = type, errorcode;
    char *buf;
    int ret = AAOS_OK;

    protobuf_set(self, PACKET_COMMAND, SCHEDULER_ADD_TELESCOPE);
    protobuf_set(self, PACKET_OPTION, option);
    protobuf_get(self, PACKET_BUF, &buf, NULL);
    if (info != buf) {
        protobuf_set(self, PACKET_BUF, info, strlen(info) + 1);
    }

    if ((ret = rpc_call(self)) == AAOS_OK) {
        protobuf_get(self, PACKET_ERRORCODE, &errorcode);
        if (errorcode != AAOS_OK) {
            return errorcode;
        }
    } else {
        return -1 * ret; /*Networking error.*/
    }

    return ret;
}

int
scheduler_delete_telescope_by_id(void *_self, uint64_t identifier)
{
    const struct SchedulerClass *class = (const struct SchedulerClass *) classOf(_self);
    
    if (isOf(class, SchedulerClass()) && class->delete_telescope_by_id.method) {
        return ((int (*)(void *, uint64_t)) class->delete_telescope_by_id.method)(_self, identifier);
    } else {
        int result;
        forward(_self, &result, (Method) scheduler_delete_telescope_by_id, "delete_telescope_by_id", _self, identifier);
        return result;
    }
}

int
Scheduler_delete_telescope_by_id(void *_self, uint64_t identifier)
{
    struct Scheduler *self = cast(Scheduler(), _self);
   
    uint16_t errorcode;
    
    int ret = AAOS_OK;

    protobuf_set(self, PACKET_COMMAND, SCHEDULER_DELETE_TELESCOPE_BY_ID);
    protobuf_set(self, PACKET_U64F0, identifier);
    protobuf_set(self, PACKET_LENGTH, 0);

    if ((ret = rpc_call(self)) == AAOS_OK) {
        protobuf_get(self, PACKET_ERRORCODE, &errorcode);
        if (errorcode != AAOS_OK) {
            return errorcode;
        }
    } else {
        return -1 * ret; /*Networking error.*/
    }

    return ret;
}

int
scheduler_delete_telescope_by_name(void *_self, const char *name)
{
    const struct SchedulerClass *class = (const struct SchedulerClass *) classOf(_self);
    
    if (isOf(class, SchedulerClass()) && class->delete_telescope_by_name.method) {
        return ((int (*)(void *, const char *)) class->delete_telescope_by_name.method)(_self, name);
    } else {
        int result;
        forward(_self, &result, (Method) scheduler_delete_telescope_by_name, "delete_telescope_by_name", _self, name);
        return result;
    }
}

int
Scheduler_delete_telescope_by_name(void *_self, const char *name)
{
    struct Scheduler *self = cast(Scheduler(), _self);
   
    uint16_t errorcode;
    
    int ret = AAOS_OK;

    protobuf_set(self, PACKET_COMMAND, SCHEDULER_DELETE_TELESCOPE_BY_NAME);
    if (strlen(name) < PACKETPARAMETERSIZE - 1) {
        protobuf_set(self, PACKET_STR, name);
        protobuf_set(self, PACKET_LENGTH, 0);
    } else {
        char *buf;
        protobuf_get(self, PACKET_BUF, &buf, NULL);
        if (name != buf) {
            protobuf_set(self, PACKET_BUF, name, strlen(name) + 1);
        }
    }

    if ((ret = rpc_call(self)) == AAOS_OK) {
        protobuf_get(self, PACKET_ERRORCODE, &errorcode);
        if (errorcode != AAOS_OK) {
            return errorcode;
        }
    } else {
        return -1 * ret; /*Networking error.*/
    }

    return ret;
}

int
scheduler_mask_telescope_by_id(void *_self, uint64_t identifier)
{
    const struct SchedulerClass *class = (const struct SchedulerClass *) classOf(_self);
    
    if (isOf(class, SchedulerClass()) && class->mask_telescope_by_id.method) {
        return ((int (*)(void *, uint64_t)) class->mask_telescope_by_id.method)(_self, identifier);
    } else {
        int result;
        forward(_self, &result, (Method) scheduler_mask_telescope_by_id, "mask_telescope_by_id", _self, identifier);
        return result;
    }
}

int
Scheduler_mask_telescope_by_id(void *_self, uint64_t identifier)
{
    struct Scheduler *self = cast(Scheduler(), _self);
   
    uint16_t errorcode;
    
    int ret = AAOS_OK;

    protobuf_set(self, PACKET_COMMAND, SCHEDULER_MASK_TELESCOPE_BY_ID);
    protobuf_set(self, PACKET_U64F0, identifier);
    protobuf_set(self, PACKET_LENGTH, 0);

    if ((ret = rpc_call(self)) == AAOS_OK) {
        protobuf_get(self, PACKET_ERRORCODE, &errorcode);
        if (errorcode != AAOS_OK) {
            return errorcode;
        }
    } else {
        return -1 * ret; /*Networking error.*/
    }

    return ret;
}

int
scheduler_mask_telescope_by_name(void *_self, const char *name)
{
    const struct SchedulerClass *class = (const struct SchedulerClass *) classOf(_self);
    
    if (isOf(class, SchedulerClass()) && class->mask_telescope_by_name.method) {
        return ((int (*)(void *, const char *)) class->mask_telescope_by_name.method)(_self, name);
    } else {
        int result;
        forward(_self, &result, (Method) scheduler_mask_telescope_by_name, "mask_telescope_by_name", _self, name);
        return result;
    }
}

int
Scheduler_mask_telescope_by_name(void *_self, const char *name)
{
    struct Scheduler *self = cast(Scheduler(), _self);
   
    uint16_t errorcode;
    
    int ret = AAOS_OK;

    protobuf_set(self, PACKET_COMMAND, SCHEDULER_MASK_TELESCOPE_BY_NAME);
    if (strlen(name) < PACKETPARAMETERSIZE - 1) {
        protobuf_set(self, PACKET_STR, name);
        protobuf_set(self, PACKET_LENGTH, 0);
    } else {
        char *buf;
        protobuf_get(self, PACKET_BUF, &buf, NULL);
        if (name != buf) {
            protobuf_set(self, PACKET_BUF, name, strlen(name) + 1);
        }
    }

    if ((ret = rpc_call(self)) == AAOS_OK) {
        protobuf_get(self, PACKET_ERRORCODE, &errorcode);
        if (errorcode != AAOS_OK) {
            return errorcode;
        }
    } else {
        return -1 * ret; /*Networking error.*/
    }

    return ret;
}

int
scheduler_unmask_telescope_by_id(void *_self, uint64_t identifier)
{
    const struct SchedulerClass *class = (const struct SchedulerClass *) classOf(_self);
    
    if (isOf(class, SchedulerClass()) && class->unmask_telescope_by_id.method) {
        return ((int (*)(void *, uint64_t)) class->unmask_telescope_by_id.method)(_self, identifier);
    } else {
        int result;
        forward(_self, &result, (Method) scheduler_unmask_telescope_by_id, "unmask_telescope_by_id", _self, identifier);
        return result;
    }
}

int
Scheduler_unmask_telescope_by_id(void *_self, uint64_t identifier)
{
    struct Scheduler *self = cast(Scheduler(), _self);
   
    uint16_t errorcode;
    
    int ret = AAOS_OK;

    protobuf_set(self, PACKET_COMMAND, SCHEDULER_UNMASK_TELESCOPE_BY_ID);
    protobuf_set(self, PACKET_U64F0, identifier);
    protobuf_set(self, PACKET_LENGTH, 0);

    if ((ret = rpc_call(self)) == AAOS_OK) {
        protobuf_get(self, PACKET_ERRORCODE, &errorcode);
        if (errorcode != AAOS_OK) {
            return errorcode;
        }
    } else {
        return -1 * ret; /*Networking error.*/
    }

    return ret;
}

int
scheduler_unmask_telescope_by_name(void *_self, const char *name)
{
    const struct SchedulerClass *class = (const struct SchedulerClass *) classOf(_self);
    
    if (isOf(class, SchedulerClass()) && class->unmask_telescope_by_name.method) {
        return ((int (*)(void *, const char *)) class->unmask_telescope_by_name.method)(_self, name);
    } else {
        int result;
        forward(_self, &result, (Method) scheduler_unmask_telescope_by_name, "mask_untelescope_by_name", _self, name);
        return result;
    }
}

int
Scheduler_unmask_telescope_by_name(void *_self, const char *name)
{
    struct Scheduler *self = cast(Scheduler(), _self);
   
    uint16_t errorcode;
    
    int ret = AAOS_OK;

    protobuf_set(self, PACKET_COMMAND, SCHEDULER_UNMASK_TELESCOPE_BY_NAME);
    if (strlen(name) < PACKETPARAMETERSIZE - 1) {
        protobuf_set(self, PACKET_STR, name);
        protobuf_set(self, PACKET_LENGTH, 0);
    } else {
        char *buf;
        protobuf_get(self, PACKET_BUF, &buf, NULL);
        if (name != buf) {
            protobuf_set(self, PACKET_BUF, name, strlen(name) + 1);
        }
    }

    if ((ret = rpc_call(self)) == AAOS_OK) {
        protobuf_get(self, PACKET_ERRORCODE, &errorcode);
        if (errorcode != AAOS_OK) {
            return errorcode;
        }
    } else {
        return -1 * ret; /*Networking error.*/
    }

    return ret;
}

int
scheduler_list_target(void *_self, char *result, size_t size, size_t *length, unsigned int  *type)
{
    const struct SchedulerClass *class = (const struct SchedulerClass *) classOf(_self);
    
    if (isOf(class, SchedulerClass()) && class->list_target.method) {
        return ((int (*)(void *, char *, size_t, size_t *, unsigned int *)) class->list_target.method)(_self, result, size, length, type);
    } else {
        int result;
        forward(_self, &result, (Method) scheduler_list_target, "list_target", _self, result, size, type);
        return result;
    }
}

static int
Scheduler_list_target(void *_self, char *result, size_t size, size_t *length, int *type)
{
    struct Scheduler *self = cast(Scheduler(), _self);
    char *res = NULL;
    uint32_t len;
    uint16_t errorcode, option;

    int ret = AAOS_OK;

    protobuf_set(self, PACKET_COMMAND, SCHEDULER_LIST_TARGET);
    protobuf_set(self, PACKET_LENGTH, 0);

    if ((ret = rpc_call(self)) == AAOS_OK) {
        protobuf_get(self, PACKET_ERRORCODE, &errorcode);
        if (errorcode != AAOS_OK) {
            return errorcode;
        }
        if (type != NULL) {
            protobuf_get(self, PACKET_OPTION, &option);
            *type = option;
        }
        protobuf_get(self, PACKET_BUF, &res, &len);
        if (result != res) {
            snprintf(result, size, "%s", res);
        }
        if (length != NULL) {
            *length = strlen(result);
        }
    } else {
        return -1 * ret; /*Networking error.*/
    }

    return ret;
}

int
scheduler_add_target(void *_self, const char *info, unsigned int type)
{
    const struct SchedulerClass *class = (const struct SchedulerClass *) classOf(_self);

    int result;
    
    if (isOf(class, SchedulerClass()) && class->add_target.method) {
        result = ((int (*)(void *, const char *, unsigned int)) class->add_target.method)(_self, info, type);
    } else {
        forward(_self, &result, (Method) scheduler_add_target, "add_target", _self, info, type);
        
    }
    return result;
}

static int
Scheduler_add_target(void *_self, const char *info, unsigned int type)
{
    struct Scheduler *self = cast(Scheduler(), _self);

    uint16_t option = type, errorcode;
    char *buf;
    int ret = AAOS_OK;

    protobuf_set(self, PACKET_COMMAND, SCHEDULER_ADD_TARGET);
    protobuf_set(self, PACKET_OPTION, option);
    protobuf_get(self, PACKET_BUF, &buf, NULL);
    if (info != buf) {
        protobuf_set(self, PACKET_BUF, info, strlen(info) + 1);
    }

    if ((ret = rpc_call(self)) == AAOS_OK) {
        protobuf_get(self, PACKET_ERRORCODE, &errorcode);
        if (errorcode != AAOS_OK) {
            return errorcode;
        }
    } else {
        return -1 * ret; /*Networking error.*/
    }

    return ret;
}

int
scheduler_delete_target_by_id(void *_self, uint64_t identifier, uint32_t nside)
{
    const struct SchedulerClass *class = (const struct SchedulerClass *) classOf(_self);
    
    if (isOf(class, SchedulerClass()) && class->delete_target_by_id.method) {
        return ((int (*)(void *, uint64_t, uint32_t)) class->delete_target_by_id.method)(_self, identifier, nside);
    } else {
        int result;
        forward(_self, &result, (Method) scheduler_delete_target_by_id, "delete_target_by_id", _self, identifier, nside);
        return result;
    }
}

int
Scheduler_delete_target_by_id(void *_self, uint64_t identifier, uint32_t nside)
{
    struct Scheduler *self = cast(Scheduler(), _self);
   
    uint16_t errorcode;
    
    int ret = AAOS_OK;

    protobuf_set(self, PACKET_COMMAND, SCHEDULER_DELETE_TARGET_BY_ID);
    protobuf_set(self, PACKET_U64F0, identifier);
    protobuf_set(self, PACKET_U32F3, nside);
    protobuf_set(self, PACKET_LENGTH, 0);

    if ((ret = rpc_call(self)) == AAOS_OK) {
        protobuf_get(self, PACKET_ERRORCODE, &errorcode);
        if (errorcode != AAOS_OK) {
            return errorcode;
        }
    } else {
        return -1 * ret; /*Networking error.*/
    }

    return ret;
}

int
scheduler_delete_target_by_name(void *_self, const char *name)
{
    const struct SchedulerClass *class = (const struct SchedulerClass *) classOf(_self);
    
    if (isOf(class, SchedulerClass()) && class->delete_target_by_name.method) {
        return ((int (*)(void *, const char *)) class->delete_target_by_name.method)(_self, name);
    } else {
        int result;
        forward(_self, &result, (Method) scheduler_delete_target_by_name, "delete_target_by_name", _self, name);
        return result;
    }
}

int
Scheduler_delete_target_by_name(void *_self, const char *name)
{
    struct Scheduler *self = cast(Scheduler(), _self);
   
    uint16_t errorcode;
    
    int ret = AAOS_OK;

    protobuf_set(self, PACKET_COMMAND, SCHEDULER_DELETE_TARGET_BY_NAME);
    if (strlen(name) < PACKETPARAMETERSIZE - 1) {
        protobuf_set(self, PACKET_STR, name);
        protobuf_set(self, PACKET_LENGTH, 0);
    } else {
        char *buf;
        protobuf_get(self, PACKET_BUF, &buf, NULL);
        if (name != buf) {
            protobuf_set(self, PACKET_BUF, name, strlen(name) + 1);
        }
    }

    if ((ret = rpc_call(self)) == AAOS_OK) {
        protobuf_get(self, PACKET_ERRORCODE, &errorcode);
        if (errorcode != AAOS_OK) {
            return errorcode;
        }
    } else {
        return -1 * ret; /*Networking error.*/
    }

    return ret;
}

int
scheduler_mask_target_by_id(void *_self, uint64_t identifier, uint32_t nside)
{
    const struct SchedulerClass *class = (const struct SchedulerClass *) classOf(_self);
    
    if (isOf(class, SchedulerClass()) && class->mask_target_by_id.method) {
        return ((int (*)(void *, uint64_t, uint32_t)) class->mask_target_by_id.method)(_self, identifier, nside);
    } else {
        int result;
        forward(_self, &result, (Method) scheduler_mask_target_by_id, "mask_target_by_id", _self, identifier, nside);
        return result;
    }
}

int
Scheduler_mask_target_by_id(void *_self, uint64_t identifier, uint32_t nside)
{
    struct Scheduler *self = cast(Scheduler(), _self);
   
    uint16_t errorcode;
    
    int ret = AAOS_OK;

    protobuf_set(self, PACKET_COMMAND, SCHEDULER_MASK_TARGET_BY_ID);
    protobuf_set(self, PACKET_U64F0, identifier);
    protobuf_set(self, PACKET_U32F3, nside);
    protobuf_set(self, PACKET_LENGTH, 0);

    if ((ret = rpc_call(self)) == AAOS_OK) {
        protobuf_get(self, PACKET_ERRORCODE, &errorcode);
        if (errorcode != AAOS_OK) {
            return errorcode;
        }
    } else {
        return -1 * ret; /*Networking error.*/
    }

    return ret;
}

int
scheduler_mask_target_by_name(void *_self, const char *name)
{
    const struct SchedulerClass *class = (const struct SchedulerClass *) classOf(_self);
    
    if (isOf(class, SchedulerClass()) && class->mask_target_by_name.method) {
        return ((int (*)(void *, const char *)) class->mask_target_by_name.method)(_self, name);
    } else {
        int result;
        forward(_self, &result, (Method) scheduler_mask_target_by_name, "mask_target_by_name", _self, name);
        return result;
    }
}

int
Scheduler_mask_target_by_name(void *_self, const char *name)
{
    struct Scheduler *self = cast(Scheduler(), _self);
   
    uint16_t errorcode;
    
    int ret = AAOS_OK;

    protobuf_set(self, PACKET_COMMAND, SCHEDULER_MASK_TARGET_BY_NAME);
    if (strlen(name) < PACKETPARAMETERSIZE - 1) {
        protobuf_set(self, PACKET_STR, name);
        protobuf_set(self, PACKET_LENGTH, 0);
    } else {
        char *buf;
        protobuf_get(self, PACKET_BUF, &buf, NULL);
        if (name != buf) {
            protobuf_set(self, PACKET_BUF, name, strlen(name) + 1);
        }
    }

    if ((ret = rpc_call(self)) == AAOS_OK) {
        protobuf_get(self, PACKET_ERRORCODE, &errorcode);
        if (errorcode != AAOS_OK) {
            return errorcode;
        }
    } else {
        return -1 * ret; /*Networking error.*/
    }

    return ret;
}

int
scheduler_unmask_target_by_id(void *_self, uint64_t identifier, uint32_t nside)
{
    const struct SchedulerClass *class = (const struct SchedulerClass *) classOf(_self);
    
    if (isOf(class, SchedulerClass()) && class->unmask_target_by_id.method) {
        return ((int (*)(void *, uint64_t, uint32_t)) class->unmask_target_by_id.method)(_self, identifier, nside);
    } else {
        int result;
        forward(_self, &result, (Method) scheduler_unmask_target_by_id, "unmask_target_by_id", _self, identifier, nside);
        return result;
    }
}

int
Scheduler_unmask_target_by_id(void *_self, uint64_t identifier, uint32_t nside)
{
    struct Scheduler *self = cast(Scheduler(), _self);
   
    uint16_t errorcode;
    
    int ret = AAOS_OK;

    protobuf_set(self, PACKET_COMMAND, SCHEDULER_UNMASK_TARGET_BY_ID);
    protobuf_set(self, PACKET_U64F0, identifier);
    protobuf_set(self, PACKET_U32F3, nside);
    protobuf_set(self, PACKET_LENGTH, 0);

    if ((ret = rpc_call(self)) == AAOS_OK) {
        protobuf_get(self, PACKET_ERRORCODE, &errorcode);
        if (errorcode != AAOS_OK) {
            return errorcode;
        }
    } else {
        return -1 * ret; /*Networking error.*/
    }

    return ret;
}

int
scheduler_unmask_target_by_name(void *_self, const char *name)
{
    const struct SchedulerClass *class = (const struct SchedulerClass *) classOf(_self);
    
    if (isOf(class, SchedulerClass()) && class->unmask_target_by_name.method) {
        return ((int (*)(void *, const char *)) class->unmask_target_by_name.method)(_self, name);
    } else {
        int result;
        forward(_self, &result, (Method) scheduler_unmask_target_by_name, "mask_untarget_by_name", _self, name);
        return result;
    }
}

int
Scheduler_unmask_target_by_name(void *_self, const char *name)
{
    struct Scheduler *self = cast(Scheduler(), _self);
   
    uint16_t errorcode;
    
    int ret = AAOS_OK;

    protobuf_set(self, PACKET_COMMAND, SCHEDULER_UNMASK_TARGET_BY_NAME);
    if (strlen(name) < PACKETPARAMETERSIZE - 1) {
        protobuf_set(self, PACKET_STR, name);
        protobuf_set(self, PACKET_LENGTH, 0);
    } else {
        char *buf;
        protobuf_get(self, PACKET_BUF, &buf, NULL);
        if (name != buf) {
            protobuf_set(self, PACKET_BUF, name, strlen(name) + 1);
        }
    }

    if ((ret = rpc_call(self)) == AAOS_OK) {
        protobuf_get(self, PACKET_ERRORCODE, &errorcode);
        if (errorcode != AAOS_OK) {
            return errorcode;
        }
    } else {
        return -1 * ret; /*Networking error.*/
    }

    return ret;
}

int 
scheduler_add_task_record(void *_self, const char *info, unsigned int type)
{
    const struct SchedulerClass *class = (const struct SchedulerClass *) classOf(_self);
    
    if (isOf(class, SchedulerClass()) && class->add_task_record.method) {
        return ((int (*)(void *, const char *, unsigned int)) class->add_task_record.method)(_self, info, type);
    } else {
        int result;
        forward(_self, &result, (Method) scheduler_add_task_record, "add_task_record", _self, info, type);
        return result;
    }
}

static int
Scheduler_add_task_record(void *_self, const char *info, unsigned int type)
{
    struct Scheduler *self = cast(Scheduler(), _self);
   
    uint16_t errorcode, option = type;
    char *buf;
    int ret = AAOS_OK;

    protobuf_set(self, PACKET_COMMAND, SCHEDULER_ADD_TASK_RECORD);
    protobuf_set(self, PACKET_OPTION, option);    
    protobuf_get(self, PACKET_BUF, &buf, NULL);
    if (info != buf) {
        protobuf_set(self, PACKET_BUF, info, strlen(info) + 1);
    }
    
    if ((ret = rpc_call(self)) == AAOS_OK) {
        protobuf_get(self, PACKET_ERRORCODE, &errorcode);
        if (errorcode != AAOS_OK) {
            return errorcode;
        }
    } else {
        return -1 * ret; /*Networking error.*/
    }

    return ret;
}

int 
scheduler_update_task_record(void *_self, uint64_t identifier, const char *info, unsigned int type)
{
    const struct SchedulerClass *class = (const struct SchedulerClass *) classOf(_self);
    
    if (isOf(class, SchedulerClass()) && class->update_task_record.method) {
        return ((int (*)(void *, uint64_t,  char *, unsigned int)) class->update_task_record.method)(_self, identifier, info, type);
    } else {
        int result;
        forward(_self, &result, (Method) scheduler_update_task_record, "update_task_record", _self, identifier, info, type);
        return result;
    }
}

static int
Scheduler_update_task_record(void *_self, uint64_t identifier, const char *info, unsigned int type)
{
    struct Scheduler *self = cast(Scheduler(), _self);
   
    uint16_t errorcode, option = type;
    char *buf;
    int ret = AAOS_OK;

    protobuf_set(self, PACKET_COMMAND, SCHEDULER_UPDATE_TASK_RECORD);
    protobuf_set(self, PACKET_OPTION, option);
    protobuf_set(self, PACKET_U64F0, identifier);    
    protobuf_get(self, PACKET_BUF, &buf, NULL);
    if (info != buf) {
        protobuf_set(self, PACKET_BUF, info, strlen(info) + 1);
    }
    
    if ((ret = rpc_call(self)) == AAOS_OK) {
        protobuf_get(self, PACKET_ERRORCODE, &errorcode);
        if (errorcode != AAOS_OK) {
            return errorcode;
        }
    } else {
        return -1 * ret; /*Networking error.*/
    }

    return ret;
}


static const void *scheduler_virtual_table(void);

static int
Scheduler_execute_get_task_by_telescope_id(struct Scheduler *self)
{
    uint16_t option, error_code;
    uint32_t length;
    uint64_t tel_id;
    char *buf;
    unsigned int type;
    size_t size;
    int ret;

    protobuf_get(self, PACKET_U64F0, &tel_id);
    protobuf_get(self, PACKET_BUF, &buf, NULL);
    protobuf_get(self, PACKET_SIZE, &size);
    if ((ret = __scheduler_get_task_by_telescope_id(scheduler, tel_id, buf, size, NULL, &type)) == AAOS_OK) {
        option = type;
        length = strlen(buf) + 1;
        protobuf_set(self, PACKET_OPTION, option);
        protobuf_set(self, PACKET_BUF, buf, length);
        protobuf_set(self, PACKET_LENGTH, length);
    }

    return ret;
}

static int
Scheduler_execute_get_task_by_telescope_name(struct Scheduler *self)
{
    char *name, *buf;
    uint32_t length;
    uint64_t tel_id;
    unsigned int type;
    size_t size;
    int ret;
    uint16_t option;

    protobuf_get(self, PACKET_LENGTH, &length);
    if (length == 0) {
        protobuf_get(self, PACKET_STR, &name);
    } else {
        protobuf_get(self, PACKET_BUF, &name, NULL);
    }

    protobuf_get(self, PACKET_BUF, &buf, NULL);
    protobuf_get(self, PACKET_SIZE, &size);

    if ((ret = __scheduler_get_task_by_telescope_name(scheduler, name, buf, size, NULL, &type)) == AAOS_OK) {
        option = type;
        length = strlen(buf) + 1;
        protobuf_set(self, PACKET_OPTION, option);
        protobuf_set(self, PACKET_BUF, buf, length);
        protobuf_set(self, PACKET_LENGTH, length);
    }

    return AAOS_OK;
}

static int
Scheduler_execute_pop_task_block(struct Scheduler *self)
{
    uint16_t option;
    uint64_t identifier;
    uint32_t length;
    char *block_buf;
    unsigned int type;
    int ret;

    protobuf_get(self, PACKET_BUF, &block_buf, &length);
    protobuf_get(self, PACKET_OPTION, &option);
    type = option;

    if ((ret = __scheduler_push_task_block(scheduler, block_buf, type)) == AAOS_OK) {
        protobuf_set(self, PACKET_LENGTH, 0);
    }

    return 0;
}

static int
Scheduler_execute_update_status(struct Scheduler *self)
{
    uint16_t option;
    uint64_t identifier;
    uint32_t length;
    char *buf;
    unsigned int type;
    int ret;
    
    protobuf_get(self, PACKET_BUF, &buf, &length);
    protobuf_get(self, PACKET_OPTION, &option);
    type = option;

    if ((ret = __scheduler_update_status(scheduler, buf, type)) == AAOS_OK) {
        option = type;
        length = strlen(buf) + 1;
        protobuf_set(self, PACKET_OPTION, option);
        protobuf_set(self, PACKET_BUF, buf, length);
        protobuf_set(self, PACKET_LENGTH, length);
    }

    return ret;
}

static int
Scheduler_execute_list_site(struct Scheduler *self)
{   
    uint16_t option;
    uint32_t length;
   
    char *buf;
    unsigned int type;
    size_t size;
    int ret;

    protobuf_get(self, PACKET_BUF, &buf, NULL);
    protobuf_get(self, PACKET_SIZE, &size);
    if ((ret = __scheduler_list_site(scheduler, buf, size, &type)) == AAOS_OK) {
        option = type;
        length = strlen(buf) + 1;
        protobuf_set(self, PACKET_OPTION, option);
        protobuf_set(self, PACKET_BUF, buf, length);
        protobuf_set(self, PACKET_LENGTH, length);
    }

    return ret;
}

static int
Scheduler_execute_add_site(struct Scheduler *self)
{   
    uint16_t option;
    uint32_t length;
    char *buf;
    unsigned int type;
    size_t size;
    int ret = AAOS_OK;

    protobuf_get(self, PACKET_BUF, &buf, NULL);
    protobuf_get(self, PACKET_OPTION, &option);
    type = option;

    if ((ret = __scheduler_add_site(scheduler, buf, type) == AAOS_OK)) {   
        protobuf_set(self, PACKET_LENGTH, 0);
    }

    return ret;
}

static int
Scheduler_execute_delete_site_by_id(struct Scheduler *self)
{   
    uint64_t identifier;
    uint16_t option;
    int ret;

    protobuf_get(self, PACKET_U64F0, &identifier);
    if ((ret = __scheduler_delete_site_by_id(scheduler, identifier) == AAOS_OK)) {   
        protobuf_set(self, PACKET_LENGTH, 0);
    }

    return ret;
}

static int
Scheduler_execute_delete_site_by_name(struct Scheduler *self)
{   
    char *name;
    uint32_t length;
    int ret = AAOS_OK;

    protobuf_get(self, PACKET_LENGTH, &length);
    if (length == 0) {
        protobuf_get(self, PACKET_STR, &name);
    } else {
        protobuf_get(self, PACKET_BUF, &name, NULL);
    }

    if ((ret = __scheduler_delete_site_by_name(scheduler, name) == AAOS_OK)) {   
        protobuf_set(self, PACKET_LENGTH, 0);
    }

    return ret;
}

static int
Scheduler_execute_mask_site_by_id(struct Scheduler *self)
{   
    uint64_t identifier;
    uint16_t option;
    int ret;

    protobuf_get(self, PACKET_U64F0, &identifier);
    if ((ret = __scheduler_mask_site_by_id(scheduler, identifier) == AAOS_OK)) {   
        protobuf_set(self, PACKET_LENGTH, 0);
    }

    return ret;
}

static int
Scheduler_execute_mask_site_by_name(struct Scheduler *self)
{   
    char *name;
    uint32_t length;
    int ret = AAOS_OK;

    protobuf_get(self, PACKET_LENGTH, &length);
    if (length == 0) {
        protobuf_get(self, PACKET_STR, &name);
    } else {
        protobuf_get(self, PACKET_BUF, &name, NULL);
    }

    if ((ret = __scheduler_mask_site_by_name(scheduler, name) == AAOS_OK)) {   
        protobuf_set(self, PACKET_LENGTH, 0);
    }

    return ret;
}

static int
Scheduler_execute_unmask_site_by_id(struct Scheduler *self)
{   
    uint64_t identifier;
    uint16_t option;
    int ret;

    protobuf_get(self, PACKET_U64F0, &identifier);
    if ((ret = __scheduler_mask_site_by_id(scheduler, identifier) == AAOS_OK)) {   
        protobuf_set(self, PACKET_LENGTH, 0);
    }

    return ret;
}

static int
Scheduler_execute_unmask_site_by_name(struct Scheduler *self)
{   
    char *name;
    uint32_t length;
    int ret = AAOS_OK;

    protobuf_get(self, PACKET_LENGTH, &length);
    if (length == 0) {
        protobuf_get(self, PACKET_STR, &name);
    } else {
        protobuf_get(self, PACKET_BUF, &name, NULL);
    }

    if ((ret = __scheduler_mask_site_by_name(scheduler, name) == AAOS_OK)) {   
        protobuf_set(self, PACKET_LENGTH, 0);
    }

    return ret;
}

static int
Scheduler_execute_list_telescope(struct Scheduler *self)
{   
    uint16_t option;
    uint32_t length;
    char *buf;
    unsigned int type;
    size_t size;
    int ret;

    protobuf_get(self, PACKET_BUF, &buf, NULL);
    protobuf_get(self, PACKET_SIZE, &size);
    if ((ret = __scheduler_list_telescope(scheduler, buf, size, &type)) == AAOS_OK) {
        option = type;
        length = strlen(buf) + 1;
        protobuf_set(self, PACKET_OPTION, option);
        protobuf_set(self, PACKET_BUF, buf, length);
        protobuf_set(self, PACKET_LENGTH, length);
    }

    return ret;
}

static int
Scheduler_execute_add_telescope(struct Scheduler *self)
{   
    uint16_t option;
    uint32_t length;
    char *buf;
    unsigned int type;
    size_t size;
    int ret = AAOS_OK;

    protobuf_get(self, PACKET_BUF, &buf, NULL);
    protobuf_get(self, PACKET_OPTION, &option);
    type = option;

    if ((ret = __scheduler_add_telescope(scheduler, buf, type) == AAOS_OK)) {   
        protobuf_set(self, PACKET_LENGTH, 0);
    }

    return ret;
}

static int
Scheduler_execute_delete_telescope_by_id(struct Scheduler *self)
{   
    uint64_t identifier;
    uint16_t option;
    int ret;

    protobuf_get(self, PACKET_U64F0, &identifier);
    if ((ret = __scheduler_delete_telescope_by_id(scheduler, identifier) == AAOS_OK)) {   
        protobuf_set(self, PACKET_LENGTH, 0);
    }

    return ret;
}

static int
Scheduler_execute_delete_telescope_by_name(struct Scheduler *self)
{   
    char *name;
    uint32_t length;
    int ret = AAOS_OK;

    protobuf_get(self, PACKET_LENGTH, &length);
    if (length == 0) {
        protobuf_get(self, PACKET_STR, &name);
    } else {
        protobuf_get(self, PACKET_BUF, &name, NULL);
    }

    if ((ret = __scheduler_delete_telescope_by_name(scheduler, name) == AAOS_OK)) {   
        protobuf_set(self, PACKET_LENGTH, 0);
    }

    return ret;
}

static int
Scheduler_execute_mask_telescope_by_id(struct Scheduler *self)
{   
    uint64_t identifier;
    uint16_t option;
    int ret;

    protobuf_get(self, PACKET_U64F0, &identifier);
    if ((ret = __scheduler_mask_telescope_by_id(scheduler, identifier) == AAOS_OK)) {   
        protobuf_set(self, PACKET_LENGTH, 0);
    }

    return ret;
}

static int
Scheduler_execute_mask_telescope_by_name(struct Scheduler *self)
{   
    char *name;
    uint32_t length;
    int ret = AAOS_OK;

    protobuf_get(self, PACKET_LENGTH, &length);
    if (length == 0) {
        protobuf_get(self, PACKET_STR, &name);
    } else {
        protobuf_get(self, PACKET_BUF, &name, NULL);
    }

    if ((ret = __scheduler_mask_telescope_by_name(scheduler, name) == AAOS_OK)) {   
        protobuf_set(self, PACKET_LENGTH, 0);
    }

    return ret;
}

static int
Scheduler_execute_unmask_telescope_by_id(struct Scheduler *self)
{   
    uint64_t identifier;
    uint16_t option;
    int ret;

    protobuf_get(self, PACKET_U64F0, &identifier);
    if ((ret = __scheduler_mask_telescope_by_id(scheduler, identifier) == AAOS_OK)) {   
        protobuf_set(self, PACKET_LENGTH, 0);
    }

    return ret;
}

static int
Scheduler_execute_unmask_telescope_by_name(struct Scheduler *self)
{   
    char *name;
    uint32_t length;
    int ret = AAOS_OK;

    protobuf_get(self, PACKET_LENGTH, &length);
    if (length == 0) {
        protobuf_get(self, PACKET_STR, &name);
    } else {
        protobuf_get(self, PACKET_BUF, &name, NULL);
    }

    if ((ret = __scheduler_mask_telescope_by_name(scheduler, name) == AAOS_OK)) {   
        protobuf_set(self, PACKET_LENGTH, 0);
    }

    return ret;
}

static int
Scheduler_execute_list_target(struct Scheduler *self)
{   
    uint16_t option;
    uint32_t length, nside;
    
    char *buf;
    unsigned int type;
    size_t size;
    int ret;

    protobuf_get(self, PACKET_BUF, &buf, NULL);
    protobuf_get(self, PACKET_SIZE, &size);
    if ((ret = __scheduler_list_target(scheduler, buf, size, &type)) == AAOS_OK) {
        option = type;
        length = strlen(buf) + 1;
        protobuf_set(self, PACKET_OPTION, option);
        protobuf_set(self, PACKET_BUF, buf, length);
        protobuf_set(self, PACKET_LENGTH, length);
    }

    return ret;
}

static int
Scheduler_execute_add_target(struct Scheduler *self)
{   
    uint16_t option;
    uint32_t length;
    char *buf;
    unsigned int type;
    size_t size;
    int ret = AAOS_OK;

    protobuf_get(self, PACKET_BUF, &buf, NULL);
    protobuf_get(self, PACKET_OPTION, &option);
    type = option;

    if ((ret = __scheduler_add_target(scheduler, buf, type) == AAOS_OK)) {   
        protobuf_set(self, PACKET_LENGTH, 0);
    }

    return ret;
}

static int
Scheduler_execute_delete_target_by_id(struct Scheduler *self)
{   
    uint64_t identifier;
    uint32_t nside;
    int ret;

    protobuf_get(self, PACKET_U64F0, &identifier);
    protobuf_get(self, PACKET_U32F3, &nside);
    if ((ret = __scheduler_delete_target_by_id(scheduler, identifier, nside) == AAOS_OK)) {   
        protobuf_set(self, PACKET_LENGTH, 0);
    }

    return ret;
}

static int
Scheduler_execute_delete_target_by_name(struct Scheduler *self)
{   
    char *name;
    uint32_t length;
    int ret = AAOS_OK;

    protobuf_get(self, PACKET_LENGTH, &length);
    if (length == 0) {
        protobuf_get(self, PACKET_STR, &name);
    } else {
        protobuf_get(self, PACKET_BUF, &name, NULL);
    }

    if ((ret = __scheduler_delete_target_by_name(scheduler, name) == AAOS_OK)) {   
        protobuf_set(self, PACKET_LENGTH, 0);
    }

    return ret;
}

static int
Scheduler_execute_mask_target_by_id(struct Scheduler *self)
{   
    uint64_t identifier;
    uint32_t nside;
    int ret;

    protobuf_get(self, PACKET_U64F0, &identifier);
    protobuf_get(self, PACKET_U32F3, &nside);
    if ((ret = __scheduler_mask_target_by_id(scheduler, identifier, nside) == AAOS_OK)) {   
        protobuf_set(self, PACKET_LENGTH, 0);
    }

    return ret;
}

static int
Scheduler_execute_mask_target_by_name(struct Scheduler *self)
{   
    char *name;
    uint32_t length;
    int ret = AAOS_OK;

    protobuf_get(self, PACKET_LENGTH, &length);
    if (length == 0) {
        protobuf_get(self, PACKET_STR, &name);
    } else {
        protobuf_get(self, PACKET_BUF, &name, NULL);
    }

    if ((ret = __scheduler_mask_target_by_name(scheduler, name) == AAOS_OK)) {   
        protobuf_set(self, PACKET_LENGTH, 0);
    }

    return ret;
}

static int
Scheduler_execute_unmask_target_by_id(struct Scheduler *self)
{   
    uint64_t identifier;
    uint32_t nside;
    int ret;

    protobuf_get(self, PACKET_U64F0, &identifier);
    protobuf_get(self, PACKET_U32F3, &nside);
    if ((ret = __scheduler_mask_target_by_id(scheduler, identifier, nside) == AAOS_OK)) {   
        protobuf_set(self, PACKET_LENGTH, 0);
    }

    return ret;
}

static int
Scheduler_execute_unmask_target_by_name(struct Scheduler *self)
{   
    char *name;
    uint32_t length;
    int ret = AAOS_OK;

    protobuf_get(self, PACKET_LENGTH, &length);
    if (length == 0) {
        protobuf_get(self, PACKET_STR, &name);
    } else {
        protobuf_get(self, PACKET_BUF, &name, NULL);
    }

    if ((ret = __scheduler_mask_target_by_name(scheduler, name) == AAOS_OK)) {   
        protobuf_set(self, PACKET_LENGTH, 0);
    }

    return ret;
}

static int
Scheduler_execute_add_task_record(struct Scheduler *self)
{
    char *buf;
    uint16_t option;
    unsigned int type;
    int ret = AAOS_OK, status = 0;

    protobuf_get(self, PACKET_BUF, &buf, NULL);
    protobuf_get(self, PACKET_OPTION, &option);
    type = option;

    if ((ret = __scheduler_add_task_record(scheduler, status, buf, type) == AAOS_OK)) {   
        protobuf_set(self, PACKET_LENGTH, 0);
    }

    return ret;
}

static int
Scheduler_execute_update_task_record(struct Scheduler *self)
{
    char *buf;
    uint64_t identifier;
    uint16_t option;
    unsigned int type;
    int ret = AAOS_OK;;

    protobuf_get(self, PACKET_BUF, &buf, NULL);
    protobuf_get(self, PACKET_OPTION, &option);
    protobuf_get(self, PACKET_U64F0, &identifier);
    type = option;

    if ((ret = __scheduler_update_task_record(scheduler, identifier, buf) == AAOS_OK)) {   
        protobuf_set(self, PACKET_LENGTH, 0);
    }

    return ret;
}

static int
Scheduler_execute(void *self)
{

    uint16_t command;
    protobuf_get(self, PACKET_COMMAND, &command);
    int ret;

    switch (command) {
        case SCHEDULER_GET_TASK_BY_TELESCOPE_ID:
            //ret = Scheduler_execute_get_target_by_telescope_id(self); 
            break;
        case SCHEDULER_GET_TASK_BY_TELESCOPE_NAME:
            //ret = Scheduler_execute_get_target_by_telescope_name(self); 
            break;
        case SCHEDULER_LIST_SITE:
            ret = Scheduler_execute_list_site(self);
            break;
        case SCHEDULER_ADD_SITE:
            ret = Scheduler_execute_add_site(self); 
            break;
        case SCHEDULER_DELETE_SITE_BY_ID:
            ret = Scheduler_execute_delete_site_by_id(self); 
            break;
        case SCHEDULER_MASK_SITE_BY_ID:
            ret = Scheduler_execute_mask_site_by_id(self); 
            break;
        case SCHEDULER_UNMASK_SITE_BY_ID:
            ret = Scheduler_execute_unmask_site_by_id(self); 
            break;
        case SCHEDULER_DELETE_SITE_BY_NAME:
            ret = Scheduler_execute_delete_site_by_name(self); 
            break;
        case SCHEDULER_MASK_SITE_BY_NAME:
            ret = Scheduler_execute_mask_site_by_name(self); 
            break;
        case SCHEDULER_UNMASK_SITE_BY_NAME:
            ret = Scheduler_execute_unmask_site_by_name(self); 
            break;
        case SCHEDULER_LIST_TELESCOPE:
            ret = Scheduler_execute_list_telescope(self);
            break;
        case SCHEDULER_ADD_TELESCOPE:
            ret = Scheduler_execute_add_telescope(self); 
            break;
        case SCHEDULER_DELETE_TELESCOPE_BY_ID:
            ret = Scheduler_execute_delete_telescope_by_id(self); 
            break;
        case SCHEDULER_MASK_TELESCOPE_BY_ID:
            ret = Scheduler_execute_mask_telescope_by_id(self); 
            break;
        case SCHEDULER_UNMASK_TELESCOPE_BY_ID:
            ret = Scheduler_execute_unmask_telescope_by_id(self); 
            break;
        case SCHEDULER_DELETE_TELESCOPE_BY_NAME:
            ret = Scheduler_execute_delete_telescope_by_name(self); 
            break;
        case SCHEDULER_MASK_TELESCOPE_BY_NAME:
            ret = Scheduler_execute_mask_telescope_by_name(self); 
            break;
        case SCHEDULER_UNMASK_TELESCOPE_BY_NAME:
            ret = Scheduler_execute_unmask_telescope_by_name(self); 
            break;
        case SCHEDULER_ADD_TARGET:
            ret = Scheduler_execute_add_target(self); 
            break;
        case SCHEDULER_DELETE_TARGET_BY_ID:
            ret = Scheduler_execute_delete_target_by_id(self); 
            break;
        case SCHEDULER_MASK_TARGET_BY_ID:
            ret = Scheduler_execute_mask_target_by_id(self); 
            break;
        case SCHEDULER_UNMASK_TARGET_BY_ID:
            ret = Scheduler_execute_unmask_target_by_id(self); 
            break;
        case SCHEDULER_DELETE_TARGET_BY_NAME:
            ret = Scheduler_execute_delete_target_by_name(self); 
            break;
        case SCHEDULER_MASK_TARGET_BY_NAME:
            ret = Scheduler_execute_mask_target_by_name(self); 
            break;
        case SCHEDULER_UNMASK_TARGET_BY_NAME:
            ret = Scheduler_execute_unmask_target_by_name(self); 
            break;
        case SCHEDULER_UPDATE_STATUS:
            ret = Scheduler_execute_update_status(self); 
            break;
        case SCHEDULER_ADD_TASK_RECORD:
            ret = Scheduler_execute_add_task_record(self); 
            break;
        case SCHEDULER_UPDATE_TASK_RECORD:
            ret = Scheduler_execute_update_task_record(self); 
            break;
        case SCHEDULER_POP_TASK_BLOCK:
            ret = Scheduler_execute_pop_task_block(self); 
            break;
        default:
            ret = AAOS_EBADCMD;
    }

    return ret;
}


static void *
Scheduler_ctor(void *_self, va_list *app)
{
    struct Scheduler *self = super_ctor(Scheduler(), _self, app);
    
    self->_._vtab = scheduler_virtual_table();

    return (void *) self;
}

static void *
Scheduler_dtor(void *_self)
{
    //struct RPC *self = cast(RPC(), _self);
    
    return super_dtor(Scheduler(), _self);
}

static void *
SchedulerClass_ctor(void *_self, va_list *app)
{
    struct SchedulerClass *self = super_ctor(SchedulerClass(), _self, app);
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
        
        if (selector == (Method) scheduler_update_status) {
            if (tag) {
                self->update_status.tag = tag;
                self->update_status.selector = selector;
            }
            self->update_status.method = method;
            continue;
        }
        if (selector == (Method) scheduler_get_task_by_telescope_id) {
            if (tag) {
                self->get_task_by_telescope_id.tag = tag;
                self->get_task_by_telescope_id.selector = selector;
            }
            self->get_task_by_telescope_id.method = method;
            continue;
        }
        if (selector == (Method) scheduler_get_task_by_telescope_name) {
            if (tag) {
                self->get_task_by_telescope_name.tag = tag;
                self->get_task_by_telescope_name.selector = selector;
            }
            self->get_task_by_telescope_name.method = method;
            continue;
        }
        if (selector == (Method) scheduler_list_telescope) {
            if (tag) {
                self->list_telescope.tag = tag;
                self->list_telescope.selector = selector;
            }
            self->list_telescope.method = method;
            continue;
        }
        if (selector == (Method) scheduler_add_telescope) {
            if (tag) {
                self->add_telescope.tag = tag;
                self->add_telescope.selector = selector;
            }
            self->add_telescope.method = method;
            continue;
        }
        if (selector == (Method) scheduler_delete_telescope_by_id) {
            if (tag) {
                self->delete_telescope_by_id.tag = tag;
                self->delete_telescope_by_id.selector = selector;
            }
            self->delete_telescope_by_id.method = method;
            continue;
        }
        if (selector == (Method) scheduler_mask_telescope_by_id) {
            if (tag) {
                self->mask_telescope_by_id.tag = tag;
                self->mask_telescope_by_id.selector = selector;
            }
            self->mask_telescope_by_id.method = method;
            continue;
        }
        if (selector == (Method) scheduler_unmask_telescope_by_id) {
            if (tag) {
                self->unmask_telescope_by_id.tag = tag;
                self->unmask_telescope_by_id.selector = selector;
            }
            self->unmask_telescope_by_id.method = method;
            continue;
        }
        if (selector == (Method) scheduler_delete_telescope_by_id) {
            if (tag) {
                self->delete_telescope_by_id.tag = tag;
                self->delete_telescope_by_id.selector = selector;
            }
            self->delete_telescope_by_id.method = method;
            continue;
        }
        if (selector == (Method) scheduler_mask_telescope_by_name) {
            if (tag) {
                self->mask_telescope_by_name.tag = tag;
                self->mask_telescope_by_name.selector = selector;
            }
            self->mask_telescope_by_name.method = method;
            continue;
        }
        if (selector == (Method) scheduler_unmask_telescope_by_name) {
            if (tag) {
                self->unmask_telescope_by_name.tag = tag;
                self->unmask_telescope_by_name.selector = selector;
            }
            self->unmask_telescope_by_name.method = method;
            continue;
        }
        if (selector == (Method) scheduler_add_target) {
            if (tag) {
                self->add_target.tag = tag;
                self->add_target.selector = selector;
            }
            self->add_target.method = method;
            continue;
        }
        if (selector == (Method) scheduler_delete_target_by_id) {
            if (tag) {
                self->delete_target_by_id.tag = tag;
                self->delete_target_by_id.selector = selector;
            }
            self->delete_target_by_id.method = method;
            continue;
        }
        if (selector == (Method) scheduler_mask_target_by_id) {
            if (tag) {
                self->mask_target_by_id.tag = tag;
                self->mask_target_by_id.selector = selector;
            }
            self->mask_target_by_id.method = method;
            continue;
        }
        if (selector == (Method) scheduler_unmask_target_by_id) {
            if (tag) {
                self->unmask_target_by_id.tag = tag;
                self->unmask_target_by_id.selector = selector;
            }
            self->unmask_target_by_id.method = method;
            continue;
        }
        if (selector == (Method) scheduler_delete_target_by_id) {
            if (tag) {
                self->delete_target_by_id.tag = tag;
                self->delete_target_by_id.selector = selector;
            }
            self->delete_target_by_id.method = method;
            continue;
        }
        if (selector == (Method) scheduler_mask_target_by_name) {
            if (tag) {
                self->mask_target_by_name.tag = tag;
                self->mask_target_by_name.selector = selector;
            }
            self->mask_target_by_name.method = method;
            continue;
        }
        if (selector == (Method) scheduler_unmask_target_by_name) {
            if (tag) {
                self->unmask_target_by_name.tag = tag;
                self->unmask_target_by_name.selector = selector;
            }
            self->unmask_target_by_name.method = method;
            continue;
        }
        if (selector == (Method) scheduler_add_task_record) {
            if (tag) {
                self->add_task_record.tag = tag;
                self->add_task_record.selector = selector;
            }
            self->add_task_record.method = method;
            continue;
        }
        if (selector == (Method) scheduler_update_task_record) {
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
    
    self->_.execute.method = (Method) 0;
    
    return self;
}

static void *_SchedulerClass;

static void
SchedulerClass_destroy(void)
{
    free((void *) _SchedulerClass);
}

static void
SchedulerClass_initialize(void)
{
    _SchedulerClass = new(RPCClass(), "SchedulerClass", RPCClass(), sizeof(struct SchedulerClass),
                        ctor, "ctor", SchedulerClass_ctor,
    
                        (void *) 0);
    
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(SchedulerClass_destroy);
#endif
}

const void *
SchedulerClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, SchedulerClass_initialize);
#endif
    
    return _SchedulerClass;
}

static void *_Scheduler;

static void
Scheduler_destroy(void)
{
    free((void *) _Scheduler);
}

static void
Scheduler_initialize(void)
{
    _Scheduler = new(SchedulerClass(), "Scheduler", RPC(), sizeof(struct Scheduler),
                    ctor, "ctor", Scheduler_ctor,
                    dtor, "dtor", Scheduler_dtor,
                    dtor, "dtor", Scheduler_dtor,
                    scheduler_get_task_by_telescope_id, "get_task_by_telescope_id", Scheduler_get_task_by_telescope_id,
                    scheduler_get_task_by_telescope_name, "get_task_by_telescope_name", Scheduler_get_task_by_telescope_name,
                    scheduler_update_status, "update_status", Scheduler_update_status,
                    scheduler_list_telescope, "list_telescope", Scheduler_list_telescope,
                    scheduler_add_telescope, "add_telescope", Scheduler_add_telescope,
                    scheduler_delete_telescope_by_id, "delete_telescope_by_id", Scheduler_delete_telescope_by_id,
                    scheduler_mask_telescope_by_id, "mask_telescope_by_id", Scheduler_mask_telescope_by_id,
                    scheduler_unmask_telescope_by_id, "unmask_telescope_by_id", Scheduler_unmask_telescope_by_id,
                    scheduler_delete_telescope_by_name, "delete_telescope_by_name", Scheduler_delete_telescope_by_name,
                    scheduler_mask_telescope_by_name, "mask_telescope_by_name", Scheduler_mask_telescope_by_name,
                    scheduler_unmask_telescope_by_name, "unmask_telescope_by_name", Scheduler_unmask_telescope_by_name,
                    scheduler_add_target, "add_target", Scheduler_add_target,
                    scheduler_delete_target_by_id, "delete_target_by_id", Scheduler_delete_target_by_id,
                    scheduler_mask_target_by_id, "mask_target_by_id", Scheduler_mask_target_by_id,
                    scheduler_unmask_target_by_id, "unmask_target_by_id", Scheduler_unmask_target_by_id,
                    scheduler_delete_target_by_name, "delete_target_by_name", Scheduler_delete_target_by_name,
                    scheduler_mask_target_by_name, "mask_target_by_name", Scheduler_mask_target_by_name,
                    scheduler_unmask_target_by_name, "unmask_target_by_name", Scheduler_unmask_target_by_name,
                    scheduler_add_task_record, "add_task_record", Scheduler_add_task_record,
                    scheduler_update_task_record, "update_task_record", Scheduler_update_task_record,

                    (void *) 0);
    
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(Scheduler_destroy);
#endif

}

const void *
Scheduler(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, Scheduler_initialize);
#endif
    
    return _Scheduler;
}

static const void *_scheduler_virtual_table;

static void
scheduler_virtual_table_destroy(void)
{
    delete((void *) _scheduler_virtual_table);
}

static void
scheduler_virtual_table_initialize(void)
{
    _scheduler_virtual_table = new(RPCVirtualTable(),
                                rpc_execute, "execute", Scheduler_execute,
                                (void *)0);
    
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(scheduler_virtual_table_destroy);
#endif
}

static const void *
scheduler_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, scheduler_virtual_table_initialize);
#endif
    
    return _scheduler_virtual_table;
}

/*
 * Scheduler client class
 */

static const void *scheduler_client_virtual_table(void);

static
int SchedulerClient_connect(void *_self, void **client)
{
    struct SchedulerClient *self = cast(SchedulerClient(), _self);
    
    int cfd = Tcp_connect(self->_._.address, self->_._.port, NULL, NULL);
    
    if (cfd < 0) {
        *client = NULL;
        return AAOS_ERROR;
    } else {
        *client = new(Scheduler(), cfd);
    }
    
    protobuf_set(*client, PACKET_PROTOCOL, PROTO_SCHEDULER);
    
    return AAOS_OK;
}

static void *
SchedulerClient_ctor(void *_self, va_list *app)
{
    struct SchedulerClient *self = super_ctor(SchedulerClient(), _self, app);
    
    self->_._vtab = scheduler_client_virtual_table();
    
    return (void *) self;
}

static void *
SchedulerClient_dtor(void *_self)
{
    //struct RPC *self = cast(RPC(), _self);
    
    return super_dtor(SchedulerClient(), _self);
}

static void *
SchedulerClientClass_ctor(void *_self, va_list *app)
{
    struct SchedulerClientClass *self = super_ctor(SchedulerClientClass(), _self, app);
    
    self->_.connect.method = (Method) 0;
    
    return self;
}

static void *_SchedulerClientClass;

static void
SchedulerClientClass_destroy(void)
{
    free((void *) _SchedulerClientClass);
}

static void
SchedulerClientClass_initialize(void)
{
    _SchedulerClientClass = new(RPCClientClass(), "SchedulerClientClass", RPCClientClass(), sizeof(struct SchedulerClientClass),
                                ctor, "ctor", SchedulerClientClass_ctor,
                                (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(SchedulerClientClass_destroy);
#endif
}

const void *
SchedulerClientClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, SchedulerClientClass_initialize);
#endif
    
    return _SchedulerClientClass;
}

static void *_SchedulerClient;

static void
SchedulerClient_destroy(void)
{
    free((void *) _SchedulerClient);
}

static void
SchedulerClient_initialize(void)
{
    _SchedulerClient = new(SchedulerClientClass(), "SchedulerClient", RPCClient(), sizeof(struct SchedulerClient),
                        ctor, "ctor", SchedulerClient_ctor,
                        dtor, "dtor", SchedulerClient_dtor,
                        (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(SchedulerClient_destroy);
#endif
}

const void *
SchedulerClient(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, SchedulerClient_initialize);
#endif
    
    return _SchedulerClient;
}

static const void *_scheduler_client_virtual_table;

static void
scheduler_client_virtual_table_destroy(void)
{
    delete((void *) _scheduler_client_virtual_table);
}

static void
scheduler_client_virtual_table_initialize(void)
{
    _scheduler_client_virtual_table = new(RPCClientVirtualTable(),
                                       rpc_client_connect, "connect", SchedulerClient_connect,
                                       (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(scheduler_client_virtual_table_destroy);
#endif
}

static const void *
scheduler_client_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, scheduler_client_virtual_table_initialize);
#endif
    
    return _scheduler_client_virtual_table;
}

/*
 * Scheduler server class
 */

static const void *scheduler_server_virtual_table(void);

static int
SchedulerServer_accept(void *_self, void **client)
{
    struct RPCServer *self = cast(RPCServer(), _self);
    
    int lfd, cfd;
    
    lfd = tcp_server_get_lfd(self);
    cfd = Accept(lfd, NULL, NULL);
    if (cfd < 0) {
        *client = NULL;
        return AAOS_ERROR;
    } else {
        if ((*client = new(Scheduler(), cfd)) == NULL) {
            Close(cfd);
            return AAOS_ERROR;
        }
        return AAOS_OK;
    }
}

static int
SchedulerServer_start(void *_self)
{
    struct RPCServer *self = cast(RPCServer(), _self);


    return AAOS_OK;
}

static void *
SchedulerServer_ctor(void *_self, va_list *app)
{
    struct SchedulerServer *self = super_ctor(SchedulerServer(), _self, app);
    
    self->_._vtab = scheduler_server_virtual_table();
    
    return (void *) self;
}

static void *
SchedulerServer_dtor(void *_self)
{
    //struct RPC *self = cast(RPC(), _self);
    
    return super_dtor(SchedulerServer(), _self);
}

static void *
SchedulerServerClass_ctor(void *_self, va_list *app)
{
    struct SchedulerServerClass *self = super_ctor(SchedulerServerClass(), _self, app);
    
    self->_.accept.method = (Method) 0;
    
    return self;
}

static void *_SchedulerServerClass;

static void
SchedulerServerClass_destroy(void)
{
    free((void *) _SchedulerServerClass);
}

static void
SchedulerServerClass_initialize(void)
{
    _SchedulerServerClass = new(RPCServerClass(), "SchedulerServerClass", RPCServerClass(), sizeof(struct SchedulerServerClass),
                             ctor, "ctor", SchedulerServerClass_ctor,
                             (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(SchedulerServerClass_destroy);
#endif
}

const void *
SchedulerServerClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, SchedulerServerClass_initialize);
#endif
    
    return _SchedulerServerClass;
}

static void *_SchedulerServer;

static void
SchedulerServer_destroy(void)
{
    free((void *) _SchedulerServer);
}

static void
SchedulerServer_initialize(void)
{
    _SchedulerServer = new(SchedulerServerClass(), "SchedulerServer", RPCServer(), sizeof(struct SchedulerServer),
                        ctor, "ctor", SchedulerServer_ctor,
                        dtor, "dtor", SchedulerServer_dtor,
                        (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(SchedulerServer_destroy);
#endif
}

const void *
SchedulerServer(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, SchedulerServer_initialize);
#endif
    
    return _SchedulerServer;
}

static const void *_scheduler_server_virtual_table;

static void
scheduler_server_virtual_table_destroy(void)
{
    delete((void *) _scheduler_server_virtual_table);
}

static void
scheduler_server_virtual_table_initialize(void)
{

    _scheduler_server_virtual_table = new(RPCServerVirtualTable(),
                                        rpc_server_accept, "accept", SchedulerServer_accept,
                                        (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(scheduler_server_virtual_table_destroy);
#endif
}

static const void *
scheduler_server_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, scheduler_server_virtual_table_initialize);
#endif
    
    return _scheduler_server_virtual_table;
}
