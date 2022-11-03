//
//  serial.c
//  AAOS
//
//  Created by huyi on 2018/10/25.
//  Copyright © 2018 National Astronomical Observatories, Chinese Academy of Sciences. All rights reserved.
//

#include "adt.h"
#include "def.h"
#include "serial_r.h"
#include "serial.h"
#include "wrapper.h"

#define READTIMEOUT 2.0
#define WRITETIMEOUT 2.0

#define SERIAL_STATE_OK 0
#define SERIAL_STATE_ERROR 1
#define SERIAL_STATE_WAIT_FOR_READY 2
#define SERIAL_STATE_UNLOADED 3

#define SERIAL_OPTION_WAIT_FOR_READY 1

#ifdef LINUX
#include <sys/epoll.h>
#include <libudev.h>
int
lookup_tty_device(const char *devname)
{
    struct udev *udev;
    struct udev_enumerate *enumerate;
    struct udev_list_entry *devices, *dev_list_entry;
    struct udev_device *dev;
    const char *path;
    int ret = AAOS_ERROR;

    if ((udev = udev_new()) == NULL) {
        return AAOS_ERROR;
    }

    if ((enumerate = udev_enumerate_new(udev)) == NULL) {
        udev_unref(udev);
        return AAOS_ERROR;
    }

    udev_enumerate_add_match_subsystem(enumerate, "tty");
    udev_enumerate_add_match_property(enumerate, "DEVNAME", devname);
    udev_enumerate_scan_devices(enumerate);

    if ((devices = udev_enumerate_get_list_entry(enumerate)) == NULL) {
        udev_enumerate_unref(enumerate);
        udev_unref(udev);
        return AAOS_ERROR;
    }

    udev_list_entry_foreach(dev_list_entry, devices) {
        const char *mydevname;
        path = udev_list_entry_get_name(dev_list_entry);
        dev = udev_device_new_from_syspath(udev, path);
        if (dev != NULL) {
            mydevname = udev_device_get_property_value(dev, "DEVNAME");
            if (strcmp(devname, mydevname) == 0) {
                udev_device_unref(dev);
                ret = AAOS_OK;
                break;
            }
            udev_device_unref(dev);
        }
    }

    udev_enumerate_unref(enumerate);
    udev_unref(udev);

    return ret;
}

int
wait_for_tty_device_ready(const char *devpath, int timeout)
{
    if (lookup_tty_device(devpath) == 0) {
        return AAOS_OK;
    }

    struct udev *udev;
    struct udev_monitor *mon;
    int fd, ret = AAOS_ERROR;

    if ((udev = udev_new()) == NULL) {
        printf("udev_new error.\n");
        return AAOS_ERROR;
    }

    if ((mon = udev_monitor_new_from_netlink(udev, "udev")) == NULL) {
        printf("udev_monitor_new_from_netlink error.\n");
        udev_unref(udev);
        return AAOS_ERROR;
    }

    if (udev_monitor_filter_add_match_subsystem_devtype(mon, "tty", NULL) < 0) {
        printf("udev_monitor_filter_add_match_subsystem_devtype error.\n");
        udev_unref(udev);
        udev_monitor_unref(mon);
        return AAOS_ERROR;
    }

    if (udev_monitor_enable_receiving(mon) < 0) {
        printf("udev_monitor_enable_receiving error.\n");
        udev_unref(udev);
        udev_monitor_unref(mon);
        return AAOS_ERROR;
    }

    fd = udev_monitor_get_fd(mon);
    
    int efd, nfds;
    efd = epoll_create1(0);
    struct epoll_event ev, event;

    ev.events = EPOLLIN;
    ev.data.fd = fd;
    epoll_ctl(efd, EPOLL_CTL_ADD, fd, &ev);
    nfds = epoll_wait(efd, &event, 1, timeout);

    if (nfds == 0) {
        printf("epoll_wait timed out.\n");
        if (lookup_tty_device(devpath) == 0) {
            ret = AAOS_OK;
        }
        goto error;
    } else if (nfds < 0) {
        printf("epoll_wait error.\n");
        goto error;
    }

    if (event.data.fd == fd) {
        struct udev_device *dev;
        const char *action;
        if ((dev = udev_monitor_receive_device(mon)) == NULL) {
            printf("udev_monitor_receive_device error.\n");
            goto error;
        }
        action = udev_device_get_action(dev);
        if (strcmp(action, "add") != 0) {
            printf("no tty devices is added.\n");
            udev_device_unref(dev);
            goto error;
        }
        if (lookup_tty_device(devpath) == 0) {
            ret = AAOS_OK;
        }
        udev_device_unref(dev);
    }

error:
    close(efd);
    udev_monitor_unref(mon);
    udev_unref(udev);

    return ret;
}
#endif

#define WAIT_FOR_TTY_READY_TIMEOUT 120

static void *
__Serial_wait_for_tty_ready_thread(void *_self)
{
    struct __Serial *self = cast(__Serial(), _self);
    
    int ret;
    Pthread_detach(pthread_self());
#ifdef LINUX
    wait_for_tty_device_ready(self->path, WAIT_FOR_TTY_READY_TIMEOUT * 1000);
#else
    sleep(WAIT_FOR_TTY_READY_TIMEOUT);
#endif
    
    Pthread_mutex_lock(&self->mtx);
    unsigned int old_option = self->option;
    self->option &= ~SERIAL_OPTION_WAIT_FOR_READY;
    if ((ret = __serial_init(self)) == AAOS_OK) {
        self->state = SERIAL_STATE_OK;
    } else {
        self->state = SERIAL_STATE_ERROR;
    }
    self->option = old_option;
    Pthread_mutex_unlock(&self->mtx);
    Pthread_cond_broadcast(&self->cond);
    
    return NULL;
}

/*
 * Serial virtual table.
 */
static void *
__SerialVirtualTable_ctor(void *_self, va_list *app)
{
    struct __SerialVirtualTable *self = super_ctor(__SerialVirtualTable(), _self, app);
    Method selector;
    
    while ((selector = va_arg(*app, Method))) {
        const char *tag = va_arg(*app, const char *);
        Method method = va_arg(*app, Method);
        
        if (selector == (Method) __serial_init) {
            if (tag) {
                self->init.tag = tag;
                self->init.selector = selector;
            }
            self->init.method = method;
            continue;
        }
        
        if (selector == (Method) __serial_feed_dog) {
            if (tag) {
                self->feed_dog.tag = tag;
                self->feed_dog.selector = selector;
            }
            self->feed_dog.method = method;
            continue;
        }
        
        if (selector == (Method) __serial_validate) {
            if (tag) {
                self->validate.tag = tag;
                self->validate.selector = selector;
            }
            self->validate.method = method;
            continue;
        }
        
        if (selector == (Method) __serial_raw) {
            if (tag) {
                self->raw.tag = tag;
                self->raw.selector = selector;
            }
            self->raw.method = method;
        }
        
        if (selector == (Method) __serial_inspect) {
            if (tag) {
                self->inspect.tag = tag;
                self->inspect.selector = selector;
            }
            self->inspect.method = method;
        }
    }
    
    return _self;
}

static void *
__SerialVirtualTable_dtor(void *_self)
{
    return super_dtor(Object(), _self);
}

static const void *___SerialVirtualTable;

static void
__SerialVirtualTable_destroy(void)
{
    free((void *) ___SerialVirtualTable);
}

static void
__SerialVirtualTable_initialize(void)
{
    ___SerialVirtualTable = new(VirtualTableClass(), "__SerialVirtualTable", VirtualTable(), sizeof(struct __SerialVirtualTable),
                               ctor, "ctor", __SerialVirtualTable_ctor,
                               dtor, "dtor", __SerialVirtualTable_dtor,
                               (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(__SerialVirtualTable_destroy);
#endif
}

const void *
__SerialVirtualTable(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, __SerialVirtualTable_initialize);
#endif
    
    return ___SerialVirtualTable;
}

/*
 * Serial class method.
 */

int
__serial_get_fd(const void *_self)
{
    const struct __SerialClass *class = (const struct __SerialClass *) classOf(_self);
    
    if (isOf(class, __SerialClass()) && class->get_fd.method) {
        return ((int (*)(const void *)) class->get_fd.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) __serial_get_fd, "get_fd", _self);
        return result;
    }
}

static int
__Serial_get_fd(const void *_self)
{
    const struct __Serial *self = cast(__Serial(), _self);
    
    return self->fd;
}

const char *
__serial_get_name(const void *_self)
{
    const struct __SerialClass *class = (const struct __SerialClass *) classOf(_self);
    
    if (isOf(class, __SerialClass()) && class->get_name.method) {
        return ((const char * (*)(const void *)) class->get_name.method)(_self);
    } else {
        const char *result;
        forward(_self, &result, (Method) __serial_get_name, "get_name", _self);
        return result;
    }
}

static const char *
__Serial_get_name(const void *_self)
{
    const struct __Serial *self = cast(__Serial(), _self);
    
    return (const char *) self->name;
}

const char *
__serial_get_path(const void *_self)
{
    const struct __SerialClass *class = (const struct __SerialClass *) classOf(_self);
    
    if (isOf(class, __SerialClass()) && class->get_path.method) {
        return ((const char * (*)(const void *)) class->get_path.method)(_self);
    } else {
        const char *result;
        forward(_self, &result, (Method) __serial_get_path, "get_path", _self);
        return result;
    }
}

static const char *
__Serial_get_path(const void *_self)
{
    const struct __Serial *self = cast(__Serial(), _self);
    
    return (const char *) self->path;
}

int
__serial_get_result(const void *_self, void **res, size_t *size)
{
    const struct __SerialClass *class = (const struct __SerialClass *) classOf(_self);
    
    if (isOf(class, __SerialClass()) && class->get_result.method) {
        return ((int (*)(const void *, void **, size_t *)) class->get_result.method)(_self, res, size);
    } else {
        int result;
        forward(_self, &result, (Method) __serial_get_result, "get_result", _self, res, size);
        return result;
    }
}

static int
__Serial_get_result(const void *_self, void **result, size_t *size)
{
    const struct __Serial *self = cast(__Serial(), _self);
    
    *size = self->read_size;
    *result = self->read_buffer;
    
    return AAOS_OK;
}

int
__serial_set_command(void *_self, const void *command, size_t size)
{
    const struct __SerialClass *class = (const struct __SerialClass *) classOf(_self);
    
    if (isOf(class, __SerialClass()) && class->set_command.method) {
        return ((int (*)(const void *, const void *, size_t)) class->get_result.method)(_self, command, size);
    } else {
        int result;
        forward(_self, &result, (Method) __serial_set_command, "set_command", _self, command, size);
        return result;
    }
}

static int
__Serial_set_command(void *_self, const void *command, size_t size)
{
    struct __Serial *self = cast(__Serial(), _self);
    
    if (size > self->write_buffer_size) {
        return AAOS_ENOMEM;
    }
    
    memcpy(self->write_buffer, command, size);
    self->write_size = size;
    
    return AAOS_OK;
}

unsigned int
__serial_set_option(void *_self, unsigned int option)
{
    const struct __SerialClass *class = (const struct __SerialClass *) classOf(_self);
    
    if (isOf(class, __SerialClass()) && class->set_option.method) {
        return ((int (*)(void *, unsigned int)) class->set_option.method)(_self, option);
    } else {
        unsigned int result;
        forward(_self, &result, (Method) __serial_set_option, "set_option", _self, option);
        return result;
    }
}

static unsigned int
__Serial_set_option(void *_self, unsigned int option)
{
    struct __Serial *self = cast(__Serial(), _self);
    
    Pthread_mutex_lock(&self->mtx);
    unsigned int old_option = self->option;
    self->option = option;
    Pthread_mutex_unlock(&self->mtx);
    return old_option;
}

unsigned int
__serial_get_option(void *_self)
{
    const struct __SerialClass *class = (const struct __SerialClass *) classOf(_self);
    
    if (isOf(class, __SerialClass()) && class->get_option.method) {
        return ((unsigned int (*)(void *)) class->get_option.method)(_self);
    } else {
        unsigned int result;
        forward(_self, &result, (Method) __serial_set_option, "get_option", _self);
        return result;
    }
}

static unsigned int
__Serial_get_option(void *_self)
{
    struct __Serial *self = cast(__Serial(), _self);
    
    Pthread_mutex_lock(&self->mtx);
    unsigned int option = self->option;
    Pthread_mutex_unlock(&self->mtx);
    return option;
}

int
__serial_set_inspect(void *_self, const void *inspect, size_t size)
{
    const struct __SerialClass *class = (const struct __SerialClass *) classOf(_self);
    
    if (isOf(class, __SerialClass()) && class->set_inspect.method) {
        return ((int (*)(void *, const void *, size_t)) class->set_inspect.method)(_self, inspect, size);
    } else {
        int result;
        forward(_self, &result, (Method) __serial_set_inspect, "set_inspect", _self, inspect, size);
        return result;
    }
}

static int
__Serial_set_inspect(void *_self, const void *inspect, size_t size)
{
    struct __Serial *self = cast(__Serial(), _self);
    
    Pthread_mutex_lock(&self->mtx);
    self->inspect = Realloc(self->inspect, size + 1);
    memset(self->inspect, '\0', size + 1);
    memcpy(self->inspect, inspect, size);
    Pthread_mutex_unlock(&self->mtx);
    
    return  AAOS_OK;
}

int
__serial_get_inspect(void *_self, void *inspect, size_t size)
{
    const struct __SerialClass *class = (const struct __SerialClass *) classOf(_self);
    
    if (isOf(class, __SerialClass()) && class->get_option.method) {
        return ((int (*)(void *, void *, size_t)) class->get_inspect.method)(_self, inspect, size);
    } else {
        unsigned int result;
        forward(_self, &result, (Method) __serial_get_inspect, "get_inspect", _self, inspect, size);
        return result;
    }
}

static int
__Serial_get_inspect(void *_self, void *inspect, size_t size)
{
    struct __Serial *self = cast(__Serial(), _self);
    
    memset(inspect, '\0', size);
    Pthread_mutex_lock(&self->mtx);
    if (self->inspect != NULL) {
        memcpy(inspect, self->inspect, min(size, strlen(self->inspect)));
    }
    Pthread_mutex_unlock(&self->mtx);
    return AAOS_OK;
}

int
__serial_reload(void *_self)
{
    const struct __SerialClass *class = (const struct __SerialClass *) classOf(_self);
    
    if (isOf(class, __SerialClass()) && class->reload.method) {
        return ((int (*)(void *)) class->reload.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) __serial_reload, "reload", _self);
        return result;
    }
}

static int
__Serial_reload(void *_self)
{
    struct __Serial *self = cast(__Serial(), _self);
    
    Pthread_mutex_lock(&self->mtx);
    if (self->state != SERIAL_STATE_UNLOADED) {
        Close(self->fd);
    }
    if (__serial_init(self) == AAOS_OK) {
        self->state = SERIAL_STATE_OK;
    } else {
        self->state = SERIAL_STATE_ERROR;
    }
    Pthread_mutex_unlock(&self->mtx);

    return AAOS_OK;
}

int
__serial_unload(void *_self)
{
    const struct __SerialClass *class = (const struct __SerialClass *) classOf(_self);
    
    if (isOf(class, __SerialClass()) && class->unload.method) {
        return ((int (*)(void *)) class->unload.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) __serial_unload, "unload", _self);
        return result;
    }
}

static int
__Serial_unload(void *_self)
{
    struct __Serial *self = cast(__Serial(), _self);
    
    Pthread_mutex_lock(&self->mtx);
    Close(self->fd);
    self->state = SERIAL_STATE_UNLOADED;
    Pthread_mutex_unlock(&self->mtx);
    
    return AAOS_OK;
}

int
__serial_load(void *_self, ...)
{
    const struct __SerialClass *class = (const struct __SerialClass *) classOf(_self);
    va_list ap;
    va_start(ap, _self);
    int result;
    
    if (isOf(class, __SerialClass()) && class->load.method) {
        result = ((int (*)(void *, va_list *)) class->load.method)(_self, &ap);
    } else {
        forward(_self, &result, (Method) __serial_load, "load", _self, &ap);
    }
    va_end(ap);
    
    return result;
}

static int
__Serial_load(void *_self, va_list *app)
{
    struct __Serial *self = cast(__Serial(), _self);
    
    Pthread_mutex_lock(&self->mtx);
    if (self->state != SERIAL_STATE_UNLOADED) {
        Pthread_mutex_unlock(&self->mtx);
        return AAOS_OK;
    }
    if (__serial_init(self) == AAOS_OK) {
        self->state = SERIAL_STATE_OK;
    } else {
        self->state = SERIAL_STATE_ERROR;
        Pthread_mutex_unlock(&self->mtx);
        return AAOS_ERROR;
    }
    Pthread_mutex_unlock(&self->mtx);
    
    return AAOS_OK;
}


int
__serial_read(void *_self, void *read_buffer, size_t request_size, size_t *read_size)
{
    const struct __SerialClass *class = (const struct __SerialClass *) classOf(_self);
    
    if (isOf(class, __SerialClass()) && class->read.method) {
        return ((int (*)(void *, void *, size_t, size_t *)) class->read.method)(_self, read_buffer, request_size, read_size);
        
    } else {
        int result;
        forward(_self, &result, (Method) __serial_read, "read", _self, read_buffer, request_size, read_size);
        return result;
    }
}

static int
__Serial_read(void *_self, void *read_buffer, size_t request_size, size_t *read_size)
{
    struct __Serial *self = cast(__Serial(), _self);
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
    tv.tv_usec = (timeout - tv.tv_sec) * 1000000;
    FD_ZERO(&readfds);
    FD_SET(fd, &readfds);
    
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

/*
 * Use internal buffer
 */

int
__serial_read2(void *_self)
{
    const struct __SerialClass *class = (const struct __SerialClass *) classOf(_self);
    
    
    if (isOf(class, __SerialClass()) && class->read2.method) {
        return ((int (*)(void *)) class->read2.method)(_self);
        
    } else {
        int result;
        forward(_self, &result, (Method) __serial_read2, "read2", _self);
        return result;
    }
}

static int
__Serial_read2(void *_self)
{
    struct __Serial *self = cast(__Serial(), _self);
    
    return __Serial_read(self, self->read_buffer, self->read_buffer_size, &self->read_size);
}

/*
 * noncanonical mode read
 */

int
__serial_read3(void *_self, void *read_buffer, size_t request_size, size_t *read_size)
{
    const struct __SerialClass *class = (const struct __SerialClass *) classOf(_self);
    
    if (isOf(class, __SerialClass()) && class->read3.method) {
        return ((int (*)(void *, void *, size_t, size_t *)) class->read3.method)(_self, read_buffer, request_size, read_size);
        
    } else {
        int result;
        forward(_self, &result, (Method) __serial_read3, "read3", _self, read_buffer, request_size, read_size);
        return result;
    }
}

static int
__Serial_read3(void *_self, void *read_buffer, size_t request_size, size_t *read_size)
{
    struct __Serial *self = cast(__Serial(), _self);
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
    tv.tv_usec = (timeout - tv.tv_sec) * 1000000;
    FD_ZERO(&readfds);
    FD_SET(fd, &readfds);
    
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
        ssize_t nleft = request_size;
        size_t nread = 0;
        char *s = read_buffer;
        while ((n = Read(fd, s, nleft)) > 0 && nleft > 0) {
            nleft -= n;
            s += n;
            nread += n;
        }
        if (n == 0) {
            switch (errno) {
                case EAGAIN:
                    if (read_size) {
                        *read_size = nread;
                    }
                    return AAOS_OK;
                    break;
                default:
                    return AAOS_ERROR;
                    break;
            }
        }
    } else {
        return AAOS_ERROR;
    }
    return AAOS_OK;
}


int
__serial_write(void *_self, const void *write_buffer, size_t request_size, size_t *write_size)
{
    const struct __SerialClass *class = (const struct __SerialClass *) classOf(_self);
    
    
    if (isOf(class, __SerialClass()) && class->write.method) {
        return ((int (*)(void *, const void *, size_t, size_t *)) class->write.method)(_self, write_buffer, request_size, write_size);
    } else {
        int result;
        forward(_self, &result, (Method) __serial_write, "write", _self, write_buffer, request_size, write_size);
        return result;
    }
}

static int
__Serial_write(void *_self, const void *write_buffer, size_t request_size, size_t *write_size)
{
    
    struct __Serial *self = cast(__Serial(), _self);
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
    tv.tv_usec = (timeout - tv.tv_sec) * 1000000;
    FD_ZERO(&writefds);
    FD_SET(fd, &writefds);
    
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

int
__serial_write2(void *_self)
{
    const struct __SerialClass *class = (const struct __SerialClass *) classOf(_self);
    
    if (isOf(class, __SerialClass()) && class->write2.method) {
        return ((int (*)(void *)) class->write2.method)(_self);
        
    } else {
        int result;
        forward(_self, &result, (Method) __serial_write2, "write2", _self);
        return result;
    }
}

static int
__Serial_write2(void *_self)
{
    struct __Serial *self = cast(__Serial(), _self);
    
    return __Serial_write(self, self->write_buffer, self->write_size, &self->write_size);
}

int
__serial_raw(void *_self, const void *write_buffer, size_t write_buffer_size, size_t *write_size, void *read_buffer, size_t read_buffer_size, size_t *read_size)
{
    const struct __SerialClass *class = (const struct __SerialClass *) classOf(_self);
    
    if (isOf(class, __SerialClass()) && class->raw.method) {
        return ((int (*)(void *, const void *, size_t, size_t *, void *, size_t, size_t *)) class->raw.method)(_self, write_buffer, write_buffer_size, write_size, read_buffer, read_buffer_size, read_size);
        
    } else {
        int result;
        forward(_self, &result, (Method) __serial_raw, "raw", _self, write_buffer, write_buffer_size, write_size, read_buffer, read_buffer_size, read_size);
        return result;
    }
}

static int
__Serial_raw(void *_self, const void *write_buffer, size_t write_buffer_size, size_t *write_size, void *read_buffer, size_t read_buffer_size, size_t *read_size)
{
    struct __Serial *self = cast(__Serial(), _self);
    
    int ret = AAOS_OK;
    Method validate;
    
    if (self->_vtab && (validate = virtualTo(self->_vtab, "validate")) != 0) {
        ret = ((int (*)(const void *, const void *, size_t)) validate)(self, write_buffer, write_buffer_size);
        if (ret != AAOS_OK) {
            return ret;
        }
    }

    Pthread_mutex_lock(&self->mtx);
    switch (self->state) {
        case SERIAL_STATE_ERROR:
            Pthread_mutex_unlock(&self->mtx);
            return AAOS_EDEVMAL;
            break;
        case SERIAL_STATE_UNLOADED:
            Pthread_mutex_unlock(&self->mtx);
            return AAOS_EDEVNOTLOADED;
            break;
        default:
            break;
    }
    while (self->state == SERIAL_STATE_WAIT_FOR_READY) {
        Pthread_cond_wait(&self->cond, &self->mtx);
    }
    if ((ret = __Serial_write(self, write_buffer, write_buffer_size, write_size)) != AAOS_OK) {
        Pthread_mutex_unlock(&self->mtx);
        return ret;
    }
    ret = __Serial_read(self, read_buffer, read_buffer_size, read_size);
    Pthread_mutex_unlock(&self->mtx);
    
    return ret;
}

int
__serial_raw_nl(void *_self, const void *write_buffer, size_t write_buffer_size, size_t *write_size, void *read_buffer, size_t read_buffer_size, size_t *read_size)
{
    const struct __SerialClass *class = (const struct __SerialClass *) classOf(_self);
    
    if (isOf(class, __SerialClass()) && class->raw_nl.method) {
        return ((int (*)(void *, const void *, size_t, size_t *, void *, size_t, size_t *)) class->raw_nl.method)(_self, write_buffer, write_buffer_size, write_size, read_buffer, read_buffer_size, read_size);
        
    } else {
        int result;
        forward(_self, &result, (Method) __serial_raw_nl, "raw_nl", _self, write_buffer, write_buffer_size, write_size, read_buffer, read_buffer_size, read_size);
        return result;
    }
}

static int
__Serial_raw_nl(void *_self, const void *write_buffer, size_t write_buffer_size, size_t *write_size, void *read_buffer, size_t read_buffer_size, size_t *read_size)
{
    struct __Serial *self = cast(__Serial(), _self);
    
    int ret = AAOS_OK;
    Method validate;
    
    if (self->_vtab && (validate = virtualTo(self->_vtab, "validate")) != 0) {
        ret = ((int (*)(const void *, const void *, size_t)) validate)(self, write_buffer, write_buffer_size);
        if (ret != AAOS_OK) {
            return ret;
        }
    }
    
    if ((ret = __Serial_write(self, write_buffer, write_buffer_size, write_size)) != AAOS_OK) {
        return ret;
    }
    ret = __Serial_read(self, read_buffer, read_buffer_size, read_size);

    return ret;
}

int
__serial_raw2(void *_self)
{
    const struct __SerialClass *class = (const struct __SerialClass *) classOf(_self);
    
    
    if (isOf(class, __SerialClass()) && class->raw2.method) {
        return ((int (*)(void *)) class->raw2.method)(_self);
        
    } else {
        int result;
        forward(_self, &result, (Method) __serial_raw2, "raw2", _self);
        return result;
    }
}

static int
__Serial_raw2(void *_self)
{
    struct __Serial *self = cast(__Serial(), _self);
    
    return __Serial_raw(self, self->write_buffer, self->write_size, &self->write_size, self->read_buffer, self->read_buffer_size, &self->read_size);
}

int
__serial_wait(void *_self, double timeout)
{
    const struct __SerialClass *class = (const struct __SerialClass *) classOf(_self);
    
    if (isOf(class, __SerialClass()) && class->wait.method) {
        return ((int (*)(void *, double)) class->wait.method)(_self, timeout);
    } else {
        int result;
        forward(_self, &result, (Method) __serial_wait, "wait", _self, timeout);
        return result;
    }
}

static int
__Serial_wait(void *_self, double timeout)
{
    struct __Serial *self = cast(__Serial(), _self);
    
    int ret = 0;
    struct timespec tp;
    
    tp.tv_sec = floor(timeout);
    tp.tv_nsec = (timeout - tp.tv_sec) * 1000000000;
    
    Pthread_mutex_lock(&self->mtx);
    while (self->state == SERIAL_STATE_ERROR) {
        if (timeout > 0) {
            ret = Pthread_cond_timedwait(&self->cond, &self->mtx, &tp);
        } else {
            ret  = Pthread_cond_wait(&self->cond, &self->mtx);
        }
    }
    Pthread_mutex_unlock(&self->mtx);
    
    if (ret == 0) {
        return AAOS_OK;
    } else if (ret == ETIMEDOUT) {
        return AAOS_ETIMEDOUT;
    } else {
        return AAOS_ERROR;
    }
}

/*
 * Serial class pure virtual method.
 */

int
__serial_feed_dog(void *_self)
{
    const struct __SerialClass *class = (const struct __SerialClass *) classOf(_self);
    
    if (isOf(class, __SerialClass()) && class->feed_dog.method) {
        return ((int (*)(const void *)) class->feed_dog.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) __serial_feed_dog, "feed_dog", _self);
        return result;
    }
}

static int
__Serial_feed_dog(void *_self)
{
    return AAOS_OK;
}

int
__serial_init(void *_self)
{
    const struct __SerialClass *class = (const struct __SerialClass *) classOf(_self);
    
    if (isOf(class, __SerialClass()) && class->init.method) {
        return ((int (*)(const void *)) class->init.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) __serial_init, "init", _self);
        return result;
    }
}

static int
__Serial_init(void *_self)
{
    return AAOS_OK;
}

int
__serial_validate(const void *_self, const void *command, size_t size)
{
    const struct __SerialClass *class = (const struct __SerialClass *) classOf(_self);
    
    if (isOf(class, __SerialClass()) && class->validate.method) {
        return ((int (*)(const void *, const void *, size_t)) class->validate.method)(_self, command, size);
    } else {
        int result;
        forward(_self, &result, (Method) __serial_validate, "validate", _self, command, size);
        return result;
    }
}

static int
__Serial_validate(const void *_self, const void *command, size_t size)
{
    return AAOS_OK;
}

int
__serial_inspect(void *_self)
{
    const struct __SerialClass *class = (const struct __SerialClass *) classOf(_self);
    
    if (isOf(class, __SerialClass()) && class->inspect.method) {
        return ((int (*)(void *)) class->inspect.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) __serial_inspect, "inspect", _self);
        return result;
    }
}

static int
__Serial_inspect(void *_self)
{
    struct __Serial *self = cast(__Serial(), _self);
    
    if (self->inspect != NULL) {
        char buf[BUFSIZE];
        Pthread_mutex_lock(&self->mtx);
        if (__serial_raw_nl(self, self->inspect, strlen(self->inspect), NULL, buf, BUFSIZE, NULL) == AAOS_OK) {
            self->state = SERIAL_STATE_OK;
            Pthread_mutex_unlock(&self->mtx);
            Pthread_cond_broadcast(&self->cond);
            return AAOS_OK;
        } else {
            self->state = SERIAL_STATE_ERROR;
            Pthread_mutex_unlock(&self->mtx);
            return AAOS_ERROR;
        }
    } else {
        return AAOS_ENOTSUP;
    }
}

/*
 * class
 */
int
__Serial_puto(const void *_self, FILE *fp)
{
    const struct __Serial *self = cast(__Serial(), _self);
    
    int ret;
    ret = fprintf(fp, "serial name:\t%s\n", self->name);
    ret = fprintf(fp, "serial device path:\t%s\n", self->path);
    switch (self->state) {
        case SERIAL_STATE_OK:
            ret = fprintf(fp, "serial state: OK\n");
            break;
        case SERIAL_STATE_UNLOADED:
            ret = fprintf(fp, "serial state: UNLOAD\n");
            break;
        case SERIAL_STATE_WAIT_FOR_READY:
            ret = fprintf(fp, "serial state: UNREADY\n");
            break;
        case SERIAL_STATE_ERROR:
            ret = fprintf(fp, "serial state: ERROR\n");
            break;
        default:
            break;
    }
    if (self->description) {
        ret = fprintf(fp, "serial device description: %s\n", self->description);
    }
    return ret;
}

static void
__Serial_forward(const void *_self, void *result, Method selector, const char *name, va_list *app)
{
    const struct __Serial *self = cast(__Serial(), _self);
    Method method = virtualTo(self->_vtab, name);
    va_list ap;
    
#ifdef va_copy
    va_copy(ap, *app);
#else
    ap = *app;
#endif
    
    void *obj = va_arg(*app, void *);
    
    if (selector == (Method) __serial_init || selector == (Method) __serial_feed_dog || selector == (Method) __serial_validate || selector == (Method) __serial_inspect) {
        *((int *) result) = ((int (*)(void *)) method)(obj);
    }  else if (selector == (Method) __serial_raw) {
        const void *write_buffer = va_arg(*app, const void *);
        size_t write_buffer_size = va_arg(*app, size_t);
        size_t *write_size = va_arg(*app, size_t *);
        void *read_buffer = va_arg(*app, void *);
        size_t read_buffer_size = va_arg(*app, size_t);
        size_t *read_size = va_arg(*app, size_t *);
        *((int *) result) = ((int (*)(void *, const void *, size_t, size_t *, void *, size_t, size_t *)) method)(obj, write_buffer, write_buffer_size, write_size, read_buffer, read_buffer_size, read_size);
    } else {
        assert(0);
    }
    
#ifdef va_copy
    va_end(ap);
#endif
}

static void *
__Serial_ctor(void *_self, va_list *app)
{
    struct __Serial *self = super_ctor(__Serial(), _self, app);
    
    const char *s, *key, *value;
    
    s = va_arg(*app, const char *);
    if (s) {
        self->name = (char *) Malloc(strlen(s) + 1);
        snprintf(self->name, strlen(s) + 1, "%s", s);
    }
    s = va_arg(*app, const char *);
    if (s) {
        self->path = (char *) Malloc(strlen(s) + 1);
        snprintf(self->path, strlen(s) + 1, "%s", s);
    }
    
    self->read_timeout = READTIMEOUT;
    self->write_timeout = WRITETIMEOUT;
    
    while ((key = va_arg(*app, const char *))) {
        if (strcmp(key, "description") == 0) {
            value = va_arg(*app, const char *);
            if (value) {
                self->description = (char *) Malloc(strlen(value) + 1);
                snprintf(self->description, strlen(value) + 1, "%s", value);
            }
            
            continue;
        } else if (strcmp(key, "read_timeout") == 0) {
            self->read_timeout = va_arg(*app, double);
            continue;
        } else if (strcmp(key, "write_timeout") == 0) {
            self->write_timeout = va_arg(*app, double);
            continue;
        } else if (strcmp(key, "read_buffer_size") == 0) {
            self->read_buffer_size = va_arg(*app, size_t);
            self->read_buffer = Malloc(self->read_buffer_size);
            if (!self->write_buffer) {
                self->write_buffer = self->read_buffer;
                self->write_buffer_size = self->read_buffer_size;
            }
            continue;
        } else if (strcmp(key, "write_buffer_size") == 0) {
            self->write_buffer_size = va_arg(*app, size_t);
            self->write_buffer = Malloc(self->write_buffer_size);
            if (!self->read_buffer) {
                self->read_buffer = self->write_buffer;
                self->read_buffer_size = self->write_buffer_size;
            }
            continue;
        }
    }
    self->fd = -1;
    self->state = SERIAL_STATE_UNLOADED;
    Pthread_mutex_init(&self->mtx, NULL);
    Pthread_cond_init(&self->cond, NULL);
    
    return (void *) self;
}

static void *
__Serial_dtor(void *_self)
{
    struct __Serial *self = cast(__Serial(), _self);
    
    free(self->name);
    free(self->path);
    free(self->description);
    free(self->inspect);
    if (self->read_buffer == self->write_buffer) {
        free(self->read_buffer);
    } else {
        free(self->read_buffer);
        free(self->write_buffer);
    }
    
    if (self->fd >= 0) {
        Close(self->fd);
    }
    
    Pthread_mutex_destroy(&self->mtx);
    Pthread_cond_destroy(&self->cond);
    
    return super_dtor(__Serial(), _self);
}

static void *
__SerialClass_ctor(void *_self, va_list *app)
{
    struct __SerialClass *self = super_ctor(__SerialClass(), _self, app);
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
        if (selector == (Method) __serial_read) {
            if (tag) {
                self->read.tag = tag;
                self->read.selector = selector;
            }
            self->read.method = method;
            continue;
        }
        if (selector == (Method) __serial_read2) {
            if (tag) {
                self->read2.tag = tag;
                self->read2.selector = selector;
            }
            self->read2.method = method;
            continue;
        }
        if (selector == (Method) __serial_read3) {
            if (tag) {
                self->read3.tag = tag;
                self->read3.selector = selector;
            }
            self->read3.method = method;
            continue;
        }
        if (selector == (Method) __serial_write) {
            if (tag) {
                self->write.tag = tag;
                self->write.selector = selector;
            }
            self->write.method = method;
            continue;
        }
        if (selector == (Method) __serial_write2) {
            if (tag) {
                self->write2.tag = tag;
                self->write2.selector = selector;
            }
            self->write2.method = method;
            continue;
        }
        if (selector == (Method) __serial_raw) {
            if (tag) {
                self->raw.tag = tag;
                self->raw.selector = selector;
            }
            self->raw.method = method;
            continue;
        }
        if (selector == (Method) __serial_raw_nl) {
            if (tag) {
                self->raw_nl.tag = tag;
                self->raw_nl.selector = selector;
            }
            self->raw_nl.method = method;
            continue;
        }
        if (selector == (Method) __serial_raw2) {
            if (tag) {
                self->raw2.tag = tag;
                self->raw2.selector = selector;
            }
            self->raw2.method = method;
            continue;
        }
        if (selector == (Method) __serial_get_fd) {
            if (tag) {
                self->get_fd.tag = tag;
                self->get_fd.selector = selector;
            }
            self->get_fd.method = method;
            continue;
        }
        if (selector == (Method) __serial_get_result) {
            if (tag) {
                self->get_result.tag = tag;
                self->get_result.selector = selector;
            }
            self->get_result.method = method;
            continue;
        }
        if (selector == (Method) __serial_set_command) {
            if (tag) {
                self->set_command.tag = tag;
                self->set_command.selector = selector;
            }
            self->set_command.method = method;
            continue;
        }
        if (selector == (Method) __serial_set_option) {
            if (tag) {
                self->set_option.tag = tag;
                self->set_option.selector = selector;
            }
            self->set_option.method = method;
            continue;
        }
        if (selector == (Method) __serial_get_option) {
            if (tag) {
                self->get_option.tag = tag;
                self->get_option.selector = selector;
            }
            self->get_option.method = method;
            continue;
        }
        if (selector == (Method) __serial_init) {
            if (tag) {
                self->init.tag = tag;
                self->init.selector = selector;
            }
            self->init.method = method;
            continue;
        }
        if (selector == (Method) __serial_feed_dog) {
            if (tag) {
                self->feed_dog.tag = tag;
                self->feed_dog.selector = selector;
            }
            self->feed_dog.method = method;
            continue;
        }
        if (selector == (Method) __serial_get_name) {
            if (tag) {
                self->get_name.tag = tag;
                self->get_name.selector = selector;
            }
            self->get_name.method = method;
            continue;
        }
        if (selector == (Method) __serial_get_path) {
            if (tag) {
                self->get_path.tag = tag;
                self->get_path.selector = selector;
            }
            self->get_path.method = method;
            continue;
        }
        
        if (selector == (Method) __serial_load) {
            if (tag) {
                self->load.tag = tag;
                self->load.selector = selector;
            }
            self->load.method = method;
            continue;
        }
        if (selector == (Method) __serial_reload) {
            if (tag) {
                self->reload.tag = tag;
                self->reload.selector = selector;
            }
            self->reload.method = method;
            continue;
        }
        if (selector == (Method) __serial_unload) {
            if (tag) {
                self->unload.tag = tag;
                self->unload.selector = selector;
            }
            self->unload.method = method;
            continue;
        }
        if (selector == (Method) __serial_inspect) {
            if (tag) {
                self->inspect.tag = tag;
                self->inspect.selector = selector;
            }
            self->inspect.method = method;
            continue;
        }
        if (selector == (Method) __serial_set_inspect) {
            if (tag) {
                self->set_inspect.tag = tag;
                self->set_inspect.selector = selector;
            }
            self->set_inspect.method = method;
            continue;
        }
        if (selector == (Method) __serial_get_inspect) {
            if (tag) {
                self->get_inspect.tag = tag;
                self->get_inspect.selector = selector;
            }
            self->get_inspect.method = method;
            continue;
        }
        if (selector == (Method) __serial_wait) {
            if (tag) {
                self->wait.tag = tag;
                self->wait.selector = selector;
            }
            self->wait.method = method;
            continue;
        }
    }
    
#ifdef va_copy
    va_end(ap);
#endif
    
    return (void *) self;
}

static const void *___SerialClass;

static void
__SerialClass_destroy(void)
{
    free((void *) ___SerialClass);
}

static void
__SerialClass_initialize(void)
{
    ___SerialClass = new(Class(), "__SerialClass", Class(), sizeof(struct __SerialClass),
                        ctor, "ctor", __SerialClass_ctor,
                        (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(__SerialClass_destroy);
#endif
}

const void *
__SerialClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, __SerialClass_initialize);
#endif
    
    return ___SerialClass;
}

static const void *___Serial;

static void
__Serial_destroy(void)
{
    free((void *)___Serial);
}

static void
__Serial_initialize(void)
{
    ___Serial = new(__SerialClass(), "__Serial", Object(), sizeof(struct __Serial),
                    ctor, "ctor", __Serial_ctor,
                    dtor, "dtor", __Serial_dtor,
                    forward, "forward", __Serial_forward,
                    puto, "puto", __Serial_puto,
                    __serial_read, "read", __Serial_read,
                    __serial_write, "write", __Serial_write,
                    __serial_read2, "read2", __Serial_read2,
                    __serial_write2, "write2", __Serial_write2,
                    __serial_read3, "read3", __Serial_read3,
                    __serial_raw, "raw", __Serial_raw,
                    __serial_raw_nl, "raw_nl", __Serial_raw_nl,
                    __serial_raw2, "raw2", __Serial_raw2,
                    __serial_get_fd, "get_fd", __Serial_get_fd,
                    __serial_get_result, "get_result", __Serial_get_result,
                    __serial_set_command, "set_command", __Serial_set_command,
                    __serial_set_option, "set_option", __Serial_set_option,
                    __serial_get_option, "get_option", __Serial_get_option,
                    __serial_get_name, "get_name", __Serial_get_name,
                    __serial_get_path, "get_path", __Serial_get_path,
                    __serial_init, "init", __Serial_init,
                    __serial_validate, "validate", __Serial_validate,
                    __serial_feed_dog, "feed_dog", __Serial_feed_dog,
                    __serial_load, "load", __Serial_load,
                    __serial_reload, "reload", __Serial_reload,
                    __serial_unload, "unload", __Serial_unload,
                    __serial_inspect, "inspect", __Serial_inspect,
                    __serial_set_inspect, "set_inspect", __Serial_set_inspect,
                    __serial_get_inspect, "get_inspect", __Serial_get_inspect,
                    __serial_wait, "wait", __Serial_wait,
                   (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(__Serial_destroy);
#endif
}

const void *
__Serial(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, __Serial_initialize);
#endif
    
    return ___Serial;
}

/*
 * JZD Serial driver, R-8520, R-8017 and R-8033
 */

static const char *pattern_jzd = "(^#[0-9A-F]{3}\r$)|(^.P\r$)";
static regex_t preg_jzd;

static const void *jzd_serial_virtual_table(void);

static void *
JZDSerial_ctor(void *_self, va_list *app)
{
    struct JZDSerial *self = super_ctor(JZDSerial(), _self, app);
    
    self->_._vtab= jzd_serial_virtual_table();
    
    return (void *) self;
}

static void *
JZDSerial_dtor(void *_self)
{
    return super_dtor(JZDSerial(), _self);
}

static void *
JZDSerialClass_ctor(void *_self, va_list *app)
{
    struct JZDSerialClass *self = super_ctor(JZDSerialClass(), _self, app);
    
    self->_.raw.method = (Method) 0;
    self->_.init.method = (Method) 0;
    self->_.validate.method = (Method) 0;
    
    return self;
}

static const void *_JZDSerialClass;

static void
JZDSerialClass_destroy(void)
{
    free((void *)_JZDSerialClass);
}

static void
JZDSerialClass_initialize(void)
{
    _JZDSerialClass = new(__SerialClass(), "JZDSerialClass", __SerialClass(), sizeof(struct JZDSerialClass),
                          ctor, "", JZDSerialClass_ctor,
                          (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(JZDSerialClass_destroy);
#endif
}

const void *
JZDSerialClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, JZDSerialClass_initialize);
#endif
    
    return _JZDSerialClass;
}

static const void *_JZDSerial;

static void
JZDSerial_destroy(void)
{
    regfree(&preg_jzd);
    free((void *)_JZDSerial);
}

static void
JZDSerial_initialize(void)
{
    regcomp(&preg_jzd, pattern_jzd, REG_EXTENDED | REG_NOSUB);
    _JZDSerial = new(JZDSerialClass(), "JZDSerial", __Serial(), sizeof(struct JZDSerial),
                     ctor, "ctor", JZDSerial_ctor,
                     dtor, "dtor", JZDSerial_dtor,
                     (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(JZDSerial_destroy);
#endif
}

const void *
JZDSerial(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, JZDSerial_initialize);
#endif

    return _JZDSerial;
}

static int
JZDSerial_init(void *_self)
{
    struct __Serial *self = cast(__Serial(), _self);
    struct termios termptr;
    int ibaud = B9600, obaud = B9600;
    
    self->fd = Open(self->path, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (self->fd < 0) {
        if (errno == ENOENT && (self->option&SERIAL_OPTION_WAIT_FOR_READY)) {
            pthread_t tid;
            self->state = SERIAL_STATE_WAIT_FOR_READY;
            Pthread_create(&tid, NULL, __Serial_wait_for_tty_ready_thread, self);
        } else {
            self->state = SERIAL_STATE_ERROR;
        }
        return AAOS_EBADF;
    } else {
        self->state = SERIAL_STATE_OK;
    }
    
    Tcgetattr(self->fd, &termptr);
    Cfsetispeed(&termptr, ibaud);
    Cfsetospeed(&termptr, obaud);
    termptr.c_iflag &= ~(BRKINT | IMAXBEL | INLCR);
    termptr.c_iflag |= (IGNBRK | ICRNL);
    termptr.c_oflag &= ~ (OPOST | OCRNL);
    termptr.c_oflag |= ONLCR;
    termptr.c_lflag &= ~(ISIG | IEXTEN | ECHO | ECHOE | ECHOK | ECHOCTL | ECHOKE);
    Tcsetattr(self->fd, TCSANOW, &termptr);
    
    return AAOS_OK;
}

static int
JZDSerial_feed_dog(void *_self)
{
    return AAOS_OK;
}

static int
JZDSerial_validate(const void *_self, const void *command, size_t size)
{
    if (regexec(&preg_jzd, command, 0, NULL, 0) != 0) {
        return AAOS_EBADCMD;
    } else {
        return AAOS_OK;
    }
}

static int
JZDSerial_raw(void *_self, const void *write_buffer, size_t write_buffer_size, size_t *write_size, void *read_buffer, size_t read_buffer_size, size_t *read_size)
{
    struct __Serial *self = cast(__Serial(), _self);
    
    int ret = AAOS_OK;
    Method validate;
    char *cmd = (char *) write_buffer;

    /*
     * append '\r'
     */
    
    if (cmd[write_buffer_size - 1] != '\r') {
        if ((cmd = (char *) Malloc(write_buffer_size + 2)) == NULL) {
            return AAOS_ENOMEM;
        }
        memset(cmd, '\0', write_buffer_size + 2);
        memcpy(cmd, write_buffer, write_buffer_size);
        cmd[write_buffer_size] = '\r';
        
    }
    
    if (self->_vtab && (validate = virtualTo(self->_vtab, "validate")) != 0) {
        ret = ((int (*)(const void *, const void *, size_t)) validate)(self, cmd, strlen(cmd));
        if (ret != AAOS_OK) {
            if (cmd != write_buffer) {
                free(cmd);
            }
            return ret;
        }
    }
    
    Pthread_mutex_lock(&self->mtx);
    switch (self->state) {
        case SERIAL_STATE_ERROR:
            Pthread_mutex_unlock(&self->mtx);
            if (cmd != write_buffer) {
                free(cmd);
            }
            return AAOS_EDEVMAL;
            break;
        case SERIAL_STATE_UNLOADED:
            Pthread_mutex_unlock(&self->mtx);
            if (cmd != write_buffer) {
                free(cmd);
            }
            return AAOS_EDEVNOTLOADED;
            break;
        default:
            break;
    }
    while (self->state == SERIAL_STATE_WAIT_FOR_READY) {
        Pthread_cond_wait(&self->cond, &self->mtx);
    }
    
    Tcflush(self->fd, TCIOFLUSH);
    if ((ret = __Serial_write(self, cmd, strlen(cmd), write_size)) != AAOS_OK) {
        Pthread_mutex_unlock(&self->mtx);
        if (cmd != write_buffer) {
            free(cmd);
        }
        return ret;
    }
    if (cmd != write_buffer) {
        free(cmd);
    }
    
    if ((ret = __Serial_read(self, read_buffer, read_buffer_size, read_size)) != AAOS_OK) {
        Pthread_mutex_unlock(&self->mtx);
        return ret;
    }
    Pthread_mutex_unlock(&self->mtx);
    
    char *s = (char *) read_buffer;
    if (read_size != NULL) {
        if (s[*read_size - 1] == '\r' || s[*read_size - 1] == '\n')  {
            s[*read_size - 1] = '\0';
        } else {
            if (*read_size < read_buffer_size) {
                s[*read_size] = '\0';
                ++(*read_size);
            }
        }
    } else {
        if (s[strlen(s) - 1] == '\r' || s[strlen(s) - 1] == '\n') {
            s[strlen(s) - 1] = '\0';
        }
    }
    
    return AAOS_OK;
}

static int
JZDSerial_inspect(void *_self)
{
    char buf[BUFSIZE];
    return JZDSerial_raw(_self, "#0A0\r", 5, NULL, buf, BUFSIZE, NULL);
}

static const void *_jzd_serial_virtual_table;

static void
jzd_serial_virtual_table_destroy(void)
{
    delete((void *) _jzd_serial_virtual_table);
}

static void
jzd_serial_virtual_table_initialize(void)
{
    _jzd_serial_virtual_table = new(__SerialVirtualTable(),
                                    __serial_init, "init", JZDSerial_init,
                                    __serial_feed_dog, "feed_dog", JZDSerial_feed_dog,
                                    __serial_validate, "validate", JZDSerial_validate,
                                    __serial_raw, "raw", JZDSerial_raw,
                                    __serial_inspect, "inspect", JZDSerial_inspect,
                                    (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(jzd_serial_virtual_table_destroy);
#endif
}

static const void *
jzd_serial_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, jzd_serial_virtual_table_initialize);
#endif
    
    return _jzd_serial_virtual_table;
}

/*
 * SQM Serial driver, command set from SQM manual version 20191025
 */

static const char *pattern_sqm = "^(([RrciIsu]x)|(rfx)|(zcal[ARDSS]x)|(zcal[568][0-9]{8}\\.[0-9]{2}x)|(zcal7[0-9]{7}\\.[0-9]{3}x)|(baud[0-9]{10}x)|(A5[01ed]?x)|(Y[RrCcPpUu]?x)|([Pp][0-9]{10}x)|([Tt][0-9]{8}\\.[0-9]{2}))\r$";
static regex_t preg_sqm;

static const void *sqm_serial_virtual_table(void);

static void *
SQMSerial_ctor(void *_self, va_list *app)
{
    struct SQMSerial *self = super_ctor(SQMSerial(), _self, app);
    
    self->_._vtab= sqm_serial_virtual_table();
    
    return (void *) self;
}

static void *
SQMSerial_dtor(void *_self)
{
    return super_dtor(SQMSerial(), _self);
}

static void *
SQMSerialClass_ctor(void *_self, va_list *app)
{
    struct SQMSerialClass *self = super_ctor(SQMSerialClass(), _self, app);
    
    self->_.raw.method = (Method) 0;
    self->_.init.method = (Method) 0;
    self->_.validate.method = (Method) 0;
    
    return self;
}

static const void *_SQMSerialClass;

static void
SQMSerialClass_destroy(void)
{
    free((void *)_SQMSerialClass);
}

static void
SQMSerialClass_initialize(void)
{
    _SQMSerialClass = new(__SerialClass(), "SQMSerialClass", __SerialClass(), sizeof(struct SQMSerialClass),
                          ctor, "", SQMSerialClass_ctor,
                          (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(SQMSerialClass_destroy);
#endif
}

const void *
SQMSerialClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, SQMSerialClass_initialize);
#endif
    
    return _SQMSerialClass;
}

static const void *_SQMSerial;

static void
SQMSerial_destroy(void)
{
    regfree(&preg_sqm);
    free((void *)_SQMSerial);
}

static void
SQMSerial_initialize(void)
{
    regcomp(&preg_sqm, pattern_sqm, REG_EXTENDED | REG_NOSUB);
    _SQMSerial = new(SQMSerialClass(), "SQMSerial", __Serial(), sizeof(struct SQMSerial),
                     ctor, "ctor", SQMSerial_ctor,
                     dtor, "dtor", SQMSerial_dtor,
                     (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(SQMSerial_destroy);
#endif
}

const void *
SQMSerial(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, SQMSerial_initialize);
#endif

    return _SQMSerial;
}

static int
SQMSerial_init(void *_self)
{
    struct __Serial *self = cast(__Serial(), _self);
    struct termios termptr;
    int ibaud = B115200, obaud = B115200;
    
    self->fd = Open(self->path, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (self->fd < 0) {
        if (errno == ENOENT && (self->option&SERIAL_OPTION_WAIT_FOR_READY)) {
            pthread_t tid;
            self->state = SERIAL_STATE_WAIT_FOR_READY;
            Pthread_create(&tid, NULL, __Serial_wait_for_tty_ready_thread, self);
        } else {
            self->state = SERIAL_STATE_ERROR;
        }
        return AAOS_EBADF;
    } else {
        self->state = SERIAL_STATE_OK;
    }
    
    Tcgetattr(self->fd, &termptr);
    Cfsetispeed(&termptr, ibaud);
    Cfsetospeed(&termptr, obaud);
    termptr.c_iflag &= ~(BRKINT | IMAXBEL | INLCR);
    termptr.c_iflag |= (IGNBRK | ICRNL);
    termptr.c_oflag &= ~ (OPOST | OCRNL);
    termptr.c_oflag |= ONLCR;
    termptr.c_lflag &= ~(ISIG | IEXTEN | ECHO | ECHOE | ECHOK | ECHOCTL | ECHOKE);
    Tcsetattr(self->fd, TCSANOW, &termptr);
    
    return AAOS_OK;
}

static int
SQMSerial_feed_dog(void *_self)
{
    return AAOS_OK;
}

static int
SQMSerial_validate(const void *_self, const void *command, size_t size)
{
    if (regexec(&preg_sqm, command, 0, NULL, 0) != 0) {
        return AAOS_EBADCMD;
    } else {
        return AAOS_OK;
    }
}

static int
SQMSerial_raw(void *_self, const void *write_buffer, size_t write_buffer_size, size_t *write_size, void *read_buffer, size_t read_buffer_size, size_t *read_size)
{
    struct __Serial *self = cast(__Serial(), _self);
    
    int ret = AAOS_OK;
    Method validate;
    char *cmd = (char *) write_buffer;

    /*
     * append '\r'
     */
    
    if (cmd[write_buffer_size - 1] != '\r') {
        if ((cmd = (char *) Malloc(write_buffer_size + 2)) == NULL) {
            return AAOS_ENOMEM;
        }
        memset(cmd, '\0', write_buffer_size + 2);
        memcpy(cmd, write_buffer, write_buffer_size);
        cmd[write_buffer_size] = '\r';
    }
    
    if (self->_vtab && (validate = virtualTo(self->_vtab, "validate")) != 0) {
        ret = ((int (*)(const void *, const void *, size_t)) validate)(self, cmd, strlen(cmd));
        if (ret != AAOS_OK) {
            if (cmd != write_buffer) {
                free(cmd);
            }
            return ret;
        }
    }
    
    Pthread_mutex_lock(&self->mtx);
    switch (self->state) {
        case SERIAL_STATE_ERROR:
            Pthread_mutex_unlock(&self->mtx);
            if (cmd != write_buffer) {
                free(cmd);
            }
            return AAOS_EDEVMAL;
            break;
        case SERIAL_STATE_UNLOADED:
            Pthread_mutex_unlock(&self->mtx);
            if (cmd != write_buffer) {
                free(cmd);
            }
            return AAOS_EDEVNOTLOADED;
            break;
        default:
            break;
    }
    while (self->state == SERIAL_STATE_WAIT_FOR_READY) {
        Pthread_cond_wait(&self->cond, &self->mtx);
    }
    Tcflush(self->fd, TCIOFLUSH);
    if ((ret = __Serial_write(self, cmd, strlen(cmd), write_size)) != AAOS_OK) {
        Pthread_mutex_unlock(&self->mtx);
        if (cmd != write_buffer) {
            free(cmd);
        }
        return ret;
    }
    if (cmd != write_buffer) {
        free(cmd);
    }
    
    if ((ret = __Serial_read(self, read_buffer, read_buffer_size, read_size)) != AAOS_OK) {
        Pthread_mutex_unlock(&self->mtx);
        return ret;
    }
    Pthread_mutex_unlock(&self->mtx);
    
    char *s = (char *) read_buffer;
    if (read_size != NULL) {
        if (s[*read_size - 1] == '\r' || s[*read_size - 1] == '\n')  {
            s[*read_size - 1] = '\0';
            if (*read_size > 1 && (s[*read_size - 2] == '\r' || s[*read_size - 2] == '\n')) {
                s[*read_size - 2] = '\0';
            }
        } else {
            if (*read_size < read_buffer_size) {
                s[*read_size] = '\0';
                ++(*read_size);
            }
        }
    } else {
        size_t length = strlen(s);
        if (s[length - 1] == '\r' || s[length - 1] == '\n') {
            s[strlen(s) - 1] = '\0';
            if (length > 1 && (s[length - 2] == '\r' || s[length - 2] == '\n')) {
                s[strlen(s) - 2] = '\0';
            }
        }
    }
    
    return AAOS_OK;
}

static int
SQMSerial_inspect(void *_self)
{
    char buf[BUFSIZE];
    return SQMSerial_raw(_self, "rx\r", 3, NULL, buf, BUFSIZE, NULL);
}

static const void *_sqm_serial_virtual_table;

static void
sqm_serial_virtual_table_destroy(void)
{
    delete((void *) _sqm_serial_virtual_table);
}


static void
sqm_serial_virtual_table_initialize(void)
{
    _sqm_serial_virtual_table = new(__SerialVirtualTable(),
                                    __serial_init, "init", SQMSerial_init,
                                    __serial_feed_dog, "feed_dog", SQMSerial_feed_dog,
                                    __serial_validate, "validate", SQMSerial_validate,
                                    __serial_raw, "raw", SQMSerial_raw,
                                    __serial_inspect, "inspect", SQMSerial_inspect,
                                    (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(sqm_serial_virtual_table_destroy);
#endif
}

static const void *
sqm_serial_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, sqm_serial_virtual_table_initialize);
#endif
    
    return _sqm_serial_virtual_table;
}

/*
 * WS100UMB Serial driver
 */

static const char *pattern_ws100umb = "^(([EM][0-9])|([IR][0-1])|X0)\r$";
static regex_t preg_ws100umb;

static const void *ws100umb_serial_virtual_table(void);

static void *
WS100UMBSerial_ctor(void *_self, va_list *app)
{
    struct WS100UMBSerial *self = super_ctor(WS100UMBSerial(), _self, app);
    
    self->_._vtab= ws100umb_serial_virtual_table();
    
    return (void *) self;
}

static void *
WS100UMBSerial_dtor(void *_self)
{
    return super_dtor(WS100UMBSerial(), _self);
}

static void *
WS100UMBSerialClass_ctor(void *_self, va_list *app)
{
    struct WS100UMBSerialClass *self = super_ctor(WS100UMBSerialClass(), _self, app);
    
    self->_.raw.method = (Method) 0;
    self->_.init.method = (Method) 0;
    self->_.validate.method = (Method) 0;
    
    return self;
}

static const void *_WS100UMBSerialClass;

static void
WS100UMBSerialClass_destroy(void)
{
    free((void *)_WS100UMBSerialClass);
}

static void
WS100UMBSerialClass_initialize(void)
{
    _WS100UMBSerialClass = new(__SerialClass(), "WS100UMBSerialClass", __SerialClass(), sizeof(struct WS100UMBSerialClass),
                          ctor, "", WS100UMBSerialClass_ctor,
                          (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(WS100UMBSerialClass_destroy);
#endif
}

const void *
WS100UMBSerialClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, WS100UMBSerialClass_initialize);
#endif
    
    return _WS100UMBSerialClass;
}

static const void *_WS100UMBSerial;

static void
WS100UMBSerial_destroy(void)
{
    regfree(&preg_ws100umb);
    free((void *)_WS100UMBSerial);
}

static void
WS100UMBSerial_initialize(void)
{
    regcomp(&preg_ws100umb, pattern_ws100umb, REG_EXTENDED | REG_NOSUB);
    _WS100UMBSerial = new(WS100UMBSerialClass(), "WS100UMBSerial", __Serial(), sizeof(struct WS100UMBSerial),
                     ctor, "ctor", WS100UMBSerial_ctor,
                     dtor, "dtor", WS100UMBSerial_dtor,
                     (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(WS100UMBSerial_destroy);
#endif
}

const void *
WS100UMBSerial(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, WS100UMBSerial_initialize);
#endif

    return _WS100UMBSerial;
}

static int
WS100UMBSerial_init(void *_self)
{
    struct __Serial *self = cast(__Serial(), _self);
    struct termios termptr;
    int ibaud = B19200, obaud = B19200;
    
    self->fd = Open(self->path, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (self->fd < 0) {
        if (errno == ENOENT && (self->option&SERIAL_OPTION_WAIT_FOR_READY)) {
            self->state = SERIAL_STATE_WAIT_FOR_READY;
        } else {
            self->state = SERIAL_STATE_ERROR;
        }
        return AAOS_EBADF;
    } else {
        self->state = SERIAL_STATE_OK;
    }
    
    Tcgetattr(self->fd, &termptr);
    Cfsetispeed(&termptr, ibaud);
    Cfsetospeed(&termptr, obaud);
    termptr.c_iflag &= ~(BRKINT | IMAXBEL | INLCR);
    termptr.c_iflag |= (IGNBRK | ICRNL);
    termptr.c_oflag &= ~ (OPOST | OCRNL);
    termptr.c_oflag |= ONLCR;
    termptr.c_lflag &= ~(ISIG | IEXTEN | ECHO | ECHOE | ECHOK | ECHOCTL | ECHOKE);
    Tcsetattr(self->fd, TCSANOW, &termptr);
    
    return AAOS_OK;
}

static int
WS100UMBSerial_feed_dog(void *_self)
{
    return AAOS_OK;
}

static int
WS100UMBSerial_validate(const void *_self, const void *command, size_t size)
{
    if (regexec(&preg_ws100umb, command, 0, NULL, 0) != 0) {
        return AAOS_EBADCMD;
    } else {
        return AAOS_OK;
    }
}

static int
WS100UMBSerial_raw(void *_self, const void *write_buffer, size_t write_buffer_size, size_t *write_size, void *read_buffer, size_t read_buffer_size, size_t *read_size)
{
    struct __Serial *self = cast(__Serial(), _self);
    
    int ret = AAOS_OK;
    Method validate;
    char *cmd = (char *) write_buffer;

    /*
     * append '\r'
     */
    
    if (cmd[write_buffer_size - 1] != '\r') {
        if ((cmd = (char *) Malloc(write_buffer_size + 2)) == NULL) {
            return AAOS_ENOMEM;
        }
        memset(cmd, '\0', write_buffer_size + 2);
        memcpy(cmd, write_buffer, write_buffer_size);
        cmd[write_buffer_size] = '\r';
    }
    
    if (self->_vtab && (validate = virtualTo(self->_vtab, "validate")) != 0) {
        ret = ((int (*)(const void *, const void *, size_t)) validate)(self, cmd, strlen(cmd));
        if (ret != AAOS_OK) {
            if (cmd != write_buffer) {
                free(cmd);
            }
            return ret;
        }
    }
    
    Pthread_mutex_lock(&self->mtx);
    switch (self->state) {
        case SERIAL_STATE_ERROR:
            Pthread_mutex_unlock(&self->mtx);
            if (cmd != write_buffer) {
                free(cmd);
            }
            return AAOS_EDEVMAL;
            break;
        case SERIAL_STATE_UNLOADED:
            Pthread_mutex_unlock(&self->mtx);
            if (cmd != write_buffer) {
                free(cmd);
            }
            return AAOS_EDEVNOTLOADED;
            break;
        default:
            break;
    }
    while (self->state == SERIAL_STATE_WAIT_FOR_READY) {
        Pthread_cond_wait(&self->cond, &self->mtx);
    }
    Tcflush(self->fd, TCIOFLUSH);
    if ((ret = __Serial_write(self, cmd, strlen(cmd), write_size)) != AAOS_OK) {
        Pthread_mutex_unlock(&self->mtx);
        if (cmd != write_buffer) {
            free(cmd);
        }
        return ret;
    }
    if (cmd != write_buffer) {
        free(cmd);
    }
    
    if ((ret = __Serial_read(self, read_buffer, read_buffer_size, read_size)) != AAOS_OK) {
        Pthread_mutex_unlock(&self->mtx);
        return ret;
    }
    Pthread_mutex_unlock(&self->mtx);
    
    char *s = (char *) read_buffer;
    if (read_size != NULL) {
        if (s[*read_size - 1] == '\r' || s[*read_size - 1] == '\n')  {
            s[*read_size - 1] = '\0';
            if (*read_size > 1 && (s[*read_size - 2] == '\r' || s[*read_size - 2] == '\n')) {
                s[*read_size - 2] = '\0';
            }
        } else {
            if (*read_size < read_buffer_size) {
                s[*read_size] = '\0';
                ++(*read_size);
            }
        }
    } else {
        size_t length = strlen(s);
        if (s[length - 1] == '\r' || s[length - 1] == '\n') {
            s[strlen(s) - 1] = '\0';
            if (length > 1 && (s[length - 2] == '\r' || s[length - 2] == '\n')) {
                s[strlen(s) - 2] = '\0';
            }
        }
    }
    
    return AAOS_OK;
}

static int
WS100UMBSerial_inspect(void *_self)
{
    char buf[BUFSIZE];
    return WS100UMBSerial_raw(_self, "E0\r", 3, NULL, buf, BUFSIZE, NULL);
}

static const void *_ws100umb_serial_virtual_table;

static void
ws100umb_serial_virtual_table_destroy(void)
{
    delete((void *) _ws100umb_serial_virtual_table);
}


static void
ws100umb_serial_virtual_table_initialize(void)
{
    _ws100umb_serial_virtual_table = new(__SerialVirtualTable(),
                                    __serial_init, "init", WS100UMBSerial_init,
                                    __serial_feed_dog, "feed_dog", WS100UMBSerial_feed_dog,
                                    __serial_validate, "validate", WS100UMBSerial_validate,
                                    __serial_raw, "raw", WS100UMBSerial_raw,
                                    __serial_inspect, "inspect", WS100UMBSerial_inspect,
                                    (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(ws100umb_serial_virtual_table_destroy);
#endif
}

static const void *
ws100umb_serial_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, ws100umb_serial_virtual_table_initialize);
#endif
    
    return _ws100umb_serial_virtual_table;
}


/*
 * Power distributing Unit deveoped by Antarctica Astronomy Group.
 */
static const char *pattern_aag_pdu = "^((BR)|(BD)|(B[H|L][0-9A-F]{8})|(BM[0-2][0-9])|(BM3[0-1])|(BT0[0-9])|(BT1[1-5]))\n$";
static regex_t preg_aag_pdu;

static const void *aag_pdu_serial_virtual_table(void);

static void *
AAGPDUSerial_ctor(void *_self, va_list *app)
{
    struct AAGPDUSerial *self = super_ctor(AAGPDUSerial(), _self, app);
    
    self->_._vtab= aag_pdu_serial_virtual_table();
    
    return (void *) self;
}

static void *
AAGPDUSerial_dtor(void *_self)
{
    return super_dtor(AAGPDUSerial(), _self);
}

static void *
AAGPDUSerialClass_ctor(void *_self, va_list *app)
{
    struct AAGPDUSerialClass *self = super_ctor(AAGPDUSerialClass(), _self, app);
    
    self->_.raw.method = (Method) 0;
    self->_.init.method = (Method) 0;
    self->_.validate.method = (Method) 0;
    self->_.feed_dog.method = (Method) 0;
    
    return self;
}

static const void *_AAGPDUSerialClass;

static void
AAGPDUSerialClass_destroy(void)
{
    free((void *)_AAGPDUSerialClass);
}

static void
AAGPDUSerialClass_initialize(void)
{
    _AAGPDUSerialClass = new(__SerialClass(), "AAGPDUSerialClass", __SerialClass(), sizeof(struct AAGPDUSerialClass),
                             ctor, "", AAGPDUSerialClass_ctor,
                             (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(AAGPDUSerialClass_destroy);
#endif
}

const void *
AAGPDUSerialClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, AAGPDUSerialClass_initialize);
#endif
    
    return _AAGPDUSerialClass;
}

static const void *_AAGPDUSerial;

static void
AAGPDUSerial_destroy(void)
{
    regfree(&preg_aag_pdu);
    free((void *)_AAGPDUSerial);
}

static void
AAGPDUSerial_initialize(void)
{
    regcomp(&preg_aag_pdu, pattern_aag_pdu, REG_EXTENDED | REG_NOSUB);
    _AAGPDUSerial = new(AAGPDUSerialClass(), "AAGPDUSerial", __Serial(), sizeof(struct AAGPDUSerial),
                        ctor, "ctor", AAGPDUSerial_ctor,
                        dtor, "dtor", AAGPDUSerial_dtor,
                        (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(AAGPDUSerial_destroy);
#endif
}

const void *
AAGPDUSerial(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, AAGPDUSerial_initialize);
#endif
    
    return _AAGPDUSerial;
}

static int
AAGPDUSerial_init(void *_self)
{
    struct __Serial *self = cast(__Serial(), _self);
    struct termios termptr;
    int ibaud = B9600, obaud = B9600;
    
    self->fd = Open(self->path, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (self->fd < 0) {
        if (errno == ENOENT && (self->option&SERIAL_OPTION_WAIT_FOR_READY)) {
            pthread_t tid;
            self->state = SERIAL_STATE_WAIT_FOR_READY;
            Pthread_create(&tid, NULL, __Serial_wait_for_tty_ready_thread, self);
        } else {
            self->state = SERIAL_STATE_ERROR;
        }
        return AAOS_EBADF;
    } else {
        self->state = SERIAL_STATE_OK;
    }
    
    Tcgetattr(self->fd, &termptr);
    Cfsetispeed(&termptr, ibaud);
    Cfsetospeed(&termptr, obaud);
    termptr.c_iflag &= ~(BRKINT | IMAXBEL | INLCR);
    termptr.c_iflag |= (IGNBRK | ICRNL);
    termptr.c_oflag &= ~ (OPOST | OCRNL);
    termptr.c_oflag |= ONLCR;
    termptr.c_lflag &= ~(ISIG | IEXTEN | ECHO | ECHOE | ECHOK | ECHOCTL | ECHOKE);
    Tcsetattr(self->fd, TCSANOW, &termptr);
    
    return AAOS_OK;
}

static int
AAGPDUSerial_validate(const void *_self, const void *command, size_t size)
{
    if (regexec(&preg_aag_pdu, command, 0, NULL, 0) != 0) {
        return AAOS_EBADCMD;
    } else {
        return AAOS_OK;
    }
}

static int
AAGPDUSerial_raw(void *_self, const void *write_buffer, size_t write_buffer_size, size_t *write_size, void *read_buffer, size_t read_buffer_size, size_t *read_size)
{
    struct __Serial *self = cast(__Serial(), _self);
    
    int ret = AAOS_OK;
    Method validate;
    char *cmd = (char *) write_buffer;
    /*
     * append '\n'
     */
    if (cmd[write_buffer_size - 1] != '\n') {
        if ((cmd = (char *) Malloc(write_buffer_size + 2)) == NULL) {
            return AAOS_ENOMEM;
        }
        memset(cmd, '\0', write_buffer_size + 2);
        memcpy(cmd, write_buffer, write_buffer_size);
        cmd[write_buffer_size] = '\n';
    }
    
    if (self->_vtab && (validate = virtualTo(self->_vtab, "validate")) != 0) {
        ret = ((int (*)(const void *, const void *, size_t)) validate)(self, cmd, strlen(cmd));
        if (ret != AAOS_OK) {
            if (cmd != write_buffer) {
                free(cmd);
            }
            return ret;
        }
    }
    
    Pthread_mutex_lock(&self->mtx);
    switch (self->state) {
        case SERIAL_STATE_ERROR:
            Pthread_mutex_unlock(&self->mtx);
            if (cmd != write_buffer) {
                free(cmd);
            }
            return AAOS_EDEVMAL;
            break;
        case SERIAL_STATE_UNLOADED:
            Pthread_mutex_unlock(&self->mtx);
            if (cmd != write_buffer) {
                free(cmd);
            }
            return AAOS_EDEVNOTLOADED;
            break;
        default:
            break;
    }
    while (self->state == SERIAL_STATE_WAIT_FOR_READY) {
        Pthread_cond_wait(&self->cond, &self->mtx);
    }
    Tcflush(self->fd, TCIOFLUSH);
    if ((ret = __Serial_write(self, cmd, strlen(cmd), write_size)) != AAOS_OK) {
        Pthread_mutex_unlock(&self->mtx);
        if (cmd != write_buffer) {
            free(cmd);
        }
        return ret;
    }
    if (cmd != write_buffer) {
        free(cmd);
    }
    if ((ret = __Serial_read(self, read_buffer, read_buffer_size, read_size)) != AAOS_OK) {
        Pthread_mutex_unlock(&self->mtx);
        return ret;
    }
    Pthread_mutex_unlock(&self->mtx);
    
    char *s = (char *) read_buffer;
    if (read_size != NULL) {
        if (s[*read_size - 1] == '\r' || s[*read_size - 1] == '\n')  {
            s[*read_size - 1] = '\0';
        } else {
            if (*read_size < read_buffer_size) {
                s[*read_size] = '\0';
                ++(*read_size);
            }
        }
    } else {
        if (s[strlen(s) - 1] == '\r' || s[strlen(s) - 1] == '\n') {
            s[strlen(s) - 1] = '\0';
        }
    }
    
    return AAOS_OK;
}

static int
AAGPDUSerial_inspect(void *_self)
{
    char buf[BUFSIZE];
    return AAGPDUSerial_raw(_self, "BR\n", 3, NULL, buf, BUFSIZE, NULL);
}

static int
AAGPDUSerial_feed_dog(void *_self)
{
    char command[COMMANDSIZE];
    char buf[COMMANDSIZE];
    snprintf(command, COMMANDSIZE, "BD\n");
    return AAGPDUSerial_raw(_self, command, strlen(command), NULL, buf, COMMANDSIZE, NULL);
}

static const void *_aag_pdu_serial_virtual_table;

static void
aag_pdu_serial_virtual_table_destroy(void)
{
    delete((void *) _aag_pdu_serial_virtual_table);
}

static void
aag_pdu_serial_virtual_table_initialize(void)
{
    _aag_pdu_serial_virtual_table = new(__SerialVirtualTable(),
                                        __serial_init, "init", AAGPDUSerial_init,
                                        __serial_feed_dog, "feed_dog", AAGPDUSerial_feed_dog,
                                        __serial_validate, "validate", AAGPDUSerial_validate,
                                        __serial_raw, "raw", AAGPDUSerial_raw,
                                        __serial_inspect, "inspect", AAGPDUSerial_inspect,
                                        (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(aag_pdu_serial_virtual_table_destroy);
#endif
}

static const void *
aag_pdu_serial_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, aag_pdu_serial_virtual_table_initialize);
#endif
    
    return _aag_pdu_serial_virtual_table;
}

/*
 * Astrophysics mount serial.
 */
static const char *pattern_apmount = "^(#|(:SG [+-]?[0-9]\{2}(:[0-9]{2}((\\.[0-9])|(:[0-9]{2}))?)?#)|(:Sg [0-9]{3}\\*[0-9]{2}(:[0-9]{2})?#)|(:St [+-]?[0-9]{2}\\*[0-9]{2}(:[0-9]\{2})?#)|(:SL [0-9]{2}:[0-9]{2}:[0-9]{2}#)|(:SC [0-9]{2}/[0-9]{2}/[0-9]{2}#)|(:[SB]r [0-9]{2}:[0-9]{2}:[0-9]{2}(\\.[0-9])?#)|(:S[da] [+-]?[0-9]{2}\\*[0-9]{2}(:[0-9]\{2})?#)|(:Sz [0-9]{3}\\*[0-9]{2}(:[0-9]\{2})?#)|(:B[dr] [0-9]{2}\\*[0-9]{2}:[0-9]\{2}#)|(:G[GgtLSRDAZC]#)|(:Q[ewns]?#)|(:RG[0-2]?#)|(:RC[0-3]?#)|(:RS[0-2]?#)|(:RT[0-29]?#)|(:M[news]([0-9]{3})?#)|(:Rc[0-9]{3}#)|(:Rs[0-9]{4}#)|(R[R|D] [+-]?[0-9]{3}\\.[0-9]{4}#)|(:(MS|NS|EW|KA|p[SRP]?|PO|FM|EM|CMR?|U|B[+-]|F[+-FSQ]|d[en]|V|h[oq])#))+$";
static regex_t preg_apmount;

static const char *pattern_apmount_have_return = "(:SG [+-]?[0-9]\{2}(:[0-9]{2}((\\.[0-9])|(:[0-9]{2}))?)?#)|(:Sg [0-9]{3}\\*[0-9]{2}(:[0-9]{2})?#)|(:St [+-]?[0-9]{2}\\*[0-9]{2}(:[0-9]\{2})?#)|(:SL [0-9]{2}:[0-9]{2}:[0-9]{2}#)|(:SC [0-9]{2}/[0-9]{2}/[0-9]{2}#)|(:[SB]r [0-9]{2}:[0-9]{2}:[0-9]{2}(\\.[0-9])?#)|(:S[da] [+-]?[0-9]{2}\\*[0-9]{2}(:[0-9]\{2})?#)|(:Sz [0-9]{3}\\*[0-9]{2}(:[0-9]\{2})?#)|(:B[dr] [0-9]{2}\\*[0-9]{2}:[0-9]\{2}#)|(:G[GgtLSRDAZC]#)|(R[R|D] [+-]?[0-9]{3}\\.[0-9]{4}#)|(:(MS|pS|CMR?|V)#)";
static regex_t preg_apmount_have_return;

static const void *apmount_serial_virtual_table(void);

#define APMOUNT_TIMEOUT 0.01

static void *
APMountSerial_ctor(void *_self, va_list *app)
{
    struct APMountSerial *self = super_ctor(APMountSerial(), _self, app);
    
    self->timeout = APMOUNT_TIMEOUT;
    self->_._vtab= apmount_serial_virtual_table();
    
    return (void *) self;
}

static void *
APMountSerial_dtor(void *_self)
{
    return super_dtor(APMountSerial(), _self);
}

static void *
APMountSerialClass_ctor(void *_self, va_list *app)
{
    struct APMountSerialClass *self = super_ctor(APMountSerialClass(), _self, app);
    
    self->_.raw.method = (Method) 0;
    self->_.init.method = (Method) 0;
    self->_.validate.method = (Method) 0;
    
    return self;
}

static const void *_APMountSerialClass;

static void
APMountSerialClass_destroy(void)
{
    free((void *) _APMountSerialClass);
}

static void
APMountSerialClass_initialize(void)
{
    _APMountSerialClass = new(__SerialClass(), "APMountSerialClass", __SerialClass(), sizeof(struct APMountSerialClass),
                              ctor, "", APMountSerialClass_ctor,
                              (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(APMountSerialClass_destroy);
#endif
}

const void *
APMountSerialClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, APMountSerialClass_initialize);
#endif
    
    return _APMountSerialClass;
}

static const void *_APMountSerial;

static void
APMountSerial_destroy(void)
{
    regfree(&preg_apmount);
    regfree(&preg_apmount_have_return);
    free((void *)_APMountSerial);
}

static void
APMountSerial_initialize(void)
{
    regcomp(&preg_apmount, pattern_apmount, REG_EXTENDED | REG_NOSUB);
    regcomp(&preg_apmount_have_return, pattern_apmount_have_return, REG_EXTENDED | REG_NOSUB);
    _APMountSerial = new(APMountSerialClass(), "APMountSerial", __Serial(), sizeof(struct APMountSerial),
                         ctor, "ctor", APMountSerial_ctor,
                         dtor, "dtor", APMountSerial_dtor,
                         (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(APMountSerial_destroy);
#endif
}

const void *
APMountSerial(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, APMountSerial_initialize);
#endif

    return _APMountSerial;
}

static int
APMountSerial_init(void *_self)
{
    struct __Serial *self = cast(__Serial(), _self);
    struct termios termptr;
    int ibaud = B9600, obaud = B9600;
    
    self->fd = Open(self->path, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (self->fd < 0) {
        if (errno == ENOENT && (self->option&SERIAL_OPTION_WAIT_FOR_READY)) {
            pthread_t tid;
            self->state = SERIAL_STATE_WAIT_FOR_READY;
            Pthread_create(&tid, NULL, __Serial_wait_for_tty_ready_thread, self);
        } else {
            self->state = SERIAL_STATE_ERROR;
        }
        return AAOS_EBADF;
    } else {
        self->state = SERIAL_STATE_OK;
    }
    
    Tcgetattr(self->fd, &termptr);
    Cfsetispeed(&termptr, ibaud);
    Cfsetospeed(&termptr, obaud);
    termptr.c_lflag &= ~(ISIG | ICANON | IEXTEN | ISIG | ECHO | ECHOE | ECHOK | ECHOCTL | ECHOKE);
    termptr.c_iflag &= ~(IXON | BRKINT | ICRNL | INPCK | ISTRIP);
    termptr.c_cflag &= ~(CSIZE | PARENB);
    termptr.c_cflag |= CS8;
    termptr.c_oflag |= OPOST;
    termptr.c_cc[VMIN] = 1;
    termptr.c_cc[VTIME] = 0;                                                             
    
    Tcsetattr(self->fd, TCSANOW, &termptr);
    
    return AAOS_OK;
}

static int
APMountSerial_raw(void *_self, void *write_buffer, size_t write_buffer_size, size_t *write_size, void *read_buffer, size_t read_buffer_size, size_t *read_size)
{
    struct __Serial *self = cast(__Serial(), _self);
    
    int ret = AAOS_OK;
    Method validate;
    
    if (self->_vtab && (validate = virtualTo(self->_vtab, "validate")) != 0) {
        ret = ((int (*)(const void *, const void *, size_t)) validate)(self, write_buffer, write_buffer_size);
        if (ret != AAOS_OK) {
            return ret;
        }
    }
    
    Pthread_mutex_lock(&self->mtx);
    switch (self->state) {
        case SERIAL_STATE_ERROR:
            Pthread_mutex_unlock(&self->mtx);
            return AAOS_EDEVMAL;
            break;
        case SERIAL_STATE_UNLOADED:
            Pthread_mutex_unlock(&self->mtx);
            return AAOS_EDEVNOTLOADED;
            break;
        default:
            break;
    }
    while (self->state == SERIAL_STATE_WAIT_FOR_READY) {
        Pthread_cond_wait(&self->cond, &self->mtx);

     }
    /*
     * must flush before IO operation.
     */
    Tcflush(self->fd, TCIOFLUSH);
    
    if ((ret = __Serial_write(self, write_buffer, write_buffer_size, write_size)) != AAOS_OK) {
        Pthread_mutex_unlock(&self->mtx);
        return ret;
    }
    
    if (regexec(&preg_apmount_have_return, write_buffer, 0, NULL, 0) != 0) {
        snprintf(read_buffer, read_buffer_size, "OK");
        if (read_size) {
            *read_size = strlen((char *) read_buffer) + 1;
        }
        Pthread_mutex_unlock(&self->mtx);
        return AAOS_OK;
    }
    
    if ((ret = __Serial_read(self, read_buffer, read_buffer_size, read_size)) != AAOS_OK) {
        Pthread_mutex_unlock(&self->mtx);
        return ret;
    }
    
    cast(APMountSerial(), _self);
    double old_timeout = self->read_timeout;
    self->read_timeout = ((struct APMountSerial *) self)->timeout;
    size_t nleft = read_buffer_size - strlen((char *) read_buffer), nread = 0, n;
    char *s = (char *) read_buffer + nleft;
    
    while ((ret = __Serial_read(self, s, nleft, &n)) == AAOS_OK) {
        nleft -= n;
        nread += n;
        s += n;
        if (nleft == 0) {
            break;
        }
    }
    if (read_size) {
        *read_size += nread;
    }
    self->read_timeout = old_timeout;
    Pthread_mutex_unlock(&self->mtx);
    
    s = (char *) read_buffer;
    if (read_size != NULL) {
        if (*read_size < read_buffer_size) {
            s[*read_size] = '\0';
            ++(*read_size);
        }
    }

    return AAOS_OK;
}

static int
APMountSerial_feed_dog(void *_self)
{
    return AAOS_OK;
}

static int
APMountSerial_validate(const void *_self, const void *command, size_t size)
{
    if (regexec(&preg_apmount, command, 0, NULL, 0) != 0) {
        return AAOS_EBADCMD;
    } else {
        return AAOS_OK;
    }
}

static int
APMountSerial_inspect(void *_self)
{
    char buf[BUFSIZE];
    return APMountSerial_raw(_self, "GG", 2, NULL, buf, BUFSIZE, NULL);
}

static const void *_apmount_serial_virtual_table;

static void
apmount_serial_virtual_table_destroy(void)
{
    delete((void *) _apmount_serial_virtual_table);
}

static void
apmount_serial_virtual_table_initialize(void)
{
    _apmount_serial_virtual_table = new(__SerialVirtualTable(),
                                        __serial_init, "init", APMountSerial_init,
                                        __serial_feed_dog, "feed_dog", APMountSerial_feed_dog,
                                        __serial_validate, "validate", APMountSerial_validate,
                                        __serial_raw, "raw", APMountSerial_raw,
                                        __serial_inspect, "inspect", APMountSerial_inspect,
                                        (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(apmount_serial_virtual_table_destroy);
#endif
}

static const void *
apmount_serial_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, apmount_serial_virtual_table_initialize);
#endif
    
    return _apmount_serial_virtual_table;
}

/*
 * HCD6871C serial.
 */

static const void *hcd6817c_serial_virtual_table(void);

#define HCD6817C_TIMEOUT 2.0
#define HCD6871C_CMD_LEN 8

static void *
HCD6817CSerial_ctor(void *_self, va_list *app)
{
    struct HCD6817CSerial *self = super_ctor(HCD6817CSerial(), _self, app);
    
    self->_._vtab= hcd6817c_serial_virtual_table();
    self->timeout = HCD6817C_TIMEOUT;
    
    return (void *) self;
}

static void *
HCD6817CSerial_dtor(void *_self)
{
    return super_dtor(HCD6817CSerial(), _self);
}

static void *
HCD6817CSerialClass_ctor(void *_self, va_list *app)
{
    struct HCD6817CSerialClass *self = super_ctor(HCD6817CSerialClass(), _self, app);
    
    self->_.raw.method = (Method) 0;
    self->_.init.method = (Method) 0;
    self->_.validate.method = (Method) 0;
    
    return self;
}

static const void *_HCD6817CSerialClass;

static void
HCD6817CSerialClass_destroy(void)
{
    free((void *) _HCD6817CSerialClass);
}

static void
HCD6817CSerialClass_initialize(void)
{
    _HCD6817CSerialClass = new(__SerialClass(), "HCD6817CSerialClass", __SerialClass(), sizeof(struct HCD6817CSerialClass),
                              ctor, "", HCD6817CSerialClass_ctor,
                              (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(HCD6817CSerialClass_destroy);
#endif
}

const void *
HCD6817CSerialClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, HCD6817CSerialClass_initialize);
#endif
    
    return _HCD6817CSerialClass;
}

static const void *_HCD6817CSerial;

static void
HCD6817CSerial_destroy(void)
{
    //regfree(&preg_apmount);
    //regfree(&preg_apmount_have_return);
    free((void *)_HCD6817CSerial);
}

static void
HCD6817CSerial_initialize(void)
{
    //regcomp(&preg_apmount, pattern_apmount, REG_EXTENDED | REG_NOSUB);
    //regcomp(&preg_apmount_have_return, pattern_apmount_have_return, REG_EXTENDED | REG_NOSUB);
    _HCD6817CSerial = new(HCD6817CSerialClass(), "HCD6817CSerial", __Serial(), sizeof(struct HCD6817CSerial),
                         ctor, "ctor", HCD6817CSerial_ctor,
                         dtor, "dtor", HCD6817CSerial_dtor,
                         (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(HCD6817CSerial_destroy);
#endif
}

const void *
HCD6817CSerial(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, HCD6817CSerial_initialize);
#endif

    return _HCD6817CSerial;
}

static int
HCD6817CSerial_init(void *_self)
{
    struct __Serial *self = cast(__Serial(), _self);
    struct termios termptr;
    int ibaud = B9600, obaud = B9600;
    
    self->fd = Open(self->path, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (self->fd < 0) {
        if (errno == ENOENT && (self->option&SERIAL_OPTION_WAIT_FOR_READY)) {
            pthread_t tid;
            self->state = SERIAL_STATE_WAIT_FOR_READY;
            Pthread_create(&tid, NULL, __Serial_wait_for_tty_ready_thread, self);
        } else {
            self->state = SERIAL_STATE_ERROR;
        }
        return AAOS_EBADF;
    } else {
        self->state = SERIAL_STATE_OK;
    }
    
    Tcgetattr(self->fd, &termptr);
    Cfsetispeed(&termptr, ibaud);
    Cfsetospeed(&termptr, obaud);
    termptr.c_lflag &= ~(ISIG | ICANON | IEXTEN | ISIG | ECHO | ECHOE | ECHOK | ECHOCTL | ECHOKE);
    termptr.c_iflag &= ~(IXON | BRKINT | ICRNL | INPCK | ISTRIP);
    termptr.c_cflag &= ~(CSIZE | PARENB);
    termptr.c_cflag |= CS8;
    termptr.c_cflag &= ~CSTOPB;
    termptr.c_iflag |= IGNPAR;
    termptr.c_oflag |= OPOST;
    termptr.c_cc[VMIN] = 1;
    termptr.c_cc[VTIME] = 0;
    
    Tcsetattr(self->fd, TCSANOW, &termptr);
    
    return AAOS_OK;
}

static uint16_t
crc16_modbus(uint8_t *data, size_t length) /* copy from https://github.com/whik/crc-lib-c */
{
    uint8_t i;
    uint16_t crc = 0xffff;
    
    while (length--) {
        crc ^= *data++;
        for (i = 0; i < 8; ++i) {
            if (crc & 1) {
                crc = (crc >> 1) ^ 0xA001;
            } else {
                crc = (crc >> 1);
            }
        }
    }
    
    return crc;
}

static inline uint16_t
swap_uint16(uint16_t val)
{
    return (val << 8) | (val >> 8 );
}

static int
HCD6817CSerial_raw(void *_self, void *write_buffer, size_t write_buffer_size, size_t *write_size, void *read_buffer, size_t read_buffer_size, size_t *read_size)
{
    struct __Serial *self = cast(__Serial(), _self);
    
    int ret = AAOS_OK;
    
    Pthread_mutex_lock(&self->mtx);
    switch (self->state) {
        case SERIAL_STATE_ERROR:
            Pthread_mutex_unlock(&self->mtx);
            return AAOS_EDEVMAL;
            break;
        case SERIAL_STATE_UNLOADED:
            Pthread_mutex_unlock(&self->mtx);
            return AAOS_EDEVNOTLOADED;
            break;
        default:
            break;
    }
    while (self->state == SERIAL_STATE_WAIT_FOR_READY) {
        Pthread_cond_wait(&self->cond, &self->mtx);
    }
    /*
     * must flush before IO operation.
     */
    Tcflush(self->fd, TCIOFLUSH);
   
    if (write_buffer_size < HCD6871C_CMD_LEN) {
        Pthread_mutex_unlock(&self->mtx);
        return AAOS_ENOMEM;
    }
    uint8_t *ctx = (uint8_t *) write_buffer;
    uint16_t crc16 = crc16_modbus(ctx, 6);
#ifndef BIGENDIAN
    crc16 = swap_uint16(crc16);
#endif
    memcpy(&crc16, ctx + 6, 2);
    
    if ((ret = __Serial_write(self, write_buffer, 8, write_size)) != AAOS_OK) {
        Pthread_mutex_unlock(&self->mtx);
        return ret;
    }
    if ((ret = __Serial_read(self, read_buffer, read_buffer_size, read_size)) != AAOS_OK) {
        Pthread_mutex_unlock(&self->mtx);
        return ret;
    }
    
    ctx = (uint8_t *) read_buffer;
    uint16_t ctx_len;
    if (read_size) {
        ctx_len = *read_size - 2;
    } else {
        ctx_len = ctx[2] + 3;
    }
    crc16 = crc16_modbus(ctx, ctx_len);
    
#ifdef BIGENDIAN
    crc16 = swap_uint16(crc16);
#endif
    
    if (memcmp(&crc16, ctx + ctx_len, 2) != 0) {
        Pthread_mutex_unlock(&self->mtx);
        return AAOS_EBADMSG;
    }
    
    Pthread_mutex_unlock(&self->mtx);
    return AAOS_OK;
}

static int
HCD6817CSerial_feed_dog(void *_self)
{
    return AAOS_OK;
}

static int
HCD6817CSerial_validate(const void *_self, const void *command, size_t size)
{
    return AAOS_OK;
}

static int
HCD6817CSerial_inspect(void *_self)
{
    return AAOS_OK;
}

static const void *_hcd6817c_serial_virtual_table;

static void
hcd6817c_serial_virtual_table_destroy(void)
{
    delete((void *) _hcd6817c_serial_virtual_table);
}

static void
hcd6817c_serial_virtual_table_initialize(void)
{
    _hcd6817c_serial_virtual_table = new(__SerialVirtualTable(),
                                         __serial_init, "init", HCD6817CSerial_init,
                                         __serial_feed_dog, "feed_dog", HCD6817CSerial_feed_dog,
                                         __serial_validate, "validate", HCD6817CSerial_validate,
                                         __serial_raw, "raw", HCD6817CSerial_raw,
                                         __serial_inspect, "inspect", HCD6817CSerial_inspect,
                                         (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(hcd6817c_serial_virtual_table_destroy);
#endif
}

static const void *
hcd6817c_serial_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, hcd6817c_serial_virtual_table_initialize);
#endif
    
    return _hcd6817c_serial_virtual_table;
}

/*
 * Beidou
 */

static const char *pattern_rdss = "^AT\\+(((ENAT|ENTP|REBOOT|FWVER\\?|HWVER\\?|IPR\\?|PARITY\\?|SRCAD\\?|DSTAD\\?|DSTAD=[0-9]{7}|WMODE\\?|WMODE=[0-1]|LOCMINS\\?|LOCMINS=[0-9]{,5}|CSQ\\?|LOCINF\\?|SLEEP|READ|READ2|READ3|DELETE)\r))|(SEND=[0-9]{6,7},\".*\"\r\n)$";
static regex_t preg_rdss;

static const void *rdss_serial_virtual_table(void);

static void *
RDSSSerial_ctor(void *_self, va_list *app)
{
    struct RDSSSerial *self = super_ctor(RDSSSerial(), _self, app);
    
    self->_._vtab= rdss_serial_virtual_table();
    self->queue = new(ThreadsafeQueue(), free);
    self->queue2 = new(ThreadsafeQueue(), free);
    
    return (void *) self;
}

static void *
RDSSSerial_dtor(void *_self)
{
    struct RDSSSerial *self = cast(RDSSSerial(), _self);
    
    delete(self->queue2);
    delete(self->queue);
    
    return super_dtor(RDSSSerial(), _self);
}

static void *
RDSSSerialClass_ctor(void *_self, va_list *app)
{
    struct RDSSSerialClass *self = super_ctor(RDSSSerialClass(), _self, app);
    
    self->_.raw.method = (Method) 0;
    self->_.init.method = (Method) 0;
    self->_.validate.method = (Method) 0;
    
    return self;
}

static const void *_RDSSSerialClass;

static void
RDSSSerialClass_destroy(void)
{
    free((void *)_RDSSSerialClass);
}

static void
RDSSSerialClass_initialize(void)
{
    _RDSSSerialClass = new(__SerialClass(), "RDSSSerialClass", __SerialClass(), sizeof(struct RDSSSerialClass),
                         ctor, "", RDSSSerialClass_ctor,
                         (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(RDSSSerialClass_destroy);
#endif
}

const void *
RDSSSerialClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, RDSSSerialClass_initialize);
#endif
    
    return _RDSSSerialClass;
}

static const void *_RDSSSerial;

static void
RDSSSerial_destroy(void)
{
    regfree(&preg_rdss);
    free((void *)_RDSSSerial);
}

static void
RDSSSerial_initialize(void)
{
    regcomp(&preg_rdss, pattern_rdss, REG_EXTENDED | REG_NOSUB);
    _RDSSSerial = new(RDSSSerialClass(), "RDSSSerial", __Serial(), sizeof(struct RDSSSerial),
                     ctor, "ctor", RDSSSerial_ctor,
                     dtor, "dtor", RDSSSerial_dtor,
                     (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(RDSSSerial_destroy);
#endif
}

const void *
RDSSSerial(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, RDSSSerial_initialize);
#endif

    return _RDSSSerial;
}

static int
RDSSSerial_init(void *_self)
{
    struct __Serial *self = cast(__Serial(), _self);
    struct termios termptr;
    int ibaud = B9600, obaud = B9600;
    
    self->fd = Open(self->path, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (self->fd < 0) {
        if (errno == ENOENT && (self->option&SERIAL_OPTION_WAIT_FOR_READY)) {
            pthread_t tid;
            self->state = SERIAL_STATE_WAIT_FOR_READY;
            Pthread_create(&tid, NULL, __Serial_wait_for_tty_ready_thread, self);
        } else {
            self->state = SERIAL_STATE_ERROR;
        }
        return AAOS_EBADF;
    } else {
        self->state = SERIAL_STATE_OK;
    }

    /*
     * buadrate 9600, no parity, 8 bit, non-canonical mode
     */
    Tcgetattr(self->fd, &termptr);
    Cfsetispeed(&termptr, ibaud);
    Cfsetospeed(&termptr, obaud);

    termptr.c_cflag |= (CREAD | CLOCAL | CS8);
    termptr.c_cflag &= ~(PARENB | CSTOPB | CSIZE);
    termptr.c_iflag |= (IGNPAR | IGNBRK | ICRNL);
    termptr.c_iflag &= ~INLCR;
    termptr.c_oflag &= ~(OPOST | ONLCR | OCRNL);
    
    Tcsetattr(self->fd, TCSANOW, &termptr);
    
    return AAOS_OK;
}

static int
RDSSSerial_feed_dog(void *_self)
{
    struct RDSSSerial *self = cast(RDSSSerial(), _self);
    struct __Serial *parent = cast(__Serial(), _self);
    
    char *buf = (char *) Malloc(128);
    char tmp[8];
    int ret;

    memset(buf, '\0', 128);
    Pthread_mutex_lock(&parent->mtx);
    ret = __Serial_read(parent, buf, 128, NULL);
    if (ret == AAOS_OK) {
        __Serial_read(parent, tmp, 8, NULL);
        threadsafe_queue_push(self->queue, buf);
    } else {
        free(buf);
    }
    Pthread_mutex_unlock(&parent->mtx);
    return AAOS_OK;
}

static int
RDSSSerial_validate(const void *_self, const void *command, size_t size)
{
    if (regexec(&preg_rdss, command, 0, NULL, 0) != 0) {
        return AAOS_EBADCMD;
    } else {
        return AAOS_OK;
    }
}

static int
RDSSSerial_raw(void *_self, const void *write_buffer, size_t write_buffer_size, size_t *write_size, void *read_buffer, size_t read_buffer_size, size_t *read_size)
{
    struct __Serial *self = cast(__Serial(), _self);
    
    
    int ret = AAOS_OK;
    Method validate;
    char *cmd = (char *) write_buffer;
    bool send_flag = false;
    bool loc_flag = false;
    /*
     * append '\r\n' or '\r'
     */
    if (strncmp(cmd, "AT+SEND", 7) != 0) {
        if (strncmp(cmd, "AT+LOCINF", 9) == 0) {
            loc_flag = true;
        }
        if (cmd[write_buffer_size - 1] != '\r') {
            if ((cmd = (char *) Malloc(write_buffer_size + 2)) == NULL) {
                return AAOS_ENOMEM;
            }
            memset(cmd, '\0', write_buffer_size + 2);
            memcpy(cmd, write_buffer, write_buffer_size);
            cmd[write_buffer_size] = '\r';
        }
    } else {
        send_flag = true;
        if(strncmp(cmd + strlen(cmd) - 2, "\r\n", 2) != 0) {
            if ((cmd = (char *) Malloc(write_buffer_size + 3)) == NULL) {
                return AAOS_ENOMEM;
            }
            memset(cmd, '\0', write_buffer_size + 3);
            memcpy(cmd, write_buffer, write_buffer_size);
            cmd[write_buffer_size] = '\r';
            cmd[write_buffer_size + 1] = '\n';
        }
    }
    
    if (self->_vtab && (validate = virtualTo(self->_vtab, "validate")) != 0) {
        ret = ((int (*)(const void *, const void *, size_t)) validate)(self, cmd, strlen(cmd));
        if (ret != AAOS_OK) {
            if (cmd != write_buffer) {
                free(cmd);
            }
            return ret;
        }
    }
    
    if (strncmp(cmd, "AT+READ\r", 8) == 0) {
        struct RDSSSerial *parent = cast(RDSSSerial(), _self);
        char *data = threadsafe_queue_try_pop(parent->queue);
        if (data != NULL) {
            snprintf(read_buffer, read_buffer_size, "%s", data);
            if (read_size) {
                *read_size = strlen(data) + 1;
            }
            threadsafe_queue_push(parent->queue2, data);
            return AAOS_OK;
        } else {
            return AAOS_EEMPTY;
        }
    }
    
    if (strncmp(cmd, "AT+READ2\r", 9) == 0) {
        struct RDSSSerial *parent = cast(RDSSSerial(), _self);
        char *data = threadsafe_queue_wait_and_pop(parent->queue);
        if (data != NULL) {
            snprintf(read_buffer, read_buffer_size, "%s", data);
            if (read_size) {
                *read_size = strlen(data) + 1;
            }
            return AAOS_OK;
        } else {
            return AAOS_EEMPTY;
        }
    }
    
    if (strncmp(cmd, "AT+READ3\r", 8) == 0) {
        struct RDSSSerial *parent = cast(RDSSSerial(), _self);
        char *data = threadsafe_queue_try_pop(parent->queue2);
        if (data != NULL) {
            snprintf(read_buffer, read_buffer_size, "%s", data);
            if (read_size) {
                *read_size = strlen(data) + 1;
            }
            free(data);
            return AAOS_OK;
        } else {
            return AAOS_EEMPTY;
        }
    }
    
    if (strncmp(cmd, "AT+DELETE\r", 9) == 0) {
        struct RDSSSerial *parent = cast(RDSSSerial(), _self);
        void *data;
        while ((data = threadsafe_queue_try_pop(parent->queue2)) != NULL) {
            free(data);
        }
    }
    
    Pthread_mutex_lock(&self->mtx);
    switch (self->state) {
        case SERIAL_STATE_ERROR:
            Pthread_mutex_unlock(&self->mtx);
            if (cmd != write_buffer) {
                free(cmd);
            }
            return AAOS_EDEVMAL;
            break;
        case SERIAL_STATE_UNLOADED:
            Pthread_mutex_unlock(&self->mtx);
            if (cmd != write_buffer) {
                free(cmd);
            }
            return AAOS_EDEVNOTLOADED;
            break;
        default:
            break;
    }
    while (self->state == SERIAL_STATE_WAIT_FOR_READY) {
        Pthread_cond_wait(&self->cond, &self->mtx);
    }
    
    //Tcflush(self->fd, TCIOFLUSH);
    if ((ret = __Serial_write(self, cmd, strlen(cmd), write_size)) != AAOS_OK) {
        Pthread_mutex_unlock(&self->mtx);
        if (cmd != write_buffer) {
            free(cmd);
        }
        return ret;
    }
    if (cmd != write_buffer) {
        free(cmd);
    }
    
    if ((ret = __Serial_read(self, read_buffer, read_buffer_size, read_size)) != AAOS_OK) {
        Pthread_mutex_unlock(&self->mtx);
        return ret;
    }
    
    while (strncmp(read_buffer, "RDSS,", 5) == 0) {
        char tmp[8];
        if ((ret = __Serial_read(self, tmp, 8, NULL)) != AAOS_OK) {
            Pthread_mutex_unlock(&self->mtx);
            return ret;
        }
        struct RDSSSerial *myself = cast(RDSSSerial(), _self);
        void *buf = Malloc(strlen(read_buffer) + 1);
        memcpy(buf, read_buffer, strlen(read_buffer) + 1);
        threadsafe_queue_push(myself->queue, buf);
        memset(read_buffer, '\0', read_buffer_size);
        if ((ret = __Serial_read(self, read_buffer, read_buffer_size, read_size)) != AAOS_OK) {
            Pthread_mutex_unlock(&self->mtx);
            return ret;
        }
    }
    
    if (!send_flag && !loc_flag) {
        char tmp[8];
        memset(tmp, '\0', 8);
        /*
         * Read '\n'
         */
        if ((ret = __Serial_read(self, tmp, 8, NULL)) != AAOS_OK) {
            Pthread_mutex_unlock(&self->mtx);
            return ret;
        }
        /*
         * Read 'OK\r' or 'OK\n'
         */
        if ((ret = __Serial_read(self, tmp, 8, NULL)) != AAOS_OK) {
            Pthread_mutex_unlock(&self->mtx);
            return ret;
        }
        if (strncmp(tmp, "OK", 2) != 0) {
            Pthread_mutex_unlock(&self->mtx);
            return AAOS_ERROR;
        }
        /*
         * Read last '\n'
         */
        if ((ret = __Serial_read(self, tmp, 8, NULL)) != AAOS_OK) {
            Pthread_mutex_unlock(&self->mtx);
            return ret;
        }
    }
    
    if (loc_flag) {
        char tmp[8];
        if ((ret = __Serial_read(self, tmp, 8, NULL)) != AAOS_OK) {
            Pthread_mutex_unlock(&self->mtx);
            return ret;
        }
    }
    
    Pthread_mutex_unlock(&self->mtx);

    char *s = (char *) read_buffer;
    if (read_size != NULL) {
        if (s[*read_size - 1] == '\r' || s[*read_size - 1] == '\n')  {
            s[*read_size - 1] = '\0';
        } else {
            if (*read_size < read_buffer_size) {
                s[*read_size] = '\0';
                ++(*read_size);
            }
        }
    } else {
        if (s[strlen(s) - 1] == '\r' || s[strlen(s) - 1] == '\n') {
            s[strlen(s) - 1] = '\0';
        }
    }
    
    return AAOS_OK;
}

static int
RDSSSerial_inspect(void *_self)
{
    struct __Serial *self = cast(__Serial(), _self);
    
    char buf[BUFSIZE];
    if (self->inspect == NULL) {
        return RDSSSerial_raw(_self, "AT+SRCAD?\r", 5, NULL, buf, BUFSIZE, NULL);
    } else {
        return RDSSSerial_raw(_self, self->inspect, 5, NULL, buf, BUFSIZE, NULL);
    }
    
}

static const void *_rdss_serial_virtual_table;

static void
rdss_serial_virtual_table_destroy(void)
{
    delete((void *) _rdss_serial_virtual_table);
}


static void
rdss_serial_virtual_table_initialize(void)
{
    _rdss_serial_virtual_table = new(__SerialVirtualTable(),
                                     __serial_init, "init", RDSSSerial_init,
                                     __serial_feed_dog, "feed_dog", RDSSSerial_feed_dog,
                                     __serial_validate, "validate", RDSSSerial_validate,
                                     __serial_inspect, "inspect", RDSSSerial_inspect,
                                     __serial_raw, "raw", RDSSSerial_raw,
                                    (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(rdss_serial_virtual_table_destroy);
#endif
}

static const void *
rdss_serial_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, rdss_serial_virtual_table_initialize);
#endif
    
    return _rdss_serial_virtual_table;
}

/*
 * Serial short message device
 */
#define SMSBUFFERSIZE 256
#define SMS_MAX_RETRY 5

static const char *pattern_sms = "(^AT$)|(^AT+)";
static regex_t preg_sms;

static const void *sms_serial_virtual_table(void);

static int
SMSSerial_raw_nl(void *_self, const void *write_buffer, size_t write_buffer_size, size_t *write_size, void *read_buffer, size_t read_buffer_size, size_t *read_size)
{
    struct __Serial *self = cast(__Serial(), _self);
    
    int ret = AAOS_OK;
    Method validate;
    
    if (self->_vtab && (validate = virtualTo(self->_vtab, "validate")) != 0) {
        ret = ((int (*)(const void *, const void *, size_t)) validate)(self, write_buffer, write_buffer_size);
        if (ret != AAOS_OK) {
            return ret;
        }
    }
    
    if ((ret = __Serial_write(self, write_buffer, write_buffer_size, write_size)) != AAOS_OK) {
        return ret;
    }
    
    memset(read_buffer, '\0', read_buffer_size);
    size_t nread = 0, nleft = read_buffer_size, size;
    char *buf = read_buffer;
    
    while ((ret = __serial_read(_self, buf, nleft, &size)) == AAOS_OK) {
        buf += size;
        nread += size;
        nleft -= size;
        if (nleft == 0) {
            if (read_size) {
                *read_size = nread;
            }
            return AAOS_ENOMEM;
        }
    }

    if (ret == AAOS_ETIMEDOUT) {
        if (strstr(read_buffer, ">") != NULL || strstr(read_buffer, "OK") != NULL) {
            ret = AAOS_OK;
        } else if (strstr(read_buffer, "EORROR") != NULL) {
            ret = AAOS_EDEVMAL;
        } else {
            ret = AAOS_ERROR;
        }
    } else {
        ret = AAOS_ERROR;
    }
    if (read_size) {
        *read_size = nread;
    }
    
    return ret;
}


int
sms_serial_send(void *_self, const char *phone_number, const char *message)
{
    const struct SMSSerialClass *class = (const struct SMSSerialClass *) classOf(_self);
    
    if (isOf(class, SMSSerialClass()) && class->send.method) {
        return ((int (*)(void *, const char *, const char *)) class->send.method)(_self, phone_number, message);
    } else {
        int result;
        forward(_self, &result, (Method) sms_serial_send, "send", _self, phone_number, message);
        return result;
    }
}

static int
SMSSerial_send(void *_self, const char *phone_number, const char *message)
{
    struct __Serial *self = cast(__Serial(), _self);
    
    int ret;
    char buf[SMSBUFFERSIZE];
    size_t read_size;
    
    
    Pthread_mutex_lock(&self->mtx);
    snprintf(buf, SMSBUFFERSIZE, "AT+CMGS=%s\r", phone_number);
    if ((ret = SMSSerial_raw_nl(_self, buf, strlen(buf), NULL, buf, SMSBUFFERSIZE, &read_size)) != AAOS_OK) {
        Pthread_mutex_unlock(&self->mtx);
        return ret;
    }
    snprintf(buf, SMSBUFFERSIZE, "%s\x1a", message);
    ret = SMSSerial_raw_nl(_self, buf, strlen(buf), NULL, buf, SMSBUFFERSIZE, &read_size);
    Pthread_mutex_unlock(&self->mtx);
    
    return ret;
}

int
sms_serial_recv(void *_self, unsigned int number, void *buf, size_t size, size_t *read_size)
{
    const struct SMSSerialClass *class = (const struct SMSSerialClass *) classOf(_self);
    
    if (isOf(class, SMSSerialClass()) && class->recv.method) {
        return ((int (*)(void *, unsigned int, void *, size_t, size_t *)) class->recv.method)(_self, number, buf, size, read_size);
    } else {
        int result;
        forward(_self, &result, (Method) sms_serial_recv, "recv", _self, number, buf, size);
        return result;
    }
}

static int
SMSSerial_recv(void *_self, unsigned int number, char *read_buf, size_t size, size_t *read_size)
{
    struct __Serial *self = cast(__Serial(), _self);
    
    int ret;
    char buf[SMSBUFFERSIZE];
    size_t buf_size;
    
    *read_size = 0;
    Pthread_mutex_lock(&self->mtx);
    snprintf(buf, SMSBUFFERSIZE, "AT+CMGS=%u\r", number);
    if ((ret = __serial_write(self, buf, strlen(buf), NULL)) != AAOS_OK) {
        Pthread_mutex_unlock(&self->mtx);
        return ret;
    }
    memset(buf, '\0', SMSBUFFERSIZE);
    if ((ret = __serial_read(self, buf, SMSBUFFERSIZE, &buf_size)) != AAOS_OK) {
        Pthread_mutex_unlock(&self->mtx);
        return ret;
    }
    buf[buf_size] = '\0';
    if (strncmp(buf, "OK", 1) != 0) {
        Pthread_mutex_unlock(&self->mtx);
        if (strncmp(buf, "ERROR", 5) == 0) {
            return AAOS_EDEVMAL;
        } else {
            return AAOS_ERROR;
        }
    }
    Pthread_mutex_unlock(&self->mtx);
    snprintf(read_buf, size, "%s", buf);
    *read_size = strlen(buf);
    return AAOS_OK;
}

int
sms_serial_del(void *_self, unsigned int number)
{
    const struct SMSSerialClass *class = (const struct SMSSerialClass *) classOf(_self);
    
    if (isOf(class, SMSSerialClass()) && class->del.method) {
        return ((int (*)(void *, unsigned int)) class->recv.method)(_self, number);
    } else {
        int result;
        forward(_self, &result, (Method) sms_serial_del, "del", _self, number);
        return result;
    }
}

static int
SMSSerial_del(void *_self, unsigned int number)
{
    struct __Serial *self = cast(__Serial(), _self);
    
    int ret;
    char buf[SMSBUFFERSIZE];
    size_t read_size;
    
    Pthread_mutex_lock(&self->mtx);
    snprintf(buf, SMSBUFFERSIZE, "AT+CMGD=%u\r", number);
    if ((ret = __serial_write(self, buf, strlen(buf), NULL)) != AAOS_OK) {
        Pthread_mutex_unlock(&self->mtx);
        return ret;
    }
    memset(buf, '\0', SMSBUFFERSIZE);
    if ((ret = __serial_read(self, buf, SMSBUFFERSIZE, &read_size)) != AAOS_OK) {
        Pthread_mutex_unlock(&self->mtx);
        return ret;
    }
    buf[read_size] = '\0';
    if (strncmp(buf, "OK", 1) != 0) {
        Pthread_mutex_unlock(&self->mtx);
        if (strncmp(buf, "ERROR", 5) == 0) {
            return AAOS_EDEVMAL;
        } else {
            return AAOS_ERROR;
        }
    }
    Pthread_mutex_unlock(&self->mtx);
    return AAOS_OK;
}

static void *
SMSSerial_ctor(void *_self, va_list *app)
{
    struct SMSSerial *self = super_ctor(SMSSerial(), _self, app);
    
    self->_._vtab= sms_serial_virtual_table();
    
    return (void *) self;
}

static void *
SMSSerial_dtor(void *_self)
{
    return super_dtor(SMSSerial(), _self);
}

static void *
SMSSerialClass_ctor(void *_self, va_list *app)
{
    struct SMSSerialClass *self = super_ctor(SMSSerialClass(), _self, app);
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

        if (selector == (Method) sms_serial_send) {
            if (tag) {
                self->send.tag = tag;
                self->send.selector = selector;
            }
            self->send.method = method;
            continue;
        }
        if (selector == (Method) sms_serial_recv) {
            if (tag) {
                self->recv.tag = tag;
                self->recv.selector = selector;
            }
            self->recv.method = method;
            continue;
        }
        if (selector == (Method) sms_serial_del) {
            if (tag) {
                self->del.tag = tag;
                self->del.selector = selector;
            }
            self->del.method = method;
            continue;
        }
    }
    
#ifdef va_copy
    va_end(ap);
#endif
    
    self->_.init.method = (Method) 0;
    self->_.validate.method = (Method) 0;
    
    return (void *) self;
}

static const void *_SMSSerialClass;

static void
SMSSerialClass_destroy(void)
{
    free((void *)_SMSSerialClass);
}

static void
SMSSerialClass_initialize(void)
{
    _SMSSerialClass = new(__SerialClass(), "SMSSerialClass", __SerialClass(), sizeof(struct SMSSerialClass),
                          ctor, "", SMSSerialClass_ctor,
                          (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(SMSSerialClass_destroy);
#endif
}

const void *
SMSSerialClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, SMSSerialClass_initialize);
#endif
    
    return _SMSSerialClass;
}

static const void *_SMSSerial;

static void
SMSSerial_destroy(void)
{
    regfree(&preg_sms);
    free((void *)_SMSSerial);
}

static void
SMSSerial_initialize(void)
{
    regcomp(&preg_sms, pattern_sms, REG_EXTENDED | REG_NOSUB);
    _SMSSerial = new(SMSSerialClass(), "SMSSerial", __Serial(), sizeof(struct SMSSerial),
                     ctor, "ctor", SMSSerial_ctor,
                     dtor, "dtor", SMSSerial_dtor,
                     sms_serial_send, "send", SMSSerial_send,
                     sms_serial_recv, "recv", SMSSerial_recv,
                     sms_serial_del, "del", SMSSerial_del,
                     (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(SMSSerial_destroy);
#endif
}

const void *
SMSSerial(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, SMSSerial_initialize);
#endif
    
    return _SMSSerial;
}

static int
SMSSerial_init(void *_self)
{
    struct __Serial *self = cast(__Serial(), _self);
    struct termios termptr;
    int ibaud = B9600, obaud = B9600;
    
    if (self->fd < 0) {
        return AAOS_EBADF;
    }
    
    Tcgetattr(self->fd, &termptr);
    
    termptr.c_cflag |= (CLOCAL|CREAD|CS8);
    termptr.c_cflag &= ~(PARENB|CSTOPB|CSIZE);
    termptr.c_lflag &= ~(ECHO|ECHOE|ISIG);
    termptr.c_oflag &= ~OPOST;
    
    Cfsetispeed(&termptr, ibaud);
    Cfsetospeed(&termptr, obaud);
    Tcflush(self->fd,TCIFLUSH);
    Tcsetattr(self->fd, TCSANOW, &termptr);
    
    char buf[SMSBUFFERSIZE];
    size_t read_size;
    int ret;

    /*
     * Test SMS device is available.
     */
    if ((ret = __serial_raw(self, "AT\r", 3, NULL, buf, SMSBUFFERSIZE - 1, &read_size)) != AAOS_OK) {
        return ret;
    }
    
    /*
     * Disable echo the command back.
     */
    if ((ret = __serial_raw(self, "ATE0\r", 5, NULL, buf, SMSBUFFERSIZE - 1, &read_size)) != AAOS_OK) {
        return ret;
    }
    /*
     * Setting recieving SMS behavior.
     */
    if ((ret = __serial_raw(self, "AT+CNMI=2,1\r", 12, NULL, buf, SMSBUFFERSIZE - 1, &read_size)) != AAOS_OK) {
        return ret;
    }
    
    return AAOS_OK;
}

static int
SMSSerial_feed_dog(void *_self)
{
    return AAOS_OK;
}

static int
SMSSerial_validate(const void *_self, const void *command, size_t size)
{
    return AAOS_OK;
}

static int
SMSSerial_raw(void *_self, const void *write_buffer, size_t write_buffer_size, size_t *write_size, void *read_buffer, size_t read_buffer_size, size_t *read_size)
{
    struct __Serial *self = cast(__Serial(), _self);
    
    int ret = AAOS_OK;
    Method validate;
    
    if (self->_vtab && (validate = virtualTo(self->_vtab, "validate")) != 0) {
        ret = ((int (*)(const void *, const void *, size_t)) validate)(self, write_buffer, write_buffer_size);
        if (ret != AAOS_OK) {
            return ret;
        }
    }
    
    Pthread_mutex_lock(&self->mtx);
    if ((ret = __Serial_write(self, write_buffer, write_buffer_size, write_size)) != AAOS_OK) {
        Pthread_mutex_unlock(&self->mtx);
        return ret;
    }
    
    memset(read_buffer, '\0', read_buffer_size);
    size_t nread = 0, nleft = read_buffer_size, size;
    char *buf = read_buffer;
    
    while ((ret = __serial_read(_self, buf, nleft, &size)) == AAOS_OK) {
        buf += size;
        nread += size;
        nleft -= size;
        if (nleft == 0) {
            Pthread_mutex_unlock(&self->mtx);
            if (read_size) {
                *read_size = nread;
            }
            return AAOS_ENOMEM;
        }
    }
    Pthread_mutex_unlock(&self->mtx);
    if (ret == AAOS_ETIMEDOUT) {
        if (strstr(read_buffer, ">") != NULL || strstr(read_buffer, "OK") != NULL) {
            ret = AAOS_OK;
        } else if (strstr(read_buffer, "EORROR") != NULL) {
            ret = AAOS_EDEVMAL;
        } else {
            ret = AAOS_ERROR;
        }
    } else {
        ret = AAOS_ERROR;
    }
    if (read_size) {
        *read_size = nread;
    }
    
    return ret;
}

static const void *_sms_serial_virtual_table;

static void
sms_serial_virtual_table_destroy(void)
{
    delete((void *) _sms_serial_virtual_table);
}


static void
sms_serial_virtual_table_initialize(void)
{
    _sms_serial_virtual_table = new(__SerialVirtualTable(),
                                    __serial_init, "init", SMSSerial_init,
                                    __serial_feed_dog, "feed_dog", SMSSerial_feed_dog,
                                    __serial_validate, "validate", SMSSerial_validate,
                                    __serial_raw, "raw", SMSSerial_raw,
                                    (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(sms_serial_virtual_table_destroy);
#endif
}

static const void *
sms_serial_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, sms_serial_virtual_table_initialize);
#endif
    
    return _sms_serial_virtual_table;
}

/*
 * MODBUS_CRC16_v3 copy from libcrc.
 */

static uint16_t
MODBUS_CRC16_v3(const unsigned char *buf, unsigned int len)
{
    static const uint16_t table[256] = {
        0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
        0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
        0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
        0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
        0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
        0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
        0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
        0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
        0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
        0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
        0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
        0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
        0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
        0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
        0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
        0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
        0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
        0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
        0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
        0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
        0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
        0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
        0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
        0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
        0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
        0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
        0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
        0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
        0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
        0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
        0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
        0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
    };

    uint8_t xor = 0;
    uint16_t crc = 0xFFFF;

    while (len--) {
        xor = (*buf++) ^ crc;
        crc >>= 8;
        crc ^= table[xor];
    }

    return crc;
}

static void
KLTPSerial_fill_output(struct KLTPSerial *self, unsigned char *buf)
{
    Pthread_mutex_lock(&self->mtx);
    self->flag = 1;
    memcpy(self->read_buf, buf, self->output_len);
    Pthread_mutex_unlock(&self->mtx);
    Pthread_cond_signal(&self->cond);
}

static void *
KLTPSerial_read_thr(void *arg)
{
    struct KLTPSerial *self = (struct KLTPSerial *) arg;

    int fd = self->_.fd;
    unsigned char buf[32];
    int ret;
    size_t read_size, output_len = self->output_len;
    uint16_t crc, crc2;
    unsigned int len;

    for (;;) {
        ret = __serial_read(self, buf, 32, &read_size);
        if (ret == 0 && read_size == output_len) {
            len = (unsigned int)(output_len - sizeof(uint16_t));
            memcpy(&crc, buf + output_len - 2, sizeof(uint16_t));
            crc2 = MODBUS_CRC16_v3(buf, len);
#ifdef BIGENDIAN
            crc2 = swap_uint16(crc2);
#endif
            if (crc == crc2) {
                switch (buf[1]) {
                    case 0x03:
                    case 0x83:
                        KLTPSerial_fill_output(self, buf);
                        break;
                    case 0x05:
                    case 0x85:
                        KLTPSerial_fill_output(self, buf);
                        break;
                    case 0x06:
                    case 0x86:
                        KLTPSerial_fill_output(self, buf);
                        break;
                    case 0x55: /* */
                        break;
                    default:
                        break;
                }
            }
        }
    }

    return NULL;
}

static int
KLTPSerial_init(void *_self)
{
    struct __Serial *self = cast(__Serial(), _self);
    struct KLTPSerial *myself = cast(KLTPSerial(), _self);
    struct termios termptr;
    int ibaud = B230400, obaud = B230400;

    self->fd = Open(self->path, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (self->fd < 0) {
        if (errno == ENOENT && (self->option&SERIAL_OPTION_WAIT_FOR_READY)) {
            pthread_t tid;
            self->state = SERIAL_STATE_WAIT_FOR_READY;
            Pthread_create(&tid, NULL, __Serial_wait_for_tty_ready_thread, self);
        } else {
            self->state = SERIAL_STATE_ERROR;
        }
        return AAOS_EBADF;
    } else {
        self->state = SERIAL_STATE_OK;
    }

    Tcgetattr(self->fd, &termptr);
    Cfsetispeed(&termptr, ibaud);
    Cfsetospeed(&termptr, obaud);
    termptr.c_lflag &= ~(ISIG | ICANON | IEXTEN | ISIG | ECHO | ECHOE | ECHOK | ECHOCTL | ECHOKE);
    termptr.c_iflag &= ~(IXON | BRKINT | ICRNL | INPCK | ISTRIP);
    termptr.c_cflag &= ~(CSIZE | PARENB);
    termptr.c_cflag |= CS8;
    termptr.c_oflag |= OPOST;
    termptr.c_cc[VMIN] = 1;
    termptr.c_cc[VTIME] = 0;

    Tcsetattr(self->fd, TCSANOW, &termptr);

    Pthread_create(&myself->tid, NULL, KLTPSerial_read_thr, myself);

    return AAOS_OK;
}

static int
KLTPSerial_raw(void *_self, void *write_buffer, size_t write_buffer_size, size_t *write_size, void *read_buffer, size_t read_buffer_size, size_t *read_size)
{
    struct __Serial *self = cast(__Serial(), _self);
    struct KLTPSerial *myself = cast(KLTPSerial(), _self);

    int ret = AAOS_OK;

    Pthread_mutex_lock(&self->mtx);
    switch (self->state) {
        case SERIAL_STATE_ERROR:
            Pthread_mutex_unlock(&self->mtx);
            return AAOS_EDEVMAL;
            break;
        case SERIAL_STATE_UNLOADED:
            Pthread_mutex_unlock(&self->mtx);
            return AAOS_EDEVNOTLOADED;
            break;
        default:
            break;
    }
    while (self->state == SERIAL_STATE_WAIT_FOR_READY) {
        Pthread_cond_wait(&self->cond, &self->mtx);
     }

    //Tcflush(self->fd, TCIOFLUSH);
    unsigned char *buf;
    buf = (unsigned char *) Malloc(write_buffer_size + sizeof(uint16_t));
    memcpy(buf, write_buffer, write_buffer_size);
    uint16_t crc;
    crc = MODBUS_CRC16_v3(buf, (unsigned int) write_buffer_size);
#ifdef BIGENDIAN
    crc = swap_uint16(crc);
#endif
    memcpy(buf + write_buffer_size, &crc, sizeof(uint16_t));
    if ((ret = __Serial_write(self, write_buffer, write_buffer_size + sizeof(uint16_t), write_size)) != AAOS_OK) {
        free(buf);
        Pthread_mutex_unlock(&self->mtx);
        return ret;
    }
    free(buf);
    if (write_size != NULL) {
        write_size -= sizeof(uint16_t);
    }
    Pthread_mutex_unlock(&self->mtx);

    Pthread_mutex_lock(&myself->mtx);
    while (myself->flag == 0) {
        Pthread_cond_wait(&self->cond, &self->mtx);
    }
    memcpy(read_buffer, myself->read_buf, min(read_buffer_size, myself->output_len));
    if (read_size != NULL) {
        *read_size = min(read_buffer_size, myself->output_len);
    }
    myself->flag = 0;
    Pthread_mutex_unlock(&myself->mtx);

    return AAOS_OK;
}

static int
KLTPSerial_feed_dog(void *_self)
{
    return AAOS_OK;
}

static int
KLTPSerial_validate(const void *_self, const void *command, size_t size)
{
    return AAOS_OK;
}

static int
KLTPSerial_inspect(void *_self)
{
    char buf[BUFSIZE];
    unsigned char command[32];
    command[0] = 0x55;
    command[1] = 0x03;
    command[2] = 0x00;
    command[3] = 0x04;
    command[5] = 0x00;
    command[6] = 0x04;
    return KLTPSerial_raw(_self, command, 6, NULL, buf, BUFSIZE, NULL);
}

static const void *_kltp_serial_virtual_table;

static void
kltp_serial_virtual_table_destroy(void)
{
    delete((void *) _kltp_serial_virtual_table);
}

static void
kltp_serial_virtual_table_initialize(void)
{
    _kltp_serial_virtual_table = new(__SerialVirtualTable(),
                                     __serial_init, "init", KLTPSerial_init,
                                     __serial_feed_dog, "feed_dog", KLTPSerial_feed_dog,
                                     __serial_validate, "validate", KLTPSerial_validate,
                                     __serial_raw, "raw", KLTPSerial_raw,
                                     __serial_inspect, "inspect", KLTPSerial_inspect,
                                     (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(kltp_serial_virtual_table_destroy);
#endif
}

static const void *
kltp_serial_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, kltp_serial_virtual_table_initialize);
#endif
    return _kltp_serial_virtual_table;
}

/*
 * Compiler-dependant initializer.
 */
#ifdef _USE_COMPILER_ATTRIBUTION_

static void __destructor__(void) __attribute__ ((destructor(_SERIAL_PRIORITY_)));

static void
__destructor__(void)
{
    KLTPSerial_destroy();
    KLTPSerialClass_destroy();
    kltp_serial_virtual_table_destroy();
    SMSSerial_destroy();
    SMSSerialClass_destroy();
    sms_serial_virtual_table_destroy();
    RDSSSerial_destroy();
    RDSSSerialClass_destroy();
    rdss_serial_virtual_table_destroy();
    APMountSerial_destroy();
    APMountSerialClass_destroy();
    apmount_serial_virtual_table_destroy();
    JZDSerial_destroy();
    JZDSerialClass_destroy();
    jzd_serial_virtual_table_destroy();
    AAGPDUSerial_destroy();
    AAGPDUSerialClass_destroy();
    aag_pdu_serial_virtual_table_destroy();
    __Serial_destroy();
    __SerialClass_destroy();
    __SerialVirtualTable_destroy();
}

static void __constructor__(void) __attribute__ ((constructor(_SERIAL_PRIORITY_)));

static void
__constructor__(void)
{
    __SerialVirtualTable_initialize();
    __SerialClass_initialize();
    __Serial_initialize();
    jzd_serial_virtual_table_initialize();
    JZDSerialClass_initialize();
    JZDSerial_initialize();
    sqm_serial_virtual_table_initialize();
    SQMSerialClass_initialize();
    SQMSerial_initialize();
    ws100umb_serial_virtual_table_initialize();
    WS100UMBSerialClass_initialize();
    WS100UMBSerial_initialize();
    aag_pdu_serial_virtual_table_initialize();
    AAGPDUSerialClass_initialize();
    AAGPDUSerial_initialize();
    apmount_serial_virtual_table_initialize();
    APMountSerialClass_initialize();
    APMountSerial_initialize();
    rdss_serial_virtual_table();
    RDSSSerialClass_initialize();
    RDSSSerialClass_initialize();
    sms_serial_virtual_table_initialize();
    SMSSerialClass_initialize();
    SMSSerial_initialize();
    kltp_serial_virtual_table_initialize();
    KLTPSerialClass_initialize();
    KLTPSerial_initialize();
}
#endif
