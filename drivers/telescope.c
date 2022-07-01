//
//  telescope.c
//  AAOS
//
//  Created by huyi on 18/6/12.
//  Copyright © 2018年 National Astronomical Observatories, Chinese Academy of Sciences. All rights reserved.
//

#include "astro.h"
#include "def.h"
#include "telescope_r.h"
#include "telescope.h"
#include "virtual.h"
#include "wrapper.h"

/*
 * TelescopeVirtualTable class.
 */

static void *
__TelescopeVirtualTable_ctor(void *_self, va_list *app)
{
    struct __TelescopeVirtualTable *self = super_ctor(__TelescopeVirtualTable(), _self, app);
    Method selector;
    
    while ((selector = va_arg(*app, Method))) {
        const char *tag = va_arg(*app, const char *);
        Method method = va_arg(*app, Method);
        if (selector == (Method) __telescope_status) {
            if (tag) {
                self->status.tag = tag;
                self->status.selector = selector;
            }
            self->status.method = method;
            continue;
        }
        if (selector == (Method) __telescope_power_on) {
            if (tag) {
                self->power_on.tag = tag;
                self->power_on.selector = selector;
            }
            self->power_on.method = method;
            continue;
        }
        if (selector == (Method) __telescope_power_off) {
            if (tag) {
                self->power_off.tag = tag;
                self->power_off.selector = selector;
            }
            self->power_off.method = method;
            continue;
        }
        if (selector == (Method) __telescope_init) {
            if (tag) {
                self->init.tag = tag;
                self->init.selector = selector;
            }
            self->init.method = method;
            continue;
        }
        if (selector == (Method) __telescope_park) {
            if (tag) {
                self->park.tag = tag;
                self->park.selector = selector;
            }
            self->park.method = method;
            continue;
        }
        if (selector == (Method) __telescope_park_off) {
            if (tag) {
                self->park_off.tag = tag;
                self->park_off.selector = selector;
            }
            self->park_off.method = method;
            continue;
        }
        if (selector == (Method) __telescope_stop) {
            if (tag) {
                self->stop.tag = tag;
                self->stop.selector = selector;
            }
            self->stop.method = method;
            continue;
        }
        if (selector == (Method) __telescope_go_home) {
            if (tag) {
                self->go_home.tag = tag;
                self->go_home.selector = selector;
            }
            self->go_home.method = method;
            continue;
        }
        if (selector == (Method) __telescope_move) {
            if (tag) {
                self->move.tag = tag;
                self->move.selector = selector;
            }
            self->move.method = method;
            continue;
        }
        if (selector == (Method) __telescope_try_move) {
            if (tag) {
                self->try_move.tag = tag;
                self->try_move.selector = selector;
            }
            self->try_move.method = method;
            continue;
        }
        if (selector == (Method) __telescope_timed_move) {
            if (tag) {
                self->timed_move.tag = tag;
                self->timed_move.selector = selector;
            }
            self->timed_move.method = method;
            continue;
        }
        if (selector == (Method) __telescope_slew) {
            if (tag) {
                self->slew.tag = tag;
                self->slew.selector = selector;
            }
            self->slew.method = method;
            continue;
        }
        if (selector == (Method) __telescope_try_slew) {
            if (tag) {
                self->try_slew.tag = tag;
                self->try_slew.selector = selector;
            }
            self->try_slew.method = method;
            continue;
        }
        if (selector == (Method) __telescope_timed_slew) {
            if (tag) {
                self->timed_slew.tag = tag;
                self->timed_slew.selector = selector;
            }
            self->timed_slew.method = method;
            continue;
        }
        if (selector == (Method) __telescope_raw) {
            if (tag) {
                self->raw.tag = tag;
                self->raw.selector = selector;
            }
            self->raw.method = method;
            continue;
        }
        if (selector == (Method) __telescope_set_move_speed) {
            if (tag) {
                self->set_move_speed.tag = tag;
                self->set_move_speed.selector = selector;
            }
            self->set_move_speed.method = method;
            continue;
        }
        if (selector == (Method) __telescope_get_move_speed) {
            if (tag) {
                self->get_move_speed.tag = tag;
                self->get_move_speed.selector = selector;
            }
            self->get_move_speed.method = method;
            continue;
        }
        if (selector == (Method) __telescope_set_slew_speed) {
            if (tag) {
                self->set_slew_speed.tag = tag;
                self->set_slew_speed.selector = selector;
            }
            self->set_slew_speed.method = method;
            continue;
        }
        if (selector == (Method) __telescope_get_slew_speed) {
            if (tag) {
                self->get_slew_speed.tag = tag;
                self->get_slew_speed.selector = selector;
            }
            self->get_slew_speed.method = method;
            continue;
        }
        if (selector == (Method) __telescope_set_track_rate) {
            if (tag) {
                self->set_track_rate.tag = tag;
                self->set_track_rate.selector = selector;
            }
            self->set_slew_speed.method = method;
            continue;
        }
        if (selector == (Method) __telescope_get_track_rate) {
            if (tag) {
                self->get_track_rate.tag = tag;
                self->get_track_rate.selector = selector;
            }
            self->get_track_rate.method = method;
            continue;
        }
    }
    
    return _self;
}

static void *
__TelescopeVirtualTable_dtor(void *_self)
{
    return super_dtor(Object(), _self);
}

static const void *___TelescopeVirtualTable;

static void
__TelescopeVirtualTable_destroy(void)
{
    free((void *) ___TelescopeVirtualTable);
}

static void
__TelescopeVirtualTable_initialize(void)
{
    ___TelescopeVirtualTable = new(VirtualTableClass(), "__TelescopeVirtualTable", VirtualTable(), sizeof(struct __TelescopeVirtualTable),
                                   ctor, "ctor", __TelescopeVirtualTable_ctor,
                                   dtor, "dtor", __TelescopeVirtualTable_dtor,
                                   (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(__TelescopeVirtualTable_destroy);
#endif
}

const void *
__TelescopeVirtualTable(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, __TelescopeVirtualTable_initialize);
#endif
    return ___TelescopeVirtualTable;
}

/*
 * Telecope class
 */

static pthread_key_t __telescope_option_key;

static
void TelescopeState_init(struct TelescopeState *t_state)
{
    Pthread_mutex_init(&t_state->mtx, NULL);
    Pthread_cond_init(&t_state->cond, NULL);
}

static
void TelescopeState_destroy(struct TelescopeState *t_state)
{
    Pthread_mutex_destroy(&t_state->mtx);
    Pthread_cond_destroy(&t_state->cond);
}

int
__telescope_release(void *_self)
{
    const struct __TelescopeClass *class = (const struct __TelescopeClass *) classOf(_self);
    
    if (isOf(class, __TelescopeClass()) && class->status.method) {
        return ((int (*)(void *)) class->status.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) __telescope_release, "release", _self);
        return result;
    }
}

static int
__Telescope_release(void *_self)
{
    struct __Telescope *self = cast(__Telescope(), _self);
    
    Pthread_mutex_lock(&self->t_state.mtx);
    if (self->t_state.state == TELESCOPE_STATE_TRACKING_WAIT) {
        self->t_state.state = TELESCOPE_STATE_TRACKING;
        Pthread_cond_broadcast(&self->t_state.cond);
    }
    Pthread_mutex_unlock(&self->t_state.mtx);
    return AAOS_OK;
}

int
__telescope_set_option(void *_self, uint16_t option)
{
    const struct __TelescopeClass *class = (const struct __TelescopeClass *) classOf(_self);
    
    if (isOf(class, __TelescopeClass()) && class->set_option.method) {
        return ((int (*)(void *, uint16_t)) class->set_option.method)(_self, option);
    } else {
        int result;
        forward(_self, &result, (Method) __telescope_set_option, "set_option", _self, option);
        return result;
    }
}

static int
__Telescope_set_option(void *_self, uint16_t option)
{
    struct __Telescope *self = cast(__Telescope(), _self);
    
    /*
    if (option & TELESCOPE_OPTION_ONESHOT) {
        uint16_t *oneshot_option = pthread_getspecific(__telescope_option_key);
        if (oneshot_option == NULL) {
            oneshot_option = (uint16_t *) Malloc(sizeof(uint16_t));
            pthread_setspecific(__telescope_option_key, oneshot_option);
        }
        *oneshot_option = option;
        return AAOS_OK;
    }
     */
    
    Pthread_mutex_lock(&self->t_state.mtx);
    self->option |= option;
    Pthread_mutex_unlock(&self->t_state.mtx);
    
    return AAOS_OK;
}

const char *
__telescope_get_name(const void *_self)
{
    const struct __TelescopeClass *class = (const struct __TelescopeClass *) classOf(_self);
    
    if (isOf(class, __TelescopeClass()) && class->get_name.method) {
        return ((const char * (*)(const void *)) class->get_name.method)(_self);
    } else {
        const char *result;
        forward(_self, &result, (Method) __telescope_get_name, "get_name", _self);
        return result;
    }
}

static const char *
__Telescope_get_name(const void *_self)
{
    struct __Telescope *self = cast(__Telescope(), _self);
    
    return self->name;
}

/*
 * Virtual function
 */

int
__telescope_status(void *_self, char *status_buffer, size_t status_buffer_size)
{
    const struct __TelescopeClass *class = (const struct __TelescopeClass *) classOf(_self);
    
    if (isOf(class, __TelescopeClass()) && class->status.method) {
        return ((int (*)(void *, char *, size_t)) class->status.method)(_self, status_buffer, status_buffer_size);
    } else {
        int result;
        forward(_self, &result, (Method) __telescope_status, "status", _self, status_buffer, status_buffer_size);
        return result;
    }
}

int
__telescope_power_on(void *_self)
{
    const struct __TelescopeClass *class = (const struct __TelescopeClass *) classOf(_self);
    
    if (isOf(class, __TelescopeClass()) && class->power_on.method) {
        return ((int (*)(void *)) class->power_on.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) __telescope_power_on, "power_on", _self);
        return result;
    }
}

int
__telescope_power_off(void *_self)
{
    const struct __TelescopeClass *class = (const struct __TelescopeClass *) classOf(_self);
    
    if (isOf(class, __TelescopeClass()) && class->power_off.method) {
        return ((int (*)(void *)) class->power_off.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) __telescope_power_off, "power_off", _self);
        return result;
    }
}

/*
 * Do any initilizing, only applied after powered on.
 * If telescope is not in UNINI state, has no effect.
 */

int
__telescope_init(void *_self)
{
    const struct __TelescopeClass *class = (const struct __TelescopeClass *) classOf(_self);
    
    if (isOf(class, __TelescopeClass()) && class->init.method) {
        return ((int (*)(void *)) class->init.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) __telescope_init, "init", _self);
        return result;
    }
}

/*
 * Stop both axes immediately,
 * no matter which state the telescope is in,
 * after park return, the telescope is in PARKED state.
 */

int
__telescope_park(void *_self)
{
    const struct __TelescopeClass *class = (const struct __TelescopeClass *) classOf(_self);
    
    if (isOf(class, __TelescopeClass()) && class->park.method) {
        return ((int (*)(void *)) class->park.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) __telescope_park, "park", _self);
        return result;
    }
}

int
__telescope_park_off(void *_self)
{
    const struct __TelescopeClass *class = (const struct __TelescopeClass *) classOf(_self);
    
    if (isOf(class, __TelescopeClass()) && class->park_off.method) {
        return ((int (*)(void *)) class->park_off.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) __telescope_park_off, "park_off", _self);
        return result;
    }
}

/*
 * if the telescope is in slewing or moving state,
 * stop slewing or moving, after stop returns,
 * the telescope is in tracking state.
 */

int
__telescope_stop(void *_self)
{
    const struct __TelescopeClass *class = (const struct __TelescopeClass *) classOf(_self);
    
    if (isOf(class, __TelescopeClass()) && class->stop.method) {
        return ((int (*)(void *)) class->stop.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) __telescope_stop, "stop", _self);
        return result;
    }
}

int
__telescope_go_home(void *_self)
{
    const struct __TelescopeClass *class = (const struct __TelescopeClass *) classOf(_self);
    
    if (isOf(class, __TelescopeClass()) && class->go_home.method) {
        return ((int (*)(void *)) class->go_home.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) __telescope_go_home, "go_home", _self);
        return result;
    }
}

int
__telescope_move(void *_self, unsigned int direction, double duration)
{
    const struct __TelescopeClass *class = (const struct __TelescopeClass *) classOf(_self);
    
    if (isOf(class, __TelescopeClass()) && class->move.method) {
        return ((int (*)(void *, unsigned int, double)) class->move.method)(_self, direction, duration);
    } else {
        int result;
        forward(_self, &result, (Method) __telescope_move, "move", _self, direction, duration);
        return result;
    }
}

int
__telescope_try_move(void *_self, unsigned direction, double duration)
{
    const struct __TelescopeClass *class = (const struct __TelescopeClass *) classOf(_self);
    
    if (isOf(class, __TelescopeClass()) && class->try_move.method) {
        return ((int (*)(void *, unsigned int, double)) class->try_move.method)(_self, direction, duration);
    } else {
        int result;
        forward(_self, &result, (Method) __telescope_try_move, "try_move", _self, direction, duration);
        return result;
    }
}

int
__telescope_timed_move(void *_self, unsigned int direction, double duration, double timeout)
{
    const struct __TelescopeClass *class = (const struct __TelescopeClass *) classOf(_self);
    
    if (isOf(class, __TelescopeClass()) && class->timed_move.method) {
        return ((int (*)(void *, unsigned int, double, double)) class->timed_move.method)(_self, direction, duration, timeout);
    } else {
        int result;
        forward(_self, &result, (Method) __telescope_timed_move, "timed_move", _self, direction, duration, timeout);
        return result;
    }
}

int
__telescope_slew(void *_self, double ra, double dec)
{
    const struct __TelescopeClass *class = (const struct __TelescopeClass *) classOf(_self);
    
    if (isOf(class, __TelescopeClass()) && class->slew.method) {
        return ((int (*)(void *, double, double)) class->slew.method)(_self, ra, dec);
    } else {
        int result;
        forward(_self, &result, (Method) __telescope_slew, "slew", _self, ra, dec);
        return result;
    }
}

int
__telescope_try_slew(void *_self, double ra, double dec)
{
    const struct __TelescopeClass *class = (const struct __TelescopeClass *) classOf(_self);
    
    if (isOf(class, __TelescopeClass()) && class->try_slew.method) {
        return ((int (*)(void *, double, double)) class->try_slew.method)(_self, ra, dec);
    } else {
        int result;
        forward(_self, &result, (Method) __telescope_try_slew, "try_slew", _self, ra, dec);
        return result;
    }
}

int
__telescope_timed_slew(void *_self, double ra, double dec, double timeout)
{
    const struct __TelescopeClass *class = (const struct __TelescopeClass *) classOf(_self);
    
    if (isOf(class, __TelescopeClass()) && class->timed_slew.method) {
        return ((int (*)(void *, double, double, double)) class->timed_slew.method)(_self, ra, dec, timeout);
    } else {
        int result;
        forward(_self, &result, (Method) __telescope_timed_slew, "timed_slew", _self, ra, dec, timeout);
        return result;
    }
}

int
__telescope_raw(void *_self, const void *command, size_t command_size, size_t *write_size, void *results, size_t results_size, size_t *return_size)
{
    const struct __TelescopeClass *class = (const struct __TelescopeClass *) classOf(_self);
    
    if (isOf(class, __TelescopeClass()) && class->raw.method) {
        return ((int (*)(void *, const void *, size_t, size_t *, void *, size_t, size_t *)) class->raw.method)(_self, command, command_size, write_size, results, results_size, return_size);
    } else {
        int result;
        forward(_self, &result, (Method) __telescope_raw, "raw", _self, command, command_size, write_size, results, results_size, return_size);
        return result;
    }
}

static int
__Telescope_raw(void *_self, const void *command, size_t command_size, size_t *write_size, void *results, size_t results_size, size_t *return_size)
{
    return AAOS_ENOTSUP;
}

int
__telescope_set_move_speed(void *_self, double move_speed)
{
    const struct __TelescopeClass *class = (const struct __TelescopeClass *) classOf(_self);
    
    if (isOf(class, __TelescopeClass()) && class->set_move_speed.method) {
        return ((int (*)(void *, double)) class->set_move_speed.method)(_self, move_speed);
    } else {
        int result;
        forward(_self, &result, (Method) __telescope_set_move_speed, "set_move_speed", _self, move_speed);
        return result;
    }
}

int
__telescope_get_move_speed(void *_self, double *move_speed)
{
    const struct __TelescopeClass *class = (const struct __TelescopeClass *) classOf(_self);
    
    if (isOf(class, __TelescopeClass()) && class->get_move_speed.method) {
        return ((int (*)(void *, double *)) class->get_slew_speed.method)(_self, move_speed);
    } else {
        int result;
        forward(_self, &result, (Method) __telescope_get_move_speed, "get_move_speed", _self, move_speed);
        return result;
    }
}

int
__telescope_set_slew_speed(void *_self, double slew_speed_x, double slew_speed_y)
{
    const struct __TelescopeClass *class = (const struct __TelescopeClass *) classOf(_self);
    
    if (isOf(class, __TelescopeClass()) && class->set_slew_speed.method) {
        return ((int (*)(void *, double, double)) class->set_slew_speed.method)(_self, slew_speed_x, slew_speed_y);
    } else {
        int result;
        forward(_self, &result, (Method) __telescope_set_slew_speed, "set_slew_speed", _self, slew_speed_x, slew_speed_y);
        return result;
    }
}

int
__telescope_get_slew_speed(void *_self, double *slew_speed_x, double *slew_speed_y)
{
    const struct __TelescopeClass *class = (const struct __TelescopeClass *) classOf(_self);
    
    if (isOf(class, __TelescopeClass()) && class->get_slew_speed.method) {
        return ((int (*)(void *, double *, double *)) class->set_slew_speed.method)(_self, slew_speed_x, slew_speed_y);
    } else {
        int result;
        forward(_self, &result, (Method) __telescope_get_slew_speed, "get_slew_speed", _self, slew_speed_x, slew_speed_y);
        return result;
    }
}

int
__telescope_set_track_rate(void *_self, double track_rate_x, double track_rate_y)
{
    const struct __TelescopeClass *class = (const struct __TelescopeClass *) classOf(_self);
    
    if (isOf(class, __TelescopeClass()) && class->set_track_rate.method) {
        return ((int (*)(void *, double, double)) class->set_track_rate.method)(_self, track_rate_x, track_rate_y);
    } else {
        int result;
        forward(_self, &result, (Method) __telescope_set_track_rate, "set_track_rate", _self, track_rate_x, track_rate_y);
        return result;
    }
}

int
__telescope_get_track_rate(void *_self, double *track_rate_x, double *track_rate_y)
{
    const struct __TelescopeClass *class = (const struct __TelescopeClass *) classOf(_self);
    
    if (isOf(class, __TelescopeClass()) && class->get_track_rate.method) {
        return ((int (*)(void *, double *, double *)) class->get_track_rate.method)(_self, track_rate_x, track_rate_y);
    } else {
        int result;
        forward(_self, &result, (Method) __telescope_get_track_rate, "get_track_rate", _self, track_rate_x, track_rate_y);
        return result;
    }
}

/*
 *
 */

static void
__Telescope_forward(const void *_self, void *result, Method selector, const char *name, va_list *app)
{
    const struct __Telescope *self = cast(__Telescope(), _self);
    Method method = virtualTo(self->_vtab, name);
    va_list ap;
    
#ifndef NDBUG
    if (!method) {
        fprintf(stderr, "__Telescope does not implement `%s` method.\n", name);
    }
#endif
    assert(method);
    
#ifdef va_copy
    va_copy(ap, *app);
#else
    ap = *app;
#endif
    
    void *obj = va_arg(*app, void *);
    if (selector == (Method) __telescope_status) {
        char *status_buffer = va_arg(*app, char *);
        size_t status_buffer_size = va_arg(*app, size_t);
        *((int *) result) = ((int (*)(void *, char *, size_t)) method)(obj, status_buffer, status_buffer_size);
    } else if (selector == (Method) __telescope_power_on) {
        *((int *) result) = ((int (*)(void *)) method)(obj);
    } else if (selector == (Method) __telescope_power_off) {
       *((int *) result) = ((int (*)(void *)) method)(obj);
    } else if (selector == (Method) __telescope_init) {
        *((int *) result) = ((int (*)(void *)) method)(obj);
    } else if (selector == (Method) __telescope_park) {
        *((int *) result) = ((int (*)(void *)) method)(obj);
    } else if (selector == (Method) __telescope_park_off) {
        *((int *) result) = ((int (*)(void *)) method)(obj);
    } else if (selector == (Method) __telescope_stop) {
        *((int *) result) = ((int (*)(void *)) method)(obj);
    } else if (selector == (Method) __telescope_go_home) {
        *((int *) result) = ((int (*)(void *)) method)(obj);
    } else if (selector == (Method) __telescope_move) {
        unsigned int direction = va_arg(*app, unsigned int);
        double duration = va_arg(*app, double);
        *((int *) result) = ((int (*)(void *, unsigned int, double)) method)(obj, direction, duration);
    } else if (selector == (Method) __telescope_try_move) {
        unsigned int direction = va_arg(*app, unsigned int);
        double duration = va_arg(*app, double);
        *((int *) result) = ((int (*)(void *, unsigned int, double)) method)(obj, direction, duration);
    } else if (selector == (Method) __telescope_timed_move) {
        unsigned int direction = va_arg(*app, unsigned int);
        double duration = va_arg(*app, double);
        double timeout = va_arg(*app, double);
        *((int *) result) = ((int (*)(void *, unsigned int, double, double)) method)(obj, direction, duration, timeout);
    } else if (selector == (Method) __telescope_slew) {
        double ra = va_arg(*app, double);
        double dec = va_arg(*app, double);
        *((int *) result) = ((int (*)(void *, double, double)) method)(obj, ra, dec);
    } else if (selector == (Method) __telescope_try_slew) {
        double ra = va_arg(*app, double);
        double dec = va_arg(*app, double);
        *((int *) result) = ((int (*)(void *, double, double)) method)(obj, ra, dec);
    } else if (selector == (Method) __telescope_timed_slew) {
        double ra = va_arg(*app, double);
        double dec = va_arg(*app, double);
        double timeout = va_arg(*app, double);
        *((int *) result) = ((int (*)(void *, double, double, double)) method)(obj, ra, dec, timeout);
    } else if (selector == (Method) __telescope_raw) {
        const void *command = va_arg(*app, const void *);
        size_t command_size = va_arg(*app, size_t);
        size_t *write_size = va_arg(*app, size_t *);
        void *results = va_arg(*app, void *);
        size_t results_size = va_arg(*app, size_t);
        size_t *return_size = va_arg(*app, size_t *);
        *((int *) result) = ((int (*)(void *, const void *, size_t, size_t *, void *, size_t, size_t *)) method)(obj, command, command_size, write_size, results, results_size, return_size);
    } else if (selector == (Method) __telescope_set_move_speed) {
        double move_speed = va_arg(*app, double);
        *((int *) result) = ((int (*)(void *, double)) method)(obj, move_speed);
    } else if (selector == (Method) __telescope_get_move_speed) {
        double *move_speed = va_arg(*app, double *);
        *((int *) result) = ((int (*)(void *, double *)) method)(obj, move_speed);
    } else if (selector == (Method) __telescope_set_slew_speed) {
        double slew_speed_x = va_arg(*app, double);
        double slew_speed_y = va_arg(*app, double);
        *((int *) result) = ((int (*)(void *, double, double)) method)(obj, slew_speed_x, slew_speed_y);
    } else if (selector == (Method) __telescope_get_slew_speed) {
        double *slew_speed_x = va_arg(*app, double *);
        double *slew_speed_y = va_arg(*app, double *);
        *((int *) result) = ((int (*)(void *, double *, double *)) method)(obj, slew_speed_x, slew_speed_y);
    } else if (selector == (Method) __telescope_set_track_rate) {
        double track_rate_x = va_arg(*app, double);
        double track_rate_y = va_arg(*app, double);
        *((int *) result) = ((int (*)(void *, double, double)) method)(obj, track_rate_x, track_rate_y);
    } else if (selector == (Method) __telescope_get_track_rate) {
        double *track_rate_x = va_arg(*app, double *);
        double *track_rate_y = va_arg(*app, double *);
        *((int *) result) = ((int (*)(void *, double *, double *)) method)(obj, track_rate_x, track_rate_y);
    } else {
        assert(0);
    }
    
#ifdef va_copy
    va_end(ap);
#endif
}

static int
__Telescope_puto(const void *_self, FILE *fp)
{
    struct __Telescope *self = cast(__Telescope(), _self);
    
    int state;
    double ra, dec;
    
    Pthread_mutex_lock(&self->t_state.mtx);
    state = self->t_state.state;
    ra = self->ra;
    dec = self->dec;
    Pthread_mutex_unlock(&self->t_state.mtx);
    
    if (self->name) {
        fprintf(fp, "Name       \t: %s\n", self->name);
    } else {
        fprintf(fp, "Name       \t:NULL\n");
        
    }
    if (self->description) {
        fprintf(fp, "Description\t: %s\n", self->description);
    } else {
        fprintf(fp, "Description\t: NULL\n");
        
    }
    switch (state) {
        case TELESCOPE_STATE_POWERED_OFF:
            fprintf(fp, "State      \t: powered off\n");
            break;
        case TELESCOPE_STATE_UNINITIALIZED:
            fprintf(fp, "State      \t: uninitialized\n");
            break;
        case TELESCOPE_STATE_PARKED:
            fprintf(fp, "State      \t: parked\n");
            break;
        case TELESCOPE_STATE_TRACKING:
            fprintf(fp, "State      \t: tracking\n");
            break;
        case TELESCOPE_STATE_MOVING:
            fprintf(fp, "State      \t: moving\n");
            break;
        case TELESCOPE_STATE_SLEWING:
            fprintf(fp, "State      \t: slewing\n");
            break;
        case TELESCOPE_STATE_MALFUNCTION:
            fprintf(fp, "State      \t: malfunction\n");
            break;
        default:
            fprintf(fp, "State      \t: unknown\n");
            
            break;
    }
    char buf[BUFSIZE];
    deg2hms(ra, buf, BUFSIZE, "%HH%MM%04.1SS");
    fprintf(fp, "RA         \t: %s\n", buf);
    deg2dms(dec, buf, BUFSIZE, "%DD%MM%04.1SS");
    fprintf(fp, "DEC        \t: %s", buf);
    return AAOS_OK;
}

static void *
__Telescope_ctor(void *_self, va_list *app)
{
    struct __Telescope *self = super_ctor(__Telescope(), _self, app);
    const char *s, *name, *value;
    
    s = va_arg(*app, const char *);
    self->name = (char *) Malloc(strlen(s) + 1);
    snprintf(self->name, strlen(s) + 1, "%s", s);
    
   
    while ((name = va_arg(*app, const char *))) {
        if (strcmp(name, "description") == 0) {
            value = va_arg(*app, const char *);
            if (value) {
                self->description = (char *) Malloc(strlen(value) + 1);
                snprintf(self->description, strlen(value) + 1, "%s", value);
            }
            continue;
        }
        if (strcmp(name, "longitude") == 0) {
            double value;
            value = va_arg(*app, double);
            self->location_lon = value;
            continue;
        }
        if (strcmp(name, "latitude") == 0) {
            double value;
            value = va_arg(*app, double);
            self->location_lat = value;
            continue;
        }
        if (strcmp(name, "gmt_offset") == 0) {
            double value;
            value = va_arg(*app, double);
            self->gmt_offset = value;
            continue;
        }
        
    }
    
    TelescopeState_init(&self->t_state);
    
    return (void *) self;
}

static void *
__Telescope_dtor(void *_self)
{
    struct __Telescope *self = cast(__Telescope(), _self);
    
    free(self->description);
    free(self->name);
    TelescopeState_destroy(&self->t_state);
    
    return super_dtor(__Telescope(), _self);
}

static void *
__TelescopeClass_ctor(void *_self, va_list *app)
{
    struct __TelescopeClass *self = super_ctor(__TelescopeClass(), _self, app);
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
        
        if (selector == (Method) __telescope_release) {
            if (tag) {
                self->release.tag = tag;
                self->release.selector = selector;
            }
            self->release.method = method;
            continue;
        }
        if (selector == (Method) __telescope_get_name) {
            if (tag) {
                self->get_name.tag = tag;
                self->get_name.selector = selector;
            }
            self->get_name.method = method;
            continue;
        }
        
        if (selector == (Method) __telescope_status) {
            if (tag) {
                self->status.tag = tag;
                self->status.selector = selector;
            }
            self->slew.method = method;
            continue;
        }
        if (selector == (Method) __telescope_power_on) {
            if (tag) {
                self->power_on.tag = tag;
                self->power_on.selector = selector;
            }
            self->power_on.method = method;
            continue;
        }
        if (selector == (Method) __telescope_power_off) {
            if (tag) {
                self->power_off.tag = tag;
                self->power_off.selector = selector;
            }
            self->power_off.method = method;
            continue;
        }
        if (selector == (Method) __telescope_init) {
            if (tag) {
                self->init.tag = tag;
                self->init.selector = selector;
            }
            self->init.method = method;
            continue;
        }
        if (selector == (Method) __telescope_park) {
            if (tag) {
                self->park.tag = tag;
                self->park.selector = selector;
            }
            self->park.method = method;
            continue;
        }
        if (selector == (Method) __telescope_park_off) {
            if (tag) {
                self->park_off.tag = tag;
                self->park_off.selector = selector;
            }
            self->park_off.method = method;
            continue;
        }
        if (selector == (Method) __telescope_stop) {
            if (tag) {
                self->stop.tag = tag;
                self->stop.selector = selector;
            }
            self->stop.method = method;
            continue;
        }
        if (selector == (Method) __telescope_go_home) {
            if (tag) {
                self->go_home.tag = tag;
                self->go_home.selector = selector;
            }
            self->go_home.method = method;
            continue;
        }
        if (selector == (Method) __telescope_move) {
            if (tag) {
                self->move.tag = tag;
                self->move.selector = selector;
            }
            self->move.method = method;
            continue;
        }
        if (selector == (Method) __telescope_try_move) {
            if (tag) {
                self->try_move.tag = tag;
                self->try_move.selector = selector;
            }
            self->try_move.method = method;
            continue;
        }
        if (selector == (Method) __telescope_timed_move) {
            if (tag) {
                self->timed_move.tag = tag;
                self->timed_move.selector = selector;
            }
            self->timed_move.method = method;
            continue;
        }
        if (selector == (Method) __telescope_slew) {
            if (tag) {
                self->slew.tag = tag;
                self->slew.selector = selector;
            }
            self->slew.method = method;
            continue;
        }
        if (selector == (Method) __telescope_try_slew) {
            if (tag) {
                self->try_slew.tag = tag;
                self->try_slew.selector = selector;
            }
            self->try_slew.method = method;
            continue;
        }
        if (selector == (Method) __telescope_timed_slew) {
            if (tag) {
                self->timed_slew.tag = tag;
                self->timed_slew.selector = selector;
            }
            self->timed_slew.method = method;
            continue;
        }
        if (selector == (Method) __telescope_raw) {
            if (tag) {
                self->raw.tag = tag;
                self->raw.selector = selector;
            }
            self->raw.method = method;
            continue;
        }
        if (selector == (Method) __telescope_set_move_speed) {
            if (tag) {
                self->set_move_speed.tag = tag;
                self->set_move_speed.selector = selector;
            }
            self->set_move_speed.method = method;
            continue;
        }
        if (selector == (Method) __telescope_get_move_speed) {
            if (tag) {
                self->get_move_speed.tag = tag;
                self->get_move_speed.selector = selector;
            }
            self->get_move_speed.method = method;
            continue;
        }
        if (selector == (Method) __telescope_set_slew_speed) {
            if (tag) {
                self->set_slew_speed.tag = tag;
                self->set_slew_speed.selector = selector;
            }
            self->set_slew_speed.method = method;
            continue;
        }
        if (selector == (Method) __telescope_get_slew_speed) {
            if (tag) {
                self->get_slew_speed.tag = tag;
                self->get_slew_speed.selector = selector;
            }
            self->get_slew_speed.method = method;
            continue;
        }
        if (selector == (Method) __telescope_set_track_rate) {
            if (tag) {
                self->set_track_rate.tag = tag;
                self->set_track_rate.selector = selector;
            }
            self->set_slew_speed.method = method;
            continue;
        }
        if (selector == (Method) __telescope_get_track_rate) {
            if (tag) {
                self->get_track_rate.tag = tag;
                self->get_track_rate.selector = selector;
            }
            self->get_track_rate.method = method;
            continue;
        }
        if (selector == (Method) __telescope_set_option) {
            if (tag) {
                self->set_option.tag = tag;
                self->set_option.selector = selector;
            }
            self->set_option.method = method;
            continue;
        }
    }
    
#ifdef va_copy
    va_end(ap);
#endif
    
    return (void *) self;     
}

static void *___TelescopeClass;

static void
__TelescopeClass_destroy(void)
{
    free((void *) ___TelescopeClass);
    pthread_key_delete(__telescope_option_key);
}

static void
__TelescopeClass_initialize(void)
{
    ___TelescopeClass = new(Class(), "__TelescopeClass", Class(), sizeof(struct __TelescopeClass),
                            ctor, "ctor", __TelescopeClass_ctor,
                            (void *) 0);
    pthread_key_create(&__telescope_option_key, free);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(__TelescopeClass_destroy);
#endif
}

const void *
__TelescopeClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, __TelescopeClass_initialize);
#endif
    
    return ___TelescopeClass;
}

static void *___Telescope;

static void
__Telescope_destroy(void)
{
    free((void *) ___Telescope);
}

static void
__Telescope_initialize(void)
{
    ___Telescope = new(__TelescopeClass(), "__Telescope", Object(), sizeof(struct __Telescope),
                       ctor, "ctor", __Telescope_ctor,
                       dtor, "dtor", __Telescope_dtor,
                       puto, "puto", __Telescope_puto,
                       forward, "forward", __Telescope_forward,
                       
                       __telescope_release, "release", __Telescope_release,
                       __telescope_get_name, "get_name", __Telescope_get_name,
                       __telescope_set_option, "set_option", __Telescope_set_option,
                       __telescope_raw, "raw", __Telescope_raw,
                       (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(__Telescope_destroy);
#endif
}

const void *
__Telescope(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, __Telescope_initialize);
#endif
    
    return ___Telescope;
}

/*
 * Virtual telescope
 */

#define SIDEREAL_TRACKING_SPEED (15./3600.)

static const void *virtual_telescope_virtual_table(void);

static double
get_current_time(void)
{
    struct timespec tp;
    
    Clock_gettime(CLOCK_REALTIME, &tp);
    
    return tp.tv_sec + tp.tv_nsec / 1000000000.;
}

static void
VirtualTelescope_get_current_postion(struct VirtualTelescope *self)
{
    unsigned int state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
    double current_time = get_current_time();
       
    double ra_arrive_time, dec_arrive_time, time_diff;
    switch (state) {
        case TELESCOPE_STATE_PARKED:
            if (self->_.dec < 90. && self->_.dec > -90.) {
                self->_.ra += SIDEREAL_TRACKING_SPEED * (current_time - self->_.last_park_begin_time);
                if (self->_.ra >= 360.) {
                    self->_.ra -= floor(fabs(self->_.ra) / 360.)  * 360.;
                } else if (self->_.ra < 0.) {
                    self->_.ra += (floor(fabs(self->_.ra) / 360.) + 1) * 360.;
                }
            }
            break;
        case TELESCOPE_STATE_MOVING:
            switch (self->_.move_direction) {
                case TELESCOPE_MOVE_EAST:
                    self->_.ra += (self->_.move_speed - SIDEREAL_TRACKING_SPEED) * (current_time - self->_.last_move_begin_time);
                    if (self->_.ra >= 360.) {
                        self->_.ra -= (floor(fabs(self->_.ra) / 360.) - 1) * 360.;
                    }
                    break;
                case TELESCOPE_MOVE_WEST:
                    self->_.ra -= (self->_.move_speed + SIDEREAL_TRACKING_SPEED) * (current_time - self->_.last_move_begin_time);
                    if (self->_.ra < 0.) {
                        self->_.ra += (floor(fabs(self->_.ra) / 360.) + 1) * 360.;
                    }
                    break;
                case TELESCOPE_MOVE_NORTH:
                    self->_.dec += self->_.move_speed * (current_time - self->_.last_move_begin_time);
                    if (self->_.dec > 90.) {
                        self->_.dec -= floor(fabs(self->_.dec) / 360.) * 360.;
                        if (self->_.dec > 90. && self->_.dec <= 270.) {
                            self->_.dec = 180. - self->_.dec;
                            self->_.ra += 180.;
                            if (self->_.ra >= 360.) {
                                self->_.ra -= 360.;
                            }
                        } else if (self->_.dec > 270.) {
                            self->_.dec *= 360. - self->_.dec;
                        }
                    }
                    break;
                case TELESCOPE_MOVE_SOUTH:
                    self->_.dec -= self->_.move_speed * (current_time - self->_.last_move_begin_time);
                    if (self->_.dec < -90.) {
                        self->_.dec += floor(fabs(self->_.dec) / 360. + 1) * 360.;
                        if (self->_.dec > 90. && self->_.dec <= 270.) {
                            self->_.dec = 180. - self->_.dec;
                            self->_.ra += 180.;
                            if (self->_.ra >= 360.) {
                                self->_.ra -= 360.;
                            }
                        } else if (self->_.dec > 270.) {
                            self->_.dec *= 360. - self->_.dec;
                        }
                    }
                    break;
                default:
                    break;
            }
            break;
        case TELESCOPE_STATE_SLEWING:
            time_diff = current_time - self->_.last_slew_begin_time;
            dec_arrive_time = fabs(self->_.dec_to - self->_.dec_from) / self->_.slew_speed_y;
            if (time_diff < dec_arrive_time) {
                self->_.dec += self->_.slew_direction_y * self->_.slew_speed_y;
            } else {
                self->_.dec = self->_.dec_to;
            }
            if (fabs(self->_.ra_to - self->_.ra_from) > 180.) {
                ra_arrive_time = (360. - fabs(self->_.ra_to - self->_.ra_from)) / self->_.slew_speed_x;
            } else {
                ra_arrive_time = fabs(self->_.ra_to - self->_.ra_from) / self->_.slew_speed_x;
            }
            if (time_diff < ra_arrive_time) {
                self->_.ra += self->_.slew_direction_x * (self->_.slew_speed_x - self->_.slew_direction_x * SIDEREAL_TRACKING_SPEED);
                if (self->_.ra > 360.) {
                    self->_.ra -= 360.;
                } else if (self->_.ra < 0.) {
                    self->_.ra= 360. + self->_.ra;
                }
            } else {
                self->_.ra = self->_.ra_to;
            }
            break;
        default:
            break;
    }
    
    //double jul_d = jd(current_time);

    //radec2altaz(jul_d, self->_.ra, self->_.dec, self->_.location_lon, self->_.location_lat, self->_.location_ele, -1., -300., &self->_.alt, &self->_.az, NULL);
}

static void *
motor_thr(void *arg)
{
    double sleep_time = *((double *) arg);
    struct timespec tp;
    
    tp.tv_sec = floor(sleep_time);
    tp.tv_nsec = (sleep_time - tp.tv_sec) * 1000000000;
    nanosleep(&tp, NULL);
    
    return NULL;
}

static int
VirtualTelescope_status(void *_self, char *status_buffer, size_t status_buffer_size)
{
    FILE *fp;
    
    VirtualTelescope_get_current_postion(_self);
       
    if ((fp = fmemopen(status_buffer, status_buffer_size, "w")) == NULL) {
    }
    
    puto(_self, fp);
    
    fclose(fp);
    
    return AAOS_OK;
}

static int
VirtualTelescope_power_on(void *_self)
{
    struct VirtualTelescope *self = cast(VirtualTelescope(), _self);
    
    Pthread_mutex_lock(&self->_.t_state.mtx);
    unsigned int state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
    unsigned int flag = state & TELESCOPE_STATE_MALFUNCTION;
    if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        return AAOS_EDEVMAL;
    }
   
    switch (state) {
        case TELESCOPE_STATE_POWERED_OFF:
            self->_.t_state.state = TELESCOPE_STATE_UNINITIALIZED | flag;
            break;
        default:
            break;
    }
    Pthread_mutex_unlock(&self->_.t_state.mtx);
    
    return AAOS_OK;
}

static int
VirtualTelescope_power_off(void *_self)
{
    struct VirtualTelescope *self = cast(VirtualTelescope(), _self);
    
    Pthread_mutex_lock(&self->_.t_state.mtx);
    unsigned int state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
    unsigned int flag = state & TELESCOPE_STATE_MALFUNCTION;
    if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        return AAOS_EDEVMAL;
    }
    switch (state) {
        case TELESCOPE_STATE_MOVING:
        case TELESCOPE_STATE_SLEWING:
            Pthread_cancel(self->_.tid);
            break;
        default:
            break;
    }
    self->_.t_state.state = TELESCOPE_STATE_POWERED_OFF | flag;
    Pthread_mutex_unlock(&self->_.t_state.mtx);
    
    return AAOS_OK;
}

static int
VirtualTelescope_init(void *_self)
{
    struct VirtualTelescope *self = cast(VirtualTelescope(), _self);
    
    self->_.ra = 0.0000001;
    self->_.dec = 90.;
    
    self->_.track_rate_x = SIDEREAL_TRACKING_SPEED;
    self->_.track_rate_y = 0.;
    self->_.slew_speed_x = SIDEREAL_TRACKING_SPEED * 1200.;
    self->_.slew_speed_y = SIDEREAL_TRACKING_SPEED * 1200.;
    self->_.move_speed = SIDEREAL_TRACKING_SPEED * 1200.;
    
    Pthread_mutex_lock(&self->_.t_state.mtx);
    unsigned int state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
    unsigned int flag = state & TELESCOPE_STATE_MALFUNCTION;
    if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        return AAOS_EDEVMAL;
    }
    switch (state) {
        case TELESCOPE_STATE_UNINITIALIZED:
            self->_.t_state.state = TELESCOPE_STATE_PARKED | flag;
            break;
        default:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EALREADY;
            break;
    }
    Pthread_mutex_unlock(&self->_.t_state.mtx);
    
    return AAOS_OK;
}

static int
VirtualTelescope_stop(void *_self)
{
    struct VirtualTelescope *self = cast(VirtualTelescope(), _self);
    
    Pthread_mutex_lock(&self->_.t_state.mtx);
    unsigned int state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
    unsigned int flag = self->_.t_state.state & TELESCOPE_STATE_MALFUNCTION;
    if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        return AAOS_EDEVMAL;
    }
    VirtualTelescope_get_current_postion(self);
    switch (state) {
        case TELESCOPE_STATE_UNINITIALIZED:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EUNINIT;
            break;
        case TELESCOPE_STATE_POWERED_OFF:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EPWROFF;
            break;
        case TELESCOPE_STATE_PARKED:
        case TELESCOPE_STATE_TRACKING:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_OK;
            break;
        case TELESCOPE_STATE_MOVING:
        case TELESCOPE_STATE_SLEWING:
        case TELESCOPE_STATE_TRACKING_WAIT:
            Pthread_cancel(self->_.tid);
            self->_.t_state.state = TELESCOPE_STATE_TRACKING | flag;
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            Pthread_cond_broadcast(&self->_.t_state.cond);
            return AAOS_OK;
            break;
        default:
            break;
    }
    
    return AAOS_OK;
}

static int
VirtualTelescope_move(void *_self, unsigned int direction, double duration)
{
    struct VirtualTelescope *self = cast(VirtualTelescope(), _self);
    void *value = NULL;
    
    if (direction != TELESCOPE_MOVE_EAST || direction != TELESCOPE_MOVE_WEST || direction != TELESCOPE_MOVE_NORTH || direction != TELESCOPE_MOVE_SOUTH || duration <= 0.) {
        return AAOS_EINVAL;
    }
    
    Pthread_mutex_lock(&self->_.t_state.mtx);
    unsigned int state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
    unsigned int flag = self->_.t_state.state & TELESCOPE_STATE_MALFUNCTION;
    if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        return AAOS_EDEVMAL;
    }
    switch (state) {
        case TELESCOPE_STATE_SLEWING:
        case TELESCOPE_STATE_MOVING:
        case TELESCOPE_STATE_TRACKING_WAIT:
            if (self->_.option & TELESCOPE_OPTION_DO_NOT_WAIT && state != TELESCOPE_STATE_TRACKING_WAIT) {
                Pthread_cancel(self->_.tid);
                break;
            }
            while (state == TELESCOPE_STATE_MOVING || state == TELESCOPE_STATE_SLEWING || state == TELESCOPE_STATE_TRACKING_WAIT) {
                Pthread_cond_wait(&self->_.t_state.cond, &self->_.t_state.mtx);
                state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
                flag = state & TELESCOPE_STATE_MALFUNCTION;
            }
            if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
                Pthread_mutex_unlock(&self->_.t_state.mtx);
                return AAOS_EDEVMAL;
            }
            switch (state) {
                case TELESCOPE_STATE_POWERED_OFF:
                    Pthread_mutex_unlock(&self->_.t_state.mtx);
                    return AAOS_EPWROFF;
                    break;
                case TELESCOPE_STATE_UNINITIALIZED:
                    Pthread_mutex_unlock(&self->_.t_state.mtx);
                    return AAOS_EUNINIT;
                    break;
                case TELESCOPE_STATE_PARKED:
                    VirtualTelescope_get_current_postion(self);
                    break;
                case TELESCOPE_STATE_TRACKING:
                    break;
                default:
                    break;
            }
            break;
        case TELESCOPE_STATE_POWERED_OFF:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EPWROFF;
            break;
        case TELESCOPE_STATE_MALFUNCTION:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EDEVMAL;
            break;
        case TELESCOPE_STATE_UNINITIALIZED:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EUNINIT;
            break;
        case TELESCOPE_STATE_PARKED:
            break;
        case TELESCOPE_STATE_TRACKING:
            break;
        default:
            break;
    }
    self->_.last_move_begin_time = get_current_time();
    self->_.move_direction = direction;
    self->_.t_state.state = TELESCOPE_STATE_MOVING | flag;
    Pthread_create(&self->_.tid, NULL, motor_thr, &duration);
    Pthread_mutex_unlock(&self->_.t_state.mtx);
    
    Pthread_join(self->_.tid, &value);
    if (value == PTHREAD_CANCELED) {
        return AAOS_ECANCELED;
    }
    /*
     * sleeping, update ra and dec, update current position, last tracking begin time.
     */
    Pthread_mutex_lock(&self->_.t_state.mtx);
    self->_.last_track_begin_time = get_current_time();
    VirtualTelescope_get_current_postion(self);
    self->_.t_state.state = TELESCOPE_STATE_TRACKING | flag;
    Pthread_mutex_unlock(&self->_.t_state.mtx);
    Pthread_cond_broadcast(&self->_.t_state.cond);
    
    return AAOS_OK;
}

static int
VirtualTelescope_try_move(void *_self, unsigned int direction, double duration)
{
    struct VirtualTelescope *self = cast(VirtualTelescope(), _self);
    void *value = NULL;
    
    if (direction != TELESCOPE_MOVE_EAST || direction != TELESCOPE_MOVE_WEST || direction != TELESCOPE_MOVE_NORTH || direction != TELESCOPE_MOVE_SOUTH || duration <= 0.) {
        return AAOS_EINVAL;
    }
    
    if (Pthread_mutex_trylock(&self->_.t_state.mtx) != 0) {
        return AAOS_EBUSY;
    }
    unsigned int state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
    unsigned int flag = self->_.t_state.state & TELESCOPE_STATE_MALFUNCTION;
    if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        return AAOS_EDEVMAL;
    }
    switch (state) {
        case TELESCOPE_STATE_SLEWING:
        case TELESCOPE_STATE_MOVING:
        case TELESCOPE_STATE_TRACKING_WAIT:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EBUSY;
            break;
        case TELESCOPE_STATE_POWERED_OFF:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EPWROFF;
            break;
        case TELESCOPE_STATE_UNINITIALIZED:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EUNINIT;
            break;
        case TELESCOPE_STATE_PARKED:
            break;
        case TELESCOPE_STATE_TRACKING:
            break;
        default:
            break;
    }
    self->_.last_move_begin_time = get_current_time();
    self->_.move_direction = direction;
    self->_.t_state.state = TELESCOPE_STATE_MOVING | flag;
    Pthread_create(&self->_.tid, NULL, motor_thr, &duration);
    Pthread_mutex_unlock(&self->_.t_state.mtx);
    
    Pthread_join(self->_.tid, &value);
    if (value == PTHREAD_CANCELED) {
        return AAOS_ECANCELED;
    }
    /*
     * sleeping, update ra and dec, update current position, last tracking begin time.
     */
    Pthread_mutex_lock(&self->_.t_state.mtx);
    self->_.last_track_begin_time = get_current_time();
    VirtualTelescope_get_current_postion(self);
    self->_.t_state.state = TELESCOPE_STATE_TRACKING | flag;
    Pthread_mutex_unlock(&self->_.t_state.mtx);
    Pthread_cond_broadcast(&self->_.t_state.cond);
    
    return AAOS_OK;
}

static int
VirtualTelescope_timed_move(void *_self, unsigned int direction, double duration, double timeout)
{
    struct VirtualTelescope *self = cast(VirtualTelescope(), _self);
    void *value = NULL;
    struct timespec tp;
    int ret;
    
    if (direction != TELESCOPE_MOVE_EAST || direction != TELESCOPE_MOVE_WEST || direction != TELESCOPE_MOVE_NORTH || direction != TELESCOPE_MOVE_SOUTH || duration <= 0. || timeout <= 0.) {
        return AAOS_EINVAL;
    }
    
    timeout += get_current_time();
    tp.tv_sec = floor(timeout);
    tp.tv_nsec = (timeout - tp.tv_sec) * 1000000000.;
    
    Pthread_mutex_lock(&self->_.t_state.mtx);
    unsigned int state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
    unsigned int flag = self->_.t_state.state & TELESCOPE_STATE_MALFUNCTION;
    if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        return AAOS_EDEVMAL;
    }
    switch (state) {
        case TELESCOPE_STATE_SLEWING:
        case TELESCOPE_STATE_MOVING:
        case TELESCOPE_STATE_TRACKING_WAIT:
            while (state == TELESCOPE_STATE_MOVING || state == TELESCOPE_STATE_SLEWING || state == TELESCOPE_STATE_TRACKING_WAIT) {
                ret = Pthread_cond_timedwait(&self->_.t_state.cond, &self->_.t_state.mtx, &tp);
                if (ret == ETIMEDOUT) {
                    Pthread_mutex_unlock(&self->_.t_state.mtx);
                    return AAOS_ETIMEDOUT;
                }
                state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
                flag = state & TELESCOPE_STATE_MALFUNCTION;
            }
            if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
                Pthread_mutex_unlock(&self->_.t_state.mtx);
                return AAOS_EDEVMAL;
            }
            switch (state) {
                case TELESCOPE_STATE_POWERED_OFF:
                    Pthread_mutex_unlock(&self->_.t_state.mtx);
                    return AAOS_EPWROFF;
                    break;
                case TELESCOPE_STATE_UNINITIALIZED:
                    Pthread_mutex_unlock(&self->_.t_state.mtx);
                    return AAOS_EUNINIT;
                    break;
                case TELESCOPE_STATE_PARKED:
                    VirtualTelescope_get_current_postion(self);
                    break;
                case TELESCOPE_STATE_TRACKING:
                    break;
                default:
                    break;
            }
            break;
        case TELESCOPE_STATE_POWERED_OFF:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EPWROFF;
            break;
        case TELESCOPE_STATE_MALFUNCTION:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EDEVMAL;
            break;
        case TELESCOPE_STATE_UNINITIALIZED:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EUNINIT;
            break;
        case TELESCOPE_STATE_PARKED:
            break;
        case TELESCOPE_STATE_TRACKING:
            break;
        default:
            break;
    }
    self->_.last_move_begin_time = get_current_time();
    self->_.move_direction = direction;
    self->_.t_state.state = TELESCOPE_STATE_MOVING | flag;
    Pthread_create(&self->_.tid, NULL, motor_thr, &duration);
    Pthread_mutex_unlock(&self->_.t_state.mtx);
    
    Pthread_join(self->_.tid, &value);
    if (value == PTHREAD_CANCELED) {
        return AAOS_ECANCELED;
    }
    /*
     * sleeping, update ra and dec, update current position, last tracking begin time.
     */
    Pthread_mutex_lock(&self->_.t_state.mtx);
    self->_.last_track_begin_time = get_current_time();
    VirtualTelescope_get_current_postion(self);
    self->_.t_state.state = TELESCOPE_STATE_TRACKING | flag;
    Pthread_mutex_unlock(&self->_.t_state.mtx);
    Pthread_cond_broadcast(&self->_.t_state.cond);
    
    return AAOS_OK;
}

static int
VirtualTelescope_slew(void *_self, double ra, double dec)
{
    struct VirtualTelescope *self = cast(VirtualTelescope(), _self);
    
    double time_diff_ra, time_diff_dec, time_diff, ra_diff;
    void *value = NULL;
    
    Pthread_mutex_lock(&self->_.t_state.mtx);
    unsigned int state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
    unsigned int flag = self->_.t_state.state & TELESCOPE_STATE_MALFUNCTION;
    if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        return AAOS_EDEVMAL;
    }
    switch (state) {
        case TELESCOPE_STATE_SLEWING:
        case TELESCOPE_STATE_MOVING:
        case TELESCOPE_STATE_TRACKING_WAIT:
            if (self->_.option & TELESCOPE_OPTION_DO_NOT_WAIT && state != TELESCOPE_STATE_TRACKING_WAIT) {
                Pthread_cancel(self->_.tid);
                break;
            }
            while (state == TELESCOPE_STATE_MOVING || state == TELESCOPE_STATE_SLEWING || state == TELESCOPE_STATE_TRACKING_WAIT) {
                Pthread_cond_wait(&self->_.t_state.cond, &self->_.t_state.mtx);
                state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
                flag = state & TELESCOPE_STATE_MALFUNCTION;
            }
            if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
                Pthread_mutex_unlock(&self->_.t_state.mtx);
                return AAOS_EDEVMAL;
            }
            switch (state) {
                case TELESCOPE_STATE_POWERED_OFF:
                    Pthread_mutex_unlock(&self->_.t_state.mtx);
                    return AAOS_EPWROFF;
                    break;
                case TELESCOPE_STATE_MALFUNCTION:
                    Pthread_mutex_unlock(&self->_.t_state.mtx);
                    return AAOS_EDEVMAL;
                    break;
                case TELESCOPE_STATE_UNINITIALIZED:
                    Pthread_mutex_unlock(&self->_.t_state.mtx);
                    return AAOS_EUNINIT;
                    break;
                case TELESCOPE_STATE_PARKED:
                    VirtualTelescope_get_current_postion(self);
                    break;
                case TELESCOPE_STATE_TRACKING:
                    break;
                default:
                    break;
            }
            break;
        case TELESCOPE_STATE_POWERED_OFF:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EPWROFF;
            break;
        case TELESCOPE_STATE_MALFUNCTION:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EDEVMAL;
            break;
        case TELESCOPE_STATE_UNINITIALIZED:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EUNINIT;
            break;
        case TELESCOPE_STATE_PARKED:
            break;
        case TELESCOPE_STATE_TRACKING:
            break;
        default:
            break;
    }
    VirtualTelescope_get_current_postion(self);
    self->_.ra_from = self->_.ra;
    self->_.dec_from = self->_.dec;
    self->_.t_state.state = TELESCOPE_STATE_SLEWING;
    self->_.ra_to = ra;
    self->_.dec_to = dec;
    if ((self->_.ra_to - self->_.ra_from > 0. && self->_.ra_to - self->_.ra_from  <= 180.) || (self->_.ra_to - self->_.ra_from  <= -180.)) {
        self->_.slew_direction_x = 1;
    } else {
        self->_.slew_direction_x = -1;
    }
    self->_.slew_direction_y = (self->_.dec_to > self->_.dec_from) ? 1 : -1;
    self->_.last_slew_begin_time = get_current_time();
    ra_diff = fabs(self->_.ra_to - self->_.ra_from);
    ra_diff = ra_diff < 180. ? ra_diff : 360. - ra_diff;
    time_diff_ra = ra_diff / (self->_.slew_speed_x - self->_.slew_speed_x * SIDEREAL_TRACKING_SPEED);
    time_diff_dec = fabs(self->_.dec_to - fabs(self->_.dec_from)) / self->_.slew_speed_y;
    time_diff = max(time_diff_ra, time_diff_dec);
    Pthread_create(&self->_.tid, NULL, motor_thr, &time_diff);
    Pthread_mutex_unlock(&self->_.t_state.mtx);
    
    Pthread_join(self->_.tid, &value);
    if (value == PTHREAD_CANCELED) {
        return AAOS_ECANCELED;
    }
    /*
     * sleeping, update ra and dec, update last_park_off time 
     */
    Pthread_mutex_lock(&self->_.t_state.mtx);
    self->_.t_state.state = TELESCOPE_STATE_TRACKING | flag;
    self->_.ra = ra;
    self->_.dec = dec;
    self->_.last_track_begin_time = get_current_time();
    Pthread_mutex_unlock(&self->_.t_state.mtx);
    Pthread_cond_broadcast(&self->_.t_state.cond);
    
    return AAOS_OK;
}

static int
VirtualTelescope_try_slew(void *_self, double ra, double dec)
{
    struct VirtualTelescope *self = cast(VirtualTelescope(), _self);
    
    double time_diff_ra, time_diff_dec, time_diff, ra_diff;
    void *value = NULL;
    
    if (Pthread_mutex_trylock(&self->_.t_state.mtx) != 0) {
        return AAOS_EBUSY;
    }
    int state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
    int flag = self->_.t_state.state & TELESCOPE_STATE_MALFUNCTION;
    if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        return AAOS_EDEVMAL;
    }
    switch (state) {
        case TELESCOPE_STATE_SLEWING:
        case TELESCOPE_STATE_MOVING:
        case TELESCOPE_STATE_TRACKING_WAIT:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EBUSY;
            break;
        case TELESCOPE_STATE_POWERED_OFF:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EPWROFF;
            break;
        case TELESCOPE_STATE_MALFUNCTION:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EDEVMAL;
            break;
        case TELESCOPE_STATE_UNINITIALIZED:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EUNINIT;
            break;
        case TELESCOPE_STATE_PARKED:
            break;
        case TELESCOPE_STATE_TRACKING:
            break;
        default:
            break;
    }
    VirtualTelescope_get_current_postion(self);
    self->_.ra_from = self->_.ra;
    self->_.dec_from = self->_.dec;
    self->_.t_state.state = TELESCOPE_STATE_SLEWING;
    self->_.ra_to = ra;
    self->_.dec_to = dec;
    if ((self->_.ra_to - self->_.ra_from > 0. && self->_.ra_to - self->_.ra_from  <= 180.) || (self->_.ra_to - self->_.ra_from  <= -180.)) {
        self->_.slew_direction_x = 1;
    } else {
        self->_.slew_direction_x = -1;
    }
    self->_.slew_direction_y = (self->_.dec_to > self->_.dec_from) ? 1 : -1;
    self->_.last_slew_begin_time = get_current_time();
    ra_diff = fabs(self->_.ra_to - self->_.ra_from);
    ra_diff = ra_diff < 180. ? ra_diff : 360. - ra_diff;
    time_diff_ra = ra_diff / (self->_.slew_speed_x - self->_.slew_speed_x * SIDEREAL_TRACKING_SPEED);
    time_diff_dec = fabs(self->_.dec_to - fabs(self->_.dec_from)) / self->_.slew_speed_y;
    time_diff = max(time_diff_ra, time_diff_dec);
    Pthread_create(&self->_.tid, NULL, motor_thr, &time_diff);
    Pthread_mutex_unlock(&self->_.t_state.mtx);
    
    Pthread_join(self->_.tid, &value);
    if (value == PTHREAD_CANCELED) {
        return AAOS_ECANCELED;
    }
    /*
     * sleeping, update ra and dec, update last_park_off time
     */
    
    Pthread_mutex_lock(&self->_.t_state.mtx);
    self->_.t_state.state = TELESCOPE_STATE_TRACKING | flag;
    self->_.ra = ra;
    self->_.dec = dec;
    self->_.last_track_begin_time = get_current_time();
    Pthread_mutex_unlock(&self->_.t_state.mtx);
    Pthread_cond_broadcast(&self->_.t_state.cond);
    
    return AAOS_OK;
}

static int
VirtualTelescope_timed_slew(void *_self, double ra, double dec, double timeout)
{
    struct VirtualTelescope *self = cast(VirtualTelescope(), _self);
    
    double time_diff_ra, time_diff_dec, time_diff, ra_diff;
    void *value = NULL;
    struct timespec tp;
    
    if (timeout <= 0.) {
        return AAOS_EINVAL;
    }
    
    timeout += get_current_time();
    tp.tv_sec = floor(timeout);
    tp.tv_nsec = (timeout - tp.tv_sec) * 1000000000.;
    
    Pthread_mutex_lock(&self->_.t_state.mtx);
    unsigned int state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
    unsigned int flag = self->_.t_state.state & TELESCOPE_STATE_MALFUNCTION;
    if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        return AAOS_EDEVMAL;
    }
    switch (state) {
        case TELESCOPE_STATE_SLEWING:
        case TELESCOPE_STATE_MOVING:
        case TELESCOPE_STATE_TRACKING_WAIT:
            while (state == TELESCOPE_STATE_MOVING || state == TELESCOPE_STATE_SLEWING || state == TELESCOPE_STATE_TRACKING_WAIT) {
                int ret = Pthread_cond_timedwait(&self->_.t_state.cond, &self->_.t_state.mtx, &tp);
                if (ret == ETIMEDOUT) {
                    Pthread_mutex_unlock(&self->_.t_state.mtx);
                    return AAOS_ETIMEDOUT;
                }
                state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
                flag = state & TELESCOPE_STATE_MALFUNCTION;
            }
            if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
                Pthread_mutex_unlock(&self->_.t_state.mtx);
                return AAOS_EDEVMAL;
            }
            switch (self->_.t_state.state) {
                case TELESCOPE_STATE_POWERED_OFF:
                    Pthread_mutex_unlock(&self->_.t_state.mtx);
                    return AAOS_EPWROFF;
                    break;
                case TELESCOPE_STATE_MALFUNCTION:
                    Pthread_mutex_unlock(&self->_.t_state.mtx);
                    return AAOS_EDEVMAL;
                    break;
                case TELESCOPE_STATE_UNINITIALIZED:
                    Pthread_mutex_unlock(&self->_.t_state.mtx);
                    return AAOS_EUNINIT;
                    break;
                case TELESCOPE_STATE_PARKED:
                    VirtualTelescope_get_current_postion(self);
                    break;
                case TELESCOPE_STATE_TRACKING:
                    break;
                default:
                    break;
            }
            break;
        case TELESCOPE_STATE_POWERED_OFF:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EPWROFF;
            break;
        case TELESCOPE_STATE_MALFUNCTION:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EDEVMAL;
            break;
        case TELESCOPE_STATE_UNINITIALIZED:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EUNINIT;
            break;
        case TELESCOPE_STATE_PARKED:
            break;
        case TELESCOPE_STATE_TRACKING:
            break;
        default:
            break;
    }
    VirtualTelescope_get_current_postion(self);
    self->_.ra_from = self->_.ra;
    self->_.dec_from = self->_.dec;
    self->_.t_state.state = TELESCOPE_STATE_SLEWING | flag;
    self->_.ra_to = ra;
    self->_.dec_to = dec;
    if ((self->_.ra_to - self->_.ra_from > 0. && self->_.ra_to - self->_.ra_from  <= 180.) || (self->_.ra_to - self->_.ra_from  <= -180.)) {
        self->_.slew_direction_x = 1;
    } else {
        self->_.slew_direction_x = -1;
    }
    self->_.slew_direction_y = (self->_.dec_to > self->_.dec_from) ? 1 : -1;
    self->_.last_slew_begin_time = get_current_time();
    ra_diff = fabs(self->_.ra_to - self->_.ra_from);
    ra_diff = ra_diff < 180. ? ra_diff : 360. - ra_diff;
    time_diff_ra = ra_diff / (self->_.slew_speed_x - self->_.slew_speed_x * SIDEREAL_TRACKING_SPEED);
    time_diff_dec = fabs(self->_.dec_to - fabs(self->_.dec_from)) / self->_.slew_speed_y;
    time_diff = max(time_diff_ra, time_diff_dec);
    Pthread_create(&self->_.tid, NULL, motor_thr, &time_diff);
    Pthread_mutex_unlock(&self->_.t_state.mtx);
    
    Pthread_join(self->_.tid, &value);
    if (value == PTHREAD_CANCELED) {
        return AAOS_ECANCELED;
    }
    
    /*
     * sleeping, update ra and dec, update last_park_off time
     */
    
    Pthread_mutex_lock(&self->_.t_state.mtx);
    self->_.t_state.state = TELESCOPE_STATE_TRACKING | flag;
    self->_.ra = ra;
    self->_.dec = dec;
    self->_.last_track_begin_time = get_current_time();
    Pthread_mutex_unlock(&self->_.t_state.mtx);
    Pthread_cond_broadcast(&self->_.t_state.cond);
    
    return AAOS_OK;
}

static int
VirtualTelescope_park(void *_self)
{
    struct VirtualTelescope *self = cast(VirtualTelescope(), _self);
    
    Pthread_mutex_lock(&self->_.t_state.mtx);
    unsigned int state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
    unsigned int flag = self->_.t_state.state & TELESCOPE_STATE_MALFUNCTION;
    if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        return AAOS_EDEVMAL;
    }
    switch (state) {
        case TELESCOPE_STATE_MALFUNCTION:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EDEVMAL;
            break;
        case TELESCOPE_STATE_TRACKING:
            self->_.t_state.state = TELESCOPE_STATE_PARKED | flag;
            self->_.last_park_begin_time = get_current_time();
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_OK;
            break;
        case TELESCOPE_STATE_TRACKING_WAIT:
            self->_.t_state.state = TELESCOPE_STATE_PARKED | flag;
            self->_.last_park_begin_time = get_current_time();
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            Pthread_cond_broadcast(&self->_.t_state.cond);
            return AAOS_OK;
            break;
        case TELESCOPE_STATE_UNINITIALIZED:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EUNINIT;
            break;
        case TELESCOPE_STATE_POWERED_OFF:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EPWROFF;
        case TELESCOPE_STATE_PARKED:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_OK;
        case TELESCOPE_STATE_MOVING:
        case TELESCOPE_STATE_SLEWING:
            Pthread_cancel(self->_.tid);
            VirtualTelescope_get_current_postion(self);
            self->_.last_park_begin_time = get_current_time();
            self->_.t_state.state = TELESCOPE_STATE_PARKED | flag;
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            Pthread_cond_broadcast(&self->_.t_state.cond);
            return AAOS_OK;
        default:
            break;
    }
    return AAOS_OK;
}

static int
VirtualTelescope_park_off(void *_self)
{
    struct VirtualTelescope *self = cast(VirtualTelescope(), _self);
    
    Pthread_mutex_lock(&self->_.t_state.mtx);
    unsigned int state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
    unsigned int flag = self->_.t_state.state & TELESCOPE_STATE_MALFUNCTION;
    if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        return AAOS_EDEVMAL;
    }
    switch (state) {
        case TELESCOPE_STATE_PARKED:
            VirtualTelescope_get_current_postion(self);
            self->_.t_state.state = TELESCOPE_STATE_TRACKING | flag;
            break;
        default:
            break;
    }
    Pthread_mutex_unlock(&self->_.t_state.mtx);
    
    return AAOS_OK;
}

static int
VirtualTelescope_go_home(void *_self)
{
    int ret;
    
    if ((ret = VirtualTelescope_slew(_self, 0.0, 90.)) != AAOS_OK) {
        return ret;
    }
    
    return VirtualTelescope_park(_self);
}

static int
VirtualTelescope_set_move_speed(void *_self, double move_speed)
{
    struct VirtualTelescope *self = cast(VirtualTelescope(), _self);
    
    int ret;
    
    Pthread_mutex_lock(&self->_.t_state.mtx);
    unsigned int state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
    unsigned int flag = self->_.t_state.state & TELESCOPE_STATE_MALFUNCTION;
    if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        return AAOS_EDEVMAL;
    }
    switch (state) {
        case TELESCOPE_STATE_POWERED_OFF:
            ret = AAOS_EPWROFF;
            break;
        case TELESCOPE_STATE_UNINITIALIZED:
            ret = AAOS_EUNINIT;
            break;
        case TELESCOPE_STATE_MOVING:
            while (state == TELESCOPE_STATE_MOVING) {
                Pthread_cond_wait(&self->_.t_state.cond, &self->_.t_state.mtx);
                state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
                flag = self->_.t_state.state & TELESCOPE_STATE_MALFUNCTION;
            }
            switch (state) {
                case TELESCOPE_STATE_POWERED_OFF:
                    ret = AAOS_EPWROFF;
                    break;
                case TELESCOPE_STATE_UNINITIALIZED:
                    ret = AAOS_EUNINIT;
                    break;
                default:
                    if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
                        Pthread_mutex_unlock(&self->_.t_state.mtx);
                        return AAOS_EDEVMAL;
                    }
                    ret = AAOS_OK;
                    self->_.move_speed = move_speed;
                    break;
            }
            break;
        default:
            if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
                Pthread_mutex_unlock(&self->_.t_state.mtx);
                return AAOS_EDEVMAL;
            }
            ret = AAOS_OK;
            self->_.move_speed = move_speed;
            break;
    }
    Pthread_mutex_unlock(&self->_.t_state.mtx);
    
    return ret;
}

static int
VirtualTelescope_get_move_speed(void *_self, double *move_speed)
{
    struct VirtualTelescope *self = cast(VirtualTelescope(), _self);
    
    int ret;
    
    Pthread_mutex_lock(&self->_.t_state.mtx);
    unsigned int state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
    unsigned int flag = self->_.t_state.state & TELESCOPE_STATE_MALFUNCTION;
    if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        return AAOS_EDEVMAL;
    }
    switch (state) {
        case TELESCOPE_STATE_POWERED_OFF:
            ret = AAOS_EPWROFF;
            break;
        case TELESCOPE_STATE_UNINITIALIZED:
            ret = AAOS_EUNINIT;
            break;
        default:
            ret = AAOS_OK;
            *move_speed = self->_.move_speed;
            break;
    }
    Pthread_mutex_unlock(&self->_.t_state.mtx);
    
    return ret;
}

static int
VirtualTelescope_set_slew_speed(void *_self, double slew_speed_x, double slew_speed_y)
{
    struct VirtualTelescope *self = cast(VirtualTelescope(), _self);
    
    int ret;
    
    Pthread_mutex_lock(&self->_.t_state.mtx);
    unsigned int state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
    unsigned int flag = self->_.t_state.state & TELESCOPE_STATE_MALFUNCTION;
    if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        return AAOS_EDEVMAL;
    }
    switch (state) {
        case TELESCOPE_STATE_POWERED_OFF:
            ret = AAOS_EPWROFF;
            break;
        case TELESCOPE_STATE_UNINITIALIZED:
            ret = AAOS_EUNINIT;
            break;
        case TELESCOPE_STATE_MALFUNCTION:
            ret = AAOS_EDEVMAL;
            break;
        case TELESCOPE_STATE_MOVING:
            while (self->_.t_state.state == TELESCOPE_STATE_SLEWING) {
                Pthread_cond_wait(&self->_.t_state.cond, &self->_.t_state.mtx);
                state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
                flag = self->_.t_state.state & TELESCOPE_STATE_MALFUNCTION;
            }
            switch (state) {
                case TELESCOPE_STATE_POWERED_OFF:
                    ret = AAOS_EPWROFF;
                    break;
                case TELESCOPE_STATE_UNINITIALIZED:
                    ret = AAOS_EUNINIT;
                    break;
                default:
                    if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
                        Pthread_mutex_unlock(&self->_.t_state.mtx);
                        return AAOS_EDEVMAL;
                    }
                    ret = AAOS_OK;
                    self->_.slew_speed_x = slew_speed_x;
                    self->_.slew_speed_y = slew_speed_y;
                    break;
            }
            break;
        default:
            if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
                Pthread_mutex_unlock(&self->_.t_state.mtx);
                return AAOS_EDEVMAL;
            }
            ret = AAOS_OK;
            self->_.slew_speed_x = slew_speed_x;
            self->_.slew_speed_y = slew_speed_y;
            break;
    }
    Pthread_mutex_unlock(&self->_.t_state.mtx);
    
    return ret;
}

static int
VirtualTelescope_get_slew_speed(void *_self, double *slew_speed_x, double *slew_speed_y)
{
    struct VirtualTelescope *self = cast(VirtualTelescope(), _self);
    
    int ret;
    
    Pthread_mutex_lock(&self->_.t_state.mtx);
    unsigned int state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
    unsigned int flag = self->_.t_state.state & TELESCOPE_STATE_MALFUNCTION;
    if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        return AAOS_EDEVMAL;
    }
    switch (state) {
        case TELESCOPE_STATE_POWERED_OFF:
            ret = AAOS_EPWROFF;
            break;
        case TELESCOPE_STATE_UNINITIALIZED:
            ret = AAOS_EUNINIT;
            break;
            break;
        default:
            ret = AAOS_OK;
            *slew_speed_x = self->_.slew_speed_x;
            *slew_speed_y = self->_.slew_speed_y;
            break;
    }
    Pthread_mutex_unlock(&self->_.t_state.mtx);
    
    return ret;
}

static int
VirtualTelescope_set_track_rate(void *_self, double track_rate_x, double track_rate_y)
{
    struct VirtualTelescope *self = cast(VirtualTelescope(), _self);
    
    int ret;
    
    Pthread_mutex_lock(&self->_.t_state.mtx);
    unsigned int state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
    unsigned int flag = self->_.t_state.state & TELESCOPE_STATE_MALFUNCTION;
    if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        return AAOS_EDEVMAL;
    }
    switch (state) {
        case TELESCOPE_STATE_POWERED_OFF:
            ret = AAOS_EPWROFF;
            break;
        case TELESCOPE_STATE_UNINITIALIZED:
            ret = AAOS_EUNINIT;
            break;
        default:
            ret = AAOS_OK;
            self->_.track_rate_x = track_rate_x;
            self->_.track_rate_y = track_rate_y;
            break;
    }
    Pthread_mutex_unlock(&self->_.t_state.mtx);
    
    return ret;
}

static int
VirtualTelescope_get_track_rate(void *_self, double *track_rate_x, double *track_rate_y)
{
    struct VirtualTelescope *self = cast(VirtualTelescope(), _self);
    
    int ret;
    
    Pthread_mutex_lock(&self->_.t_state.mtx);
    unsigned int state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
    unsigned int flag = self->_.t_state.state & TELESCOPE_STATE_MALFUNCTION;
    if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        return AAOS_EDEVMAL;
    }
    switch (state) {
        case TELESCOPE_STATE_POWERED_OFF:
            ret = AAOS_EPWROFF;
            break;
        case TELESCOPE_STATE_UNINITIALIZED:
            ret = AAOS_EUNINIT;
            break;
        default:
            ret = AAOS_OK;
            *track_rate_x = self->_.track_rate_x;
            *track_rate_y = self->_.track_rate_y;
            break;
    }
    Pthread_mutex_unlock(&self->_.t_state.mtx);
    
    return ret;
}

static void *
VirtualTelescope_ctor(void *_self, va_list *app)
{
    struct VirtualTelescope *self = super_ctor(VirtualTelescope(), _self, app);
    
    self->_._vtab = virtual_telescope_virtual_table();
    
    return (void *) self;
}

static void *
VirtualTelescope_dtor(void *_self)
{
    return super_dtor(VirtualTelescope(), _self);
}

static void *
VirtualTelescopeClass_ctor(void *_self, va_list *app)
{
    struct VirtualTelescopeClass *self = super_ctor(VirtualTelescopeClass(), _self, app);

    return self;
}

static const void *_VirtualTelescopeClass;

static void
VirtualTelescopeClass_destroy(void)
{
    free((void *)_VirtualTelescopeClass);
}

static void
VirtualTelescopeClass_initialize(void)
{
    _VirtualTelescopeClass = new(__TelescopeClass(), "VirtualTelescopeClass", __TelescopeClass(), sizeof(struct VirtualTelescopeClass),
                                 ctor, "", VirtualTelescopeClass_ctor,
                                 (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(VirtualTelescopeClass_destroy);
#endif
}

const void *
VirtualTelescopeClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, VirtualTelescopeClass_initialize);
#endif

    return _VirtualTelescopeClass;
}

static const void *_VirtualTelescope;

static void
VirtualTelescope_destroy(void)
{
    free((void *)_VirtualTelescope);
}

static void
VirtualTelescope_initialize(void)
{
    _VirtualTelescope = new(VirtualTelescopeClass(), "VirtualTelescope", __Telescope(), sizeof(struct VirtualTelescope),
                            ctor, "ctor", VirtualTelescope_ctor,
                            dtor, "dtor", VirtualTelescope_dtor,
                            (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(VirtualTelescope_destroy);
#endif
}

const void *
VirtualTelescope(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, VirtualTelescope_initialize);
#endif

    return _VirtualTelescope;
}

/*
 * Virtual Telescope's virtual table.
 */

const static void * _virtual_telescope_virtual_table;

static void
virtual_telescope_virtual_table_destroy(void)
{
    delete((void *) _virtual_telescope_virtual_table);
}

static void
virtual_telescope_virtual_table_initialize(void)
{
    _virtual_telescope_virtual_table = new(__TelescopeVirtualTable(),
                                           __telescope_status, "status", VirtualTelescope_status,
                                           __telescope_power_on, "power_on", VirtualTelescope_power_on,
                                           __telescope_power_off, "power_off", VirtualTelescope_power_off,
                                           __telescope_init, "init", VirtualTelescope_init,
                                           __telescope_park, "park", VirtualTelescope_park,
                                           __telescope_park_off, "park_off", VirtualTelescope_park_off,
                                           __telescope_stop, "stop", VirtualTelescope_stop,
                                           __telescope_go_home, "go_home", VirtualTelescope_go_home,
                                           __telescope_move, "move", VirtualTelescope_move,
                                           __telescope_try_move, "try_move", VirtualTelescope_try_move,
                                           __telescope_timed_move, "timed_move", VirtualTelescope_timed_move,
                                           __telescope_slew, "slew", VirtualTelescope_slew,
                                           __telescope_try_slew, "try_slew", VirtualTelescope_try_slew,
                                           __telescope_timed_slew, "timed_slew", VirtualTelescope_timed_slew,
                                           __telescope_set_move_speed, "set_move_speed", VirtualTelescope_set_move_speed,
                                           __telescope_get_move_speed, "get_move_speed", VirtualTelescope_get_move_speed,
                                           __telescope_set_slew_speed, "set_slew_speed", VirtualTelescope_set_slew_speed,
                                           __telescope_get_slew_speed, "get_slew_speed", VirtualTelescope_get_slew_speed,
                                           __telescope_set_track_rate, "set_track_rate", VirtualTelescope_set_track_rate,
                                           __telescope_get_track_rate, "get_track_rate", VirtualTelescope_get_track_rate,
                                           (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(virtual_telescope_virtual_table_destroy);
#endif
}

static const void *
virtual_telescope_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, virtual_telescope_virtual_table_initialize);
#endif
    
    return _virtual_telescope_virtual_table;
}

/*
 * Driver for Astropysics Mount.
 */
#include "serial_rpc.h"

#define APMOUNT_POLL_INTERVAL           5
#define APMOUNT_WAITING_FOR_STEADY_TIME 2.
#define APMOUNT_MAX_POLL_COUNT          50
#define APMOUNT_THRESHOLD_WAITING       2.
#define APMOUNT_THRESHOLD_RETURN        0.5
#define APMOUNT_CHECK_PARK_INTERVAL     2.5
#define APMOUNT_MAX_TRY_PARK            3

static int
APMount_get_current_postion(struct APMount *self)
{
    char command[COMMANDSIZE], buf[BUFSIZE];
    size_t size;
    int ret;
    
    snprintf(command, COMMANDSIZE, ":GR#");
    if ((ret = serial_raw(self->serial_rpc, command, strlen(command), buf, BUFSIZE, &size) != AAOS_OK)) {
        return AAOS_EDEVMAL;
    }
    if (buf[size - 1] == '#') {
        buf[size - 1] = '\0';
    }
    self->_.ra = hms2deg(buf);
    snprintf(command, COMMANDSIZE, ":GD#");
    if ((ret = serial_raw(self->serial_rpc, command, strlen(command), buf, BUFSIZE, &size) != AAOS_OK)) {
        return AAOS_EDEVMAL;
    }
    if (buf[size - 1] == '#') {
        buf[size - 1] = '\0';
    }
    self->_.dec = dms2deg(buf);
    
    return AAOS_OK;
}

static int
APMount_really_parked(struct APMount *self)
{
    char command[COMMANDSIZE], az[COMMANDSIZE], old_az[COMMANDSIZE], ra[COMMANDSIZE], old_ra[COMMANDSIZE];
    size_t size;
    int ret;

    snprintf(command, COMMANDSIZE, ":GR#");
    if ((ret = serial_raw(self->serial_rpc, command, strlen(command), old_ra, COMMANDSIZE, &size) != AAOS_OK)) {
        return AAOS_EDEVMAL;
    }
    if (old_ra[size - 1] == '#') {
        old_ra[size - 1] = '\0';
    }
    snprintf(command, COMMANDSIZE, ":GZ#");
    if ((ret = serial_raw(self->serial_rpc, command, strlen(command), old_az, COMMANDSIZE, &size) != AAOS_OK)) {
        return AAOS_EDEVMAL;
    }
    if (old_az[size - 1] == '#') {
        old_az[size - 1] = '\0';
    }
    Nanosleep(APMOUNT_CHECK_PARK_INTERVAL);
    snprintf(command, COMMANDSIZE, ":GR#");
    if ((ret = serial_raw(self->serial_rpc, command, strlen(command), ra, COMMANDSIZE, &size) != AAOS_OK)) {
        return AAOS_EDEVMAL;
    }
    if (ra[size - 1] == '#') {
        ra[size - 1] = '\0';
    }
    snprintf(command, COMMANDSIZE, ":GZ#");
    if ((ret = serial_raw(self->serial_rpc, command, strlen(command), az, COMMANDSIZE, &size) != AAOS_OK)) {
        return AAOS_EDEVMAL;
    }
    if (az[size - 1] == '#') {
        az[size - 1] = '\0';
    }
    
    if ((strcmp(ra, old_ra) != 0) &&  (strcmp(az, old_az) == 0)) {
        return AAOS_OK;
    } else {
        return AAOS_ERROR;
    }
}

static int
APMount_status(void *_self, char *status_buffer, size_t status_buffer_size)
{
    struct APMount *self = cast(APMount(), _self);
    FILE *fp;
    char buf[BUFSIZE];
    
    if ((fp = fmemopen(status_buffer, status_buffer_size, "w")) == NULL) {
        return AAOS_ERROR;
    }
    
    fprintf(fp, "Name\t\t: %s\n", self->_.name);
    fprintf(fp, "Description\t: %s\n", self->_.description);
    
    switch (self->type) {
        case TELESCOPE_EQUATORIAL:
            fprintf(fp, "Type:\t\tEquatorial\n");
            break;
        case TELESCOPE_HORIZONTAL:
            fprintf(fp, "Type:\t\tHorizontal\n");
            break;
        default:
            break;
    }
    Pthread_mutex_lock(&self->_.t_state.mtx);
    unsigned int state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
    unsigned int flag = state & TELESCOPE_STATE_MALFUNCTION;
    if (flag) {
        fprintf(fp, "State:\t\tMalfunction\n");
        goto error;
    }
    switch (state) {
        case TELESCOPE_STATE_POWERED_OFF:
            fprintf(fp, "State:\t\tPowered off\n");
            goto error;
            break;
        case TELESCOPE_STATE_UNINITIALIZED:
            fprintf(fp, "State:\t\tUninitialized\n");
            goto error;
            break;
        case TELESCOPE_STATE_PARKED:
            fprintf(fp, "State:\t\tParked\n");
            break;
        case TELESCOPE_STATE_TRACKING:
            fprintf(fp, "State:\t\tTracking\n");
            break;
        case TELESCOPE_STATE_MOVING:
            fprintf(fp, "State:\t\tMoving\n");
            break;
        case TELESCOPE_STATE_SLEWING:
            fprintf(fp, "State:\t\tSlewing\n");
            break;
        case TELESCOPE_STATE_TRACKING_WAIT:
            fprintf(fp, "State:\t\tTracking and waiting\n");
            break;
        default:
            break;
    }
    if (APMount_get_current_postion(self) != AAOS_OK) {
        goto error;
    }
    switch (self->type) {
        case TELESCOPE_EQUATORIAL:
            deg2hms(self->_.ra, buf, BUFSIZE, "%Hh%Mm%04.1Ss");
            fprintf(fp, "RA:\t%s\n", buf);
            deg2dms(self->_.dec, buf, BUFSIZE, "%Dd%Mm%04.1Ss");
            fprintf(fp, "DEC:\t%s", buf);
            break;
        case TELESCOPE_HORIZONTAL:
            break;
        default:
            break;
    }
    
error:
    Pthread_mutex_unlock(&self->_.t_state.mtx);
    fclose(fp);
    return AAOS_OK;
}

static int
APMount_power_on(void *_self)
{
    struct APMount *self = cast(APMount(), _self);
    
    Pthread_mutex_lock(&self->_.t_state.mtx);
    unsigned int state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
    unsigned int flag = state & TELESCOPE_STATE_MALFUNCTION;
    if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        return AAOS_EDEVMAL;
    }
    switch (state) {
        case TELESCOPE_STATE_POWERED_OFF:
            self->_.t_state.state = TELESCOPE_STATE_UNINITIALIZED | flag;
            break;
        default:
            break;
    }
    Pthread_mutex_unlock(&self->_.t_state.mtx);
    
    return AAOS_OK;
}

static int
APMount_power_off(void *_self)
{
    struct APMount *self = cast(APMount(), _self);
    
    Pthread_mutex_lock(&self->_.t_state.mtx);
    unsigned int state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
    unsigned int flag = state & TELESCOPE_STATE_MALFUNCTION;
    if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        return AAOS_EDEVMAL;
    }
    switch (state) {
        case TELESCOPE_STATE_MOVING:
        case TELESCOPE_STATE_SLEWING:
            Pthread_cancel(self->_.tid);
            break;
        default:
            break;
    }
    self->_.t_state.state = TELESCOPE_STATE_POWERED_OFF | flag;
    Pthread_mutex_unlock(&self->_.t_state.mtx);
    
    return AAOS_OK;
}

/*
 * see APPENDIX D in "Astro-Physics GTO Servo Drive System Model GTOCP4 Operating Instructions" mannual
 * for the underline raw RS-232 commands.
 */

static int
APMount_init(void *_self)
{
    struct APMount *self = cast(APMount(), _self);
    
    char command[COMMANDSIZE], buf[BUFSIZE];
    int ret;
    time_t current_time;
    struct tm tp;
    
    Pthread_mutex_lock(&self->_.t_state.mtx);
    unsigned int state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
    unsigned int flag = self->_.t_state.state & TELESCOPE_STATE_MALFUNCTION;
    if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        return AAOS_EDEVMAL;
    }
    switch (state) {
        case TELESCOPE_STATE_UNINITIALIZED:
            
            if (APMount_really_parked(self) != AAOS_OK) {
                int i;
                for (i = 0; i < APMOUNT_MAX_TRY_PARK; i++) {
                    snprintf(command, COMMANDSIZE, ":KA#");
                    if ((ret = serial_raw(self->serial_rpc, command, strlen(command), buf, BUFSIZE, NULL) != AAOS_OK)) {
                        Pthread_mutex_unlock(&self->_.t_state.mtx);
                        return AAOS_EDEVMAL;
                    }
                    if (APMount_really_parked(self) == AAOS_OK) {
                        break;
                    }
                }
                if (i == APMOUNT_MAX_TRY_PARK) {
                    Pthread_mutex_unlock(&self->_.t_state.mtx);
                    return AAOS_ETIMEDOUT;
                }
            }
            snprintf(command, COMMANDSIZE, "#");
            if ((ret = serial_raw(self->serial_rpc, command, strlen(command), buf, BUFSIZE, NULL) != AAOS_OK)) {
                Pthread_mutex_unlock(&self->_.t_state.mtx);
                return AAOS_EDEVMAL;
            }
            snprintf(command, COMMANDSIZE, ":U#");
            if ((ret = serial_raw(self->serial_rpc, command, strlen(command), buf, BUFSIZE, NULL) != AAOS_OK)) {
                Pthread_mutex_unlock(&self->_.t_state.mtx);
                return AAOS_EDEVMAL;
            }
            time(&current_time);
            current_time -= 3600 * self->_.gmt_offset;
            gmtime_r(&current_time, &tp);
            strftime(command, COMMANDSIZE, ":SL %H:%M:%S#", &tp);
            if ((ret = serial_raw(self->serial_rpc, command, strlen(command), buf, BUFSIZE, NULL) != AAOS_OK)) {
                Pthread_mutex_unlock(&self->_.t_state.mtx);
                return AAOS_EDEVMAL;
            }
            strftime(command, COMMANDSIZE, ":SC %m/%d/%y#", &tp);
            if ((ret = serial_raw(self->serial_rpc, command, strlen(command), buf, BUFSIZE, NULL) != AAOS_OK)) {
                Pthread_mutex_unlock(&self->_.t_state.mtx);
                return AAOS_EDEVMAL;
            }
            double lon = self->_.location_lon;
            if (lon > 180. || lon < -180.) {
                Pthread_mutex_unlock(&self->_.t_state.mtx);
                return AAOS_EINVAL;
            }
            if (lon > 0) {
                lon = 360 - lon;
            } else {
                lon *= -1;
            }
            lon_deg2dms(lon, command, COMMANDSIZE, ":Sg %D*%M:%S#");
            if ((ret = serial_raw(self->serial_rpc, command, strlen(command), buf, BUFSIZE, NULL) != AAOS_OK)) {
                Pthread_mutex_unlock(&self->_.t_state.mtx);
                return AAOS_EDEVMAL;
            }
            lat_deg2dms(self->_.location_lat, command, COMMANDSIZE, ":St %D*%M:%S#");
            if ((ret = serial_raw(self->serial_rpc, command, strlen(command), buf, BUFSIZE, NULL) != AAOS_OK)) {
                Pthread_mutex_unlock(&self->_.t_state.mtx);
                return AAOS_EDEVMAL;
            }
            
            if (self->_.gmt_offset > 0.) {
                snprintf(command, COMMANDSIZE, ":SG %02d:00:00#", (int) floor(self->_.gmt_offset));
            } else {
                snprintf(command, COMMANDSIZE, ":SG -%02d:00:00#", (int) floor(fabs(self->_.gmt_offset)));
            }
            if ((ret = serial_raw(self->serial_rpc, command, strlen(command), buf, BUFSIZE, NULL) != AAOS_OK)) {
                Pthread_mutex_unlock(&self->_.t_state.mtx);
                return AAOS_EDEVMAL;
            }
            
            snprintf(command, COMMANDSIZE, ":PO#");
            if ((ret = serial_raw(self->serial_rpc, command, strlen(command), buf, BUFSIZE, NULL) != AAOS_OK)) {
                Pthread_mutex_unlock(&self->_.t_state.mtx);
                return AAOS_EDEVMAL;
            }
            snprintf(command, COMMANDSIZE, ":Q#");
            if ((ret = serial_raw(self->serial_rpc, command, strlen(command), buf, BUFSIZE, NULL) != AAOS_OK)) {
                Pthread_mutex_unlock(&self->_.t_state.mtx);
                return AAOS_EDEVMAL;
            }
            self->_.t_state.state = TELESCOPE_STATE_TRACKING | flag;
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_OK;
            break;
        case TELESCOPE_STATE_MALFUNCTION:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EDEVMAL;
            break;
        default:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EALREADY;
            break;
    }
}

static int
APMount_stop(void *_self)
{
    struct APMount *self = cast(APMount(), _self);
    
    char command[COMMANDSIZE], buf[BUFSIZE];
    int ret;
    
    snprintf(command, COMMANDSIZE, ":Q#");
    Pthread_mutex_lock(&self->_.t_state.mtx);
    unsigned int state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
    unsigned int flag = self->_.t_state.state & TELESCOPE_STATE_MALFUNCTION;
    if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        return AAOS_EDEVMAL;
    }
    switch (state) {
        case TELESCOPE_STATE_UNINITIALIZED:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EUNINIT;
            break;
        case TELESCOPE_STATE_POWERED_OFF:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EPWROFF;
            break;
        case TELESCOPE_STATE_PARKED:
        case TELESCOPE_STATE_TRACKING:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_OK;
            break;
        case TELESCOPE_STATE_MOVING:
        case TELESCOPE_STATE_SLEWING:
        case TELESCOPE_STATE_TRACKING_WAIT:
            Pthread_cancel(self->_.tid);
            if ((ret = serial_raw(self->serial_rpc, command, strlen(command), buf, BUFSIZE, NULL) != AAOS_OK)) {
                return AAOS_EDEVMAL;
            }
            self->_.t_state.state = TELESCOPE_STATE_TRACKING | flag;
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            Pthread_cond_broadcast(&self->_.t_state.cond);
            return AAOS_OK;
            break;
        default:
            break;
    }
    return AAOS_OK;
}

static void *
APMount_do_move(void *arg)
{
    struct APMount *self = (struct APMount *) arg;
    
    Nanosleep(self->_.move_duration);
    
    return NULL;
}

static void
APMount_move_speed_to_string(struct APMount *self, char *command, size_t command_size)
{
    double move_speed = self->_.move_speed;
    
    if (fabs(move_speed / SIDEREAL_TRACKING_SPEED - 12 ) <= 0.1) {
        snprintf(command, command_size, ":RC0#");
    } else if (fabs(move_speed / SIDEREAL_TRACKING_SPEED - 64 ) <= 0.1) {
        snprintf(command, command_size, ":RC1#");
    } else if (fabs(move_speed / SIDEREAL_TRACKING_SPEED - 600 ) <= 0.1) {
        snprintf(command, command_size, ":RC2#");
    } else if (fabs(move_speed / SIDEREAL_TRACKING_SPEED - 1200 ) <= 0.1) {
        snprintf(command, command_size, ":RC3#");
    }
}

static int
APMount_move(void *_self, unsigned int direction, double duration)
{
    struct APMount *self = cast(APMount(), _self);
    
    int ret;
    char command[COMMANDSIZE], buf[BUFSIZE];
    pthread_t tid;
    
    char *s;
    APMount_move_speed_to_string(self, command, COMMANDSIZE);
    s = command + strlen(command);
    
    switch (direction) {
        case TELESCOPE_MOVE_EAST:
            snprintf(s, COMMANDSIZE, ":Me#");
            break;
        case TELESCOPE_MOVE_WEST:
            snprintf(s, COMMANDSIZE, ":Mw#");
            break;
        case TELESCOPE_MOVE_NORTH:
            snprintf(s, COMMANDSIZE, ":Mn#");
            break;
        case TELESCOPE_MOVE_SOUTH:
            snprintf(s, COMMANDSIZE, ":Ms#");
            break;
        default:
            return AAOS_EINVAL;
            break;
    }
    
    if (duration < 0) {
        return AAOS_EINVAL;
    }
    
    Pthread_mutex_lock(&self->_.t_state.mtx);
    unsigned int state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
    unsigned int flag = self->_.t_state.state & TELESCOPE_STATE_MALFUNCTION;
    if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        return AAOS_EDEVMAL;
    }
    switch (state) {
        case TELESCOPE_STATE_SLEWING:
        case TELESCOPE_STATE_MOVING:
        case TELESCOPE_STATE_TRACKING_WAIT:
            if (self->_.option & TELESCOPE_OPTION_DO_NOT_WAIT && state != TELESCOPE_STATE_TRACKING_WAIT) {
                Pthread_cancel(self->_.tid);
                break;
            }
            while (state == TELESCOPE_STATE_MOVING || state == TELESCOPE_STATE_SLEWING || state == TELESCOPE_STATE_TRACKING_WAIT) {
                Pthread_cond_wait(&self->_.t_state.cond, &self->_.t_state.mtx);
                state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
                flag = state & TELESCOPE_STATE_MALFUNCTION;
            }
            if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
                Pthread_mutex_unlock(&self->_.t_state.mtx);
                return AAOS_EDEVMAL;
            }
            switch (state) {
                case TELESCOPE_STATE_POWERED_OFF:
                    Pthread_mutex_unlock(&self->_.t_state.mtx);
                    return AAOS_EPWROFF;
                    break;
                case TELESCOPE_STATE_MALFUNCTION:
                    Pthread_mutex_unlock(&self->_.t_state.mtx);
                    return AAOS_EDEVMAL;
                    break;
                case TELESCOPE_STATE_UNINITIALIZED:
                    Pthread_mutex_unlock(&self->_.t_state.mtx);
                    return AAOS_EUNINIT;
                    break;
                case TELESCOPE_STATE_PARKED:
                case TELESCOPE_STATE_TRACKING:
                    break;
                default:
                    break;
            }
            break;
        case TELESCOPE_STATE_POWERED_OFF:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EPWROFF;
            break;
        case TELESCOPE_STATE_MALFUNCTION:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EDEVMAL;
            break;
        case TELESCOPE_STATE_UNINITIALIZED:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EUNINIT;
            break;
        case TELESCOPE_STATE_PARKED:
            break;
        case TELESCOPE_STATE_TRACKING:
            break;
        default:
            break;
    }
    
    if ((ret = serial_raw(self->serial_rpc, command, strlen(command), buf, BUFSIZE, NULL) != AAOS_OK)) {
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        return AAOS_EDEVMAL;
    }

    self->_.last_move_begin_time = get_current_time();
    self->_.move_duration = duration;
    Pthread_create(&tid, NULL, APMount_do_move, self);
    self->_.tid = tid;
    self->_.t_state.state = TELESCOPE_STATE_MOVING | flag;
    Pthread_mutex_unlock(&self->_.t_state.mtx);
    
    void *value;
    
    Pthread_join(tid, &value);
    
    Pthread_mutex_lock(&self->_.t_state.mtx);
    if (value == NULL) {
        flag = self->_.t_state.state & TELESCOPE_STATE_MALFUNCTION;
        self->_.t_state.state = TELESCOPE_STATE_TRACKING | flag;
        switch (direction) {
            case TELESCOPE_MOVE_EAST:
                snprintf(command, COMMANDSIZE, ":Qe#");
                break;
            case TELESCOPE_MOVE_WEST:
                snprintf(command, COMMANDSIZE, ":Qw#");
                break;
            case TELESCOPE_MOVE_NORTH:
                snprintf(command, COMMANDSIZE, ":Qn#");
                break;
            case TELESCOPE_MOVE_SOUTH:
                snprintf(command, COMMANDSIZE, ":Qs#");
                break;
            default:
                return AAOS_EINVAL;
                break;
        }
        if ((ret = serial_raw(self->serial_rpc, command, strlen(command), buf, BUFSIZE, NULL) != AAOS_OK)) {
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EDEVMAL;
        }
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        Pthread_cond_broadcast(&self->_.t_state.cond);
        self->_.last_track_begin_time = get_current_time();
        return AAOS_OK;
    } else if (value == PTHREAD_CANCELED) {
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        Pthread_cond_broadcast(&self->_.t_state.cond);
        return AAOS_ECANCELED;
    } else {
        if (value == (void *) AAOS_EDEVMAL) {
            self->_.t_state.state = TELESCOPE_STATE_MALFUNCTION;
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            Pthread_cond_broadcast(&self->_.t_state.cond);
            return AAOS_EDEVMAL;
        } else if (value == (void *) AAOS_EDEVMAL) {
            self->_.t_state.state = TELESCOPE_STATE_MALFUNCTION;
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            Pthread_cond_broadcast(&self->_.t_state.cond);
            return AAOS_ETIMEDOUT;
        }
    }
    Pthread_mutex_unlock(&self->_.t_state.mtx);
    Pthread_cond_broadcast(&self->_.t_state.cond);
    return AAOS_ERROR;
}

static int
APMount_try_move(void *_self, unsigned int direction, double duration)
{
    struct APMount *self = cast(APMount(), _self);
    
    int ret;
    char command[COMMANDSIZE], buf[BUFSIZE];
    pthread_t tid;
    
    char *s;
    APMount_move_speed_to_string(self, command, COMMANDSIZE);
    s = command + strlen(command);
    
    switch (direction) {
        case TELESCOPE_MOVE_EAST:
            snprintf(s, COMMANDSIZE, ":Me#");
            break;
        case TELESCOPE_MOVE_WEST:
            snprintf(s, COMMANDSIZE, ":Mw#");
            break;
        case TELESCOPE_MOVE_NORTH:
            snprintf(s, COMMANDSIZE, ":Mn#");
            break;
        case TELESCOPE_MOVE_SOUTH:
            snprintf(s, COMMANDSIZE, ":Ms#");
            break;
        default:
            return AAOS_EINVAL;
            break;
    }
    
    if (duration < 0) {
        return AAOS_EINVAL;
    }
    
    if (Pthread_mutex_trylock(&self->_.t_state.mtx) != 0) {
        return AAOS_EBUSY;
    }
    unsigned int state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
    unsigned int flag = self->_.t_state.state & TELESCOPE_STATE_MALFUNCTION;
    if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        return AAOS_EDEVMAL;
    }
    switch (state) {
        case TELESCOPE_STATE_SLEWING:
        case TELESCOPE_STATE_MOVING:
        case TELESCOPE_STATE_TRACKING_WAIT:
            return AAOS_EBUSY;
            break;
        case TELESCOPE_STATE_POWERED_OFF:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EPWROFF;
            break;
        case TELESCOPE_STATE_MALFUNCTION:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EDEVMAL;
            break;
        case TELESCOPE_STATE_UNINITIALIZED:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EUNINIT;
            break;
        case TELESCOPE_STATE_PARKED:
            break;
        case TELESCOPE_STATE_TRACKING:
            break;
        default:
            break;
    }
    if ((ret = serial_raw(self->serial_rpc, command, strlen(command), buf, BUFSIZE, NULL) != AAOS_OK)) {
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        return AAOS_EDEVMAL;
    }
    
    self->_.last_move_begin_time = get_current_time();
    self->_.move_duration = duration;
    Pthread_create(&tid, NULL, APMount_do_move, self);
    self->_.tid = tid;
    self->_.t_state.state = TELESCOPE_STATE_MOVING | flag;
    Pthread_mutex_unlock(&self->_.t_state.mtx);
    
    void *value;
    
    Pthread_join(tid, &value);
    
    Pthread_mutex_lock(&self->_.t_state.mtx);
    if (value == NULL) {
        flag = self->_.t_state.state & TELESCOPE_STATE_MALFUNCTION;
        self->_.t_state.state = TELESCOPE_STATE_TRACKING | flag;
        switch (direction) {
            case TELESCOPE_MOVE_EAST:
                snprintf(command, COMMANDSIZE, ":Qe#");
                break;
            case TELESCOPE_MOVE_WEST:
                snprintf(command, COMMANDSIZE, ":Qw#");
                break;
            case TELESCOPE_MOVE_NORTH:
                snprintf(command, COMMANDSIZE, ":Qn#");
                break;
            case TELESCOPE_MOVE_SOUTH:
                snprintf(command, COMMANDSIZE, ":Qs#");
                break;
            default:
                return AAOS_EINVAL;
                break;
        }
        if ((ret = serial_raw(self->serial_rpc, command, strlen(command), buf, BUFSIZE, NULL) != AAOS_OK)) {
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EDEVMAL;
        }
        self->_.last_track_begin_time = get_current_time();
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        Pthread_cond_broadcast(&self->_.t_state.cond);
        return AAOS_OK;
    } else if (value == PTHREAD_CANCELED) {
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        Pthread_cond_broadcast(&self->_.t_state.cond);
        return AAOS_ECANCELED;
    } else {
        if (value == (void *) AAOS_EDEVMAL) {
            self->_.t_state.state = TELESCOPE_STATE_MALFUNCTION;
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            Pthread_cond_broadcast(&self->_.t_state.cond);
            return AAOS_EDEVMAL;
        } else if (value == (void *) AAOS_EDEVMAL) {
            self->_.t_state.state = TELESCOPE_STATE_MALFUNCTION;
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            Pthread_cond_broadcast(&self->_.t_state.cond);
            return AAOS_ETIMEDOUT;
        }
    }
    Pthread_mutex_unlock(&self->_.t_state.mtx);
    Pthread_cond_broadcast(&self->_.t_state.cond);
    return AAOS_ERROR;
}


static int
APMount_timed_move(void *_self, unsigned int direction, double duration, double timeout)
{
    struct APMount *self = cast(APMount(), _self);
    
    int ret;
    char command[COMMANDSIZE], buf[BUFSIZE];
    pthread_t tid;
    
    struct timespec tp;
    if (timeout <= 0.) {
        return AAOS_EINVAL;
    }
    timeout += get_current_time();
    tp.tv_sec = floor(timeout);
    tp.tv_nsec = (timeout - tp.tv_sec) * 1000000000.;

    char *s;
    APMount_move_speed_to_string(self, command, COMMANDSIZE);
    s = command + strlen(command);
    
    switch (direction) {
        case TELESCOPE_MOVE_EAST:
            snprintf(s, COMMANDSIZE, ":Me#");
            break;
        case TELESCOPE_MOVE_WEST:
            snprintf(s, COMMANDSIZE, ":Mw#");
            break;
        case TELESCOPE_MOVE_NORTH:
            snprintf(s, COMMANDSIZE, ":Mn#");
            break;
        case TELESCOPE_MOVE_SOUTH:
            snprintf(s, COMMANDSIZE, ":Ms#");
            break;
        default:
            return AAOS_EINVAL;
            break;
    }
    if (duration < 0) {
        return AAOS_EINVAL;
    }
    
    Pthread_mutex_lock(&self->_.t_state.mtx);
    unsigned int state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
    unsigned int flag = self->_.t_state.state & TELESCOPE_STATE_MALFUNCTION;
    if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        return AAOS_EDEVMAL;
    }
    switch (state) {
        case TELESCOPE_STATE_SLEWING:
        case TELESCOPE_STATE_MOVING:
        case TELESCOPE_STATE_TRACKING_WAIT:
            while (state == TELESCOPE_STATE_MOVING || state == TELESCOPE_STATE_SLEWING || state == TELESCOPE_STATE_TRACKING_WAIT) {
                Pthread_cond_timedwait(&self->_.t_state.cond, &self->_.t_state.mtx, &tp);
                state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
                flag = state & TELESCOPE_STATE_MALFUNCTION;
            }
            if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
                Pthread_mutex_unlock(&self->_.t_state.mtx);
                return AAOS_EDEVMAL;
            }
            switch (state) {
                case TELESCOPE_STATE_POWERED_OFF:
                    Pthread_mutex_unlock(&self->_.t_state.mtx);
                    return AAOS_EPWROFF;
                    break;
                case TELESCOPE_STATE_MALFUNCTION:
                    Pthread_mutex_unlock(&self->_.t_state.mtx);
                    return AAOS_EDEVMAL;
                    break;
                case TELESCOPE_STATE_UNINITIALIZED:
                    Pthread_mutex_unlock(&self->_.t_state.mtx);
                    return AAOS_EUNINIT;
                    break;
                case TELESCOPE_STATE_PARKED:
                case TELESCOPE_STATE_TRACKING:
                    break;
                default:
                    break;
            }
            break;
        case TELESCOPE_STATE_POWERED_OFF:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EPWROFF;
            break;
        case TELESCOPE_STATE_MALFUNCTION:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EDEVMAL;
            break;
        case TELESCOPE_STATE_UNINITIALIZED:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EUNINIT;
            break;
        case TELESCOPE_STATE_PARKED:
            break;
        case TELESCOPE_STATE_TRACKING:
            break;
        default:
            break;
    }
    if ((ret = serial_raw(self->serial_rpc, command, strlen(command), buf, BUFSIZE, NULL) != AAOS_OK)) {
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        return AAOS_EDEVMAL;
    }
    
    self->_.t_state.state = TELESCOPE_STATE_MOVING | flag;
    self->_.last_move_begin_time = get_current_time();
    self->_.move_duration = duration;
    Pthread_create(&tid, NULL, APMount_do_move, self);
    self->_.tid = tid;
    Pthread_mutex_unlock(&self->_.t_state.mtx);
    
    void *value;
    
    Pthread_join(tid, &value);
    
    Pthread_mutex_lock(&self->_.t_state.mtx);
    if (value == NULL) {
        flag = self->_.t_state.state & TELESCOPE_STATE_MALFUNCTION;
        self->_.t_state.state = TELESCOPE_STATE_TRACKING | flag;
        switch (direction) {
            case TELESCOPE_MOVE_EAST:
                snprintf(command, COMMANDSIZE, ":Qe#");
                break;
            case TELESCOPE_MOVE_WEST:
                snprintf(command, COMMANDSIZE, ":Qw#");
                break;
            case TELESCOPE_MOVE_NORTH:
                snprintf(command, COMMANDSIZE, ":Qn#");
                break;
            case TELESCOPE_MOVE_SOUTH:
                snprintf(command, COMMANDSIZE, ":Qs#");
                break;
            default:
                return AAOS_EINVAL;
                break;
        }
        if ((ret = serial_raw(self->serial_rpc, command, strlen(command), buf, BUFSIZE, NULL) != AAOS_OK)) {
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EDEVMAL;
        }
        self->_.last_track_begin_time = get_current_time();
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        Pthread_cond_broadcast(&self->_.t_state.cond);
        return AAOS_OK;
    } else if (value == PTHREAD_CANCELED) {
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        Pthread_cond_broadcast(&self->_.t_state.cond);
        return AAOS_ECANCELED;
    } else {
        if (value == (void *) AAOS_EDEVMAL) {
            self->_.t_state.state = TELESCOPE_STATE_MALFUNCTION;
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            Pthread_cond_broadcast(&self->_.t_state.cond);
            return AAOS_EDEVMAL;
        } else if (value == (void *) AAOS_EDEVMAL) {
            self->_.t_state.state = TELESCOPE_STATE_MALFUNCTION;
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            Pthread_cond_broadcast(&self->_.t_state.cond);
            return AAOS_ETIMEDOUT;
        }
    }
    
    Pthread_mutex_unlock(&self->_.t_state.mtx);
    Pthread_cond_broadcast(&self->_.t_state.cond);
    return AAOS_ERROR;
}

static void
APMount_slew_speed_to_string(struct APMount *self, char *command, size_t command_size)
{
    double slew_speed_x = self->_.slew_speed_x;
    
    if (fabs(slew_speed_x / SIDEREAL_TRACKING_SPEED - 600) <= 0.1) {
        snprintf(command, COMMANDSIZE, ":RS0#");
    } else if (fabs(slew_speed_x / SIDEREAL_TRACKING_SPEED - 900 ) <= 0.1) {
        snprintf(command, COMMANDSIZE, ":RS1#");
    } else if (fabs(slew_speed_x / SIDEREAL_TRACKING_SPEED - 1200 ) <= 0.1) {
        snprintf(command, COMMANDSIZE, ":RS2#");
    } else {
        snprintf(command, COMMANDSIZE, ":Rs%03d#", (int) floor(slew_speed_x / SIDEREAL_TRACKING_SPEED));
    }
}

#define APMOUNT_SLEW_WAIT_TIME 0.005

static void *
APMount_do_slew(void *arg)
{
    struct APMount *self = (struct APMount *) arg;
    
    int count;
    int ret;
    double distance;
    
    Nanosleep(APMOUNT_SLEW_WAIT_TIME);
    
    for (count = 0; count < APMOUNT_MAX_POLL_COUNT; count++) {
        Pthread_mutex_lock(&self->_.t_state.mtx);
        if ((ret = APMount_get_current_postion(self)) != AAOS_OK) {
            uint64_t p = ret;
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return (void *) p;
        }
        distance = equatorial_distance(self->_.ra, self->_.dec, self->_.ra_to, self->_.dec_to);
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        if (distance <= APMOUNT_THRESHOLD_WAITING && distance > APMOUNT_THRESHOLD_RETURN) {
            Nanosleep(APMOUNT_WAITING_FOR_STEADY_TIME);
        } else if (distance <= APMOUNT_THRESHOLD_RETURN) {
            return NULL;
        } else {
            Nanosleep(APMOUNT_POLL_INTERVAL);
        }
    }
    
    return (void *) AAOS_ETIMEDOUT;
}

static int
APMount_slew(void *_self, double ra, double dec)
{
    struct APMount *self = cast(APMount(), _self);
    
    int ret;
    char command[COMMANDSIZE], buf[BUFSIZE];
    pthread_t tid;
    
    Pthread_mutex_lock(&self->_.t_state.mtx);
    unsigned int state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
    unsigned int flag = self->_.t_state.state & TELESCOPE_STATE_MALFUNCTION;
    if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        return AAOS_EDEVMAL;
    }
    switch (state) {
        case TELESCOPE_STATE_SLEWING:
        case TELESCOPE_STATE_MOVING:
        case TELESCOPE_STATE_TRACKING_WAIT:
            if (self->_.option & TELESCOPE_OPTION_DO_NOT_WAIT && state != TELESCOPE_STATE_TRACKING_WAIT) {
                Pthread_cancel(self->_.tid);
                break;
            }
            while (state == TELESCOPE_STATE_MOVING || state == TELESCOPE_STATE_SLEWING || state == TELESCOPE_STATE_TRACKING_WAIT) {
                Pthread_cond_wait(&self->_.t_state.cond, &self->_.t_state.mtx);
                state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
                flag = state & TELESCOPE_STATE_MALFUNCTION;
            }
            if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
                Pthread_mutex_unlock(&self->_.t_state.mtx);
                return AAOS_EDEVMAL;
            }
            switch (state) {
                case TELESCOPE_STATE_POWERED_OFF:
                    Pthread_mutex_unlock(&self->_.t_state.mtx);
                    return AAOS_EPWROFF;
                    break;
                case TELESCOPE_STATE_MALFUNCTION:
                    Pthread_mutex_unlock(&self->_.t_state.mtx);
                    return AAOS_EDEVMAL;
                    break;
                case TELESCOPE_STATE_UNINITIALIZED:
                    Pthread_mutex_unlock(&self->_.t_state.mtx);
                    return AAOS_EUNINIT;
                    break;
                case TELESCOPE_STATE_PARKED:
                case TELESCOPE_STATE_TRACKING:
                    break;
                default:
                    break;
            }
            break;
        case TELESCOPE_STATE_POWERED_OFF:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EPWROFF;
            break;
        case TELESCOPE_STATE_MALFUNCTION:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EDEVMAL;
            break;
        case TELESCOPE_STATE_UNINITIALIZED:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EUNINIT;
            break;
        case TELESCOPE_STATE_PARKED:
            break;
        case TELESCOPE_STATE_TRACKING:
            break;
        default:
            break;
    }
    if (self->type == TELESCOPE_HORIZONTAL) {

    } else {
        char *s = command;
        ssize_t nleft = COMMANDSIZE;
        APMount_slew_speed_to_string(self, s, nleft);
        nleft -= strlen(s);
        s += strlen(s);
        deg2hms(ra, s, nleft, ":Sr %H:%M:%S#");
        nleft -= strlen(s);
        s += strlen(s);
        deg2dms(dec, s, nleft, ":Sd %D*%M:%S#");
        nleft -= strlen(s);
        s += strlen(s);
        snprintf(s, nleft, ":MS#");
        self->_.ra_to = ra;
        self->_.dec_to = dec;
        if ((ret = serial_raw(self->serial_rpc, command, strlen(command), buf, BUFSIZE, NULL) != AAOS_OK)) {
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EDEVMAL;
        }
    }
    self->_.t_state.state = TELESCOPE_STATE_SLEWING | flag;
    self->_.last_slew_begin_time = get_current_time();
    Pthread_create(&tid, NULL, APMount_do_slew, self);
    self->_.tid = tid;
    Pthread_mutex_unlock(&self->_.t_state.mtx);
    
    void *value;
    
    Pthread_join(tid, &value);
    
    Pthread_mutex_lock(&self->_.t_state.mtx);
    if (value == NULL) {
        flag = self->_.t_state.state & TELESCOPE_STATE_MALFUNCTION;
        self->_.t_state.state = TELESCOPE_STATE_TRACKING | flag;
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        Pthread_cond_broadcast(&self->_.t_state.cond);
        return AAOS_OK;
    } else if (value == PTHREAD_CANCELED) {
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        Pthread_cond_broadcast(&self->_.t_state.cond);
        return AAOS_ECANCELED;
    } else {
        if (value == (void *) AAOS_EDEVMAL) {
            self->_.t_state.state = TELESCOPE_STATE_MALFUNCTION;
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            Pthread_cond_broadcast(&self->_.t_state.cond);
            return AAOS_EDEVMAL;
        } else if (value == (void *) AAOS_ETIMEDOUT) {
            flag = self->_.t_state.state & TELESCOPE_STATE_MALFUNCTION;
            self->_.t_state.state = TELESCOPE_STATE_TRACKING | flag;
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            Pthread_cond_broadcast(&self->_.t_state.cond);
            return AAOS_ETIMEDOUT;
        }
    }
    self->_.last_track_begin_time = get_current_time();
    Pthread_mutex_unlock(&self->_.t_state.mtx);
    Pthread_cond_broadcast(&self->_.t_state.cond);
    return AAOS_ERROR;
}

static int
APMount_try_slew(void *_self, double ra, double dec)
{
    struct APMount *self = cast(APMount(), _self);
    
    int ret;
    char command[COMMANDSIZE], buf[BUFSIZE];
    pthread_t tid;
    
    if (Pthread_mutex_trylock(&self->_.t_state.mtx) != 0) {
        return AAOS_EBUSY;
    }
    unsigned int state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
    unsigned int flag = self->_.t_state.state & TELESCOPE_STATE_MALFUNCTION;
    if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        return AAOS_EDEVMAL;
    }
    switch (state) {
        case TELESCOPE_STATE_SLEWING:
        case TELESCOPE_STATE_MOVING:
        case TELESCOPE_STATE_TRACKING_WAIT:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EBUSY;
            break;
        case TELESCOPE_STATE_POWERED_OFF:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EPWROFF;
            break;
        case TELESCOPE_STATE_MALFUNCTION:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EDEVMAL;
            break;
        case TELESCOPE_STATE_UNINITIALIZED:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EUNINIT;
            break;
        case TELESCOPE_STATE_PARKED:
            break;
        case TELESCOPE_STATE_TRACKING:
            break;
        default:
            break;
    }
    if (self->type == TELESCOPE_HORIZONTAL) {
        
    } else {
        char *s = command;
        ssize_t nleft = COMMANDSIZE;
        APMount_slew_speed_to_string(self, s, nleft);
        nleft -= strlen(s);
        s += strlen(s);
        deg2hms(ra, s, nleft, ":Sr %H:%M:%S#");
        nleft -= strlen(s);
        s += strlen(s);
        deg2dms(dec, s, nleft, ":Sd %D*%M:%S#");
        nleft -= strlen(s);
        s += strlen(s);
        snprintf(s, nleft, ":MS#");
        self->_.ra_to = ra;
        self->_.dec_to = dec;
        if ((ret = serial_raw(self->serial_rpc, command, strlen(command), buf, BUFSIZE, NULL) != AAOS_OK)) {
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EDEVMAL;
        }
    }
    self->_.t_state.state = TELESCOPE_STATE_SLEWING | flag;
    self->_.last_slew_begin_time = get_current_time();
    Pthread_create(&tid, NULL, APMount_do_slew, self);
    self->_.tid = tid;
    Pthread_mutex_unlock(&self->_.t_state.mtx);
    
    void *value;
    
    Pthread_join(tid, &value);
    
    Pthread_mutex_lock(&self->_.t_state.mtx);
    if (value == NULL) {
        flag = self->_.t_state.state & TELESCOPE_STATE_MALFUNCTION;
        self->_.t_state.state = TELESCOPE_STATE_TRACKING | flag;
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        Pthread_cond_broadcast(&self->_.t_state.cond);
        return AAOS_OK;
    } else if (value == PTHREAD_CANCELED) {
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        Pthread_cond_broadcast(&self->_.t_state.cond);
        return AAOS_ECANCELED;
    } else {
        if (value == (void *) AAOS_EDEVMAL) {
            self->_.t_state.state = TELESCOPE_STATE_MALFUNCTION;
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            Pthread_cond_broadcast(&self->_.t_state.cond);
            return AAOS_EDEVMAL;
        } else if (value == (void *) AAOS_ETIMEDOUT) {
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            Pthread_cond_broadcast(&self->_.t_state.cond);
            return AAOS_ETIMEDOUT;
        }
    }
    self->_.last_track_begin_time = get_current_time();
    Pthread_mutex_unlock(&self->_.t_state.mtx);
    Pthread_cond_broadcast(&self->_.t_state.cond);
    return AAOS_ERROR;
}

static int
APMount_timed_slew(void *_self, double ra, double dec, double timeout)
{
    struct APMount *self = cast(APMount(), _self);
    
    int ret;
    char command[COMMANDSIZE], buf[BUFSIZE];
    pthread_t tid;
    struct timespec tp;
    
    if (timeout <= 0.) {
        return AAOS_EINVAL;
    }
    
    timeout += get_current_time();
    tp.tv_sec = floor(timeout);
    tp.tv_nsec = (timeout - tp.tv_sec) * 1000000000.;
    
    Pthread_mutex_lock(&self->_.t_state.mtx);
    unsigned int state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
    unsigned int flag = self->_.t_state.state & TELESCOPE_STATE_MALFUNCTION;
    if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        return AAOS_EDEVMAL;
    }
    switch (state) {
        case TELESCOPE_STATE_SLEWING:
        case TELESCOPE_STATE_MOVING:
        case TELESCOPE_STATE_TRACKING_WAIT:
            while (state == TELESCOPE_STATE_MOVING || state == TELESCOPE_STATE_SLEWING || state == TELESCOPE_STATE_TRACKING_WAIT) {
                ret = Pthread_cond_timedwait(&self->_.t_state.cond, &self->_.t_state.mtx, &tp);
                if (ret == ETIMEDOUT) {
                    Pthread_mutex_unlock(&self->_.t_state.mtx);
                    return AAOS_ETIMEDOUT;
                }
                state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
                flag = state & TELESCOPE_STATE_MALFUNCTION;
            }
            if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
                Pthread_mutex_unlock(&self->_.t_state.mtx);
                return AAOS_EDEVMAL;
            }
            switch (state) {
                case TELESCOPE_STATE_POWERED_OFF:
                    Pthread_mutex_unlock(&self->_.t_state.mtx);
                    return AAOS_EPWROFF;
                    break;
                case TELESCOPE_STATE_MALFUNCTION:
                    Pthread_mutex_unlock(&self->_.t_state.mtx);
                    return AAOS_EDEVMAL;
                    break;
                case TELESCOPE_STATE_UNINITIALIZED:
                    Pthread_mutex_unlock(&self->_.t_state.mtx);
                    return AAOS_EUNINIT;
                    break;
                case TELESCOPE_STATE_PARKED:
                case TELESCOPE_STATE_TRACKING:
                    break;
                default:
                    break;
            }
            break;
        case TELESCOPE_STATE_POWERED_OFF:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EPWROFF;
            break;
        case TELESCOPE_STATE_MALFUNCTION:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EDEVMAL;
            break;
        case TELESCOPE_STATE_UNINITIALIZED:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EUNINIT;
            break;
        case TELESCOPE_STATE_PARKED:
            break;
        case TELESCOPE_STATE_TRACKING:
            break;
        default:
            break;
    }
    if (self->type == TELESCOPE_HORIZONTAL) {
        
    } else {
        char *s = command;
        ssize_t nleft = COMMANDSIZE;
        APMount_slew_speed_to_string(self, s, nleft);
        nleft -= strlen(s);
        s += strlen(s);
        deg2hms(ra, s, nleft, ":Sr %H:%M:%S#");
        nleft -= strlen(s);
        s += strlen(s);
        deg2dms(dec, s, nleft, ":Sd %D*%M:%S#");
        nleft -= strlen(s);
        s += strlen(s);
        snprintf(s, nleft, ":MS#");
        self->_.ra_to = ra;
        self->_.dec_to = dec;
        if ((ret = serial_raw(self->serial_rpc, command, strlen(command), buf, BUFSIZE, NULL) != AAOS_OK)) {
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EDEVMAL;
        }
    }
    self->_.t_state.state = TELESCOPE_STATE_SLEWING | flag;
    self->_.last_slew_begin_time = get_current_time();
    Pthread_create(&tid, NULL, APMount_do_slew, self);
    self->_.tid = tid;
    Pthread_mutex_unlock(&self->_.t_state.mtx);
    
    void *value;
    
    Pthread_join(tid, &value);
    
    Pthread_mutex_lock(&self->_.t_state.mtx);
    if (value == NULL) {
        self->_.t_state.state = TELESCOPE_STATE_TRACKING | flag;
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        Pthread_cond_broadcast(&self->_.t_state.cond);
        return AAOS_OK;
    } else if (value == PTHREAD_CANCELED) {
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        Pthread_cond_broadcast(&self->_.t_state.cond);
        return AAOS_ECANCELED;
    } else {
        if (value == (void *) AAOS_EDEVMAL) {
            self->_.t_state.state = TELESCOPE_STATE_MALFUNCTION;
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            Pthread_cond_broadcast(&self->_.t_state.cond);
            return AAOS_EDEVMAL;
        } else if (value == (void *) AAOS_ETIMEDOUT) {
            self->_.t_state.state = TELESCOPE_STATE_MALFUNCTION;
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            Pthread_cond_broadcast(&self->_.t_state.cond);
            return AAOS_ETIMEDOUT;
        }
    }
    self->_.last_track_begin_time = get_current_time();
    Pthread_mutex_unlock(&self->_.t_state.mtx);
    Pthread_cond_broadcast(&self->_.t_state.cond);
    return AAOS_ERROR;
}

static int
APMount_park(void *_self)
{
    struct APMount *self = cast(APMount(), _self);
    char command[COMMANDSIZE], buf[BUFSIZE];
    int ret;
    
    snprintf(command, COMMANDSIZE, ":KA#");
    Pthread_mutex_lock(&self->_.t_state.mtx);
    unsigned int state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
    unsigned int flag = self->_.t_state.state & TELESCOPE_STATE_MALFUNCTION;
    if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
        self->_.t_state.state = TELESCOPE_STATE_PARKED | flag;
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        return AAOS_EDEVMAL;
    }
    if ((ret = serial_raw(self->serial_rpc, command, strlen(command), buf, BUFSIZE, NULL) != AAOS_OK)) {
        self->_.t_state.state = TELESCOPE_STATE_PARKED | flag;
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        return AAOS_EDEVMAL;
    }
    self->_.last_park_begin_time = get_current_time();
    switch (state) {
        case TELESCOPE_STATE_TRACKING:
            self->_.t_state.state = TELESCOPE_STATE_PARKED | flag;
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_OK;
            break;
        case TELESCOPE_STATE_TRACKING_WAIT:
            self->_.t_state.state = TELESCOPE_STATE_PARKED | flag;
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            Pthread_cond_broadcast(&self->_.t_state.cond);
            return AAOS_OK;
            break;
        case TELESCOPE_STATE_UNINITIALIZED:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EUNINIT;
            break;
        case TELESCOPE_STATE_POWERED_OFF:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EPWROFF;
        case TELESCOPE_STATE_PARKED:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_OK;
        case TELESCOPE_STATE_MOVING:
        case TELESCOPE_STATE_SLEWING:
            Pthread_cancel(self->_.tid);
            self->_.t_state.state = TELESCOPE_STATE_PARKED | flag;
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            Pthread_cond_broadcast(&self->_.t_state.cond);
            return AAOS_OK;
        default:
            break;
    }
    return AAOS_OK;
}

static int
APMount_park_off(void *_self)
{
    struct APMount *self = cast(APMount(), _self);
    char command[COMMANDSIZE], buf[BUFSIZE];
    int ret;
    time_t current_time;
    struct tm tp;
    
    Pthread_mutex_lock(&self->_.t_state.mtx);
    unsigned int state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
    unsigned int flag = self->_.t_state.state & TELESCOPE_STATE_MALFUNCTION;
    if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        return AAOS_EDEVMAL;
    }
    switch (state) {
        case TELESCOPE_STATE_PARKED:
            time(&current_time);
            current_time -= 3600 * self->_.gmt_offset;
            gmtime_r(&current_time, &tp);
            strftime(command, COMMANDSIZE, ":SL %H:%M:%S#", &tp);
            if ((ret = serial_raw(self->serial_rpc, command, strlen(command), buf, BUFSIZE, NULL) != AAOS_OK)) {
                Pthread_mutex_unlock(&self->_.t_state.mtx);
                return AAOS_EDEVMAL;
            }
            
            strftime(command, COMMANDSIZE, ":SC %m/%d/%y#", &tp);
            if ((ret = serial_raw(self->serial_rpc, command, strlen(command), buf, BUFSIZE, NULL) != AAOS_OK)) {
                Pthread_mutex_unlock(&self->_.t_state.mtx);
                return AAOS_EDEVMAL;
            }
            snprintf(command, COMMANDSIZE, ":PO#");
            if ((ret = serial_raw(self->serial_rpc, command, strlen(command), buf, BUFSIZE, NULL) != AAOS_OK)) {
                Pthread_mutex_unlock(&self->_.t_state.mtx);
                return AAOS_EDEVMAL;
            }
            self->_.t_state.state = TELESCOPE_STATE_TRACKING | flag;
            break;
        default:
            break;
    }
    Pthread_mutex_unlock(&self->_.t_state.mtx);
    
    return AAOS_OK;
}

static int
APMount_go_home(void *_self)
{
    int ret;
    
    if ((ret = APMount_slew(_self, 359.54, 89.8)) != AAOS_OK) {
        return ret;
    }
    
    return APMount_park(_self);
}

static int
APMount_set_move_speed(void *_self, double move_speed)
{
    struct APMount *self = cast(APMount(), _self);
    char command[COMMANDSIZE], buf[BUFSIZE];
    int ret;
    
    if (move_speed < 0. || move_speed > SIDEREAL_TRACKING_SPEED * 1200.) {
        return AAOS_EINVAL;
    }
    
    if (fabs(move_speed / SIDEREAL_TRACKING_SPEED - 12 ) <= 0.1) {
        snprintf(command, COMMANDSIZE, ":RC0#");
    } else if (fabs(move_speed / SIDEREAL_TRACKING_SPEED - 64 ) <= 0.1) {
        snprintf(command, COMMANDSIZE, ":RC1#");
    } else if (fabs(move_speed / SIDEREAL_TRACKING_SPEED - 600 ) <= 0.1) {
        snprintf(command, COMMANDSIZE, ":RC2#");
    } else if (fabs(move_speed / SIDEREAL_TRACKING_SPEED - 1200 ) <= 0.1) {
        snprintf(command, COMMANDSIZE, ":RC3#");
    } else {
        return AAOS_EINVAL;
    }
    
    Pthread_mutex_lock(&self->_.t_state.mtx);
    unsigned int state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
    unsigned int flag = self->_.t_state.state & TELESCOPE_STATE_MALFUNCTION;
    if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        return AAOS_EDEVMAL;
    }
    switch (state) {
        case TELESCOPE_STATE_POWERED_OFF:
            ret = AAOS_EPWROFF;
            break;
        case TELESCOPE_STATE_UNINITIALIZED:
            ret = AAOS_EUNINIT;
            break;
        case TELESCOPE_STATE_MOVING:
            while (state == TELESCOPE_STATE_MOVING) {
                Pthread_cond_wait(&self->_.t_state.cond, &self->_.t_state.mtx);
                state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
            }
            switch (state) {
                case TELESCOPE_STATE_POWERED_OFF:
                    ret = AAOS_EPWROFF;
                    break;
                case TELESCOPE_STATE_UNINITIALIZED:
                    ret = AAOS_EUNINIT;
                    break;
                default:
                    ret = AAOS_OK;
                    if ((ret = serial_raw(self->serial_rpc, command, strlen(command), buf, BUFSIZE, NULL) != AAOS_OK)) {
                        Pthread_mutex_unlock(&self->_.t_state.mtx);
                        return AAOS_EDEVMAL;
                    }
                    self->_.move_speed = move_speed;
                    break;
            }
            break;
        default:
            ret = AAOS_OK;
            if ((ret = serial_raw(self->serial_rpc, command, strlen(command), buf, BUFSIZE, NULL) != AAOS_OK)) {
                Pthread_mutex_unlock(&self->_.t_state.mtx);
                return AAOS_EDEVMAL;
            }
            self->_.move_speed = move_speed;
            break;
    }
    Pthread_mutex_unlock(&self->_.t_state.mtx);
    
    return ret;
}

static int
APMount_get_move_speed(void *_self, double *move_speed)
{
    struct APMount *self = cast(APMount(), _self);
    
    int ret;
    
    Pthread_mutex_lock(&self->_.t_state.mtx);
    unsigned int state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
    unsigned int flag = self->_.t_state.state & TELESCOPE_STATE_MALFUNCTION;
    if (flag) {
        ret = AAOS_EDEVMAL;
        goto error;
    }
    switch (state) {
        case TELESCOPE_STATE_POWERED_OFF:
            ret = AAOS_EPWROFF;
            break;
        case TELESCOPE_STATE_UNINITIALIZED:
            ret = AAOS_EUNINIT;
            break;
        default:
            ret = AAOS_OK;
            *move_speed = self->_.move_speed;
            break;
    }
error:
    Pthread_mutex_unlock(&self->_.t_state.mtx);
    
    return ret;
}

static int
APMount_set_slew_speed(void *_self, double slew_speed_x, double slew_speed_y)
{
    struct APMount *self = cast(APMount(), _self);
    char command[COMMANDSIZE], buf[BUFSIZE];
    int ret;
    
    if (slew_speed_x < 0. || slew_speed_x > SIDEREAL_TRACKING_SPEED * 1200.) {
        return AAOS_EINVAL;
    }
    
    if (fabs(slew_speed_x / SIDEREAL_TRACKING_SPEED - 600) <= 0.1) {
        snprintf(command, COMMANDSIZE, ":RS0#");
    } else if (fabs(slew_speed_x / SIDEREAL_TRACKING_SPEED - 900 ) <= 0.1) {
        snprintf(command, COMMANDSIZE, ":RS1#");
    } else if (fabs(slew_speed_x / SIDEREAL_TRACKING_SPEED - 1200 ) <= 0.1) {
        snprintf(command, COMMANDSIZE, ":RS2#");
    } else {
        snprintf(command, COMMANDSIZE, ":Rs%03d#", (int) floor(slew_speed_x / SIDEREAL_TRACKING_SPEED));
    }
    
    Pthread_mutex_lock(&self->_.t_state.mtx);
    unsigned int state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
    unsigned int flag = self->_.t_state.state & TELESCOPE_STATE_MALFUNCTION;
    if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        return AAOS_EDEVMAL;
    }
    switch (state) {
        case TELESCOPE_STATE_POWERED_OFF:
            ret = AAOS_EPWROFF;
            break;
        case TELESCOPE_STATE_UNINITIALIZED:
            ret = AAOS_EUNINIT;
            break;
        case TELESCOPE_STATE_MALFUNCTION:
            ret = AAOS_EDEVMAL;
            break;
        case TELESCOPE_STATE_MOVING:
            while (state == TELESCOPE_STATE_SLEWING) {
                Pthread_cond_wait(&self->_.t_state.cond, &self->_.t_state.mtx);
                state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
                flag = self->_.t_state.state & TELESCOPE_STATE_MALFUNCTION;
            }
            if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
                Pthread_mutex_unlock(&self->_.t_state.mtx);
                return AAOS_EDEVMAL;
            }
            switch (state) {
                case TELESCOPE_STATE_POWERED_OFF:
                    ret = AAOS_EPWROFF;
                    break;
                case TELESCOPE_STATE_UNINITIALIZED:
                    ret = AAOS_EUNINIT;
                    break;
                default:
                    ret = AAOS_OK;
                    if ((ret = serial_raw(self->serial_rpc, command, strlen(command), buf, BUFSIZE, NULL) != AAOS_OK)) {
                        Pthread_mutex_unlock(&self->_.t_state.mtx);
                        return AAOS_EDEVMAL;
                    }
                    self->_.slew_speed_x = slew_speed_x;
                    self->_.slew_speed_y = slew_speed_y;
                    break;
            }
            break;
        default:
            ret = AAOS_OK;
            if ((ret = serial_raw(self->serial_rpc, command, strlen(command), buf, BUFSIZE, NULL) != AAOS_OK)) {
                Pthread_mutex_unlock(&self->_.t_state.mtx);
                return AAOS_EDEVMAL;
            }
            self->_.slew_speed_x = slew_speed_x;
            self->_.slew_speed_y = slew_speed_y;
            break;
    }
    Pthread_mutex_unlock(&self->_.t_state.mtx);
    
    return ret;
}

static int
APMount_get_slew_speed(void *_self, double *slew_speed_x, double *slew_speed_y)
{
    struct APMount *self = cast(APMount(), _self);
    
    int ret;
    
    Pthread_mutex_lock(&self->_.t_state.mtx);
    unsigned int state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
    unsigned int flag = self->_.t_state.state & TELESCOPE_STATE_MALFUNCTION;
    if (flag) {
        ret = AAOS_EDEVMAL;
        goto error;
    }
    switch (state) {
        case TELESCOPE_STATE_POWERED_OFF:
            ret = AAOS_EPWROFF;
            break;
        case TELESCOPE_STATE_UNINITIALIZED:
            ret = AAOS_EUNINIT;
            break;
        default:
            ret = AAOS_OK;
            *slew_speed_x = self->_.slew_speed_x;
            *slew_speed_y = self->_.slew_speed_y;
            break;
    }
error:
    Pthread_mutex_unlock(&self->_.t_state.mtx);
    
    return ret;
}

static int
APMount_set_track_rate(void *_self, double track_rate_x, double track_rate_y)
{
    struct APMount *self = cast(APMount(), _self);
    
    char command[COMMANDSIZE], buf[BUFSIZE];
    int ret;
    
    if (fabs(track_rate_x - TELESCOPE_TRACK_RATE_LUNAR) <= 0.001) {
        snprintf(command, COMMANDSIZE, ":RT0#");
    } else if (fabs(track_rate_x - TELESCOPE_TRACK_RATE_SOLAR) <= 0.001) {
        snprintf(command, COMMANDSIZE, ":RT1#");
    } else if (fabs(track_rate_x - TELESCOPE_TRACK_RATE_SIDEREAL) <= 0.001) {
        snprintf(command, COMMANDSIZE, ":RT2#");
    } else {
        track_rate_x = track_rate_x / SIDEREAL_TRACKING_SPEED;
        track_rate_y = track_rate_y / SIDEREAL_TRACKING_SPEED;
        if (fabs(track_rate_x) >+ 1000. || fabs(track_rate_y) >= 1000.) {
            return AAOS_EINVAL;
        }
        snprintf(command, COMMANDSIZE, ":RR %8.4f#:RD %8.4f#", track_rate_x, track_rate_y);
    }
    
    Pthread_mutex_lock(&self->_.t_state.mtx);
    unsigned int state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
    unsigned int flag = self->_.t_state.state & TELESCOPE_STATE_MALFUNCTION;
    if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        return AAOS_EDEVMAL;
    }
    switch (state) {
        case TELESCOPE_STATE_POWERED_OFF:
            ret = AAOS_EPWROFF;
            break;
        case TELESCOPE_STATE_UNINITIALIZED:
            ret = AAOS_EUNINIT;
            break;
        case TELESCOPE_STATE_MOVING:
            while (state == TELESCOPE_STATE_SLEWING) {
                Pthread_cond_wait(&self->_.t_state.cond, &self->_.t_state.mtx);
                state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
                flag = self->_.t_state.state & TELESCOPE_STATE_MALFUNCTION;
            }
            if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
                Pthread_mutex_unlock(&self->_.t_state.mtx);
                return AAOS_EDEVMAL;
            }
            switch (state) {
                case TELESCOPE_STATE_POWERED_OFF:
                    ret = AAOS_EPWROFF;
                    break;
                case TELESCOPE_STATE_UNINITIALIZED:
                    ret = AAOS_EUNINIT;
                    break;
                default:
                    ret = AAOS_OK;
                    if ((ret = serial_raw(self->serial_rpc, command, strlen(command), buf, BUFSIZE, NULL) != AAOS_OK)) {
                        Pthread_mutex_unlock(&self->_.t_state.mtx);
                        return AAOS_EDEVMAL;
                    }
                    self->_.track_rate_x = track_rate_x;
                    break;
            }
            break;
        default:
            ret = AAOS_OK;
            if ((ret = serial_raw(self->serial_rpc, command, strlen(command), buf, BUFSIZE, NULL) != AAOS_OK)) {
                Pthread_mutex_unlock(&self->_.t_state.mtx);
                return AAOS_EDEVMAL;
            }
            if (fabs(track_rate_x - TELESCOPE_TRACK_RATE_LUNAR) <= 0.001) {
                self->_.track_rate_x = track_rate_x;
            } else if (fabs(track_rate_x - TELESCOPE_TRACK_RATE_SOLAR) <= 0.001) {
                self->_.track_rate_x = track_rate_x;
            } else if (fabs(track_rate_x - TELESCOPE_TRACK_RATE_SIDEREAL) <= 0.001) {
                self->_.track_rate_x = track_rate_x;
            } else {
                self->_.track_rate_x = track_rate_x * SIDEREAL_TRACKING_SPEED;
                self->_.track_rate_y = track_rate_y * SIDEREAL_TRACKING_SPEED;
            }
            break;
    }
    Pthread_mutex_unlock(&self->_.t_state.mtx);
    
    return ret;
}

static int
APMount_get_track_rate(void *_self, double *track_rate_x, double *track_rate_y)
{
    struct APMount *self = cast(APMount(), _self);
    
    int ret;
    
    Pthread_mutex_lock(&self->_.t_state.mtx);
    unsigned int state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
    unsigned int flag = self->_.t_state.state & TELESCOPE_STATE_MALFUNCTION;
    if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        return AAOS_EDEVMAL;
    }
    switch (state) {
        case TELESCOPE_STATE_POWERED_OFF:
            ret = AAOS_EPWROFF;
            break;
        case TELESCOPE_STATE_UNINITIALIZED:
            ret = AAOS_EUNINIT;
            break;
        default:
            ret = AAOS_OK;
            *track_rate_x = self->_.track_rate_x;
            *track_rate_y = self->_.track_rate_y;
            
            break;
    }
    Pthread_mutex_unlock(&self->_.t_state.mtx);
    
    return ret;
}

static int
APMount_raw(void *_self, const void *raw_command, size_t size, size_t *write_size, void *results, size_t results_size, size_t *return_size)
{
    struct APMount *self = cast(APMount(), _self);
    int ret;
    
    if ((ret = serial_raw(self->serial_rpc, raw_command, size, results, results_size, return_size)) == AAOS_OK) {
        if (write_size != NULL) {
            *write_size = strlen(raw_command);
        }
    }
    return ret;
}

static const void *ap_mount_virtual_table(void);

static void *
APMount_ctor(void *_self, va_list *app)
{
    struct APMount *self = super_ctor(APMount(), _self, app);
    
    self->type = va_arg(*app, int);
    const char *value;
    value = va_arg(*app, const char *);
    if (value) {
        self->serial_server_address = (char *) Malloc(strlen(value) + 1);
        snprintf(self->serial_server_address, strlen(value) + 1, "%s", value);
    }
    value = va_arg(*app, const char *);
    if (value) {
        self->serial_server_port = (char *) Malloc(strlen(value) + 1);
        snprintf(self->serial_server_port, strlen(value) + 1, "%s", value);
    }
    value = va_arg(*app, const char *);
    if (value) {
        self->serial_name = (char *) Malloc(strlen(value) + 1);
        snprintf(self->serial_name, strlen(value) + 1, "%s", value);
    }
    
    int ret;
    void *client = new(SerialClient(), self->serial_server_address, self->serial_server_port);
    
    if ((ret = rpc_client_connect(client, &self->serial_rpc)) != AAOS_OK) {
        self->_.t_state.state |= TELESCOPE_STATE_MALFUNCTION;
        goto error;
    }
    if ((ret = serial_get_index_by_name(self->serial_rpc, self->serial_name)) != AAOS_OK) {
        goto error;
    }
    self->_.move_speed = 600. * SIDEREAL_TRACKING_SPEED;
    self->_.slew_speed_x = 1200. * SIDEREAL_TRACKING_SPEED;
error:
    delete(client);
    self->_._vtab = ap_mount_virtual_table();
    
    return (void *) self;
}


static void *
APMount_dtor(void *_self)
{
    struct APMount *self = cast(APMount(), _self);
    
    delete(self->serial_rpc);
    free(self->serial_name);
    free(self->serial_server_port);
    free(self->serial_server_address);
    
    return super_dtor(APMount(), _self);
}

static void *
APMountClass_ctor(void *_self, va_list *app)
{
    struct APMountClass *self = super_ctor(APMountClass(), _self, app);
    
    self->_.raw.method = (Method) 0;
    
    return self;
}

static const void *_APMountClass;

static void
APMountClass_destroy(void)
{
    free((void *)_APMountClass);
}

static void
APMountClass_initialize(void)
{
    _APMountClass = new(__TelescopeClass(), "APMountClass", __TelescopeClass(), sizeof(struct APMountClass),
                        ctor, "", APMountClass_ctor,
                        (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(APMountClass_destroy);
#endif
}

const void *
APMountClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, APMountClass_initialize);
#endif
    
    return _APMountClass;
}

static const void *_APMount;

static void
APMount_destroy(void)
{
    free((void *)_APMount);
}

static void
APMount_initialize(void)
{
    _APMount = new(APMountClass(), "APMount", __Telescope(), sizeof(struct APMount),
                   ctor, "ctor", APMount_ctor,
                   dtor, "dtor", APMount_dtor,
                   (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(APMount_destroy);
#endif
}

const void *
APMount(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, APMount_initialize);
#endif
    
    return _APMount;
}

const static void * _ap_mount_virtual_table;

static void
ap_mount_virtual_table_destroy(void)
{
    delete((void *) _ap_mount_virtual_table);
}

static void
ap_mount_virtual_table_initialize(void)
{
    _ap_mount_virtual_table = new(__TelescopeVirtualTable(),
                                  __telescope_status, "status", APMount_status,
                                  __telescope_power_on, "power_on", APMount_power_on,
                                  __telescope_power_off, "power_off", APMount_power_off,
                                  __telescope_init, "init", APMount_init,
                                  __telescope_park, "park", APMount_park,
                                  __telescope_park_off, "park_off", APMount_park_off,
                                  __telescope_stop, "stop", APMount_stop,
                                  __telescope_go_home, "go_home", APMount_go_home,
                                  __telescope_move, "move", APMount_move,
                                  __telescope_try_move, "try_move", APMount_try_move,
                                  __telescope_timed_move, "timed_move", APMount_timed_move,
                                  __telescope_slew, "slew", APMount_slew,
                                  __telescope_try_slew, "try_slew", APMount_try_slew,
                                  __telescope_timed_slew, "timed_slew", APMount_timed_slew,
                                  __telescope_set_move_speed, "set_move_speed", APMount_set_move_speed,
                                  __telescope_get_move_speed, "get_move_speed", APMount_get_move_speed,
                                  __telescope_set_slew_speed, "set_slew_speed", APMount_set_slew_speed,
                                  __telescope_get_slew_speed, "get_slew_speed", APMount_get_slew_speed,
                                  __telescope_set_track_rate, "set_track_rate", APMount_set_track_rate,
                                  __telescope_get_track_rate, "get_track_rate", APMount_get_track_rate,
                                  __telescope_raw, "raw", APMount_raw,
                                  (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(ap_mount_virtual_table_destroy);
#endif
}

static const void *
ap_mount_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, ap_mount_virtual_table_initialize);
#endif
    
    return _ap_mount_virtual_table;
}

#ifdef __USE_ASCOM__

/*
 * see https://ascom-standards.org/api for ASCOM's Alpaca API,
 */

/*
#include "ascom.h"
#include <cjson/cJSON.h>

static int
ASCOMMount_power_on(void *_self)
{
    return AAOS_OK;
}

static int
ASCOMMount_power_off(void *_self)
{
    return AAOS_OK;
}

static int
ASCOMMount_init(void *_self)
{
    struct ASCOMMount *self = cast(ASCOMMount(), _self);
    
    char data[BUFSIZE], time_buf[BUFSIZE], *buf = NULL;
    size_t size = 0;
    time_t current_time;
    struct timespec tp;
    struct tm tm;
    int ret = AAOS_OK;
    
    Pthread_mutex_lock(&self->_.t_state.mtx);
    unsigned int state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
    unsigned int flag = self->_.t_state.state & TELESCOPE_STATE_MALFUNCTION;
    if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        return AAOS_EDEVMAL;
    }
    switch (state) {
        case TELESCOPE_STATE_UNINITIALIZED:
            snprintf(data, BUFSIZE, "sitelatitude=%.6f", self->_.location_lat);
            if ((ret = ascom_put(self->ascom, "sitelatitude", data, buf, &size)) != AAOS_OK) {
                goto error;
            }
            if (ascom_get_error_code(buf, &ret) != AAOS_OK || ret != AAOS_OK) {
                ret = AAOS_ERROR;
                goto error;
            }
            
            snprintf(data, BUFSIZE, "sitelongitude=%.6f", self->_.location_lon);
            if ((ret = ascom_put(self->ascom, "sitelongitude", data, buf, &size)) != AAOS_OK) {
                goto error;
            }
            if (ascom_get_error_code(buf, &ret) != AAOS_OK || ret != AAOS_OK) {
                ret = AAOS_ERROR;
                goto error;
            }
            
            snprintf(data, BUFSIZE, "siteelevation=%.2f", self->_.location_lon);
            if ((ret = ascom_put(self->ascom, "siteelevation", data, buf, &size)) != AAOS_OK) {
                goto error;
            }
            if (ascom_get_error_code(buf, &ret) != AAOS_OK || ret != AAOS_OK) {
                ret = AAOS_ERROR;
                goto error;
            }
            
            Clock_gettime(CLOCK_REALTIME, &tp);
            current_time = tp.tv_sec;
            gmtime_r(&current_time, &tm);
            strftime(time_buf, BUFSIZE, "%Y-%m-%dT%H:%M:%S", &tm);
            snprintf(data, BUFSIZE, "utcdate=%s.%07ldZ", time_buf, tp.tv_nsec * 100);
            if ((ascom_put(self->ascom, "utcdate", data, buf, &size)) == AAOS_OK) {
                goto error;
            }
            if (ascom_get_error_code(buf, &ret) != AAOS_OK || ret != AAOS_OK) {
                ret = AAOS_ERROR;
                goto error;
            }
            
        error:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            free(buf);
            return ret;
            break;
        case TELESCOPE_STATE_MALFUNCTION:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EDEVMAL;
            break;
        default:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EALREADY;
            break;
    }
}

static int
ASCOMMount_stop(void *_self)
{
    struct ASCOMMount *self = cast(ASCOMMount(), _self);
    
    char *buf = NULL;
    size_t size = 0;
    int ret;

    Pthread_mutex_lock(&self->_.t_state.mtx);
    unsigned int state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
    unsigned int flag = self->_.t_state.state & TELESCOPE_STATE_MALFUNCTION;
    if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        return AAOS_EDEVMAL;
    }
    switch (state) {
        case TELESCOPE_STATE_UNINITIALIZED:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EUNINIT;
            break;
        case TELESCOPE_STATE_POWERED_OFF:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EPWROFF;
            break;
        case TELESCOPE_STATE_PARKED:
        case TELESCOPE_STATE_TRACKING:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_OK;
            break;
        case TELESCOPE_STATE_MOVING:
        case TELESCOPE_STATE_SLEWING:
        case TELESCOPE_STATE_TRACKING_WAIT:
            Pthread_cancel(self->_.tid);
            if ((ret = ascom_put(self->ascom, "abortslew", NULL, buf, &size)) != AAOS_OK) {
                goto error;
            }
            if (ascom_get_error_code(buf, &ret) != AAOS_OK || ret != AAOS_OK) {
                ret = AAOS_ERROR;
                goto error;
            }
        error:
            if (ret != AAOS_OK) {
                self->_.t_state.state = TELESCOPE_STATE_MALFUNCTION | flag;
            } else {
                self->_.t_state.state = TELESCOPE_STATE_TRACKING | flag;
            }
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            Pthread_cond_broadcast(&self->_.t_state.cond);
            return ret;
            break;
        default:
            break;
    }
    return AAOS_OK;
}

struct ASCOMMoveThreadArg {
    struct ASCOMMount *self;
    int guide_direction;
    int guide_duration;
    int error_code;
};

static void *
ASCOMMount_do_move(void *arg)
{
    struct ASCOMMoveThreadArg *my_arg = (struct ASCOMMoveThreadArg *) arg;
    char data[BUFSIZE], *buf = NULL;
    size_t size = 0;
    int ret;
    
    snprintf(data, BUFSIZE, "direction=%d&duration=%d", my_arg->guide_direction, my_arg->guide_duration);
    if ((ret = ascom_put(my_arg->self->ascom, "pulseguide", data, buf, &size)) != AAOS_OK) {
        my_arg->error_code = ret;
        goto error;
    }
    if (ascom_get_error_code(buf, &ret) != AAOS_OK || ret != AAOS_OK) {
        ret = AAOS_ERROR;
        goto error;
    }
    
error:
    free(buf);
    return NULL;
}


static int
ASCOMMount_move(void *_self, unsigned int direction, double duration)
{
    struct ASCOMMount *self = cast(ASCOMMount(), _self);
    
    pthread_t tid;
    int guide_direction, guide_duration;
    
    switch (direction) {
        case TELESCOPE_MOVE_EAST:
            guide_direction = 2;
            break;
        case TELESCOPE_MOVE_WEST:
            guide_direction = 3;
            break;
        case TELESCOPE_MOVE_NORTH:
            guide_direction = 0;
            break;
        case TELESCOPE_MOVE_SOUTH:
            guide_direction = 1;
            break;
        default:
            return AAOS_EINVAL;
            break;
    }
    
    if (duration < 0) {
        return AAOS_EINVAL;
    }
    
    guide_duration = (int) duration * 1000;
    
    struct ASCOMMoveThreadArg arg;
    
    arg.self = self;
    arg.guide_direction = guide_direction;
    arg.guide_duration = guide_duration;
    
    Pthread_mutex_lock(&self->_.t_state.mtx);
    unsigned int state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
    unsigned int flag = self->_.t_state.state & TELESCOPE_STATE_MALFUNCTION;
    if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        return AAOS_EDEVMAL;
    }
    switch (state) {
        case TELESCOPE_STATE_SLEWING:
        case TELESCOPE_STATE_MOVING:
        case TELESCOPE_STATE_TRACKING_WAIT:
            if (self->_.option & TELESCOPE_OPTION_DO_NOT_WAIT && state != TELESCOPE_STATE_TRACKING_WAIT) {
                Pthread_cancel(self->_.tid);
                break;
            }
            while (state == TELESCOPE_STATE_MOVING || state == TELESCOPE_STATE_SLEWING || state == TELESCOPE_STATE_TRACKING_WAIT) {
                Pthread_cond_wait(&self->_.t_state.cond, &self->_.t_state.mtx);
                state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
                flag = state & TELESCOPE_STATE_MALFUNCTION;
            }
            if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
                Pthread_mutex_unlock(&self->_.t_state.mtx);
                return AAOS_EDEVMAL;
            }
            switch (state) {
                case TELESCOPE_STATE_POWERED_OFF:
                    Pthread_mutex_unlock(&self->_.t_state.mtx);
                    return AAOS_EPWROFF;
                    break;
                case TELESCOPE_STATE_MALFUNCTION:
                    Pthread_mutex_unlock(&self->_.t_state.mtx);
                    return AAOS_EDEVMAL;
                    break;
                case TELESCOPE_STATE_UNINITIALIZED:
                    Pthread_mutex_unlock(&self->_.t_state.mtx);
                    return AAOS_EUNINIT;
                    break;
                case TELESCOPE_STATE_PARKED:
                case TELESCOPE_STATE_TRACKING:
                    break;
                default:
                    break;
            }
            break;
        case TELESCOPE_STATE_POWERED_OFF:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EPWROFF;
            break;
        case TELESCOPE_STATE_MALFUNCTION:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EDEVMAL;
            break;
        case TELESCOPE_STATE_UNINITIALIZED:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EUNINIT;
            break;
        case TELESCOPE_STATE_PARKED:
            break;
        case TELESCOPE_STATE_TRACKING:
            break;
        default:
            break;
    }

    self->_.last_move_begin_time = get_current_time();
    self->_.move_duration = duration;
    Pthread_create(&tid, NULL, ASCOMMount_do_move, &arg);
    self->_.tid = tid;
    self->_.t_state.state = TELESCOPE_STATE_MOVING | flag;
    Pthread_mutex_unlock(&self->_.t_state.mtx);
    
    void *value;
    
    Pthread_join(tid, &value);
    
    Pthread_mutex_lock(&self->_.t_state.mtx);
    if (value == NULL) {
        flag = self->_.t_state.state & TELESCOPE_STATE_MALFUNCTION;
        if (arg.error_code != AAOS_OK) {
            self->_.t_state.state = TELESCOPE_STATE_MALFUNCTION | flag;
        } else {
            self->_.t_state.state = TELESCOPE_STATE_TRACKING | flag;
        }
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        Pthread_cond_broadcast(&self->_.t_state.cond);
        self->_.last_track_begin_time = get_current_time();
        return arg.error_code;
    } else if (value == PTHREAD_CANCELED) {
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        Pthread_cond_broadcast(&self->_.t_state.cond);
        return AAOS_ECANCELED;
    }
    Pthread_mutex_unlock(&self->_.t_state.mtx);
    Pthread_cond_broadcast(&self->_.t_state.cond);
    return AAOS_ERROR;
}

static int
ASCOMMount_try_move(void *_self, unsigned int direction, double duration)
{
    struct ASCOMMount *self = cast(ASCOMMount(), _self);
    
    pthread_t tid;
    
    int guide_direction, guide_duration;

    switch (direction) {
        case TELESCOPE_MOVE_EAST:
            guide_direction = 2;
            break;
        case TELESCOPE_MOVE_WEST:
            guide_direction = 3;
            break;
        case TELESCOPE_MOVE_NORTH:
            guide_direction = 0;
            break;
        case TELESCOPE_MOVE_SOUTH:
            guide_direction = 1;
            break;
        default:
            return AAOS_EINVAL;
            break;
    }
    
    if (duration < 0) {
        return AAOS_EINVAL;
    }
    
    guide_duration = (int) duration * 1000;
    
    struct ASCOMMoveThreadArg arg;
    
    arg.self = self;
    arg.guide_direction = guide_direction;
    arg.guide_duration = guide_duration;
    
    if (duration < 0) {
        return AAOS_EINVAL;
    }
    
    if (Pthread_mutex_trylock(&self->_.t_state.mtx) != 0) {
        return AAOS_EBUSY;
    }
    unsigned int state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
    unsigned int flag = self->_.t_state.state & TELESCOPE_STATE_MALFUNCTION;
    if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        return AAOS_EDEVMAL;
    }
    switch (state) {
        case TELESCOPE_STATE_SLEWING:
        case TELESCOPE_STATE_MOVING:
        case TELESCOPE_STATE_TRACKING_WAIT:
            return AAOS_EBUSY;
            break;
        case TELESCOPE_STATE_POWERED_OFF:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EPWROFF;
            break;
        case TELESCOPE_STATE_MALFUNCTION:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EDEVMAL;
            break;
        case TELESCOPE_STATE_UNINITIALIZED:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EUNINIT;
            break;
        case TELESCOPE_STATE_PARKED:
            break;
        case TELESCOPE_STATE_TRACKING:
            break;
        default:
            break;
    }
    
    self->_.last_move_begin_time = get_current_time();
    self->_.move_duration = duration;
    Pthread_create(&tid, NULL, ASCOMMount_do_move, &arg);
    self->_.tid = tid;
    self->_.t_state.state = TELESCOPE_STATE_MOVING | flag;
    Pthread_mutex_unlock(&self->_.t_state.mtx);
    
    void *value;
    
    Pthread_join(tid, &value);
    
    Pthread_mutex_lock(&self->_.t_state.mtx);
    if (value == NULL) {
        flag = self->_.t_state.state & TELESCOPE_STATE_MALFUNCTION;
        if (arg.error_code != AAOS_OK) {
            self->_.t_state.state = TELESCOPE_STATE_MALFUNCTION | flag;
        } else {
            self->_.t_state.state = TELESCOPE_STATE_TRACKING | flag;
        }
        self->_.last_track_begin_time = get_current_time();
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        Pthread_cond_broadcast(&self->_.t_state.cond);
        return AAOS_OK;
    } else if (value == PTHREAD_CANCELED) {
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        Pthread_cond_broadcast(&self->_.t_state.cond);
        return AAOS_ECANCELED;
    }
    Pthread_mutex_unlock(&self->_.t_state.mtx);
    Pthread_cond_broadcast(&self->_.t_state.cond);
    return AAOS_ERROR;
}

static int
ASCOMMount_timed_move(void *_self, unsigned int direction, double duration, double timeout)
{
    struct ASCOMMount *self = cast(ASCOMMount(), _self);
    
    pthread_t tid;
    
    struct timespec tp;
    int guide_direction, guide_duration;
    
    if (timeout <= 0.) {
        return AAOS_EINVAL;
    }
    timeout += get_current_time();
    tp.tv_sec = floor(timeout);
    tp.tv_nsec = (timeout - tp.tv_sec) * 1000000000.;
    
    switch (direction) {
        case TELESCOPE_MOVE_EAST:
            guide_direction = 2;
            break;
        case TELESCOPE_MOVE_WEST:
            guide_direction = 3;
            break;
        case TELESCOPE_MOVE_NORTH:
            guide_direction = 0;
            break;
        case TELESCOPE_MOVE_SOUTH:
            guide_direction = 1;
            break;
        default:
            return AAOS_EINVAL;
            break;
    }
    
    if (duration < 0) {
        return AAOS_EINVAL;
    }

    guide_duration = (int) duration * 1000;
    
    struct ASCOMMoveThreadArg arg;
    
    arg.self = self;
    arg.guide_direction = guide_direction;
    arg.guide_duration = guide_duration;
    
    Pthread_mutex_lock(&self->_.t_state.mtx);
    unsigned int state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
    unsigned int flag = self->_.t_state.state & TELESCOPE_STATE_MALFUNCTION;
    if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        return AAOS_EDEVMAL;
    }
    switch (state) {
        case TELESCOPE_STATE_SLEWING:
        case TELESCOPE_STATE_MOVING:
        case TELESCOPE_STATE_TRACKING_WAIT:
            while (state == TELESCOPE_STATE_MOVING || state == TELESCOPE_STATE_SLEWING || state == TELESCOPE_STATE_TRACKING_WAIT) {
                Pthread_cond_timedwait(&self->_.t_state.cond, &self->_.t_state.mtx, &tp);
                state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
                flag = state & TELESCOPE_STATE_MALFUNCTION;
            }
            if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
                Pthread_mutex_unlock(&self->_.t_state.mtx);
                return AAOS_EDEVMAL;
            }
            switch (state) {
                case TELESCOPE_STATE_POWERED_OFF:
                    Pthread_mutex_unlock(&self->_.t_state.mtx);
                    return AAOS_EPWROFF;
                    break;
                case TELESCOPE_STATE_MALFUNCTION:
                    Pthread_mutex_unlock(&self->_.t_state.mtx);
                    return AAOS_EDEVMAL;
                    break;
                case TELESCOPE_STATE_UNINITIALIZED:
                    Pthread_mutex_unlock(&self->_.t_state.mtx);
                    return AAOS_EUNINIT;
                    break;
                case TELESCOPE_STATE_PARKED:
                case TELESCOPE_STATE_TRACKING:
                    break;
                default:
                    break;
            }
            break;
        case TELESCOPE_STATE_POWERED_OFF:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EPWROFF;
            break;
        case TELESCOPE_STATE_MALFUNCTION:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EDEVMAL;
            break;
        case TELESCOPE_STATE_UNINITIALIZED:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EUNINIT;
            break;
        case TELESCOPE_STATE_PARKED:
            break;
        case TELESCOPE_STATE_TRACKING:
            break;
        default:
            break;
    }

    
    self->_.t_state.state = TELESCOPE_STATE_MOVING | flag;
    self->_.last_move_begin_time = get_current_time();
    self->_.move_duration = duration;
    Pthread_create(&tid, NULL, ASCOMMount_do_move, &arg);
    self->_.tid = tid;
    Pthread_mutex_unlock(&self->_.t_state.mtx);
    
    void *value;
    
    Pthread_join(tid, &value);
    
    Pthread_mutex_lock(&self->_.t_state.mtx);
    if (value == NULL) {
        flag = self->_.t_state.state & TELESCOPE_STATE_MALFUNCTION;
        if (arg.error_code != AAOS_OK) {
            self->_.t_state.state = TELESCOPE_STATE_MALFUNCTION | flag;
        } else {
            self->_.t_state.state = TELESCOPE_STATE_TRACKING | flag;
        }
        self->_.last_track_begin_time = get_current_time();
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        Pthread_cond_broadcast(&self->_.t_state.cond);
        return AAOS_OK;
    } else if (value == PTHREAD_CANCELED) {
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        Pthread_cond_broadcast(&self->_.t_state.cond);
        return AAOS_ECANCELED;
    }
    Pthread_mutex_unlock(&self->_.t_state.mtx);
    Pthread_cond_broadcast(&self->_.t_state.cond);
    return AAOS_ERROR;
}

struct ASCOMSlewThreadArg {
    struct ASCOMMount *self;
    double ra;
    double dec;
    int error_code;
};

static int
ASCOMMount_is_slewing(void *ascom, bool *is_slewing)
{
    char *buf = NULL;
    size_t size = 0;
    int ret = AAOS_OK;
    
    if ((ret = ascom_put(ascom, "slewing", NULL, buf, &size)) != AAOS_OK) {
        goto error;
    }
    if (ascom_get_error_code(buf, &ret) != AAOS_OK || ret != AAOS_OK) {
        ret = AAOS_ERROR;
        goto error;
    }
    
error:
    free(buf);
    return 0;
}

static void *
ASCOMMount_do_slew(void *arg)
{
    struct ASCOMSlewThreadArg *my_arg = (struct ASCOMSlewThreadArg *) arg;
    char data[BUFSIZE], *buf = NULL;
    size_t size = 0;
    int ret;
   
    snprintf(data, BUFSIZE, "ra=%11.6f&dec=%11.6f", my_arg->ra, my_arg->dec);
    
    if (my_arg->self->can_slew_async) {
        if ((ret = ascom_put(my_arg->self->ascom, "slewcoordinatesasync", data, buf, &size)) != AAOS_OK) {
            my_arg->error_code = ret;
            goto error;
        }
        if (ascom_get_error_code(buf, &ret) != AAOS_OK || ret != AAOS_OK) {
            ret = AAOS_ERROR;
            goto error;
        }
        int count;
        bool is_slewing;
        for (count = 0; count < APMOUNT_MAX_POLL_COUNT; count++) {
            Nanosleep(APMOUNT_SLEW_WAIT_TIME);
            if (ASCOMMount_is_slewing(my_arg->self->ascom, &is_slewing) != AAOS_OK) {
                ret = AAOS_ERROR;
                goto error;
            }
            if (is_slewing) {
                break;
            }
        }
        ret = AAOS_ETIMEDOUT;
    } else {
        if ((ret = ascom_put(my_arg->self->ascom, "slewcoordinates", data, buf, &size)) != AAOS_OK) {
            my_arg->error_code = ret;
            goto error;
        }
        if (ascom_get_error_code(buf, &ret) != AAOS_OK || ret != AAOS_OK) {
            ret = AAOS_ERROR;
            goto error;
        }
        free(buf);
    }
    
error:
    free(buf);
    my_arg->error_code = ret;
    return NULL;
}

static int
ASCOMMount_slew(void *_self, double ra, double dec)
{
    struct ASCOMMount *self = cast(ASCOMMount(), _self);
    
    pthread_t tid;
    
    struct ASCOMSlewThreadArg arg;
    
    arg.self = self;
    arg.ra = ra;
    arg.dec = dec;
    
    Pthread_mutex_lock(&self->_.t_state.mtx);
    unsigned int state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
    unsigned int flag = self->_.t_state.state & TELESCOPE_STATE_MALFUNCTION;
    if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        return AAOS_EDEVMAL;
    }
    switch (state) {
        case TELESCOPE_STATE_SLEWING:
        case TELESCOPE_STATE_MOVING:
        case TELESCOPE_STATE_TRACKING_WAIT:
            if (self->_.option & TELESCOPE_OPTION_DO_NOT_WAIT && state != TELESCOPE_STATE_TRACKING_WAIT) {
                Pthread_cancel(self->_.tid);
                break;
            }
            while (state == TELESCOPE_STATE_MOVING || state == TELESCOPE_STATE_SLEWING || state == TELESCOPE_STATE_TRACKING_WAIT) {
                Pthread_cond_wait(&self->_.t_state.cond, &self->_.t_state.mtx);
                state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
                flag = state & TELESCOPE_STATE_MALFUNCTION;
            }
            if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
                Pthread_mutex_unlock(&self->_.t_state.mtx);
                return AAOS_EDEVMAL;
            }
            switch (state) {
                case TELESCOPE_STATE_POWERED_OFF:
                    Pthread_mutex_unlock(&self->_.t_state.mtx);
                    return AAOS_EPWROFF;
                    break;
                case TELESCOPE_STATE_MALFUNCTION:
                    Pthread_mutex_unlock(&self->_.t_state.mtx);
                    return AAOS_EDEVMAL;
                    break;
                case TELESCOPE_STATE_UNINITIALIZED:
                    Pthread_mutex_unlock(&self->_.t_state.mtx);
                    return AAOS_EUNINIT;
                    break;
                case TELESCOPE_STATE_PARKED:
                case TELESCOPE_STATE_TRACKING:
                    break;
                default:
                    break;
            }
            break;
        case TELESCOPE_STATE_POWERED_OFF:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EPWROFF;
            break;
        case TELESCOPE_STATE_MALFUNCTION:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EDEVMAL;
            break;
        case TELESCOPE_STATE_UNINITIALIZED:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EUNINIT;
            break;
        case TELESCOPE_STATE_PARKED:
            break;
        case TELESCOPE_STATE_TRACKING:
            break;
        default:
            break;
    }
    
    self->_.t_state.state = TELESCOPE_STATE_SLEWING | flag;
    self->_.last_slew_begin_time = get_current_time();
    Pthread_create(&tid, NULL, ASCOMMount_do_slew, &arg);
    self->_.tid = tid;
    Pthread_mutex_unlock(&self->_.t_state.mtx);
    
    void *value;
    
    Pthread_join(tid, &value);
    
    Pthread_mutex_lock(&self->_.t_state.mtx);
    if (value == NULL) {
        flag = self->_.t_state.state & TELESCOPE_STATE_MALFUNCTION;
        self->_.t_state.state = TELESCOPE_STATE_TRACKING | flag;
        if (arg.error_code != AAOS_OK) {
            self->_.t_state.state = TELESCOPE_STATE_MALFUNCTION | flag;
        } else {
            self->_.t_state.state = TELESCOPE_STATE_TRACKING | flag;
        }
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        Pthread_cond_broadcast(&self->_.t_state.cond);
        return arg.error_code;
    } else if (value == PTHREAD_CANCELED) {
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        Pthread_cond_broadcast(&self->_.t_state.cond);
        return AAOS_ECANCELED;
    }
    self->_.last_track_begin_time = get_current_time();
    Pthread_mutex_unlock(&self->_.t_state.mtx);
    Pthread_cond_broadcast(&self->_.t_state.cond);
    return AAOS_ERROR;
}

static int
ASCOMMount_try_slew(void *_self, double ra, double dec)
{
    struct ASCOMMount *self = cast(ASCOMMount(), _self);
    
    pthread_t tid;
    
    struct ASCOMSlewThreadArg arg;
    
    arg.self = self;
    arg.ra = ra;
    arg.dec = dec;
    
    if (Pthread_mutex_trylock(&self->_.t_state.mtx) != 0) {
        return AAOS_EBUSY;
    }
    unsigned int state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
    unsigned int flag = self->_.t_state.state & TELESCOPE_STATE_MALFUNCTION;
    if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        return AAOS_EDEVMAL;
    }
    switch (state) {
        case TELESCOPE_STATE_SLEWING:
        case TELESCOPE_STATE_MOVING:
        case TELESCOPE_STATE_TRACKING_WAIT:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EBUSY;
            break;
        case TELESCOPE_STATE_POWERED_OFF:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EPWROFF;
            break;
        case TELESCOPE_STATE_MALFUNCTION:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EDEVMAL;
            break;
        case TELESCOPE_STATE_UNINITIALIZED:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EUNINIT;
            break;
        case TELESCOPE_STATE_PARKED:
            break;
        case TELESCOPE_STATE_TRACKING:
            break;
        default:
            break;
    }
    
    self->_.t_state.state = TELESCOPE_STATE_SLEWING | flag;
    self->_.last_slew_begin_time = get_current_time();
    Pthread_create(&tid, NULL, ASCOMMount_do_slew, &arg);
    self->_.tid = tid;
    Pthread_mutex_unlock(&self->_.t_state.mtx);
    
    void *value;
    
    Pthread_join(tid, &value);
    
    Pthread_mutex_lock(&self->_.t_state.mtx);
    if (value == NULL) {
        flag = self->_.t_state.state & TELESCOPE_STATE_MALFUNCTION;
        self->_.t_state.state = TELESCOPE_STATE_TRACKING | flag;
        if (arg.error_code != AAOS_OK) {
            self->_.t_state.state = TELESCOPE_STATE_MALFUNCTION | flag;
        } else {
            self->_.t_state.state = TELESCOPE_STATE_TRACKING | flag;
        }
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        Pthread_cond_broadcast(&self->_.t_state.cond);
        return arg.error_code;
    } else if (value == PTHREAD_CANCELED) {
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        Pthread_cond_broadcast(&self->_.t_state.cond);
        return AAOS_ECANCELED;
    }
    self->_.last_track_begin_time = get_current_time();
    Pthread_mutex_unlock(&self->_.t_state.mtx);
    Pthread_cond_broadcast(&self->_.t_state.cond);
    return AAOS_ERROR;
}

static int
ASCOMMount_timed_slew(void *_self, double ra, double dec, double timeout)
{
    struct ASCOMMount *self = cast(ASCOMMount(), _self);
    
    int ret;
    struct timespec tp;
    pthread_t tid;
    struct ASCOMSlewThreadArg arg;
    
    arg.self = self;
    arg.ra = ra;
    arg.dec = dec;
    
    if (timeout <= 0.) {
        return AAOS_EINVAL;
    }
    
    timeout += get_current_time();
    tp.tv_sec = floor(timeout);
    tp.tv_nsec = (timeout - tp.tv_sec) * 1000000000.;
    
    Pthread_mutex_lock(&self->_.t_state.mtx);
    unsigned int state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
    unsigned int flag = self->_.t_state.state & TELESCOPE_STATE_MALFUNCTION;
    if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        return AAOS_EDEVMAL;
    }
    switch (state) {
        case TELESCOPE_STATE_SLEWING:
        case TELESCOPE_STATE_MOVING:
        case TELESCOPE_STATE_TRACKING_WAIT:
            while (state == TELESCOPE_STATE_MOVING || state == TELESCOPE_STATE_SLEWING || state == TELESCOPE_STATE_TRACKING_WAIT) {
                ret = Pthread_cond_timedwait(&self->_.t_state.cond, &self->_.t_state.mtx, &tp);
                if (ret == ETIMEDOUT) {
                    Pthread_mutex_unlock(&self->_.t_state.mtx);
                    return AAOS_ETIMEDOUT;
                }
                state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
                flag = state & TELESCOPE_STATE_MALFUNCTION;
            }
            if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
                Pthread_mutex_unlock(&self->_.t_state.mtx);
                return AAOS_EDEVMAL;
            }
            switch (state) {
                case TELESCOPE_STATE_POWERED_OFF:
                    Pthread_mutex_unlock(&self->_.t_state.mtx);
                    return AAOS_EPWROFF;
                    break;
                case TELESCOPE_STATE_MALFUNCTION:
                    Pthread_mutex_unlock(&self->_.t_state.mtx);
                    return AAOS_EDEVMAL;
                    break;
                case TELESCOPE_STATE_UNINITIALIZED:
                    Pthread_mutex_unlock(&self->_.t_state.mtx);
                    return AAOS_EUNINIT;
                    break;
                case TELESCOPE_STATE_PARKED:
                case TELESCOPE_STATE_TRACKING:
                    break;
                default:
                    break;
            }
            break;
        case TELESCOPE_STATE_POWERED_OFF:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EPWROFF;
            break;
        case TELESCOPE_STATE_MALFUNCTION:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EDEVMAL;
            break;
        case TELESCOPE_STATE_UNINITIALIZED:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EUNINIT;
            break;
        case TELESCOPE_STATE_PARKED:
            break;
        case TELESCOPE_STATE_TRACKING:
            break;
        default:
            break;
    }
    
    self->_.t_state.state = TELESCOPE_STATE_SLEWING | flag;
    self->_.last_slew_begin_time = get_current_time();
    Pthread_create(&tid, NULL, ASCOMMount_do_slew, self);
    self->_.tid = tid;
    Pthread_mutex_unlock(&self->_.t_state.mtx);
    
    void *value;
    
    Pthread_join(tid, &value);
    
    Pthread_mutex_lock(&self->_.t_state.mtx);
    if (value == NULL) {
        if (arg.error_code != AAOS_OK) {
            self->_.t_state.state = TELESCOPE_STATE_MALFUNCTION | flag;
        } else {
            self->_.t_state.state = TELESCOPE_STATE_TRACKING | flag;
        }
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        Pthread_cond_broadcast(&self->_.t_state.cond);
        return arg.error_code;
    } else if (value == PTHREAD_CANCELED) {
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        Pthread_cond_broadcast(&self->_.t_state.cond);
        return AAOS_ECANCELED;
    }
    self->_.last_track_begin_time = get_current_time();
    Pthread_mutex_unlock(&self->_.t_state.mtx);
    Pthread_cond_broadcast(&self->_.t_state.cond);
    return AAOS_ERROR;
}

static int
ASCOMMount_park(void *_self)
{
    struct ASCOMMount *self = cast(ASCOMMount(), _self);
    char *buf = NULL;
    size_t size = 0;
    int ret, error_code;
    
    if (!self->can_park) {
        return AAOS_ENOTSUP;
    }
    
    Pthread_mutex_lock(&self->_.t_state.mtx);
    unsigned int state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
    unsigned int flag = self->_.t_state.state & TELESCOPE_STATE_MALFUNCTION;
    if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
        self->_.t_state.state = TELESCOPE_STATE_PARKED | flag;
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        return AAOS_EDEVMAL;
    }
    if ((ret = ascom_put(self->ascom, "setpark", NULL, buf, &size)) != AAOS_OK) {
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        free(buf);
        return ret;
    }
    if ((ret = ascom_get_error_code(buf, &error_code)) != 0 && error_code != AAOS_OK) {
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        free(buf);
        if (ret != AAOS_OK) {
            return ret;
        }
        return error_code;
    }
    if ((ret = ascom_put(self->ascom, "park", NULL, buf, &size)) != AAOS_OK) {
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        free(buf);
        return ret;
    }
    if ((ret = ascom_get_error_code(buf, &error_code)) != 0 && error_code != AAOS_OK) {
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        free(buf);
        if (ret != AAOS_OK) {
            return ret;
        }
        return error_code;
    }
    self->_.last_park_begin_time = get_current_time();
    switch (state) {
        case TELESCOPE_STATE_TRACKING:
            self->_.t_state.state = TELESCOPE_STATE_PARKED | flag;
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_OK;
            break;
        case TELESCOPE_STATE_TRACKING_WAIT:
            self->_.t_state.state = TELESCOPE_STATE_PARKED | flag;
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            Pthread_cond_broadcast(&self->_.t_state.cond);
            return AAOS_OK;
            break;
        case TELESCOPE_STATE_UNINITIALIZED:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EUNINIT;
            break;
        case TELESCOPE_STATE_POWERED_OFF:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_EPWROFF;
        case TELESCOPE_STATE_PARKED:
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            return AAOS_OK;
        case TELESCOPE_STATE_MOVING:
        case TELESCOPE_STATE_SLEWING:
            Pthread_cancel(self->_.tid);
            self->_.t_state.state = TELESCOPE_STATE_PARKED | flag;
            Pthread_mutex_unlock(&self->_.t_state.mtx);
            Pthread_cond_broadcast(&self->_.t_state.cond);
            return AAOS_OK;
        default:
            break;
    }
    free(buf);
    return AAOS_OK;
}

static int
ASCOMMount_park_off(void *_self)
{
    struct ASCOMMount *self = cast(ASCOMMount(), _self);
    char *buf = NULL;
    size_t size = 0;
    int ret, error_code;
   
    if (!self->can_park) {
        return AAOS_ENOTSUP;
    }
    
    Pthread_mutex_lock(&self->_.t_state.mtx);
    unsigned int state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
    unsigned int flag = self->_.t_state.state & TELESCOPE_STATE_MALFUNCTION;
    if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        return AAOS_EDEVMAL;
    }
    switch (state) {
        case TELESCOPE_STATE_PARKED:
            if ((ret = ascom_put(self->ascom, "unpark", NULL, buf, &size)) != AAOS_OK) {
                Pthread_mutex_unlock(&self->_.t_state.mtx);
                free(buf);
                return ret;
            }
            if ((ret = ascom_get_error_code(buf, &error_code)) != 0 && error_code != AAOS_OK) {
                Pthread_mutex_unlock(&self->_.t_state.mtx);
                free(buf);
                if (ret != AAOS_OK) {
                    return ret;
                }
                return error_code;
            }
            
            self->_.t_state.state = TELESCOPE_STATE_TRACKING | flag;
            break;
        default:
            break;
    }
    Pthread_mutex_unlock(&self->_.t_state.mtx);
    free(buf);
    return AAOS_OK;
}

static int
ASCOMMount_go_home(void *_self)
{
    int ret;
    
    if ((ret = ASCOMMount_slew(_self, 359.54, 89.8)) != AAOS_OK) {
        return ret;
    }
    
    return ASCOMMount_park(_self);
}

static int
ASCOMMount_set_move_speed(void *_self, double move_speed)
{
    struct ASCOMMount *self = cast(ASCOMMount(), _self);
    char *buf = NULL;
    size_t size;
    int ret, error_code;
    
    
    if (move_speed < 0. || move_speed > SIDEREAL_TRACKING_SPEED * 1200.) {
        return AAOS_EINVAL;
    }
    
    Pthread_mutex_lock(&self->_.t_state.mtx);
    unsigned int state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
    unsigned int flag = self->_.t_state.state & TELESCOPE_STATE_MALFUNCTION;
    if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        return AAOS_EDEVMAL;
    }
    switch (state) {
        case TELESCOPE_STATE_POWERED_OFF:
            ret = AAOS_EPWROFF;
            break;
        case TELESCOPE_STATE_UNINITIALIZED:
            ret = AAOS_EUNINIT;
            break;
        case TELESCOPE_STATE_MOVING:
            while (state == TELESCOPE_STATE_MOVING) {
                Pthread_cond_wait(&self->_.t_state.cond, &self->_.t_state.mtx);
                state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
            }
            switch (state) {
                case TELESCOPE_STATE_POWERED_OFF:
                    ret = AAOS_EPWROFF;
                    break;
                case TELESCOPE_STATE_UNINITIALIZED:
                    ret = AAOS_EUNINIT;
                    break;
                default:
                    ret = AAOS_OK;
                    if ((ret = ascom_put(self->ascom, "guideratedeclination", NULL, buf, &size)) != AAOS_OK) {
                        Pthread_mutex_unlock(&self->_.t_state.mtx);
                        free(buf);
                        return ret;
                    }
                    if ((ret = ascom_get_error_code(buf, &error_code)) != 0 && error_code != AAOS_OK) {
                        Pthread_mutex_unlock(&self->_.t_state.mtx);
                        free(buf);
                        if (ret != AAOS_OK) {
                            return ret;
                        }
                        return error_code;
                    }
                    if ((ret = ascom_put(self->ascom, "guideraterightascension", NULL, buf, &size)) != AAOS_OK) {
                        Pthread_mutex_unlock(&self->_.t_state.mtx);
                        free(buf);
                        return ret;
                    }
                    if ((ret = ascom_get_error_code(buf, &error_code)) != 0 && error_code != AAOS_OK) {
                        Pthread_mutex_unlock(&self->_.t_state.mtx);
                        free(buf);
                        if (ret != AAOS_OK) {
                            return ret;
                        }
                        return error_code;
                    }
                    self->_.move_speed = move_speed;
                    break;
            }
            break;
        default:
            ret = AAOS_OK;
            if ((ret = ascom_put(self->ascom, "guideratedeclination", NULL, buf, &size)) != AAOS_OK) {
                Pthread_mutex_unlock(&self->_.t_state.mtx);
                free(buf);
                return ret;
            }
            if ((ret = ascom_get_error_code(buf, &error_code)) != 0 && error_code != AAOS_OK) {
                Pthread_mutex_unlock(&self->_.t_state.mtx);
                free(buf);
                if (ret != AAOS_OK) {
                    return ret;
                }
                return error_code;
            }
            if ((ret = ascom_put(self->ascom, "guideraterightascension", NULL, buf, &size)) != AAOS_OK) {
                Pthread_mutex_unlock(&self->_.t_state.mtx);
                free(buf);
                return ret;
            }
            if ((ret = ascom_get_error_code(buf, &error_code)) != 0 && error_code != AAOS_OK) {
                Pthread_mutex_unlock(&self->_.t_state.mtx);
                free(buf);
                if (ret != AAOS_OK) {
                    return ret;
                }
                return error_code;
            }
            self->_.move_speed = move_speed;
            break;
    }
    Pthread_mutex_unlock(&self->_.t_state.mtx);
    
    free(buf);
    return ret;
}

static int
ASCOMMount_get_move_speed(void *_self, double *move_speed)
{
    struct ASCOMMount *self = cast(ASCOMMount(), _self);
    
    int ret;
    
    Pthread_mutex_lock(&self->_.t_state.mtx);
    unsigned int state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
    unsigned int flag = self->_.t_state.state & TELESCOPE_STATE_MALFUNCTION;
    if (flag) {
        ret = AAOS_EDEVMAL;
        goto error;
    }
    switch (state) {
        case TELESCOPE_STATE_POWERED_OFF:
            ret = AAOS_EPWROFF;
            break;
        case TELESCOPE_STATE_UNINITIALIZED:
            ret = AAOS_EUNINIT;
            break;
        default:
            ret = AAOS_OK;
            *move_speed = self->_.move_speed;
            break;
    }
error:
    Pthread_mutex_unlock(&self->_.t_state.mtx);
    
    return ret;
}

static int
ASCOMMount_set_slew_speed(void *_self, double slew_speed_x, double slew_speed_y)
{
    struct ASCOMMount *self = cast(ASCOMMount(), _self);
    char *buf = NULL;
    size_t size;
    int ret, error_code;
    
    Pthread_mutex_lock(&self->_.t_state.mtx);
    unsigned int state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
    unsigned int flag = self->_.t_state.state & TELESCOPE_STATE_MALFUNCTION;
    if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        return AAOS_EDEVMAL;
    }
    switch (state) {
        case TELESCOPE_STATE_POWERED_OFF:
            ret = AAOS_EPWROFF;
            break;
        case TELESCOPE_STATE_UNINITIALIZED:
            ret = AAOS_EUNINIT;
            break;
        case TELESCOPE_STATE_MALFUNCTION:
            ret = AAOS_EDEVMAL;
            break;
        case TELESCOPE_STATE_MOVING:
            while (state == TELESCOPE_STATE_SLEWING) {
                Pthread_cond_wait(&self->_.t_state.cond, &self->_.t_state.mtx);
                state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
                flag = self->_.t_state.state & TELESCOPE_STATE_MALFUNCTION;
            }
            if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
                Pthread_mutex_unlock(&self->_.t_state.mtx);
                return AAOS_EDEVMAL;
            }
            switch (state) {
                case TELESCOPE_STATE_POWERED_OFF:
                    ret = AAOS_EPWROFF;
                    break;
                case TELESCOPE_STATE_UNINITIALIZED:
                    ret = AAOS_EUNINIT;
                    break;
                default:
                    ret = AAOS_OK;
                    if ((ret = ascom_put(self->ascom, "guideratedeclination", NULL, buf, &size)) != AAOS_OK) {
                        Pthread_mutex_unlock(&self->_.t_state.mtx);
                        free(buf);
                        return ret;
                    }
                    if ((ret = ascom_get_error_code(buf, &error_code)) != 0 && error_code != AAOS_OK) {
                        Pthread_mutex_unlock(&self->_.t_state.mtx);
                        free(buf);
                        if (ret != AAOS_OK) {
                            return ret;
                        }
                        return error_code;
                    }
                    if ((ret = ascom_put(self->ascom, "guideraterightascension", NULL, buf, &size)) != AAOS_OK) {
                        Pthread_mutex_unlock(&self->_.t_state.mtx);
                        free(buf);
                        return ret;
                    }
                    if ((ret = ascom_get_error_code(buf, &error_code)) != 0 && error_code != AAOS_OK) {
                        Pthread_mutex_unlock(&self->_.t_state.mtx);
                        free(buf);
                        if (ret != AAOS_OK) {
                            return ret;
                        }
                        return error_code;
                    }
                    self->_.slew_speed_x = slew_speed_x;
                    self->_.slew_speed_y = slew_speed_y;
                    break;
            }
            break;
        default:
            ret = AAOS_OK;
            if ((ret = ascom_put(self->ascom, "guideratedeclination", NULL, buf, &size)) != AAOS_OK) {
                Pthread_mutex_unlock(&self->_.t_state.mtx);
                free(buf);
                return ret;
            }
            if ((ret = ascom_get_error_code(buf, &error_code)) != 0 && error_code != AAOS_OK) {
                Pthread_mutex_unlock(&self->_.t_state.mtx);
                free(buf);
                if (ret != AAOS_OK) {
                    return ret;
                }
                return error_code;
            }
            if ((ret = ascom_put(self->ascom, "guideraterightascension", NULL, buf, &size)) != AAOS_OK) {
                Pthread_mutex_unlock(&self->_.t_state.mtx);
                free(buf);
                return ret;
            }
            if ((ret = ascom_get_error_code(buf, &error_code)) != 0 && error_code != AAOS_OK) {
                Pthread_mutex_unlock(&self->_.t_state.mtx);
                free(buf);
                if (ret != AAOS_OK) {
                    return ret;
                }
                return error_code;
            }
            self->_.slew_speed_x = slew_speed_x;
            self->_.slew_speed_y = slew_speed_y;
            break;
    }
    Pthread_mutex_unlock(&self->_.t_state.mtx);
    
    return ret;
}

static int
ASCOMMount_get_slew_speed(void *_self, double *slew_speed_x, double *slew_speed_y)
{
    struct ASCOMMount *self = cast(ASCOMMount(), _self);
    
    int ret;
    
    Pthread_mutex_lock(&self->_.t_state.mtx);
    unsigned int state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
    unsigned int flag = self->_.t_state.state & TELESCOPE_STATE_MALFUNCTION;
    if (flag) {
        ret = AAOS_EDEVMAL;
        goto error;
    }
    switch (state) {
        case TELESCOPE_STATE_POWERED_OFF:
            ret = AAOS_EPWROFF;
            break;
        case TELESCOPE_STATE_UNINITIALIZED:
            ret = AAOS_EUNINIT;
            break;
        default:
            ret = AAOS_OK;
            *slew_speed_x = self->_.slew_speed_x;
            *slew_speed_y = self->_.slew_speed_y;
            break;
    }
error:
    Pthread_mutex_unlock(&self->_.t_state.mtx);
    
    return ret;
}

static int
ASCOMMount_set_track_rate(void *_self, double track_rate_x, double track_rate_y)
{
    struct ASCOMMount *self = cast(ASCOMMount(), _self);
    
    char *buf = NULL;
    size_t size;
    int ret, error_code;
    
    
    Pthread_mutex_lock(&self->_.t_state.mtx);
    unsigned int state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
    unsigned int flag = self->_.t_state.state & TELESCOPE_STATE_MALFUNCTION;
    if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        return AAOS_EDEVMAL;
    }
    switch (state) {
        case TELESCOPE_STATE_POWERED_OFF:
            ret = AAOS_EPWROFF;
            break;
        case TELESCOPE_STATE_UNINITIALIZED:
            ret = AAOS_EUNINIT;
            break;
        case TELESCOPE_STATE_MOVING:
            while (state == TELESCOPE_STATE_SLEWING) {
                Pthread_cond_wait(&self->_.t_state.cond, &self->_.t_state.mtx);
                state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
                flag = self->_.t_state.state & TELESCOPE_STATE_MALFUNCTION;
            }
            if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
                Pthread_mutex_unlock(&self->_.t_state.mtx);
                return AAOS_EDEVMAL;
            }
            switch (state) {
                case TELESCOPE_STATE_POWERED_OFF:
                    ret = AAOS_EPWROFF;
                    break;
                case TELESCOPE_STATE_UNINITIALIZED:
                    ret = AAOS_EUNINIT;
                    break;
                default:
                    ret = AAOS_OK;
                    if ((ret = ascom_put(self->ascom, "declinationrate", NULL, buf, &size)) != AAOS_OK) {
                        Pthread_mutex_unlock(&self->_.t_state.mtx);
                        free(buf);
                        return ret;
                    }
                    if ((ret = ascom_get_error_code(buf, &error_code)) != 0 && error_code != AAOS_OK) {
                        Pthread_mutex_unlock(&self->_.t_state.mtx);
                        free(buf);
                        if (ret != AAOS_OK) {
                            return ret;
                        }
                        return error_code;
                    }
                    if ((ret = ascom_put(self->ascom, "rightascensionrate", NULL, buf, &size)) != AAOS_OK) {
                        Pthread_mutex_unlock(&self->_.t_state.mtx);
                        free(buf);
                        return ret;
                    }
                    if ((ret = ascom_get_error_code(buf, &error_code)) != 0 && error_code != AAOS_OK) {
                        Pthread_mutex_unlock(&self->_.t_state.mtx);
                        free(buf);
                        if (ret != AAOS_OK) {
                            return ret;
                        }
                        return error_code;
                    }
                    self->_.track_rate_x = track_rate_x;
                    self->_.track_rate_y = track_rate_y;
                    break;
            }
            break;
        default:
            ret = AAOS_OK;
            if ((ret = ascom_put(self->ascom, "declinationrate", NULL, buf, &size)) != AAOS_OK) {
                Pthread_mutex_unlock(&self->_.t_state.mtx);
                free(buf);
                return ret;
            }
            if ((ret = ascom_get_error_code(buf, &error_code)) != 0 && error_code != AAOS_OK) {
                Pthread_mutex_unlock(&self->_.t_state.mtx);
                free(buf);
                if (ret != AAOS_OK) {
                    return ret;
                }
                return error_code;
            }
            if ((ret = ascom_put(self->ascom, "rightascensionrate", NULL, buf, &size)) != AAOS_OK) {
                Pthread_mutex_unlock(&self->_.t_state.mtx);
                free(buf);
                return ret;
            }
            if ((ret = ascom_get_error_code(buf, &error_code)) != 0 && error_code != AAOS_OK) {
                Pthread_mutex_unlock(&self->_.t_state.mtx);
                free(buf);
                if (ret != AAOS_OK) {
                    return ret;
                }
                return error_code;
            }
            self->_.track_rate_x = track_rate_x;
            self->_.track_rate_y = track_rate_y;

            break;
    }
    Pthread_mutex_unlock(&self->_.t_state.mtx);
    
    return ret;
}

static int
ASCOMMount_get_track_rate(void *_self, double *track_rate_x, double *track_rate_y)
{
    struct ASCOMMount *self = cast(ASCOMMount(), _self);
    
    int ret;
    
    Pthread_mutex_lock(&self->_.t_state.mtx);
    unsigned int state = self->_.t_state.state & (~TELESCOPE_STATE_MALFUNCTION);
    unsigned int flag = self->_.t_state.state & TELESCOPE_STATE_MALFUNCTION;
    if (!(self->_.option&TELESCOPE_OPTION_IGNORE_MALFUNCTION) && flag) {
        Pthread_mutex_unlock(&self->_.t_state.mtx);
        return AAOS_EDEVMAL;
    }
    switch (state) {
        case TELESCOPE_STATE_POWERED_OFF:
            ret = AAOS_EPWROFF;
            break;
        case TELESCOPE_STATE_UNINITIALIZED:
            ret = AAOS_EUNINIT;
            break;
        default:
            ret = AAOS_OK;
            *track_rate_x = self->_.track_rate_x;
            *track_rate_y = self->_.track_rate_y;
            
            break;
    }
    Pthread_mutex_unlock(&self->_.t_state.mtx);
    
    return ret;
}

static int
ASCOMMount_raw(void *_self, const void *raw_command, size_t size, size_t *write_size, void *results, size_t results_size, size_t *return_size)
{
    
    return AAOS_ENOTSUP;
}

static const void *ascom_mount_virtual_table(void);

static void *
ASCOMMount_ctor(void *_self, va_list *app)
{
    struct ASCOMMount *self = super_ctor(ASCOMMount(), _self, app);
    
    const char *address, *port, *version;
    unsigned int device_number;
    char *buf = NULL;
    size_t size = 0;
    
    address = va_arg(*app, const char *);
    port = va_arg(*app, const char *);
    version = va_arg(*app, const char *);
    device_number = va_arg(*app, unsigned int);
    
    self->ascom = new(ASCOM(), address, port, version, "telescope", device_number);
    
    ascom_get(self->ascom, "canpark", buf, &size);
    ascom_get_bool_value(buf, &self->can_park);
    ascom_get(self->ascom, "canslew", buf, &size);
    ascom_get_bool_value(buf, &self->can_slew_sync);
    ascom_get(self->ascom, "canslewasync", buf, &size);
    ascom_get_bool_value(buf, &self->can_slew_async);
    
    self->_._vtab = ascom_mount_virtual_table();
    
    return (void *) self;
}


static void *
ASCOMMount_dtor(void *_self)
{
    struct ASCOMMount *self = cast(ASCOMMount(), _self);
    
    delete(self->ascom);
    
    return super_dtor(ASCOMMount(), _self);
}

static void *
ASCOMMountClass_ctor(void *_self, va_list *app)
{
    struct ASCOMMountClass *self = super_ctor(ASCOMMountClass(), _self, app);
    
    self->_.raw.method = (Method) 0;
    
    return self;
}

static const void *_ASCOMMountClass;

static void
ASCOMMountClass_destroy(void)
{
    free((void *)_ASCOMMountClass);
}

static void
ASCOMMountClass_initialize(void)
{
    _ASCOMMountClass = new(__TelescopeClass(), "ASCOMMountClass", __TelescopeClass(), sizeof(struct ASCOMMountClass),
                        ctor, "", ASCOMMountClass_ctor,
                        (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(ASCOMMountClass_destroy);
#endif
}

const void *
ASCOMMountClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, ASCOMMountClass_initialize);
#endif
    
    return _ASCOMMountClass;
}

static const void *_ASCOMMount;

static void
ASCOMMount_destroy(void)
{
    free((void *)_ASCOMMount);
}

static void
ASCOMMount_initialize(void)
{
    _ASCOMMount = new(ASCOMMountClass(), "ASCOMMount", __Telescope(), sizeof(struct ASCOMMount),
                   ctor, "ctor", ASCOMMount_ctor,
                   dtor, "dtor", ASCOMMount_dtor,
                   (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(ASCOMMount_destroy);
#endif
}

const void *
ASCOMMount(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, ASCOMMount_initialize);
#endif
    
    return _ASCOMMount;
}

const static void * _ascom_mount_virtual_table;

static void
ascom_mount_virtual_table_destroy(void)
{
    delete((void *) _ascom_mount_virtual_table);
}

static void
ascom_mount_virtual_table_initialize(void)
{
    _ascom_mount_virtual_table = new(__TelescopeVirtualTable(),
                                     //__telescope_status, "status", _status,
                                     __telescope_power_on, "power_on", ASCOMMount_power_on,
                                     __telescope_power_off, "power_off", ASCOMMount_power_off,
                                     __telescope_init, "init", ASCOMMount_init,
                                     __telescope_park, "park", ASCOMMount_park,
                                     __telescope_park_off, "park_off", ASCOMMount_park_off,
                                     __telescope_stop, "stop", ASCOMMount_stop,
                                     __telescope_go_home, "go_home", ASCOMMount_go_home,
                                     __telescope_move, "move", ASCOMMount_move,
                                     __telescope_try_move, "try_move", ASCOMMount_try_move,
                                     __telescope_timed_move, "timed_move", ASCOMMount_timed_move,
                                     __telescope_slew, "slew", ASCOMMount_slew,
                                     __telescope_try_slew, "try_slew", ASCOMMount_try_slew,
                                     __telescope_timed_slew, "timed_slew", ASCOMMount_timed_slew,
                                     __telescope_set_move_speed, "set_move_speed", ASCOMMount_set_move_speed,
                                     __telescope_get_move_speed, "get_move_speed", ASCOMMount_get_move_speed,
                                     __telescope_set_slew_speed, "set_slew_speed", ASCOMMount_set_slew_speed,
                                     __telescope_get_slew_speed, "get_slew_speed", ASCOMMount_get_slew_speed,
                                     __telescope_set_track_rate, "set_track_rate", ASCOMMount_set_track_rate,
                                     __telescope_get_track_rate, "get_track_rate", ASCOMMount_get_track_rate,
                                     __telescope_raw, "raw", ASCOMMount_raw,
                                  (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(ascom_mount_virtual_table_destroy);
#endif
}

static const void *
ascom_mount_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, ascom_mount_virtual_table_initialize);
#endif
    
    return _ascom_mount_virtual_table;
}
 */

#endif

#ifdef _USE_COMPILER_ATTRIBUTION_

static void __destructor__(void) __attribute__ ((destructor(_TELESCOPE_PRIORITY_)));

static void
__destructor__(void)
{
    ASCOMMount_destroy();
    ASCOMMountClass_destroy();
    ascom_mount_virtual_table_destroy();
    
    APMount_destroy();
    APMountClass_destroy();
    ap_mount_virtual_table_destroy();
    
    VirtualTelescope_destroy();
    VirtualTelescopeClass_destroy();
    virtual_telescope_virtual_table_destroy();
    
    __Telescope_destroy();
    __TelescopeClass_destroy();
    
    __TelescopeVirtualTable_destroy();
}

static void __constructor__(void) __attribute__ ((constructor(_TELESCOPE_PRIORITY_)));

static void
__constructor__(void)
{
    __TelescopeVirtualTable_initialize();
    
    __TelescopeClass_initialize();
    __Telescope_initialize();
    
    virtual_telescope_virtual_table();
    VirtualTelescopeClass_initializ();
    VirtualTelescope_initialize();
    
    ap_mount_virtual_table();
    APMountClass_initializ();
    APMount_initialize();
    
    ascom_mount_virtual_table();
    ASCOMMountClass_initializ();
    ASCOMMount_initialize();
}
#endif
