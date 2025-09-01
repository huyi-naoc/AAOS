//
//  device.c
//  AAOS
//
//  Created by Hu Yi on 2022/7/23.
//  Copyright © 2022 NAOC. All rights reserved.
//

#include "def.h"
#include "device.h"
#include "device_r.h"
#include "wrapper.h"

/*
 * Device virtual table.
 */
static void *
DeviceVirtualTable_ctor(void *_self, va_list *app)
{
    struct DeviceVirtualTable *self = super_ctor(DeviceVirtualTable(), _self, app);
    Method selector;
    
    while ((selector = va_arg(*app, Method))) {
        const char *tag = va_arg(*app, const char *);
        Method method = va_arg(*app, Method);
        
        if (selector == (Method) device_inspect) {
            if (tag) {
                self->inspect.tag = tag;
                self->inspect.selector = selector;
            }
            self->inspect.method = method;
        }
        if (selector == (Method) device_wait) {
            if (tag) {
                self->wait.tag = tag;
                self->wait.selector = selector;
            }
            self->wait.method = method;
        }
    }
    
    return _self;
}

static void *
DeviceVirtualTable_dtor(void *_self)
{
    return super_dtor(Object(), _self);
}

static const void *_DeviceVirtualTable;

static void
DeviceVirtualTable_destroy(void)
{
    free((void *) _DeviceVirtualTable);
}

static void
DeviceVirtualTable_initialize(void)
{
    _DeviceVirtualTable = new(VirtualTableClass(), "DeviceVirtualTable", VirtualTable(), sizeof(struct DeviceVirtualTable),
                               ctor, "ctor", DeviceVirtualTable_ctor,
                               dtor, "dtor", DeviceVirtualTable_dtor,
                               (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(DeviceVirtualTable_destroy);
#endif
}

const void *
DeviceVirtualTable(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, DeviceVirtualTable_initialize);
#endif
    
    return _DeviceVirtualTable;
}

/*
 * Device base class.
 */

/*
 * Pure virtual functions.
 */
int
device_inspect(void *_self)
{
    const struct DeviceClass *class = (const struct DeviceClass *) classOf(_self);
    
    if (isOf(class, DeviceClass()) && class->inspect.method) {
        return ((int (*)(void *)) class->inspect.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) device_inspect, "inspect", _self);
        return result;
    }
}

int
Device_wait(void *_self, double timeout)
{
    struct Device *self = cast(Device(), _self);
    
    int ret = AAOS_OK;
    
    Pthread_mutex_lock(&self->mtx);
    if (self->state != DEVICE_MALFUNCTION) {
        Pthread_mutex_unlock(&self->mtx);
    } else {
        if (timeout < 0.) {
            while (self->state == DEVICE_MALFUNCTION) {
                Pthread_cond_wait(&self->cond, &self->mtx);
            }
        } else if (timeout == 0) {
            ret = self->state;
        } else {
            struct timespec tp;
            tp.tv_sec = floor(timeout);
            tp.tv_nsec = (timeout - tp.tv_sec) * 1000000000.;
            ret = Pthread_cond_timedwait(&self->cond, &self->mtx, &tp);
        }
    }
    Pthread_mutex_unlock(&self->mtx);
    
    return ret;
}

int
device_wait(void *_self, double timeout)
{
    const struct DeviceClass *class = (const struct DeviceClass *) classOf(_self);
    
    if (isOf(class, DeviceClass()) && class->wait.method) {
        return ((int (*)(void *, double)) class->inspect.method)(_self, timeout);
    } else {
        int result;
        forward(_self, &result, (Method) device_wait, "wait", _self, timeout);
        return result;
    }
}

static void
Device_forward(const void *_self, void *result, Method selector, const char *name, va_list *app)
{
    const struct Device *self = cast(Device(), _self);
    Method method = virtualTo(self->_vtab, name);
    va_list ap;
    
#ifdef va_copy
    va_copy(ap, *app);
#else
    ap = *app;
#endif
    
    void *obj = va_arg(*app, void *);
    
    if (selector == (Method) device_inspect) {
        *((int *) result) = ((int (*)(void *)) method)(obj);
    }  else if (selector == (Method) device_wait) {
        double timeout = va_arg(*app, double);
        *((int *) result) = ((int (*)(void *,  double)) method)(obj, timeout);
    } else {
        assert(0);
    }
    
#ifdef va_copy
    va_end(ap);
#endif
}

static void *
Device_ctor(void *_self, va_list *app)
{
    struct Device *self = super_ctor(Device(), _self, app);
    
    Pthread_mutex_init(&self->mtx, NULL);
    Pthread_cond_init(&self->cond, NULL);
    
    return (void *) self;
}

static void *
Device_dtor(void *_self)
{
    struct Device *self = cast(Device(), _self);
    
    
    Pthread_mutex_destroy(&self->mtx);
    Pthread_cond_destroy(&self->cond);
    
    return super_dtor(Device(), _self);
}

static void *
DeviceClass_ctor(void *_self, va_list *app)
{
    struct DeviceClass *self = super_ctor(DeviceClass(), _self, app);
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
        
        if (selector == (Method) device_inspect) {
            if (tag) {
                self->inspect.tag = tag;
                self->inspect.selector = selector;
            }
            self->inspect.method = method;
            continue;
        }
        
        if (selector == (Method) device_wait) {
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

static const void *_DeviceClass;

static void
DeviceClass_destroy(void)
{
    free((void *) _DeviceClass);
}

static void
DeviceClass_initialize(void)
{
    _DeviceClass = new(Class(), "DeviceClass", Class(), sizeof(struct DeviceClass),
                       ctor, "ctor", DeviceClass_ctor,
                       (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(DeviceClass_destroy);
#endif
}

const void *
DeviceClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, DeviceClass_initialize);
#endif
    
    return _DeviceClass;
}

static const void *_Device;

static void
Device_destroy(void)
{
    free((void *)_Device);
}

static void
Device_initialize(void)
{
    _Device = new(DeviceClass(), "Device", Object(), sizeof(struct Device),
                  ctor, "ctor", Device_ctor,
                  dtor, "dtor", Device_dtor,
                  device_wait, "wait", Device_wait,
                  forward, "forward", Device_forward,
                  
                  (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(Device_destroy);
#endif
}

const void *
Device(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, Device_initialize);
#endif
    
    return _Device;
}
