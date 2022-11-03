//
//  aws_rpc.h
//  AAOS
//
//  Created by Hu Yi on 2020/1/16.
//  Copyright © 2020 NAOC. All rights reserved.
//

#ifndef aws_rpc_h
#define aws_rpc_h

#include "rpc.h"
#include <string.h>
#include <stdio.h>

#define AWS_COMMAND_GET_TEMPERATURE         1
#define AWS_COMMAND_GET_WIND_SPEED          2
#define AWS_COMMAND_GET_WIND_DIRECTION      3
#define AWS_COMMAND_GET_RELATIVE_HUMIDITY   4
#define AWS_COMMAND_GET_AIR_PRESSURE        5
#define AWS_COMMAND_GET_SKY_QUALITY         6
#define AWS_COMMAND_GET_PRECIPITATION       7
#define AWS_COMMAND_GET_DATA                8
#define AWS_COMMAND_GET_RAW_DATA            9
#define AWS_COMMAND_DATA_LOG                10
#define AWS_COMMAND_DATA_FIELD              11
#define AWS_COMMAND_STATUS                  12
#define AWS_COMMAND_INSPECT                 0xFFFE
#define AWS_COMMAND_REGISTER                0xFFFF

#define AWS_COMMAND_GET_INDEX_BY_NAME 23
#define AWS_COMMAND_GET_CHANNEL_BY_NAME 24

int aws_get_index_by_name(void *_self, const char *name);
int aws_get_channel_by_name(void *_self, const char *name);
int aws_get_temperature(void *_self, double *temperature, size_t size);
int aws_get_wind_speed(void *_self, double *wind_speed, size_t size);
int aws_get_wind_direction(void *_self, double *wind_direction, size_t size);
int aws_get_relative_humidity(void *_self, double *relative_humidity, size_t size);
int aws_get_air_pressure(void *_self, double *air_pressure, size_t size);
int aws_get_sky_quality(void *_self, double *sky_quality, size_t size);
int aws_get_precipitation(void *_self, double *precipitation, size_t size);
int aws_get_data(void *_self, double *data, size_t size);
int aws_get_raw_data(void *_self, void *data, size_t size);

int aws_get_temperature_by_name(void *_self, const char *aws_name, const char * channel_name, double *temperature, size_t size);
int aws_get_temperature_by_channel(void *_self, unsigned int index, unsigned int channel, double *temperature, size_t size);
int aws_get_wind_speed_by_name(void *_self, const char *aws_name, const char * channel_name, double *wind_speed, size_t size);
int aws_get_wind_speed_by_channel(void *_self, unsigned int index, unsigned int channel, double *wind_speed, size_t size);
int aws_get_wind_direction_by_name(void *_self, const char *aws_name, const char * channel_name, double *wind_direction, size_t size);
int aws_get_wind_direction_by_channel(void *_self, unsigned int index, unsigned int channel, double *wind_direction, size_t size);
int aws_get_relative_humidity_by_name(void *_self, const char *aws_name, const char * channel_name, double *relative_humidity, size_t size);
int aws_get_relative_humidity_by_channel(void *_self, unsigned int index, unsigned int channel, double *relative_humidity, size_t size);
int aws_get_sky_quality_by_name(void *_self, const char *aws_name, const char * channel_name, double *sky_quality, size_t size);
int aws_get_sky_quality_by_channel(void *_self, unsigned int index, unsigned int channel, double *sky_quality, size_t size);
int aws_get_precipitation_by_name(void *_self, const char *aws_name, const char * channel_name, double *precipitation, size_t size);
int aws_get_precipitation_by_channel(void *_self, unsigned int index, unsigned int channel, double *precipitation, size_t size);
int aws_get_data_by_name(void *_self, const char *aws_name, const char *channel_name, double *data, size_t size);
int aws_get_data_by_channel(void *_self, unsigned int index, unsigned int channel, double *data, size_t size);
int aws_get_raw_data_by_name(void *_self, const char *aws_name, const char *channel_name, void *data, size_t size);
int aws_get_raw_data_by_channel(void *_self, unsigned int index, unsigned int channel, void *data, size_t size);
int aws_status(void *_self, FILE *fp);
int aws_data_log(void *_self, FILE *fp);
int aws_data_field(void *_self, FILE *fp);
int aws_register(void *_self, double timeout);
int aws_inspect(void *_self);

extern const void *AWS(void);
extern const void *AWSClass(void);

extern const void *AWSClient(void);
extern const void *AWSClientClass(void);

extern const void *AWSServer(void);
extern const void *AWSServerClass(void);

extern void **awses;
extern size_t n_aws;

#endif /* aws_rpc_h */
