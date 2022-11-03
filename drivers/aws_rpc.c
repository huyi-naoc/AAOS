//
//  aws_rpc.c
//  AAOS
//
//  Created by Hu Yi on 2020/1/16.
//  Copyright © 2020 NAOC. All rights reserved.
//

#include "aws_def.h"
#include "aws_rpc.h"
#include "aws_rpc_r.h"
#include "aws.h"
#include "def.h"
#include "protocol.h"
#include "wrapper.h"

void **awses;
size_t n_aws;

static int
get_index_by_name(const char *name, int *index)
{
    size_t i;
    const char *s;
    
    for (i = 0; i < n_aws; i++) {
        s = __aws_get_name(awses[i]);
        if (strcmp(name, s) == 0) {
            *index = (int) i + 1;
            return AAOS_OK;
        }
    }
    return AAOS_ENOTFOUND;
}

inline static void *
get_aws_by_index(int index)
{
    if (index > 0 && index <= n_aws) {
        return awses[index - 1];
    } else {
        return NULL;
    }
}

static int
get_channel_by_name(const char *name, int index, unsigned int *channel)
{
    void *aws = get_aws_by_index(index);
    
    return __aws_get_channel_by_name(aws, name, channel);

}

inline static int
AWS_protocol_check(void *_self)
{
    uint16_t protocol;
    protobuf_get(_self, PACKET_PROTOCOL, &protocol);
    if (protocol != PROTO_AWS && protocol != PROTO_SYSTEM) {
        protobuf_set(_self, PACKET_ERRORCODE, AAOS_EPROTOWRONG);
        protobuf_set(_self, PACKET_LENGTH, 0);
        return AAOS_EPROTOWRONG;
    } else {
        return AAOS_OK;
    }
}

/*
 * AWS class.
 */

int
aws_get_temperature_by_name(void *_self, const char *aws_name, const char *channel_name, double *temperature, size_t size)
{
    int ret;
    
    if ((ret = aws_get_index_by_name(_self, aws_name)) != AAOS_OK && (ret = aws_get_channel_by_name(_self, channel_name)) != AAOS_OK) {
        return ret;
    }
    
    return aws_get_temperature(_self, temperature, size);
}

int
aws_get_temperature_by_channel(void *_self, unsigned int index, unsigned int channel, double *temperature, size_t size)
{
    protobuf_set(_self, PACKET_INDEX, index);
    protobuf_set(_self, PACKET_CHANNEL, channel);
    
    return aws_get_temperature(_self, temperature, size);
}

int
aws_get_wind_speed_by_name(void *_self, const char *aws_name, const char *channel_name, double *wind_speed, size_t size)
{
    int ret;
    
    if ((ret = aws_get_index_by_name(_self, aws_name)) != AAOS_OK && (ret = aws_get_channel_by_name(_self, channel_name)) != AAOS_OK) {
        return ret;
    }
    
    return aws_get_wind_speed(_self,wind_speed, size);
}

int
aws_get_wind_speed_by_channel(void *_self, unsigned int index, unsigned int channel, double *wind_speed, size_t size)
{
    protobuf_set(_self, PACKET_INDEX, index);
    protobuf_set(_self, PACKET_CHANNEL, channel);
    
    return aws_get_wind_speed(_self, wind_speed, size);
}

int
aws_get_wind_direction_by_name(void *_self, const char *aws_name, const char *channel_name, double *wind_direction, size_t size)
{
    int ret;
    
    if ((ret = aws_get_index_by_name(_self, aws_name)) != AAOS_OK && (ret = aws_get_channel_by_name(_self, channel_name)) != AAOS_OK) {
        return ret;
    }
    
    return aws_get_wind_direction(_self,wind_direction, size);
}

int
aws_get_wind_direction_by_channel(void *_self, unsigned int index, unsigned int channel, double *wind_direction, size_t size)
{
    protobuf_set(_self, PACKET_INDEX, index);
    protobuf_set(_self, PACKET_CHANNEL, channel);
    
    return aws_get_wind_direction(_self, wind_direction, size);
}

int
aws_get_relative_humidity_by_name(void *_self, const char *aws_name, const char *channel_name, double *relative_humidity, size_t size)
{
    int ret;
    
    if ((ret = aws_get_index_by_name(_self, aws_name)) != AAOS_OK && (ret = aws_get_channel_by_name(_self, channel_name)) != AAOS_OK) {
        return ret;
    }
    
    return aws_get_relative_humidity(_self, relative_humidity, size);
}

int
aws_get_relative_humidity_by_channel(void *_self, unsigned int index, unsigned int channel, double *relative_humidity, size_t size)
{
    protobuf_set(_self, PACKET_INDEX, index);
    protobuf_set(_self, PACKET_CHANNEL, channel);
    
    return aws_get_relative_humidity(_self, relative_humidity, size);
}

int
aws_get_air_pressure_by_name(void *_self, const char *aws_name, const char *channel_name, double *air_pressure, size_t size)
{
    int ret;
    
    if ((ret = aws_get_index_by_name(_self, aws_name)) != AAOS_OK && (ret = aws_get_channel_by_name(_self, channel_name)) != AAOS_OK) {
        return ret;
    }
    
    return aws_get_air_pressure(_self, air_pressure, size);
}

int
aws_get_air_pressure_by_channel(void *_self, unsigned int index, unsigned int channel, double *air_pressure, size_t size)
{
    protobuf_set(_self, PACKET_INDEX, index);
    protobuf_set(_self, PACKET_CHANNEL, channel);
    
    return aws_get_air_pressure(_self, air_pressure, size);
}

int
aws_get_sky_quality_by_name(void *_self, const char *aws_name, const char *channel_name, double *sky_quality, size_t size)
{
    int ret;
    
    if ((ret = aws_get_index_by_name(_self, aws_name)) != AAOS_OK && (ret = aws_get_channel_by_name(_self, channel_name)) != AAOS_OK) {
        return ret;
    }
    
    return aws_get_sky_quality(_self, sky_quality, size);
}

int
aws_get_sky_quality_by_channel(void *_self, unsigned int index, unsigned int channel, double *sky_quality, size_t size)
{
    protobuf_set(_self, PACKET_INDEX, index);
    protobuf_set(_self, PACKET_CHANNEL, channel);
    
    return aws_get_sky_quality(_self, sky_quality, size);
}

int
aws_get_precipitation_by_name(void *_self, const char *aws_name, const char *channel_name, double *precipitation, size_t size)
{
    int ret;
    
    if ((ret = aws_get_index_by_name(_self, aws_name)) != AAOS_OK && (ret = aws_get_channel_by_name(_self, channel_name)) != AAOS_OK) {
        return ret;
    }
    
    return aws_get_precipitation(_self, precipitation, size);
}

int
aws_get_precipitation_by_channel(void *_self, unsigned int index, unsigned int channel, double *precipitation, size_t size)
{
    protobuf_set(_self, PACKET_INDEX, index);
    protobuf_set(_self, PACKET_CHANNEL, channel);
    
    return aws_get_precipitation(_self, precipitation, size);
}

int
aws_get_data_by_name(void *_self, const char *aws_name, const char *channel_name, double *data, size_t size)
{
    int ret;
    
    if ((ret = aws_get_index_by_name(_self, aws_name)) != AAOS_OK && (ret = aws_get_channel_by_name(_self, channel_name)) != AAOS_OK) {
        return ret;
    }
    
    return aws_get_data(_self, data, size);
}

int
aws_get_data_by_channel(void *_self, unsigned int index, unsigned int channel, double *data, size_t size)
{
    protobuf_set(_self, PACKET_INDEX, index);
    protobuf_set(_self, PACKET_CHANNEL, channel);
    
    return aws_get_data(_self, data, size);
}

int
aws_get_raw_data_by_name(void *_self, const char *aws_name, const char *channel_name, void *data, size_t size)
{
    int ret;
    
    if ((ret = aws_get_index_by_name(_self, aws_name)) != AAOS_OK && (ret = aws_get_channel_by_name(_self, channel_name)) != AAOS_OK) {
        return ret;
    }
    
    return aws_get_raw_data(_self, data, size);
}

int
aws_get_raw_data_by_channel(void *_self, unsigned int indedx, unsigned int channel, void *data, size_t size)
{
    protobuf_set(_self, PACKET_INDEX, indedx);
    protobuf_set(_self, PACKET_CHANNEL, channel);
    
    return aws_get_raw_data(_self, data, size);
}

int
aws_get_index_by_name(void *_self, const char *name)
{
    const struct AWSClass *class = (const struct AWSClass *) classOf(_self);
    
    if (isOf(class, AWSClass()) && class->get_index_by_name.method) {
        return ((int (*)(void *, const char *)) class->get_index_by_name.method)(_self, name);
    } else {
        int result;
        forward(_self, &result, (Method) aws_get_index_by_name, "get_index_by_name", _self, name);
        return result;
    }
}

static int
AWS_get_index_by_name(void *_self, const char *name)
{
    struct AWS *self = cast(AWS(), _self);
    
    size_t length;
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_AWS);
    protobuf_set(self, PACKET_COMMAND, AWS_COMMAND_GET_INDEX_BY_NAME);
    
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
aws_get_channel_by_name(void *_self, const char *name)
{
    const struct AWSClass *class = (const struct AWSClass *) classOf(_self);
    
    if (isOf(class, AWSClass()) && class->get_channel_by_name.method) {
        return ((int (*)(void *, const char *)) class->get_channel_by_name.method)(_self, name);
    } else {
        int result;
        forward(_self, &result, (Method) aws_get_channel_by_name, "get_channel_by_name", _self, name);
        return result;
    }
}

static int
AWS_get_channel_by_name(void *_self, const char *name)
{
    struct AWS *self = cast(AWS(), _self);
    
    size_t length;
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_AWS);
    protobuf_set(self, PACKET_COMMAND, AWS_COMMAND_GET_CHANNEL_BY_NAME);
    
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
aws_get_temperature(void *_self, double *temperature, size_t size)
{
    const struct AWSClass *class = (const struct AWSClass *) classOf(_self);
    
    if (isOf(class, AWSClass()) && class->get_temperature.method) {
        return ((int (*)(void *, double *, size_t)) class->get_temperature.method)(_self, temperature, size);
    } else {
        int result;
        forward(_self, &result, (Method) aws_get_temperature, "get_temperature", _self, temperature, size);
        return result;
    }
}

static int
AWS_get_temperature(void *_self, double *temperature, size_t size)
{
    struct AWS *self = cast(AWS(), _self);
    
    int ret;
    uint32_t length;
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_AWS);
    protobuf_set(self, PACKET_COMMAND, AWS_COMMAND_GET_TEMPERATURE);
    protobuf_set(self, PACKET_U32F0, size);
    protobuf_set(self, PACKET_LENGTH, 0);
    
    if ((ret = rpc_call(self)) != AAOS_OK) {
        return ret;
    }
    
    protobuf_get(self, PACKET_LENGTH, &length);
    
    if (length == 0) {
        protobuf_get(self, PACKET_DF0, &temperature[0]);
        if (size > 1) {
            protobuf_get(self, PACKET_DF1, &temperature[1]);
        }
    } else {
        const char *buf;
        size_t i, n;
        
        protobuf_get(self, PACKET_BUF, &buf, NULL);
        n = min(length / sizeof(double), size);
        for (i = 0; i < n; i++) {
            memcpy(&temperature[i], buf + i * sizeof(double), sizeof(double));
        }
    }
    
    return AAOS_OK;
}

int
aws_get_wind_speed(void *_self, double *wind_speed, size_t size)
{
    const struct AWSClass *class = (const struct AWSClass *) classOf(_self);
    
    if (isOf(class, AWSClass()) && class->get_wind_speed.method) {
        return ((int (*)(void *, double *, size_t)) class->get_wind_speed.method)(_self, wind_speed, size);
    } else {
        int result;
        forward(_self, &result, (Method) aws_get_wind_speed, "get_wind_speed", _self, wind_speed, size);
        return result;
    }
}

static int
AWS_get_wind_speed(void *_self, double *wind_speed, size_t size)
{
    struct AWS *self = cast(AWS(), _self);
    
    int ret;
    uint32_t length;
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_AWS);
    protobuf_set(self, PACKET_COMMAND, AWS_COMMAND_GET_WIND_SPEED);
    protobuf_set(self, PACKET_U32F0, size);
    protobuf_set(self, PACKET_LENGTH, 0);
    
    if ((ret = rpc_call(self)) != AAOS_OK) {
        return ret;
    }
    
    protobuf_get(self, PACKET_LENGTH, &length);
    
    if (length == 0) {
        protobuf_get(self, PACKET_DF0, &wind_speed[0]);
        if (size > 1) {
            protobuf_get(self, PACKET_DF1, &wind_speed[1]);
        }
    } else {
        const char *buf;
        size_t i, n;
        
        protobuf_get(self, PACKET_BUF, &buf, NULL);
        n = min(length / sizeof(double), size);
        for (i = 0; i < n; i++) {
            memcpy(&wind_speed[i], buf + i * sizeof(double), sizeof(double));
        }
    }
    
    return AAOS_OK;
}

int
aws_get_wind_direction(void *_self, double *wind_speed, size_t size)
{
    const struct AWSClass *class = (const struct AWSClass *) classOf(_self);
    
    if (isOf(class, AWSClass()) && class->get_wind_direction.method) {
        return ((int (*)(void *, double *, size_t)) class->get_wind_direction.method)(_self, wind_speed, size);
    } else {
        int result;
        forward(_self, &result, (Method) aws_get_wind_speed, "get_wind_speed", _self, wind_speed, size);
        return result;
    }
}

static int
AWS_get_wind_direction(void *_self, double *wind_direction, size_t size)
{
    struct AWS *self = cast(AWS(), _self);
    
    int ret;
    uint32_t length;
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_AWS);
    protobuf_set(self, PACKET_COMMAND, AWS_COMMAND_GET_WIND_DIRECTION);
    protobuf_set(self, PACKET_U32F0, size);
    protobuf_set(self, PACKET_LENGTH, 0);
    
    if ((ret = rpc_call(self)) != AAOS_OK) {
        return ret;
    }
    
    protobuf_get(self, PACKET_LENGTH, &length);
    
    if (length == 0) {
        protobuf_get(self, PACKET_DF0, &wind_direction[0]);
        if (size > 1) {
            protobuf_get(self, PACKET_DF1, &wind_direction[1]);
        }
    } else {
        const char *buf;
        size_t i, n;
        
        protobuf_get(self, PACKET_BUF, &buf, NULL);
        n = min(length / sizeof(double), size);
        for (i = 0; i < n; i++) {
            memcpy(&wind_direction[i], buf + i * sizeof(double), sizeof(double));
        }
    }
    
    return AAOS_OK;
}

int
aws_get_relative_humidity(void *_self, double *relative_humidity, size_t size)
{
    const struct AWSClass *class = (const struct AWSClass *) classOf(_self);
    
    if (isOf(class, AWSClass()) && class->get_relative_humidity.method) {
        return ((int (*)(void *, double *, size_t)) class->get_relative_humidity.method)(_self, relative_humidity, size);
    } else {
        int result;
        forward(_self, &result, (Method) aws_get_relative_humidity, "get_relative_humidity", _self, relative_humidity, size);
        return result;
    }
}

static int
AWS_get_relative_humidity(void *_self, double *relative_humidity, size_t size)
{
    struct AWS *self = cast(AWS(), _self);
    
    int ret;
    uint32_t length;
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_AWS);
    protobuf_set(self, PACKET_COMMAND, AWS_COMMAND_GET_RELATIVE_HUMIDITY);
    protobuf_set(self, PACKET_U32F0, size);
    protobuf_set(self, PACKET_LENGTH, 0);
    
    if ((ret = rpc_call(self)) != AAOS_OK) {
        return ret;
    }
    
    protobuf_get(self, PACKET_LENGTH, &length);
    
    if (length == 0) {
        protobuf_get(self, PACKET_DF0, &relative_humidity[0]);
        if (size > 1) {
            protobuf_get(self, PACKET_DF1, &relative_humidity[1]);
        }
    } else {
        const char *buf;
        size_t i, n;
        
        protobuf_get(self, PACKET_BUF, &buf, NULL);
        n = min(length / sizeof(double), size);
        for (i = 0; i < n; i++) {
            memcpy(&relative_humidity[i], buf + i * sizeof(double), sizeof(double));
        }
    }
    
    return AAOS_OK;
}

int
aws_get_air_pressure(void *_self, double *air_pressure, size_t size)
{
    const struct AWSClass *class = (const struct AWSClass *) classOf(_self);
    
    if (isOf(class, AWSClass()) && class->get_air_pressure.method) {
        return ((int (*)(void *, double *, size_t)) class->get_air_pressure.method)(_self, air_pressure, size);
    } else {
        int result;
        forward(_self, &result, (Method) aws_get_air_pressure, "get_air_pressure", _self, air_pressure, size);
        return result;
    }
}

static int
AWS_get_air_pressure(void *_self, double *air_pressure, size_t size)
{
    struct AWS *self = cast(AWS(), _self);
    
    int ret;
    uint32_t length;
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_AWS);
    protobuf_set(self, PACKET_COMMAND, AWS_COMMAND_GET_RELATIVE_HUMIDITY);
    protobuf_set(self, PACKET_U32F0, size);
    protobuf_set(self, PACKET_LENGTH, 0);
    
    if ((ret = rpc_call(self)) != AAOS_OK) {
        return ret;
    }
    
    protobuf_get(self, PACKET_LENGTH, &length);
    
    if (length == 0) {
        protobuf_get(self, PACKET_DF0, &air_pressure[0]);
        if (size > 1) {
            protobuf_get(self, PACKET_DF1, &air_pressure[1]);
        }
    } else {
        const char *buf;
        size_t i, n;
        
        protobuf_get(self, PACKET_BUF, &buf, NULL);
        n = min(length / sizeof(double), size);
        for (i = 0; i < n; i++) {
            memcpy(&air_pressure[i], buf + i * sizeof(double), sizeof(double));
        }
    }
    
    return AAOS_OK;
}

int
aws_get_sky_quality(void *_self, double *sky_quality, size_t size)
{
    const struct AWSClass *class = (const struct AWSClass *) classOf(_self);
    
    if (isOf(class, AWSClass()) && class->get_sky_quality.method) {
        return ((int (*)(void *, double *, size_t)) class->get_sky_quality.method)(_self, sky_quality, size);
    } else {
        int result;
        forward(_self, &result, (Method) aws_get_sky_quality, "get_sky_quality", _self, sky_quality, size);
        return result;
    }
}

static int
AWS_get_sky_quality(void *_self, double *sky_quality, size_t size)
{
    struct AWS *self = cast(AWS(), _self);
    
    int ret;
    uint32_t length;
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_AWS);
    protobuf_set(self, PACKET_COMMAND, AWS_COMMAND_GET_SKY_QUALITY);
    protobuf_set(self, PACKET_U32F0, size);
    protobuf_set(self, PACKET_LENGTH, 0);
    
    if ((ret = rpc_call(self)) != AAOS_OK) {
        return ret;
    }
    
    protobuf_get(self, PACKET_LENGTH, &length);
    
    if (length == 0) {
        protobuf_get(self, PACKET_DF0, &sky_quality[0]);
        if (size > 1) {
            protobuf_get(self, PACKET_DF1, &sky_quality[1]);
        }
    } else {
        const char *buf;
        size_t i, n;
        
        protobuf_get(self, PACKET_BUF, &buf, NULL);
        n = min(length / sizeof(double), size);
        for (i = 0; i < n; i++) {
            memcpy(&sky_quality[i], buf + i * sizeof(double), sizeof(double));
        }
    }
    
    return AAOS_OK;
}

int
aws_get_precipitation(void *_self, double *precipitation, size_t size)
{
    const struct AWSClass *class = (const struct AWSClass *) classOf(_self);
    
    if (isOf(class, AWSClass()) && class->get_precipitation.method) {
        return ((int (*)(void *, double *, size_t)) class->get_precipitation.method)(_self, precipitation, size);
    } else {
        int result;
        forward(_self, &result, (Method) aws_get_precipitation, "get_precipitation", _self, precipitation, size);
        return result;
    }
}

static int
AWS_get_precipitation(void *_self, double *precipitation, size_t size)
{
    struct AWS *self = cast(AWS(), _self);
    
    int ret;
    uint32_t length;
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_AWS);
    protobuf_set(self, PACKET_COMMAND, AWS_COMMAND_GET_PRECIPITATION);
    protobuf_set(self, PACKET_U32F0, size);
    protobuf_set(self, PACKET_LENGTH, 0);
    
    if ((ret = rpc_call(self)) != AAOS_OK) {
        return ret;
    }
    
    protobuf_get(self, PACKET_LENGTH, &length);
    
    if (length == 0) {
        protobuf_get(self, PACKET_DF0, &precipitation[0]);
        if (size > 1) {
            protobuf_get(self, PACKET_DF1, &precipitation[1]);
        }
    } else {
        const char *buf;
        size_t i, n;
        
        protobuf_get(self, PACKET_BUF, &buf, NULL);
        n = min(length / sizeof(double), size);
        for (i = 0; i < n; i++) {
            memcpy(&precipitation[i], buf + i * sizeof(double), sizeof(double));
        }
    }
    
    return AAOS_OK;
}

int
aws_get_data(void *_self, double *data, size_t size)
{
    const struct AWSClass *class = (const struct AWSClass *) classOf(_self);
    
    if (isOf(class, AWSClass()) && class->get_data.method) {
        return ((int (*)(void *, double *, size_t)) class->get_data.method)(_self, data, size);
    } else {
        int result;
        forward(_self, &result, (Method) aws_get_data, "get_data", _self, data, size);
        return result;
    }
}

static int
AWS_get_data(void *_self, double *data, size_t size)
{
    struct AWS *self = cast(AWS(), _self);
    
    int ret;
    uint32_t length;
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_AWS);
    protobuf_set(self, PACKET_COMMAND, AWS_COMMAND_GET_DATA);
    protobuf_set(self, PACKET_U32F0, size);
    protobuf_set(self, PACKET_LENGTH, 0);
    
    if ((ret = rpc_call(self)) != AAOS_OK) {
        return ret;
    }
    
    protobuf_get(self, PACKET_LENGTH, &length);
    
    if (length == 0) {
        protobuf_get(self, PACKET_DF0, &data[0]);
        if (size > 1) {
            protobuf_get(self, PACKET_DF1, &data[1]);
        }
    } else {
        const char *buf;
        size_t i, n;
        
        protobuf_get(self, PACKET_BUF, &buf, NULL);
        n = min(length / sizeof(double), size);
        for (i = 0; i < n; i++) {
            memcpy(&data[i], buf + i * sizeof(double), sizeof(double));
        }
    }
    
    return AAOS_OK;
}

int
aws_get_raw_data(void *_self, void *data, size_t size)
{
    const struct AWSClass *class = (const struct AWSClass *) classOf(_self);
    
    if (isOf(class, AWSClass()) && class->get_raw_data.method) {
        return ((int (*)(void *, void *, size_t)) class->get_raw_data.method)(_self, data, size);
    } else {
        int result;
        forward(_self, &result, (Method) aws_get_raw_data, "get_raw_data", _self, data, size);
        return result;
    }
}

static int
AWS_get_raw_data(void *_self, void *data, size_t size)
{
    struct AWS *self = cast(AWS(), _self);
    
    int ret;
    uint32_t length;
    char *s;
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_AWS);
    protobuf_set(self, PACKET_COMMAND, AWS_COMMAND_GET_RAW_DATA);
    protobuf_set(self, PACKET_LENGTH, 0);
    
    if ((ret = rpc_call(self)) != AAOS_OK) {
        return ret;
    }
    
    protobuf_get(self, PACKET_LENGTH, &length);
    
    if (length == 0) {
        protobuf_get(self, PACKET_STR, &s);
    } else {
        protobuf_get(self, PACKET_BUF, &s, NULL);
    }
    
    if (s != data) {
        snprintf(data, size, "%s", s);
    }
    
    return AAOS_OK;
}

int
aws_status(void *_self, FILE *fp)
{
    const struct AWSClass *class = (const struct AWSClass *) classOf(_self);
    
    if (isOf(class, AWSClass()) && class->status.method) {
        return ((int (*)(void *, FILE *fp)) class->status.method)(_self, fp);
    } else {
        int result;
        forward(_self, &result, (Method) aws_status, "status", _self, fp);
        return result;
    }
}

static int
AWS_status(void *_self, FILE *fp)
{
    struct AWS *self = cast(AWS(), _self);
    
    int ret;
    uint32_t length;
    char *s;
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_AWS);
    protobuf_set(self, PACKET_COMMAND, AWS_COMMAND_STATUS);
    
    if ((ret = rpc_call(self)) != AAOS_OK) {
        return ret;
    }
    
    protobuf_get(self, PACKET_LENGTH, &length);
    
    if (length == 0) {
        protobuf_get(self, PACKET_STR, &s);
    } else {
        protobuf_get(self, PACKET_BUF, &s, NULL);
    }
    fprintf(fp, "%s", s);
    
    return AAOS_OK;
}

int 
aws_data_log(void *_self, FILE *fp)
{
    const struct AWSClass *class = (const struct AWSClass *) classOf(_self);
    
    if (isOf(class, AWSClass()) && class->data_log.method) {
        return ((int (*)(void *, FILE *fp)) class->data_log.method)(_self, fp);
    } else {
        int result;
        forward(_self, &result, (Method) aws_data_log, "data_log", _self, fp);
        return result;
    }
}

static int
AWS_data_log(void *_self, FILE *fp)
{
    struct AWS *self = cast(AWS(), _self);
    
    int ret;
    uint32_t length;
    char *s;
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_AWS);
    protobuf_set(self, PACKET_COMMAND, AWS_COMMAND_DATA_LOG);
    
    if ((ret = rpc_call(self)) != AAOS_OK) {
        return ret;
    }
    
    protobuf_get(self, PACKET_LENGTH, &length);
    
    if (length == 0) {
        protobuf_get(self, PACKET_STR, &s);
    } else {
        protobuf_get(self, PACKET_BUF, &s, NULL);
    }
    fprintf(fp, "%s", s);
    
    return AAOS_OK;
}

int
aws_data_field(void *_self, FILE *fp)
{
    const struct AWSClass *class = (const struct AWSClass *) classOf(_self);
    
    if (isOf(class, AWSClass()) && class->data_field.method) {
        return ((int (*)(void *, FILE *fp)) class->data_field.method)(_self, fp);
    } else {
        int result;
        forward(_self, &result, (Method) aws_data_field, "data_field", _self, fp);
        return result;
    }
}

static int
AWS_data_field(void *_self, FILE *fp)
{
    struct AWS *self = cast(AWS(), _self);
    
    int ret;
    uint32_t length;
    char *s;
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_AWS);
    protobuf_set(self, PACKET_COMMAND, AWS_COMMAND_DATA_FIELD);
    
    if ((ret = rpc_call(self)) != AAOS_OK) {
        return ret;
    }
    
    protobuf_get(self, PACKET_LENGTH, &length);
    
    if (length == 0) {
        protobuf_get(self, PACKET_STR, &s);
    } else {
        protobuf_get(self, PACKET_BUF, &s, NULL);
    }
    fprintf(fp, "%s", s);
    
    return AAOS_OK;
}

int
aws_inspect(void *_self)
{
    const struct AWSClass *class = (const struct AWSClass *) classOf(_self);
    
    if (isOf(class, AWSClass()) && class->inspect.method) {
        return ((int (*)(void *)) class->inspect.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) aws_inspect, "inspect", _self);
        return result;
    }
}

static int
AWS_inspect(void *_self)
{
    struct AWS *self = cast(AWS(), _self);
    void *protobuf = self->_.protobuf;
    
    protobuf_set(protobuf, PACKET_PROTOCOL, PROTO_SERIAL);
    protobuf_set(protobuf, PACKET_COMMAND, AWS_COMMAND_INSPECT);
    protobuf_set(self, PACKET_LENGTH, 0);
    
    return rpc_call(self);
}

int
aws_register(void *_self, double timeout)
{
    const struct AWSClass *class = (const struct AWSClass *) classOf(_self);
    
    if (isOf(class, AWSClass()) && class->reg.method) {
        return ((int (*)(void *, double)) class->reg.method)(_self, timeout);
    } else {
        int result;
        forward(_self, &result, (Method) aws_register, "register", _self, timeout);
        return result;
    }
}

static int
AWS_register(void *_self, double timeout)
{
    struct AWS *self = cast(AWS(), _self);
    
    void *protobuf = self->_.protobuf;
    
    protobuf_set(protobuf, PACKET_PROTOCOL, PROTO_SYSTEM);
    protobuf_set(protobuf, PACKET_COMMAND, AWS_COMMAND_REGISTER);
    protobuf_set(protobuf, PACKET_DF0, timeout);
    
    return rpc_call(_self);
}

static int
AWS_execute_get_index_by_name(struct AWS *self)
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
AWS_execute_get_channel_by_name(struct AWS *self)
{
    char *name;
    int ret;
    uint16_t index;
    uint32_t length;
    const void *aws;
    unsigned int channel;
    
    protobuf_get(self, PACKET_LENGTH, &length);
    protobuf_get(self, PACKET_INDEX, &index);
    
    if (length == 0) {
        protobuf_get(self, PACKET_STR, &name);
    } else {
        protobuf_get(self, PACKET_BUF, &name, NULL);
    }
    
    if ((aws = get_aws_by_index(index)) == NULL) {
        return AAOS_ENOTFOUND;
    }
    
    if ((ret = get_channel_by_name(name, index, &channel)) != AAOS_OK) {
        return ret;
    } else {
        uint16_t chan = (uint16_t) channel;
        protobuf_set(self, PACKET_CHANNEL, chan);
        protobuf_set(self, PACKET_LENGTH, 0);
    }
    
    return AAOS_OK;
}

static int
AWS_execute_get_temperature(struct AWS *self)
{
    void *aws;
    uint16_t index, channel;
    uint32_t size;
    int ret;
    double *temperature;
    
    protobuf_get(self, PACKET_INDEX, &index);
    protobuf_get(self, PACKET_CHANNEL, &channel);
    protobuf_get(self, PACKET_U32F0, &size);
    
    if ((temperature = (double *) Malloc(size * sizeof(double))) == NULL) {
        return AAOS_ENOMEM;
    }
    memset(temperature, '\0', size * sizeof(double));
    
    if ((aws = get_aws_by_index(index)) == NULL) {
        return AAOS_ENOTFOUND;
    }
    
    if ((ret = __aws_get_temperature_by_channel(aws, channel, temperature, size)) != AAOS_OK) {
        return ret;
    }
    
    if (size < 3) {
        protobuf_set(self, PACKET_LENGTH, 0);
        protobuf_set(self, PACKET_DF0, temperature[0]);
        if (size > 1) {
            protobuf_set(self, PACKET_DF1, temperature[1]);
        }
    } else {
        protobuf_set(self, PACKET_LENGTH, size * sizeof(double));
        protobuf_set(self, PACKET_BUF, temperature, size * sizeof(double));
    }
    
    free(temperature);
    
    return AAOS_OK;
}

static int
AWS_execute_get_wind_speed(struct AWS *self)
{
    void *aws;
    uint16_t index, channel;
    uint32_t size;
    int ret;
    double *wind_speed;
    
    protobuf_get(self, PACKET_INDEX, &index);
    protobuf_get(self, PACKET_CHANNEL, &channel);
    protobuf_get(self, PACKET_U32F0, &size);
    
    if ((wind_speed = (double *) Malloc(size * sizeof(double))) == NULL) {
        return AAOS_ENOMEM;
    }
    memset(wind_speed, '\0', size * sizeof(double));
    
    if ((aws = get_aws_by_index(index)) == NULL) {
        return AAOS_ENOTFOUND;
    }
    
    if ((ret = __aws_get_wind_speed_by_channel(aws, channel, wind_speed, size)) != AAOS_OK) {
        return ret;
    }
    
    if (size < 3) {
        protobuf_set(self, PACKET_LENGTH, 0);
        protobuf_set(self, PACKET_DF0, wind_speed[0]);
        if (size > 1) {
            protobuf_set(self, PACKET_DF1, wind_speed[1]);
        }
    } else {
        protobuf_set(self, PACKET_LENGTH, size * sizeof(double));
        protobuf_set(self, PACKET_BUF, wind_speed, size * sizeof(double));
    }
    
    free(wind_speed);
    
    return AAOS_OK;
}

static int
AWS_execute_get_wind_direction(struct AWS *self)
{
    void *aws;
    uint16_t index, channel;
    uint32_t size;
    int ret;
    double *wind_direction;
    
    protobuf_get(self, PACKET_INDEX, &index);
    protobuf_get(self, PACKET_CHANNEL, &channel);
    protobuf_get(self, PACKET_U32F0, &size);
    
    if ((wind_direction = (double *) Malloc(size * sizeof(double))) == NULL) {
        return AAOS_ENOMEM;
    }
    memset(wind_direction, '\0', size * sizeof(double));
    
    if ((aws = get_aws_by_index(index)) == NULL) {
        return AAOS_ENOTFOUND;
    }
    
    if ((ret = __aws_get_wind_direction_by_channel(aws, channel, wind_direction, size)) != AAOS_OK) {
        return ret;
    }
    
    if (size < 3) {
        protobuf_set(self, PACKET_LENGTH, 0);
        protobuf_set(self, PACKET_DF0, wind_direction[0]);
        if (size > 1) {
            protobuf_set(self, PACKET_DF1, wind_direction[1]);
        }
    } else {
        protobuf_set(self, PACKET_LENGTH, size * sizeof(double));
        protobuf_set(self, PACKET_BUF, wind_direction, size * sizeof(double));
    }
    
    free(wind_direction);
    
    return AAOS_OK;
}

static int
AWS_execute_get_relative_humidity(struct AWS *self)
{
    void *aws;
    uint16_t index, channel;
    uint32_t size;
    int ret;
    double *relative_humidity;
    
    protobuf_get(self, PACKET_INDEX, &index);
    protobuf_get(self, PACKET_CHANNEL, &channel);
    protobuf_get(self, PACKET_U32F0, &size);
    
    if ((relative_humidity = (double *) Malloc(size * sizeof(double))) == NULL) {
        return AAOS_ENOMEM;
    }
    memset(relative_humidity, '\0', size * sizeof(double));
    
    if ((aws = get_aws_by_index(index)) == NULL) {
        return AAOS_ENOTFOUND;
    }
    
    if ((ret = __aws_get_relative_humidity_by_channel(aws, channel, relative_humidity, size)) != AAOS_OK) {
        return ret;
    }
    
    if (size < 3) {
        protobuf_set(self, PACKET_LENGTH, 0);
        protobuf_set(self, PACKET_DF0, relative_humidity[0]);
        if (size > 1) {
            protobuf_set(self, PACKET_DF1, relative_humidity[1]);
        }
    } else {
        protobuf_set(self, PACKET_LENGTH, size * sizeof(double));
        protobuf_set(self, PACKET_BUF, relative_humidity, size * sizeof(double));
    }
    
    free(relative_humidity);
    
    return AAOS_OK;
}

static int
AWS_execute_get_air_pressure(struct AWS *self)
{
    void *aws;
    uint16_t index, channel;
    uint32_t size;
    int ret;
    double *air_pressure;
    
    protobuf_get(self, PACKET_INDEX, &index);
    protobuf_get(self, PACKET_CHANNEL, &channel);
    protobuf_get(self, PACKET_U32F0, &size);
    
    if ((air_pressure = (double *) Malloc(size * sizeof(double))) == NULL) {
        return AAOS_ENOMEM;
    }
    memset(air_pressure, '\0', size * sizeof(double));
    
    if ((aws = get_aws_by_index(index)) == NULL) {
        return AAOS_ENOTFOUND;
    }
    
    if ((ret = __aws_get_air_pressure_by_channel(aws, channel, air_pressure, size)) != AAOS_OK) {
        return ret;
    }
    
    if (size < 3) {
        protobuf_set(self, PACKET_LENGTH, 0);
        protobuf_set(self, PACKET_DF0, air_pressure[0]);
        if (size > 1) {
            protobuf_set(self, PACKET_DF1, air_pressure[1]);
        }
    } else {
        protobuf_set(self, PACKET_LENGTH, size * sizeof(double));
        protobuf_set(self, PACKET_BUF, air_pressure, size * sizeof(double));
    }
    
    free(air_pressure);
    
    return AAOS_OK;
}


static int
AWS_execute_get_sky_quality(struct AWS *self)
{
    void *aws;
    uint16_t index, channel;
    uint32_t size;
    int ret;
    double *sky_quality;
    
    protobuf_get(self, PACKET_INDEX, &index);
    protobuf_get(self, PACKET_CHANNEL, &channel);
    protobuf_get(self, PACKET_U32F0, &size);
    
    if ((sky_quality = (double *) Malloc(size * sizeof(double))) == NULL) {
        return AAOS_ENOMEM;
    }
    memset(sky_quality, '\0', size * sizeof(double));
    
    if ((aws = get_aws_by_index(index)) == NULL) {
        return AAOS_ENOTFOUND;
    }
    
    if ((ret = __aws_get_sky_quality_by_channel(aws, channel, sky_quality, size)) != AAOS_OK) {
        return ret;
    }
    
    if (size < 3) {
        protobuf_set(self, PACKET_LENGTH, 0);
        protobuf_set(self, PACKET_DF0, sky_quality[0]);
        if (size > 1) {
            protobuf_set(self, PACKET_DF1, sky_quality[1]);
        }
    } else {
        protobuf_set(self, PACKET_LENGTH, size * sizeof(double));
        protobuf_set(self, PACKET_BUF, sky_quality, size * sizeof(double));
    }
    
    free(sky_quality);
    
    return AAOS_OK;
}

static int
AWS_execute_get_precipitation(struct AWS *self)
{
    void *aws;
    uint16_t index, channel;
    uint32_t size;
    int ret;
    double *precipitation;
    
    protobuf_get(self, PACKET_INDEX, &index);
    protobuf_get(self, PACKET_CHANNEL, &channel);
    protobuf_get(self, PACKET_U32F0, &size);
    
    if ((precipitation = (double *) Malloc(size * sizeof(double))) == NULL) {
        return AAOS_ENOMEM;
    }
    memset(precipitation, '\0', size * sizeof(double));
    
    if ((aws = get_aws_by_index(index)) == NULL) {
        return AAOS_ENOTFOUND;
    }
    
    if ((ret = __aws_get_precipitation_by_channel(aws, channel, precipitation, size)) != AAOS_OK) {
        return ret;
    }
    
    if (size < 3) {
        protobuf_set(self, PACKET_LENGTH, 0);
        protobuf_set(self, PACKET_DF0, precipitation[0]);
        if (size > 1) {
            protobuf_set(self, PACKET_DF1, precipitation[1]);
        }
    } else {
        protobuf_set(self, PACKET_LENGTH, size * sizeof(double));
        protobuf_set(self, PACKET_BUF, precipitation, size * sizeof(double));
    }
    
    free(precipitation);
    
    return AAOS_OK;
}
                 
static int
AWS_execute_get_data(struct AWS *self)
{
    void *aws, *sensor;
    uint16_t index, channel;
    unsigned int type;
    
    protobuf_get(self, PACKET_INDEX, &index);
    protobuf_get(self, PACKET_CHANNEL, &channel);
    
    if ((aws = get_aws_by_index(index)) == NULL) {
        return AAOS_ENOTFOUND;
    }
    
    if ((sensor = __aws_get_sensor(aws, channel)) == NULL) {
        return AAOS_ENOTFOUND;
    }
    
    type = sensor_get_type(sensor);
    
    switch (type) {
        case SENSOR_TYPE_TEMEPRATURE:
            return AWS_execute_get_temperature(self);
            break;
        case SENSOR_TYPE_WIND_SPEED:
            return AWS_execute_get_wind_speed(self);
            break;
        case SENSOR_TYPE_WIND_DIRECTION:
            return AWS_execute_get_wind_direction(self);
            break;
        case SENSOR_TYPE_RELATIVE_HUMIDITY:
            return AWS_execute_get_relative_humidity(self);
            break;
        case SENSOR_TYPE_AIR_PRESSURE:
            return AWS_execute_get_air_pressure(self);
            break;
        case SENSOR_TYPE_SKY_QUALITY:
            return AWS_execute_get_sky_quality(self);
            break;
        case SENSOR_TYPE_PRECIPITATION:
            return AWS_execute_get_precipitation(self);
            break;
        default:
            return AAOS_ERROR;
            break;
    }
}

static int
AWS_execute_get_raw_data(struct AWS *self)
{
    void *aws, *sensor;
    uint16_t index, channel;
    int ret;
    char buf[COMMANDSIZE];
    
    protobuf_get(self, PACKET_INDEX, &index);
    protobuf_get(self, PACKET_CHANNEL, &channel);
    
    if ((aws = get_aws_by_index(index)) == NULL) {
        return AAOS_ENOTFOUND;
    }
    
    if ((sensor = __aws_get_sensor(aws, channel)) == NULL) {
        return AAOS_ENOTFOUND;
    }
    
    if ((ret = __aws_get_raw_data_by_channel(aws, channel, buf, COMMANDSIZE)) != AAOS_OK) {
        protobuf_set(self, PACKET_LENGTH, 0);
        return ret;
    }
    if (strlen(buf) < PACKETPARAMETERSIZE - 1) {
        protobuf_set(self, PACKET_STR, buf);
        protobuf_set(self, PACKET_LENGTH, 0);
    } else {
        protobuf_set(self, PACKET_BUF, buf, strlen(buf) + 1);
        protobuf_set(self, PACKET_LENGTH, strlen(buf) + 1);
    }
    
    return AAOS_OK;
}


static int
AWS_execute_status(struct AWS *self)
{
    void *aws;
    uint16_t index;
    size_t size;
    char *buf;
    FILE *fp;
    
    protobuf_get(self, PACKET_INDEX, &index);
    if ((aws = get_aws_by_index(index)) == NULL) {
        return AAOS_ENOTFOUND;
    }
    
    fp = open_memstream(&buf, &size);
    __aws_status(aws, fp);
    fclose(fp);
    protobuf_set(self, PACKET_BUF, buf, size);
    free(buf);
  
    return AAOS_OK;
}

static int
AWS_execute_data_log(struct AWS *self)
{
    void *aws;
    uint16_t index;
    size_t size;
    char *buf;
    FILE *fp;
    
    protobuf_get(self, PACKET_INDEX, &index);
    if ((aws = get_aws_by_index(index)) == NULL) {
        return AAOS_ENOTFOUND;
    }
    
    fp = open_memstream(&buf, &size);
    __aws_data_log(aws, fp);
    fclose(fp);
    protobuf_set(self, PACKET_BUF, buf, size);
    free(buf);
  
    return AAOS_OK;
}
                 
static int
AWS_execute_data_field(struct AWS *self)
{
    void *aws;
    uint16_t index, errorcode;
    size_t size;
    char *buf;
    FILE *fp;
    
    protobuf_get(self, PACKET_INDEX, &index);
    if ((aws = get_aws_by_index(index)) == NULL) {
        errorcode = AAOS_ENOTFOUND;
        protobuf_set(self, PACKET_ERRORCODE, errorcode);
        protobuf_set(self, PACKET_LENGTH, 0);
        return AAOS_ENOTFOUND;
    }
    /*
     * it seems that linux do not support fmemopen for write, so use open_memstream instead.
     */
    fp = open_memstream(&buf, &size);
    __aws_data_field(aws, fp);
    fclose(fp);
    protobuf_set(self, PACKET_BUF, buf, size);
    free(buf);
    
    return AAOS_OK;
}

static int
AWS_execute_inspect(struct AWS *self)
{
    uint16_t index;
    void *aws;
    
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
        if ((get_index_by_name(s, &idx)) != AAOS_OK) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
        index = (uint16_t) idx;
        protobuf_set(self, PACKET_INDEX, &index);
        if ((aws = get_aws_by_index(index)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    } else {
        if ((aws = get_aws_by_index(index)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    }
    
    return __aws_inspect(aws);
}

static int
AWS_execute_register(struct AWS *self)
{
    void *protobuf = self->_.protobuf;
    
    uint16_t index;
    void *aws;
    double timeout;
    int ret;
    
    protobuf_get(protobuf, PACKET_INDEX, &index);
    protobuf_get(protobuf, PACKET_DF0, &timeout);
    
    if ((aws = get_aws_by_index((int) index)) == NULL) {
        protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
        protobuf_set(self, PACKET_LENGTH, 0);
        return AAOS_ENOTFOUND;
    }
    
    if (__aws_inspect(aws) != AAOS_OK) {
        ret = __aws_wait(aws, timeout);
        protobuf_set(self, PACKET_ERRORCODE, AAOS_ETIMEDOUT);
        protobuf_set(self, PACKET_LENGTH, 0);
        return ret;
    }
    
    return AAOS_OK;
}

static int
AWS_execute_default(struct AWS *self)
{
    return AAOS_EBADCMD;
}

static int
AWS_execute(void *_self)
{
    struct AWS *self = cast(AWS(), _self);
    uint16_t command;
    int ret;
    
    if (AWS_protocol_check(self) != AAOS_OK) {
        return AAOS_EPROTOWRONG;
    }
    
    protobuf_get(self, PACKET_COMMAND, &command);
    
    switch (command) {
        case AWS_COMMAND_GET_INDEX_BY_NAME:
            ret = AWS_execute_get_index_by_name(self);
            break;
        case AWS_COMMAND_GET_CHANNEL_BY_NAME:
            ret = AWS_execute_get_channel_by_name(self);
            break;
        case AWS_COMMAND_GET_TEMPERATURE:
            ret = AWS_execute_get_temperature(self);
            break;
        case AWS_COMMAND_GET_WIND_SPEED:
            ret = AWS_execute_get_wind_speed(self);
            break;
        case AWS_COMMAND_GET_WIND_DIRECTION:
            ret = AWS_execute_get_wind_direction(self);
            break;
        case AWS_COMMAND_GET_RELATIVE_HUMIDITY:
            ret = AWS_execute_get_relative_humidity(self);
            break;
        case AWS_COMMAND_GET_AIR_PRESSURE:
            ret = AWS_execute_get_air_pressure(self);
            break;
        case AWS_COMMAND_GET_SKY_QUALITY:
            ret = AWS_execute_get_sky_quality(self);
            break;
        case AWS_COMMAND_GET_PRECIPITATION:
            ret = AWS_execute_get_precipitation(self);
            break;
        case AWS_COMMAND_GET_DATA:
            ret = AWS_execute_get_data(self);
            break;
        case AWS_COMMAND_GET_RAW_DATA:
            ret = AWS_execute_get_raw_data(self);
            break;
        case AWS_COMMAND_DATA_LOG:
            ret = AWS_execute_data_log(self);
            break;
        case AWS_COMMAND_DATA_FIELD:
            ret = AWS_execute_data_field(self);
            break;
        case AWS_COMMAND_STATUS:
            ret = AWS_execute_status(self);
            break;
        case SYSTEM_COMMAND_INSPECT:
            ret = AWS_execute_inspect(self);
            break;
        case SYSTEM_COMMAND_REGISTER:
            ret = AWS_execute_register(self);
            break;
        default:
            return ret = AWS_execute_default(self);
            break;
    }
    
    return ret;
}

static const void *aws_virtual_table(void);

static void *
AWS_ctor(void *_self, va_list *app)
{
    struct AWS *self = super_ctor(AWS(), _self, app);
    
    self->_._vtab = aws_virtual_table();
    
    return (void *) self;
}

static void *
AWS_dtor(void *_self)
{
    //struct RPC *self = cast(RPC(), _self);
    
    return super_dtor(AWS(), _self);
}

static void *
AWSClass_ctor(void *_self, va_list *app)
{
    struct AWSClass *self = super_ctor(AWSClass(), _self, app);
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
        
        if (selector == (Method) aws_get_index_by_name) {
            if (tag) {
                self->get_index_by_name.tag = tag;
                self->get_index_by_name.selector = selector;
            }
            self->get_index_by_name.method = method;
            continue;
        }
        if (selector == (Method) aws_get_channel_by_name) {
            if (tag) {
                self->get_channel_by_name.tag = tag;
                self->get_channel_by_name.selector = selector;
            }
            self->get_channel_by_name.method = method;
            continue;
        }
        if (selector == (Method) aws_get_temperature) {
            if (tag) {
                self->get_temperature.tag = tag;
                self->get_temperature.selector = selector;
            }
            self->get_temperature.method = method;
            continue;
        }
        if (selector == (Method) aws_get_wind_speed) {
            if (tag) {
                self->get_wind_speed.tag = tag;
                self->get_wind_speed.selector = selector;
            }
            self->get_wind_speed.method = method;
            continue;
        }
        if (selector == (Method) aws_get_wind_direction) {
            if (tag) {
                self->get_wind_direction.tag = tag;
                self->get_wind_direction.selector = selector;
            }
            self->get_wind_direction.method = method;
            continue;
        }
        if (selector == (Method) aws_get_relative_humidity) {
            if (tag) {
                self->get_relative_humidity.tag = tag;
                self->get_relative_humidity.selector = selector;
            }
            self->get_relative_humidity.method = method;
            continue;
        }
        if (selector == (Method) aws_get_air_pressure) {
            if (tag) {
                self->get_air_pressure.tag = tag;
                self->get_air_pressure.selector = selector;
            }
            self->get_air_pressure.method = method;
            continue;
        }
        if (selector == (Method) aws_get_sky_quality) {
            if (tag) {
                self->get_sky_quality.tag = tag;
                self->get_sky_quality.selector = selector;
            }
            self->get_sky_quality.method = method;
            continue;
        }
        if (selector == (Method) aws_get_sky_quality) {
            if (tag) {
                self->get_sky_quality.tag = tag;
                self->get_sky_quality.selector = selector;
            }
            self->get_sky_quality.method = method;
            continue;
        }
        if (selector == (Method) aws_get_precipitation) {
            if (tag) {
                self->get_precipitation.tag = tag;
                self->get_precipitation.selector = selector;
            }
            self->get_precipitation.method = method;
            continue;
        }
        if (selector == (Method) aws_get_data) {
            if (tag) {
                self->get_data.tag = tag;
                self->get_data.selector = selector;
            }
            self->get_data.method = method;
            continue;
        }
        if (selector == (Method) aws_get_raw_data) {
            if (tag) {
                self->get_raw_data.tag = tag;
                self->get_raw_data.selector = selector;
            }
            self->get_raw_data.method = method;
            continue;
        }
        if (selector == (Method) aws_status) {
            if (tag) {
                self->status.tag = tag;
                self->status.selector = selector;
            }
            self->status.method = method;
            continue;
        }
        if (selector == (Method) aws_data_log) {
            if (tag) {
                self->data_log.tag = tag;
                self->data_log.selector = selector;
            }
            self->data_log.method = method;
            continue;
        }
        if (selector == (Method) aws_data_field) {
            if (tag) {
                self->data_field.tag = tag;
                self->data_field.selector = selector;
            }
            self->data_field.method = method;
            continue;
        }
        if (selector == (Method) aws_inspect) {
            if (tag) {
                self->inspect.tag = tag;
                self->inspect.selector = selector;
            }
            self->inspect.method = method;
            continue;
        }
        if (selector == (Method) aws_register) {
            if (tag) {
                self->reg.tag = tag;
                self->reg.selector = selector;
            }
            self->reg.method = method;
            continue;
        }
    }
    
#ifdef va_copy
    va_end(ap);
#endif
    return (void *) self;
    
}

static void *_AWSClass;

static void
AWSClass_destroy(void)
{
    free((void *) _AWSClass);
}

static void
AWSClass_initialize(void)
{
    _AWSClass = new(RPCClass(), "AWSClass", RPCClass(), sizeof(struct AWSClass),
                    ctor, "ctor", AWSClass_ctor,
                    (void *) 0);
    
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(AWSClass_destroy);
#endif
    
}

const void *
AWSClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, AWSClass_initialize);
#endif
    
    return _AWSClass;
}

static void *_AWS;

static void
AWS_destroy(void)
{
    free((void *) _AWS);
}

static void
AWS_initialize(void)
{
    _AWS = new(AWSClass(), "AWS", RPC(), sizeof(struct AWS),
               ctor, "ctor", AWS_ctor,
               dtor, "dtor", AWS_dtor,
               aws_get_index_by_name, "get_index_by_name", AWS_get_index_by_name,
               aws_get_channel_by_name, "get_channel_by_name", AWS_get_channel_by_name,
               aws_get_temperature, "get_temperature", AWS_get_temperature,
               aws_get_wind_speed, "get_wind_speed", AWS_get_wind_speed,
               aws_get_wind_direction, "get_wind_direction", AWS_get_wind_direction,
               aws_get_relative_humidity, "get_relative_humidity", AWS_get_relative_humidity,
               aws_get_air_pressure, "get_air_pressure", AWS_get_air_pressure,
               aws_get_sky_quality, "get_sky_quality", AWS_get_sky_quality,
               aws_get_precipitation, "get_precipitation", AWS_get_precipitation,
               aws_get_data, "get_data", AWS_get_data,
               aws_get_raw_data, "get_raw_data", AWS_get_raw_data,
               aws_data_log, "data_log", AWS_data_log,
               aws_data_field, "data_field", AWS_data_field,
               aws_status, "status", AWS_status,
               aws_inspect, "inspect", AWS_inspect,
               aws_register, "register", AWS_register,
               (void *) 0);
    
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(AWS_destroy);
#endif
    
}

const void *
AWS(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, AWS_initialize);
#endif
    
    return _AWS;
}

static const void *_aws_virtual_table;

static void
aws_virtual_table_destroy(void)
{
    delete((void *) _aws_virtual_table);
}


static void
aws_virtual_table_initialize(void)
{
    _aws_virtual_table = new(RPCVirtualTable(),
                             rpc_execute, "execute", AWS_execute,
                             (void *)0);
    
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(aws_virtual_table_destroy);
#endif
    
}

static const void *
aws_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, aws_virtual_table_initialize);
#endif
    
    return _aws_virtual_table;
}

/*
 * AWS client
 */

static const void *aws_client_virtual_table(void);

static
int AWSClient_connect(void *_self, void **client)
{
    struct AWSClient *self = cast(AWSClient(), _self);
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
    
    *client = new(AWS(), cfd);
    protobuf_set(*client, PACKET_PROTOCOL, PROTO_AWS);
    
    return ret;
}

static void *
AWSClient_ctor(void *_self, va_list *app)
{
    struct AWSClient *self = super_ctor(AWSClient(), _self, app);
    
    self->_._vtab = aws_client_virtual_table();
    
    return (void *) self;
}

static void *
AWSClient_dtor(void *_self)
{
    //struct RPC *self = cast(RPC(), _self);
    
    return super_dtor(AWSClient(), _self);
}

static void *
AWSClientClass_ctor(void *_self, va_list *app)
{
    struct AWSClientClass *self = super_ctor(AWSClientClass(), _self, app);
    
    self->_.connect.method = (Method) 0;
    
    return self;
}

static void *_AWSClientClass;

static void
AWSClientClass_destroy(void)
{
    free((void *) _AWSClientClass);
}

static void
AWSClientClass_initialize(void)
{
    _AWSClientClass = new(RPCClientClass(), "AWSClientClass", RPCClientClass(), sizeof(struct AWSClientClass),
                          ctor, "ctor", AWSClientClass_ctor,
                          (void *) 0);
    
    
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(AWSClientClass_destroy);
#endif
    
}

const void *
AWSClientClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, AWSClientClass_initialize);
#endif
    
    return _AWSClientClass;
}

static void *_AWSClient;

static void
AWSClient_destroy(void)
{
    free((void *) _AWSClient);
}

static void
AWSClient_initialize(void)
{
    _AWSClient = new(AWSClientClass(), "AWSClient", RPCClient(), sizeof(struct AWSClient),
                           ctor, "ctor", AWSClient_ctor,
                           dtor, "dtor", AWSClient_dtor,
                           (void *) 0);
    
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(AWSClient_destroy);
#endif
}

const void *
AWSClient(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, AWSClient_initialize);
#endif
    
    return _AWSClient;
}

static const void *_aws_client_virtual_table;

static void
aws_client_virtual_table_destroy(void)
{
    delete((void *) _aws_client_virtual_table);
}

static void
aws_client_virtual_table_initialize(void)
{
    _aws_client_virtual_table = new(RPCClientVirtualTable(),
                                    rpc_client_connect, "connect", AWSClient_connect,
                                    (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(aws_client_virtual_table_destroy);
#endif
    
}

static const void *
aws_client_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, aws_client_virtual_table_initialize);
#endif
    
    return _aws_client_virtual_table;
}

/*
* AWS server
*/

static const void *aws_server_virtual_table(void);

static int
AWSServer_accept(void *_self, void **client)
{
    struct RPCServer *self = cast(RPCServer(), _self);
    
    int lfd, cfd;
    
    lfd = tcp_server_get_lfd(self);
    cfd = Accept(lfd, NULL, NULL);
    if (cfd < 0) {
        *client = NULL;
        return AAOS_ERROR;
    } else {
        if ((*client = new(AWS(), cfd)) == NULL) {
            Close(cfd);
            return AAOS_ERROR;
        }
        return AAOS_OK;
    }
}

static void *
AWSServer_ctor(void *_self, va_list *app)
{
    struct AWSServer *self = super_ctor(AWSServer(), _self, app);
    
    self->_._vtab = aws_server_virtual_table();
    
    return (void *) self;
}

static void *
AWSServer_dtor(void *_self)
{
    //struct RPC *self = cast(RPC(), _self);
    
    return super_dtor(AWSServer(), _self);
}

static void *
AWSServerClass_ctor(void *_self, va_list *app)
{
    struct AWSServerClass *self = super_ctor(AWSServerClass(), _self, app);
    
    self->_.accept.method = (Method) 0;
    
    return self;
}

static void *_AWSServerClass;

static void
AWSServerClass_destroy(void)
{
    free((void *) _AWSServerClass);
}

static void
AWSServerClass_initialize(void)
{
    _AWSServerClass = new(RPCServerClass(), "AWSServerClass", RPCServerClass(), sizeof(struct AWSServerClass),
                             ctor, "ctor", AWSServerClass_ctor,
                             (void *) 0);
    
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(AWSServerClass_destroy);
#endif
}

const void *
AWSServerClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, AWSServerClass_initialize);
#endif
    
    return _AWSServerClass;
}

static void *_AWSServer;

static void
AWSServer_destroy(void)
{
    free((void *) _AWSServer);
}

static void
AWSServer_initialize(void)
{
    _AWSServer = new(AWSServerClass(), "AWSServer", RPCServer(), sizeof(struct AWSServer),
                     ctor, "ctor", AWSServer_ctor,
                     dtor, "dtor", AWSServer_dtor,
                     (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(AWSServer_destroy);
#endif
}

const void *
AWSServer(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, AWSServer_initialize);
#endif
    
    return _AWSServer;
}

static const void *_aws_server_virtual_table;

static void
aws_server_virtual_table_destroy(void)
{
    delete((void *) _aws_server_virtual_table);
}

static void
aws_server_virtual_table_initialize(void)
{
    _aws_server_virtual_table = new(RPCServerVirtualTable(),
                                       rpc_server_accept, "accept", AWSServer_accept,
                                       (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(aws_server_virtual_table_destroy);
#endif
}

static const void *
aws_server_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, aws_server_virtual_table_initialize);
#endif
    
    return _aws_server_virtual_table;
}

#ifdef _USE_COMPILER_ATTRIBUTION_
static void __constructor__(void) __attribute__ ((constructor(_AWS_RPC_PRIORITY_)));

static void
__constructor__(void)
{
    aws_virtual_table_initialize();
    AWSClass_initialize();
    AWS_initialize();
    aws_client_virtual_table_initialize();
    AWSClientClass_initialize();
    AWSClient_initialize();
    aws_server_virtual_table_initialize();
    AWSServerClass_initialize();
    AWSServer_initialize();
}

static void __destructor__(void) __attribute__ ((destructor(_AWS_RPC_PRIORITY_)));

static void
__destructor__(void)
{
    AWSServer_destroy();
    AWSServerClass_destroy();
    aws_server_virtual_table_destroy();
    AWSClient_destroy();
    AWSClientClass_destroy();
    aws_client_virtual_table_destroy();
    AWS_destroy();
    AWSClass_destroy();
    aws_virtual_table_destroy();
}
#endif
