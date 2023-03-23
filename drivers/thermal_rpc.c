//
//  thermal_rpc.c
//  AAOS
//
//  Created by Hu Yi on 2023/3/22.
//  Copyright © 2023 NAOC. All rights reserved.
//

#include "def.h"
#include "thermal_def.h"
#include "thermal.h"
#include "thermal_rpc.h"
#include "thermal_rpc_r.h"
#include "wrapper.h"

void **units;
size_t n_unit;

inline static int
get_index_by_name(const char *name, int *index)
{
    size_t i;
    const char *s;
    
    for (i = 0; i < n_unit; i++) {
        s = __thermal_unit_get_name(units[i]);
        if (strcmp(name, s) == 0) {
            *index = (int) i + 1;
            return AAOS_OK;
        }
    }
    return AAOS_ENOTFOUND;
}

inline static void *
get_thermal_unit_by_index(int index)
{
    if (index > 0 && index <= n_unit) {
        return units[index - 1];
    } else {
        return NULL;
    }
}

inline static int
ThermalUnit_protocol_check(void *_self)
{
    uint16_t protocol;
    protobuf_get(_self, PACKET_PROTOCOL, &protocol);
    if (protocol != PROTO_TELESCOPE) {
        protobuf_set(_self, PACKET_ERRORCODE, AAOS_EPROTOWRONG);
        protobuf_set(_self, PACKET_LENGTH, 0);
        return AAOS_EPROTOWRONG;
    } else {
        return AAOS_OK;
    }
}

int
thermal_unit_get_index_by_name(void *_self, const char *name)
{
    const struct ThermalUnitClass *class = (const struct ThermalUnitClass *) classOf(_self);
    
    if (isOf(class, ThermalUnitClass()) && class->get_index_by_name.method) {
        return ((int (*)(void *, const char *)) class->get_index_by_name.method)(_self, name);
    } else {
        int result;
        forward(_self, &result, (Method) thermal_unit_get_index_by_name, "get_index_by_name", _self, name);
        return result;
    }
}

static int
ThermalUnit_get_index_by_name(void *_self, const char *name)
{
    struct ThermalUnit *self = cast(ThermalUnit(), _self);
    
    size_t length;
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_THERMAL);
    protobuf_set(self, PACKET_COMMAND, THERMAL_UNIT_COMMAND_GET_INDEX_BY_NAME);
    
    length = strlen(name);
    if (length < PACKETPARAMETERSIZE) {
        char *s;
        protobuf_get(self, PACKET_STR, &s);
        if (s != name) {
            snprintf(s, PACKETPARAMETERSIZE, "%s", name);
        }
        protobuf_set(self, PACKET_LENGTH, 0);
    } else {
        char *buf;
        protobuf_get(self, PACKET_BUF, &buf, NULL);
        if (name != buf) {
            protobuf_set(self, PACKET_BUF, name, length + 1);
        }
        uint32_t len = (uint32_t) length + 1;
        protobuf_set(self, PACKET_LENGTH, len);
    }
    
    return rpc_call(self);
}

int
thermal_unit_status(void *_self, void *res, size_t res_size, size_t *res_len)
{
    const struct ThermalUnitClass *class = (const struct ThermalUnitClass *) classOf(_self);
    
    if (isOf(class, ThermalUnitClass()) && class->status.method) {
        return ((int (*)(void *, char *, size_t, size_t *)) class->status.method)(_self, res, res_size, res_len);
    } else {
        int result;
        forward(_self, &result, (Method) thermal_unit_status, "status", _self, res, res_size, res_len);
        return result;
    }
}

static int
ThermalUnit_status(void *_self, char *res, size_t res_size, size_t *res_len)
{
    struct ThermalUnit *self = cast(ThermalUnit(), _self);
    int ret;
    
    if (res == NULL) {
        return -1 * AAOS_EINVAL;
    }
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_TELESCOPE);
    protobuf_set(self, PACKET_COMMAND, THERMAL_UNIT_COMMAND_STATUS);
    
    if ((ret = rpc_call(self)) == AAOS_OK) {
        uint32_t length;
        protobuf_get(self, PACKET_LENGTH, &length);
        if (length == 0) {
            char *s;
            protobuf_get(self, PACKET_STR, &s);
            if (s != res) {
                snprintf(res, res_size, "%s", s);
                if (res_len != NULL) {
                    *res_len = strlen(res) + 1;
                }
            }
        } else {
            char *buf;
            protobuf_get(self, PACKET_BUF, &buf, NULL);
            if (buf != res) {
                snprintf(res, res_size, "%s", buf);
            }
            if (res_len != NULL) {
                *res_len = strlen(res) + 1;
            }
        }
    }
    
    return ret;
}

int
thermal_unit_info(void *_self, FILE *fp)
{
    const struct ThermalUnitClass *class = (const struct ThermalUnitClass *) classOf(_self);
    
    if (isOf(class, ThermalUnitClass()) && class->info.method) {
        return ((int (*)(void *, FILE *fp)) class->info.method)(_self, fp);
    } else {
        int result;
        forward(_self, &result, (Method) thermal_unit_info, "info", _self, fp);
        return result;
    }
}

static int
ThermalUnit_info(void *_self, FILE *fp)
{
    struct ThermalUnit *self = cast(ThermalUnit(), _self);
    
    int ret;
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_THERMAL);
    protobuf_set(self, PACKET_COMMAND, THERMAL_UNIT_COMMAND_INFO);
    
    if ((ret = rpc_call(self)) == AAOS_OK) {
        size_t length;
        uint16_t status;
        void *buf;
        protobuf_get(self, PACKET_BUF, &buf, &length);
        
        size_t i;
        for (i = 0; i < length/sizeof(uint16_t); i++) {
            memcpy(&status, (char *) buf + i * sizeof(uint16_t), sizeof(uint16_t));
            switch (status) {
                case THERMAL_UNIT_STATE_ON:
                    fprintf(fp, "%02zd thermal unit: on\n", i + 1);
                    break;
                case THERMAL_UNIT_STATE_OFF:
                    fprintf(fp, "%02zd thermal unit: off\n", i + 1);
                    break;
                default:
                    break;
            }
        }
    }
    
    return ret;
}


/*
 * Virtual functions
 */

static int
ThermalUnit_execute_get_index_by_name(struct ThermalUnit *self)
{
    char *name;
    int index, ret;
    uint32_t length;
    
    protobuf_get(self, PACKET_LENGTH, &length);
    
    if (length == 0) {
        protobuf_get(self, PACKET_STR, &name);
    } else {
        protobuf_get(self, PACKET_BUF, &name, NULL);
    }
    
    if ((ret = get_index_by_name(name, &index)) != AAOS_OK) {
        return ret;
    } else {
        uint16_t idx = (uint16_t) index;
        protobuf_set(self, PACKET_INDEX, idx);
        protobuf_set(self, PACKET_LENGTH, 0);
    }
    
    return AAOS_OK;
}

static int
ThermalUnit_execute_status(struct ThermalUnit *self)
{
    int ret;
    uint16_t index;
    uint32_t length;
    void *unit;
    void *buf;
    size_t payload;
    size_t len;
    
    protobuf_get(self, PACKET_LENGTH, &length);
    protobuf_get(self, PACKET_INDEX, &index);
    
    if (index == 0) {
        char *name;
        int idx;
        if (length == 0) {
            protobuf_get(self, PACKET_STR, &name);
        } else {
            protobuf_get(self, PACKET_BUF, &name, NULL);
        }
        if ((ret = get_index_by_name(name, &idx)) != AAOS_OK) {
            return ret;
        }
        index = (int) idx;
        protobuf_set(self, PACKET_INDEX, index);
    }
    
    if ((unit = get_thermal_unit_by_index(index)) == NULL) {
        return AAOS_ENOTFOUND;
    }
    
    protobuf_get(self, PACKET_BUF, &buf, NULL);
    payload = protobuf_payload(self);
    
    
    ret = __thermal_unit_status(unit, buf, payload, &len);

    if (ret != AAOS_OK) {
        protobuf_set(self, PACKET_LENGTH, 0);
    } else {
        length = (uint32_t) len;
        protobuf_set(self, PACKET_LENGTH, length);
    }
    
    return ret;
}

static int
ThermalUnit_execute_info(struct ThermalUnit *self)
{
    int ret = AAOS_OK;
    uint16_t index;
    uint32_t length;
    void *unit;
    
    protobuf_get(self, PACKET_LENGTH, &length);
    protobuf_get(self, PACKET_INDEX, &index);
    
    if (index == 0) {
        char *name;
        int idx;
        if (length == 0) {
            protobuf_get(self, PACKET_STR, &name);
        } else {
            protobuf_get(self, PACKET_BUF, &name, NULL);
        }
        if ((ret = get_index_by_name(name, &idx)) != AAOS_OK) {
            return ret;
        }
        index = (int) idx;
        protobuf_set(self, PACKET_INDEX, index);
    }
    
    if ((unit = get_thermal_unit_by_index(index)) == NULL) {
        return AAOS_ENOTFOUND;
    }
    
    /*
    ret = __thermal_unit_power_on(thermal_unit);
    protobuf_set(self, PACKET_LENGTH, 0);
     */
    
    return ret;
}



static int
ThermalUnit_execute_default(struct ThermalUnit *self)
{
    return AAOS_EBADCMD;
}

static int
ThermalUnit_execute(void *_self)
{
    struct ThermalUnit *self = cast(ThermalUnit(), _self);
    uint16_t command;
    int ret;
    
    if (ThermalUnit_protocol_check(self) != AAOS_OK) {
        return AAOS_EPROTOWRONG;
    }
    
    protobuf_get(self, PACKET_COMMAND, &command);
    
    switch (command) {
        case THERMAL_UNIT_COMMAND_GET_INDEX_BY_NAME:
            ret = ThermalUnit_execute_get_index_by_name(self);
            break;
        case THERMAL_UNIT_COMMAND_STATUS:
            ret = ThermalUnit_execute_status(self);
            break;
        case THERMAL_UNIT_COMMAND_INFO:
            ret = ThermalUnit_execute_info(self);
            break;
        
        default:
            return ThermalUnit_execute_default(self);
            break;
    }
    
    return ret;
}

static const void *thermal_unit_virtual_table(void);

static void *
ThermalUnit_ctor(void *_self, va_list *app)
{
    struct ThermalUnit *self = super_ctor(ThermalUnit(), _self, app);
    
    self->_._vtab = thermal_unit_virtual_table();
    
    return (void *) self;
}

static void *
ThermalUnit_dtor(void *_self)
{
    //struct RPC *self = cast(RPC(), _self);
    
    return super_dtor(ThermalUnit(), _self);
}

static void *
ThermalUnitClass_ctor(void *_self, va_list *app)
{
    struct ThermalUnitClass *self = super_ctor(ThermalUnitClass(), _self, app);
    Method selector;
    
    self->_.execute.method = (Method) 0;
    
#ifdef va_copy
    va_list ap;
    va_copy(ap, *app);
#else
    va_list ap = *app;
#endif
    
    while ((selector = va_arg(ap, Method))) {
        const char *tag = va_arg(ap, const char *);
        Method method = va_arg(ap, Method);
        
        if (selector == (Method) thermal_unit_status) {
            if (tag) {
                self->status.tag = tag;
                self->status.selector = selector;
            }
            self->status.method = method;
            continue;
        }
        if (selector == (Method) thermal_unit_get_index_by_name) {
            if (tag) {
                self->get_index_by_name.tag = tag;
                self->get_index_by_name.selector = selector;
            }
            self->get_index_by_name.method = method;
            continue;
        }
        if (selector == (Method) thermal_unit_info) {
            if (tag) {
                self->info.tag = tag;
                self->info.selector = selector;
            }
            self->info.method = method;
            continue;
        }
    }
    
#ifdef va_copy
    va_end(ap);
#endif
    return (void *) self;
    
}

static void *_ThermalUnitClass;

static void
ThermalUnitClass_destroy(void)
{
    free((void *) _ThermalUnitClass);
}

static void
ThermalUnitClass_initialize(void)
{
    _ThermalUnitClass = new(RPCClass(), "ThermalUnitClass", RPCClass(), sizeof(struct ThermalUnitClass),
                            ctor, "ctor", ThermalUnitClass_ctor,
                            (void *) 0);
    
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(ThermalUnitClass_destroy);
#endif
    
}

const void *
ThermalUnitClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, ThermalUnitClass_initialize);
#endif
    
    return _ThermalUnitClass;
}

static void *_ThermalUnit;

static void
ThermalUnit_destroy(void)
{
    free((void *) _ThermalUnit);
}

static void
ThermalUnit_initialize(void)
{
    _ThermalUnit= new(ThermalUnitClass(), "ThermalUnit", RPC(), sizeof(struct ThermalUnit),
                      ctor, "ctor", ThermalUnit_ctor,
                      dtor, "dtor", ThermalUnit_dtor,
                      thermal_unit_get_index_by_name, "get_index_by_name", ThermalUnit_get_index_by_name,
                      thermal_unit_info, "info", ThermalUnit_info,
                      thermal_unit_status, "status", ThermalUnit_status,
                      (void *) 0);
    
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(ThermalUnit_destroy);
#endif
    
}

const void *
ThermalUnit(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, ThermalUnit_initialize);
#endif
    
    return _ThermalUnit;
}

static const void *_thermal_unit_virtual_table;

static void
thermal_unit_virtual_table_destroy(void)
{
    delete((void *) _thermal_unit_virtual_table);
}

static void
thermal_unit_virtual_table_initialize(void)
{
    _thermal_unit_virtual_table = new(RPCVirtualTable(),
                                      rpc_execute, "execute", ThermalUnit_execute,
                                      (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(thermal_unit_virtual_table_destroy);
#endif
    
}

static const void *
thermal_unit_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, thermal_unit_virtual_table_initialize);
#endif
    
    return _thermal_unit_virtual_table;
}

/*
 * ThermalUnit client class
 */

static const void *thermal_unit_client_virtual_table(void);

static
int ThermalUnitClient_connect(void *_self, void **client)
{
    struct ThermalUnitClient *self = cast(ThermalUnitClient(), _self);
    int ret = AAOS_OK;
    
    int cfd = Tcp_connect(self->_._.address, self->_._.port, NULL, NULL);
    
    if (cfd < 0) {
        switch (errno) {
            case ECONNREFUSED:
                ret = AAOS_ECONNREFUSED;
                break;
            case ENETUNREACH:
                ret = AAOS_ENETUNREACH;
                break;
            case ETIMEDOUT:
                ret = AAOS_ETIMEDOUT;
                break;
            default:
                ret = AAOS_ERROR;
                break;
        }
    }
    
    *client = new(ThermalUnit(), cfd);
    protobuf_set(*client, PACKET_PROTOCOL, PROTO_THERMAL);
    
    return ret;
}

static void *
ThermalUnitClient_ctor(void *_self, va_list *app)
{
    struct ThermalUnitClient *self = super_ctor(ThermalUnitClient(), _self, app);
    
    self->_._vtab = thermal_unit_client_virtual_table();
    
    return (void *) self;
}

static void *
ThermalUnitClient_dtor(void *_self)
{
    //struct RPC *self = cast(RPC(), _self);
    
    return super_dtor(ThermalUnitClient(), _self);
}

static void *
ThermalUnitClientClass_ctor(void *_self, va_list *app)
{
    struct ThermalUnitClientClass *self = super_ctor(ThermalUnitClientClass(), _self, app);
    
    self->_.connect.method = (Method) 0;
    
    return self;
}

static void *_ThermalUnitClientClass;

static void
ThermalUnitClientClass_destroy(void)
{
    free((void *) _ThermalUnitClientClass);
}

static void
ThermalUnitClientClass_initialize(void)
{
    _ThermalUnitClientClass = new(RPCClientClass(), "ThermalUnitClientClass", RPCClientClass(), sizeof(struct ThermalUnitClientClass),
                                  ctor, "ctor", ThermalUnitClientClass_ctor,
                                  (void *) 0);
    
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(ThermalUnitClientClass_destroy);
#endif
    
}

const void *
ThermalUnitClientClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, ThermalUnitClientClass_initialize);
#endif
    
    return _ThermalUnitClientClass;
}

static void *_ThermalUnitClient;

static void
ThermalUnitClient_destroy(void)
{
    free((void *) _ThermalUnitClient);
}

static void
ThermalUnitClient_initialize(void)
{
    _ThermalUnitClient = new(ThermalUnitClientClass(), "ThermalUnitClient", RPCClient(), sizeof(struct ThermalUnitClient),
                             ctor, "ctor", ThermalUnitClient_ctor,
                             dtor, "dtor", ThermalUnitClient_dtor,
                             (void *) 0);
    
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(ThermalUnitClient_destroy);
#endif
}

const void *
ThermalUnitClient(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, ThermalUnitClient_initialize);
#endif
    
    return _ThermalUnitClient;
}

static const void *_thermal_unit_client_virtual_table;

static void
thermal_unit_client_virtual_table_destroy(void)
{
    delete((void *) _thermal_unit_client_virtual_table);
}

static void
thermal_unit_client_virtual_table_initialize(void)
{
    _thermal_unit_client_virtual_table = new(RPCClientVirtualTable(),
                                             rpc_client_connect, "connect", ThermalUnitClient_connect,
                                             (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(thermal_unit_client_virtual_table_destroy);
#endif
    
}

static const void *
thermal_unit_client_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, thermal_unit_client_virtual_table_initialize);
#endif
    
    return _thermal_unit_client_virtual_table;
}

/*
 * ThermalUnit server class
 */

static const void *thermal_unit_server_virtual_table(void);

static int
ThermalUnitServer_accept(void *_self, void **client)
{
    struct RPCServer *self = cast(RPCServer(), _self);
    
    int lfd, cfd;
    
    lfd = tcp_server_get_lfd(self);
    cfd = Accept(lfd, NULL, NULL);
    if (cfd < 0) {
        *client = NULL;
        return AAOS_ERROR;
    } else {
        if ((*client = new(ThermalUnit(), cfd)) == NULL) {
            Close(cfd);
            return AAOS_ERROR;
        }
        return AAOS_OK;
    }
}

static void *
ThermalUnitServer_ctor(void *_self, va_list *app)
{
    struct ThermalUnitServer *self = super_ctor(ThermalUnitServer(), _self, app);
    
    self->_._vtab = thermal_unit_server_virtual_table();
    
    return (void *) self;
}

static void *
ThermalUnitServer_dtor(void *_self)
{
    //struct RPC *self = cast(RPC(), _self);
    
    return super_dtor(ThermalUnitServer(), _self);
}

static void *
ThermalUnitServerClass_ctor(void *_self, va_list *app)
{
    struct ThermalUnitServerClass *self = super_ctor(ThermalUnitServerClass(), _self, app);
    
    self->_.accept.method = (Method) 0;
    
    return self;
}

static void *_ThermalUnitServerClass;

static void
ThermalUnitServerClass_destroy(void)
{
    free((void *) _ThermalUnitServerClass);
}

static void
ThermalUnitServerClass_initialize(void)
{
    _ThermalUnitServerClass = new(RPCServerClass(), "ThermalUnitServerClass", RPCServerClass(), sizeof(struct ThermalUnitServerClass),
                             ctor, "ctor", ThermalUnitServerClass_ctor,
                             (void *) 0);
    
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(ThermalUnitServerClass_destroy);
#endif
}

const void *
ThermalUnitServerClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, ThermalUnitServerClass_initialize);
#endif
    
    return _ThermalUnitServerClass;
}

static void *_ThermalUnitServer;

static void
ThermalUnitServer_destroy(void)
{
    free((void *) _ThermalUnitServer);
}

static void
ThermalUnitServer_initialize(void)
{
    _ThermalUnitServer = new(ThermalUnitServerClass(), "ThermalUnitServer", RPCServer(), sizeof(struct ThermalUnitServer),
                        ctor, "ctor", ThermalUnitServer_ctor,
                        dtor, "dtor", ThermalUnitServer_dtor,
                        (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(ThermalUnitServer_destroy);
#endif
}

const void *
ThermalUnitServer(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, ThermalUnitServer_initialize);
#endif
    
    return _ThermalUnitServer;
}

static const void *_thermal_unit_server_virtual_table;

static void
thermal_unit_server_virtual_table_destroy(void)
{
    delete((void *) _thermal_unit_server_virtual_table);
}

static void
thermal_unit_server_virtual_table_initialize(void)
{
    _thermal_unit_server_virtual_table = new(RPCServerVirtualTable(),
                                       rpc_server_accept, "accept", ThermalUnitServer_accept,
                                       (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(thermal_unit_server_virtual_table_destroy);
#endif
}

static const void *
thermal_unit_server_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, thermal_unit_server_virtual_table_initialize);
#endif
    
    return _thermal_unit_server_virtual_table;
}

#ifdef _USE_COMPILER_ATTRIBUTION_

static void __destructor__(void) __attribute__ ((destructor(_TELESCOPE_RPC_PRIORITY_)));

static void
__destructor__(void)
{
    ThermalUnitServer_destroy();
    ThermalUnitServerClass_destroy();
    thermal_unit_server_virtual_table_destroy();
    ThermalUnitClient_destroy();
    ThermalUnitClientClass_destroy();
    thermal_unit_client_virtual_table_destroy();
    ThermalUnit_destroy();
    ThermalUnitClass_destroy();
    thermal_unit_virtual_table_destroy();
}

static void __constructor__(void) __attribute__ ((constructor(_TELESCOPE_RPC_PRIORITY_)));

static void
__constructor__(void)
{
    thermal_unit_virtual_table_initialize();
    ThermalUnitClass_initialize();
    ThermalUnit_initialize();
    thermal_unit_client_virtual_table_initialize();
    ThermalUnitClientClass_initialize();
    ThermalUnitClient_initialize();
    thermal_unit_server_virtual_table_initialize();
    ThermalUnitServerClass_initialize();
    ThermalUnitServer_initialize();
}
#endif
