#include "serial_r.h"
#include "serial.h"

static void *
SerialVirtualTable_ctor(void *_self, va_list *app)
{
    struct SerialVirtualTable *self = super_ctor(SerialVirtualTable(), _self, app);
    Method selector;
    
    while ((selector = va_arg(*app, Method))) {
        const char *tag = va_arg(*app, const char *);
        Method method = va_arg(*app, Method);
        
        if (selector == (Method) serial_init) {
            if (tag) {
                self->init.tag = tag;
                self->init.selector = selector;
            }
            self->init.method = method;
            continue;
        }
        if (selector == (Method) serial_process) {
            if (tag) {
                self->process.tag = tag;
                self->process.selector = selector;
            }
            self->process.method = method;
            continue;
        }
        if (selector == (Method) serial_feed_dog) {
            if (tag) {
                self->feed_dog.tag = tag;
                self->feed_dog.selector = selector;
            }
            self->feed_dog.method = method;
            continue;
        }
    }
    
    return _self;
}

static void *
SerialVirtualTable_dtor(void *_self)
{
    return super_dtor(Object(), _self);
}

static const void *_SerialVirtualTable;

static void
SerialVirtualTable_destroy(void)
{
    free((void *) _SerialVirtualTable);
}

static void
SerialVirtualTable_initialize(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(SerialVirtualTable_destroy);
#endif
    _SerialVirtualTable = new(VirtualTableClass(), "SerialVirtualTable", VirtualTable(), sizeof(struct SerialVirtualTable),
                              ctor, "ctor", SerialVirtualTable_ctor,
                              dtor, "dtor", SerialVirtualTable_dtor,
                              (void *)0);
}

const void *
SerialVirtualTable(void)
{
 #ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, SerialVirtualTable_initialize);
#endif
    
    return _SerialVirtualTable;
}

static int
Serial_feed_dog(void *_self)
{
    return AAOS_OK;
}

int
serial_process(void *_self, uint32_t command, ...)
{
    const struct SerialClass *class = (const struct SerialClass *) classOf(_self);
    int result;
    va_list ap;
    va_start(ap, command);

    if (isOf(class, SerialClass()) && class->process.method) {
        result = ((int (*)(void *, uint32_t, va_list *)) class->process.method)(_self, command, &ap);
    } else {
        forward(_self, &result, (Method) serial_process, "process", _self, command, &ap);
    }
    
    va_end(ap);
    return result;
}

int
serial_init(void *_self)
{
    const struct SerialClass *class = (const struct SerialClass *) classOf(_self);
    
    if (isOf(class, SerialClass()) && class->init.method) {
        return ((int (*)(void *)) class->init.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) serial_init, "init", _self);
        return result;
    }
}

int
serial_feed_dog(void *_self) //pure virtual function
{
    const struct SerialClass *class = (const struct SerialClass *) classOf(_self);
    
    if (isOf(class, SerialClass()) && class->feed_dog.method) {
        return ((int (*)(void *)) class->feed_dog.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) serial_init, "init", _self);
        return result;
    }
}

int
serial_name_of(void *_self, const char *name)
{
    const struct SerialClass *class = (const struct SerialClass *) classOf(_self);
    
    
    if (isOf(class, SerialClass()) && class->name_of.method) {
        return ((int (*)(void *, const char *)) class->read.method)(_self, name);
        
    } else {
        int result;
        forward(_self, &result, (Method) serial_name_of, "name_of", _self, name);
        return result;
    }
}

static int
Serial_name_of(void *_self, const char *name)
{
    struct Serial *self = cast(Serial(), _self);
    
    return strcmp(self->name, name);
}

int
serial_path_of(void *_self, const char *pathname)
{
    const struct SerialClass *class = (const struct SerialClass *) classOf(_self);
    
    
    if (isOf(class, SerialClass()) && class->path_of.method) {
        return ((int (*)(void *, const char *)) class->read.method)(_self, pathname);
        
    } else {
        int result;
        forward(_self, &result, (Method) serial_path_of, "path_of", _self, pathname);
        return result;
    }
}

static int
Serial_path_of(void *_self, const char *pathname)
{
    struct Serial *self = cast(Serial(), _self);
    
    return strcmp(self->path, pathname);
}

int
serial_read(void *_self, void *read_buffer, size_t request_size, size_t *read_size)
{
    const struct SerialClass *class = (const struct SerialClass *) classOf(_self);
    
    
    if (isOf(class, SerialClass()) && class->read.method) {
        return ((int (*)(void *, void *, size_t, size_t *)) class->read.method)(_self, read_buffer, request_size, read_size);
        
    } else {
        int result;
        forward(_self, &result, (Method) serial_read, "read", _self, read_buffer, request_size, read_size);
        return result;
    }
}

int
serial_read(void *_self, void *read_buffer, size_t request_size, size_t *read_size)
{
    const struct SerialClass *class = (const struct SerialClass *) classOf(_self);
    
    
    if (isOf(class, SerialClass()) && class->read.method) {
        return ((int (*)(void *, void *, size_t, size_t *)) class->read.method)(_self, read_buffer, request_size, read_size);
        
    } else {
        int result;
        forward(_self, &result, (Method) serial_read, "read", _self, read_buffer, request_size, read_size);
        return result;
    }
}

static int
Serial_read(void *_self, void *read_buffer, size_t request_size, size_t *read_size)
{
    struct Serial *self = cast(Serial(), _self);
    int fd = self->fd;
    double timeout = self->read_timeout;
    int ret;
    ssize_t n = -1;
    struct timeval tv;
    fd_set readfds;
    
    
    if (fd == -1) {
        return AAOS_EINVAL;
    }
    
    tv.tv_sec = floor(timeout);
    tv.tv_usec = (timeout - tv.tv_usec) * 1000000;
    FD_ZERO(&readfds);
    
    ret = Select(fd + 1, &readfds, NULL, NULL, &tv);
    switch (ret) {
        case -1:
            return AAOS_ERROR;
            break;
        case 0:
            return AAOS_ETIMEDOUT;
        default:
            break;
    }
    
    if (FD_ISSET(fd, &readfds)) {
        if ((n = Read(fd, read_buffer, request_size)) < 0) {
            return AAOS_ERROR;
        } else {
            if (read_size) {
                *read_size = n;
            }
        }
    }
    
    return AAOS_OK;
}

int
serial_write(void *_self, const void *write_buffer, size_t request_size, size_t *write_size)
{
    const struct SerialClass *class = (const struct SerialClass *) classOf(_self);
    
    
    if (isOf(class, SerialClass()) && class->write.method) {
        return ((int (*)(void *, const void *, size_t, size_t *)) class->write.method)(_self, write_buffer, request_size, write_size);
    } else {
        int result;
        forward(_self, &result, (Method) serial_write, "write", _self, write_buffer, request_size, write_size);
        return result;
    }
}

static ssize_t
Serial_write(void *_self, const void *write_buffer, size_t request_size, size_t *write_size)
{
    
    struct Serial *self = cast(Serial(), _self);
    int fd = self->fd;
    double timeout = self->write_timeout;
    int ret;
    ssize_t n = -1;
    struct timeval tv;
    fd_set writefds;
    
    if (fd == -1) {
        return AAOS_EINVAL;
    }
    
    tv.tv_sec = floor(timeout);
    tv.tv_usec = (timeout - tv.tv_usec) * 1000000;
    FD_ZERO(&writefds);
    
    ret = Select(fd + 1, NULL, &writefds, NULL, &tv);
    switch (ret) {
        case -1:
            return AAOS_ERROR;
            break;
        case 0:
            return AAOS_ETIMEDOUT;
        default:
            break;
    }
    
    if (FD_ISSET(fd, &writefds)) {
        if ((n = Write(fd, write_buffer, request_size)) < 0) {
            return AAOS_ERROR;
        } else {
            if (write_size) {
                *write_size = n;
            }
        }
    }
    
    return AAOS_OK;
}

