//
//  log_rpc.c
//  AAOS
//
//  Created by huyi on 2025/12/23.
//

#include "def.h"
#include "log.h"
#include "log_rpc.h"
#include "log_rpc_r.h"
#include "rpc.h"
#include "rpc_r.h"
#include "protocol.h"
#include "wrapper.h"

#define LOG_CONNECT_TIMEOUT 1.0

void **logs;
size_t n_log;
pthread_t *log_tids;
size_t n_log_thread;


static void *
get_log_by_facility(const char *facility)
{
    size_t i;
    const char *s = NULL;
   
    for (i = 0; i < n_log; i++) {
        if (logs[i] != NULL) {
            s = __log_get_facility(logs[i]);
        }
        if (facility != NULL && s != NULL && strcmp(facility, s) == 0) {
            return  logs[i];
        }
    }
    
    return logs[0];
}

static void *
get_log_by_index(int index)
{
    if (index > 0 && index <= n_log) {
        return logs[index - 1];
    } else {
        return NULL;
    }
}

inline static int
Log_protocol_check(void *_self)
{
    uint16_t protocol;
    protobuf_get(_self, PACKET_PROTOCOL, &protocol);
    if (protocol != PROTO_LOG && protocol != PROTO_SYSTEM) {
        protobuf_set(_self, PACKET_ERRORCODE, AAOS_EPROTOWRONG);
        protobuf_set(_self, PACKET_LENGTH, 0);
        return AAOS_EPROTOWRONG;
    } else {
        return AAOS_OK;
    }
}

int
log_get_index_by_facility(void *_self, const char *facility)
{
    const struct LogClass *class = (const struct LogClass *) classOf(_self);
        
    if (isOf(class, SerialClass()) && class->get_index_by_facility.method) {
        return ((int (*)(void *, const char *)) class->get_index_by_facility.method)(_self, facility);
    } else {
        int result;
        forward(_self, &result, (Method) log_get_index_by_facility, "get_index_by_facility", _self, facility);
        return result;
    } 
}

static int
Log_get_index_by_facility(void *_self, const char *facility)
{
    struct Log *self = cast(Serial(), _self);
    size_t length;
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_LOG);
    protobuf_set(self, PACKET_COMMAND, LOG_COMMAND_GET_INDEX_BY_FACILITY);
    protobuf_set(self, PACKET_INDEX, 0);
    
    length = strlen(facility);
    if (length < PACKETPARAMETERSIZE) {
        char *s;
        protobuf_get(self, PACKET_STR, &s);
        if (s != facility) {
            snprintf(s, PACKETPARAMETERSIZE, "%s", facility);
        }
        protobuf_set(self, PACKET_LENGTH, 0);
    } else {
        char *buf;
        protobuf_get(self, PACKET_BUF, &buf, NULL);
        if (facility != buf) {
            protobuf_set(self, PACKET_BUF, facility, length + 1);
        }
        uint32_t len = (uint32_t) length;
        protobuf_set(self, PACKET_LENGTH, len + 1);
    }
    
    return rpc_call(self);
}

int
log_submit(void *_self, const char *msg)
{
    const struct LogClass *class = (const struct LogClass *) classOf(_self);
    
    if (isOf(class, LogClass()) && class->sumit.method) {
        return ((int (*)(void *, const char *)) class->info.method)(_self, msg);
    } else {
        int result;
        forward(_self, &result, (Method) log_submit, "submit", _self, msg);
        return result;
    }
}

static int
Log_submit(void *_self, const char *msg)
{
    struct Log *self = cast(Log(), _self);
    
    void *protobuf = self->_.protobuf;
    int ret;
    
    protobuf_set(protobuf, PACKET_PROTOCOL, PROTO_LOG);
    protobuf_set(protobuf, PACKET_COMMAND, LOG_COMMAND_SUBMIT);
    
    return ret;
}

static int
Log_execute_get_index_by_facility(void *_self)
{
    char *facilyt;
    int index, ret;
    uint16_t idx;
    uint32_t length;

    protobuf_get(self, PACKET_LENGTH, &length);
    /*
     * if lenght == 0, use str field.
     */
    if (length == 0) {
        protobuf_get(self, PACKET_STR, &facility);
    } else {
        protobuf_get(self, PACKET_BUF, &name, NULL);
    }
    
    if ((ret = get_index_by_name(facility, &index)) != AAOS_OK) {
        return ret;
    } else {
        idx = (uint16_t) index;
        protobuf_set(self, PACKET_INDEX, idx);
        protobuf_set(self, PACKET_LENGTH, 0);
    }

    return AAOS_OK;
}

static int
Log_execute_submit(void *_self)
{
    struct Log *self = cast(Log(), _self);

    void *log_; 
    uint16_t idx, level;
    char *msg;
    int ret = AAOS_OK;

    protobuf_get(self, PACKET_INDEX, &idx);
    protobuf_get(self, PACKET_CHANNEL, &level);
    protobuf_get(self, PACKET_BUF, &msg, NULL);

    log_ = get_log_by_index(idx);

    if (log_ != NULL) {
        __log_write(log_, level, msg);
    }

}

static int
Log_execute_default(void *_self)
{
    return AAOS_EBADCMD;
}


static int
Log_execute(void *_self)
{
    struct Log *self = cast(Log(), _self);

    uint16_t command;
    
    if (Log_protocol_check(self) != AAOS_OK) {
        return AAOS_EPROTOWRONG;
    }
    protobuf_get(self, PACKET_COMMAND, &command);
    switch (command) {
        case LOG_COMMAND_GET_INDEX_BY_FACILITY:
            return Log_execute_get_index_by_facility(self);
            break;
        case SERIAL_COMMAND_SUBMIT:
            return Log_execute_submit(self);
            break;
        default:
            return Log_execute_default(self);
            break;
    }

    return AAOS_OK;
}

static int
Log_read(void *_self)
{
    struct RPC *self = cast(RPC(), _self);
    
    void *protobuf = self->protobuf, *header, *buf;
    int ret;
    uint32_t length;
    size_t payload;
    uint16_t errorcode;
    
    for (; ;) {
        header = protobuf_header(protobuf);
        if ((ret = tcp_socket_read_nb(self, header, PACKETHEADERSIZE, NULL)) != AAOS_OK) {
            return -1 * ret;
        }
        protobuf_get(self, PACKET_LENGTH, &length);
        if (length == 0) {
            /*
             * execute.
             */
            rpc_execute(self);
            return ret;
        }
        payload = protobuf_payload(protobuf);
        if (payload < length) {
            if ((ret = protobuf_reallocate(self, (size_t) length)) != AAOS_OK) {
                return ret;
            }
        }
        protobuf_get(protobuf, PACKET_BUF, &buf, NULL);
        if ((ret = tcp_socket_read_nb(self, buf, (size_t) length, NULL)) != AAOS_OK) {
            return -1 * ret;
        }
        rpc_execute(self);
        protobuf_get(self, PACKET_ERRORCODE, &errorcode);
    }
    
    return errorcode;
}

static int
Log_write(void *_self)
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

static int
Log_process(void *_self)
{
    int ret;
    uint16_t command, error_code;

    while ((ret = rpc_read(_self) != AAOS_OK)) {
        ret = rpc_execute(_self);
        protobuf_get(_self, PACKET_COMMAND, &command);
        if (command != LOG_COMMAND_SUBMIT) {
            error_code = (uint16_t) ret;
            protobuf_set(_self, PACKET_ERRODCODE, error_code);
            rpc_write(_self);        
        }
    }

    return ret;
}

static int
Log_call(void *_self)
{
    int ret;
    uint16_t command;
    
    if ((ret = rpc_write(_self)) != AAOS_OK) {
        return ret;
    }

    protobuf_get(_self, PACKET_COMMAND, &command);

    if (command == LOG_COMMAND_SUBMIT) {
        return ret;
    }

    return rpc_read(_self);
}

static int
LogClient_connect(void *_self, void **client)
{
    struct LogClient *self = cast(LogClient(), _self);

    int cfd;
    
    if (self->_._.address != NULL && Access(self->_._.address, F_OK) == 0) {
        cfd = Un_stream_connect_nb(self->_._.address, LOG_CONNECT_TIMEOUT);
    } else {
        cfd = Tcp_connect_nb(self->_._.address, self->_._.port, NULL, NULL, LOG_CONNECT_TIMEOUT);
    }
    
    if (cfd < 0) {
        *client = NULL;
        return AAOS_ERROR;
    } else {
        *client = new(Log(), cfd);
    }
    
    protobuf_set(*client, PACKET_PROTOCOL, PROTO_LOG);
    
    return AAOS_OK;
}


static const void *log_server_virtual_table(void);


static int
LogServer_accept(void *_self, void **client)
{
    struct RPCServer *self = cast(RPCServer(), _self);
    
    int lfd, cfd;
    
    lfd = tcp_server_get_lfd(self);
    cfd = Accept(lfd, NULL, NULL);
    if (cfd < 0) {
        *client = NULL;
        return AAOS_ERROR;
    } else {
        if ((*client = new(Log(), cfd)) == NULL) {
            Close(cfd);
            return AAOS_ERROR;
        }
        Fcntl(cfd, F_SETFL, O_NONBLOCK);
        return AAOS_OK;
    }
}

static int
LogServer_accept2(void *_self, void **client)
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
        if ((*client = new(Log(), cfd)) == NULL) {
            Close(cfd);
            return AAOS_ERROR;
        }
        Fcntl(cfd, F_SETFL, O_NONBLOCK);
        return AAOS_OK;
    }
}

static void *
LogServer_thr(void *arg)
{
    struct LogServer *myarg = (struct LogServer *) arg;
    
    int lfd = myarg->_._.lfd, lfd2 = myarg->_._.lfd2, cfd;
    int ret;
    void *client;
#ifdef LINUX
    int efd, nfds, timeout, i;
    struct epoll_event ev, *evs;
    if (self->timeout < 0) {
        timeout = -1;
    } else {
        tiemout = (int) self->timeout * 1000;
    }
    evs = (struct epoll_event *) Malloc(myarg->n_event * sizeof(struct epoll_event));
    efd = epoll_create(1);
    if (lfd != -1) {
        ev.events = EPOLLIN;
        ev.data.fd = lfd;
        epoll_ctl(efd, EPOLL_CTL_ADD, lfd, &ev);
    }
    if (lfd2 != -1) {
        ev.events = EPOLLIN;
        ev.data.fd = lfd2;
        epoll_ctl(efd, EPOLL_CTL_ADD, lfd2, &ev);
    }
    for (; ;) {
        if ((nfds = epoll_wait(efd, evs, myarg->n_event, timeout)) != -1) {
            for (i = 0; i < nfds; i++) {
                if (evs[i].data.ptr != NULL && ((struct Object *)evs[i].data.ptr)->magic == MAGIC) {
                    ret = rpc_process(client);
                    if (ret != AAOS_OK && ret != AAOS_EAGAIN) {
                        cfd = tcp_socket_get_sockfd(client);
                        epoll_ctl(efd, EPOLL_CTL_DEL, cfd, &ev);
                        delete(client);
                    }
                } else {
                    *client = NULL;
                    if (evs[i].data.fd == lfd) {
                        LogServer_accept(myarg, &client);
                    } else if (evs[i].data.fd == lfd2) {
                        LogServer_accept2(myarg, &client);
                    }
                    if (client != NULL) {
                        ev.events = EPOLLIN | EPOLLET;
                        ev.data.ptr = client;
                        epoll_ctl(efd, EPOLL_CTL_ADD, tcp_socket_get_sockfd(client), &ev);
                    }
                }
            }
        }
    }
#endif
#ifdef MACOSX
    int kq, nchanges = 0, nevents = (int) myarg->n_event, n, i, j;
    struct kevent kev, *changelist, *eventlist;
    struct timespec tp;
    tp->tv_sec = floor(self->timeout);
    tp->tv_nsec = (self->timeout - tp->tv_sec) * 1000000000;
    
    changelist = (struct kevent *) Malloc(sizeof(struct kevent) * myarg->n_event);
    memset(changelist, '\0', sizeof(struct kevent) * myarg->n_event);
    eventlist = (struct kevent *) Malloc(sizeof(struct kevent) * myarg->n_event);
    
    if (lfd != -1) {
        EV_SET(&changelist[nchanges], lfd, EVFILT_READ, EV_ADD, 0, 0, myarg);
        nchanges++;
    }
    if (lfd2 != -1) {
        EV_SET(&changelist[nchanges], lfd2, EVFILT_READ, EV_ADD, 0, 0, myarg);
        nchanges++;
    }
    nchanges = myarg->n_event;
    for (; ;) {
        if ((n = kevent(kq, changelist, nchanges, eventlist, nevents, &tp)) > 0) {
            for (i = 0; i < n; i++) {
                if (eventlist[i].ident == lfd) {
                    ret = LogServer_accept(myarg, &client);
                    if (ret == AAOS_OK && client != NULL) {
                        for (j = 0; i < nchanges; j++) {
                            if (changelist[j].udata == NULL) {
                                EV_SET(&changelist[j], tcp_socket_get_sockfd(client), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, client);
                                break;
                            }
                        }
                        if (j == nchanges) {
                            delete(client);
                        }
                    }
                } else if (eventlist[i].ident == lfd2) {
                    ret = LogServer_accept2(myarg, &client);
                    if (ret == AAOS_OK && client != NULL) {
                        for (j = 0; j < nchanges; i++) {
                            if (changelist[j].udata == NULL) {
                                EV_SET(&changelist[j], tcp_socket_get_sockfd(client), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, client);
                                break;
                            }
                        }
                        if (i == nchanges) {
                            delete(client);
                        }
                    }
                } else {
                    client = eventlist[i].udata;
                    cfd = eventlist[i].ident;
                    if (client != NULL && tcp_socket_get_sockfd(client) == cfd) {
                        ret = rpc_process(client);
                        if (ret != AAOS_OK && ret != AAOS_EAGAIN) {
                            for (j = 0; j < nchanges; j++) {
                                if (client == changelist[j].udata) {
                                    EV_SET(&changelist[j], cfd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
                                    delete(client);
                                    break;
                                }
                            }
                            
                        }
                    }
                }
            }
            
        }
    }
#endif
    
    return NULL;
}

static int
LogServer_start(void *_self)
{
    struct LogServer *self = cast(LogServer(), _self);
    
    size_t i;
    pthread_t *tids;
    
    tids = (pthread_t *) Malloc(sizeof(pthread_t) * self->n_thread);
    
    
    if (self->_._.address != NULL && self->_._.port != NULL && (self->_._.lfd = Tcp_listen(self->_._.address, self->_._.port, NULL, NULL)) >= 0) {
        Fcntl(self->_._.lfd, F_SETFL, O_NONBLOCK);
        self->count++;
    }
    if (self->_._.path != NULL && (self->_._.lfd2 = self->_._.lfd2 = Un_stream_listen(self->_._.path)) >= 0) {
        Fcntl(self->_._.lfd2, F_SETFL, O_NONBLOCK);
        self->count++;
    }
    
    for (i = 0; i < self->n_thread; i++) {
        Pthread_create(&tids[i], NULL, LogServer_thr, self);
    }
    
    for (i = 0; i < self->n_thread; i++) {
        Pthread_join(tids[i], NULL);
    }
    return AAOS_OK;
}

static void *
LogServer_ctor(void *_self, va_list *app)
{
    struct LogServer *self = super_ctor(LogServer(), _self, app);
    
    self->_._vtab = log_server_virtual_table();
    
    return (void *) self;
}

static void *
LogServer_dtor(void *_self)
{
    //struct RPC *self = cast(RPC(), _self);
    
    return super_dtor(LogServer(), _self);
}

static void *
LogServerClass_ctor(void *_self, va_list *app)
{
    struct LogServerClass *self = super_ctor(LogServerClass(), _self, app);
    
    self->_.accept.method = (Method) 0;
    self->_.accept2.method = (Method) 0;
    self->_.start.method = (Method) 0;
    
    return self;
}

static void *_LogServerClass;

static void
LogServerClass_destroy(void)
{
    free((void *) _LogServerClass);
}

static void
LogServerClass_initialize(void)
{
    _LogServerClass = new(RPCServerClass(), "LogServerClass", RPCServerClass(), sizeof(struct LogServerClass),
                          ctor, "ctor", LogServerClass_ctor,
                          (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(LogServerClass_destroy);
#endif
}

const void *
LogServerClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, LogServerClass_initialize);
#endif
    
    return _LogServerClass;
}

static void *_LogServer;

static void
LogServer_destroy(void)
{
    free((void *) _LogServer);
}

static void
LogServer_initialize(void)
{
    _LogServer = new(LogServerClass(), "LogServer", RPCServer(), sizeof(struct LogServer),
                     ctor, "ctor", LogServer_ctor,
                     dtor, "dtor", LogServer_dtor,
                     (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(LogServer_destroy);
#endif
}

const void *
SerialServer(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, LogServer_initialize);
#endif
    
    return _LogServer;
}

static const void *_log_server_virtual_table;

static void
log_server_virtual_table_destroy(void)
{
    delete((void *) _log_server_virtual_table);
}

static void
log_server_virtual_table_initialize(void)
{
    _log_server_virtual_table = new(RPCServerVirtualTable(),
                                    //rpc_server_accept, "accept", SerialServer_accept,
                                    //rpc_server_accept2, "accept2", SerialServer_accept2,
                                    (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(log_server_virtual_table_destroy);
#endif
}

static const void *
log_server_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, log_server_virtual_table_initialize);
#endif
    
    return _log_server_virtual_table;
}




