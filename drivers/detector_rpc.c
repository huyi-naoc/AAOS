//
//  detector_rpc.c
//  AAOS
//
//  Created by Hu Yi on 2021/11/16.
//  Copyright © 2021 NAOC. All rights reserved.
//

#include "detector_rpc.h"

#include "adt.h"
#include "def.h"
#include "protocol.h"
#include "detector.h"
#include "detector_rpc.h"
#include "detector_rpc_r.h"
#include "wrapper.h"

static bool
detector_find_by_name_if(void *detector, va_list *app)
{
    const char *name, *myname;
    
    myname = __detector_get_name(detector);
#ifdef va_copy
    va_list ap;
    va_copy(ap, *app);
    name = va_arg(ap, const char *);
    va_end(ap);
#else
    name = va_arg(*app, const char *);
#endif
    if (strcmp(name, myname) == 0) {
        return true;
    } else {
        return false;
    }
}

static int
get_index_by_name(const char *name, int *index)
{
    size_t i;
    const char *s;
    *index = 0;
    
    for (i = 0; i < n_detector; i++) {
        s = __detector_get_name(detectors[i]);
        if (strcmp(name, s) == 0) {
            *index = (int) i + 1;
            return AAOS_OK;
        }
    }
    
    if (*index == 0) {
        if (threadsafe_list_find_first_if(detector_list, detector_find_by_name_if, name) != NULL) {
            return AAOS_OK;
        }
    }

    return AAOS_ENOTFOUND;
}

static void *
get_detector_by_name(const char *name)
{
    size_t i;
    const char *s;
    void *detector = NULL;
    
    for (i = 0; i < n_detector; i++) {
        s = __detector_get_name(detectors[i]);
        if (strcmp(name, s) == 0) {
            return  detectors[i];
        }
    }
    
    if (detector == NULL) {
        detector = threadsafe_list_find_first_if(detector_list, detector_find_by_name_if, name);
    }
    
    return detector;
}

static void *
get_detector_by_index(int index)
{
    if (index > 0 && index <= n_detector) {
        return detectors[index - 1];
    } else {
        return NULL;
    }
}

/*
 * Detector class
 */

// private function

static void
Detector_get_result(void *protobuf, int command, ...)
{
    va_list ap;
    va_start(ap, command);
    switch (command) {
        case DETECTOR_COMMAND_INFO:
        case DETECTOR_COMMAND_RAW:
        case DETECTOR_COMMAND_STATUS:
        case DETECTOR_COMMAND_EXPOSE:
        case DETECTOR_COMMAND_GET_PREFIX:
        case DETECTOR_COMMAND_GET_TEMPLATE:
        case DETECTOR_COMMAND_GET_DIRECTORY:
            
        {
            uint32_t length;
            void *res = va_arg(ap, void *);
            size_t res_size = va_arg(ap, size_t);
            size_t *res_length = va_arg(ap, size_t *);
            protobuf_get(protobuf, PACKET_LENGTH, &length);
            if (length == 0) {
                char *s;
                protobuf_get(protobuf, PACKET_STR, &s);
                if (s != res) {
                    snprintf(res, res_size, "%s", s);
                }
                if (res_length != NULL) {
                    *res_length = strlen(res);
                }
            } else {
                char *buf;
                protobuf_get(protobuf, PACKET_BUF, &buf, NULL);
                if (buf != res) {
                    snprintf(res, res_size, "%s", buf);
                }
                if (res_length != NULL) {
                    *res_length = strlen(res);
                }
            }
        }
            break;
        default:
            break;
    }
    
    va_end(ap);
}

inline static int
Detector_protocol_check(void *_self)
{
    uint16_t protocol;
    protobuf_get(_self, PACKET_PROTOCOL, &protocol);
    if (protocol != PROTO_DETECTOR && protocol != PROTO_SYSTEM) {
        protobuf_set(_self, PACKET_ERRORCODE, AAOS_EPROTOWRONG);
        protobuf_set(_self, PACKET_LENGTH, 0);
        return AAOS_EPROTOWRONG;
    } else {
        return AAOS_OK;
    }
}

int
detector_get_index_by_name(void *_self, const char *name)
{
    const struct DetectorClass *class = (const struct DetectorClass *) classOf(_self);
    
    if (isOf(class, DetectorClass()) && class->get_index_by_name.method) {
        return ((int (*)(void *, const char *)) class->get_index_by_name.method)(_self, name);
    } else {
        int result;
        forward(_self, &result, (Method) detector_get_index_by_name, "get_index_by_name", _self, name);
        return result;
    }
}

static int
Detector_get_index_by_name(void *_self, const char *name)
{
    struct Detector *self = cast(Detector(), _self);
    size_t length;
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_DETECTOR);
    protobuf_set(self, PACKET_COMMAND, DETECTOR_COMMAND_GET_INDEX_BY_NAME);
    protobuf_set(self, PACKET_INDEX, 0);
    
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
        uint32_t len = (uint32_t) length;
        protobuf_set(self, PACKET_LENGTH, len + 1);
    }
    
    return rpc_call(self);
}

int
detector_abort(void *_self)
{
    const struct DetectorClass *class = (const struct DetectorClass *) classOf(_self);
    
    if (isOf(class, DetectorClass()) && class->abort.method) {
        return ((int (*)(void *)) class->abort.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) detector_abort, "abort", _self);
        return result;
    }
}

static int
Detector_abort(void *_self)
{
    struct Detector *self = cast(Detector(), _self);
    
    void *protobuf = self->_.protobuf;
    
    protobuf_set(protobuf, PACKET_PROTOCOL, PROTO_DETECTOR);
    protobuf_set(protobuf, PACKET_COMMAND, DETECTOR_COMMAND_ABORT);
    
    return rpc_call(self);
}

int
detector_stop(void *_self)
{
    const struct DetectorClass *class = (const struct DetectorClass *) classOf(_self);
    
    if (isOf(class, DetectorClass()) && class->stop.method) {
        return ((int (*)(void *)) class->stop.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) detector_stop, "stop", _self);
        return result;
    }
}

static int
Detector_stop(void *_self)
{
    struct Detector *self = cast(Detector(), _self);
    
    void *protobuf = self->_.protobuf;
    
    protobuf_set(protobuf, PACKET_PROTOCOL, PROTO_DETECTOR);
    protobuf_set(protobuf, PACKET_COMMAND, DETECTOR_COMMAND_STOP);
    
    return rpc_call(self);
}

int
detector_enable_cooling(void *_self)
{
    const struct DetectorClass *class = (const struct DetectorClass *) classOf(_self);
    
    if (isOf(class, DetectorClass()) && class->enable_cooling.method) {
        return ((int (*)(void *)) class->enable_cooling.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) detector_enable_cooling, "enable_cooling", _self);
        return result;
    }
}

static int
Detector_enable_cooling(void *_self)
{
    struct Detector *self = cast(Detector(), _self);
    
    void *protobuf = self->_.protobuf;
    
    protobuf_set(protobuf, PACKET_PROTOCOL, PROTO_DETECTOR);
    protobuf_set(protobuf, PACKET_COMMAND, DETECTOR_COMMAND_COOLING_ENABLE);
    
    return rpc_call(self);
}

int
detector_disable_cooling(void *_self)
{
    const struct DetectorClass *class = (const struct DetectorClass *) classOf(_self);
    
    if (isOf(class, DetectorClass()) && class->disable_cooling.method) {
        return ((int (*)(void *)) class->disable_cooling.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) detector_disable_cooling, "disable_cooling", _self);
        return result;
    }
}

static int
Detector_disable_cooling(void *_self)
{
    struct Detector *self = cast(Detector(), _self);
    
    void *protobuf = self->_.protobuf;
    
    protobuf_set(protobuf, PACKET_PROTOCOL, PROTO_DETECTOR);
    protobuf_set(protobuf, PACKET_COMMAND, DETECTOR_COMMAND_COOLING_DISABLE);
    
    return rpc_call(self);
}

int
detector_info(void *_self, void *res, size_t res_size, size_t *res_len)
{
    const struct DetectorClass *class = (const struct DetectorClass *) classOf(_self);
    
    if (isOf(class, DetectorClass()) && class->info.method) {
        return ((int (*)(void *, void *, size_t, size_t *)) class->info.method)(_self, res, res_size, res_len);
    } else {
        int result;
        forward(_self, &result, (Method) detector_info, "info", _self, res, res_size, res_len);
        return result;
    }
}

static int
Detector_info(void *_self, void *res, size_t res_size, size_t *res_len)
{
    struct Detector *self = cast(Detector(), _self);
    
    void *protobuf = self->_.protobuf;
    int ret;
    
    protobuf_set(protobuf, PACKET_PROTOCOL, PROTO_DETECTOR);
    protobuf_set(protobuf, PACKET_COMMAND, DETECTOR_COMMAND_INFO);
    
    if ((ret = rpc_call(self)) == AAOS_OK) {
        Detector_get_result(protobuf, DETECTOR_COMMAND_INFO, res, res_size, res_len);
    }
    
    return ret;
}

int
detector_status(void *_self, void *res, size_t res_size, size_t *res_len)
{
    const struct DetectorClass *class = (const struct DetectorClass *) classOf(_self);
    
    if (isOf(class, DetectorClass()) && class->status.method) {
        return ((int (*)(void *, void *, size_t, size_t *)) class->status.method)(_self, res, res_size, res_len);
    } else {
        int result;
        forward(_self, &result, (Method) detector_status, "status", _self, res, res_size, res_len);
        return result;
    }
}

static int
Detector_status(void *_self, void *res, size_t res_size, size_t *res_len)
{
    struct Detector *self = cast(Detector(), _self);
    
    void *protobuf = self->_.protobuf;
    int ret;
    
    protobuf_set(protobuf, PACKET_PROTOCOL, PROTO_DETECTOR);
    protobuf_set(protobuf, PACKET_COMMAND, DETECTOR_COMMAND_STATUS);
    
    if ((ret = rpc_call(self)) == AAOS_OK) {
        Detector_get_result(protobuf, DETECTOR_COMMAND_INFO, res, res_size, res_len);
    }
    
    return ret;
}

int
detector_set_binning(void *_self, uint32_t x_binning, uint32_t y_binning)
{
    const struct DetectorClass *class = (const struct DetectorClass *) classOf(_self);
    
    if (isOf(class, DetectorClass()) && class->set_binning.method) {
        return ((int (*)(void *, uint32_t, uint32_t)) class->set_binning.method)(_self, x_binning, y_binning);
    } else {
        int result;
        forward(_self, &result, (Method) detector_set_binning, "set_binning", _self, x_binning, y_binning);
        return result;
    }
}

static int
Detector_set_binning(void *_self, uint32_t x_binning, uint32_t y_binning)
{
    struct Detector *self = cast(Detector(), _self);
    
    void *protobuf = self->_.protobuf;
    
    protobuf_set(protobuf, PACKET_PROTOCOL, PROTO_DETECTOR);
    protobuf_set(protobuf, PACKET_COMMAND, DETECTOR_COMMAND_SET_BINNING);
    protobuf_set(protobuf, PACKET_U32F0, x_binning);
    protobuf_set(protobuf, PACKET_U32F1, y_binning);
    
    return rpc_call(self);
}

int
detector_get_binning(void *_self, uint32_t *x_binning, uint32_t *y_binning)
{
    const struct DetectorClass *class = (const struct DetectorClass *) classOf(_self);
    
    if (isOf(class, DetectorClass()) && class->get_binning.method) {
        return ((int (*)(void *, uint32_t *, uint32_t *)) class->get_binning.method)(_self, x_binning, y_binning);
    } else {
        int result;
        forward(_self, &result, (Method) detector_get_binning, "get_binning", _self, x_binning, y_binning);
        return result;
    }
}

static int
Detector_get_binning(void *_self, uint32_t *x_binning, uint32_t *y_binning)
{
    struct Detector *self = cast(Detector(), _self);
    
    void *protobuf = self->_.protobuf;
    int ret = AAOS_OK;
    
    protobuf_set(protobuf, PACKET_PROTOCOL, PROTO_DETECTOR);
    protobuf_set(protobuf, PACKET_COMMAND, DETECTOR_COMMAND_GET_BINNING);
    
    if ((ret = rpc_call(self)) != AAOS_OK) {
        return ret;
    }
    
    protobuf_get(protobuf, PACKET_U32F0, x_binning);
    protobuf_get(protobuf, PACKET_U32F1, y_binning);
    return ret;
}

int
detector_set_exposure_time(void *_self, double exposure_time)
{
    const struct DetectorClass *class = (const struct DetectorClass *) classOf(_self);
    
    if (isOf(class, DetectorClass()) && class->set_exposure_time.method) {
        return ((int (*)(void *, double)) class->set_exposure_time.method)(_self, exposure_time);
    } else {
        int result;
        forward(_self, &result, (Method) detector_set_exposure_time, "set_exposure_time", _self, exposure_time);
        return result;
    }
}

static int
Detector_set_exposure_time(void *_self, double exposure_time)
{
    struct Detector *self = cast(Detector(), _self);
    
    void *protobuf = self->_.protobuf;
    
    protobuf_set(protobuf, PACKET_PROTOCOL, PROTO_DETECTOR);
    protobuf_set(protobuf, PACKET_COMMAND, DETECTOR_COMMAND_SET_EXPOSURE_TIME);
    protobuf_set(protobuf, PACKET_DF0, exposure_time);
    
    return rpc_call(self);
}

int
detector_get_exposure_time(void *_self, double *exposure_time)
{
    const struct DetectorClass *class = (const struct DetectorClass *) classOf(_self);
    
    if (isOf(class, DetectorClass()) && class->get_exposure_time.method) {
        return ((int (*)(void *, double *)) class->get_exposure_time.method)(_self, exposure_time);
    } else {
        int result;
        forward(_self, &result, (Method) detector_get_exposure_time, "get_exposure_time", _self, exposure_time);
        return result;
    }
}

static int
Detector_get_exposure_time(void *_self, double *exposure_time)
{
    struct Detector *self = cast(Detector(), _self);
    
    void *protobuf = self->_.protobuf;
    int ret = AAOS_OK;
    
    protobuf_set(protobuf, PACKET_PROTOCOL, PROTO_DETECTOR);
    protobuf_set(protobuf, PACKET_COMMAND, DETECTOR_COMMAND_GET_EXPOSURE_TIME);
    
    if ((ret = rpc_call(self)) != AAOS_OK) {
        return ret;
    }
    
    protobuf_get(protobuf, PACKET_DF0, exposure_time);
    
    return ret;
}

int
detector_set_frame_rate(void *_self, double frame_rate)
{
    const struct DetectorClass *class = (const struct DetectorClass *) classOf(_self);
    
    if (isOf(class, DetectorClass()) && class->set_frame_rate.method) {
        return ((int (*)(void *, double)) class->set_frame_rate.method)(_self, frame_rate);
    } else {
        int result;
        forward(_self, &result, (Method) detector_set_frame_rate, "set_frame_rate", _self, frame_rate);
        return result;
    }
}

static int
Detector_set_frame_rate(void *_self, double frame_rate)
{
    struct Detector *self = cast(Detector(), _self);
    
    void *protobuf = self->_.protobuf;
    
    protobuf_set(protobuf, PACKET_PROTOCOL, PROTO_DETECTOR);
    protobuf_set(protobuf, PACKET_COMMAND, DETECTOR_COMMAND_SET_FRAME_RATE);
    protobuf_set(protobuf, PACKET_DF0, frame_rate);
    
    return rpc_call(self);
}

int
detector_get_frame_rate(void *_self, double *frame_rate)
{
    const struct DetectorClass *class = (const struct DetectorClass *) classOf(_self);
    
    if (isOf(class, DetectorClass()) && class->get_frame_rate.method) {
        return ((int (*)(void *, double *)) class->get_frame_rate.method)(_self, frame_rate);
    } else {
        int result;
        forward(_self, &result, (Method) detector_get_frame_rate, "get_frame_rate", _self, frame_rate);
        return result;
    }
}

static int
Detector_get_frame_rate(void *_self, double *frame_rate)
{
    struct Detector *self = cast(Detector(), _self);
    
    void *protobuf = self->_.protobuf;
    int ret = AAOS_OK;
    
    protobuf_set(protobuf, PACKET_PROTOCOL, PROTO_DETECTOR);
    protobuf_set(protobuf, PACKET_COMMAND, DETECTOR_COMMAND_GET_FRAME_RATE);
    
    if ((ret = rpc_call(self)) != AAOS_OK) {
        return ret;
    }
    
    protobuf_get(protobuf, PACKET_DF0, frame_rate);
    
    return ret;
}

int
detector_set_gain(void *_self, double gain)
{
    const struct DetectorClass *class = (const struct DetectorClass *) classOf(_self);
    
    if (isOf(class, DetectorClass()) && class->set_gain.method) {
        return ((int (*)(void *, double)) class->set_gain.method)(_self, gain);
    } else {
        int result;
        forward(_self, &result, (Method) detector_set_gain, "set_gain", _self, gain);
        return result;
    }
}

static int
Detector_set_gain(void *_self, double gain)
{
    struct Detector *self = cast(Detector(), _self);
    
    void *protobuf = self->_.protobuf;
    
    protobuf_set(protobuf, PACKET_PROTOCOL, PROTO_DETECTOR);
    protobuf_set(protobuf, PACKET_COMMAND, DETECTOR_COMMAND_SET_GAIN);
    protobuf_set(protobuf, PACKET_DF0, gain);
    
    return rpc_call(self);
}

int
detector_get_gain(void *_self, double *gain)
{
    const struct DetectorClass *class = (const struct DetectorClass *) classOf(_self);
    
    if (isOf(class, DetectorClass()) && class->get_gain.method) {
        return ((int (*)(void *, double *)) class->get_gain.method)(_self, gain);
    } else {
        int result;
        forward(_self, &result, (Method) detector_get_gain, "get_gain", _self, gain);
        return result;
    }
}

static int
Detector_get_gain(void *_self, double *gain)
{
    struct Detector *self = cast(Detector(), _self);
    
    void *protobuf = self->_.protobuf;
    int ret = AAOS_OK;
    
    protobuf_set(protobuf, PACKET_PROTOCOL, PROTO_DETECTOR);
    protobuf_set(protobuf, PACKET_COMMAND, DETECTOR_COMMAND_GET_GAIN);
    
    if ((ret = rpc_call(self)) != AAOS_OK) {
        return ret;
    }
    
    protobuf_get(protobuf, PACKET_DF0, gain);
    
    return ret;
}

int
detector_set_readout_rate(void *_self, double readout_rate)
{
    const struct DetectorClass *class = (const struct DetectorClass *) classOf(_self);
    
    if (isOf(class, DetectorClass()) && class->set_readout_rate.method) {
        return ((int (*)(void *, double)) class->set_readout_rate.method)(_self, readout_rate);
    } else {
        int result;
        forward(_self, &result, (Method) detector_set_readout_rate, "set_readout_rate", _self, readout_rate);
        return result;
    }
}

static int
Detector_set_readout_rate(void *_self, double readout_rate)
{
    struct Detector *self = cast(Detector(), _self);
    
    void *protobuf = self->_.protobuf;
    
    protobuf_set(protobuf, PACKET_PROTOCOL, PROTO_DETECTOR);
    protobuf_set(protobuf, PACKET_COMMAND, DETECTOR_COMMAND_SET_READOUT_RATE);
    protobuf_set(protobuf, PACKET_DF0, readout_rate);
    
    return rpc_call(self);
}

int
detector_get_readout_rate(void *_self, double *readout_rate)
{
    const struct DetectorClass *class = (const struct DetectorClass *) classOf(_self);
    
    if (isOf(class, DetectorClass()) && class->get_readout_rate.method) {
        return ((int (*)(void *, double *)) class->get_readout_rate.method)(_self, readout_rate);
    } else {
        int result;
        forward(_self, &result, (Method) detector_get_readout_rate, "get_readout_rate", _self, readout_rate);
        return result;
    }
}

static int
Detector_get_readout_rate(void *_self, double *readout_rate)
{
    struct Detector *self = cast(Detector(), _self);
    
    void *protobuf = self->_.protobuf;
    int ret = AAOS_OK;
    
    protobuf_set(protobuf, PACKET_PROTOCOL, PROTO_DETECTOR);
    protobuf_set(protobuf, PACKET_COMMAND, DETECTOR_COMMAND_GET_READOUT_RATE);
    
    if ((ret = rpc_call(self)) != AAOS_OK) {
        return ret;
    }
    
    protobuf_get(protobuf, PACKET_DF0, readout_rate);
    
    return ret;
}

int
detector_set_region(void *_self, uint32_t x_offset, uint32_t y_offset, uint32_t width, uint32_t height)
{
    const struct DetectorClass *class = (const struct DetectorClass *) classOf(_self);
    
    if (isOf(class, DetectorClass()) && class->set_region.method) {
        return ((int (*)(void *, uint32_t, uint32_t, uint32_t, uint32_t)) class->set_region.method)(_self, x_offset, y_offset, width, height);
    } else {
        int result;
        forward(_self, &result, (Method) detector_set_region, "set_region", _self, x_offset, y_offset, width, height);
        return result;
    }
}

static int
Detector_set_region(void *_self, uint32_t x_offset, uint32_t y_offset, uint32_t width, uint32_t height)
{
    struct Detector *self = cast(Detector(), _self);
    
    void *protobuf = self->_.protobuf;
    
    protobuf_set(protobuf, PACKET_PROTOCOL, PROTO_DETECTOR);
    protobuf_set(protobuf, PACKET_COMMAND, DETECTOR_COMMAND_SET_REGION);
    protobuf_set(protobuf, PACKET_U32F0, x_offset);
    protobuf_set(protobuf, PACKET_U32F1, y_offset);
    protobuf_set(protobuf, PACKET_U32F2, width);
    protobuf_set(protobuf, PACKET_U32F3, height);
    
    return rpc_call(self);
}

int
detector_get_region(void *_self, uint32_t *x_offset, uint32_t *y_offset, uint32_t *width, uint32_t *height)
{
    const struct DetectorClass *class = (const struct DetectorClass *) classOf(_self);
    
    if (isOf(class, DetectorClass()) && class->get_region.method) {
        return ((int (*)(void *, uint32_t *, uint32_t *, uint32_t *, uint32_t *)) class->get_region.method)(_self, x_offset, y_offset, width, height);
    } else {
        int result;
        forward(_self, &result, (Method) detector_get_region, "get_region", _self, x_offset, y_offset, width, height);
        return result;
    }
}

static int
Detector_get_region(void *_self, uint32_t *x_offset, uint32_t *y_offset, uint32_t *width, uint32_t *height)
{
    struct Detector *self = cast(Detector(), _self);
    
    void *protobuf = self->_.protobuf;
    int ret = AAOS_OK;
    
    protobuf_set(protobuf, PACKET_PROTOCOL, PROTO_DETECTOR);
    protobuf_set(protobuf, PACKET_COMMAND, DETECTOR_COMMAND_GET_REGION);
    
    if ((ret = rpc_call(self)) != AAOS_OK) {
        return ret;
    }
    
    protobuf_get(protobuf, PACKET_U32F0, &x_offset);
    protobuf_get(protobuf, PACKET_U32F1, &y_offset);
    protobuf_get(protobuf, PACKET_U32F2, &width);
    protobuf_get(protobuf, PACKET_U32F3, &height);
    
    return ret;
}

int
detector_set_temperature(void *_self, double temperature)
{
    const struct DetectorClass *class = (const struct DetectorClass *) classOf(_self);
    
    if (isOf(class, DetectorClass()) && class->set_temperature.method) {
        return ((int (*)(void *, double)) class->set_temperature.method)(_self, temperature);
    } else {
        int result;
        forward(_self, &result, (Method) detector_set_temperature, "set_temperature", _self, temperature);
        return result;
    }
}

static int
Detector_set_temperature(void *_self, double temperature)
{
    struct Detector *self = cast(Detector(), _self);
    
    void *protobuf = self->_.protobuf;
    
    protobuf_set(protobuf, PACKET_PROTOCOL, PROTO_DETECTOR);
    protobuf_set(protobuf, PACKET_COMMAND, DETECTOR_COMMAND_SET_TEMPERATURE);
    protobuf_set(protobuf, PACKET_DF0, temperature);
    
    return rpc_call(self);
}

int
detector_get_temperature(void *_self, double *temperature)
{
    const struct DetectorClass *class = (const struct DetectorClass *) classOf(_self);
    
    if (isOf(class, DetectorClass()) && class->get_temperature.method) {
        return ((int (*)(void *, double *)) class->get_temperature.method)(_self, temperature);
    } else {
        int result;
        forward(_self, &result, (Method) detector_get_temperature, "get_temperature", _self, temperature);
        return result;
    }
}

static int
Detector_get_temperature(void *_self, double *temperature)
{
    struct Detector *self = cast(Detector(), _self);
    
    void *protobuf = self->_.protobuf;
    int ret = AAOS_OK;
    
    protobuf_set(protobuf, PACKET_PROTOCOL, PROTO_DETECTOR);
    protobuf_set(protobuf, PACKET_COMMAND, DETECTOR_COMMAND_GET_TEMPERATURE);
    
    if ((ret = rpc_call(self)) != AAOS_OK) {
        return ret;
    }
    
    protobuf_get(protobuf, PACKET_DF0, temperature);
    
    return ret;
}

int
detector_set_option(void *_self, uint16_t option)
{
    const struct DetectorClass *class = (const struct DetectorClass *) classOf(_self);
    
    if (isOf(class, DetectorClass()) && class->set_option.method) {
        return ((int (*)(void *, uint16_t)) class->set_option.method)(_self, option);
    } else {
        int result;
        forward(_self, &result, (Method) detector_set_option, "set_option", _self, option);
        return result;
    }
}

static int
Detector_set_option(void *_self, double option)
{
    struct Detector *self = cast(Detector(), _self);
    
    void *protobuf = self->_.protobuf;
    
    protobuf_set(protobuf, PACKET_PROTOCOL, PROTO_DETECTOR);
    protobuf_set(protobuf, PACKET_COMMAND, DETECTOR_COMMAND_SET_OPTION);
    protobuf_set(protobuf, PACKET_OPTION, option);
    
    return rpc_call(self);
}

int
detector_get_option(void *_self, uint16_t *option)
{
    const struct DetectorClass *class = (const struct DetectorClass *) classOf(_self);
    
    if (isOf(class, DetectorClass()) && class->get_option.method) {
        return ((int (*)(void *, uint16_t *)) class->get_option.method)(_self, option);
    } else {
        int result;
        forward(_self, &result, (Method) detector_get_option, "get_option", _self, option);
        return result;
    }
}

static int
Detector_get_option(void *_self, uint16_t *option)
{
    struct Detector *self = cast(Detector(), _self);
    
    void *protobuf = self->_.protobuf;
    int ret = AAOS_OK;
    
    protobuf_set(protobuf, PACKET_PROTOCOL, PROTO_DETECTOR);
    protobuf_set(protobuf, PACKET_COMMAND, DETECTOR_COMMAND_GET_OPTION);
    
    if ((ret = rpc_call(self)) != AAOS_OK) {
        return ret;
    }
    
    protobuf_get(protobuf, PACKET_OPTION, &option);
    
    return ret;
}

int
detector_clear_option(void *_self)
{
    const struct DetectorClass *class = (const struct DetectorClass *) classOf(_self);
    
    if (isOf(class, DetectorClass()) && class->clear_option.method) {
        return ((int (*)(void *)) class->clear_option.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) detector_clear_option, "clear_option", _self);
        return result;
    }
}

static int
Detector_clear_option(void *_self)
{
    struct Detector *self = cast(Detector(), _self);
    
    void *protobuf = self->_.protobuf;
    
    protobuf_set(protobuf, PACKET_PROTOCOL, PROTO_DETECTOR);
    protobuf_set(protobuf, PACKET_COMMAND, DETECTOR_COMMAND_CLEAR_OPTION);
    protobuf_set(protobuf, PACKET_OPTION, 0);
    
    return rpc_call(self);
}

int
detector_set_prefix(void *_self, const char *prefix)
{
    const struct DetectorClass *class = (const struct DetectorClass *) classOf(_self);
    
    if (isOf(class, DetectorClass()) && class->set_prefix.method) {
        return ((int (*)(void *, const char *)) class->set_prefix.method)(_self, prefix);
    } else {
        int result;
        forward(_self, &result, (Method) detector_set_prefix, "set_prefix", _self, prefix);
        return result;
    }
}

static int
Detector_set_prefix(void *_self, const char *prefix)
{
    struct Detector *self = cast(Detector(), _self);
    
    uint16_t index;
    size_t size = strlen(prefix) + 1; //including NULL-terminated character '\0'.
    
    if (prefix == NULL) {
        return -1 * AAOS_EINVAL;
    }
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_DETECTOR);
    protobuf_set(self, PACKET_COMMAND, DETECTOR_COMMAND_SET_PREFIX);
    protobuf_get(self, PACKET_INDEX, &index);
   
    if (size <= PACKETPARAMETERSIZE && index != 0) {
        char *s;
        protobuf_get(self, PACKET_STR, &s);
        if (s != prefix) {
            memcpy(s, prefix, size);
        }
        protobuf_set(self, PACKET_LENGTH, 0);
    } else {
        uint32_t length = (uint32_t) size;
        protobuf_set(self, PACKET_LENGTH, length);
        char *buf;
        protobuf_get(self, PACKET_BUF, &buf, NULL);
        size_t payload = protobuf_payload(self);
        if (payload < size) {
            return AAOS_ERROR;
        }
        if (buf != prefix) {
            memcpy(buf, prefix, size);
        }
    }
    
    return rpc_call(self);
}

int
detector_get_prefix(void *_self, char *prefix, size_t size)
{
    const struct DetectorClass *class = (const struct DetectorClass *) classOf(_self);
    
    if (isOf(class, DetectorClass()) && class->get_temperature.method) {
        return ((int (*)(void *, char *, size_t)) class->get_temperature.method)(_self, prefix, size);
    } else {
        int result;
        forward(_self, &result, (Method) detector_get_prefix, "get_prefix", _self, prefix, size);
        return result;
    }
}

static int
Detector_get_prefix(void *_self, char *prefix, size_t size)
{
    struct Detector *self = cast(Detector(), _self);
    
    void *protobuf = self->_.protobuf;
    int ret = AAOS_OK;
    
    protobuf_set(protobuf, PACKET_PROTOCOL, PROTO_DETECTOR);
    protobuf_set(protobuf, PACKET_COMMAND, DETECTOR_COMMAND_GET_PREFIX);
    
    if ((ret = rpc_call(self)) != AAOS_OK) {
        return ret;
    }
    
    Detector_get_result(protobuf, DETECTOR_COMMAND_GET_PREFIX, prefix, size, NULL);
    
    return ret;
}

int
detector_set_directory(void *_self, const char *directory)
{
    const struct DetectorClass *class = (const struct DetectorClass *) classOf(_self);
    
    if (isOf(class, DetectorClass()) && class->set_directory.method) {
        return ((int (*)(void *, const char *)) class->set_directory.method)(_self, directory);
    } else {
        int result;
        forward(_self, &result, (Method) detector_set_directory, "set_directory", _self, directory);
        return result;
    }
}

static int
Detector_set_directory(void *_self, const char *directory)
{
    struct Detector *self = cast(Detector(), _self);
    
    uint16_t index;
    size_t size = strlen(directory) + 1;
    
    if (directory == NULL) {
        return -1 * AAOS_EINVAL;
    }
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_DETECTOR);
    protobuf_set(self, PACKET_COMMAND, DETECTOR_COMMAND_SET_DIRECTORY);
    protobuf_get(self, PACKET_INDEX, &index);
   
    if (size <= PACKETPARAMETERSIZE && index != 0) {
        char *s;
        protobuf_get(self, PACKET_STR, &s);
        if (s != directory) {
            memcpy(s, directory, size);
        }
        protobuf_set(self, PACKET_LENGTH, 0);
    } else {
        uint32_t length = (uint32_t) size;
        protobuf_set(self, PACKET_LENGTH, length);
        char *buf;
        protobuf_get(self, PACKET_BUF, &buf, NULL);
        size_t payload = protobuf_payload(self);
        if (payload < size) {
            return AAOS_ERROR;
        }
        if (buf != directory) {
            memcpy(buf, directory, size);
        }
    }
    
    return rpc_call(self);
}

int
detector_get_directory(void *_self, char *directory, size_t size)
{
    const struct DetectorClass *class = (const struct DetectorClass *) classOf(_self);
    
    if (isOf(class, DetectorClass()) && class->get_temperature.method) {
        return ((int (*)(void *, char *, size_t)) class->get_temperature.method)(_self, directory, size);
    } else {
        int result;
        forward(_self, &result, (Method) detector_get_directory, "get_directory", _self, directory, size);
        return result;
    }
}

static int
Detector_get_directory(void *_self, char *directory, size_t size)
{
    struct Detector *self = cast(Detector(), _self);
    
    void *protobuf = self->_.protobuf;
    int ret = AAOS_OK;
    
    protobuf_set(protobuf, PACKET_PROTOCOL, PROTO_DETECTOR);
    protobuf_set(protobuf, PACKET_COMMAND, DETECTOR_COMMAND_GET_DIRECTORY);
    
    if ((ret = rpc_call(self)) != AAOS_OK) {
        return ret;
    }
    
    Detector_get_result(protobuf, DETECTOR_COMMAND_GET_DIRECTORY, directory, size, NULL);
    
    return ret;
}

int
detector_set_template(void *_self, const char *template)
{
    const struct DetectorClass *class = (const struct DetectorClass *) classOf(_self);
    
    if (isOf(class, DetectorClass()) && class->set_template.method) {
        return ((int (*)(void *, const char *)) class->set_template.method)(_self, template);
    } else {
        int result;
        forward(_self, &result, (Method) detector_set_template, "set_template", _self, template);
        return result;
    }
}

static int
Detector_set_template(void *_self, const char *template)
{
    struct Detector *self = cast(Detector(), _self);
    
    uint16_t index;
    size_t size = strlen(template) + 1;
    
    if (template == NULL) {
        return -1 * AAOS_EINVAL;
    }
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_DETECTOR);
    protobuf_set(self, PACKET_COMMAND, DETECTOR_COMMAND_SET_TEMPLATE);
    protobuf_get(self, PACKET_INDEX, &index);
   
    if (size <= PACKETPARAMETERSIZE && index != 0) {
        char *s;
        protobuf_get(self, PACKET_STR, &s);
        if (s != template) {
            memcpy(s, template, size);
        }
        protobuf_set(self, PACKET_LENGTH, 0);
    } else {
        uint32_t length = (uint32_t) size;
        protobuf_set(self, PACKET_LENGTH, length);
        char *buf;
        protobuf_get(self, PACKET_BUF, &buf, NULL);
        size_t payload = protobuf_payload(self);
        if (payload < size) {
            return AAOS_ERROR;
        }
        if (buf != template) {
            memcpy(buf, template, size);
        }
    }
    
    return rpc_call(self);
}

int
detector_get_template(void *_self, char *template, size_t size)
{
    const struct DetectorClass *class = (const struct DetectorClass *) classOf(_self);
    
    if (isOf(class, DetectorClass()) && class->get_temperature.method) {
        return ((int (*)(void *, char *, size_t)) class->get_temperature.method)(_self, template, size);
    } else {
        int result;
        forward(_self, &result, (Method) detector_get_template, "get_template", _self, template, size);
        return result;
    }
}

static int
Detector_get_template(void *_self, char *template, size_t size)
{
    struct Detector *self = cast(Detector(), _self);
    
    void *protobuf = self->_.protobuf;
    int ret = AAOS_OK;
    
    protobuf_set(protobuf, PACKET_PROTOCOL, PROTO_DETECTOR);
    protobuf_set(protobuf, PACKET_COMMAND, DETECTOR_COMMAND_GET_TEMPLATE);
    
    if ((ret = rpc_call(self)) != AAOS_OK) {
        return ret;
    }
    
    Detector_get_result(protobuf, DETECTOR_COMMAND_GET_TEMPLATE, template, size, NULL);
    
    return ret;
}

int
detector_get_image(void *_self, const char *filename)
{
    const struct DetectorClass *class = (const struct DetectorClass *) classOf(_self);
    
    if (isOf(class, DetectorClass()) && class->get_image.method) {
        return ((int (*)(void *, const char *)) class->delete_image.method)(_self, filename);
    } else {
        int result;
        forward(_self, &result, (Method) detector_get_image, "get_image", _self, filename);
        return result;
    }
}

static int
Detector_get_image(void *_self, const char *filename)
{
    struct Detector *self = cast(Detector(), _self);
    
    void *protobuf = self->_.protobuf;
    uint16_t index;
    int ret;
    size_t n, nread = 0;
    uint64_t file_size;
    ssize_t nleft;
    int fd;
    char buf[4096];
    
    if ((fd = Open(filename, O_CREAT | O_EXCL)) < 0) {
        switch (errno) {
            case EACCES:
                return AAOS_EACCES;
                break;
            case EEXIST:
                return AAOS_EEXIST;
                break;
            default:
                return AAOS_ERROR;
                break;
        }
    }
    
    protobuf_get(protobuf, PACKET_INDEX, &index);
    
    if (index == 0 && strlen(filename) < PACKETPARAMETERSIZE - 1) {
        char *s;
        protobuf_get(protobuf, PACKET_STR, &s);
        snprintf(s, PACKETPARAMETERSIZE, "%s", filename);
        protobuf_set(protobuf, PACKET_LENGTH, 0);
    } else {
        protobuf_set(protobuf, PACKET_BUF, filename, strlen(filename) + 1);
    }
    
    protobuf_set(protobuf, PACKET_PROTOCOL, PROTO_DETECTOR);
    protobuf_set(protobuf, PACKET_COMMAND, DETECTOR_COMMAND_GET_IMAGE);
    
    if ((ret = rpc_call(self)) != AAOS_OK) {
        return ret;
    }
    
    protobuf_get(self, PACKET_U64F0, &file_size);
    
    nleft = (ssize_t) file_size;
    
    
    while (nleft > 0) {
        tcp_socket_read(self, buf, 4096, &n);
        nleft -= n;
        nread += n;
        Write(fd, buf, 4096);
    }
    Close(fd);
    
    return rpc_call(self);
}

int
detector_delete_image(void *_self, const char *filename)
{
    const struct DetectorClass *class = (const struct DetectorClass *) classOf(_self);
    
    if (isOf(class, DetectorClass()) && class->delete_image.method) {
        return ((int (*)(void *, const char *)) class->delete_image.method)(_self, filename);
    } else {
        int result;
        forward(_self, &result, (Method) detector_delete_image, "delete_image", _self, filename);
        return result;
    }
}

static int
Detector_delete_image(void *_self, const char *filename)
{
    struct Detector *self = cast(Detector(), _self);
    
    void *protobuf = self->_.protobuf;
    uint16_t index;
    
    protobuf_get(protobuf, PACKET_INDEX, &index);
    
    if (index == 0 && strlen(filename) < PACKETPARAMETERSIZE - 1) {
        char *s;
        protobuf_get(protobuf, PACKET_STR, &s);
        snprintf(s, PACKETPARAMETERSIZE, "%s", filename);
        protobuf_set(protobuf, PACKET_LENGTH, 0);
    } else {
        protobuf_set(protobuf, PACKET_BUF, filename, strlen(filename) + 1);
    }
    
    protobuf_set(protobuf, PACKET_PROTOCOL, PROTO_DETECTOR);
    protobuf_set(protobuf, PACKET_COMMAND, DETECTOR_COMMAND_DELETE_IMAGE);
    
    return rpc_call(self);
}

int
detector_delete_all_image(void *_self)
{
    const struct DetectorClass *class = (const struct DetectorClass *) classOf(_self);
    
    if (isOf(class, DetectorClass()) && class->delete_all_image.method) {
        return ((int (*)(void *)) class->delete_all_image.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) detector_delete_all_image, "delete_all_image", _self);
        return result;
    }
}

static int
Detector_delete_all_image(void *_self)
{
    struct Detector *self = cast(Detector(), _self);
    
    void *protobuf = self->_.protobuf;
    
    protobuf_set(protobuf, PACKET_PROTOCOL, PROTO_DETECTOR);
    protobuf_set(protobuf, PACKET_COMMAND, DETECTOR_COMMAND_DELETE_ALL_IMAGE);
    
    return rpc_call(self);
}

int
detector_list_image(void *_self, char *filelist, size_t size)
{
    const struct DetectorClass *class = (const struct DetectorClass *) classOf(_self);
    
    if (isOf(class, DetectorClass()) && class->list_image.method) {
        return ((int (*)(void *, char *, size_t)) class->list_image.method)(_self, filelist, size);
    } else {
        int result;
        forward(_self, &result, (Method) detector_list_image, "list_image", _self, filelist, size);
        return result;
    }
}

static int
Detector_list_image(void *_self, char *filelist, size_t size)
{
    struct Detector *self = cast(Detector(), _self);
    
    void *protobuf = self->_.protobuf;
    int ret = AAOS_OK;
    
    protobuf_set(protobuf, PACKET_PROTOCOL, PROTO_DETECTOR);
    protobuf_set(protobuf, PACKET_COMMAND, DETECTOR_COMMAND_LIST_IMAGE);
    
    if ((ret = rpc_call(self)) != AAOS_OK) {
        return ret;
    }
    
    Detector_get_result(protobuf, DETECTOR_COMMAND_GET_TEMPLATE, filelist, size, NULL);
    
    return ret;
}

int
detector_expose(void *_self, double exposure_time, uint32_t n_frame, void (*image_callback)(void *, const char *, ...), ...)
{
    const struct DetectorClass *class = (const struct DetectorClass *) classOf(_self);
    
    int result;
    va_list ap;
    va_start(ap, image_callback);
    if (isOf(class, DetectorClass()) && class->expose.method) {
        result = ((int (*)(void *, double, uint32_t, void (*)(void *, const char *, ...), va_list *)) class->expose.method)(_self, exposure_time, n_frame, image_callback, &ap);
    } else {
        forward(_self, &result, (Method) detector_expose, "expose", _self, exposure_time, n_frame, image_callback, &ap);
    }
    va_end(ap);
    
    return result;
}

#include <cjson/cJSON.h>

static int
Detector_expose(void *_self, double exposure_time, uint32_t n_frame, void (*image_callback)(void *, const char *, ...), va_list *app)
{
    struct Detector *self = cast(Detector(), _self);
    
    void *protobuf = self->_.protobuf;
    uint16_t option;
    int ret = AAOS_OK;
    char filename[FILENAMESIZE];
    uint32_t i, length;
    
    protobuf_set(protobuf, PACKET_PROTOCOL, PROTO_DETECTOR);
    protobuf_set(protobuf, PACKET_COMMAND, DETECTOR_COMMAND_EXPOSE);
    protobuf_set(protobuf, PACKET_DF0, exposure_time);
    protobuf_set(protobuf, PACKET_U32F2, n_frame);
    
    /*
     * Serialize the input parameters.
     */
    protobuf_get(protobuf, PACKET_LENGTH, &length);
    char *buffer = NULL;
    cJSON *params = cJSON_CreateObject(), *entry = NULL;
    const char *keyname, *keyval;
    while ((keyname = va_arg(*app, const char *))) {
        keyval = va_arg(*app, const char *);
        entry = cJSON_CreateString(keyval);
        cJSON_AddItemToObject(params, keyname, entry);
    }
    
    buffer = cJSON_Print(params);
    cJSON_Delete(params);
    if (strcmp(buffer, "{\nn}") != 0) {
        if (length == 0) {
            protobuf_set(protobuf, PACKET_BUF, buffer, strlen(buffer) + 1);
        } else {
            char *s;
            size_t payload = protobuf_payload(protobuf);
            if (payload < length + strlen(buffer) + 1) {
                protobuf_reallocate(protobuf, length + strlen(buffer) + 1);
            }
            protobuf_get(protobuf, PACKET_BUF, &s, NULL);
            s += length;
            snprintf(s, strlen(buffer) + 1, "%s", buffer);
            protobuf_set(protobuf, PACKET_LENGTH, length + strlen(buffer) + 1);
        }
    }
    
    free(buffer);
    
    if ((ret = rpc_write(self)) != AAOS_OK) {
        return ret;
    }
    protobuf_get(protobuf, PACKET_OPTION, &option);
    if (option & DETECTOR_OPTION_NOTIFY_EACH_COMPLETION) {
        
        if (option & DETECTOR_OPTION_NOTIFY_LAST_FILLING) {
            for (i = 0; i < n_frame - 1; i++) {
                if ((ret = rpc_read(self)) != AAOS_OK) {
                    goto error;
                }
                memset(filename, '\0', FILENAMESIZE);
                Detector_get_result(protobuf, DETECTOR_COMMAND_EXPOSE, filename, FILENAMESIZE, NULL);
                if (image_callback == NULL) {
                    fprintf(stderr, "%s", filename);
                } else {
                    image_callback(_self, filename);
                }
            }
        } else {
            for (i = 0; i < n_frame; i++) {
                if ((ret = rpc_read(self)) != AAOS_OK ) {
                    goto error;
                }
                memset(filename, '\0', FILENAMESIZE);
                Detector_get_result(protobuf, DETECTOR_COMMAND_EXPOSE, filename, FILENAMESIZE, NULL);
                if (image_callback == NULL) {
                    fprintf(stderr, "%s", filename);
                } else {
                    image_callback(_self, filename);
                }
            }
        }
    } else {
        if ((ret = rpc_read(self)) != AAOS_OK ) {
            goto error;
        }
    }
    
error:
    return rpc_read(self);
}

int
detector_wait_for_completion(void *_self, void (*image_callback)(void *_self, const char *filename, ...))
{
    const struct DetectorClass *class = (const struct DetectorClass *) classOf(_self);
    
    if (isOf(class, DetectorClass()) && class->wait_for_completion.method) {
        return ((int (*)(void *, void (*)(void *, const char *, ...) )) class->expose.method)(_self, image_callback);
    } else {
        int result;
        forward(_self, &result, (Method) detector_wait_for_completion, "wait_for_completion", _self, image_callback);
        return result;
    }
}

static int
Detector_wait_for_completion(void *_self, void (*image_callback)(void *, const char *, ...))
{
    struct Detector *self = cast(Detector(), _self);
    
    void *protobuf = self->_.protobuf;
    uint16_t option;
    int ret = AAOS_OK;
    char filename[FILENAMESIZE];
    
    protobuf_set(protobuf, PACKET_PROTOCOL, PROTO_DETECTOR);
    protobuf_set(protobuf, PACKET_COMMAND, DETECTOR_COMMAND_WAIT_FOR_COMPLETION);
   
    protobuf_get(protobuf, PACKET_OPTION, &option);
    
    if (option & DETECTOR_OPTION_NOTIFY_LAST_FILLING) {
        if ((ret = rpc_call(self)) != AAOS_OK ) {
            return ret;
        }
        memset(filename, '\0', FILENAMESIZE);
        Detector_get_result(protobuf, DETECTOR_COMMAND_EXPOSE, filename, FILENAMESIZE, NULL);
        if (image_callback == NULL) {
            fprintf(stderr, "%s", filename);
        } else {
            image_callback(_self, filename);
        }
    }
    
    return ret;
}

int
detector_raw(void *_self, const void *cmd, size_t cmd_size, void *res, size_t res_size, size_t *res_len)
{
    const struct DetectorClass *class = (const struct DetectorClass *) classOf(_self);
    
    if (isOf(class, DetectorClass()) && class->raw.method) {
        return ((int (*)(void *, const void *cmd, size_t, void *, size_t, size_t *)) class->raw.method)(_self, cmd, cmd_size, res, res_size, res_len);
    } else {
        int result;
        forward(_self, &result, (Method) detector_raw, "raw", _self, cmd, cmd_size, res, res_size, res_len);
        return result;
    }
}

static int
Detector_raw(void *_self, const void *cmd, size_t cmd_size, void *res, size_t res_size, size_t *res_len)
{
    struct Detector *self = cast(Detector(), _self);
    uint16_t index;
    int ret;
    
    if (cmd == NULL || res == NULL) {
        return -1 * AAOS_EINVAL;
    }
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_DETECTOR);
    protobuf_set(self, PACKET_COMMAND, DETECTOR_COMMAND_RAW);
    protobuf_get(self, PACKET_INDEX, &index);
   
    if (cmd_size < PACKETPARAMETERSIZE && index != 0) {
        char *s;
        protobuf_get(self, PACKET_STR, &s);
        if (s != cmd) {
            memcpy(s, cmd, cmd_size);
            s[cmd_size] = '\0';
        }
        protobuf_set(self, PACKET_LENGTH, 0);
    } else {
        uint32_t length = (uint32_t) cmd_size;
        protobuf_set(self, PACKET_LENGTH, length);
        char *buf;
        protobuf_get(self, PACKET_BUF, &buf, NULL);
        size_t payload = protobuf_payload(self);
        if (payload < cmd_size) {
            return AAOS_ERROR;
        }
        if (buf != cmd) {
            memcpy(buf, cmd, cmd_size);
            buf[cmd_size] = '\0';
        }
    }
    if ((ret = rpc_call(self)) == AAOS_OK) {
        uint32_t length;
        protobuf_get(self, PACKET_LENGTH, &length);
        if (length == 0) {
            char *s;
            protobuf_get(self, PACKET_STR, &s);
            if (s != res) {
                snprintf(res, res_size, "%s", s);
            }
            if (res_len != NULL) {
                *res_len = strlen(res);
            }
        } else {
            char *buf;
            protobuf_get(self, PACKET_BUF, &buf, NULL);
            if (buf != res) {
                snprintf(res, res_size, "%s", buf);
            }
            if (res_len != NULL) {
                *res_len = strlen(res);
            }
        }
    }
    return ret;
}

/*
 * Virtual function for detector rpc.
 */
static const void *detector_virtual_table(void);

static int
Detector_execute_info(struct Detector *self)
{
    void *buf;
    size_t size;
    uint16_t index;
    uint32_t length;
    void *detector;
    int ret;
    
    protobuf_get(self, PACKET_INDEX, &index);
    protobuf_get(self, PACKET_LENGTH, &length);
    
    if (index == 0) {
        int idx;
        char *s;
        if (length == 0) {
            protobuf_get(self, PACKET_STR, &s);
        } else {
            protobuf_get(self, PACKET_BUF, &s, NULL);
        }
        get_index_by_name(s, &idx);
        index = (uint16_t) idx;
        protobuf_set(self, PACKET_INDEX, &index);
        if ((detector = get_detector_by_name(s)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    } else {
        if ((detector = get_detector_by_index((int) index)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    }

    size = protobuf_payload(self);
    protobuf_get(self, PACKET_BUF, &buf, NULL);
    
    if ((ret =  __detector_info(detector, buf, size, NULL)) != AAOS_OK) {
        protobuf_set(self, PACKET_ERRORCODE, ret);
        protobuf_set(self, PACKET_LENGTH, 0);
    } else {
        protobuf_set(self, PACKET_LENGTH, strlen(buf) + 1);
    }
    
    return ret;
}

/*
 * if a bad command is given, just return AAOS_OK,
 * but set the error code AAOS_EBADCMD.
 */

static int
Detector_execute_raw(struct Detector *self)
{
    char *command;
    int ret;
    uint16_t index;
    uint16_t option;
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
    protobuf_get(self, PACKET_OPTION, &option);
    
    if (index == 0) {
        /*
         * If index is zero, means STR field contains device name or device path, payload must none zero.
         */
        if (length == 0) {
            return AAOS_EBADCMD;
        } else {
            char *s, *buf;
            void *detector;
            int idx;
            size_t payload, read_size, length;
            protobuf_get(self, PACKET_STR, &s);
            
            get_index_by_name(s, &idx);
            
            index = (uint16_t) idx;
            protobuf_set(self, PACKET_INDEX, &index);
            if ((detector = get_detector_by_name(s)) == NULL) {
                protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
                protobuf_set(self, PACKET_LENGTH, 0);
                return AAOS_ENOTFOUND;
            }
            protobuf_get(self, PACKET_BUF, &buf, &length);
            if (length == 0) {
                length = (uint32_t) strlen(command);
            }
            payload = protobuf_payload(self);
            if ((ret = __detector_raw(detector, command, length, NULL, buf, payload, &read_size)) != AAOS_OK) {
                return ret;
            }
            /*
             * If STR field is large enough and not in binary mode, use it.
             */
            if (read_size < PACKETPARAMETERSIZE) {
                memcpy(s, buf, read_size);
                protobuf_set(self, PACKET_LENGTH, 0);
            } else {
                length = (uint32_t) read_size;
                protobuf_set(self, PACKET_LENGTH, length);
            }
            protobuf_set(self, PACKET_ERRORCODE, AAOS_OK);
        }
    } else {
        char *s, *buf;
        void *detector;
        if ((detector = get_detector_by_index((int) index)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
        size_t payload, read_size;
        protobuf_get(self, PACKET_LENGTH, &length);
        if (length == 0) {
            length = (uint32_t) strlen(command);
        }
        payload = protobuf_payload(self);
        protobuf_get(self, PACKET_BUF, &buf, NULL);
        
        if ((ret = __detector_raw(detector, command, length, NULL, buf, payload, &read_size)) != AAOS_OK) {
            return ret;
        }
        if (read_size < PACKETPARAMETERSIZE) {
            protobuf_get(self, PACKET_STR, &s);
            memcpy(s, buf, read_size);
            protobuf_set(self, PACKET_LENGTH, 0);
        } else {
            length = (uint32_t) read_size;
            protobuf_set(self, PACKET_LENGTH, length);
        }
        protobuf_set(self, PACKET_ERRORCODE, AAOS_OK);
    }
    return ret;
}

static int
Detector_execute_unload(struct Detector *self)
{
    uint16_t index;
    void *detector;
    
    protobuf_get(self, PACKET_INDEX, &index);
    
    if (index == 0) {
        uint32_t length;
        char *s;
        int idx;
        protobuf_get(self, PACKET_LENGTH, &length);
        if (length == 0) {
            protobuf_get(self, PACKET_STR, &s);
        } else {
            protobuf_get(self, PACKET_BUF, &s, NULL);
        }
        get_index_by_name(s, &idx);
        
        index = (uint16_t) idx;
        protobuf_set(self, PACKET_INDEX, &index);
        if ((detector = get_detector_by_name(s)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    } else {
        if ((detector = get_detector_by_index((int) index)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    }
    protobuf_set(self, PACKET_LENGTH, 0);
    
    return __detector_unload(detector);
}

static int
Detector_execute_register(struct Detector *self)
{
    void *protobuf = self->_.protobuf;
    
    uint16_t index;
    void *detector;
    double timeout;
    int ret;
    
    protobuf_get(protobuf, PACKET_INDEX, &index);
    protobuf_get(protobuf, PACKET_DF0, &timeout);
    
    if ((detector = get_detector_by_index((int) index)) == NULL) {
        protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
        protobuf_set(self, PACKET_LENGTH, 0);
        return AAOS_ENOTFOUND;
    }
    
    if (__detector_inspect(detector) != AAOS_OK) {
        ret = __detector_wait(detector, timeout);
        protobuf_set(self, PACKET_ERRORCODE, AAOS_ETIMEDOUT);
        protobuf_set(self, PACKET_LENGTH, 0);
        return ret;
    }
    protobuf_set(self, PACKET_LENGTH, 0);
    
    return AAOS_OK;
}

static int
Detector_execute_reload(struct Detector *self)
{
    uint16_t index;
    void *detector;
    
    protobuf_get(self, PACKET_INDEX, &index);
    
    if (index == 0) {
        uint32_t length;
        char *s;
        int idx;
        protobuf_get(self, PACKET_LENGTH, &length);
        if (length == 0) {
            protobuf_get(self, PACKET_STR, &s);
        } else {
            protobuf_get(self, PACKET_BUF, &s, NULL);
        }
        get_index_by_name(s, &idx);
        
        index = (uint16_t) idx;
        protobuf_set(self, PACKET_INDEX, &index);
        if ((detector = get_detector_by_name(s)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    } else {
        if ((detector = get_detector_by_index((int) index)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    }
    protobuf_set(self, PACKET_LENGTH, 0);
    
    return __detector_reload(detector);
}

static int
Detector_execute_expose(struct Detector *self)
{
    uint16_t index;
    void *detector;
    double exposure_time;
    uint32_t n_frame;
    
    protobuf_get(self, PACKET_INDEX, &index);
    protobuf_get(self, PACKET_DF0, &exposure_time);
    protobuf_get(self, PACKET_U32F0, &n_frame);
    
    
    if (index == 0) {
        uint32_t length;
        char *s;
        int idx;
        protobuf_get(self, PACKET_LENGTH, &length);
        if (length == 0) {
            protobuf_get(self, PACKET_STR, &s);
        } else {
            protobuf_get(self, PACKET_BUF, &s, NULL);
        }
        get_index_by_name(s, &idx);
        index = (uint16_t) idx;
        protobuf_set(self, PACKET_INDEX, &index);
        if ((detector = get_detector_by_name(s)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
        if (length != 0) {
            memmove(s, s + strlen(s), length - strlen(s) - 1);
        }
    } else {
        if ((detector = get_detector_by_index((int) index)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    }
    
    return __detector_expose(detector, exposure_time, n_frame, self);
}

static int
Detector_execute_wait_for_completion(struct Detector *self)
{
    uint16_t index;
    void *detector;

    protobuf_get(self, PACKET_INDEX, &index);
  
    if (index == 0) {
        uint32_t length;
        char *s;
        int idx;
        protobuf_get(self, PACKET_LENGTH, &length);
        if (length == 0) {
            protobuf_get(self, PACKET_STR, &s);
        } else {
            protobuf_get(self, PACKET_BUF, &s, NULL);
        }
        get_index_by_name(s, &idx);
        
        index = (uint16_t) idx;
        protobuf_set(self, PACKET_INDEX, &index);
        if ((detector = get_detector_by_name(s)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    } else {
        if ((detector = get_detector_by_index((int) index)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    }

    return __detector_wait_for_completion(self);
}

static int
Detector_execute_load(struct Detector *self)
{
    uint16_t index;
    void *detector;
    
    protobuf_get(self, PACKET_INDEX, &index);
    
    if (index == 0) {
        uint32_t length;
        char *s;
        int idx;
        protobuf_get(self, PACKET_LENGTH, &length);
        if (length == 0) {
            protobuf_get(self, PACKET_STR, &s);
        } else {
            protobuf_get(self, PACKET_BUF, &s, NULL);
        }
        get_index_by_name(s, &idx);
        
        index = (uint16_t) idx;
        protobuf_set(self, PACKET_INDEX, &index);
        if ((detector = get_detector_by_name(s)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    } else {
        if ((detector = get_detector_by_index((int) index)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    }
    
    protobuf_set(self, PACKET_LENGTH, 0);
    return __detector_load(detector);
}

static int
Detector_execute_inspect(struct Detector *self)
{
    uint16_t index;
    void *detector;
    
    protobuf_get(self, PACKET_INDEX, &index);
    
    if (index == 0) {
        uint32_t length;
        char *s;
        int idx;
        protobuf_get(self, PACKET_LENGTH, &length);
        if (length == 0) {
            protobuf_get(self, PACKET_STR, &s);
        } else {
            protobuf_get(self, PACKET_BUF, &s, NULL);
        }
        get_index_by_name(s, &idx);
        
        index = (uint16_t) idx;
        protobuf_set(self, PACKET_INDEX, &index);
        if ((detector = get_detector_by_name(s)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    } else {
        if ((detector = get_detector_by_index((int) index)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    }
    protobuf_set(self, PACKET_LENGTH, 0);
    return __detector_inspect(detector);
}

static int
Detector_execute_abort(struct Detector *self)
{
    uint16_t index;
    void *detector;
    
    protobuf_get(self, PACKET_INDEX, &index);
    
    if (index == 0) {
        uint32_t length;
        char *s;
        int idx;
        protobuf_get(self, PACKET_LENGTH, &length);
        if (length == 0) {
            protobuf_get(self, PACKET_STR, &s);
        } else {
            protobuf_get(self, PACKET_BUF, &s, NULL);
        }
        get_index_by_name(s, &idx);
        
        index = (uint16_t) idx;
        protobuf_set(self, PACKET_INDEX, &index);
        if ((detector = get_detector_by_name(s)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    } else {
        if ((detector = get_detector_by_index((int) index)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    }
    protobuf_set(self, PACKET_LENGTH, 0);
    return __detector_abort(detector);
}

static int
Detector_execute_stop(struct Detector *self)
{
    uint16_t index;
    void *detector;
    
    protobuf_get(self, PACKET_INDEX, &index);
    
    if (index == 0) {
        uint32_t length;
        char *s;
        int idx;
        protobuf_get(self, PACKET_LENGTH, &length);
        if (length == 0) {
            protobuf_get(self, PACKET_STR, &s);
        } else {
            protobuf_get(self, PACKET_BUF, &s, NULL);
        }
        get_index_by_name(s, &idx);
        
        index = (uint16_t) idx;
        protobuf_set(self, PACKET_INDEX, &index);
        if ((detector = get_detector_by_name(s)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    } else {
        if ((detector = get_detector_by_index((int) index)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    }
    protobuf_set(self, PACKET_LENGTH, 0);
    return __detector_stop(detector);
}

static int
Detector_execute_set_binning(struct Detector *self)
{
    uint16_t index;
    void *detector;
    uint32_t x_binning, y_binning;
    
    protobuf_get(self, PACKET_INDEX, &index);
    
    if (index == 0) {
        uint32_t length;
        char *s;
        int idx;
        protobuf_get(self, PACKET_LENGTH, &length);
        if (length == 0) {
            protobuf_get(self, PACKET_STR, &s);
        } else {
            protobuf_get(self, PACKET_BUF, &s, NULL);
        }
        get_index_by_name(s, &idx);
        
        index = (uint16_t) idx;
        protobuf_set(self, PACKET_INDEX, &index);
        if ((detector = get_detector_by_name(s)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    } else {
        if ((detector = get_detector_by_index((int) index)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    }
    protobuf_get(self, PACKET_U32F0, &x_binning);
    protobuf_get(self, PACKET_U32F1, &y_binning);
    protobuf_set(self, PACKET_LENGTH, 0);
    return __detector_set_binning(detector, x_binning, y_binning);
}

static int
Detector_execute_get_binning(struct Detector *self)
{
    uint16_t index;
    void *detector;
    uint32_t x_binning, y_binning;
    int ret = AAOS_OK;
    
    protobuf_get(self, PACKET_INDEX, &index);
    
    if (index == 0) {
        uint32_t length;
        char *s;
        int idx;
        protobuf_get(self, PACKET_LENGTH, &length);
        if (length == 0) {
            protobuf_get(self, PACKET_STR, &s);
        } else {
            protobuf_get(self, PACKET_BUF, &s, NULL);
        }
        get_index_by_name(s, &idx);
        
        index = (uint16_t) idx;
        protobuf_set(self, PACKET_INDEX, &index);
        if ((detector = get_detector_by_name(s)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    } else {
        if ((detector = get_detector_by_index((int) index)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    }
    
    if ((ret = __detector_get_binning(detector, &x_binning, &y_binning)) != AAOS_OK) {
        return ret;
    }
    
    protobuf_set(self, PACKET_U32F0, x_binning);
    protobuf_set(self, PACKET_U32F1, y_binning);
    protobuf_set(self, PACKET_LENGTH, 0);
    
    return ret;
}

static int
Detector_execute_set_exposure_time(struct Detector *self)
{
    uint16_t index;
    void *detector;
    double exposure_time;
    
    protobuf_get(self, PACKET_INDEX, &index);
    
    if (index == 0) {
        uint32_t length;
        char *s;
        int idx;
        protobuf_get(self, PACKET_LENGTH, &length);
        if (length == 0) {
            protobuf_get(self, PACKET_STR, &s);
        } else {
            protobuf_get(self, PACKET_BUF, &s, NULL);
        }
        get_index_by_name(s, &idx);
        
        index = (uint16_t) idx;
        protobuf_set(self, PACKET_INDEX, &index);
        if ((detector = get_detector_by_name(s)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    } else {
        if ((detector = get_detector_by_index((int) index)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    }
    protobuf_get(self, PACKET_DF0, &exposure_time);
    protobuf_set(self, PACKET_LENGTH, 0);
    return __detector_set_exposure_time(detector, exposure_time);
}

static int
Detector_execute_get_exposure_time(struct Detector *self)
{
    uint16_t index;
    void *detector;
    double exposure_time;
    int ret = AAOS_OK;
    
    protobuf_get(self, PACKET_INDEX, &index);
    
    if (index == 0) {
        uint32_t length;
        char *s;
        int idx;
        protobuf_get(self, PACKET_LENGTH, &length);
        if (length == 0) {
            protobuf_get(self, PACKET_STR, &s);
        } else {
            protobuf_get(self, PACKET_BUF, &s, NULL);
        }
        get_index_by_name(s, &idx);
        
        index = (uint16_t) idx;
        protobuf_set(self, PACKET_INDEX, &index);
        if ((detector = get_detector_by_name(s)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    } else {
        if ((detector = get_detector_by_index((int) index)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    }
    
    if ((ret = __detector_get_exposure_time(detector, &exposure_time)) != AAOS_OK) {
        return ret;
    }
    
    protobuf_set(self, PACKET_DF0, exposure_time);
    protobuf_set(self, PACKET_LENGTH, 0);
    
    return ret;
}

static int
Detector_execute_set_frame_rate(struct Detector *self)
{
    uint16_t index;
    void *detector;
    double frame_rate;
    
    protobuf_get(self, PACKET_INDEX, &index);
    
    if (index == 0) {
        uint32_t length;
        char *s;
        int idx;
        protobuf_get(self, PACKET_LENGTH, &length);
        if (length == 0) {
            protobuf_get(self, PACKET_STR, &s);
        } else {
            protobuf_get(self, PACKET_BUF, &s, NULL);
        }
        get_index_by_name(s, &idx);
        
        index = (uint16_t) idx;
        protobuf_set(self, PACKET_INDEX, &index);
        if ((detector = get_detector_by_name(s)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    } else {
        if ((detector = get_detector_by_index((int) index)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    }
    protobuf_get(self, PACKET_DF0, &frame_rate);
    protobuf_set(self, PACKET_LENGTH, 0);
    
    return __detector_set_frame_rate(detector, frame_rate);
}

static int
Detector_execute_get_frame_rate(struct Detector *self)
{
    uint16_t index;
    void *detector;
    double frame_rate;
    int ret = AAOS_OK;
    
    protobuf_get(self, PACKET_INDEX, &index);
    
    if (index == 0) {
        uint32_t length;
        char *s;
        int idx;
        protobuf_get(self, PACKET_LENGTH, &length);
        if (length == 0) {
            protobuf_get(self, PACKET_STR, &s);
        } else {
            protobuf_get(self, PACKET_BUF, &s, NULL);
        }
        get_index_by_name(s, &idx);
        
        index = (uint16_t) idx;
        protobuf_set(self, PACKET_INDEX, &index);
        if ((detector = get_detector_by_name(s)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    } else {
        if ((detector = get_detector_by_index((int) index)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    }
    
    if ((ret = __detector_get_frame_rate(detector, &frame_rate)) != AAOS_OK) {
        return ret;
    }
    
    protobuf_set(self, PACKET_DF0, frame_rate);
    protobuf_set(self, PACKET_LENGTH, 0);
    
    return ret;
}

static int
Detector_execute_set_gain(struct Detector *self)
{
    uint16_t index;
    void *detector;
    double gain;
    
    protobuf_get(self, PACKET_INDEX, &index);
    
    if (index == 0) {
        uint32_t length;
        char *s;
        int idx;
        protobuf_get(self, PACKET_LENGTH, &length);
        if (length == 0) {
            protobuf_get(self, PACKET_STR, &s);
        } else {
            protobuf_get(self, PACKET_BUF, &s, NULL);
        }
        get_index_by_name(s, &idx);
        
        index = (uint16_t) idx;
        protobuf_set(self, PACKET_INDEX, &index);
        if ((detector = get_detector_by_name(s)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    } else {
        if ((detector = get_detector_by_index((int) index)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    }
    protobuf_get(self, PACKET_DF0, &gain);
    protobuf_set(self, PACKET_LENGTH, 0);
    
    return __detector_set_gain(detector, gain);
}

static int
Detector_execute_get_gain(struct Detector *self)
{
    uint16_t index;
    void *detector;
    double gain;
    int ret = AAOS_OK;
    
    protobuf_get(self, PACKET_INDEX, &index);
    
    if (index == 0) {
        uint32_t length;
        char *s;
        int idx;
        protobuf_get(self, PACKET_LENGTH, &length);
        if (length == 0) {
            protobuf_get(self, PACKET_STR, &s);
        } else {
            protobuf_get(self, PACKET_BUF, &s, NULL);
        }
        get_index_by_name(s, &idx);
        
        index = (uint16_t) idx;
        protobuf_set(self, PACKET_INDEX, &index);
        if ((detector = get_detector_by_name(s)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    } else {
        if ((detector = get_detector_by_index((int) index)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    }
    
    if ((ret = __detector_get_gain(detector, &gain)) != AAOS_OK) {
        return ret;
    }
    
    protobuf_set(self, PACKET_DF0, gain);
    protobuf_set(self, PACKET_LENGTH, 0);
    
    return ret;
}

static int
Detector_execute_set_readout_rate(struct Detector *self)
{
    uint16_t index;
    void *detector;
    double readout_rate;
    
    protobuf_get(self, PACKET_INDEX, &index);
    
    if (index == 0) {
        uint32_t length;
        char *s;
        int idx;
        protobuf_get(self, PACKET_LENGTH, &length);
        if (length == 0) {
            protobuf_get(self, PACKET_STR, &s);
        } else {
            protobuf_get(self, PACKET_BUF, &s, NULL);
        }
        get_index_by_name(s, &idx);
        
        index = (uint16_t) idx;
        protobuf_set(self, PACKET_INDEX, &index);
        if ((detector = get_detector_by_name(s)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    } else {
        if ((detector = get_detector_by_index((int) index)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    }
    protobuf_get(self, PACKET_DF0, &readout_rate);
    protobuf_set(self, PACKET_LENGTH, 0);
    
    return __detector_set_readout_rate(detector, readout_rate);
}

static int
Detector_execute_get_readout_rate(struct Detector *self)
{
    uint16_t index;
    void *detector;
    double readout_rate;
    int ret = AAOS_OK;
    
    protobuf_get(self, PACKET_INDEX, &index);
    
    if (index == 0) {
        uint32_t length;
        char *s;
        int idx;
        protobuf_get(self, PACKET_LENGTH, &length);
        if (length == 0) {
            protobuf_get(self, PACKET_STR, &s);
        } else {
            protobuf_get(self, PACKET_BUF, &s, NULL);
        }
        get_index_by_name(s, &idx);
        
        index = (uint16_t) idx;
        protobuf_set(self, PACKET_INDEX, &index);
        if ((detector = get_detector_by_name(s)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    } else {
        if ((detector = get_detector_by_index((int) index)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    }
    
    if ((ret = __detector_get_readout_rate(detector, &readout_rate)) != AAOS_OK) {
        return ret;
    }
    
    protobuf_set(self, PACKET_DF0, readout_rate);
    protobuf_set(self, PACKET_LENGTH, 0);
    
    return ret;
}

static int
Detector_execute_set_region(struct Detector *self)
{
    uint16_t index;
    void *detector;
    uint32_t x_offset, y_offset, width, height;
    
    protobuf_get(self, PACKET_INDEX, &index);
    
    if (index == 0) {
        uint32_t length;
        char *s;
        int idx;
        protobuf_get(self, PACKET_LENGTH, &length);
        if (length == 0) {
            protobuf_get(self, PACKET_STR, &s);
        } else {
            protobuf_get(self, PACKET_BUF, &s, NULL);
        }
        get_index_by_name(s, &idx);
        
        index = (uint16_t) idx;
        protobuf_set(self, PACKET_INDEX, &index);
        if ((detector = get_detector_by_name(s)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    } else {
        if ((detector = get_detector_by_index((int) index)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    }
    protobuf_get(self, PACKET_U32F0, &x_offset);
    protobuf_get(self, PACKET_U32F1, &y_offset);
    protobuf_get(self, PACKET_U32F2, &width);
    protobuf_get(self, PACKET_U32F3, &height);
    protobuf_set(self, PACKET_LENGTH, 0);
    
    return __detector_set_region(detector, x_offset, y_offset, width, height);
}

static int
Detector_execute_get_region(struct Detector *self)
{
    uint16_t index;
    void *detector;
    uint32_t x_offset, y_offset, width, height;
    int ret = AAOS_OK;
    
    protobuf_get(self, PACKET_INDEX, &index);
    
    if (index == 0) {
        uint32_t length;
        char *s;
        int idx;
        protobuf_get(self, PACKET_LENGTH, &length);
        if (length == 0) {
            protobuf_get(self, PACKET_STR, &s);
        } else {
            protobuf_get(self, PACKET_BUF, &s, NULL);
        }
        get_index_by_name(s, &idx);
        
        index = (uint16_t) idx;
        protobuf_set(self, PACKET_INDEX, &index);
        if ((detector = get_detector_by_name(s)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    } else {
        if ((detector = get_detector_by_index((int) index)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    }
    
    if ((ret = __detector_get_region(detector, &x_offset, &y_offset, &width, &height)) != AAOS_OK) {
        return ret;
    }
    
    protobuf_set(self, PACKET_U32F0, x_offset);
    protobuf_set(self, PACKET_U32F1, y_offset);
    protobuf_set(self, PACKET_U32F2, width);
    protobuf_set(self, PACKET_U32F3, height);
    protobuf_set(self, PACKET_LENGTH, 0);
    
    return ret;
}

static int
Detector_execute_set_temperature(struct Detector *self)
{
    uint16_t index;
    void *detector;
    double temperature;
    
    protobuf_get(self, PACKET_INDEX, &index);
    
    if (index == 0) {
        uint32_t length;
        char *s;
        int idx;
        protobuf_get(self, PACKET_LENGTH, &length);
        if (length == 0) {
            protobuf_get(self, PACKET_STR, &s);
        } else {
            protobuf_get(self, PACKET_BUF, &s, NULL);
        }
        get_index_by_name(s, &idx);
        
        index = (uint16_t) idx;
        protobuf_set(self, PACKET_INDEX, &index);
        if ((detector = get_detector_by_name(s)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    } else {
        if ((detector = get_detector_by_index((int) index)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    }
    protobuf_get(self, PACKET_DF0, &temperature);
    protobuf_set(self, PACKET_LENGTH, 0);
    
    return __detector_set_temperature(detector, temperature);
}

static int
Detector_execute_get_temperature(struct Detector *self)
{
    uint16_t index;
    void *detector;
    double temperature;
    int ret = AAOS_OK;
    
    protobuf_get(self, PACKET_INDEX, &index);
    
    if (index == 0) {
        uint32_t length;
        char *s;
        int idx;
        protobuf_get(self, PACKET_LENGTH, &length);
        if (length == 0) {
            protobuf_get(self, PACKET_STR, &s);
        } else {
            protobuf_get(self, PACKET_BUF, &s, NULL);
        }
        get_index_by_name(s, &idx);
        
        index = (uint16_t) idx;
        protobuf_set(self, PACKET_INDEX, &index);
        if ((detector = get_detector_by_name(s)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    } else {
        if ((detector = get_detector_by_index((int) index)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    }
    
    if ((ret = __detector_get_temperature(detector, &temperature)) != AAOS_OK) {
        return ret;
    }
    
    protobuf_set(self, PACKET_DF0, temperature);
    protobuf_set(self, PACKET_LENGTH, 0);
    
    return ret;
}

static int
Detector_execute_set_prefix(struct Detector *self)
{
    uint16_t index;
    uint32_t length;
    void *detector;
    char *prefix;
    
    protobuf_get(self, PACKET_INDEX, &index);
    protobuf_get(self, PACKET_LENGTH, &length);
    
    if (index == 0) {
        char *s;
        int idx;
        if (length == 0) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ERROR);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ERROR;
        } else {
            protobuf_get(self, PACKET_STR, &s);
            protobuf_get(self, PACKET_BUF, &prefix, NULL);
        }
        get_index_by_name(s, &idx);
        index = (uint16_t) idx;
        protobuf_set(self, PACKET_INDEX, &index);
        if ((detector = get_detector_by_name(s)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    } else {
        if ((detector = get_detector_by_index((int) index)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
        if (length == 0) {
            protobuf_get(self, PACKET_STR, &prefix);
        } else {
            protobuf_get(self, PACKET_BUF, &prefix, NULL);
        }
    }
    
    __detector_set_prefix(detector, prefix);
    protobuf_set(self, PACKET_LENGTH, 0);
    
    return AAOS_OK;
}

static int
Detector_execute_get_prefix(struct Detector *self)
{
    uint16_t index;
    uint32_t length;
    void *detector;
    char *prefix;
    size_t payload;
    
    protobuf_get(self, PACKET_INDEX, &index);
    protobuf_get(self, PACKET_LENGTH, &length);
    payload = protobuf_payload(self);
    
    if (index == 0) {
        char *s;
        int idx;
        if (length == 0) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ERROR);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ERROR;
        } else {
            protobuf_get(self, PACKET_STR, &s);
            protobuf_get(self, PACKET_BUF, &prefix, NULL);
        }
        get_index_by_name(s, &idx);
        index = (uint16_t) idx;
        protobuf_set(self, PACKET_INDEX, &index);
        if ((detector = get_detector_by_name(s)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    } else {
        if ((detector = get_detector_by_index((int) index)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
        if (length == 0) {
            protobuf_get(self, PACKET_STR, &prefix);
        } else {
            protobuf_get(self, PACKET_BUF, &prefix, NULL);
        }
    }
    
    __detector_get_prefix(detector, prefix, payload);
    protobuf_set(self, PACKET_LENGTH, strlen(prefix) + 1);
    
    return AAOS_OK;
}

static int
Detector_execute_set_directory(struct Detector *self)
{
    uint16_t index;
    uint32_t length;
    void *detector;
    char *directory;
    
    protobuf_get(self, PACKET_INDEX, &index);
    protobuf_get(self, PACKET_LENGTH, &length);
    
    if (index == 0) {
        char *s;
        int idx;
        if (length == 0) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ERROR);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ERROR;
        } else {
            protobuf_get(self, PACKET_STR, &s);
            protobuf_get(self, PACKET_BUF, &directory, NULL);
        }
        get_index_by_name(s, &idx);
        index = (uint16_t) idx;
        protobuf_set(self, PACKET_INDEX, &index);
        if ((detector = get_detector_by_name(s)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    } else {
        if ((detector = get_detector_by_index((int) index)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
        if (length == 0) {
            protobuf_get(self, PACKET_STR, &directory);
        } else {
            protobuf_get(self, PACKET_BUF, &directory, NULL);
        }
    }

    __detector_set_directory(detector, directory);
    protobuf_set(self, PACKET_LENGTH, 0);
    
    return AAOS_OK;
}

static int
Detector_execute_get_directory(struct Detector *self)
{
    uint16_t index;
    uint32_t length;
    void *detector;
    char *directory;
    size_t payload;
    
    protobuf_get(self, PACKET_INDEX, &index);
    protobuf_get(self, PACKET_LENGTH, &length);
    payload = protobuf_payload(self);
    
    if (index == 0) {
        char *s;
        int idx;
        if (length == 0) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ERROR);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ERROR;
        } else {
            protobuf_get(self, PACKET_STR, &s);
            protobuf_get(self, PACKET_BUF, &directory, NULL);
        }
        get_index_by_name(s, &idx);
        index = (uint16_t) idx;
        protobuf_set(self, PACKET_INDEX, &index);
        if ((detector = get_detector_by_name(s)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    } else {
        if ((detector = get_detector_by_index((int) index)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
        if (length == 0) {
            protobuf_get(self, PACKET_STR, &directory);
        } else {
            protobuf_get(self, PACKET_BUF, &directory, NULL);
        }
    }

    __detector_get_directory(detector, directory, payload);
    protobuf_set(self, PACKET_LENGTH, strlen(directory) + 1);
    
    return AAOS_OK;
}

static int
Detector_execute_set_template(struct Detector *self)
{
    uint16_t index;
    uint32_t length;
    void *detector;
    char *template;
    
    protobuf_get(self, PACKET_INDEX, &index);
    protobuf_get(self, PACKET_LENGTH, &length);
    
    if (index == 0) {
        char *s;
        int idx;
        if (length == 0) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ERROR);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ERROR;
        } else {
            protobuf_get(self, PACKET_STR, &s);
            protobuf_get(self, PACKET_BUF, &template, NULL);
        }
        get_index_by_name(s, &idx);
        index = (uint16_t) idx;
        protobuf_set(self, PACKET_INDEX, &index);
        if ((detector = get_detector_by_name(s)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    } else {
        if ((detector = get_detector_by_index((int) index)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
        if (length == 0) {
            protobuf_get(self, PACKET_STR, &template);
        } else {
            protobuf_get(self, PACKET_BUF, &template, NULL);
        }
    }

    __detector_set_template(detector, template);
    protobuf_set(self, PACKET_LENGTH, 0);
    
    return AAOS_OK;
}

static int
Detector_execute_get_template(struct Detector *self)
{
    uint16_t index;
    uint32_t length;
    void *detector;
    char *template;
    size_t payload;
    
    protobuf_get(self, PACKET_INDEX, &index);
    protobuf_get(self, PACKET_LENGTH, &length);
    payload = protobuf_payload(self);
    
    if (index == 0) {
        char *s;
        int idx;
        if (length == 0) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ERROR);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ERROR;
        } else {
            protobuf_get(self, PACKET_STR, &s);
            protobuf_get(self, PACKET_BUF, &template, NULL);
        }
        get_index_by_name(s, &idx);
        index = (uint16_t) idx;
        protobuf_set(self, PACKET_INDEX, &index);
        if ((detector = get_detector_by_name(s)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    } else {
        if ((detector = get_detector_by_index((int) index)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
        if (length == 0) {
            protobuf_get(self, PACKET_STR, &template);
        } else {
            protobuf_get(self, PACKET_BUF, &template, NULL);
        }
    }

    __detector_get_template(detector, template, payload);
    protobuf_set(self, PACKET_LENGTH, strlen(template) + 1);
    
    return AAOS_OK;
}

static int
Detector_execute_set_option(struct Detector *self)
{
    uint16_t index;
    void *detector;
    uint16_t option;
    
    protobuf_get(self, PACKET_INDEX, &index);
    
    if (index == 0) {
        uint32_t length;
        char *s;
        int idx;
        protobuf_get(self, PACKET_LENGTH, &length);
        if (length == 0) {
            protobuf_get(self, PACKET_STR, &s);
        } else {
            protobuf_get(self, PACKET_BUF, &s, NULL);
        }
        get_index_by_name(s, &idx);
        
        index = (uint16_t) idx;
        protobuf_set(self, PACKET_INDEX, &index);
        if ((detector = get_detector_by_name(s)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    } else {
        if ((detector = get_detector_by_index((int) index)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    }
    
    protobuf_get(self, PACKET_OPTION, &option);
    __detector_set_option(detector, option);
    protobuf_get(self, PACKET_OPTION, &option);
    protobuf_set(self, PACKET_LENGTH, 0);

    return AAOS_OK;
}

static int
Detector_execute_get_option(struct Detector *self)
{
    uint16_t index;
    void *detector;
    uint16_t option;
    
    protobuf_get(self, PACKET_INDEX, &index);
    
    if (index == 0) {
        uint32_t length;
        char *s;
        int idx;
        protobuf_get(self, PACKET_LENGTH, &length);
        if (length == 0) {
            protobuf_get(self, PACKET_STR, &s);
        } else {
            protobuf_get(self, PACKET_BUF, &s, NULL);
        }
        get_index_by_name(s, &idx);
        
        index = (uint16_t) idx;
        protobuf_set(self, PACKET_INDEX, &index);
        if ((detector = get_detector_by_name(s)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    } else {
        if ((detector = get_detector_by_index((int) index)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    }
    
    option = __detector_get_option(detector);
    
    protobuf_set(self, PACKET_OPTION, option);
    protobuf_set(self, PACKET_LENGTH, 0);
    
    return AAOS_OK;
}

static int
Detector_execute_clear_option(struct Detector *self)
{
    uint16_t index;
    void *detector;

    protobuf_get(self, PACKET_INDEX, &index);
    
    if (index == 0) {
        uint32_t length;
        char *s;
        int idx;
        protobuf_get(self, PACKET_LENGTH, &length);
        if (length == 0) {
            protobuf_get(self, PACKET_STR, &s);
        } else {
            protobuf_get(self, PACKET_BUF, &s, NULL);
        }
        get_index_by_name(s, &idx);
        
        index = (uint16_t) idx;
        protobuf_set(self, PACKET_INDEX, &index);
        if ((detector = get_detector_by_name(s)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    } else {
        if ((detector = get_detector_by_index((int) index)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    }
    
    __detector_clear_option(detector);
    protobuf_set(self, PACKET_LENGTH, 0);
    
    return AAOS_OK;
}

static int
Detector_execute_delete_image(struct Detector *self)
{
    uint16_t index;
    void *detector;
    char *buf, directory[FILENAMESIZE], filename[PATHSIZE];
    uint32_t length;

    protobuf_get(self, PACKET_INDEX, &index);
    protobuf_get(self, PACKET_LENGTH, &length);
    
    if (index == 0) {
        char *s;
        int idx;
        if (length == 0) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ERROR);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_OK;
        } else {
            protobuf_get(self, PACKET_STR, &s);
            protobuf_get(self, PACKET_BUF, &buf);
        }
        get_index_by_name(s, &idx);
        
        index = (uint16_t) idx;
        protobuf_set(self, PACKET_INDEX, &index);
        if ((detector = get_detector_by_name(s)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    } else {
        if ((detector = get_detector_by_index((int) index)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
        if (length == 0) {
            protobuf_get(self, PACKET_STR, &buf);
        } else {
            protobuf_get(self, PACKET_BUF, &buf);
        }
        
    }
    
    __detector_get_directory(detector, directory, FILENAMESIZE);
    
    snprintf(filename, PATHSIZE, "%s/%s", directory, buf);
    
    Unlink(filename);
 
    protobuf_set(self, PACKET_LENGTH, 0);
    
    return AAOS_OK;
}

static int
Detector_execute_delete_all_image(struct Detector *self)
{
    uint16_t index;
    void *detector;
    char directory[FILENAMESIZE], prefix[FILENAMESIZE], template[FILENAMESIZE], filename[PATHSIZE];
    DIR *dirp;
    struct dirent *dp;

    protobuf_get(self, PACKET_INDEX, &index);
    
    if (index == 0) {
        uint32_t length;
        char *s;
        int idx;
        protobuf_get(self, PACKET_LENGTH, &length);
        if (length == 0) {
            protobuf_get(self, PACKET_STR, &s);
        } else {
            protobuf_get(self, PACKET_BUF, &s, NULL);
        }
        get_index_by_name(s, &idx);
        
        index = (uint16_t) idx;
        protobuf_set(self, PACKET_INDEX, &index);
        if ((detector = get_detector_by_name(s)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    } else {
        if ((detector = get_detector_by_index((int) index)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    }
    
    if ((dirp = Opendir(directory)) == NULL) {
        switch (errno) {
            case EEXIST:
                protobuf_set(self, PACKET_ERRORCODE, AAOS_EEXIST);
                protobuf_set(self, PACKET_LENGTH, 0);
                break;
            case ENOTDIR:
                protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTDIR);
                protobuf_set(self, PACKET_LENGTH, 0);
                break;
            case ENOMEM:
                protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOMEM);
                protobuf_set(self, PACKET_LENGTH, 0);
                break;
            case ENOENT:
                protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOENT);
                protobuf_set(self, PACKET_LENGTH, 0);
                break;
            case EACCES:
                protobuf_set(self, PACKET_ERRORCODE, AAOS_EACCES);
                protobuf_set(self, PACKET_LENGTH, 0);
                break;
            default:
                break;
        }
    }
    
    __detector_get_directory(detector, directory, FILENAMESIZE);
    __detector_get_prefix(detector, prefix, FILENAMESIZE);
    __detector_get_template(detector, template, FILENAMESIZE);
    
    while ((dp = Readdir(dirp)) != NULL) {
        if (strncmp(dp->d_name, prefix, strlen(prefix)) == 0 && (memcmp(dp->d_name + strlen(dp->d_name) - 5, ".fits", 5) == 0 || memcmp(dp->d_name + strlen(dp->d_name) - 8, ".fits.fz", 8) == 0)) {
            if (strcmp(dp->d_name, template) == 0) {
                continue;
            }
            switch (dp->d_type) {
                case DT_REG:
                    snprintf(filename, PATHSIZE, "%s/%s", directory, dp->d_name);
                    Unlink(filename);
                    break;
                case DT_UNKNOWN:
                {
                    struct stat sb;
                    Stat(dp->d_name, &sb);
                    if (S_ISREG(sb.st_mode)) {
                        snprintf(filename, PATHSIZE, "%s/%s", directory, dp->d_name);
                        Unlink(filename);
                    }
                }
                    break;
                default:
                    break;
            }
        }
    }
    
    protobuf_set(self, PACKET_LENGTH, 0);
    
    return AAOS_OK;
}

static int
Detector_execute_list_image(struct Detector *self)
{
    uint16_t index;
    void *detector;
    char *buf, directory[FILENAMESIZE], prefix[FILENAMESIZE], template[FILENAMESIZE];
    size_t payload;
    FILE *fp;
    DIR *dirp;
    struct dirent *dp;

    protobuf_get(self, PACKET_INDEX, &index);
    protobuf_get(self, PACKET_BUF, &buf, NULL);
    payload = protobuf_payload(self);
    
    if (index == 0) {
        uint32_t length;
        char *s;
        int idx;
        protobuf_get(self, PACKET_LENGTH, &length);
        if (length == 0) {
            protobuf_get(self, PACKET_STR, &s);
        } else {
            protobuf_get(self, PACKET_BUF, &s, NULL);
        }
        get_index_by_name(s, &idx);
        
        index = (uint16_t) idx;
        protobuf_set(self, PACKET_INDEX, &index);
        if ((detector = get_detector_by_name(s)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    } else {
        if ((detector = get_detector_by_index((int) index)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    }
    
    if ((dirp = Opendir(directory)) == NULL) {
        switch (errno) {
            case EEXIST:
                protobuf_set(self, PACKET_ERRORCODE, AAOS_EEXIST);
                protobuf_set(self, PACKET_LENGTH, 0);
                break;
            case ENOTDIR:
                protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTDIR);
                protobuf_set(self, PACKET_LENGTH, 0);
                break;
            case ENOMEM:
                protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOMEM);
                protobuf_set(self, PACKET_LENGTH, 0);
                break;
            case ENOENT:
                protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOENT);
                protobuf_set(self, PACKET_LENGTH, 0);
                break;
            case EACCES:
                protobuf_set(self, PACKET_ERRORCODE, AAOS_EACCES);
                protobuf_set(self, PACKET_LENGTH, 0);
                break;
            default:
                break;
        }
    }
    
    if ((fp = fmemopen(buf, payload, "w+")) == NULL) {
        protobuf_set(self, PACKET_ERRORCODE, AAOS_ERROR);
        protobuf_set(self, PACKET_LENGTH, 0);
        return AAOS_ERROR;
    }
    
    __detector_get_directory(detector, directory, FILENAMESIZE);
    __detector_get_prefix(detector, prefix, FILENAMESIZE);
    __detector_get_template(detector, template, FILENAMESIZE);
    
    while ((dp = Readdir(dirp)) != NULL) {
        if (strncmp(dp->d_name, prefix, strlen(prefix)) == 0 && (memcmp(dp->d_name + strlen(dp->d_name) - 5, ".fits", 5) == 0 || memcmp(dp->d_name + strlen(dp->d_name) - 8, ".fits.fz", 8) == 0)) {
            if (strcmp(dp->d_name, template) == 0) {
                continue;
            }
            switch (dp->d_type) {
                case DT_REG:
                    fprintf(fp, "%s\n", dp->d_name);
                    break;
                case DT_UNKNOWN:
                {
                    struct stat sb;
                    Stat(dp->d_name, &sb);
                    if (S_ISREG(sb.st_mode)) {
                        fprintf(fp, "%s\n", dp->d_name);
                    }
                }
                    break;
                default:
                    break;
            }
        }
    }
    
    fclose(fp);
    
    protobuf_set(self, PACKET_LENGTH, strlen(buf) + 1);
    
    return AAOS_OK;
}

static int
Detector_execute_get_image(struct Detector *self)
{
    uint16_t index;
    void *detector;
    char *buf, directory[FILENAMESIZE], filename[PATHSIZE];
    uint32_t length;
    int fd, sockfd, ret;
    struct stat sb;

    protobuf_get(self, PACKET_INDEX, &index);
    protobuf_get(self, PACKET_LENGTH, &length);
    
    if (index == 0) {
        /*
         * If index is 0, PACKET_STR must be filled with the name of the detector,
         * and PACKET_BUF should not be empty but filled with the filename to be get.
         */
        char *s;
        int idx;
        if (length == 0) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ERROR);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_OK;
        } else {
            protobuf_get(self, PACKET_STR, &s);
            protobuf_get(self, PACKET_BUF, &buf);
        }
        get_index_by_name(s, &idx);
        
        index = (uint16_t) idx;
        protobuf_set(self, PACKET_INDEX, &index);
        if ((detector = get_detector_by_name(s)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    } else {
        if ((detector = get_detector_by_index((int) index)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
        if (length == 0) {
            protobuf_get(self, PACKET_STR, &buf);
        } else {
            protobuf_get(self, PACKET_BUF, &buf, NULL);
        }
    }
    
    __detector_get_directory(detector, directory, FILENAMESIZE);
    
    snprintf(filename, PATHSIZE, "%s/%s", directory, buf);
    
    if ((ret = Stat(filename, &sb)) < 0) {
        switch (errno) {
            case EACCES:
                return AAOS_EACCES;
                break;
            case ENOENT:
                return AAOS_ENOENT;
            case ENOTDIR:
                return AAOS_ENOTDIR;
            default:
                return AAOS_ERROR;
                break;
        }
        if (!S_ISREG(sb.st_mode)) {
            return AAOS_EBADF;
        }
    }
    
    if ((fd = Open(filename, O_RDONLY)) < 0) {
        return AAOS_ERROR;
    }
    
    protobuf_set(self, PACKET_U64F0, (uint64_t) sb.st_size);
    protobuf_set(self, PACKET_LENGTH, 0);
    
    if ((ret = rpc_call(self)) != AAOS_OK) {
        return ret;
    }
    
    sockfd = tcp_socket_get_sockfd(self);
    
    if (Sendfile(fd, sockfd, 0, &sb.st_size, NULL, 0) < 0) {
        Close(fd);
        return AAOS_ERROR;
    }
    Close(fd);
    protobuf_set(self, PACKET_LENGTH, 0);
    
    return rpc_call(self);
}

static int
Detector_execute_enable_cooling(struct Detector *self)
{
    uint16_t index;
    void *detector;
    
    protobuf_get(self, PACKET_INDEX, &index);
    
    if (index == 0) {
        uint32_t length;
        char *s;
        int idx;
        protobuf_get(self, PACKET_LENGTH, &length);
        if (length == 0) {
            protobuf_get(self, PACKET_STR, &s);
        } else {
            protobuf_get(self, PACKET_BUF, &s, NULL);
        }
        get_index_by_name(s, &idx);
        
        index = (uint16_t) idx;
        protobuf_set(self, PACKET_INDEX, &index);
        if ((detector = get_detector_by_name(s)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    } else {
        if ((detector = get_detector_by_index((int) index)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    }
    protobuf_set(self, PACKET_LENGTH, 0);
    
    return __detector_enable_cooling(detector);
}

static int
Detector_execute_disable_cooling(struct Detector *self)
{
    uint16_t index;
    void *detector;
    
    protobuf_get(self, PACKET_INDEX, &index);
    
    if (index == 0) {
        uint32_t length;
        char *s;
        int idx;
        protobuf_get(self, PACKET_LENGTH, &length);
        if (length == 0) {
            protobuf_get(self, PACKET_STR, &s);
        } else {
            protobuf_get(self, PACKET_BUF, &s, NULL);
        }
        get_index_by_name(s, &idx);
        
        index = (uint16_t) idx;
        protobuf_set(self, PACKET_INDEX, &index);
        if ((detector = get_detector_by_name(s)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    } else {
        if ((detector = get_detector_by_index((int) index)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    }
    protobuf_set(self, PACKET_LENGTH, 0);
    
    return __detector_disable_cooling(detector);
}

/*
 * if detector command is illegal
 */
static int
Detector_execute_default(struct Detector *self)
{
    return AAOS_EBADCMD;
}

static int
Detector_execute_get_index_by_name(struct Detector *self)
{
   
    char *name;
    int index, ret;
    uint16_t idx;
    uint32_t length;

    protobuf_get(self, PACKET_LENGTH, &length);
    /*
     * if lenght == 0, use str field.
     */
    if (length == 0) {
        protobuf_get(self, PACKET_STR, &name);
    } else {
        protobuf_get(self, PACKET_BUF, &name, NULL);
    }
    
    if ((ret = get_index_by_name(name, &index)) != AAOS_OK) {
        return ret;
    } else {
        idx = (uint16_t) index;
        protobuf_set(self, PACKET_INDEX, idx);
        protobuf_set(self, PACKET_LENGTH, 0);
    }

    return AAOS_OK;
}

static int
Detector_execute(void *_self)
{
    struct Detector *self = cast(Detector(), _self);
    uint16_t command;
    
    if (Detector_protocol_check(self) != AAOS_OK) {
        return AAOS_EPROTOWRONG;
    }
    protobuf_get(self, PACKET_COMMAND, &command);
    switch (command) {
        case DETECTOR_COMMAND_GET_INDEX_BY_NAME:
            return Detector_execute_get_index_by_name(self);
            break;
        case DETECTOR_COMMAND_INFO:
            return Detector_execute_info(self);
            break;
        case DETECTOR_COMMAND_RAW:
            return Detector_execute_raw(self);
            break;
        case DETECTOR_COMMAND_UNLOAD:
            return Detector_execute_unload(self);
            break;
        case DETECTOR_COMMAND_LOAD:
            return Detector_execute_load(self);
            break;
        case DETECTOR_COMMAND_RELOAD:
            return Detector_execute_reload(self);
            break;
        case DETECTOR_COMMAND_EXPOSE:
            return Detector_execute_expose(self);
            break;
        case DETECTOR_COMMAND_WAIT_FOR_COMPLETION:
            return Detector_execute_wait_for_completion(self);
            break;
        case DETECTOR_COMMAND_SET_BINNING:
            return Detector_execute_set_binning(self);
            break;
        case DETECTOR_COMMAND_GET_BINNING:
            return Detector_execute_get_binning(self);
            break;
        case DETECTOR_COMMAND_SET_EXPOSURE_TIME:
            return Detector_execute_set_exposure_time(self);
            break;
        case DETECTOR_COMMAND_GET_EXPOSURE_TIME:
            return Detector_execute_get_exposure_time(self);
            break;
        case DETECTOR_COMMAND_SET_FRAME_RATE:
            return Detector_execute_set_frame_rate(self);
            break;
        case DETECTOR_COMMAND_GET_FRAME_RATE:
            return Detector_execute_get_frame_rate(self);
            break;
        case DETECTOR_COMMAND_SET_GAIN:
            return Detector_execute_set_gain(self);
            break;
        case DETECTOR_COMMAND_GET_GAIN:
            return Detector_execute_get_gain(self);
            break;
        case DETECTOR_COMMAND_SET_READOUT_RATE:
            return Detector_execute_set_readout_rate(self);
            break;
        case DETECTOR_COMMAND_GET_READOUT_RATE:
            return Detector_execute_get_readout_rate(self);
            break;
        case DETECTOR_COMMAND_SET_REGION:
            return Detector_execute_set_region(self);
            break;
        case DETECTOR_COMMAND_GET_REGION:
            return Detector_execute_get_region(self);
            break;
        case DETECTOR_COMMAND_SET_TEMPERATURE:
            return Detector_execute_set_temperature(self);
            break;
        case DETECTOR_COMMAND_GET_TEMPERATURE:
            return Detector_execute_get_temperature(self);
            break;
        case DETECTOR_COMMAND_SET_PREFIX:
            return Detector_execute_set_prefix(self);
            break;
        case DETECTOR_COMMAND_GET_PREFIX:
            return Detector_execute_get_prefix(self);
            break;
        case DETECTOR_COMMAND_SET_DIRECTORY:
            return Detector_execute_set_directory(self);
            break;
        case DETECTOR_COMMAND_GET_DIRECTORY:
            return Detector_execute_get_directory(self);
            break;
        case DETECTOR_COMMAND_SET_TEMPLATE:
            return Detector_execute_set_template(self);
            break;
        case DETECTOR_COMMAND_GET_TEMPLATE:
            return Detector_execute_get_template(self);
            break;
        case DETECTOR_COMMAND_SET_OPTION:
            return Detector_execute_set_option(self);
            break;
        case DETECTOR_COMMAND_GET_OPTION:
            return Detector_execute_get_option(self);
            break;
        case DETECTOR_COMMAND_CLEAR_OPTION:
            return Detector_execute_clear_option(self);
            break;
        case DETECTOR_COMMAND_ABORT:
            return Detector_execute_abort(self);
            break;
        case DETECTOR_COMMAND_STOP:
            return Detector_execute_stop(self);
            break;
        case DETECTOR_COMMAND_GET_IMAGE:
            return Detector_execute_get_image(self);
            break;
        case DETECTOR_COMMAND_DELETE_IMAGE:
            return Detector_execute_delete_image(self);
            break;
        case DETECTOR_COMMAND_DELETE_ALL_IMAGE:
            return Detector_execute_delete_all_image(self);
            break;
        case DETECTOR_COMMAND_LIST_IMAGE:
            return Detector_execute_list_image(self);
            break;
        case DETECTOR_COMMAND_COOLING_ENABLE:
            return Detector_execute_enable_cooling(self);
            break;
        case DETECTOR_COMMAND_COOLING_DISABLE:
            return Detector_execute_disable_cooling(self);
            break;
        case SYSTEM_COMMAND_INSPECT:
            return Detector_execute_inspect(self);
            break;
        case SYSTEM_COMMAND_REGISTER:
            return Detector_execute_register(self);
            break;
        default:
            return Detector_execute_default(self);
            break;
    }
}

static void *
Detector_ctor(void *_self, va_list *app)
{
    struct Detector *self = super_ctor(Detector(), _self, app);
    
    self->_._vtab = detector_virtual_table();
    
    return (void *) self;
}

static void *
Detector_dtor(void *_self)
{
    //struct RPC *self = cast(RPC(), _self);
    
    return super_dtor(Detector(), _self);
}

static void *
DetectorClass_ctor(void *_self, va_list *app)
{
    struct DetectorClass *self = super_ctor(DetectorClass(), _self, app);
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
        
        if (selector == (Method) detector_get_index_by_name) {
            if (tag) {
                self->get_index_by_name.tag = tag;
                self->get_index_by_name.selector = selector;
            }
            self->get_index_by_name.method = method;
            continue;
        }
        if (selector == (Method) detector_raw) {
            if (tag) {
                self->raw.tag = tag;
                self->raw.selector = selector;
            }
            self->raw.method = method;
            continue;
        }
        if (selector == (Method) detector_expose) {
            if (tag) {
                self->expose.tag = tag;
                self->expose.selector = selector;
            }
            self->expose.method = method;
            continue;
        }
        if (selector == (Method) detector_status) {
            if (tag) {
                self->status.tag = tag;
                self->status.selector = selector;
            }
            self->status.method = method;
            continue;
        }
        if (selector == (Method) detector_info) {
            if (tag) {
                self->info.tag = tag;
                self->info.selector = selector;
            }
            self->info.method = method;
            continue;
        }
        if (selector == (Method) detector_set_binning) {
            if (tag) {
                self->set_binning.tag = tag;
                self->set_binning.selector = selector;
            }
            self->set_binning.method = method;
            continue;
        }
        if (selector == (Method) detector_get_binning) {
            if (tag) {
                self->get_binning.tag = tag;
                self->get_binning.selector = selector;
            }
            self->get_binning.method = method;
            continue;
        }
        if (selector == (Method) detector_set_exposure_time) {
            if (tag) {
                self->set_exposure_time.tag = tag;
                self->set_exposure_time.selector = selector;
            }
            self->set_exposure_time.method = method;
            continue;
        }
        if (selector == (Method) detector_get_exposure_time) {
            if (tag) {
                self->get_exposure_time.tag = tag;
                self->get_exposure_time.selector = selector;
            }
            self->get_exposure_time.method = method;
            continue;
        }
        if (selector == (Method) detector_set_frame_rate) {
            if (tag) {
                self->set_frame_rate.tag = tag;
                self->set_frame_rate.selector = selector;
            }
            self->set_frame_rate.method = method;
            continue;
        }
        if (selector == (Method) detector_get_frame_rate) {
            if (tag) {
                self->get_frame_rate.tag = tag;
                self->get_frame_rate.selector = selector;
            }
            self->get_frame_rate.method = method;
            continue;
        }
        if (selector == (Method) detector_set_gain) {
            if (tag) {
                self->set_gain.tag = tag;
                self->set_gain.selector = selector;
            }
            self->set_gain.method = method;
            continue;
        }
        if (selector == (Method) detector_get_gain) {
            if (tag) {
                self->get_gain.tag = tag;
                self->get_gain.selector = selector;
            }
            self->get_gain.method = method;
            continue;
        }
        if (selector == (Method) detector_set_readout_rate) {
            if (tag) {
                self->set_readout_rate.tag = tag;
                self->set_readout_rate.selector = selector;
            }
            self->set_readout_rate.method = method;
            continue;
        }
        if (selector == (Method) detector_get_readout_rate) {
            if (tag) {
                self->get_readout_rate.tag = tag;
                self->get_readout_rate.selector = selector;
            }
            self->get_readout_rate.method = method;
            continue;
        }
        if (selector == (Method) detector_set_region) {
            if (tag) {
                self->set_region.tag = tag;
                self->set_region.selector = selector;
            }
            self->set_region.method = method;
            continue;
        }
        if (selector == (Method) detector_get_region) {
            if (tag) {
                self->get_region.tag = tag;
                self->get_region.selector = selector;
            }
            self->get_gain.method = method;
            continue;
        }
        if (selector == (Method) detector_set_temperature) {
            if (tag) {
                self->set_temperature.tag = tag;
                self->set_temperature.selector = selector;
            }
            self->set_temperature.method = method;
            continue;
        }
        if (selector == (Method) detector_get_temperature) {
            if (tag) {
                self->get_temperature.tag = tag;
                self->get_temperature.selector = selector;
            }
            self->get_temperature.method = method;
            continue;
        }
        if (selector == (Method) detector_set_template) {
            if (tag) {
                self->set_template.tag = tag;
                self->set_template.selector = selector;
            }
            self->set_template.method = method;
            continue;
        }
        if (selector == (Method) detector_get_template) {
            if (tag) {
                self->get_template.tag = tag;
                self->get_template.selector = selector;
            }
            self->get_template.method = method;
            continue;
        }
        if (selector == (Method) detector_get_directory) {
            if (tag) {
                self->get_directory.tag = tag;
                self->get_directory.selector = selector;
            }
            self->get_directory.method = method;
            continue;
        }
        if (selector == (Method) detector_set_directory) {
            if (tag) {
                self->set_directory.tag = tag;
                self->set_directory.selector = selector;
            }
            self->set_directory.method = method;
            continue;
        }
        if (selector == (Method) detector_get_prefix) {
            if (tag) {
                self->get_prefix.tag = tag;
                self->get_prefix.selector = selector;
            }
            self->get_prefix.method = method;
            continue;
        }
        if (selector == (Method) detector_set_prefix) {
            if (tag) {
                self->set_prefix.tag = tag;
                self->set_prefix.selector = selector;
            }
            self->set_prefix.method = method;
            continue;
        }
        if (selector == (Method) detector_get_template) {
            if (tag) {
                self->get_template.tag = tag;
                self->get_template.selector = selector;
            }
            self->get_template.method = method;
            continue;
        }
        if (selector == (Method) detector_set_template) {
            if (tag) {
                self->set_template.tag = tag;
                self->set_template.selector = selector;
            }
            self->set_template.method = method;
            continue;
        }
        if (selector == (Method) detector_clear_option) {
            if (tag) {
                self->clear_option.tag = tag;
                self->clear_option.selector = selector;
            }
            self->clear_option.method = method;
            continue;
        }
        if (selector == (Method) detector_get_option) {
            if (tag) {
                self->get_option.tag = tag;
                self->get_option.selector = selector;
            }
            self->get_option.method = method;
            continue;
        }
        if (selector == (Method) detector_set_option) {
            if (tag) {
                self->set_option.tag = tag;
                self->set_option.selector = selector;
            }
            self->set_option.method = method;
            continue;
        }
        if (selector == (Method) detector_abort) {
            if (tag) {
                self->abort.tag = tag;
                self->abort.selector = selector;
            }
            self->abort.method = method;
            continue;
        }
        if (selector == (Method) detector_stop) {
            if (tag) {
                self->stop.tag = tag;
                self->stop.selector = selector;
            }
            self->stop.method = method;
            continue;
        }
        if (selector == (Method) detector_list_image) {
            if (tag) {
                self->list_image.tag = tag;
                self->list_image.selector = selector;
            }
            self->list_image.method = method;
            continue;
        }
        if (selector == (Method) detector_delete_image) {
            if (tag) {
                self->delete_image.tag = tag;
                self->delete_image.selector = selector;
            }
            self->delete_image.method = method;
            continue;
        }
        if (selector == (Method) detector_get_image) {
            if (tag) {
                self->get_image.tag = tag;
                self->get_image.selector = selector;
            }
            self->get_image.method = method;
            continue;
        }
        if (selector == (Method) detector_delete_all_image) {
            if (tag) {
                self->delete_all_image.tag = tag;
                self->delete_all_image.selector = selector;
            }
            self->delete_all_image.method = method;
            continue;
        }
        if (selector == (Method) detector_enable_cooling) {
            if (tag) {
                self->enable_cooling.tag = tag;
                self->enable_cooling.selector = selector;
            }
            self->enable_cooling.method = method;
            continue;
        }
        if (selector == (Method) detector_disable_cooling) {
            if (tag) {
                self->disable_cooling.tag = tag;
                self->disable_cooling.selector = selector;
            }
            self->disable_cooling.method = method;
            continue;
        }
        
        /*
        if (selector == (Method) detector_inspect) {
            if (tag) {
                self->inspect.tag = tag;
                self->inspect.selector = selector;
            }
            self->inspect.method = method;
            continue;
        }
        if (selector == (Method) detector_register) {
            if (tag) {
                self->reg.tag = tag;
                self->reg.selector = selector;
            }
            self->reg.method = method;
            continue;
        }
         */
    }
    
#ifdef va_copy
    va_end(ap);
#endif
    
    self->_.execute.method = (Method) 0;
    
    return self;
}

static void *_DetectorClass;

static void
DetectorClass_destroy(void)
{
    free((void *) _DetectorClass);
}

static void
DetectorClass_initialize(void)
{
    _DetectorClass = new(RPCClass(), "DetectorClass", RPCClass(), sizeof(struct DetectorClass),
                        ctor, "ctor", DetectorClass_ctor,
                        (void *) 0);
    
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(DetectorClass_destroy);
#endif
}

const void *
DetectorClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, DetectorClass_initialize);
#endif
    
    return _DetectorClass;
}

static void *_Detector;

static void
Detector_destroy(void)
{
    free((void *) _Detector);
}

static void
Detector_initialize(void)
{
    _Detector = new(DetectorClass(), "Detector", RPC(), sizeof(struct Detector),
                    ctor, "ctor", Detector_ctor,
                    dtor, "dtor", Detector_dtor,
                    detector_raw, "raw", Detector_raw,
                    detector_abort, "abort", Detector_abort,
                    detector_stop, "stop", Detector_stop,
                    detector_info, "info", Detector_info,
                    detector_status, "status", Detector_status,
                    detector_get_index_by_name, "get_index_by_name", Detector_get_index_by_name,
                    detector_expose, "expose", Detector_expose,
                    detector_wait_for_completion, "wait_for_completion", Detector_wait_for_completion,
                    detector_set_binning, "set_binning", Detector_set_binning,
                    detector_get_binning, "get_binning", Detector_get_binning,
                    detector_set_exposure_time, "set_exposure_time", Detector_set_exposure_time,
                    detector_get_exposure_time, "get_exposure_time", Detector_get_exposure_time,
                    detector_set_frame_rate, "set_frame_rate", Detector_set_frame_rate,
                    detector_get_frame_rate, "get_frame_rate", Detector_get_frame_rate,
                    detector_set_gain, "set_gain", Detector_set_gain,
                    detector_get_gain, "get_gain", Detector_get_gain,
                    detector_set_readout_rate, "set_readout_rate", Detector_set_readout_rate,
                    detector_get_readout_rate, "get_readout_rate", Detector_get_readout_rate,
                    detector_set_temperature, "set_temperature", Detector_set_temperature,
                    detector_get_temperature, "get_temperature", Detector_get_temperature,
                    detector_set_region, "set_region", Detector_set_region,
                    detector_get_region, "get_region", Detector_get_region,
                    detector_clear_option, "clear_option", Detector_clear_option,
                    detector_set_option, "set_option", Detector_set_option,
                    detector_get_option, "get_option", Detector_get_option,
                    detector_set_directory, "set_directory", Detector_set_directory,
                    detector_get_directory, "get_directory", Detector_get_directory,
                    detector_set_prefix, "set_prefix", Detector_set_prefix,
                    detector_get_prefix, "get_prefix", Detector_get_prefix,
                    detector_set_template, "set_template", Detector_set_template,
                    detector_get_template, "get_template", Detector_get_template,
                    detector_list_image, "list_image", Detector_list_image,
                    detector_delete_image, "delete_image", Detector_delete_image,
                    detector_delete_all_image, "delete_all_image", Detector_delete_all_image,
                    detector_get_image, "get_image", Detector_get_image,
                    detector_enable_cooling, "enable_cooling", Detector_enable_cooling,
                    detector_disable_cooling, "enable_cooling", Detector_disable_cooling,
                    (void *) 0);
    
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(Detector_destroy);
#endif

}

const void *
Detector(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, Detector_initialize);
#endif
    
    return _Detector;
}

static const void *_detector_virtual_table;

static void
detector_virtual_table_destroy(void)
{
    delete((void *) _detector_virtual_table);
}

static void
detector_virtual_table_initialize(void)
{
    _detector_virtual_table = new(RPCVirtualTable(),
                                rpc_execute, "execute", Detector_execute,
                                (void *)0);
    
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(detector_virtual_table_destroy);
#endif
}

static const void *
detector_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, detector_virtual_table_initialize);
#endif
    
    return _detector_virtual_table;
}

/*
 * Detector client class
 */

static const void *detector_client_virtual_table(void);

static
int DetectorClient_connect(void *_self, void **client)
{
    struct DetectorClient *self = cast(DetectorClient(), _self);
    
    int cfd = Tcp_connect(self->_._.address, self->_._.port, NULL, NULL);
    
    if (cfd < 0) {
        *client = NULL;
        return AAOS_ERROR;
    } else {
        *client = new(Detector(), cfd);
    }
    
    protobuf_set(*client, PACKET_PROTOCOL, PROTO_DETECTOR);
    
    return AAOS_OK;
}

static void *
DetectorClient_ctor(void *_self, va_list *app)
{
    struct DetectorClient *self = super_ctor(DetectorClient(), _self, app);
    
    self->_._vtab = detector_client_virtual_table();
    
    return (void *) self;
}

static void *
DetectorClient_dtor(void *_self)
{
    //struct RPC *self = cast(RPC(), _self);
    
    return super_dtor(DetectorClient(), _self);
}

static void *
DetectorClientClass_ctor(void *_self, va_list *app)
{
    struct DetectorClientClass *self = super_ctor(DetectorClientClass(), _self, app);
    
    self->_.connect.method = (Method) 0;
    
    return self;
}

static void *_DetectorClientClass;

static void
DetectorClientClass_destroy(void)
{
    free((void *) _DetectorClientClass);
}

static void
DetectorClientClass_initialize(void)
{
    _DetectorClientClass = new(RPCClientClass(), "DetectorClientClass", RPCClientClass(), sizeof(struct DetectorClientClass),
                             ctor, "ctor", DetectorClientClass_ctor,
                             (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(DetectorClientClass_destroy);
#endif
}

const void *
DetectorClientClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, DetectorClientClass_initialize);
#endif
    
    return _DetectorClientClass;
}

static void *_DetectorClient;

static void
DetectorClient_destroy(void)
{
    free((void *) _DetectorClient);
}

static void
DetectorClient_initialize(void)
{
    _DetectorClient = new(DetectorClientClass(), "DetectorClient", RPCClient(), sizeof(struct DetectorClient),
                        ctor, "ctor", DetectorClient_ctor,
                        dtor, "dtor", DetectorClient_dtor,
                        (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(DetectorClient_destroy);
#endif
}

const void *
DetectorClient(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, DetectorClient_initialize);
#endif
    
    return _DetectorClient;
}

static const void *_detector_client_virtual_table;

static void
detector_client_virtual_table_destroy(void)
{
    delete((void *) _detector_client_virtual_table);
}

static void
detector_client_virtual_table_initialize(void)
{
    _detector_client_virtual_table = new(RPCClientVirtualTable(),
                                       rpc_client_connect, "connect", DetectorClient_connect,
                                       (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(detector_client_virtual_table_destroy);
#endif
}

static const void *
detector_client_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, detector_client_virtual_table_initialize);
#endif
    
    return _detector_client_virtual_table;
}

/*
 * Detector server class
 */

static const void *detector_server_virtual_table(void);

static int
DetectorServer_accept(void *_self, void **client)
{
    struct RPCServer *self = cast(RPCServer(), _self);
    
    int lfd, cfd;
    
    lfd = tcp_server_get_lfd(self);
    cfd = Accept(lfd, NULL, NULL);
    if (cfd < 0) {
        *client = NULL;
        return AAOS_ERROR;
    } else {
        if ((*client = new(Detector(), cfd)) == NULL) {
            Close(cfd);
            return AAOS_ERROR;
        }
        return AAOS_OK;
    }
}

static void *
DetectorServer_ctor(void *_self, va_list *app)
{
    struct DetectorServer *self = super_ctor(DetectorServer(), _self, app);
    
    self->_._vtab = detector_server_virtual_table();
    
    return (void *) self;
}

static void *
DetectorServer_dtor(void *_self)
{
    //struct RPC *self = cast(RPC(), _self);
    
    return super_dtor(DetectorServer(), _self);
}

static void *
DetectorServerClass_ctor(void *_self, va_list *app)
{
    struct DetectorServerClass *self = super_ctor(DetectorServerClass(), _self, app);
    
    self->_.accept.method = (Method) 0;
    
    return self;
}

static void *_DetectorServerClass;

static void
DetectorServerClass_destroy(void)
{
    free((void *) _DetectorServerClass);
}

static void
DetectorServerClass_initialize(void)
{
    _DetectorServerClass = new(RPCServerClass(), "DetectorServerClass", RPCServerClass(), sizeof(struct DetectorServerClass),
                             ctor, "ctor", DetectorServerClass_ctor,
                             (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(DetectorServerClass_destroy);
#endif
}

const void *
DetectorServerClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, DetectorServerClass_initialize);
#endif
    
    return _DetectorServerClass;
}

static void *_DetectorServer;

static void
DetectorServer_destroy(void)
{
    free((void *) _DetectorServer);
}

static void
DetectorServer_initialize(void)
{
    _DetectorServer = new(DetectorServerClass(), "DetectorServer", RPCServer(), sizeof(struct DetectorServer),
                        ctor, "ctor", DetectorServer_ctor,
                        dtor, "dtor", DetectorServer_dtor,
                        (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(DetectorServer_destroy);
#endif
}

const void *
DetectorServer(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, DetectorServer_initialize);
#endif
    
    return _DetectorServer;
}

static const void *_detector_server_virtual_table;

static void
detector_server_virtual_table_destroy(void)
{
    delete((void *) _detector_server_virtual_table);
}

static void
detector_server_virtual_table_initialize(void)
{

    _detector_server_virtual_table = new(RPCServerVirtualTable(),
                                       rpc_server_accept, "accept", DetectorServer_accept,
                                       (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(detector_server_virtual_table_destroy);
#endif
}

static const void *
detector_server_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, detector_server_virtual_table_initialize);
#endif
    
    return _detector_server_virtual_table;
}

#ifdef _USE_COMPILER_ATTRIBUTION_
static void __destructor__(void) __attribute__ ((destructor(_DETECTOR_RPC_PRIORITY_)));

static void
__destructor__(void)
{
    DetectorServer_destroy();
    DetectorServerClass_destroy();
    detector_server_virtual_table_destroy();
    DetectorClient_destroy();
    DetectorClientClass_destroy();
    detector_client_virtual_table_destroy();
    Detector_destroy();
    DetectorClass_destroy();
    detector_virtual_table_destroy();
}

static void __constructor__(void) __attribute__ ((constructor(_DETECTOR_RPC_PRIORITY_)));

static void
__constructor__(void)
{
    detector_virtual_table_initialize();
    DetectorClass_initialize();
    Detector_initialize();
    detector_client_virtual_table_initialize();
    DetectorClientClass_initialize();
    DetectorClient_initialize();
    detector_server_virtual_table_initialize();
    DetectorServerClass_initialize();
    DetectorServer_initialize();
}
#endif

/*
 * Compiler-dependant initializer.
 */
#ifdef _USE_COMPILER_ATTRIBUTION_
static void __destructor__(void) __attribute__ ((destructor(_DETECTOR_RPC_PRIORITY_)));

static void
__destructor__(void)
{
    detector_virtual_table_initialize();
    DetectorClass_initialize();
    Detector_initialize();
    detector_server_virtual_table_initialize();
    DetectorServerClass_initialize();
    DetectorServer_initialize();
    detector_client_virtual_table_initialize();
    DetectorClientClass_initialize();
    DetectorClient_initialize();
}

static void __constructor__(void) __attribute__ ((constructor(_DETECTOR_RPC_PRIORITY_)));

static void
__constructor__(void)
{
    DetectorClient_destroy();
    DetectorClientClass_destroy();
    detector_client_virtual_table_destroy();
    DetectorServer_destroy();
    DetectorServerClass_destroy();
    detector_server_virtual_table_destroy();
    Detector_destroy();
    DetectorClass_destroy();
    detector_virtual_table_destroy();
}
#endif
