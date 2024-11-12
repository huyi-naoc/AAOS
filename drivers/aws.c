//
//  aws.c
//  AAOS
//
//  Created by huyi on 2018/10/29.
//  Copyright © 2018 National Astronomical Observatories, Chinese Academy of Sciences. All rights reserved.
//

/*
 * Segmentation error, index -1 VS -2.
 */

#include "aws_def.h"
#include "aws.h"
#include "aws_r.h"
#include "def.h"
#include "protocol.h"
#include "serial_rpc.h"
#include "wrapper.h"

#define AWS_STATE_OK 0
#define AWS_STATE_ERROR 1

/*
 * Sensor virtual table.
 */
static void *
SensorVirtualTable_ctor(void *_self, va_list *app)
{
    struct SensorVirtualTable *self = super_ctor(SensorVirtualTable(), _self, app);
    Method selector;
    
    while ((selector = va_arg(*app, Method))) {
        const char *tag = va_arg(*app, const char *);
        Method method = va_arg(*app, Method);
        
        if (selector == (Method) sensor_read_data) {
            if (tag) {
                self->read_data.tag = tag;
                self->read_data.selector = selector;
            }
            self->read_data.method = method;
            continue;
        }
        if (selector == (Method) sensor_read_raw_data) {
            if (tag) {
                self->read_raw_data.tag = tag;
                self->read_raw_data.selector = selector;
            }
            self->read_raw_data.method = method;
            continue;
        }
        if (selector == (Method) sensor_format_put) {
            if (tag) {
                self->format_put.tag = tag;
                self->format_put.selector = selector;
            }
            self->format_put.method = method;
            continue;
        }
        if (selector == (Method) sensor_set_controller) {
            if (tag) {
                self->set_controller.tag = tag;
                self->set_controller.selector = selector;
            }
            self->set_controller.method = method;
            continue;
        }
        if (selector == (Method) sensor_set_device) {
            if (tag) {
                self->set_device.tag = tag;
                self->set_device.selector = selector;
            }
            self->set_device.method = method;
            continue;
        }
    }
    
    return _self;
}

static void *
SensorVirtualTable_dtor(void *_self)
{
    return super_dtor(Object(), _self);
}

static const void *_SensorVirtualTable;

static void
SensorVirtualTable_destroy(void)
{
    free((void *) _SensorVirtualTable);
}

static void
SensorVirtualTable_initialize(void)
{
    _SensorVirtualTable = new(VirtualTableClass(), "SensorVirtualTable", VirtualTable(), sizeof(struct SensorVirtualTable),
                              ctor, "ctor", SensorVirtualTable_ctor,
                              dtor, "dtor", SensorVirtualTable_dtor,
                              (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(SensorVirtualTable_destroy);
#endif
}

const void *
SensorVirtualTable(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, SensorVirtualTable_initialize);
#endif
    
    return _SensorVirtualTable;
}

/*
 * Sensor
 */

const char *
sensor_get_name(const void *_self)
{
    const struct SensorClass *class = (const struct SensorClass *) classOf(_self);
    
    if (isOf(class, SensorClass()) && class->get_name.method) {
        return ((const char * (*)(const void *)) class->get_name.method)(_self);
    } else {
        const char *result;
        forward(_self, &result, (Method) sensor_get_name, "get_name", _self);
        return result;
    }
}

static const char *
Sensor_get_name(const void *_self)
{
    struct Sensor *self = cast(Sensor(), _self);
    
    return self->name;
}

unsigned int
sensor_get_channel(const void *_self)
{
    const struct SensorClass *class = (const struct SensorClass *) classOf(_self);
    
    if (isOf(class, SensorClass()) && class->get_channel.method) {
        return ((unsigned int (*)(const void *)) class->get_channel.method)(_self);
    } else {
        unsigned int result;
        forward(_self, &result, (Method) sensor_get_channel, "get_channel", _self);
        return result;
    }
}

static unsigned int
Sensor_get_channel(const void *_self)
{
    struct Sensor *self = cast(Sensor(), _self);
    
    return self->channel;
}


void
sensor_set_channel(void *_self, unsigned int channel)
{
    const struct SensorClass *class = (const struct SensorClass *) classOf(_self);
    
    if (isOf(class, SensorClass()) && class->set_channel.method) {
        return ((void (*)(void *, unsigned int)) class->set_channel.method)(_self, channel);
    } else {
        
        forward(_self, 0, (Method) sensor_set_channel, "set_channel", _self, channel);
    }
}

static void
Sensor_set_channel(void *_self, unsigned int channel)
{
    struct Sensor *self = cast(Sensor(), _self);
    
    self->channel = channel;
}

unsigned int
sensor_get_type(const void *_self)
{
    const struct SensorClass *class = (const struct SensorClass *) classOf(_self);
    
    if (isOf(class, SensorClass()) && class->get_type.method) {
        return ((unsigned int (*)(const void *)) class->get_type.method)(_self);
    } else {
        unsigned int result;
        forward(_self, &result, (Method) sensor_get_type, "get_type", _self);
        return result;
    }
}

static unsigned int
Sensor_get_type(const void *_self)
{
    struct Sensor *self = cast(Sensor(), _self);
    
    return self->type;
}


void
sensor_set_type(void *_self, unsigned int type)
{
    const struct SensorClass *class = (const struct SensorClass *) classOf(_self);
    
    if (isOf(class, SensorClass()) && class->set_type.method) {
        return ((void (*)(void *, unsigned int)) class->set_type.method)(_self, type);
    } else {
        
        forward(_self, 0, (Method) sensor_set_type, "set_type", _self, type);
    }
}

static void
Sensor_set_type(void *_self, unsigned int type)
{
    struct Sensor *self = cast(Sensor(), _self);
    
    self->type = type;
}

void
sensor_set_controller(void *_self, const void *controller)
{
    const struct SensorClass *class = (const struct SensorClass *) classOf(_self);
    
    if (isOf(class, SensorClass()) && class->set_controller.method) {
        return ((void (*)(void *, const void *)) class->set_controller.method)(_self, controller);
    } else {
        
        forward(_self, 0, (Method) sensor_set_controller, "set_controller", _self, controller);
    }
}

static void
Sensor_set_controller(void *_self, const void *controller)
{
    struct Sensor *self = cast(Sensor(), _self);
    
    self->controller = (void *) controller;
}

void
sensor_set_device(void *_self, const void *device)
{
    const struct SensorClass *class = (const struct SensorClass *) classOf(_self);
    
    if (isOf(class, SensorClass()) && class->set_device.method) {
        return ((void (*)(void *, const void *)) class->set_device.method)(_self, device);
    } else {
        
        forward(_self, 0, (Method) sensor_set_device, "set_device", _self, device);
    }
}

static void
Sensor_set_device(void *_self, const void *device)
{
    struct Sensor *self = cast(Sensor(), _self);
    
    self->device = (void *) device;
}


void
sensor_format_put(void *_self, FILE *fp)
{
    const struct SensorClass *class = (const struct SensorClass *) classOf(_self);
    
    if (isOf(class, SensorClass()) && class->format_put.method) {
        return ((void (*)(void *, FILE *)) class->format_put.method)(_self, fp);
    } else {
        forward(_self, 0, (Method) sensor_format_put, "format_put", _self, fp);
    }
}

static void
Sensor_format_put(void *_self, FILE *fp)
{
    struct Sensor *self = cast(Sensor(), _self);
    
    double data;
    
    sensor_read_data(_self, &data, 1);
    
    if (self->format == NULL) {
        fprintf(fp, "%.2f", data);
    } else {
        fprintf(fp, self->format, data);
    }
}


int
sensor_read_data(void *_self, double *data, size_t size)
{
    const struct SensorClass *class = (const struct SensorClass *) classOf(_self);
    
    if (isOf(class, SensorClass()) && class->read_data.method) {
        return ((int (*)(void *, double *, size_t)) class->read_data.method)(_self, data, size);
    } else {
        int result;
        forward(_self, &result, (Method) sensor_read_data, "read_data", _self, data, size);       
        return result;
    }
}

int
sensor_read_raw_data(void *_self, void *data, size_t size)
{
    const struct SensorClass *class = (const struct SensorClass *) classOf(_self);
    
    if (isOf(class, SensorClass()) && class->read_raw_data.method) {
        return ((int (*)(void *, void *, size_t)) class->read_raw_data.method)(_self, data, size);
    } else {
        int result;
        forward(_self, &result, (Method) sensor_read_raw_data, "read_raw_data", _self, data, size);       
        return result;
    }
}

static void
Sensor_forward(const void *_self, void *result, Method selector, const char *name, va_list *app)
{
    const struct Sensor *self = cast(Sensor(), _self);
    Method method = virtualTo(self->_vtab, name);
    va_list ap;
    
#ifdef va_copy
    va_copy(ap, *app);
#else
    ap = *app;
#endif
    
    void *obj = va_arg(*app, void *);
    
    if (selector == (Method) sensor_read_data) {
        double *data = va_arg(*app, double *);
        size_t size = va_arg(*app, size_t);
        *((int *) result) = ((int (*)(void *, double *, size_t)) method)(obj, data, size);
    } else if (selector == (Method) sensor_set_controller) {
        void *controller = va_arg(*app, void *);
        ((void (*)(void *, void *)) method)(obj, controller);
    } else if (selector == (Method) sensor_read_raw_data) {
        void *data = va_arg(*app, void *);
        size_t size = va_arg(*app, size_t);
        *((int *) result) = ((int (*)(void *, void *, size_t)) method)(obj, data, size);
    } else if (selector == (Method) sensor_format_put) {
        FILE *fp = va_arg(*app, FILE *);
        ((void (*)(void *, FILE *)) method)(obj, fp);
    } else {
        assert(0);
    }
    
#ifdef va_copy
    va_end(ap);
#endif
}

static void *
Sensor_ctor(void *_self, va_list *app)
{
    struct Sensor *self = super_ctor(Sensor(), _self, app);
    
    const char *s, *key, *value;
    
    s = va_arg(*app, const char *);
    if (s) {
        self->name = (char *) Malloc(strlen(s) + 1);
        snprintf(self->name, strlen(s) + 1, "%s", s);
    }
    s = va_arg(*app, const void *);
    if (s) {
        self->command = (char *) Malloc(strlen(s) + 1);
        snprintf(self->command, strlen(s) + 1, "%s", s);
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
        if (strcmp(key, "model") == 0) {
            value = va_arg(*app, const char *);
            if (value) {
                self->model = (char *) Malloc(strlen(value) + 1);
                snprintf(self->model, strlen(value) + 1, "%s", value);
            }
            
            continue;
        }
        if (strcmp(key, "type") == 0) {
            value = va_arg(*app, const char *);
            if (value) {
                if (strcmp(value, "temperature") == 0) {
                    self->type = SENSOR_TYPE_TEMEPRATURE;
                } else if (strcmp(value, "wind speed") == 0 || strcmp(value, "wind_speed") == 0) {
                    self->type = SENSOR_TYPE_WIND_SPEED;
                } else if (strcmp(value, "wind direction") == 0 || strcmp(value, "wind_direction") == 0) {
                    self->type = SENSOR_TYPE_WIND_DIRECTION;
                }
            }
            continue;
        }
    }
    
    return (void *) self;
}

static void *
Sensor_dtor(void *_self)
{
    struct Sensor *self = cast(Sensor(), _self);
    
    free(self->name);
    free(self->model);
    free(self->command);
    free(self->description);

    return super_dtor(Sensor(), _self);
}

static void *
SensorClass_ctor(void *_self, va_list *app)
{
    struct SensorClass *self = super_ctor(SensorClass(), _self, app);
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
        if (selector == (Method) sensor_set_channel) {
            if (tag) {
                self->set_channel.tag = tag;
                self->set_channel.selector = selector;
            }
            self->set_channel.method = method;
            continue;
        }
        if (selector == (Method) sensor_get_channel) {
            if (tag) {
                self->get_channel.tag = tag;
                self->get_channel.selector = selector;
            }
            self->get_channel.method = method;
            continue;
        }
        if (selector == (Method) sensor_set_type) {
            if (tag) {
                self->set_type.tag = tag;
                self->set_type.selector = selector;
            }
            self->set_type.method = method;
            continue;
        }
        if (selector == (Method) sensor_get_type) {
            if (tag) {
                self->get_type.tag = tag;
                self->get_type.selector = selector;
            }
            self->get_type.method = method;
            continue;
        }
        if (selector == (Method) sensor_set_controller) {
            if (tag) {
                self->set_controller.tag = tag;
                self->set_controller.selector = selector;
            }
            self->set_controller.method = method;
            continue;
        }
        if (selector == (Method) sensor_set_device) {
            if (tag) {
                self->set_device.tag = tag;
                self->set_device.selector = selector;
            }
            self->set_device.method = method;
            continue;
        }
        if (selector == (Method) sensor_get_name) {
            if (tag) {
                self->get_name.tag = tag;
                self->get_name.selector = selector;
            }
            self->get_name.method = method;
            continue;
        }
        if (selector == (Method) sensor_format_put) {
            if (tag) {
                self->format_put.tag = tag;
                self->format_put.selector = selector;
            }
            self->format_put.method = method;
            continue;
        }
    }
    
#ifdef va_copy
    va_end(ap);
#endif
    
    return (void *) self;
}

static const void *_SensorClass;

static void
SensorClass_destroy(void)
{
    free((void *) _SensorClass);
}

static void
SensorClass_initialize(void)
{
    _SensorClass = new(Class(), "SensorClass", Class(), sizeof(struct SensorClass),
                        ctor, "ctor", SensorClass_ctor,
                        (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(SensorClass_destroy);
#endif
}

const void *
SensorClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, SensorClass_initialize);
#endif
    
    return _SensorClass;
}

static const void *_Sensor;

static void
Sensor_destroy(void)
{
    free((void *)_Sensor);
}

static void
Sensor_initialize(void)
{
    _Sensor = new(SensorClass(), "Sensor", Object(), sizeof(struct Sensor),
                  ctor, "ctor", Sensor_ctor,
                  dtor, "dtor", Sensor_dtor,
                  forward, "forward", Sensor_forward,
                  sensor_set_controller, "set_controller", Sensor_set_controller,
                  sensor_set_device, "set_device", Sensor_set_device,
                  sensor_get_name, "get_name", Sensor_get_name,
                  sensor_get_channel, "get_channel", Sensor_get_channel,
                  sensor_set_channel, "set_channel", Sensor_set_channel,
                  sensor_get_type, "get_type", Sensor_get_type,
                  sensor_set_type, "set_type", Sensor_set_type,
                  sensor_format_put, "format_put", Sensor_format_put,
                  (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(Sensor_destroy);
#endif
}

const void *
Sensor(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, Sensor_initialize);
#endif
    
    return _Sensor;
}

static int KLAWSController_try_connect(struct KLAWSController *self);

/*
 * PT100 temperature sensor.
 */

static const void *pt100_virtual_table(void);

static void *
PT100_ctor(void *_self, va_list *app)
{
    struct PT100 *self = super_ctor(PT100(), _self, app);
    
    const char *key;
    while ((key = va_arg(*app, const char *))) {
        if (strcmp(key, "output_type") == 0) {
            self->output_type = va_arg(*app, int);
        }
        continue;
    }

    self->_._vtab= pt100_virtual_table();
    return (void *) self;
}

static void *
PT100_dtor(void *_self)
{
    return super_dtor(PT100(), _self);
}

static void *
PT100Class_ctor(void *_self, va_list *app)
{
    struct PT100Class *self = super_ctor(PT100Class(), _self, app);
    
    self->_.read_data.method = (Method) 0;
    self->_.read_raw_data.method = (Method) 0;
    
    return self;
}

static const void *_PT100Class;

static void
PT100Class_destroy(void)
{
    free((void *)_PT100Class);
}

static void
PT100Class_initialize(void)
{
    _PT100Class = new(SensorClass(), "PT100Class", SensorClass(), sizeof(struct PT100Class),
                           ctor, "", PT100Class_ctor,
                           (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(PT100Class_destroy);
#endif
}

const void *
PT100Class(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, PT100Class_initialize);
#endif
    
    return _PT100Class;
}

static const void *_PT100;

static void
PT100_destroy(void)
{
    free((void *)_PT100);
}

static void
PT100_initialize(void)
{
    
    _PT100 = new(PT100Class(), "PT100", Sensor(), sizeof(struct PT100),
                     ctor, "ctor", PT100_ctor,
                     dtor, "dtor", PT100_dtor,
                     (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(PT100_destroy);
#endif
}

const void *
PT100(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, PT100_initialize);
#endif

    return _PT100;
}

#ifdef __USE_GSL__
static const double PT100_resistance[] = {
    18.52, 18.95, 19.38, 19.82, 20.25, 20.68, 21.11, 21.54, 21.97, 22.40,
    22.83, 23.25, 23.68, 24.11, 24.54, 24.97, 25.39, 25.82, 26.24, 26.67,
    27.10, 27.52, 27.95, 28.37, 28.80, 29.22, 29.64, 30.07, 30.49, 30.91,
    31.34, 31.76, 32.18, 32.60, 33.02, 33.44, 33.86, 34.28, 34.70, 35.12,
    35.54, 35.96, 36.38, 36.80, 37.22, 37.64, 38.05, 38.47, 38.89, 39.31,
    39.72, 40.14, 40.56, 40.97, 41.39, 41.80, 42.22, 42.63, 43.05, 43.46,
    43.88, 44.29, 44.70, 45.12, 45.53, 45.94, 46.36, 46.77, 47.18, 47.59,
    48.00, 48.42, 48.83, 49.24, 49.65, 50.06, 50.47, 50.88, 51.29, 51.70,
    52.11, 52.52, 52.93, 53.34, 53.75, 54.15, 54.56, 54.97, 55.38, 55.79,
    56.19, 56.60, 57.01, 57.41, 57.82, 58.23, 58.63, 59.04, 59.44, 59.85,
    60.26, 60.66, 61.07, 61.47, 61.88, 62.28, 62.68, 63.09, 63.49, 63.90,
    64.30, 64.70, 65.11, 65.51, 65.91, 66.31, 66.72, 67.12, 67.52, 67.92,
    68.33, 68.73, 69.13, 69.53, 69.93, 70.33, 70.73, 71.13, 71.53, 71.93,
    72.33, 72.73, 73.13, 73.53, 73.93, 74.33, 74.73, 75.13, 75.53, 75.93,
    76.33, 76.73, 77.13, 77.52, 77.92, 78.32, 78.72, 79.11, 79.51, 79.91,
    80.31, 80.70, 81.10, 81.50, 81.89, 82.29, 82.69, 83.08, 83.48, 83.88,
    84.27, 84.67, 85.06, 85.46, 85.85, 86.25, 86.64, 87.04, 87.43, 87.83,
    88.22, 88.62, 89.01, 89.40, 89.80, 90.19, 90.59, 90.93, 91.37, 91.77,
    92.16, 92.55, 92.95, 93.34, 93.73, 94.12, 94.52, 94.91, 95.30, 95.69,
    96.09, 96.48, 96.87, 97.26, 97.65, 98.04, 98.44, 98.83, 99.22, 99.61,
    100.00, 100.39, 100.78, 101.17, 101.56, 101.95, 102.34, 102.73, 103.12, 103.51,
    103.90, 104.29, 104.68, 105.07, 105.46, 105.85, 106.24, 106.63, 107.02, 107.40,
    107.79, 108.18, 108.75, 108.96, 109.35, 109.73, 110.12, 110.51, 110.90, 111.28,
    111.67, 112.06, 112.45, 112.83, 113.22, 113.61, 113.99, 114.38, 114.77, 115.15,
    115.54, 115.93, 116.31, 116.70, 117.08, 117.47, 117.85, 118.24, 118.62, 119.01,
    119.40, 119.78, 120.16, 120.55, 120.93, 121.32, 121.70, 122.09, 122.47, 122.86,
    123.24, 123.62, 124.01, 124.39, 124.77, 125.16, 125.54, 125.92, 126.31, 126.69,
    127.07, 127.45, 127.84, 128.22, 128.60, 128.98, 129.37, 129.75, 130.13, 130.51,
    130.89, 131.27, 131.66, 132.04, 132.42, 132.80, 133.18, 133.56, 133.94, 134.32,
    134.70, 135.08, 135.46, 135.84, 136.22, 136.60, 136.98, 137.36, 137.74, 138.12,
    138.50, 138.88, 139.26, 139.64, 140.02, 140.39, 140.77, 141.15, 141.53, 141.91,
    142.29, 142.66, 143.04, 143.42, 143.80, 144.17, 144.55, 144.93, 145.31, 145.68,
    146.06, 146.44, 146.81, 147.19, 147.57, 147.94, 148.32, 148.70, 149.07, 149.45,
    149.82, 150.20, 150.57, 150.95, 151.33, 151.70, 152.08, 152.45, 152.83, 153.20,
    153.58, 153.95, 154.32, 154.70, 155.07, 155.45, 155.82, 156.19, 156.57, 156.94,
    157.31, 157.69, 158.06, 158.43, 158.81, 159.18, 159.55, 159.93, 160.30, 160.67,
    161.04, 161.42, 161.79, 162.16, 162.53, 162.90, 163.27, 163.65, 164.02, 164.39,
    164.76, 165.13, 165.50, 165.87, 166.24, 166.61, 166.98, 167.35, 167.72, 168.09,
    168.46, 168.83, 169.20, 169.57, 169.94, 170.31, 170.68, 171.05, 171.42, 171.79,
    172.16, 172.53, 172.90, 173.26, 173.63, 174.00, 174.37, 174.74, 175.10, 175.47,
    175.84, 176.21, 176.57, 176.94, 177.31, 177.68, 178.04, 178.41, 178.78, 179.14,
    179.51, 179.88, 180.24, 180.61, 180.97, 181.34, 181.71, 182.07, 182.44, 182.80,
    183.17, 183.53, 183.90, 184.26, 184.63, 184.99, 185.36, 185.72, 186.09, 186.45,
    186.82, 187.18, 187.54, 187.91, 188.27, 188.63, 189.00, 189.36, 189.72, 190.09,
    190.45, 190.81, 191.18, 191.54, 191.90, 192.26, 192.63, 192.99, 193.35, 193.71,
    194.07, 194.44, 194.80, 195.16, 195.52, 195.88, 196.24, 196.60, 196.96, 197.33,
    197.69, 198.05, 198.41, 198.77, 199.13, 199.49, 199.85, 200.21, 200.57, 200.93,
    201.29, 201.65, 202.01, 202.36, 202.72, 203.08, 203.44, 203.80, 204.16, 204.52,
    204.88, 205.23, 205.59, 205.95, 206.31, 206.67, 207.02, 207.38, 207.74, 208.10,
    208.45, 208.81, 209.17, 209.52, 209.88, 210.24, 210.59, 210.95, 211.31, 211.66,
    212.02, 212.37, 212.73, 213.09, 213.44, 213.80, 214.15, 214.51, 214.86, 215.22,
    215.57, 215.93, 216.28, 216.64, 216.99, 217.35, 217.70, 218.05, 218.41, 218.76,
    219.12, 219.47, 219.82, 220.18, 220.53, 220.88, 221.24, 221.59, 221.94, 222.29,
    222.65, 223.00, 223.35, 223.70, 224.06, 224.41, 224.76, 225.11, 225.46, 225.81,
    226.17, 226.52, 226.87, 227.22, 227.57, 227.92, 228.27, 228.62, 228.97, 229.32,
    229.62, 230.02, 230.42, 230.82, 231.22, 231.62, 232.02, 232.42, 232.82, 233.01,
    233.17, 233.52, 233.87, 234.22, 234.56, 234.91, 235.26, 235.61, 235.96, 236.31,
    236.65, 237.00, 237.35, 237.70, 238.04, 238.39, 238.74, 239.09, 239.45, 239.78,
    240.13, 240.47, 240.82, 241.17, 241.51, 241.86, 242.20, 242.55, 242.90, 243.24,
    243.59, 243.93, 244.28, 244.62, 244.97, 245.31, 245.66, 246.00, 246.35, 246.69,
    247.04, 247.38, 247.73, 248.07, 248.41, 248.76, 249.10, 249.45, 249.79, 250.13,
    250.48, 250.82, 251.16, 251.50, 251.85, 252.19, 252.53, 252.87, 253.22, 253.56,
    253.90, 254.24, 254.59, 254.93, 255.27, 255.61, 255.95, 256.29, 256.64, 256.98,
    257.32, 257.66, 258.00, 258.34, 258.68, 259.02, 259.36, 259.70, 260.04, 260.38,
    260.72, 261.06, 261.40, 261.74, 262.08, 262.42, 262.76, 263.10, 263.43, 263.77,
    264.11, 264.45, 264.79, 265.13, 265.47, 265.80, 266.14, 266.48, 266.82, 267.15,
    267.49, 267.83, 268.17, 268.50, 268.84, 269.18, 269.51, 269.85, 270.19, 270.52,
    270.86, 271.20, 271.53, 271.87, 272.20, 272.54, 272.88, 273.21, 273.55, 273.88,
    274.22, 274.55, 274.89, 275.22, 275.56, 275.89, 276.23, 276.56, 276.89, 277.23,
    277.56, 277.90, 278.23, 278.56, 278.90, 279.23, 279.56, 279.90, 280.23, 280.56,
    280.90, 281.23, 281.56, 281.89, 282.23, 282.56, 282.89, 283.22, 283.55, 283.89,
    284.22, 284.55, 284.88, 285.21, 285.54, 285.87, 286.21, 286.54, 286.87, 287.20,
    287.53, 287.86, 288.19, 288.52, 288.85, 289.18, 289.51, 289.84, 290.17, 290.50,
    290.83, 291.16, 291.49, 291.81, 292.14, 292.47, 292.80, 293.13, 293.46, 293.79,
    294.11, 294.44, 294.77, 295.10, 295.43, 295.75, 296.08, 296.41, 296.74, 297.06,
    297.39, 297.72, 298.04, 298.37, 298.70, 299.02, 299.35, 299.68, 300.00, 300.33,
    300.65, 300.98, 301.31, 301.63, 301.96, 302.28, 302.61, 302.93, 303.26, 303.58,
    303.91, 304.23, 304.56, 304.88, 305.20, 305.53, 305.85, 306.18, 306.50, 306.82,
    307.15, 307.47, 307.79, 308.12, 308.44, 308.76, 309.09, 309.41, 309.73, 310.05,
    310.38, 310.70, 311.02, 311.34, 311.67, 311.99, 312.31, 312.63, 312.95, 313.27,
    313.59, 313.92, 314.24, 314.56, 314.88, 315.20, 315.52, 315.84, 316.16, 316.48,
    316.80, 317.12, 317.44, 317.76, 318.08, 318.40, 318.72, 319.04, 319.36, 319.68,
    319.99, 320.31, 320.63, 320.95, 321.27, 321.59, 321.91, 322.22, 322.54, 322.86,
    323.18, 323.49, 323.81, 324.13, 324.45, 324.76, 325.08, 325.40, 325.72, 326.03,
    326.35, 326.66, 326.98, 327.30, 327.61, 327.93, 328.25, 328.56, 328.88, 329.19,
    329.51, 329.82, 330.14, 330.45, 330.77, 331.08, 331.40, 331.71, 332.03, 332.34,
    332.66, 332.97, 333.28, 333.60, 333.91, 334.23, 334.54, 334.85, 335.17, 335.48,
    335.79, 336.11, 336.42, 336.73, 337.04, 337.36, 337.67, 337.98, 338.29, 338.61,
    338.92, 339.23, 339.54, 339.85, 340.16, 340.48, 340.79, 341.10, 341.41, 341.72,
    342.03, 342.34, 342.65, 342.96, 343.27, 343.58, 343.89, 344.20, 344.51, 344.82,
    345.13, 345.44, 345.75, 346.06, 346.37, 346.68, 346.99, 347.30, 347.60, 347.91,
    348.22, 348.53, 348.84, 349.15, 349.45, 349.76, 350.07, 350.38, 350.69, 350.99,
    351.30, 351.61, 351.91, 352.22, 352.53, 352.83, 353.14, 353.45, 353.75, 354.06,
    354.37, 354.67, 354.98, 355.28, 355.59, 355.90, 356.20, 356.51, 356.81, 357.12,
    357.42, 357.73, 358.03, 358.34, 358.64, 358.95, 359.25, 359.55, 359.86, 360.16,
    360.47, 360.77, 361.07, 361.38, 361.68, 361.98, 362.29, 362.59, 362.89, 363.19,
    363.50, 363.80, 364.10, 364.40, 364.71, 365.01, 365.31, 365.61, 365.91, 366.22,
    366.52, 366.82, 367.12, 367.42, 367.72, 368.02, 368.32, 368.63, 368.93, 369.23,
    369.53, 369.83, 370.13, 370.43, 370.73, 371.03, 371.33, 371.63, 371.93, 372.22,
    372.52, 372.82, 373.12, 373.42, 373.72, 374.02, 374.32, 374.61, 374.91, 375.21,
    375.51, 375.81, 376.10, 376.40, 376.70, 377.00, 377.29, 377.59, 377.89, 378.19
};

static const double PT100_temperature[] = {
    -200.00, -199.00, -198.00, -197.00, -196.00, -195.00, -194.00, -193.00, -192.00, -191.00,
    -190.00, -189.00, -188.00, -187.00, -186.00, -185.00, -184.00, -183.00, -182.00, -181.00,
    -180.00, -179.00, -178.00, -177.00, -176.00, -175.00, -174.00, -173.00, -172.00, -171.00,
    -170.00, -169.00, -168.00, -167.00, -166.00, -165.00, -164.00, -163.00, -162.00, -161.00,
    -160.00, -159.00, -158.00, -157.00, -156.00, -155.00, -154.00, -153.00, -152.00, -151.00,
    -150.00, -149.00, -148.00, -147.00, -146.00, -145.00, -144.00, -143.00, -142.00, -141.00,
    -140.00, -139.00, -138.00, -137.00, -136.00, -135.00, -134.00, -133.00, -132.00, -131.00,
    -130.00, -129.00, -128.00, -127.00, -126.00, -125.00, -124.00, -123.00, -122.00, -121.00,
    -120.00, -119.00, -118.00, -117.00, -116.00, -115.00, -114.00, -113.00, -112.00, -111.00,
    -110.00, -109.00, -108.00, -107.00, -106.00, -105.00, -104.00, -103.00, -102.00, -101.00,
    -100.00, -99.00, -98.00, -97.00, -96.00, -95.00, -94.00, -93.00, -92.00, -91.00,
    -90.00, -89.00, -88.00, -87.00, -86.00, -85.00, -84.00, -83.00, -82.00, -81.00,
    -80.00, -79.00, -78.00, -77.00, -76.00, -75.00, -74.00, -73.00, -72.00, -71.00,
    -70.00, -69.00, -68.00, -67.00, -66.00, -65.00, -64.00, -63.00, -62.00, -61.00,
    -60.00, -59.00, -58.00, -57.00, -56.00, -55.00, -54.00, -53.00, -52.00, -51.00,
    -50.00, -49.00, -48.00, -47.00, -46.00, -45.00, -44.00, -43.00, -42.00, -41.00,
    -40.00, -39.00, -38.00, -37.00, -36.00, -35.00, -34.00, -33.00, -32.00, -31.00,
    -30.00, -29.00, -28.00, -27.00, -26.00, -25.00, -24.00, -23.00, -22.00, -21.00,
    -20.00, -19.00, -18.00, -17.00, -16.00, -15.00, -14.00, -13.00, -12.00, -11.00,
    -10.00, -9.00, -8.00, -7.00, -6.00, -5.00, -4.00, -3.00, -2.00, -1.00,
    0.00, 1.00, 2.00, 3.00, 4.00, 5.00, 6.00, 7.00, 8.00, 9.00,
    10.00, 11.00, 12.00, 13.00, 14.00, 15.00, 16.00, 17.00, 18.00, 19.00,
    20.00, 21.00, 22.00, 23.00, 24.00, 25.00, 26.00, 27.00, 28.00, 29.00,
    30.00, 31.00, 32.00, 33.00, 34.00, 35.00, 36.00, 37.00, 38.00, 39.00,
    40.00, 41.00, 42.00, 43.00, 44.00, 45.00, 46.00, 47.00, 48.00, 49.00,
    50.00, 51.00, 52.00, 53.00, 54.00, 55.00, 56.00, 57.00, 58.00, 59.00,
    60.00, 61.00, 62.00, 63.00, 64.00, 65.00, 66.00, 67.00, 68.00, 69.00,
    70.00, 71.00, 72.00, 73.00, 74.00, 75.00, 76.00, 77.00, 78.00, 79.00,
    80.00, 81.00, 82.00, 83.00, 84.00, 85.00, 86.00, 87.00, 88.00, 89.00,
    90.00, 91.00, 92.00, 93.00, 94.00, 95.00, 96.00, 97.00, 98.00, 99.00,
    100.00, 101.00, 102.00, 103.00, 104.00, 105.00, 106.00, 107.00, 108.00, 109.00,
    110.00, 111.00, 112.00, 113.00, 114.00, 115.00, 116.00, 117.00, 118.00, 119.00,
    120.00, 121.00, 122.00, 123.00, 124.00, 125.00, 126.00, 127.00, 128.00, 129.00,
    130.00, 131.00, 132.00, 133.00, 134.00, 135.00, 136.00, 137.00, 138.00, 139.00,
    140.00, 141.00, 142.00, 143.00, 144.00, 145.00, 146.00, 147.00, 148.00, 149.00,
    150.00, 151.00, 152.00, 153.00, 154.00, 155.00, 156.00, 157.00, 158.00, 159.00,
    160.00, 161.00, 162.00, 163.00, 164.00, 165.00, 166.00, 167.00, 168.00, 169.00,
    170.00, 171.00, 172.00, 173.00, 174.00, 175.00, 176.00, 177.00, 178.00, 179.00,
    180.00, 181.00, 182.00, 183.00, 184.00, 185.00, 186.00, 187.00, 188.00, 189.00,
    190.00, 191.00, 192.00, 193.00, 194.00, 195.00, 196.00, 197.00, 198.00, 199.00,
    200.00, 201.00, 202.00, 203.00, 204.00, 205.00, 206.00, 207.00, 208.00, 209.00,
    210.00, 211.00, 212.00, 213.00, 214.00, 215.00, 216.00, 217.00, 218.00, 219.00,
    220.00, 221.00, 222.00, 223.00, 224.00, 225.00, 226.00, 227.00, 228.00, 229.00,
    230.00, 231.00, 232.00, 233.00, 234.00, 235.00, 236.00, 237.00, 238.00, 239.00,
    240.00, 241.00, 242.00, 243.00, 244.00, 245.00, 246.00, 247.00, 248.00, 249.00,
    250.00, 251.00, 252.00, 253.00, 254.00, 255.00, 256.00, 257.00, 258.00, 259.00,
    260.00, 261.00, 262.00, 263.00, 264.00, 265.00, 266.00, 267.00, 268.00, 269.00,
    270.00, 271.00, 272.00, 273.00, 274.00, 275.00, 276.00, 277.00, 278.00, 279.00,
    280.00, 281.00, 282.00, 283.00, 284.00, 285.00, 286.00, 287.00, 288.00, 289.00,
    290.00, 291.00, 292.00, 293.00, 294.00, 295.00, 296.00, 297.00, 298.00, 299.00,
    300.00, 301.00, 302.00, 303.00, 304.00, 305.00, 306.00, 307.00, 308.00, 309.00,
    310.00, 311.00, 312.00, 313.00, 314.00, 315.00, 316.00, 317.00, 318.00, 319.00,
    320.00, 321.00, 322.00, 323.00, 324.00, 325.00, 326.00, 327.00, 328.00, 329.00,
    330.00, 331.00, 332.00, 333.00, 334.00, 335.00, 336.00, 337.00, 338.00, 339.00,
    340.00, 341.00, 342.00, 343.00, 344.00, 345.00, 346.00, 347.00, 348.00, 349.00,
    350.00, 351.00, 352.00, 353.00, 354.00, 355.00, 356.00, 357.00, 358.00, 359.00,
    360.00, 361.00, 362.00, 363.00, 364.00, 365.00, 366.00, 367.00, 368.00, 369.00,
    370.00, 371.00, 372.00, 373.00, 374.00, 375.00, 376.00, 377.00, 378.00, 379.00,
    380.00, 381.00, 382.00, 383.00, 384.00, 385.00, 386.00, 387.00, 388.00, 389.00,
    390.00, 391.00, 392.00, 393.00, 394.00, 395.00, 396.00, 397.00, 398.00, 399.00,
    400.00, 401.00, 402.00, 403.00, 404.00, 405.00, 406.00, 407.00, 408.00, 409.00,
    410.00, 411.00, 412.00, 413.00, 414.00, 415.00, 416.00, 417.00, 418.00, 419.00,
    420.00, 421.00, 422.00, 423.00, 424.00, 425.00, 426.00, 427.00, 428.00, 429.00,
    430.00, 431.00, 432.00, 433.00, 434.00, 435.00, 436.00, 437.00, 438.00, 439.00,
    440.00, 441.00, 442.00, 443.00, 444.00, 445.00, 446.00, 447.00, 448.00, 449.00,
    450.00, 451.00, 452.00, 453.00, 454.00, 455.00, 456.00, 457.00, 458.00, 459.00,
    460.00, 461.00, 462.00, 463.00, 464.00, 465.00, 466.00, 467.00, 468.00, 469.00,
    470.00, 471.00, 472.00, 473.00, 474.00, 475.00, 476.00, 477.00, 478.00, 479.00,
    480.00, 481.00, 482.00, 483.00, 484.00, 485.00, 486.00, 487.00, 488.00, 489.00,
    490.00, 491.00, 492.00, 493.00, 494.00, 495.00, 496.00, 497.00, 498.00, 499.00,
    500.00, 501.00, 502.00, 503.00, 504.00, 505.00, 506.00, 507.00, 508.00, 509.00,
    510.00, 511.00, 512.00, 513.00, 514.00, 515.00, 516.00, 517.00, 518.00, 519.00,
    520.00, 521.00, 522.00, 523.00, 524.00, 525.00, 526.00, 527.00, 528.00, 529.00,
    530.00, 531.00, 532.00, 533.00, 534.00, 535.00, 536.00, 537.00, 538.00, 539.00,
    540.00, 541.00, 542.00, 543.00, 544.00, 545.00, 546.00, 547.00, 548.00, 549.00,
    550.00, 551.00, 552.00, 553.00, 554.00, 555.00, 556.00, 557.00, 558.00, 559.00,
    560.00, 561.00, 562.00, 563.00, 564.00, 565.00, 566.00, 567.00, 568.00, 569.00,
    570.00, 571.00, 572.00, 573.00, 574.00, 575.00, 576.00, 577.00, 578.00, 579.00,
    580.00, 581.00, 582.00, 583.00, 584.00, 585.00, 586.00, 587.00, 588.00, 589.00,
    590.00, 591.00, 592.00, 593.00, 594.00, 595.00, 596.00, 597.00, 598.00, 599.00,
    600.00, 601.00, 602.00, 603.00, 604.00, 605.00, 606.00, 607.00, 608.00, 609.00,
    610.00, 611.00, 612.00, 613.00, 614.00, 615.00, 616.00, 617.00, 618.00, 619.00,
    620.00, 621.00, 622.00, 623.00, 624.00, 625.00, 626.00, 627.00, 628.00, 629.00,
    630.00, 631.00, 632.00, 633.00, 634.00, 635.00, 636.00, 637.00, 638.00, 639.00,
    640.00, 641.00, 642.00, 643.00, 644.00, 645.00, 646.00, 647.00, 648.00, 649.00,
    650.00, 651.00, 652.00, 653.00, 654.00, 655.00, 656.00, 657.00, 658.00, 659.00,
    660.00, 661.00, 662.00, 663.00, 664.00, 665.00, 666.00, 667.00, 668.00, 669.00,
    670.00, 671.00, 672.00, 673.00, 674.00, 675.00, 676.00, 677.00, 678.00, 679.00,
    680.00, 681.00, 682.00, 683.00, 684.00, 685.00, 686.00, 687.00, 688.00, 689.00,
    690.00, 691.00, 692.00, 693.00, 694.00, 695.00, 696.00, 697.00, 698.00, 699.00,
    700.00, 701.00, 702.00, 703.00, 704.00, 705.00, 706.00, 707.00, 708.00, 709.00,
    710.00, 711.00, 712.00, 713.00, 714.00, 715.00, 716.00, 717.00, 718.00, 719.00,
    720.00, 721.00, 722.00, 723.00, 724.00, 725.00, 726.00, 727.00, 728.00, 729.00,
    730.00, 731.00, 732.00, 733.00, 734.00, 735.00, 736.00, 737.00, 738.00, 739.00,
    740.00, 741.00, 742.00, 743.00, 744.00, 745.00, 746.00, 747.00, 748.00, 749.00,
    750.00, 751.00, 752.00, 753.00, 754.00, 755.00, 756.00, 757.00, 758.00, 759.00,
    760.00, 761.00, 762.00, 763.00, 764.00, 765.00, 766.00, 767.00, 768.00, 769.00,
    770.00, 771.00, 772.00, 773.00, 774.00, 775.00, 776.00, 777.00, 778.00, 779.00,
    780.00, 781.00, 782.00, 783.00, 784.00, 785.00, 786.00, 787.00, 788.00, 789.00,
    790.00, 791.00, 792.00, 793.00, 794.00, 795.00, 796.00, 797.00, 798.00, 799.00,
    800.00, 801.00, 802.00, 803.00, 804.00, 805.00, 806.00, 807.00, 808.00, 809.00
};

#include <gsl/gsl_errno.h>
#include <gsl/gsl_spline.h>

static gsl_interp_accel *PT100_acc;
static gsl_spline *PT100_spline;

#else

#define A -2.45002301e+02
#define B 2.33913372e-01
#define C 1.10965165e-05

#define A_ -2.45679585e+02
#define B_ 2.35566770e-01
#define C_ 1.01150208e-05

double
PT100_r2t_1(double r)
{
    r = r * 10.;
    return A + B * r + C * r * r;
}

double
PT100_r2t_2(double r)
{
    r = r * 10.;
    return A_ + B_ * r + C_ * r * r;
}

#endif

static double
PT100_r2t(double resistance)
{
#ifdef __USE_GSL__
    
    double temperature;
    int ret;
    
    if ((ret = gsl_spline_eval_e(PT100_spline, resistance, PT100_acc, &temperature)) == GSL_SUCCESS) {
        return temperature;
    } else {
        return 9999.;
    }
#else
    if (resistance <= 100.) {
        return PT100_r2t_1(resistance);
    } else if (resistance > 100. && resistance < 900.){
        return PT100_r2t_2(resistance);
    } else {
        return 9999.;
    }
#endif
    
}

static int
PT100_read_data(void *_self, double *data, size_t size)
{
    struct Sensor *self = cast(Sensor(), _self);

    int ret;
    char buf[BUFSIZE];
    void *controller = self->controller, *device = self->device;
    void *serial = klaws_controller_get_serial(controller);
    uint16_t index = klaws_device_get_index(device);
    
    klaws_controller_lock(controller);
    if (index == 0) {
        klaws_device_set_index(device, serial);
        index = klaws_device_get_index(device);
        if (index == 0) {
            klaws_controller_unlock(controller);
            return AAOS_ENOTFOUND;
        }
    }
    protobuf_set(serial, PACKET_INDEX, index);
    
    if ((ret = serial_raw(serial, self->command, strlen(self->command), buf, BUFSIZE, NULL)) == AAOS_OK) {
        const char *s = buf + 1;
        data[0] = atof(s);
        struct PT100 *myself = cast(PT100(), _self);
        if (myself->output_type == TEMEPRATURE_OUTPUT_RESISTANSE) {
            if (data[0] > 900.0) {
                data[0] = 9999.0;
            } else {
                data[0] = PT100_r2t(data[0]);
            }
        }
    } else {
        /*
         * When detecting network problem, try re-connect again.
         */
        switch (ret) {
            case -1 * AAOS_EPIPE:
            case -1 * AAOS_ECONNRESET:
            case -1 * AAOS_ETIMEDOUT:
            case -1 * AAOS_ENETDOWN:
            case -1 * AAOS_ENETUNREACH:
                if ((ret = KLAWSController_try_connect(self->controller)) == AAOS_OK && (ret = serial_raw(serial, self->command, strlen(self->command), buf, BUFSIZE, NULL)) == AAOS_OK) {
                    const char *s = buf + 1;
                    data[0] = atof(s);
                    struct PT100 *myself = cast(PT100(), _self);
                    if (myself->output_type == TEMEPRATURE_OUTPUT_RESISTANSE) {
                        if (data[0] > 900.) {
                            data[0] = 9999.0;
                        } else {
                            data[0] = PT100_r2t(data[0]);
                        }
                    }
                }
                break;
            default:
                break;
        }
    }
    klaws_controller_unlock(controller);
    
    return ret;
}
 
static int
PT100_read_raw_data(void *_self, void *data, size_t size)
{
    struct Sensor *self = cast(Sensor(), _self);

    int ret;
    char buf[BUFSIZE];
    void *controller = self->controller, *device = self->device;
    void *serial = klaws_controller_get_serial(controller);
    uint16_t index = klaws_device_get_index(device);
    
    klaws_controller_lock(controller);
    if (index == 0) {
        klaws_device_set_index(device, serial);
        index = klaws_device_get_index(device);
        if (index == 0) {
            klaws_controller_unlock(controller);
            return AAOS_ENOTFOUND;
        }
    }
    protobuf_set(serial, PACKET_INDEX, index);
    if ((ret = serial_raw(serial, self->command, strlen(self->command), buf, BUFSIZE, NULL)) == AAOS_OK) {
        const char *s = buf + 1;
        snprintf(data, size, "%s", s);
    } else {
        switch (ret) {
            case -1 * AAOS_EPIPE:
            case -1 * AAOS_ECONNRESET:
            case -1 * AAOS_ETIMEDOUT:
            case -1 * AAOS_ENETDOWN:
            case -1 * AAOS_ENETUNREACH:
                if ((ret = KLAWSController_try_connect(self->controller)) == AAOS_OK && (ret = serial_raw(serial, self->command, strlen(self->command), buf, BUFSIZE, NULL)) == AAOS_OK) {
                    const char *s = buf + 1;
                    snprintf(data, size, "%s", s);
                }
                break;
            default:
                break;
        }
    }
    klaws_controller_unlock(controller);
    
    return ret;
}

static const void *_pt100_virtual_table;

static void
pt100_virtual_table_destroy(void)
{
    delete((void *) _pt100_virtual_table);
}


static void
pt100_virtual_table_initialize(void)
{
    _pt100_virtual_table = new(SensorVirtualTable(),
                                    sensor_read_data, "read_data", PT100_read_data,
                                    sensor_read_raw_data, "read_raw_data", PT100_read_raw_data,
                                    (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(pt100_virtual_table_destroy);
#endif
}

static const void *
pt100_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, pt100_virtual_table_initialize);
#endif
    
    return _pt100_virtual_table;
}

/*
 * AAGPDUPT1000 temperature sensor.
 */

static const void *aag_pdu_pt1000_virtual_table(void);

static void *
AAGPDUPT1000_ctor(void *_self, va_list *app)
{
    struct AAGPDUPT1000 *self = super_ctor(AAGPDUPT1000(), _self, app);
    
    self->output_type = va_arg(*app, int);
    
    self->_._vtab= aag_pdu_pt1000_virtual_table();
    
    return (void *) self;
}

static void *
AAGPDUPT1000_dtor(void *_self)
{
    return super_dtor(AAGPDUPT1000(), _self);
}

static void *
AAGPDUPT1000Class_ctor(void *_self, va_list *app)
{
    struct AAGPDUPT1000Class *self = super_ctor(AAGPDUPT1000Class(), _self, app);
    
    self->_.read_data.method = (Method) 0;
    self->_.read_raw_data.method = (Method) 0;
    
    return self;
}

static const void *_AAGPDUPT1000Class;

static void
AAGPDUPT1000Class_destroy(void)
{
    free((void *)_AAGPDUPT1000Class);
}

static void
AAGPDUPT1000Class_initialize(void)
{
    _AAGPDUPT1000Class = new(SensorClass(), "AAGPDUPT1000Class", SensorClass(), sizeof(struct AAGPDUPT1000Class),
                           ctor, "", AAGPDUPT1000Class_ctor,
                           (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(AAGPDUPT1000Class_destroy);
#endif
}

const void *
AAGPDUPT1000Class(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, AAGPDUPT1000Class_initialize);
#endif
    
    return _AAGPDUPT1000Class;
}

static const void *_AAGPDUPT1000;

static void
AAGPDUPT1000_destroy(void)
{
    free((void *)_AAGPDUPT1000);
}

static void
AAGPDUPT1000_initialize(void)
{
    
    _AAGPDUPT1000 = new(AAGPDUPT1000Class(), "AAGPDUPT1000", Sensor(), sizeof(struct AAGPDUPT1000),
                     ctor, "ctor", AAGPDUPT1000_ctor,
                     dtor, "dtor", AAGPDUPT1000_dtor,
                     (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(AAGPDUPT1000_destroy);
#endif
}

const void *
AAGPDUPT1000(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, AAGPDUPT1000_initialize);
#endif

    return _AAGPDUPT1000;
}

static int
AAGPDUPT1000_read_data(void *_self, double *data, size_t size)
{
    struct Sensor *self = cast(Sensor(), _self);

    int ret;
    char buf[BUFSIZE];
    void *controller = self->controller, *device = self->device;
    void *serial = klaws_controller_get_serial(controller);
    uint16_t index = klaws_device_get_index(device);
    
    klaws_controller_lock(controller);
    if (index == 0) {
        klaws_device_set_index(device, serial);
        index = klaws_device_get_index(device);
        if (index == 0) {
            klaws_controller_unlock(controller);
            return AAOS_ENOTFOUND;
        }
    }
    protobuf_set(serial, PACKET_INDEX, index);
    
    if ((ret = serial_raw(serial, self->command, strlen(self->command), buf, BUFSIZE, NULL)) == AAOS_OK) {
        const char *s = strrchr(buf, ':');
        if (s != NULL) {
            s++;
            data[0] = ((double) atoi(s)) / 1000.;
        } else {
            data[0] = 9999.;
        }
        struct AAGPDUPT1000 *myself = cast(AAGPDUPT1000(), _self);
        if (myself->output_type == TEMEPRATURE_OUTPUT_RESISTANSE) {
            if (data[0] > 900.0) {
                data[0] = 9999.0;
            } else {
                data[0] = PT100_r2t(data[0]);
            }
        }
    } else {
        /*
         * When detecting network problem, try re-connect again.
         */
        switch (ret) {
            case -1 * AAOS_EPIPE:
            case -1 * AAOS_ECONNRESET:
            case -1 * AAOS_ETIMEDOUT:
            case -1 * AAOS_ENETDOWN:
            case -1 * AAOS_ENETUNREACH:
                if ((ret = KLAWSController_try_connect(self->controller)) == AAOS_OK && (ret = serial_raw(serial, self->command, strlen(self->command), buf, BUFSIZE, NULL)) == AAOS_OK) {
                    const char *s = strrchr(buf, ':');
                    if (s != NULL) {
                        s++;
                        data[0] = ((double) atoi(s)) / 1000.;
                    } else {
                        data[0] = 9999.;
                    }
                    struct AAGPDUPT1000 *myself = cast(AAGPDUPT1000(), _self);
                    if (myself->output_type == TEMEPRATURE_OUTPUT_RESISTANSE) {
                        if (data[0] > 900.) {
                            data[0] = 9999.0;
                        } else {
                            data[0] = PT100_r2t(data[0]);
                        }
                    }
                }
                break;
            default:
                break;
        }
    }
    klaws_controller_unlock(controller);
    
    return ret;
}
 
static int
AAGPDUPT1000_read_raw_data(void *_self, void *data, size_t size)
{
    struct Sensor *self = cast(Sensor(), _self);

    int ret;
    char buf[BUFSIZE];
    void *controller = self->controller, *device = self->device;
    void *serial = klaws_controller_get_serial(controller);
    uint16_t index = klaws_device_get_index(device);
    
    klaws_controller_lock(controller);
    if (index == 0) {
        klaws_device_set_index(device, serial);
        index = klaws_device_get_index(device);
        if (index == 0) {
            klaws_controller_unlock(controller);
            return AAOS_ENOTFOUND;
        }
    }
    protobuf_set(serial, PACKET_INDEX, index);
    if ((ret = serial_raw(serial, self->command, strlen(self->command), buf, BUFSIZE, NULL)) == AAOS_OK) {
        const char *s = strrchr(buf, ':');
        if (s != NULL) {
            s++;
            snprintf(data, size, "%s", s);
        }
    } else {
        switch (ret) {
            case -1 * AAOS_EPIPE:
            case -1 * AAOS_ECONNRESET:
            case -1 * AAOS_ETIMEDOUT:
            case -1 * AAOS_ENETDOWN:
            case -1 * AAOS_ENETUNREACH:
                if ((ret = KLAWSController_try_connect(self->controller)) == AAOS_OK && (ret = serial_raw(serial, self->command, strlen(self->command), buf, BUFSIZE, NULL)) == AAOS_OK) {
                    const char *s = strrchr(buf, ':');
                    if (s != NULL) {
                        s++;
                        snprintf(data, size, "%s", s);
                    }
                }
                break;
            default:
                break;
        }
    }
    klaws_controller_unlock(controller);
    
    return ret;
}

static const void *_aag_pdu_pt1000_virtual_table;

static void
aag_pdu_pt1000_virtual_table_destroy(void)
{
    delete((void *) _aag_pdu_pt1000_virtual_table);
}


static void
aag_pdu_pt1000_virtual_table_initialize(void)
{
    _aag_pdu_pt1000_virtual_table = new(SensorVirtualTable(),
                                    sensor_read_data, "read_data", AAGPDUPT1000_read_data,
                                    sensor_read_raw_data, "read_raw_data", AAGPDUPT1000_read_raw_data,
                                    (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(aag_pdu_pt1000_virtual_table_destroy);
#endif
}

static const void *
aag_pdu_pt1000_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, aag_pdu_pt1000_virtual_table_initialize);
#endif
    
    return _aag_pdu_pt1000_virtual_table;
}


/*
 * Young 41342 temperature sensor.
 */

static const void *young41342_virtual_table(void);

static void *
Young41342_ctor(void *_self, va_list *app)
{
    struct Young41342 *self = super_ctor(Young41342(), _self, app);
    
    const char *key;
    while ((key = va_arg(*app, const char *))) {
        if (strcmp(key, "output_type") == 0) {
            self->output_type = va_arg(*app, int);
        }
        continue;
    }
    
    self->_._vtab= young41342_virtual_table();
    
    return (void *) self;
}

static void *
Young41342_dtor(void *_self)
{
    return super_dtor(Young41342(), _self);
}

static void *
Young41342Class_ctor(void *_self, va_list *app)
{
    struct Young41342Class *self = super_ctor(Young41342Class(), _self, app);
    
    self->_.read_data.method = (Method) 0;
    self->_.read_raw_data.method = (Method) 0;
    
    return self;
}

static const void *_Young41342Class;

static void
Young41342Class_destroy(void)
{
    free((void *)_Young41342Class);
}

static void
Young41342Class_initialize(void)
{
    _Young41342Class = new(SensorClass(), "Young41342Class", SensorClass(), sizeof(struct Young41342Class),
                           ctor, "", Young41342Class_ctor,
                           (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(Young41342Class_destroy);
#endif
}

const void *
Young41342Class(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, Young41342Class_initialize);
#endif
    
    return _Young41342Class;
}

static const void *_Young41342;

static void
Young41342_destroy(void)
{
    free((void *)_Young41342);
}

static void
Young41342_initialize(void)
{
    
    _Young41342 = new(Young41342Class(), "Young41342", Sensor(), sizeof(struct Young41342),
                     ctor, "ctor", Young41342_ctor,
                     dtor, "dtor", Young41342_dtor,
                     (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(Young41342_destroy);
#endif
}

const void *
Young41342(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, Young41342_initialize);
#endif

    return _Young41342;
}

static double
Young41342_r2t(double resistance)
{
    double a = 1.1279e-5, b = 2.3985e-1, c = -251.1326;
    
    return a * resistance * resistance + b * resistance + c;
}

static int
Young41342_read_data(void *_self, double *data, size_t size)
{
    struct Sensor *self = cast(Sensor(), _self);

    int ret;
    char buf[BUFSIZE];
    void *controller = self->controller, *device = self->device;
    void *serial = klaws_controller_get_serial(controller);
    uint16_t index = klaws_device_get_index(device);
    
    klaws_controller_lock(controller);
    if (index == 0) {
        klaws_device_set_index(device, serial);
        index = klaws_device_get_index(device);
        if (index == 0) {
            klaws_controller_unlock(controller);
            return AAOS_ENOTFOUND;
        }
    }
    protobuf_set(serial, PACKET_INDEX, index);
    
    
    if ((ret = serial_raw(serial, self->command, strlen(self->command), buf, BUFSIZE, NULL)) == AAOS_OK) {
        const char *s = buf + 1;
        data[0] = atof(s);
        struct Young41342 *myself = cast(Young41342(), _self);
        if (myself->output_type == TEMEPRATURE_OUTPUT_RESISTANSE) {
            data[0] *= 10;
            if (data[0] > 9000) {
                data[0] = 9999.0;
            } else {
                data[0] = Young41342_r2t(data[0]);
            }
        }
    } else {
        /*
         * When detecting network problem, try re-connect again.
         */
        switch (ret) {
            case -1 * AAOS_EPIPE:
            case -1 * AAOS_ECONNRESET:
            case -1 * AAOS_ETIMEDOUT:
            case -1 * AAOS_ENETDOWN:
            case -1 * AAOS_ENETUNREACH:
                if ((ret = KLAWSController_try_connect(self->controller)) == AAOS_OK && (ret = serial_raw(serial, self->command, strlen(self->command), buf, BUFSIZE, NULL)) == AAOS_OK) {
                    const char *s = buf + 1;
                    data[0] = atof(s);
                    struct Young41342 *myself = cast(Young41342(), _self);
                    if (myself->output_type == TEMEPRATURE_OUTPUT_RESISTANSE) {
                        data[0] *= 10;
                        if (data[0] > 9000) {
                            data[0] = 9999.0;
                        } else {
                            data[0] = Young41342_r2t(data[0]);
                        }
                    }
                }
                break;
            default:
                break;
        }
    }
    klaws_controller_unlock(controller);
    
    return ret;
}
 
static int
Young41342_read_raw_data(void *_self, void *data, size_t size)
{
    struct Sensor *self = cast(Sensor(), _self);

    int ret;
    char buf[BUFSIZE];
    void *controller = self->controller, *device = self->device;
    void *serial = klaws_controller_get_serial(controller);
    uint16_t index = klaws_device_get_index(device);
    
    klaws_controller_lock(controller);
    if (index == 0) {
        klaws_device_set_index(device, serial);
        index = klaws_device_get_index(device);
        if (index == 0) {
            klaws_controller_unlock(controller);
            return AAOS_ENOTFOUND;
        }
    }
    protobuf_set(serial, PACKET_INDEX, index);
    if ((ret = serial_raw(serial, self->command, strlen(self->command), buf, BUFSIZE, NULL)) == AAOS_OK) {
        const char *s = buf + 1;
        snprintf(data, size, "%s", s);
    } else {
        switch (ret) {
            case -1 * AAOS_EPIPE:
            case -1 * AAOS_ECONNRESET:
            case -1 * AAOS_ETIMEDOUT:
            case -1 * AAOS_ENETDOWN:
            case -1 * AAOS_ENETUNREACH:
                if ((ret = KLAWSController_try_connect(self->controller)) == AAOS_OK && (ret = serial_raw(serial, self->command, strlen(self->command), buf, BUFSIZE, NULL)) == AAOS_OK) {
                    const char *s = buf + 1;
                    snprintf(data, size, "%s", s);
                }
                break;
            default:
                break;
        }
    }
    klaws_controller_unlock(controller);
    
    return ret;
}

static const void *_young41342_virtual_table;

static void
young41342_virtual_table_destroy(void)
{
    delete((void *) _young41342_virtual_table);
}


static void
young41342_virtual_table_initialize(void)
{
    _young41342_virtual_table = new(SensorVirtualTable(),
                                    sensor_read_data, "read_data", Young41342_read_data,
                                    sensor_read_raw_data, "read_raw_data", Young41342_read_raw_data,
                                    (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(young41342_virtual_table_destroy);
#endif
}

static const void *
young41342_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, young41342_virtual_table_initialize);
#endif
    
    return _young41342_virtual_table;
}

/*
 * Young 05305V wind speed sensor.
 */
static const void *young05305vs_virtual_table(void);

static void *
Young05305VS_ctor(void *_self, va_list *app)
{
    struct Young05305VS *self = super_ctor(Young05305VS(), _self, app);
    
    self->_._vtab= young05305vs_virtual_table();
    
    return (void *) self;
}

static void *
Young05305VS_dtor(void *_self)
{
    return super_dtor(Young05305VS(), _self);
}

static void *
Young05305VSClass_ctor(void *_self, va_list *app)
{
    struct Young05305VSClass *self = super_ctor(Young05305VSClass(), _self, app);
    
    self->_.read_data.method = (Method) 0;
    self->_.read_raw_data.method = (Method) 0;
    
    return self;
}

static const void *_Young05305VSClass;

static void
Young05305VSClass_destroy(void)
{
    free((void *)_Young05305VSClass);
}

static void
Young05305VSClass_initialize(void)
{
    _Young05305VSClass = new(SensorClass(), "Young05305VSClass", SensorClass(), sizeof(struct Young05305VSClass),
                           ctor, "", Young05305VSClass_ctor,
                           (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(Young05305VSClass_destroy);
#endif
}

const void *
Young05305VSClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, Young05305VSClass_initialize);
#endif
    
    return _Young05305VSClass;
}

static const void *_Young05305VS;

static void
Young05305VS_destroy(void)
{
    free((void *)_Young05305VS);
}

static void
Young05305VS_initialize(void)
{
    
    _Young05305VS = new(Young05305VSClass(), "Young05305VS", Sensor(), sizeof(struct Young05305VS),
                     ctor, "ctor", Young05305VS_ctor,
                     dtor, "dtor", Young05305VS_dtor,
                     (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(Young05305VS_destroy);
#endif
}

const void *
Young05305VS(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, Young05305VS_initialize);
#endif

    return _Young05305VS;
}

static int
Young05305VS_read_data(void *_self, double *data, size_t size)
{
    struct Sensor *self = cast(Sensor(), _self);

    int ret;
    char buf[BUFSIZE];
    void *controller = self->controller, *device = self->device;
    void *serial = klaws_controller_get_serial(controller);
    uint16_t index = klaws_device_get_index(device);
    
    klaws_controller_lock(controller);
    if (index == 0) {
        klaws_device_set_index(device, serial);
        index = klaws_device_get_index(device);
        if (index == 0) {
            klaws_controller_unlock(controller);
            return AAOS_ENOTFOUND;
        }
    }
    protobuf_set(serial, PACKET_INDEX, index);
    if ((ret = serial_raw(serial, self->command, strlen(self->command), buf, BUFSIZE, NULL)) == AAOS_OK) {
        const char *s = buf + 1;
        data[0] = atof(s);
        data[0] *= 20.;
    } else {
        switch (ret) {
            case -1 * AAOS_EPIPE:
            case -1 * AAOS_ECONNRESET:
            case -1 * AAOS_ETIMEDOUT:
            case -1 * AAOS_ENETDOWN:
            case -1 * AAOS_ENETUNREACH:
                if ((ret = KLAWSController_try_connect(self->controller)) == AAOS_OK && (ret = serial_raw(serial, self->command, strlen(self->command), buf, BUFSIZE, NULL)) == AAOS_OK) {
                    const char *s = buf + 1;
                    data[0] = atof(s);
                    data[0] *= 20.;
                }
                break;
            default:
                break;
        }
    }
     klaws_controller_unlock(controller);
        
    return ret;
}
 
static int
Young05305VS_read_raw_data(void *_self, void *data, size_t size)
{
    struct Sensor *self = cast(Sensor(), _self);

    void *controller = self->controller;
    void *device = self->device;
    void *serial = klaws_controller_get_serial(controller);
    uint16_t index = klaws_device_get_index(device);
    char buf[BUFSIZE];
    int ret;
    
    klaws_controller_lock(controller);
    if (index == 0) {
        klaws_device_set_index(device, serial);
        index = klaws_device_get_index(device);
        if (index == 0) {
            klaws_controller_unlock(controller);
            return AAOS_ENOTFOUND;
        }
    }
    if ((ret = serial_raw(serial, self->command, strlen(self->command), buf, BUFSIZE, NULL)) == AAOS_OK) {
        const char *s = buf + 1;
        snprintf(data, size, "%s", s);
    } else {
        switch (ret) {
            case -1 * AAOS_EPIPE:
            case -1 * AAOS_ECONNRESET:
            case -1 * AAOS_ETIMEDOUT:
            case -1 * AAOS_ENETDOWN:
            case -1 * AAOS_ENETUNREACH:
                if ((ret = KLAWSController_try_connect(self->controller)) == AAOS_OK && (ret = serial_raw(serial, self->command, strlen(self->command), buf, BUFSIZE, NULL)) == AAOS_OK) {
                    const char *s = buf + 1;
                    snprintf(data, size, "%s", s);
                }
                break;
            default:
                break;
        }
    }
    klaws_controller_lock(controller);
    
    return ret;
}

static const void *_young05305vs_virtual_table;

static void
young05305vs_virtual_table_destroy(void)
{
    delete((void *) _young05305vs_virtual_table);
}


static void
young05305vs_virtual_table_initialize(void)
{
    _young05305vs_virtual_table = new(SensorVirtualTable(),
                                    sensor_read_data, "read_data", Young05305VS_read_data,
                                    sensor_read_raw_data, "read_raw_data", Young05305VS_read_raw_data,
                                    (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(young05305vs_virtual_table_destroy);
#endif
}

static const void *
young05305vs_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, young05305vs_virtual_table_initialize);
#endif
    
    return _young05305vs_virtual_table;
}

/*
 * Young 05305V wind direction sensor.
 */
static const void *young05305vd_virtual_table(void);

static void *
Young05305VD_ctor(void *_self, va_list *app)
{
    struct Young05305VD *self = super_ctor(Young05305VD(), _self, app);
    
    self->_._vtab= young05305vd_virtual_table();
    self->offset = 0.;
    
    const char *key;
    while ((key = va_arg(*app, const char *))) {
        if (strcmp(key, "offset") == 0) {
            self->offset = va_arg(*app, double);
        }
        continue;
    }
    
    return (void *) self;
}

static void *
Young05305VD_dtor(void *_self)
{
    return super_dtor(Young05305VD(), _self);
}

static void *
Young05305VDClass_ctor(void *_self, va_list *app)
{
    struct Young05305VDClass *self = super_ctor(Young05305VDClass(), _self, app);
    
    self->_.read_data.method = (Method) 0;
    self->_.read_raw_data.method = (Method) 0;
    
    return self;
}

static const void *_Young05305VDClass;

static void
Young05305VDClass_destroy(void)
{
    free((void *)_Young05305VDClass);
}

static void
Young05305VDClass_initialize(void)
{
    _Young05305VDClass = new(SensorClass(), "Young05305VDClass", SensorClass(), sizeof(struct Young05305VDClass),
                           ctor, "", Young05305VDClass_ctor,
                           (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(Young05305VDClass_destroy);
#endif
}

const void *
Young05305VDClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, Young05305VDClass_initialize);
#endif
    
    return _Young05305VDClass;
}

static const void *_Young05305VD;

static void
Young05305VD_destroy(void)
{
    free((void *)_Young05305VD);
}

static void
Young05305VD_initialize(void)
{
    
    _Young05305VD = new(Young05305VDClass(), "Young05305VD", Sensor(), sizeof(struct Young05305VD),
                     ctor, "ctor", Young05305VD_ctor,
                     dtor, "dtor", Young05305VD_dtor,
                     (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(Young05305VD_destroy);
#endif
}

const void *
Young05305VD(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, Young05305VD_initialize);
#endif

    return _Young05305VD;
}

static int
Young05305VD_read_data(void *_self, double *data, size_t size)
{
    struct Sensor *self = cast(Sensor(), _self);
    struct Young05305VD *myself = cast(Young05305VD(), _self);
    
    int ret;
    char buf[BUFSIZE];
    void *controller = self->controller, *device = self->device;
    void *serial = klaws_controller_get_serial(controller);
    uint16_t index = klaws_device_get_index(device);
    
    klaws_controller_lock(controller);
    if (index == 0) {
        klaws_device_set_index(device, serial);
        index = klaws_device_get_index(device);
        if (index == 0) {
            klaws_controller_unlock(controller);
            return AAOS_ENOTFOUND;
        }
    }
    protobuf_set(serial, PACKET_INDEX, index);
    if ((ret = serial_raw(serial, self->command, strlen(self->command), buf, BUFSIZE, NULL)) == AAOS_OK) {
        const char *s = buf + 1;
        data[0] = atof(s) * 72. + myself->offset;
        if (data[0] > 360.) {
            data[0] -= 360.;
        }
    } else {
        switch (ret) {
            case -1 * AAOS_EPIPE:
            case -1 * AAOS_ECONNRESET:
            case -1 * AAOS_ETIMEDOUT:
            case -1 * AAOS_ENETDOWN:
            case -1 * AAOS_ENETUNREACH:
                if ((ret = KLAWSController_try_connect(self->controller)) == AAOS_OK && (ret = serial_raw(serial, self->command, strlen(self->command), buf, BUFSIZE, NULL)) == AAOS_OK) {
                    const char *s = buf + 1;
                    data[0] = atof(s) * 72. + myself->offset;
                    if (data[0] > 360.) {
                        data[0] -= 360.;
                    }
                }
                break;
            default:
                break;
        }
    }
    klaws_controller_unlock(controller);
        
    return ret;
}
 
static int
Young05305VD_read_raw_data(void *_self, void *data, size_t size)
{
    struct Sensor *self = cast(Sensor(), _self);
    
    int ret;
    char buf[BUFSIZE];
    void *controller = self->controller, *device = self->device;
    void *serial = klaws_controller_get_serial(controller);
    uint16_t index = klaws_device_get_index(device);
    
    klaws_controller_lock(controller);
    if (index == 0) {
        klaws_device_set_index(device, serial);
        index = klaws_device_get_index(device);
        if (index == 0) {
            klaws_controller_unlock(controller);
            return AAOS_ENOTFOUND;
        }
    }
    protobuf_set(serial, PACKET_INDEX, index);
    if ((ret = serial_raw(serial, self->command, strlen(self->command), buf, BUFSIZE, NULL)) == AAOS_OK) {
        const char *s = buf + 1;
        snprintf(data, size, "%s", s);
    } else {
        switch (ret) {
            case -1 * AAOS_EPIPE:
            case -1 * AAOS_ECONNRESET:
            case -1 * AAOS_ETIMEDOUT:
            case -1 * AAOS_ENETDOWN:
            case -1 * AAOS_ENETUNREACH:
                if ((ret = KLAWSController_try_connect(self->controller)) == AAOS_OK && (ret = serial_raw(serial, self->command, strlen(self->command), buf, BUFSIZE, NULL)) == AAOS_OK) {
                    const char *s = buf + 1;
                    snprintf(data, size, "%s", s);
                }
                break;
            default:
                break;
        }
    }
    klaws_controller_unlock(controller);
    
    return ret;
}

static const void *_young05305vd_virtual_table;

static void
young05305vd_virtual_table_destroy(void)
{
    delete((void *) _young05305vd_virtual_table);
}


static void
young05305vd_virtual_table_initialize(void)
{
    _young05305vd_virtual_table = new(SensorVirtualTable(),
                                    sensor_read_data, "read_data", Young05305VD_read_data,
                                    sensor_read_raw_data, "read_raw_data", Young05305VD_read_raw_data,
                                    (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(young05305vd_virtual_table_destroy);
#endif
}

static const void *
young05305vd_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, young05305vd_virtual_table_initialize);
#endif
    
    return _young05305vd_virtual_table;
}

/*
 * Young Young61302V barometer sensor.
 */
static const void *young61302v_virtual_table(void);

static void *
Young61302V_ctor(void *_self, va_list *app)
{
    struct Young61302V *self = super_ctor(Young61302V(), _self, app);
    
    self->_._vtab= young61302v_virtual_table();
    
    return (void *) self;
}

static void *
Young61302V_dtor(void *_self)
{
    return super_dtor(Young61302V(), _self);
}

static void *
Young61302VClass_ctor(void *_self, va_list *app)
{
    struct Young61302VClass *self = super_ctor(Young61302VClass(), _self, app);
    
    self->_.read_data.method = (Method) 0;
    self->_.read_raw_data.method = (Method) 0;
    
    return self;
}

static const void *_Young61302VClass;

static void
Young61302VClass_destroy(void)
{
    free((void *)_Young61302VClass);
}

static void
Young61302VClass_initialize(void)
{
    _Young61302VClass = new(SensorClass(), "Young61302VClass", SensorClass(), sizeof(struct Young61302VClass),
                           ctor, "", Young61302VClass_ctor,
                           (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(Young61302VClass_destroy);
#endif
}

const void *
Young61302VClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, Young61302VClass_initialize);
#endif
    
    return _Young61302VClass;
}

static const void *_Young61302V;

static void
Young61302V_destroy(void)
{
    free((void *)_Young61302V);
}

static void
Young61302V_initialize(void)
{
    
    _Young61302V = new(Young61302VClass(), "Young61302V", Sensor(), sizeof(struct Young61302V),
                     ctor, "ctor", Young61302V_ctor,
                     dtor, "dtor", Young61302V_dtor,
                     (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(Young61302V_destroy);
#endif
}

const void *
Young61302V(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, Young61302V_initialize);
#endif

    return _Young61302V;
}

static int
Young61302V_read_data(void *_self, double *data, size_t size)
{
    struct Sensor *self = cast(Sensor(), _self);

    int ret;
    char buf[BUFSIZE];
    void *controller = self->controller, *device = self->device;
    void *serial = klaws_controller_get_serial(controller);
    uint16_t index = klaws_device_get_index(device);
    
    klaws_controller_lock(controller);
    if (index == 0) {
        klaws_device_set_index(device, serial);
        index = klaws_device_get_index(device);
        if (index == 0) {
            klaws_controller_unlock(controller);
            return AAOS_ENOTFOUND;
        }
    }
    protobuf_set(serial, PACKET_INDEX, index);
    if ((ret = serial_raw(serial, self->command, strlen(self->command), buf, BUFSIZE, NULL)) == AAOS_OK) {
        const char *s = buf + 1;
        data[0] = atof(s);
        data[0] = data[0] * 120. + 500.;
    } else {
        switch (ret) {
            case -1 * AAOS_EPIPE:
            case -1 * AAOS_ECONNRESET:
            case -1 * AAOS_ETIMEDOUT:
            case -1 * AAOS_ENETDOWN:
            case -1 * AAOS_ENETUNREACH:
                if ((ret = KLAWSController_try_connect(self->controller)) == AAOS_OK && (ret = serial_raw(serial, self->command, strlen(self->command), buf, BUFSIZE, NULL)) == AAOS_OK) {
                    const char *s = buf + 1;
                    data[0] = atof(s);
                    data[0] = data[0] * 120. + 500.;
                }
                break;
            default:
                break;
        }
    }
     
klaws_controller_unlock(controller);
        
    return ret;
}
 
static int
Young61302V_read_raw_data(void *_self, void *data, size_t size)
{
    struct Sensor *self = cast(Sensor(), _self);

    int ret;
    char buf[BUFSIZE];
    void *controller = self->controller, *device = self->device;
    void *serial = klaws_controller_get_serial(controller);
    uint16_t index = klaws_device_get_index(device);
    
    klaws_controller_lock(controller);
    if (index == 0) {
        klaws_device_set_index(device, serial);
        index = klaws_device_get_index(device);
        if (index == 0) {
            klaws_controller_unlock(controller);
            return AAOS_ENOTFOUND;
        }
    }
    protobuf_set(serial, PACKET_INDEX, index);
    if ((ret = serial_raw(serial, self->command, strlen(self->command), buf, BUFSIZE, NULL)) == AAOS_OK) {
        const char *s = buf + 1;
        snprintf(data, size, "%s", s);
    } else {
        switch (ret) {
            case -1 * AAOS_EPIPE:
            case -1 * AAOS_ECONNRESET:
            case -1 * AAOS_ETIMEDOUT:
            case -1 * AAOS_ENETDOWN:
            case -1 * AAOS_ENETUNREACH:
                if ((ret = KLAWSController_try_connect(self->controller)) == AAOS_OK && (ret = serial_raw(serial, self->command, strlen(self->command), buf, BUFSIZE, NULL)) == AAOS_OK) {
                    const char *s = buf + 1;
                    snprintf(data, size, "%s", s);
                }
                break;
            default:
                break;
        }
    }
    klaws_controller_unlock(controller);
    
    return ret;
}

static const void *_young61302v_virtual_table;

static void
young61302v_virtual_table_destroy(void)
{
    delete((void *) _young61302v_virtual_table);
}


static void
young61302v_virtual_table_initialize(void)
{
    _young61302v_virtual_table = new(SensorVirtualTable(),
                                    sensor_read_data, "read_data", Young61302V_read_data,
                                    sensor_read_raw_data, "read_raw_data", Young61302V_read_raw_data,
                                    (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(young61302v_virtual_table_destroy);
#endif
}

static const void *
young61302v_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, young61302v_virtual_table_initialize);
#endif
    
    return _young61302v_virtual_table;
}

/*
 * Young PTB210 barometer sensor.
 */
static const void *ptb210_virtual_table(void);

static void *
PTB210_ctor(void *_self, va_list *app)
{
    struct PTB210 *self = super_ctor(PTB210(), _self, app);
    
    self->_._vtab= ptb210_virtual_table();
    
    return (void *) self;
}

static void *
PTB210_dtor(void *_self)
{
    return super_dtor(PTB210(), _self);
}

static void *
PTB210Class_ctor(void *_self, va_list *app)
{
    struct PTB210Class *self = super_ctor(PTB210Class(), _self, app);
    
    self->_.read_data.method = (Method) 0;
    self->_.read_raw_data.method = (Method) 0;
    
    return self;
}

static const void *_PTB210Class;

static void
PTB210Class_destroy(void)
{
    free((void *)_PTB210Class);
}

static void
PTB210Class_initialize(void)
{
    _PTB210Class = new(SensorClass(), "PTB210Class", SensorClass(), sizeof(struct PTB210Class),
                           ctor, "", PTB210Class_ctor,
                           (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(PTB210Class_destroy);
#endif
}

const void *
PTB210Class(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, PTB210Class_initialize);
#endif
    
    return _PTB210Class;
}

static const void *_PTB210;

static void
PTB210_destroy(void)
{
    free((void *)_PTB210);
}

static void
PTB210_initialize(void)
{
    _PTB210 = new(PTB210Class(), "PTB210", Sensor(), sizeof(struct PTB210),
                  ctor, "ctor", PTB210_ctor,
                  dtor, "dtor", PTB210_dtor,
                  (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(PTB210_destroy);
#endif
}

const void *
PTB210(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, PTB210_initialize);
#endif

    return _PTB210;
}

static int
PTB210_read_data(void *_self, double *data, size_t size)
{
    struct Sensor *self = cast(Sensor(), _self);

    int ret;
    char buf[BUFSIZE];
    void *controller = self->controller, *device = self->device;
    void *serial = klaws_controller_get_serial(controller);
    uint16_t index = klaws_device_get_index(device);
    
    klaws_controller_lock(controller);
    if (index == 0) {
        klaws_device_set_index(device, serial);
        index = klaws_device_get_index(device);
        if (index == 0) {
            klaws_controller_unlock(controller);
            return AAOS_ENOTFOUND;
        }
    }
    protobuf_set(serial, PACKET_INDEX, index);
    if ((ret = serial_raw(serial, self->command, strlen(self->command), buf, BUFSIZE, NULL)) == AAOS_OK) {
        data[0] = atof(buf);
    } else {
        switch (ret) {
            case -1 * AAOS_EPIPE:
            case -1 * AAOS_ECONNRESET:
            case -1 * AAOS_ETIMEDOUT:
            case -1 * AAOS_ENETDOWN:
            case -1 * AAOS_ENETUNREACH:
                if ((ret = KLAWSController_try_connect(self->controller)) == AAOS_OK && (ret = serial_raw(serial, self->command, strlen(self->command), buf, BUFSIZE, NULL)) == AAOS_OK) {
                    data[0] = atof(buf);
                }
                break;
            default:
                break;
        }
    }
    
    klaws_controller_unlock(controller);
        
    return ret;
}
 
static int
PTB210_read_raw_data(void *_self, void *data, size_t size)
{
    struct Sensor *self = cast(Sensor(), _self);

    int ret;
    char buf[BUFSIZE];
    void *controller = self->controller, *device = self->device;
    void *serial = klaws_controller_get_serial(controller);
    uint16_t index = klaws_device_get_index(device);
    
    klaws_controller_lock(controller);
    if (index == 0) {
        klaws_device_set_index(device, serial);
        index = klaws_device_get_index(device);
        if (index == 0) {
            klaws_controller_unlock(controller);
            return AAOS_ENOTFOUND;
        }
    }
    protobuf_set(serial, PACKET_INDEX, index);
    if ((ret = serial_raw(serial, self->command, strlen(self->command), buf, BUFSIZE, NULL)) == AAOS_OK) {
        snprintf(data, size, "%s", buf);
    } else {
        switch (ret) {
            case -1 * AAOS_EPIPE:
            case -1 * AAOS_ECONNRESET:
            case -1 * AAOS_ETIMEDOUT:
            case -1 * AAOS_ENETDOWN:
            case -1 * AAOS_ENETUNREACH:
                if ((ret = KLAWSController_try_connect(self->controller)) == AAOS_OK && (ret = serial_raw(serial, self->command, strlen(self->command), buf, BUFSIZE, NULL)) == AAOS_OK) {
                    snprintf(data, size, "%s", buf);
                }
                break;
            default:
                break;
        }
    }
    klaws_controller_unlock(controller);
    
    return ret;
}

static const void *_ptb210_virtual_table;

static void
ptb210_virtual_table_destroy(void)
{
    delete((void *) _ptb210_virtual_table);
}


static void
ptb210_virtual_table_initialize(void)
{
    _ptb210_virtual_table = new(SensorVirtualTable(),
                                sensor_read_data, "read_data", PTB210_read_data,
                                sensor_read_raw_data, "read_raw_data", PTB210_read_raw_data,
                                (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(ptb210_virtual_table_destroy);
#endif
}

static const void *
ptb210_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, ptb210_virtual_table_initialize);
#endif
    
    return _ptb210_virtual_table;
}

/*
 * Young Young41382VCR relative humodity sensor.
 */
static const void *young41382vcr_virtual_table(void);

static void *
Young41382VCR_ctor(void *_self, va_list *app)
{
    struct Young41382VCR *self = super_ctor(Young41382VCR(), _self, app);
    
    self->_._vtab= young41382vcr_virtual_table();
    
    return (void *) self;
}

static void *
Young41382VCR_dtor(void *_self)
{
    return super_dtor(Young41382VCR(), _self);
}

static void *
Young41382VCRClass_ctor(void *_self, va_list *app)
{
    struct Young41382VCRClass *self = super_ctor(Young41382VCRClass(), _self, app);
    
    self->_.read_data.method = (Method) 0;
    self->_.read_raw_data.method = (Method) 0;
    
    return self;
}

static const void *_Young41382VCRClass;

static void
Young41382VCRClass_destroy(void)
{
    free((void *)_Young41382VCRClass);
}

static void
Young41382VCRClass_initialize(void)
{
    _Young41382VCRClass = new(SensorClass(), "Young41382VCRClass", SensorClass(), sizeof(struct Young41382VCRClass),
                           ctor, "", Young41382VCRClass_ctor,
                           (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(Young41382VCRClass_destroy);
#endif
}

const void *
Young41382VCRClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, Young41382VCRClass_initialize);
#endif
    
    return _Young41382VCRClass;
}

static const void *_Young41382VCR;

static void
Young41382VCR_destroy(void)
{
    free((void *)_Young41382VCR);
}

static void
Young41382VCR_initialize(void)
{
    
    _Young41382VCR = new(Young41382VCRClass(), "Young41382VCR", Sensor(), sizeof(struct Young41382VCR),
                     ctor, "ctor", Young41382VCR_ctor,
                     dtor, "dtor", Young41382VCR_dtor,
                     (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(Young41382VCR_destroy);
#endif
}

const void *
Young41382VCR(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, Young41382VCR_initialize);
#endif

    return _Young41382VCR;
}

static int
Young41382VCR_read_data(void *_self, double *data, size_t size)
{
    struct Sensor *self = cast(Sensor(), _self);

    int ret;
    char buf[BUFSIZE];
    void *controller = self->controller, *device = self->device;
    void *serial = klaws_controller_get_serial(controller);
    uint16_t index = klaws_device_get_index(device);
    
    klaws_controller_lock(controller);
    if (index == 0) {
        klaws_device_set_index(device, serial);
        index = klaws_device_get_index(device);
        if (index == 0) {
            klaws_controller_unlock(controller);
            return AAOS_ENOTFOUND;
        }
    }
    protobuf_set(serial, PACKET_INDEX, index);
    if ((ret = serial_raw(serial, self->command, strlen(self->command), buf, BUFSIZE, NULL)) == AAOS_OK) {
        const char *s = buf + 1;
        data[0] = atof(s);
    } else {
        switch (ret) {
            case -1 * AAOS_EPIPE:
            case -1 * AAOS_ECONNRESET:
            case -1 * AAOS_ETIMEDOUT:
            case -1 * AAOS_ENETDOWN:
            case -1 * AAOS_ENETUNREACH:
                if ((ret = KLAWSController_try_connect(self->controller)) == AAOS_OK && (ret = serial_raw(serial, self->command, strlen(self->command), buf, BUFSIZE, NULL)) == AAOS_OK) {
                    const char *s = buf + 1;
                    data[0] = atof(s);
                }
                break;
            default:
                break;
        }
    }
    klaws_controller_unlock(controller);
        
    return ret;
}
 
static int
Young41382VCR_read_raw_data(void *_self, void *data, size_t size)
{
    struct Sensor *self = cast(Sensor(), _self);

    int ret;
    char buf[BUFSIZE];
    void *controller = self->controller, *device = self->device;
    void *serial = klaws_controller_get_serial(controller);
    uint16_t index = klaws_device_get_index(device);
    
    klaws_controller_lock(controller);
    if (index == 0) {
        klaws_device_set_index(device, serial);
        index = klaws_device_get_index(device);
        if (index == 0) {
            klaws_controller_unlock(controller);
            return AAOS_ENOTFOUND;
        }
    }
    protobuf_set(serial, PACKET_INDEX, index);
    if ((ret = serial_raw(serial, self->command, strlen(self->command), buf, BUFSIZE, NULL)) == AAOS_OK) {
        const char *s = buf + 1;
        snprintf(data, size, "%s", s);
    } else {
        switch (ret) {
            case -1 * AAOS_EPIPE:
            case -1 * AAOS_ECONNRESET:
            case -1 * AAOS_ETIMEDOUT:
            case -1 * AAOS_ENETDOWN:
            case -1 * AAOS_ENETUNREACH:
                if ((ret = KLAWSController_try_connect(self->controller)) == AAOS_OK && (ret = serial_raw(serial, self->command, strlen(self->command), buf, BUFSIZE, NULL)) == AAOS_OK) {
                    const char *s = buf + 1;
                    snprintf(data, size, "%s", s);
                }
                break;
            default:
                break;
        }
    }
    klaws_controller_unlock(controller);
    
    return ret;
}

static const void *_young41382vcr_virtual_table;

static void
young41382vcr_virtual_table_destroy(void)
{
    delete((void *) _young41382vcr_virtual_table);
}


static void
young41382vcr_virtual_table_initialize(void)
{
    _young41382vcr_virtual_table = new(SensorVirtualTable(),
                                    sensor_read_data, "read_data", Young41382VCR_read_data,
                                    sensor_read_raw_data, "read_raw_data", Young41382VCR_read_raw_data,
                                    (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(young41382vcr_virtual_table_destroy);
#endif
}

static const void *
young41382vcr_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, young41382vcr_virtual_table_initialize);
#endif
    
    return _young41382vcr_virtual_table;
}

/*
 * Young Young41382VC relative humodity sensor.
 */
static const void *young41382vct_virtual_table(void);

static void *
Young41382VCT_ctor(void *_self, va_list *app)
{
    struct Young41382VCT *self = super_ctor(Young41382VCT(), _self, app);
    
    self->_._vtab= young41382vct_virtual_table();
    
    return (void *) self;
}

static void *
Young41382VCT_dtor(void *_self)
{
    return super_dtor(Young41382VCT(), _self);
}

static void *
Young41382VCTClass_ctor(void *_self, va_list *app)
{
    struct Young41382VCTClass *self = super_ctor(Young41382VCTClass(), _self, app);
    
    self->_.read_data.method = (Method) 0;
    self->_.read_raw_data.method = (Method) 0;
    
    return self;
}

static const void *_Young41382VCTClass;

static void
Young41382VCTClass_destroy(void)
{
    free((void *)_Young41382VCTClass);
}

static void
Young41382VCTClass_initialize(void)
{
    _Young41382VCTClass = new(SensorClass(), "Young41382VCTClass", SensorClass(), sizeof(struct Young41382VCTClass),
                           ctor, "", Young41382VCTClass_ctor,
                           (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(Young41382VCTClass_destroy);
#endif
}

const void *
Young41382VCTClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, Young41382VCTClass_initialize);
#endif
    
    return _Young41382VCTClass;
}

static const void *_Young41382VCT;

static void
Young41382VCT_destroy(void)
{
    free((void *)_Young41382VCT);
}

static void
Young41382VCT_initialize(void)
{
    
    _Young41382VCT = new(Young41382VCTClass(), "Young41382VCT", Sensor(), sizeof(struct Young41382VCT),
                     ctor, "ctor", Young41382VCT_ctor,
                     dtor, "dtor", Young41382VCT_dtor,
                     (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(Young41382VCT_destroy);
#endif
}

const void *
Young41382VCT(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, Young41382VCT_initialize);
#endif

    return _Young41382VCT;
}

static int
Young41382VCT_read_data(void *_self, double *data, size_t size)
{
    struct Sensor *self = cast(Sensor(), _self);

    int ret;
    char buf[BUFSIZE];
    void *controller = self->controller, *device = self->device;
    void *serial = klaws_controller_get_serial(controller);
    uint16_t index = klaws_device_get_index(device);
    
    klaws_controller_lock(controller);
    if (index == 0) {
        klaws_device_set_index(device, serial);
        index = klaws_device_get_index(device);
        if (index == 0) {
            klaws_controller_unlock(controller);
            return AAOS_ENOTFOUND;
        }
    }
    protobuf_set(serial, PACKET_INDEX, index);
    if ((ret = serial_raw(serial, self->command, strlen(self->command), buf, BUFSIZE, NULL)) == AAOS_OK) {
        const char *s = buf + 1;
        data[0] = atof(s);
        data[0] = data[0] * 100. - 50.;
    } else {
        switch (ret) {
            case -1 * AAOS_EPIPE:
            case -1 * AAOS_ECONNRESET:
            case -1 * AAOS_ETIMEDOUT:
            case -1 * AAOS_ENETDOWN:
            case -1 * AAOS_ENETUNREACH:
                if ((ret = KLAWSController_try_connect(self->controller)) == AAOS_OK && (ret = serial_raw(serial, self->command, strlen(self->command), buf, BUFSIZE, NULL)) == AAOS_OK) {
                    const char *s = buf + 1;
                    data[0] = atof(s);
                    data[0] = data[0] * 100. - 50.;
                }
                break;
            default:
                break;
        }
    }
    klaws_controller_unlock(controller);
        
    return ret;
}
 
static int
Young41382VCT_read_raw_data(void *_self, void *data, size_t size)
{
    struct Sensor *self = cast(Sensor(), _self);
    
    int ret;
    char buf[BUFSIZE];
    void *controller = self->controller, *device = self->device;
    void *serial = klaws_controller_get_serial(controller);
    uint16_t index = klaws_device_get_index(device);
    
    klaws_controller_lock(controller);
    if (index == 0) {
        klaws_device_set_index(device, serial);
        index = klaws_device_get_index(device);
        if (index == 0) {
            klaws_controller_unlock(controller);
            return AAOS_ENOTFOUND;
        }
    }
    protobuf_set(serial, PACKET_INDEX, index);
    if ((ret = serial_raw(serial, self->command, strlen(self->command), buf, BUFSIZE, NULL)) == AAOS_OK) {
        const char *s = buf + 1;
        snprintf(data, size, "%s", s);
    } else {
        switch (ret) {
            case -1 * AAOS_EPIPE:
            case -1 * AAOS_ECONNRESET:
            case -1 * AAOS_ETIMEDOUT:
            case -1 * AAOS_ENETDOWN:
            case -1 * AAOS_ENETUNREACH:
                if ((ret = KLAWSController_try_connect(self->controller)) == AAOS_OK && (ret = serial_raw(serial, self->command, strlen(self->command), buf, BUFSIZE, NULL)) == AAOS_OK) {
                    const char *s = buf + 1;
                    snprintf(data, size, "%s", s);
                }
                break;
            default:
                break;
        }
    }
    klaws_controller_unlock(controller);
    
    return ret;
}

static const void *_young41382vct_virtual_table;

static void
young41382vct_virtual_table_destroy(void)
{
    delete((void *) _young41382vct_virtual_table);
}


static void
young41382vct_virtual_table_initialize(void)
{
    _young41382vct_virtual_table = new(SensorVirtualTable(),
                                    sensor_read_data, "read_data", Young41382VCT_read_data,
                                    sensor_read_raw_data, "read_raw_data", Young41382VCT_read_raw_data,
                                    (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(young41382vct_virtual_table_destroy);
#endif
}

static const void *
young41382vct_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, young41382vct_virtual_table_initialize);
#endif
    
    return _young41382vct_virtual_table;
}

/*
 * Young Young41382VC relative humodity sensor.
 */
static const void *sky_quality_monitor_virtual_table(void);

static void *
SkyQualityMonitor_ctor(void *_self, va_list *app)
{
    struct SkyQualityMonitor *self = super_ctor(SkyQualityMonitor(), _self, app);
    
    self->_._vtab= sky_quality_monitor_virtual_table();
    self->extinction = 0.;
    
    const char *key;
    
    while ((key = va_arg(*app, const char *))) {
        if (strcmp(key, "extinction") == 0) {
            self->extinction = va_arg(*app, double);
        }
        continue;
    }
    
    return (void *) self;
}

static void *
SkyQualityMonitor_dtor(void *_self)
{
    return super_dtor(SkyQualityMonitor(), _self);
}

static void *
SkyQualityMonitorClass_ctor(void *_self, va_list *app)
{
    struct SkyQualityMonitorClass *self = super_ctor(SkyQualityMonitorClass(), _self, app);
    
    self->_.read_data.method = (Method) 0;
    self->_.read_raw_data.method = (Method) 0;
    
    return self;
}

static const void *_SkyQualityMonitorClass;

static void
SkyQualityMonitorClass_destroy(void)
{
    free((void *)_SkyQualityMonitorClass);
}

static void
SkyQualityMonitorClass_initialize(void)
{
    _SkyQualityMonitorClass = new(SensorClass(), "SkyQualityMonitorClass", SensorClass(), sizeof(struct SkyQualityMonitorClass),
                           ctor, "", SkyQualityMonitorClass_ctor,
                           (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(SkyQualityMonitorClass_destroy);
#endif
}

const void *
SkyQualityMonitorClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, SkyQualityMonitorClass_initialize);
#endif
    
    return _SkyQualityMonitorClass;
}

static const void *_SkyQualityMonitor;

static void
SkyQualityMonitor_destroy(void)
{
    free((void *)_SkyQualityMonitor);
}

static void
SkyQualityMonitor_initialize(void)
{
    
    _SkyQualityMonitor = new(SkyQualityMonitorClass(), "SkyQualityMonitor", Sensor(), sizeof(struct SkyQualityMonitor),
                     ctor, "ctor", SkyQualityMonitor_ctor,
                     dtor, "dtor", SkyQualityMonitor_dtor,
                     (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(SkyQualityMonitor_destroy);
#endif
}

const void *
SkyQualityMonitor(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, SkyQualityMonitor_initialize);
#endif

    return _SkyQualityMonitor;
}

static int
SkyQualityMonitor_read_data(void *_self, double *data, size_t size)
{
    struct Sensor *self = cast(Sensor(), _self);
    struct SkyQualityMonitor *myself = cast(SkyQualityMonitor(), _self);
    
    int ret;
    char buf[BUFSIZE];
    void *controller = self->controller, *device = self->device;
    void *serial = klaws_controller_get_serial(controller);
    uint16_t index = klaws_device_get_index(device);
    
    klaws_controller_lock(controller);
    if (index == 0) {
        klaws_device_set_index(device, serial);
        index = klaws_device_get_index(device);
        if (index == 0) {
            klaws_controller_unlock(controller);
            return AAOS_ENOTFOUND;
        }
    }
    protobuf_set(serial, PACKET_INDEX, index);
    if ((ret = serial_raw(serial, self->command, strlen(self->command), buf, BUFSIZE, NULL)) == AAOS_OK) {
        size_t n = min(size, 5), i;
        size_t start[] = {3, 10, 23, 35, 48}, end[] = {8, 20, 33, 46, 54};
        char mybuf[32];
        memset(mybuf, '\0', 32);
        memcpy(mybuf, buf + start[0], end[0] - start[0]);
        data[0] = atof(mybuf) - myself->extinction;
        for (i = 1; i < n; i++) {
            memset(mybuf, '\0', 32);
            memcpy(mybuf, buf + start[i], end[i] - start[i]);
            data[i] = atof(mybuf);
        }
    } else {
        switch (ret) {
            case -1 * AAOS_EPIPE:
            case -1 * AAOS_ECONNRESET:
            case -1 * AAOS_ETIMEDOUT:
            case -1 * AAOS_ENETDOWN:
            case -1 * AAOS_ENETUNREACH:
                if ((ret = KLAWSController_try_connect(self->controller)) == AAOS_OK && (ret = serial_raw(serial, self->command, strlen(self->command), buf, BUFSIZE, NULL)) == AAOS_OK) {
                    size_t n = min(size, 5), i;
                    size_t start[] = {3, 10, 23, 35, 48}, end[] = {8, 20, 33, 46, 54};
                    char mybuf[32];
                    memset(mybuf, '\0', 32);
                    memcpy(mybuf, buf + start[0], end[0] - start[0]);
                    data[0] = atof(mybuf) - myself->extinction;
                    for (i = 1; i < n; i++) {
                        memset(mybuf, '\0', 32);
                        memcpy(mybuf, buf + start[i], end[i] - start[i]);
                        data[i] = atof(mybuf);
                    }
                }
                break;
            default:
                break;
        }
    }
    klaws_controller_unlock(controller);
        
    return ret;
}
 
static int
SkyQualityMonitor_read_raw_data(void *_self, void *data, size_t size)
{
    struct Sensor *self = cast(Sensor(), _self);

    int ret;
    char buf[BUFSIZE];
    void *controller = self->controller, *device = self->device;
    void *serial = klaws_controller_get_serial(controller);
    uint16_t index = klaws_device_get_index(device);
    
    klaws_controller_lock(controller);
    if (index == 0) {
        klaws_device_set_index(device, serial);
        index = klaws_device_get_index(device);
        if (index == 0) {
            klaws_controller_unlock(controller);
            return AAOS_ENOTFOUND;
        }
    }
    protobuf_set(serial, PACKET_INDEX, index);
    if ((ret = serial_raw(serial, self->command, strlen(self->command), buf, BUFSIZE, NULL)) == AAOS_OK) {
        const char *s = buf;
        snprintf(data, size, "%s", s);
    } else {
        switch (ret) {
            case -1 * AAOS_EPIPE:
            case -1 * AAOS_ECONNRESET:
            case -1 * AAOS_ETIMEDOUT:
            case -1 * AAOS_ENETDOWN:
            case -1 * AAOS_ENETUNREACH:
                if ((ret = KLAWSController_try_connect(self->controller)) == AAOS_OK && (ret = serial_raw(serial, self->command, strlen(self->command), buf, BUFSIZE, NULL)) == AAOS_OK) {
                    const char *s = buf;
                    snprintf(data, size, "%s", s);
                }
                break;
            default:
                break;
        }
    }
    klaws_controller_unlock(controller);
    
    return ret;
}

static const void *_sky_quality_monitor_virtual_table;

static void
sky_quality_monitor_virtual_table_destroy(void)
{
    delete((void *) _sky_quality_monitor_virtual_table);
}


static void
sky_quality_monitor_virtual_table_initialize(void)
{
    _sky_quality_monitor_virtual_table = new(SensorVirtualTable(),
                                    sensor_read_data, "read_data", SkyQualityMonitor_read_data,
                                    sensor_read_raw_data, "read_raw_data", SkyQualityMonitor_read_raw_data,
                                    (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(sky_quality_monitor_virtual_table_destroy);
#endif
}

static const void *
sky_quality_monitor_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, sky_quality_monitor_virtual_table_initialize);
#endif
    
    return _sky_quality_monitor_virtual_table;
}

/*
 * WS100UMB sensor.
 */

static const void *ws100umb_virtual_table(void);

static void *
WS100UMB_ctor(void *_self, va_list *app)
{
    struct WS100UMB *self = super_ctor(WS100UMB(), _self, app);
    
    self->_._vtab= ws100umb_virtual_table();
    
    return (void *) self;
}

static void *
WS100UMB_dtor(void *_self)
{
    return super_dtor(WS100UMB(), _self);
}

static void *
WS100UMBClass_ctor(void *_self, va_list *app)
{
    struct WS100UMBClass *self = super_ctor(WS100UMBClass(), _self, app);
    
    self->_.read_data.method = (Method) 0;
    self->_.read_raw_data.method = (Method) 0;
    self->_.format_put.method = (Method) 0;
    
    return self;
}

static const void *_WS100UMBClass;

static void
WS100UMBClass_destroy(void)
{
    free((void *)_WS100UMBClass);
}

static void
WS100UMBClass_initialize(void)
{
    _WS100UMBClass = new(SensorClass(), "WS100UMBClass", SensorClass(), sizeof(struct WS100UMBClass),
                           ctor, "", WS100UMBClass_ctor,
                           (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(WS100UMBClass_destroy);
#endif
}

const void *
WS100UMBClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, WS100UMBClass_initialize);
#endif
    
    return _WS100UMBClass;
}

static const void *_WS100UMB;

static void
WS100UMB_destroy(void)
{
    free((void *)_WS100UMB);
}

static void
WS100UMB_initialize(void)
{
    
    _WS100UMB = new(WS100UMBClass(), "WS100UMB", Sensor(), sizeof(struct WS100UMB),
                    ctor, "ctor", WS100UMB_ctor,
                    dtor, "dtor", WS100UMB_dtor,
                    (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(WS100UMB_destroy);
#endif
}

const void *
WS100UMB(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, WS100UMB_initialize);
#endif

    return _WS100UMB;
}

static int
WS100UMB_read_data(void *_self, double *data, size_t size)
{
    struct Sensor *self = cast(Sensor(), _self);

    int ret;
    char buf[BUFSIZE];
    void *controller = self->controller, *device = self->device;
    void *serial = klaws_controller_get_serial(controller);
    uint16_t index = klaws_device_get_index(device);
    
    klaws_controller_lock(controller);
    if (index == 0) {
        klaws_device_set_index(device, serial);
        index = klaws_device_get_index(device);
        if (index == 0) {
            klaws_controller_unlock(controller);
            return AAOS_ENOTFOUND;
        }
    }
    protobuf_set(serial, PACKET_INDEX, index);
    if ((ret = serial_raw(serial, self->command, strlen(self->command), buf, BUFSIZE, NULL)) == AAOS_OK) {
        size_t n = min(size, 10), i;
        size_t start[] = {89, 76, 97, 5, 15, 25, 35, 45, 56, 66}, end[] = {93, 85, 104, 11, 21, 31, 41, 52, 62, 72};
        char mybuf[32];
        for (i = 0; i < n; i++) {
            memset(mybuf, '\0', 32);
            memcpy(mybuf, buf + start[i], end[i] - start[i]);
            data[i] = atof(mybuf);
        }
    } else {
        switch (ret) {
            case -1 * AAOS_EPIPE:
            case -1 * AAOS_ECONNRESET:
            case -1 * AAOS_ETIMEDOUT:
            case -1 * AAOS_ENETDOWN:
            case -1 * AAOS_ENETUNREACH:
                if ((ret = KLAWSController_try_connect(self->controller)) == AAOS_OK && (ret = serial_raw(serial, self->command, strlen(self->command), buf, BUFSIZE, NULL)) == AAOS_OK) {
                    size_t n = min(size, 10), i;
                    size_t start[] = {89, 76, 97, 5, 15, 25, 35, 45, 56, 66}, end[] = {93, 85, 104, 11, 21, 31, 41, 52, 62, 72};
                    char mybuf[32];
                    for (i = 0; i < n; i++) {
                        memset(mybuf, '\0', 32);
                        memcpy(mybuf, buf + start[i], end[i] - start[i]);
                        data[i] = atof(mybuf);
                    }
                }
                break;
            default:
                break;
        }
    }
    klaws_controller_unlock(controller);
        
    return ret;
}
 
static int
WS100UMB_read_raw_data(void *_self, void *data, size_t size)
{
    struct Sensor *self = cast(Sensor(), _self);

    int ret;
    char buf[BUFSIZE];
    void *controller = self->controller, *device = self->device;
    void *serial = klaws_controller_get_serial(controller);
    uint16_t index = klaws_device_get_index(device);
    
    klaws_controller_lock(controller);
    if (index == 0) {
        klaws_device_set_index(device, serial);
        index = klaws_device_get_index(device);
        if (index == 0) {
            klaws_controller_unlock(controller);
            return AAOS_ENOTFOUND;
        }
    }
    protobuf_set(serial, PACKET_INDEX, index);
    if ((ret = serial_raw(serial, self->command, strlen(self->command), buf, BUFSIZE, NULL)) == AAOS_OK) {
        const char *s = buf;
        snprintf(data, size, "%s", s);
    } else {
        switch (ret) {
            case -1 * AAOS_EPIPE:
            case -1 * AAOS_ECONNRESET:
            case -1 * AAOS_ETIMEDOUT:
            case -1 * AAOS_ENETDOWN:
            case -1 * AAOS_ENETUNREACH:
                if ((ret = KLAWSController_try_connect(self->controller)) == AAOS_OK && (ret = serial_raw(serial, self->command, strlen(self->command), buf, BUFSIZE, NULL)) == AAOS_OK) {
                    const char *s = buf;
                    snprintf(data, size, "%s", s);
                }
                break;
            default:
                break;
        }
    }
    klaws_controller_unlock(controller);
    
    return ret;
}

static void
WS100UMB_format_put(void *_self, FILE *fp)
{
    struct Sensor *self = cast(Sensor(), _self);
    
    double data[10];
    
    sensor_read_data(_self, data, 10);
    
    fprintf(fp, "%d ", (int) data[0]);
    if (self->format == NULL) {
        fprintf(fp, "%.2f ", data[1]);
        fprintf(fp, "%.2f", data[2]);
    } else {
        fprintf(fp, self->format, data[1]);
        fputc(' ', fp);
        fprintf(fp, self->format, data[2]);
    }
}

static const void *_ws100umb_virtual_table;

static void
ws100umb_virtual_table_destroy(void)
{
    delete((void *) _ws100umb_virtual_table);
}


static void
ws100umb_virtual_table_initialize(void)
{
    _ws100umb_virtual_table = new(SensorVirtualTable(),
                                  sensor_read_data, "read_data", WS100UMB_read_data,
                                  sensor_read_raw_data, "read_raw_data", WS100UMB_read_raw_data,
                                  sensor_format_put, "format_put", WS100UMB_format_put,
                                  (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(ws100umb_virtual_table_destroy);
#endif
}

static const void *
ws100umb_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, ws100umb_virtual_table_initialize);
#endif
    
    return _ws100umb_virtual_table;
}

/*
 * HCD6817C sensor.
 */

static const void *hcd6817c_virtual_table(void);

static void *
HCD6817C_ctor(void *_self, va_list *app)
{
    struct HCD6817C *self = super_ctor(HCD6817C(), _self, app);
    
    self->_._vtab= hcd6817c_virtual_table();
    
    self->n_function = va_arg(*app, size_t);
    
    size_t i;
    self->functions = (unsigned int *) Malloc(sizeof(unsigned int) * self->n_function);
    memset(self->functions, '\0', sizeof(unsigned int) * self->n_function);
    const char *s;
    
    for (i = 0; i < self->n_function; i++) {
        s = va_arg(*app, const char *);
        if (strcmp(s, "temperature") == 0) {
            self->functions[i] = SENSOR_TYPE_TEMEPRATURE;
        } else if (strcmp(s, "wind speed") == 0) {
            self->functions[i] = SENSOR_TYPE_WIND_SPEED;
        } else if (strcmp(s, "wind direction") == 0) {
            self->functions[i] = SENSOR_TYPE_WIND_DIRECTION;
        } else if (strcmp(s, "air pressure") == 0) {
            self->functions[i] = SENSOR_TYPE_AIR_PRESSURE;
        } else if (strcmp(s, "humidity") == 0) {
            self->functions[i] = SENSOR_TYPE_RELATIVE_HUMIDITY;
        } else if (strcmp(s, "precipitation") == 0) {
            self->functions[i] = SENSOR_TYPE_PRECIPITATION;
        } else if (strcmp(s, "radiation") == 0) {
            self->functions[i] = SENSOR_TYPE_RADIATION;
        } else if (strcmp(s, "sky quality") == 0) {
            self->functions[i] = SENSOR_TYPE_SKY_QUALITY;
        }
    }
    return (void *) self;
}

static void *
HCD6817C_dtor(void *_self)
{
    struct HCD6817C *self = cast(HCD6817C(), _self);
    
    free(self->functions);
    
    return super_dtor(HCD6817C(), _self);
}
 
static void *
HCD6817CClass_ctor(void *_self, va_list *app)
{
    struct HCD6817CClass *self = super_ctor(HCD6817CClass(), _self, app);
    
    self->_.read_data.method = (Method) 0;
    self->_.read_raw_data.method = (Method) 0;
    self->_.format_put.method = (Method) 0;
    
    return self;
}

static const void *_HCD6817CClass;

static void
HCD6817CClass_destroy(void)
{
    free((void *)_HCD6817CClass);
}

static void
HCD6817CClass_initialize(void)
{
    _HCD6817CClass = new(SensorClass(), "HCD6817CClass", SensorClass(), sizeof(struct HCD6817CClass),
                           ctor, "", HCD6817CClass_ctor,
                           (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(HCD6817CClass_destroy);
#endif
}

const void *
HCD6817CClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, HCD6817CClass_initialize);
#endif
    
    return _HCD6817CClass;
}

static const void *_HCD6817C;

static void
HCD6817C_destroy(void)
{
    free((void *)_HCD6817C);
}

static void
HCD6817C_initialize(void)
{
    
    _HCD6817C = new(HCD6817CClass(), "HCD6817C", Sensor(), sizeof(struct HCD6817C),
                    ctor, "ctor", HCD6817C_ctor,
                    dtor, "dtor", HCD6817C_dtor,
                    (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(HCD6817C_destroy);
#endif
}

const void *
HCD6817C(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, HCD6817C_initialize);
#endif

    return _HCD6817C;
}

static int
HCD6817C_read_data(void *_self, double *data, size_t size)
{
    struct Sensor *self = cast(Sensor(), _self);

    int ret;
    char buf[BUFSIZE];
    void *controller = self->controller, *device = self->device;
    void *serial = klaws_controller_get_serial(controller);
    uint16_t index = klaws_device_get_index(device);
    
    klaws_controller_lock(controller);
    if (index == 0) {
        klaws_device_set_index(device, serial);
        index = klaws_device_get_index(device);
        if (index == 0) {
            klaws_controller_unlock(controller);
            return AAOS_ENOTFOUND;
        }
    }
    protobuf_set(serial, PACKET_INDEX, index);
    if ((ret = serial_raw(serial, self->command, strlen(self->command), buf, BUFSIZE, NULL)) == AAOS_OK) {
        size_t n = min(size, 10), i;
        size_t start[] = {89, 76, 97, 5, 15, 25, 35, 45, 56, 66}, end[] = {93, 85, 104, 11, 21, 31, 41, 52, 62, 72};
        char mybuf[32];
        for (i = 0; i < n; i++) {
            memset(mybuf, '\0', 32);
            memcpy(mybuf, buf + start[i], end[i] - start[i]);
            data[i] = atof(mybuf);
        }
    } else {
        switch (ret) {
            case -1 * AAOS_EPIPE:
            case -1 * AAOS_ECONNRESET:
            case -1 * AAOS_ETIMEDOUT:
            case -1 * AAOS_ENETDOWN:
            case -1 * AAOS_ENETUNREACH:
                if ((ret = KLAWSController_try_connect(self->controller)) == AAOS_OK && (ret = serial_raw(serial, self->command, strlen(self->command), buf, BUFSIZE, NULL)) == AAOS_OK) {
                    size_t n = min(size, 10), i;
                    size_t start[] = {89, 76, 97, 5, 15, 25, 35, 45, 56, 66}, end[] = {93, 85, 104, 11, 21, 31, 41, 52, 62, 72};
                    char mybuf[32];
                    for (i = 0; i < n; i++) {
                        memset(mybuf, '\0', 32);
                        memcpy(mybuf, buf + start[i], end[i] - start[i]);
                        data[i] = atof(mybuf);
                    }
                }
                break;
            default:
                break;
        }
    }
    klaws_controller_unlock(controller);
        
    return ret;
}
 
static int
HCD6817C_read_raw_data(void *_self, void *data, size_t size)
{
    struct Sensor *self = cast(Sensor(), _self);

    int ret;
    char buf[BUFSIZE];
    void *controller = self->controller, *device = self->device;
    void *serial = klaws_controller_get_serial(controller);
    uint16_t index = klaws_device_get_index(device);
    
    klaws_controller_lock(controller);
    if (index == 0) {
        klaws_device_set_index(device, serial);
        index = klaws_device_get_index(device);
        if (index == 0) {
            klaws_controller_unlock(controller);
            return AAOS_ENOTFOUND;
        }
    }
    protobuf_set(serial, PACKET_INDEX, index);
    if ((ret = serial_raw(serial, self->command, strlen(self->command), buf, BUFSIZE, NULL)) == AAOS_OK) {
        const char *s = buf;
        snprintf(data, size, "%s", s);
    } else {
        switch (ret) {
            case -1 * AAOS_EPIPE:
            case -1 * AAOS_ECONNRESET:
            case -1 * AAOS_ETIMEDOUT:
            case -1 * AAOS_ENETDOWN:
            case -1 * AAOS_ENETUNREACH:
                if ((ret = KLAWSController_try_connect(self->controller)) == AAOS_OK && (ret = serial_raw(serial, self->command, strlen(self->command), buf, BUFSIZE, NULL)) == AAOS_OK) {
                    const char *s = buf;
                    snprintf(data, size, "%s", s);
                }
                break;
            default:
                break;
        }
    }
    klaws_controller_unlock(controller);
    
    return ret;
}

static void
HCD6817C_format_put(void *_self, FILE *fp)
{
    struct Sensor *self = cast(Sensor(), _self);
    
    double data[10];
    
    sensor_read_data(_self, data, 10);
    
    fprintf(fp, "%d ", (int) data[0]);
    if (self->format == NULL) {
        fprintf(fp, "%.2f ", data[1]);
        fprintf(fp, "%.2f", data[2]);
    } else {
        fprintf(fp, self->format, data[1]);
        fputc(' ', fp);
        fprintf(fp, self->format, data[2]);
    }
}

static const void *_hcd6817c_virtual_table;

static void
hcd6817c_virtual_table_destroy(void)
{
    delete((void *) _hcd6817c_virtual_table);
}


static void
hcd6817c_virtual_table_initialize(void)
{
    _hcd6817c_virtual_table = new(SensorVirtualTable(),
                                  sensor_read_data, "read_data", HCD6817C_read_data,
                                  sensor_read_raw_data, "read_raw_data", HCD6817C_read_raw_data,
                                  sensor_format_put, "format_put", HCD6817C_format_put,
                                  (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(hcd6817c_virtual_table_destroy);
#endif
}

static const void *
hcd6817c_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, hcd6817c_virtual_table_initialize);
#endif
    
    return _hcd6817c_virtual_table;
}


/*
 * AWS virtual table.
 */

static void *
__AWSVirtualTable_ctor(void *_self, va_list *app)
{
    struct __AWSVirtualTable *self = super_ctor(__AWSVirtualTable(), _self, app);
    Method selector;
    
    while ((selector = va_arg(*app, Method))) {
        const char *tag = va_arg(*app, const char *);
        Method method = va_arg(*app, Method);
        
        if (selector == (Method) __aws_get_temperature_by_name) {
            if (tag) {
                               
                self->get_temperature_by_name.tag = tag;
                self->get_temperature_by_name.selector = selector;
            }
            self->get_temperature_by_name.method = method;
            continue;
        }
        if (selector == (Method) __aws_get_relative_humidity_by_name) {
            if (tag) {
                self->get_relative_humidity_by_name.tag = tag;
                self->get_relative_humidity_by_name.selector = selector;
            }
            self->get_relative_humidity_by_name.method = method;
            continue;
        }
        if (selector == (Method) __aws_get_wind_speed_by_name) {
            if (tag) {
                self->get_wind_speed_by_name.tag = tag;
                self->get_wind_speed_by_name.selector = selector;
            }
            self->get_wind_speed_by_name.method = method;
            continue;
        }
        if (selector == (Method) __aws_get_wind_direction_by_name) {
            if (tag) {
                self->get_wind_direction_by_name.tag = tag;
                self->get_wind_direction_by_name.selector = selector;
            }
            self->get_wind_direction_by_name.method = method;
            continue;
        }
        if (selector == (Method) __aws_get_air_pressure_by_name) {
            if (tag) {
                self->get_air_pressure_by_name.tag = tag;
                self->get_air_pressure_by_name.selector = selector;
            }
            self->get_wind_direction_by_name.method = method;
            continue;
        }
        if (selector == (Method) __aws_get_sky_quality_by_name) {
            if (tag) {
                self->get_sky_quality_by_name.tag = tag;
                self->get_sky_quality_by_name.selector = selector;
            }
            self->get_sky_quality_by_name.method = method;
            continue;
        }
        if (selector == (Method) __aws_get_sky_quality_by_name) {
            if (tag) {
                self->get_sky_quality_by_name.tag = tag;
                self->get_sky_quality_by_name.selector = selector;
            }
            self->get_sky_quality_by_name.method = method;
            continue;
        }
        if (selector == (Method) __aws_get_precipitation_by_name) {
            if (tag) {
                self->get_precipitation_by_name.tag = tag;
                self->get_precipitation_by_name.selector = selector;
            }
            self->get_precipitation_by_name.method = method;
            continue;
        }
        if (selector == (Method) __aws_get_temperature_by_channel) {
            if (tag) {
                               
                self->get_temperature_by_channel.tag = tag;
                self->get_temperature_by_channel.selector = selector;
            }
            self->get_temperature_by_channel.method = method;
            continue;
        }
        if (selector == (Method) __aws_get_relative_humidity_by_channel) {
            if (tag) {
                self->get_relative_humidity_by_channel.tag = tag;
                self->get_relative_humidity_by_channel.selector = selector;
            }
            self->get_relative_humidity_by_channel.method = method;
            continue;
        }
        if (selector == (Method) __aws_get_wind_speed_by_channel) {
            if (tag) {
                self->get_wind_speed_by_channel.tag = tag;
                self->get_wind_speed_by_channel.selector = selector;
            }
            self->get_wind_speed_by_channel.method = method;
            continue;
        }
        if (selector == (Method) __aws_get_wind_direction_by_channel) {
            if (tag) {
                self->get_wind_direction_by_channel.tag = tag;
                self->get_wind_direction_by_channel.selector = selector;
            }
            self->get_wind_direction_by_channel.method = method;
            continue;
        }
        if (selector == (Method) __aws_get_air_pressure_by_channel) {
            if (tag) {
                self->get_air_pressure_by_channel.tag = tag;
                self->get_air_pressure_by_channel.selector = selector;
            }
            self->get_wind_direction_by_channel.method = method;
            continue;
        }
        if (selector == (Method) __aws_get_sky_quality_by_channel) {
            if (tag) {
                self->get_sky_quality_by_channel.tag = tag;
                self->get_sky_quality_by_channel.selector = selector;
            }
            self->get_sky_quality_by_channel.method = method;
            continue;
        }
        if (selector == (Method) __aws_get_sky_quality_by_channel) {
            if (tag) {
                self->get_sky_quality_by_channel.tag = tag;
                self->get_sky_quality_by_channel.selector = selector;
            }
            self->get_sky_quality_by_channel.method = method;
            continue;
        }
        if (selector == (Method) __aws_get_precipitation_by_channel) {
            if (tag) {
                self->get_precipitation_by_channel.tag = tag;
                self->get_precipitation_by_channel.selector = selector;
            }
            self->get_precipitation_by_channel.method = method;
            continue;
        }
        if (selector == (Method) __aws_data_log) {
            if (tag) {
                self->data_log.tag = tag;
                self->data_log.selector = selector;
            }
            self->data_log.method = method;
            continue;
        }
        if (selector == (Method) __aws_inspect) {
            if (tag) {
                self->inspect.tag = tag;
                self->inspect.selector = selector;
            }
            self->inspect.method = method;
            continue;
        }
    }
    
    return _self;
}

static void *
__AWSVirtualTable_dtor(void *_self)
{
    return super_dtor(Object(), _self);
}

static const void *___AWSVirtualTable;

static void
__AWSVirtualTable_destroy(void)
{
    free((void *) ___AWSVirtualTable);
}

static void
__AWSVirtualTable_initialize(void)
{
    ___AWSVirtualTable = new(VirtualTableClass(), "AWSVirtualTable", VirtualTable(), sizeof(struct __AWSVirtualTable),
                             ctor, "ctor", __AWSVirtualTable_ctor,
                             dtor, "dtor", __AWSVirtualTable_dtor,
                             (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(__AWSVirtualTable_destroy);
#endif
}

const void *
__AWSVirtualTable(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, __AWSVirtualTable_initialize);
#endif
    
    return ___AWSVirtualTable;
}

/*
 * AWS class
 */
const char *
__aws_get_name(const void *_self)
{
    const struct __AWSClass *class = (const struct __AWSClass *) classOf(_self);
    
    if (isOf(class, __AWSClass()) && class->get_name.method) {
        return ((const char * (*)(const void *)) class->get_name.method)(_self);
    } else {
        const char *result;
        forward(_self, &result, (Method) __aws_get_name, "get_name", _self);
        return result;
    }
}

static const char *
__AWS_get_name(const void *_self)
{
    const struct __AWS *self = cast(__AWS(), _self);
    
    return (const char *) self->name;
}

int
__aws_get_channel_by_name(const void *_self, const char *name, unsigned int *channel)
{
    const struct __AWSClass *class = (const struct __AWSClass *) classOf(_self);
    
    if (isOf(class, __AWSClass()) && class->get_channel_by_name.method) {
        return ((int(*)(const void *, const char *, unsigned int *)) class->get_channel_by_name.method)(_self, name, channel);
    } else {
        int result;
        forward(_self, &result, (Method) __aws_get_channel_by_name, "get_channel_by_name", _self, name, channel);
        return result;
    }
}

static int
__AWS_get_channel_by_name(const void *_self, const char *name, unsigned int *channel)
{
    const struct __AWS *self = cast(__AWS(), _self);
    
    size_t i;
    
    for (i = 0; i < self->n_sensors; i++) {
        const char *s = sensor_get_name(self->sensors[i]);
        if (strcmp(name, s) == 0) {
            *channel = sensor_get_channel(self->sensors[i]);
            return AAOS_OK;
        }
    }

    return AAOS_ENOTFOUND;
}

int
__aws_get_temperature_by_name(void *_self, const char *name, double *temperature, size_t size)
{
    const struct __AWSClass *class = (const struct __AWSClass *) classOf(_self);
    
    if (isOf(class, __AWSClass()) && class->get_temperature_by_name.method) {
        return ((int (*)(void *, const char *, double *, size_t)) class->get_temperature_by_name.method)(_self, name, temperature, size);
        
    } else {
        int result;
        forward(_self, &result, (Method) __aws_get_temperature_by_name, "get_temperature_by_name", _self, name, temperature, size);
        return result;
    }
}

static int
__AWS_get_temperature_by_name(void *_self, const char *name, double *temperature, size_t size)
{
    struct __AWS *self = cast(__AWS(), _self);
    
    size_t i;
    int ret = AAOS_ENOTFOUND;
    for (i = 0; i < self->n_sensors; i++) {
        if (strcmp(sensor_get_name(self->sensors[i]), name) == 0 && sensor_get_type(self->sensors[i]) == SENSOR_TYPE_TEMEPRATURE) {
            ret = sensor_read_data(self->sensors[i], temperature, size);
            break;
        }
    }
    
    return ret;
}

int
__aws_get_temperature_by_channel(void *_self, unsigned int channel, double *temperature, size_t size)
{
    const struct __AWSClass *class = (const struct __AWSClass *) classOf(_self);
    
    if (isOf(class, __AWSClass()) && class->get_temperature_by_channel.method) {
        return ((int (*)(void *, unsigned int, double *, size_t)) class->get_temperature_by_channel.method)(_self, channel, temperature, size);
        
    } else {
        int result;
        forward(_self, &result, (Method) __aws_get_temperature_by_channel, "get_temperature_by_channel", _self, channel, temperature, size);
        return result;
    }
}

static int
__AWS_get_temperature_by_channel(void *_self, unsigned int channel, double *temperature, size_t size)
{
    struct __AWS *self = cast(__AWS(), _self);
    
    channel--;
    if (channel >= self->n_sensors || sensor_get_type(self->sensors[channel]) != SENSOR_TYPE_TEMEPRATURE) {
        return AAOS_ENOTFOUND;
    }
    
    return sensor_read_data(self->sensors[channel], temperature, size);
}

int
__aws_get_wind_speed_by_name(void *_self, const char *name, double *wind_speed, size_t size)
{
    const struct __AWSClass *class = (const struct __AWSClass *) classOf(_self);
    
    if (isOf(class, __AWSClass()) && class->get_wind_speed_by_name.method) {
        return ((int (*)(void *, const char *, double *, size_t)) class->get_wind_speed_by_name.method)(_self, name, wind_speed, size);
        
    } else {
        int result;
        forward(_self, &result, (Method) __aws_get_wind_speed_by_name, "get_wind_speed_by_name", _self, name, wind_speed, size);
        return result;
    }
}

static int
__AWS_get_wind_speed_by_name(void *_self, const char *name, double *wind_speed, size_t size)
{
    struct __AWS *self = cast(__AWS(), _self);
    
    size_t i;
    int ret = AAOS_ENOTFOUND;
    for (i = 0; i < self->n_sensors; i++) {
        if (strcmp(sensor_get_name(self->sensors[i]), name) == 0 && sensor_get_type(self->sensors[i]) == SENSOR_TYPE_WIND_SPEED) {
            ret = sensor_read_data(self->sensors[i], wind_speed, size);
            break;
        }
    }
    
    return ret;
}

int
__aws_get_wind_speed_by_channel(void *_self, unsigned int channel, double *wind_speed, size_t size)
{
    const struct __AWSClass *class = (const struct __AWSClass *) classOf(_self);
    
    if (isOf(class, __AWSClass()) && class->get_wind_speed_by_channel.method) {
        return ((int (*)(void *, unsigned int, double *, size_t)) class->get_wind_speed_by_channel.method)(_self, channel, wind_speed, size);
        
    } else {
        int result;
        forward(_self, &result, (Method) __aws_get_wind_speed_by_channel, "get_wind_speed_by_channel", _self, channel, wind_speed, size);
        return result;
    }
}

static int
__AWS_get_wind_speed_by_channel(void *_self, unsigned int channel, double *wind_speed, size_t size)
{
    struct __AWS *self = cast(__AWS(), _self);
    
    channel--;
    if (channel >= self->n_sensors || sensor_get_type(self->sensors[channel]) != SENSOR_TYPE_WIND_SPEED) {
        return AAOS_ENOTFOUND;
    }
    
    return sensor_read_data(self->sensors[channel], wind_speed, size);

}

int
__aws_get_wind_direction_by_name(void *_self, const char *name, double *wind_direction, size_t size)
{
    const struct __AWSClass *class = (const struct __AWSClass *) classOf(_self);
    
    if (isOf(class, __AWSClass()) && class->get_wind_direction_by_name.method) {
        return ((int (*)(void *, const char *, double *, size_t)) class->get_wind_direction_by_name.method)(_self, name, wind_direction, size);
        
    } else {
        int result;
        forward(_self, &result, (Method) __aws_get_wind_direction_by_name, "get_wind_direction_by_name", _self, name, wind_direction, size);
        return result;
    }
}

static int
__AWS_get_wind_direction_by_name(void *_self, const char *name, double *wind_direction, size_t size)
{
    struct __AWS *self = cast(__AWS(), _self);
    
    size_t i;
    int ret = AAOS_ENOTFOUND;
    for (i = 0; i < self->n_sensors; i++) {
        if (strcmp(sensor_get_name(self->sensors[i]), name) == 0 && sensor_get_type(self->sensors[i]) == SENSOR_TYPE_WIND_DIRECTION) {
            ret = sensor_read_data(self->sensors[i], wind_direction, size);
            break;
        }
    }
    
    return ret;
}

int
__aws_get_wind_direction_by_channel(void *_self, unsigned int channel, double *wind_direction, size_t size)
{
    const struct __AWSClass *class = (const struct __AWSClass *) classOf(_self);
    
    if (isOf(class, __AWSClass()) && class->get_wind_direction_by_channel.method) {
        return ((int (*)(void *, unsigned int, double *, size_t)) class->get_wind_direction_by_channel.method)(_self, channel, wind_direction, size);
        
    } else {
        int result;
        forward(_self, &result, (Method) __aws_get_wind_direction_by_channel, "get_wind_direction_by_channel", _self, channel, wind_direction, size);
        return result;
    }
}

static int
__AWS_get_wind_direction_by_channel(void *_self, unsigned int channel, double *wind_direction, size_t size)
{
    struct __AWS *self = cast(__AWS(), _self);
    
    channel--;
    if (channel >= self->n_sensors || sensor_get_type(self->sensors[channel]) != SENSOR_TYPE_WIND_DIRECTION) {
        return AAOS_ENOTFOUND;
    }
    
    return sensor_read_data(self->sensors[channel], wind_direction, size);
}

int
__aws_get_relative_humidity_by_name(void *_self, const char *name, double *relative_humidity, size_t size)
{
    const struct __AWSClass *class = (const struct __AWSClass *) classOf(_self);
    
    if (isOf(class, __AWSClass()) && class->get_relative_humidity_by_name.method) {
        return ((int (*)(void *, const char *, double *, size_t)) class->get_relative_humidity_by_name.method)(_self, name, relative_humidity, size);
        
    } else {
        int result;
        forward(_self, &result, (Method) __aws_get_relative_humidity_by_name, "get_relative_humidity_by_name", _self, name, relative_humidity, size);
        return result;
    }
}

static int
__AWS_get_relative_humidity_by_name(void *_self, const char *name, double *relative_humidity, size_t size)
{
    struct __AWS *self = cast(__AWS(), _self);
    
    size_t i;
    int ret = AAOS_ENOTFOUND;
    for (i = 0; i < self->n_sensors; i++) {
        if (strcmp(sensor_get_name(self->sensors[i]), name) == 0 && sensor_get_type(self->sensors[i]) == SENSOR_TYPE_RELATIVE_HUMIDITY) {
            ret = sensor_read_data(self->sensors[i], relative_humidity, size);
            break;
        }
    }
    
    return ret;
}

int
__aws_get_relative_humidity_by_channel(void *_self, unsigned int channel, double *relative_humidity, size_t size)
{
    const struct __AWSClass *class = (const struct __AWSClass *) classOf(_self);
    
    if (isOf(class, __AWSClass()) && class->get_relative_humidity_by_channel.method) {
        return ((int (*)(void *, unsigned int, double *, size_t)) class->get_relative_humidity_by_channel.method)(_self, channel, relative_humidity, size);
        
    } else {
        int result;
        forward(_self, &result, (Method) __aws_get_relative_humidity_by_channel, "get_relative_humidity_by_channel", _self, channel, relative_humidity, size);
        return result;
    }
}

static int
__AWS_get_relative_humidity_by_channel(void *_self, unsigned int channel, double *relative_humidity, size_t size)
{
    struct __AWS *self = cast(__AWS(), _self);
    
    channel--;
    if (channel >= self->n_sensors || sensor_get_type(self->sensors[channel]) != SENSOR_TYPE_RELATIVE_HUMIDITY) {
        return AAOS_ENOTFOUND;
    }
    
    return sensor_read_data(self->sensors[channel], relative_humidity, size);
}

int
__aws_get_air_pressure_by_name(void *_self, const char *name, double *air_pressure, size_t size)
{
    const struct __AWSClass *class = (const struct __AWSClass *) classOf(_self);
    
    if (isOf(class, __AWSClass()) && class->get_air_pressure_by_name.method) {
        return ((int (*)(void *, const char *, double *, size_t)) class->get_air_pressure_by_name.method)(_self, name, air_pressure, size);
        
    } else {
        int result;
        forward(_self, &result, (Method) __aws_get_air_pressure_by_name, "get_air_pressure_by_name", _self, name, air_pressure, size);
        return result;
    }
}

static int
__AWS_get_air_pressure_by_name(void *_self, const char *name, double *air_pressure, size_t size)
{
    struct __AWS *self = cast(__AWS(), _self);
    
    size_t i;
    int ret = AAOS_ENOTFOUND;
    for (i = 0; i < self->n_sensors; i++) {
        if (strcmp(sensor_get_name(self->sensors[i]), name) == 0 && sensor_get_type(self->sensors[i]) == SENSOR_TYPE_AIR_PRESSURE) {
            ret = sensor_read_data(self->sensors[i], air_pressure, size);
            break;
        }
    }
    
    return ret;
}

int
__aws_get_air_pressure_by_channel(void *_self, unsigned int channel, double *air_pressure, size_t size)
{
    const struct __AWSClass *class = (const struct __AWSClass *) classOf(_self);
    
    if (isOf(class, __AWSClass()) && class->get_air_pressure_by_channel.method) {
        return ((int (*)(void *, unsigned int, double *, size_t)) class->get_air_pressure_by_channel.method)(_self, channel, air_pressure, size);
        
    } else {
        int result;
        forward(_self, &result, (Method) __aws_get_air_pressure_by_channel, "get_air_pressure_by_channel", _self, channel, air_pressure, size);
        return result;
    }
}

static int
__AWS_get_air_pressure_by_channel(void *_self, unsigned int channel, double *air_pressure, size_t size)
{
    struct __AWS *self = cast(__AWS(), _self);
    
    channel--;
    if (channel >= self->n_sensors || sensor_get_type(self->sensors[channel]) != SENSOR_TYPE_AIR_PRESSURE) {
        return AAOS_ENOTFOUND;
    }
    
    return sensor_read_data(self->sensors[channel], air_pressure, size);
}

int
__aws_get_sky_quality_by_name(void *_self, const char *name, double *sky_quality, size_t size)
{
    const struct __AWSClass *class = (const struct __AWSClass *) classOf(_self);
    
    if (isOf(class, __AWSClass()) && class->get_sky_quality_by_name.method) {
        return ((int (*)(void *, const char *, double *, size_t)) class->get_sky_quality_by_name.method)(_self, name, sky_quality, size);
        
    } else {
        int result;
        forward(_self, &result, (Method) __aws_get_sky_quality_by_name, "get_sky_quality_by_name", _self, name, sky_quality, size);
        return result;
    }
}

static int
__AWS_get_sky_quality_by_name(void *_self, const char *name, double *sky_quality, size_t size)
{
    struct __AWS *self = cast(__AWS(), _self);
    
    size_t i;
    int ret = AAOS_ENOTFOUND;
    for (i = 0; i < self->n_sensors; i++) {
        if (strcmp(sensor_get_name(self->sensors[i]), name) == 0 && sensor_get_type(self->sensors[i]) == SENSOR_TYPE_SKY_QUALITY) {
            ret = sensor_read_data(self->sensors[i], sky_quality, size);
            break;
        }
    }
    
    return ret;
}

int
__aws_get_sky_quality_by_channel(void *_self, unsigned int channel, double *sky_quality, size_t size)
{
    const struct __AWSClass *class = (const struct __AWSClass *) classOf(_self);
    
    if (isOf(class, __AWSClass()) && class->get_sky_quality_by_channel.method) {
        return ((int (*)(void *, unsigned int, double *, size_t)) class->get_sky_quality_by_channel.method)(_self, channel, sky_quality, size);
        
    } else {
        int result;
        forward(_self, &result, (Method) __aws_get_sky_quality_by_channel, "get_sky_quality_by_channel", _self, channel, sky_quality, size);
        return result;
    }
}

static int
__AWS_get_sky_quality_by_channel(void *_self, unsigned int channel, double *sky_quality, size_t size)
{
    struct __AWS *self = cast(__AWS(), _self);
    
    channel--;
    if (channel >= self->n_sensors || sensor_get_type(self->sensors[channel]) != SENSOR_TYPE_SKY_QUALITY) {
        return AAOS_ENOTFOUND;
    }
    
    return sensor_read_data(self->sensors[channel], sky_quality, size);
}

int
__aws_get_precipitation_by_name(void *_self, const char *name, double *precipitation, size_t size)
{
    const struct __AWSClass *class = (const struct __AWSClass *) classOf(_self);
    
    if (isOf(class, __AWSClass()) && class->get_precipitation_by_name.method) {
        return ((int (*)(void *, const char *, double *, size_t)) class->get_precipitation_by_name.method)(_self, name, precipitation, size);
        
    } else {
        int result;
        forward(_self, &result, (Method) __aws_get_precipitation_by_name, "get_precipitation_by_name", _self, name, precipitation, size);
        return result;
    }
}

static int
__AWS_get_precipitation_by_name(void *_self, const char *name, double *precipitation, size_t size)
{
    struct __AWS *self = cast(__AWS(), _self);
    
    size_t i;
    int ret = AAOS_ENOTFOUND;
    for (i = 0; i < self->n_sensors; i++) {
        if (strcmp(sensor_get_name(self->sensors[i]), name) == 0 && sensor_get_type(self->sensors[i]) == SENSOR_TYPE_PRECIPITATION) {
            ret = sensor_read_data(self->sensors[i], precipitation, size);
            break;
        }
    }
    
    return ret;
}

int
__aws_get_precipitation_by_channel(void *_self, unsigned int channel, double *precipitation, size_t size)
{
    const struct __AWSClass *class = (const struct __AWSClass *) classOf(_self);
    
    if (isOf(class, __AWSClass()) && class->get_precipitation_by_channel.method) {
        return ((int (*)(void *, unsigned int, double *, size_t)) class->get_precipitation_by_channel.method)(_self, channel, precipitation, size);
        
    } else {
        int result;
        forward(_self, &result, (Method) __aws_get_precipitation_by_channel, "get_precipitation_by_channel", _self, channel, precipitation, size);
        return result;
    }
}

static int
__AWS_get_precipitation_by_channel(void *_self, unsigned int channel, double *precipitation, size_t size)
{
    struct __AWS *self = cast(__AWS(), _self);
    
    channel--;
    if (channel >= self->n_sensors || sensor_get_type(self->sensors[channel]) != SENSOR_TYPE_PRECIPITATION) {
        return AAOS_ENOTFOUND;
    }
    
    return sensor_read_data(self->sensors[channel], precipitation, size);
}

int
__aws_get_raw_data_by_name(void *_self, const char *name, void *data, size_t size)
{
    const struct __AWSClass *class = (const struct __AWSClass *) classOf(_self);
    
    if (isOf(class, __AWSClass()) && class->get_raw_data_by_name.method) {
        return ((int (*)(void *, const char *, void *, size_t)) class->get_raw_data_by_name.method)(_self, name, data, size);
    } else {
        int result;
        forward(_self, &result, (Method) __aws_get_raw_data_by_name, "__aws_get_raw_data_by_name", _self, name, data, size);
        return result;
    }
}

static int
__AWS_get_raw_data_by_name(void *_self, const char *name, void *data, size_t size)
{
    struct __AWS *self = cast(__AWS(), _self);
    
    size_t i;
    int ret = AAOS_ENOTFOUND;
    for (i = 0; i < self->n_sensors; i++) {
        if (strcmp(sensor_get_name(self->sensors[i]), name) == 0) {
            ret = sensor_read_raw_data(self->sensors[i], data, size);
            break;
        }
    }
    
    return ret;
}

int
__aws_get_raw_data_by_channel(void *_self, unsigned int channel, void * data, size_t size)
{
    const struct __AWSClass *class = (const struct __AWSClass *) classOf(_self);
    
    if (isOf(class, __AWSClass()) && class->get_raw_data_by_channel.method) {
        return ((int (*)(void *, unsigned int, void *, size_t)) class->get_raw_data_by_channel.method)(_self, channel, data, size);
        
    } else {
        int result;
        forward(_self, &result, (Method) __aws_get_raw_data_by_channel, "get_raw_data_by_channel", _self, channel, data, size);
        return result;
    }
}

static int
__AWS_get_raw_data_by_channel(void *_self, unsigned int channel, void *data, size_t size)
{
    struct __AWS *self = cast(__AWS(), _self);
    
    channel--;
    if (channel >= self->n_sensors) {
        return AAOS_ENOTFOUND;
    }
    
    return sensor_read_raw_data(self->sensors[channel], data, size);
}

void
__aws_set_sensor(void *_self, const void *sensor, size_t index)
{
    const struct __AWSClass *class = (const struct __AWSClass *) classOf(_self);
    
    if (isOf(class, __AWSClass()) && class->set_sensor.method) {
        ((void (*)(void *, const void *, size_t)) class->set_sensor.method)(_self, sensor, index);
    } else {
        forward(_self, 0, (Method) __aws_set_sensor, "set_sensor", _self, sensor, index);
    }
}

static void
__AWS_set_sensor(void *_self, const void *sensor, size_t index)
{
    struct __AWS *self = cast(__AWS(), _self);
    
    if (index < self->n_sensors && self->sensors != NULL) {
        self->sensors[index] = (void *) sensor;
    }
}

void *
__aws_get_sensor(void *_self, size_t index)
{
    const struct __AWSClass *class = (const struct __AWSClass *) classOf(_self);
    
    if (isOf(class, __AWSClass()) && class->set_sensor.method) {
        return ((void * (*)(void *, size_t)) class->get_sensor.method)(_self, index);
    } else {
        void *result;
        forward(_self, &result, (Method) __aws_get_sensor, "get_sensor", _self,  index);
        return result;
    }
}

static void *
__AWS_get_sensor(void *_self, size_t index)
{
    struct __AWS *self = cast(__AWS(), _self);
    
    if (index <= self->n_sensors && self->sensors != NULL) {
        return self->sensors[index - 1];
    } else {
        return NULL;
    }
}

void
__aws_status(void *_self, FILE *fp)
{
    const struct __AWSClass *class = (const struct __AWSClass *) classOf(_self);
    
    if (isOf(class, __AWSClass()) && class->status.method) {
        ((void (*)(void *, FILE *)) class->status.method)(_self, fp);
    } else {
        forward(_self, 0, (Method) __aws_status, "status", _self, fp);
    }
}

static void
__AWS_status(void *_self, FILE *fp)
{
    struct __AWS *self = cast(__AWS(), _self);
    
    size_t i;
    int ret;
    char buf[BUFSIZE];
    
    ret = sensor_read_raw_data(self->sensors[0], buf, BUFSIZE);
    
    if (ret == AAOS_OK) {
        fprintf(fp, "0");
    } else {
        fprintf(fp, "1");
    }
    for (i = 1; i < self->n_sensors; i++) {
        if (self->delimiter) {
            fprintf(fp, "%s", self->delimiter);
        } else {
            fputc(' ', fp);
        }
        ret = sensor_read_raw_data(self->sensors[i], buf, BUFSIZE);
        if (ret == AAOS_OK) {
            fprintf(fp, "0");
        } else {
            fprintf(fp, "1");
        }
    }
    if (self->newline) {
        fprintf(fp, "%s", self->newline);
    } else {
        fputc('\n', fp);
    }
}


void
__aws_data_log(void *_self, FILE *fp)
{
    const struct __AWSClass *class = (const struct __AWSClass *) classOf(_self);
    
    if (isOf(class, __AWSClass()) && class->data_log.method) {
        ((void (*)(void *, FILE *)) class->data_log.method)(_self, fp);
    } else {
        forward(_self, 0, (Method) __aws_data_log, "data_log", _self, fp);
    }
}

static void
__AWS_data_log(void *_self, FILE *fp)
{
    struct __AWS *self = cast(__AWS(), _self);
    
    size_t i;
    sensor_format_put(self->sensors[0], fp);
    for (i = 1; i < self->n_sensors; i++) {
        if (self->delimiter) {
            fprintf(fp, "%s", self->delimiter);
        } else {
            fputc(' ', fp);
        }
        sensor_format_put(self->sensors[i], fp);
    }
    if (self->newline) {
        fprintf(fp, "%s", self->newline);
    } else {
        fputc('\n', fp);
    }
}

void
__aws_data_field(void *_self, FILE *fp)
{
    const struct __AWSClass *class = (const struct __AWSClass *) classOf(_self);
    
    if (isOf(class, __AWSClass()) && class->data_field.method) {
        ((void (*)(void *, FILE *)) class->data_field.method)(_self, fp);
    } else {
        forward(_self, 0, (Method) __aws_data_field, "data_field", _self, fp);
    }
}

static void
__AWS_data_field(void *_self, FILE *fp)
{
    struct __AWS *self = cast(__AWS(), _self);
    
    size_t i;
        
    fprintf(fp, "%s", sensor_get_name(self->sensors[0]));
    
    for (i = 1; i < self->n_sensors; i++) {
        if (self->delimiter) {
            fprintf(fp, "%s", self->delimiter);
        } else {
            fputc(' ', fp);
        }
        fprintf(fp, "%s", sensor_get_name(self->sensors[i]));
    }
    if (self->newline) {
        fprintf(fp, "%s", self->newline);
    } else {
        fputc('\n', fp);
    }
}

int
__aws_wait(void *_self, double timeout)
{
    const struct __AWSClass *class = (const struct __AWSClass *) classOf(_self);
    
    if (isOf(class, __AWSClass()) && class->wait.method) {
        return ((int (*)(void *, double)) class->wait.method)(_self, timeout);
    } else {
        int result;
        forward(_self, &result, (Method) __aws_wait, "wait", _self, timeout);
        return result;
    }
}

static int
__AWS_wait(void *_self, double timeout)
{
    struct __AWS *self = cast(__AWS(), _self);
    
    int ret = 0;
    struct timespec tp;
    
    tp.tv_sec = floor(timeout);
    tp.tv_nsec = (timeout - tp.tv_sec) * 1000000000;
    
    Pthread_mutex_lock(&self->mtx);
    while (self->state == AWS_STATE_ERROR) {
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

int
__aws_inspect(void *_self)
{
    const struct __AWSClass *class = (const struct __AWSClass *) classOf(_self);
    
    if (isOf(class, __AWSClass()) && class->inspect.method) {
        return ((int (*)(void *)) class->inspect.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) __aws_inspect, "inspect", _self);
        return result;
    }
}

static void
__AWS_forward(const void *_self, void *result, Method selector, const char *name, va_list *app)
{
    const struct __AWS *self = cast(__AWS(), _self);
    Method method = virtualTo(self->_vtab, name);
    va_list ap;
    
#ifdef va_copy
    va_copy(ap, *app);
#else
    ap = *app;
#endif
    
    void *obj = va_arg(*app, void *);
    
    if (selector == (Method) __aws_get_temperature_by_name) {
        const char *name = va_arg(*app, const char *);
        double *temperature = va_arg(*app, double *);
        size_t size = va_arg(*app, size_t);
        *((int *) result) = ((int (*)(void *, const char *, double *, size_t)) method)(obj, name, temperature, size);
    } else if (selector == (Method) __aws_get_temperature_by_channel) {
        size_t index = va_arg(*app, size_t);
        double *temperature = va_arg(*app, double *);
        size_t size = va_arg(*app, size_t);
        *((int *) result) = ((int (*)(void *, size_t, double *, size_t)) method)(obj, index, temperature, size);
    } else if (selector == (Method) __aws_inspect) {
        *((int *) result) = ((int (*)(void *)) method)(obj);
    } else {
        assert(0);
    }
    
#ifdef va_copy
    va_end(ap);
#endif
}

static void *
__AWS_ctor(void *_self, va_list *app)
{
    struct __AWS *self = super_ctor(__AWS(), _self, app);
    
    const char *s, *key, *value;
    size_t n_sensors;
    
    s = va_arg(*app, const char *);
    if (s) {
        self->name = (char *) Malloc(strlen(s) + 1);
        snprintf(self->name, strlen(s) + 1, "%s", s);
    }
    
    n_sensors = va_arg(*app, size_t);
    
    self->n_sensors = n_sensors;
    self->sensors = (void **) Malloc(sizeof(void *) * n_sensors);
    if (self->sensors == NULL) {
        free(self->name);
        return NULL;
    }
    memset(self->sensors, '\0', sizeof(void *) * n_sensors);
    
    while ((key = va_arg(*app, const char *))) {
        if (strcmp(key, "description") == 0) {
            value = va_arg(*app, const char *);
            if (value) {
                self->description = (char *) Malloc(strlen(value) + 1);
                snprintf(self->description, strlen(value) + 1, "%s", value);
            }
            continue;
        }
        if (strcmp(key, "delimiter") == 0) {
            value = va_arg(*app, const char *);
            if (value) {
                self->delimiter = (char *) Malloc(strlen(value) + 1);
                snprintf(self->delimiter, strlen(value) + 1, "%s", value);
            }
            continue;
        }
        if (strcmp(key, "newline") == 0) {
            value = va_arg(*app, const char *);
            if (value) {
                self->newline = (char *) Malloc(strlen(value) + 1);
                snprintf(self->newline, strlen(value) + 1, "%s", value);
            }
            continue;
        }
    }
    
    Pthread_mutex_init(&self->mtx, NULL);
    Pthread_cond_init(&self->cond, NULL);
    
    return (void *) self;
    
}


static void *
__AWS_dtor(void *_self)
{
    struct __AWS *self = cast(__AWS(), _self);
    
    free(self->name);
    free(self->description);
    free(self->delimiter);
    free(self->newline);
    
    size_t i;
    
    for (i = 0; i < self->n_sensors; i++) {
        if (self->sensors[i] != NULL) {
            delete(self->sensors[i]);
        }
    }
    Pthread_mutex_destroy(&self->mtx);
    Pthread_cond_destroy(&self->cond);
    
    return super_dtor(__AWS(), _self);
}

static void *
__AWSClass_ctor(void *_self, va_list *app)
{
    struct __AWSClass *self = super_ctor(__AWSClass(), _self, app);
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
        
        if (selector == (Method) __aws_set_sensor) {
            if (tag) {
                               
                self->set_sensor.tag = tag;
                self->set_sensor.selector = selector;
            }
            self->set_sensor.method = method;
            continue;
        }
        if (selector == (Method) __aws_get_sensor) {
            if (tag) {
                               
                self->get_sensor.tag = tag;
                self->get_sensor.selector = selector;
            }
            self->get_sensor.method = method;
            continue;
        }
        if (selector == (Method) __aws_get_name) {
            if (tag) {
                               
                self->get_name.tag = tag;
                self->get_name.selector = selector;
            }
            self->get_name.method = method;
            continue;
        }
        if (selector == (Method) __aws_get_channel_by_name) {
            if (tag) {
                               
                self->get_channel_by_name.tag = tag;
                self->get_channel_by_name.selector = selector;
            }
            self->get_channel_by_name.method = method;
            continue;
        }
        if (selector == (Method) __aws_get_temperature_by_name) {
            if (tag) {
                               
                self->get_temperature_by_name.tag = tag;
                self->get_temperature_by_name.selector = selector;
            }
            self->get_temperature_by_name.method = method;
            continue;
        }
        if (selector == (Method) __aws_get_relative_humidity_by_name) {
            if (tag) {
                self->get_relative_humidity_by_name.tag = tag;
                self->get_relative_humidity_by_name.selector = selector;
            }
            self->get_relative_humidity_by_name.method = method;
            continue;
        }
        if (selector == (Method) __aws_get_wind_speed_by_name) {
            if (tag) {
                self->get_wind_speed_by_name.tag = tag;
                self->get_wind_speed_by_name.selector = selector;
            }
            self->get_wind_speed_by_name.method = method;
            continue;
        }
        if (selector == (Method) __aws_get_wind_direction_by_name) {
            if (tag) {
                self->get_wind_direction_by_name.tag = tag;
                self->get_wind_direction_by_name.selector = selector;
            }
            self->get_wind_direction_by_name.method = method;
            continue;
        }
        if (selector == (Method) __aws_get_air_pressure_by_name) {
            if (tag) {
                self->get_air_pressure_by_name.tag = tag;
                self->get_air_pressure_by_name.selector = selector;
            }
            self->get_air_pressure_by_name.method = method;
            continue;
        }
        if (selector == (Method) __aws_get_sky_quality_by_name) {
            if (tag) {
                self->get_sky_quality_by_name.tag = tag;
                self->get_sky_quality_by_name.selector = selector;
            }
            self->get_sky_quality_by_name.method = method;
            continue;
        }
        if (selector == (Method) __aws_get_sky_quality_by_name) {
            if (tag) {
                self->get_sky_quality_by_name.tag = tag;
                self->get_sky_quality_by_name.selector = selector;
            }
            self->get_sky_quality_by_name.method = method;
            continue;
        }
        if (selector == (Method) __aws_get_precipitation_by_name) {
            if (tag) {
                self->get_precipitation_by_name.tag = tag;
                self->get_precipitation_by_name.selector = selector;
            }
            self->get_precipitation_by_name.method = method;
            continue;
        }
        if (selector == (Method) __aws_get_raw_data_by_name) {
            if (tag) {
                self->get_raw_data_by_name.tag = tag;
                self->get_raw_data_by_name.selector = selector;
            }
            self->get_raw_data_by_name.method = method;
            continue;
        }
        if (selector == (Method) __aws_get_temperature_by_channel) {
            if (tag) {
                               
                self->get_temperature_by_channel.tag = tag;
                self->get_temperature_by_channel.selector = selector;
            }
            self->get_temperature_by_channel.method = method;
            continue;
        }
        if (selector == (Method) __aws_get_relative_humidity_by_channel) {
            if (tag) {
                self->get_relative_humidity_by_channel.tag = tag;
                self->get_relative_humidity_by_channel.selector = selector;
            }
            self->get_relative_humidity_by_channel.method = method;
            continue;
        }
        if (selector == (Method) __aws_get_wind_speed_by_channel) {
            if (tag) {
                self->get_wind_speed_by_channel.tag = tag;
                self->get_wind_speed_by_channel.selector = selector;
            }
            self->get_wind_speed_by_channel.method = method;
            continue;
        }
        if (selector == (Method) __aws_get_wind_direction_by_channel) {
            if (tag) {
                self->get_wind_direction_by_channel.tag = tag;
                self->get_wind_direction_by_channel.selector = selector;
            }
            self->get_wind_direction_by_channel.method = method;
            continue;
        }
        if (selector == (Method) __aws_get_air_pressure_by_channel) {
            if (tag) {
                self->get_air_pressure_by_channel.tag = tag;
                self->get_air_pressure_by_channel.selector = selector;
            }
            self->get_air_pressure_by_channel.method = method;
            continue;
        }
        if (selector == (Method) __aws_get_sky_quality_by_channel) {
            if (tag) {
                self->get_sky_quality_by_channel.tag = tag;
                self->get_sky_quality_by_channel.selector = selector;
            }
            self->get_sky_quality_by_channel.method = method;
            continue;
        }
        if (selector == (Method) __aws_get_sky_quality_by_channel) {
            if (tag) {
                self->get_sky_quality_by_channel.tag = tag;
                self->get_sky_quality_by_channel.selector = selector;
            }
            self->get_sky_quality_by_channel.method = method;
            continue;
        }
        if (selector == (Method) __aws_get_precipitation_by_channel) {
            if (tag) {
                self->get_precipitation_by_channel.tag = tag;
                self->get_precipitation_by_channel.selector = selector;
            }
            self->get_precipitation_by_channel.method = method;
            continue;
        }
        if (selector == (Method) __aws_get_raw_data_by_channel) {
            if (tag) {
                self->get_raw_data_by_channel.tag = tag;
                self->get_raw_data_by_channel.selector = selector;
            }
            self->get_raw_data_by_channel.method = method;
            continue;
        }
        if (selector == (Method) __aws_data_log) {
            if (tag) {
                self->data_log.tag = tag;
                self->data_log.selector = selector;
            }
            self->data_log.method = method;
            continue;
        }
        if (selector == (Method) __aws_data_field) {
            if (tag) {
                self->data_field.tag = tag;
                self->data_field.selector = selector;
            }
            self->data_field.method = method;
            continue;
        }
        if (selector == (Method) __aws_status) {
            if (tag) {
                self->status.tag = tag;
                self->status.selector = selector;
            }
            self->status.method = method;
            continue;
        }
        if (selector == (Method) __aws_inspect) {
            if (tag) {
                self->inspect.tag = tag;
                self->inspect.selector = selector;
            }
            self->inspect.method = method;
            continue;
        }
        if (selector == (Method) __aws_wait) {
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

static const void *___AWSClass;

static void
__AWSClass_destroy(void)
{
    free((void *) ___AWSClass);
}

static void
__AWSClass_initialize(void)
{
    ___AWSClass = new(Class(), "__AWSClass", Class(), sizeof(struct __AWSClass),
                        ctor, "ctor", __AWSClass_ctor,
                        (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(__AWSClass_destroy);
#endif
}

const void *
__AWSClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, __AWSClass_initialize);
#endif
    
    return ___AWSClass;
}

static const void *___AWS;

static void
__AWS_destroy(void)
{
    free((void *)___AWS);
}

static void
__AWS_initialize(void)
{
    ___AWS = new(__AWSClass(), "__AWS", Object(), sizeof(struct __AWS),
                 ctor, "ctor", __AWS_ctor,
                 dtor, "dtor", __AWS_dtor,
                 forward, "forward", __AWS_forward,
                 __aws_get_name, "get_name", __AWS_get_name,
                 __aws_get_channel_by_name, "get_channel_by_name", __AWS_get_channel_by_name,
                 __aws_set_sensor, "set_sensor", __AWS_set_sensor,
                 __aws_get_sensor, "get_sensor", __AWS_get_sensor,
                 __aws_get_temperature_by_name, "get_temperature_by_name", __AWS_get_temperature_by_name,
                 __aws_get_temperature_by_channel, "get_temperature_by_channel", __AWS_get_temperature_by_channel,
                 __aws_get_wind_speed_by_name, "get_wind_speed_by_name", __AWS_get_wind_speed_by_name,
                 __aws_get_wind_speed_by_channel, "get_wind_speed_by_channel", __AWS_get_wind_speed_by_channel,
                 __aws_get_wind_direction_by_name, "get_wind_direction_by_name", __AWS_get_wind_direction_by_name,
                 __aws_get_wind_direction_by_channel, "get_wind_direction_by_channel", __AWS_get_wind_direction_by_channel,
                 __aws_get_relative_humidity_by_name, "get_relative_humidity_by_name", __AWS_get_relative_humidity_by_name,
                 __aws_get_relative_humidity_by_channel, "get_relative_humidity_by_channel", __AWS_get_relative_humidity_by_channel,
                 __aws_get_air_pressure_by_name, "get_air_pressure_by_name", __AWS_get_air_pressure_by_name,
                 __aws_get_air_pressure_by_channel, "get_temperature_by_channel", __AWS_get_air_pressure_by_channel,
                 __aws_get_sky_quality_by_name, "get_sky_quality_by_name", __AWS_get_sky_quality_by_name,
                 __aws_get_sky_quality_by_channel, "get_sky_quality_by_channel", __AWS_get_sky_quality_by_channel,
                 __aws_get_precipitation_by_name, "get_precipitation_by_name", __AWS_get_precipitation_by_name,
                 __aws_get_precipitation_by_channel, "get_precipitation_by_channel", __AWS_get_precipitation_by_channel,
                 __aws_get_raw_data_by_name, "get_raw_data_by_name", __AWS_get_raw_data_by_name,
                 __aws_get_raw_data_by_channel, "get_raw_data_by_channel", __AWS_get_raw_data_by_channel,
                 __aws_data_log, "data_log", __AWS_data_log,
                 __aws_data_field, "data_field", __AWS_data_field,
                 __aws_status, "status", __AWS_status,
                 __aws_wait, "wait", __AWS_wait,
                 (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(__AWS_destroy);
#endif
}

const void *
__AWS(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, __AWS_initialize);
#endif
    
    return ___AWS;
}

/*
 * Kunlun Automated Weather Station.
 */

static const void *klaws_virtual_table(void);

void
klaws_set_controller(void *_self, const void *controller, size_t index)
{
    const struct KLAWSClass *class = (const struct KLAWSClass *) classOf(_self);
    
    if (isOf(class, KLAWSClass()) && class->set_controller.method) {
        ((void (*)(void *, const void *, size_t)) class->set_controller.method)(_self, controller, index);
    } else {
        forward(_self, 0, (Method) klaws_set_controller, "set_controller", _self, controller, index);
    }
}

static void
KLAWS_set_controller(void *_self, const void *controller, size_t index)
{
    struct KLAWS *self = cast(KLAWS(), _self);
    
    if (index < self->n_controller && self->controllers != NULL) {
        self->controllers[index] = (struct KLAWSController *) controller;
    }
}

void *
klaws_get_controller(void *_self, size_t index)
{
    const struct KLAWSClass *class = (const struct KLAWSClass *) classOf(_self);
    
     if (isOf(class, KLAWSClass()) && class->set_controller.method) {
        return ((void * (*)(void *, size_t)) class->get_controller.method)(_self, index);
    } else {
        void *result;
        forward(_self, &result, (Method) klaws_get_controller, "get_controller", _self, index);
        return result;
    }
}

static void *
KLAWS_get_controller(void *_self, size_t index)
{
    struct KLAWS *self = cast(KLAWS(), _self);
    
    if (index >= self->n_controller) {
        return NULL;
    } else {
        return self->controllers[index];
    }
}

static int
KLAWS_inspect(void *_self)
{
    struct KLAWS *self = cast(KLAWS(), _self);
    
    size_t i, n = self->n_controller;
    void *controller;
    int ret;
    
    for (i = 0; i < n; i++) {
        controller = self->controllers[i];
        if ((ret = klaws_controller_inspect(controller)) != AAOS_OK) {
            Pthread_mutex_lock(&self->_.mtx);
            self->_.state = AWS_STATE_ERROR;
            Pthread_mutex_unlock(&self->_.mtx);
            return ret;
        }
    }
    Pthread_mutex_lock(&self->_.mtx);
    self->_.state = AWS_STATE_OK;
    Pthread_mutex_unlock(&self->_.mtx);
    Pthread_cond_broadcast(&self->_.cond);
    
    return AAOS_OK;
}

static void *
KLAWS_ctor(void *_self, va_list *app)
{
    struct KLAWS *self = super_ctor(KLAWS(), _self, app);
    
    size_t n_controller;
    n_controller = va_arg(*app, size_t);
    self->n_controller = n_controller;
    self->controllers = (void **) Malloc(sizeof(void *) * n_controller);
    if (self->controllers == NULL) {
        super_dtor(__AWS(), _self);
        return NULL;
    }
    memset(self->controllers, '\0', sizeof(void *) * n_controller);
    self->_._vtab = klaws_virtual_table();
    
    return (void *) self;
}

static void *
KLAWS_dtor(void *_self)
{
    struct KLAWS *self = cast(KLAWS(), _self);
    
    size_t i;
    
    for (i = 0; i < self->n_controller; i++) {
        if (self->controllers[i] != NULL)  {
            delete(self->controllers[i]);
        }
    }
    free(self->controllers);
    
    return super_dtor(KLAWS(), _self);
}

static void *
KLAWSClass_ctor(void *_self, va_list *app)
{
    struct KLAWSClass *self = super_ctor(KLAWSClass(), _self, app);
    Method selector;
    
#ifdef va_copy
    va_list ap;
    va_copy(ap, *app);
#else
    va_list ap = *app;
#endif
    
    self->_.inspect.method = (Method) 0;
    
    while ((selector = va_arg(ap, Method))) {
        const char *tag = va_arg(ap, const char *);
        Method method = va_arg(ap, Method);
        
        if (selector == (Method) klaws_set_controller) {
            if (tag) {
                               
                self->set_controller.tag = tag;
                self->set_controller.selector = selector;
            }
            self->set_controller.method = method;
            continue;
        }
        if (selector == (Method) klaws_get_controller) {
            if (tag) {
                               
                self->get_controller.tag = tag;
                self->get_controller.selector = selector;
            }
            self->get_controller.method = method;
            continue;
        }
    }
    
#ifdef va_copy
    va_end(ap);
#endif
    
    return (void *) self;
}

static const void *_KLAWSClass;

static void
KLAWSClass_destroy(void)
{
    free((void *) _KLAWSClass);
}

static void
KLAWSClass_initialize(void)
{
    _KLAWSClass = new(__AWSClass(), "KLAWSClass", __AWSClass(), sizeof(struct KLAWSClass),
                      ctor, "ctor", KLAWSClass_ctor,
                      (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(KLAWSClass_destroy);
#endif
}

const void *
KLAWSClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, KLAWSClass_initialize);
#endif
    
    return _KLAWSClass;
}

static const void *_KLAWS;

static void
KLAWS_destroy(void)
{
    free((void *)_KLAWS);
}

static void
KLAWS_initialize(void)
{
    _KLAWS = new(KLAWSClass(), "KLAWS", __AWS(), sizeof(struct KLAWS),
                 ctor, "ctor", KLAWS_ctor,
                 dtor, "dtor", KLAWS_dtor,
                 klaws_set_controller, "set_controller", KLAWS_set_controller,
                 klaws_get_controller, "get_controller", KLAWS_get_controller,
                 (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(KLAWS_destroy);
#endif
}

const void *
KLAWS(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, KLAWS_initialize);
#endif
    
    return _KLAWS;
}

static const void *_klaws_virtual_table;

static void
klaws_virtual_table_destroy(void)
{
    delete((void *) _klaws_virtual_table);
}

static void
klaws_virtual_table_initialize(void)
{
    _klaws_virtual_table = new(__AWSVirtualTable(),
                               __aws_inspect, "inspect", KLAWS_inspect,
                               (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(klaws_virtual_table_destroy);
#endif
}

static const void *
klaws_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, klaws_virtual_table_initialize);
#endif
    
    return _klaws_virtual_table;
}

/*
 * KLAWS controller class
 */

void *
klaws_controller_get_device(void *_self, size_t index)
{
    const struct KLAWSControllerClass *class = (const struct KLAWSControllerClass *) classOf(_self);
    
    if (isOf(class, KLAWSControllerClass()) && class->get_device.method) {
        return ((void * (*)(void *, size_t)) class->get_device.method)(_self, index);
    } else {
        void *result;
        forward(_self, &result, (Method) klaws_controller_get_device, "get_device", _self, index);
        return result;
    }
}

static void *
KLAWSController_get_device(void *_self, size_t index)
{
    struct KLAWSController *self = cast(KLAWSController(), _self);
    
    if (index >= self->n_device) {
        return NULL;
    } else {
        return self->devices[index];
    }
}

void
klaws_controller_set_device(void *_self, const void *device, size_t index)
{
    const struct KLAWSControllerClass *class = (const struct KLAWSControllerClass *) classOf(_self);
    
    if (isOf(class, KLAWSControllerClass()) && class->set_device.method) {
        ((void (*)(void *, const void *, size_t)) class->set_device.method)(_self, device, index);
    } else {
        forward(_self, 0, (Method) klaws_controller_set_device, "set_device", _self, device, index);
    }
}

static void
KLAWSController_set_device(void *_self, const void *device, size_t index)
{
    struct KLAWSController *self = cast(KLAWSController(), _self);
    
    if (index < self->n_device && self->devices != NULL) {
        self->devices[index] = (void *) device;
    }
}

void *
klaws_controller_get_serial(void *_self)
{
    const struct KLAWSControllerClass *class = (const struct KLAWSControllerClass *) classOf(_self);
    
    if (isOf(class, KLAWSControllerClass()) && class->get_serial.method) {
        return ((void * (*)(void *)) class->get_serial.method)(_self);
    } else {
        void *result;
        forward(_self, &result, (Method) klaws_controller_get_serial, "get_serial", _self);
        return result;
    }
}

static void *
KLAWSController_get_serial(void *_self)
{
    struct KLAWSController *self = cast(KLAWSController(), _self);
    
    return self->serial;
}

void
klaws_controller_lock(void *_self)
{
    const struct KLAWSControllerClass *class = (const struct KLAWSControllerClass *) classOf(_self);
    
    if (isOf(class, KLAWSControllerClass()) && class->lock.method) {
        ((void (*)(void *)) class->lock.method)(_self);
    } else {
        forward(_self, 0, (Method) klaws_controller_lock, "lock", _self);
    }
}

static void
KLAWSController_lock(void *_self)
{
    struct KLAWSController *self = cast(KLAWSController(), _self);
    
    Pthread_mutex_lock(&self->mtx);
}

void
klaws_controller_unlock(void *_self)
{
    const struct KLAWSControllerClass *class = (const struct KLAWSControllerClass *) classOf(_self);
    
    if (isOf(class, KLAWSControllerClass()) && class->unlock.method) {
        ((void (*)(void *)) class->unlock.method)(_self);
    } else {
        forward(_self, 0, (Method) klaws_controller_unlock, "unlock", _self);
    }
}

static void
KLAWSController_unlock(void *_self)
{
    struct KLAWSController *self = cast(KLAWSController(), _self);
    
    Pthread_mutex_unlock(&self->mtx);
}


int
klaws_controller_inspect(void *_self)
{
    const struct KLAWSControllerClass *class = (const struct KLAWSControllerClass *) classOf(_self);
    
    if (isOf(class, KLAWSControllerClass()) && class->inspect.method) {
        return ((int (*)(void *)) class->inspect.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) klaws_controller_inspect, "inspect", _self);
        return result;
    }
}

static int
KLAWSController_inspect(void *_self)
{
    struct KLAWSController *self = cast(KLAWSController(), _self);
    
    size_t i, n;
    n = self->n_device;
    void *serial = self->serial, *device;
    int ret;
    int flag = AAOS_OK;
    
    for (i = 0; i < n; i++) {
        /*
         * inspect all the devices attached to the  controller.
         */
        Pthread_mutex_lock(&self->mtx);
        device = self->devices[i];
        if ((ret = klaws_device_inspect(device, serial)) != AAOS_OK) {
            flag = ret;
        }
        Pthread_mutex_unlock(&self->mtx);
    }
    ret = flag;
    
    if (self->critical == 0) {
        return AAOS_OK;
    }
    
    return AAOS_OK;
}

static int
KLAWSController_try_connect(struct KLAWSController *self)
{
    void *old_serial = self->serial, *client;
    int ret;
    
    client = new(SerialClient(), self->address, self->port);
    
    ret = rpc_client_connect(client, &self->serial);
    
    if (old_serial != NULL) {
        delete(old_serial);
    }
    delete(client);
    
    return ret;
}



static void *
KLAWSController_ctor(void *_self, va_list *app)
{
    struct KLAWSController *self = super_ctor(KLAWSController(), _self, app);
    
    const char *address, *port;
    void *client;
    size_t n_device;
    int critical;
    
    address = va_arg(*app, const char *);
    port = va_arg(*app, const char *);
    n_device = va_arg(*app, size_t);
    
    self->address = Malloc(strlen(address) + 1);
    snprintf(self->address, strlen(address) + 1, "%s", address);
    self->port = Malloc(strlen(port) + 1);
    snprintf(self->port, strlen(port) + 1, "%s", port);
    
    self->n_device = n_device;
    self->devices = Malloc(sizeof(void **) *n_device);
    memset(self->devices, '\0', sizeof(void **) *n_device);
    
    critical = va_arg(*app, int);
    self->critical = critical;
    
    client = new(SerialClient(), self->address, self->port);
    rpc_client_connect(client, &self->serial);
    delete(client);
    
    Pthread_mutex_init(&self->mtx, NULL);
    
    return (void *) self;
}

static void *
KLAWSController_dtor(void *_self)
{
    struct KLAWSController *self = cast(KLAWSController(), _self);
    
    size_t i;
    
    free(self->address);
    free(self->port);
    
    Pthread_mutex_destroy(&self->mtx);
    
    for (i = 0; i < self->n_device; i++) {
        if (self->devices[i] != NULL) {
            delete(self->devices);
        }
    }
    if (self->serial != NULL) {
        delete(self->serial);
    }
    
    return super_dtor(KLAWSController(), _self);
}

static void *
KLAWSControllerClass_ctor(void *_self, va_list *app)
{
    struct KLAWSControllerClass *self = super_ctor(KLAWSControllerClass(), _self, app);
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
        
        if (selector == (Method) klaws_controller_get_device) {
            if (tag) {
                self->get_device.tag = tag;
                self->get_device.selector = selector;
            }
            self->get_device.method = method;
            continue;
        }
        if (selector == (Method) klaws_controller_set_device) {
            if (tag) {
                self->set_device.tag = tag;
                self->set_device.selector = selector;
            }
            self->set_device.method = method;
            continue;
        }
        if (selector == (Method) klaws_controller_get_serial) {
            if (tag) {
                self->get_serial.tag = tag;
                self->get_serial.selector = selector;
            }
            self->get_serial.method = method;
            continue;
        }
        if (selector == (Method) klaws_controller_lock) {
            if (tag) {
                self->lock.tag = tag;
                self->lock.selector = selector;
            }
            self->lock.method = method;
            continue;
        }
        if (selector == (Method) klaws_controller_unlock) {
            if (tag) {
                self->unlock.tag = tag;
                self->unlock.selector = selector;
            }
            self->unlock.method = method;
            continue;
        }
        if (selector == (Method) klaws_controller_inspect) {
            if (tag) {
                self->inspect.tag = tag;
                self->inspect.selector = selector;
            }
            self->inspect.method = method;
            continue;
        }
    }
    
#ifdef va_copy
    va_end(ap);
#endif
    
    return (void *) self;
}

static const void *_KLAWSControllerClass;

static void
KLAWSControllerClass_destroy(void)
{
    free((void *) _KLAWSControllerClass);
}

static void
KLAWSControllerClass_initialize(void)
{
    _KLAWSControllerClass = new(Class(), "KLAWSControllerClass", Class(), sizeof(struct KLAWSControllerClass),
                                ctor, "ctor", KLAWSControllerClass_ctor,
                                (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(KLAWSControllerClass_destroy);
#endif
}

const void *
KLAWSControllerClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, KLAWSControllerClass_initialize);
#endif
    
    return _KLAWSControllerClass;
}

static const void *_KLAWSController;

static void
KLAWSController_destroy(void)
{
    free((void *)_KLAWSController);
}

static void
KLAWSController_initialize(void)
{
    _KLAWSController = new(KLAWSControllerClass(), "KLAWSController", Object(), sizeof(struct KLAWSController),
                           ctor, "ctor", KLAWSController_ctor,
                           dtor, "dtor", KLAWSController_dtor,
                           klaws_controller_get_device, "get_device", KLAWSController_get_device,
                           klaws_controller_set_device, "get_device", KLAWSController_set_device,
                           klaws_controller_get_serial, "get_serial", KLAWSController_get_serial,
                           klaws_controller_lock, "lock", KLAWSController_lock,
                           klaws_controller_unlock, "unlock", KLAWSController_unlock,
                           klaws_controller_inspect, "inspect", KLAWSController_inspect,
                           (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(KLAWSController_destroy);
#endif
}

const void *
KLAWSController(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, KLAWSController_initialize);
#endif
    
    return _KLAWSController;
}

/*
 * KLAWS device class
 */


uint16_t
klaws_device_get_index(void *_self)
{
    const struct KLAWSDeviceClass *class = (const struct KLAWSDeviceClass *) classOf(_self);
    
    if (isOf(class, KLAWSDeviceClass()) && class->get_index.method) {
        return ((uint16_t (*)(void *)) class->get_index.method)(_self);
    } else {
        uint16_t result;
        forward(_self, &result, (Method) klaws_device_get_index, "get_index", _self);
        return result;
    }
}

static uint16_t
KLAWSDevice_get_index(void *_self)
{
    struct KLAWSDevice *self = cast(KLAWSDevice(), _self);
    
    return self->index;
}

void
klaws_device_set_index(void *_self, void *serial)
{
    const struct KLAWSDeviceClass *class = (const struct KLAWSDeviceClass *) classOf(_self);
    
    if (isOf(class, KLAWSDeviceClass()) && class->set_index.method) {
        ((void (*)(void *, void *)) class->set_index.method)(_self, serial);
    } else {
        forward(_self, 0, (Method) klaws_device_set_index, "set_index", _self);
    }
}

static void
KLAWSDevice_set_index(void *_self, void *serial)
{
    struct KLAWSDevice *self = cast(KLAWSDevice(), _self);
    
    int ret;
    
    if (self->name == NULL) {
        self->index = 0;
        return;
    }
    
    if ((ret = serial_get_index_by_name(serial, self->name)) == AAOS_OK) {
        protobuf_get(serial, PACKET_INDEX, &self->index);
    } else {
        self->index = 0;
    }
}

void
klaws_device_set_index2(void *_self, void *serial)
{
    const struct KLAWSDeviceClass *class = (const struct KLAWSDeviceClass *) classOf(_self);
    
    if (isOf(class, KLAWSDeviceClass()) && class->set_index2.method) {
        ((void (*)(void *, void *)) class->set_index2.method)(_self, serial);
    } else {
        forward(_self, 0, (Method) klaws_device_set_index2, "set_index2", _self);
    }
}

static void
KLAWSDevice_set_index2(void *_self, void *serial)
{
    struct KLAWSDevice *self = cast(KLAWSDevice(), _self);
    
    int ret;
    
    if (self->name2 == NULL) {
        self->index = 0;
        return;
    }
    
    if ((ret = serial_get_index_by_name(serial, self->name2)) == AAOS_OK) {
        protobuf_get(serial, PACKET_INDEX, &self->index);
    } else {
        self->index = 0;
    }
}

int
klaws_device_inspect(void *_self, void *serial)
{
    const struct KLAWSDeviceClass *class = (const struct KLAWSDeviceClass *) classOf(_self);
    
    if (isOf(class, KLAWSDeviceClass()) && class->inspect.method) {
        return ((int (*)(void *, void *)) class->inspect.method)(_self, serial);
    } else {
        int result;
        forward(_self, &result, (Method) klaws_device_inspect, "inspect", _self, serial);
        return result;
    }
}

static int
KLAWSDevice_inspect(void *_self, void *serial)
{
    struct KLAWSDevice *self = cast(KLAWSDevice(), _self);
    int ret;
    
    
    KLAWSDevice_set_index(self, serial);
    if ((ret = serial_inspect(serial)) != AAOS_OK) {
        /*
         * name1 serial failed, change to name2
         */
        KLAWSDevice_set_index2(self, serial);
        ret = serial_inspect(serial);
    }
    
    /*
     * If this device is not critical, always return AAOS_OK.
     */
    if (self->critical == 0) {
        return AAOS_OK;
    } else {
        return ret;
    }
}

static void *
KLAWSDevice_ctor(void *_self, va_list *app)
{
    struct KLAWSDevice *self = super_ctor(KLAWSDevice(), _self, app);
    
    const char *name;
    int critical;
        
    name = va_arg(*app, const char *);
    self->name = Malloc(strlen(name) + 1);
    snprintf(self->name, strlen(name) + 1, "%s", name);
    name = va_arg(*app, const char *);
    if (name != NULL) {
        self->name2 = Malloc(strlen(name) + 1);
        snprintf(self->name2, strlen(name) + 1, "%s", name);
    }
    critical = va_arg(*app, int);
    self->critical = critical;
    
    return (void *) self;
}

static void *
KLAWSDevice_dtor(void *_self)
{
    struct KLAWSDevice *self = cast(KLAWSDevice(), _self);
    
    free(self->name);
    free(self->name2);
    
    return super_dtor(KLAWSDevice(), _self);
}

static void *
KLAWSDeviceClass_ctor(void *_self, va_list *app)
{
    struct KLAWSDeviceClass *self = super_ctor(KLAWSDeviceClass(), _self, app);
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
        
        if (selector == (Method) klaws_device_get_index) {
            if (tag) {
                self->get_index.tag = tag;
                self->get_index.selector = selector;
            }
            self->get_index.method = method;
            continue;
        }
        if (selector == (Method) klaws_device_set_index) {
            if (tag) {
                self->set_index.tag = tag;
                self->set_index.selector = selector;
            }
            self->set_index.method = method;
            continue;
        }
        if (selector == (Method) klaws_device_set_index2) {
            if (tag) {
                self->set_index2.tag = tag;
                self->set_index2.selector = selector;
            }
            self->set_index2.method = method;
            continue;
        }
        if (selector == (Method) klaws_device_inspect) {
            if (tag) {
                self->inspect.tag = tag;
                self->inspect.selector = selector;
            }
            self->inspect.method = method;
            continue;
        }
    }
    
#ifdef va_copy
    va_end(ap);
#endif
    
    return (void *) self;
}

static const void *_KLAWSDeviceClass;

static void
KLAWSDeviceClass_destroy(void)
{
    free((void *) _KLAWSDeviceClass);
}

static void
KLAWSDeviceClass_initialize(void)
{
    _KLAWSDeviceClass = new(Class(), "KLAWSDeviceClass", Class(), sizeof(struct KLAWSDeviceClass),
                                ctor, "ctor", KLAWSDeviceClass_ctor,
                                (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(KLAWSDeviceClass_destroy);
#endif
}

const void *
KLAWSDeviceClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, KLAWSDeviceClass_initialize);
#endif
    
    return _KLAWSDeviceClass;
}

static const void *_KLAWSDevice;

static void
KLAWSDevice_destroy(void)
{
    free((void *)_KLAWSDevice);
}

static void
KLAWSDevice_initialize(void)
{
    _KLAWSDevice = new(KLAWSDeviceClass(), "KLAWSDevice", Object(), sizeof(struct KLAWSDevice),
                       ctor, "ctor", KLAWSDevice_ctor,
                       dtor, "dtor", KLAWSDevice_dtor,
                       klaws_device_get_index, "get_index", KLAWSDevice_get_index,
                       klaws_device_set_index, "set_index", KLAWSDevice_set_index,
                       klaws_device_set_index2, "set_index2", KLAWSDevice_set_index2,
                       klaws_device_inspect, "inspect", KLAWSDevice_inspect,
                       (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(KLAWSDevice_destroy);
#endif
}

const void *
KLAWSDevice(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, KLAWSDevice_initialize);
#endif
    
    return _KLAWSDevice;
}

#ifdef __USE_GSL__
static void __spline_destructor__(void) __attribute__ ((destructor(101)));
static void
__spline_destructor__(void)
{
    gsl_spline_free(PT100_spline);
    gsl_interp_accel_free(PT100_acc);
}

static void __spline_constructor__(void) __attribute__ ((constructor(101)));

static void
__spline_constructor__(void)
{
    PT100_acc = gsl_interp_accel_alloc();
    PT100_spline = gsl_spline_alloc(gsl_interp_cspline, sizeof(PT100_resistance)/sizeof(double));
    gsl_spline_init(PT100_spline, PT100_resistance, PT100_temperature, sizeof(PT100_resistance)/sizeof(double));
    gsl_set_error_handler_off();
}
#endif

/*
 * Compiler-dependant initializer.
 */
#ifdef _USE_COMPILER_ATTRIBUTION_
static void __destructor__(void) __attribute__ ((destructor(_AWS_PRIORITY_)));

static void
__destructor__(void)
{
    SensorVirtualTable_initialize();
    SensorClass_initialize();
    Sensor_initialize();
    young41342_virtual_table_initialize();
    Young41342Class_initialize();
    Young41342_initialize();
    young05305vs_virtual_table_initialize();
    Young05305VSClass_initialize();
    Young05305VS_initialize();
    young05305vd_virtual_table_initialize();
    Young05305VDClass_initialize();
    Young05305VD_initialize();
    young41382vcr_virtual_table_initialize();
    Young41382VCRClass_initialize();
    Young41382VCR_initialize();
    young41382vct_virtual_table_initialize();
    Young41382VCTClass_initialize();
    Young41382VCT_initialize();
    sky_quality_monitor_virtual_table_initialize();
    SkyQualityMonitorClass_initialize();
    SkyQualityMonitor_initialize();
    ws100umb_virtual_table_initialize();
    WS100UMBClass_initialize();
    WS100UMB_initialize();
    __AWSVirtualTable_initialize();
    __AWSClass_initialize();
    __AWS_initialize();
    KLAWSControllerClass_initialize();
    KLAWSController_initialize();
    KLAWSDeviceClass_initialize();
    KLAWSDevice_initialize();
    klaws_virtual_table_initialize();
    KLAWSClass_initialize();
    KLAWS_initialize();
}

static void __constructor__(void) __attribute__ ((constructor(_AWS_PRIORITY_)));

static void
__constructor__(void)
{
    KLAWS_destroy();
    KLAWSClass_destroy();
    klaws_virtual_table_destroy();
    KLAWSDevice_destroy();
    KLAWSDeviceClass_destroy();
    KLAWSController_destroy();
    KLAWSControllerClass_destroy();
    __AWS_destroy();
    __AWSClass_destroy();
    __AWSVirtualTable_destroy();
    WS100UMB_destroy();
    WS100UMBClass_destroy();
    ws100umb_virtual_table_destroy();
    SkyQualityMonitor_destroy();
    SkyQualityMonitorClass_destroy();
    sky_quality_monitor_virtual_table_destroy();
    Young41382VCT_destroy();
    Young41382VCTClass_destroy();
    young41382vct_virtual_table_destroy();
    Young41382VCR_destroy();
    Young41382VCRClass_destroy();
    young41382vcr_virtual_table_destroy();
    Young05305VD_destroy();
    Young05305VDClass_destroy();
    young05305vd_virtual_table_destroy();
    Young05305VS_destroy();
    Young05305VSClass_destroy();
    young05305vs_virtual_table_destroy();
    Young41342_destroy();
    Young41342Class_destroy();
    young41342_virtual_table_destroy();
    Sensor_destroy();
    SensorClass_destroy();
    SensorVirtualTable_destroy();
}
#endif
