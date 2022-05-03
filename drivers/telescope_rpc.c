//
//  telescope_rpc.c
//  AAOS
//
//  Created by Hu Yi on 2019/7/9.
//  Copyright © 2019 NAOC. All rights reserved.
//

#include "def.h"
#include "telescope.h"
#include "telescope_rpc_r.h"
#include "telescope_rpc.h"
#include "wrapper.h"

void **telescopes;
size_t n_telescope;

inline static int
get_index_by_name(const char *name, int *index)
{
    size_t i;
    const char *s;
    
    for (i = 0; i < n_telescope; i++) {
        s = __telescope_get_name(telescopes[i]);
        if (strcmp(name, s) == 0) {
            *index = (int) i + 1;
            return AAOS_OK;
        }
    }
    return AAOS_ENOTFOUND;
}

inline static void *
get_telescope_by_index(int index)
{
    if (index > 0 && index <= n_telescope) {
        return telescopes[index - 1];
    } else {
        return NULL;
    }
}

inline static int
Telescope_protocol_check(void *_self)
{
    uint16_t protocol;
    protobuf_get(_self, PACKET_PROTOCOL, &protocol);
    if (protocol != PROTO_TELESCOPE) {
        protobuf_set(_self, PACKET_ERRORCODE, AAOS_EPROTOWRONG);
        protobuf_set(_self, PACKET_LENGTH, 0);
        return AAOS_EPROTOWRONG;
    } else {
        return AAOS_OK;
    }
}

int
telescope_get_index_by_name(void *_self, const char *name)
{
    const struct TelescopeClass *class = (const struct TelescopeClass *) classOf(_self);
    
    if (isOf(class, TelescopeClass()) && class->get_index_by_name.method) {
        return ((int (*)(void *, const char *)) class->get_index_by_name.method)(_self, name);
    } else {
        int result;
        forward(_self, &result, (Method) telescope_get_index_by_name, "get_index_by_name", _self, name);
        return result;
    }
}

static int
Telescope_get_index_by_name(void *_self, const char *name)
{
    struct Telescope *self = cast(Telescope(), _self);
    
    size_t length;
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_TELESCOPE);
    protobuf_set(self, PACKET_COMMAND, TELESCOPE_COMMAND_GET_INDEX_BY_NAME);
    
    length = strlen(name);
    if (length < PACKETPARAMETERSIZE) {
        char *s;
        protobuf_get(self, PACKET_STR, &s);
        if (s != name) {
            snprintf(s, PACKETPARAMETERSIZE, "%s", name);
        }
        protobuf_set(self, PACKET_LENGTH, 0);
    } else {
        char *buf;
        protobuf_get(self, PACKET_BUF, &buf, NULL);
        if (name != buf) {
            protobuf_set(self, PACKET_BUF, name, length + 1);
        }
        uint32_t len = (uint32_t) length + 1;
        protobuf_set(self, PACKET_LENGTH, len);
    }
    
    return rpc_call(self);
}

int
telescope_status(void *_self, char *res, size_t res_size, size_t *res_len)
{
    const struct TelescopeClass *class = (const struct TelescopeClass *) classOf(_self);
    
    if (isOf(class, TelescopeClass()) && class->status.method) {
        return ((int (*)(void *, char *, size_t, size_t *)) class->status.method)(_self, res, res_size, res_len);
    } else {
        int result;
        forward(_self, &result, (Method) telescope_status, "status", _self, res, res_size, res_len);
        return result;
    }
}

static int
Telescope_status(void *_self, char *res, size_t res_size, size_t *res_len)
{
    struct Telescope *self = cast(Telescope(), _self);
    int ret;
    
    if (res == NULL) {
        return -1 * AAOS_EINVAL;
    }
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_TELESCOPE);
    protobuf_set(self, PACKET_COMMAND, TELESCOPE_COMMAND_STATUS);
    
    if ((ret = rpc_call(self)) == AAOS_OK) {
        uint32_t length;
        protobuf_get(self, PACKET_LENGTH, &length);
        if (length == 0) {
            char *s;
            protobuf_get(self, PACKET_STR, &s);
            if (s != res) {
                snprintf(res, res_size, "%s", s);
                if (res_len != NULL) {
                    *res_len = strlen(res) + 1;
                }
            }
        } else {
            char *buf;
            protobuf_get(self, PACKET_BUF, &buf, NULL);
            if (buf != res) {
                snprintf(res, res_size, "%s", buf);
            }
            if (res_len != NULL) {
                *res_len = strlen(res) + 1;
            }
        }
    }
    
    return ret;
}

int
telescope_power_on(void *_self)
{
    const struct TelescopeClass *class = (const struct TelescopeClass *) classOf(_self);
    
    if (isOf(class, TelescopeClass()) && class->power_on.method) {
        return ((int (*)(void *)) class->power_on.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) telescope_power_on, "power on", _self);
        return result;
    }
}

static int
Telescope_power_on(void *_self)
{
    struct Telescope *self = cast(Telescope(), _self);
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_TELESCOPE);
    protobuf_set(self, PACKET_COMMAND, TELESCOPE_COMMAND_POWER_ON);
    
    return rpc_call(self);
}

int
telescope_power_off(void *_self)
{
    const struct TelescopeClass *class = (const struct TelescopeClass *) classOf(_self);
    
    if (isOf(class, TelescopeClass()) && class->power_off.method) {
        return ((int (*)(void *)) class->power_off.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) telescope_power_off, "power off", _self);
        return result;
    }
}

static int
Telescope_power_off(void *_self)
{
    struct Telescope *self = cast(Telescope(), _self);
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_TELESCOPE);
    protobuf_set(self, PACKET_COMMAND, TELESCOPE_COMMAND_POWER_OFF);
    
    return rpc_call(self);
}

int
telescope_init(void *_self)
{
    const struct TelescopeClass *class = (const struct TelescopeClass *) classOf(_self);
    
    if (isOf(class, TelescopeClass()) && class->init.method) {
        return ((int (*)(void *)) class->init.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) telescope_init, "init", _self);
        return result;
    }
}

static int
Telescope_init(void *_self)
{
    struct Telescope *self = cast(Telescope(), _self);
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_TELESCOPE);
    protobuf_set(self, PACKET_COMMAND, TELESCOPE_COMMAND_INIT);
    
    return rpc_call(self);
}

int
telescope_park(void *_self)
{
    const struct TelescopeClass *class = (const struct TelescopeClass *) classOf(_self);
    
    if (isOf(class, TelescopeClass()) && class->park.method) {
        return ((int (*)(void *)) class->park.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) telescope_park, "park", _self);
        return result;
    }
}

static int
Telescope_park(void *_self)
{
    struct Telescope *self = cast(Telescope(), _self);
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_TELESCOPE);
    protobuf_set(self, PACKET_COMMAND, TELESCOPE_COMMAND_PARK);
    
    return rpc_call(self);
}

int
telescope_park_off(void *_self)
{
    const struct TelescopeClass *class = (const struct TelescopeClass *) classOf(_self);
    
    if (isOf(class, TelescopeClass()) && class->park_off.method) {
        return ((int (*)(void *)) class->park_off.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) telescope_park_off, "park off", _self);
        return result;
    }
}

static int
Telescope_park_off(void *_self)
{
    struct Telescope *self = cast(Telescope(), _self);
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_TELESCOPE);
    protobuf_set(self, PACKET_COMMAND, TELESCOPE_COMMAND_PARK_OFF);
    
    return rpc_call(self);
}

int
telescope_stop(void *_self)
{
    const struct TelescopeClass *class = (const struct TelescopeClass *) classOf(_self);
    
    if (isOf(class, TelescopeClass()) && class->stop.method) {
        return ((int (*)(void *)) class->stop.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) telescope_stop, "stop", _self);
        return result;
    }
}

static int
Telescope_stop(void *_self)
{
    struct Telescope *self = cast(Telescope(), _self);
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_TELESCOPE);
    protobuf_set(self, PACKET_COMMAND, TELESCOPE_COMMAND_STOP);
    
    return rpc_call(self);
}

int
telescope_go_home(void *_self)
{
    const struct TelescopeClass *class = (const struct TelescopeClass *) classOf(_self);
    
    if (isOf(class, TelescopeClass()) && class->go_home.method) {
        return ((int (*)(void *)) class->go_home.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) telescope_go_home, "go_home", _self);
        return result;
    }
}

static int
Telescope_go_home(void *_self)
{
    struct Telescope *self = cast(Telescope(), _self);
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_TELESCOPE);
    protobuf_set(self, PACKET_COMMAND, TELESCOPE_COMMAND_GO_HOME);
    
    return rpc_call(self);
}

int
telescope_move(void *_self, unsigned int direction, double duration)
{
    const struct TelescopeClass *class = (const struct TelescopeClass *) classOf(_self);
    
    if (isOf(class, TelescopeClass()) && class->move.method) {
        return ((int (*)(void *, unsigned int, double)) class->move.method)(_self, direction, duration);
    } else {
        int result;
        forward(_self, &result, (Method) telescope_move, "move", _self, direction, duration);
        return result;
    }
}

static int
Telescope_move(void *_self, unsigned int direction, double duration)
{
    struct Telescope *self = cast(Telescope(), _self);
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_TELESCOPE);
    protobuf_set(self, PACKET_COMMAND, TELESCOPE_COMMAND_MOVE);
    protobuf_set(self, PACKET_U32F0, (uint32_t) direction);
    protobuf_set(self, PACKET_DF1, duration);
    
    return rpc_call(self);
}

int
telescope_try_move(void *_self, unsigned direction, double duration)
{
    const struct TelescopeClass *class = (const struct TelescopeClass *) classOf(_self);
    
    if (isOf(class, TelescopeClass()) && class->try_move.method) {
        return ((int (*)(void *, unsigned int, double)) class->try_move.method)(_self, direction, duration);
    } else {
        int result;
        forward(_self, &result, (Method) telescope_try_move, "try_move", _self, direction, duration);
        return result;
    }
}

static int
Telescope_try_move(void *_self, unsigned int direction, double duration)
{
    struct Telescope *self = cast(Telescope(), _self);
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_TELESCOPE);
    protobuf_set(self, PACKET_COMMAND, TELESCOPE_COMMAND_TRY_MOVE);
    protobuf_set(self, PACKET_U32F0, (uint32_t) direction);
    protobuf_set(self, PACKET_DF1, duration);
    
    return rpc_call(self);
}

int
telescope_timed_move(void *_self, unsigned int direction, double duration, double timeout)
{
    const struct TelescopeClass *class = (const struct TelescopeClass *) classOf(_self);
    
    if (isOf(class, TelescopeClass()) && class->timed_move.method) {
        return ((int (*)(void *, unsigned int, double, double)) class->timed_move.method)(_self, direction, duration, timeout);
    } else {
        int result;
        forward(_self, &result, (Method) telescope_timed_move, "timed_move", _self, direction, duration, timeout);
        return result;
    }
}

static int
Telescope_timed_move(void *_self, unsigned int direction, double duration, double timeout)
{
    struct Telescope *self = cast(Telescope(), _self);
    double buf[2];
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_TELESCOPE);
    protobuf_set(self, PACKET_COMMAND, TELESCOPE_COMMAND_TIMED_MOVE);
    protobuf_set(self, PACKET_U32F0, (uint32_t) direction);
    
    buf[0] = duration;
    buf[1] = timeout;
    
    protobuf_set(self, PACKET_BUF, buf, sizeof(buf));
    
    return rpc_call(self);
}

int
telescope_slew(void *_self, double ra, double dec)
{
    const struct TelescopeClass *class = (const struct TelescopeClass *) classOf(_self);
    
    if (isOf(class, TelescopeClass()) && class->slew.method) {
        return ((int (*)(void *, double, double)) class->slew.method)(_self, ra, dec);
    } else {
        int result;
        forward(_self, &result, (Method) telescope_slew, "slew", _self, ra, dec);
        return result;
    }
}

static int
Telescope_slew(void *_self, double ra, double dec)
{
    struct Telescope *self = cast(Telescope(), _self);
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_TELESCOPE);
    protobuf_set(self, PACKET_COMMAND, TELESCOPE_COMMAND_SLEW);
    protobuf_set(self, PACKET_DF0, ra);
    protobuf_set(self, PACKET_DF1, dec);
    
    return rpc_call(self);
}

int
telescope_try_slew(void *_self, double ra, double dec)
{
    const struct TelescopeClass *class = (const struct TelescopeClass *) classOf(_self);
    
    if (isOf(class, TelescopeClass()) && class->try_slew.method) {
        return ((int (*)(void *, double, double)) class->try_slew.method)(_self, ra, dec);
    } else {
        int result;
        forward(_self, &result, (Method) telescope_try_slew, "try_slew", _self, ra, dec);
        return result;
    }
}

static int
Telescope_try_slew(void *_self, double ra, double dec)
{
    struct Telescope *self = cast(Telescope(), _self);

    protobuf_set(self, PACKET_PROTOCOL, PROTO_TELESCOPE);
    protobuf_set(self, PACKET_COMMAND, TELESCOPE_COMMAND_TRY_SLEW);
    protobuf_set(self, PACKET_DF0, ra);
    protobuf_set(self, PACKET_DF1, dec);
    
    return rpc_call(self);
}

int
telescope_timed_slew(void *_self, double ra, double dec, double timeout)
{
    const struct TelescopeClass *class = (const struct TelescopeClass *) classOf(_self);
    
    if (isOf(class, TelescopeClass()) && class->timed_slew.method) {
        return ((int (*)(void *, double, double, double)) class->timed_slew.method)(_self, ra, dec, timeout);
    } else {
        int result;
        forward(_self, &result, (Method) telescope_timed_slew, "timed_slew", _self, ra, dec, timeout);
        return result;
    }
}

static int
Telescope_timed_slew(void *_self, double ra, double dec, double timeout)
{
    struct Telescope *self = cast(Telescope(), _self);

    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_TELESCOPE);
    protobuf_set(self, PACKET_COMMAND, TELESCOPE_COMMAND_TIMED_SLEW);
    protobuf_set(self, PACKET_DF0, ra);
    protobuf_set(self, PACKET_DF1, dec);
    protobuf_set(self, PACKET_BUF, &timeout, sizeof(timeout));
    
    return rpc_call(self);
}

int
telescope_raw(void *_self, const void *command, size_t command_size, void *results, size_t results_size, size_t *return_size)
{
    const struct TelescopeClass *class = (const struct TelescopeClass *) classOf(_self);
    
    if (isOf(class, TelescopeClass()) && class->raw.method) {
        return ((int (*)(void *, const void *, size_t, void *, size_t, size_t *)) class->raw.method)(_self, command, command_size, results, results_size, return_size);
    } else {
        int result;
        forward(_self, &result, (Method) telescope_raw, "raw", _self, command, command_size, results, results_size, return_size);
        return result;
    }
}

static int
Telescope_raw(void *_self, const void *command, size_t command_size, void *results, size_t results_size, size_t *return_size)
{
    struct Telescope *self = cast(Telescope(), _self);
    void *buf;
    size_t size;
    uint32_t length;
    int ret;
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_TELESCOPE);
    protobuf_set(self, PACKET_COMMAND, TELESCOPE_COMMAND_RAW);
    protobuf_set(self, PACKET_BUF, command, command_size);
    
    if ((ret = rpc_call(self)) != AAOS_OK) {
        return ret;
    }

    protobuf_get(self, PACKET_LENGTH, &length);
    
    if (length == 0) {
        protobuf_get(self, PACKET_STR, &buf);
        size = PACKETPARAMETERSIZE;
    } else {
        protobuf_get(self, PACKET_BUF, &buf, &size);
    }
    
    if (results != buf) {
        if (size > results_size) {
            if (return_size) {
                *return_size = results_size;
            }
            
        } else {
            if (return_size) {
                *return_size = results_size;
            }
        }
        memcpy(results, buf, *return_size);
    } else {
        if (return_size) {
            *return_size = results_size;
        }
    }
    
    return AAOS_OK;
}

int
telescope_set_move_speed(void *_self, double move_speed)
{
    const struct TelescopeClass *class = (const struct TelescopeClass *) classOf(_self);
    
    if (isOf(class, TelescopeClass()) && class->set_move_speed.method) {
        return ((int (*)(void *, double)) class->set_move_speed.method)(_self, move_speed);
    } else {
        int result;
        forward(_self, &result, (Method) telescope_set_move_speed, "set_move_speed", _self, move_speed);
        return result;
    }
}

static int
Telescope_set_move_speed(void *_self, double move_speed)
{
    struct Telescope *self = cast(Telescope(), _self);
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_TELESCOPE);
    protobuf_set(self, PACKET_COMMAND, TELESCOPE_COMMAND_SET_MOVE_SPEED);
    protobuf_set(self, PACKET_DF0, move_speed);
    
    return rpc_call(self);
}

int
telescope_get_move_speed(void *_self, double *move_speed)
{
    const struct TelescopeClass *class = (const struct TelescopeClass *) classOf(_self);
    
    if (isOf(class, TelescopeClass()) && class->get_move_speed.method) {
        return ((int (*)(void *, double *)) class->get_move_speed.method)(_self, move_speed);
    } else {
        int result;
        forward(_self, &result, (Method) telescope_get_move_speed, "get_move_speed", _self, move_speed);
        return result;
    }
}

static int
Telescope_get_move_speed(void *_self, double *move_speed)
{
    struct Telescope *self = cast(Telescope(), _self);
    int ret;
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_TELESCOPE);
    protobuf_set(self, PACKET_COMMAND, TELESCOPE_COMMAND_GET_MOVE_SPEED);
    
    if ((ret = rpc_call(self)) != AAOS_OK) {
        return ret;
    }
    
    protobuf_get(self, PACKET_DF0, move_speed);
    
    return AAOS_OK;
}

int
telescope_set_slew_speed(void *_self, double slew_speed_x, double slew_speed_y)
{
    const struct TelescopeClass *class = (const struct TelescopeClass *) classOf(_self);
    
    if (isOf(class, TelescopeClass()) && class->set_slew_speed.method) {
        return ((int (*)(void *, double, double)) class->set_slew_speed.method)(_self, slew_speed_x, slew_speed_y);
    } else {
        int result;
        forward(_self, &result, (Method) telescope_set_slew_speed, "set_slew_speed", _self, slew_speed_x, slew_speed_y);
        return result;
    }
}

static int
Telescope_set_slew_speed(void *_self, double slew_speed_x, double slew_speed_y)
{
    struct Telescope *self = cast(Telescope(), _self);
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_TELESCOPE);
    protobuf_set(self, PACKET_COMMAND, TELESCOPE_COMMAND_SET_SLEW_SPEED);
    protobuf_set(self, PACKET_DF0, slew_speed_x);
    protobuf_set(self, PACKET_DF1, slew_speed_y);
    
    return rpc_call(self);
}

int
telescope_get_slew_speed(void *_self, double *slew_speed_x, double *slew_speed_y)
{
    const struct TelescopeClass *class = (const struct TelescopeClass *) classOf(_self);
    
    if (isOf(class, TelescopeClass()) && class->get_slew_speed.method) {
        return ((int (*)(void *, double *, double *)) class->set_slew_speed.method)(_self, slew_speed_x, slew_speed_y);
    } else {
        int result;
        forward(_self, &result, (Method) telescope_get_slew_speed, "get_slew_speed", _self, slew_speed_x, slew_speed_y);
        return result;
    }
}

static int
Telescope_get_slew_speed(void *_self, double *slew_speed_x, double *slew_speed_y)
{
    struct Telescope *self = cast(Telescope(), _self);
    int ret;
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_TELESCOPE);
    protobuf_set(self, PACKET_COMMAND, TELESCOPE_COMMAND_GET_SLEW_SPEED);
    
    if ((ret = rpc_call(self)) != AAOS_OK) {
        return ret;
    }
    
    protobuf_get(self, PACKET_DF0, slew_speed_x);
    protobuf_get(self, PACKET_DF1, slew_speed_y);
    
    return AAOS_OK;
}

int
telescope_set_track_rate(void *_self, double track_rate_x, double track_rate_y)
{
    const struct TelescopeClass *class = (const struct TelescopeClass *) classOf(_self);
    
    if (isOf(class, TelescopeClass()) && class->set_track_rate.method) {
        return ((int (*)(void *, double, double)) class->set_track_rate.method)(_self, track_rate_x, track_rate_y);
    } else {
        int result;
        forward(_self, &result, (Method) telescope_set_track_rate, "set_track_rate", _self, track_rate_x, track_rate_y);
        return result;
    }
}

static int
Telescope_set_track_rate(void *_self, double track_rate_x, double track_rate_y)
{
    struct Telescope *self = cast(Telescope(), _self);
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_TELESCOPE);
    protobuf_set(self, PACKET_COMMAND, TELESCOPE_COMMAND_SET_TRACK_RATE);
    protobuf_set(self, PACKET_DF0, track_rate_x);
    protobuf_set(self, PACKET_DF1, track_rate_y);
    
    return rpc_call(self);
}

int
telescope_get_track_rate(void *_self, double *track_rate_x, double *track_rate_y)
{
    const struct TelescopeClass *class = (const struct TelescopeClass *) classOf(_self);
    
    if (isOf(class, TelescopeClass()) && class->get_track_rate.method) {
        return ((int (*)(void *, double *, double *)) class->get_track_rate.method)(_self, track_rate_x, track_rate_y);
    } else {
        int result;
        forward(_self, &result, (Method) telescope_get_track_rate, "get_track_rate", _self, track_rate_x, track_rate_y);
        return result;
    }
}

static int
Telescope_get_track_rate(void *_self, double *track_rate_x, double *track_rate_y)
{
    struct Telescope *self = cast(Telescope(), _self);
    int ret;
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_TELESCOPE);
    protobuf_set(self, PACKET_COMMAND, TELESCOPE_COMMAND_GET_TRACK_RATE);
    
    if ((ret = rpc_call(self)) != AAOS_OK) {
        return ret;
    }
    
    protobuf_get(self, PACKET_DF0, track_rate_x);
    protobuf_get(self, PACKET_DF1, track_rate_y);
    
    return AAOS_OK;
}

int
telescope_set_option(void *_self, uint16_t option)
{
    const struct TelescopeClass *class = (const struct TelescopeClass *) classOf(_self);
    
    if (isOf(class, TelescopeClass()) && class->set_option.method) {
        return ((int (*)(void *, uint16_t)) class->set_track_rate.method)(_self, option);
    } else {
        int result;
        forward(_self, &result, (Method) telescope_set_option, "set_option", _self, option);
        return result;
    }
}

static int
Telescope_set_option(void *_self, uint16_t option)
{
    struct Telescope *self = cast(Telescope(), _self);
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_TELESCOPE);
    protobuf_set(self, PACKET_COMMAND, TELESCOPE_COMMAND_SET_OPTION);
    protobuf_set(self, PACKET_OPTION, option);
    
    return rpc_call(self);
}

/*
 * Virtual functions
 */

static int
Telescope_execute_get_index_by_name(struct Telescope *self)
{
    char *name;
    int index, ret;
    uint32_t length;
    
    protobuf_get(self, PACKET_LENGTH, &length);
    
    if (length == 0) {
        protobuf_get(self, PACKET_STR, &name);
    } else {
        protobuf_get(self, PACKET_BUF, &name, NULL);
    }
    
    if ((ret = get_index_by_name(name, &index)) != AAOS_OK) {
        return ret;
    } else {
        uint16_t idx = (uint16_t) index;
        protobuf_set(self, PACKET_INDEX, idx);
        protobuf_set(self, PACKET_LENGTH, 0);
    }
    
    return AAOS_OK;
}

static int
Telescope_execute_status(struct Telescope *self)
{
    int ret;
    uint16_t index;
    uint32_t length;
    void *telescope;
    void *buf;
    size_t payload;
    
    protobuf_get(self, PACKET_LENGTH, &length);
    protobuf_get(self, PACKET_INDEX, &index);
    
    if (index == 0) {
        char *name;
        int idx;
        if (length == 0) {
            protobuf_get(self, PACKET_STR, &name);
        } else {
            protobuf_get(self, PACKET_BUF, &name, NULL);
        }
        if ((ret = get_index_by_name(name, &idx)) != AAOS_OK) {
            return ret;
        }
        index = (int) idx;
        protobuf_set(self, PACKET_INDEX, index);
    }
    
    if ((telescope = get_telescope_by_index(index)) == NULL) {
        return AAOS_ENOTFOUND;
    }
    
    protobuf_get(self, PACKET_BUF, &buf, NULL);
    payload = protobuf_payload(self);
    
    ret = __telescope_status(telescope, buf, payload);

    if (ret != AAOS_OK) {
        protobuf_set(self, PACKET_LENGTH, 0);
    } else {
        length = (uint32_t) strlen(buf);
        protobuf_set(self, PACKET_LENGTH, length);
    }
    
    return ret;
}

static int
Telescope_execute_power_on(struct Telescope *self)
{
    int ret;
    uint16_t index;
    uint32_t length;
    void *telescope;
    
    protobuf_get(self, PACKET_LENGTH, &length);
    protobuf_get(self, PACKET_INDEX, &index);
    
    if (index == 0) {
        char *name;
        int idx;
        if (length == 0) {
            protobuf_get(self, PACKET_STR, &name);
        } else {
            protobuf_get(self, PACKET_BUF, &name, NULL);
        }
        if ((ret = get_index_by_name(name, &idx)) != AAOS_OK) {
            return ret;
        }
        index = (int) idx;
        protobuf_set(self, PACKET_INDEX, index);
    }
    
    if ((telescope = get_telescope_by_index(index)) == NULL) {
        return AAOS_ENOTFOUND;
    }
    
    ret = __telescope_power_on(telescope);
    protobuf_set(self, PACKET_LENGTH, 0);
    
    return ret;
}

static int
Telescope_execute_power_off(struct Telescope *self)
{
    int ret;
    uint16_t index;
    uint32_t length;
    void *telescope;
    
    protobuf_get(self, PACKET_LENGTH, &length);
    protobuf_get(self, PACKET_INDEX, &index);
    
    if (index == 0) {
        char *name;
        int idx;
        if (length == 0) {
            protobuf_get(self, PACKET_STR, &name);
        } else {
            protobuf_get(self, PACKET_BUF, &name, NULL);
        }
        if ((ret = get_index_by_name(name, &idx)) != AAOS_OK) {
            return ret;
        }
        index = (int) idx;
        protobuf_set(self, PACKET_INDEX, index);
    }
    
    if ((telescope = get_telescope_by_index(index)) == NULL) {
        return AAOS_ENOTFOUND;
    }
    ret = __telescope_power_off(telescope);
    protobuf_set(self, PACKET_LENGTH, 0);
    
    return ret;
}

static int
Telescope_execute_init(struct Telescope *self)
{
    int ret;
    uint16_t index;
    uint32_t length;
    void *telescope;
    
    protobuf_get(self, PACKET_LENGTH, &length);
    protobuf_get(self, PACKET_INDEX, &index);
    
    if (index == 0) {
        char *name;
        int idx;
        if (length == 0) {
            protobuf_get(self, PACKET_STR, &name);
        } else {
            protobuf_get(self, PACKET_BUF, &name, NULL);
        }
        if ((ret = get_index_by_name(name, &idx)) != AAOS_OK) {
            return ret;
        }
        index = (int) idx;
        protobuf_set(self, PACKET_INDEX, index);
    }
    
    if ((telescope = get_telescope_by_index(index)) == NULL) {
        return AAOS_ENOTFOUND;
    }
    ret = __telescope_init(telescope);
    protobuf_set(self, PACKET_LENGTH, 0);
    
    return ret;
}

static int
Telescope_execute_park(struct Telescope *self)
{
    int ret;
    uint16_t index;
    uint32_t length;
    void *telescope;
    
    protobuf_get(self, PACKET_LENGTH, &length);
    protobuf_get(self, PACKET_INDEX, &index);
    
    if (index == 0) {
        char *name;
        int idx;
        if (length == 0) {
            protobuf_get(self, PACKET_STR, &name);
        } else {
            protobuf_get(self, PACKET_BUF, &name, NULL);
        }
        if ((ret = get_index_by_name(name, &idx)) != AAOS_OK) {
            return ret;
        }
        index = (int) idx;
        protobuf_set(self, PACKET_INDEX, index);
    }
    
    if ((telescope = get_telescope_by_index(index)) == NULL) {
        
        return AAOS_ENOTFOUND;
    }
    ret = __telescope_park(telescope);
    protobuf_set(self, PACKET_LENGTH, 0);
    
    return ret;
}

static int
Telescope_execute_park_off(struct Telescope *self)
{
    int ret;
    uint16_t index;
    uint32_t length;
    void *telescope;
    
    protobuf_get(self, PACKET_LENGTH, &length);
    protobuf_get(self, PACKET_INDEX, &index);
    
    if ((telescope = get_telescope_by_index(index)) == NULL) {
        return AAOS_ENOTFOUND;
    }
    
    ret = __telescope_park_off(telescope);
    protobuf_set(self, PACKET_LENGTH, 0);
    
    return ret;
}

static int
Telescope_execute_stop(struct Telescope *self)
{
    int ret;
    uint16_t index;
    uint32_t length;
    void *telescope;
    
    protobuf_get(self, PACKET_LENGTH, &length);
    protobuf_get(self, PACKET_INDEX, &index);
    
    if (index == 0) {
        char *name;
        int idx;
        if (length == 0) {
            protobuf_get(self, PACKET_STR, &name);
        } else {
            protobuf_get(self, PACKET_BUF, &name, NULL);
        }
        if ((ret = get_index_by_name(name, &idx)) != AAOS_OK) {
            return ret;
        }
        index = (int) idx;
        protobuf_set(self, PACKET_INDEX, index);
    }
    
    if ((telescope = get_telescope_by_index(index)) == NULL) {
        return AAOS_ENOTFOUND;
    }
    
    ret = __telescope_stop(telescope);
    protobuf_set(self, PACKET_LENGTH, 0);
    
    return ret;
}

static int
Telescope_execute_go_home(struct Telescope *self)
{
    int ret;
    uint16_t index;
    uint32_t length;
    void *telescope;
    
    protobuf_get(self, PACKET_LENGTH, &length);
    protobuf_get(self, PACKET_INDEX, &index);
    
    if (index == 0) {
        char *name;
        int idx;
        if (length == 0) {
            protobuf_get(self, PACKET_STR, &name);
        } else {
            protobuf_get(self, PACKET_BUF, &name, NULL);
        }
        if ((ret = get_index_by_name(name, &idx)) != AAOS_OK) {
            return ret;
        }
        index = (int) idx;
        protobuf_set(self, PACKET_INDEX, index);
    }
    
    if ((telescope = get_telescope_by_index(index)) == NULL) {
        return AAOS_ENOTFOUND;
    }
    
    ret = __telescope_go_home(telescope);
    protobuf_set(self, PACKET_LENGTH, 0);
    
    return ret;
}

static int
Telescope_execute_move(struct Telescope *self)
{
    int ret;
    uint16_t index;
    uint32_t length;
    void *telescope;
    uint32_t direction;
    double duration;
    
    protobuf_get(self, PACKET_LENGTH, &length);
    protobuf_get(self, PACKET_INDEX, &index);
    
    if ((telescope = get_telescope_by_index(index)) == NULL) {
        return AAOS_ENOTFOUND;
    }
    
    protobuf_get(self, PACKET_U32F0, &direction);
    protobuf_get(self, PACKET_DF1, &duration);
    
    ret = __telescope_move(telescope, direction, duration);
    
    protobuf_set(self, PACKET_LENGTH, 0);
    
    return ret;
}

static int
Telescope_execute_try_move(struct Telescope *self)
{
    int ret;
    uint16_t index;
    uint32_t length;
    void *telescope;
    uint32_t direction;
    double duration;
    
    protobuf_get(self, PACKET_LENGTH, &length);
    protobuf_get(self, PACKET_INDEX, &index);
    
    if ((telescope = get_telescope_by_index(index)) == NULL) {
        return AAOS_ENOTFOUND;
    }
    
    protobuf_get(self, PACKET_U32F0, &direction);
    protobuf_get(self, PACKET_DF1, &duration);
    
    ret = __telescope_try_move(telescope, direction, duration);
    
    protobuf_set(self, PACKET_LENGTH, 0);
    
    return ret;
}

static int
Telescope_execute_timed_move(struct Telescope *self)
{
    int ret;
    uint16_t index;
    uint32_t length;
    void *telescope;
    uint32_t direction;
    
    double *buf;
    
    protobuf_get(self, PACKET_LENGTH, &length);
    protobuf_get(self, PACKET_INDEX, &index);
    
    if (length != sizeof(double) * 2) {
        return AAOS_EINVAL;
    }
    
    if ((telescope = get_telescope_by_index(index)) == NULL) {
        return AAOS_ENOTFOUND;
    }
    
    protobuf_get(self, PACKET_U32F0, &direction);
    protobuf_get(self, PACKET_BUF, &buf, NULL);
    
    ret = __telescope_timed_move(telescope, direction, buf[0], buf[1]);
    
    protobuf_set(self, PACKET_LENGTH, 0);
    
    return ret;
}

static int
Telescope_execute_slew(struct Telescope *self)
{
    int ret;
    uint16_t index;
    void *telescope;
    double ra;
    double dec;
    
    protobuf_get(self, PACKET_INDEX, &index);
    
    if ((telescope = get_telescope_by_index(index)) == NULL) {
        return AAOS_ENOTFOUND;
    }
    
    protobuf_get(self, PACKET_DF0, &ra);
    protobuf_get(self, PACKET_DF1, &dec);
    
    ret = __telescope_slew(telescope, ra, dec);
    
    protobuf_set(self, PACKET_LENGTH, 0);
    
    return ret;
}

static int
Telescope_execute_try_slew(struct Telescope *self)
{
    int ret;
    uint16_t index;
    void *telescope;
    double ra;
    double dec;
    
    protobuf_get(self, PACKET_INDEX, &index);
    
    if ((telescope = get_telescope_by_index(index)) == NULL) {
        return AAOS_ENOTFOUND;
    }
    
    protobuf_get(self, PACKET_DF0, &ra);
    protobuf_get(self, PACKET_DF1, &dec);
    
    ret = __telescope_slew(telescope, ra, dec);
    
    protobuf_set(self, PACKET_LENGTH, 0);
    
    return ret;
}

static int
Telescope_execute_timed_slew(struct Telescope *self)
{
    int ret;
    uint16_t index;
    uint32_t length;
    void *telescope;
    
    double *buf;
    
    protobuf_get(self, PACKET_LENGTH, &length);
    protobuf_get(self, PACKET_INDEX, &index);
    
    if (length != sizeof(double) * 3) {
        return AAOS_EINVAL;
    }
    
    if ((telescope = get_telescope_by_index(index)) == NULL) {
        return AAOS_ENOTFOUND;
    }
    
    protobuf_get(self, PACKET_BUF, &buf, NULL);
    
    ret = __telescope_timed_slew(telescope, buf[0], buf[1], buf[2]);
    
    protobuf_set(self, PACKET_LENGTH, 0);
    
    return ret;
}

static int
Telescope_execute_set_move_speed(struct Telescope *self)
{
    int ret;
    uint16_t index;
    void *telescope;
    double move_speed;
    
    
    protobuf_get(self, PACKET_INDEX, &index);
    
    if ((telescope = get_telescope_by_index(index)) == NULL) {
        return AAOS_ENOTFOUND;
    }

    protobuf_get(self, PACKET_DF0, &move_speed);
    
    ret = __telescope_set_move_speed(telescope, move_speed);
    
    protobuf_set(self, PACKET_LENGTH, 0);
    
    return ret;
}

static int
Telescope_execute_get_move_speed(struct Telescope *self)
{
    int ret;
    uint16_t index;
    void *telescope;
    double move_speed;
    
    protobuf_get(self, PACKET_INDEX, &index);
    
    if ((telescope = get_telescope_by_index(index)) == NULL) {
        return AAOS_ENOTFOUND;
    }
    
    ret = __telescope_get_move_speed(telescope, &move_speed);
    
    protobuf_set(self, PACKET_DF0, move_speed);
    protobuf_set(self, PACKET_LENGTH, 0);
    
    return ret;
}

static int
Telescope_execute_set_slew_speed(struct Telescope *self)
{
    int ret;
    uint16_t index;
    void *telescope;
    double slew_speed_x, slew_speed_y;
    
    
    protobuf_get(self, PACKET_INDEX, &index);
    
    if ((telescope = get_telescope_by_index(index)) == NULL) {
        return AAOS_ENOTFOUND;
    }
    
    protobuf_get(self, PACKET_DF0, &slew_speed_x);
    protobuf_get(self, PACKET_DF1, &slew_speed_y);
    
    ret = __telescope_set_slew_speed(telescope, slew_speed_x, slew_speed_y);
    
    protobuf_set(self, PACKET_LENGTH, 0);
    
    return ret;
}

static int
Telescope_execute_get_slew_speed(struct Telescope *self)
{
    int ret;
    uint16_t index;
    void *telescope;
    double slew_speed_x, slew_speed_y;
    
    protobuf_get(self, PACKET_INDEX, &index);
    
    if ((telescope = get_telescope_by_index(index)) == NULL) {
        return AAOS_ENOTFOUND;
    }
    
    ret = __telescope_get_slew_speed(telescope, &slew_speed_x, &slew_speed_y);
    
    protobuf_set(self, PACKET_DF0, slew_speed_x);
    protobuf_set(self, PACKET_DF1, slew_speed_y);
    protobuf_set(self, PACKET_LENGTH, 0);
    
    return ret;
}

static int
Telescope_execute_set_track_rate(struct Telescope *self)
{
    int ret;
    uint16_t index;
    void *telescope;
    double track_rate_x, track_rate_y;
    
    
    protobuf_get(self, PACKET_INDEX, &index);
    
    if ((telescope = get_telescope_by_index(index)) == NULL) {
        return AAOS_ENOTFOUND;
    }
    
    protobuf_get(self, PACKET_DF0, &track_rate_x);
    protobuf_get(self, PACKET_DF1, &track_rate_y);

    ret = __telescope_set_track_rate(telescope, track_rate_x, track_rate_y);
    
    protobuf_set(self, PACKET_LENGTH, 0);
    
    return ret;
}

static int
Telescope_execute_get_track_rate(struct Telescope *self)
{
    int ret;
    uint16_t index;
    void *telescope;
    double track_rate_x, track_rate_y;
    
    protobuf_get(self, PACKET_INDEX, &index);
    
    if ((telescope = get_telescope_by_index(index)) == NULL) {
        return AAOS_ENOTFOUND;
    }
    
    ret = __telescope_get_track_rate(telescope, &track_rate_x, &track_rate_y);
    
    protobuf_set(self, PACKET_DF0, track_rate_x);
    protobuf_set(self, PACKET_DF1, track_rate_y);
    protobuf_set(self, PACKET_LENGTH, 0);
    
    return ret;
}

static int
Telescope_execute_raw(struct Telescope *self)
{
    char *command;
    int ret;
    uint16_t index;
    uint32_t length;
    
    /*
     * If payload is zero, just use STR field as the input raw command; otherwise, use payload as the input
     */
    
    protobuf_get(self, PACKET_LENGTH, &length);
    if (length == 0) {
        protobuf_get(self, PACKET_STR, &command);
    } else {
        protobuf_get(self, PACKET_BUF, &command, NULL);
    }
    
    protobuf_get(self, PACKET_INDEX, &index);
    
    if (index == 0) {
        /*
         * If index is zero, means STR field contains device name or device path, payload must none zero.
         */
        if (length == 0) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_EBADCMD);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_EBADCMD;
        } else {
            char *s, *buf;
            void *telescope;
            int idx;
            size_t payload, read_size;
            protobuf_get(self, PACKET_STR, &s);
            if ((get_index_by_name(s, &idx)) != AAOS_OK) {
                protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
                protobuf_set(self, PACKET_LENGTH, 0);
                return AAOS_ENOTFOUND;
            }
            index = (uint16_t) idx;
            protobuf_set(self, PACKET_INDEX, &index);
            telescope = get_telescope_by_index(idx);
            protobuf_get(self, PACKET_BUF, &buf, NULL);
            payload = protobuf_payload(self);
            if ((ret = __telescope_raw(telescope, command, strlen(command), NULL, buf, payload, &read_size)) != AAOS_OK) {
                return ret;
            }
            /*
             * If STR field is enough, use it.
             */
            if (read_size < PACKETPARAMETERSIZE) {
                memcpy(s, buf, read_size);
                protobuf_set(self, PACKET_LENGTH, 0);
            } else {
                length = (uint32_t) read_size;
                protobuf_set(self, PACKET_LENGTH, length);
            }
        }
    } else {
        char *s, *buf;
        void *telescope;
        if ((telescope = get_telescope_by_index((int) index)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
        size_t payload, read_size, write_size;
        protobuf_get(self, PACKET_LENGTH, &length);
        payload = protobuf_payload(self);
        protobuf_get(self, PACKET_BUF, &buf, NULL);
        if ((ret = __telescope_raw(telescope, command, strlen(command), &write_size, buf, payload, &read_size)) != AAOS_OK) {
            return ret;
        }
        protobuf_set(self, PACKET_ERRORCODE, AAOS_OK);
        if (read_size < PACKETPARAMETERSIZE) {
            protobuf_get(self, PACKET_STR, &s);
            memcpy(s, buf, read_size);
            protobuf_set(self, PACKET_LENGTH, 0);
        } else {
            length = (uint32_t) read_size;
            protobuf_set(self, PACKET_LENGTH, length);
        }
    }
    return ret;
}

static int
Telescope_execute_set_option(struct Telescope *self)
{
    int ret;
    uint16_t index, option;
    void *telescope;

    protobuf_get(self, PACKET_INDEX, &index);
    
    if ((telescope = get_telescope_by_index(index)) == NULL) {
        return AAOS_ENOTFOUND;
    }
    
    protobuf_get(self, PACKET_OPTION, &option);
    
    ret = __telescope_set_option(telescope, option);
    
    protobuf_set(self, PACKET_LENGTH, 0);
    
    return ret;
}

static int
Telescope_execute_default(struct Telescope *self)
{
    return AAOS_EBADCMD;
}

static int
Telescope_execute(void *_self)
{
    struct Telescope *self = cast(Telescope(), _self);
    uint16_t command;
    int ret;
    
    if (Telescope_protocol_check(self) != AAOS_OK) {
        return AAOS_EPROTOWRONG;
    }
    
    protobuf_get(self, PACKET_COMMAND, &command);
    
    switch (command) {
        case TELESCOPE_COMMAND_GET_INDEX_BY_NAME:
            ret = Telescope_execute_get_index_by_name(self);
            break;
        case TELESCOPE_COMMAND_STATUS:
            ret = Telescope_execute_status(self);
            break;
        case TELESCOPE_COMMAND_POWER_ON:
            ret = Telescope_execute_power_on(self);
            break;
        case TELESCOPE_COMMAND_POWER_OFF:
            ret = Telescope_execute_power_off(self);
            break;
        case TELESCOPE_COMMAND_INIT:
            ret = Telescope_execute_init(self);
            break;
        case TELESCOPE_COMMAND_PARK:
            ret = Telescope_execute_park(self);
            break;
        case TELESCOPE_COMMAND_PARK_OFF:
            ret = Telescope_execute_park_off(self);
            break;
        case TELESCOPE_COMMAND_STOP:
            ret = Telescope_execute_stop(self);
            break;
        case TELESCOPE_COMMAND_GO_HOME:
            ret = Telescope_execute_go_home(self);
            break;
        case TELESCOPE_COMMAND_MOVE:
            ret = Telescope_execute_move(self);
            break;
        case TELESCOPE_COMMAND_TRY_MOVE:
            ret = Telescope_execute_try_move(self);
            break;
        case TELESCOPE_COMMAND_TIMED_MOVE:
            ret = Telescope_execute_timed_move(self);
            break;
        case TELESCOPE_COMMAND_SLEW:
            ret = Telescope_execute_slew(self);
            break;
        case TELESCOPE_COMMAND_TRY_SLEW:
            ret = Telescope_execute_try_slew(self);
            break;
        case TELESCOPE_COMMAND_TIMED_SLEW:
            ret = Telescope_execute_timed_slew(self);
            break;
        case TELESCOPE_COMMAND_RAW:
            ret = Telescope_execute_raw(self);
            break;
        case TELESCOPE_COMMAND_SET_MOVE_SPEED:
            ret = Telescope_execute_set_move_speed(self);
            break;
        case TELESCOPE_COMMAND_GET_MOVE_SPEED:
            ret = Telescope_execute_get_move_speed(self);
            break;
        case TELESCOPE_COMMAND_SET_SLEW_SPEED:
            ret = Telescope_execute_set_slew_speed(self);
            break;
        case TELESCOPE_COMMAND_GET_SLEW_SPEED:
            ret = Telescope_execute_get_slew_speed(self);
            break;
        case TELESCOPE_COMMAND_SET_TRACK_RATE:
            ret = Telescope_execute_set_track_rate(self);
            break;
        case TELESCOPE_COMMAND_GET_TRACK_RATE:
            ret = Telescope_execute_get_track_rate(self);
            break;
        case TELESCOPE_COMMAND_SET_OPTION:
            ret = Telescope_execute_set_option(self);
            break;
        default:
            return Telescope_execute_default(self);
            break;
    }
    
    return ret;
}

static const void *telescope_virtual_table(void);

static void *
Telescope_ctor(void *_self, va_list *app)
{
    struct Telescope *self = super_ctor(Telescope(), _self, app);
    
    self->_._vtab = telescope_virtual_table();
    
    return (void *) self;
}

static void *
Telescope_dtor(void *_self)
{
    //struct RPC *self = cast(RPC(), _self);
    
    return super_dtor(Telescope(), _self);
}

static void *
TelescopeClass_ctor(void *_self, va_list *app)
{
    struct TelescopeClass *self = super_ctor(TelescopeClass(), _self, app);
    Method selector;
    
    self->_.execute.method = (Method) 0;
    
#ifdef va_copy
    va_list ap;
    va_copy(ap, *app);
#else
    va_list ap = *app;
#endif
    
    while ((selector = va_arg(ap, Method))) {
        const char *tag = va_arg(ap, const char *);
        Method method = va_arg(ap, Method);
        
        if (selector == (Method) telescope_status) {
            if (tag) {
                self->status.tag = tag;
                self->status.selector = selector;
            }
            self->status.method = method;
            continue;
        }
        if (selector == (Method) telescope_get_index_by_name) {
            if (tag) {
                self->get_index_by_name.tag = tag;
                self->get_index_by_name.selector = selector;
            }
            self->get_index_by_name.method = method;
            continue;
        }
        if (selector == (Method) telescope_raw) {
            if (tag) {
                self->raw.tag = tag;
                self->raw.selector = selector;
            }
            self->raw.method = method;
            continue;
        }
        if (selector == (Method) telescope_power_on) {
            if (tag) {
                self->power_on.tag = tag;
                self->power_on.selector = selector;
            }
            self->power_on.method = method;
            continue;
        }
        if (selector == (Method) telescope_power_off) {
            if (tag) {
                self->power_off.tag = tag;
                self->power_off.selector = selector;
            }
            self->power_off.method = method;
            continue;
        }
        if (selector == (Method) telescope_init) {
            if (tag) {
                self->init.tag = tag;
                self->init.selector = selector;
            }
            self->init.method = method;
            continue;
        }
        if (selector == (Method) telescope_park) {
            if (tag) {
                self->park.tag = tag;
                self->park.selector = selector;
            }
            self->park.method = method;
            continue;
        }
        if (selector == (Method) telescope_park_off) {
            if (tag) {
                self->park_off.tag = tag;
                self->init.selector = selector;
            }
            self->park_off.method = method;
            continue;
        }
        if (selector == (Method) telescope_stop) {
            if (tag) {
                self->stop.tag = tag;
                self->stop.selector = selector;
            }
            self->stop.method = method;
            continue;
        }
        if (selector == (Method) telescope_move) {
            if (tag) {
                self->move.tag = tag;
                self->move.selector = selector;
            }
            self->move.method = method;
            continue;
        }
        if (selector == (Method) telescope_try_move) {
            if (tag) {
                self->try_move.tag = tag;
                self->try_move.selector = selector;
            }
            self->try_move.method = method;
            continue;
        }
        if (selector == (Method) telescope_timed_move) {
            if (tag) {
                self->timed_move.tag = tag;
                self->timed_move.selector = selector;
            }
            self->timed_move.method = method;
            continue;
        }
        if (selector == (Method) telescope_slew) {
            if (tag) {
                self->slew.tag = tag;
                self->slew.selector = selector;
            }
            self->slew.method = method;
            continue;
        }
        if (selector == (Method) telescope_try_slew) {
            if (tag) {
                self->try_slew.tag = tag;
                self->try_slew.selector = selector;
            }
            self->try_slew.method = method;
            continue;
        }
        if (selector == (Method) telescope_timed_slew) {
            if (tag) {
                self->timed_slew.tag = tag;
                self->timed_slew.selector = selector;
            }
            self->timed_slew.method = method;
            continue;
        }
        if (selector == (Method) telescope_set_move_speed) {
            if (tag) {
                self->set_move_speed.tag = tag;
                self->set_move_speed.selector = selector;
            }
            self->set_move_speed.method = method;
            continue;
        }
        if (selector == (Method) telescope_get_move_speed) {
            if (tag) {
                self->get_move_speed.tag = tag;
                self->get_move_speed.selector = selector;
            }
            self->get_move_speed.method = method;
            continue;
        }
        if (selector == (Method) telescope_set_slew_speed) {
            if (tag) {
                self->set_slew_speed.tag = tag;
                self->set_slew_speed.selector = selector;
            }
            self->set_slew_speed.method = method;
            continue;
        }
        if (selector == (Method) telescope_get_slew_speed) {
            if (tag) {
                self->get_slew_speed.tag = tag;
                self->get_slew_speed.selector = selector;
            }
            self->get_slew_speed.method = method;
            continue;
        }
        if (selector == (Method) telescope_set_track_rate) {
            if (tag) {
                self->set_track_rate.tag = tag;
                self->set_track_rate.selector = selector;
            }
            self->set_track_rate.method = method;
            continue;
        }
        if (selector == (Method) telescope_get_track_rate) {
            if (tag) {
                self->get_track_rate.tag = tag;
                self->get_track_rate.selector = selector;
            }
            self->get_track_rate.method = method;
            continue;
        }
        if (selector == (Method) telescope_set_option) {
            if (tag) {
                self->set_option.tag = tag;
                self->set_option.selector = selector;
            }
            self->set_option.method = method;
            continue;
        }
        if (selector == (Method) telescope_go_home) {
            if (tag) {
                self->go_home.tag = tag;
                self->go_home.selector = selector;
            }
            self->go_home.method = method;
            continue;
        }
    }
    
#ifdef va_copy
    va_end(ap);
#endif
    return (void *) self;
    
}

static void *_TelescopeClass;

static void
TelescopeClass_destroy(void)
{
    free((void *) _TelescopeClass);
}

static void
TelescopeClass_initialize(void)
{
    _TelescopeClass = new(RPCClass(), "TelescopeClass", RPCClass(), sizeof(struct TelescopeClass),
                          ctor, "ctor", TelescopeClass_ctor,
                          (void *) 0);
    
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(TelescopeClass_destroy);
#endif
    
}

const void *
TelescopeClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, TelescopeClass_initialize);
#endif
    
    return _TelescopeClass;
}

static void *_Telescope;

static void
Telescope_destroy(void)
{
    free((void *) _Telescope);
}

static void
Telescope_initialize(void)
{
    _Telescope = new(TelescopeClass(), "Telescope", RPC(), sizeof(struct Telescope),
                     ctor, "ctor", Telescope_ctor,
                     dtor, "dtor", Telescope_dtor,
                     telescope_get_index_by_name, "get_index_by_name", Telescope_get_index_by_name,
                     telescope_raw, "raw", Telescope_raw,
                     telescope_status, "status", Telescope_status,
                     telescope_power_on, "power_on", Telescope_power_on,
                     telescope_power_off, "power_off", Telescope_power_off,
                     telescope_init, "init", Telescope_init,
                     telescope_park, "park", Telescope_park,
                     telescope_park_off, "park_off", Telescope_park_off,
                     telescope_stop, "stop", Telescope_stop,
                     telescope_go_home, "go_home", Telescope_go_home,
                     telescope_move, "move", Telescope_move,
                     telescope_try_move, "try_move", Telescope_try_move,
                     telescope_timed_move, "timed_move", Telescope_timed_move,
                     telescope_slew, "slew", Telescope_slew,
                     telescope_try_slew, "try_slew", Telescope_try_slew,
                     telescope_timed_slew, "timed_slew", Telescope_timed_slew,
                     telescope_set_move_speed, "set_move_speed", Telescope_set_move_speed,
                     telescope_get_move_speed, "get_move_speed", Telescope_get_move_speed,
                     telescope_set_slew_speed, "set_slew_speed", Telescope_set_slew_speed,
                     telescope_get_slew_speed, "get_move_speed", Telescope_get_slew_speed,
                     telescope_set_track_rate, "set_track_rate", Telescope_set_track_rate,
                     telescope_get_track_rate, "get_track_rate", Telescope_get_track_rate,
                     telescope_set_option, "set_option", Telescope_set_option,
                     (void *) 0);
    
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(Telescope_destroy);
#endif
    
}

const void *
Telescope(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, Telescope_initialize);
#endif
    
    return _Telescope;
}

static const void *_telescope_virtual_table;

static void
telescope_virtual_table_destroy(void)
{
    delete((void *) _telescope_virtual_table);
}


static void
telescope_virtual_table_initialize(void)
{
    _telescope_virtual_table = new(RPCVirtualTable(),
                                   rpc_execute, "execute", Telescope_execute,
                                   (void *)0);
    
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(telescope_virtual_table_destroy);
#endif
    
}

static const void *
telescope_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, telescope_virtual_table_initialize);
#endif
    
    return _telescope_virtual_table;
}

/*
 * Telescope client class
 */

static const void *telescope_client_virtual_table(void);

static
int TelescopeClient_connect(void *_self, void **client)
{
    struct TelescopeClient *self = cast(TelescopeClient(), _self);
    int ret = AAOS_OK;
    
    int cfd = Tcp_connect(self->_._.address, self->_._.port, NULL, NULL);
    
    if (cfd < 0) {
        switch (errno) {
            case ECONNREFUSED:
                ret = AAOS_ECONNREFUSED;
                break;
            case ENETUNREACH:
                ret = AAOS_ENETUNREACH;
                break;
            case ETIMEDOUT:
                ret = AAOS_ETIMEDOUT;
                break;
            default:
                ret = AAOS_ERROR;
                break;
        }
    }
    
    *client = new(Telescope(), cfd);
    protobuf_set(*client, PACKET_PROTOCOL, PROTO_TELESCOPE);
    
    return ret;
}

static void *
TelescopeClient_ctor(void *_self, va_list *app)
{
    struct TelescopeClient *self = super_ctor(TelescopeClient(), _self, app);
    
    self->_._vtab = telescope_client_virtual_table();
    
    return (void *) self;
}

static void *
TelescopeClient_dtor(void *_self)
{
    //struct RPC *self = cast(RPC(), _self);
    
    return super_dtor(TelescopeClient(), _self);
}

static void *
TelescopeClientClass_ctor(void *_self, va_list *app)
{
    struct TelescopeClientClass *self = super_ctor(TelescopeClientClass(), _self, app);
    
    self->_.connect.method = (Method) 0;
    
    return self;
}

static void *_TelescopeClientClass;

static void
TelescopeClientClass_destroy(void)
{
    free((void *) _TelescopeClientClass);
}

static void
TelescopeClientClass_initialize(void)
{
    _TelescopeClientClass = new(RPCClientClass(), "TelescopeClientClass", RPCClientClass(), sizeof(struct TelescopeClientClass),
                                ctor, "ctor", TelescopeClientClass_ctor,
                                (void *) 0);
    
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(TelescopeClientClass_destroy);
#endif
    
}

const void *
TelescopeClientClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, TelescopeClientClass_initialize);
#endif
    
    return _TelescopeClientClass;
}

static void *_TelescopeClient;

static void
TelescopeClient_destroy(void)
{
    free((void *) _TelescopeClient);
}

static void
TelescopeClient_initialize(void)
{
    _TelescopeClient = new(TelescopeClientClass(), "TelescopeClient", RPCClient(), sizeof(struct TelescopeClient),
                           ctor, "ctor", TelescopeClient_ctor,
                           dtor, "dtor", TelescopeClient_dtor,
                           (void *) 0);
    
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(TelescopeClient_destroy);
#endif
}

const void *
TelescopeClient(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, TelescopeClient_initialize);
#endif
    
    return _TelescopeClient;
}

static const void *_telescope_client_virtual_table;

static void
telescope_client_virtual_table_destroy(void)
{
    delete((void *) _telescope_client_virtual_table);
}

static void
telescope_client_virtual_table_initialize(void)
{
    _telescope_client_virtual_table = new(RPCClientVirtualTable(),
                                          rpc_client_connect, "connect", TelescopeClient_connect,
                                          (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(telescope_client_virtual_table_destroy);
#endif
    
}

static const void *
telescope_client_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, telescope_client_virtual_table_initialize);
#endif
    
    return _telescope_client_virtual_table;
}

/*
 * Telescope server class
 */

static const void *telescope_server_virtual_table(void);

static int
TelescopeServer_accept(void *_self, void **client)
{
    struct RPCServer *self = cast(RPCServer(), _self);
    
    int lfd, cfd;
    
    lfd = tcp_server_get_lfd(self);
    cfd = Accept(lfd, NULL, NULL);
    if (cfd < 0) {
        *client = NULL;
        return AAOS_ERROR;
    } else {
        if ((*client = new(Telescope(), cfd)) == NULL) {
            Close(cfd);
            return AAOS_ERROR;
        }
        return AAOS_OK;
    }
}

static void *
TelescopeServer_ctor(void *_self, va_list *app)
{
    struct TelescopeServer *self = super_ctor(TelescopeServer(), _self, app);
    
    self->_._vtab = telescope_server_virtual_table();
    
    return (void *) self;
}

static void *
TelescopeServer_dtor(void *_self)
{
    //struct RPC *self = cast(RPC(), _self);
    
    return super_dtor(TelescopeServer(), _self);
}

static void *
TelescopeServerClass_ctor(void *_self, va_list *app)
{
    struct TelescopeServerClass *self = super_ctor(TelescopeServerClass(), _self, app);
    
    self->_.accept.method = (Method) 0;
    
    return self;
}

static void *_TelescopeServerClass;

static void
TelescopeServerClass_destroy(void)
{
    free((void *) _TelescopeServerClass);
}

static void
TelescopeServerClass_initialize(void)
{
    _TelescopeServerClass = new(RPCServerClass(), "TelescopeServerClass", RPCServerClass(), sizeof(struct TelescopeServerClass),
                             ctor, "ctor", TelescopeServerClass_ctor,
                             (void *) 0);
    
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(TelescopeServerClass_destroy);
#endif
}

const void *
TelescopeServerClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, TelescopeServerClass_initialize);
#endif
    
    return _TelescopeServerClass;
}

static void *_TelescopeServer;

static void
TelescopeServer_destroy(void)
{
    free((void *) _TelescopeServer);
}

static void
TelescopeServer_initialize(void)
{
    _TelescopeServer = new(TelescopeServerClass(), "TelescopeServer", RPCServer(), sizeof(struct TelescopeServer),
                        ctor, "ctor", TelescopeServer_ctor,
                        dtor, "dtor", TelescopeServer_dtor,
                        (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(TelescopeServer_destroy);
#endif
}

const void *
TelescopeServer(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, TelescopeServer_initialize);
#endif
    
    return _TelescopeServer;
}

static const void *_telescope_server_virtual_table;

static void
telescope_server_virtual_table_destroy(void)
{
    delete((void *) _telescope_server_virtual_table);
}

static void
telescope_server_virtual_table_initialize(void)
{
    _telescope_server_virtual_table = new(RPCServerVirtualTable(),
                                       rpc_server_accept, "accept", TelescopeServer_accept,
                                       (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(telescope_server_virtual_table_destroy);
#endif
}

static const void *
telescope_server_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, telescope_server_virtual_table_initialize);
#endif
    
    return _telescope_server_virtual_table;
}

#ifdef _USE_COMPILER_ATTRIBUTION_

static void __destructor__(void) __attribute__ ((destructor(_TELESCOPE_RPC_PRIORITY_)));

static void
__destructor__(void)
{
    TelescopeServer_destroy();
    TelescopeServerClass_destroy();
    telescope_server_virtual_table_destroy();
    TelescopeClient_destroy();
    TelescopeClientClass_destroy();
    telescope_client_virtual_table_destroy();
    Telescope_destroy();
    TelescopeClass_destroy();
    telescope_virtual_table_destroy();
}

static void __constructor__(void) __attribute__ ((constructor(_TELESCOPE_RPC_PRIORITY_)));

static void
__constructor__(void)
{
    telescope_virtual_table_initialize();
    TelescopeClass_initialize();
    Telescope_initialize();
    telescope_client_virtual_table_initialize();
    TelescopeClientClass_initialize();
    TelescopeClient_initialize();
    telescope_server_virtual_table_initialize();
    TelescopeServerClass_initialize();
    TelescopeServer_initialize();
}
#endif
