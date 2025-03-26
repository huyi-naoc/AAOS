//
//  rpc.c
//  AAOS
//
//  Created by huyi on 2018/11/8.
//  Copyright © 2018年 National Astronomical Observatories, Chinese Academy of Sciences. All rights reserved.
//

#include "def.h"
#include "rpc_r.h"
#include "rpc.h"
#include "wrapper.h"

#ifndef DEFAULTPACKETSIZE
#define DEFAULTPACKETSIZE 992
#endif

void
rpc_diagnose(int error_code, FILE *fp)
{
    switch (error_code) {
        case AAOS_EINTR:
            fprintf(fp, "rpc is interupted by a signal.");
            break;
        case AAOS_ECONNRESET:
            fprintf(fp, "the connection is reset during rpc.");
            break;
        case AAOS_EPIPE:
            fprintf(fp, "the peer process aborted or exit during rpc.");
            break;
        default:
            break;
    }
}

/*
 * RPC virtual table.
 */
 
static void *
RPCVirtualTable_ctor(void *_self, va_list *app)
{
    struct RPCVirtualTable *self = super_ctor(RPCVirtualTable(), _self, app);
    Method selector;
    
    while ((selector = va_arg(*app, Method))) {
        const char *tag = va_arg(*app, const char *);
        Method method = va_arg(*app, Method);
        if (selector == (Method) rpc_call) {
            if (tag) {
                self->call.tag = tag;
                self->call.selector = selector;
            }
            self->call.method = method;
            continue;
        }
        if (selector == (Method) rpc_execute) {
            if (tag) {
                self->execute.tag = tag;
                self->execute.selector = selector;
            }
            self->execute.method = method;
            continue;
        }
        if (selector == (Method) rpc_process) {
            if (tag) {
                self->process.tag = tag;
                self->process.selector = selector;
            }
            self->process.method = method;
            continue;
        }
        if (selector == (Method) rpc_inspect) {
            if (tag) {
                self->inspect.tag = tag;
                self->inspect.selector = selector;
            }
            self->inspect.method = method;
            continue;
        }
        if (selector == (Method) rpc_register) {
            if (tag) {
                self->reg.tag = tag;
                self->reg.selector = selector;
            }
            self->reg.method = method;
            continue;
        }
    }
    
    return _self;
}

static void *
RPCVirtualTable_dtor(void *_self)
{
    return super_dtor(Object(), _self);
}

static const void *_RPCVirtualTable;

static void
RPCVirtualTable_destroy(void)
{
    free((void *) _RPCVirtualTable);
}

static void
RPCVirtualTable_initialize(void)
{
    _RPCVirtualTable = new(VirtualTableClass(), "RPCVirtualTable", VirtualTable(), sizeof(struct RPCVirtualTable),
                           ctor, "ctor", RPCVirtualTable_ctor,
                           dtor, "dtor", RPCVirtualTable_dtor,
                           (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(RPCVirtualTable_destroy);
#endif
}

const void *
RPCVirtualTable(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, RPCVirtualTable_initialize);
#endif
    return _RPCVirtualTable;
}

/*
 * RPC class.
 */

int
rpc_process(void *_self)
{
    struct RPCClass *class = (struct RPCClass *) classOf(_self);
    
    if (isOf(class, RPCClass()) && class->process.method) {
        return ((int (*)(void *)) class->process.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) rpc_process, "process", _self);
        return result;
    }
    return AAOS_OK;
}

/*
 * return AAOS_OK if there is no networking problem.
 * if properly set errorcode according to rpc_execute return value.
 * positive return value means error happen when excecuting rpc_execute
 * negative return value means networking error.
 */

static int
RPC_process(void *_self)
{
    struct RPC *self = cast(RPC(), _self);
    uint32_t length;
    int ret;
    size_t payload;
    void *buf, *header;
    
    /*
     * Read header.
     */
    header = protobuf_header(self);
    if ((ret = tcp_socket_read(self, header, PACKETHEADERSIZE, NULL)) != AAOS_OK) {
        return -1 * ret;
    }
    
    protobuf_get(self, PACKET_LENGTH, &length);

    /*
     * if any payload, read it.
     */
   
    if (length != 0) {
        payload = protobuf_payload(self);
        if (payload < length) {
            if ((ret = protobuf_reallocate(self, payload)) != AAOS_OK) {
                protobuf_set(self, PACKET_LENGTH, 0);
                protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOMEM);
                tcp_socket_write(self, header, PACKETHEADERSIZE, NULL);
                return ret;
            }
            header = protobuf_header(self);
        }
        protobuf_get(self, PACKET_BUF, &buf, NULL);
        if ((ret = tcp_socket_read(self, buf, (size_t) length, NULL)) != AAOS_OK) {
            return ret;
        }
    }
    
    /*
     * call virtual execute function.
     * if rpc_execute failed, tell the RPC caller executing error, return AAOS_OK;
     */
    uint16_t cmd;
    protobuf_get(self, PACKET_COMMAND, &cmd);

    if ((ret = rpc_execute(self)) != AAOS_OK) {
        if (ret < 0) {
            ret = -1 * ret;
        }
        uint16_t errorcode = (uint16_t) ret;
        protobuf_set(self, PACKET_ERRORCODE, errorcode);
        protobuf_set(self, PACKET_LENGTH, 0);
        tcp_socket_write(self, header, PACKETHEADERSIZE, NULL);
        return AAOS_OK;
    }
    /*
     * return result to the caller
     */
    protobuf_set(self, PACKET_ERRORCODE, AAOS_OK);
    header = protobuf_header(self);
    protobuf_get(self, PACKET_LENGTH, &length);
    if ((ret = tcp_socket_write(self, header, (size_t) length + PACKETHEADERSIZE, NULL)) != AAOS_OK) {
        return -1 * ret;
    }

    return ret;
}

int
rpc_execute(void *_self)
{
    struct RPCClass *class = (struct RPCClass *) classOf(_self);
    
    if (isOf(class, RPCClass()) && class->execute.method) {
        return ((int (*)(void *)) class->execute.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) rpc_execute, "execute", _self);
        return result;
    }
}

static int
RPC_execute(void *_self)
{
    char *s;
    size_t size;
    protobuf_get(_self, PACKET_BUF, &s, &size);
    printf("%s\n", s);
    char c = s[0];
    memset(s, c, size - 1);
    
    return AAOS_OK;
}

int
rpc_read(void *_self)
{
    struct RPCClass *class = (struct RPCClass *) classOf(_self);
    
    if (isOf(class, RPCClass()) && class->read.method) {
        return ((int (*)(void *)) class->read.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) rpc_read, "read", _self);
        return result;
    }
}

static int
RPC_read(void *_self)
{
    struct RPC *self = cast(RPC(), _self);
    
    void *protobuf = self->protobuf, *header, *buf;
    int ret;
    uint32_t length;
    size_t payload;
    uint16_t errorcode;
    
    header = protobuf_header(protobuf);
    if ((ret = tcp_socket_read(self, header, PACKETHEADERSIZE, NULL)) != AAOS_OK) {
        return -1 * ret;
    }
    protobuf_get(self, PACKET_LENGTH, &length);
    if (length == 0) {
        return ret;
    }
    payload = protobuf_payload(protobuf);
    if (payload < length) {
        if ((ret = protobuf_reallocate(self, (size_t) length)) != AAOS_OK) {
            return ret;
        }
    }
    protobuf_get(protobuf, PACKET_BUF, &buf, NULL);
    if ((ret = tcp_socket_read(self, buf, (size_t) length, NULL)) != AAOS_OK) {
        return -1 * ret;
    }
    protobuf_get(self, PACKET_ERRORCODE, &errorcode);
   
    return errorcode;
}

int
rpc_write(void *_self)
{
    struct RPCClass *class = (struct RPCClass *) classOf(_self);
    
    if (isOf(class, RPCClass()) && class->write.method) {
        return ((int (*)(void *)) class->write.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) rpc_write, "write", _self);
        return result;
    }
}

static int
RPC_write(void *_self)
{
    struct RPC *self = cast(RPC(), _self);
    
    void *protobuf = self->protobuf, *header;
    int ret;
    uint32_t length;

    header = protobuf_header(protobuf);
    protobuf_get(self, PACKET_LENGTH, &length);
    if ((ret = tcp_socket_write(self, header, (size_t) length + PACKETHEADERSIZE, NULL)) != AAOS_OK) {
        return -1 * ret;
    }
    
    return AAOS_OK;
}

int
rpc_call(void *_self)
{
    struct RPCClass *class = (struct RPCClass *) classOf(_self);
    
    if (isOf(class, RPCClass()) && class->call.method) {
        return ((int (*)(void *)) class->call.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) rpc_call, "call", _self);
        return result;
    }
}

/*
 * return a negtive error means a local error, otherwise, remote error.
 * a return value of AAOS_EMOREPACKET means the caller should issue calls
 * until the return value is other than AAOS_EMOREPACKET.
 */
static int
RPC_call(void *_self)
{
    struct RPC *self = cast(RPC(), _self);
    
    uint32_t length;
    uint16_t option;
    int ret;
    size_t payload;
    void *buf, *header;
    
    /*
     * Make a remote procedure call
     */
    header = protobuf_header(self);
    protobuf_get(self, PACKET_OPTION, &option);
    if (option & PROTO_OPTION_MORE_PACKET) {
        /*
         * skip send command.
         */
    } else {
        protobuf_get(self, PACKET_LENGTH, &length);
        if ((ret = tcp_socket_write(self, header, (size_t) length + PACKETHEADERSIZE, NULL)) != AAOS_OK) {
            protobuf_set(self, PACKET_OPTION, option & ~PROTO_OPTION_MORE_PACKET);
            return -1 * ret;
        }
    }
    
    /*
     * Read header from the server
     */
    if ((ret = tcp_socket_read(self, header, PACKETHEADERSIZE, NULL)) != AAOS_OK) {
        protobuf_set(self, PACKET_OPTION, option & ~PROTO_OPTION_MORE_PACKET);
        return -1 * ret;
    }
    protobuf_get(self, PACKET_LENGTH, &length);
    protobuf_get(self, PACKET_OPTION, &option);
    if (length == 0) {
        uint16_t errorcode;
        protobuf_get(self, PACKET_ERRORCODE, &errorcode);
        ret = errorcode;
        if (ret == AAOS_OK && (option & PROTO_OPTION_MORE_PACKET)) {
            ret = AAOS_EMOREPACK;
        }
        return ret;
    }
    /*
     * if any  payload, read it.
     */
    payload = protobuf_payload(self->protobuf);
    if (payload < length) {
        if ((ret = protobuf_reallocate(self, (size_t) length)) != AAOS_OK) {
            protobuf_set(self, PACKET_OPTION, option & ~PROTO_OPTION_MORE_PACKET);
            return ret;
        }
    }
    protobuf_get(self, PACKET_BUF, &buf, NULL);

    if ((ret = tcp_socket_read(self, buf, (size_t) length, NULL)) != AAOS_OK) {
        protobuf_set(self, PACKET_OPTION, option & ~PROTO_OPTION_MORE_PACKET);
        return -1 * ret;
    } else {
        uint16_t errorcode;
        protobuf_get(self, PACKET_ERRORCODE, &errorcode);
        ret = errorcode;
        if (ret == AAOS_OK && (option & PROTO_OPTION_MORE_PACKET)) {
            ret = AAOS_EMOREPACK;
        }
        return ret;
    }
}

int
rpc_inspect(void *_self)
{
    struct RPCClass *class = (struct RPCClass *) classOf(_self);
    
    if (isOf(class, RPCClass()) && class->inspect.method) {
        return ((int (*)(void *)) class->inspect.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) rpc_inspect, "inspect", _self);
        return result;
    }
}

int
RPC_inspect(void *_self)
{
    struct RPC *self = cast(RPC(), _self);
    
    void *protobuf = self->protobuf;
    
    protobuf_set(protobuf, PACKET_PROTOCOL, PROTO_SYSTEM);
    protobuf_set(protobuf, PACKET_COMMAND, SYSTEM_COMMAND_INSPECT);
  
    return rpc_call(_self);
}

int
rpc_register(void *_self, double timeout)
{
    struct RPCClass *class = (struct RPCClass *) classOf(_self);
    
    if (isOf(class, RPCClass()) && class->reg.method) {
        return ((int (*)(void *, double)) class->reg.method)(_self, timeout);
    } else {
        int result;
        forward(_self, &result, (Method) rpc_register, "register", _self, timeout);
        return result;
    }
}

int
RPC_register(void *_self, double timeout)
{
    struct RPC *self = cast(RPC(), _self);
    
    void *protobuf = self->protobuf;
    
    protobuf_set(protobuf, PACKET_PROTOCOL, PROTO_SYSTEM);
    protobuf_set(protobuf, PACKET_COMMAND, SYSTEM_COMMAND_REGISTER);
    protobuf_set(protobuf, PACKET_DF0, timeout);
    
    return rpc_call(_self);
}

static void
RPC_forward(const void *_self, void *result, Method selector, const char *name, va_list *app)
{
    const struct RPC *self = cast(RPC(), _self);
    Method method = (Method) 0;
    va_list ap;
    
    method = respondsTo(self->protobuf, name);

    if (!method) {
        if (self->_vtab) {
            method = virtualTo(self->_vtab, name);
        
#ifndef NDEBUG
            if (!method) {
                fprintf(stderr, "RPC doest not implement `%s` method.\n", name);
            }
#endif
        } else {
#ifndef NDEBUG
            fprintf(stderr, "RPC virtual table is null.\n");
#endif
        }
    }
    
    assert(method);
    
#ifdef va_copy
    va_copy(ap, *app);
#else
    ap = *app;
#endif
    
    void *obj = va_arg(ap, void *);
    
    if (selector == (Method) rpc_call || selector == (Method) rpc_execute || selector == (Method) rpc_process || selector == (Method) rpc_inspect) {
        *((int *) result) = ((int (*)(void *)) method)(obj);
    } else if (selector == (Method) rpc_register) {
        double timeout = va_arg(ap, double);
        *((int *) result) = ((int (*)(void *, double)) method)(obj, timeout);
    /*
     * delegation, multiple inheritance, ^_^.
     */
    } else if (selector == (Method) protobuf_set) {
        /*
         * for protobuf_set and protobuf_get, ONLY ONE parameter passed in, whose type is "va_list *".
         */
        cast(RPC(), obj);
        unsigned int field = va_arg(ap, unsigned int);
        switch (field) {
            case PACKET_BUF:
            {
                va_list *myapp = va_arg(ap, va_list *);
                const void *value = va_arg(*myapp, const void *);
                size_t size = va_arg(*myapp, size_t);
                ((void (*)(void *, unsigned int, ...)) method)(((struct RPC *) obj)->protobuf, field, value, size);
            }
                break;
            case PACKET_STR:
            {
                va_list *myapp = va_arg(ap, va_list *);
                const void *value = va_arg(*myapp, const void *);
                ((void (*)(void *, unsigned int, ...)) method)(((struct RPC *) obj)->protobuf, field, value);
            }
                break;
            case PACKET_FF0:
            case PACKET_FF1:
            case PACKET_FF2:
            case PACKET_FF3:
            case PACKET_DF0:
            case PACKET_DF1:
            {
                va_list *myapp = va_arg(ap, va_list *);
                double value = va_arg(*myapp, double);
                ((void (*)(void *, unsigned int, ...)) method)(((struct RPC *) obj)->protobuf, field, value);
            }
                break;
            default:
            {
                va_list *myapp = va_arg(ap, va_list *);
                uint32_t value = va_arg(*myapp, uint32_t);
                ((void (*)(void *, unsigned int, ...)) method)(((struct RPC *) obj)->protobuf, field, value);
            }
                break;
        }
    } else if (selector == (Method) protobuf_get) {
        cast(RPC(), obj);
        unsigned int field = va_arg(ap, unsigned int);
        switch (field) {
            case PACKET_STR:
            {
                va_list *myapp = va_arg(ap, va_list *);
                void **value = va_arg(*myapp, void **);
                ((void (*)(void *, unsigned int, ...)) method)(((struct RPC *) obj)->protobuf, field, value);
            }
                break;
            case PACKET_BUF:
            {
                va_list *myapp = va_arg(ap, va_list *);
                void *value = va_arg(*myapp, void *);
                size_t *size = va_arg(*myapp, size_t *);
                ((void (*)(void *, unsigned int, ...)) method)(((struct RPC *) obj)->protobuf, field, value, size);
            }
                break;
            default:
            {
                va_list *myapp = va_arg(ap, va_list *);
                void *value = va_arg(*myapp, void *);
                ((void (*)(void *, unsigned int, ...)) method)(((struct RPC *) obj)->protobuf, field, value);
                
            }
                break;
        }
    } else if (selector == (Method) protobuf_reallocate) {
        cast(RPC(), obj);
        size_t size = va_arg(ap, size_t);
        *((int *) result) = ((int (*)(void *, size_t)) method)(((struct RPC *) obj)->protobuf, size);
    } else if (selector == (Method) protobuf_header) {
        cast(RPC(), obj);
        *((void **) result) = ((void *(*)(void *)) method)(((struct RPC *) obj)->protobuf);
    } else if (selector == (Method) protobuf_payload) {
        *((size_t *) result) = ((size_t (*)(void *)) method)(((struct RPC *) obj)->protobuf);
    } else {
        assert(0);
    }
    
#ifdef va_copy
    va_end(ap);
#endif
    
}


static int 
RPC_puto(void *_self, FILE *fp)
{
    return AAOS_OK;
}


static void *
RPC_cctor(void *_self, const void *_from)
{
    struct RPC *self = super_cctor(RPC(), _self);
    const struct RPC *from = cast(RPC(), _from);

    self->_vtab = from->_vtab;
    self->option = from->option;
    self->protobuf = ocopy(ProtoBuf(), from->protobuf);
    
    return self;
}

static void *
RPC_ctor(void *_self, va_list *app)
{
    struct RPC *self = super_ctor(RPC(), _self, app);
    
    self->protobuf = new(ProtoBuf(), DEFAULTPACKETSIZE);
    
    return (void *) self;
}

static void *
RPC_dtor(void *_self)
{
    struct RPC *self = cast(RPC(), _self);
    
    delete(self->protobuf);
    
    return super_dtor(RPC(), _self);
}

static void *
RPCClass_ctor(void *_self, va_list *app)
{
    struct RPCClass *self = super_ctor(RPCClass(), _self, app);
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
        
        if (selector == (Method) rpc_read) {
            if (tag) {
                self->read.tag = tag;
                self->read.selector = selector;
            }
            self->read.method = method;
            continue;
        }
        if (selector == (Method) rpc_write) {
            if (tag) {
                self->write.tag = tag;
                self->write.selector = selector;
            }
            self->write.method = method;
            continue;
        }
        if (selector == (Method) rpc_call) {
            if (tag) {
                self->call.tag = tag;
                self->call.selector = selector;
            }
            self->call.method = method;
            continue;
        }
        if (selector == (Method) rpc_process) {
            if (tag) {
                self->process.tag = tag;
                self->process.selector = selector;
            }
            self->process.method = method;
            continue;
        }
        if (selector == (Method) rpc_execute) {
            if (tag) {
                self->execute.tag = tag;
                self->execute.selector = selector;
            }
            self->execute.method = method;
            continue;
        }
        if (selector == (Method) rpc_inspect) {
            if (tag) {
                self->inspect.tag = tag;
                self->inspect.selector = selector;
            }
            self->inspect.method = method;
            continue;
        }
        if (selector == (Method) rpc_register) {
            if (tag) {
                self->reg.tag = tag;
                self->reg.selector = selector;
            }
            self->reg.method = method;
            continue;
        }
    }
    
#ifdef va_copy
    va_end(ap);
#endif
    
    return self;
}

static void *_RPCClass;

static void
RPCClass_destroy(void)
{
    free((void *) _RPCClass);
}

static void
RPCClass_initialize(void)
{
    _RPCClass = new(TCPSocketClass(), "RPCClass", TCPSocketClass(), sizeof(struct RPCClass),
                    ctor, "ctor", RPCClass_ctor, 
                    (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(RPCClass_destroy);
#endif
}

const void *
RPCClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, RPCClass_initialize);
#endif
    
    return _RPCClass;
}

static void *_RPC;

static void
RPC_destroy(void)
{
    free((void *) _RPC);
}

static void
RPC_initialize(void)
{
    _RPC = new(RPCClass(), "RPC", TCPSocket(), sizeof(struct RPC),
               ctor, "ctor", RPC_ctor,
               cctor, "cctor", RPC_cctor,
               dtor, "dtor", RPC_dtor,
               puto, "puto", RPC_puto,
               forward, "forward", RPC_forward,
               rpc_execute, "execute", RPC_execute,
               rpc_read, "read", RPC_read,
               rpc_write, "write", RPC_write,
               rpc_process, "process", RPC_process,
               rpc_call, "call", RPC_call,
               rpc_register, "register", RPC_register,
               rpc_inspect, "inspect", RPC_inspect,
               (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(RPC_destroy);
#endif
}

const void *
RPC(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, RPC_initialize);
#endif
    
    return _RPC;
}

/*
 * RPC client virtual table
 */

static void *
RPCClientVirtualTable_ctor(void *_self, va_list *app)
{
    struct RPCClientVirtualTable *self = super_ctor(RPCClientVirtualTable(), _self, app);
    Method selector;
    
    while ((selector = va_arg(*app, Method))) {
        const char *tag = va_arg(*app, const char *);
        Method method = va_arg(*app, Method);
        if (selector == (Method) rpc_client_connect) {
            if (tag) {
                self->connect.tag = tag;
                self->connect.selector = selector;
            }
            self->connect.method = method;
            continue;
        }
    }
    
    return _self;
}

static void *
RPCClientVirtualTable_dtor(void *_self)
{
    return super_dtor(Object(), _self);
}

static const void *_RPCClientVirtualTable;

static void
RPCClientVirtualTable_destroy(void)
{
    free((void *) _RPCClientVirtualTable);
}

static void
RPCClientVirtualTable_initialize(void)
{
    _RPCClientVirtualTable = new(VirtualTableClass(), "RPCClientVirtualTable", VirtualTable(), sizeof(struct RPCClientVirtualTable),
                           ctor, "ctor", RPCClientVirtualTable_ctor,
                           dtor, "dtor", RPCClientVirtualTable_dtor,
                           (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(RPCClientVirtualTable_destroy);
#endif
}

const void *
RPCClientVirtualTable(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, RPCClientVirtualTable_initialize);
#endif
    
    return _RPCClientVirtualTable;
}

/*
 * RPC client class.
 */

int 
rpc_client_connect(void *_self, void **client)
{
    struct RPCClientClass *class = (struct RPCClientClass *) classOf(_self);
    
    int result;
    if (isOf(class, RPCClientClass()) && class->connect.method) {
        return ((int (*)(void *, void **)) class->connect.method)(_self, client);
    } else {
        forward(_self, &result, (Method) rpc_client_connect, "connect", _self, client);
        return result;
    }
}

static int
RPCClient_connect(void *_self, void **client)
{
    struct RPCClient *self = cast(RPCClient(), _self);
    
    int cfd;
    cfd = Tcp_connect(self->_.address, self->_.port, NULL, NULL);
    if (cfd < 0) {
        *client = NULL;
        switch (errno) {
            case ECONNREFUSED:
                return AAOS_ECONNREFUSED;
                break;
            case EHOSTUNREACH:
                return AAOS_EHOSTUNREACH;
                break;
            case ENETDOWN:
                return AAOS_ENETDOWN;
                break;
            case ENETUNREACH:
                return AAOS_ENETUNREACH;
                break;
            default:
                return AAOS_ERROR;
                break;
        }
    } else {
        *client = new(RPC(), cfd);
        return AAOS_OK;
    }
}

static void
RPCClient_forward(const void *_self, void *result, Method selector, const char *name, va_list *app)
{
    const struct RPCClient *self = cast(RPCClient(), _self);
    Method method = virtualTo(self->_vtab, name);
    va_list ap;
    
#ifdef va_copy
    va_copy(ap, *app);
#else
    ap = *app;
#endif
    
    void *obj = va_arg(*app, void *);
    
    if (selector == (Method) rpc_client_connect) {
        void **client = va_arg(*app, void **);
        *((int *) result) = ((int (*)(void *, void **)) method)(obj, client);
    } else if (selector == (Method) rpc_server_start) {
        *((int *) result) = ((int (*)(void *)) method)(obj);
    } else {
        assert(0);
    }
    
#ifdef va_copy
    va_end(ap);
#endif
}

static void *
RPCClient_ctor(void *_self, va_list *app)
{
    struct RPCClient *self = super_ctor(RPCClient(), _self, app);
    
    return (void *) self;
}

static void *
RPCClient_dtor(void *_self)
{
    //struct RPCClient *self = cast(RPCClient(), _self);
    
    return super_dtor(RPCClient(), _self); 
}

static void *
RPCClientClass_ctor(void *_self, va_list *app)
{
    struct RPCClientClass *self = super_ctor(RPCClientClass(), _self, app);
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
        
        if (selector == (Method) rpc_client_connect) {
            if (tag) {
                self->connect.tag = tag;
                self->connect.selector = selector;
            }
            self->connect.method = method;
            continue;
        }
    }
    
#ifdef va_copy
    va_end(ap);
#endif
    
    return self;
}

static const void *_RPCClientClass;

static void
RPCClientClass_destroy(void)
{
    free((void *)_RPCClientClass);
}

static void
RPCClientClass_initialize(void)
{
    _RPCClientClass = new(TCPClientClass(), "RPCClientClass", TCPClientClass(), sizeof(struct RPCClientClass),
                          ctor, "ctor", RPCClientClass_ctor,
                          (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(RPCClientClass_destroy);
#endif
}

const void *
RPCClientClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, RPCClientClass_initialize);
#endif
    return _RPCClientClass;
}

static const void *_RPCClient;

static void
RPCClient_destroy(void)
{
    free((void *)_RPCClient);
}

static void
RPCClient_initialize(void)
{
    _RPCClient = new(RPCClientClass(), "RPCClient", TCPClient(), sizeof(struct RPCClient),
                     ctor, "ctor", RPCClient_ctor,
                     ctor, "ctor", RPCClient_ctor,
                     dtor, "dtor", RPCClient_dtor,
                     forward, "forward", RPCClient_forward,
                     rpc_client_connect, "connect", RPCClient_connect,
                     (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(RPCClient_destroy);
#endif
}

const void *
RPCClient(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, RPCClient_initialize);
#endif
    return _RPCClient;
}

/*
 * RPC server virtual table
 */

static void *
RPCServerVirtualTable_ctor(void *_self, va_list *app)
{
    struct RPCServerVirtualTable *self = super_ctor(RPCServerVirtualTable(), _self, app);
    Method selector;
    
    while ((selector = va_arg(*app, Method))) {
        const char *tag = va_arg(*app, const char *);
        Method method = va_arg(*app, Method);
        if (selector == (Method) rpc_server_accept) {
            if (tag) {
                self->accept.tag = tag;
                self->accept.selector = selector;
            }
            self->accept.method = method;
            continue;
        }
        if (selector == (Method) rpc_server_start) {
            if (tag) {
                self->start.tag = tag;
                self->start.selector = selector;
            }
            self->start.method = method;
            continue;
        }
    }
    
    return _self;
}

static void *
RPCServerVirtualTable_dtor(void *_self)
{
    return super_dtor(Object(), _self);
}

static const void *_RPCServerVirtualTable;

static void
RPCServerVirtualTable_destroy(void)
{
    free((void *) _RPCServerVirtualTable);
}

static void
RPCServerVirtualTable_initialize(void)
{
    _RPCServerVirtualTable = new(VirtualTableClass(), "RPCServerVirtualTable", VirtualTable(), sizeof(struct RPCServerVirtualTable),
                           ctor, "ctor", RPCServerVirtualTable_ctor,
                           dtor, "dtor", RPCServerVirtualTable_dtor,
                           (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(RPCServerVirtualTable_destroy);
#endif
}

const void *
RPCServerVirtualTable(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, RPCServerVirtualTable_initialize);
#endif
    return _RPCServerVirtualTable;
}

/*
 * RPC server class.
 */

int
rpc_server_accept(void *_self, void **client)
{
    struct RPCServerClass *class = (struct RPCServerClass *) classOf(_self);
    
    int result;
    if (isOf(class, RPCServerClass()) && class->accept.method) {
       return ((int (*)(void *, void **)) class->accept.method)(_self, client);
    } else {
        forward(_self, &result, (Method) rpc_server_accept, "accept", _self, client);
        return result;
    }
}

static int
RPCServer_accept(void *_self, void **client)
{
    struct RPCServer *self = cast(RPCServer(), _self);
    
    int lfd, cfd;
    
    lfd = tcp_server_get_lfd(self);
    cfd = Accept(lfd, NULL, NULL);
    if (cfd < 0) {
        *client = NULL;
        return AAOS_ERROR;
    } else {
        *client = new(RPC(), cfd);
        return AAOS_OK;
    }
}

void
rpc_server_start(void *_self)
{
    struct RPCServerClass *class = (struct RPCServerClass *) classOf(_self);
    
    if (isOf(class, RPCServerClass()) && class->start.method) {
       return ((void (*)(void *)) class->start.method)(_self);
    } else {
       forward(_self, 0, (Method) rpc_server_start, "start", _self);
    }
}

static void *
RPCServer_process_thr(void *arg)
{
    int ret;
    
    Pthread_detach(pthread_self());
    
    for (; ;) {
        if ((ret = rpc_process(arg)) != AAOS_OK) {
            break;
        }
    }
    
    delete(arg);
    return NULL;
}

static void *
RPCServer_process_thr2(void *arg)
{
    struct RPCServer *self = (struct RPCServer *) arg;

    int lfd = self->_.lfd, sockfd, ret, n_events;
    size_t i;

#ifdef LINUX
    int efd = epoll_create(1);
    struct epoll_event ev, *events;

    events = (struct epoll_event *) Malloc(sizeof(struct epoll_event) * self->_.max_events);
    ev.events = EPOLLIN | EPOLLET | EPOLLEXCLUSIVE;
    ev.data.ptr = self;
    epoll_ctl(efd, EPOLL_CTL_ADD, lfd, &ev);
    int timeout = self->_.timeout * 1000;
    for (;;) {
        n_events = epoll_wait(efd, events, self->_.max_events, timeout);
        for (i = 0; i < n_events; i++) {
            if (events[i].data.ptr == self) {
                void *client;
                for (;;) {
                    ret = rpc_server_accept(self, &client);
                    if (ret != AAOS_OK) {
                        break;
                    }
                    ev.events = EPOLLIN | EPOLLET;
                    ev.data.ptr = client;
                    sockfd = tcp_socket_get_sockfd(client);
                    epoll_ctl(efd, EPOLL_CTL_ADD, sockfd, &ev);
                }
            } else {
                for (;;) {
                    ret = rpc_process(events[i].data.ptr);
                    if (ret == AAOS_OK) {
                        continue;
                    } else if (ret == AAOS_EAGAIN) {
                        break;
                    } else {
                        sockfd = tcp_socket_get_sockfd(events[i].data.ptr);
                        epoll_ctl(efd, EPOLL_CTL_DEL, sockfd, NULL);
                        delete(events[i].data.ptr);
                        break;
                    }
                }
            }
        }
    }

error:
    Close(efd);
    free(events);
#endif 

#ifdef MACOSX
    int kq, cfd, j = 1;
    struct kevent *changelist, *eventlist;
    struct timespec tp;
    void *client;

    tp.tv_sec = floor(self->_.timeout);
    tp.tv_nsec = (self->_.timeout - tp.tv_sec) * 1000000000;

    kq = kqueue();
    eventlist = (struct kevent *) Malloc(sizeof(struct kevent) * self->_.max_events);
    changelist = (struct kevent *) Malloc(sizeof(struct kevent) * self->_.max_events);
    EV_SET(&changelist[0], lfd, EVFILT_READ, EV_ADD, 0, 0, &self);
    for (;;) {
        n_events = kevent(kq, changelist, j, eventlist, self->_.max_events, &tp);
        for (i = 0; i < n_events; i++) {
            if (eventlist[i].udata == self) {
                for (j = 1; j < self->_.max_events; j++) {
                    ret = rpc_server_accept(self, &client);
                    if (ret != AAOS_OK) {
                        break;
                    }
                    cfd = tcp_socket_get_sockfd(client);
                    EV_SET(&changelist[j], cfd, EVFILT_READ, EV_ADD, 0, 0, client);
                }
            } else {
                for (;;) {
                    ret = rpc_process(changelist[i].udata);
                    if (ret == AAOS_OK) {
                        continue;
                    } else if (ret == AAOS_EAGAIN) {
                        break;
                    } else {
                        delete(changelist[i].udata);
                        break;
                    }
                }
            }
        }
    }
error:
    free(changelist);
    free(eventlist);
    Close(kq);
    free(eventlist);
#endif

    return NULL;
}

void
RPCServer_start(void *_self)
{
    struct RPCServer *self = cast(RPCServer(), _self);
    
    void *client;
    pthread_t tid, *tids;
    sigset_t set;
    int ret;
    size_t i;

    sigemptyset(&set);
    sigaddset(&set, SIGPIPE);
    Pthread_sigmask(SIG_BLOCK, &set, NULL);
    
    self->_.lfd = Tcp_listen(NULL, self->_.port, NULL, NULL);

    switch (self->_.option) {
        case TCPSERVER_OPTION_DEFAULT:
            for (;;) {
                if ((ret = rpc_server_accept(self, &client)) == AAOS_OK) {
                    Pthread_create(&tid, NULL, RPCServer_process_thr, client);
                }
            }
            break;
        case TCPSERVER_OPTION_NONBLOCK_PRETHEADED:
            Fcntl(self->_.lfd, F_SETFL, O_NONBLOCK);
            tids = (pthread_t *) Malloc(sizeof(pthread_t) * self->_.n_threads);
            for (i = 0; i < self->_.n_threads; i ++) {
                Pthread_create(&tids[i], NULL, RPCServer_process_thr2, self);
            }
            for (i = 0; i < self->_.n_threads; i++) {
                Pthread_join(tids[i], NULL);
            }
            free(tids);
            break;
        default:
            break;
    }

    /*
     *
    if (self->option&RPC_PRE_THREADED) {
        pthread_t tids[8];
        int i;
        for (i = 0; i < 8; i++) {
            Pthread_create(&tids[i], NULL, RPCServer_process_thr2, self);
        }
        for (i = 0; i < 8; i++) {
            Pthread_join(tids[i], NULL);
        }
    } else {
        for (; ;) {
            if ((ret = rpc_server_accept(self, &client)) == AAOS_OK) {
                Pthread_create(&tid, NULL, RPCServer_process_thr, client);
            }
        }
    }
    */
}

static void
RPCServer_forward(const void *_self, void *result, Method selector, const char *name, va_list *app)
{
    const struct RPCServer *self = cast(RPCServer(), _self);
    Method method = virtualTo(self->_vtab, name);
    va_list ap;
    
#ifdef va_copy
    va_copy(ap, *app);
#else
    ap = *app;
#endif
    
    void *obj = va_arg(*app, void *);
    
    if (selector == (Method) rpc_server_accept) {
        void **client = va_arg(*app, void **);
        *((int *) result) = ((int (*)(void *, void **)) method)(obj, client);
    } else if (selector == (Method) rpc_server_start) {
        *((int *) result) = ((int (*)(void *)) method)(obj);
    } else {
        assert(0);
    }
    
#ifdef va_copy
    va_end(ap);
#endif
}

static void *
RPCServer_ctor(void *_self, va_list *app)
{
    struct RPCServer *self = super_ctor(RPCServer(), _self, app);
    
    return (void *) self;
}

static void *
RPCServer_dtor(void *_self)
{
    struct RPCServer *self = cast(RPCServer(), _self);
    
    return (void *) super_dtor(RPCServer(), self); 
}

static void *
RPCServerClass_ctor(void *_self, va_list *app)
{
    struct RPCServerClass *self = super_ctor(RPCServerClass(), _self, app);
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
        
        if (selector == (Method) rpc_server_accept) {
            if (tag) {
                self->accept.tag = tag;
                self->accept.selector = selector;
            }
            self->accept.method = method;
            continue;
        }
        if (selector == (Method) rpc_server_start) {
            if (tag) {
                self->start.tag = tag;
                self->start.selector = selector;
            }
            self->start.method = method;
            continue;
        }
    }
    
#ifdef va_copy
    va_end(ap);
#endif
    
    return self;
}

static const void *_RPCServerClass;

static void
RPCServerClass_destroy(void)
{
    free((void *)_RPCServerClass);
}

static void
RPCServerClass_initialize(void)
{
    _RPCServerClass = new(TCPServerClass(), "RPCServerClass", TCPServerClass(), sizeof(struct RPCServerClass),
                          ctor, "ctor", RPCServerClass_ctor,
                          (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(RPCServerClass_destroy);
#endif
}

const void *
RPCServerClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, RPCServerClass_initialize);
#endif
    return _RPCServerClass;
}

static const void *_RPCServer;

static void
RPCServer_destroy(void)
{
    free((void *)_RPCServer);
}

static void
RPCServer_initialize(void)
{
    _RPCServer = new(RPCServerClass(), "RPCServer", TCPServer(), sizeof(struct RPCServer),
                     ctor, "ctor", RPCServer_ctor,
                     dtor, "dtor", RPCServer_dtor,
                     forward, "forward", RPCServer_forward,
                     rpc_server_accept, "accept", RPCServer_accept,
                     rpc_server_start, "start", RPCServer_start,
                     (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(RPCServer_destroy);
#endif
}

const void *
RPCServer(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, RPCServer_initialize);
#endif
    return _RPCServer;
}

#ifdef _USE_COMPILER_ATTRIBUTION_

static void __destructor__(void) __attribute__ ((destructor(_RPC_PRIORITY_)));

static void
__destructor__(void)
{
    RPCServer_destroy();
    RPCServerClass_destroy();
    RPCServerVirtualTable_destroy();
    RPCClient_destroy();
    RPCClientClass_destroy();
    RPCClientVirtualTable_destroy();
    RPC_destroy();
    RPCClass_destroy();
    RPCVirtualTable_destroy();
}

static void __constructor__(void) __attribute__ ((constructor(_RPC_PRIORITY_)));

static void
__constructor__(void)
{
    RPCVirtualTable_initialize();
    RPCClass_initialize();
    RPC_initialize();
    RPCClientVirtualTable_initialize();
    RPCClientClass_init();
    RPCClient_initialize();
    RPCServerVirtualTable_initialize();
    RPCServerClass_initialize();
    RPCServer_initialize();
}

#endif
