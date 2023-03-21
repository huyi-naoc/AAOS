//
//  thermal.c
//  AAOS
//
//  Created by Hu Yi on 2023/2/25.
//  Copyright © 2023 NAOC. All rights reserved.
//

#include "def.h"
#include "thermal.h"
#include "thermal_r.h"
#include "virtual.h"
#include "wrapper.h"

/*
 * ThermalUnit virtual table.
 */
static void *
ThermalUnitVirtualTable_ctor(void *_self, va_list *app)
{
    struct ThermalUnitVirtualTable *self = super_ctor(ThermalUnitVirtualTable(), _self, app);
    Method selector;
    
    while ((selector = va_arg(*app, Method))) {
        const char *tag = va_arg(*app, const char *);
        Method method = va_arg(*app, Method);
        
        if (selector == (Method) thermal_unit_turn_on) {
            if (tag) {
                self->turn_on.tag = tag;
                self->turn_on.selector = selector;
            }
            self->turn_on.method = method;
            continue;
        }
        
        if (selector == (Method) thermal_unit_turn_off) {
            if (tag) {
                self->turn_off.tag = tag;
                self->turn_off.selector = selector;
            }
            self->turn_off.method = method;
            continue;
        }
        
        if (selector == (Method) thermal_unit_get_temperature) {
            if (tag) {
                self->get_temperature.tag = tag;
                self->get_temperature.selector = selector;
            }
            self->get_temperature.method = method;
            continue;
        }
        
        if (selector == (Method) thermal_unit_thermal_control) {
            if (tag) {
                self->get_temperature.tag = tag;
                self->get_temperature.selector = selector;
            }
            self->get_temperature.method = method;
            continue;
        }
    }
    
    return _self;
}

static void *
ThermalUnitVirtualTable_dtor(void *_self)
{
    return super_dtor(Object(), _self);
}

static const void *_ThermalUnitVirtualTable;

static void
ThermalUnitVirtualTable_destroy(void)
{
    free((void *) _ThermalUnitVirtualTable);
}

static void
ThermalUnitVirtualTable_initialize(void)
{
    _ThermalUnitVirtualTable = new(VirtualTableClass(), "ThermalUnitVirtualTable", VirtualTable(), sizeof(struct ThermalUnitVirtualTable),
                                   ctor, "ctor", ThermalUnitVirtualTable_ctor,
                                   dtor, "dtor", ThermalUnitVirtualTable_dtor,
                                   (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(ThermalUnitVirtualTable_destroy);
#endif
}

const void *
ThermalUnitVirtualTable(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, ThermalUnitVirtualTable_initialize);
#endif
    
    return _ThermalUnitVirtualTable;
}

void *
thermal_unit_thermal_control(void *_self)
{
    const struct ThermalUnitClass *class = (const struct ThermalUnitClass *) classOf(_self);
    
    if (isOf(class, ThermalUnitClass()) && class->thermal_control.method) {
        return ((void * (*)(void *)) class->thermal_control.method)(_self);
    } else {
        void *result;
        forward(_self, &result, (Method) thermal_unit_thermal_control, "thermal_control", _self);
        return result;
    }
}

static void *
ThermalUnit_thermal_control(void *_self)
{
    struct ThermalUnit *self = cast(ThermalUnit(), _self);
    double temperature;
    int ret;
    for (; ;) {
        if ((ret = thermal_unit_get_temperature(_self, &temperature)) != AAOS_OK) {
            /*
             *
             */
        }
        if (temperature > self->highest && self->state == THERMAL_UNIT_STATE_ON) {
            thermal_unit_turn_off(_self);
        } else if (temperature < self->lowest && self->state == THERMAL_UNIT_STATE_OFF) {
            thermal_unit_turn_on(_self);
        }
        Nanosleep(self->period);
    }
    return NULL;
}

int
thermal_unit_turn_on(void *_self)
{
    const struct ThermalUnitClass *class = (const struct ThermalUnitClass *) classOf(_self);
    
    if (isOf(class, ThermalUnitClass()) && class->turn_on.method) {
        return ((int (*)(void *)) class->turn_on.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) thermal_unit_turn_on, "turn_on", _self);
        return result;
    }
}

int
thermal_unit_turn_off(void *_self)
{
    const struct ThermalUnitClass *class = (const struct ThermalUnitClass *) classOf(_self);
    
    if (isOf(class, ThermalUnitClass()) && class->turn_off.method) {
        return ((int (*)(void *)) class->turn_off.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) thermal_unit_turn_off, "turn_off", _self);
        return result;
    }
}

int
thermal_unit_get_temperature(const void *_self, double *temperature)
{
    const struct ThermalUnitClass *class = (const struct ThermalUnitClass *) classOf(_self);
    
    if (isOf(class, ThermalUnitClass()) && class->get_temperature.method) {
        return ((int (*)(const void *, double *)) class->turn_off.method)(_self, temperature);
    } else {
        int result;
        forward(_self, &result, (Method) thermal_unit_turn_off, "turn_off", _self, temperature);
        return result;
    }
}

static void
ThermalUnit_forward(const void *_self, void *result, Method selector, const char *name, va_list *app)
{
    const struct ThermalUnit *self = cast(ThermalUnit(), _self);
    Method method = virtualTo(self->_vtab, name);
    va_list ap;
    
#ifdef va_copy
    va_copy(ap, *app);
#else
    ap = *app;
#endif
    
    void *obj = va_arg(*app, void *);
    
    if (selector == (Method) thermal_unit_turn_on || selector == (Method) thermal_unit_turn_off) {
        *((int *) result) = ((int (*)(void *)) method)(obj);
    } else if (selector == (Method) thermal_unit_get_temperature) {
        double *temperature = va_arg(*app, double *);
        *((int *) result) = ((int (*)(const void *, double *)) method)(obj, temperature);
    } else if (selector == (Method) thermal_unit_thermal_control) {
        *((void **) result) = ((void * (*)(void *)) method)(obj);
    } else {
        assert(0);
    }
    
#ifdef va_copy
    va_end(ap);
#endif
}

/*
 * tu = new(ThermalUnit(), name, description, highest, lowest, period)
 */

static void *
ThermalUnit_ctor(void *_self, va_list *app)
{
    struct ThermalUnit *self = super_ctor(ThermalUnit(), _self, app);
    
    const char *s;
    
    s = va_arg(*app, const char *);
    if (s) {
        self->name = (char *) Malloc(strlen(s) + 1);
        snprintf(self->name, strlen(s) + 1, "%s", s);
    }
    s = va_arg(*app, const char *);
    if (s) {
        self->description = (char *) Malloc(strlen(s) + 1);
        snprintf(self->description, strlen(s) + 1, "%s", s);
    }
    self->highest = va_arg(*app, double);
    self->lowest = va_arg(*app, double);
    self->period = va_arg(*app, double);
    self->state = THERMAL_UNIT_STATE_OFF;
    
    return (void *) self;
}

static void *
ThermalUnit_dtor(void *_self)
{
    struct ThermalUnit *self = cast(ThermalUnit(), _self);
    
    free(self->name);
    free(self->description);
    
    return super_dtor(ThermalUnit(), _self);
}

static void *
ThermalUnitClass_ctor(void *_self, va_list *app)
{
    struct ThermalUnitClass *self = super_ctor(ThermalUnitClass(), _self, app);
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
        if (selector == (Method) thermal_unit_turn_on) {
            if (tag) {
                self->turn_on.tag = tag;
                self->turn_on.selector = selector;
            }
            self->turn_on.method = method;
            continue;
        }
        if (selector == (Method) thermal_unit_turn_off) {
            if (tag) {
                self->turn_off.tag = tag;
                self->turn_off.selector = selector;
            }
            self->turn_off.method = method;
            continue;
        }
        if (selector == (Method) thermal_unit_thermal_control) {
            if (tag) {
                self->thermal_control.tag = tag;
                self->thermal_control.selector = selector;
            }
            self->thermal_control.method = method;
            continue;
        }
        if (selector == (Method) thermal_unit_get_temperature) {
            if (tag) {
                self->get_temperature.tag = tag;
                self->get_temperature.selector = selector;
            }
            self->get_temperature.method = method;
            continue;
        }
    }
    
#ifdef va_copy
    va_end(ap);
#endif
    
    return (void *) self;
}

static const void *_ThermalUnitClass;

static void
ThermalUnitClass_destroy(void)
{
    free((void *) _ThermalUnitClass);
}

static void
ThermalUnitClass_initialize(void)
{
    _ThermalUnitClass = new(Class(), "ThermalUnitClass", Class(), sizeof(struct ThermalUnitClass),
                            ctor, "ctor", ThermalUnitClass_ctor,
                            (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(ThermalUnitClass_destroy);
#endif
}

const void *
ThermalUnitClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, ThermalUnitClass_initialize);
#endif
    
    return _ThermalUnitClass;
}

static const void *_ThermalUnit;

static void
ThermalUnit_destroy(void)
{
    free((void *)_ThermalUnit);
}

static void
ThermalUnit_initialize(void)
{
    _ThermalUnit = new(ThermalUnitClass(), "ThermalUnit", Object(), sizeof(struct ThermalUnit),
                       ctor, "ctor", ThermalUnit_ctor,
                       dtor, "dtor", ThermalUnit_dtor,
                       forward, "forward", ThermalUnit_forward,
                       thermal_unit_thermal_control, "thermal_control", ThermalUnit_thermal_control,
                       (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(ThermalUnit_destroy);
#endif
}

const void *
ThermalUnit(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, ThermalUnit_initialize);
#endif
    
    return _ThermalUnit;
}

/*
 * A simple KLCAM thermal control, directly using low level commands.
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
    _KLCAMSimpleThermalUnitClass = new(ThermalUnitClass(), "KLCAMSimpleThermalUnitClass", ThermalUnitClass(), sizeof(struct KLCAMSimpleThermalUnitClass),
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
  
    _KLCAMSimpleThermalUnit = new(KLCAMSimpleThermalUnitClass(), "KLCAMSimpleThermalUnit", ThermalUnit(), sizeof(struct KLCAMSimpleThermalUnit),
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
        self->_.state = THERMAL_UNIT_STATE_ON;
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
        self->_.state = THERMAL_UNIT_STATE_OFF;
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
        self->_.state = THERMAL_UNIT_STATE_OFF;
        pclose(fp);
    }
    
    if (self->turn_off_cmd2 != NULL && (fp = popen(self->turn_off_cmd2, "r")) != NULL) {
        self->state = THERMAL_UNIT_STATE_OFF;
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
                self->_.state = THERMAL_UNIT_STATE_ON;
                pclose(fp);
            }
        } else if (temp > highest && self->state == THERMAL_UNIT_STATE_ON) {
            if (self->turn_off_cmd != NULL && (fp = popen(self->turn_off_cmd, "r")) != NULL) {
                self->_.state = THERMAL_UNIT_STATE_OFF;
                pclose(fp);
            }
        }
        /*
         * external heater(s)
         */
        if (temp3 < threshold && self->state == THERMAL_UNIT_STATE_OFF) {
            if (self->turn_on_cmd2 != NULL && (fp = popen(self->turn_on_cmd2, "r")) != NULL) {
                self->state = THERMAL_UNIT_STATE_ON;
                pclose(fp);
            }
        } else if (temp3 > threshold && self->state == THERMAL_UNIT_STATE_ON) {
            if (self->turn_off_cmd != NULL && (fp = popen(self->turn_off_cmd, "r")) != NULL) {
                self->_.state = THERMAL_UNIT_STATE_OFF;
                pclose(fp);
            }
        }
        
        Nanosleep(self->_.period);
    }
    
    return NULL;
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
    _klcam_simple_thermal_unit_virtual_table = new(ThermalUnitVirtualTable(),
                                                   thermal_unit_turn_on, "turn_on", KLCAMSimpleThermalUnit_turn_on,
                                                   thermal_unit_turn_off, "turn_off", KLCAMSimpleThermalUnit_turn_off,
                                                   thermal_unit_get_temperature, "get_temperature", KLCAMSimpleThermalUnit_get_temperature,
                                                   thermal_unit_thermal_control, "thermal_control", KLCAMSimpleThermalUnit_thermal_control,
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
