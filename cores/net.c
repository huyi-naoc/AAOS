//
//  net.c
//  AAOS
//
//  Created by huyi on 2018/11/8.
//  Copyright © 2018年 National Astronomical Observatories, Chinese Academy of Sciences. All rights reserved.
//

#include "def.h"
#include "net_r.h"
#include "net.h"
#include "wrapper.h"

/*
 * TCP socket virtual table
 */

static void *
TCPSocketVirtualTable_ctor(void *_self, va_list *app)
{
    struct TCPSocketVirtualTable *self = super_ctor(TCPSocketVirtualTable(), _self, app);
    Method selector;
    
    while ((selector = va_arg(*app, Method))) {
        const char *tag = va_arg(*app, const char *);
        Method method = va_arg(*app, Method);
        if (selector == (Method) tcp_socket_read) {
            if (tag) {
                self->read.tag = tag;
                self->read.selector = selector;
            }
            self->read.method = method;
            continue;
        }
        if (selector == (Method) tcp_socket_write) {
            if (tag) {
                self->write.tag = tag;
                self->write.selector = selector;
            }
            self->write.method = method;
            continue;
        }
    }
    return _self;
}

static void *
TCPSocketVirtualTable_dtor(void *_self)
{
    return super_dtor(Object(), _self);
}

static const void *_TCPSocketVirtualTable;

static void
TCPSocketVirtualTable_destroy(void)
{
    free((void *) _TCPSocketVirtualTable);
}

static void
TCPSocketVirtualTable_initialize(void)
{
    _TCPSocketVirtualTable = new(VirtualTableClass(), "TCPSocketVirtualTable", VirtualTable(), sizeof(struct TCPSocketVirtualTable),
                                 ctor, "ctor", TCPSocketVirtualTable_ctor,
                                 dtor, "dtor", TCPSocketVirtualTable_dtor,
                                 (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(TCPSocketVirtualTable_destroy);
#endif
}

const void *
TCPSocketVirtualTable(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, TCPSocketVirtualTable_initialize);
#endif
    return _TCPSocketVirtualTable;
}

/*
 * TCP socket
 */

int
tcp_socket_read(void *_self, void *read_buffer, size_t request_size, size_t *read_size)
{
    const struct TCPSocketClass *class = (const struct TCPSocketClass*) classOf(_self);
    int result;
    
    if (isOf(class, TCPSocketClass()) && class->read.method) {
        result = ((int (*)(void *, void *, size_t, size_t *)) class->read.method)( _self, read_buffer, request_size, read_size);
    } else {
        forward(_self, &result, (Method) tcp_socket_read, "read", _self, read_buffer, request_size, read_size);
    }
    
    return result;
}

static int
TCPSocket_read(const void *_self, void *read_buffer, size_t request_size, size_t *read_size)
{
    struct TCPSocket *self = cast(TCPSocket(), _self);
    ssize_t n;
    
    if (self->option & TCPSOCKET_OPTION_DO_NOT_RESTART_ON_SIGNAL) {
        n = Readn2(self->sockfd, read_buffer, request_size);
    } else {
        n = Readn(self->sockfd, read_buffer, request_size);
    }
    
    if (n > 0) {
        if (read_size) {
            *read_size = n;
        }
        return AAOS_OK;
    } else if (n == 0) {
        if (read_size) {
            *read_size = 0;
        }
        return AAOS_ECLOSED;
    } else {
        if (read_size) {
            *read_size = 0;
        }
        switch (errno) {
            case EINTR:
                return AAOS_EINTR;
                break;
            case ECONNRESET:
                return AAOS_ECONNRESET;
                break;
            case ETIMEDOUT:
                return AAOS_ETIMEDOUT;
                break;
            default:
                return AAOS_ERROR;
                break;
        }
    }
}

int
tcp_socket_read_until(void *_self, void *read_buffer, size_t request_size, size_t *read_size, const char *delim)
{
    const struct TCPSocketClass *class = (const struct TCPSocketClass*) classOf(_self);
    int result;
    
    if (isOf(class, TCPSocketClass()) && class->read_until.method) {
        result = ((int (*)(void *, void *, size_t, size_t *, const char *)) class->read_until.method)( _self, read_buffer, request_size, read_size, delim);
    } else {
        forward(_self, &result, (Method) tcp_socket_read_until, "read_until", _self, read_buffer, request_size, read_size, delim);
    }
    
    return result;
}

static int
TCPSocket_read_until(const void *_self, void *read_buffer, size_t request_size, size_t *read_size, const char *delim)
{
    struct TCPSocket *self = cast(TCPSocket(), _self);
    ssize_t n, nleft = request_size;
    size_t nread = 0, size = strlen(delim);
    char *s = (char *) read_buffer;
    
    while (nleft > 0) {
        n = Read(self->sockfd, s, nleft);
        if (n < 0) {
            if (read_size) {
                *read_size = 0;
            }
            switch (errno) {
                case EINTR:
                    return AAOS_EINTR;
                    break;
                case ECONNRESET:
                    return AAOS_ECONNRESET;
                    break;
                case ETIMEDOUT:
                    return AAOS_ETIMEDOUT;
                    break;
                default:
                    return AAOS_ERROR;
                    break;
            }
        } else if (n == 0) {
            if (read_size) {
                *read_size = 0;
            }
            return AAOS_ECLOSED;
        } else {
            nread += n;
            nleft -= n;
            s += n;
            if (memcpy(s - size, delim, size) == 0) {
                break;
            }
        }
    }
    if (read_size) {
        *read_size = nread;
    }
    return AAOS_OK;
}


int
tcp_socket_write(void *_self, const void *write_buffer, size_t request_size, size_t *write_size)
{
    const struct TCPSocketClass *class = (const struct TCPSocketClass*) classOf(_self);
    int result;
    
    if (isOf(class, TCPSocketClass()) && class->write.method) {
        result = ((int (*)(void *, const void *, size_t, size_t *)) class->write.method)(_self, write_buffer, request_size, write_size);
    } else {
        forward(_self, &result, (Method) tcp_socket_write, "write", _self, write_buffer, request_size, write_size);
    }
    
    return result;
}

static int
TCPSocket_write(void *_self, const void *write_buffer, size_t request_size, size_t *write_size)
{
    struct TCPSocket *self = cast(TCPSocket(), _self);
    ssize_t n;
    
    if (self->option & TCPSOCKET_OPTION_DO_NOT_RESTART_ON_SIGNAL) {
        n = Writen2(self->sockfd, write_buffer, request_size);
    } else {
        n = Writen(self->sockfd, write_buffer, request_size);
    }
    
    if (n > 0) {
        if (write_size) {
            *write_size = n;
        }
        return AAOS_OK;
    } else {
        if (write_size) {
            *write_size = 0;
        }
        switch (errno) {
            case EINTR:
                return AAOS_EINTR;
                break;
            case ECONNRESET:
                return AAOS_ECONNRESET;
                break;
            case ENETDOWN:
                return AAOS_ENETDOWN;
                break;
            case ENETUNREACH:
                return AAOS_ENETUNREACH;
                break;
            case EPIPE:
                return AAOS_EPIPE;
                break;
            default:
                return AAOS_ERROR;
                break;
        }
    }
}

int
tcp_socket_get_sockfd(const void *_self)
{
    const struct TCPSocketClass *class = (const struct TCPSocketClass*) classOf(_self);
    int result;
    
    if (isOf(class, TCPSocketClass()) && class->get_sockfd.method) {
        result = ((int (*)(const void *)) class->get_sockfd.method)( _self);
    } else {
        forward(_self, &result, (Method) tcp_socket_get_sockfd, "get_sockfd", _self);
    }
    
    return result;
}

static int
TCPSocket_get_sockfd(const void *_self)
{
    const struct TCPSocket *self = cast(TCPSocket(), _self);
    
    return self->sockfd;
}

unsigned int
tcp_socket_set_option(void *_self, unsigned int option)
{
    struct TCPSocketClass *class = (struct TCPSocketClass*) classOf(_self);
    unsigned int result;
    
    if (isOf(class, TCPSocketClass()) && class->set_option.method) {
        result = ((unsigned int (*)(void *, unsigned int)) class->set_option.method)( _self, option);
    } else {
        forward(_self, &result, (Method) tcp_socket_get_sockfd, "set_option", _self, option);
    }
    
    return result;
}

static unsigned int
TCPSocket_set_option(void *_self, unsigned int option)
{
    struct TCPSocket *self = cast(TCPSocket(), _self);
    
    unsigned int old_option = self->option;
    self->option |= option;
    
    if (!(old_option&TCPSOCKET_OPTION_NOBLOCKING) && (option&TCPSOCKET_OPTION_NOBLOCKING)) {
        Fcntl(self->sockfd, F_SETFL, O_NONBLOCK);
    }

    return old_option;
}

int
tcp_socket_get_host_info(void *_self, char *addr, size_t addr_size, char *port, size_t port_size)
{
    struct TCPSocketClass *class = (struct TCPSocketClass*) classOf(_self);
    int result;
    
    if (isOf(class, TCPSocketClass()) && class->get_host_info.method) {
        result = ((unsigned int (*)(void *, char *, size_t, char *, size_t)) class->get_host_info.method)( _self, addr, addr_size, port, port_size);
    } else {
        forward(_self, &result, (Method) tcp_socket_get_host_info, "get_host_info", _self, addr, addr_size, port, port_size);
    }
    
    return result;
}

static int
TCPSocket_get_host_info(void *_self, char *addr, size_t addr_size, char *port, size_t port_size)
{
    struct TCPSocket *self = cast(TCPSocket(), _self);
    
    socklen_t address_len = sizeof(SA);
    SA sa;
    char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
    
    getsockname(self->sockfd, &sa, &address_len);
    getnameinfo(&sa, sizeof(sa), hbuf, sizeof(hbuf), sbuf, sizeof(sbuf), 0);
    
    if (addr != NULL) {
        snprintf(addr, addr_size, "%s", hbuf);
    }
    if (port != NULL) {
        snprintf(port, port_size, "%s", sbuf);
    }
    
    return AAOS_OK;
}

int
tcp_socket_get_peer_info(void *_self, char *addr, size_t addr_size, char *port, size_t port_size)
{
    struct TCPSocketClass *class = (struct TCPSocketClass*) classOf(_self);
    int result;
    
    if (isOf(class, TCPSocketClass()) && class->get_peer_info.method) {
        result = ((unsigned int (*)(void *, char *, size_t, char *, size_t)) class->get_peer_info.method)( _self, addr, addr_size, port, port_size);
    } else {
        forward(_self, &result, (Method) tcp_socket_get_peer_info, "get_peer_info", _self, addr, addr_size, port, port_size);
    }
    
    return result;
}

static int
TCPSocket_get_peer_info(void *_self, char *addr, size_t addr_size, char *port, size_t port_size)
{
    struct TCPSocket *self = cast(TCPSocket(), _self);
    
    socklen_t address_len = sizeof(SA);
    SA sa;
    char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
    
    getpeername(self->sockfd, &sa, &address_len);
    getnameinfo(&sa, sizeof(sa), hbuf, sizeof(hbuf), sbuf, sizeof(sbuf), 0);
    
    if (addr != NULL) {
        snprintf(addr, addr_size, "%s", hbuf);
    }
    if (port != NULL) {
        snprintf(port, port_size, "%s", sbuf);
    }
    
    return AAOS_OK;
}


static void
TCPSocket_forward(const void *_self, void *result, Method selector, const char *name, va_list *app)
{
    struct TCPSocket *self = cast(TCPSocket(), _self);
    Method method = virtualTo(self->_vtab, name);
    
    void *obj = va_arg(*app, void *);
    if (selector == (Method) tcp_socket_read) {
        void *read_buffer = va_arg(*app, void *);
        size_t request_size = va_arg(*app, size_t);
        size_t *read_size = va_arg(*app, size_t *);
        *((void **) result) = ((void * (*)(void *, void *, size_t, size_t *)) method)(obj, read_buffer, request_size, read_size);
    } else if (selector == (Method) tcp_socket_write) {
        const void *write_buffer = va_arg(*app, const void *);
        size_t request_size = va_arg(*app, size_t);
        size_t *write_size = va_arg(*app, size_t *);
        *((void **) result) = ((void * (*)(void *, const void *, size_t, size_t *)) method)(obj, write_buffer, request_size, write_size);
    } else {
        assert(0);
    }
}


static void *
TCPSocket_cctor(void *_self, const void *_from)
{
    struct TCPSocket *self = super_cctor(TCPSocket(), _self);
    const struct TCPSocket *from = cast(TCPSocket(), _from);
    
    self->_vtab = from->_vtab;
    self->sockfd = dup(from->sockfd);
    self->option = from->option;
    
    return self;
}

static void *
TCPSocket_mctor(void *_self, void *_from)
{
    struct TCPSocket *self = super_mctor(TCPSocket(), _self);
    struct TCPSocket *from = cast(TCPSocket(), _from);
    
    self->_vtab = from->_vtab;
    self->option = from->option;
    self->sockfd = from->sockfd;
    self->timeout = from->timeout;
    from->sockfd = -1;
    
    return self;
}

static void *
TCPSocket_ctor(void *_self, va_list *app)
{
    struct TCPSocket *self = super_ctor(TCPSocket(), _self, app);
    int sockfd;
    
    sockfd = va_arg(*app, int);
    self->sockfd = sockfd;
    
    return (void *) self;
}

static void *
TCPSocket_dtor(void *_self)
{
    struct TCPSocket *self = cast(TCPSocket(), _self);
    
    if (self->sockfd >= 0) {
        Close(self->sockfd);
    }

    return super_dtor(TCPSocket(), _self);
}

static void *
TCPSocketClass_ctor(void *_self, va_list *app)
{
    struct TCPSocketClass *self = super_ctor(TCPSocketClass(), _self, app);
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
        
        if (selector == (Method) tcp_socket_read) {
            if (tag) {
                self->read.tag = tag;
                self->read.selector = selector;
            }
            self->read.method = method;
            continue;
        }
        if (selector == (Method) tcp_socket_read_until) {
            if (tag) {
                self->read_until.tag = tag;
                self->read_until.selector = selector;
            }
            self->read_until.method = method;
            continue;
        }
        if (selector == (Method) tcp_socket_write) {
            if (tag) {
                self->write.tag = tag;
                self->write.selector = selector;
            }
            self->write.method = method;
            continue;
        }
        if (selector == (Method) tcp_socket_get_sockfd) {
            if (tag) {
                self->get_sockfd.tag = tag;
                self->get_sockfd.selector = selector;
            }
            self->get_sockfd.method = method;
            continue;
        }
        if (selector == (Method) tcp_socket_set_option) {
            if (tag) {
                self->set_option.tag = tag;
                self->set_option.selector = selector;
            }
            self->set_option.method = method;
            continue;
        }
        if (selector == (Method) tcp_socket_get_host_info) {
            if (tag) {
                self->get_host_info.tag = tag;
                self->get_host_info.selector = selector;
            }
            self->get_host_info.method = method;
            continue;
        }
        if (selector == (Method) tcp_socket_get_peer_info) {
            if (tag) {
                self->get_peer_info.tag = tag;
                self->get_peer_info.selector = selector;
            }
            self->get_peer_info.method = method;
            continue;
        }
    }
    
#ifdef va_copy
    va_end(ap);
#endif
    
    return self;
}

static const void *_TCPSocketClass;

static void
TCPSocketClass_destroy(void)
{
    free((void *)_TCPSocketClass);
}

static void
TCPSocketClass_initialize(void)
{
    _TCPSocketClass = new(Class(), "TCPSocketClass", Class(), sizeof(struct TCPSocketClass),
                          ctor, "ctor", TCPSocketClass_ctor,
                          (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(TCPSocketClass_destroy);
#endif
}

const void *
TCPSocketClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, TCPSocketClass_initialize);
#endif
    
    return _TCPSocketClass;
}

static const void *_TCPSocket;

static void
TCPSocket_destroy(void)
{
    free((void *)_TCPSocket);
}

static void
TCPSocket_initialize(void)
{
    _TCPSocket = new(TCPSocketClass(), "TCPSocket", Object(), sizeof(struct TCPSocket),
                     ctor, "ctor", TCPSocket_ctor,
                     cctor, "cctor", TCPSocket_cctor,
                     mctor, "mctor", TCPSocket_mctor,
                     dtor, "dtor", TCPSocket_dtor,
                     forward, "forward", TCPSocket_forward,
                     tcp_socket_read, "read", TCPSocket_read,
                     tcp_socket_read_until, "read_until", TCPSocket_read_until,
                     tcp_socket_write, "write", TCPSocket_write,
                     tcp_socket_get_sockfd, "get_socket", TCPSocket_get_sockfd,
                     tcp_socket_set_option, "set_option", TCPSocket_set_option,
                     tcp_socket_get_host_info, "get_host_info", TCPSocket_get_host_info,
                     tcp_socket_get_peer_info, "get_peer_info", TCPSocket_get_peer_info,
                     (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(TCPSocket_destroy);
#endif
}

const void *
TCPSocket(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, TCPSocket_initialize);
#endif
    return _TCPSocket;
}

/*
 * TCP client virtual table
 */

static void *
TCPClientVirtualTable_ctor(void *_self, va_list *app)
{
    struct TCPClientVirtualTable *self = super_ctor(TCPClientVirtualTable(), _self, app);
    Method selector;
    
    while ((selector = va_arg(*app, Method))) {
        const char *tag = va_arg(*app, const char *);
        Method method = va_arg(*app, Method);
        if (selector == (Method) tcp_client_connect) {
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
TCPClientVirtualTable_dtor(void *_self)
{
    return super_dtor(Object(), _self);
}

static const void *_TCPClientVirtualTable;

static void
TCPClientVirtualTable_destroy(void)
{
    free((void *) _TCPClientVirtualTable);
}

static void
TCPClientVirtualTable_initialize(void)
{
    _TCPClientVirtualTable = new(VirtualTableClass(), "TCPClientVirtualTable", VirtualTable(), sizeof(struct TCPClientVirtualTable),
                                 ctor, "ctor", TCPClientVirtualTable_ctor,
                                 dtor, "dtor", TCPClientVirtualTable_dtor,
                                 (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(TCPClientVirtualTable_destroy);
#endif
}

const void *
TCPClientVirtualTable(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, TCPClientVirtualTable_initialize);
#endif
    return _TCPClientVirtualTable;
}

/*
 * TCP client
 */

int
tcp_client_connect(const void *_self, void **client)
{
    const struct TCPClientClass *class = (const struct TCPClientClass*) classOf(_self);
    
    
    if (isOf(class, TCPClientClass()) && class->connect.method) {
        return ((int (*)(const void *, void **)) class->connect.method)( _self, client);
    } else {
        int result;
        forward(_self, &result, (Method) tcp_client_connect, "connect", _self, client);
        return result;
    }
}

static int
TCPClient_connect(const void *_self, void **client)
{
    struct TCPClient *self = cast(TCPClient(), _self);
    int fd;
    
    fd = Tcp_connect(self->address, self->port, NULL, NULL);
    if (fd < 0) {
        return AAOS_ERROR;
    } else {
        *client = new(TCPSocket(), fd);
    }
    return AAOS_OK;
    
}

static void
TCPClient_forward(const void *_self, void *result, Method selector, const char *name, va_list *app)
{
    struct TCPClient *self = cast(TCPClient(), _self);
    Method method = virtualTo(self->_vtab, name);
    void *obj = va_arg(*app, void *);
    
    if (selector == (Method) tcp_client_connect) {
        void **client = va_arg(*app, void **);
        *((int *) result) = ((int (*)(void *, void **)) method)(obj, client);
    } else {
        assert(0);
    }
}


static void *
TCPClient_ctor(void *_self, va_list *app)
{
    struct TCPClient *self = super_ctor(TCPClient(), _self, app);
    const char *s;
    
    s = va_arg(*app, const char *);
    self->address = (char *) Malloc(strlen(s) + 1);
    snprintf(self->address, strlen(s) + 1, "%s", s);
    s = va_arg(*app, const char *);
    self->port = (char *) Malloc(strlen(s) + 1);
    snprintf(self->port, strlen(s) + 1, "%s", s);
    
    return (void *) self;
}

static void *
TCPClient_dtor(void *_self)
{
    struct TCPClient *self = cast(TCPClient(), _self);
    
    free(self->address);
    free(self->port);
    
    return super_dtor(TCPSocket(), _self);
    
}

static void *
TCPClientClass_ctor(void *_self, va_list *app)
{
    struct TCPClientClass *self = super_ctor(TCPClientClass(), _self, app);
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
        
        if (selector == (Method) tcp_client_connect) {
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

static const void *_TCPClientClass;

static void
TCPClientClass_destroy(void)
{
    free((void *)_TCPClientClass);
}

static void
TCPClientClass_initialize(void)
{
    _TCPClientClass = new(Class(), "TCPClientClass", Class(), sizeof(struct TCPClientClass),
                          ctor, "ctor", TCPClientClass_ctor,
                          (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(TCPClientClass_destroy);
#endif
}

const void *
TCPClientClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, TCPClientClass_initialize);
#endif
    return _TCPClientClass;
}

static const void *_TCPClient;

static void
TCPClient_destroy(void)
{
    free((void *)_TCPClient);
}

static void
TCPClient_initialize(void)
{
    _TCPClient = new(TCPClientClass(), "TCPClient", Object(), sizeof(struct TCPClient),
                     ctor, "ctor", TCPClient_ctor,
                     dtor, "dtor", TCPClient_dtor,
                     tcp_client_connect, "connect", TCPClient_connect,
                     forward, "forward", TCPClient_forward,
                     (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(TCPClient_destroy);
#endif
}

const void *
TCPClient(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, TCPClient_initialize);
#endif
    return _TCPClient;
}

/*
 * TCP server virtual table
 */
static void *
TCPServerVirtualTable_ctor(void *_self, va_list *app)
{
    struct TCPServerVirtualTable *self = super_ctor(TCPServerVirtualTable(), _self, app);
    Method selector;
    
    while ((selector = va_arg(*app, Method))) {
        const char *tag = va_arg(*app, const char *);
        Method method = va_arg(*app, Method);
        if (selector == (Method) tcp_server_accept) {
            if (tag) {
                self->accept.tag = tag;
                self->accept.selector = selector;
            }
            self->accept.method = method;
            continue;
        }
    }
    
    return _self;
}

static void *
TCPServerVirtualTable_dtor(void *_self)
{
    return super_dtor(Object(), _self);
}

static const void *_TCPServerVirtualTable;

static void
TCPServerVirtualTable_destroy(void)
{
    free((void *) _TCPServerVirtualTable);
}

static void
TCPServerVirtualTable_initialize(void)
{
    _TCPServerVirtualTable = new(VirtualTableClass(), "TCPServerVirtualTable", VirtualTable(), sizeof(struct TCPServerVirtualTable),
                                 ctor, "ctor", TCPServerVirtualTable_ctor,
                                 dtor, "dtor", TCPServerVirtualTable_dtor,
                                 (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(TCPServerVirtualTable_destroy);
#endif
}

const void *
TCPServerVirtualTable(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, TCPServerVirtualTable_initialize);
#endif
    
    return _TCPServerVirtualTable;
}

/*
 * TCP server
 */

int
tcp_server_accept(void *_self, void **client)
{
    const struct TCPServerClass *class = (const struct TCPServerClass*) classOf(_self);
    
    if (isOf(class, TCPServerClass()) && class->accept.method) {
        return ((int (*)(const void *, void **)) class->accept.method)( _self, client);
    } else {
        int result;
        forward(_self, &result, (Method) tcp_server_accept, "accept", _self, client);
        return result;
    }
}

static int
TCPServer_accept(const void *_self, void **client)
{
    struct TCPServer *self = cast(TCPServer(), _self);
    int fd;
    
    fd = Accept(self->lfd, NULL, NULL);
    if (fd < 0) {
        *client = NULL;
        return AAOS_ERROR;
    } else {
        *client = new(TCPSocket(), fd);
        return AAOS_OK;
    }
}

int
tcp_server_get_lfd(const void *_self)
{
    const struct TCPServerClass *class = (const struct TCPServerClass*) classOf(_self);
    int result;
    
    if (isOf(class, TCPServerClass()) && class->get_lfd.method) {
        result = ((int (*)(const void *)) class->get_lfd.method)( _self);
    } else {
        forward(_self, &result, (Method) tcp_server_get_lfd, "get_lfd", _self);
    }
    
    return result;
}

static int
TCPServer_get_lfd(const void *_self)
{
    const struct TCPServer *self = cast(TCPServer(), _self);
    
    return self->lfd;
}


void
tcp_server_start(void *_self)
{
    struct TCPServerClass *class = (struct TCPServerClass*) classOf(_self);
    
    
    if (isOf(class, TCPServerClass()) && class->start.method) {
        ((void  (*)(void *)) class->start.method)( _self);
    } else {
        return forward(_self, 0, (Method) tcp_server_start, "start", _self);
    }
}

static void *
TCPServer_doit(void *arg)
{
    void *client = arg;
    
    Pthread_detach(pthread_self());
    char buf[1024];
    
    while (tcp_socket_read(client, buf, 1024, NULL) == AAOS_OK) {
        if (tcp_socket_write(client, buf, 1024, NULL) != AAOS_OK) {
            break;
        }
    }
    
    delete(client);
    
    return NULL;
}

static void
TCPServer_start(const void *_self)
{
    struct TCPServer *self = cast(TCPServer(), _self);
    void *client;
    pthread_t tid;
    
    self->lfd = Tcp_listen(NULL, self->port, NULL, NULL);
    
    int ret;
    
    for (;;) {
        if ((ret = tcp_server_accept((void *) _self, &client)) == AAOS_OK) {
            Pthread_create(&tid, NULL, TCPServer_doit, client);
        }
    }
}

static void
TCPServer_forward(const void *_self, void *result, Method selector, const char *name, va_list *app)
{
    struct TCPServer *self = cast(TCPServer(), _self);
    Method method = virtualTo(self->_vtab, name);
    
    void *obj = va_arg(*app, void *);
    if (selector == (Method) tcp_server_accept) {
        void **client = va_arg(*app, void **);
        *((void **) result) = ((void * (*)(void *, void **)) method)(obj, client);
    } else {
        assert(0);
    }
}

static void *
TCPServer_ctor(void *_self, va_list *app)
{
    struct TCPServer *self = super_ctor(TCPServer(), _self, app);
    const char *s;
    
    s = va_arg(*app, const char *);
    self->port = (char *) Malloc(strlen(s) + 1);
    snprintf(self->port, strlen(s) + 1, "%s", s);
    self->lfd = -1;
    
    return (void *) self;
}

static void *
TCPServer_dtor(void *_self)
{
    struct TCPServer *self = cast(TCPServer(), _self);
    
    if (self->lfd >= 0) {
        Close(self->lfd);
    }
    
    free(self->port);
    
    return super_dtor(TCPServer(), _self);
    
}

static void *
TCPServerClass_ctor(void *_self, va_list *app)
{
    struct TCPServerClass *self = super_ctor(TCPServerClass(), _self, app);
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
        
        if (selector == (Method) tcp_server_accept) {
            if (tag) {
                self->accept.tag = tag;
                self->accept.selector = selector;
            }
            self->accept.method = method;
            continue;
        }
        if (selector == (Method) tcp_server_start) {
            if (tag) {
                self->start.tag = tag;
                self->start.selector = selector;
            }
            self->start.method = method;
            continue;
        }
        if (selector == (Method) tcp_server_get_lfd) {
            if (tag) {
                self->get_lfd.tag = tag;
                self->get_lfd.selector = selector;
            }
            self->get_lfd.method = method;
            continue;
        }
    }
    
#ifdef va_copy
    va_end(ap);
#endif
    
    return self;
}

static const void *_TCPServerClass;

static void
TCPServerClass_destroy(void)
{
    free((void *)_TCPServerClass);
}

static void
TCPServerClass_initialize(void)
{
    _TCPServerClass = new(Class(), "TCPServerClass", Class(), sizeof(struct TCPServerClass),
                          ctor, "ctor", TCPServerClass_ctor,
                          (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(TCPServerClass_destroy);
#endif
}

const void *
TCPServerClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, TCPServerClass_initialize);
#endif
    return _TCPServerClass;
}

static const void *_TCPServer;

static void
TCPServer_destroy(void)
{
    free((void *)_TCPServer);
}

static void
TCPServer_initialize(void)
{
    _TCPServer = new(TCPServerClass(), "TCPServer", Object(), sizeof(struct TCPServer),
                     ctor, "ctor", TCPServer_ctor,
                     dtor, "dtor", TCPServer_dtor,
                     forward, "forward", TCPServer_forward,
                     tcp_server_accept, "accept", TCPServer_accept,
                     tcp_server_get_lfd, "get_lfd", TCPServer_get_lfd,
                     tcp_server_start, "start", TCPServer_start,
                     (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(TCPServer_destroy);
#endif
}

const void *
TCPServer(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, TCPServer_initialize);
#endif
    return _TCPServer;
}

#ifdef _USE_COMPILER_ATTRIBUTION_
static void __destructor__(void) __attribute__ ((destructor(_NET_PRIORITY_)));

static void
__destructor__(void)
{
    TCPSocketVirtualTable_destroy();
    
}

static void __constructor__(void) __attribute__ ((constructor(_NET_PRIORITY_)));

static void
__constructor__(void)
{
    TCPSocketVirtualTable_initializ();
    TCPSocketClass_initialize();
    TCPSocket_initialize();
    TCPClientVirtualTable_initialize();
    TCPClientClass_initialize();
    TCPClient_initialize();
    TCPServerClass_initialize();
    TCPServer_initialize();
}
#endif
