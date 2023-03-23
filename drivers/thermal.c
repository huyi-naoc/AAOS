//
//  thermal.c
//  AAOS
//
//  Created by Hu Yi on 2023/2/25.
//  Copyright © 2023 NAOC. All rights reserved.
//

#include "def.h"
#include "thermal_def.h"
#include "thermal.h"
#include "thermal_r.h"
#include "virtual.h"
#include "wrapper.h"

/*
 * ThermalUnit virtual table.
 */
static void *
__ThermalUnitVirtualTable_ctor(void *_self, va_list *app)
{
    struct __ThermalUnitVirtualTable *self = super_ctor(__ThermalUnitVirtualTable(), _self, app);
    Method selector;
    
    while ((selector = va_arg(*app, Method))) {
        const char *tag = va_arg(*app, const char *);
        Method method = va_arg(*app, Method);
        
        if (selector == (Method) __thermal_unit_turn_on) {
            if (tag) {
                self->turn_on.tag = tag;
                self->turn_on.selector = selector;
            }
            self->turn_on.method = method;
            continue;
        }
        
        if (selector == (Method) __thermal_unit_turn_off) {
            if (tag) {
                self->turn_off.tag = tag;
                self->turn_off.selector = selector;
            }
            self->turn_off.method = method;
            continue;
        }
        
        if (selector == (Method) __thermal_unit_get_temperature) {
            if (tag) {
                self->get_temperature.tag = tag;
                self->get_temperature.selector = selector;
            }
            self->get_temperature.method = method;
            continue;
        }
        
        if (selector == (Method) __thermal_unit_thermal_control) {
            if (tag) {
                self->get_temperature.tag = tag;
                self->get_temperature.selector = selector;
            }
            self->get_temperature.method = method;
            continue;
        }
        if (selector == (Method) __thermal_unit_status) {
            if (tag) {
                self->status.tag = tag;
                self->status.selector = selector;
            }
            self->status.method = method;
            continue;
        }
    }
    
    return _self;
}

static void *
__ThermalUnitVirtualTable_dtor(void *_self)
{
    return super_dtor(Object(), _self);
}

static const void *___ThermalUnitVirtualTable;

static void
__ThermalUnitVirtualTable_destroy(void)
{
    free((void *) ___ThermalUnitVirtualTable);
}

static void
__ThermalUnitVirtualTable_initialize(void)
{
    ___ThermalUnitVirtualTable = new(VirtualTableClass(), "__ThermalUnitVirtualTable", VirtualTable(), sizeof(struct __ThermalUnitVirtualTable),
                                     ctor, "ctor", __ThermalUnitVirtualTable_ctor,
                                     dtor, "dtor", __ThermalUnitVirtualTable_dtor,
                                     (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(__ThermalUnitVirtualTable_destroy);
#endif
}

const void *
__ThermalUnitVirtualTable(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, __ThermalUnitVirtualTable_initialize);
#endif
    
    return ___ThermalUnitVirtualTable;
}

const char *
__thermal_unit_get_name(const void *_self)
{
    const struct __ThermalUnitClass *class = (const struct __ThermalUnitClass *) classOf(_self);
    
    if (isOf(class, __ThermalUnitClass()) && class->get_name.method) {
        return ((const char * (*)(const void *)) class->get_name.method)(_self);
    } else {
        const char *result;
        forward(_self, &result, (Method) __thermal_unit_get_name, "get_name", _self);
        return result;
    }
}

static const char *
__ThermalUnit_get_name(const void *_self)
{
    struct __ThermalUnit *self = cast(__ThermalUnit(), _self);
    
    return self->name;
}

int
__thermal_unit_status(void *_self, void *buffer, size_t size, size_t *res_len)
{
    const struct __ThermalUnitClass *class = (const struct __ThermalUnitClass *) classOf(_self);
    
    if (isOf(class, __ThermalUnitClass()) && class->status.method) {
        return ((int (*)(void *, void *, size_t, size_t *)) class->status.method)(_self, buffer, size, res_len);
    } else {
        int result;
        forward(_self, &result, (Method) __thermal_unit_turn_on, "status", _self, buffer, size, res_len);
        return result;
    }
}

static int
__ThermalUnit_status(void *_self, void *buffer, size_t size, size_t *res_len)
{
    struct __ThermalUnit *self = cast(__ThermalUnit(), _self);
    
    uint16_t status;
    
    Pthread_mutex_lock(&self->mtx);
    status = (uint16_t) self->state;
    Pthread_mutex_unlock(&self->mtx);
    
    if (size < sizeof(uint16_t)) {
        return AAOS_ENOMEM;
    }
    
    memcpy(buffer, &status, sizeof(uint16_t));
    
    if (res_len != NULL) {
        *res_len = sizeof(uint16_t);
    }
    
    return AAOS_OK;
}

void *
__thermal_unit_thermal_control(void *_self)
{
    const struct __ThermalUnitClass *class = (const struct __ThermalUnitClass *) classOf(_self);
    
    if (isOf(class, __ThermalUnitClass()) && class->thermal_control.method) {
        return ((void * (*)(void *)) class->thermal_control.method)(_self);
    } else {
        void *result;
        forward(_self, &result, (Method) __thermal_unit_thermal_control, "thermal_control", _self);
        return result;
    }
}

static void *
__ThermalUnit_thermal_control(void *_self)
{
    struct __ThermalUnit *self = cast(__ThermalUnit(), _self);
    double temperature;
    int ret;
    for (; ;) {
        if ((ret = __thermal_unit_get_temperature(_self, &temperature)) != AAOS_OK) {
            /*
             *
             */
        }
        if (temperature > self->highest && self->state == THERMAL_UNIT_STATE_ON) {
            __thermal_unit_turn_off(_self);
        } else if (temperature < self->lowest && self->state == THERMAL_UNIT_STATE_OFF) {
            __thermal_unit_turn_on(_self);
        }
        Nanosleep(self->period);
    }
    return NULL;
}

int
__thermal_unit_turn_on(void *_self)
{
    const struct __ThermalUnitClass *class = (const struct __ThermalUnitClass *) classOf(_self);
    
    if (isOf(class, __ThermalUnitClass()) && class->turn_on.method) {
        return ((int (*)(void *)) class->turn_on.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) __thermal_unit_turn_on, "turn_on", _self);
        return result;
    }
}

int
__thermal_unit_turn_off(void *_self)
{
    const struct __ThermalUnitClass *class = (const struct __ThermalUnitClass *) classOf(_self);
    
    if (isOf(class, __ThermalUnitClass()) && class->turn_off.method) {
        return ((int (*)(void *)) class->turn_off.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) __thermal_unit_turn_off, "turn_off", _self);
        return result;
    }
}

int
__thermal_unit_get_temperature(const void *_self, double *temperature)
{
    const struct __ThermalUnitClass *class = (const struct __ThermalUnitClass *) classOf(_self);
    
    if (isOf(class, __ThermalUnitClass()) && class->get_temperature.method) {
        return ((int (*)(const void *, double *)) class->turn_off.method)(_self, temperature);
    } else {
        int result;
        forward(_self, &result, (Method) __thermal_unit_get_temperature, "get_temperature", _self, temperature);
        return result;
    }
}

static void
__ThermalUnit_forward(const void *_self, void *result, Method selector, const char *name, va_list *app)
{
    const struct __ThermalUnit *self = cast(__ThermalUnit(), _self);
    Method method = virtualTo(self->_vtab, name);
    va_list ap;
    
#ifdef va_copy
    va_copy(ap, *app);
#else
    ap = *app;
#endif
    
    void *obj = va_arg(*app, void *);
    
    if (selector == (Method) __thermal_unit_turn_on || selector == (Method) __thermal_unit_turn_off) {
        *((int *) result) = ((int (*)(void *)) method)(obj);
    } else if (selector == (Method) __thermal_unit_get_temperature) {
        double *temperature = va_arg(*app, double *);
        *((int *) result) = ((int (*)(const void *, double *)) method)(obj, temperature);
    } else if (selector == (Method) __thermal_unit_thermal_control) {
        *((void **) result) = ((void * (*)(void *)) method)(obj);
    } else if (selector == (Method) __thermal_unit_status) {
        void *buffer = va_arg(*app, void *);
        size_t size = va_arg(*app, size_t);
        size_t *res_size = va_arg(*app, size_t *);
        *((int *) result) = ((int (*)(void *, void *, size_t, size_t *)) method)(obj, buffer, size, res_size);
    } else {
        assert(0);
    }
    
#ifdef va_copy
    va_end(ap);
#endif
}

/*
 * tu = new(__ThermalUnit(), name, highest, lowest, period, "description", '\0')
 */

static void *
__ThermalUnit_ctor(void *_self, va_list *app)
{
    struct __ThermalUnit *self = super_ctor(__ThermalUnit(), _self, app);
    
    const char *s, *key, *value;
    
    s = va_arg(*app, const char *);
    if (s) {
        self->name = (char *) Malloc(strlen(s) + 1);
        snprintf(self->name, strlen(s) + 1, "%s", s);
    }


    self->highest = va_arg(*app, double);
    self->lowest = va_arg(*app, double);
    self->period = va_arg(*app, double);
    
    while ((key = va_arg(*app, const char *))) {
        if (strcmp(key, "description") == 0) {
            value = va_arg(*app, const char *);
            if (value) {
                self->description = (char *) Malloc(strlen(value) + 1);
                snprintf(self->description, strlen(value) + 1, "%s", value);
            }
            continue;
        }
    }
    
    Pthread_mutex_init(&self->mtx, NULL);
    
    return (void *) self;
}

static void *
__ThermalUnit_dtor(void *_self)
{
    struct __ThermalUnit *self = cast(__ThermalUnit(), _self);
    
    free(self->name);
    free(self->description);
    
    Pthread_mutex_destroy(&self->mtx);
    
    return super_dtor(__ThermalUnit(), _self);
}

static void *
__ThermalUnitClass_ctor(void *_self, va_list *app)
{
    struct __ThermalUnitClass *self = super_ctor(__ThermalUnitClass(), _self, app);
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
        if (selector == (Method) __thermal_unit_turn_on) {
            if (tag) {
                self->turn_on.tag = tag;
                self->turn_on.selector = selector;
            }
            self->turn_on.method = method;
            continue;
        }
        if (selector == (Method) __thermal_unit_turn_off) {
            if (tag) {
                self->turn_off.tag = tag;
                self->turn_off.selector = selector;
            }
            self->turn_off.method = method;
            continue;
        }
        if (selector == (Method) __thermal_unit_thermal_control) {
            if (tag) {
                self->thermal_control.tag = tag;
                self->thermal_control.selector = selector;
            }
            self->thermal_control.method = method;
            continue;
        }
        if (selector == (Method) __thermal_unit_get_temperature) {
            if (tag) {
                self->get_temperature.tag = tag;
                self->get_temperature.selector = selector;
            }
            self->get_temperature.method = method;
            continue;
        }
        if (selector == (Method) __thermal_unit_get_name) {
            if (tag) {
                self->get_name.tag = tag;
                self->get_name.selector = selector;
            }
            self->get_name.method = method;
            continue;
        }
    }
    
#ifdef va_copy
    va_end(ap);
#endif
    
    return (void *) self;
}

static const void *___ThermalUnitClass;

static void
__ThermalUnitClass_destroy(void)
{
    free((void *) ___ThermalUnitClass);
}

static void
__ThermalUnitClass_initialize(void)
{
    ___ThermalUnitClass = new(Class(), "__ThermalUnitClass", Class(), sizeof(struct __ThermalUnitClass),
                              ctor, "ctor", __ThermalUnitClass_ctor,
                              (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(__ThermalUnitClass_destroy);
#endif
}

const void *
__ThermalUnitClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, __ThermalUnitClass_initialize);
#endif
    
    return ___ThermalUnitClass;
}

static const void *___ThermalUnit;

static void
__ThermalUnit_destroy(void)
{
    free((void *)___ThermalUnit);
}

static void
__ThermalUnit_initialize(void)
{
    ___ThermalUnit = new(__ThermalUnitClass(), "__ThermalUnit", Object(), sizeof(struct __ThermalUnit),
                         ctor, "ctor", __ThermalUnit_ctor,
                         dtor, "dtor", __ThermalUnit_dtor,
                         forward, "forward", __ThermalUnit_forward,
                         __thermal_unit_get_name, "get_name", __ThermalUnit_get_name,
                         __thermal_unit_thermal_control, "thermal_control", __ThermalUnit_thermal_control,
                       (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(__ThermalUnit_destroy);
#endif
}

const void *
__ThermalUnit(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, __ThermalUnit_initialize);
#endif
    
    return ___ThermalUnit;
}

/*
 * A simple  thermal control, directly using low level commands.
 *
 */

static const void *simple_thermal_unit_virtual_table(void);

/*
 * tu = new(SimpleThermalUnit(), ..., temp_cmd, turn_on_cmd, turn_off_cmd)
 */
static void *
SimpleThermalUnit_ctor(void *_self, va_list *app)
{
    struct SimpleThermalUnit *self = super_ctor(SimpleThermalUnit(), _self, app);
    
    self->_._vtab= simple_thermal_unit_virtual_table();
    
    char *s;
    
    s = va_arg(*app, char *);
    if (s) {
        self->temp_cmd = (char *) Malloc(strlen(s) + 1);
        snprintf(self->temp_cmd, strlen(s) + 1, "%s", s);
    }
    s = va_arg(*app, char *);
    if (s) {
        self->turn_on_cmd = (char *) Malloc(strlen(s) + 1);
        snprintf(self->turn_on_cmd, strlen(s) + 1, "%s", s);
    }
    s = va_arg(*app, char *);
    if (s) {
        self->turn_off_cmd = (char *) Malloc(strlen(s) + 1);
        snprintf(self->turn_off_cmd, strlen(s) + 1, "%s", s);
    }
    
    return (void *) self;
}

static void *
SimpleThermalUnit_dtor(void *_self)
{
    struct SimpleThermalUnit *self = cast(SimpleThermalUnit(), _self);
    
    free(self->temp_cmd);
    free(self->turn_on_cmd);
    free(self->turn_off_cmd);
    
    return super_dtor(KLCAMSimpleThermalUnit(), _self);
}

static void *
SimpleThermalUnitClass_ctor(void *_self, va_list *app)
{
    struct SimpleThermalUnitClass *self = super_ctor(SimpleThermalUnitClass(), _self, app);
    
    self->_.get_temperature.method = (Method) 0;
    self->_.turn_on.method = (Method) 0;
    self->_.turn_off.method = (Method) 0;
    
    return self;
}

static const void *_SimpleThermalUnitClass;

static void
SimpleThermalUnitClass_destroy(void)
{
    free((void *)_SimpleThermalUnitClass);
}

static void
SimpleThermalUnitClass_initialize(void)
{
    _SimpleThermalUnitClass = new(__ThermalUnitClass(), "SimpleThermalUnitClass", __ThermalUnitClass(), sizeof(struct SimpleThermalUnitClass),
                                  ctor, "", SimpleThermalUnitClass_ctor,
                                  (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(SimpleThermalUnitClass_destroy);
#endif
}

const void *
SimpleThermalUnitClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, SimpleThermalUnitClass_initialize);
#endif
    
    return _SimpleThermalUnitClass;
}

static const void *_SimpleThermalUnit;

static void
SimpleThermalUnit_destroy(void)
{
    
    free((void *)_SimpleThermalUnit);
}

static void
SimpleThermalUnit_initialize(void)
{
  
    _SimpleThermalUnit = new(SimpleThermalUnitClass(), "SimpleThermalUnit", __ThermalUnit(), sizeof(struct SimpleThermalUnit),
                             ctor, "ctor", SimpleThermalUnit_ctor,
                             dtor, "dtor", SimpleThermalUnit_dtor,
                             (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(SimpleThermalUnit_destroy);
#endif
}

const void *
SimpleThermalUnit(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, SimpleThermalUnit_initialize);
#endif

    return _SimpleThermalUnit;
}

static int
SimpleThermalUnit_turn_on(void *_self)
{
    struct SimpleThermalUnit *self = cast(SimpleThermalUnit(), _self);
    
    int ret = AAOS_ERROR;
    FILE *fp;
    
    if (self->turn_on_cmd != NULL && (fp = popen(self->turn_on_cmd, "r")) != NULL) {
        Pthread_mutex_lock(&self->_.mtx);
        self->_.state = THERMAL_UNIT_STATE_ON;
        Pthread_mutex_unlock(&self->_.mtx);
        pclose(fp);
    }
    
    return ret;
}

static int
SimpleThermalUnit_turn_off(void *_self)
{
    struct SimpleThermalUnit *self = cast(SimpleThermalUnit(), _self);
    
    int ret = AAOS_ERROR;
    FILE *fp;
    
    if (self->turn_off_cmd != NULL && (fp = popen(self->turn_off_cmd, "r")) != NULL) {
        Pthread_mutex_lock(&self->_.mtx);
        self->_.state = THERMAL_UNIT_STATE_OFF;
        Pthread_mutex_unlock(&self->_.mtx);
        pclose(fp);
    }
    
    return ret;
}

static int
SimpleThermalUnit_get_temperature(void *_self, double *temperature)
{
    struct SimpleThermalUnit *self = cast(SimpleThermalUnit(), _self);
    
    int ret = AAOS_ERROR;
    FILE *fp;
    
    *temperature = 9999.;
    if (self->temp_cmd != NULL && (fp = popen(self->temp_cmd, "r")) != NULL) {
        fscanf(fp, "%lf", temperature);
        pclose(fp);
    }
    
    return ret;
}

static const void *_simple_thermal_unit_virtual_table;

static void
simple_thermal_unit_virtual_table_destroy(void)
{
    delete((void *) _simple_thermal_unit_virtual_table);
}

static void
simple_thermal_unit_virtual_table_initialize(void)
{
   _simple_thermal_unit_virtual_table = new(__ThermalUnitVirtualTable(),
                                            __thermal_unit_turn_on, "turn_on", SimpleThermalUnit_turn_on,
                                            __thermal_unit_turn_off, "turn_off", SimpleThermalUnit_turn_off,
                                            __thermal_unit_get_temperature, "get_temperature", SimpleThermalUnit_get_temperature,
                                            (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(simple_thermal_unit_virtual_table_destroy);
#endif
}

static const void *
simple_thermal_unit_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, simple_thermal_unit_virtual_table_initialize);
#endif
    
    return _simple_thermal_unit_virtual_table;
}

/*
 * A simple KLCAM thermal control, directly using low level commands.
 * Two level thermal control, internal and external.
 */

static const void *klcam_simple_thermal_unit_virtual_table(void);

/*
 * tu = new(KLCAMSimpleThermalUnit(), ..., temp_cmd, turn_on_cmd, turn_off_cmd, temp_cmd2, turn_on_cmd2, turn_off_cmd2, temp_cmd3, threshold)
 */
static void *
KLCAMSimpleThermalUnit_ctor(void *_self, va_list *app)
{
    struct KLCAMSimpleThermalUnit *self = super_ctor(KLCAMSimpleThermalUnit(), _self, app);
    
    self->_._vtab= klcam_simple_thermal_unit_virtual_table();
    
    char *s;
    
    s = va_arg(*app, char *);
    if (s) {
        self->temp_cmd = (char *) Malloc(strlen(s) + 1);
        snprintf(self->temp_cmd, strlen(s) + 1, "%s", s);
    }
    s = va_arg(*app, char *);
    if (s) {
        self->turn_on_cmd = (char *) Malloc(strlen(s) + 1);
        snprintf(self->turn_on_cmd, strlen(s) + 1, "%s", s);
    }
    s = va_arg(*app, char *);
    if (s) {
        self->turn_off_cmd = (char *) Malloc(strlen(s) + 1);
        snprintf(self->turn_off_cmd, strlen(s) + 1, "%s", s);
    }
    s = va_arg(*app, char *);
    if (s) {
        self->temp_cmd2 = (char *) Malloc(strlen(s) + 1);
        snprintf(self->temp_cmd2, strlen(s) + 1, "%s", s);
    }
    s = va_arg(*app, char *);
    if (s) {
        self->turn_on_cmd2 = (char *) Malloc(strlen(s) + 1);
        snprintf(self->turn_on_cmd2, strlen(s) + 1, "%s", s);
    }
    s = va_arg(*app, char *);
    if (s) {
        self->turn_off_cmd2 = (char *) Malloc(strlen(s) + 1);
        snprintf(self->turn_off_cmd2, strlen(s) + 1, "%s", s);
    }
    s = va_arg(*app, char *);
    if (s) {
        self->temp_cmd3 = (char *) Malloc(strlen(s) + 1);
        snprintf(self->temp_cmd3, strlen(s) + 1, "%s", s);
    }
    
    Pthread_mutex_init(&self->mtx, NULL);
    return (void *) self;
}

static void *
KLCAMSimpleThermalUnit_dtor(void *_self)
{
    struct KLCAMSimpleThermalUnit *self = cast(KLCAMSimpleThermalUnit(), _self);
    
    free(self->temp_cmd);
    free(self->turn_on_cmd);
    free(self->turn_off_cmd);
    free(self->temp_cmd2);
    free(self->turn_on_cmd2);
    free(self->turn_off_cmd2);
    free(self->temp_cmd3);
    
    Pthread_mutex_destroy(&self->mtx);
    
    return super_dtor(KLCAMSimpleThermalUnit(), _self);
}

static void *
KLCAMSimpleThermalUnitClass_ctor(void *_self, va_list *app)
{
    struct KLCAMSimpleThermalUnitClass *self = super_ctor(KLCAMSimpleThermalUnitClass(), _self, app);
    
    self->_.get_temperature.method = (Method) 0;
    self->_.turn_on.method = (Method) 0;
    self->_.turn_off.method = (Method) 0;
    self->_.thermal_control.method = (Method) 0;
    self->_.status.method = (Method) 0;
    
    return self;
}

static const void *_KLCAMSimpleThermalUnitClass;

static void
KLCAMSimpleThermalUnitClass_destroy(void)
{
    free((void *)_KLCAMSimpleThermalUnitClass);
}

static void
KLCAMSimpleThermalUnitClass_initialize(void)
{
    _KLCAMSimpleThermalUnitClass = new(__ThermalUnitClass(), "KLCAMSimpleThermalUnitClass", __ThermalUnitClass(), sizeof(struct KLCAMSimpleThermalUnitClass),
                                       ctor, "", KLCAMSimpleThermalUnitClass_ctor,
                                       (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(KLCAMSimpleThermalUnitClass_destroy);
#endif
}

const void *
KLCAMSimpleThermalUnitClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, KLCAMSimpleThermalUnitClass_initialize);
#endif
    
    return _KLCAMSimpleThermalUnitClass;
}

static const void *_KLCAMSimpleThermalUnit;

static void
KLCAMSimpleThermalUnit_destroy(void)
{
    
    free((void *)_KLCAMSimpleThermalUnit);
}

static void
KLCAMSimpleThermalUnit_initialize(void)
{
  
    _KLCAMSimpleThermalUnit = new(KLCAMSimpleThermalUnitClass(), "KLCAMSimpleThermalUnit", __ThermalUnit(), sizeof(struct KLCAMSimpleThermalUnit),
                                  ctor, "ctor", KLCAMSimpleThermalUnit_ctor,
                                  dtor, "dtor", KLCAMSimpleThermalUnit_dtor,
                                  (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(KLCAMSimpleThermalUnit_destroy);
#endif
}

const void *
KLCAMSimpleThermalUnit(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, KLCAMSimpleThermalUnit_initialize);
#endif

    return _KLCAMSimpleThermalUnit;
}

static int
KLCAMSimpleThermalUnit_turn_on(void *_self)
{
    struct KLCAMSimpleThermalUnit *self = cast(KLCAMSimpleThermalUnit(), _self);
    
    int ret = AAOS_ERROR;
    FILE *fp;
    
    if (self->turn_on_cmd != NULL && (fp = popen(self->turn_on_cmd, "r")) != NULL) {
        Pthread_mutex_lock(&self->_.mtx);
        self->_.state = THERMAL_UNIT_STATE_ON;
        Pthread_mutex_unlock(&self->_.mtx);
        pclose(fp);
    }
    
    return ret;
}

static int
KLCAMSimpleThermalUnit_turn_off(void *_self)
{
    struct KLCAMSimpleThermalUnit *self = cast(KLCAMSimpleThermalUnit(), _self);
    
    int ret = AAOS_ERROR;
    FILE *fp;
    
    if (self->turn_off_cmd != NULL && (fp = popen(self->turn_off_cmd, "r")) != NULL) {
        Pthread_mutex_lock(&self->_.mtx);
        self->_.state = THERMAL_UNIT_STATE_OFF;
        Pthread_mutex_unlock(&self->_.mtx);
        pclose(fp);
    }
    
    return ret;
}

static int
KLCAMSimpleThermalUnit_get_temperature(void *_self, double *temperature)
{
    struct KLCAMSimpleThermalUnit *self = cast(KLCAMSimpleThermalUnit(), _self);
    
    int ret = AAOS_ERROR;
    FILE *fp;
    
    *temperature = 9999.;
    if (self->temp_cmd != NULL && (fp = popen(self->temp_cmd, "r")) != NULL) {
        fscanf(fp, "%lf", temperature);
        pclose(fp);
    }
    
    return ret;
}

static void *
KLCAMSimpleThermalUnit_thermal_control(void *_self)
{
    struct KLCAMSimpleThermalUnit *self = cast(KLCAMSimpleThermalUnit(), _self);
    
    double temp, temp2, temp3, highest = self->_.highest, lowest = self->_.lowest, threshold = self->threshold;
    FILE *fp;
    
    if (self->turn_off_cmd != NULL && (fp = popen(self->turn_off_cmd, "r")) != NULL) {
        Pthread_mutex_lock(&self->_.mtx);
        self->_.state = THERMAL_UNIT_STATE_OFF;
        Pthread_mutex_unlock(&self->_.mtx);
        pclose(fp);
    }
    
    if (self->turn_off_cmd2 != NULL && (fp = popen(self->turn_off_cmd2, "r")) != NULL) {
        Pthread_mutex_lock(&self->mtx);
        self->state = THERMAL_UNIT_STATE_OFF;
        Pthread_mutex_unlock(&self->mtx);
        pclose(fp);
    }
    
    for (;;) {
        temp = 9999.;
        if (self->temp_cmd != NULL && (fp = popen(self->temp_cmd, "r")) != NULL) {
            fscanf(fp, "%lf", &temp);
            pclose(fp);
        }
        temp2 = 9999.;
        if (self->temp_cmd2 != NULL && (fp = popen(self->temp_cmd2, "r")) != NULL) {
            fscanf(fp, "%lf", &temp2);
            pclose(fp);
        }
        temp3 = 9999.;
        if (self->temp_cmd3 != NULL && (fp = popen(self->temp_cmd3, "r")) != NULL) {
            fscanf(fp, "%lf", &temp3);
            pclose(fp);
        }
        if (temp == 9999. && temp2 != 9999.) {
            temp = temp2;
        }
        
        /*
         * internal heater(s)
         */
        if (temp < lowest && self->_.state == THERMAL_UNIT_STATE_OFF) {
            if (self->turn_on_cmd != NULL && (fp = popen(self->turn_on_cmd, "r")) != NULL) {
                Pthread_mutex_lock(&self->_.mtx);
                self->_.state = THERMAL_UNIT_STATE_ON;
                Pthread_mutex_unlock(&self->_.mtx);
                pclose(fp);
            }
        } else if (temp > highest && self->state == THERMAL_UNIT_STATE_ON) {
            if (self->turn_off_cmd != NULL && (fp = popen(self->turn_off_cmd, "r")) != NULL) {
                Pthread_mutex_lock(&self->_.mtx);
                self->_.state = THERMAL_UNIT_STATE_OFF;
                Pthread_mutex_unlock(&self->_.mtx);
                pclose(fp);
            }
        }
        /*
         * external heater(s)
         */
        if (temp3 < threshold && self->state == THERMAL_UNIT_STATE_OFF) {
            if (self->turn_on_cmd2 != NULL && (fp = popen(self->turn_on_cmd2, "r")) != NULL) {
                Pthread_mutex_lock(&self->mtx);
                self->state = THERMAL_UNIT_STATE_ON;
                Pthread_mutex_unlock(&self->mtx);
                pclose(fp);
            }
        } else if (temp3 > threshold && self->state == THERMAL_UNIT_STATE_ON) {
            if (self->turn_off_cmd != NULL && (fp = popen(self->turn_off_cmd, "r")) != NULL) {
                Pthread_mutex_lock(&self->mtx);
                self->state = THERMAL_UNIT_STATE_OFF;
                Pthread_mutex_unlock(&self->mtx);
                pclose(fp);
            }
        }
        
        Nanosleep(self->_.period);
    }
    
    return NULL;
}

static int
KLCAMSimpleThermalUnit_status(void *_self, void *buffer, size_t size, size_t *res_len)
{
    struct KLCAMSimpleThermalUnit *self = cast(KLCAMSimpleThermalUnit(), _self);
    
    uint16_t status, status2;
    
    Pthread_mutex_lock(&self->mtx);
    status2 = self->state;
    Pthread_mutex_unlock(&self->mtx);
    
    Pthread_mutex_lock(&self->_.mtx);
    status2 = self->_.state;
    Pthread_mutex_unlock(&self->_.mtx);
    
    if (size < 4) {
        return AAOS_ENOMEM;
    }
    
    memcpy(buffer, &status, sizeof(uint16_t));
    memcpy((char *) buffer + sizeof(uint16_t), &status2, sizeof(uint16_t));
    
    if (res_len != NULL) {
        *res_len = 2 * sizeof(uint16_t);
    }
    
    return AAOS_OK;
}

static const void *_klcam_simple_thermal_unit_virtual_table;

static void
klcam_simple_thermal_unit_virtual_table_destroy(void)
{
    delete((void *) _klcam_simple_thermal_unit_virtual_table);
}

static void
klcam_simple_thermal_unit_virtual_table_initialize(void)
{
    _klcam_simple_thermal_unit_virtual_table = new(__ThermalUnitVirtualTable(),
                                                   __thermal_unit_turn_on, "turn_on", KLCAMSimpleThermalUnit_turn_on,
                                                   __thermal_unit_turn_off, "turn_off", KLCAMSimpleThermalUnit_turn_off,
                                                   __thermal_unit_status, "status", KLCAMSimpleThermalUnit_status,
                                                   __thermal_unit_get_temperature, "get_temperature", KLCAMSimpleThermalUnit_get_temperature,
                                                   __thermal_unit_thermal_control, "thermal_control", KLCAMSimpleThermalUnit_thermal_control,
                                                   (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(klcam_simple_thermal_unit_virtual_table_destroy);
#endif
}

static const void *
klcam_simple_thermal_unit_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, klcam_simple_thermal_unit_virtual_table_initialize);
#endif
    
    return _klcam_simple_thermal_unit_virtual_table;
}

/*
 * KLCAM thermal control
 */

/*
static const void *klcam_thermal_unit_virtual_table(void);

static void *
KLCAMThermalUnit_ctor(void *_self, va_list *app)
{
    struct KLCAMThermalUnit *self = super_ctor(KLCAMThermalUnit(), _self, app);
    
    self->_._vtab= klcam_thermal_unit_virtual_table();
    
    char *s;
    
    s = va_arg(*app, char *);
    if (s) {
        self->external_addr = (char *) Malloc(strlen(s) + 1);
        snprintf(self->external_addr, strlen(s) + 1, "%s", s);
    }
    s = va_arg(*app, char *);
    if (s) {
        self->external_port = (char *) Malloc(strlen(s) + 1);
        snprintf(self->external_port, strlen(s) + 1, "%s", s);
    }
    s = va_arg(*app, char *);
    if (s) {
        self->external_aws = (char *) Malloc(strlen(s) + 1);
        snprintf(self->external_aws, strlen(s) + 1, "%s", s);
    }
    s = va_arg(*app, char *);
    if (s) {
        self->external_sensor = (char *) Malloc(strlen(s) + 1);
        snprintf(self->external_sensor, strlen(s) + 1, "%s", s);
    }
    if (s) {
        self->external_addr2 = (char *) Malloc(strlen(s) + 1);
        snprintf(self->external_addr2, strlen(s) + 1, "%s", s);
    }
    s = va_arg(*app, char *);
    if (s) {
        self->external_port2 = (char *) Malloc(strlen(s) + 1);
        snprintf(self->external_port2, strlen(s) + 1, "%s", s);
    }
    s = va_arg(*app, char *);
    if (s) {
        self->external_switch = (char *) Malloc(strlen(s) + 1);
        snprintf(self->external_switch, strlen(s) + 1, "%s", s);
    }
    s = va_arg(*app, char *);
    if (s) {
        self->external_channel = (char *) Malloc(strlen(s) + 1);
        snprintf(self->external_channel, strlen(s) + 1, "%s", s);
    }
    s = va_arg(*app, char *);
    if (s) {
        self->internal_addr = (char *) Malloc(strlen(s) + 1);
        snprintf(self->internal_addr, strlen(s) + 1, "%s", s);
    }
    s = va_arg(*app, char *);
    if (s) {
        self->internal_port = (char *) Malloc(strlen(s) + 1);
        snprintf(self->internal_port, strlen(s) + 1, "%s", s);
    }
    s = va_arg(*app, char *);
    if (s) {
        self->internal_aws = (char *) Malloc(strlen(s) + 1);
        snprintf(self->internal_aws, strlen(s) + 1, "%s", s);
    }
    s = va_arg(*app, char *);
    if (s) {
        self->internal_sensor = (char *) Malloc(strlen(s) + 1);
        snprintf(self->internal_sensor, strlen(s) + 1, "%s", s);
    }
    s = va_arg(*app, char *);
    if (s) {
        self->internal_addr2 = (char *) Malloc(strlen(s) + 1);
        snprintf(self->internal_addr2, strlen(s) + 1, "%s", s);
    }
    s = va_arg(*app, char *);
    if (s) {
        self->internal_port2 = (char *) Malloc(strlen(s) + 1);
        snprintf(self->internal_port2, strlen(s) + 1, "%s", s);
    }
    s = va_arg(*app, char *);
    if (s) {
        self->internal_switch = (char *) Malloc(strlen(s) + 1);
        snprintf(self->internal_aws, strlen(s) + 1, "%s", s);
    }
    s = va_arg(*app, char *);
    if (s) {
        self->internal_sensor = (char *) Malloc(strlen(s) + 1);
        snprintf(self->internal_channel, strlen(s) + 1, "%s", s);
    }
    
    return (void *) self;
}

static void *
KLCAMThermalUnit_dtor(void *_self)
{
    struct KLCAMThermalUnit *self = cast(KLCAMThermalUnit(), _self);
    
    free(self->external_addr);
    free(self->external_port);
    free(self->external_aws);
    free(self->external_sensor);
    free(self->external_addr2);
    free(self->external_port2);
    free(self->external_switch);
    free(self->external_channel);
    free(self->internal_addr);
    free(self->internal_port);
    free(self->internal_aws);
    free(self->internal_sensor);
    free(self->internal_addr2);
    free(self->internal_port2);
    free(self->internal_switch);
    free(self->internal_channel);
    
    return super_dtor(KLCAMThermalUnit(), _self);
}

static void *
KLCAMThermalUnitClass_ctor(void *_self, va_list *app)
{
    struct KLCAMThermalUnitClass *self = super_ctor(KLCAMThermalUnitClass(), _self, app);
    
    self->_.get_temperature.method = (Method) 0;
    self->_.turn_on.method = (Method) 0;
    self->_.turn_off.method = (Method) 0;
    
    return self;
}

static const void *_KLCAMThermalUnitClass;

static void
KLCAMThermalUnitClass_destroy(void)
{
    free((void *)_KLCAMThermalUnitClass);
}

static void
KLCAMThermalUnitClass_initialize(void)
{
    _KLCAMThermalUnitClass = new(ThermalUnitClass(), "KLCAMThermalUnitClass", ThermalUnitClass(), sizeof(struct KLCAMThermalUnitClass),
                                 ctor, "", KLCAMThermalUnitClass_ctor,
                                 (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(KLCAMThermalUnitClass_destroy);
#endif
}

const void *
KLCAMThermalUnitClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, KLCAMThermalUnitClass_initialize);
#endif
    
    return _KLCAMThermalUnitClass;
}

static const void *_KLCAMThermalUnit;

static void
KLCAMThermalUnit_destroy(void)
{
    
    free((void *)_KLCAMThermalUnit);
}

static void
KLCAMThermalUnit_initialize(void)
{
  
    _KLCAMThermalUnit = new(KLCAMThermalUnitClass(), "KLCAMThermalUnit", ThermalUnit(), sizeof(struct KLCAMThermalUnit),
                            ctor, "ctor", KLCAMThermalUnit_ctor,
                            dtor, "dtor", KLCAMThermalUnit_dtor,
                            (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(KLCAMThermalUnit_destroy);
#endif
}

const void *
KLCAMThermalUnit(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, KLCAMThermalUnit_initialize);
#endif

    return _KLCAMThermalUnit;
}

static int
KLCAMThermalUnit_turn_on(void *_self)
{
    struct KLCAMThermalUnit *self = cast(KLCAMThermalUnit(), _self);
    
    if (self->internal_addr2 == NULL && self->internal_port2 == NULL) {
        
        FILE *fp;
        if (self->internal_switch != NULL && (fp = popen(self->internal_switch, "r")) != NULL) {
            pclose(fp);
        }
    }
    
    if (self->external_addr2 == NULL && self->external_port2 == NULL) {
        
        FILE *fp;
        if (self->external_switch != NULL && (fp = popen(self->external_switch, "r")) != NULL) {
            pclose(fp);
        }
    }
    
    return AAOS_OK;
}

static int
KLCAMThermalUnit_turn_off(void *_self)
{
    struct KLCAMThermalUnit *self = cast(KLCAMThermalUnit(), _self);
    
    if (self->internal_addr2 == NULL && self->internal_port2 == NULL) {
       
        FILE *fp;
        if (self->internal_channel != NULL && (fp = popen(self->internal_channel, "r")) != NULL) {
            pclose(fp);
        }
    }
    
    if (self->external_addr2 == NULL && self->external_port2 == NULL) {
       
        FILE *fp;
        if (self->external_channel != NULL && (fp = popen(self->external_channel, "r")) != NULL) {
            pclose(fp);
        }
    }
    
    return AAOS_OK;
}

static int
KLCAMThermalUnit_get_temperature(void *_self, double *temperature)
{
    
    struct KLCAMThermalUnit *self = cast(KLCAMThermalUnit(), _self);
    
    char command[COMMANDSIZE];
    FILE fp;
    
    snprintf(command, COMMANDSIZE, "ssh %s@%s '%s'", self->internal_port, self->internal_addr, self->internal_aws);
    
    return 0;
}

static const void *_klcam_thermal_unit_virtual_table;

static void
klcam_thermal_unit_virtual_table_destroy(void)
{
    delete((void *) _klcam_thermal_unit_virtual_table);
}

static void
klcam_thermal_unit_virtual_table_initialize(void)
{
    _klcam_thermal_unit_virtual_table = new(ThermalUnitVirtualTable(),
                                            thermal_unit_turn_on, "turn_on", KLCAMThermalUnit_turn_on,
                                            thermal_unit_turn_off, "turn_off", KLCAMThermalUnit_turn_off,
                                            thermal_unit_get_temperature, "get_temperature", KLCAMThermalUnit_get_temperature,
                                            (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(klcam_thermal_unit_virtual_table_destroy);
#endif
}

static const void *
klcam_thermal_unit_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, klcam_thermal_unit_virtual_table_initialize);
#endif
    
    return _klcam_thermal_unit_virtual_table;
}
*/
