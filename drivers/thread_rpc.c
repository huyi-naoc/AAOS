//
//  thread_rpc.c
//  AAOS
//
//  Created by Hu Yi on 2026/2/21.
//

#include "def.h"
#include "thread_rpc.h"
#include "thread_rpc_r.h"
#include "thread.h"
#include "wrapper.h"

void **threads;
size_t n_thread;

static int
get_index_by_name(const char *name, int *index)
{
    size_t i;
    const char *s;
    *index = 0;
    
    for (i = 0; i < n_thread; i++) {
        s = __observation_thread_get_name(threads[i]);
        if (s != NULL && strcmp(name, s) == 0) {
            *index = (int) i + 1;
            return AAOS_OK;
        }
    }
    
    return AAOS_ENOTFOUND;
}

static void *
get_observation_thread_by_name(const char *name)
{
    size_t i;
    const char *s;
    void *thread = NULL;
    
    for (i = 0; i < n_thread; i++) {
        s = __observation_thread_get_name(threads[i]);
        if (s != NULL && strcmp(name, s) == 0) {
            return  threads[i];
        }
    }
    
    return thread;
}

static void *
get_observation_thread_by_index(int index)
{
    if (index > 0 && index <= n_thread) {
        return threads[index - 1];
    } else {
        return NULL;
    }
}

inline static int
ObservationThread_protocol_check(void *_self)
{
    uint16_t protocol;
    protobuf_get(_self, PACKET_PROTOCOL, &protocol);
    if (protocol != PROTO_THREAD && protocol != PROTO_SYSTEM) {
        protobuf_set(_self, PACKET_ERRORCODE, AAOS_EPROTOWRONG);
        protobuf_set(_self, PACKET_LENGTH, 0);
        return AAOS_EPROTOWRONG;
    } else {
        return AAOS_OK;
    }
}

int
observation_thread_get_index_by_name(void *_self, const char *name)
{
    const struct ObservationThreadClass *class = (const struct ObservationThreadClass *) classOf(_self);
    
    if (name == NULL) {
        return AAOS_ENOTFOUND;
    }
    
    if (isOf(class, ObservationThreadClass()) && class->get_index_by_name.method) {
        return ((int (*)(void *, const char *)) class->get_index_by_name.method)(_self, name);
    } else {
        int result;
        forward(_self, &result, (Method) observation_thread_get_index_by_name, "get_index_by_name", _self, name);
        return result;
    }
}

static int
ObservationThread_get_index_by_name(void *_self, const char *name)
{
    struct ObservationThread *self = cast(ObservationThread(), _self);
    size_t length;
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_THREAD);
    protobuf_set(self, PACKET_COMMAND, OT_COMMAND_GET_INDEX_BY_NAME);
    protobuf_set(self, PACKET_INDEX, 0);
    
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
        uint32_t len = (uint32_t) length;
        protobuf_set(self, PACKET_LENGTH, len + 1);
    }
    
    return rpc_call(self);
}

int
observation_thread_start(void *_self)
{
    const struct ObservationThreadClass *class = (const struct ObservationThreadClass *) classOf(_self);
    
    if (isOf(class, ObservationThreadClass()) && class->start.method) {
        return ((int (*)(void *)) class->start.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) observation_thread_start, "start", _self);
        return result;
    }
}

static int
ObservationThread_start(void *_self)
{
    struct ObservationThread *self = cast(ObservationThread(), _self);

    protobuf_set(self, PACKET_PROTOCOL, PROTO_THREAD);
    protobuf_set(self, PACKET_COMMAND, OT_COMMAND_START);
    protobuf_set(self, PACKET_LENGTH, 0);
        
    return rpc_call(self);
}

int
observation_thread_terminate(void *_self)
{
    const struct ObservationThreadClass *class = (const struct ObservationThreadClass *) classOf(_self);
        
    if (isOf(class, ObservationThreadClass()) && class->terminate.method) {
        return ((int (*)(void *)) class->terminate.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) observation_thread_terminate, "terminate", _self);
        return result;
    }
}

static int
ObservationThread_terminate(void *_self)
{
    struct ObservationThread *self = cast(ObservationThread(), _self);


    protobuf_set(self, PACKET_PROTOCOL, PROTO_THREAD);
    protobuf_set(self, PACKET_COMMAND, OT_COMMAND_START);
    protobuf_set(self, PACKET_LENGTH, 0);
        
    return rpc_call(self);
}

int
observation_thread_resume(void *_self)
{
    const struct ObservationThreadClass *class = (const struct ObservationThreadClass *) classOf(_self);
        
    if (isOf(class, ObservationThreadClass()) && class->resume.method) {
        return ((int (*)(void *)) class->resume.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) observation_thread_resume, "resume", _self);
        return result;
    }
}

static int
ObservationThread_resume(void *_self)
{
    struct ObservationThread *self = cast(ObservationThread(), _self);


    protobuf_set(self, PACKET_PROTOCOL, PROTO_THREAD);
    protobuf_set(self, PACKET_COMMAND, OT_COMMAND_RESUME);
    protobuf_set(self, PACKET_LENGTH, 0);
        
    return rpc_call(self);
}

int
observation_thread_suspend(void *_self, uint32_t flag)
{
    const struct ObservationThreadClass *class = (const struct ObservationThreadClass *) classOf(_self);
        
    if (isOf(class, ObservationThreadClass()) && class->suspend.method) {
        return ((int (*)(void *, uint32_t)) class->suspend.method)(_self, flag);
    } else {
        int result;
        forward(_self, &result, (Method) observation_thread_suspend, "suspend", _self, flag);
        return result;
    }
}

static int
ObservationThread_suspend(void *_self, uint32_t flag)
{
    struct ObservationThread *self = cast(ObservationThread(), _self);

    protobuf_set(self, PACKET_PROTOCOL, PROTO_THREAD);
    protobuf_set(self, PACKET_COMMAND, OT_COMMAND_SUSPEND);
    protobuf_set(self, PACKET_LENGTH, 0);
    protobuf_set(self, PACKET_U32F0, flag);
        
    return rpc_call(self);
}

int
observation_thread_stop(void *_self, uint32_t flag)
{
    const struct ObservationThreadClass *class = (const struct ObservationThreadClass *) classOf(_self);
        
    if (isOf(class, ObservationThreadClass()) && class->stop.method) {
        return ((int (*)(void *, uint32_t)) class->stop.method)(_self, flag);
    } else {
        int result;
        forward(_self, &result, (Method) observation_thread_stop, "stop", _self, flag);
        return result;
    }
}

static int
ObservationThread_stop(void *_self, uint32_t flag)
{
    struct ObservationThread *self = cast(ObservationThread(), _self);

    protobuf_set(self, PACKET_PROTOCOL, PROTO_THREAD);
    protobuf_set(self, PACKET_COMMAND, OT_COMMAND_STOP);
    protobuf_set(self, PACKET_LENGTH, 0);
    protobuf_set(self, PACKET_U32F0, flag);
        
    return rpc_call(self);
}

int
observation_thread_cancel(void *_self, uint32_t flag)
{
    const struct ObservationThreadClass *class = (const struct ObservationThreadClass *) classOf(_self);
        
    if (isOf(class, ObservationThreadClass()) && class->cancel.method) {
        return ((int (*)(void *, uint32_t)) class->cancel.method)(_self, flag);
    } else {
        int result;
        forward(_self, &result, (Method) observation_thread_cancel, "cancel", _self, flag);
        return result;
    }
}

static int
ObservationThread_cancel(void *_self, uint32_t flag)
{
    struct ObservationThread *self = cast(ObservationThread(), _self);

    protobuf_set(self, PACKET_PROTOCOL, PROTO_THREAD);
    protobuf_set(self, PACKET_COMMAND, OT_COMMAND_CANCEL);
    protobuf_set(self, PACKET_LENGTH, 0);
    protobuf_set(self, PACKET_U32F0, flag);
        
    return rpc_call(self);
}

/*
 * Virtual function for observation thread rpc.
 */
static const void *observation_thread_virtual_table(void);

static int
ObservationThread_execute_default(struct ObservationThread *self)
{
    return AAOS_EBADCMD;
}

static int
ObservationThread_execute_get_index_by_name(struct ObservationThread *self)
{
   
    char *name;
    int index, ret;
    uint16_t idx;
    uint32_t length;

    protobuf_get(self, PACKET_LENGTH, &length);
    /*
     * if lenght == 0, use str field.
     */
    if (length == 0) {
        protobuf_get(self, PACKET_STR, &name);
    } else {
        protobuf_get(self, PACKET_BUF, &name, NULL);
    }
    
    if ((ret = get_index_by_name(name, &index)) != AAOS_OK) {
        return ret;
    } else {
        idx = (uint16_t) index;
        protobuf_set(self, PACKET_INDEX, idx);
        protobuf_set(self, PACKET_LENGTH, 0);
    }

    return AAOS_OK;
}


static int
ObservationThread_execute_start(struct ObservationThread *self)
{
    uint16_t index;
    void *thread;
    
    protobuf_get(self, PACKET_INDEX, &index);
    
    if ((thread = get_observation_thread_by_index((int) index)) == NULL) {
        protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
        protobuf_set(self, PACKET_LENGTH, 0);
        return AAOS_ENOTFOUND;
    }    
    protobuf_set(self, PACKET_LENGTH, 0);

    return __observation_thread_start(thread);
}

static int
ObservationThread_execute_terminate(struct ObservationThread *self)
{
    uint16_t index;
    void *thread;
    
    protobuf_get(self, PACKET_INDEX, &index);
    
    if ((thread = get_observation_thread_by_index((int) index)) == NULL) {
        protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
        protobuf_set(self, PACKET_LENGTH, 0);
        return AAOS_ENOTFOUND;
    }
    protobuf_set(self, PACKET_LENGTH, 0);
    
    return __observation_thread_terminate(thread);
   
}

static int
ObservationThread_execute_resume(struct ObservationThread *self)
{
    uint16_t index;
    void *thread;
    
    protobuf_get(self, PACKET_INDEX, &index);
    
    if ((thread = get_observation_thread_by_index((int) index)) == NULL) {
        protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
        protobuf_set(self, PACKET_LENGTH, 0);
        return AAOS_ENOTFOUND;
    }
    protobuf_set(self, PACKET_LENGTH, 0);
    
    return  __observation_thread_resume(thread);
}

static int
ObservationThread_execute_suspend(struct ObservationThread *self)
{
    uint16_t index;
    uint32_t flag;
    void *thread;
    
    protobuf_get(self, PACKET_INDEX, &index);
    protobuf_get(self, PACKET_U32F0, &flag);
    
    if ((thread = get_observation_thread_by_index((int) index)) == NULL) {
        protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
        protobuf_set(self, PACKET_LENGTH, 0);
        return AAOS_ENOTFOUND;
    }
    protobuf_set(self, PACKET_LENGTH, 0);

    return __observation_thread_suspend(thread, flag);
}

static int
ObservationThread_execute_stop(struct ObservationThread *self)
{
    uint16_t index;
    uint32_t flag;
    void *thread;
    
    protobuf_get(self, PACKET_INDEX, &index);
    protobuf_get(self, PACKET_U32F0, &flag);
    
    if ((thread = get_observation_thread_by_index((int) index)) == NULL) {
        protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
        protobuf_set(self, PACKET_LENGTH, 0);
        return AAOS_ENOTFOUND;
    }
    protobuf_set(self, PACKET_LENGTH, 0);
    
    return __observation_thread_stop(thread, flag);
}

static int
ObservationThread_execute_cancel(struct ObservationThread *self)
{
    uint16_t index;
    uint32_t flag;
    void *thread;
    
    protobuf_get(self, PACKET_INDEX, &index);
    protobuf_get(self, PACKET_U32F0, &flag);
    
    if ((thread = get_observation_thread_by_index((int) index)) == NULL) {
        protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
        protobuf_set(self, PACKET_LENGTH, 0);
        return AAOS_ENOTFOUND;
    }
    protobuf_set(self, PACKET_LENGTH, 0);
    
    return __observation_thread_cancel(thread, flag);
}

static int
ObservationThread_execute(void *_self)
{
    struct ObservationThread *self = cast(ObservationThread(), _self);
    uint16_t command;
    
    if (ObservationThread_protocol_check(self) != AAOS_OK) {
        return AAOS_EPROTOWRONG;
    }
    protobuf_get(self, PACKET_COMMAND, &command);
    switch (command) {
        case OT_COMMAND_GET_INDEX_BY_NAME:
            return ObservationThread_execute_get_index_by_name(self);
            break;
        case OT_COMMAND_START:
            return ObservationThread_execute_start(self);
            break;
        case OT_COMMAND_TERMINATE:
            return ObservationThread_execute_terminate(self);
            break;
        case OT_COMMAND_RESUME:
            return ObservationThread_execute_resume(self);
            break;
        case OT_COMMAND_SUSPEND:
            return ObservationThread_execute_suspend(self);
            break;
        case OT_COMMAND_STOP:
            return ObservationThread_execute_stop(self);
            break;
        case OT_COMMAND_CANCEL:
            return ObservationThread_execute_cancel(self);
            break;
        default:
            return ObservationThread_execute_default(self);
            break;
    }
}

static void *
ObservationThread_ctor(void *_self, va_list *app)
{
    struct ObservationThread *self = super_ctor(ObservationThread(), _self, app);
    
    self->_._vtab = observation_thread_virtual_table();
    
    return (void *) self;
}

static void *
ObservationThread_dtor(void *_self)
{
    //struct RPC *self = cast(RPC(), _self);
    
    return super_dtor(ObservationThread(), _self);
}

static void *
ObservationThreadClass_ctor(void *_self, va_list *app)
{
    struct ObservationThreadClass *self = super_ctor(ObservationThreadClass(), _self, app);
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
        
        if (selector == (Method) observation_thread_get_index_by_name) {
            if (tag) {
                self->get_index_by_name.tag = tag;
                self->get_index_by_name.selector = selector;
            }
            self->get_index_by_name.method = method;
            continue;
        }
        if (selector == (Method) observation_thread_start) {
            if (tag) {
                self->start.tag = tag;
                self->start.selector = selector;
            }
            self->start.method = method;
            continue;
        }
        if (selector == (Method) observation_thread_terminate) {
            if (tag) {
                self->terminate.tag = tag;
                self->terminate.selector = selector;
            }
            self->terminate.method = method;
            continue;
        }
        if (selector == (Method) observation_thread_resume) {
            if (tag) {
                self->resume.tag = tag;
                self->resume.selector = selector;
            }
            self->resume.method = method;
            continue;
        }
        if (selector == (Method) observation_thread_suspend) {
            if (tag) {
                self->suspend.tag = tag;
                self->suspend.selector = selector;
            }
            self->suspend.method = method;
            continue;
        }
        if (selector == (Method) observation_thread_stop) {
            if (tag) {
                self->stop.tag = tag;
                self->stop.selector = selector;
            }
            self->stop.method = method;
            continue;
        }
        if (selector == (Method) observation_thread_cancel) {
            if (tag) {
                self->cancel.tag = tag;
                self->cancel.selector = selector;
            }
            self->cancel.method = method;
            continue;
        }
    }
    
#ifdef va_copy
    va_end(ap);
#endif
    
    self->_.execute.method = (Method) 0;
    
    return self;
}

static void *_ObservationThreadClass;

static void
ObservationThreadClass_destroy(void)
{
    free((void *) _ObservationThreadClass);
}

static void
ObservationThreadClass_initialize(void)
{
    _ObservationThreadClass = new(RPCClass(), "ObservationThreadClass", RPCClass(), sizeof(struct ObservationThreadClass),
                                ctor, "ctor", ObservationThreadClass_ctor,
                        (void *) 0);
    
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(ObservationThreadClass_destroy);
#endif
}

const void *
ObservationThreadClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, ObservationThreadClass_initialize);
#endif
    
    return _ObservationThreadClass;
}

static void *_ObservationThread;

static void
ObservationThread_destroy(void)
{
    free((void *) _ObservationThread);
}

static void
ObservationThread_initialize(void)
{
    _ObservationThread = new(ObservationThreadClass(), "ObservationThread", RPC(), sizeof(struct ObservationThread),
                             ctor, "ctor", ObservationThread_ctor,
                             dtor, "dtor", ObservationThread_dtor,
                  
                             observation_thread_get_index_by_name, "get_index_by_name", ObservationThread_get_index_by_name,
                             observation_thread_start, "start", ObservationThread_start,
                             observation_thread_terminate, "terminate", ObservationThread_terminate,
                             observation_thread_resume, "resume", ObservationThread_resume,
                             observation_thread_suspend, "suspend", ObservationThread_suspend,
                             observation_thread_stop, "stop", ObservationThread_stop,
                             observation_thread_cancel, "cancel", ObservationThread_cancel,
                  
                             (void *) 0);
    
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(ObservationThread_destroy);
#endif

}

const void *
ObservationThread(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, ObservationThread_initialize);
#endif
    
    return _ObservationThread;
}

static const void *_observation_thread_virtual_table;

static void
observation_thread_virtual_table_destroy(void)
{
    delete((void *) _observation_thread_virtual_table);
}

static void
observation_thread_virtual_table_initialize(void)
{
    _observation_thread_virtual_table = new(RPCVirtualTable(),
                                rpc_execute, "execute", ObservationThread_execute,
                                (void *)0);
    
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(observation_thread_virtual_table_destroy);
#endif
}

static const void *
observation_thread_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, observation_thread_virtual_table_initialize);
#endif
    
    return _observation_thread_virtual_table;
}

static const void *observation_thread_client_virtual_table(void);

static
int ObservationThreadClient_connect(void *_self, void **client)
{
    struct ObservationThreadClient *self = cast(ObservationThreadClient(), _self);
    
    int cfd;
    
    if (self->_._.address != NULL && Access(self->_._.address, F_OK) == 0) {
        cfd = Un_stream_connect(self->_._.address);
    } else {
        cfd = Tcp_connect(self->_._.address, self->_._.port, NULL, NULL);
    }
    
    if (cfd < 0) {
        *client = NULL;
        return AAOS_ERROR;
    } else {
        *client = new(ObservationThread(), cfd);
    }
    
    protobuf_set(*client, PACKET_PROTOCOL, PROTO_THREAD);
    
    return AAOS_OK;
}

static void *
ObservationThreadClient_ctor(void *_self, va_list *app)
{
    struct ObservationThreadClient *self = super_ctor(ObservationThreadClient(), _self, app);
    
    self->_._vtab = observation_thread_client_virtual_table();
    
    return (void *) self;
}

static void *
ObservationThreadClient_dtor(void *_self)
{
    //struct RPC *self = cast(RPC(), _self);
    
    return super_dtor(ObservationThreadClient(), _self);
}

static void *
ObservationThreadClientClass_ctor(void *_self, va_list *app)
{
    struct ObservationThreadClientClass *self = super_ctor(ObservationThreadClientClass(), _self, app);
    
    self->_.connect.method = (Method) 0;
    
    return self;
}

static void *_ObservationThreadClientClass;

static void
ObservationThreadClientClass_destroy(void)
{
    free((void *) _ObservationThreadClientClass);
}

static void
ObservationThreadClientClass_initialize(void)
{
    _ObservationThreadClientClass = new(RPCClientClass(), "ObservationThreadClientClass", RPCClientClass(), sizeof(struct ObservationThreadClientClass),
                                        ctor, "ctor", ObservationThreadClientClass_ctor,
                             
                                        (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(ObservationThreadClientClass_destroy);
#endif
}

const void *
ObservationThreadClientClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, ObservationThreadClientClass_initialize);
#endif
    
    return _ObservationThreadClientClass;
}

static void *_ObservationThreadClient;

static void
ObservationThreadClient_destroy(void)
{
    free((void *) _ObservationThreadClient);
}

static void
ObservationThreadClient_initialize(void)
{
    _ObservationThreadClient = new(ObservationThreadClientClass(), "ObservationThreadClient", RPCClient(), sizeof(struct ObservationThreadClient),
                                   ctor, "ctor", ObservationThreadClient_ctor,
                                   dtor, "dtor", ObservationThreadClient_dtor,
                        
                                   (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(ObservationThreadClient_destroy);
#endif
}

const void *
ObservationThreadClient(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, ObservationThreadClient_initialize);
#endif
    
    return _ObservationThreadClient;
}

static const void *_observation_thread_client_virtual_table;

static void
observation_thread_client_virtual_table_destroy(void)
{
    delete((void *) _observation_thread_client_virtual_table);
}

static void
observation_thread_client_virtual_table_initialize(void)
{
    _observation_thread_client_virtual_table = new(RPCClientVirtualTable(),
                                       rpc_client_connect, "connect", ObservationThreadClient_connect,
                                       (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(observation_thread_client_virtual_table_destroy);
#endif
}

static const void *
observation_thread_client_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, observation_thread_client_virtual_table_initialize);
#endif
    
    return _observation_thread_client_virtual_table;
}

/*
 * ObservationThread server class
 */

static const void *observation_thread_server_virtual_table(void);

static int
ObservationThreadServer_accept(void *_self, void **client)
{
    struct RPCServer *self = cast(RPCServer(), _self);
    
    int lfd, cfd;
    
    lfd = tcp_server_get_lfd(self);
    cfd = Accept(lfd, NULL, NULL);
    if (cfd < 0) {
        *client = NULL;
        return AAOS_ERROR;
    } else {
        if ((*client = new(ObservationThread(), cfd)) == NULL) {
            Close(cfd);
            return AAOS_ERROR;
        }
        return AAOS_OK;
    }
}

static int
ObservationThreadServer_accept2(void *_self, void **client)
{
    struct RPCServer *self = cast(RPCServer(), _self);
    
    int lfd, cfd, lfds[2];
    
    tcp_server_get_lfds(self, lfds);
    lfd = lfds[1];
    
    cfd = Accept(lfd, NULL, NULL);
    if (cfd < 0) {
        *client = NULL;
        return AAOS_ERROR;
    } else {
        if ((*client = new(ObservationThread(), cfd)) == NULL) {
            Close(cfd);
            return AAOS_ERROR;
        }
        return AAOS_OK;
    }
}

static void *
ObservationThreadServer_ctor(void *_self, va_list *app)
{
    struct ObservationThreadServer *self = super_ctor(ObservationThreadServer(), _self, app);
    
    self->_._vtab = observation_thread_server_virtual_table();
    
    return (void *) self;
}

static void *
ObservationThreadServer_dtor(void *_self)
{
    //struct RPC *self = cast(RPC(), _self);
    
    return super_dtor(ObservationThreadServer(), _self);
}

static void *
ObservationThreadServerClass_ctor(void *_self, va_list *app)
{
    struct ObservationThreadServerClass *self = super_ctor(ObservationThreadServerClass(), _self, app);
    
    self->_.accept.method = (Method) 0;
    self->_.accept2.method = (Method) 0;
    
    return self;
}

static void *_ObservationThreadServerClass;

static void
ObservationThreadServerClass_destroy(void)
{
    free((void *) _ObservationThreadServerClass);
}

static void
ObservationThreadServerClass_initialize(void)
{
    _ObservationThreadServerClass = new(RPCServerClass(), "ObservationThreadServerClass", RPCServerClass(), sizeof(struct ObservationThreadServerClass),
                                        ctor, "ctor", ObservationThreadServerClass_ctor,
                                        (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(ObservationThreadServerClass_destroy);
#endif
}

const void *
ObservationThreadServerClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, ObservationThreadServerClass_initialize);
#endif
    
    return _ObservationThreadServerClass;
}

static void *_ObservationThreadServer;

static void
ObservationThreadServer_destroy(void)
{
    free((void *) _ObservationThreadServer);
}

static void
ObservationThreadServer_initialize(void)
{
    _ObservationThreadServer = new(ObservationThreadServerClass(), "ObservationThreadServer", RPCServer(), sizeof(struct ObservationThreadServer),
                                   ctor, "ctor", ObservationThreadServer_ctor,
                                   dtor, "dtor", ObservationThreadServer_dtor,
                                   
                                   (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(ObservationThreadServer_destroy);
#endif
}

const void *
ObservationThreadServer(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, ObservationThreadServer_initialize);
#endif
    
    return _ObservationThreadServer;
}

static const void *_observation_thread_server_virtual_table;

static void
observation_thread_server_virtual_table_destroy(void)
{
    delete((void *) _observation_thread_server_virtual_table);
}

static void
observation_thread_server_virtual_table_initialize(void)
{
    _observation_thread_server_virtual_table = new(RPCServerVirtualTable(),
                                                   rpc_server_accept, "accept", ObservationThreadServer_accept,
                                                   rpc_server_accept2, "accept2", ObservationThreadServer_accept2,
                                                   (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(observation_thread_server_virtual_table_destroy);
#endif
}

static const void *
observation_thread_server_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, observation_thread_server_virtual_table_initialize);
#endif
    
    return _observation_thread_server_virtual_table;
}

#ifdef _USE_COMPILER_ATTRIBUTION_
static void __destructor__(void) __attribute__ ((destructor(_THREAD_RPC_PRIORITY_)));

static void
__destructor__(void)
{
    ObservationThreadServer_destroy();
    ObservationThreadServerClass_destroy();
    observation_thread_server_virtual_table_destroy();
    ObservationThreadClient_destroy();
    ObservationThreadClientClass_destroy();
    observation_thread_client_virtual_table_destroy();
    ObservationThread_destroy();
    ObservationThreadClass_destroy();
    observation_thread_virtual_table_destroy();
}

static void __constructor__(void) __attribute__ ((constructor(_THREAD_RPC_PRIORITY_)));

static void
__constructor__(void)
{
    observation_thread_virtual_table_initialize();
    ObservationThreadClass_initialize();
    ObservationThread_initialize();
    observation_thread_client_virtual_table_initialize();
    ObservationThreadClientClass_initialize();
    ObservationThreadClient_initialize();
    observation_thread_server_virtual_table_initialize();
    ObservationThreadServerClass_initialize();
    ObservationThreadServer_initialize();
}
#endif

/*
 * Compiler-dependant initializer.
 */
#ifdef _USE_COMPILER_ATTRIBUTION_
static void __destructor__(void) __attribute__ ((destructor(_THREAD_RPC_PRIORITY_)));

static void
__destructor__(void)
{
    observation_thread_virtual_table_initialize();
    ObservationThreadClass_initialize();
    ObservationThread_initialize();
    observation_thread_server_virtual_table_initialize();
    ObservationThreadServerClass_initialize();
    ObservationThreadServer_initialize();
    observation_thread_client_virtual_table_initialize();
    ObservationThreadClientClass_initialize();
    ObservationThreadClient_initialize();
}

static void __constructor__(void) __attribute__ ((constructor(_THREAD_RPC_PRIORITY_)));

static void
__constructor__(void)
{
    ObservationThreadClient_destroy();
    ObservationThreadClientClass_destroy();
    observation_thread_client_virtual_table_destroy();
    ObservationThreadServer_destroy();
    ObservationThreadServerClass_destroy();
    observation_thread_server_virtual_table_destroy();
    ObservationThread_destroy();
    ObservationThreadClass_destroy();
    observation_thread_virtual_table_destroy();
}
#endif
