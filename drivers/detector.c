//
//  __detector.c
//  AAOS
//
//  Created by huyi on 18/7/26.
//  Copyright © 2018年 National Astronomical Observatories, Chinese Academy of Sciences. All rights reserved.
//

#include "adt.h"
#include "def.h"
#include "detector_def.h"
#include "detector.h"
#include "detector_r.h"
#include "object.h"
#include "protocol.h"
#include "rpc.h"
#include "virtual.h"
#include "wrapper.h"

#include <fitsio2.h>
#include <cjson/cJSON.h>

static int
detector_post_acquisition(void *detector, const char *filename, ...)
{
    va_list ap;
    va_start(ap, filename);
    void *rpc = va_arg(ap, void *);
    va_end(ap);
    int ret;
    
    protobuf_set(rpc, PACKET_BUF, filename, strlen(filename) + 1);
    ret = rpc_write(rpc);
    
    return AAOS_OK;
}

/*
 * Detector virtual table.
 */
static void *
__DetectorVirtualTable_ctor(void *_self, va_list *app)
{
    struct __DetectorVirtualTable *self = super_ctor(__DetectorVirtualTable(), _self, app);
    Method selector;
    
    while ((selector = va_arg(*app, Method))) {
        const char *tag = va_arg(*app, const char *);
        Method method = va_arg(*app, Method);
        if (selector == (Method) __detector_power_on) {
            if (tag) {
                self->power_on.tag = tag;
                self->power_on.selector = selector;
            }
            self->power_on.method = method;
            continue;
        }
        if (selector == (Method) __detector_power_off) {
            if (tag) {
                self->power_off.tag = tag;
                self->power_off.selector = selector;
            }
            self->power_off.method = method;
            continue;
        }
        if (selector == (Method) __detector_init) {
            if (tag) {
                self->init.tag = tag;
                self->init.selector = selector;
            }
            self->init.method = method;
            continue;
        }
        if (selector == (Method) __detector_get_binning) {
            if (tag) {
                self->get_binning.tag = tag;
                self->get_binning.selector = selector;
            }
            self->get_binning.method = method;
            continue;
        }
        if (selector == (Method) __detector_set_binning) {
            if (tag) {
                self->set_binning.tag = tag;
                self->set_binning.selector = selector;
            }
            self->set_binning.method = method;
            continue;
        }
        if (selector == (Method) __detector_get_gain) {
            if (tag) {
                self->get_gain.tag = tag;
                self->get_gain.selector = selector;
            }
            self->get_gain.method = method;
            continue;
        }
        if (selector == (Method) __detector_set_gain) {
            if (tag) {
                self->set_gain.tag = tag;
                self->set_gain.selector = selector;
            }
            self->set_gain.method = method;
            continue;
        }
        if (selector == (Method) __detector_get_frame_rate) {
            if (tag) {
                self->get_frame_rate.tag = tag;
                self->get_frame_rate.selector = selector;
            }
            self->get_frame_rate.method = method;
            continue;
        }
        if (selector == (Method) __detector_set_frame_rate) {
            if (tag) {
                self->set_frame_rate.tag = tag;
                self->set_frame_rate.selector = selector;
            }
            self->set_frame_rate.method = method;
            continue;
        }
        if (selector == (Method) __detector_set_exposure_time) {
            if (tag) {
                self->set_exposure_time.tag = tag;
                self->set_exposure_time.selector = selector;
            }
            self->set_exposure_time.method = method;
            continue;
        }
        if (selector == (Method) __detector_get_exposure_time) {
            if (tag) {
                self->get_exposure_time.tag = tag;
                self->get_exposure_time.selector = selector;
            }
            self->get_exposure_time.method = method;
            continue;
        }
        if (selector == (Method) __detector_set_readout_rate) {
            if (tag) {
                self->set_readout_rate.tag = tag;
                self->set_readout_rate.selector = selector;
            }
            self->set_readout_rate.method = method;
            continue;
        }
        if (selector == (Method) __detector_get_readout_rate) {
            if (tag) {
                self->get_readout_rate.tag = tag;
                self->get_readout_rate.selector = selector;
            }
            self->get_readout_rate.method = method;
            continue;
        }
        if (selector == (Method) __detector_set_region) {
            if (tag) {
                self->set_region.tag = tag;
                self->set_region.selector = selector;
            }
            self->set_region.method = method;
            continue;
        }
        if (selector == (Method) __detector_get_region) {
            if (tag) {
                self->get_region.tag = tag;
                self->get_region.selector = selector;
            }
            self->get_region.method = method;
            continue;
        }
        if (selector == (Method) __detector_expose) {
            if (tag) {
                self->expose.tag = tag;
                self->expose.selector = selector;
            }
            self->expose.method = method;
            continue;
        }
        if (selector == (Method) __detector_raw) {
            if (tag) {
                self->raw.tag = tag;
                self->raw.selector = selector;
            }
            self->raw.method = method;
            continue;
        }
        if (selector == (Method) __detector_inspect) {
            if (tag) {
                self->inspect.tag = tag;
                self->inspect.selector = selector;
            }
            self->inspect.method = method;
            continue;
        }
        if (selector == (Method) __detector_wait) {
            if (tag) {
                self->wait.tag = tag;
                self->wait.selector = selector;
            }
            self->wait.method = method;
            continue;
        }
        if (selector == (Method) __detector_wait_for_completion) {
            if (tag) {
                self->wait_for_completion.tag = tag;
                self->wait_for_completion.selector = selector;
            }
            self->wait_for_completion.method = method;
            continue;
        }
        if (selector == (Method) __detector_status) {
            if (tag) {
                self->status.tag = tag;
                self->status.selector = selector;
            }
            self->status.method = method;
            continue;
        }
        if (selector == (Method) __detector_info) {
            if (tag) {
                self->info.tag = tag;
                self->info.selector = selector;
            }
            self->info.method = method;
            continue;
        }
        if (selector == (Method) __detector_abort) {
            if (tag) {
                self->abort.tag = tag;
                self->abort.selector = selector;
            }
            self->abort.method = method;
            continue;
        }
        if (selector == (Method) __detector_stop) {
            if (tag) {
                self->stop.tag = tag;
                self->stop.selector = selector;
            }
            self->stop.method = method;
            continue;
        }
        if (selector == (Method) __detector_get_temperature) {
            if (tag) {
                self->get_temperature.tag = tag;
                self->get_temperature.selector = selector;
            }
            self->get_temperature.method = method;
            continue;
        }
        if (selector == (Method) __detector_set_temperature) {
            if (tag) {
                self->set_temperature.tag = tag;
                self->set_temperature.selector = selector;
            }
            self->set_temperature.method = method;
            continue;
        }
        if (selector == (Method) __detector_enable_cooling) {
            if (tag) {
                self->enable_cooling.tag = tag;
                self->enable_cooling.selector = selector;
            }
            self->enable_cooling.method = method;
            continue;
        }
        if (selector == (Method) __detector_disable_cooling) {
            if (tag) {
                self->disable_cooling.tag = tag;
                self->disable_cooling.selector = selector;
            }
            self->disable_cooling.method = method;
            continue;
        }
        if (selector == (Method) __detector_set_directory) {
            if (tag) {
                self->set_directory.tag = tag;
                self->set_directory.selector = selector;
            }
            self->set_directory.method = method;
            continue;
        }
        if (selector == (Method) __detector_set_prefix) {
            if (tag) {
                self->set_prefix.tag = tag;
                self->set_prefix.selector = selector;
            }
            self->set_prefix.method = method;
            continue;
        }
    }
    
    return _self;
}

static void *
__DetectorVirtualTable_dtor(void *_self)
{
    return super_dtor(Object(), _self);
}

static const void *___DetectorVirtualTable;

static void
__DetectorVirtualTable_destroy(void)
{
    free((void *) ___DetectorVirtualTable);
}

static void
__DetectorVirtualTable_initialize(void)
{
    ___DetectorVirtualTable = new(VirtualTableClass(), "__DetectorVirtualTable", VirtualTable(), sizeof(struct __DetectorVirtualTable),
                                  ctor, "ctor", __DetectorVirtualTable_ctor,
                                  dtor, "dtor", __DetectorVirtualTable_dtor,
                                  (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(__DetectorVirtualTable_destroy);
#endif
}

const void *
__DetectorVirtualTable(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, __DetectorVirtualTable_initialize);
#endif
    
    return ___DetectorVirtualTable;
}

/*
 * Detector class method.
 */

void
__detector_clear_option(void *_self)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->clear_option.method) {
        return ((void(*)(void *)) class->clear_option.method)(_self);
    } else {
        forward(_self, 0, (Method) __detector_clear_option, "clear_option", _self);
    }
}

static void
__Detector_clear_option(void *_self)
{
    struct __Detector *self = cast(__Detector(), _self);
    
    Pthread_mutex_lock(&self->d_state.mtx);
    self->d_state.options = 0;
    self->d_exp.notify_last_frame_filling = 0;
    self->d_exp.notify_each_frame_done = 0;
    Pthread_mutex_unlock(&self->d_state.mtx);
}

void
__detector_set_option(void *_self, uint16_t option)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->set_option.method) {
        return ((void(*)(void *, uint16_t)) class->set_option.method)(_self, option);
    } else {
        forward(_self, 0, (Method) __detector_set_option, "set_option", _self, option);
    }
}

static void
__Detector_set_option(void *_self, uint16_t option)
{
    struct __Detector *self = cast(__Detector(), _self);
    
    Pthread_mutex_lock(&self->d_state.mtx);
    if (option & DETECTOR_OPTION_NOTIFY_LAST_FILLING) {
        self->d_exp.notify_last_frame_filling = 1;
    }
    if (option & DETECTOR_OPTION_NOTIFY_EACH_COMPLETION) {
        self->d_exp.notify_each_frame_done = 1;
    }
    self->d_state.options |= option;
    Pthread_mutex_unlock(&self->d_state.mtx);
}

uint16_t
__detector_get_option(void *_self)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->get_option.method) {
        return ((uint16_t (*)(void *)) class->get_option.method)(_self);
    } else {
        uint16_t result;
        forward(_self, &result, (Method) __detector_get_option, "get_option", _self);
        return result;
    }
}

static uint16_t
__Detector_get_option(void *_self)
{
    struct __Detector *self = cast(__Detector(), _self);
    
    uint16_t result;
    
    Pthread_mutex_lock(&self->d_state.mtx);
    result = self->d_state.options;
    Pthread_mutex_unlock(&self->d_state.mtx);
    
    return result;
}

void
__detector_set_directory(void *_self, const char *directory)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->set_directory.method) {
        return ((void (*)(void *, const char *)) class->set_directory.method)(_self, directory);
    } else {
        forward(_self, 0, (Method) __detector_set_directory, "set_directory", _self, directory);
    }
}

static void
__Detector_set_directory(void *_self, const char *directory)
{
    struct __Detector *self = cast(__Detector(), _self);
    
    Pthread_mutex_lock(&self->d_state.mtx);
    while (self->d_state.state == DETECTOR_STATE_READING || self->d_state.state == DETECTOR_STATE_EXPOSING) {
        Pthread_cond_wait(&self->d_state.cond, &self->d_state.mtx);
    }
    if (self->d_proc.image_directory != NULL) {
        free(self->d_proc.image_directory);
    }
    self->d_proc.image_directory = (char *) Malloc(strlen(directory) + 1);
    snprintf(self->d_proc.image_directory, strlen(directory) + 1, "%s", directory);
    Pthread_mutex_unlock(&self->d_state.mtx);
}

void
__detector_get_directory(void *_self, char *directory, size_t size)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->get_directory.method) {
        return ((void (*)(void *, char *, size_t)) class->set_directory.method)(_self, directory, size);
    } else {
        forward(_self, 0, (Method) __detector_get_directory, "get_directory", _self, directory, size);
    }
}

static void
__Detector_get_directory(void *_self, char *directory, size_t size)
{
    struct __Detector *self = cast(__Detector(), _self);
    
    Pthread_mutex_lock(&self->d_state.mtx);
    snprintf(directory, size, "%s", self->d_proc.image_directory);
    Pthread_mutex_unlock(&self->d_state.mtx);
}

void
__detector_set_prefix(void *_self, const char *prefix)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->set_prefix.method) {
        return ((void (*)(void *, const char *)) class->set_prefix.method)(_self, prefix);
    } else {
        forward(_self, 0, (Method) __detector_set_prefix, "set_prefix", _self, prefix);
    }
}

static void
__Detector_set_prefix(void *_self, const char *prefix)
{
    struct __Detector *self = cast(__Detector(), _self);
    
    Pthread_mutex_lock(&self->d_state.mtx);
    while (self->d_state.state == DETECTOR_STATE_READING || self->d_state.state == DETECTOR_STATE_EXPOSING) {
        Pthread_cond_wait(&self->d_state.cond, &self->d_state.mtx);
    }
    if (self->d_proc.image_prefix != NULL) {
        free(self->d_proc.image_prefix);
    }
    self->d_proc.image_prefix = (char *) Malloc(strlen(prefix) + 1);
    snprintf(self->d_proc.image_prefix, strlen(prefix) + 1, "%s", prefix);
    Pthread_mutex_unlock(&self->d_state.mtx);
}

void
__detector_get_prefix(void *_self, char *prefix, size_t size)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->get_prefix.method) {
        return ((void (*)(void *, char *, size_t)) class->set_prefix.method)(_self, prefix, size);
    } else {
        forward(_self, 0, (Method) __detector_get_prefix, "get_prefix", _self, prefix, size);
    }
}

static void
__Detector_get_prefix(void *_self, char *prefix, size_t size)
{
    struct __Detector *self = cast(__Detector(), _self);
    
    Pthread_mutex_lock(&self->d_state.mtx);
    snprintf(prefix, size, "%s", self->d_proc.image_prefix);
    Pthread_mutex_unlock(&self->d_state.mtx);
}

void
__detector_set_template(void *_self, const char *template)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->set_template.method) {
        return ((void (*)(void *, const char *)) class->set_template.method)(_self, template);
    } else {
        forward(_self, 0, (Method) __detector_set_template, "set_template", _self, template);
    }
}

static void
__Detector_set_template(void *_self, const char *template)
{
    struct __Detector *self = cast(__Detector(), _self);
    
    Pthread_mutex_lock(&self->d_state.mtx);
    while (self->d_state.state == DETECTOR_STATE_READING || self->d_state.state == DETECTOR_STATE_EXPOSING) {
        Pthread_cond_wait(&self->d_state.cond, &self->d_state.mtx);
    }
    if (self->d_proc.tpl_filename != NULL) {
        free(self->d_proc.tpl_filename);
    }
    self->d_proc.tpl_filename = (char *) Malloc(strlen(template) + 1);
    snprintf(self->d_proc.tpl_filename, strlen(template) + 1, "%s", template);
    Pthread_mutex_unlock(&self->d_state.mtx);
}

void
__detector_get_template(void *_self, char *template, size_t size)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->get_template.method) {
        return ((void (*)(void *, char *, size_t)) class->set_template.method)(_self, template, size);
    } else {
        forward(_self, 0, (Method) __detector_get_prefix, "get_template", _self, template, size);
    }
}

static void
__Detector_get_template(void *_self, char *template, size_t size)
{
    struct __Detector *self = cast(__Detector(), _self);
    
    Pthread_mutex_lock(&self->d_state.mtx);
    snprintf(template, size, "%s", self->d_proc.tpl_filename);
    Pthread_mutex_unlock(&self->d_state.mtx);
}

void
__detector_set_pre_acquisition(void *_self, int (*pre_acquisition)(void *, const char *, ...))
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->set_pre_acquisition.method) {
        return ((void (*)(void *, int (*)(void *, const char *, ...))) class->set_pre_acquisition.method)(_self, pre_acquisition);
    } else {
        forward(_self, 0, (Method) __detector_set_pre_acquisition, "set_pre_acquisition", _self, pre_acquisition);
    }
}

static void
__Detector_set_pre_acquisition(void *_self, int (*pre_acquisition)(void *, const char *, ...))
{
    struct __Detector *self = cast(__Detector(), _self);
    
    Pthread_mutex_lock(&self->d_state.mtx);
    while (self->d_state.state == DETECTOR_STATE_READING || self->d_state.state == DETECTOR_STATE_EXPOSING) {
        Pthread_cond_wait(&self->d_state.cond, &self->d_state.mtx);
    }
    
    self->d_proc.pre_acquisition = pre_acquisition;
    Pthread_mutex_unlock(&self->d_state.mtx);
}

void
__detector_set_post_acquisition(void *_self, int (*post_acquisition)(void *, const char *, ...))
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->set_post_acquisition.method) {
        return ((void (*)(void *, int (*)(void *, const char *, ...))) class->set_post_acquisition.method)(_self, post_acquisition);
    } else {
        forward(_self, 0, (Method) __detector_set_post_acquisition, "set_post_acquisition", _self, post_acquisition);
    }
}

static void
__Detector_set_post_acquisition(void *_self, int (*post_acquisition)(void *, const char *, ...))
{
    struct __Detector *self = cast(__Detector(), _self);
    
    Pthread_mutex_lock(&self->d_state.mtx);
    while (self->d_state.state == DETECTOR_STATE_READING || self->d_state.state == DETECTOR_STATE_EXPOSING) {
        Pthread_cond_wait(&self->d_state.cond, &self->d_state.mtx);
    }
    self->d_proc.post_acquisition = post_acquisition;
    Pthread_mutex_unlock(&self->d_state.mtx);
}

void
__detector_set_name_convention(void *_self, void (*name_convention)(void *, char *, size_t, ...))
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->set_name_convention.method) {
        return ((void (*)(void *, void (*)(void *, char *, size_t, ...))) class->set_name_convention.method)(_self, name_convention);
    } else {
        forward(_self, 0, (Method) __detector_set_name_convention, "set_name_convention", _self, name_convention);
    }
}

static void
__Detector_set_name_convention(void *_self, void (*name_convention)(void *, char *, size_t, ...))
{
    struct __Detector *self = cast(__Detector(), _self);
    
    Pthread_mutex_lock(&self->d_state.mtx);
    while (self->d_state.state == DETECTOR_STATE_READING || self->d_state.state == DETECTOR_STATE_EXPOSING) {
        Pthread_cond_wait(&self->d_state.cond, &self->d_state.mtx);
    }
    self->d_proc.name_convention = name_convention;
    Pthread_mutex_unlock(&self->d_state.mtx);
}

int
__detector_wait(void *_self, double timeout)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->wait.method) {
        return ((int (*)(void *, double)) class->wait.method)(_self, timeout);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_wait, "wait", _self, timeout);
        return result;
    }
}

static int
__Detector_wait(void *_self, double timeout)
{
    struct __Detector *self = cast(__Detector(), _self);
    
    int ret = 0;
    struct timespec tp;
    
    tp.tv_sec = floor(timeout);
    tp.tv_nsec = (timeout - tp.tv_sec) * 1000000000;
    
    Pthread_mutex_lock(&self->d_state.mtx);
    while (self->d_state.state == DETECTOR_STATE_MALFUNCTION) {
        if (timeout > 0) {
            ret = Pthread_cond_timedwait(&self->d_state.cond, &self->d_state.mtx, &tp);
        } else {
            ret  = Pthread_cond_wait(&self->d_state.cond, &self->d_state.mtx);
        }
    }
    Pthread_mutex_unlock(&self->d_state.mtx);
    
    if (ret == 0) {
        return AAOS_OK;
    } else if (ret == ETIMEDOUT) {
        return AAOS_ETIMEDOUT;
    } else {
        return AAOS_ERROR;
    }
}

/*
 * Virtual functions of detector class.
 */
int
__detector_power_on(void *_self)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->power_on.method) {
        return ((int (*)(void *)) class->power_on.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_power_on, "power_on", _self);
        return result;
    }
}

static int
__Detector_power_on(void *_self)
{
    struct __Detector *self = cast(__Detector(), _self);
    
    Pthread_mutex_lock(&self->d_state.mtx);
    if (self->d_state.state != DETECTOR_STATE_MALFUNCTION) {
        self->d_state.state = DETECTOR_STATE_IDLE;
    }
    Pthread_mutex_unlock(&self->d_state.mtx);
    
    return AAOS_OK;
}

int
__detector_power_off(void *_self)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->power_off.method) {
        return ((int (*)(void *)) class->power_off.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_power_off, "power_off", _self);
        return result;
    }
}

static int
__Detector_power_off(void *_self)
{
    struct __Detector *self = cast(__Detector(), _self);
    
    Pthread_mutex_lock(&self->d_state.mtx);
    self->d_state.state = DETECTOR_STATE_OFFLINE;
    Pthread_mutex_unlock(&self->d_state.mtx);
    
    return AAOS_OK;
}

int
__detector_enable_cooling(void *_self)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->enable_cooling.method) {
        return ((int (*)(void *)) class->enable_cooling.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_enable_cooling, "enable_cooling", _self);
        return result;
    }
}

static int
__Detector_enable_cooling(void *_self)
{
    return AAOS_ENOTSUP;
}

int
__detector_disable_cooling(void *_self)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->disable_cooling.method) {
        return ((int (*)(void *)) class->disable_cooling.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_disable_cooling, "disable_cooling", _self);
        return result;
    }
}

static int
__Detector_disable_cooling(void *_self)
{
    return AAOS_ENOTSUP;
}

int
__detector_init(void *_self)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->init.method) {
        return ((int (*)(void *)) class->init.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_init, "init", _self);
        return result;
    }
}

static int
__Detector_init(void *_self, uint16_t options)
{
    struct __Detector *self = cast(__Detector(), _self);
    
    Pthread_mutex_lock(&self->d_state.mtx);
    if (self->d_state.state == DETECTOR_STATE_MALFUNCTION) {
        Pthread_mutex_unlock(&self->d_state.mtx);
        return AAOS_EDEVMAL;
    } else if (self->d_state.state == DETECTOR_STATE_OFFLINE) {
        Pthread_mutex_unlock(&self->d_state.mtx);
        return AAOS_EPWROFF;
    }
    Pthread_mutex_unlock(&self->d_state.mtx);
    
    return AAOS_OK;
}

int
__detector_set_exposure_time(void *_self, double exposure_time)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->set_exposure_time.method) {
        return ((int (*)(void *, double)) class->set_exposure_time.method)(_self, exposure_time);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_set_exposure_time, "set_exposure_time", _self, exposure_time);
        return result;
    }
}

static int
__Detector_set_exposure_time(void *_self, double exposure_time)
{
    struct __Detector *self = cast(__Detector(), _self);
    
    if (!self->d_cap.exposure_time_available) {
        return AAOS_ENOTSUP;
    }
    
    if (exposure_time > self->d_cap.exposure_time_max || exposure_time < self->d_cap.exposure_time_min) {
        return AAOS_EINVAL;
    }
    
    Pthread_mutex_lock(&self->d_state.mtx);
    self->d_param.exposure_time = exposure_time;
    Pthread_mutex_unlock(&self->d_state.mtx);
    
    return AAOS_OK;
}

int
__detector_get_exposure_time(void *_self, double *exposure_time)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->get_exposure_time.method) {
        return ((int (*)(void *, double *)) class->get_exposure_time.method)(_self, exposure_time);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_set_exposure_time, "get_exposure_time", _self, exposure_time);
        return result;
    }
}

static int
__Detector_get_exposure_time(void *_self, double *exposure_time)
{
    struct __Detector *self = cast(__Detector(), _self);

    Pthread_mutex_lock(&self->d_state.mtx);
    *exposure_time = self->d_param.exposure_time;
    Pthread_mutex_unlock(&self->d_state.mtx);
    
    return AAOS_OK;
}

int
__detector_set_frame_rate(void *_self, double frame_rate)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->set_frame_rate.method) {
        return ((int (*)(void *, double)) class->set_frame_rate.method)(_self, frame_rate);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_set_frame_rate, "set_frame_rate", _self, frame_rate);
        return result;
    }
}

static int
__Detector_set_frame_rate(void *_self, double frame_rate)
{
    struct __Detector *self = cast(__Detector(), _self);
    
    if (frame_rate > self->d_cap.frame_rate_max || frame_rate < self->d_cap.frame_rate_min) {
        return AAOS_EINVAL;
    }
    Pthread_mutex_lock(&self->d_state.mtx);
    self->d_param.frame_rate = frame_rate;
    Pthread_mutex_unlock(&self->d_state.mtx);
    
    return AAOS_OK;
}

int
__detector_get_frame_rate(void *_self, double *frame_rate)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->get_frame_rate.method) {
        return ((int (*)(void *, double *)) class->get_frame_rate.method)(_self, frame_rate);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_get_frame_rate, "get_frame_rate", _self, frame_rate);
        return result;
    }
}

static int
__Detector_get_frame_rate(void *_self, double *frame_rate)
{
    struct __Detector *self = cast(__Detector(), _self);
    
    if (!self->d_cap.frame_rate_available) {
        return AAOS_ENOTSUP;
    }
    
    Pthread_mutex_lock(&self->d_state.mtx);
    *frame_rate = self->d_param.frame_rate;
    Pthread_mutex_unlock(&self->d_state.mtx);
    
    return AAOS_OK;
}

int
__detector_set_gain(void *_self, double gain)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->set_gain.method) {
        return ((int (*)(void *, double)) class->set_gain.method)(_self, gain);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_set_gain, "set_gain", _self, gain);
        return result;
    }
}

static int
__Detector_set_gain(void *_self, double gain)
{
    struct __Detector *self = cast(__Detector(), _self);
    
    if (gain > self->d_cap.gain_max || gain < self->d_cap.gain_min) {
        return AAOS_EINVAL;
    }
    
    Pthread_mutex_lock(&self->d_state.mtx);
    self->d_param.frame_rate = gain;
    Pthread_mutex_unlock(&self->d_state.mtx);
    
    return AAOS_OK;
}

int
__detector_get_gain(void *_self, double *gain)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->get_gain.method) {
        return ((int (*)(void *, double *)) class->get_gain.method)(_self, gain);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_get_gain, "get_gain", _self, gain);
        return result;
    }
}

static int
__Detector_get_gain(void *_self, double *gain)
{
    struct __Detector *self = cast(__Detector(), _self);
    
    if (!self->d_cap.gain_available) {
        return AAOS_ENOTSUP;
    }
    
    Pthread_mutex_lock(&self->d_state.mtx);
    *gain = self->d_param.gain;
    Pthread_mutex_unlock(&self->d_state.mtx);
    
    return AAOS_OK;
}

int
__detector_get_temperature(void *_self, double *temperature)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->get_temperature.method) {
        return ((int (*)(void *, double *)) class->get_temperature.method)(_self, temperature);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_get_temperature, "get_temperature", _self, temperature);
        return result;
    }
}

static int
__Detector_get_temperature(void *_self, double *temperature)
{
    return AAOS_ENOTSUP;
}

int
__detector_set_temperature(void *_self, double temperature)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->set_temperature.method) {
        return ((int (*)(void *, double)) class->set_temperature.method)(_self, temperature);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_set_temperature, "set_temperature", _self, temperature);
        return result;
    }
}

static int
__Detector_set_temperature(void *_self, double temperature)
{
    struct __Detector *self = cast(__Detector(), _self);
    
    Pthread_mutex_lock(&self->d_state.mtx);
    self->d_param.temperature = temperature;
    Pthread_mutex_unlock(&self->d_state.mtx);
    
    return AAOS_OK;
}

const char *
__detector_get_name(const void *_self)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->get_name.method) {
        return ((const char * (*)(const void *)) class->get_name.method)(_self);
    } else {
        const char *result;
        forward(_self, &result, (Method) __detector_get_name, "get_name", _self);
        return result;
    }
}

static const char *
__Detector_get_name(void *_self)
{
    struct __Detector *self = cast(__Detector(), _self);
    
    return self->name;
}

int
__detector_set_readout_rate(void *_self, double readout_rate)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->set_readout_rate.method) {
        return ((int (*)(void *, double)) class->set_readout_rate.method)(_self, readout_rate);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_set_readout_rate, "set_readout_rate", _self, readout_rate);
        return result;
    }
}

static int
__Detector_set_readout_rate(void *_self, double readout_rate)
{
    struct __Detector *self = cast(__Detector(), _self);
    
    if (readout_rate > self->d_cap.readout_rate_max || readout_rate < self->d_cap.readout_rate_min) {
        return AAOS_EINVAL;
    }
    Pthread_mutex_lock(&self->d_state.mtx);
    self->d_param.readout_rate = readout_rate;
    Pthread_mutex_unlock(&self->d_state.mtx);
    
    return AAOS_OK;
}

int
__detector_get_readout_rate(void *_self, double *readout_rate)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->get_readout_rate.method) {
        return ((int (*)(void *, double *)) class->get_readout_rate.method)(_self, readout_rate);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_get_readout_rate, "get_readout_rate", _self, readout_rate);
        return result;
    }
}

static int
__Detector_get_readout_rate(void *_self, double *readout_rate)
{
    struct __Detector *self = cast(__Detector(), _self);
    
    if (!self->d_cap.readout_rate_available) {
        return AAOS_ENOTSUP;
    }
    
    Pthread_mutex_lock(&self->d_state.mtx);
    *readout_rate = self->d_param.readout_rate;
    Pthread_mutex_unlock(&self->d_state.mtx);
    
    return AAOS_OK;
}

int
__detector_set_region(void *_self, uint32_t x_offset, uint32_t y_offset, uint32_t width, uint32_t height)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->set_region.method) {
        return ((int (*)(void *, uint32_t, uint32_t, uint32_t, uint32_t)) class->set_region.method)(_self, x_offset, y_offset, width, height);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_set_region, "set_region", _self, x_offset, y_offset, width, height);
        return result;
    }
}

static int
__Detector_set_region(void *_self, uint32_t x_offset, uint32_t y_offset, uint32_t width, uint32_t height)
{
    struct __Detector *self = cast(__Detector(), _self);
    size_t image_width, image_height;
    
    Pthread_mutex_lock(&self->d_state.mtx);
    
    image_width = self->d_cap.width / self->d_param.x_binning;
    image_height = self->d_cap.height / self->d_param.y_binning;
    
    if ((x_offset + width) >= image_width || (y_offset + height) >= image_height) {
        Pthread_mutex_unlock(&self->d_state.mtx);
        return AAOS_EINVAL;
    }
    
    self->d_param.x_offset = x_offset;
    self->d_param.y_offset = y_offset;
    self->d_param.image_width = width;
    self->d_param.image_height = height;
    
    Pthread_mutex_unlock(&self->d_state.mtx);
    
    return AAOS_OK;
}

int
__detector_get_region(void *_self, uint32_t *x_offset, uint32_t *y_offset, uint32_t *width, uint32_t *height)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->get_region.method) {
        return ((int (*)(void *, uint32_t *, uint32_t *, uint32_t *, uint32_t *)) class->get_region.method)(_self, x_offset, y_offset, width, height);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_get_region, "get_region", _self, x_offset, y_offset, width, height);
        return result;
    }
}

static int
__Detector_get_region(void *_self, uint32_t *x_offset, uint32_t *y_offset, uint32_t *width, uint32_t *height)
{
    struct __Detector *self = cast(__Detector(), _self);
    
    Pthread_mutex_lock(&self->d_state.mtx);
    
    *x_offset = (uint32_t) self->d_param.x_offset;
    *y_offset = (uint32_t) self->d_param.y_offset;
    *width = (uint32_t) self->d_param.image_width;
    *height = (uint32_t) self->d_param.image_height;
    
    Pthread_mutex_unlock(&self->d_state.mtx);
    
    return AAOS_OK;
}

int
__detector_set_binning(void *_self, uint32_t x_binning, uint32_t y_binning)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->set_binning.method) {
        return ((int (*)(void *, uint32_t, uint32_t)) class->set_binning.method)(_self, x_binning, y_binning);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_set_binning, "set_binning", _self, x_binning, y_binning);
        return result;
    }
}

static int
__Detector_set_binning(void *_self, uint32_t x_binning, uint32_t y_binning)
{
    struct __Detector *self = cast(__Detector(), _self);
    
    if (self->d_cap.x_binning_array) {
        size_t i;
        for (i = 0; i < self->d_cap.n_x_binning; i++) {
            if (x_binning == self->d_cap.x_binning_array[i]) {
                break;
            }
        }
        return AAOS_EINVAL;
    } else {
        if (x_binning < self->d_cap.x_binning_min || x_binning > self->d_cap.x_offset_max) {
            return AAOS_EINVAL;
        }
    }
    if (self->d_cap.y_binning_array) {
        size_t i;
        for (i = 0; i < self->d_cap.n_y_binning; i++) {
            if (y_binning == self->d_cap.y_binning_array[i]) {
                break;
            }
        }
        return AAOS_EINVAL;
    } else {
        if (y_binning < self->d_cap.y_binning_min || y_binning > self->d_cap.y_offset_max) {
            return AAOS_EINVAL;
        }
    }
    
    Pthread_mutex_lock(&self->d_state.mtx);
    self->d_param.x_binning = (size_t) x_binning;
    self->d_param.y_binning = (size_t) y_binning;
    Pthread_mutex_unlock(&self->d_state.mtx);
    
    return AAOS_OK;
}

int
__detector_get_binning(void *_self, uint32_t *x_binning, uint32_t *y_binnning)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->get_binning.method) {
        return ((int (*)(void *, uint32_t *, uint32_t *)) class->get_binning.method)(_self, x_binning, y_binnning);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_get_binning, "get_binning", _self, x_binning, y_binnning);
        return result;
    }
}

static int
__Detector_get_binning(void *_self, uint32_t *x_binning, uint32_t *y_binning)
{
    struct __Detector *self = cast(__Detector(), _self);
    
    Pthread_mutex_lock(&self->d_state.mtx);
    *x_binning = (uint32_t) self->d_param.x_binning;
    *y_binning = (uint32_t) self->d_param.y_binning;
    Pthread_mutex_unlock(&self->d_state.mtx);
    
    return AAOS_OK;
}

/*
 * Pure virtual functions.
 */
int
__detector_status(void *_self, void *res, size_t res_size, size_t *res_len)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->status.method) {
        return ((int (*)(void *, void *, size_t, size_t *)) class->status.method)(_self, res, res_size, res_len);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_status, "status", _self, res, res_size, res_len);
        return result;
    }
}

int
__detector_info(void *_self, void *res, size_t res_size, size_t *res_len)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->info.method) {
        return ((int (*)(void *, void *, size_t, size_t *)) class->info.method)(_self, res, res_size, res_len);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_info, "info", _self, res, res_size, res_len);
        return result;
    }
}

int
__detector_expose(void *_self, double exposure_time, uint32_t n_frame, ...)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    va_list ap;
    va_start(ap, n_frame);
    int result;
    
    if (isOf(class, __DetectorClass()) && class->expose.method) {
        result = ((int (*)(void *, double, uint32_t, void *)) class->expose.method)(_self, exposure_time, n_frame, &ap);
    } else {
        forward(_self, &result, (Method) __detector_expose, "expose", _self, exposure_time, n_frame, &ap);
        return result;
    }
    va_end(ap);
    
    return result;
}

int __detector_raw(void *_self, const void *write_buffer, size_t write_buffer_size, size_t *write_size, void *read_buffer, size_t read_buffer_size, size_t *read_size)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->raw.method) {
        return ((int (*)(void *, const void *, size_t, size_t *, void *, size_t, size_t *)) class->expose.method)(_self, write_buffer, write_buffer_size, write_size, read_buffer, read_buffer_size, read_size);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_raw, "raw", _self, write_buffer, write_buffer_size, write_size, read_buffer, read_buffer_size, read_size);
        return result;
    }
}

int
__detector_inspect(void *_self)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->inspect.method) {
        return ((int (*)(void *)) class->inspect.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_inspect, "inspect", _self);
        return result;
    }
}

int
__detector_abort(void *_self)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->abort.method) {
        return ((int (*)(void *)) class->abort.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_abort, "abort", _self);
        return result;
    }
}

int
__detector_stop(void *_self)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->stop.method) {
        return ((int (*)(void *)) class->stop.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_stop, "stop", _self);
        return result;
    }
}

int
__detector_load(void *_self, ...)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    int result;
    va_list ap;
    
    va_start(ap, _self);
    if (isOf(class, __DetectorClass()) && class->load.method) {
        result = ((int (*)(void *, va_list *)) class->load.method)(_self, &ap);
    } else {
        forward(_self, &result, (Method) __detector_load, "load", _self, &ap);
    }
    va_end(ap);
    
    return result;
}

int
__detector_reload(void *_self)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->reload.method) {
        return ((int (*)(void *)) class->reload.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_reload, "reload", _self);
        return result;
    }
}

int
__detector_unload(void *_self)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->unload.method) {
        return ((int (*)(void *)) class->unload.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_unload, "unload", _self);
        return result;
    }
}

int
__detector_wait_for_completion(void *_self)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->wait_for_completion.method) {
        return ((int (*)(void *)) class->wait_for_completion.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_wait_for_completion, "wait_for_completion", _self);
        return result;
    }
}

/*
 * class basic function
 */
static void
__Detector_forward(const void *_self, void *result, Method selector, const char *name, va_list *app)
{
    const struct __Detector *self = cast(__Detector(), _self);
    Method method = virtualTo(self->_vtab, name);
    va_list ap;
    
#ifdef va_copy
    va_copy(ap, *app);
#else
    ap = *app;
#endif
    
    void *obj = va_arg(*app, void *);
    
    if (selector == (Method) __detector_init || selector == (Method) __detector_power_on || selector == (Method) __detector_power_off || selector == (Method) __detector_inspect || selector == (Method) __detector_wait_for_completion || selector == (Method) __detector_reload || selector == (Method) __detector_unload || selector == (Method) __detector_abort || selector == (Method) __detector_stop || selector == (Method) __detector_enable_cooling || selector == (Method) __detector_disable_cooling) {
        *((int *) result) = ((int (*)(void *)) method)(obj);
    } else if (selector == (Method) __detector_get_gain || selector == (Method) __detector_get_frame_rate || selector == (Method) __detector_get_exposure_time || selector == (Method) __detector_get_readout_rate || selector == (Method) __detector_get_temperature) {
        double *value = va_arg(*app, double *);
        *((int *) result) = ((int (*)(void *, double *)) method)(obj, value);
    } else if (selector == (Method) __detector_set_gain || selector == (Method) __detector_set_frame_rate || selector == (Method) __detector_set_exposure_time || selector == (Method) __detector_set_readout_rate || selector == (Method) __detector_wait || selector == (Method) __detector_get_temperature) {
        double value = va_arg(*app, double);
        *((int *) result) = ((int (*)(void *, double)) method)(obj, value);
    } else if (selector == (Method) __detector_get_region) {
        double *x_offset, *y_offset, *width, *height;
        x_offset = va_arg(*app, double *);
        y_offset = va_arg(*app, double *);
        width = va_arg(*app, double *);
        height = va_arg(*app, double *);
        *((int *) result) = ((int (*)(void *, double *, double *, double *, double *)) method)(obj, x_offset, y_offset, width, height);
    } else if (selector == (Method) __detector_set_region) {
        double x_offset, y_offset, width, height;
        x_offset = va_arg(*app, double);
        y_offset = va_arg(*app, double);
        width = va_arg(*app, double);
        height = va_arg(*app, double);
        *((int *) result) = ((int (*)(void *, double, double, double, double)) method)(obj, x_offset, y_offset, width, height);
    } else if (selector == (Method) __detector_get_binning) {
        uint32_t *x_binning, *y_binning;
        x_binning = va_arg(*app, uint32_t *);
        y_binning = va_arg(*app, uint32_t *);
        *((int *) result) = ((int (*)(void *, uint32_t *, uint32_t *)) method)(obj, x_binning, y_binning);
    } else if (selector == (Method) __detector_set_binning) {
        uint32_t x_binning, y_binning;
        x_binning = va_arg(*app, uint32_t);
        y_binning = va_arg(*app, uint32_t);
        *((int *) result) = ((int (*)(void *, uint32_t, uint32_t)) method)(obj, x_binning, y_binning);
    } else if (selector == (Method) __detector_expose) {
        double exposure_time;
        uint32_t n_frame;
        exposure_time = va_arg(*app, double);
        n_frame = va_arg(*app, uint32_t);
        void *callback_param = va_arg(*app, void *);
        *((int *) result) = ((int (*)(void *, double, uint32_t, void *)) method)(obj, exposure_time, n_frame, callback_param);
    } else if (selector == (Method) __detector_raw) {
        const void *write_buffer = va_arg(*app, const void *);
        size_t write_buffer_size = va_arg(*app, size_t);
        size_t *write_size = va_arg(*app, size_t *);
        void *read_buffer = va_arg(*app, void *);
        size_t read_buffer_size = va_arg(*app, size_t);
        size_t *read_size = va_arg(*app, size_t *);
        *((int *) result) = ((int (*)(void *, const void *, size_t, size_t *, void *, size_t, size_t *)) method)(obj, write_buffer, write_buffer_size, write_size, read_buffer, read_buffer_size, read_size);
    } else if (selector == (Method) __detector_load) {
        va_list *myapp = va_arg(*app, va_list *);
        *((int *) result) = ((int (*)(void *, va_list *)) method)(obj, myapp);
    } else if (selector == (Method) __detector_status || selector == (Method) __detector_info) {
        char *buffer = va_arg(*app, char *);
        size_t size = va_arg(*app, size_t);
        *((int *) result) = ((int (*)(void *, char *, size_t)) method)(obj, buffer, size);
    } else if (selector == (Method) __detector_set_prefix || selector == (Method) __detector_set_prefix) {
        const char *value = va_arg(*app, const char *);
        *((int *) result) = ((int (*)(void *, const char *)) method)(obj, value);
    } else {
        assert(0);
    }
    
#ifdef va_copy
    va_end(ap);
#endif
}

static void *
__Detector_ctor(void *_self, va_list *app)
{
    struct __Detector *self = super_ctor(__Detector(), _self, app);
    
    const char *s, *key, *value;
    
    s = va_arg(*app, const char *);
    self->name = (char *) Malloc(strlen(s) + 1);
    snprintf(self->name, strlen(s) + 1, "%s", s);
    self->d_cap.n_chip = 1;
    
    while ((key = va_arg(*app, const char *))) {
        if (strcmp(key, "description") == 0) {
            value = va_arg(*app, const char *);
            self->description = (char *) Malloc(strlen(value) + 1);
            snprintf(self->description, strlen(value) + 1, "%s", value);
            continue;
        }
        if (strcmp(key, "prefix") == 0) {
            value = va_arg(*app, const char *);
            self->d_proc.image_prefix = (char *) Malloc(strlen(value) + 1);
            snprintf(self->d_proc.image_prefix, strlen(value) + 1, "%s", value);
            continue;
        }
        if (strcmp(key, "directory") == 0) {
            value = va_arg(*app, const char *);
            self->d_proc.image_directory = (char *) Malloc(strlen(value) + 1);
            snprintf(self->d_proc.image_directory, strlen(value) + 1, "%s", value);
            continue;
        }
        if (strcmp(key, "template") == 0) {
            value = va_arg(*app, const char *);
            self->d_proc.tpl_filename = (char *) Malloc(strlen(value) + 1);
            snprintf(self->d_proc.tpl_filename, strlen(value) + 1, "%s", value);
            continue;
        }
        if (strcmp(key, "option") == 0) {
            self->d_state.options = va_arg(*app, uint32_t);
            continue;
        }
    }
    
    self->d_state.state = DETECTOR_STATE_UNINITIALIZED;
    self->d_state.options = DETECTOR_OPTION_NOTIFY_EACH_COMPLETION;
    self->d_exp.notify_each_frame_done = 1;

    self->d_proc.queue = new(ThreadsafeQueue(), free);
    Pthread_cond_init(&self->d_exp.cond, NULL);
    Pthread_mutex_init(&self->d_exp.mtx, NULL);
    Pthread_cond_init(&self->d_state.cond, NULL);
    Pthread_mutex_init(&self->d_state.mtx, NULL);
    
    return (void *) self;
}

static void *
__Detector_dtor(void *_self)
{
    struct __Detector *self = cast(__Detector(), _self);
    
    Pthread_cond_destroy(&self->d_state.cond);
    Pthread_mutex_destroy(&self->d_state.mtx);
    
    if (self->d_cap.x_binning_array != NULL) {
        free(self->d_cap.x_binning_array);
    }
    if (self->d_cap.y_binning_array != NULL) {
        free(self->d_cap.x_binning_array);
    }
    if (self->d_cap.gain_array != NULL) {
        free(self->d_cap.gain_array);
    }
    if (self->d_cap.readout_rate_array) {
        free(self->d_cap.readout_rate_array);
    }
    
    Pthread_cond_destroy(&self->d_exp.cond);
    Pthread_mutex_destroy(&self->d_exp.mtx);
    
    free(self->d_proc.tpl_filename);
    free(self->d_proc.image_prefix);
    free(self->d_proc.image_directory);
    
    if (self->d_proc.tpl_fptr != NULL) {
        int status = 0;
        fits_close_file(self->d_proc.tpl_fptr, &status);
    }
    if (self->d_proc.img_fptr != NULL) {
        int status = 0;
        fits_close_file(self->d_proc.img_fptr, &status);
    }
    if (self->d_proc.queue != NULL) {
        delete(self->d_proc.queue);
    }
    
    if (self->d_proc.img_filename != NULL) {
        free(self->d_proc.img_filename);
    }
    
    free(self->name);
    free(self->description);
    
    return super_dtor(__Detector(), _self);
}

static void *
__DetectorClass_ctor(void *_self, va_list *app)
{
    struct __DetectorClass *self = super_ctor(__DetectorClass(), _self, app);
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
        if (selector == (Method) __detector_clear_option) {
            if (tag) {
                self->clear_option.tag = tag;
                self->clear_option.selector = selector;
            }
            self->clear_option.method = method;
            continue;
        }
        if (selector == (Method) __detector_get_option) {
            if (tag) {
                self->get_option.tag = tag;
                self->get_option.selector = selector;
            }
            self->get_option.method = method;
            continue;
        }
        if (selector == (Method) __detector_set_option) {
            if (tag) {
                self->set_option.tag = tag;
                self->set_option.selector = selector;
            }
            self->set_option.method = method;
            continue;
        }
        if (selector == (Method) __detector_get_directory) {
            if (tag) {
                self->get_directory.tag = tag;
                self->get_directory.selector = selector;
            }
            self->get_directory.method = method;
            continue;
        }
        if (selector == (Method) __detector_set_directory) {
            if (tag) {
                self->set_directory.tag = tag;
                self->set_directory.selector = selector;
            }
            self->set_directory.method = method;
            continue;
        }
        if (selector == (Method) __detector_get_prefix) {
            if (tag) {
                self->get_prefix.tag = tag;
                self->get_prefix.selector = selector;
            }
            self->get_prefix.method = method;
            continue;
        }
        if (selector == (Method) __detector_set_prefix) {
            if (tag) {
                self->set_prefix.tag = tag;
                self->set_prefix.selector = selector;
            }
            self->set_prefix.method = method;
            continue;
        }
        if (selector == (Method) __detector_get_template) {
            if (tag) {
                self->get_template.tag = tag;
                self->get_template.selector = selector;
            }
            self->get_template.method = method;
            continue;
        }
        if (selector == (Method) __detector_set_template) {
            if (tag) {
                self->set_template.tag = tag;
                self->set_template.selector = selector;
            }
            self->set_template.method = method;
            continue;
        }
        if (selector == (Method) __detector_set_pre_acquisition) {
            if (tag) {
                self->set_pre_acquisition.tag = tag;
                self->set_pre_acquisition.selector = selector;
            }
            self->set_pre_acquisition.method = method;
            continue;
        }
        if (selector == (Method) __detector_set_post_acquisition) {
            if (tag) {
                self->set_post_acquisition.tag = tag;
                self->set_post_acquisition.selector = selector;
            }
            self->set_post_acquisition.method = method;
            continue;
        }
        if (selector == (Method) __detector_set_name_convention) {
            if (tag) {
                self->set_name_convention.tag = tag;
                self->set_name_convention.selector = selector;
            }
            self->set_name_convention.method = method;
            continue;
        }
        if (selector == (Method) __detector_power_on) {
            if (tag) {
                self->power_on.tag = tag;
                self->power_on.selector = selector;
            }
            self->power_on.method = method;
            continue;
        }
        if (selector == (Method) __detector_power_off) {
            if (tag) {
                self->power_off.tag = tag;
                self->power_off.selector = selector;
            }
            self->power_off.method = method;
            continue;
        }
        if (selector == (Method) __detector_init) {
            if (tag) {
                self->init.tag = tag;
                self->init.selector = selector;
            }
            self->init.method = method;
            continue;
        }
        if (selector == (Method) __detector_get_binning) {
            if (tag) {
                self->get_binning.tag = tag;
                self->get_binning.selector = selector;
            }
            self->get_binning.method = method;
            continue;
        }
        if (selector == (Method) __detector_set_binning) {
            if (tag) {
                self->set_binning.tag = tag;
                self->set_binning.selector = selector;
            }
            self->set_binning.method = method;
            continue;
        }
        if (selector == (Method) __detector_get_gain) {
            if (tag) {
                self->get_gain.tag = tag;
                self->get_gain.selector = selector;
            }
            self->get_gain.method = method;
            continue;
        }
        if (selector == (Method) __detector_set_gain) {
            if (tag) {
                self->set_gain.tag = tag;
                self->set_gain.selector = selector;
            }
            self->set_gain.method = method;
            continue;
        }
        if (selector == (Method) __detector_get_frame_rate) {
            if (tag) {
                self->get_frame_rate.tag = tag;
                self->get_frame_rate.selector = selector;
            }
            self->get_frame_rate.method = method;
            continue;
        }
        if (selector == (Method) __detector_set_frame_rate) {
            if (tag) {
                self->set_frame_rate.tag = tag;
                self->set_frame_rate.selector = selector;
            }
            self->set_frame_rate.method = method;
            continue;
        }
        if (selector == (Method) __detector_set_exposure_time) {
            if (tag) {
                self->set_exposure_time.tag = tag;
                self->set_exposure_time.selector = selector;
            }
            self->set_exposure_time.method = method;
            continue;
        }
        if (selector == (Method) __detector_get_exposure_time) {
            if (tag) {
                self->get_exposure_time.tag = tag;
                self->get_exposure_time.selector = selector;
            }
            self->get_exposure_time.method = method;
            continue;
        }
        if (selector == (Method) __detector_set_readout_rate) {
            if (tag) {
                self->set_readout_rate.tag = tag;
                self->set_readout_rate.selector = selector;
            }
            self->set_readout_rate.method = method;
            continue;
        }
        if (selector == (Method) __detector_get_readout_rate) {
            if (tag) {
                self->get_readout_rate.tag = tag;
                self->get_readout_rate.selector = selector;
            }
            self->get_readout_rate.method = method;
            continue;
        }
        if (selector == (Method) __detector_set_region) {
            if (tag) {
                self->set_region.tag = tag;
                self->set_region.selector = selector;
            }
            self->set_region.method = method;
            continue;
        }
        if (selector == (Method) __detector_get_region) {
            if (tag) {
                self->get_region.tag = tag;
                self->get_region.selector = selector;
            }
            self->get_region.method = method;
            continue;
        }
        if (selector == (Method) __detector_expose) {
            if (tag) {
                self->expose.tag = tag;
                self->expose.selector = selector;
            }
            self->expose.method = method;
            continue;
        }
        if (selector == (Method) __detector_raw) {
            if (tag) {
                self->raw.tag = tag;
                self->raw.selector = selector;
            }
            self->raw.method = method;
            continue;
        }
        if (selector == (Method) __detector_load) {
            if (tag) {
                self->load.tag = tag;
                self->load.selector = selector;
            }
            self->load.method = method;
            continue;
        }
        if (selector == (Method) __detector_reload) {
            if (tag) {
                self->reload.tag = tag;
                self->reload.selector = selector;
            }
            self->reload.method = method;
            continue;
        }
        if (selector == (Method) __detector_unload) {
            if (tag) {
                self->unload.tag = tag;
                self->unload.selector = selector;
            }
            self->unload.method = method;
            continue;
        }
        if (selector == (Method) __detector_inspect) {
            if (tag) {
                self->inspect.tag = tag;
                self->inspect.selector = selector;
            }
            self->inspect.method = method;
            continue;
        }
        if (selector == (Method) __detector_wait) {
            if (tag) {
                self->wait.tag = tag;
                self->wait.selector = selector;
            }
            self->wait.method = method;
            continue;
        }
        if (selector == (Method) __detector_wait_for_completion) {
            if (tag) {
                self->wait_for_completion.tag = tag;
                self->wait_for_completion.selector = selector;
            }
            self->wait_for_completion.method = method;
            continue;
        }
        if (selector == (Method) __detector_status) {
            if (tag) {
                self->status.tag = tag;
                self->status.selector = selector;
            }
            self->status.method = method;
            continue;
        }
        if (selector == (Method) __detector_info) {
            if (tag) {
                self->info.tag = tag;
                self->info.selector = selector;
            }
            self->info.method = method;
            continue;
        }
        if (selector == (Method) __detector_abort) {
            if (tag) {
                self->abort.tag = tag;
                self->abort.selector = selector;
            }
            self->abort.method = method;
            continue;
        }
        if (selector == (Method) __detector_stop) {
            if (tag) {
                self->stop.tag = tag;
                self->stop.selector = selector;
            }
            self->stop.method = method;
            continue;
        }
        if (selector == (Method) __detector_get_name) {
            if (tag) {
                self->get_name.tag = tag;
                self->get_name.selector = selector;
            }
            self->get_name.method = method;
            continue;
        }
        if (selector == (Method) __detector_get_temperature) {
            if (tag) {
                self->get_temperature.tag = tag;
                self->get_temperature.selector = selector;
            }
            self->get_temperature.method = method;
            continue;
        }
        if (selector == (Method) __detector_set_temperature) {
            if (tag) {
                self->set_temperature.tag = tag;
                self->set_temperature.selector = selector;
            }
            self->set_temperature.method = method;
            continue;
        }
        if (selector == (Method) __detector_enable_cooling) {
            if (tag) {
                self->enable_cooling.tag = tag;
                self->enable_cooling.selector = selector;
            }
            self->enable_cooling.method = method;
            continue;
        }
        if (selector == (Method) __detector_disable_cooling) {
            if (tag) {
                self->disable_cooling.tag = tag;
                self->disable_cooling.selector = selector;
            }
            self->disable_cooling.method = method;
            continue;
        }
    }
    
#ifdef va_copy
    va_end(ap);
#endif
    
    return (void *) self;
}

static const void *___DetectorClass;

static void
__DetectorClass_destroy(void)
{
    free((void *) ___DetectorClass);
}

static void
__DetectorClass_initialize(void)
{
    ___DetectorClass = new(Class(), "__DetectorClass", Class(), sizeof(struct __DetectorClass),
                         ctor, "ctor", __DetectorClass_ctor,
                         (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(__DetectorClass_destroy);
#endif
}

const void *
__DetectorClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, __DetectorClass_initialize);
#endif
    
    return ___DetectorClass;
}

static const void *___Detector;

static void
__Detector_destroy(void)
{
    free((void *)___Detector);
}

static void
__Detector_initialize(void)
{

    ___Detector = new(__DetectorClass(), "__Detector", Object(), sizeof(struct __Detector),
                      ctor, "ctor", __Detector_ctor,
                      dtor, "dtor", __Detector_dtor,
                      forward, "forward", __Detector_forward,
                      __detector_clear_option, "clear_option", __Detector_clear_option,
                      __detector_set_option, "set_option", __Detector_set_option,
                      __detector_get_option, "get_option", __Detector_get_option,
                      __detector_set_directory, "set_directory", __Detector_set_directory,
                      __detector_get_directory, "get_directory", __Detector_get_directory,
                      __detector_set_prefix, "set_prefix", __Detector_set_prefix,
                      __detector_get_prefix, "get_prefix", __Detector_get_prefix,
                      __detector_set_template, "set_template", __Detector_set_template,
                      __detector_get_template, "get_template", __Detector_get_template,
                      __detector_set_pre_acquisition, "set_pre_acquisition", __Detector_set_pre_acquisition,
                      __detector_set_post_acquisition, "set_pre_acquisition", __Detector_set_post_acquisition,
                      __detector_set_name_convention, "set_name_convention", __Detector_set_name_convention,
                      
                      __detector_wait, "wait", __Detector_wait,
                      __detector_power_on, "power_on", __Detector_power_on,
                      __detector_power_off, "power_off", __Detector_power_off,
                      __detector_enable_cooling, "enable_cooling", __Detector_enable_cooling,
                      __detector_disable_cooling, "disable_cooling", __Detector_disable_cooling,
                      __detector_init, "init", __Detector_init,
                      __detector_set_binning, "set_binning", __Detector_set_binning,
                      __detector_get_binning, "get_binning", __Detector_get_binning,
                      __detector_set_exposure_time, "set_exposure_time", __Detector_set_exposure_time,
                      __detector_get_exposure_time, "get_exposure_time", __Detector_get_exposure_time,
                      __detector_set_frame_rate, "set_frame_rate", __Detector_set_frame_rate,
                      __detector_get_frame_rate, "get_frame_rate", __Detector_get_frame_rate,
                      __detector_set_gain, "set_gain", __Detector_set_gain,
                      __detector_get_gain, "get_gain", __Detector_get_gain,
                      __detector_set_readout_rate, "set_readout_rate", __Detector_set_readout_rate,
                      __detector_get_readout_rate, "get_readout_rate", __Detector_get_readout_rate,
                      __detector_set_temperature, "set_temperature", __Detector_set_temperature,
                      __detector_get_temperature, "get_temperature", __Detector_get_temperature,
                      __detector_set_region, "set_region", __Detector_set_region,
                      __detector_get_region, "get_region", __Detector_get_region,
                      __detector_get_name, "get_name", __Detector_get_name,
                      
                      //__detector_raw, "raw", __Detector_raw,
                      //__detector_load, "load", __Detector_load,
                      //__detector_reload, "reload", __Detector_reload,
                      //__detector_unload, "unload", __Detector_unload,
                    (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(__Detector_destroy);
#endif
}

const void *
__Detector(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, __Detector_initialize);
#endif
    
    return ___Detector;
}

/*
 * Virtual detector.
 */

static const void *virtual_detector_virtual_table(void);

static void *
VirtualDetector_ctor(void *_self, va_list *app)
{
    struct VirtualDetector *self = super_ctor(VirtualDetector(), _self, app);
    
    self->_.d_state.state = DETECTOR_STATE_UNINITIALIZED;
    const char *name = va_arg(*app, const char *);
    
   
    
    self->_._vtab= virtual_detector_virtual_table();
    
    return (void *) self;
}

static void *
VirtualDetector_dtor(void *_self)
{
    struct VirtualDetector *self = cast(VirtualDetector(), _self);
    
    return super_dtor(VirtualDetector(), _self);
}

static void *
VirtualDetectorClass_ctor(void *_self, va_list *app)
{
    struct VirtualDetectorClass *self = super_ctor(VirtualDetectorClass(), _self, app);
    
    self->_.raw.method = (Method) 0;
    self->_.init.method = (Method) 0;
    self->_.status.method = (Method) 0;
    
    self->_.get_binning.method = (Method) 0;
    self->_.set_binning.method = (Method) 0;
    self->_.set_exposure_time.method = (Method) 0;
    self->_.get_exposure_time.method = (Method) 0;
    self->_.set_frame_rate.method = (Method) 0;
    self->_.get_frame_rate.method = (Method) 0;
    self->_.set_gain.method = (Method) 0;
    self->_.get_gain.method = (Method) 0;
    self->_.set_region.method = (Method) 0;
    self->_.get_region.method = (Method) 0;
    self->_.inspect.method = (Method) 0;
    //self->_.wait.method = (Method) 0;
    self->_.wait_for_completion.method = (Method) 0;
    self->_.raw.method = (Method) 0;
    self->_.expose.method = (Method) 0;
    self->_.status.method = (Method) 0;
    self->_.info.method = (Method) 0;
    self->_.stop.method = (Method) 0;
    self->_.abort.method = (Method) 0;
    
    return self;
}

static const void *_VirtualDetectorClass;

static void
VirtualDetectorClass_destroy(void)
{
    free((void *) _VirtualDetectorClass);
}

static void
VirtualDetectorClass_initialize(void)
{
    _VirtualDetectorClass = new(__DetectorClass(), "VirtualDetectorClass", __DetectorClass(), sizeof(struct VirtualDetectorClass),
                                ctor, "", VirtualDetectorClass_ctor,
                                (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(VirtualDetectorClass_destroy);
#endif
}

const void *
VirtualDetectorClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, VirtualDetectorClass_initialize);
#endif
    
    return _VirtualDetectorClass;
}

static const void *_VirtualDetector;

static void
VirtualDetector_destroy(void)
{
    free((void *)_VirtualDetector);
}

static void
VirtualDetector_initialize(void)
{
    _VirtualDetector = new(VirtualDetectorClass(), "VirtualDetector", __Detector(), sizeof(struct VirtualDetector),
                           ctor, "ctor", VirtualDetector_ctor,
                           dtor, "dtor", VirtualDetector_dtor,
                           (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(VirtualDetector_destroy);
#endif
}

const void *
VirtualDetector(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, VirtualDetector_initialize);
#endif

    return _VirtualDetector;
}

static int
VirtualDetector_power_on(void *_self)
{
    struct VirtualDetector *self = cast(VirtualDetector(), _self);
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    if (self->_.d_state.state == DETECTOR_STATE_OFFLINE) {
        self->_.d_state.state = DETECTOR_STATE_IDLE;
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    return AAOS_OK;
}

static int
VirtualDetector_init(void *_self)
{
    struct VirtualDetector *self = cast(VirtualDetector(), _self);
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    if (self->_.d_state.state == DETECTOR_STATE_UNINITIALIZED) {
        self->_.d_state.state = DETECTOR_STATE_IDLE;
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    return AAOS_OK;
}

static int
VirtualDetector_power_off(void *_self)
{
    struct VirtualDetector *self = cast(VirtualDetector(), _self);
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    while (self->_.d_state.state == DETECTOR_STATE_READING || self->_.d_state.state == DETECTOR_STATE_EXPOSING) {
        Pthread_cond_wait(&self->_.d_state.cond, &self->_.d_state.mtx);
    }
    self->_.d_state.state = DETECTOR_STATE_OFFLINE;
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    return AAOS_OK;
}

static void
VirtualDetector_generate_frame(struct VirtualDetector *detector, void **data)
{
    size_t width = detector->_.d_param.image_width, height = detector->_.d_param.image_width;
    
    /*
    double gain = detector->_.d_param.gain, read_noise = detector->read_noise, bias = detector->bias_level;
     */
    switch (detector->_.d_param.pixel_format) {
        case DETECTOR_PIXEL_FORMAT_MONO_8:
            *data = Malloc(width * height);
            break;
        case DETECTOR_PIXEL_FORMAT_MONO_12:
        case DETECTOR_PIXEL_FORMAT_MONO_14:
        case DETECTOR_PIXEL_FORMAT_MONO_16:
            *data = Malloc(width * height * 2);
            break;
        case DETECTOR_PIXEL_FORMAT_MONO_18:
        case DETECTOR_PIXEL_FORMAT_MONO_24:
        case DETECTOR_PIXEL_FORMAT_MONO_32:
            *data = Malloc(width * height * 4);
            break;
        case DETECTOR_PIXEL_FORMAT_MONO_64:
            *data = Malloc(width * height * 8);
            break;
        default:
            break;
    }
    
}

static void *
VirtualDetector_do_expose(void *arg)
{
    va_list *app = (va_list *) arg;
    
    struct VirtualDetector *detector = va_arg(*app, struct VirtualDetector *);
    
    size_t i, n = detector->_.d_exp.request_frames;
    double frame_length = 1. / detector->_.d_param.frame_rate;
    double readout_time = detector->_.d_param.image_width * detector->_.d_param.image_height / detector->_.d_param.readout_rate;
    double gap_time = frame_length - readout_time - detector->_.d_param.exposure_time;
    int bitpix = USHORT_IMG, datatype = TUSHORT;
    long naxes[2];
    
    char *filename = NULL;
    int status = 0;
    
    switch (detector->_.d_param.pixel_format) {
        case 8:
            bitpix = BYTE_IMG;
            datatype = TBYTE;
            break;
        case 16:
            bitpix = USHORT_IMG;
            datatype = TUSHORT;
            break;
        case 32:
            bitpix = ULONG_IMG;
            datatype = TULONG;
            break;
        case 64:
            bitpix = ULONGLONG_IMG;
            datatype = TULONGLONG;
            break;
        default:
            break;
    }
    
    naxes[0] = detector->_.d_param.image_width;
    naxes[1] = detector->_.d_param.image_width;

    if (!detector->_.d_exp.notify_each_frame_done) {
        filename = (char *) Malloc(FILENAMESIZE);
        if (detector->_.d_proc.tpl_fptr == NULL) {
            if (detector->_.d_proc.tpl_filename != NULL) {
                if (detector->_.d_proc.image_directory != NULL) {
                    snprintf(filename, FILENAMESIZE, "%s/%s", detector->_.d_proc.image_directory, detector->_.d_proc.tpl_filename);
                } else {
                    snprintf(filename, FILENAMESIZE, "%s", detector->_.d_proc.tpl_filename);
                }
                fitsfile *tpl_fptr, *img_fptr;
                fits_open_file(&tpl_fptr, filename, READONLY, &status);
                detector->_.d_proc.name_convention(detector, filename, FILENAMESIZE, app);
                if (status == 0) {
                    detector->_.d_proc.tpl_fptr = (void *) tpl_fptr;
                    fits_create_file(&img_fptr, filename, &status);
                    detector->_.d_proc.img_fptr = (void *) img_fptr;
                    fits_copy_file(tpl_fptr, img_fptr, 1, 1, 1, &status);
                } else {
                    fits_create_file(&img_fptr, filename, &status);
                    detector->_.d_proc.img_fptr = (void *) img_fptr;
                }
            } else {
                detector->_.d_proc.name_convention(detector, filename, FILENAMESIZE, app);
                fitsfile *tpl_fptr, *img_fptr;
                tpl_fptr = (fitsfile *) detector->_.d_proc.tpl_fptr;
                fits_create_file(&img_fptr, filename, &status);
                fits_copy_file(tpl_fptr, img_fptr, 1, 1, 1, &status);
                detector->_.d_proc.img_fptr = (void *) img_fptr;
            }
        } else {
            fitsfile *img_fptr, *tpl_fptr;
            tpl_fptr = (fitsfile *) detector->_.d_proc.tpl_fptr;
            detector->_.d_proc.name_convention(detector, filename, FILENAMESIZE, app);
            fits_create_file(&img_fptr, filename, &status);
            fits_copy_file(tpl_fptr, img_fptr, 1, 1, 1, &status);
            detector->_.d_proc.img_fptr = (void *) img_fptr;
        }
    }
    for (i = 1; i < n; i++) {
        void *data;
        fitsfile *img_fptr;
        Pthread_mutex_lock(&detector->mtx);
        if (detector->stop_flag) {
            Pthread_mutex_unlock(&detector->mtx);
            return NULL;
        }
        Pthread_mutex_unlock(&detector->mtx);
        
        /*
         * Simulating exposing.
         */
        Pthread_mutex_lock(&detector->_.d_state.mtx);
        detector->_.d_state.state = DETECTOR_STATE_EXPOSING;
        Pthread_mutex_unlock(&detector->_.d_state.mtx);
        Nanosleep(detector->_.d_param.exposure_time);
        
        /*
         * Simulating reading out.
         */
        Pthread_mutex_lock(&detector->_.d_state.mtx);
        detector->_.d_state.state = DETECTOR_STATE_READING;
        Pthread_mutex_unlock(&detector->_.d_state.mtx);
        Nanosleep(readout_time);
        
        /*
         * After read out, start processing data.
         */
        if (detector->_.d_exp.notify_last_frame_filling && i == n - 1) {
            Pthread_mutex_lock(&detector->_.d_exp.mtx);
            detector->_.d_exp.last_frame_filling_flag = 1;
            Pthread_mutex_unlock(&detector->_.d_exp.mtx);
            Pthread_cond_signal(&detector->_.d_exp.cond);
        }
        VirtualDetector_generate_frame(detector, &data);
        if (detector->_.d_exp.notify_each_frame_done) {
            filename = (char *) Malloc(FILENAMESIZE);
            if (detector->_.d_proc.tpl_fptr == NULL) {
                if (detector->_.d_proc.image_directory != NULL) {
                    snprintf(filename, FILENAMESIZE, "%s/%s", detector->_.d_proc.image_directory, detector->_.d_proc.tpl_filename);
                } else {
                    snprintf(filename, FILENAMESIZE, "%s", detector->_.d_proc.tpl_filename);
                }
                fitsfile *tpl_fptr;
                fits_open_file(&tpl_fptr, filename, READONLY, &status);
                detector->_.d_proc.name_convention(detector, filename, FILENAMESIZE, app);
                if (status == 0) {
                    detector->_.d_proc.tpl_fptr = (void *) tpl_fptr;
                    fits_create_file(&img_fptr, filename, &status);
                    detector->_.d_proc.img_fptr = (void *) img_fptr;
                    fits_copy_file(tpl_fptr, img_fptr, 1, 1, 1, &status);
                } else {
                    fits_create_file(&img_fptr, filename, &status);
                    detector->_.d_proc.img_fptr = (void *) img_fptr;
                }
            } else {
                detector->_.d_proc.name_convention(detector, filename, FILENAMESIZE, app);
                fitsfile *tpl_fptr = (fitsfile *) detector->_.d_proc.tpl_fptr;
                fits_create_file(&img_fptr, filename, &status);
                fits_copy_file(tpl_fptr, img_fptr, 1, 1, 1, &status);
                detector->_.d_proc.img_fptr = (void *) img_fptr;
            }
            fits_create_img(img_fptr, bitpix, 2, naxes, &status);
            fits_write_img(img_fptr, datatype, 1, naxes[0] * naxes[1], (void *) data, &status);
            fits_close_file(img_fptr, &status);
            free(data);
            threadsafe_queue_push(detector->_.d_proc.queue, filename);
        } else {
            img_fptr = (fitsfile *) detector->_.d_proc.img_fptr;
            fits_create_img(img_fptr, bitpix, 2, naxes, &status);
            fits_write_img(img_fptr, datatype, 1, naxes[0] * naxes[1], (void *) data, &status);
            free(data);
            if (i == n - 1) {
                fits_close_file(img_fptr, &status);
                detector->_.d_proc.img_fptr = NULL;
                threadsafe_queue_push(detector->_.d_proc.queue, filename);
            }
        }
        if (gap_time > 0.00001) {
            Nanosleep(gap_time);
        }
        Pthread_mutex_lock(&detector->_.d_state.mtx);
        detector->_.d_state.state = DETECTOR_STATE_IDLE;
        Pthread_mutex_unlock(&detector->_.d_state.mtx);
    }
    return NULL;
}

static int
VirtualDetector_expose(void *_self, double exposure_time, uint32_t n_frame, va_list *app)
{
    struct VirtualDetector *self = cast(VirtualDetector(), _self);
    
    pthread_t tid;
    uint32_t i;
    void *retval;
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    while (self->_.d_state.state == DETECTOR_STATE_READING || self->_.d_state.state == DETECTOR_STATE_EXPOSING) {
        Pthread_cond_wait(&self->_.d_state.cond, &self->_.d_state.mtx);
    }
    self->_.d_state.state = DETECTOR_STATE_EXPOSING;
    self->_.d_param.exposure_time = exposure_time;
    if (exposure_time > 1. / self->_.d_param.frame_rate) {
        self->_.d_param.frame_rate = 1. / exposure_time;
    }
    self->_.d_exp.request_frames = n_frame;
    Pthread_create(&tid, NULL, VirtualDetector_do_expose, (void *) self);
    self->_.d_state.tid = tid;
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    char *filename;
    
    if (self->_.d_proc.pre_acquisition != NULL) {
#ifdef va_copy
        va_list ap;
        va_copy(ap, *app);
        self->_.d_proc.pre_acquisition(self, filename, &ap);
        va_end(ap);
#else
        va_list *myapp = app;
        self->_.d_proc.pre_acquisition(self, filename, myapp);
#endif
    }
    if (self->_.d_exp.notify_each_frame_done) {
        for (i = 0; i != n_frame - 1; i ++) {
            Pthread_mutex_lock(&self->mtx);
            if (self->stop_flag) {
                while ((filename = threadsafe_queue_try_pop(self->_.d_proc.queue)) != NULL) {
                    if (self->_.d_proc.post_acquisition != NULL) {
#ifdef va_copy
                        va_list ap;
                        va_copy(ap, *app);
                        self->_.d_proc.post_acquisition(self, filename, &ap);
                        va_end(ap);
#else
                        va_list *myapp = app;
                        self->_.d_proc.post_acquisition(self, filename, myapp);
#endif
                    } else {
                        fprintf(stdout, "%s\n", filename);
                    }
                    free(filename);
                }
                self->stop_flag = 0;
                Pthread_mutex_unlock(&self->mtx);
                return AAOS_EINTR;
            }
            Pthread_mutex_unlock(&self->mtx);
            filename = threadsafe_queue_wait_and_pop(self->_.d_proc.queue);
            if (self->_.d_proc.post_acquisition != NULL) {
#ifdef va_copy
                va_list ap;
                va_copy(ap, *app);
                self->_.d_proc.post_acquisition(self, filename, &ap);
                va_end(ap);
#else
                va_list *myapp = app;
                self->_.d_proc.post_acquisition(self, filename, myapp);
#endif
            } else {
                fprintf(stdout, "%s\n", filename);
            }
            free(filename);
        }
    }
    if (self->_.d_exp.notify_last_frame_filling) {
        Pthread_mutex_lock(&self->_.d_exp.mtx);
        while (self->_.d_exp.last_frame_filling_flag) {
            Pthread_cond_wait(&self->_.d_exp.cond, &self->_.d_exp.mtx);
        }
        self->_.d_exp.last_frame_filling_flag = 0;
        Pthread_mutex_unlock(&self->_.d_exp.mtx);
        return AAOS_OK;
    }
    
    filename = threadsafe_queue_wait_and_pop(self->_.d_proc.queue);
    if (self->_.d_proc.post_acquisition != NULL) {
#ifdef va_copy
        va_list ap;
        va_copy(ap, *app);
        self->_.d_proc.post_acquisition(self, filename, &ap);
        va_end(ap);
#else
        va_list *myapp = app;
        self->_.d_proc.post_acquisition(self, filename, myapp);
#endif
    } else {
        fprintf(stdout, "%s\n", filename);
    }
    free(filename);
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    if (self->_.d_state.state == DETECTOR_STATE_READING) {
        self->_.d_state.state = DETECTOR_STATE_IDLE;
    }
    Pthread_mutex_lock(&self->mtx);
    self->stop_flag = 0;
    Pthread_mutex_unlock(&self->mtx);
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    Pthread_cond_broadcast(&self->_.d_state.cond);
    
    Pthread_join(tid, &retval);
    
    if (retval == PTHREAD_CANCELED) {
        return AAOS_EINTR;
    } else {
        return AAOS_OK;
    }
}

static int
VirtualDetector_wait_for_completion(void *_self, ...)
{
    struct VirtualDetector *self = cast(VirtualDetector(), _self);
    
    if (self->_.d_exp.notify_last_frame_filling) {
        Pthread_mutex_lock(&self->_.d_exp.mtx);
        while (self->_.d_exp.last_frame_filling_flag) {
            Pthread_cond_wait(&self->_.d_exp.cond, &self->_.d_exp.mtx);
        }
        self->_.d_exp.last_frame_filling_flag = 0;
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        char *filename;
        filename = threadsafe_queue_wait_and_pop(self->_.d_proc.queue);
        va_list ap;
        va_start(ap, _self);
        self->_.d_proc.post_acquisition(self, filename, &ap);
        va_end(ap);
        free(filename);
    }
    Pthread_mutex_lock(&self->_.d_state.mtx);
    if (self->_.d_state.state == DETECTOR_STATE_READING) {
        self->_.d_state.state = DETECTOR_STATE_IDLE;
    }
    Pthread_mutex_lock(&self->mtx);
    self->stop_flag = 0;
    Pthread_mutex_unlock(&self->mtx);
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    Pthread_cond_broadcast(&self->_.d_state.cond);
    
    return AAOS_OK;
}

static int
VirtualDetector_stop(void *_self)
{
    struct VirtualDetector *self = cast(VirtualDetector(), _self);
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    if (self->_.d_state.state == DETECTOR_STATE_READING || self->_.d_state.state == DETECTOR_STATE_EXPOSING) {
        Pthread_mutex_lock(&self->mtx);
        self->stop_flag = 1;
        Pthread_mutex_unlock(&self->mtx);
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);

    return AAOS_OK;
}

static int
VirtualDetector_abort(void *_self)
{
    struct VirtualDetector *self = cast(VirtualDetector(), _self);
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    if (self->_.d_state.state == DETECTOR_STATE_READING || self->_.d_state.state == DETECTOR_STATE_EXPOSING) {
        Pthread_cancel(self->_.d_state.tid);
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    return AAOS_OK;
}

static int
VirtualDetector_inspect(void *_self)
{
    struct VirtualDetector *self = cast(VirtualDetector(), _self);
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    if (self->_.d_state.state == DETECTOR_STATE_MALFUNCTION) {
        self->_.d_state.state = DETECTOR_STATE_IDLE;
        Pthread_cond_broadcast(&self->_.d_state.cond);
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);

    return AAOS_OK;
}

static const void *_virtual_detector_virtual_table;

static void
virtual_detector_virtual_table_destroy(void)
{
    delete((void *) _virtual_detector_virtual_table);
}

static void
virtual_detector_virtual_table_initialize(void)
{
    _virtual_detector_virtual_table = new(__DetectorVirtualTable(),
                                 
                                          //__detector_init, "init", Virtual_init,
                                 
                                          //__detector_info, "info", GenICam_info,
                                 
                                      
                                          //__detector_set_binning, "set_binning", GenICam_set_binning,
                                 
                                          //__detector_get_binning, "get_binning", GenICam_get_binning,
                                
                                     
                                          //__detector_get_exposure_time, "get_exposure_time", GenICam_get_exposure_time,
                                 
                                          //__detector_set_frame_rate, "set_frame_rate", GenICam_set_frame_rate,
                                 
                                          //__detector_get_frame_rate, "get_frame_rate", GenICam_get_frame_rate,
                                 
                                          //__detector_set_gain, "set_gain", GenICam_set_gain,
                                 
                                          //__detector_get_gain, "get_gain", GenICam_get_gain,
                                 
                                          //__detector_set_region, "set_region", GenICam_set_region,
                                 
                                          //__detector_get_region, "get_region", GenICam_get_region,
                                 
                                          //__detector_raw, "raw", GenICam_raw,
                                 
                                          //__detector_inspect, "inspect", GenICam_inspect,
                                          //__detector_wait_for_completion, "wait_for_completion", GenICam_wait_for_last_completion,
                                 (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(virtual_detector_virtual_table_destroy);
#endif
}

static const void *
virtual_detector_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, virtual_detector_virtual_table_initialize);
#endif
    
    return _virtual_detector_virtual_table;
}

/*
 * SITIAN Camera
 */

#include "net.h"

static const char *pattern_sitian_cam = "^MSG=((ExpoTime:)|(HDR:[0-2])|(ObjectType:[0-4])|(GetTemp)|(SetTemp:-?([0-9]*.))|(SetDir:)|(MultiStart::)|(ExpoStart)|(ExpoStop)|(Status)|(ExpoAbort))\r\n$";
static regex_t preg_sitiancam;

static const void *sitiancam_virtual_table(void);

static void *
SitianCam_ctor(void *_self, va_list *app)
{
    struct SitianCam *self = super_ctor(SitianCam(), _self, app);
    
    const char *value;
    
    self->_.d_state.state = DETECTOR_STATE_UNINITIALIZED;
    value = va_arg(*app, const char *);
    self->address = (char *) Malloc(strlen(value) + 1);
    snprintf(self->address, strlen(value) + 1, "%s", value);
    value = va_arg(*app, const char *);
    self->port = (char *) Malloc(strlen(value) + 1);
    snprintf(self->port, strlen(value) + 1, "%s", value);
    
    self->_._vtab= sitiancam_virtual_table();
    
    return (void *) self;
}

static void *
SitianCam_dtor(void *_self)
{
    struct SitianCam *self = cast(SitianCam(), _self);
    
    free(self->address);
    free(self->port);
    
    if (self->camera != NULL) {
        delete(self->camera);
    }

    return super_dtor(SitianCam(), _self);
}

static void *
SitianCamClass_ctor(void *_self, va_list *app)
{
    struct SitianCamClass *self = super_ctor(SitianCamClass(), _self, app);
    
    self->_.raw.method = (Method) 0;
    self->_.init.method = (Method) 0;
    self->_.status.method = (Method) 0;
    
    self->_.inspect.method = (Method) 0;
    //self->_.wait.method = (Method) 0;
    //self->_.wait_for_completion.method = (Method) 0;
    self->_.raw.method = (Method) 0;
    self->_.expose.method = (Method) 0;
    
    return self;
}

static const void *_SitianCamClass;

static void
SitianCamClass_destroy(void)
{
    free((void *) _SitianCamClass);
}

static void
SitianCamClass_initialize(void)
{
    _SitianCamClass = new(__DetectorClass(), "SitianCamClass", __DetectorClass(), sizeof(struct SitianCamClass),
                          ctor, "", SitianCamClass_ctor,
                          (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(SitianCamClass_destroy);
#endif
}

const void *
SitianCamClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, SitianCamClass_initialize);
#endif
    
    return _SitianCamClass;
}

static const void *_SitianCam;

static void
SitianCam_destroy(void)
{
    free((void *)_SitianCam);
}

static void
SitianCam_initialize(void)
{
    _SitianCam = new(SitianCamClass(), "SitianCam", __Detector(), sizeof(struct SitianCam),
                     ctor, "ctor", SitianCam_ctor,
                     dtor, "dtor", SitianCam_dtor,
                     (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(SitianCam_destroy);
#endif
}

const void *
SitianCam(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, SitianCam_initialize);
#endif

    return _SitianCam;
}

static int
SitianCam_init(void *_self)
{
    struct SitianCam *self = cast(SitianCam(), _self);
    
    void *client;
    int ret = AAOS_OK;
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    switch (self->_.d_state.state) {
        case DETECTOR_STATE_UNINITIALIZED:
            client = new(TCPClient(), self->address, self->port);
            ret = tcp_client_connect(client, &self->camera);
            delete(client);
            break;
        case DETECTOR_STATE_MALFUNCTION:
            ret = AAOS_EDEVMAL;
            break;
        default:
            break;
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    return ret;
}

static int
SitianCam_raw(void *_self, const void *write_buffer, size_t write_buffer_size, size_t *write_size, void *read_buffer, size_t read_buffer_size, size_t *read_size)
{
    struct SitianCam *self = cast(SitianCam(), _self);
    
    int ret;
    
    if ((ret = tcp_socket_write(self->camera, write_buffer, write_buffer_size, write_size)) != AAOS_OK) {
        return ret;
    }
    
    if ((ret = tcp_socket_read_until(self->camera, read_buffer, read_buffer_size, read_size, "\r\n")) != AAOS_OK) {
        return ret;
    }
    
    return AAOS_OK;
}

static int
SitianCam_set_exposure_time(void *_self, double exposure_time)
{
    struct SitianCam *self = cast(SitianCam(), _self);
    
    int ret = AAOS_OK;
    char buffer[BUFSIZE];
    void *camera = self->camera;
    char tmp[256];
    int error_code;
    const char *s;
    
    memset(tmp, '\0', 256);
    snprintf(buffer, BUFSIZE, "MSG=ExpoTime:%.2f\r\n", exposure_time);
    Pthread_mutex_lock(&self->_.d_state.mtx);
    switch (self->_.d_state.state) {
        case DETECTOR_STATE_MALFUNCTION:
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EDEVMAL;
            break;
        case DETECTOR_STATE_UNINITIALIZED:
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EUNINIT;
            break;
        case DETECTOR_STATE_READING:
        case DETECTOR_STATE_EXPOSING:
            while (self->_.d_state.state == DETECTOR_STATE_EXPOSING || self->_.d_state.state == DETECTOR_STATE_READING) {
                Pthread_cond_wait(&self->_.d_state.cond, &self->_.d_state.mtx);
            }
            switch (self->_.d_state.state) {
                case DETECTOR_STATE_MALFUNCTION:
                    Pthread_mutex_unlock(&self->_.d_state.mtx);
                    return AAOS_EDEVMAL;
                    break;
                case DETECTOR_STATE_UNINITIALIZED:
                    Pthread_mutex_unlock(&self->_.d_state.mtx);
                    return AAOS_EUNINIT;
                    break;
                case DETECTOR_STATE_IDLE:
                    if ((ret = tcp_socket_write(camera, buffer, strlen(buffer), NULL)) != AAOS_OK) {
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        return ret;
                    }
                    if ((ret = tcp_socket_read_until(camera, buffer, BUFSIZE, NULL, "\r\n")) != AAOS_OK) {
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        return ret;
                    }
                    s = strchr(buffer, ':') + 1;
                    memcpy(tmp, s, strlen(s) - 2);
                    error_code = atoi(tmp);
                    if (error_code != 0) {
                        switch (error_code) {
                            case 1:
                                Pthread_mutex_unlock(&self->_.d_state.mtx);
                                return AAOS_EBADCMD;
                                break;
                            case 2:
                                Pthread_mutex_unlock(&self->_.d_state.mtx);
                                return AAOS_EINVAL;
                                break;
                            case 3:
                            case 4:
                                Pthread_mutex_unlock(&self->_.d_state.mtx);
                                return AAOS_EINTR;
                                break;
                            default:
                                Pthread_mutex_unlock(&self->_.d_state.mtx);
                                return AAOS_ERROR;
                                break;
                        }
                    }
                    self->_.d_param.exposure_time = exposure_time;
                    if (exposure_time > 1. / self->_.d_param.frame_rate) {
                        self->_.d_param.frame_rate = 1. / exposure_time;
                    }
                    break;
                default:
                    break;
            }
            break;
        case DETECTOR_STATE_IDLE:
            if ((ret = tcp_socket_write(camera, buffer, strlen(buffer), NULL)) != AAOS_OK) {
                Pthread_mutex_unlock(&self->_.d_state.mtx);
                return ret;
            }
            if ((ret = tcp_socket_read_until(camera, buffer, BUFSIZE, NULL, "\r\n")) != AAOS_OK) {
                Pthread_mutex_unlock(&self->_.d_state.mtx);
                return ret;
            }
            s = strchr(buffer, ':') + 1;
            memcpy(tmp, s, strlen(s) - 2);
            error_code = atoi(tmp);
            if (error_code != 0) {
                switch (error_code) {
                    case 1:
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        return AAOS_EBADCMD;
                        break;
                    case 2:
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        return AAOS_EINVAL;
                        break;
                    case 3:
                    case 4:
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        return AAOS_EINTR;
                        break;
                    default:
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        return AAOS_ERROR;
                        break;
                }
            }
            self->_.d_param.exposure_time = exposure_time;
            if (exposure_time > 1. / self->_.d_param.frame_rate) {
                self->_.d_param.frame_rate = 1. / exposure_time;
            }
            break;
        default:
            break;
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    return ret;
}

#define SITIAN_DETECTOR_GAIN_LOW        0.
#define SITIAN_DETECTOR_GAIN_HIGH       1.
#define SITIAN_DETECTOR_GAIN_LOW_HIGH   2.

static int
SitianCam_set_gain(void *_self, double gain)
{
    struct SitianCam *self = cast(SitianCam(), _self);
    
    int ret = AAOS_OK;
    char buffer[BUFSIZE];
    void *camera = self->camera;
    char tmp[256];
    int error_code;
    const char *s;
    
    memset(tmp, '\0', 256);
    if (gain != SITIAN_DETECTOR_GAIN_LOW || gain != SITIAN_DETECTOR_GAIN_HIGH || gain != SITIAN_DETECTOR_GAIN_LOW_HIGH) {
        return AAOS_EINVAL;
    }
    snprintf(buffer, BUFSIZE, "MSG=HDR:%d\r\n", (int) gain);
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    switch (self->_.d_state.state) {
        case DETECTOR_STATE_MALFUNCTION:
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EDEVMAL;
            break;
        case DETECTOR_STATE_UNINITIALIZED:
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EUNINIT;
            break;
        case DETECTOR_STATE_READING:
        case DETECTOR_STATE_EXPOSING:
            while (self->_.d_state.state == DETECTOR_STATE_EXPOSING || self->_.d_state.state == DETECTOR_STATE_READING) {
                Pthread_cond_wait(&self->_.d_state.cond, &self->_.d_state.mtx);
            }
            switch (self->_.d_state.state) {
                case DETECTOR_STATE_MALFUNCTION:
                    Pthread_mutex_unlock(&self->_.d_state.mtx);
                    return AAOS_EDEVMAL;
                    break;
                case DETECTOR_STATE_UNINITIALIZED:
                    Pthread_mutex_unlock(&self->_.d_state.mtx);
                    return AAOS_EUNINIT;
                    break;
                case DETECTOR_STATE_IDLE:
                    if ((ret = tcp_socket_write(camera, buffer, strlen(buffer), NULL)) != AAOS_OK) {
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        return ret;
                    }
                    if ((ret = tcp_socket_read_until(camera, buffer, BUFSIZE, NULL, "\r\n")) != AAOS_OK) {
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        return ret;
                    }
                    s = strchr(buffer, ':') + 1;
                    memcpy(tmp, s, strlen(s) - 2);
                    error_code = atoi(tmp);
                    if (error_code != 0) {
                        switch (error_code) {
                            case 1:
                                Pthread_mutex_unlock(&self->_.d_state.mtx);
                                return AAOS_EBADCMD;
                                break;
                            case 2:
                                Pthread_mutex_unlock(&self->_.d_state.mtx);
                                return AAOS_EINVAL;
                                break;
                            case 3:
                            case 4:
                                Pthread_mutex_unlock(&self->_.d_state.mtx);
                                return AAOS_EINTR;
                                break;
                            default:
                                Pthread_mutex_unlock(&self->_.d_state.mtx);
                                return AAOS_ERROR;
                                break;
                        }
                    }
                    self->_.d_param.gain = gain;
                    break;
                default:
                    break;
            }
            break;
        case DETECTOR_STATE_IDLE:
            if ((ret = tcp_socket_write(camera, buffer, strlen(buffer), NULL)) != AAOS_OK) {
                Pthread_mutex_unlock(&self->_.d_state.mtx);
                return ret;
            }
            if ((ret = tcp_socket_read_until(camera, buffer, BUFSIZE, NULL, "\r\n")) != AAOS_OK) {
                Pthread_mutex_unlock(&self->_.d_state.mtx);
                return ret;
            }
            s = strchr(buffer, ':') + 1;
            memcpy(tmp, s, strlen(s) - 2);
            error_code = atoi(tmp);
            if (error_code != 0) {
                switch (error_code) {
                    case 1:
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        return AAOS_EBADCMD;
                        break;
                    case 2:
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        return AAOS_EINVAL;
                        break;
                    case 3:
                    case 4:
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        return AAOS_EINTR;
                        break;
                    default:
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        return AAOS_ERROR;
                        break;
                }
            }
            self->_.d_param.gain = gain;
            break;
        default:
            break;
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    return ret;
}

static int
SitianCam_set_temperature(void *_self, double temperature)
{
    struct SitianCam *self = cast(SitianCam(), _self);
    
    int ret = AAOS_OK;
    char buffer[BUFSIZE];
    void *camera = self->camera;
    char tmp[256];
    int error_code;
    const char *s;
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    switch (self->_.d_state.state) {
        case DETECTOR_STATE_MALFUNCTION:
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EDEVMAL;
            break;
        case DETECTOR_STATE_UNINITIALIZED:
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EUNINIT;
            break;
        default:
            break;
    }
    self->_.d_param.temperature = temperature;
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    snprintf(buffer, BUFSIZE, "MSG=SetTemp:%.2f\r\n", temperature);
    
    if ((ret = tcp_socket_write(camera, buffer, strlen(buffer), NULL)) != AAOS_OK) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        return ret;
    }
    if ((ret = tcp_socket_read_until(camera, buffer, BUFSIZE, NULL, "\r\n")) != AAOS_OK) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        return ret;
    }
    
    s = strchr(buffer, ':') + 1;
    memset(tmp, '\0', 256);
    memcpy(tmp, s, strlen(s) - 2);
    error_code = atoi(tmp);
    if (error_code != 0) {
        switch (error_code) {
            case 1:
                return AAOS_EBADCMD;
                break;
            case 2:
                return AAOS_EINVAL;
                break;
            default:
                return AAOS_ERROR;
                break;
        }
    }
    
    return ret;
}

static int
SitianCam_get_temperature(void *_self, double *temperature)
{
    struct SitianCam *self = cast(SitianCam(), _self);
    
    int ret = AAOS_OK;
    char buffer[BUFSIZE];
    void *camera = self->camera;
    char tmp[256];
    int error_code;
    const char *s, *s2;
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    switch (self->_.d_state.state) {
        case DETECTOR_STATE_MALFUNCTION:
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EDEVMAL;
            break;
        case DETECTOR_STATE_UNINITIALIZED:
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EUNINIT;
            break;
        default:
            break;
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    snprintf(buffer, BUFSIZE, "MSG=GetTemp\r\n");
    
    if ((ret = tcp_socket_write(camera, buffer, strlen(buffer), NULL)) != AAOS_OK) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        return ret;
    }
    if ((ret = tcp_socket_read_until(camera, buffer, BUFSIZE, NULL, "\r\n")) != AAOS_OK) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        return ret;
    }
    
    s = strchr(buffer, ':') + 1;
    s2 = strchr(s, ':');
    memset(tmp, '\0', 256);
    memcpy(tmp, s, s2 - s - 1);
    error_code = atoi(tmp);
    if (error_code != 0) {
        switch (error_code) {
            case 1:
                return AAOS_EBADCMD;
                break;
            default:
                return AAOS_ERROR;
                break;
        }
    }
    s = s2 + 1;
    s2 = strchr(s, ':');
    memset(tmp, '\0', 256);
    memcpy(tmp, s, s2 - s - 1);
    *temperature = atof(tmp);
    
    return ret;
}

static int
SitianCam_set_directory(void *_self, const char *directory)
{
    struct SitianCam *self = cast(SitianCam(), _self);
    
    int ret = AAOS_OK;
    char buffer[BUFSIZE];
    void *camera = self->camera;
    char tmp[256];
    int error_code;
    const char *s;
    
    memset(tmp, '\0', 256);
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    switch (self->_.d_state.state) {
        case DETECTOR_STATE_MALFUNCTION:
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EDEVMAL;
            break;
        case DETECTOR_STATE_UNINITIALIZED:
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EUNINIT;
            break;
        case DETECTOR_STATE_READING:
        case DETECTOR_STATE_EXPOSING:
            while (self->_.d_state.state == DETECTOR_STATE_EXPOSING || self->_.d_state.state == DETECTOR_STATE_READING) {
                Pthread_cond_wait(&self->_.d_state.cond, &self->_.d_state.mtx);
            }
            switch (self->_.d_state.state) {
                case DETECTOR_STATE_MALFUNCTION:
                    Pthread_mutex_unlock(&self->_.d_state.mtx);
                    return AAOS_EDEVMAL;
                    break;
                case DETECTOR_STATE_UNINITIALIZED:
                    Pthread_mutex_unlock(&self->_.d_state.mtx);
                    return AAOS_EUNINIT;
                    break;
                case DETECTOR_STATE_IDLE:
                    if (self->_.d_proc.image_prefix != NULL) {
                        snprintf(buffer, BUFSIZE, "MSG=SetDir:%s/$%s_$fits\r\n", directory, self->_.d_proc.image_prefix);
                    } else {
                        snprintf(buffer, BUFSIZE, "MSG=SetDir:%s/test_$fits\r\n", directory);
                    }
                    if ((ret = tcp_socket_write(camera, buffer, strlen(buffer), NULL)) != AAOS_OK) {
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        return ret;
                    }
                    if ((ret = tcp_socket_read_until(camera, buffer, BUFSIZE, NULL, "\r\n")) != AAOS_OK) {
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        return ret;
                    }
                    s = strchr(buffer, ':') + 1;
                    memcpy(tmp, s, strlen(s) - 2);
                    error_code = atoi(tmp);
                    if (error_code != 0) {
                        switch (error_code) {
                            case 1:
                                Pthread_mutex_unlock(&self->_.d_state.mtx);
                                return AAOS_EBADCMD;
                                break;
                            case 2:
                                Pthread_mutex_unlock(&self->_.d_state.mtx);
                                return AAOS_EINVAL;
                                break;
                            case 3:
                            case 4:
                                Pthread_mutex_unlock(&self->_.d_state.mtx);
                                return AAOS_EINTR;
                                break;
                            default:
                                Pthread_mutex_unlock(&self->_.d_state.mtx);
                                return AAOS_ERROR;
                                break;
                        }
                    }
                    if (self->_.d_proc.image_directory != NULL) {
                        free(self->_.d_proc.image_directory);
                    }
                    self->_.d_proc.image_directory = (char *) Malloc(strlen(directory) + 1);
                    snprintf(self->_.d_proc.image_directory, strlen(directory) + 1, "%s", directory);
                    break;
                default:
                    break;
            }
            break;
        case DETECTOR_STATE_IDLE:
            if (self->_.d_proc.image_prefix != NULL) {
                snprintf(buffer, BUFSIZE, "MSG=SetDir:%s/$%s_$fits\r\n", directory, self->_.d_proc.image_prefix);
            } else {
                snprintf(buffer, BUFSIZE, "MSG=SetDir:%s/test_$fits\r\n", directory);
            }
            if ((ret = tcp_socket_write(camera, buffer, strlen(buffer), NULL)) != AAOS_OK) {
                Pthread_mutex_unlock(&self->_.d_state.mtx);
                return ret;
            }
            if ((ret = tcp_socket_read_until(camera, buffer, BUFSIZE, NULL, "\r\n")) != AAOS_OK) {
                Pthread_mutex_unlock(&self->_.d_state.mtx);
                return ret;
            }
            s = strchr(buffer, ':') + 1;
            memcpy(tmp, s, strlen(s) - 2);
            error_code = atoi(tmp);
            if (error_code != 0) {
                switch (error_code) {
                    case 1:
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        return AAOS_EBADCMD;
                        break;
                    case 2:
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        return AAOS_EINVAL;
                        break;
                    case 3:
                    case 4:
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        return AAOS_EINTR;
                        break;
                    default:
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        return AAOS_ERROR;
                        break;
                }
            }
            if (self->_.d_proc.image_directory != NULL) {
                free(self->_.d_proc.image_directory);
            }
            self->_.d_proc.image_directory = (char *) Malloc(strlen(directory) + 1);
            snprintf(self->_.d_proc.image_directory, strlen(directory) + 1, "%s", directory);
            break;
        default:
            break;
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    return ret;
}

static int
SitianCam_set_prefix(void *_self, const char *prefix)
{
    struct SitianCam *self = cast(SitianCam(), _self);
    
    int ret = AAOS_OK;
    char buffer[BUFSIZE];
    void *camera = self->camera;
    char tmp[256];
    int error_code;
    const char *s;
    
    memset(tmp, '\0', 256);
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    switch (self->_.d_state.state) {
        case DETECTOR_STATE_MALFUNCTION:
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EDEVMAL;
            break;
        case DETECTOR_STATE_UNINITIALIZED:
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EUNINIT;
            break;
        case DETECTOR_STATE_READING:
        case DETECTOR_STATE_EXPOSING:
            while (self->_.d_state.state == DETECTOR_STATE_EXPOSING || self->_.d_state.state == DETECTOR_STATE_READING) {
                Pthread_cond_wait(&self->_.d_state.cond, &self->_.d_state.mtx);
            }
            switch (self->_.d_state.state) {
                case DETECTOR_STATE_MALFUNCTION:
                    Pthread_mutex_unlock(&self->_.d_state.mtx);
                    return AAOS_EDEVMAL;
                    break;
                case DETECTOR_STATE_UNINITIALIZED:
                    Pthread_mutex_unlock(&self->_.d_state.mtx);
                    return AAOS_EUNINIT;
                    break;
                case DETECTOR_STATE_IDLE:
                    if (self->_.d_proc.image_directory != NULL) {
                        snprintf(buffer, BUFSIZE, "MSG=SetDir:%s/$%s_$fits\r\n", self->_.d_proc.image_directory, prefix);
                    } else {
                        snprintf(buffer, BUFSIZE, "MSG=SetDir:./$%s_$fits\r\n", prefix);
                    }
                    if ((ret = tcp_socket_write(camera, buffer, strlen(buffer), NULL)) != AAOS_OK) {
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        return ret;
                    }
                    if ((ret = tcp_socket_read_until(camera, buffer, BUFSIZE, NULL, "\r\n")) != AAOS_OK) {
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        return ret;
                    }
                    s = strchr(buffer, ':') + 1;
                    memcpy(tmp, s, strlen(s) - 2);
                    error_code = atoi(tmp);
                    if (error_code != 0) {
                        switch (error_code) {
                            case 1:
                                Pthread_mutex_unlock(&self->_.d_state.mtx);
                                return AAOS_EBADCMD;
                                break;
                            case 2:
                                Pthread_mutex_unlock(&self->_.d_state.mtx);
                                return AAOS_EINVAL;
                                break;
                            case 3:
                            case 4:
                                Pthread_mutex_unlock(&self->_.d_state.mtx);
                                return AAOS_EINTR;
                                break;
                            default:
                                Pthread_mutex_unlock(&self->_.d_state.mtx);
                                return AAOS_ERROR;
                                break;
                        }
                    }
                    if (self->_.d_proc.image_prefix != NULL) {
                        free(self->_.d_proc.image_prefix);
                    }
                    self->_.d_proc.image_prefix = (char *) Malloc(strlen(prefix) + 1);
                    snprintf(self->_.d_proc.image_prefix, strlen(prefix) + 1, "%s", prefix);
                    break;
                default:
                    break;
            }
            break;
        case DETECTOR_STATE_IDLE:
            if (self->_.d_proc.image_directory != NULL) {
                snprintf(buffer, BUFSIZE, "MSG=SetDir:%s/$%s_$fits\r\n", self->_.d_proc.image_directory, prefix);
            } else {
                snprintf(buffer, BUFSIZE, "MSG=SetDir:./$%s_$fits\r\n", prefix);
            }
            if ((ret = tcp_socket_write(camera, buffer, strlen(buffer), NULL)) != AAOS_OK) {
                Pthread_mutex_unlock(&self->_.d_state.mtx);
                return ret;
            }
            if ((ret = tcp_socket_read_until(camera, buffer, BUFSIZE, NULL, "\r\n")) != AAOS_OK) {
                Pthread_mutex_unlock(&self->_.d_state.mtx);
                return ret;
            }
            s = strchr(buffer, ':') + 1;
            memcpy(tmp, s, strlen(s) - 2);
            error_code = atoi(tmp);
            if (error_code != 0) {
                switch (error_code) {
                    case 1:
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        return AAOS_EBADCMD;
                        break;
                    case 2:
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        return AAOS_EINVAL;
                        break;
                    case 3:
                    case 4:
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        return AAOS_EINTR;
                        break;
                    default:
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        return AAOS_ERROR;
                        break;
                }
            }
            if (self->_.d_proc.image_prefix != NULL) {
                free(self->_.d_proc.image_prefix);
            }
            self->_.d_proc.image_prefix = (char *) Malloc(strlen(prefix) + 1);
            snprintf(self->_.d_proc.image_prefix, strlen(prefix) + 1, "%s", prefix);
            break;
        default:
            break;
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    return ret;
}

static int
SitianCam_expose(void *_self, double exposure_time, uint32_t n_frames, va_list *app)
{
    struct SitianCam *self = cast(SitianCam(), _self);
    
    int ret = AAOS_OK;
    char buffer[BUFSIZE];
    double frame_rate, frame_length;
    uint32_t i;
    void *camera = self->camera;
    size_t size;
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    switch (self->_.d_state.state) {
        case DETECTOR_STATE_MALFUNCTION:
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EDEVMAL;
            break;
        case DETECTOR_STATE_READING:
        case DETECTOR_STATE_EXPOSING:
            while (self->_.d_state.state != DETECTOR_STATE_IDLE) {
                Pthread_cond_wait(&self->_.d_state.cond, &self->_.d_state.mtx);
            }
            switch (self->_.d_state.state) {
                case DETECTOR_STATE_IDLE:
                    self->_.d_state.state = DETECTOR_STATE_EXPOSING;
                    break;
                case DETECTOR_STATE_MALFUNCTION:
                    Pthread_mutex_unlock(&self->_.d_state.mtx);
                    return AAOS_EDEVMAL;
                    break;
                case DETECTOR_STATE_UNINITIALIZED:
                    Pthread_mutex_unlock(&self->_.d_state.mtx);
                    return AAOS_EUNINIT;
                    break;
                default:
                    break;
            }
            break;
        case DETECTOR_STATE_UNINITIALIZED:
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EUNINIT;
            break;
        case DETECTOR_STATE_IDLE:
            self->_.d_state.state = DETECTOR_STATE_EXPOSING;
            break;
        default:
            break;
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);

    /*
    if ((ret = __detector_get_frame_rate(self, &frame_rate)) != AAOS_OK) {
        return ret;
    }
    */
    frame_rate = self->_.d_param.frame_rate;
    frame_length = 1. / frame_rate;
    if (frame_length < exposure_time) {
        /*
        if ((ret = __detector_set_frame_rate(self, 1. / exposure_time)) != AAOS_OK) {
            return ret;
        }
        frame_length = 1. / exposure_time;
         */
        frame_rate = 1. / exposure_time;
        frame_length = exposure_time;
    }
    
    snprintf(buffer, BUFSIZE, "MSG=MulStart:%.2f:%.2f:%ud\r\n", exposure_time, frame_length - exposure_time, n_frames);
    if ((ret = tcp_socket_write(camera, buffer, strlen(buffer), NULL)) != AAOS_OK) {
        return ret;
    }
    
    for (i = 0; i < n_frames; i++) {
        if ((ret = tcp_socket_read_until(camera, buffer, BUFSIZE, &size, "\r\n")) != AAOS_OK) {
            return ret;
        }
        const char *s = buffer, *s2;
        char tmp[FILENAMESIZE];
        unsigned int frame_no;
        int error_code;
        s = strchr(buffer, ':') + 1;
        s2 = strchr(s, ':');
        memset(tmp, '\0', FILENAMESIZE);
        memcpy(tmp, s, s2 - s - 1);
        error_code = atoi(tmp);
        if (error_code != 0) {
            switch (error_code) {
                case 1:
                    Pthread_mutex_lock(&self->_.d_state.mtx);
                    self->_.d_state.state = DETECTOR_STATE_IDLE;
                    Pthread_mutex_unlock(&self->_.d_state.mtx);
                    Pthread_cond_broadcast(&self->_.d_state.cond);
                    return AAOS_EBADCMD;
                    break;
                case 2:
                    Pthread_mutex_lock(&self->_.d_state.mtx);
                    self->_.d_state.state = DETECTOR_STATE_IDLE;
                    Pthread_mutex_unlock(&self->_.d_state.mtx);
                    Pthread_cond_broadcast(&self->_.d_state.cond);
                    return AAOS_EINVAL;
                    break;
                case 3:
                case 4:
                    Pthread_mutex_lock(&self->_.d_state.mtx);
                    self->_.d_state.state = DETECTOR_STATE_IDLE;
                    Pthread_mutex_unlock(&self->_.d_state.mtx);
                    Pthread_cond_broadcast(&self->_.d_state.cond);
                    return AAOS_EINTR;
                    break;
                default:
                    Pthread_mutex_lock(&self->_.d_state.mtx);
                    self->_.d_state.state = DETECTOR_STATE_IDLE;
                    Pthread_mutex_unlock(&self->_.d_state.mtx);
                    Pthread_cond_broadcast(&self->_.d_state.cond);
                    return AAOS_ERROR;
                    break;
            }
        }
        s = s2 + 1;
        s2 = strchr(s, ':');
        memset(tmp, '\0', FILENAMESIZE);
        memcpy(tmp, s, s2 - s - 1);
        frame_no = atoi(tmp);
        s = s2 + 1;
        memcpy(tmp, s, strlen(s) - 2);
        if (self->_.d_proc.post_acquisition != NULL) {
            self->_.d_proc.post_acquisition(self, tmp, app);
        } else {
            printf("%s\n", tmp);
        }
    }
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    self->_.d_state.state = DETECTOR_STATE_IDLE;
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    Pthread_cond_broadcast(&self->_.d_state.cond);
    
    return ret;
}

static const void *_sitiancam_virtual_table;

static void
sitiancam_virtual_table_destroy(void)
{

    delete((void *) _sitiancam_virtual_table);
}

static void
sitiancam_virtual_table_initialize(void)
{
    _sitiancam_virtual_table = new(__DetectorVirtualTable(),
                                   __detector_init, "init", SitianCam_init,
                                   //__detector_info, "info", GenICam_info,
                                   __detector_expose, "expose", SitianCam_expose,
                                   //__detector_set_binning, "set_binning", GenICam_set_binning,
                                   //__detector_get_binning, "get_binning", GenICam_get_binning,
                                   __detector_set_exposure_time, "set_exposure_time", SitianCam_set_exposure_time,
                                   //__detector_get_exposure_time, "get_exposure_time", GenICam_get_exposure_time,
                                   //__detector_set_frame_rate, "set_frame_rate", GenICam_set_frame_rate,
                                   //__detector_get_frame_rate, "get_frame_rate", GenICam_get_frame_rate,
                                   __detector_set_gain, "set_gain", SitianCam_set_gain,
                                   //__detector_get_gain, "get_gain", GenICam_get_gain,
                                   //__detector_set_region, "set_region", GenICam_set_region,
                                   //__detector_get_region, "get_region", GenICam_get_region,
                                   __detector_raw, "raw", SitianCam_raw,
                                   //__detector_inspect, "inspect", GenICam_inspect,
                                   //__detector_wait_for_completion, "wait_for_completion", GenICam_wait_for_last_completion,
                                   (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(sitiancam_virtual_table_destroy);
#endif
}

static const void *
sitiancam_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, sitiancam_virtual_table_initialize);
#endif
    
    return _sitiancam_virtual_table;
}

/*
 *
 *
 */

/*
 * GenICam
 * use libaravis
 * Tested camera:
 * ImperX, inc.
 */

#ifdef __USE_ARAVIS__

#include <aravis-0.8/arv.h>
#include <glib-2.0/glib.h>

static void
tp2str(struct timespec *tp, char *date_time, size_t size)
{
    time_t tloc = tp->tv_sec;
    int nloc = tp->tv_nsec / 1000000;
    struct tm res;
    size_t left;
    gmtime_r(&tloc, &res);
    strftime(date_time, size, "%Y-%m-%dT%H:%M:%S", &res);
    left = size - strlen(date_time);
    if (left > 1) {
    snprintf(date_time + strlen(date_time), left, ".%03d", nloc);
    }
}

static const void *genicam_virtual_table(void);

static void *
GenICam_ctor(void *_self, va_list *app)
{
    struct GenICam *self = super_ctor(GenICam(), _self, app);
    
    self->_.d_state.state = DETECTOR_STATE_UNINITIALIZED;
    const char *name = va_arg(*app, const char *);
    
    self->name = (char *) Malloc(strlen(name) + 1);
    snprintf(self->name, strlen(name) + 1, "%s", name);
    
    self->_._vtab= genicam_virtual_table();
    
    return (void *) self;
}

static void *
GenICam_dtor(void *_self)
{
    struct GenICam *self = cast(GenICam(), _self);
    
    free(self->name);
    
    if (ARV_IS_CAMERA(self->camera)) {
        g_object_unref(self->camera);
    }
    
    if (ARV_IS_STREAM(self->stream)) {
        g_object_unref(self->stream);
    }
    
    return super_dtor(GenICam(), _self);
}

static void *
GenICamClass_ctor(void *_self, va_list *app)
{
    struct GenICamClass *self = super_ctor(GenICamClass(), _self, app);
    
    self->_.raw.method = (Method) 0;
    self->_.init.method = (Method) 0;
    self->_.status.method = (Method) 0;
    
    self->_.get_binning.method = (Method) 0;
    self->_.set_binning.method = (Method) 0;
    self->_.set_exposure_time.method = (Method) 0;
    self->_.get_exposure_time.method = (Method) 0;
    self->_.set_frame_rate.method = (Method) 0;
    self->_.get_frame_rate.method = (Method) 0;
    self->_.set_gain.method = (Method) 0;
    self->_.get_gain.method = (Method) 0;
    self->_.set_region.method = (Method) 0;
    self->_.get_region.method = (Method) 0;
    self->_.inspect.method = (Method) 0;
    //self->_.wait.method = (Method) 0;
    self->_.wait_for_completion.method = (Method) 0;
    self->_.raw.method = (Method) 0;
    self->_.expose.method = (Method) 0;
    self->_.status.method = (Method) 0;
    self->_.info.method = (Method) 0;
    self->_.stop.method = (Method) 0;
    self->_.abort.method = (Method) 0;
    
    return self;
}

static const void *_GenICamClass;

static void
GenICamClass_destroy(void)
{
    free((void *) _GenICamClass);
}

static void
GenICamClass_initialize(void)
{
    _GenICamClass = new(__DetectorClass(), "GenICamClass", __DetectorClass(), sizeof(struct GenICamClass),
                          ctor, "", GenICamClass_ctor,
                          (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(GenICamClass_destroy);
#endif
}

const void *
GenICamClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, GenICamClass_initialize);
#endif
    
    return _GenICamClass;
}

static const void *_GenICam;

static void
GenICam_destroy(void)
{
    free((void *)_GenICam);
}

static void
GenICam_initialize(void)
{
    _GenICam = new(GenICamClass(), "GenICam", __Detector(), sizeof(struct GenICam),
                     ctor, "ctor", GenICam_ctor,
                     dtor, "dtor", GenICam_dtor,
                     (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(GenICam_destroy);
#endif
}

const void *
GenICam(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, GenICam_initialize);
#endif

    return _GenICam;
}

static int
GenICam_init(void *_self)
{
    struct GenICam *self = cast(GenICam(), _self);

    int ret = AAOS_OK;
    Pthread_mutex_lock(&self->_.d_state.mtx);
    if (self->_.d_state.state != DETECTOR_STATE_UNINITIALIZED) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        goto error;
    } else {
        ArvCamera *camera = NULL;
        GError *error = NULL;
        camera = arv_camera_new(self->name, &error);
        if (error != NULL) {
            g_clear_error(&error);
            ret = AAOS_ERROR;
        } else {
            self->camera = (void *) camera;
            self->_.d_state.state = DETECTOR_STATE_IDLE;
            
            if (arv_camera_is_binning_available(camera, NULL)) {
                gint min_ = 0, max_ = 0, x_binning = 0, y_binning  = 0;
                self->_.d_cap.binning_available = true;
                arv_camera_get_x_binning_bounds(camera, &min_, &max_, NULL);
                self->_.d_cap.x_binning_max = max_;
                self->_.d_cap.x_binning_min = min_;
                //increment = arv_camera_get_x_binning_increment(camera, NULL);
                arv_camera_get_y_binning_bounds(camera, &min_, &max_, NULL);
                self->_.d_cap.y_binning_max = max_;
                self->_.d_cap.y_binning_min = min_;
                //increment = arv_camera_get_y_binning_increment(camera, NULL);
                arv_camera_get_binning(camera, &x_binning, &y_binning, NULL);
                self->_.d_param.x_binning = x_binning;
                self->_.d_param.y_binning = y_binning;
            }
            
            {
                self->_.d_cap.offset_available = true;
                gint x, y;
                gint width, height;
                gint min_, max_;
                arv_camera_get_x_offset_bounds(camera, &min_, &max_, NULL);
                self->_.d_cap.x_offset_max = max_;
                self->_.d_cap.x_offset_min = min_;
                //increment = arv_camera_get_x_offset_increment(camera, NULL);
                arv_camera_get_y_offset_bounds(camera, &min_, &max_, NULL);
                self->_.d_cap.y_offset_max = max_;
                self->_.d_cap.y_offset_min = min_;
                //increment = arv_camera_get_y_offset_increment(camera, NULL);
                arv_camera_get_width_bounds(camera, &min_, &max_, NULL);
                self->_.d_cap.image_width_max = max_;
                self->_.d_cap.image_width_min = min_;
                //increment = arv_camera_get_width_increment(camera, NULL);
                arv_camera_get_height_bounds(camera, &min_, &max_, NULL);
                self->_.d_cap.image_height_max = max_;
                self->_.d_cap.image_height_min = min_;
                //increment = arv_camera_get_height_increment(camera, NULL);
                arv_camera_get_region(camera, &x, &y, &width, &height, NULL);
                self->_.d_param.image_width = width;
                self->_.d_param.image_height = height;
                self->_.d_param.x_offset = x;
                self->_.d_param.y_offset = y;
            }
            
            if (arv_camera_is_frame_rate_available(camera, NULL)) {
                self->_.d_cap.frame_rate_available = true;
                double min_ = 0., max_ = 0., frame_rate = 0.;
                arv_camera_get_frame_rate_bounds(camera, &min_, &max_, NULL);
                self->_.d_cap.frame_rate_max = max_;
                self->_.d_cap.frame_rate_min = min_;
                frame_rate = arv_camera_get_frame_rate(camera, NULL);
                self->_.d_param.frame_rate = frame_rate;
            }
            
            if (arv_camera_is_exposure_time_available(camera, NULL)) {
                self->_.d_cap.exposure_time_available = true;
                double min_ = 0., max_ = 0., exposure_time = 0.;
                arv_camera_get_exposure_time_bounds(camera, &min_, &max_, NULL);
                self->_.d_cap.exposure_time_max = max_;
                self->_.d_cap.exposure_time_min = min_;
                exposure_time = arv_camera_get_exposure_time(camera, NULL);
                self->_.d_param.exposure_time = exposure_time;
            }
            
            if (arv_camera_is_gain_available(camera, NULL)) {
                self->_.d_cap.gain_available = true;
                double min_ = 0., max_ = 0., gain = 0.;
                arv_camera_get_gain_bounds(camera, &min_, &max_, NULL);
                self->_.d_cap.gain_max = max_;
                self->_.d_cap.gain_min = min_;
                gain = arv_camera_get_gain(camera, NULL);
                self->_.d_param.gain = gain;
            }
        }
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
error:
    return ret;
}

static int
GenICam_raw(void *_self, const void *write_buffer, size_t write_buffer_size, size_t *write_size, void *read_buffer, size_t read_buffer_size, size_t *read_size)
{
    struct GenICam *self = cast(GenICam(), _self);
    ArvCamera *camera = (ArvCamera *) self->camera;
    int ret = AAOS_OK;
    
    GError *error = NULL;
    char cmd[16], type[32], feature[64];
    const char *s;
    gboolean available;
    
    s = (const char *) write_buffer;
    sscanf(s, "%s %s", cmd, feature);
    available = arv_camera_is_feature_available(camera, feature, &error);
    if (error == NULL) {
        if (!available) {
            ret = AAOS_EBADCMD;
            goto error;
        }
    } else {
        ret = AAOS_ERROR;
        goto error;
    }

    s = strstr(write_buffer, feature) + strlen(feature) + 1;
    if (strcmp(cmd, "get") == 0) {
        sscanf(s, "%s", type);
        if (strcmp(type, "int") == 0 || strcmp(type, "integer") == 0) {
            gint64 value = arv_camera_get_integer(camera, feature, &error);
            if (error == NULL) {
                snprintf(read_buffer, read_buffer_size, "%s: %ld", feature, value);
                if (read_size != NULL) {
                    *read_size = strlen(read_buffer) + 1;
                }
            } else {
                ret = AAOS_ERROR;
                goto error;
            }
        } else if (strcmp(type, "bool") == 0 || strcmp(type, "boolean") == 0) {
            gboolean value = arv_camera_get_boolean(camera, feature, &error);
            if (error == NULL) {
                if (value) {
                    
                    snprintf(read_buffer, read_buffer_size, "%s: TRUE\n", feature);
                } else {
                    snprintf(read_buffer, read_buffer_size, "%s: FALSE\n", feature);
                }
                if (read_size != NULL) {
                    *read_size = strlen(read_buffer) + 1;
                }
            } else {
                ret = AAOS_ERROR;
                goto error;
            }
        } else if (strcmp(type, "float") == 0 || strcmp(type, "double") == 0) {
            double value = arv_camera_get_float(camera, feature, &error);
            if (error == NULL) {
                snprintf(read_buffer, read_buffer_size, "%s: %f\n", feature, value);
                if (read_size != NULL) {
                    *read_size = strlen(read_buffer) + 1;
                }
            } else {
                ret = AAOS_ERROR;
                goto error;
            }
        } else if (strcmp(type, "enum") == 0 || strcmp(type, "enumeration") == 0) {
            guint n_values;
            gint64 *values = arv_camera_dup_available_enumerations(camera, feature, &n_values, &error);
            if (error == NULL) {
                guint i;
                char *s = read_buffer;
                ssize_t nleft = read_buffer_size;

                snprintf(s, nleft, "%s: [%ld",  feature, values[0]);
                nleft -= strlen(s);
                s += strlen(s);
                if (nleft > 0) {
                    for (i = 1; i < n_values; i++) {
                        snprintf(s, nleft, ", %ld", values[i]);
                        nleft -= strlen(s);
                        s += strlen(s);
                        if (nleft < 0) {
                            break;
                        }
                    }
                }
                if (nleft > 0) {
                    snprintf(s, nleft, "]\n");
                }
                if (read_size != NULL) {
                    *read_size = strlen(read_buffer) + 1;
                }
            } else {
                ret = AAOS_ERROR;
                goto error;
            }
    } else if (strcmp(type, "enum_as_str") == 0 || strcmp(type, "enumeration_as_string") == 0) {
            guint n_values;
            const char **values = arv_camera_dup_available_enumerations_as_strings(camera, feature, &n_values, &error);
            if (error == NULL) {
                guint i;
                gint left = read_buffer_size;
                char *idx = (char *) read_buffer;
                if (read_size != NULL) {
                    *read_size = 0;
                }
                for (i = 0; i < n_values; i++) {
                    if (left <= 0) {
                        break;
                    }
                    snprintf(idx, left, "%s", values[i]);
                    idx += strlen(idx) + 1;
                    left -= strlen(idx) + 1;
                    if (read_size != NULL) {
                        *read_size += strlen(idx) + 1;
                    }
                }
            } else {
                ret = AAOS_ERROR;
                goto error;
            }
        } else if (strcmp(type, "enum_as_name") == 0 || strcmp(type, "enumeration_as_name") == 0) {
            guint n_values;
            const char **values = arv_camera_dup_available_enumerations_as_display_names(camera, feature, &n_values, &error);
            if (error == NULL) {
                guint i;
                gint left = read_buffer_size;
                char *idx = (char *) read_buffer;
                if (read_size != NULL) {
                    *read_size = 0;
                }
                for (i = 0; i < n_values; i++) {
                    if (left <= 0) {
                        break;
                    }
                    snprintf(idx, left, "%s", values[i]);
                    idx += strlen(idx) + 1;
                    left -= strlen(idx) + 1;
                    if (read_size != NULL) {
                        *read_size += strlen(idx) + 1;
                    }
                }
            } else {
                ret = AAOS_ERROR;
                goto error;
            }
        } else if (strcmp(type, "int_incr") == 0 || strcmp(type, "integer_increment") == 0) {
            gint64 value = arv_camera_get_integer_increment(camera, feature, &error);
            if (error == NULL) {
                snprintf(read_buffer, read_buffer_size, "%s increment: %ld\n", feature, value);
                if (read_size != NULL) {
                    *read_size = strlen(read_buffer) + 1;
                }
            } else {
                ret = AAOS_ERROR;
                goto error;
            }
        } else if (strcmp(type, "float_incr") == 0 || strcmp(type, "double_incr") == 0 || strcmp(type, "float_increment") == 0 || strcmp(type, "double_increment") == 0) {
            double value = arv_camera_get_float_increment(camera, feature, &error);
            if (error == NULL) {
                snprintf(read_buffer, read_buffer_size, "%s increment: %f\n", feature, value);
                if (read_size != NULL) {
                    *read_size = strlen(read_buffer) + 1;
                }
            } else {
                ret = AAOS_ERROR;
                goto error;
            }
        } else if (strcmp(type, "float_bound") == 0 || strcmp(type, "double_bound") == 0) {
            double bounds[2];
            arv_camera_get_float_bounds(camera, feature, &bounds[0], &bounds[1], &error);
            if (error == NULL) {
                snprintf(read_buffer, read_buffer_size, "%s bounds: [%f, %f]\n", feature, bounds[0], bounds[1]);
                if (read_size != NULL) {
                    *read_size = strlen(read_buffer) + 1;
                }
            } else {
                ret = AAOS_ERROR;
                goto error;
            }
        } else if (strcmp(type, "int_bound") == 0 || strcmp(type, "integer_bound") == 0) {
            gint64 bounds[2];
            arv_camera_get_integer_bounds(camera, feature, &bounds[0], &bounds[1], &error);
            if (error == NULL) {
                snprintf(read_buffer, read_buffer_size, "%s bounds: [%ld, %ld]\n", feature, bounds[0], bounds[1]);
                if (read_size != NULL) {
                    *read_size = strlen(read_buffer) + 1;
                }
            } else {
                ret = AAOS_ERROR;
                goto error;
            }
        } else if (strcmp(type, "str") == 0 || strcmp(type, "string") == 0) {
            const char *value = arv_camera_get_string(camera, feature, &error);
            if (error == NULL) {
                snprintf(read_buffer, read_buffer_size, "%s", value);
                if (read_size != NULL) {
                    *read_size = strlen(read_buffer) + 1;
                }
            } else {
                ret = AAOS_ERROR;
                goto error;
            }
        }else {
            ret = AAOS_EBADCMD;
            goto error;
        }
    } else if (strcmp(cmd, "set") == 0) {
        sscanf(s, "%s", type);
        s = strstr(s, type) + strlen(type) + 1;
        if (strcmp(type, "bool") == 0 || strcmp(type, "boolean") == 0) {
            gboolean value;
            sscanf(s, "%d", &value);
            arv_camera_set_boolean(camera, feature, value, &error);
            if (error != NULL) {
                ret = AAOS_ERROR;
                goto error;
            }
        } else if (strcmp(type, "int") == 0 || strcmp(type, "integer") == 0) {
            gint64 value;
            sscanf(s, "%ld", &value);
            arv_camera_set_integer(camera, feature, value, &error);
            if (error != NULL) {
                ret = AAOS_ERROR;
                goto error;
            }
        } else if (strcmp(type, "double") == 0 || strcmp(type, "float") == 0) {
            double value;
            sscanf(s, "%lf", &value);
            arv_camera_set_float(camera, feature, value, &error);
            if (error != NULL) {
                ret = AAOS_ERROR;
                goto error;
            }
        } else if (strcmp(type, "str") == 0 || strcmp(type, "string") == 0) {
            while (*s == ' ' || *s == '\t')
                s++;
            arv_camera_set_string(camera, feature, s, &error);
            if (error != NULL) {
                ret = AAOS_ERROR;
                goto error;
            }
        } else {
            ret = AAOS_EBADCMD;
            goto error;
        }
        snprintf(read_buffer, read_buffer_size, "Set `%s` successfully\n", feature);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else if (strcmp(cmd, "execute") == 0) {
        arv_camera_execute_command(camera, feature, &error);
        if (error != NULL) {
            ret = AAOS_ERROR;
            goto error;
        }
        snprintf(read_buffer, read_buffer_size, "Execute `%s` successfully\n", feature);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else {
        ret = AAOS_EBADCMD;
        goto error;
    }
error:
    if (error != NULL) {
        g_clear_error(&error);
    }
    return ret;
}

/*
 * if exposure time is negative, set camera exposure duration to auto mode.
 *
 * RETURN VALUE
 *        On success, set_exposure_time() returns AAOS_OK; on error, it
 *        returns an error number.
 * ERRORS
 *        AAOS_EINVAL exposure_time is negative, but the camera do not
 *                    support auto exposure function.
 *        AAOS_EINVAL exposure_time is out of the camera's exposure time
 *                    time range.
 */

static int
GenICam_set_exposure_time_nl(void *_self, double exposure_time)
{
    struct GenICam *self = cast(GenICam(), _self);
    ArvCamera *camera = (ArvCamera *) self->camera;
    
    double exposure_time_us = 1000000. * exposure_time;
    ArvAuto auto_mode;
    gboolean auto_available = arv_camera_is_exposure_auto_available(camera, NULL);
    GError *error = NULL;
    int ret = AAOS_OK;

    if (exposure_time_us < 0) {
        if (auto_available) {
            auto_mode = arv_camera_get_exposure_time_auto(camera, &error);
            if (error != NULL) {
                ret = AAOS_ERROR;
                goto error;
            }
            if (auto_mode == ARV_AUTO_OFF) {
                arv_camera_set_exposure_time_auto(camera, ARV_AUTO_ONCE, &error);
                if (error != NULL) {
                    ret = AAOS_ERROR;
                    goto error;
                }
            }
        } else {
            ret = AAOS_ERROR;
            goto error;
        }
    } else {
        if (auto_available) {
            auto_mode = arv_camera_get_exposure_time_auto(camera, &error);
            if (error != NULL) {
                ret = AAOS_ERROR;
                goto error;
            }
            if (auto_mode != ARV_AUTO_OFF) {
                arv_camera_set_exposure_time_auto(camera, ARV_AUTO_ONCE, &error);
                if (error != NULL) {
                    ret = AAOS_ERROR;
                    goto error;
                }
            }
        }
        double min, max;
        arv_camera_get_exposure_time_bounds(camera, &min, &max, &error);
        if (error != NULL) {
            ret = AAOS_ERROR;
            goto error;
        }
        if (exposure_time_us > max || exposure_time_us < min) {
            double min, max;
            arv_camera_get_frame_rate_bounds(camera, &min, &max, &error);
            if (error != NULL) {
                ret = AAOS_ERROR;
                goto error;
            }
            if (exposure_time_us > 1000000. / min || exposure_time_us < 1000000. / max) {
                ret = AAOS_EINVAL;
                goto error;
            } else {
                arv_camera_set_frame_rate(camera, 1000000. / (exposure_time_us + 10), &error);
                if (error != NULL) {
                    ret = AAOS_ERROR;
                    goto error;
                }
                self->_.d_param.frame_rate = 1000000. / (exposure_time_us + 10);
            }
        }
        double current_exposure_time;
        current_exposure_time= arv_camera_get_exposure_time(camera, &error);
        if (error != NULL) {
            ret = AAOS_ERROR;
            goto error;
        }
        if (fabs(current_exposure_time -  exposure_time_us) > 1.) {
            Nanosleep(current_exposure_time / 1000000.);
            arv_camera_set_exposure_time(camera, exposure_time_us, &error);
            if (error != NULL) {
                ret = AAOS_ERROR;
                goto error;
            }
            self->_.d_param.exposure_time = exposure_time;
        }
    }
error:
    if (error != NULL) {
        g_clear_error(&error);
    }
    return ret;
}

static int
GenICam_set_exposure_time(void *_self, double exposure_time)
{
    struct GenICam *self = cast(GenICam(), _self);
    ArvCamera *camera = (ArvCamera *) self->camera;
    
    double exposure_time_us = 1000000. * exposure_time;
    ArvAuto auto_mode;
    gboolean auto_available = arv_camera_is_exposure_auto_available(camera, NULL);
    GError *error = NULL;
    int ret;
    double current_exposure_time;

    if (exposure_time_us < 0) {
        if (auto_available) {
            auto_mode = arv_camera_get_exposure_time_auto(camera, &error);
            if (error != NULL) {
                ret = AAOS_ERROR;
                goto error;
            }
            if (auto_mode == ARV_AUTO_OFF) {
                arv_camera_set_exposure_time_auto(camera, ARV_AUTO_ONCE, &error);
                if (error != NULL) {
                    ret = AAOS_ERROR;
                    goto error;
                }
            }
        } else {
            ret = AAOS_ERROR;
            goto error;
        }
    } else {
        if (auto_available) {
            auto_mode = arv_camera_get_exposure_time_auto(camera, &error);
            if (error != NULL) {
                ret = AAOS_ERROR;
                goto error;
            }
            if (auto_mode != ARV_AUTO_OFF) {
                arv_camera_set_exposure_time_auto(camera, ARV_AUTO_ONCE, &error);
                if (error != NULL) {
                    ret = AAOS_ERROR;
                    goto error;
                }
            }
        }
        double min, max;
        arv_camera_get_exposure_time_bounds(camera, &min, &max, &error);
        if (error != NULL) {
            ret = AAOS_ERROR;
            goto error;
        }
        
        Pthread_mutex_lock(&self->_.d_state.mtx);
        switch (self->_.d_state.state) {
            case DETECTOR_STATE_IDLE:
                if (exposure_time_us > max || exposure_time_us < min) {
                    double min, max;
                    arv_camera_get_frame_rate_bounds(camera, &min, &max, &error);
                    if (error != NULL) {
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        ret = AAOS_ERROR;
                        goto error;
                    }
                    if (exposure_time_us > 1000000. / min || exposure_time_us < 1000000. / max) {
                        ret = AAOS_EINVAL;
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        goto error;
                    } else {
                        arv_camera_set_frame_rate(camera, 1000000. / (exposure_time_us + 10), &error);
                        if (error != NULL) {
                            ret = AAOS_ERROR;
                            Pthread_mutex_unlock(&self->_.d_state.mtx);
                            goto error;
                        }
                        self->_.d_param.frame_rate = 1000000. / (exposure_time_us + 10);
                    }
                }
                
                current_exposure_time= arv_camera_get_exposure_time(camera, &error);
                if (error != NULL) {
                    ret = AAOS_ERROR;
                    Pthread_mutex_unlock(&self->_.d_state.mtx);
                    goto error;
                }
                if (fabs(current_exposure_time -  exposure_time_us) > 1.) {
                    Nanosleep(current_exposure_time / 1000000.);
                    arv_camera_set_exposure_time(camera, exposure_time_us, &error);
                    if (error != NULL) {
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        ret = AAOS_ERROR;
                        goto error;
                    }
                    self->_.d_param.exposure_time = exposure_time;
                    Pthread_mutex_unlock(&self->_.d_state.mtx);
                }
                break;
            case DETECTOR_STATE_EXPOSING:
            case DETECTOR_STATE_READING:
                while (self->_.d_state.state == DETECTOR_STATE_READING || self->_.d_state.state == DETECTOR_STATE_EXPOSING) {
                    Pthread_cond_wait(&self->_.d_state.cond, &self->_.d_state.mtx);
                }
                if (exposure_time_us > max || exposure_time_us < min) {
                    double min, max;
                    arv_camera_get_frame_rate_bounds(camera, &min, &max, &error);
                    if (error != NULL) {
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        ret = AAOS_ERROR;
                        goto error;
                    }
                    if (exposure_time_us > 1000000. / min || exposure_time_us < 1000000. / max) {
                        ret = AAOS_EINVAL;
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        goto error;
                    } else {
                        arv_camera_set_frame_rate(camera, 1000000. / (exposure_time_us + 10), &error);
                        if (error != NULL) {
                            ret = AAOS_ERROR;
                            Pthread_mutex_unlock(&self->_.d_state.mtx);
                            goto error;
                        }
                        self->_.d_param.frame_rate = 1000000. / (exposure_time_us + 10);
                    }
                }
                
                current_exposure_time= arv_camera_get_exposure_time(camera, &error);
                if (error != NULL) {
                    ret = AAOS_ERROR;
                    Pthread_mutex_unlock(&self->_.d_state.mtx);
                    goto error;
                }
                if (fabs(current_exposure_time -  exposure_time_us) > 1.) {
                    Nanosleep(current_exposure_time / 1000000.);
                    arv_camera_set_exposure_time(camera, exposure_time_us, &error);
                    if (error != NULL) {
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        ret = AAOS_ERROR;
                        goto error;
                    }
                    self->_.d_param.exposure_time = exposure_time;
                    Pthread_mutex_unlock(&self->_.d_state.mtx);
                }
                break;
            case DETECTOR_STATE_MALFUNCTION:
                ret = AAOS_EDEVMAL;
                break;
            default:
                break;
        }
        Pthread_mutex_unlock(&self->_.d_state.mtx);
    }
error:
    
    if (error != NULL) {
        g_clear_error(&error);
    }
    return ret;
}

static void
GenICam_name_convention(void *user_data, char *filename, size_t size, ...)
{
    struct GenICam *udata = (struct GenICam *) user_data;
    
    struct tm res;
    size_t left;
    struct timespec *tp;
    va_list ap;
    
    va_start(ap, size);
    tp = va_arg(ap, struct timespec *);
    va_end(ap);
    
    time_t tloc = tp->tv_sec;
    left = size;
    
    if (udata->_.d_proc.image_directory != NULL) {
        snprintf(filename, size, "%s", udata->_.d_proc.image_directory);
        left = size - strlen(filename);
        filename += strlen(filename);
    }
    
    if (udata->_.d_proc.image_prefix != NULL) {
        snprintf(filename, size, "%s", udata->_.d_proc.image_prefix);
        left = size - strlen(filename);
        filename += strlen(filename);
    }
    
    gmtime_r(&tloc, &res);
    
    if (left > 1) {
        strftime(filename, left, "%y%m%d_%H%M%S.fits", &res);
    }
}

static void
GenICam_process_image(void *user_data, ArvBuffer *buffer, char *filename, size_t size)
{
    struct GenICam *udata = (struct GenICam *) user_data;
    
    fitsfile *fptr = (fitsfile *) udata->_.d_proc.img_fptr;
    int status = 0;
    struct timespec tp;
    
    gint x, y, width, height;
    const void *data = arv_buffer_get_data(buffer, &size);
    size_t pixel_size = ARV_PIXEL_FORMAT_BIT_PER_PIXEL(arv_buffer_get_image_pixel_format(buffer));
    long naxes[2];
    int bitpix, datatype;
    char date_time[32];
    
    arv_buffer_get_image_region(buffer, &x, &y, &width, &height);
    switch (pixel_size) {
        case 8:
            bitpix = BYTE_IMG;
            datatype = TBYTE;
            break;
        case 16:
            bitpix = USHORT_IMG;
            datatype = TUSHORT;
            break;
        case 32:
            bitpix = ULONG_IMG;
            datatype = TULONG;
            break;
        case 64:
            bitpix = ULONGLONG_IMG;
            datatype = TULONGLONG;
            break;
        default:
            break;
    }
    naxes[0] = width;
    naxes[1] = height;
    
    int flag = 0;
    if (fptr == NULL) {
        flag = 1;
        Clock_gettime(CLOCK_REALTIME, &tp);
        GenICam_name_convention(user_data, filename, size, &tp);
        if (udata->_.d_proc.tpl_fptr == NULL) {
            if (udata->_.d_proc.tpl_filename != NULL) {
                char tmp[FLEN_FILENAME];
                if (udata->_.d_proc.image_directory != NULL) {
                    snprintf(tmp, FLEN_FILENAME, "%s/%s", udata->_.d_proc.image_directory, udata->_.d_proc.tpl_filename);
                } else {
                    snprintf(tmp, FLEN_FILENAME, "%s", udata->_.d_proc.tpl_filename);
                }
                fits_create_template(&fptr, filename, tmp, &status);
            } else {
                fits_create_file(&fptr, filename, &status);
                fits_copy_file(udata->_.d_proc.tpl_fptr, fptr, 1, 1, 1, &status);
            }
        }
    }
    /*
     * write to the file
     */
    Clock_gettime(CLOCK_REALTIME, &tp);
    tp2str(&tp, date_time, 32);
    fits_create_img(fptr, bitpix, 2, naxes, &status);
    fits_write_img(fptr, datatype, 1, width * height, (void *) data, &status);
    fits_update_key_longstr(fptr, "DATE-OBS", date_time, NULL, &status);
    
    /*
     * close file properly.
     */
    if (flag) {
        if (udata->_.d_exp.notify_each_frame_done) {
            fits_close_file(fptr, &status);
            udata->_.d_proc.img_fptr = NULL;
        } else {
            udata->_.d_proc.img_fptr = fptr;
            if (udata->_.d_exp.count == udata->_.d_exp.request_frames) {
                fits_close_file(fptr, &status);
                udata->_.d_proc.img_fptr = NULL;
            }
        }
    } else {
        if (!udata->_.d_exp.notify_each_frame_done && udata->_.d_exp.count == udata->_.d_exp.request_frames) {
                fits_close_file(fptr, &status);
                udata->_.d_proc.img_fptr = NULL;
        }
    }
}

static void
GenICam_stream_callback(void *user_data, ArvStreamCallbackType type, ArvBuffer *buffer)
{
    struct GenICam *udata = (struct GenICam *) user_data;

    switch (type) {
        case ARV_STREAM_CALLBACK_TYPE_INIT:
            udata->_.d_exp.count = 0;
            break;
        case ARV_STREAM_CALLBACK_TYPE_START_BUFFER:
            udata->_.d_exp.count++;
            /*
             * Make exposure function return when last frame filling start.
             */
        
            Pthread_mutex_lock(&udata->_.d_state.mtx);
            udata->_.d_state.state = DETECTOR_STATE_READING;
            Pthread_mutex_unlock(&udata->_.d_state.mtx);
            if (udata->_.d_exp.notify_last_frame_filling) {
                Pthread_mutex_lock(&udata->_.d_exp.mtx);
                if (udata->_.d_exp.count == udata->_.d_exp.request_frames && udata->_.d_exp.notify_last_frame_filling) {
                    udata->_.d_exp.last_frame_filling_flag = 1;
                    Pthread_cond_signal(&udata->_.d_exp.cond);
                }
                Pthread_mutex_unlock(&udata->_.d_exp.mtx);
            }
            break;
        case ARV_STREAM_CALLBACK_TYPE_BUFFER_DONE:
            Pthread_mutex_lock(&udata->_.d_state.mtx);
            udata->_.d_state.state = DETECTOR_STATE_EXPOSING;
            Pthread_mutex_unlock(&udata->_.d_state.mtx);
            if (udata->_.d_exp.notify_each_frame_done) {
                if (buffer != NULL) {
                    char *filename = (char *) Malloc(FLEN_FILENAME);
                    GenICam_process_image(user_data, buffer, filename, FLEN_FILENAME);
                    threadsafe_queue_push(udata->_.d_proc.queue, filename);
                }
            } else {
                if (buffer != NULL) {
                    if (udata->_.d_exp.count != 1) {
                        GenICam_process_image(user_data, buffer, NULL, 0);
                        if (udata->_.d_exp.count == udata->_.d_exp.request_frames) {
                            threadsafe_queue_push(udata->_.d_proc.queue, udata->_.d_proc.img_filename);
                            udata->_.d_proc.img_filename = NULL;
                        }
                    } else {
                        udata->_.d_proc.img_filename = (char *) Malloc(FLEN_FILENAME);
                        GenICam_process_image(user_data, buffer, udata->_.d_proc.img_filename, FLEN_FILENAME);
                    }
                }
            }
            break;
        case ARV_STREAM_CALLBACK_TYPE_EXIT:
            break;
    }
}

static int
GenICam_expose(void *_self, double exposure_time, uint32_t n_frames, va_list *app)
{
    struct GenICam *self = cast(GenICam(), _self);
    
    ArvCamera *camera = (ArvCamera *) self->camera;
    GError *error = NULL;
    ArvStream *stream = NULL;
    gint payload;
    int old_state, ret;
    char *filename;
    
    self->_.d_exp.count = 0;
    self->_.d_exp.request_frames = n_frames;
    
    /*
     * Do not allow cancel while set exposure time, since it is a fast
     * instruction.
     */
    Pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &old_state);

    g_return_val_if_fail (ARV_IS_CAMERA (camera), EXIT_FAILURE);
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    while (self->_.d_state.state == DETECTOR_STATE_EXPOSING || self->_.d_state.state == DETECTOR_STATE_READING) {
        Pthread_cond_wait(&self->_.d_state.cond, &self->_.d_state.mtx);
    }
    self->_.d_state.state = DETECTOR_STATE_EXPOSING;
    self->_.d_exp.request_frames =  n_frames;
    self->n_buffers = n_frames;
    self->callback_param = (void *) app;
    Pthread_mutex_unlock(&self->_.d_state.mtx);

    if ((ret = GenICam_set_exposure_time_nl(self, exposure_time)) != AAOS_OK) {
        Pthread_cond_broadcast(&self->_.d_state.cond);
        goto error;
    }

    stream = arv_camera_create_stream (camera, GenICam_stream_callback, self, &error);
    if (error != NULL) {
        g_clear_error(&error);
        Pthread_mutex_lock(&self->_.d_state.mtx);
        self->_.d_state.state = DETECTOR_STATE_IDLE;
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        Pthread_cond_broadcast(&self->_.d_state.cond);
        ret = AAOS_ERROR;
        goto error;
    }
    self->stream = (void *) stream;
    
    payload = arv_camera_get_payload (camera, &error);
    if (error != NULL) {
        g_clear_error(&error);
        Pthread_mutex_lock(&self->_.d_state.mtx);
        self->_.d_state.state = DETECTOR_STATE_IDLE;
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        Pthread_cond_broadcast(&self->_.d_state.cond);
        ret = AAOS_ERROR;
        goto error;
    }
    if (n_frames == 1) {
        arv_stream_push_buffer(stream, arv_buffer_new(payload, NULL));
        arv_camera_set_acquisition_mode(camera, ARV_ACQUISITION_MODE_SINGLE_FRAME, &error);
        if (error != NULL) {
            if (error->code == ARV_GC_ERROR_ENUM_ENTRY_NOT_FOUND) {
                arv_camera_set_acquisition_mode(camera, ARV_ACQUISITION_MODE_CONTINUOUS, &error);
                if (error != NULL) {
                    g_clear_error(&error);
                    Pthread_mutex_lock(&self->_.d_state.mtx);
                    self->_.d_state.state = DETECTOR_STATE_IDLE;
                    Pthread_mutex_unlock(&self->_.d_state.mtx);
                    Pthread_cond_broadcast(&self->_.d_state.cond);
                    ret = AAOS_ERROR;
                    goto error;
                }
            } else {
                g_clear_error(&error);
                Pthread_mutex_lock(&self->_.d_state.mtx);
                self->_.d_state.state = DETECTOR_STATE_IDLE;
                Pthread_mutex_unlock(&self->_.d_state.mtx);
                Pthread_cond_broadcast(&self->_.d_state.cond);
                ret = AAOS_ERROR;
                goto error;
            }
        }
    } else {
        int i;
        for (i = 0; i < self->n_buffers; i++) {
                arv_stream_push_buffer(stream, arv_buffer_new(payload, NULL));
        }
        arv_camera_set_acquisition_mode(camera, ARV_ACQUISITION_MODE_CONTINUOUS, &error);
        if (error != NULL) {
            g_clear_error(&error);
            Pthread_mutex_lock(&self->_.d_state.mtx);
            self->_.d_state.state = DETECTOR_STATE_IDLE;
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            Pthread_cond_broadcast(&self->_.d_state.cond);
            ret = AAOS_ERROR;
            goto error;
        }
    }
    
    Pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    arv_camera_start_acquisition(camera, &error);
    if (error != NULL) {
        g_clear_error(&error);
        Pthread_mutex_lock(&self->_.d_state.mtx);
        self->_.d_state.state = DETECTOR_STATE_IDLE;
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        Pthread_cond_broadcast(&self->_.d_state.cond);
        ret = AAOS_ERROR;
        goto error;
    }
    Pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
    
    if (self->_.d_exp.notify_each_frame_done) {
        size_t i, n = n_frames - 1;
        for (i = 0; i < n; i++) {
            filename = threadsafe_queue_wait_and_pop(self->_.d_proc.queue);
            if (self->_.d_proc.post_acquisition) {
                self->_.d_proc.post_acquisition(self, filename, self->callback_param);
            } else {
                printf("%s\n", filename);
            }
            free(filename);
        }
    }
    
    /*
     * return just after last frame's exposure completes.
     */
    if (self->_.d_exp.notify_last_frame_filling) {
        Pthread_mutex_lock(&self->_.d_exp.mtx);
        while (self->_.d_exp.last_frame_filling_flag == 0) {
            Pthread_cond_wait(&self->_.d_exp.cond, &self->_.d_exp.mtx);
        }
        self->_.d_exp.last_frame_filling_flag = 0;
        Pthread_mutex_unlock(&self->_.d_exp.mtx);
        return ret;
    }
    
    filename = threadsafe_queue_wait_and_pop(self->_.d_proc.queue);
    if (self->_.d_proc.post_acquisition) {
        self->_.d_proc.post_acquisition(self, filename, self->callback_param);
    } else {
        printf("%s\n", filename);
    }
    
    free(filename);
    
    arv_camera_stop_acquisition(camera, &error);
    while ((filename = threadsafe_queue_try_pop(self->_.d_proc.queue)) != NULL) {
        Unlink(filename);
        free(filename);
    }
    
error:
    Pthread_mutex_lock(&self->_.d_state.mtx);
    self->_.d_state.state = DETECTOR_STATE_IDLE;
    if (error != NULL) {
        g_clear_error(&error);
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        ret = AAOS_ERROR;
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    Pthread_cond_broadcast(&self->_.d_state.cond);

    if (stream != NULL && ARV_IS_STREAM(stream)) {
        g_object_unref(stream);
    }
    self->stream = NULL;
    Pthread_setcancelstate(old_state, NULL);
    return ret;
}

static int
GenICam_wait_for_last_completion(void *_self)
{
    struct GenICam *self = cast(GenICam(), _self);
    
    ArvCamera *camera = (ArvCamera *) self->camera;
    ArvStream *stream = (ArvStream *) self->stream;
    int ret = AAOS_OK;
    GError *error = NULL;
    char *filename;
    
    if (self->_.d_exp.notify_last_frame_filling) {
        filename = threadsafe_queue_wait_and_pop(self->_.d_proc.queue);
        if (self->_.d_proc.post_acquisition) {
            self->_.d_proc.post_acquisition(self, filename, self->callback_param);
        } else {
            printf("%s\n", filename);
        }
        free(filename);
    }

    arv_camera_stop_acquisition(camera, &error);
    while ((filename = threadsafe_queue_try_pop(self->_.d_proc.queue)) != NULL) {
        Unlink(filename);
        free(filename);
    }
    Pthread_mutex_lock(&self->_.d_state.mtx);
    self->_.d_state.state = DETECTOR_STATE_IDLE;
    if (error != NULL) {
        g_clear_error(&error);
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        ret = AAOS_ERROR;
        goto error;
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    Pthread_cond_broadcast(&self->_.d_state.cond);
    
error:
    if (stream != NULL && ARV_IS_STREAM(stream)) {
        g_object_unref(stream);
    }
    
    return ret;
}

static int
GenICam_set_region(void *_self, uint32_t x_offset, uint32_t y_offset, uint32_t width, uint32_t height)
{
    struct GenICam *self = cast(GenICam(), _self);
    
    ArvCamera *camera = (ArvCamera *) self->camera;
    GError *error = NULL;
    int ret = AAOS_OK;
    
    if (x_offset > self->_.d_cap.x_offset_max || x_offset < self->_.d_cap.x_offset_min || y_offset > self->_.d_cap.y_offset_max || y_offset < self->_.d_cap.y_offset_min || width > self->_.d_cap.image_width_max || width < self->_.d_cap.image_width_min || height > self->_.d_cap.image_height_max || height < self->_.d_cap.image_height_min) {
        return AAOS_EINVAL;
    }
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    switch (self->_.d_state.state) {
        case DETECTOR_STATE_IDLE:
            arv_camera_set_region(camera, x_offset, y_offset, width, height, &error);
            if (error != NULL) {
                Pthread_mutex_unlock(&self->_.d_state.mtx);
                ret = AAOS_ERROR;
                goto error;
            }
            self->_.d_param.x_offset = x_offset;
            self->_.d_param.y_offset = y_offset;
            self->_.d_param.image_width = width;
            self->_.d_param.image_height = height;
            break;
        case DETECTOR_STATE_EXPOSING:
        case DETECTOR_STATE_READING:
            while (self->_.d_state.state == DETECTOR_STATE_EXPOSING || self->_.d_state.state == DETECTOR_STATE_READING) {
                Pthread_cond_wait(&self->_.d_state.cond, &self->_.d_state.mtx);
            }
            switch (self->_.d_state.state) {
                case DETECTOR_STATE_MALFUNCTION:
                    ret = AAOS_EDEVMAL;
                    break;
                case DETECTOR_STATE_OFFLINE:
                case DETECTOR_STATE_UNINITIALIZED:
                    ret = AAOS_EUNINIT;
                    break;
                default:
                    arv_camera_set_region(camera, x_offset, y_offset, width, height, &error);
                    if (error != NULL) {
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        ret = AAOS_ERROR;
                        goto error;
                    }
                    self->_.d_param.x_offset = x_offset;
                    self->_.d_param.y_offset = y_offset;
                    self->_.d_param.image_width = width;
                    self->_.d_param.image_height = height;
                    break;
            }
            break;
        case DETECTOR_STATE_MALFUNCTION:
            ret = AAOS_EDEVMAL;
            break;
        default:
            break;
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
error:
    if (error != NULL) {
        g_clear_error(&error);
    }
    return ret;
}

static int
GenICam_get_region(void *_self, uint32_t *x_offset, uint32_t *y_offset, uint32_t *width, uint32_t *height)
{
    struct GenICam *self = cast(GenICam(), _self);
    
    ArvCamera *camera = (ArvCamera *) self->camera;
    GError *error = NULL;
    int ret = AAOS_OK;
    
    gint x, y, w, h;
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    switch (self->_.d_state.state) {
        case DETECTOR_STATE_IDLE:
            arv_camera_get_region(camera, &x, &y, &w, &h, &error);
            if (error != NULL) {
                Pthread_mutex_unlock(&self->_.d_state.mtx);
                ret = AAOS_ERROR;
                goto error;
            }
            *x_offset = (uint32_t) x;
            *y_offset = (uint32_t) y;
            *width = (uint32_t) w;
            *height = (uint32_t) h;
            self->_.d_param.x_offset = *x_offset;
            self->_.d_param.y_offset = *y_offset;
            self->_.d_param.image_width = *width;
            self->_.d_param.image_height = *height;
            break;
        case DETECTOR_STATE_EXPOSING:
        case DETECTOR_STATE_READING:
            while (self->_.d_state.state == DETECTOR_STATE_EXPOSING || self->_.d_state.state == DETECTOR_STATE_READING) {
                Pthread_cond_wait(&self->_.d_state.cond, &self->_.d_state.mtx);
            }
            switch (self->_.d_state.state) {
                case DETECTOR_STATE_MALFUNCTION:
                    ret = AAOS_EDEVMAL;
                    break;
                case DETECTOR_STATE_OFFLINE:
                case DETECTOR_STATE_UNINITIALIZED:
                    ret = AAOS_EUNINIT;
                    break;
                default:
                    arv_camera_get_region(camera, &x, &y, &w, &h, &error);
                    if (error != NULL) {
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        ret = AAOS_ERROR;
                        goto error;
                    }
                    *x_offset = (uint32_t) x;
                    *y_offset = (uint32_t) y;
                    *width = (uint32_t) w;
                    *height = (uint32_t) h;
                    self->_.d_param.x_offset = *x_offset;
                    self->_.d_param.y_offset = *y_offset;
                    self->_.d_param.image_width = *width;
                    self->_.d_param.image_height = *height;
                    break;
            }
            break;
        case DETECTOR_STATE_MALFUNCTION:
            ret = AAOS_EDEVMAL;
            break;
        default:
            break;
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
error:
    if (error != NULL) {
        g_clear_error(&error);
    }
    return ret;
}

static int
GenICam_set_binning(void *_self, uint32_t x_binning, uint32_t y_binning)
{
    struct GenICam *self = cast(GenICam(), _self);
    
    ArvCamera *camera = (ArvCamera *) self->camera;
    GError *error = NULL;
    int ret = AAOS_OK;
    
    if (x_binning > self->_.d_cap.x_binning_max || x_binning < self->_.d_cap.x_binning_min || y_binning > self->_.d_cap.y_binning_max || y_binning < self->_.d_cap.y_binning_min) {
        return AAOS_EINVAL;
    }
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    switch (self->_.d_state.state) {
        case DETECTOR_STATE_IDLE:
            arv_camera_set_binning(camera, x_binning, y_binning, &error);
            if (error != NULL) {
                Pthread_mutex_unlock(&self->_.d_state.mtx);
                ret = AAOS_ERROR;
                goto error;
            }
            self->_.d_param.x_binning = x_binning;
            self->_.d_param.y_binning = y_binning;
            break;
        case DETECTOR_STATE_EXPOSING:
        case DETECTOR_STATE_READING:
            while (self->_.d_state.state == DETECTOR_STATE_EXPOSING || self->_.d_state.state == DETECTOR_STATE_READING) {
                Pthread_cond_wait(&self->_.d_state.cond, &self->_.d_state.mtx);
            }
            switch (self->_.d_state.state) {
                case DETECTOR_STATE_MALFUNCTION:
                    ret = AAOS_EDEVMAL;
                    break;
                case DETECTOR_STATE_OFFLINE:
                case DETECTOR_STATE_UNINITIALIZED:
                    ret = AAOS_EUNINIT;
                    break;
                default:
                    arv_camera_set_binning(camera, x_binning, y_binning, &error);
                    if (error != NULL) {
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        ret = AAOS_ERROR;
                        goto error;
                    }
                    self->_.d_param.x_binning = x_binning;
                    self->_.d_param.y_binning = y_binning;
                    break;
            }
            break;
        case DETECTOR_STATE_MALFUNCTION:
            ret = AAOS_EDEVMAL;
            break;
        default:
            break;
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
error:
    if (error != NULL) {
        g_clear_error(&error);
    }
    return ret;
}

static int
GenICam_get_binning(void *_self, uint32_t *x_binning, uint32_t *y_binning)
{
    struct GenICam *self = cast(GenICam(), _self);
    
    ArvCamera *camera = (ArvCamera *) self->camera;
    GError *error = NULL;
    int ret = AAOS_OK;
    
    gint x, y;
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    switch (self->_.d_state.state) {
        case DETECTOR_STATE_IDLE:
            arv_camera_get_binning(camera, &x, &y, &error);
            if (error != NULL) {
                Pthread_mutex_unlock(&self->_.d_state.mtx);
                ret = AAOS_ERROR;
                goto error;
            }
            *x_binning = (uint32_t) x;
            *y_binning = (uint32_t) y;
            self->_.d_param.x_binning = *x_binning;
            self->_.d_param.y_binning = *y_binning;
            break;
        case DETECTOR_STATE_EXPOSING:
        case DETECTOR_STATE_READING:
            while (self->_.d_state.state == DETECTOR_STATE_EXPOSING || self->_.d_state.state == DETECTOR_STATE_READING) {
                Pthread_cond_wait(&self->_.d_state.cond, &self->_.d_state.mtx);
            }
            switch (self->_.d_state.state) {
                case DETECTOR_STATE_MALFUNCTION:
                    ret = AAOS_EDEVMAL;
                    break;
                case DETECTOR_STATE_OFFLINE:
                case DETECTOR_STATE_UNINITIALIZED:
                    ret = AAOS_EUNINIT;
                    break;
                default:
                    arv_camera_get_binning(camera, &x, &y, &error);
                    if (error != NULL) {
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        ret = AAOS_ERROR;
                        goto error;
                    }
                    *x_binning = (uint32_t) x;
                    *y_binning = (uint32_t) y;
                    self->_.d_param.x_binning = *x_binning;
                    self->_.d_param.y_binning = *y_binning;
                    break;
            }
            break;
        case DETECTOR_STATE_MALFUNCTION:
            ret = AAOS_EDEVMAL;
            break;
        default:
            break;
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
error:
    if (error != NULL) {
        g_clear_error(&error);
    }
    return ret;
}

static int
GenICam_set_frame_rate(void *_self, double frame_rate)
{
    struct GenICam *self = cast(GenICam(), _self);
    
    ArvCamera *camera = (ArvCamera *) self->camera;
    GError *error = NULL;
    int ret = AAOS_OK;
    
    if (frame_rate > self->_.d_cap.frame_rate_max || frame_rate < self->_.d_cap.frame_rate_min) {
        return AAOS_EINVAL;
    }
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    switch (self->_.d_state.state) {
        case DETECTOR_STATE_IDLE:
            arv_camera_set_frame_rate(camera, frame_rate, &error);
            if (error != NULL) {
                Pthread_mutex_unlock(&self->_.d_state.mtx);
                ret = AAOS_ERROR;
                goto error;
            }
            self->_.d_param.frame_rate = frame_rate;
            break;
        case DETECTOR_STATE_EXPOSING:
        case DETECTOR_STATE_READING:
            while (self->_.d_state.state == DETECTOR_STATE_EXPOSING || self->_.d_state.state == DETECTOR_STATE_READING) {
                Pthread_cond_wait(&self->_.d_state.cond, &self->_.d_state.mtx);
            }
            switch (self->_.d_state.state) {
                case DETECTOR_STATE_MALFUNCTION:
                    ret = AAOS_EDEVMAL;
                    break;
                case DETECTOR_STATE_OFFLINE:
                case DETECTOR_STATE_UNINITIALIZED:
                    ret = AAOS_EUNINIT;
                    break;
                default:
                    arv_camera_set_frame_rate(camera, frame_rate, &error);
                    if (error != NULL) {
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        ret = AAOS_ERROR;
                        goto error;
                    }
                    self->_.d_param.frame_rate = frame_rate;
                    break;
            }
            break;
        case DETECTOR_STATE_MALFUNCTION:
            ret = AAOS_EDEVMAL;
            break;
        default:
            break;
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
error:
    if (error != NULL) {
        g_clear_error(&error);
    }
    return ret;
}

static int
GenICam_get_frame_rate(void *_self, double *frame_rate)
{
    struct GenICam *self = cast(GenICam(), _self);
    
    ArvCamera *camera = (ArvCamera *) self->camera;
    GError *error = NULL;
    int ret = AAOS_OK;
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    switch (self->_.d_state.state) {
        case DETECTOR_STATE_IDLE:
            *frame_rate = arv_camera_get_frame_rate(camera, &error);
            if (error != NULL) {
                Pthread_mutex_unlock(&self->_.d_state.mtx);
                ret = AAOS_ERROR;
                goto error;
            }
            self->_.d_param.frame_rate = *frame_rate;
            break;
        case DETECTOR_STATE_EXPOSING:
        case DETECTOR_STATE_READING:
            while (self->_.d_state.state == DETECTOR_STATE_EXPOSING || self->_.d_state.state == DETECTOR_STATE_READING) {
                Pthread_cond_wait(&self->_.d_state.cond, &self->_.d_state.mtx);
            }
            switch (self->_.d_state.state) {
                case DETECTOR_STATE_MALFUNCTION:
                    ret = AAOS_EDEVMAL;
                    break;
                case DETECTOR_STATE_OFFLINE:
                case DETECTOR_STATE_UNINITIALIZED:
                    ret = AAOS_EUNINIT;
                    break;
                default:
                    *frame_rate = arv_camera_get_frame_rate(camera, &error);
                    if (error != NULL) {
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        ret = AAOS_ERROR;
                        goto error;
                    }
                    self->_.d_param.frame_rate = *frame_rate;
                    break;
            }
            break;
        case DETECTOR_STATE_MALFUNCTION:
            ret = AAOS_EDEVMAL;
            break;
        default:
            break;
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
error:
    if (error != NULL) {
        g_clear_error(&error);
    }
    return ret;
}

static int
GenICam_set_gain(void *_self, double gain)
{
    struct GenICam *self = cast(GenICam(), _self);
    
    ArvCamera *camera = (ArvCamera *) self->camera;
    GError *error = NULL;
    int ret = AAOS_OK;
    
    if (gain > self->_.d_cap.gain_max || gain < self->_.d_cap.gain_min) {
        return AAOS_EINVAL;
    }
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    switch (self->_.d_state.state) {
        case DETECTOR_STATE_IDLE:
            arv_camera_set_gain(camera, gain, &error);
            if (error != NULL) {
                Pthread_mutex_unlock(&self->_.d_state.mtx);
                ret = AAOS_ERROR;
                goto error;
            }
            self->_.d_param.gain = gain;
            break;
        case DETECTOR_STATE_EXPOSING:
        case DETECTOR_STATE_READING:
            while (self->_.d_state.state == DETECTOR_STATE_EXPOSING || self->_.d_state.state == DETECTOR_STATE_READING) {
                Pthread_cond_wait(&self->_.d_state.cond, &self->_.d_state.mtx);
            }
            switch (self->_.d_state.state) {
                case DETECTOR_STATE_MALFUNCTION:
                    ret = AAOS_EDEVMAL;
                    break;
                case DETECTOR_STATE_OFFLINE:
                case DETECTOR_STATE_UNINITIALIZED:
                    ret = AAOS_EUNINIT;
                    break;
                default:
                    arv_camera_set_gain(camera, gain, &error);
                    if (error != NULL) {
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        ret = AAOS_ERROR;
                        goto error;
                    }
                    self->_.d_param.gain = gain;
                    break;
            }
            break;
        case DETECTOR_STATE_MALFUNCTION:
            ret = AAOS_EDEVMAL;
            break;
        default:
            break;
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
error:
    if (error != NULL) {
        g_clear_error(&error);
    }
    return ret;
}

static int
GenICam_get_gain(void *_self, double *gain)
{
    struct GenICam *self = cast(GenICam(), _self);
    
    ArvCamera *camera = (ArvCamera *) self->camera;
    GError *error = NULL;
    int ret = AAOS_OK;
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    switch (self->_.d_state.state) {
        case DETECTOR_STATE_IDLE:
            *gain = arv_camera_get_gain(camera, &error);
            if (error != NULL) {
                Pthread_mutex_unlock(&self->_.d_state.mtx);
                ret = AAOS_ERROR;
                goto error;
            }
            self->_.d_param.gain = *gain;
            break;
        case DETECTOR_STATE_EXPOSING:
        case DETECTOR_STATE_READING:
            while (self->_.d_state.state == DETECTOR_STATE_EXPOSING || self->_.d_state.state == DETECTOR_STATE_READING) {
                Pthread_cond_wait(&self->_.d_state.cond, &self->_.d_state.mtx);
            }
            switch (self->_.d_state.state) {
                case DETECTOR_STATE_MALFUNCTION:
                    ret = AAOS_EDEVMAL;
                    break;
                case DETECTOR_STATE_OFFLINE:
                case DETECTOR_STATE_UNINITIALIZED:
                    ret = AAOS_EUNINIT;
                    break;
                default:
                    *gain = arv_camera_get_gain(camera, &error);
                    if (error != NULL) {
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        ret = AAOS_ERROR;
                        goto error;
                    }
                    self->_.d_param.gain = *gain;
                    break;
            }
            break;
        case DETECTOR_STATE_MALFUNCTION:
            ret = AAOS_EDEVMAL;
            break;
        default:
            break;
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
error:
    if (error != NULL) {
        g_clear_error(&error);
    }
    return ret;
}

static int
GenICam_get_exposure_time(void *_self, double *exposure_time)
{
    struct GenICam *self = cast(GenICam(), _self);
    
    ArvCamera *camera = (ArvCamera *) self->camera;
    GError *error = NULL;
    int ret = AAOS_OK;
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    switch (self->_.d_state.state) {
        case DETECTOR_STATE_IDLE:
            *exposure_time = arv_camera_get_exposure_time(camera, &error);
            if (error != NULL) {
                Pthread_mutex_unlock(&self->_.d_state.mtx);
                ret = AAOS_ERROR;
                goto error;
            }
            self->_.d_param.exposure_time = *exposure_time;
            break;
        case DETECTOR_STATE_EXPOSING:
        case DETECTOR_STATE_READING:
            while (self->_.d_state.state == DETECTOR_STATE_EXPOSING || self->_.d_state.state == DETECTOR_STATE_READING) {
                Pthread_cond_wait(&self->_.d_state.cond, &self->_.d_state.mtx);
            }
            switch (self->_.d_state.state) {
                case DETECTOR_STATE_MALFUNCTION:
                    ret = AAOS_EDEVMAL;
                    break;
                case DETECTOR_STATE_OFFLINE:
                case DETECTOR_STATE_UNINITIALIZED:
                    ret = AAOS_EUNINIT;
                    break;
                default:
                    *exposure_time = arv_camera_get_exposure_time(camera, &error);
                    if (error != NULL) {
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        ret = AAOS_ERROR;
                        goto error;
                    }
                    self->_.d_param.exposure_time = *exposure_time;
                    break;
            }
            break;
        case DETECTOR_STATE_MALFUNCTION:
            ret = AAOS_EDEVMAL;
            break;
        default:
            break;
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
error:
    if (error != NULL) {
        g_clear_error(&error);
    }
    return ret;
}

static int
GenICam_inspect(void *_self)
{
    struct GenICam *self = cast(GenICam(), _self);
    
    ArvCamera *camera = (ArvCamera *) self->camera;
    GError *error = NULL;
    int ret = AAOS_OK;
    
    arv_camera_get_string(camera, "Vendor", &error);
    Pthread_mutex_lock(&self->_.d_state.mtx);
    if (error != NULL) {
        self->_.d_state.state = DETECTOR_STATE_MALFUNCTION;
        ret = AAOS_EDEVMAL;
        g_clear_error(&error);
    } else {
        self->_.d_state.state = DETECTOR_STATE_IDLE;
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    Pthread_cond_broadcast(&self->_.d_state.cond);
    
    return ret;
}

static int
GenICam_info(void *_self, void *res, size_t res_size, size_t *res_len)
{
    struct GenICam *self = cast(GenICam(), _self);
    
    ArvCamera *camera = (ArvCamera *) self->camera;
    GError *error = NULL;
    FILE *fp;
    int ret = AAOS_OK;
    
    fp = fmemopen(res, res_size, "w+");
    fprintf(fp, "Vendor:\t\t%s\n", arv_camera_get_vendor_name(camera, NULL));
    fprintf(fp, "Model:\t\t%s\n", arv_camera_get_model_name(camera, NULL));
    fprintf(fp, "Serial:\t\t%s\n", arv_camera_get_device_serial_number(camera, NULL));
    fprintf(fp, "DeviceID:\t%s\n", arv_camera_get_device_id(camera, NULL));
    fprintf(fp, "DeviceUserID:\t%s\n", arv_camera_get_device_id(camera, NULL));
    
    gint width, height;
    arv_camera_get_sensor_size(camera, &width, &height, NULL);
    fprintf(fp, "\nSensor format:\t%dx%d\n", width, height);

    gint x, y;
    arv_camera_get_region(camera, &x, &y, &width, &height, NULL);
    fprintf(fp, "Current region:\t[%d, %d, %d, %d]\n", x, y, width, height);
    gint min, max, increment;
    arv_camera_get_x_offset_bounds(camera, &min, &max, NULL);
    increment = arv_camera_get_x_offset_increment(camera, NULL);
    fprintf(fp, "X-Offset range:\t[%d, %d, %d]\n", min, max, increment);
    arv_camera_get_y_offset_bounds(camera, &min, &max, NULL);
    increment = arv_camera_get_y_offset_increment(camera, NULL);
    fprintf(fp, "Y-Offset range:\t[%d, %d, %d]\n", min, max, increment);
    arv_camera_get_width_bounds(camera, &min, &max, NULL);
    increment = arv_camera_get_width_increment(camera, NULL);
    fprintf(fp, "Width range:\t[%d, %d, %d]\n", min, max, increment);
    arv_camera_get_height_bounds(camera, &min, &max, NULL);
    increment = arv_camera_get_height_increment(camera, NULL);
    fprintf(fp, "Height range:\t[%d, %d, %d]\n", min, max, increment);
    
    guint n;
    const char **available_pixel_formats = arv_camera_dup_available_pixel_formats_as_display_names(camera, &n, NULL);
    if (available_pixel_formats != NULL) {
        guint i;
        fprintf(fp, "\nPixel format:\t%s\n", available_pixel_formats[0]);
        for (i = 1; i < n; i++) {
            fprintf(fp, "\t\t%s\n", available_pixel_formats[i]);
        }
        g_clear_pointer (&available_pixel_formats, g_free);
    } else {
        fprintf(fp, "\nPixel format:\tnot available\n");
    }
    fprintf(fp, "Current:\t%s\n", arv_camera_get_pixel_format_as_string(camera, NULL));

    if (arv_camera_is_frame_rate_available(camera, NULL)) {
        fprintf(fp, "\nFramerate:\tavailable\n");
        double min = 0., max = 0., frame_rate = 0.;
        arv_camera_get_frame_rate_bounds(camera, &min, &max, NULL);
        fprintf(fp, "Range:\t\t[%.3f, %.3f] Hz\n", min, max);
        frame_rate = arv_camera_get_frame_rate(camera, NULL);
        fprintf(fp, "Current:\t%.6f Hz\n", frame_rate);
    } else {
        fprintf(fp, "\nFramerate:\tnot available\n");
    }
                
    if (arv_camera_is_exposure_time_available(camera, NULL)) {
        fprintf(fp, "\nExposureTime:\tavailable\n");
        double min = 0., max = 0., exposure_time = 0.;
        arv_camera_get_exposure_time_bounds(camera, &min, &max, NULL);
        fprintf(fp, "Range:\t\t[%.0f, %.0f] us\n", min, max);
        exposure_time = arv_camera_get_exposure_time(camera, NULL);
        fprintf(fp, "Current:\t%.0f us\n", exposure_time);
    } else {
        fprintf(fp, "\nExposureTime:\tnot available\n");
    }

    if (arv_camera_is_gain_available(camera, NULL)) {
        fprintf(fp, "\nGain:\t\tavailable\n");
        double min = 0., max = 0., gain = 0.;
        arv_camera_get_gain_bounds(camera, &min, &max, NULL);
        fprintf(fp, "Range:\t\t[%.2f, %.2f] e-/ADU\n", min, max);
        gain = arv_camera_get_gain(camera, NULL);
        fprintf(fp, "Current:\t%.2f e-/ADU\n", gain);
    } else {
        fprintf(fp, "\nGain\t:\tnot available\n");
    }
    
    if (arv_camera_is_binning_available(camera, NULL)) {
        fprintf(fp, "\nBinning:\tavailable\n");
        gint min = 0, max = 0, increment = 0, x_binning = 0, y_binning  = 0;
        arv_camera_get_x_binning_bounds(camera, &min, &max, NULL);
        increment = arv_camera_get_x_binning_increment(camera, NULL);
        fprintf(fp, "Range:\t\thorizontal %d, %d, %d\n", min, max, increment);
        arv_camera_get_y_binning_bounds(camera, &min, &max, NULL);
        increment = arv_camera_get_y_binning_increment(camera, NULL);
        fprintf(fp, "Range:\t\tvertical   %d, %d, %d\n", min, max, increment);
        arv_camera_get_binning(camera, &x_binning, &y_binning, NULL);
        fprintf(fp, "Current:\t%dx%d\n", x_binning, y_binning);
    } else {
        fprintf(fp, "\nExposureTime:\tnot available\n");
    }

    fclose(fp);
    if (res_len != NULL) {
        *res_len = strlen(res) + 1;
    }
    return ret;
}

static const void *_genicam_virtual_table;

static void
genicam_virtual_table_destroy(void)
{
    delete((void *) _genicam_virtual_table);
}

static void
genicam_virtual_table_initialize(void)
{
    _genicam_virtual_table = new(__DetectorVirtualTable(),
                                 __detector_init, "init", GenICam_init,
                                 __detector_info, "info", GenICam_info,
                                 __detector_expose, "expose", GenICam_expose,
                                 __detector_set_binning, "set_binning", GenICam_set_binning,
                                 __detector_get_binning, "get_binning", GenICam_get_binning,
                                 __detector_set_exposure_time, "set_exposure_time", GenICam_set_exposure_time,
                                 __detector_get_exposure_time, "get_exposure_time", GenICam_get_exposure_time,
                                 __detector_set_frame_rate, "set_frame_rate", GenICam_set_frame_rate,
                                 __detector_get_frame_rate, "get_frame_rate", GenICam_get_frame_rate,
                                 __detector_set_gain, "set_gain", GenICam_set_gain,
                                 __detector_get_gain, "get_gain", GenICam_get_gain,
                                 __detector_set_region, "set_region", GenICam_set_region,
                                 __detector_get_region, "get_region", GenICam_get_region,
                                 __detector_raw, "raw", GenICam_raw,
                                 __detector_inspect, "inspect", GenICam_inspect,
                                 __detector_wait_for_completion, "wait_for_completion", GenICam_wait_for_last_completion,
                                 (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(genicam_virtual_table_destroy);
#endif
}

static const void *
genicam_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, genicam_virtual_table_initialize);
#endif
    
    return _genicam_virtual_table;
}
#endif

/*
 * USTC cameras.
 */

#define USTC_CCD_SUCCESS                    20001
#define USTC_CCD_SCAN_ERROR                 20002
#define USTC_CCD_DEVICE_NOT_FOUND           20003
#define USTC_CCD_FORMAT_ERROR               20004
#define USTC_CCD_DEVICE_BUSY                20005
#define USTC_CCD_ARG_ERROR                  20006
#define USTC_CCD_ACCESS_ERROR               20007
#define USTC_CCD_NOT_INIT                   20008
#define USTC_CCD_SOFTWARE_BUG               20009
#define USTC_CCD_OUTOFRANGE                 20010
#define USTC_CCD_TEMPERATURE_ERROR          20011
#define USTC_CCD_TEMPERATURE_STABILIZED     20012
#define USTC_CCD_TEMPERATURE_NOT_REACHED    20013
#define USTC_CCD_TEMPERATURE_OFF            20014
#define USTC_CCD_NO_IMAGE                   20015
#define USTC_CCD_TIMEOUT                    20016
#define USTC_CCD_ACQURING                   20017
#define USTC_CCD_IMAGE_ALREADY_TRANSFERED   20018
#define USTC_CCD_CONFIG_NOT_FOUND           20019
#define USTC_CCD_NOT_IMPLEMENTED            20020
#define USTC_CCD_IO_ERROR                   20021
#define USTC_CCD_ACCESS_ABORT               20022

typedef struct {
    uint8_t Model_Enum;
    uint8_t MB_ID;
    uint8_t Year;
    uint8_t Month;
    uint8_t Day;
    uint8_t Hour;
    uint8_t version;
    uint8_t version_aa;
    uint8_t id;
    uint8_t checksum;
} PIXELX_SERIAL;

static const void *ustc_camera_virtual_table(void);

static void *
USTCCamera_ctor(void *_self, va_list *app)
{
    struct USTCCamera *self = super_ctor(USTCCamera(), _self, app);
    
	const char *s;
	
    self->_.d_state.state = (DETECTOR_STATE_OFFLINE|DETECTOR_STATE_UNINITIALIZED);
    s = va_arg(*app, const char *);
	self->so_path = (char *) Malloc(strlen(s) + 1);
	snprintf(self->so_path, strlen(s) + 1, "%s", s);
    self->log_level = va_arg(*app, unsigned int);
    self->which = va_arg(*app, unsigned int);
    Pthread_mutex_init(&self->mtx, NULL);
    Pthread_cond_init(&self->cond, NULL);
	self->dlh = dlopen(self->so_path, RTLD_LAZY | RTLD_LOCAL);
    
    self->_._vtab= ustc_camera_virtual_table();
    
    return (void *) self;
}

static void *
USTCCamera_dtor(void *_self)
{
    struct USTCCamera *self = cast(USTCCamera(), _self);
    
    Pthread_mutex_destroy(&self->mtx);
    Pthread_cond_destroy(&self->cond);
	free(self->so_path);
	dlclose(self->dlh);
	
    return super_dtor(USTCCamera(), _self);
}

static void *
USTCCameraClass_ctor(void *_self, va_list *app)
{
    struct USTCCameraClass *self = super_ctor(USTCCameraClass(), _self, app);
    
    self->_.raw.method = (Method) 0;
    self->_.init.method = (Method) 0;
    
    self->_.get_binning.method = (Method) 0;
    self->_.set_binning.method = (Method) 0;
    self->_.set_exposure_time.method = (Method) 0;
    self->_.get_exposure_time.method = (Method) 0;
    self->_.set_frame_rate.method = (Method) 0;
    self->_.get_frame_rate.method = (Method) 0;
    self->_.set_gain.method = (Method) 0;
    self->_.get_gain.method = (Method) 0;
    self->_.set_region.method = (Method) 0;
    self->_.get_region.method = (Method) 0;
    self->_.inspect.method = (Method) 0;
    //self->_.wait.method = (Method) 0;
    self->_.wait_for_completion.method = (Method) 0;
    self->_.raw.method = (Method) 0;
    self->_.expose.method = (Method) 0;
    self->_.status.method = (Method) 0;
    self->_.info.method = (Method) 0;
    self->_.stop.method = (Method) 0;
    self->_.abort.method = (Method) 0;
    self->_.power_on.method = (Method) 0;
    self->_.power_off.method = (Method) 0;
    
    return self;
}

static const void *_USTCCameraClass;

static void
USTCCameraClass_destroy(void)
{
    free((void *) _USTCCameraClass);
}

static void
USTCCameraClass_initialize(void)
{
    _USTCCameraClass = new(__DetectorClass(), "USTCCameraClass", __DetectorClass(), sizeof(struct USTCCameraClass),
                           ctor, "", USTCCameraClass_ctor,
                           (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(USTCCameraClass_destroy);
#endif
}

const void *
USTCCameraClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, USTCCameraClass_initialize);
#endif
    
    return _USTCCameraClass;
}

static const void *_USTCCamera;

static void
USTCCamera_destroy(void)
{
    free((void *)_USTCCamera);
}

static void
USTCCamera_initialize(void)
{
    _USTCCamera = new(USTCCameraClass(), "USTCCamera", __Detector(), sizeof(struct USTCCamera),
                      ctor, "ctor", USTCCamera_ctor,
                      dtor, "dtor", USTCCamera_dtor,
                      (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(USTCCamera_destroy);
#endif
}

const void *
USTCCamera(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, USTCCamera_initialize);
#endif

    return _USTCCamera;
}

static int
ustc_error_mapping(int ustc_error)
{
	switch (ustc_error) {
        case USTC_CCD_SUCCESS:
            return AAOS_OK;
            break;
        case USTC_CCD_ARG_ERROR:
        case USTC_CCD_OUTOFRANGE:
            return AAOS_EINVAL;
            break;
        case USTC_CCD_NOT_INIT:
            return AAOS_EUNINIT;
            break;
        case USTC_CCD_ACCESS_ERROR:
            return AAOS_ECONNREFUSED;
            break;
        case USTC_CCD_TIMEOUT:
            return AAOS_ETIMEDOUT;
            break;
        case USTC_CCD_DEVICE_BUSY:
            return AAOS_EBUSY;
            break;
        case USTC_CCD_IO_ERROR:
            return AAOS_EIO;
            break;
        case USTC_CCD_NOT_IMPLEMENTED:
            return AAOS_ENOTSUP;
            break;
        case USTC_CCD_CONFIG_NOT_FOUND:
            return AAOS_ENOENT;
            break;
        case USTC_CCD_ACCESS_ABORT:
            return AAOS_EINTR;
            break;
        case USTC_CCD_DEVICE_NOT_FOUND:
            return AAOS_ENOTFOUND;
            break;
        default:
            return AAOS_ERROR;
            break;
    }
    
    return AAOS_OK;
}

static int
USTCCamera_raw(void *_self, const void *write_buffer, size_t write_buffer_size, size_t *write_size, void *read_buffer, size_t read_buffer_size, size_t *read_size)
{
	struct USTCCamera *self = cast(USTCCamera(), _self);
	
	FILE *fp = fmemopen((void *) write_buffer, write_buffer_size, "r+");
	char func_name[NAMESIZE];
	int ret = AAOS_OK;

	fscanf(fp, "%s", func_name);
	
	if (strcmp(func_name, "Initialize") == 0 ) {
        unsigned int (*Initialize) (unsigned int , unsigned int);
		unsigned int which, log_level;
		Initialize = dlsym(self->dlh, "Initialize");
        if ((ret = fscanf(fp, "%u %u", &log_level, &which)) < 2 ){
			ret = AAOS_EBADCMD;
            goto error;
        } else {
			ret = Initialize(log_level, which);
			ret = ustc_error_mapping(ret);
            if (read_size != NULL) {
                *read_size = 0;
            }
        }
	} else if (strcmp(func_name, "ShutDown") == 0 ) {
        unsigned int (*ShutDown) (void);
        ShutDown = dlsym(self->dlh, "ShutDown");
        ret = ShutDown();
        ret = ustc_error_mapping(ret);
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "CameraReset") == 0 ) {
        unsigned int (*CameraReset) (void);
        CameraReset = dlsym(self->dlh, "CameraReset");
        ret = CameraReset();
        ret = ustc_error_mapping(ret);
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "CancelWait") == 0 ) {
        unsigned int (*CancelWait) (void);
        CancelWait = dlsym(self->dlh, "CancelWait");
        ret = CancelWait();
        ret = ustc_error_mapping(ret);
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "GetExposureInterval") == 0 ) {
        double interval = -1.00;
        unsigned int (*GetExposureInterval) (double *);
        GetExposureInterval = dlsym(self->dlh, "GetExposureInterval");
        ret = GetExposureInterval(&interval);
        ret = ustc_error_mapping(ret);
        snprintf(read_buffer, read_buffer_size, "%.2f", interval);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else if (strcmp(func_name, "GetExposureTime") == 0 ) {
        double exposure_time = -1.00;
        unsigned int (*GetExposureTime) (double *);
        GetExposureTime = dlsym(self->dlh, "GetExposureTime");
        ret = GetExposureTime(&exposure_time);
        ret = ustc_error_mapping(ret);
        snprintf(read_buffer, read_buffer_size, "%.2f", exposure_time);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else if (strcmp(func_name, "GetLDCMode") == 0 ) {
        uint8_t mode = 255;
        unsigned int (*GetLDCMode) (uint8_t *);
        GetLDCMode = dlsym(self->dlh, "GetLDCMode");
        ret = GetLDCMode(&mode);
        ret = ustc_error_mapping(ret);
        snprintf(read_buffer, read_buffer_size, "%d", mode);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else if (strcmp(func_name, "GetNumReadoutSpeed") == 0 ) {
        unsigned int (*GetNumReadoutSpeed) (void);
        GetNumReadoutSpeed = dlsym(self->dlh, "GetNumReadoutSpeed");
        snprintf(read_buffer, read_buffer_size, "%d", GetNumReadoutSpeed());
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else if (strcmp(func_name, "GetTriggerMode") == 0 ) {
        uint8_t mode = 255;
        unsigned int (*GetTriggerMode) (uint8_t *);
        GetTriggerMode = dlsym(self->dlh, "GetTriggerMode");
        ret = GetTriggerMode(&mode);
        ret = ustc_error_mapping(ret);
        snprintf(read_buffer, read_buffer_size, "%d", mode);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else if (strcmp(func_name, "SetContinuousCapture") == 0 ) {
        unsigned int (*SetContinuousCapture) (uint16_t);
        unsigned int n_frame;
        SetContinuousCapture = dlsym(self->dlh, "SetContinuousCapture");
        if ((ret = fscanf(fp, "%u", &n_frame)) < 1 ){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            ret = SetContinuousCapture(n_frame);
            ret = ustc_error_mapping(ret);
            if (read_size != NULL) {
                *read_size = 0;
            }
        }
    } else if (strcmp(func_name, "SetEraseCount") == 0 ) {
        unsigned int (*SetEraseCount) (uint8_t);
        uint8_t count;
        SetEraseCount = dlsym(self->dlh, "SetEraseCount");
        if ((ret = fscanf(fp, "%c", &count)) < 1 ){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            ret = SetEraseCount(count);
            ret = ustc_error_mapping(ret);
            if (read_size != NULL) {
                *read_size = 0;
            }
        }
    } else if (strcmp(func_name, "SetExposureInterval") == 0 ) {
        unsigned int (*SetExposureInterval) (double);
		double interval;
		SetExposureInterval = dlsym(self->dlh, "SetExposureInterval");
		if ((ret = fscanf(fp, "%lf", &interval)) < 1 ){
			ret = AAOS_EBADCMD;
            goto error;
        } else {
			ret = SetExposureInterval(interval);
			ret = ustc_error_mapping(ret);
            if (read_size != NULL) {
                *read_size = 0;
            }
        }
	} else if (strcmp(func_name, "SetExposureTime") == 0 ) {
        unsigned int (*SetExposureTime) (double);
		double exposure_time;
		SetExposureTime = dlsym(self->dlh, "SetExposureTime");
		if ((ret = fscanf(fp, "%lf", &exposure_time)) < 1 ){
			ret = AAOS_EBADCMD;
            goto error;
        } else {
			ret = SetExposureTime(exposure_time);
			ret = ustc_error_mapping(ret);
            if (read_size != NULL) {
                *read_size = 0;
            }
        }
	} else if (strcmp(func_name, "SetEraseCount") == 0 ) {
        unsigned int (*SetEraseCount) (uint8_t);
        uint8_t count;
        SetEraseCount = dlsym(self->dlh, "SetEraseCount");
        if ((ret = fscanf(fp, "%c", &count)) < 1 ){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            ret = SetEraseCount(count);
            ret = ustc_error_mapping(ret);
            if (read_size != NULL) {
                *read_size = 0;
            }
        }
    } else if (strcmp(func_name, "SetPreAmpGain") == 0 ) {
        unsigned int (*SetPreAmpGain) (uint8_t);
        uint8_t gain;
        if ((ret = fscanf(fp, "%c", &gain)) < 1 ){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            if (gain > 1) {
                ret = AAOS_EINVAL;
                goto error;
            }
            SetPreAmpGain = dlsym(self->dlh, "SetPreAmpGain");
            ret = SetPreAmpGain(gain);
            ret = ustc_error_mapping(ret);
            if (read_size != NULL) {
                *read_size = 0;
            }
        }
	} else if (strcmp(func_name, "SetShutter") == 0 ) {
        unsigned int (*SetShutter) (uint8_t);
        uint8_t mode;
        if ((ret = fscanf(fp, "%c", &mode)) < 1 ){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            if (mode > 2) {
                ret = AAOS_EINVAL;
                goto error;
            }
            SetShutter = dlsym(self->dlh, "SetShutter");
            ret = SetShutter(mode);
            ret = ustc_error_mapping(ret);
            if (read_size != NULL) {
                *read_size = 0;
            }
        }
    } else if (strcmp(func_name, "SetTriggerMode") == 0 ) {
        unsigned int (*SetTriggerMode) (uint8_t);
        uint8_t mode;
        if ((ret = fscanf(fp, "%c", &mode)) < 1 ){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            if (mode > 3) {
                ret = AAOS_EINVAL;
                goto error;
            }
            SetTriggerMode = dlsym(self->dlh, "SetTriggerMode");
            ret = SetTriggerMode(mode);
            ret = ustc_error_mapping(ret);
            if (read_size != NULL) {
                *read_size = 0;
            }
        }
    } else if (strcmp(func_name, "StopExposure") == 0 ) {
        unsigned int (*StopExposure) (void);
		StopExposure = dlsym(self->dlh, "StopExposure");
		ret = StopExposure();
		ret = ustc_error_mapping(ret);
        if (read_size != NULL) {
            *read_size = 0;
        }
	} else if (strcmp(func_name, "StartExposure") == 0 ) {
        unsigned int (*StartExposure) (void);
        StartExposure = dlsym(self->dlh, "StartExposure");
        ret = StartExposure();
        ret = ustc_error_mapping(ret);
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "WaitForAcquisition") == 0 ) {
        unsigned int (*WaitForAcquisition) (void);
        WaitForAcquisition = dlsym(self->dlh, "WaitForAcquisition");
        ret = WaitForAcquisition();
        ret = ustc_error_mapping(ret);
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "WaitForAcquisitionTimeOut") == 0 ) {
        unsigned int (*WaitForAcquisitionTimeOut) (int);
        int timeout;
        if ((ret = fscanf(fp, "%d", &timeout)) < 1 ){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            if (timeout < 0) {
                ret = AAOS_EINVAL;
                goto error;
            }
            WaitForAcquisitionTimeOut = dlsym(self->dlh, "WaitForAcquisitionTimeOut");
            ret = WaitForAcquisitionTimeOut(timeout);
            ret = ustc_error_mapping(ret);
            if (read_size != NULL) {
                *read_size = 0;
            }
        }
    } else if (strcmp(func_name, "DetectorPowerOn") == 0 ) {
        unsigned int (*DetectorPowerON) (void);
        DetectorPowerON = dlsym(self->dlh, "DetectorPowerOn");
        ret = DetectorPowerON();
        ret = ustc_error_mapping(ret);
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "DetectorPowerOff") == 0 ) {
        unsigned int (*DetectorPowerOFF) (void);
        DetectorPowerOFF = dlsym(self->dlh, "DetectorPowerOff");
        ret = DetectorPowerOFF();
        ret = ustc_error_mapping(ret);
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "Controller_temperature") == 0 ) {
        unsigned int (*Controller_temperature) (double *);
        double temperature = 9999.00;
        Controller_temperature = dlsym(self->dlh, "Controller_temperature");
        ret = Controller_temperature(&temperature);
        ret = ustc_error_mapping(ret);
        snprintf(read_buffer, read_buffer_size, "%.2f", temperature);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else if (strcmp(func_name, "ControllerFan") == 0 ) {
        unsigned int (*ControllerFan) (uint8_t);
        unsigned int level;
        if ((ret = fscanf(fp, "%u", &level)) < 1 ){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            ControllerFan = dlsym(self->dlh, "ControllerFan");
            ret = ControllerFan(level);
            ret = ustc_error_mapping(ret);
            if (read_size != NULL) {
                *read_size = 0;
            }
        }
    } else if (strcmp(func_name, "ControllerHeat") == 0 ) {
        unsigned int (*ControllerHeat) (uint8_t);
        unsigned int heatpwm;
        if ((ret = fscanf(fp, "%u", &heatpwm)) < 1 ){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            ControllerHeat = dlsym(self->dlh, "ControllerHeat");
            ret = ControllerHeat(heatpwm);
            ret = ustc_error_mapping(ret);
            if (read_size != NULL) {
                *read_size = 0;
            }
        }
    } else if (strcmp(func_name, "CoolerOff") == 0 ) {
        unsigned int (*CoolerOff) (void);
        CoolerOff = dlsym(self->dlh, "CoolerOff");
        ret = CoolerOff();
        ret = ustc_error_mapping(ret);
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "CoolerOn") == 0 ) {
        unsigned int (*CoolerOn) (void);
        CoolerOn = dlsym(self->dlh, "CoolerOn");
        ret = CoolerOn();
        ret = ustc_error_mapping(ret);
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "entemp") == 0 ) {
        unsigned int (*entemp) (double *);
        double temperature = 9999.00;
        entemp = dlsym(self->dlh, "entemp");
        ret = entemp(&temperature);
        ret = ustc_error_mapping(ret);
        snprintf(read_buffer, read_buffer_size, "%.2f", temperature);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else if (strcmp(func_name, "get_power") == 0 ) {
        unsigned int (*get_power) (double *);
        double power = -1.00;
        get_power = dlsym(self->dlh, "get_power");
        ret = get_power(&power);
        ret = ustc_error_mapping(ret);
        snprintf(read_buffer, read_buffer_size, "%.2f", power);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else if (strcmp(func_name, "GetBitDepth") == 0 ) {
        unsigned int (*GetBitDepth) (int *);
        int depth = -1;
        GetBitDepth = dlsym(self->dlh, "GetBitDepth");
        ret = GetBitDepth(&depth);
        ret = ustc_error_mapping(ret);
        snprintf(read_buffer, read_buffer_size, "%d", depth);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else if (strcmp(func_name, "GetCameraReady") == 0 ) {
        unsigned int (*GetCameraReady) (uint8_t *);
        uint8_t stat_;
        GetCameraReady = dlsym(self->dlh, "GetCameraReady");
        ret = GetCameraReady(&stat_);
        ret = ustc_error_mapping(ret);
        snprintf(read_buffer, read_buffer_size, "%u", stat_);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else if (strcmp(func_name, "GetChipNum") == 0) {
        unsigned int (*GetChipNum) (int *);
        int chipnum = 1;
        GetChipNum = dlsym(self->dlh, "GetChipNum");
        ret = GetChipNum(&chipnum);
        ret = ustc_error_mapping(ret);
        snprintf(read_buffer, read_buffer_size, "%d", chipnum);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else if (strcmp(func_name, "GetCoolerStatus") == 0 ) {
        unsigned int (*GetCoolerStatus) (uint8_t *);
        uint8_t coolstat;
        GetCoolerStatus = dlsym(self->dlh, "GetCoolerStatus");
        ret = GetCoolerStatus(&coolstat);
        ret = ustc_error_mapping(ret);
        snprintf(read_buffer, read_buffer_size, "%u", coolstat);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else if (strcmp(func_name, "GetCurrentByChannel") == 0) {
        unsigned int (*GetCurrentByChannel) (int, float *);
        int chan;
        float cur = -1.00;
        if ((ret = fscanf(fp, "%d", &chan)) < 1 ){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            if (chan < 0 || chan > 3) {
                ret = AAOS_EINVAL;
                goto error;
            } else {
                GetCurrentByChannel = dlsym(self->dlh, "GetCurrentByChannel");
                ret = GetCurrentByChannel(chan, &cur);
                snprintf(read_buffer, read_buffer_size, "%.2f", cur);
                if (read_size != NULL) {
                    *read_size = strlen(read_buffer) + 1;
                }
            }
        }
    } else if (strcmp(func_name, "GetDetector") == 0 ) {
        unsigned int (*GetDetector) (int *, int *);
        int width = -1, height = -1;
        GetDetector = dlsym(self->dlh, "GetDetector");
        ret = GetDetector(&width, &height);
        ret = ustc_error_mapping(ret);
        snprintf(read_buffer, read_buffer_size, "%d %d", width, height);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else if (strcmp(func_name, "GetExposureROI") == 0 ) {
        unsigned int (*GetExposureROI) (int *, int *, int *, int *);
        int rowStartNum = -1, rowKeepNum = -1, colStartNum = -1, colKeepNum = -1;
        GetExposureROI = dlsym(self->dlh, "GetExposureROI");
        ret = GetExposureROI(&rowStartNum, &rowKeepNum, &colStartNum, &colKeepNum);
        ret = ustc_error_mapping(ret);
        snprintf(read_buffer, read_buffer_size, "%d %d %d %d", rowStartNum, rowKeepNum, colStartNum, colKeepNum);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else if (strcmp(func_name, "GetFanSpeed") == 0 ) {
        unsigned int (*GetFanSpeed) (uint8_t *);
        uint8_t speed = 255;
        GetFanSpeed = dlsym(self->dlh, "GetFanSpeed");
        ret = GetFanSpeed(&speed);
        ret = ustc_error_mapping(ret);
        snprintf(read_buffer, read_buffer_size, "%u", speed);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else if (strcmp(func_name, "GetGain") == 0 ) {
        unsigned int (*GetGain) (float *, float *);
        float high = -1.00, low = -1.00;
        GetGain = dlsym(self->dlh, "GetGain");
        ret = GetGain(&high, &low);
        ret = ustc_error_mapping(ret);
        snprintf(read_buffer, read_buffer_size, "%.2f %.2f", high, low);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else if (strcmp(func_name, "GetHeatPWM") == 0 ) {
        unsigned int (*GetHeatPWM) (uint8_t *);
        uint8_t heatpwm = 255;
        GetHeatPWM = dlsym(self->dlh, "GetHeatPWM");
        ret = GetHeatPWM(&heatpwm);
        ret = ustc_error_mapping(ret);
        snprintf(read_buffer, read_buffer_size, "%u", heatpwm);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else if (strcmp(func_name, "GetPumpCur") == 0 ) {
        unsigned int (*GetPumpCur) (double *);
        double cur = -1.00;
        GetPumpCur = dlsym(self->dlh, "GetPumpCur");
        ret = GetPumpCur(&cur);
        ret = ustc_error_mapping(ret);
        snprintf(read_buffer, read_buffer_size, "%.2f", cur);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else if (strcmp(func_name, "GetPumpMaxPower") == 0 ) {
        unsigned int (*GetPumpMaxPower) (double *);
        double power = -1.00;
        GetPumpMaxPower = dlsym(self->dlh, "GetPumpMaxPower");
        ret = GetPumpMaxPower(&power);
        ret = ustc_error_mapping(ret);
        snprintf(read_buffer, read_buffer_size, "%.2f", power);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else if (strcmp(func_name, "GetPumpTargetVol") == 0 ) {
        unsigned int (*GetPumpTargetVol) (double *);
        double voltage = -1.00;
        GetPumpTargetVol = dlsym(self->dlh, "GetPumpTargetVol");
        ret = GetPumpTargetVol(&voltage);
        ret = ustc_error_mapping(ret);
        snprintf(read_buffer, read_buffer_size, "%.2f", voltage);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else if (strcmp(func_name, "GetPumpUint") == 0 ) {
        unsigned int (*GetPumpUint) (int *);
        int unit = -1;
        GetPumpUint = dlsym(self->dlh, "GetPumpUint");
        ret = GetPumpUint(&unit);
        ret = ustc_error_mapping(ret);
        snprintf(read_buffer, read_buffer_size, "%d", unit);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else if (strcmp(func_name, "GetPumpVacuum") == 0 ) {
        unsigned int (* GetPumpVacuum) (double *);
        double vac = -1.00;
        GetPumpVacuum = dlsym(self->dlh, " GetPumpVacuum");
        ret =  GetPumpVacuum(&vac);
        ret = ustc_error_mapping(ret);
        snprintf(read_buffer, read_buffer_size, "%.2f", vac);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else if (strcmp(func_name, "GetPumpVol") == 0 ) {
        unsigned int (*GetPumpVol) (double *);
        double voltage = -1.00;
        GetPumpVol = dlsym(self->dlh, "GetPumpVol");
        ret = GetPumpVol(&voltage);
        ret = ustc_error_mapping(ret);
        snprintf(read_buffer, read_buffer_size, "%.2f", voltage);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else if (strcmp(func_name, "GetTcbTemp") == 0 ) {
        char temp_name[32];
        if ((ret = fscanf(fp, "%s", temp_name)) < 1 ){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            unsigned int (*GetTcbTemp) (const char *, float *);
            float temperature = 9999.00;
            GetTcbTemp = dlsym(self->dlh, "GetTcbTemp");
            ret = GetTcbTemp(temp_name, &temperature);
            ret = ustc_error_mapping(ret);
            snprintf(read_buffer, read_buffer_size, "%.2f", temperature);
            if (read_size != NULL) {
                *read_size = strlen(read_buffer) + 1;
            }
        }
    } else if (strcmp(func_name, "GetTemperature") == 0 ) {
        char temp_name[32];
        if ((ret = fscanf(fp, "%s", temp_name)) < 1 ){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            unsigned int (*GetTemperature) (const char *, float *);
            float temperature = 9999.00;
            GetTemperature = dlsym(self->dlh, "GetTemperature");
            ret = GetTemperature(temp_name, &temperature);
            ret = ustc_error_mapping(ret);
            snprintf(read_buffer, read_buffer_size, "%.2f", temperature);
            if (read_size != NULL) {
                *read_size = strlen(read_buffer) + 1;
            }
        }
    } else if (strcmp(func_name, "GetVacuum") == 0 ) {
        int pr;
        if ((ret = fscanf(fp, "%d", &pr)) < 1 ){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            unsigned int (*GetVacuum) (int, double *);
            double press = -1.00;
            GetVacuum = dlsym(self->dlh, "GetVacuum");
            ret = GetVacuum(pr, &press);
            ret = ustc_error_mapping(ret);
            snprintf(read_buffer, read_buffer_size, "%.2f", press);
            if (read_size != NULL) {
                *read_size = strlen(read_buffer) + 1;
            }
        }
    } else if (strcmp(func_name, "GetVideoMode") == 0 ) {
        unsigned int (*GetVideoMode) (uint16_t *);
        uint16_t mode = 65535;
        GetVideoMode = dlsym(self->dlh, "GetVideoMode");
        ret = GetVideoMode(&mode);
        ret = ustc_error_mapping(ret);
        snprintf(read_buffer, read_buffer_size, "%u", mode);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else if (strcmp(func_name, "GetVoltageByChannel") == 0) {
        unsigned int (*GetVoltageByChannel) (int, float *);
        int chan;
        float voltage = -1.00;
        if ((ret = fscanf(fp, "%d", &chan)) < 1 ){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            if (chan < 0 || chan > 3) {
                ret = AAOS_EINVAL;
                goto error;
            } else {
                GetVoltageByChannel = dlsym(self->dlh, "GetVoltageByChannel");
                ret = GetVoltageByChannel(chan, &voltage);
                snprintf(read_buffer, read_buffer_size, "%.2f", voltage);
                if (read_size != NULL) {
                    *read_size = strlen(read_buffer) + 1;
                }
            }
        }
    } else if (strcmp(func_name, "hottemp") == 0 ) {
        unsigned int (*hottemp) (double *);
        double temperature = 9999.00;
        hottemp = dlsym(self->dlh, "hottemp");
        ret = hottemp(&temperature);
        ret = ustc_error_mapping(ret);
        snprintf(read_buffer, read_buffer_size, "%.2f", temperature);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else if (strcmp(func_name, "ListCameras") == 0 ) {
        unsigned int (*ListCameras) (int *);
        int camera_num = -1;
        ListCameras = dlsym(self->dlh, "ListCameras");
        ret = ListCameras(&camera_num);
        ret = ustc_error_mapping(ret);
        snprintf(read_buffer, read_buffer_size, "%d", camera_num);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else if (strcmp(func_name, "Motor_temperature") == 0 ) {
        unsigned int (*Motor_temperature) (double *);
        double temperature = 9999.00;
        Motor_temperature = dlsym(self->dlh, "Motor_temperature");
        ret = Motor_temperature(&temperature);
        ret = ustc_error_mapping(ret);
        snprintf(read_buffer, read_buffer_size, "%.2f", temperature);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else if (strcmp(func_name, "PumpOff") == 0 ) {
        unsigned int (*PumpOff) (void);
        PumpOff = dlsym(self->dlh, "PumpOff");
        ret = PumpOff();
        ret = ustc_error_mapping(ret);
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "PumpOn") == 0 ) {
        unsigned int (*PumpOn) (void);
        PumpOn = dlsym(self->dlh, "PumpOn");
        ret = PumpOn();
        ret = ustc_error_mapping(ret);
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "PumpProtect") == 0) {
        unsigned int (*PumpProtect) (bool);
        unsigned int onoff_;
        bool onoff;
        if ((ret = fscanf(fp, "%u", &onoff_)) < 1 ){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            if (onoff_) {
                onoff = true;
            } else {
                onoff = false;
            }
            PumpProtect = dlsym(self->dlh, "PumpProtect");
            ret = PumpProtect(onoff);
            ret = ustc_error_mapping(ret);
            if (read_size != NULL) {
                *read_size = 0;
            }
        }
    } else if (strcmp(func_name, "read_cooler_status") == 0 ) {
        unsigned int (*read_cooler_status) (int *);
        int error = -1;
        read_cooler_status = dlsym(self->dlh, "read_cooler_status");
        ret = read_cooler_status(&error);
        ret = ustc_error_mapping(ret);
        snprintf(read_buffer, read_buffer_size, "%d", error);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else if (strcmp(func_name, "RetransNthImg") == 0) {
        unsigned int (*RetransNthImg) (uint16_t);
        uint16_t idx;
        if ((ret = fscanf(fp, "%hu", &idx)) < 1 ){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            RetransNthImg = dlsym(self->dlh, "RetransNthImg");
            ret = RetransNthImg(idx);
            ret = ustc_error_mapping(ret);
            if (read_size != NULL) {
                *read_size = 0;
            }
        }
    } else if (strcmp(func_name, "ROIDisable") == 0) {
        unsigned int (*ROIDisable) (void);
        ROIDisable = dlsym(self->dlh, "ROIDisable");
        ret = ROIDisable();
        ret = ustc_error_mapping(ret);
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "ROIEnable") == 0) {
        unsigned int (*ROIEnable) (uint16_t, uint16_t, uint16_t, uint16_t);
        uint16_t rowStartNum, rowKeepNum, colStartNum, colKeepNum;
        if ((ret = fscanf(fp, "%hu %hu %hu %hu", &rowStartNum, &rowKeepNum, &colStartNum, &colKeepNum)) < 4 ){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            ROIEnable = dlsym(self->dlh, "ROIEnable");
            ret = ROIEnable(rowStartNum, rowKeepNum, colStartNum, colKeepNum);
            ret = ustc_error_mapping(ret);
            if (read_size != NULL) {
                *read_size = 0;
            }
        }
    } else if (strcmp(func_name, "Runningtime") == 0 ) {
        unsigned int (*Runningtime) (double *);
        double running_time = -1.00;
        Runningtime = dlsym(self->dlh, "Runningtime");
        ret = Runningtime(&running_time);
        ret = ustc_error_mapping(ret);
        snprintf(read_buffer, read_buffer_size, "%.2f", running_time);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else if (strcmp(func_name, " Save_configuration") == 0) {
        unsigned int (* Save_configuration) (void);
        Save_configuration = dlsym(self->dlh, " Save_configuration");
        ret =  Save_configuration();
        ret = ustc_error_mapping(ret);
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "set_channel_votlage") == 0) {
        unsigned int (*set_channel_votlage) (int, float);
        int chan;
        float voltage;
        if ((ret = fscanf(fp, "%d %f", &chan, &voltage)) < 2 ){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            if (chan < 1 || chan > 3) {
                ret = AAOS_EINVAL;
                goto error;
            }
            set_channel_votlage = dlsym(self->dlh, "set_channel_votlage");
            ret = set_channel_votlage(chan, voltage);
            ret = ustc_error_mapping(ret);
            if (read_size != NULL) {
                *read_size = 0;
            }
        }
    } else if (strcmp(func_name, "set_cooltemp") == 0) {
        unsigned int (*set_cooltemp) (float);
        float temperature;
        if ((ret = fscanf(fp, "%f", &temperature)) < 1){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            set_cooltemp = dlsym(self->dlh, "set_cooltemp");
            ret = set_cooltemp(temperature);
            ret = ustc_error_mapping(ret);
            if (read_size != NULL) {
                *read_size = 0;
            }
        }
    } else if (strcmp(func_name, "set_pid") == 0) {
        unsigned int (*set_pid) (int, int);
        int chan, status;
        if ((ret = fscanf(fp, "%d %d", &chan, &status)) < 2 ){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            if (chan < 1 || chan > 3) {
                ret = AAOS_EINVAL;
                goto error;
            }
            if (status != 0 && status != 1) {
                ret = AAOS_EINVAL;
                goto error;
            }
            set_pid = dlsym(self->dlh, "set_pid");
            ret = set_pid(chan, status);
            ret = ustc_error_mapping(ret);
            if (read_size != NULL) {
                *read_size = 0;
            }
        }
    } else if (strcmp(func_name, "set_power") == 0) {
        unsigned int (*set_power) (int, int);
        int chan, status;
        if ((ret = fscanf(fp, "%d %d", &chan, &status)) < 2 ){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            if (chan != 1 && chan != 2) {
                ret = AAOS_EINVAL;
                goto error;
            }
            if (status != 0 && status != 1) {
                ret = AAOS_EINVAL;
                goto error;
            }
            set_power = dlsym(self->dlh, "set_power");
            ret = set_power(chan, status);
            ret = ustc_error_mapping(ret);
            if (read_size != NULL) {
                *read_size = 0;
            }
        }
    } else if (strcmp(func_name, "set_tcb_target_temp") == 0) {
        unsigned int (*set_tcb_target_temp) (int, int);
        int chan, temperature;
        if ((ret = fscanf(fp, "%d %d", &chan, &temperature)) < 2 ){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            if (chan < 1 || chan > 3) {
                ret = AAOS_EINVAL;
                goto error;
            }
            set_tcb_target_temp = dlsym(self->dlh, "set_tcb_target_temp");
            ret = set_tcb_target_temp(chan, temperature);
            ret = ustc_error_mapping(ret);
            if (read_size != NULL) {
                *read_size = 0;
            }
        }
    } else if (strcmp(func_name, "SetGain") == 0) {
        unsigned int (*SetGain) (float, float);
        float high, low;
        if ((ret = fscanf(fp, "%f %f", &high, &low)) < 2 ){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            SetGain = dlsym(self->dlh, "SetGain");
            ret = SetGain(high, low);
            ret = ustc_error_mapping(ret);
            if (read_size != NULL) {
                *read_size = 0;
            }
        }
    } else if (strcmp(func_name, "SetHeatPWM") == 0) {
        unsigned int (*SetHeatPWM) (int, uint8_t *);
        int chan;
        uint8_t heatpwm;
        if ((ret = fscanf(fp, "%d %c", &chan, &heatpwm)) < 2 ){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            SetHeatPWM = dlsym(self->dlh, "SetHeatPWM");
            ret = SetHeatPWM(chan, &heatpwm);
            ret = ustc_error_mapping(ret);
            if (read_size != NULL) {
                *read_size = 0;
            }
        }
    } else if (strcmp(func_name, "SetPumpMaxPower") == 0) {
        unsigned int (*SetPumpMaxPower) (double);
        double power;
        if ((ret = fscanf(fp, "%lf", &power)) < 1){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            SetPumpMaxPower = dlsym(self->dlh, "SetPumpMaxPower");
            if (power < 10. || power > 40.) {
                ret =AAOS_EINVAL;
                goto error;
            }
            ret = SetPumpMaxPower(power);
            ret = ustc_error_mapping(ret);
            if (read_size != NULL) {
                *read_size = 0;
            }
        }
    } else if (strcmp(func_name, "SetPumpTargetVol") == 0) {
        unsigned int (*SetPumpTargetVol) (double);
        double voltage;
        if ((ret = fscanf(fp, "%lf", &voltage)) < 1){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            SetPumpTargetVol = dlsym(self->dlh, "SetPumpMaxPower");
            if (voltage < 3000. || voltage > 7000.) {
                ret =AAOS_EINVAL;
                goto error;
            }
            ret = SetPumpTargetVol(voltage);
            ret = ustc_error_mapping(ret);
            if (read_size != NULL) {
                *read_size = 0;
            }
        }
    } else if (strcmp(func_name, "SetPumpUnit") == 0) {
        unsigned int (*SetPumpUnit) (int);
        int unit;
        if ((ret = fscanf(fp, "%d", &unit)) < 1){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            SetPumpUnit = dlsym(self->dlh, "SetPumpUnit");
            if (unit < 0 || unit > 2) {
                ret =AAOS_EINVAL;
                goto error;
            }
            ret = SetPumpUnit(unit);
            ret = ustc_error_mapping(ret);
            if (read_size != NULL) {
                *read_size = 0;
            }
        }
    } else if (strcmp(func_name, "SetRtnsMode") == 0) {
        unsigned int (*SetRtnsMode) (uint16_t);
        uint16_t mode;
        if ((ret = fscanf(fp, "%hu", &mode)) < 1){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            SetRtnsMode = dlsym(self->dlh, "SetRtnsMode");
            if (mode > 1) {
                ret =AAOS_EINVAL;
                goto error;
            }
            ret = SetRtnsMode(mode);
            ret = ustc_error_mapping(ret);
            if (read_size != NULL) {
                *read_size = 0;
            }
        }
    } else if (strcmp(func_name, "SetVacuumUnit") == 0) {
        unsigned int (*SetVacuumUnit) (int);
        int unit;
        if ((ret = fscanf(fp, "%d", &unit)) < 1){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            SetVacuumUnit = dlsym(self->dlh, "SetVacuumUnit");
            if (unit < 0 || unit > 2) {
                ret =AAOS_EINVAL;
                goto error;
            }
            ret = SetVacuumUnit(unit);
            ret = ustc_error_mapping(ret);
            if (read_size != NULL) {
                *read_size = 0;
            }
        }
    } else if (strcmp(func_name, "VideoMode") == 0) {
        unsigned int (*VideoMode) (uint16_t);
        uint16_t mode;
        if ((ret = fscanf(fp, "%hu", &mode)) < 1){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            VideoMode = dlsym(self->dlh, "VideoMode");
            if (mode > 1) {
                ret =AAOS_EINVAL;
                goto error;
            }
            ret = VideoMode(mode);
            ret = ustc_error_mapping(ret);
            if (read_size != NULL) {
                *read_size = 0;
            }
        }
    } else if (strcmp(func_name, "SetSerialNumber") == 0) {
        unsigned int (*SetSerialNumber) (PIXELX_SERIAL *);
        uint8_t Model_Enum, MB_ID, Year, Month, Day, Hour, version, version_aa, id, checksum;
        if ((ret = fscanf(fp, "%c %c %c %c %c %c %c %c %c %c", &Model_Enum, &MB_ID, &Year, &Month, &Day, &Hour, &version, &version_aa, &id, &checksum)) < 10){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            PIXELX_SERIAL serial_no;
            serial_no.Model_Enum = Model_Enum;
            serial_no.MB_ID = MB_ID;
            serial_no.Year = Year;
            serial_no.Month = Month;
            serial_no.Day = Day;
            serial_no.Hour = Hour;
            serial_no.version = version;
            serial_no.version_aa = version_aa;
            serial_no.id = id;
            serial_no.checksum = checksum;
            SetSerialNumber = dlsym(self->dlh, "SetSerialNumber");
            ret = SetSerialNumber(&serial_no);
            ret = ustc_error_mapping(ret);
            if (read_size != NULL) {
                *read_size = 0;
            }
        }
    } else if (strcmp(func_name, "GetSerialNumber") == 0) {
        unsigned int (*GetSerialNumber) (PIXELX_SERIAL *);
        PIXELX_SERIAL serial_no;
            
        GetSerialNumber = dlsym(self->dlh, "GetSerialNumber");
        ret = GetSerialNumber(&serial_no);
        ret = ustc_error_mapping(ret);
        snprintf(read_buffer, read_buffer_size, "%03d %03d %02d %02d %02d %02d %03d %03d %03d %03d", serial_no.Model_Enum, serial_no.MB_ID, serial_no.Year, serial_no.Month, serial_no.Day, serial_no.Hour, serial_no.version, serial_no.version_aa, serial_no.id, serial_no.checksum);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else if (strcmp(func_name, "GetTemperatureDiff") == 0) {
        unsigned int (*GetTemperatureDiff) (double *, double *);
        double cold_diff, hot_diff;
        GetTemperatureDiff = dlsym(self->dlh, "GetTemperatureDiff");
        ret = GetTemperatureDiff(&cold_diff, &hot_diff);
        ret = ustc_error_mapping(ret);
        snprintf(read_buffer, read_buffer_size, "%.2f %.2f", cold_diff, hot_diff);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else if (strcmp(func_name, "CamErrorName") == 0) {
        const char * (*CamErrorName) (int);
        int errorcode;
        if ((ret = fscanf(fp, "%d", &errorcode)) < 1){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            CamErrorName = dlsym(self->dlh, "CamErrorName");
            snprintf(read_buffer, read_buffer_size, "%s", CamErrorName(errorcode));
            if (read_size != NULL) {
                *read_size = strlen(read_buffer) + 1;
            }
        }
    } else if (strcmp(func_name, "CamErrorStrerror") == 0) {
        const char * (*CamErrorStrerror) (int);
        int errorcode;
        if ((ret = fscanf(fp, "%d", &errorcode)) < 1){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            CamErrorStrerror = dlsym(self->dlh, "CamErrorStrerror");
            snprintf(read_buffer, read_buffer_size, "%s", CamErrorStrerror(errorcode));
            if (read_size != NULL) {
                *read_size = strlen(read_buffer) + 1;
            }
        }
    } else if (strcmp(func_name, "SaveAsFITS") == 0) {
        unsigned int (*SaveAsFITS) (const char *);
        char path[PATHSIZE];
        if ((ret = fscanf(fp, "%s", path)) < 1 ){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            SaveAsFITS = dlsym(self->dlh, "SaveAsFITS");
            ret = SaveAsFITS(path);
            ret = ustc_error_mapping(ret);
            if (read_size != NULL) {
                *read_size = 0;
            }
        }
    } else if (strcmp(func_name, "SaveAsFITSAccum") == 0) {
        unsigned int (*SaveAsFITSAccum) (const char *);
        char path[PATHSIZE];
        if ((ret = fscanf(fp, "%s", path)) < 1 ){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            SaveAsFITSAccum = dlsym(self->dlh, "SaveAsFITSAccum");
            ret = SaveAsFITSAccum(path);
            ret = ustc_error_mapping(ret);
            if (read_size != NULL) {
                *read_size = 0;
            }
        }
    } else if (strcmp(func_name, "SaveNthAsFITS") == 0) {
        unsigned int (*SaveNthAsFITS) (const char *, uint16_t);
        char path[PATHSIZE];
        uint16_t idx;
        if ((ret = fscanf(fp, "%s %hu", path, &idx)) < 2 ){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            SaveNthAsFITS = dlsym(self->dlh, "SaveNthAsFITS");
            ret = SaveNthAsFITS(path, idx);
            ret = ustc_error_mapping(ret);
            if (read_size != NULL) {
                *read_size = 0;
            }
        }
    }
    else {
		ret = AAOS_EBADCMD;
	}
    
error:
	fclose(fp);
	return ret;
}

static int
USTCCamera_abort(void *_self)
{
    struct USTCCamera *self = cast(USTCCamera(), _self);
    
    int ret = AAOS_OK;
    
    unsigned int (*StopExposure)(void) = dlsym(self->dlh, "StopExposure");
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    if ((self->_.d_state.state&DETECTOR_STATE_EXPOSING) || (self->_.d_state.state&DETECTOR_STATE_READING)) {
        Pthread_mutex_lock(&self->mtx);
        ret = StopExposure();
        Pthread_mutex_unlock(&self->mtx);
        if (ret == USTC_CCD_SUCCESS) {
            ret = AAOS_OK;
            self->_.d_state.state |= DETECTOR_STATE_IDLE;
        } else {
            ret = ustc_error_mapping(ret);
        }
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    return ret;
}

static int
USTCCamera_stop(void *_self)
{
    return AAOS_ENOTSUP;
}

static int
USTCCamera_set_frame_rate(void *_self, double frame_rate)
{
    struct USTCCamera *self = cast(USTCCamera(), _self);
    
    unsigned int (*SetExposureInterval)(double) = dlsym(self->dlh, "SetExposureInterval");
    int ret;
	
    Pthread_mutex_lock(&self->_.d_state.mtx);
    Pthread_mutex_lock(&self->mtx);
    ret = SetExposureInterval(1./frame_rate);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        self->_.d_param.frame_rate = 1./frame_rate;
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
	return ustc_error_mapping(ret);
}

static int
USTCCamera_get_frame_rate(void *_self, double *frame_rate)
{
    struct USTCCamera *self = cast(USTCCamera(), _self);
    
    unsigned int (*GetExposureInterval)(double *) = dlsym(self->dlh, "GetExposureInterval");
    int ret;
    
    Pthread_mutex_lock(&self->mtx);
    ret = GetExposureInterval(frame_rate);
    *frame_rate = 1. / *frame_rate;
    Pthread_mutex_unlock(&self->mtx);
    
	return ustc_error_mapping(ret);
}

static int
USTCCamera_set_exposure_time(void *_self, double exposure_time)
{
    struct USTCCamera *self = cast(USTCCamera(), _self);
    
    unsigned int (*SetExposureTime)(double) = dlsym(self->dlh, "SetExposureTime");
    int ret;
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    Pthread_mutex_lock(&self->mtx);
    ret = SetExposureTime(exposure_time);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        self->_.d_param.exposure_time = exposure_time;
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    return ustc_error_mapping(ret);
}

static int
USTCCamera_get_exposure_time(void *_self, double *exposure_time)
{
    struct USTCCamera *self = cast(USTCCamera(), _self);
    
    unsigned int (*GetExposureInterval)(double *) = dlsym(self->dlh, "GetExposureInterval");
    int ret;
    
    Pthread_mutex_lock(&self->mtx);
    ret = GetExposureInterval(exposure_time);
    Pthread_mutex_unlock(&self->mtx);
    
    return ustc_error_mapping(ret);
}

static int
USTCCamera_enable_cooling(void *_self)
{
    struct USTCCamera *self = cast(USTCCamera(), _self);
    
    unsigned int (*CoolerOn)(void) = dlsym(self->dlh, "CoolerOn");
    int ret;
    
    Pthread_mutex_lock(&self->mtx);
    ret = CoolerOn();
    Pthread_mutex_unlock(&self->mtx);
    
	return ustc_error_mapping(ret);
}

static int
USTCCamera_disable_cooling(void *_self)
{
    struct USTCCamera *self = cast(USTCCamera(), _self);
    
    unsigned int (*CoolerOff)(void) = dlsym(self->dlh, "CoolerOff");
    int ret;
    
    Pthread_mutex_lock(&self->mtx);
    ret = CoolerOff();
    Pthread_mutex_unlock(&self->mtx);
    
    return ustc_error_mapping(ret);
}

static int
USTCCamera_set_temperature(void *_self, double temperature)
{
    struct USTCCamera *self = cast(USTCCamera(), _self);
    
    unsigned int (*Coolertemp)(double) = dlsym(self->dlh, "set_cooltemp");
    int ret;
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    Pthread_mutex_lock(&self->mtx);
    ret = Coolertemp(temperature);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        self->_.d_param.temperature = temperature;
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    return ustc_error_mapping(ret);
}

static int
USTCCamera_get_temperature(void *_self, double *temperature)
{
    struct USTCCamera *self = cast(USTCCamera(), _self);
    
    unsigned int (*Coolertemp)(double *) = dlsym(self->dlh, "Coolertemp");
    int ret;
    
    Pthread_mutex_lock(&self->mtx);
    ret = Coolertemp(temperature);
    Pthread_mutex_unlock(&self->mtx);
    
    return ustc_error_mapping(ret);
}

static int
USTCCamera_set_region(void *_self, uint32_t x_offset, uint32_t y_offset, uint32_t width, uint32_t height)
{
    struct USTCCamera *self = cast(USTCCamera(), _self);
    
    int ret;
    
    int (*ROIEnable)(uint16_t, uint16_t, uint16_t, uint16_t) = dlsym(self->dlh, "ROIEnable");
    if (ROIEnable == NULL) {
        return AAOS_ENOTSUP;
    }
    Pthread_mutex_lock(&self->_.d_state.mtx);
    while (self->_.d_state.state == DETECTOR_STATE_EXPOSING || self->_.d_state.state == DETECTOR_STATE_READING) {
        Pthread_cond_wait(&self->_.d_state.cond, &self->_.d_state.mtx);
    }
    Pthread_mutex_lock(&self->mtx);
    ret = ROIEnable(x_offset, width, y_offset, height);
    Pthread_mutex_unlock(&self->mtx);
    ret = ustc_error_mapping(ret);
    if (ret == AAOS_OK) {
        self->_.d_param.image_width = width;
        self->_.d_param.image_height = height;
        self->_.d_param.x_offset = x_offset;
        self->_.d_param.y_offset = y_offset;
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
	return ret;
}

static int
USTCCamera_get_region(void *_self, uint32_t *x_offset, uint32_t *y_offset, uint32_t *width, uint32_t *height)
{
    struct USTCCamera *self = cast(USTCCamera(), _self);
    
    int ret;
    uint16_t xo, yo, w, h;
    
    unsigned int (*GetExposureROI)(uint16_t *, uint16_t *, uint16_t *, uint16_t *) = dlsym(self->dlh, "GetExposureROI");
    if (GetExposureROI == NULL) {
        return AAOS_ENOTSUP;
    }
    Pthread_mutex_lock(&self->_.d_state.mtx);
    Pthread_mutex_lock(&self->mtx);
    ret = GetExposureROI(&xo, &w, &yo, &h);
    Pthread_mutex_unlock(&self->mtx);
    ret = ustc_error_mapping(ret);
    if (ret == AAOS_OK) {
        *x_offset = xo;
        *y_offset = yo;
        *width = w;
        *height = h;
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    return ret;
}

static int 
USTCCamera_set_gain(void *_self, double gain)
{
    struct USTCCamera *self = cast(USTCCamera(), _self);
    
    int ret;
    unsigned int (*SetPreAmpGain)(uint8_t) = dlsym(self->dlh, "SetPreAmpGain");
    
    if (fabs(gain - 5.0) < 0.0000001) {
        ret = SetPreAmpGain(1);
        Pthread_mutex_lock(&self->_.d_state.mtx);
        self->_.d_param.gain = 5.0;
        Pthread_mutex_unlock(&self->_.d_state.mtx);
    } else if (fabs(gain - 10.0) < 0.0000001) {
        ret = SetPreAmpGain(0);
        Pthread_mutex_lock(&self->_.d_state.mtx);
        self->_.d_param.gain = 10.0;
        Pthread_mutex_unlock(&self->_.d_state.mtx);
    } else {
        return AAOS_EINVAL;
    }

	return ustc_error_mapping(ret);
}

static int 
USTCCamera_get_gain(void *_self, double *gain)
{
    struct USTCCamera *self = cast(USTCCamera(), _self);
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    *gain = self->_.d_param.gain;
    Pthread_mutex_unlock(&self->_.d_state.mtx);

	return AAOS_OK;
}

static int 
USTCCamera_set_readout_speed(void *_self, double readout_speed)
{
	return AAOS_ENOTSUP;
}

static int 
USTCCamera_get_readout_speed(void *_self, double *readout_speed)
{
	return AAOS_ENOTSUP;
}

static int 
USTCCamera_power_on(void *_self)
{
    struct USTCCamera *self = cast(USTCCamera(), _self);
    
    unsigned int (*DetectorPowerOn)(void) = dlsym(self->dlh, "DetectorPowerOn");
    int ret;

    Pthread_mutex_lock(&self->_.d_state.mtx);
    if (!(self->_.d_state.state&DETECTOR_STATE_OFFLINE)) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        return AAOS_OK;
    } else if (self->_.d_state.state&DETECTOR_STATE_UNINITIALIZED) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        return AAOS_EUNINIT;
    }
    Pthread_mutex_lock(&self->mtx);
    ret = DetectorPowerOn();
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        self->_.d_state.state = DETECTOR_STATE_IDLE;
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    return ustc_error_mapping(ret);
}

static int 
USTCCamera_power_off(void *_self)
{
    struct USTCCamera *self = cast(USTCCamera(), _self);
    
    unsigned int (*DetectorPowerOff)(void) = dlsym(self->dlh, "DetectorPowerOff");
    int ret;
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    if (self->_.d_state.state&DETECTOR_STATE_OFFLINE) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        return AAOS_OK;
    } else if (self->_.d_state.state&DETECTOR_STATE_UNINITIALIZED) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        return AAOS_EUNINIT;
    }
    Pthread_mutex_lock(&self->mtx);
    ret = DetectorPowerOff();
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        self->_.d_state.state |= DETECTOR_STATE_OFFLINE;
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    return ustc_error_mapping(ret);
}

static int
USTCCamera_init(void *_self)
{
    struct USTCCamera *self = cast(USTCCamera(), _self);
    
    int cam_num, chipnum = 0, width = 0, height = 0;
    float high = -1., low = -1.;
    int ret;
    
    unsigned int (*ListCameras) (int *) = dlsym(self->dlh, "ListCameras");
    unsigned int (*Initialize) (unsigned int , unsigned int) = dlsym(self->dlh, "Initialize");
    unsigned int (*DetectorPowerON) (void) = dlsym(self->dlh, "DetectorPowerON");
    unsigned int (*GetChipNum) (int *) = dlsym(self->dlh, "GetChipNum");
    unsigned int (*GetDetector) (int *, int *) = dlsym(self->dlh, "GetDetector");
    unsigned int (*GetGain) (float *, float *) = dlsym(self->dlh, "GetGain");
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    if (!(self->_.d_state.state&DETECTOR_STATE_UNINITIALIZED)) {
        ret = USTC_CCD_SUCCESS;
        goto power_on;
    }
    Pthread_mutex_lock(&self->mtx);
    ret = ListCameras(&cam_num);
    Pthread_mutex_unlock(&self->mtx);
    if (ret != USTC_CCD_SUCCESS) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        goto error;
    }
    Pthread_mutex_lock(&self->mtx);
    ret = Initialize(self->log_level, self->which);
    Pthread_mutex_lock(&self->mtx);
    if (ret != USTC_CCD_SUCCESS) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        goto error;
    }
    self->_.d_state.state &= ~DETECTOR_STATE_UNINITIALIZED;
power_on:
    if (!(self->_.d_state.state&DETECTOR_STATE_OFFLINE)) {
        ret = USTC_CCD_SUCCESS;
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        goto error;
    }
    Pthread_mutex_lock(&self->mtx);
    ret = DetectorPowerON();
    Pthread_mutex_lock(&self->mtx);
    self->_.d_state.state &= ~DETECTOR_STATE_OFFLINE;
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    self->_.d_cap.binning_available = TRUE;
    self->_.d_cap.cooling_available = TRUE;
    self->_.d_cap.exposure_time_available = TRUE;
    self->_.d_cap.frame_rate_available = TRUE;
    self->_.d_cap.gain_available = TRUE;
    self->_.d_cap.gain_available = TRUE;
    self->_.d_cap.n_gain = 2;
    self->_.d_cap.gain_array = (double *) Malloc(2 * sizeof(double));

    Pthread_mutex_lock(&self->mtx);
    if ((ret = GetChipNum(&chipnum)) != USTC_CCD_SUCCESS) {
        Pthread_mutex_unlock(&self->mtx);
        goto error;
    }
    self->_.d_cap.n_chip = chipnum;
    if ((ret = GetDetector(&width, &height)) != USTC_CCD_SUCCESS) {
        Pthread_mutex_unlock(&self->mtx);
        goto error;
    }
    self->_.d_cap.width = width;
    self->_.d_cap.height = height;
    if ((ret = GetGain(&high, &low)) != USTC_CCD_SUCCESS) {
        Pthread_mutex_unlock(&self->mtx);
        goto error;
    }
    self->_.d_cap.gain_array[0] = low;
    self->_.d_cap.gain_array[1] = high;
    Pthread_mutex_unlock(&self->mtx);
    
error:
    return ustc_error_mapping(ret);
}

struct USTCCameraFrameProcess {
    struct USTCCamera *camera;
    unsigned char *buf;
    char *json_string;
    void *rpc;
    struct timespec tp;
    size_t nth;
    size_t n;
};

void static
USTCCamera_name_convention(struct USTCCamera *self, char *buf, size_t size, ...)
{
    FILE *fp;
    char time_buf[TIMESTAMPSIZE];
    struct timespec tp;
    struct tm tm_buf;
    size_t i, n;
    va_list ap;
    
    va_start(ap, size);
    i = va_arg(ap, size_t);
    n = va_arg(ap, size_t);
    va_end(ap);
    
    fp = fmemopen(buf, size, "rw+");
    if (self->_.d_proc.image_directory != NULL) {
        fprintf(fp, "%s/", self->_.d_proc.image_directory);
    }
    if (self->_.d_proc.image_prefix != NULL) {
        fprintf(fp, "%s_", self->_.d_proc.image_prefix);
    }
    if (self->_.name != NULL) {
        fprintf(fp, "%s_", self->_.name);
    }
    Clock_gettime(CLOCK_REALTIME, &tp);
    gmtime_r(&tp.tv_sec, &tm_buf);
    if (n == 1) {
        strftime(time_buf, TIMESTAMPSIZE, "%Y%m%d%H%M%S.fits", &tm_buf);
        fprintf(fp, "%s", time_buf);
    } else {
        strftime(time_buf, TIMESTAMPSIZE, "%Y%m%d%H%M%S", &tm_buf);
        fprintf(fp, "%s_%04lu.fits", time_buf, i + 1);
    }
    
    fclose(fp);
}

void static
USTCCamera_post_acquisition(struct USTCCamera *self, const char *pathname, ...)
{
    va_list ap;
    
    va_start(ap, pathname);
    void *rpc = va_arg(ap, void *);
    va_end(ap);
    if (rpc != NULL) {
        protobuf_set(rpc, PACKET_LENGTH, pathname, strlen(pathname) + 1);
        rpc_write(rpc);
    }
}

void static
USTCCamera_write_image(struct USTCCamera *self, void *buf, struct timespec *tp, size_t nth, size_t n_frame, char *string, void *rpc)
{
    char pathname[PATHSIZE], date_obs[TIMESTAMPSIZE], time_obs[TIMESTAMPSIZE], *s;
    struct tm time_buf;
    fitsfile *fptr;
    int status = 0;
    int naxis = 2;
    long naxes[2], nelements;
    unsigned short *array = buf;
    double gain, settemp = 9999.00;
    size_t i, n_chip = self->_.d_cap.n_chip;
    
    gmtime_r(&tp->tv_sec, &time_buf);
    strftime(date_obs, TIMESTAMPSIZE, "%Y-%m-%d", &time_buf);
    strftime(time_obs, TIMESTAMPSIZE, "%H:%M:%S", &time_buf);
    s = time_obs + strlen(time_obs);
    snprintf(s, TIMESTAMPSIZE - strlen(time_obs), "%06d", (int) tp->tv_nsec / 1000);
    
    naxes[0] = self->_.d_param.image_width;
    naxes[1] = self->_.d_param.image_height;
    if (self->_.d_proc.name_convention == NULL) {
        USTCCamera_name_convention(self, pathname, PATHSIZE, nth, n_frame);
    } else {
        self->_.d_proc.name_convention(self, pathname, PATHSIZE, nth, n_frame);
    }
    
    nelements = naxes[0] * naxes[1];
    fits_create_template(&fptr, pathname, self->_.d_proc.tpl_filename, &status);
    
    //fits_update_key_lng(fptr, "NEXTEND", 3, "Number of standard extensions", &status);
    fits_update_key_longstr(fptr, "DATE-OBS", date_obs, NULL, &status);
    fits_update_key_longstr(fptr, "TIME-OBS", time_obs, NULL, &status);
    USTCCamera_get_gain(self, &gain);
    fits_update_key_dbl(fptr, "GAIN", gain, 3, NULL, &status);
    Pthread_mutex_lock(&self->_.d_state.mtx);
    settemp = self->_.d_param.temperature;
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    fits_update_key_dbl(fptr, "SETTEMP", settemp, 2, NULL, &status);
    
    double controller_temperature = 9999.00, cool_temp = 9999.00, en_temp = 9999.0, power = -1.00, press = -1.00, hot_temp = 9999.00, motor_temperature = 9999.00;
    uint8_t coolstat = 255, speed = 255, heatpwm = 255;
    float cur2 = -1.00, vol2 = -1.00, temp = 9999.00;
    
    Pthread_mutex_lock(&self->mtx);
    unsigned int (*Controller_temperature)(double *) = dlsym(self->dlh, "Controller_temperature");
    unsigned int (*Coolertemp)(double *) = dlsym(self->dlh, "Coolertemp");
    unsigned int (*entemp)(double *) = dlsym(self->dlh, "entemp");
    unsigned int (*get_power)(double *) = dlsym(self->dlh, "get_power");
    unsigned int (*GetCoolerStatus)(uint8_t *) = dlsym(self->dlh, "GetCoolerStatus");
    unsigned int (*GetCurrentByChannel)(int, float *) = dlsym(self->dlh, "GetCurrentByChannel");
    unsigned int (*GetFanSpeed)(uint8_t *) = dlsym(self->dlh, "GetFanSpeed");
    unsigned int (*GetHeatPWM)(uint8_t *) = dlsym(self->dlh, "GetHeatPWM");
    unsigned int (*GetTemperature)(const char *, float *) = dlsym(self->dlh, "GetTemperature");
    unsigned int (*GetVacuum)(int, double *) = dlsym(self->dlh, "GetVacuum");
    unsigned int (*GetVoltageByChannel)(int, float *) = dlsym(self->dlh, "GetVoltageByChannel");
    unsigned int (*hottemp)(double *) = dlsym(self->dlh, "hottemp");
    unsigned int (*Motor_temperature)(double *) = dlsym(self->dlh, "Motor_temperature");
    
    Controller_temperature(&controller_temperature);
    fits_update_key_dbl(fptr, "CTRLTEMP", controller_temperature, 2, NULL, &status);
    Coolertemp(&cool_temp);
    fits_update_key_dbl(fptr, "COOLTEMP", cool_temp, 2, NULL, &status);
    entemp(&en_temp);
    fits_update_key_dbl(fptr, "ENVTEMP", en_temp, 2, NULL, &status);
    get_power(&power);
    fits_update_key_dbl(fptr, "POWER", power, 2, NULL, &status);
    GetCoolerStatus(&coolstat);
    fits_update_key_ulng(fptr, "COOLSTAT", coolstat, NULL, &status);
    GetCurrentByChannel(0, &cur2);
    cur2 = -1.00;
    fits_update_key_flt(fptr, "PB24V_I", cur2, 2, NULL, &status);
    cur2 = -1.00;
    GetCurrentByChannel(1, &cur2);
    fits_update_key_flt(fptr, "PB12V_I", cur2, 2, NULL, &status);
    cur2 = -1.00;
    GetCurrentByChannel(2, &cur2);
    fits_update_key_flt(fptr, "PB5V_I", cur2, 2, NULL, &status);
    cur2 = -1.00;
    GetCurrentByChannel(3, &cur2);
    fits_update_key_flt(fptr, "PB6V_I", cur2, 2, NULL, &status);
    GetFanSpeed(&speed);
    fits_update_key_ulng(fptr, "FANSPEED", speed, NULL, &status);
    GetHeatPWM(&heatpwm);
    fits_update_key_ulng(fptr, "HEATPWM", heatpwm, NULL, &status);
    GetTemperature("D1", &temp);
    fits_update_key_flt(fptr, "D1TEMP", temp, 2, NULL, &status);
    temp = 9999.00;
    GetTemperature("D2", &temp);
    fits_update_key_flt(fptr, "D2TEMP", temp, 2, NULL, &status);
    temp = 9999.00;
    GetTemperature("PT1", &temp);
    fits_update_key_flt(fptr, "PT1TEMP", temp, 2, NULL, &status);
    temp = 9999.00;
    GetTemperature("PT2", &temp);
    fits_update_key_flt(fptr, "PT2TEMP", temp, 2, NULL, &status);
    temp = 9999.00;
    GetTemperature("PT3", &temp);
    fits_update_key_flt(fptr, "PT3TEMP", temp, 2, NULL, &status);
    temp = 9999.00;
    GetTemperature("PT4", &temp);
    fits_update_key_flt(fptr, "PT4TEMP", temp, 2, NULL, &status);
    GetVacuum(1, &press);
    fits_update_key_dbl(fptr, "PR1", press, 2, NULL, &status);
    press = -1.00;
    GetVacuum(2, &press);
    fits_update_key_dbl(fptr, "PR2", press, 2, NULL, &status);
    press = -1.00;
    GetVacuum(3, &press);
    fits_update_key_dbl(fptr, "PR3", press, 2, NULL, &status);
    press = -1.00;
    GetVacuum(4, &press);
    fits_update_key_dbl(fptr, "PR4", press, 2, NULL, &status);
    press = -1.00;
    GetVacuum(5, &press);
    fits_update_key_dbl(fptr, "PR5", press, 2, NULL, &status);
    GetVoltageByChannel(0, &vol2);
    fits_update_key_flt(fptr, "PB24V_V", vol2, 2, NULL, &status);
    vol2 = -1.00;
    GetVoltageByChannel(1, &vol2);
    fits_update_key_flt(fptr, "PB12V_V", vol2, 2, NULL, &status);
    vol2 = -1.00;
    GetVoltageByChannel(2, &vol2);
    fits_update_key_flt(fptr, "PB5V_V", vol2, 2, NULL, &status);
    vol2 = -1.00;
    GetVoltageByChannel(3, &vol2);
    fits_update_key_flt(fptr, "PB6V_V", vol2, 2, NULL, &status);
    hottemp(&hot_temp);
    fits_update_key_dbl(fptr, "HOTTEMP", hot_temp, 2, NULL, &status);
    Motor_temperature(&motor_temperature);
    fits_update_key_dbl(fptr, "MTRREMP", motor_temperature, 2, NULL, &status);
    Pthread_mutex_unlock(&self->mtx);
    
    if (string != NULL) {
        
    }
    
    for (i = 0; i < n_chip; i++) {
        fits_create_img(fptr, USHORT_IMG, naxis, naxes, &status);
        fits_update_key_str(fptr, "EXTNAME", "raw", "extension name", &status);
        fits_update_key_lng(fptr, "EXTVER", i + 1, "extension version", &status);
        fits_write_img(fptr, TSHORT, 1, nelements, array + i * nelements, &status);
    }
    fits_close_file(fptr, &status);
    
    if (self->_.d_proc.post_acquisition == NULL) {
        USTCCamera_post_acquisition(self, pathname, rpc);
    } else {
        self->_.d_proc.post_acquisition(self, pathname, rpc);
    }
}

static void *
USTCCamera_image_process(void *arg)
{
    struct USTCCamera *self = ((struct USTCCameraFrameProcess *) arg)->camera;
    unsigned char *buf = ((struct USTCCameraFrameProcess *) arg)->buf;
    char *string = ((struct USTCCameraFrameProcess *) arg)->json_string;
    struct timespec *tp = &((struct USTCCameraFrameProcess *) arg)->tp;
    size_t i = ((struct USTCCameraFrameProcess *) arg)->nth, n = ((struct USTCCameraFrameProcess *) arg)->n;
    void *rpc = ((struct USTCCameraFrameProcess *) arg)->rpc;
    USTCCamera_write_image(self, buf, tp, i, n, string, rpc);
    free(buf);
    free(arg);
    if (i == n - 1) {
        free(string);
    }
    
    return NULL;
}

static int
USTCCamera_expose(void *_self, double exposure_time, uint32_t n_frame, va_list *app)
{
    struct USTCCamera *self = cast(USTCCamera(), _self);
    
    double frame_rate;
    uint32_t i, j, n;
    unsigned char *buf;
    pthread_t *tids;
    int ret = AAOS_OK;
    void *rpc = va_arg(*app, char *);
    char *json_string = va_arg(*app, char *);
	unsigned int (*SetExposureInterval)(double) = dlsym(self->dlh, "SetExposureInterval");
    unsigned int (*SetExposureTime)(double)= dlsym(self->dlh, "SetExposureTime");
    unsigned int (*SetContinuousCapture)(uint16_t) = dlsym(self->dlh, "SetContinuousCapture");
    unsigned int (*StartExposure)(void) = dlsym(self->dlh, "StartExposure");
    unsigned int (*StopExposure)(void) = dlsym(self->dlh, "StopExposure");
    unsigned int (*GetImage)(void *, int) = dlsym(self->dlh, "GetImage");
    unsigned int (*WaitForAcquisition)(void) = dlsym(self->dlh, "WaitForAcquisition");
    unsigned int (*SetEraseCount)(uint8_t) = dlsym(self->dlh, "SetEraseCount");
    unsigned int (*GetAcquisitionStatus)(void) = dlsym(self->dlh, "GetAcquisitionStatus");
    
    if (exposure_time < 0.) {
        return AAOS_EINVAL;
    }
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    switch (self->_.d_state.state) {
        case DETECTOR_STATE_IDLE:
        {
            self->_.d_param.exposure_time = exposure_time;
            self->_.d_param.frame_rate = 1. / exposure_time;
            Pthread_mutex_lock(&self->mtx);
            ret = SetExposureInterval(exposure_time);
            ret = ustc_error_mapping(ret);
            if (ret != AAOS_OK) {
                goto error2;
            }
            ret = SetExposureTime(exposure_time);
            ret = ustc_error_mapping(ret);
            if (ret != AAOS_OK) {
                goto error2;
            }
            ret = SetContinuousCapture(n_frame);
            ret = ustc_error_mapping(ret);
            if (ret != AAOS_OK) {
                goto error2;
            }
            ret = SetEraseCount(self->erase_count);
            ret = ustc_error_mapping(ret);
            if (ret != AAOS_OK) {
                goto error2;
            }
            ret = StartExposure();
            ret = ustc_error_mapping(ret);
            if (ret != AAOS_OK) {
                goto error2;
            }
            Pthread_mutex_unlock(&self->mtx);
            self->_.d_state.state = DETECTOR_STATE_EXPOSING;
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            
            tids = (pthread_t *) Malloc(sizeof(pthread_t));
            for (i = 0; i < n_frame; i++) {
                Nanosleep(exposure_time);
                Pthread_mutex_lock(&self->mtx);
                ret = GetAcquisitionStatus();
                Pthread_mutex_unlock(&self->mtx);
                if (ret == USTC_CCD_SUCCESS) {
                    Pthread_mutex_lock(&self->_.d_state.mtx);
                    self->_.d_state.state = DETECTOR_STATE_EXPOSING;
                    Pthread_mutex_unlock(&self->_.d_state.mtx);
                    n = (uint32_t) (self->_.d_param.image_width * self->_.d_param.image_height * self->_.d_cap.n_chip);
                    buf = (unsigned char *) Malloc(n * 2);
                    Pthread_mutex_lock(&self->mtx);
                    ret = GetImage(buf, n * 2);
                    Pthread_mutex_unlock(&self->mtx);
                    if (ret == USTC_CCD_SUCCESS) {
                        struct USTCCameraFrameProcess *arg = (struct USTCCameraFrameProcess *) Malloc(sizeof(struct USTCCameraFrameProcess));
                        arg->camera = self;
                        arg->buf = buf;
                        arg->json_string = json_string;
                        arg->n = n_frame;
                        arg->nth = i;
                        arg->rpc = rpc;
                        Clock_gettime(CLOCK_REALTIME, &arg->tp);
                        Pthread_create(&tids[i], NULL, USTCCamera_image_process, (void *) arg);
                    } else {
                        Pthread_mutex_lock(&self->_.d_state.mtx);
                        self->_.d_state.state = DETECTOR_STATE_IDLE;
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        free(buf);
                        free(json_string);
                        for (j = 0; j < i; j++) {
                            Pthread_join(tids[j], NULL);
                        }
                        free(tids);
                        return AAOS_ECANCELED;
                    }
                } else if (ret == USTC_CCD_ACQURING) {
                    Pthread_mutex_lock(&self->_.d_state.mtx);
                    self->_.d_state.state = DETECTOR_STATE_READING;
                    Pthread_mutex_unlock(&self->_.d_state.mtx);
                    Pthread_mutex_lock(&self->mtx);
                    ret = WaitForAcquisition();
                    Pthread_mutex_unlock(&self->mtx);
                    if (ret == USTC_CCD_SUCCESS) {
                        Pthread_mutex_lock(&self->_.d_state.mtx);
                        self->_.d_state.state = DETECTOR_STATE_EXPOSING;
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        n = (uint32_t) (self->_.d_param.image_width * self->_.d_param.image_height * self->_.d_cap.n_chip);
                        buf = (unsigned char *) Malloc(n * 2);
                        Pthread_mutex_lock(&self->mtx);
                        ret = GetImage(buf, n * 2);
                        Pthread_mutex_unlock(&self->mtx);
                        if (ret == USTC_CCD_SUCCESS) {
                            struct USTCCameraFrameProcess *arg = (struct USTCCameraFrameProcess *) Malloc(sizeof(struct USTCCameraFrameProcess));
                            arg->camera = self;
                            arg->buf = buf;
                            arg->json_string = json_string;
                            arg->nth = i;
                            arg->n = n_frame;
                            arg->rpc = rpc;
                            Clock_gettime(CLOCK_REALTIME, &arg->tp);
                            Pthread_create(&tids[i], NULL, USTCCamera_image_process, (void *) arg);
                        } else {
                            Pthread_mutex_lock(&self->_.d_state.mtx);
                            self->_.d_state.state = DETECTOR_STATE_IDLE;
                            Pthread_mutex_unlock(&self->_.d_state.mtx);
                            free(buf);
                            free(json_string);
                            for (j = 0; j < i; j++) {
                                Pthread_join(tids[j], NULL);
                            }
                            free(tids);
                            return AAOS_ECANCELED;
                        }
                    } else {
                        Pthread_mutex_lock(&self->_.d_state.mtx);
                        self->_.d_state.state = DETECTOR_STATE_IDLE;
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        free(json_string);
                        free(tids);
                        return AAOS_ECANCELED;
                    }
                } else {
                    Pthread_mutex_lock(&self->_.d_state.mtx);
                    self->_.d_state.state = DETECTOR_STATE_IDLE;
                    Pthread_mutex_unlock(&self->_.d_state.mtx);
                    free(json_string);
                    free(tids);
                    return AAOS_ECANCELED;
                }
            }
            for (i = 0; i < n_frame; i++) {
                Pthread_join(tids[i], NULL);
            }
            Pthread_mutex_lock(&self->_.d_state.mtx);
            Pthread_mutex_lock(&self->mtx);
            ret = StopExposure();
            Pthread_mutex_unlock(&self->mtx);
            self->_.d_state.state = DETECTOR_STATE_IDLE;
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            ret = ustc_error_mapping(ret);
            return ret;
        }
            break;
        case DETECTOR_STATE_UNINITIALIZED:
            ret = AAOS_EUNINIT;
            goto error;
            break;
        case DETECTOR_STATE_MALFUNCTION:
            ret = AAOS_EDEVMAL;
            goto error;
        case DETECTOR_STATE_READING:
        case DETECTOR_STATE_EXPOSING:
            ret = AAOS_EBUSY;
            goto error;
        default:
            ret = AAOS_ERROR;
            goto error;
            break;
    }
error2:
    Pthread_mutex_unlock(&self->mtx);
error:
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    return ret;
}

static int
USTCCamera_status(void *_self, void *res, size_t res_size, size_t *res_len)
{
    struct USTCCamera *self = cast(USTCCamera(), _self);
    
    cJSON *root_json, *detector_json, *dewars_json, *pump_json, *electronics_json;
    char *json_string = NULL;
    uint8_t readystat, coolstat, speed, heatpwm;
    uint16_t rowStartNum, rowKeepNum, colStartNum, colKeepNum;
    int chan;
    double running_time, power, temperature, en_temp, hot_temp, press, cur, vol;
    float vol2, cur2, temp;
    int ret;
    
    unsigned int (*GetAcquisitionStatus)(void) = dlsym(self->dlh, "GetAcquisitionStatus");
    unsigned int (*GetCameraReady)(uint8_t *) = dlsym(self->dlh, "GetCameraReady");
    unsigned int (*Runningtime)(double *) = dlsym(self->dlh, "Runningtime");
    unsigned int (*GetExposureROI)(uint16_t *, uint16_t *, uint16_t *, uint16_t *) = dlsym(self->dlh, "GetExposureROI");
    unsigned int (*GetCoolerStatus)(uint8_t *) = dlsym(self->dlh, "GetCoolerStatus");
    unsigned int (*get_power)(double *) = dlsym(self->dlh, "get_power");
    unsigned int (*Controller_temperature)(double *) = dlsym(self->dlh, "Controller_temperature");
    unsigned int (*entemp)(double *) = dlsym(self->dlh, "entemp");
    unsigned int (*hottemp)(double *) = dlsym(self->dlh, "hottemp");
    unsigned int (*GetVacuum)(int, double *) = dlsym(self->dlh, "ControllerHeat");
    unsigned int (*GetPumpVol)(double *) = dlsym(self->dlh, "GetPumpVol");
    unsigned int (*GetPumpCur)(double *) = dlsym(self->dlh, "GetPumpCur");
    unsigned int (*GetPumpTargetVol)(double *) = dlsym(self->dlh, "GetPumpTargetVol");
    unsigned int (*GetCurrentByChannel)(int, float *) = dlsym(self->dlh, "GetCurrentByChannel");
    unsigned int (*GetVoltageByChannel)(int, float *) = dlsym(self->dlh, "GetVoltageByChannel");
    unsigned int (*GetFanSpeed)(uint8_t *) = dlsym(self->dlh, "GetFanSpeed");
    unsigned int (*GetHeatPWM)(uint8_t *) = dlsym(self->dlh, "GetHeatPWM");
    unsigned int (*GetTemperature)(const char *, float *) = dlsym(self->dlh, "GetTemperature");
    
    root_json = cJSON_CreateObject();
    
    detector_json = cJSON_CreateObject();
    cJSON_AddItemToObject(root_json, "Detector", detector_json);
    Pthread_mutex_lock(&self->mtx);
    ret = GetAcquisitionStatus();
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_ACQURING) {
        cJSON_AddStringToObject(detector_json, "STATE", "EXPOSING");
    } else {
        unsigned int state;
        Pthread_mutex_lock(&self->_.d_state.mtx);
        state = self->_.d_state.state;
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        switch (state) {
            case DETECTOR_STATE_IDLE:
                cJSON_AddStringToObject(detector_json, "STATE", "IDLE");
                break;
            case DETECTOR_STATE_OFFLINE:
                cJSON_AddStringToObject(detector_json, "STATE", "POWER OFF");
                break;
            case DETECTOR_STATE_MALFUNCTION:
                cJSON_AddStringToObject(detector_json, "STATE", "MALFUNCTION");
                goto error;
                break;
            case DETECTOR_STATE_UNINITIALIZED:
                cJSON_AddStringToObject(detector_json, "STATE", "UNINITIALIZED");
                goto error;
                break;
            case DETECTOR_STATE_READING:
                cJSON_AddStringToObject(detector_json, "STATE", "READING");
                break;
            default:
                cJSON_AddStringToObject(detector_json, "STATE", "UNKNOWN");
                goto error;
                break;
        }
    }
    Pthread_mutex_lock(&self->mtx);
    ret = GetCameraReady(&readystat);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(detector_json, "ReadyState", readystat);
    } else {
        cJSON_AddNumberToObject(detector_json, "ReadyState", -1);
    }
    Pthread_mutex_lock(&self->mtx);
    ret = Runningtime(&running_time);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(detector_json, "RunningTime", running_time);
    } else {
        cJSON_AddNumberToObject(detector_json, "RunningTime", -1);
    }
    Pthread_mutex_lock(&self->mtx);
    ret = GetExposureROI(&rowStartNum, &rowKeepNum, &colStartNum, &colKeepNum);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(detector_json, "RIO-X", rowStartNum);
        cJSON_AddNumberToObject(detector_json, "RIO-Y", colStartNum);
        cJSON_AddNumberToObject(detector_json, "RIO-W", rowKeepNum);
        cJSON_AddNumberToObject(detector_json, "RIO-H", colKeepNum);
    } else {
        cJSON_AddNumberToObject(detector_json, "RIO-X", -1);
        cJSON_AddNumberToObject(detector_json, "RIO-Y", -1);
        cJSON_AddNumberToObject(detector_json, "RIO-W", -1);
        cJSON_AddNumberToObject(detector_json, "RIO-H", -1);
    }

    dewars_json = cJSON_CreateObject();
    cJSON_AddItemToObject(root_json, "Dewars", dewars_json);
    Pthread_mutex_lock(&self->mtx);
    ret = GetCoolerStatus(&coolstat);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(dewars_json, "CoolerStatus", coolstat);
    } else {
        cJSON_AddNumberToObject(dewars_json, "CoolerStatus", -1);
    }
    Pthread_mutex_lock(&self->mtx);
    ret = get_power(&power);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(dewars_json, "CoolerPower", power);
    } else {
        cJSON_AddNumberToObject(dewars_json, "CoolerPower", -1.);
    }
    Pthread_mutex_lock(&self->mtx);
    ret = Controller_temperature(&temperature);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(dewars_json, "ControllerTemp", temperature);
    } else {
        cJSON_AddNumberToObject(dewars_json, "ControllerTemp", 9999.0);
    }
    Pthread_mutex_lock(&self->mtx);
    ret = entemp(&en_temp);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(dewars_json, "AmbientTemp", en_temp);
    } else {
        cJSON_AddNumberToObject(dewars_json, "AmbientTemp", 9999.0);
    }
    Pthread_mutex_lock(&self->mtx);
    ret = hottemp(&hot_temp);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(dewars_json, "HotEndTemp", hot_temp);
    } else {
        cJSON_AddNumberToObject(dewars_json, "HotEndTemp", 9999.0);
    }
    Pthread_mutex_lock(&self->mtx);
    Pthread_mutex_lock(&self->mtx);
    ret = GetFanSpeed(&speed);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(dewars_json, "FanSpeed", speed);
    } else {
        cJSON_AddNumberToObject(dewars_json, "FanSpeed", -1);
    }
    Pthread_mutex_lock(&self->mtx);
    ret = GetHeatPWM(&heatpwm);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(dewars_json, "HeatPWM", heatpwm);
    } else {
        cJSON_AddNumberToObject(dewars_json, "HeatPWM", -1);
    }
    
    pump_json = cJSON_CreateObject();
    cJSON_AddItemToObject(root_json, "Pump", pump_json);
    Pthread_mutex_lock(&self->mtx);
    ret = GetVacuum(1, &press);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(pump_json, "PR1", press);
    } else {
        cJSON_AddNumberToObject(pump_json, "PR1", -1.);
    }
    Pthread_mutex_lock(&self->mtx);
    ret = GetVacuum(2, &press);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(pump_json, "PR2", press);
    } else {
        cJSON_AddNumberToObject(pump_json, "PR2", -1.);
    }
    Pthread_mutex_lock(&self->mtx);
    ret = GetVacuum(3, &press);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(pump_json, "PR3", press);
    } else {
        cJSON_AddNumberToObject(pump_json, "PR3", -1.);
    }
    Pthread_mutex_lock(&self->mtx);
    ret = GetVacuum(4, &press);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(pump_json, "PR4", press);
    } else {
        cJSON_AddNumberToObject(pump_json, "PR4", -1.);
    }
    Pthread_mutex_lock(&self->mtx);
    ret = GetVacuum(5, &press);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(pump_json, "PR5", press);
    } else {
        cJSON_AddNumberToObject(pump_json, "PR5", -1.);
    }
    Pthread_mutex_lock(&self->mtx);
    ret = GetPumpVol(&vol);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(pump_json, "PumpVoltage", vol);
    } else {
        cJSON_AddNumberToObject(pump_json, "PumpVoltage", -1.);
    }
    Pthread_mutex_lock(&self->mtx);
    ret = GetPumpCur(&cur);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(pump_json, "PumpCurrent", cur);
    } else {
        cJSON_AddNumberToObject(pump_json, "PumpCurrent", -1.);
    }
    Pthread_mutex_lock(&self->mtx);
    ret = GetPumpTargetVol(&vol);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(pump_json, "PumpTargetVoltage", vol);
    } else {
        cJSON_AddNumberToObject(pump_json, "PumpTargetVoltage", -1.);
    }
    
    electronics_json = cJSON_CreateObject();
    cJSON_AddItemToObject(root_json, "Elesctronics", pump_json);
    Pthread_mutex_lock(&self->mtx);
    ret = GetVoltageByChannel(0, &vol2);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(electronics_json, "PB24V Voltage", vol2);
    } else {
        cJSON_AddNumberToObject(electronics_json, "PB24V Voltage", -1.);
    }
    Pthread_mutex_lock(&self->mtx);
    ret = GetCurrentByChannel(0, &cur2);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(electronics_json, "PB24V Current", cur2);
    } else {
        cJSON_AddNumberToObject(electronics_json, "PB24V Current", -1.);
    }
    Pthread_mutex_lock(&self->mtx);
    ret = GetVoltageByChannel(1, &vol2);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(electronics_json, "PB12V Voltage", vol2);
    } else {
        cJSON_AddNumberToObject(electronics_json, "PB12V Voltage", -1.);
    }
    Pthread_mutex_lock(&self->mtx);
    ret = GetCurrentByChannel(1, &cur2);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(electronics_json, "PB12V Current", cur2);
    } else {
        cJSON_AddNumberToObject(electronics_json, "PB12V Current", -1.);
    }
    Pthread_mutex_lock(&self->mtx);
    ret = GetVoltageByChannel(2, &vol2);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(electronics_json, "PB5V Voltage", vol2);
    } else {
        cJSON_AddNumberToObject(electronics_json, "PB5V Voltage", -1.);
    }
    Pthread_mutex_lock(&self->mtx);
    ret = GetCurrentByChannel(2, &cur2);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(electronics_json, "PB5V Current", cur2);
    } else {
        cJSON_AddNumberToObject(electronics_json, "PB5V Current", -1.);
    }
    Pthread_mutex_lock(&self->mtx);
    ret = GetVoltageByChannel(3, &vol2);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(electronics_json, "PB6V Voltage", vol2);
    } else {
        cJSON_AddNumberToObject(electronics_json, "PB6V Voltage", -1.);
    }
    Pthread_mutex_lock(&self->mtx);
    ret = GetCurrentByChannel(3, &cur2);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(electronics_json, "PB6V Current", cur2);
    } else {
        cJSON_AddNumberToObject(electronics_json, "PB6V Current", -1.);
    }
    Pthread_mutex_lock(&self->mtx);
    ret = GetTemperature("D1", &temp);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(electronics_json, "D1Temp", temp - 273.15);
    } else {
        cJSON_AddNumberToObject(electronics_json, "D1Temp", 9999.0);
    }
    Pthread_mutex_lock(&self->mtx);
    ret = GetTemperature("D2", &temp);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(electronics_json, "D2Temp", temp - 273.15);
    } else {
        cJSON_AddNumberToObject(electronics_json, "D2Temp", 9999.0);
    }
    Pthread_mutex_lock(&self->mtx);
    ret = GetTemperature("PT1", &temp);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(electronics_json, "PT1", temp - 273.15);
    } else {
        cJSON_AddNumberToObject(electronics_json, "PT1", 9999.0);
    }
    Pthread_mutex_lock(&self->mtx);
    ret = GetTemperature("PT2", &temp);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(electronics_json, "PT2", temp - 273.15);
    } else {
        cJSON_AddNumberToObject(electronics_json, "PT2", 9999.0);
    }
    Pthread_mutex_lock(&self->mtx);
    ret = GetTemperature("PT3", &temp);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(electronics_json, "PT3", temp - 273.15);
    } else {
        cJSON_AddNumberToObject(electronics_json, "PT3", 9999.0);
    }
    Pthread_mutex_lock(&self->mtx);
    ret = GetTemperature("PT4", &temp);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(electronics_json, "PT4", temp - 273.15);
    } else {
        cJSON_AddNumberToObject(electronics_json, "PT4", 9999.0);
    }
    
error:
    json_string = cJSON_Print(root_json);
    cJSON_Delete(root_json);
    snprintf(res, res_size, "%s", json_string);
    if (res_len != NULL) {
        *res_len = strlen(res) + 1;
    }
    free(json_string);
    return AAOS_OK;
}

static int
USTCCamera_info(void *_self, void *res, size_t res_size, size_t *res_len)
{
    struct USTCCamera *self = cast(USTCCamera(), _self);
    
    cJSON *root_json;
    char *json_string = NULL;
    PIXELX_SERIAL serial_no;
    int chipnum, width, height;
    
    int ret;
    
    unsigned int(*GetSerialNumber)(PIXELX_SERIAL *) = dlsym(self->dlh, "GetSerialNumber");
    unsigned int(*GetChipNum)(int *) = dlsym(self->dlh, "GetChipNum");
    unsigned int(*GetDetector)(int *, int *) = dlsym(self->dlh, "GetDetector");
    
    root_json = cJSON_CreateObject();
    cJSON_AddStringToObject(root_json, "Name", self->_.name);
    if (self->_.description != NULL) {
        cJSON_AddStringToObject(root_json, "Description", self->_.description);
    }
    
    Pthread_mutex_lock(&self->mtx);
    ret = GetSerialNumber(&serial_no);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        char buf[TIMESTAMPSIZE];
        cJSON_AddNumberToObject(root_json, "Model_Enum", serial_no.Model_Enum);
        cJSON_AddNumberToObject(root_json, "MB_ID", serial_no.MB_ID);
        cJSON_AddNumberToObject(root_json, "version", serial_no.version);
        cJSON_AddNumberToObject(root_json, "version_aa", serial_no.version_aa);
        cJSON_AddNumberToObject(root_json, "ID", serial_no.id);
        cJSON_AddNumberToObject(root_json, "Checksum", serial_no.checksum);
        snprintf(buf, TIMESTAMPSIZE, "%04u-%02u-%02u %02u", serial_no.Year, serial_no.Month, serial_no.Day, serial_no.Hour);
        cJSON_AddStringToObject(root_json, "Date", buf);
        
    }
    Pthread_mutex_lock(&self->mtx);
    ret = GetChipNum(&chipnum);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(root_json, "ChipNumber", chipnum);
    } else {
        cJSON_AddNumberToObject(root_json, "ChipNumber", 1);
    }
    Pthread_mutex_lock(&self->mtx);
    ret = GetDetector(&width, &height);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(root_json, "Width", width);
        cJSON_AddNumberToObject(root_json, "Height", height);
    } else {
        cJSON_AddNumberToObject(root_json, "Width", self->_.d_cap.width);
        cJSON_AddNumberToObject(root_json, "Height", self->_.d_cap.height);
    }
    
error:
    json_string = cJSON_Print(root_json);
    cJSON_Delete(root_json);
    snprintf(res, res_size, "%s", json_string);
    if (res_len != NULL) {
        *res_len = strlen(res) + 1;
    }
    cJSON_Delete(root_json);
    free(json_string);
    return AAOS_OK;
}

static const void *_ustc_camera_virtual_table;

static void
ustc_camera_virtual_table_destroy(void)
{
    delete((void *) _ustc_camera_virtual_table);
}

static void
ustc_camera_virtual_table_initialize(void)
{
    _ustc_camera_virtual_table = new(__DetectorVirtualTable(),
                                     __detector_init, "init", USTCCamera_init,
                                     __detector_info, "info", USTCCamera_info,
                                     __detector_status, "status", USTCCamera_status,
                                     __detector_expose, "expose", USTCCamera_expose,
                                     __detector_power_on, "power_on", USTCCamera_power_on,
                                     __detector_power_off, "power_off", USTCCamera_power_off,
                                     //__detector_set_binning, "set_binning", USTCCamera_set_binning,
                                     //__detector_get_binning, "get_binning", USTCCamera_get_binning,
                                     __detector_set_exposure_time, "set_exposure_time", USTCCamera_set_exposure_time,
                                     __detector_get_exposure_time, "get_exposure_time", USTCCamera_get_exposure_time,
                                     __detector_set_frame_rate, "set_frame_rate", USTCCamera_set_frame_rate,
                                     __detector_get_frame_rate, "get_frame_rate", USTCCamera_get_frame_rate,
                                     __detector_set_gain, "set_gain", USTCCamera_set_gain,
                                     __detector_get_gain, "get_gain", USTCCamera_get_gain,
                                     __detector_set_region, "set_region", USTCCamera_set_region,
                                     __detector_get_region, "get_region", USTCCamera_get_region,
                                     __detector_raw, "raw", USTCCamera_raw,
                                     //__detector_inspect, "inspect", USTCCamera_inspect,
                                     //__detector_wait_for_completion, "wait_for_completion", USTCCamera_wait_for_last_completion,
                                     (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(ustc_camera_virtual_table_destroy);
#endif
}

static const void *
ustc_camera_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, ustc_camera_virtual_table_initialize);
#endif
    
    return _ustc_camera_virtual_table;
}

#ifdef _USE_COMPILER_ATTRIBUTION_

static void __destructor__(void) __attribute__ ((destructor(_DETECTOR_PRIORITY_)));

static void
__destructor__(void)
{
	USTCCamera_destroy();
	USTCCameraClass_destroy();
	ustc_camera_virtual_table_destroy();
    
#ifdef __USE_ARAVIS__
    GenICam_destroy();
    GenICamClass_destroy();
    genicam_virtual_table_destroy();
#endif
    __Detector_destroy();
    __DetectorClass_destroy();
    __DetectorVirtualTable_destroy();
}

static void __constructor__(void) __attribute__ ((constructor(_DETECTOR_PRIORITY_)));

static void
__constructor__(void)
{
    __DetectorVirtualTable_initialize();
    __DetectorClass_initialize();
    __Detector_initialize();
	
	ustc_camera_virtual_table_initialize();
	USTCCameraClass_initialize();
	USTCCamera_initialize();
	
#ifdef __USE_ARAVIS__
    genicam_virtual_table_initialize();
    GenICamClass_initialize();
    GenICam_initialize();
#endif
}
#endif
