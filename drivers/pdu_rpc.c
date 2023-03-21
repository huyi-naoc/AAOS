//
//  pdu_rpc.c
//  AAOS
//
//  Created by Hu Yi on 2020/7/16.
//  Copyright © 2020 NAOC. All rights reserved.
//

#include "def.h"
#include "pdu.h"
#include "pdu_rpc.h"
#include "pdu_rpc_r.h"
#include "wrapper.h"

static int
get_index_by_name(const char *name, int *index)
{
    size_t i;
    const char *s;
    
    for (i = 0; i < n_pdu; i++) {
        s = __pdu_get_name(pdus[i]);
        if (strcmp(name, s) == 0) {
            *index = (int) i + 1;
            return AAOS_OK;
        }
    }
    return AAOS_ENOTFOUND;
}

inline static void *
get_pdu_by_index(int index)
{
    if (index > 0 && index <= n_pdu) {
        return pdus[index - 1];
    } else {
        return NULL;
    }
}

static int
get_channel_by_name(const char *name, int index, unsigned int *channel)
{
    void *pdu = get_pdu_by_index(index);
    
    if (pdu == NULL) {
        return AAOS_ENOTFOUND;
    }
    
    return __pdu_get_channel_by_name(pdu, name, channel);

}


inline static int
PDU_protocol_check(void *_self)
{
    uint16_t protocol;
    protobuf_get(_self, PACKET_PROTOCOL, &protocol);
    if (protocol != PROTO_PDU) {
        protobuf_set(_self, PACKET_ERRORCODE, AAOS_EPROTOWRONG);
        protobuf_set(_self, PACKET_LENGTH, 0);
        return AAOS_EPROTOWRONG;
    } else {
        return AAOS_OK;
    }
}

int
pdu_get_index_by_name(void *_self, const char *name)
{
    const struct PDUClass *class = (const struct PDUClass *) classOf(_self);
    
    if (isOf(class, PDUClass()) && class->get_index_by_name.method) {
        return ((int (*)(void *, const char *)) class->get_index_by_name.method)(_self, name);
    } else {
        int result;
        forward(_self, &result, (Method) pdu_get_index_by_name, "get_index_by_name", _self, name);
        return result;
    }
}

static int
PDU_get_index_by_name(void *_self, const char *name)
{
    struct PDU *self = cast(PDU(), _self);
    
    size_t length;
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_PDU);
    protobuf_set(self, PACKET_COMMAND, PDU_COMMAND_GET_INDEX_BY_NAME);
    
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
pdu_get_channel_by_name(void *_self, const char *name)
{
    const struct PDUClass *class = (const struct PDUClass *) classOf(_self);
    
    if (isOf(class, PDUClass()) && class->get_channel_by_name.method) {
        return ((int (*)(void *, const char *)) class->get_channel_by_name.method)(_self, name);
    } else {
        int result;
        forward(_self, &result, (Method) pdu_get_channel_by_name, "get_channel_by_name", _self, name);
        return result;
    }
}

static int
PDU_get_channel_by_name(void *_self, const char *name)
{
    struct PDU *self = cast(PDU(), _self);
    
    size_t length;
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_PDU);
    protobuf_set(self, PACKET_COMMAND, PDU_COMMAND_GET_CHANNEL_BY_NAME);
    
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
pdu_turn_on(void *_self)
{
    const struct PDUClass *class = (const struct PDUClass *) classOf(_self);
    
    if (isOf(class, PDUClass()) && class->turn_on.method) {
        return ((int (*)(void *)) class->turn_on.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) pdu_turn_on, "turn_on", _self);
        return result;
    }
}

static int
PDU_turn_on(void *_self)
{
    struct PDU *self = cast(PDU(), _self);
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_PDU);
    protobuf_set(self, PACKET_COMMAND, PDU_COMMAND_TURN_ON);
    protobuf_set(self, PACKET_LENGTH, 0);
    
    return rpc_call(self);
    
}

int
pdu_turn_off(void *_self)
{
    const struct PDUClass *class = (const struct PDUClass *) classOf(_self);
    
    if (isOf(class, PDUClass()) && class->turn_off.method) {
        return ((int (*)(void *)) class->turn_off.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) pdu_turn_off, "turn_off", _self);
        return result;
    }
}

static int
PDU_turn_off(void *_self)
{
    struct PDU *self = cast(PDU(), _self);
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_PDU);
    protobuf_set(self, PACKET_COMMAND, PDU_COMMAND_TURN_OFF);
    protobuf_set(self, PACKET_LENGTH, 0);
    
    return rpc_call(self);
}

int
pdu_get_current(void *_self, double *current)
{
    const struct PDUClass *class = (const struct PDUClass *) classOf(_self);
    
    if (isOf(class, PDUClass()) && class->get_current.method) {
        return ((int (*)(void *, double *)) class->get_current.method)(_self, current);
    } else {
        int result;
        forward(_self, &result, (Method) pdu_get_current, "get_current", _self, current);
        return result;
    }
}

static int
PDU_get_current(void *_self, double *current)
{
    struct PDU *self = cast(PDU(), _self);
    
    int ret;
    
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_PDU);
    protobuf_set(self, PACKET_COMMAND, PDU_COMMAND_GET_CURRENT);
    
    protobuf_set(self, PACKET_LENGTH, 0);
    
    if ((ret = rpc_call(self)) != AAOS_OK) {
        return ret;
    }
    
    protobuf_get(self, PACKET_DF0, &current);
    
    return AAOS_OK;
}

int
pdu_get_voltage(void *_self, double *voltage)
{
    const struct PDUClass *class = (const struct PDUClass *) classOf(_self);
    
    if (isOf(class, PDUClass()) && class->get_voltage.method) {
        return ((int (*)(void *, double *)) class->get_voltage.method)(_self, voltage);
    } else {
        int result;
        forward(_self, &result, (Method) pdu_get_voltage, "get_voltage", _self, voltage);
        return result;
    }
}

static int
PDU_get_voltage(void *_self, double *voltage)
{
    struct PDU *self = cast(PDU(), _self);
    
    int ret;
    
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_PDU);
    protobuf_set(self, PACKET_COMMAND, PDU_COMMAND_GET_VOLTAGE);
    
    protobuf_set(self, PACKET_LENGTH, 0);
    
    if ((ret = rpc_call(self)) != AAOS_OK) {
        return ret;
    }
    
    protobuf_get(self, PACKET_DF0, &voltage);
    
    return AAOS_OK;
}

int
pdu_get_voltage_current(void *_self, double *voltage, double *current)
{
    const struct PDUClass *class = (const struct PDUClass *) classOf(_self);
    
    if (isOf(class, PDUClass()) && class->get_voltage_current.method) {
        return ((int (*)(void *, double *, double *)) class->get_voltage_current.method)(_self, voltage, current);
    } else {
        int result;
        forward(_self, &result, (Method) pdu_get_voltage_current, "get_voltage_current", _self, voltage, current);
        return result;
    }
}

static int
PDU_get_voltage_current(void *_self, double *voltage, double *current)
{
    struct PDU *self = cast(PDU(), _self);
    
    int ret;
    
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_PDU);
    protobuf_set(self, PACKET_COMMAND, PDU_COMMAND_GET_VOLTAGE_CURRENT);
    
    protobuf_set(self, PACKET_LENGTH, 0);
    
    if ((ret = rpc_call(self)) != AAOS_OK) {
        return ret;
    }
    
    protobuf_get(self, PACKET_DF0, &voltage);
    protobuf_get(self, PACKET_DF1, &current);
    
    return AAOS_OK;
}

int
pdu_status(void *_self, unsigned char *status, size_t size)
{
    const struct PDUClass *class = (const struct PDUClass *) classOf(_self);
    
    if (isOf(class, PDUClass()) && class->status.method) {
        return ((int (*)(void *, unsigned char *, size_t)) class->status.method)(_self, status, size);
    } else {
        int result;
        forward(_self, &result, (Method) pdu_status, "status", _self, status, size);
        return result;
    }
}

static int
PDU_status(void *_self, double *status, size_t size)
{
    struct PDU *self = cast(PDU(), _self);
    
    int ret;
    uint32_t length;
    const char *buf;
    size_t n;
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_PDU);
    protobuf_set(self, PACKET_COMMAND, PDU_COMMAND_STATUS);
    protobuf_set(self, PACKET_U32F0, size);
    protobuf_set(self, PACKET_LENGTH, 0);
    
    if ((ret = rpc_call(self)) != AAOS_OK) {
        return ret;
    }
    
    protobuf_get(self, PACKET_BUF, &buf, &length);
    n = min(length, size);
    memcpy(status, buf, length);
    
    return AAOS_OK;
}

static int
PDU_execute_get_index_by_name(struct PDU *self)
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
PDU_execute_get_channel_by_name(struct PDU *self)
{
    char *name;
    int ret;
    uint16_t index;
    uint32_t length;
    const void *pdu;
    unsigned int channel;
    
    protobuf_get(self, PACKET_LENGTH, &length);
    protobuf_get(self, PACKET_INDEX, &index);
    
    if (length == 0) {
        protobuf_get(self, PACKET_STR, &name);
    } else {
        protobuf_get(self, PACKET_BUF, &name, NULL);
    }
    
    if ((pdu = get_pdu_by_index(index)) == NULL) {
        return AAOS_ENOTFOUND;
    }
    
    if ((ret = get_channel_by_name(name, index, &channel)) != AAOS_OK) {
        return ret;
    } else {
        uint16_t chan = (uint16_t) channel;
        protobuf_set(self, PACKET_CHANNEL, chan);
        protobuf_set(self, PACKET_LENGTH, 0);
    }
    
    return AAOS_OK;
}

static int
PDU_execute_turn_on(struct PDU *self)
{
    void *pdu;
    uint16_t index, channel;
   
    
    protobuf_get(self, PACKET_INDEX, &index);
    protobuf_get(self, PACKET_CHANNEL, &channel);
    
    if ((pdu = get_pdu_by_index(index)) == NULL) {
        return AAOS_ENOTFOUND;
    }
    
    return __pdu_turn_on(pdu, channel);
}

static int
PDU_execute_turn_off(struct PDU *self)
{
    void *pdu;
    uint16_t index, channel;
   
    
    protobuf_get(self, PACKET_INDEX, &index);
    protobuf_get(self, PACKET_CHANNEL, &channel);
    
    if ((pdu = get_pdu_by_index(index)) == NULL) {
        return AAOS_ENOTFOUND;
    }
    
    return __pdu_turn_off(pdu, channel);
}

static int
PDU_execute_get_current(struct PDU *self)
{
    void *pdu;
    uint16_t index, channel;
    int ret;
    double current;
    
    protobuf_get(self, PACKET_INDEX, &index);
    protobuf_get(self, PACKET_CHANNEL, &channel);
    
    if ((pdu = get_pdu_by_index(index)) == NULL) {
        return AAOS_ENOTFOUND;
    }
    
    if ((ret = __pdu_get_current(pdu, channel, &current)) != AAOS_OK) {
        return ret;
    }
    
    protobuf_set(self, PACKET_DF0, current);
    
    return AAOS_OK;
}

static int
PDU_execute_get_voltage(struct PDU *self)
{
    void *pdu;
    uint16_t index, channel;
    int ret;
    double voltage;
    
    protobuf_get(self, PACKET_INDEX, &index);
    protobuf_get(self, PACKET_CHANNEL, &channel);
    
    if ((pdu = get_pdu_by_index(index)) == NULL) {
        return AAOS_ENOTFOUND;
    }
    
    if ((ret = __pdu_get_voltage(pdu, channel, &voltage)) != AAOS_OK) {
        return ret;
    }
    
    protobuf_set(self, PACKET_DF0, voltage);
    
    return AAOS_OK;
}

static int
PDU_execute_get_voltage_current(struct PDU *self)
{
    void *pdu;
    uint16_t index, channel;
    int ret;
    double voltage, current;
    
    protobuf_get(self, PACKET_INDEX, &index);
    protobuf_get(self, PACKET_CHANNEL, &channel);
    
    if ((pdu = get_pdu_by_index(index)) == NULL) {
        return AAOS_ENOTFOUND;
    }
    
    if ((ret = __pdu_get_voltage_current(pdu, channel, &voltage, &current)) != AAOS_OK) {
        return ret;
    }
    
    protobuf_set(self, PACKET_DF0, voltage);
    protobuf_set(self, PACKET_DF0, current);
    
    return AAOS_OK;
}

static int
PDU_execute_status(struct PDU *self)
{
    void *pdu;
    uint16_t index, channel;
    uint32_t size;
    int ret;
    unsigned char *status;
    
    protobuf_get(self, PACKET_INDEX, &index);
    protobuf_get(self, PACKET_CHANNEL, &channel);
    protobuf_get(self, PACKET_U32F0, &size);
    
    if ((pdu = get_pdu_by_index(index)) == NULL) {
        return AAOS_ENOTFOUND;
    }
    
    status = (unsigned char *) Malloc(size);
    
    if (status == NULL) {
        return AAOS_ENOMEM;
    }
    
    if ((ret = __pdu_status(pdu, status, size)) != AAOS_OK) {
        free(status);
        return ret;
    }

    protobuf_set(self, PACKET_BUF, status, size);
    
    free(status);
    return AAOS_OK;
}

static int
PDU_execute_default(struct PDU *self)
{
    return AAOS_EBADCMD;
}

static int
PDU_execute(void *_self)
{
    struct PDU *self = cast(PDU(), _self);
    uint16_t command;
    int ret;
    
    if (PDU_protocol_check(self) != AAOS_OK) {
        return AAOS_EPROTOWRONG;
    }
    
    protobuf_get(self, PACKET_COMMAND, &command);
    
    switch (command) {
        case PDU_COMMAND_GET_INDEX_BY_NAME:
            ret = PDU_execute_get_index_by_name(self);
            break;
        case PDU_COMMAND_GET_CHANNEL_BY_NAME:
            ret = PDU_execute_get_channel_by_name(self);
            break;
        case PDU_COMMAND_TURN_ON:
            ret = PDU_execute_turn_on(self);
            break;
        case PDU_COMMAND_TURN_OFF:
            ret = PDU_execute_turn_off(self);
            break;
        case PDU_COMMAND_GET_VOLTAGE:
            ret = PDU_execute_get_voltage(self);
            break;
        case PDU_COMMAND_GET_CURRENT:
            ret = PDU_execute_get_current(self);
            break;
        case PDU_COMMAND_GET_VOLTAGE_CURRENT:
            ret = PDU_execute_get_voltage_current(self);
            break;
        case PDU_COMMAND_STATUS:
            ret = PDU_execute_status(self);
            break;
        default:
            return ret = PDU_execute_default(self);
            break;
    }
    
    return ret;
}

static const void *pdu_virtual_table(void);

static void *
PDU_ctor(void *_self, va_list *app)
{
    struct PDU *self = super_ctor(PDU(), _self, app);
    
    self->_._vtab = pdu_virtual_table();
    
    return (void *) self;
}

static void *
PDU_dtor(void *_self)
{
    //struct RPC *self = cast(RPC(), _self);
    
    return super_dtor(PDU(), _self);
}

static void *
PDUClass_ctor(void *_self, va_list *app)
{
    struct PDUClass *self = super_ctor(PDUClass(), _self, app);
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
        
        if (selector == (Method) pdu_get_index_by_name) {
            if (tag) {
                self->get_index_by_name.tag = tag;
                self->get_index_by_name.selector = selector;
            }
            self->get_index_by_name.method = method;
            continue;
        }
        if (selector == (Method) pdu_get_channel_by_name) {
            if (tag) {
                self->get_channel_by_name.tag = tag;
                self->get_channel_by_name.selector = selector;
            }
            self->get_channel_by_name.method = method;
            continue;
        }
        if (selector == (Method) pdu_turn_on) {
            if (tag) {
                self->turn_on.tag = tag;
                self->turn_on.selector = selector;
            }
            self->turn_on.method = method;
            continue;
        }
        if (selector == (Method) pdu_turn_off) {
            if (tag) {
                self->turn_off.tag = tag;
                self->turn_off.selector = selector;
            }
            self->turn_off.method = method;
            continue;
        }
        if (selector == (Method) pdu_get_voltage) {
            if (tag) {
                self->get_voltage.tag = tag;
                self->get_voltage.selector = selector;
            }
            self->get_voltage.method = method;
            continue;
        }
        if (selector == (Method) pdu_get_current) {
            if (tag) {
                self->get_current.tag = tag;
                self->get_current.selector = selector;
            }
            self->get_current.method = method;
            continue;
        }
        if (selector == (Method) pdu_get_voltage_current) {
            if (tag) {
                self->get_voltage_current.tag = tag;
                self->get_voltage_current.selector = selector;
            }
            self->get_voltage_current.method = method;
            continue;
        }
        if (selector == (Method) pdu_status) {
            if (tag) {
                self->status.tag = tag;
                self->status.selector = selector;
            }
            self->status.method = method;
            continue;
        }
    }
    
#ifdef va_copy
    va_end(ap);
#endif
    return (void *) self;
    
}

static void *_PDUClass;

static void
PDUClass_destroy(void)
{
    free((void *) _PDUClass);
}

static void
PDUClass_initialize(void)
{
    _PDUClass = new(RPCClass(), "PDUClass", RPCClass(), sizeof(struct PDUClass),
                    ctor, "ctor", PDUClass_ctor,
                    (void *) 0);
    
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(PDUClass_destroy);
#endif
    
}

const void *
PDUClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, PDUClass_initialize);
#endif
    
    return _PDUClass;
}

static void *_PDU;

static void
PDU_destroy(void)
{
    free((void *) _PDU);
}

static void
PDU_initialize(void)
{
    _PDU = new(PDUClass(), "PDU", RPC(), sizeof(struct PDU),
               ctor, "ctor", PDU_ctor,
               dtor, "dtor", PDU_dtor,
               pdu_get_index_by_name, "get_index_by_name", PDU_get_index_by_name,
               pdu_get_channel_by_name, "get_channel_by_name", PDU_get_channel_by_name,
               pdu_turn_on, "turn_on", PDU_turn_on,
               pdu_turn_off, "turn_off", PDU_turn_off,
               pdu_get_voltage, "get_voltage", PDU_get_voltage,
               pdu_get_current, "get_current", PDU_get_current,
               pdu_get_voltage_current, "get_voltage_current", PDU_get_voltage_current,
               pdu_status, "status", PDU_status,
               (void *) 0);
    
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(PDU_destroy);
#endif
    
}

const void *
PDU(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, PDU_initialize);
#endif
    
    return _PDU;
}

static const void *_pdu_virtual_table;

static void
pdu_virtual_table_destroy(void)
{
    delete((void *) _pdu_virtual_table);
}


static void
pdu_virtual_table_initialize(void)
{
    _pdu_virtual_table = new(RPCVirtualTable(),
                             rpc_execute, "execute", PDU_execute,
                             (void *)0);
    
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(pdu_virtual_table_destroy);
#endif
    
}

static const void *
pdu_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, pdu_virtual_table_initialize);
#endif
    
    return _pdu_virtual_table;
}

/*
 * PDU client
 */

static const void *pdu_client_virtual_table(void);

static
int PDUClient_connect(void *_self, void **client)
{
    struct PDUClient *self = cast(PDUClient(), _self);
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
    
    *client = new(PDU(), cfd);
    protobuf_set(*client, PACKET_PROTOCOL, PROTO_PDU);
    
    return ret;
}

static void *
PDUClient_ctor(void *_self, va_list *app)
{
    struct PDUClient *self = super_ctor(PDUClient(), _self, app);
    
    self->_._vtab = pdu_client_virtual_table();
    
    return (void *) self;
}

static void *
PDUClient_dtor(void *_self)
{
    //struct RPC *self = cast(RPC(), _self);
    
    return super_dtor(PDUClient(), _self);
}

static void *
PDUClientClass_ctor(void *_self, va_list *app)
{
    struct PDUClientClass *self = super_ctor(PDUClientClass(), _self, app);
    
    self->_.connect.method = (Method) 0;
    
    return self;
}

static void *_PDUClientClass;

static void
PDUClientClass_destroy(void)
{
    free((void *) _PDUClientClass);
}

static void
PDUClientClass_initialize(void)
{
    _PDUClientClass = new(RPCClientClass(), "PDUClientClass", RPCClientClass(), sizeof(struct PDUClientClass),
                          ctor, "ctor", PDUClientClass_ctor,
                          (void *) 0);
    
    
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(PDUClientClass_destroy);
#endif
    
}

const void *
PDUClientClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, PDUClientClass_initialize);
#endif
    
    return _PDUClientClass;
}

static void *_PDUClient;

static void
PDUClient_destroy(void)
{
    free((void *) _PDUClient);
}

static void
PDUClient_initialize(void)
{
    _PDUClient = new(PDUClientClass(), "PDUClient", RPCClient(), sizeof(struct PDUClient),
                           ctor, "ctor", PDUClient_ctor,
                           dtor, "dtor", PDUClient_dtor,
                           (void *) 0);
    
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(PDUClient_destroy);
#endif
}

const void *
PDUClient(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, PDUClient_initialize);
#endif
    
    return _PDUClient;
}

static const void *_pdu_client_virtual_table;

static void
pdu_client_virtual_table_destroy(void)
{
    delete((void *) _pdu_client_virtual_table);
}

static void
pdu_client_virtual_table_initialize(void)
{
    _pdu_client_virtual_table = new(RPCClientVirtualTable(),
                                    rpc_client_connect, "connect", PDUClient_connect,
                                    (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(pdu_client_virtual_table_destroy);
#endif
    
}

static const void *
pdu_client_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, pdu_client_virtual_table_initialize);
#endif
    
    return _pdu_client_virtual_table;
}

/*
* PDU server
*/

static const void *pdu_server_virtual_table(void);

static int
PDUServer_accept(void *_self, void **client)
{
    struct RPCServer *self = cast(RPCServer(), _self);
    
    int lfd, cfd;
    
    lfd = tcp_server_get_lfd(self);
    cfd = Accept(lfd, NULL, NULL);
    if (cfd < 0) {
        *client = NULL;
        return AAOS_ERROR;
    } else {
        if ((*client = new(PDU(), cfd)) == NULL) {
            Close(cfd);
            return AAOS_ERROR;
        }
        return AAOS_OK;
    }
}

static void *
PDUServer_ctor(void *_self, va_list *app)
{
    struct PDUServer *self = super_ctor(PDUServer(), _self, app);
    
    self->_._vtab = pdu_server_virtual_table();
    
    return (void *) self;
}

static void *
PDUServer_dtor(void *_self)
{
    //struct RPC *self = cast(RPC(), _self);
    
    return super_dtor(PDUServer(), _self);
}

static void *
PDUServerClass_ctor(void *_self, va_list *app)
{
    struct PDUServerClass *self = super_ctor(PDUServerClass(), _self, app);
    
    self->_.accept.method = (Method) 0;
    
    return self;
}

static void *_PDUServerClass;

static void
PDUServerClass_destroy(void)
{
    free((void *) _PDUServerClass);
}

static void
PDUServerClass_initialize(void)
{
    _PDUServerClass = new(RPCServerClass(), "PDUServerClass", RPCServerClass(), sizeof(struct PDUServerClass),
                             ctor, "ctor", PDUServerClass_ctor,
                             (void *) 0);
    
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(PDUServerClass_destroy);
#endif
}

const void *
PDUServerClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, PDUServerClass_initialize);
#endif
    
    return _PDUServerClass;
}

static void *_PDUServer;

static void
PDUServer_destroy(void)
{
    free((void *) _PDUServer);
}

static void
PDUServer_initialize(void)
{
    _PDUServer = new(PDUServerClass(), "PDUServer", RPCServer(), sizeof(struct PDUServer),
                     ctor, "ctor", PDUServer_ctor,
                     dtor, "dtor", PDUServer_dtor,
                     (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(PDUServer_destroy);
#endif
}

const void *
PDUServer(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, PDUServer_initialize);
#endif
    
    return _PDUServer;
}

static const void *_pdu_server_virtual_table;

static void
pdu_server_virtual_table_destroy(void)
{
    delete((void *) _pdu_server_virtual_table);
}

static void
pdu_server_virtual_table_initialize(void)
{
    _pdu_server_virtual_table = new(RPCServerVirtualTable(),
                                       rpc_server_accept, "accept", PDUServer_accept,
                                       (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(pdu_server_virtual_table_destroy);
#endif
}

static const void *
pdu_server_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, pdu_server_virtual_table_initialize);
#endif
    
    return _pdu_server_virtual_table;
}

#ifdef _USE_COMPILER_ATTRIBUTION_
static void __constructor__(void) __attribute__ ((constructor(_PDU_RPC_PRIORITY_)));

static void
__constructor__(void)
{
    pdu_virtual_table_initialize();
    PDUClass_initialize();
    PDU_initialize();
    pdu_client_virtual_table_initialize();
    PDUClientClass_initialize();
    PDUClient_initialize();
    pdu_server_virtual_table_initialize();
    PDUServerClass_initialize();
    PDUServer_initialize();
}

static void __destructor__(void) __attribute__ ((destructor(_PDU_RPC_PRIORITY_)));

static void
__destructor__(void)
{
    PDUServer_destroy();
    PDUServerClass_destroy();
    pdu_server_virtual_table_destroy();
    PDUClient_destroy();
    PDUClientClass_destroy();
    pdu_client_virtual_table_destroy();
    PDU_destroy();
    PDUClass_destroy();
    pdu_virtual_table_destroy();
}
#endif
