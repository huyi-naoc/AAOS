//
//  scheduler.c
//  AAOS
//
//  Created by huyi on 2024/07/18.
//  Copyright © 2024 National Astronomical Observatories, Chinese Academy of Sciences. All rights reserved.
//


#include "def.h"
#include "scheduler.h"
#include "scheduler_r.h"
#include "wrapper.h"

int
scheduler_get_task_by_id(void *_self, int identifier , char *result, size_t size, size_t *length, int *type)
{
    const struct SchedulerClass *class = (const struct SchedulerClass *) classOf(_self);
    
    if (isOf(class, SchedulerClass()) && class->get_task_by_id.method) {
        return ((int (*)(void *, int, char *, size_t, size_t *, int *)) class->get_task_by_id.method)(_self, identifier, result, size, length, type);
    } else {
        int result;
        forward(_self, &result, (Method) scheduler_get_task_by_id, "get_task_by_id", _self, identifier, result, size, type);
        return result;
    }
}

static int
Scheduler_get_task_by_id(void *_self, int identifier , char *result, size_t size, size_t *length, int *type)
{
    struct Scheduler *self = cast(Scheduler(), _self);
    char buf[BUFSIZE], *res = NULL;
    struct timespec tp;
    uint32_t len;
    uint16_t errorcode, option;

    int ret = AAOS_OK;

    Clock_gettime(CLOCK_REALTIME, &tp);

    if (type==NULL || *type == SCHEDULER_FORMAT_JSON) {
        snprintf(buf, BUFSIZE, "{\"GENERAL-INFO\":{\"operate\":\"request\",\"timestam\":%d},\"TELESCOPE-INFO\":{\"tel_id=%d\"}}", tp.tv_sec, identifier);
    } else {
        return AAOS_ENOTSUP;
    }
    len = strlen(buf);
    protobuf_set(self, PACKET_COMMAND, SCHEDULER_GET_TASK_BY_ID);
    protobuf_set(self, PACKET_BUF, buf, len);

    if ((ret = rpc_call(self)) == AAOS_OK) {
        protobuf_get(self, PACKET_ERRORCODE, &errorcode);
        if (errorcode != AAOS_OK) {
            return errorcode;
        }
        if (*type != NULL) {
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
scheduler_get_task_by_name(void *_self, const char *name, char *result, size_t size, size_t *length, int *type)
{
    const struct SchedulerClass *class = (const struct SchedulerClass *) classOf(_self);
    
    if (isOf(class, SchedulerClass()) && class->get_task_by_id.method) {
        return ((int (*)(void *, const char *, char *, size_t, size_t *, int *)) class->get_task_by_name.method)(_self, name, result, size, length, type);
    } else {
        int result;
        forward(_self, &result, (Method) scheduler_get_task_by_name, "get_task_by_name", _self, name, result, size, type);
        return result;
    }
}

static int
Scheduler_get_task_by_name(void *_self, const char *name , char *result, size_t size, size_t *length, int *type)
{
    struct Scheduler *self = cast(Scheduler(), _self);
    char buf[BUFSIZE], *res = NULL;
    struct timespec tp;
    uint32_t len;
    uint16_t errorcode, option;

    int ret = AAOS_OK;

    Clock_gettime(CLOCK_REALTIME, &tp);

    if (type==NULL || *type == SCHEDULER_FORMAT_JSON) {
        snprintf(buf, BUFSIZE, "{\"GENERAL-INFO\":{\"operate\":\"request\",\"timestam\":%d},\"TELESCOPE-INFO\":{\"telescop=%s\"}}", tp.tv_sec, name);
    } else {
        return AAOS_ENOTSUP;
    }
    len = strlen(buf);
    protobuf_set(self, PACKET_COMMAND, SCHEDULER_GET_TASK_BY_ID);
    protobuf_set(self, PACKET_BUF, buf, len);

    if ((ret = rpc_call(self)) == AAOS_OK) {
        protobuf_get(self, PACKET_ERRORCODE, &errorcode);
        if (errorcode != AAOS_OK) {
            return errorcode;
        }
        if (*type != NULL) {
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
scheduler_list_telescope(void *_self, char *result, size_t size, size_t length, int *type)
{
    const struct SchedulerClass *class = (const struct SchedulerClass *) classOf(_self);
    
    if (isOf(class, SchedulerClass()) && class->list_telescope.method) {
        return ((int (*)(void *, char *, size_t, size_t *, int *)) class->get_task_by_name.method)(_self, result, size, length, type);
    } else {
        int result;
        forward(_self, &result, (Method) scheduler_list_telescope, "list_telescope", _self, result, size, type);
        return result;
    }
}

static int
Scheduler_list_telescope(void *_self, char *result, size_t size, size_t *length, int *type)
{
    struct Scheduler *self = cast(Scheduler(), _self);
    char *res = NULL;
    uint32_t len;
    uint16_t errorcode, option;

    int ret = AAOS_OK;

    protobuf_set(self, PACKET_COMMAND, SCHEDULER_LIST_TELESCOPE);
   
    if ((ret = rpc_call(self)) == AAOS_OK) {
        protobuf_get(self, PACKET_ERRORCODE, &errorcode);
        if (errorcode != AAOS_OK) {
            return errorcode;
        }
        if (*type != NULL) {
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
scheduler_add_telescope(void *_self, ...)
{
    const struct SchedulerClass *class = (const struct SchedulerClass *) classOf(_self);

    int result;
    va_list ap;
    va_start(ap, _self);
    if (isOf(class, SchedulerClass()) && class->add_telescope.method) {
        result = ((int (*)(void *, va_list *)) class->add_telescope.method)(_self, &ap);
    } else {
        forward(_self, &result, (Method) scheduler_add_telescope, "add_telescope", _self, &ap);
        
    }
    va_end(ap);
    return result;
}

int
Scheduler_add_telescope(void *_self, va_list *app)
{
    struct Scheduler *self = cast(Scheduler(), _self);
    int type;
    uint16_t option, errorcode;
    const char *telescope;
    int ret = AAOS_OK;

    type = va_arg(*app, int);
    option = type;
    telescope = va_arg(*app, const char *);

    protobuf_set(self, PACKET_COMMAND, SCHEDULER_ADD_TELESCOPE);
    protobuf_set(self, PACKET_BUF, telescope, strlen(telescope)+1);
    protobuf_set(self, PACKET_OPTION, option);

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
scheduler_delete_telescope_by_id(void *_self, int identifier)
{
    const struct SchedulerClass *class = (const struct SchedulerClass *) classOf(_self);
    
    if (isOf(class, SchedulerClass()) && class->delete_telescope_by_id.method) {
        return ((int (*)(void *, int)) class->delete_telescope_by_id.method)(_self, identifier);
    } else {
        int result;
        forward(_self, &result, (Method) scheduler_delete_telescope_by_id, "delete_telescope_by_id", _self, identifier);
        return result;
    }
}

int
Scheduler_delete_telescope_by_id(void *_self, int identifier)
{
    struct Scheduler *self = cast(Scheduler(), _self);
   
    uint16_t errorcode;
    
    int ret = AAOS_OK;

    protobuf_set(self, PACKET_COMMAND, SCHEDULER_DELETE_TELESCOPE_BY_ID);
    protobuf_set(self, PACKET_U32F0, identifier);
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
scheduler_mask_telescope_by_id(void *_self, int identifier)
{
    const struct SchedulerClass *class = (const struct SchedulerClass *) classOf(_self);
    
    if (isOf(class, SchedulerClass()) && class->mask_telescope_by_id.method) {
        return ((int (*)(void *, int)) class->mask_telescope_by_id.method)(_self, identifier);
    } else {
        int result;
        forward(_self, &result, (Method) scheduler_mask_telescope_by_id, "mask_telescope_by_id", _self, identifier);
        return result;
    }
}

int
Scheduler_mask_telescope_by_id(void *_self, int identifier)
{
    struct Scheduler *self = cast(Scheduler(), _self);
   
    uint16_t errorcode;
    
    int ret = AAOS_OK;

    protobuf_set(self, PACKET_COMMAND, SCHEDULER_MASK_TELESCOPE_BY_ID);
    protobuf_set(self, PACKET_U32F0, identifier);
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
scheduler_unmask_telescope_by_id(void *_self, int identifier)
{
    const struct SchedulerClass *class = (const struct SchedulerClass *) classOf(_self);
    
    if (isOf(class, SchedulerClass()) && class->unmask_telescope_by_id.method) {
        return ((int (*)(void *, int)) class->unmask_telescope_by_id.method)(_self, identifier);
    } else {
        int result;
        forward(_self, &result, (Method) scheduler_unmask_telescope_by_id, "unmask_telescope_by_id", _self, identifier);
        return result;
    }
}

int
Scheduler_unmask_telescope_by_id(void *_self, int identifier)
{
    struct Scheduler *self = cast(Scheduler(), _self);
   
    uint16_t errorcode;
    
    int ret = AAOS_OK;

    protobuf_set(self, PACKET_COMMAND, SCHEDULER_UNMASK_TELESCOPE_BY_ID);
    protobuf_set(self, PACKET_U32F0, identifier);
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
scheduler_add_target(void *_self, ...)
{
    const struct SchedulerClass *class = (const struct SchedulerClass *) classOf(_self);

    int result;
    va_list ap;
    va_start(ap, _self);
    if (isOf(class, SchedulerClass()) && class->add_target.method) {
        result = ((int (*)(void *, va_list *)) class->add_target.method)(_self, &ap);
    } else {
        forward(_self, &result, (Method) scheduler_add_target, "add_target", _self, &ap);
        
    }
    va_end(ap);
    return result;
}

int
Scheduler_add_target(void *_self, va_list *app)
{
    struct Scheduler *self = cast(Scheduler(), _self);
    int type;
    uint16_t option, errorcode;
    const char *target;
    int ret = AAOS_OK;

    type = va_arg(*app, int);
    option = type;
    target = va_arg(*app, const char *);

    protobuf_set(self, PACKET_COMMAND, SCHEDULER_ADD_TARGET);
    protobuf_set(self, PACKET_BUF, target, strlen(target)+1);
    protobuf_set(self, PACKET_OPTION, option);

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
scheduler_delete_target(void *_self, ...)
{
    const struct SchedulerClass *class = (const struct SchedulerClass *) classOf(_self);

    int result;
    va_list ap;
    va_start(ap, _self);
    if (isOf(class, SchedulerClass()) && class->delete_target.method) {
        result = ((int (*)(void *, va_list *)) class->delete_target.method)(_self, &ap);
    } else {
        forward(_self, &result, (Method) scheduler_delete_target, "delete_target", _self, &ap);
        
    }
    va_end(ap);
    return result;
}

int
Scheduler_delete_target(void *_self, va_list *app)
{
    struct Scheduler *self = cast(Scheduler(), _self);
    int type;
    uint16_t option, errorcode;
    const char *target;
    int ret = AAOS_OK;

    type = va_arg(*app, int);
    option = type;
    target = va_arg(*app, const char *);

    protobuf_set(self, PACKET_COMMAND, SCHEDULER_DELETE_TARGET);
    protobuf_set(self, PACKET_BUF, target, strlen(target)+1);
    protobuf_set(self, PACKET_OPTION, option);

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
scheduler_mask_target(void *_self, ...)
{
    const struct SchedulerClass *class = (const struct SchedulerClass *) classOf(_self);

    int result;
    va_list ap;
    va_start(ap, _self);
    if (isOf(class, SchedulerClass()) && class->mask_target.method) {
        result = ((int (*)(void *, va_list *)) class->mask_target.method)(_self, &ap);
    } else {
        forward(_self, &result, (Method) scheduler_mask_target, "mask_target", _self, &ap);
        
    }
    va_end(ap);
    return result;
}

int
Scheduler_mask_target(void *_self, va_list *app)
{
    struct Scheduler *self = cast(Scheduler(), _self);
    int type;
    uint16_t option, errorcode;
    const char *target;
    int ret = AAOS_OK;

    type = va_arg(*app, int);
    option = type;
    target = va_arg(*app, const char *);

    protobuf_set(self, PACKET_COMMAND, SCHEDULER_MASK_TARGET);
    protobuf_set(self, PACKET_BUF, target, strlen(target)+1);
    protobuf_set(self, PACKET_OPTION, option);

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
scheduler_unmask_target(void *_self, ...)
{
    const struct SchedulerClass *class = (const struct SchedulerClass *) classOf(_self);

    int result;
    va_list ap;
    va_start(ap, _self);
    if (isOf(class, SchedulerClass()) && class->unmask_target.method) {
        result = ((int (*)(void *, va_list *)) class->unmask_target.method)(_self, &ap);
    } else {
        forward(_self, &result, (Method) scheduler_unmask_target, "unmask_target", _self, &ap);
        
    }
    va_end(ap);
    return result;
}

int
Scheduler_unmask_target(void *_self, va_list *app)
{
    struct Scheduler *self = cast(Scheduler(), _self);
    int type;
    uint16_t option, errorcode;
    const char *target;
    int ret = AAOS_OK;

    type = va_arg(*app, int);
    option = type;
    target = va_arg(*app, const char *);

    protobuf_set(self, PACKET_COMMAND, SCHEDULER_UNMASK_TARGET);
    protobuf_set(self, PACKET_BUF, target, strlen(target)+1);
    protobuf_set(self, PACKET_OPTION, option);

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
scheduler_delete_target_by_id(void *_self, int identifier)
{
    const struct SchedulerClass *class = (const struct SchedulerClass *) classOf(_self);
    
    if (isOf(class, SchedulerClass()) && class->delete_target_by_id.method) {
        return ((int (*)(void *, int)) class->delete_target_by_id.method)(_self, identifier);
    } else {
        int result;
        forward(_self, &result, (Method) scheduler_delete_target_by_id, "delete_target_by_id", _self, identifier);
        return result;
    }
}

int
Scheduler_delete_target_by_id(void *_self, int identifier)
{
    struct Scheduler *self = cast(Scheduler(), _self);
   
    uint16_t errorcode;
    
    int ret = AAOS_OK;

    protobuf_set(self, PACKET_COMMAND, SCHEDULER_DELETE_TARGET_BY_ID);
    protobuf_set(self, PACKET_U32F0, identifier);
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
scheduler_mask_target_by_id(void *_self, int identifier)
{
    const struct SchedulerClass *class = (const struct SchedulerClass *) classOf(_self);
    
    if (isOf(class, SchedulerClass()) && class->mask_target_by_id.method) {
        return ((int (*)(void *, int)) class->mask_target_by_id.method)(_self, identifier);
    } else {
        int result;
        forward(_self, &result, (Method) scheduler_mask_target_by_id, "mask_target_by_id", _self, identifier);
        return result;
    }
}

int
Scheduler_mask_target_by_id(void *_self, int identifier)
{
    struct Scheduler *self = cast(Scheduler(), _self);
   
    uint16_t errorcode;
    
    int ret = AAOS_OK;

    protobuf_set(self, PACKET_COMMAND, SCHEDULER_MASK_TARGET_BY_ID);
    protobuf_set(self, PACKET_U32F0, identifier);
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
scheduler_unmask_target_by_id(void *_self, int identifier)
{
    const struct SchedulerClass *class = (const struct SchedulerClass *) classOf(_self);
    
    if (isOf(class, SchedulerClass()) && class->unmask_target_by_id.method) {
        return ((int (*)(void *, int)) class->unmask_target_by_id.method)(_self, identifier);
    } else {
        int result;
        forward(_self, &result, (Method) scheduler_unmask_target_by_id, "unmask_target_by_id", _self, identifier);
        return result;
    }
}

int
Scheduler_unmask_target_by_id(void *_self, int identifier)
{
    struct Scheduler *self = cast(Scheduler(), _self);
   
    uint16_t errorcode;
    
    int ret = AAOS_OK;

    protobuf_set(self, PACKET_COMMAND, SCHEDULER_UNMASK_TARGET_BY_ID);
    protobuf_set(self, PACKET_U32F0, identifier);
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
scheduler_push_target_block(void *_self, const char *name, const char *target_block, int type)
{
    const struct SchedulerClass *class = (const struct SchedulerClass *) classOf(_self);
    
    if (isOf(class, SchedulerClass()) && class->push_task_block.method) {
        return ((int (*)(void *, const char *, const char *, int)) class->push_task_block.method)(_self, name, target_block, type);
    } else {
        int result;
        forward(_self, &result, (Method) scheduler_push_target_block, "push_target_block", _self, name, target_block, type);
        return result;
    }
}

int
Scheduler_push_target_block(void *_self, const char *name, const char *target_block, int type)
{
    struct Scheduler *self = cast(Scheduler(), _self);
   
    uint16_t errorcode, option;
    option = type;
    int ret = AAOS_OK;

    char *buf;

    protobuf_set(self, PACKET_COMMAND, SCHEDULER_PUSH_TARGET_BLOCK);
    protobuf_set(self, PACKET_OPTION, option);
    protobuf_get(self, PACKET_BUF, &buf, NULL);
    if (target_block != buf) {
        protobuf_set(self, PACKET_BUF, strlen(target_block) + 1);
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
Scheduler_execute_get_task_by_id(struct Scheduler *self)
{
    uint16_t option;

    protobuf_get(self, PACKET_OPTION, &option);
    if (option == SCHEDULER_FORMAT_JSON) {

    } else {
        return AAOS_ENOTSUP;
    }



    return AAOS_OK;
}

static int
Scheduler_execute_get_task_by_name(struct Scheduler *self)
{

    uint16_t option;

    protobuf_get(self, PACKET_OPTION, &option);
    if (option == SCHEDULER_FORMAT_JSON) {

    } else {
        return AAOS_ENOTSUP;
    }

    return AAOS_OK;
}

static int
Scheduler_execute_list_telescope(struct Scheduler *self)
{   
    return AAOS_OK;
}

static int
Scheduler_execute_add_telescope(struct Scheduler *self)
{   
    return AAOS_OK;
}

static int
Scheduler_execute_delete_telescope_by_id(struct Scheduler *self)
{   
    return AAOS_OK;
}

static int
Scheduler_execute_delete_telescope_by_name(struct Scheduler *self)
{   
    return AAOS_OK;
}

static int
Scheduler_execute_mask_telescope_by_id(struct Scheduler *self)
{   
    return AAOS_OK;
}

static int
Scheduler_execute_mask_telescope_by_name(struct Scheduler *self)
{   
    return AAOS_OK;
}

static int
Scheduler_execute_unmask_telescope_by_id(struct Scheduler *self)
{   
    return AAOS_OK;
}

static int
Scheduler_execute_unmask_telescope_by_name(struct Scheduler *self)
{   
    return AAOS_OK;
}

static int
Scheduler_execute_add_target(struct Scheduler *self)
{   
    return AAOS_OK;
}

static int
Scheduler_execute_delete_target(struct Scheduler *self)
{   
    return AAOS_OK;
}

static int
Scheduler_execute_mask_target(struct Scheduler *self)
{   
    return AAOS_OK;
}

static int
Scheduler_execute_unmask_target(struct Scheduler *self)
{   
    return AAOS_OK;
}

static int
Scheduler_execute_delete_target_by_id(struct Scheduler *self)
{   
    return AAOS_OK;
}

static int
Scheduler_execute_delete_target_by_name(struct Scheduler *self)
{   
    return AAOS_OK;
}

static int
Scheduler_execute_mask_target_by_id(struct Scheduler *self)
{   
    return AAOS_OK;
}

static int
Scheduler_execute_mask_target_by_name(struct Scheduler *self)
{   
    return AAOS_OK;
}

static int
Scheduler_execute_unmask_target_by_id(struct Scheduler *self)
{   
    return AAOS_OK;
}

static int
Scheduler_execute_unmask_target_by_name(struct Scheduler *self)
{   
    return AAOS_OK;
}

static int
Scheduler_execute_push_target_block(struct Scheduler *self)
{   
    return AAOS_OK;
}

static int
Scheduler_execute(void *self)
{

    uint16_t command;
    protobuf_get(self, PACKET_COMMAND, &command);
    int ret;

    switch (command) {
        case SCHEDULER_GET_TASK_BY_ID:
            ret = Scheduler_execute_get_target_by_id(self); 
            break;
        case SCHEDULER_GET_TASK_BY_NAME:
            ret = Scheduler_execute_get_target_by_id(self); 
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
        case SCHEDULER_DELETE_TARGET:
            ret = Scheduler_execute_delete_target(self); 
            break;
        case SCHEDULER_MASK_TARGET:
            ret = Scheduler_execute_mask_target(self); 
            break;
        case SCHEDULER_UNMASK_TARGET:
            ret = Scheduler_execute_unmask_target(self); 
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
        
        if (selector == (Method) scheduler_get_task_by_id) {
            if (tag) {
                self->get_task_by_id.tag = tag;
                self->get_task_by_id.selector = selector;
            }
            self->get_task_by_id.method = method;
            continue;
        }
        if (selector == (Method) scheduler_get_task_by_name) {
            if (tag) {
                self->get_task_by_name.tag = tag;
                self->get_task_by_name.selector = selector;
            }
            self->get_task_by_name.method = method;
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
        if (selector == (Method) Scheduler_mask_telescope_by_id) {
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
        if (selector == (Method) Scheduler_mask_telescope_by_name) {
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
        if (selector == (Method) scheduler_delete_target) {
            if (tag) {
                self->delete_target.tag = tag;
                self->delete_target.selector = selector;
            }
            self->delete_target.method = method;
            continue;
        }
        if (selector == (Method) scheduler_mask_target) {
            if (tag) {
                self->mask_target.tag = tag;
                self->mask_target.selector = selector;
            }
            self->mask_target.method = method;
            continue;
        }
        if (selector == (Method) scheduler_unmask_target) {
            if (tag) {
                self->unmask_target.tag = tag;
                self->unmask_target.selector = selector;
            }
            self->unmask_target.method = method;
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
        if (selector == (Method) Scheduler_mask_target_by_id) {
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
        if (selector == (Method) Scheduler_mask_target_by_name) {
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
                    scheduler_get_task_by_id, "get_task_by_id", Scheduler_get_task_by_id,
                    scheduler_get_task_by_name, "get_task_by_name", Scheduler_get_task_by_name,
                    scheduler_list_telescope, "list_telescope", Scheduler_list_telescope,
                    scheduler_add_telescope, "add_telescope", Scheduler_add_telescope,
                    scheduler_delete_telescope_by_id, "delete_telescope_by_id", Scheduler_delete_telescope_by_id,
                    scheduler_mask_telescope_by_id, "mask_telescope_by_id", Scheduler_mask_telescope_by_id,
                    scheduler_unmask_telescope_by_id, "unmask_telescope_by_id", Scheduler_unmask_telescope_by_id,
                    scheduler_delete_telescope_by_name, "delete_telescope_by_name", Scheduler_delete_telescope_by_name,
                    scheduler_mask_telescope_by_name, "mask_telescope_by_name", Scheduler_mask_telescope_by_name,
                    scheduler_unmask_telescope_by_name, "unmask_telescope_by_name", Scheduler_unmask_telescope_by_name,
                    scheduler_add_target, "add_target", Scheduler_add_target,
                    scheduler_delete_target, "delete_target", Scheduler_delete_target,
                    scheduler_mask_target, "mask_target", Scheduler_mask_target,
                    scheduler_unmask_target, "unmask_target", Scheduler_unmask_target,
                    scheduler_delete_target_by_id, "delete_target_by_id", Scheduler_delete_target_by_id,
                    scheduler_mask_target_by_id, "mask_target_by_id", Scheduler_mask_target_by_id,
                    scheduler_unmask_target_by_id, "unmask_target_by_id", Scheduler_unmask_target_by_id,
                    scheduler_delete_target_by_name, "delete_target_by_name", Scheduler_delete_target_by_name,
                    scheduler_mask_target_by_name, "mask_target_by_name", Scheduler_mask_target_by_name,
                    scheduler_unmask_target_by_name, "unmask_target_by_name", Scheduler_unmask_target_by_name,
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
