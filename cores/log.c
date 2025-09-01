//
//  log.c
//  AAOS
//
//  Created by Hu Yi on 2019/5/24.
//  Copyright © 2019 National Astronomical Observatories, Chinese Academy of Sciences. All rights reserved.
//

#include "def.h"
#include "log.h"
#include "log_r.h"
#include "log_def.h"
#include "wrapper.h"

#ifndef NUMBER_OF_LOG_LEVELS
#define NUMBER_OF_LOG_LEVELS 8
#endif

static const char *__LOG_LEVELS[] = {"emerg", "aler", "crit", "err", "warn", "notice", "info", "debug"};

const char *
__log_get_facility(void *_self)
{
    const struct __LogClass *class = (const struct __LogClass *) classOf(_self);
    
    if (isOf(class, __LogClass()) && class->get_facility.method) {
        return ((const char * (*)(void *)) class->get_facility.method)(_self);
        
    } else {
        const char *result;
        forward(_self, &result, (Method) __log_get_facility, "get_facility", _self);
        return result;
    }
}

static const char *
__Log_get_facility(void *_self)
{
    struct __Log *self = cast(__Log(), _self);
    
    return (const char *) self->facility;
}

int
__log_write(void *_self, unsigned int level, const char *message)
{
    const struct __LogClass *class = (const struct __LogClass *) classOf(_self);
    
    if (isOf(class, __LogClass()) && class->write.method) {
        return ((int (*)(void *, unsigned int, const char *)) class->write.method)(_self, level, message);
        
    } else {
        int result;
        forward(_self, &result, (Method) __log_write, "write", _self, level, message);
        return result;
    }
}

static int
__Log_write(void *_self, unsigned int level, const char *message)
{
    struct __Log *self = cast(__Log(), _self);
    struct timespec tp;
    struct tm t;
    char log_buf[LOG_RECORD_MAX_LENGTH + TIMESTAMPSIZE], time_buf[TIMESTAMPSIZE];
    ssize_t n;
    
    clock_gettime(CLOCK_REALTIME, &tp);
    gmtime_r(&tp.tv_sec, &t);
    strftime(time_buf, TIMESTAMPSIZE, "%Y-%m-%dT%H:%M:%S", &t);
    snprintf(log_buf, LOG_RECORD_MAX_LENGTH + TIMESTAMPSIZE, "[%s.%d]: %s\n", time_buf, (int) (tp.tv_nsec/1000), message);
    
    if (level&LOG_LEVEL_DEBUG) {
        n = Write(self->fds[7], log_buf, strlen(log_buf));
    }
    if (level&LOG_LEVEL_INFO) {
        n = Write(self->fds[6], log_buf, strlen(log_buf));
    }
    if (level&LOG_LEVEL_NOTICE) {
        n = Write(self->fds[5], log_buf, strlen(log_buf));
    }
    if (level&LOG_LEVEL_WARNING) {
        n = Write(self->fds[4], log_buf, strlen(log_buf));
    }
    if (level&LOG_LEVEL_ERR) {
        n = Write(self->fds[3], log_buf, strlen(log_buf));
    }
    if (level&LOG_LEVEL_CRIT) {
        n = Write(self->fds[2], log_buf, strlen(log_buf));
    }
    if (level&LOG_LEVEL_ALERT) {
        n = Write(self->fds[1], log_buf, strlen(log_buf));
    }
    if (level&LOG_LEVEL_EMERG) {
        n = Write(self->fds[0], log_buf, strlen(log_buf));
    }
    
    return AAOS_OK;
}

static void *
__Log_ctor(void *_self, va_list *app)
{
    struct __Log *self = super_ctor(__Log(), _self, app);
    
    size_t i;
    char path[PATHSIZE];
    
    const char *s;
    
    s = va_arg(*app, const char *);
    if (s) {
        self->facility = (char *) Malloc(strlen(s) + 1);
        snprintf(self->facility, strlen(s) + 1, "%s", s);
    }
    
    s = va_arg(*app, const char *);
    if (s) {
        self->work_directory = (char *) Malloc(strlen(s) + 1);
        snprintf(self->work_directory, strlen(s) + 1, "%s", s);
    }
    
    for (i = 0; i < NUMBER_OF_LOG_LEVELS; i++) {
        snprintf(path, PATHSIZE, "%s/%s.%s.log", self->work_directory, self->facility, __LOG_LEVELS[i]);
        self->fds[i] = Open(path, O_RDWR | O_CREAT | O_APPEND, FMODE);
    }
    
    return (void *) self;
}

static void *
__Log_dtor(void *_self)
{
    struct __Log *self = cast(__Log(), _self);
    
    size_t i;
    
    free(self->facility);
    free(self->work_directory);
    
    for (i = 0; i < 8; i++) {
        Close(self->fds[i]);
    }
    
    return super_dtor(__Log(), _self);
}

static void *
__LogClass_ctor(void *_self, va_list *app)
{
    struct __LogClass *self = super_ctor(__LogClass(), _self, app);
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
        if (selector == (Method) __log_write) {
            if (tag) {
                self->write.tag = tag;
                self->write.selector = selector;
            }
            self->write.method = method;
            continue;
        }
        if (selector == (Method) __log_get_facility) {
            if (tag) {
                self->get_facility.tag = tag;
                self->get_facility.selector = selector;
            }
            self->get_facility.method = method;
            continue;
        }
    }
    
#ifdef va_copy
    va_end(ap);
#endif
    
    return (void *) self;
}

static const void *___LogClass;

static void
__LogClass_destroy(void)
{
    free((void *) ___LogClass);
}

static void
__LogClass_initialize(void)
{
    ___LogClass = new(Class(), "__LogClass", Class(), sizeof(struct __LogClass),
                      ctor, "ctor", __LogClass_ctor,
                      (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(__LogClass_destroy);
#endif
}

const void *
__LogClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, __LogClass_initialize);
#endif
    
    return ___LogClass;
}

static const void *___Log;

static void
__Log_destroy(void)
{
    free((void *)___Log);
}

static void
__Log_initialize(void)
{
    ___Log = new(__LogClass(), "__Log", Object(), sizeof(struct __Log),
                 ctor, "ctor", __Log_ctor,
                 dtor, "dtor", __Log_dtor,
                 __log_write, "read", __Log_write,
                 __log_get_facility, "get_facility", __Log_get_facility,
                 (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(__Log_destroy);
#endif
}

const void *
__Log(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, __Log_initialize);
#endif
    
    return ___Log;
}
