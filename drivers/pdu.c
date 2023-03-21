//
//  pdu.c
//  AAOS
//
//  Created by Hu Yi on 2020/6/29.
//  Copyright © 2020 NAOC. All rights reserved.
//

#include "def.h"
#include "device.h"
#include "pdu_def.h"
#include "pdu_r.h"
#include "pdu.h"
#include "protocol.h"
#include "serial_rpc.h"
#include "virtual.h"
#include "wrapper.h"

/*
 * Switch class.
 */

static void *
SwitchVirtualTable_ctor(void *_self, va_list *app)
{
    struct SwitchVirtualTable *self = super_ctor(SwitchVirtualTable(), _self, app);
    Method selector;
    
    while ((selector = va_arg(*app, Method))) {
        const char *tag = va_arg(*app, const char *);
        Method method = va_arg(*app, Method);
        
        if (selector == (Method) switch_turn_on) {
            if (tag) {
                self->turn_on.tag = tag;
                self->turn_on.selector = selector;
            }
            self->turn_on.method = method;
            continue;
        }
        if (selector == (Method) switch_turn_off) {
            if (tag) {
                self->turn_off.tag = tag;
                self->turn_off.selector = selector;
            }
            self->turn_off.method = method;
            continue;
        }
        if (selector == (Method) switch_status) {
            if (tag) {
                self->status.tag = tag;
                self->status.selector = selector;
            }
            self->status.method = method;
            continue;
        }
        if (selector == (Method) switch_get_voltage) {
            if (tag) {
                self->get_voltage.tag = tag;
                self->get_voltage.selector = selector;
            }
            self->get_voltage.method = method;
            continue;
        }
        if (selector == (Method) switch_get_current) {
            if (tag) {
                self->get_current.tag = tag;
                self->get_current.selector = selector;
            }
            self->get_current.method = method;
            continue;
        }
        if (selector == (Method) switch_get_voltage_current) {
            if (tag) {
                self->get_voltage_current.tag = tag;
                self->get_voltage_current.selector = selector;
            }
            self->get_voltage_current.method = method;
            continue;
        }
    }
    
    return _self;
}

static void *
SwitchVirtualTable_dtor(void *_self)
{
    return super_dtor(Object(), _self);
}

static const void *_SwitchVirtualTable;

static void
SwitchVirtualTable_destroy(void)
{
    free((void *) _SwitchVirtualTable);
}

static void
SwitchVirtualTable_initialize(void)
{
    _SwitchVirtualTable = new(VirtualTableClass(), "SwitchVirtualTable", VirtualTable(), sizeof(struct SwitchVirtualTable),
                              ctor, "ctor", SwitchVirtualTable_ctor,
                              dtor, "dtor", SwitchVirtualTable_dtor,
                              (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(SwitchVirtualTable_destroy);
#endif
}

const void *
SwitchVirtualTable(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, SwitchVirtualTable_initialize);
#endif
    
    return _SwitchVirtualTable;
}

/*
 * Switch
 */

const char *
switch_get_name(const void *_self)
{
    const struct SwitchClass *class = (const struct SwitchClass *) classOf(_self);
    
    if (isOf(class, SwitchClass()) && class->get_name.method) {
        return ((const char * (*)(const void *)) class->get_name.method)(_self);
    } else {
        const char *result;
        forward(_self, &result, (Method) switch_get_name, "get_name", _self);
        return result;
    }
}

static const char *
Switch_get_name(const void *_self)
{
    struct Switch *self = cast(Switch(), _self);
    
    return self->name;
}

unsigned int
switch_get_channel(const void *_self)
{
    const struct SwitchClass *class = (const struct SwitchClass *) classOf(_self);
    
    if (isOf(class, SwitchClass()) && class->get_channel.method) {
        return ((unsigned int (*)(const void *)) class->get_channel.method)(_self);
    } else {
        unsigned int result;
        forward(_self, &result, (Method) switch_get_channel, "get_channel", _self);
        return result;
    }
}

static unsigned int
Switch_get_channel(const void *_self)
{
    struct Switch *self = cast(Switch(), _self);
    
    return self->channel;
}

void
switch_set_channel(void *_self, unsigned int channel)
{
    const struct SwitchClass *class = (const struct SwitchClass *) classOf(_self);
    
    if (isOf(class, SwitchClass()) && class->set_channel.method) {
        return ((void (*)(void *, unsigned int)) class->set_channel.method)(_self, channel);
    } else {
        
        forward(_self, 0, (Method) switch_set_channel, "set_channel", _self, channel);
    }
}

static void
Switch_set_channel(void *_self, unsigned int channel)
{
    struct Switch *self = cast(Switch(), _self);
    
    self->channel = channel;
}

unsigned int
switch_get_type(const void *_self)
{
    const struct SwitchClass *class = (const struct SwitchClass *) classOf(_self);
    
    if (isOf(class, SwitchClass()) && class->get_type.method) {
        return ((unsigned int (*)(const void *)) class->get_type.method)(_self);
    } else {
        unsigned int result;
        forward(_self, &result, (Method) switch_get_type, "get_type", _self);
        return result;
    }
}

static unsigned int
Switch_get_type(const void *_self)
{
    struct Switch *self = cast(Switch(), _self);
    
    return self->type;
}


void
switch_set_type(void *_self, unsigned int type)
{
    const struct SwitchClass *class = (const struct SwitchClass *) classOf(_self);
    
    if (isOf(class, SwitchClass()) && class->set_type.method) {
        return ((void (*)(void *, unsigned int)) class->set_type.method)(_self, type);
    } else {
        
        forward(_self, 0, (Method) switch_set_type, "set_type", _self, type);
    }
}

static void
Switch_set_type(void *_self, unsigned int type)
{
    struct Switch *self = cast(Switch(), _self);
    
    self->type = type;
}

void
switch_set_pdu(void *_self, const void *pdu)
{
    const struct SwitchClass *class = (const struct SwitchClass *) classOf(_self);
    
    if (isOf(class, SwitchClass()) && class->set_pdu.method) {
        return ((void (*)(void *, const void *)) class->set_pdu.method)(_self, pdu);
    } else {
        
        forward(_self, 0, (Method) switch_set_pdu, "set_pdu", _self, pdu);
    }
}

static void
Switch_set_pdu(void *_self, const void *pdu)
{
    struct Switch *self = cast(Switch(), _self);
    
    self->pdu = (void *) pdu;
}

int
switch_status(void *_self, unsigned char *status)
{
    const struct SwitchClass *class = (const struct SwitchClass *) classOf(_self);
    
    if (isOf(class, SwitchClass()) && class->status.method) {
        return ((int (*)(void *, unsigned char *)) class->status.method)(_self, status);
    } else {
        int result;
        forward(_self, &result, (Method) switch_status, "status", _self);
        return result;
    }
}

static int
Switch_status(void *_self, unsigned char *status)
{
    return AAOS_ENOTSUP;
}


int
switch_turn_on(void *_self)
{
    const struct SwitchClass *class = (const struct SwitchClass *) classOf(_self);
    
    if (isOf(class, SwitchClass()) && class->turn_on.method) {
        return ((int (*)(void *)) class->turn_on.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) switch_turn_on, "turn_on", _self);
        return result;
    }
}

static int
Switch_turn_on(void *_self)
{
    return AAOS_ENOTSUP;
}

int
switch_turn_off(void *_self)
{
    const struct SwitchClass *class = (const struct SwitchClass *) classOf(_self);
    
    if (isOf(class, SwitchClass()) && class->turn_off.method) {
        return ((int (*)(void *)) class->turn_off.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) switch_turn_off, "turn_off", _self);
        return result;
    }
}

static int
Switch_turn_off(void *_self)
{
    return AAOS_ENOTSUP;
}

int
switch_get_voltage(void *_self, double *voltage)
{
    const struct SwitchClass *class = (const struct SwitchClass *) classOf(_self);
    
    if (isOf(class, SwitchClass()) && class->get_voltage.method) {
        return ((int (*)(void *, double *)) class->get_voltage.method)(_self, voltage);
    } else {
        int result;
        forward(_self, &result, (Method) switch_get_voltage, "get_voltage", _self, voltage);
        return result;
    }
}

static int
Switch_get_voltage(void *_self, double *voltage)
{
    return AAOS_ENOTSUP;
}

int
switch_get_current(void *_self, double *current)
{
    const struct SwitchClass *class = (const struct SwitchClass *) classOf(_self);
    
    if (isOf(class, SwitchClass()) && class->get_current.method) {
        return ((int (*)(void *, double *)) class->get_current.method)(_self, current);
    } else {
        int result;
        forward(_self, &result, (Method) switch_get_current, "get_current", _self, current);
        return result;
    }
}

static int
Switch_get_current(void *_self, double *current)
{
    return AAOS_ENOTSUP;
}

int
switch_get_voltage_current(void *_self, double *voltage, double *current)
{
    const struct SwitchClass *class = (const struct SwitchClass *) classOf(_self);
    
    if (isOf(class, SwitchClass()) && class->get_voltage_current.method) {
        return ((int (*)(void *, double *, double *)) class->get_voltage_current.method)(_self, voltage, current);
    } else {
        int result;
        forward(_self, &result, (Method) switch_get_voltage_current, "get_voltage_current", _self, voltage, current);
        return result;
    }
}

static int
Switch_get_voltage_current(void *_self, double *voltage, double *current)
{
    return AAOS_ENOTSUP;
}


static void
Switch_forward(const void *_self, void *result, Method selector, const char *name, va_list *app)
{
    const struct Switch *self = cast(Switch(), _self);
    Method method = virtualTo(self->_vtab, name);
    va_list ap;
    
#ifdef va_copy
    va_copy(ap, *app);
#else
    ap = *app;
#endif
    
    void *obj = va_arg(*app, void *);
    
    if (selector == (Method) switch_turn_on) {
        *((int *) result) = ((int (*)(void *)) method)(obj);
    } else if (selector == (Method) switch_turn_off) {
        *((int *) result) = ((int (*)(void *)) method)(obj);
    } else if (selector == (Method) switch_get_voltage) {
        double *voltage = va_arg(*app, double *);
        *((int *) result) = ((int (*)(void *, double *)) method)(obj, voltage);
    } else if (selector == (Method) switch_get_current) {
        double *current = va_arg(*app, double *);
        *((int *) result) = ((int (*)(void *, double *)) method)(obj, current);
    } else if (selector == (Method) switch_get_voltage_current) {
        double *voltage = va_arg(*app, double *);
        double *current = va_arg(*app, double *);
        *((int *) result) = ((int (*)(void *, double *, double *)) method)(obj, voltage, current);
    } else {
        assert(0);
    }
    
#ifdef va_copy
    va_end(ap);
#endif
}

static void *
Switch_ctor(void *_self, va_list *app)
{
    struct Switch *self = super_ctor(Switch(), _self, app);
    
    const char *s, *key, *value;
    
    s = va_arg(*app, const char *);
    if (s) {
        self->name = (char *) Malloc(strlen(s) + 1);
        snprintf(self->name, strlen(s) + 1, "%s", s);
    }
    s = va_arg(*app, const void *);
    
    while ((key = va_arg(*app, const char *))) {
        
        if (strcmp(key, "type") == 0) {
            value = va_arg(*app, const char *);
            if (value) {
                if (strcmp(value, "AAGPDU") == 0) {
                    self->type = PDU_TYPE_AAGPDU;
                }
            }
            continue;
        }
    }
    
    return (void *) self;
}

static void *
Switch_dtor(void *_self)
{
    struct Switch *self = cast(Switch(), _self);
    
    free(self->name);


    return super_dtor(Switch(), _self);
}

static void *
SwitchClass_ctor(void *_self, va_list *app)
{
    struct SwitchClass *self = super_ctor(SwitchClass(), _self, app);
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
        if (selector == (Method) switch_set_channel) {
            if (tag) {
                self->set_channel.tag = tag;
                self->set_channel.selector = selector;
            }
            self->set_channel.method = method;
            continue;
        }
        if (selector == (Method) switch_get_channel) {
            if (tag) {
                self->get_channel.tag = tag;
                self->get_channel.selector = selector;
            }
            self->get_channel.method = method;
            continue;
        }
        if (selector == (Method) switch_set_type) {
            if (tag) {
                self->set_type.tag = tag;
                self->set_type.selector = selector;
            }
            self->set_type.method = method;
            continue;
        }
        if (selector == (Method) switch_get_type) {
            if (tag) {
                self->get_type.tag = tag;
                self->get_type.selector = selector;
            }
            self->get_type.method = method;
            continue;
        }
        if (selector == (Method) switch_set_pdu) {
            if (tag) {
                self->set_pdu.tag = tag;
                self->set_pdu.selector = selector;
            }
            self->set_pdu.method = method;
            continue;
        }
        
        if (selector == (Method) switch_get_name) {
            if (tag) {
                self->get_name.tag = tag;
                self->get_name.selector = selector;
            }
            self->get_name.method = method;
            continue;
        }
        if (selector == (Method) switch_status) {
            if (tag) {
                self->status.tag = tag;
                self->status.selector = selector;
            }
            self->status.method = method;
            continue;
        }
    }
    
#ifdef va_copy
    va_end(ap);
#endif
    
    return (void *) self;
}

static const void *_SwitchClass;

static void
SwitchClass_destroy(void)
{
    free((void *) _SwitchClass);
}

static void
SwitchClass_initialize(void)
{
    _SwitchClass = new(Class(), "SwitchClass", Class(), sizeof(struct SwitchClass),
                        ctor, "ctor", SwitchClass_ctor,
                        (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(SwitchClass_destroy);
#endif
}

const void *
SwitchClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, SwitchClass_initialize);
#endif
    
    return _SwitchClass;
}

static const void *_Switch;

static void
Switch_destroy(void)
{
    free((void *)_Switch);
}

static void
Switch_initialize(void)
{
    _Switch = new(SwitchClass(), "Switch", Object(), sizeof(struct Switch),
                  ctor, "ctor", Switch_ctor,
                  dtor, "dtor", Switch_dtor,
                  forward, "forward", Switch_forward,
                  switch_set_pdu, "set_pdu", Switch_set_pdu,
                  switch_get_name, "get_name", Switch_get_name,
                  switch_get_channel, "get_channel", Switch_get_channel,
                  switch_set_channel, "set_channel", Switch_set_channel,
                  switch_get_type, "get_type", Switch_get_type,
                  switch_set_type, "set_type", Switch_set_type,
                  switch_status, "status", Switch_status,
                  switch_turn_on, "turn_on", Switch_turn_on,
                  switch_turn_off, "turn_off", Switch_turn_off,
                  switch_get_voltage, "get_voltage", Switch_get_voltage,
                  switch_get_current, "get_current", Switch_get_current,
                  switch_get_voltage_current, "get_voltage_current", Switch_get_voltage_current,
                  (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(Switch_destroy);
#endif
}

const void *
Switch(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, Switch_initialize);
#endif
    
    return _Switch;
}

static int AAGPDU_try_connect(struct AAGPDU *self);

static const void *aag_switch_virtual_table(void);

static void *
AAGSwitch_ctor(void *_self, va_list *app)
{
    struct AAGSwitch *self = super_ctor(AAGSwitch(), _self, app);
    
    self->_._vtab= aag_switch_virtual_table();
    
    return (void *) self;
}

static void *
AAGSwitch_dtor(void *_self)
{
    return super_dtor(AAGSwitch(), _self);
}

static void *
AAGSwitchClass_ctor(void *_self, va_list *app)
{
    struct AAGSwitchClass *self = super_ctor(AAGSwitchClass(), _self, app);
    
    self->_.status.method = (Method) 0;
    self->_.get_voltage.method = (Method) 0;
    self->_.get_current.method = (Method) 0;
    self->_.get_voltage_current.method = (Method) 0;
    self->_.turn_on.method = (Method) 0;
    self->_.turn_off.method = (Method) 0;
    
    return self;
}

static const void *_AAGSwitchClass;

static void
AAGSwitchClass_destroy(void)
{
    free((void *) _AAGSwitchClass);
}

static void
AAGSwitchClass_initialize(void)
{
    _AAGSwitchClass = new(SwitchClass(), "AAGSwitchClass", SwitchClass(), sizeof(struct AAGSwitchClass),
                          ctor, "", AAGSwitchClass_ctor,
                          (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(AAGSwitchClass_destroy);
#endif
}

const void *
AAGSwitchClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, AAGSwitchClass_initialize);
#endif
    
    return _AAGSwitchClass;
}

static const void *_AAGSwitch;

static void
AAGSwitch_destroy(void)
{
    free((void *)_AAGSwitch);
}

static void
AAGSwitch_initialize(void)
{
    _AAGSwitch = new(AAGSwitchClass(), "AAGSwitch", AAGSwitch(), sizeof(struct AAGSwitch),
                     ctor, "ctor", AAGSwitch_ctor,
                     dtor, "dtor", AAGSwitch_dtor,
                     (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(AAGSwitch_destroy);
#endif
}

const void *
AAGSwitch(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, AAGSwitch_initialize);
#endif

    return _AAGSwitch;
}

static int
AAGSwitch_turn_on(void *_self)
{
    struct AAGSwitch *self = cast(__PDU(), _self);
    struct AAGPDU *pdu = (struct AAGPDU *) self->_.pdu;
    unsigned int channel = self->_.channel;
    void *serial = pdu->serial;
    uint16_t idx1 = pdu->serial_index_1, idx2 = pdu->serial_index_2;
    int ret1, ret2, ret;
    char cmd[COMMANDSIZE], res[COMMANDSIZE];
    
    Pthread_mutex_lock(&pdu->mtx);
    protobuf_set(serial, PACKET_INDEX, idx1);
    if (self->flag) {
        /*
         * Wrong wiring case.
         * p01a BH00000010, p01b is BH00000001
         */
        if ((channel%2) == 0) {
            snprintf(cmd, COMMANDSIZE, "BH%08X\n", 1<<(channel-2));
        } else {
            snprintf(cmd, COMMANDSIZE, "BH%08X\n", 1<<channel);
        }
    } else {
        snprintf(cmd, COMMANDSIZE, "BH%08X\n", 1<<(channel - 1));
    }
    protobuf_set(serial, PACKET_INDEX, idx1);
    ret1 = serial_raw(pdu->serial, cmd, strlen(cmd), res, COMMANDSIZE, NULL);
    if (ret1 < 0) {
        AAGPDU_try_connect(pdu);
        ret1 = serial_raw(pdu->serial, cmd, strlen(cmd), res, COMMANDSIZE, NULL);
    }
    protobuf_set(serial, PACKET_INDEX, idx2);
    ret2 = serial_raw(pdu->serial, cmd, strlen(cmd), res, COMMANDSIZE, NULL);
    if (ret2 < 0) {
        AAGPDU_try_connect(pdu);
        ret2 = serial_raw(pdu->serial, cmd, strlen(cmd), res, COMMANDSIZE, NULL);
    }
    
    if (ret1 == AAOS_OK || ret2 == AAOS_OK) {
        ret = AAOS_OK;
    } else {
        ret = ret2;
    }
    Pthread_mutex_unlock(&pdu->mtx);
    
    return ret;
    
}

static int
AAGSwitch_turn_off(void *_self)
{
    struct AAGSwitch *self = cast(__PDU(), _self);
    struct AAGPDU *pdu = (struct AAGPDU *) self->_.pdu;
    unsigned int channel = self->_.channel;
    void *serial = pdu->serial;
    uint16_t idx1 = pdu->serial_index_1, idx2 = pdu->serial_index_2;
    int ret1, ret2, ret;
    char cmd[COMMANDSIZE], res[COMMANDSIZE];
    
    Pthread_mutex_lock(&pdu->mtx);
    protobuf_set(serial, PACKET_INDEX, idx1);
    if (self->flag) {
        if ((channel%2) == 0) {
            snprintf(cmd, COMMANDSIZE, "BL%08X\n", 1<<(channel-2));
        } else {
            snprintf(cmd, COMMANDSIZE, "BL%08X\n", 1<<channel);
        }
        
    } else {
        snprintf(cmd, COMMANDSIZE, "BL%08X\n", 1<<(channel - 1));
    }
    protobuf_set(serial, PACKET_INDEX, idx1);
    
    ret1 = serial_raw(pdu->serial, cmd, strlen(cmd), res, COMMANDSIZE, NULL);
    if (ret1 < 0) {
        AAGPDU_try_connect(pdu);
        ret1 = serial_raw(pdu->serial, cmd, strlen(cmd), res, COMMANDSIZE, NULL);
    }
    protobuf_set(serial, PACKET_INDEX, idx2);
    ret2 = serial_raw(pdu->serial, cmd, strlen(cmd), res, COMMANDSIZE, NULL);
    if (ret2 < 0) {
        AAGPDU_try_connect(pdu);
        ret2 = serial_raw(pdu->serial, cmd, strlen(cmd), res, COMMANDSIZE, NULL);
    }
    if (ret1 == AAOS_OK || ret2 == AAOS_OK) {
        ret = AAOS_OK;
    } else {
        ret = ret2;
    }
    Pthread_mutex_unlock(&pdu->mtx);
    
    return ret;
}

static int
AAGSwitch_get_voltage_current(void *_self, double *voltage, double *current)
{
    struct AAGSwitch *self = cast(__PDU(), _self);
    struct AAGPDU *pdu = (struct AAGPDU *) self->_.pdu;
    unsigned int channel = self->_.channel;
    void *serial = pdu->serial;
    uint16_t idx1 = pdu->serial_index_1, idx2 = pdu->serial_index_2;
    int ret1, ret2, ret = AAOS_ERROR;
    char cmd[COMMANDSIZE], res[COMMANDSIZE];
    
    Pthread_mutex_lock(&pdu->mtx);
    protobuf_set(serial, PACKET_INDEX, idx1);
    if (self->flag) {
        if ((channel%2) == 0) {
            snprintf(cmd, COMMANDSIZE, "BM%02d\n", channel-2);
        } else {
            snprintf(cmd, COMMANDSIZE, "BM%02d\n", channel);
        }
        
    } else {
        snprintf(cmd, COMMANDSIZE, "BM%02d\n", channel - 1);
    }
    protobuf_set(serial, PACKET_INDEX, idx1);
    ret1 = serial_raw(pdu->serial, cmd, strlen(cmd), res, COMMANDSIZE, NULL);
    if (ret1 != AAOS_OK) {
        if (ret1 < 0) {
            AAGPDU_try_connect(pdu);
            if ((ret1 = serial_raw(pdu->serial, cmd, strlen(cmd), res, COMMANDSIZE, NULL))== AAOS_OK ) {
                ret = AAOS_OK;
                goto error;
            }
        }
        protobuf_set(serial, PACKET_INDEX, idx2);
        ret2 = serial_raw(pdu->serial, cmd, strlen(cmd), res, COMMANDSIZE, NULL);
        
        ret = ret2;
    }
    
error:
    if (ret == AAOS_OK) {
        char *v_str, *i_str;
        i_str = strchr(res, ',');
        if (i_str != NULL) {
            *i_str = '\0';
            i_str++;
            *current = ((double) atoi(v_str)) / 400.;
        }
        v_str = strrchr(res, ':');
        if (v_str != NULL) {
            v_str++;
            *voltage = ((double) atoi(v_str)) / 100.;
        }
    }
    
    Pthread_mutex_unlock(&pdu->mtx);
    
    return ret;
}

static int
AAGSwitch_get_voltage(void *_self, double *voltage)
{
    return AAGSwitch_get_voltage_current(_self, voltage, NULL);
}

static int
AAGSwitch_get_current(void *_self, double *current)
{
    return AAGSwitch_get_voltage_current(_self, NULL, current);
}

static int
AAGSwitch_status(void *_self, unsigned char *status)
{
    int ret;
    
    double current;
    
    if ((ret = AAGSwitch_get_current(_self, &current)) == AAOS_OK) {
        if (current < 0.02) {
            *status = SWITCH_STATUS_OFF;
        } else {
            *status = SWITCH_STATUS_ON;
        }
    } else {
        *status = SWITCH_STATUS_UNKNOWN;
    }
    
    return AAOS_OK;
    /*
    struct AAGSwitch *self = cast(__PDU(), _self);
    struct AAGPDU *pdu = (struct AAGPDU *) self->_.pdu;
    unsigned int channel = self->_.channel;
    void *serial = pdu->serial;
    uint16_t idx1 = pdu->serial_index_1, idx2 = pdu->serial_index_2;
    int ret1, ret2, ret;
    char cmd[COMMANDSIZE], res[COMMANDSIZE];
    
    Pthread_mutex_lock(&pdu->mtx);
    protobuf_set(serial, PACKET_INDEX, idx1);
    if (self->flag) {
        if ((channel%2) == 0) {
            snprintf(cmd, COMMANDSIZE, "BM%02d\r", channel-2);
        } else {
            snprintf(cmd, COMMANDSIZE, "BM%02d\r", channel);
        }
        
    } else {
        snprintf(cmd, COMMANDSIZE, "BM%02d\r", channel - 1);
    }
    protobuf_set(serial, PACKET_INDEX, idx1);
    ret1 = serial_raw(pdu->serial, cmd, strlen(cmd), res, COMMANDSIZE, NULL);
    if (ret1 != AAOS_OK) {
        if (ret1 < 0) {
            AAGPDU_try_connect(pdu);
            if ((ret1 = serial_raw(pdu->serial, cmd, strlen(cmd), res, COMMANDSIZE, NULL))== AAOS_OK ) {
                ret = AAOS_OK;
                goto error;
            }
        }
        protobuf_set(serial, PACKET_INDEX, idx2);
        ret2 = serial_raw(pdu->serial, cmd, strlen(cmd), res, COMMANDSIZE, NULL);
        ret = ret2;
    } else {
        ret = ret1;
    }
    
error:
    if (ret == AAOS_OK) {
        *status = SWITCH_STATUS_ON;
    } else {
        *status = SWITCH_STATUS_UNKNOWN;
    }
    
    Pthread_mutex_unlock(&pdu->mtx);
    */
    
    return ret;
}

static const void *_aag_switch_virtual_table;

static void
aag_switch_virtual_table_destroy(void)
{
    delete((void *) _aag_switch_virtual_table);
}


static void
aag_switch_virtual_table_initialize(void)
{
    _aag_switch_virtual_table = new(SwitchVirtualTable(),
                                    switch_status, "status", AAGSwitch_status,
                                    switch_get_current, "get_current", AAGSwitch_get_current,
                                    switch_get_voltage, "get_voltage", AAGSwitch_get_voltage,
                                    switch_get_voltage_current, "get_voltage_current", AAGSwitch_get_voltage_current,
                                    switch_turn_on, "turn_on", AAGSwitch_turn_on,
                                    switch_turn_off, "turn_off", AAGSwitch_turn_off,
                                    (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(aag_switch_virtual_table_destroy);
#endif
}

static const void *
aag_switch_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, aag_switch_virtual_table_initialize);
#endif
    
    return _aag_switch_virtual_table;
}


/*
 * PDU virtual table.
 */

static void *
__PDUVirtualTable_ctor(void *_self, va_list *app)
{
    struct __PDUVirtualTable *self = super_ctor(__PDUVirtualTable(), _self, app);
    Method selector;
    
    while ((selector = va_arg(*app, Method))) {
        const char *tag = va_arg(*app, const char *);
        Method method = va_arg(*app, Method);
        
        
        if (selector == (Method) __pdu_turn_on) {
            if (tag) {
                self->turn_on.tag = tag;
                self->turn_on.selector = selector;
            }
            self->turn_on.method = method;
            continue;
        }
        if (selector == (Method) __pdu_turn_off) {
            if (tag) {
                self->turn_off.tag = tag;
                self->turn_off.selector = selector;
            }
            self->turn_off.method = method;
            continue;
        }
        if (selector == (Method) __pdu_get_current) {
            if (tag) {
                self->get_current.tag = tag;
                self->get_current.selector = selector;
            }
            self->get_current.method = method;
            continue;
        }
        if (selector == (Method) __pdu_get_voltage) {
            if (tag) {
                self->get_voltage.tag = tag;
                self->get_voltage.selector = selector;
            }
            self->get_voltage.method = method;
            continue;
        }
        if (selector == (Method) __pdu_get_voltage_current) {
            if (tag) {
                self->get_voltage_current.tag = tag;
                self->get_voltage_current.selector = selector;
            }
            self->get_voltage_current.method = method;
            continue;
        }
        if (selector == (Method) __pdu_status) {
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
__PDUVirtualTable_dtor(void *_self)
{
    return super_dtor(Object(), _self);
}

static const void *___PDUVirtualTable;

static void
__PDUVirtualTable_destroy(void)
{
    free((void *) ___PDUVirtualTable);
}

static void
__PDUVirtualTable_initialize(void)
{
    ___PDUVirtualTable = new(VirtualTableClass(), "PDUVirtualTable", VirtualTable(), sizeof(struct __PDUVirtualTable),
                             ctor, "ctor", __PDUVirtualTable_ctor,
                             dtor, "dtor", __PDUVirtualTable_dtor,
                             (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(__PDUVirtualTable_destroy);
#endif
}

const void *
__PDUVirtualTable(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, __PDUVirtualTable_initialize);
#endif
    
    return ___PDUVirtualTable;
}


/*
 * PDU class
 */

int
__pdu_get_channel_by_name(const void *_self, const char *name, unsigned int *channel)
{
    const struct __PDUClass *class = (const struct __PDUClass *) classOf(_self);
    
    if (isOf(class, __PDUClass()) && class->get_channel_by_name.method) {
        return ((int(*)(const void *, const char *, unsigned int *)) class->get_channel_by_name.method)(_self, name, channel);
    } else {
        int result;
        forward(_self, &result, (Method) __pdu_get_channel_by_name, "get_channel_by_name", _self, name, channel);
        return result;
    }
}

static int
__PDU_get_channel_by_name(const void *_self, const char *name, unsigned int *channel)
{
    const struct __PDU *self = cast(__PDU(), _self);
    
    size_t i;
    
    for (i = 0; i < self->n_swicth; i++) {
        const char *s = switch_get_name(self->swicthes[i]);
        if (strcmp(name, s) == 0) {
            *channel = switch_get_channel(self->swicthes[i]);
            return AAOS_OK;
        }
    }

    return AAOS_ENOTFOUND;
}

int
__pdu_status(void *_self, unsigned char *status, size_t size)
{
    const struct __PDUClass *class = (const struct __PDUClass *) classOf(_self);
    
    if (isOf(class, __PDUClass()) && class->status.method) {
        return ((int (*)(const void *)) class->status.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) __pdu_status, "status", _self);
        return result;
    }
}

static int
__PDU_status(const void *_self, unsigned char *status, size_t size)
{
    const struct __PDU *self = cast(__PDU(), _self);
    size_t i, n = min(self->n_swicth, size);
    int ret;
    
    for (i = 0; i < n; i++) {
        ret = switch_status(self->swicthes[i], status + i);
        if (ret != AAOS_OK) {
            return ret;
        }
    }
    
    return AAOS_OK;
}

const char *
__pdu_get_name(const void *_self)
{
    const struct __PDUClass *class = (const struct __PDUClass *) classOf(_self);
    
    if (isOf(class, __PDUClass()) && class->get_name.method) {
        return ((const char * (*)(const void *)) class->get_name.method)(_self);
    } else {
        const char *result;
        forward(_self, &result, (Method) __pdu_get_name, "get_name", _self);
        return result;
    }
}

static const char *
__PDU_get_name(const void *_self)
{
    const struct __PDU *self = cast(__PDU(), _self);
    
    return (const char *) self->name;
}

void **
__pdu_get_switches(const void *_self)
{
    const struct __PDUClass *class = (const struct __PDUClass *) classOf(_self);
    
    if (isOf(class, __PDUClass()) && class->get_switches.method) {
        return ((void ** (*)(const void *)) class->get_switches.method)(_self);
    } else {
        void **result;
        forward(_self, &result, (Method) __pdu_get_switches, "get_switches", _self);
        return result;
    }
}

void
__pdu_set_switch(void *_self, const void *myswitch, size_t index)
{
    const struct __PDUClass *class = (const struct __PDUClass *) classOf(_self);
    
    if (isOf(class, __PDUClass()) && class->set_switch.method) {
        ((void (*)(void *, const void *, size_t)) class->set_switch.method)(_self, myswitch, index);
    } else {
        forward(_self, 0, (Method) __pdu_set_switch, "set_switch", _self, myswitch, index);
    }
}

static void
__PDU_set_switch(void *_self, const void *myswitch, size_t index)
{
    struct __PDU *self = cast(__PDU(), _self);
    
    if (index < self->n_swicth && self->swicthes != NULL) {
        self->swicthes[index] = (void *) myswitch;
    }
}

void *
__pdu_get_switch(void *_self, size_t index)
{
    const struct __PDUClass *class = (const struct __PDUClass *) classOf(_self);
    
    if (isOf(class, __PDUClass()) && class->set_switch.method) {
        return ((void * (*)(void *, size_t)) class->get_switch.method)(_self, index);
    } else {
        void *result;
        forward(_self, &result, (Method) __pdu_get_switch, "get_switch", _self,  index);
        return result;
    }
}

static void *
__PDU_get_switch(void *_self, size_t index)
{
    struct __PDU *self = cast(__PDU(), _self);
    
    if (index <= self->n_swicth && self->swicthes!= NULL) {
        return self->swicthes[index - 1];
    } else {
        return NULL;
    }
}


void **
__PDU_get_switches(const void *_self)
{
    const struct __PDU *self = cast(__PDU(), _self);
    
    return self->swicthes;
}

void
__pdu_set_type(void *_self, const char *type)
{
    const struct __PDUClass *class = (const struct __PDUClass *) classOf(_self);
    
    if (isOf(class, __PDUClass()) && class->set_type.method) {
        return ((void (*)(void *, const char *)) class->set_type.method)(_self, type);
    } else {
        
        forward(_self, (void *) 0, (Method) __pdu_set_type, "set_type", _self, type);
        
    }
}

static void
__PDU_set_type(void *_self, const char *type)
{
    struct __PDU *self = cast(__PDU(), _self);
    
    self->type = Realloc(self->type, strlen(type) + 1);
    snprintf(self->type, strlen(type), "%s", type);
}


int
__pdu_get_current(void *_self, unsigned int channel, double *current)
{
    const struct __PDUClass *class = (const struct __PDUClass *) classOf(_self);
    
    if (isOf(class, __PDUClass()) && class->get_current.method) {
        return ((int (*)(void *, unsigned int, double *)) class->get_current.method)(_self, channel, current);
    } else {
        int result;
        forward(_self, &result, (Method) __pdu_get_current, "get_current", _self, channel, current);
        return result;
    }
}

static int
__PDU_get_current(void *_self, unsigned int channel, double *current)
{
    struct __PDU *self = cast(__PDU(), _self);
    size_t i, n = self->n_swicth;
    
    for (i = 0; i < n; i++) {
        unsigned int switch_channel = switch_get_channel(self->swicthes[i]);
        if (switch_channel == channel) {
            return switch_get_current(self->swicthes[i], current);
            break;
        }
    }
    
    return AAOS_ENOTFOUND;
}

int
__pdu_get_current_by_name(void *_self, const char *name, double *current)
{
    const struct __PDUClass *class = (const struct __PDUClass *) classOf(_self);
    
    if (isOf(class, __PDUClass()) && class->get_current_by_name.method) {
        return ((int (*)(void *, const char *, double *)) class->get_current_by_name.method)(_self, name, current);
    } else {
        int result;
        forward(_self, &result, (Method) __pdu_get_current_by_name, "get_current_byname", _self, name, current);
        return result;
    }
}

static int
__PDU_get_current_by_name(void *_self, const char *name, double *current)
{
    struct __PDU *self = cast(__PDU(), _self);
    size_t i, n = self->n_swicth;
    
    for (i = 0; i < n; i++) {
        const char *switch_name = switch_get_name(self->swicthes[i]);
        if (strcmp(name, switch_name) == 0) {
            return switch_get_current(self->swicthes[i], current);
            break;
        }
    }
    
    return AAOS_ENOTFOUND;
}

int
__pdu_get_voltage(void *_self, unsigned int channel, double *voltage)
{
    const struct __PDUClass *class = (const struct __PDUClass *) classOf(_self);
    
    if (isOf(class, __PDUClass()) && class->get_voltage.method) {
        return ((int (*)(void *, unsigned int, double *)) class->get_voltage.method)(_self, channel, voltage);
    } else {
        int result;
        forward(_self, &result, (Method) __pdu_get_voltage, "get_voltage", _self, channel, voltage);
        return result;
    }
}

static int
__PDU_get_voltage(void *_self, unsigned int channel, double *voltage)
{
    struct __PDU *self = cast(__PDU(), _self);
    size_t i, n = self->n_swicth;
    
    for (i = 0; i < n; i++) {
        unsigned int switch_channel = switch_get_channel(self->swicthes[i]);
        if (switch_channel == channel) {
            return switch_get_voltage(self->swicthes[i], voltage);
            break;
        }
    }
    
    return AAOS_ENOTFOUND;
}

int
__pdu_get_voltage_by_name(void *_self, const char *name, double *voltage)
{
    const struct __PDUClass *class = (const struct __PDUClass *) classOf(_self);
    
    if (isOf(class, __PDUClass()) && class->get_voltage_by_name.method) {
        return ((int (*)(void *, const char *, double *)) class->get_voltage_by_name.method)(_self, name, voltage);
    } else {
        int result;
        forward(_self, &result, (Method) __pdu_get_voltage_by_name, "get_voltage_byname", _self, name, voltage);
        return result;
    }
}

static int
__PDU_get_voltage_by_name(void *_self, const char *name, double *voltage)
{
    struct __PDU *self = cast(__PDU(), _self);
    size_t i, n = self->n_swicth;
    
    for (i = 0; i < n; i++) {
        const char *switch_name = switch_get_name(self->swicthes[i]);
        if (strcmp(name, switch_name) == 0) {
            return switch_get_voltage(self->swicthes[i], voltage);
            break;
        }
    }
    
    return AAOS_ENOTFOUND;
}

int
__pdu_get_voltage_current(void *_self, unsigned int channel, double *voltage, double *current)
{
    const struct __PDUClass *class = (const struct __PDUClass *) classOf(_self);
    
    if (isOf(class, __PDUClass()) && class->get_voltage_current.method) {
        return ((int (*)(void *, unsigned int, double *, double *)) class->get_voltage_current.method)(_self, channel, voltage, current);
    } else {
        int result;
        forward(_self, &result, (Method) __pdu_get_voltage_current, "get_voltage_current", _self, channel, voltage, current);
        return result;
    }
}

static int
__PDU_get_voltage_current(void *_self, unsigned int channel, double *voltage, double *current)
{
    struct __PDU *self = cast(__PDU(), _self);
    size_t i, n = self->n_swicth;
    
    for (i = 0; i < n; i++) {
        unsigned int switch_channel = switch_get_channel(self->swicthes[i]);
        if (switch_channel == channel) {
            return switch_get_voltage_current(self->swicthes[i], voltage, current);
            break;
        }
    }
    
    return AAOS_ENOTFOUND;
}

int
__pdu_get_voltage_current_by_name(void *_self, const char *name, double *voltage, double *current)
{
    const struct __PDUClass *class = (const struct __PDUClass *) classOf(_self);
    
    if (isOf(class, __PDUClass()) && class->get_voltage_current_by_name.method) {
        return ((int (*)(void *, const char *, double *, double *)) class->get_voltage_current_by_name.method)(_self, name, voltage, current);
    } else {
        int result;
        forward(_self, &result, (Method) __pdu_get_voltage_current_by_name, "get_voltage_current_by_name", _self, name, voltage, current);
        return result;
    }
}

static int
__PDU_get_voltage_current_by_name(void *_self, const char *name, double *voltage, double *current)
{
    struct __PDU *self = cast(__PDU(), _self);
    size_t i, n = self->n_swicth;
    
    for (i = 0; i < n; i++) {
        const char *switch_name = switch_get_name(self->swicthes[i]);
        if (strcmp(name, switch_name) == 0) {
            return switch_get_voltage_current(self->swicthes[i], voltage, current);
            break;
        }
    }
    
    return AAOS_ENOTFOUND;
}

int
__pdu_turn_on(void *_self, unsigned int channel)
{
    const struct __PDUClass *class = (const struct __PDUClass *) classOf(_self);
    
    if (isOf(class, __PDUClass()) && class->turn_on.method) {
        return ((int (*)(void *, unsigned int)) class->turn_on.method)(_self, channel);
    } else {
        int result;
        forward(_self, &result, (Method) __pdu_turn_on, "turn_on", _self, channel);
        return result;
    }
}

static int
__PDU_turn_on(void *_self, unsigned int channel)
{
    struct __PDU *self = cast(__PDU(), _self);
    size_t i, n = self->n_swicth;
    
    for (i = 0; i < n; i++) {
        unsigned int switch_channel = switch_get_channel(self->swicthes[i]);
        if (switch_channel == channel) {
            return switch_turn_on(self->swicthes[i]);
            break;
        }
    }
    
    return AAOS_ENOTFOUND;
}

int
__pdu_turn_on_by_name(void *_self, const char *name)
{
    const struct __PDUClass *class = (const struct __PDUClass *) classOf(_self);
    
    if (isOf(class, __PDUClass()) && class->turn_on_by_name.method) {
        return ((int (*)(void *, const char *)) class->turn_on_by_name.method)(_self, name);
    } else {
        int result;
        forward(_self, &result, (Method) __pdu_turn_on_by_name, "turn_on_by_name", _self, name);
        return result;
    }
}

static int
__PDU_turn_on_by_name(void *_self, const char *name)
{
    struct __PDU *self = cast(__PDU(), _self);
    size_t i, n = self->n_swicth;
    
    for (i = 0; i < n; i++) {
        const char *switch_name = switch_get_name(self->swicthes[i]);
        if (strcmp(name, switch_name) == 0) {
            return switch_turn_on(self->swicthes[i]);
            break;
        }
    }
    
    return AAOS_ENOTFOUND;
}

int
__pdu_turn_off(void *_self, unsigned int channel)
{
    const struct __PDUClass *class = (const struct __PDUClass *) classOf(_self);
    
    if (isOf(class, __PDUClass()) && class->turn_off.method) {
        return ((int (*)(void *, unsigned int)) class->turn_off.method)(_self, channel);
    } else {
        int result;
        forward(_self, &result, (Method) __pdu_turn_off, "turn_off", _self, channel);
        return result;
    }
}

static int
__PDU_turn_off(void *_self, unsigned int channel)
{
    struct __PDU *self = cast(__PDU(), _self);
    size_t i, n = self->n_swicth;
    
    for (i = 0; i < n; i++) {
        unsigned int switch_channel = switch_get_channel(self->swicthes[i]);
        if (switch_channel == channel) {
            return switch_turn_off(self->swicthes[i]);
            break;
        }
    }
    
    return AAOS_ENOTFOUND;
}

int
__pdu_turn_off_by_name(void *_self, const char *name)
{
    const struct __PDUClass *class = (const struct __PDUClass *) classOf(_self);
    
    if (isOf(class, __PDUClass()) && class->turn_off_by_name.method) {
        return ((int (*)(void *, const char *)) class->turn_off_by_name.method)(_self, name);
    } else {
        int result;
        forward(_self, &result, (Method) __pdu_turn_off_by_name, "turn_off_by_name", _self, name);
        return result;
    }
}

static int
__PDU_turn_off_by_name(void *_self, const char *name)
{
    struct __PDU *self = cast(__PDU(), _self);
    size_t i, n = self->n_swicth;
    
    for (i = 0; i < n; i++) {
        const char *switch_name = switch_get_name(self->swicthes[i]);
        if (strcmp(name, switch_name) == 0) {
            return switch_turn_off(self->swicthes[i]);
            break;
        }
    }
    
    return AAOS_ENOTFOUND;
}

static void
__PDU_forward(const void *_self, void *result, Method selector, const char *name, va_list *app)
{
    const struct __PDU *self = cast(__PDU(), _self);
    Method method = virtualTo(self->_vtab, name);
    va_list ap;
    
#ifdef va_copy
    va_copy(ap, *app);
#else
    ap = *app;
#endif
    
    void *obj = va_arg(*app, void *);
    
    if (selector == (Method) __pdu_turn_on) {
        unsigned int channel = va_arg(*app, unsigned int);
        *((int *) result) = ((int (*)(void *, unsigned int)) method)(obj, channel);
    } else if (selector == (Method) __pdu_turn_off) {
        unsigned int channel = va_arg(*app, unsigned int);
        *((int *) result) = ((int (*)(void *, unsigned int)) method)(obj, channel);
    } else if (selector == (Method) __pdu_get_current) {
        unsigned int channel = va_arg(*app, unsigned int);
        double *current = va_arg(*app, double *);
        *((int *) result) = ((int (*)(void *, unsigned int, double *)) method)(obj, channel, current);
    } else if (selector == (Method) __pdu_get_voltage) {
        unsigned int channel = va_arg(*app, unsigned int);
        double *voltage = va_arg(*app, double *);
        *((int *) result) = ((int (*)(void *, unsigned int, double *)) method)(obj, channel, voltage);
    } else if (selector == (Method) __pdu_get_voltage_current) {
        unsigned int channel = va_arg(*app, unsigned int);
        double *voltage = va_arg(*app, double *);
        double *current = va_arg(*app, double *);
        *((int *) result) = ((int (*)(void *, unsigned int, double *, double *)) method)(obj, channel, voltage, current);
    } else if (selector == (Method) __pdu_turn_on_by_name) {
        const char *name = va_arg(*app, const char *);
        *((int *) result) = ((int (*)(void *, const char *)) method)(obj, name);
    } else if (selector == (Method) __pdu_turn_off_by_name) {
        const char *name = va_arg(*app, const char *);
        *((int *) result) = ((int (*)(void *, const char *)) method)(obj, name);
    } else if (selector == (Method) __pdu_get_current_by_name) {
        const char *name = va_arg(*app, const char *);
        double *current = va_arg(*app, double *);
        *((int *) result) = ((int (*)(void *, const char *, double *)) method)(obj, name, current);
    } else if (selector == (Method) __pdu_get_voltage_by_name) {
        const char *name = va_arg(*app, const char *);
        double *voltage = va_arg(*app, double *);
        *((int *) result) = ((int (*)(void *, const char *, double *)) method)(obj, name, voltage);
    } else if (selector == (Method) __pdu_get_voltage_current_by_name) {
        const char *name = va_arg(*app, const char *);
        double *voltage = va_arg(*app, double *);
        double *current = va_arg(*app, double *);
        *((int *) result) = ((int (*)(void *, const char *, double *, double *)) method)(obj, name, voltage, current);
    } else if (selector == (Method) __pdu_status) {
        unsigned char *status = va_arg(*app, unsigned char *);
        size_t size = va_arg(*app, size_t);
        *((int *) result) = ((int (*)(void *, unsigned char *, size_t)) method)(obj, status, size);
    } else {
        assert(0);
    }
    
#ifdef va_copy
    va_end(ap);
#endif
}

static void *
__PDU_ctor(void *_self, va_list *app)
{
    struct __PDU *self = super_ctor(__PDU(), _self, app);
    
    const char *s, *key, *value;
    size_t n_switch;
    
    s = va_arg(*app, const char *);
    if (s) {
        self->name = (char *) Malloc(strlen(s) + 1);
        snprintf(self->name, strlen(s) + 1, "%s", s);
    }
    
    n_switch = va_arg(*app, size_t);
    
    self->n_swicth = n_switch;
    self->switch_status =  (unsigned char *) Malloc(sizeof(unsigned char) * n_switch);
    if (self->switch_status == NULL) {
        free(self->name);
        return NULL;
    }
    self->swicthes = (void **) Malloc(sizeof(void *) * n_switch);
    if (self->swicthes == NULL) {
        free(self->name);
        free(self->switch_status);
        return NULL;
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
        if (strcmp(key, "type") == 0) {
            value = va_arg(*app, const char *);
            if (value) {
                self->type = (char *) Malloc(strlen(value) + 1);
                snprintf(self->type, strlen(value) + 1, "%s", value);
            }
            continue;
        }
    }
    
    return (void *) self;
}

static void *
__PDU_dtor(void *_self)
{
    struct __PDU *self = cast(__PDU(), _self);
    
    size_t i, n = self->n_swicth;
    
    for (i = 0; i < n; i++) {
        if (self->swicthes[i] != NULL) {
            delete(self->swicthes[i]);
        }
    }
    
    free(self->swicthes);
    free(self->switch_status);
    free(self->name);
    free(self->description);
    free(self->type);
    
    return super_dtor(__PDU(), _self);
}

static void *
__PDUClass_ctor(void *_self, va_list *app)
{
    struct __PDUClass *self = super_ctor(__PDUClass(), _self, app);
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
        
        if (selector == (Method) __pdu_turn_on) {
            if (tag) {
                self->turn_on.tag = tag;
                self->turn_on.selector = selector;
            }
            self->turn_on.method = method;
            continue;
        }
        if (selector == (Method) __pdu_turn_on_by_name) {
            if (tag) {
                self->turn_on_by_name.tag = tag;
                self->turn_on_by_name.selector = selector;
            }
            self->turn_on_by_name.method = method;
            continue;
        }
        if (selector == (Method) __pdu_turn_off_by_name) {
            if (tag) {
                self->turn_off_by_name.tag = tag;
                self->turn_off_by_name.selector = selector;
            }
            self->turn_off_by_name.method = method;
            continue;
        }
        if (selector == (Method) __pdu_get_current_by_name) {
            if (tag) {
                self->get_current_by_name.tag = tag;
                self->get_current_by_name.selector = selector;
            }
            self->get_current_by_name.method = method;
            continue;
        }
        if (selector == (Method) __pdu_get_voltage_by_name) {
            if (tag) {
                self->get_voltage_by_name.tag = tag;
                self->get_voltage_by_name.selector = selector;
            }
            self->get_voltage_by_name.method = method;
            continue;
        }
        if (selector == (Method) __pdu_get_voltage_current_by_name) {
            if (tag) {
                self->get_voltage_current_by_name.tag = tag;
                self->get_voltage_current_by_name.selector = selector;
            }
            self->get_voltage_current_by_name.method = method;
            continue;
        }
        if (selector == (Method) __pdu_get_name) {
            if (tag) {
                self->get_name.tag = tag;
                self->get_name.selector = selector;
            }
            self->get_name.method = method;
            continue;
        }
        if (selector == (Method) __pdu_set_type) {
            if (tag) {
                self->set_type.tag = tag;
                self->set_type.selector = selector;
            }
            self->set_type.method = method;
            continue;
        }
        if (selector == (Method) __pdu_status) {
            if (tag) {
                self->status.tag = tag;
                self->status.selector = selector;
            }
            self->status.method = method;
            continue;
        }
        if (selector == (Method) __pdu_get_channel_by_name) {
            if (tag) {
                self->get_channel_by_name.tag = tag;
                self->get_channel_by_name.selector = selector;
            }
            self->get_channel_by_name.method = method;
            continue;
        }
        if (selector == (Method) __pdu_get_switches) {
            if (tag) {
                self->get_switches.tag = tag;
                self->get_switches.selector = selector;
            }
            self->get_switches.method = method;
            continue;
        }
        if (selector == (Method) __pdu_get_switch) {
            if (tag) {
                self->get_switch.tag = tag;
                self->get_switch.selector = selector;
            }
            self->get_switch.method = method;
            continue;
        }
        if (selector == (Method) __pdu_set_switch) {
            if (tag) {
                self->set_switch.tag = tag;
                self->set_switch.selector = selector;
            }
            self->set_switch.method = method;
            continue;
        }
        
    }
    
#ifdef va_copy
    va_end(ap);
#endif
    
    return (void *) self;
}

static const void *___PDUClass;

static void
__PDUClass_destroy(void)
{
    free((void *) ___PDUClass);
}

static void
__PDUClass_initialize(void)
{
    ___PDUClass = new(Class(), "__PDUClass", Class(), sizeof(struct __PDUClass),
                        ctor, "ctor", __PDUClass_ctor,
                        (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(__PDUClass_destroy);
#endif
}

const void *
__PDUClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, __PDUClass_initialize);
#endif
    
    return ___PDUClass;
}

static const void *___PDU;

static void
__PDU_destroy(void)
{
    free((void *)___PDU);
}

static void
__PDU_initialize(void)
{
    ___PDU = new(__PDUClass(), "__PDU", Object(), sizeof(struct __PDU),
                 ctor, "ctor", __PDU_ctor,
                 dtor, "dtor", __PDU_dtor,
                 forward, "forward", __PDU_forward,
                 __pdu_get_name, "get_name", __PDU_get_name,
                 __pdu_set_type, "set_type", __PDU_set_type,
                 __pdu_get_switches, "get_switches", __PDU_get_switches,
                 __pdu_set_switch, "set_switch", __PDU_set_switch,
                 __pdu_get_switch, "get_switch", __PDU_get_switch,
                 __pdu_status, "status", __PDU_status,
                 __pdu_turn_on, "turn_on", __PDU_turn_on,
                 __pdu_turn_on_by_name, "turn_on_by_name", __PDU_turn_on_by_name,
                 __pdu_turn_off, "turn_off", __PDU_turn_off,
                 __pdu_turn_off_by_name, "turn_off_by_name", __PDU_turn_off_by_name,
                 __pdu_get_voltage, "get_voltage", __PDU_get_voltage,
                 __pdu_get_voltage_by_name, "get_voltage_by_name", __PDU_get_voltage_by_name,
                 __pdu_get_current, "get_current", __PDU_get_current,
                 __pdu_get_current_by_name, "get_current_by_name", __PDU_get_current_by_name,
                 __pdu_get_voltage_current, "get_voltage_current", __PDU_get_voltage_current,
                 __pdu_get_voltage_current_by_name, "get_voltage_current_by_name", __PDU_get_voltage_current_by_name,
                 __pdu_get_channel_by_name, "get_channel_by_name", __PDU_get_channel_by_name,
                 
                 (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(__PDU_destroy);
#endif
}

const void *
__PDU(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, __PDU_initialize);
#endif
    
    return ___PDU;
}

static const void *aag_pdu_virtual_table(void);
static const void *aag_pdu_device_virtual_table(void);

/*
 * pdu = new(AAGPDU(), name, n_switches, "description", xxx, "type", xxx, '\0', addr, port, serial1, inspect1, serial2, inspect2)
 */
static void *
AAGPDU_ctor(void *_self, va_list *app)
{
    struct AAGPDU *self = super_ctor(AAGPDU(), _self, app);
    
    self->_._vtab= aag_pdu_virtual_table();
    self->_._._vtab = aag_pdu_device_virtual_table();
    
    const char *s;
    s = va_arg(*app, const char *);
    if ((self->serial_address = (char *) Malloc(strlen(s) + 1)) == NULL) {
        goto error;
    }
    snprintf(self->serial_address, strlen(s) + 1, "%s", s);
    s = va_arg(*app, const char *);
    if ((self->serial_port = (char *) Malloc(strlen(s) + 1)) == NULL) {
        goto error;
    }
    snprintf(self->serial_port, strlen(s) + 1, "%s", s);
    s = va_arg(*app, const char *);
    if ((self->serial1 = (char *) Malloc(strlen(s) + 1)) == NULL) {
        goto error;
    }
    snprintf(self->serial1, strlen(s) + 1, "%s", s);
    s = va_arg(*app, const char *);
    if ((self->inspect1 = (char *) Malloc(strlen(s) + 1)) == NULL) {
        goto error;
    }
    snprintf(self->inspect1, strlen(s) + 1, "%s", s);
    s = va_arg(*app, const char *);
    if ((self->serial2 = (char *) Malloc(strlen(s) + 1)) == NULL) {
        goto error;
    }
    snprintf(self->serial2, strlen(s) + 1, "%s", s);
    s = va_arg(*app, const char *);
    if ((self->inspect2 = (char *) Malloc(strlen(s) + 1)) == NULL) {
        goto error;
    }
    snprintf(self->inspect2, strlen(s) + 1, "%s", s);
    
    void *client;
    client = new(SerialClient(), self->serial_address, self->serial_port);
    rpc_client_connect(client, &self->serial);
    delete(client);

    Pthread_mutex_init(&self->mtx, NULL);
    
    int ret;
    uint16_t idx;
    if ((ret = serial_get_index_by_name(self->serial, self->serial1)) == AAOS_ENOTFOUND) {
        serial_get_index_by_path(self->serial, self->serial1);
    }
    protobuf_get(self->serial, PACKET_INDEX, &idx);
    self->serial_index_1 = idx;
    if ((ret = serial_get_index_by_name(self->serial, self->serial2)) == AAOS_ENOTFOUND) {
        serial_get_index_by_path(self->serial, self->serial2);
    }
    protobuf_get(self->serial, PACKET_INDEX, &idx);
    self->serial_index_2 = idx;
    
    return (void *) self;
    
error:
    super_dtor(AAGPDU(), _self);
    free(self->serial_address);
    free(self->serial_port);
    if (self->serial != NULL) {
        delete(self->serial);
    }
    free(self->serial1);
    free(self->inspect1);
    free(self->serial2);
    free(self->inspect2);
    
    return NULL;
}

static void *
AAGPDU_dtor(void *_self)
{
    struct AAGPDU *self = cast(AAGPDU(), _self);
    
    Pthread_mutex_destroy(&self->mtx);
    delete(self->serial);
    free(self->serial2);
    free(self->serial1);
    free(self->serial_port);
    free(self->serial_address);
    
    return super_dtor(AAGPDU(), _self);
}

static void *
AAGPDUClass_ctor(void *_self, va_list *app)
{
    struct AAGPDUClass *self = super_ctor(AAGPDUClass(), _self, app);
    
    self->_.status.method = (Method) 0;
    self->_._.inspect.method = (Method) 0;
    
    return self;
}

static const void *_AAGPDUClass;

static void
AAGPDUClass_destroy(void)
{
    free((void *) _AAGPDUClass);
}

static void
AAGPDUClass_initialize(void)
{
    _AAGPDUClass = new(__PDUClass(), "AAGPDuClass", __PDUClass(), sizeof(struct AAGPDUClass),
                       ctor, "", AAGPDUClass_ctor,
                       (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(AAGPDUClass_destroy);
#endif
}

const void *
AAGPDUClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, AAGPDUClass_initialize);
#endif
    
    return _AAGPDUClass;
}

static const void *_AAGPDU;

static void
AAGPDU_destroy(void)
{
    free((void *)_AAGPDU);
}

static void
AAGPDU_initialize(void)
{
    _AAGPDU = new(AAGPDUClass(), "AAGPDU", AAGPDU(), sizeof(struct AAGPDU),
                     ctor, "ctor", AAGPDU_ctor,
                     dtor, "dtor", AAGPDU_dtor,
                     (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(AAGPDU_destroy);
#endif
}

const void *
AAGPDU(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, AAGPDU_initialize);
#endif

    return _AAGPDU;
}

static int
AAGPDU_try_connect(struct AAGPDU *self)
{
    void *client = new(SerialClient(), self->serial_address, self->serial_port);
    int ret;
    if (self->serial != NULL) {
        delete(self->serial);
    }
    
    ret = rpc_client_connect(client, &self->serial);
    
    if (client != NULL) {
        delete(client);
    }
    
    return ret;
}

static int
AAGPDU_status(void *_self, unsigned char *status, size_t size)
{
    struct AAGPDU *self = cast(AAGPDU(), _self);
    
    char res[COMMANDSIZE];
    int ret1, ret2, ret = AAOS_ERROR;
    uint16_t idx1 = self->serial_index_1, idx2 = self->serial_index_2;
    void *serial = self->serial;
    uint32_t mystatus = 0;
    
    Pthread_mutex_lock(&self->mtx);
    
    protobuf_set(serial, PACKET_INDEX, idx1);
    if ((ret1 = serial_raw(serial, "BR\n", 3, res, COMMANDSIZE, NULL)) != AAOS_OK) {
        if (ret1 < 0) {
            AAGPDU_try_connect(self);
            if ((ret1 = serial_raw(serial, "BR\n", 3, res, COMMANDSIZE, NULL)) != AAOS_OK) {
                ret = AAOS_OK;
                goto error;
            }
        }
        protobuf_set(serial, PACKET_INDEX, idx2);
        ret2 = serial_raw(serial, "BR\n", 3, res, COMMANDSIZE, NULL);
        ret = ret2;
    }
    
error:
    
    if (ret == AAOS_OK) {
        char *s = strchr(res, '*');
        if (s != NULL) {
            s -= 1;
            s[0] = '0';
            s[1] = 'x';
            sscanf(s, "%x", &mystatus);
        }
        
        size_t i, n = min(size, AAGPDU_CHANNEL_NUMBER);
        for (i = 0; i < n; i++) {
            if (self->flag) {
                if (i%2) {
                    if (mystatus&1>>(i + 1)) {
                        status[i] = SWITCH_STATUS_ON;
                    } else {
                        status[i] = SWITCH_STATUS_OFF;
                    }
                } else {
                    if (mystatus&1>>(i - 1)) {
                        status[i] = SWITCH_STATUS_ON;
                    } else {
                        status[i] = SWITCH_STATUS_OFF;
                    }
                }
                if (mystatus&1>>i) {
                    status[i] = SWITCH_STATUS_ON;
                }
            } else {
                if (mystatus&1>>i) {
                    status[i] = SWITCH_STATUS_ON;
                } else {
                    status[i] = SWITCH_STATUS_OFF;
                }
            }
        }
    } else if (ret < 0) {
        AAGPDU_try_connect(self);
    }
    Pthread_mutex_unlock(&self->mtx);
    return ret;
}

static const void *_aag_pdu_virtual_table;

static void
aag_pdu_virtual_table_destroy(void)
{
    delete((void *) _aag_pdu_virtual_table);
}

static void
aag_pdu_virtual_table_initialize(void)
{
    _aag_pdu_virtual_table = new(__PDUVirtualTable(),
                                 __pdu_status, "status", AAGPDU_status,
                                 (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(aag_pdu_virtual_table_destroy);
#endif
}

static const void *
aag_pdu_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, aag_pdu_virtual_table_initialize);
#endif
    
    return _aag_pdu_virtual_table;
}

static int
AAGPDU_inspect(void *_self)
{
    struct AAGPDU *self = cast(AAGPDU(), _self);
    
    uint16_t idx1 = self->serial_index_1, idx2 = self->serial_index_2;
    void *serial = self->serial;
    int ret1, ret2, ret;
    char res[COMMANDSIZE];
        
    Pthread_mutex_lock(&self->_._.mtx);
    protobuf_set(serial, PACKET_INDEX, idx1);
    ret1 = serial_raw(self->serial, self->inspect1, strlen(self->inspect1), res, COMMANDSIZE, NULL);
    protobuf_set(serial, PACKET_INDEX, idx2);
    ret2 = serial_raw(self->serial, self->inspect2, strlen(self->inspect2), res, COMMANDSIZE, NULL);
    if (ret1 == AAOS_OK || ret2 == AAOS_OK) {
        if (self->_._.state == DEVICE_MALFUNCTION) {
            self->_._.state = DEVICE_OK;
            Pthread_cond_broadcast(&self->_._.cond);
        }
        ret = AAOS_OK;
    } else {
        self->_._.state = DEVICE_MALFUNCTION;
        ret = AAOS_ERROR;
    }
    Pthread_mutex_unlock(&self->_._.mtx);
    
    return ret;
}

static const void *_aag_pdu_device_virtual_table;

static void
aag_pdu_device_virtual_table_destroy(void)
{
    delete((void *) _aag_pdu_device_virtual_table);
}

static void
aag_pdu_device_virtual_table_initialize(void)
{
    _aag_pdu_device_virtual_table = new(DeviceVirtualTable(),
                                        device_inspect, "inspect", AAGPDU_inspect,
                                        (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(aag_pdu_device_virtual_table_destroy);
#endif
}

static const void *
aag_pdu_device_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, aag_pdu_device_virtual_table_initialize);
#endif
    
    return _aag_pdu_device_virtual_table;
}

#ifdef _USE_COMPILER_ATTRIBUTION_
static void __destructor__(void) __attribute__ ((destructor(_PDU_PRIORITY_)));

static void
__destructor__(void)
{
    SwitchVirtualTable_initialize();
    SwitchClass_initialize();
    Switch_initialize();
    aag_switch_virtual_table_initialize();
    AAGSwitch_initialize();
    __PDUVirtualTable_initialize();
    __PDUClass_initialize();
    __PDU_initialize();
    aag_pdu_virtual_table_initialize();
    AAGPDUClass_initialize();
    AAGPDU_initialize();
    
}

static void __constructor__(void) __attribute__ ((constructor(_PDU_PRIORITY_)));

static void
__constructor__(void)
{
    AAGPDU_destroy();
    AAGPDUClass_destroy();
    aag_pdu_virtual_table_destroy();
    __PDU_destroy();
    __PDUClass_destroy();
    __PDUVirtualTable_destroy();
    AAGSwitch_destroy();
    AAGSwitchClass_destroy();
    aag_switch_virtual_table_destroy();
    Switch_destroy();
    SwitchClass_destroy();
    SwitchVirtualTable_destroy();
}
#endif
