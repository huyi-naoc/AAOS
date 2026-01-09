//
//  dome.c
//  AAOS
//
//  Created by huyi on 2025/4/27.
//

#include "def.h"
#include "dome.h"
#include "dome_r.h"
#include "dome_def.h"
#include "object.h"
#include "virtual.h"
#include "wrapper.h"

#include <cjson/cJSON.h>

/*
 * Dome virtual table.
 */
static void *
__DomeVirtualTable_ctor(void *_self, va_list *app)
{
    struct __DomeVirtualTable *self = super_ctor(__DomeVirtualTable(), _self, app);
    Method selector;
    
    while ((selector = va_arg(*app, Method))) {
        const char *tag = va_arg(*app, const char *);
        Method method = va_arg(*app, Method);
        
        if (selector == (Method) __dome_init) {
            if (tag) {
                self->init.tag = tag;
                self->init.selector = selector;
            }
            self->init.method = method;
            continue;
        }
        if (selector == (Method) __dome_open_window) {
            if (tag) {
                self->open_window.tag = tag;
                self->open_window.selector = selector;
            }
            self->open_window.method = method;
            continue;
        }
        if (selector == (Method) __dome_close_window) {
            if (tag) {
                self->close_window.tag = tag;
                self->close_window.selector = selector;
            }
            self->close_window.method = method;
            continue;
        }
        
        if (selector == (Method) __dome_stop_window) {
            if (tag) {
                self->stop_window.tag = tag;
                self->stop_window.selector = selector;
            }
            self->stop_window.method = method;
            continue;
        }
        if (selector == (Method) __dome_status) {
            if (tag) {
                self->status.tag = tag;
                self->status.selector = selector;
            }
            self->status.method = method;
            continue;
        }
        if (selector == (Method) __dome_inspect) {
            if (tag) {
                self->inspect.tag = tag;
                self->inspect.selector = selector;
            }
            self->inspect.method = method;
            continue;
        }
        if (selector == (Method) __dome_register) {
            if (tag) {
                self->reg.tag = tag;
                self->reg.selector = selector;
            }
            self->reg.method = method;
            continue;
        }
        if (selector == (Method) __dome_raw) {
            if (tag) {
                self->raw.tag = tag;
                self->raw.selector = selector;
            }
            self->raw.method = method;
            continue;
        }
        if (selector == (Method) __dome_get_window_position) {
            if (tag) {
                self->get_window_position.tag = tag;
                self->get_window_position.selector = selector;
            }
            self->get_window_position.method = method;
            continue;
        }
        if (selector == (Method) __dome_get_window_open_speed) {
            if (tag) {
                self->get_window_open_speed.tag = tag;
                self->get_window_open_speed.selector = selector;
            }
            self->get_window_open_speed.method = method;
            continue;
        }
        if (selector == (Method) __dome_set_window_open_speed) {
            if (tag) {
                self->set_window_open_speed.tag = tag;
                self->set_window_open_speed.selector = selector;
            }
            self->set_window_open_speed.method = method;
            continue;
        }
        if (selector == (Method) __dome_get_window_close_speed) {
            if (tag) {
                self->get_window_close_speed.tag = tag;
                self->get_window_close_speed.selector = selector;
            }
            self->get_window_close_speed.method = method;
            continue;
        }
        if (selector == (Method) __dome_set_window_close_speed) {
            if (tag) {
                self->set_window_close_speed.tag = tag;
                self->set_window_close_speed.selector = selector;
            }
            self->set_window_close_speed.method = method;
            continue;
        }
    }
    
    return _self;
}

static void *
__DomeVirtualTable_dtor(void *_self)
{
    return super_dtor(Object(), _self);
}

static const void *___DomeVirtualTable;

static void
__DomeVirtualTable_destroy(void)
{
    free((void *) ___DomeVirtualTable);
}

static void
__DomeVirtualTable_initialize(void)
{
    ___DomeVirtualTable = new(VirtualTableClass(), "__DomeVirtualTable", VirtualTable(), sizeof(struct __DomeVirtualTable),
                              ctor, "ctor", __DomeVirtualTable_ctor,
                              dtor, "dtor", __DomeVirtualTable_dtor,
                              (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(__DomeVirtualTable_destroy);
#endif
}

const void *
__DomeVirtualTable(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, __DomeVirtualTable_initialize);
#endif
    
    return ___DomeVirtualTable;
}

/*
 * Dome class.
 */

const char *
__dome_get_name(const void *_self)
{
    const struct __DomeClass *class = (const struct __DomeClass *) classOf(_self);
    
    if (isOf(class, __DomeClass()) && class->get_name.method) {
        return ((const char * (*)(const void *)) class->get_name.method) (_self);
    } else {
        const char *result;
        forward(_self, &result, (Method) __dome_get_name, "get_name", _self);
        return result;
    }
}

static const char *
__Dome_get_name(const void *_self)
{
    const struct __Dome *self = cast(__Dome(), _self);
    
    return self->name;
}

int
__dome_register(void *_self, double timeout)
{
    const struct __DomeClass *class = (const struct __DomeClass *) classOf(_self);
    
    if (isOf(class, __DomeClass()) && class->reg.method) {
        return ((int (*)(void *, double)) class->reg.method) (_self, timeout);
    } else {
        int result;
        forward(_self, &result, (Method) __dome_register, "register", _self, timeout);
        return result;
    }
}

static int
__Dome_register(void *_self, double timeout)
{
    struct __Dome *self = cast(__Dome(), _self);
    
    int ret = AAOS_OK;
    
    Pthread_mutex_lock(&self->d_state.mtx);
    if (timeout < 0) {
        while (self->d_state.state&DOME_STATE_MALFUNCTION) {
            Pthread_cond_wait(&self->d_state.cond, &self->d_state.mtx);
        }
    } else {
        struct timespec tp;
        tp.tv_sec = floor(timeout);
        tp.tv_nsec = (timeout - tp.tv_sec) * 1000000000;
        ret = Pthread_cond_timedwait(&self->d_state.cond, &self->d_state.mtx, &tp);
    }
    Pthread_mutex_unlock(&self->d_state.mtx);
    
    return ret;
}

/*
 * pure virtual functions of Dome class.
 */

int
__dome_init(void *_self)
{
    const struct __DomeClass *class = (const struct __DomeClass *) classOf(_self);
    
    if (isOf(class, __DomeClass()) && class->init.method) {
        return ((int (*)(void *)) class->init.method) (_self);
    } else {
        int result;
        forward(_self, &result, (Method) __dome_init, "init", _self);
        return result;
    }
}

int
__dome_open_window(void *_self)
{
    const struct __DomeClass *class = (const struct __DomeClass *) classOf(_self);
    
    if (isOf(class, __DomeClass()) && class->open_window.method) {
        return ((int (*)(void *)) class->open_window.method) (_self);
    } else {
        int result;
        forward(_self, &result, (Method) __dome_open_window, "open_window", _self);
        return result;
    }
}

int
__dome_close_window(void *_self)
{
    const struct __DomeClass *class = (const struct __DomeClass *) classOf(_self);
    
    if (isOf(class, __DomeClass()) && class->close_window.method) {
        return ((int (*)(void *)) class->close_window.method) (_self);
    } else {
        int result;
        forward(_self, &result, (Method) __dome_close_window, "close_window", _self);
        return result;
    }
}

int
__dome_stop_window(void *_self)
{
    const struct __DomeClass *class = (const struct __DomeClass *) classOf(_self);
    
    if (isOf(class, __DomeClass()) && class->stop_window.method) {
        return ((int (*)(void *)) class->stop_window.method) (_self);
    } else {
        int result;
        forward(_self, &result, (Method) __dome_stop_window, "stop_window", _self);
        return result;
    }
}

int
__dome_inspect(void *_self)
{
    const struct __DomeClass *class = (const struct __DomeClass *) classOf(_self);
    
    if (isOf(class, __DomeClass()) && class->inspect.method) {
        return ((int (*)(void *)) class->inspect.method) (_self);
    } else {
        int result;
        forward(_self, &result, (Method) __dome_inspect, "inspect", _self);
        return result;
    }
}

int
__dome_get_window_position(void *_self, double *position)
{
    const struct __DomeClass *class = (const struct __DomeClass *) classOf(_self);
    
    if (isOf(class, __DomeClass()) && class->get_window_position.method) {
        return ((int (*)(void *, double *)) class->get_window_position.method) (_self, position);
    } else {
        int result;
        forward(_self, &result, (Method) __dome_get_window_position, "get_window_position", _self);
        return result;
    }
}

int
__dome_get_window_open_speed(void *_self, double *speed)
{
    const struct __DomeClass *class = (const struct __DomeClass *) classOf(_self);
    
    if (isOf(class, __DomeClass()) && class->get_window_open_speed.method) {
        return ((int (*)(void *, double *)) class->get_window_open_speed.method) (_self, speed);
    } else {
        int result;
        forward(_self, &result, (Method) __dome_get_window_open_speed, "get_window_open_speed", _self);
        return result;
    }
}

int
__dome_set_window_open_speed(void *_self, double speed)
{
    const struct __DomeClass *class = (const struct __DomeClass *) classOf(_self);
    
    if (isOf(class, __DomeClass()) && class->set_window_open_speed.method) {
        return ((int (*)(void *, double )) class->set_window_open_speed.method) (_self, speed);
    } else {
        int result;
        forward(_self, &result, (Method) __dome_set_window_open_speed, "set_window_open_speed", _self);
        return result;
    }
}

int
__dome_get_window_close_speed(void *_self, double *speed)
{
    const struct __DomeClass *class = (const struct __DomeClass *) classOf(_self);
    
    if (isOf(class, __DomeClass()) && class->get_window_close_speed.method) {
        return ((int (*)(void *, double *)) class->get_window_close_speed.method) (_self, speed);
    } else {
        int result;
        forward(_self, &result, (Method) __dome_get_window_close_speed, "get_window_close_speed", _self);
        return result;
    }
}

int
__dome_set_window_close_speed(void *_self, double speed)
{
    const struct __DomeClass *class = (const struct __DomeClass *) classOf(_self);
    
    if (isOf(class, __DomeClass()) && class->set_window_close_speed.method) {
        return ((int (*)(void *, double )) class->set_window_close_speed.method) (_self, speed);
    } else {
        int result;
        forward(_self, &result, (Method) __dome_set_window_close_speed, "set_window_close_speed", _self);
        return result;
    }
}

int
__dome_status(void *_self, void *status_buffer, size_t size, size_t *length)
{
    const struct __DomeClass *class = (const struct __DomeClass *) classOf(_self);
    
    if (isOf(class, __DomeClass()) && class->status.method) {
        return ((int (*)(void *, void *, size_t, size_t *)) class->status.method) (_self, status_buffer, size, length);
    } else {
        int result;
        forward(_self, &result, (Method) __dome_status, "status", _self, status_buffer, size, length);
        return result;
    }
}

int __dome_raw(void *_self, const void *write_buffer, size_t write_buffer_size, size_t *write_size, void *read_buffer, size_t read_buffer_size, size_t *read_size)
{
    const struct __DomeClass *class = (const struct __DomeClass *) classOf(_self);
    
    if (isOf(class, __DomeClass()) && class->raw.method) {
        return ((int (*)(void *, const void *, size_t, size_t *, void *, size_t, size_t *)) class->raw.method)(_self, write_buffer, write_buffer_size, write_size, read_buffer, read_buffer_size, read_size);
    } else {
        int result;
        forward(_self, &result, (Method) __dome_raw, "raw", _self, write_buffer, write_buffer_size, write_size, read_buffer, read_buffer_size, read_size);
        return result;
    }
}

static void
__Dome_forward(const void *_self, void *result, Method selector, const char *name, va_list *app)
{
    const struct __Dome *self = cast(__Dome(), _self);
    Method method = virtualTo(self->_vtab, name);
    va_list ap;
    
#ifdef va_copy
    va_copy(ap, *app);
#else
    ap = *app;
#endif
    
    void *obj = va_arg(*app, void *);
    
    if (selector == (Method) __dome_init || selector == (Method) __dome_open_window || selector == (Method) __dome_stop_window || selector == (Method) __dome_close_window || selector == (Method) __dome_inspect) {
        *((int *) result) = ((int (*)(void *)) method)(obj);
    } else if (selector == (Method) __dome_status) {
        void *status_buffer = va_arg(*app, void *);
        size_t size = va_arg(*app, size_t);
        size_t *length = va_arg(*app, size_t *);
        *((int *) result) = ((int (*)(void *, void *, size_t, size_t *)) method)(obj, status_buffer, size, length);
    } else if (selector == (Method) __dome_raw) {
        const void *write_buffer = va_arg(*app, const void *);
        size_t write_buffer_size = va_arg(*app, size_t);
        size_t *write_size = va_arg(*app, size_t *);
        void *read_buffer = va_arg(*app, void *);
        size_t read_buffer_size = va_arg(*app, size_t);
        size_t *read_size = va_arg(*app, size_t *);
        *((int *) result) = ((int (*)(void *, const void *, size_t, size_t *, void *, size_t, size_t *)) method)(obj, write_buffer, write_buffer_size, write_size, read_buffer, read_buffer_size, read_size);
    } else if (selector == (Method) __dome_register || selector == (Method) __dome_set_window_open_speed || selector == (Method) __dome_set_window_close_speed) {
        double value = va_arg(*app, double);
        *((int *) result) = ((int (*)(void *, double)) method)(obj, value);
    } else if (selector == (Method) __dome_get_window_position || selector == (Method) __dome_get_window_open_speed || selector == (Method) __dome_get_window_close_speed) {
        double *value = va_arg(*app, double *);
        *((int *) result) = ((int (*)(void *, double *)) method)(obj, value);
    } else {
        assert(0);
    }
    
#ifdef va_copy
    va_end(ap);
#endif
}

static void *
__Dome_ctor(void *_self, va_list *app)
{
    struct __Dome *self = super_ctor(__Dome(), _self, app);
    
    const char *s, *key, *value;
    
    s = va_arg(*app, const char *);
    if (s) {
        self->name = (char *) Malloc(strlen(s) + 1);
        snprintf(self->name, strlen(s) + 1, "%s", s);
    }
    
    while ((key = va_arg(*app, const char *))) {
        if (strcmp(key, "description") == 0) {
            value = va_arg(*app, const char *);
            if (value) {
                self->description = (char *) Malloc(strlen(value) + 1);
                snprintf(self->description, strlen(value) + 1, "%s", value);
            }
            continue;
        }
		if (strcmp(key, "window_open_speed") == 0) {
			self->window_open_speed = va_arg(*app, double);
			continue;
		}
		if (strcmp(key, "window_close_speed") == 0) {
			self->window_close_speed = va_arg(*app, double);
			continue;
		}
    }
    self->d_state.state = DOME_STATE_UNINITIALIZED;
    Pthread_mutex_init(&self->d_state.mtx, NULL);
    Pthread_cond_init(&self->d_state.cond, NULL);
    
    return (void *) self;
}

static void
__Dome_puto(void *_self, FILE *fp)
{
    struct __Dome *self = cast(__Dome(), _self);
    
    unsigned int state;
    
    if (self->name) {
        fprintf(fp, "Name:\t%s\n", self->name);
    }
    if (self->description) {
        fprintf(fp, "Description:\t%s\n", self->description);
    }
    Pthread_mutex_lock(&self->d_state.mtx);
    state = self->d_state.state;
    Pthread_mutex_unlock(&self->d_state.mtx);
    
    if (state&DOME_STATE_MALFUNCTION) {
        fprintf(fp, "STATE:\tMALFUNCTION\tand\t");
    } else {
        fprintf(fp, "STATE:\tOK\tand\t");
    }
    state &= ~(DOME_STATE_MALFUNCTION|DOME_STATE_UNINITIALIZED|0x00F0);
    
    if (state == DOME_STATE_WINDOW_OPENED) {
        fprintf(fp, "WINDOW: OPENED\n");
    } else if (state == DOME_STATE_WINDOW_CLOSED) {
        fprintf(fp, "WINDOW: CLOSED\n");
    } else if (state == DOME_STATE_WINDOW_OPENING) {
        fprintf(fp, "WINDOW: OPENING\n");
    } else if (state == DOME_STATE_WINDOW_CLOSING) {
        fprintf(fp, "WINDOW: CLOSING\n");
    } else if (state == DOME_STATE_WINDOW_STOPPED) {
        fprintf(fp, "WINDOW: STOPPED\n");
    } else {
        fprintf(fp, "UNKNOWN\n");
    }
    
}

static void *
__Dome_dtor(void *_self)
{
    struct __Dome *self = cast(__Dome(), _self);
    
    free(self->name);
    free(self->description);
    
    Pthread_mutex_destroy(&self->d_state.mtx);
    Pthread_cond_destroy(&self->d_state.cond);
    
    return super_dtor(__Dome(), _self);
}

static void *
__DomeClass_ctor(void *_self, va_list *app)
{
    struct __DomeClass *self = super_ctor(__DomeClass(), _self, app);
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
        if (selector == (Method) __dome_init) {
            if (tag) {
                self->init.tag = tag;
                self->init.selector = selector;
            }
            self->init.method = method;
            continue;
        }
        if (selector == (Method) __dome_open_window) {
            if (tag) {
                self->open_window.tag = tag;
                self->open_window.selector = selector;
            }
            self->open_window.method = method;
            continue;
        }
        if (selector == (Method) __dome_close_window) {
            if (tag) {
                self->close_window.tag = tag;
                self->close_window.selector = selector;
            }
            self->close_window.method = method;
            continue;
        }
        if (selector == (Method) __dome_stop_window) {
            if (tag) {
                self->stop_window.tag = tag;
                self->stop_window.selector = selector;
            }
            self->stop_window.method = method;
            continue;
        }
        if (selector == (Method) __dome_status) {
            if (tag) {
                self->status.tag = tag;
                self->status.selector = selector;
            }
            self->status.method = method;
            continue;
        }
        if (selector == (Method) __dome_register) {
            if (tag) {
                self->reg.tag = tag;
                self->reg.selector = selector;
            }
            self->reg.method = method;
            continue;
        }
        if (selector == (Method) __dome_inspect) {
            if (tag) {
                self->inspect.tag = tag;
                self->inspect.selector = selector;
            }
            self->inspect.method = method;
            continue;
        }
        if (selector == (Method) __dome_get_name) {
            if (tag) {
                self->get_name.tag = tag;
                self->get_name.selector = selector;
            }
            self->get_name.method = method;
            continue;
        }
        if (selector == (Method) __dome_get_window_position) {
            if (tag) {
                self->get_window_position.tag = tag;
                self->get_window_position.selector = selector;
            }
            self->get_window_position.method = method;
            continue;
        }
        if (selector == (Method) __dome_get_window_open_speed) {
            if (tag) {
                self->get_window_open_speed.tag = tag;
                self->get_window_open_speed.selector = selector;
            }
            self->get_window_open_speed.method = method;
            continue;
        }
        if (selector == (Method) __dome_set_window_open_speed) {
            if (tag) {
                self->set_window_open_speed.tag = tag;
                self->set_window_open_speed.selector = selector;
            }
            self->set_window_open_speed.method = method;
            continue;
        }
        if (selector == (Method) __dome_get_window_close_speed) {
            if (tag) {
                self->get_window_close_speed.tag = tag;
                self->get_window_close_speed.selector = selector;
            }
            self->get_window_close_speed.method = method;
            continue;
        }
        if (selector == (Method) __dome_set_window_close_speed) {
            if (tag) {
                self->set_window_close_speed.tag = tag;
                self->set_window_close_speed.selector = selector;
            }
            self->set_window_close_speed.method = method;
            continue;
        }
        if (selector == (Method) __dome_raw) {
            if (tag) {
                self->raw.tag = tag;
                self->raw.selector = selector;
            }
            self->raw.method = method;
            continue;
        }
    }
    
#ifdef va_copy
    va_end(ap);
#endif
    
    return (void *) self;
}

static const void *___DomeClass;

static void
__DomeClass_destroy(void)
{
    free((void *) ___DomeClass);
}

static void
__DomeClass_initialize(void)
{
    ___DomeClass = new(Class(), "__DomeClass", Class(), sizeof(struct __DomeClass),
                        ctor, "ctor", __DomeClass_ctor,
                        (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(__DomeClass_destroy);
#endif
}

const void *
__DomeClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, __DomeClass_initialize);
#endif
    
    return ___DomeClass;
}

static const void *___Dome;

static void
__Dome_destroy(void)
{
    free((void *)___Dome);
}

static void
__Dome_initialize(void)
{
    ___Dome = new(__DomeClass(), "__Dome", Object(), sizeof(struct __Dome),
                    ctor, "ctor", __Dome_ctor,
                    dtor, "dtor", __Dome_dtor,
                    puto, "puto", __Dome_puto,
                    forward, "forward", __Dome_forward,
                    __dome_register, "register", __Dome_register,
                    __dome_get_name, "get_name", __Dome_get_name,
                   (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(__Dome_destroy);
#endif
}

const void *
__Dome(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, __Dome_initialize);
#endif
    
    return ___Dome;
}

/*
 * Virtual Dome.
 */
static const void *virtual_dome_virtual_table(void);

static void *
VirtualDome_ctor(void *_self, va_list *app)
{
    struct VirtualDome *self = super_ctor(VirtualDome(), _self, app);
    
    self->_.d_state.state = DOME_STATE_UNINITIALIZED;
  
    self->_._vtab= virtual_dome_virtual_table();
    
    return (void *) self;
}

static void *
VirtualDome_dtor(void *_self)
{
    struct VirtualDome *self = cast(VirtualDome(), _self);
    
    return super_dtor(VirtualDome(), _self);
}

static void *
VirtualDomeClass_ctor(void *_self, va_list *app)
{
    struct VirtualDomeClass *self = super_ctor(VirtualDomeClass(), _self, app);
	
    self->_.raw.method = (Method) 0;
    self->_.init.method = (Method) 0;
    self->_.status.method = (Method) 0;
    self->_.open_window.method = (Method) 0;
	self->_.close_window.method = (Method) 0;
	self->_.stop_window.method = (Method) 0;
	self->_.reg.method = (Method) 0;
	self->_.inspect.method = (Method) 0;
	self->_.get_window_position.method = (Method) 0;
	self->_.get_window_open_speed.method = (Method) 0;
	self->_.set_window_open_speed.method = (Method) 0;
	self->_.get_window_close_speed.method = (Method) 0;
	self->_.set_window_close_speed.method = (Method) 0;
	
    return self;
}

static const void *_VirtualDomeClass;

static void
VirtualDomeClass_destroy(void)
{
    free((void *) _VirtualDomeClass);
}

static void
VirtualDomeClass_initialize(void)
{
    _VirtualDomeClass = new(__DomeClass(), "VirtualDomeClass", __DomeClass(), sizeof(struct VirtualDomeClass),
                                ctor, "", VirtualDomeClass_ctor,
                                (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(VirtualDomeClass_destroy);
#endif
}

const void *
VirtualDomeClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, VirtualDomeClass_initialize);
#endif
    
    return _VirtualDomeClass;
}

static const void *_VirtualDome;

static void
VirtualDome_destroy(void)
{
    free((void *)_VirtualDome);
}

static void
VirtualDome_initialize(void)
{
    _VirtualDome = new(VirtualDomeClass(), "VirtualDome", __Dome(), sizeof(struct VirtualDome),
                           ctor, "ctor", VirtualDome_ctor,
                           dtor, "dtor", VirtualDome_dtor,
                           (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(VirtualDome_destroy);
#endif
}

const void *
VirtualDome(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, VirtualDome_initialize);
#endif

    return _VirtualDome;
}

static int
VirtualDome_init(void *_self)
{
    struct VirtualDome *self = cast(VirtualDome(), _self);
	
	unsigned int state;
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
	state = self->_.d_state.state;
	if (state&DOME_STATE_MALFUNCTION) {
		Pthread_mutex_unlock(&self->_.d_state.mtx);
		return AAOS_EDEVMAL;
	}
	state &= ~DOME_STATE_MALFUNCTION;
    if (state == DOME_STATE_UNINITIALIZED) {
        if (self->_.slew_available) {
            self->_.d_state.state = DOME_STATE_WINDOW_CLOSED | DOME_STATE_PARKED;
        } else {
            self->_.d_state.state = DOME_STATE_WINDOW_CLOSED;
        }
		self->_.window_position = 0.;
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    return AAOS_OK;
}

static void *
VirtualDome_simulate_thr(void *arg)
{
    double *sleep_time = (double *) arg;
    
    Nanosleep(*sleep_time);
    
    return NULL;
}

static int
VirtualDome_open_window(void *_self)
{
    struct VirtualDome *self = cast(VirtualDome(), _self);
    
	unsigned int state, tmp;
    int ret = AAOS_OK;
    double duration;
    pthread_t tid;
    struct timespec tp;
    void *result;
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
	state = self->_.d_state.state;
    tmp = 0xFFF0&state;
	if (state&DOME_STATE_MALFUNCTION) {
		Pthread_mutex_unlock(&self->_.d_state.mtx);
		return AAOS_EDEVMAL;
	}
    if (state&DOME_STATE_UNINITIALIZED) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        return AAOS_EUNINIT;
    }
	state &= ~(DOME_STATE_MALFUNCTION|DOME_STATE_UNINITIALIZED|0x00F0);
    switch (state) {
        case DOME_STATE_WINDOW_CLOSED:
        case DOME_STATE_WINDOW_STOPPED:
            Clock_gettime(CLOCK_MONOTONIC, &tp);
            self->last_window_open_time = tp.tv_sec + tp.tv_nsec / 1000000000.;
            duration = (1. - self->_.window_position) / self->_.window_open_speed;
            Pthread_create(&tid, NULL, VirtualDome_simulate_thr, &duration);
            self->tid = tid;
            self->_.d_state.state = DOME_STATE_WINDOW_OPENING;
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            Pthread_join(tid, &result);
            Pthread_mutex_lock(&self->_.d_state.mtx);
            if (result == NULL) {
                self->_.d_state.state = tmp | DOME_STATE_WINDOW_OPENED;
                self->_.window_position = 1.;
                ret = AAOS_OK;
            } else {
                Clock_gettime(CLOCK_MONOTONIC, &tp);
                self->_.window_position += (tp.tv_sec + tp.tv_nsec / 1000000000. - self->last_window_open_time) * self->_.window_open_speed;
                if (self->_.window_position > 1.) {
                    self->_.window_position = 1.;
                }
                ret = AAOS_ECANCELED;
            }
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            Pthread_cond_broadcast(&self->_.d_state.cond);
            break;
        case DOME_STATE_WINDOW_CLOSING:
            Pthread_cancel(self->tid);
            Clock_gettime(CLOCK_MONOTONIC, &tp);
            self->last_window_open_time = tp.tv_sec + tp.tv_nsec / 1000000000.;
            self->_.window_position -= (self->last_window_open_time - self->last_window_close_time) * self->_.window_close_speed;
            if (self->_.window_position < 0.) {
                self->_.window_position = 0.;
            }
            duration = (1. - self->_.window_position) / self->_.window_open_speed;
            Pthread_create(&tid, NULL, VirtualDome_simulate_thr, &duration);
            self->tid = tid;
            self->_.d_state.state = tmp | DOME_STATE_WINDOW_OPENING;
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            Pthread_join(tid, &result);
            Pthread_mutex_lock(&self->_.d_state.mtx);
            if (result == NULL) {
                self->_.d_state.state = tmp | DOME_STATE_WINDOW_OPENED;
                self->_.window_position = 1.;
                ret = AAOS_OK;
            } else {
                Clock_gettime(CLOCK_MONOTONIC, &tp);
                self->_.window_position += (tp.tv_sec + tp.tv_nsec / 1000000000. - self->last_window_open_time) * self->_.window_open_speed;
                if (self->_.window_position > 1.) {
                    self->_.window_position = 1.;
                }
                ret = AAOS_ECANCELED;
            }
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            Pthread_cond_broadcast(&self->_.d_state.cond);
            break;
        case DOME_STATE_WINDOW_OPENING:
            while (self->_.d_state.state&DOME_STATE_WINDOW_OPENING) {
                Pthread_cond_wait(&self->_.d_state.cond, &self->_.d_state.mtx);
            }
            state = self->_.d_state.state;
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            if (state&DOME_STATE_MALFUNCTION) {
                ret = AAOS_EDEVMAL;
            } else {
                state &= ~DOME_STATE_MALFUNCTION;
                if (state & DOME_STATE_WINDOW_OPENED) {
                    ret = AAOS_OK;
                } else {
                    ret = AAOS_ECANCELED;
                }
            }
            break;
        default:
            /*
             * OPENED state, do nothing
             */
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            break;
    }
    
    return ret;
}

static int
VirtualDome_close_window(void *_self)
{
    struct VirtualDome *self = cast(VirtualDome(), _self);
    
    unsigned int state, tmp;
    int ret = AAOS_OK;
    double duration;
    pthread_t tid;
    struct timespec tp;
    void *result;
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    tmp = 0xFFF0&state;
    if (state&DOME_STATE_MALFUNCTION) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        return AAOS_EDEVMAL;
    }
    if (state&DOME_STATE_UNINITIALIZED) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        return AAOS_EUNINIT;
    }
    state &= ~(DOME_STATE_MALFUNCTION|DOME_STATE_UNINITIALIZED|0x00F0);
    switch (state) {
        case DOME_STATE_WINDOW_OPENED:
        case DOME_STATE_WINDOW_STOPPED:
            Clock_gettime(CLOCK_MONOTONIC, &tp);
            self->last_window_close_time = tp.tv_sec + tp.tv_nsec / 1000000000.;
            duration =  self->_.window_position / self->_.window_close_speed;
            Pthread_create(&tid, NULL, VirtualDome_simulate_thr, &duration);
            self->tid = tid;
            self->_.d_state.state = tmp | DOME_STATE_WINDOW_CLOSING;
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            Pthread_join(tid, &result);
            Pthread_mutex_lock(&self->_.d_state.mtx);
            if (result == NULL) {
                self->_.d_state.state = tmp | DOME_STATE_WINDOW_CLOSED;
                self->_.window_position = 0.;
                ret = AAOS_OK;
            } else {
                Clock_gettime(CLOCK_MONOTONIC, &tp);
                self->_.window_position -= (tp.tv_sec + tp.tv_nsec / 1000000000. - self->last_window_close_time) * self->_.window_close_speed;
                if (self->_.window_position < 0.) {
                    self->_.window_position = 0.;
                }
                ret = AAOS_ECANCELED;
            }
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            Pthread_cond_broadcast(&self->_.d_state.cond);
            break;
        case DOME_STATE_WINDOW_OPENING:
            Pthread_cancel(self->tid);
            Clock_gettime(CLOCK_MONOTONIC, &tp);
            self->last_window_close_time = tp.tv_sec + tp.tv_nsec / 1000000000.;
            self->_.window_position += (self->last_window_close_time - self->last_window_open_time) * self->_.window_open_speed;
            if (self->_.window_position > 1.) {
                self->_.window_position = 1.;
            }
            duration = self->_.window_position / self->_.window_close_speed;
            Pthread_create(&tid, NULL, VirtualDome_simulate_thr, &duration);
            self->tid = tid;
            self->_.d_state.state = tmp | DOME_STATE_WINDOW_CLOSING;
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            Pthread_join(tid, &result);
            Pthread_mutex_lock(&self->_.d_state.mtx);
            if (result == NULL) {
                self->_.d_state.state = tmp | DOME_STATE_WINDOW_CLOSED;
                self->_.window_position = 0.;
                ret = AAOS_OK;
            } else {
                Clock_gettime(CLOCK_MONOTONIC, &tp);
                self->_.window_position -= (tp.tv_sec + tp.tv_nsec / 1000000000. - self->last_window_close_time) * self->_.window_open_speed;
                if (self->_.window_position < 0.) {
                    self->_.window_position = 0.;
                }
                ret = AAOS_ECANCELED;
            }
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            Pthread_cond_broadcast(&self->_.d_state.cond);
            break;
        case DOME_STATE_WINDOW_CLOSING:
            while (self->_.d_state.state&DOME_STATE_WINDOW_CLOSING) {
                Pthread_cond_wait(&self->_.d_state.cond, &self->_.d_state.mtx);
            }
            state = self->_.d_state.state;
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            if (state&DOME_STATE_MALFUNCTION) {
                ret = AAOS_EDEVMAL;
            } else {
                state &= ~DOME_STATE_MALFUNCTION;
                if (state&DOME_STATE_WINDOW_CLOSED) {
                    ret = AAOS_OK;
                } else {
                    ret = AAOS_ECANCELED;
                }
            }
            break;
        default:
            /*
             * CLOSED state, do nothing
             */
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            break;
    }
    
    return ret;
}

static int
VirtualDome_stop_window(void *_self)
{
    struct VirtualDome *self = cast(VirtualDome(), _self);
    
    unsigned int state, tmp;
    int ret = AAOS_OK;
    double duration;
    pthread_t tid;
    struct timespec tp;
    void *result;
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    tmp = 0xFFF0&state;
    if (state&DOME_STATE_MALFUNCTION) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        return AAOS_EDEVMAL;
    }
    if (state&DOME_STATE_UNINITIALIZED) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        return AAOS_EUNINIT;
    }
    state &= ~DOME_STATE_MALFUNCTION;
    switch (state) {
        case DOME_STATE_WINDOW_OPENED:
        case DOME_STATE_WINDOW_STOPPED:
        case DOME_STATE_WINDOW_CLOSED:
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            ret = AAOS_OK;
            break;
        case DOME_STATE_WINDOW_OPENING:
            Pthread_cancel(self->tid);
            Clock_gettime(CLOCK_MONOTONIC, &tp);
            self->_.d_state.state = tmp|DOME_STATE_WINDOW_STOPPED;
            self->_.window_position += (tp.tv_sec + tp.tv_nsec / 1000000000. -  self->last_window_open_time) * self->_.window_open_speed;
            if (self->_.window_position > 1.) {
                self->_.window_position = 1.;
            }
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            ret = AAOS_OK;
            break;
        case DOME_STATE_WINDOW_CLOSING:
            Pthread_cancel(self->tid);
            Clock_gettime(CLOCK_MONOTONIC, &tp);
            self->_.d_state.state = tmp|DOME_STATE_WINDOW_STOPPED;
            self->_.window_position -= (tp.tv_sec + tp.tv_nsec / 1000000000. -  self->last_window_close_time) * self->_.window_open_speed;
            if (self->_.window_position < 0.) {
                self->_.window_position = 0.;
            }
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            ret = AAOS_OK;
            break;
        default:
            break;
    }
    
    return ret;
}

static int
VirtualDome_status(void *_self, void *status_buffer, size_t size, size_t *length)
{
    struct VirtualDome *self = cast(VirtualDome(), _self);
    
    unsigned int state, state2, tmp;
    bool status;
    double position = 0., open_speed = 0., close_speed = 0.;
    struct timespec tp;

    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    state2 = state;
    if (state&DOME_STATE_MALFUNCTION) {
        status = false;
    } else {
        status = true;
    }
    state &= ~DOME_STATE_MALFUNCTION;
    state &= 0x000F;
    if (state == DOME_STATE_WINDOW_CLOSED || state == DOME_STATE_WINDOW_OPENED || state == DOME_STATE_WINDOW_OPENED) {
        position = self->_.window_position;
    } else if (state == DOME_STATE_WINDOW_CLOSING) {
        Clock_gettime(CLOCK_MONOTONIC, &tp);
        position = self->_.window_position - (tp.tv_sec + tp.tv_nsec/1000000000. - self->last_window_close_time) * self->_.window_close_speed;
        if (position < 0.) {
            position = 0.;
        }
    } else if (state == DOME_STATE_WINDOW_OPENING) {
        Clock_gettime(CLOCK_MONOTONIC, &tp);
        position = self->_.window_position + (tp.tv_sec + tp.tv_nsec/1000000000. - self->last_window_open_time) * self->_.window_open_speed;
        if (position > 1.) {
            position = 1.;
        }
    }
    open_speed = self->_.window_open_speed;
    close_speed = self->_.window_close_speed;
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    cJSON *root_json;
    
    if ((root_json = cJSON_CreateObject()) != NULL) {
        cJSON_AddStringToObject(root_json, "name", self->_.name);
        if (self->_.description != NULL) {
            cJSON_AddStringToObject(root_json, "description", self->_.description);
        }
        if (status) {
            cJSON_AddStringToObject(root_json, "status", "OK");
        } else {
            cJSON_AddStringToObject(root_json, "status", "MALFUNCTION");
        }
        switch (state) {
            case DOME_STATE_WINDOW_CLOSED:
                cJSON_AddStringToObject(root_json, "state", "CLOSED");
                break;
            case DOME_STATE_WINDOW_OPENED:
                cJSON_AddStringToObject(root_json, "state", "OPEN");
                break;
            case DOME_STATE_WINDOW_STOPPED:
                cJSON_AddStringToObject(root_json, "state", "STOPPED");
                break;
            case DOME_STATE_WINDOW_OPENING:
                cJSON_AddStringToObject(root_json, "state", "OPENING");
                break;
            case DOME_STATE_WINDOW_CLOSING:
                cJSON_AddStringToObject(root_json, "state", "CLOSING");
                break;
            default:
                break;
        }
        cJSON_AddNumberToObject(root_json, "open_speed", open_speed);
        cJSON_AddNumberToObject(root_json, "close_speed", close_speed);
        cJSON_AddNumberToObject(root_json, "position", position);
        cJSON_PrintPreallocated(root_json, status_buffer, (int) size, true);
        cJSON_Delete(root_json);
        if (length != NULL) {
            *length = strlen(status_buffer) + 1;
        }
    }
    
    return AAOS_OK;
}

static int
VirtualDome_get_window_open_speed(void *_self, double *speed)
{
    struct VirtualDome *self = cast(VirtualDome(), _self);
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    *speed = self->_.window_open_speed;
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    return AAOS_OK;
}

static int
VirtualDome_set_window_open_speed(void *_self, double speed)
{
    struct VirtualDome *self = cast(VirtualDome(), _self);
    
    int ret;
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    if (!(self->_.d_state.state&DOME_STATE_WINDOW_OPENING)) {
        self->_.window_open_speed = speed;
        ret = AAOS_OK;
    } else {
        ret = AAOS_EBUSY;
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    return ret;
}

static int
VirtualDome_get_window_close_speed(void *_self, double *speed)
{
    struct VirtualDome *self = cast(VirtualDome(), _self);
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    *speed = self->_.window_close_speed;
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    return AAOS_OK;
}

static int
VirtualDome_set_window_close_speed(void *_self, double speed)
{
    struct VirtualDome *self = cast(VirtualDome(), _self);
    
    int ret;
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    if (!(self->_.d_state.state&DOME_STATE_WINDOW_CLOSING)) {
        self->_.window_close_speed = speed;
        ret = AAOS_OK;
    } else {
        ret = AAOS_EBUSY;
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    return ret;
}

static int
VirtualDome_get_window_position(void *_self, double *position)
{
    struct VirtualDome *self = cast(VirtualDome(), _self);
    
    unsigned int state;
    struct timespec tp;
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    if (state&DOME_STATE_UNINITIALIZED) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        return AAOS_EUNINIT;
    }
    state &= 0x000F;
    switch (state) {
        case DOME_STATE_WINDOW_CLOSED:
            *position = 0.;
            break;
        case DOME_STATE_WINDOW_OPENED:
            *position = 1.;
            break;
        case DOME_STATE_WINDOW_STOPPED:
            *position = self->_.window_position;
            break;
        case DOME_STATE_WINDOW_OPENING:
            Clock_gettime(CLOCK_MONOTONIC, &tp);
            *position = self->_.window_position + (tp.tv_sec + tp.tv_nsec/ 1000000000. - self->last_window_open_time) * self->_.window_open_speed;
            if (*position > 1.) {
                *position = 1.;
            }
            break;
        case DOME_STATE_WINDOW_CLOSING:
            Clock_gettime(CLOCK_MONOTONIC, &tp);
            *position = self->_.window_position - (tp.tv_sec + tp.tv_nsec/ 1000000000. - self->last_window_close_time) * self->_.window_close_speed;
            if (*position < 0.) {
                *position = 0.;
            }
            break;
        default:
            break;
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    return AAOS_OK;
}

static int
VirtualDome_inspect(void *_self)
{
    return AAOS_ENOTSUP;
}

static int
VirtualDome_register(void *_self, double timeout)
{
    return AAOS_ENOTSUP;
}

static const void *_virtual_dome_virtual_table;

static void
virtual_dome_virtual_table_destroy(void)
{
    delete((void *) _virtual_dome_virtual_table);
}

static void
virtual_dome_virtual_table_initialize(void)
{
    _virtual_dome_virtual_table = new(__DomeVirtualTable(),
                                      __dome_init, "init", VirtualDome_init,
                                      __dome_open_window, "open_window", VirtualDome_open_window,
                                      __dome_close_window, "close_window", VirtualDome_close_window,
                                      __dome_stop_window, "stop_windows", VirtualDome_stop_window,
                                      __dome_status, "status", VirtualDome_status,
                                      __dome_get_window_open_speed, "get_window_open_speed", VirtualDome_get_window_open_speed,
                                      __dome_set_window_open_speed, "set_window_open_speed", VirtualDome_set_window_open_speed,
                                      __dome_get_window_close_speed, "get_window_close_speed", VirtualDome_get_window_close_speed,
                                      __dome_set_window_close_speed, "set_window_close_speed", VirtualDome_set_window_close_speed,
                                      __dome_get_window_position, "get_window_position", VirtualDome_get_window_position,
                                      __dome_inspect, "inspect", VirtualDome_inspect,
                                      __dome_register, "register", VirtualDome_register,
                                      (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(virtual_dome_virtual_table_destroy);
#endif
}

static const void *
virtual_dome_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, virtual_dome_virtual_table_initialize);
#endif
    
    return _virtual_dome_virtual_table;
}

#ifdef _USE_COMPILER_ATTRIBUTION_
static void __destructor__(void) __attribute__ ((destructor(_DOME_PRIORITY_)));

static void
__destructor__(void)
{
    VirtualDome_destroy();
    VirtualDomeClass_destroy();
    virtual_dome_virtual_table_destroy();
    
    __Dome_destroy();
    __DomeClass_destroy();
    __DomeVirtualTable_destroy();
}

static void __constructor__(void) __attribute__ ((constructor(_DOME_PRIORITY_)));

static void
__constructor__(void)
{
    __DomeVirtualTable_initialize();
    __DomeClass_initialize();
    __Dome_initialize();
    
    virtual_dome_virtual_table_initialize();
    VirtualDomeClass_initialize();
    VirtualDome_initialize();
}
#endif
