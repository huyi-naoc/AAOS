#include "serial_r.h"
#include "serial.h"

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
