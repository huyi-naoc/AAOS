//
//  aws.h
//  AAOS
//
//  Created by huyi on 2018/10/29.
//  Copyright © 2018 National Astronomical Observatories, Chinese Academy of Sciences. All rights reserved.
//

#ifndef aws_h
#define aws_h

#define TEMEPRATURE_OUTPUT_TEMPERATURE  1
#define TEMEPRATURE_OUTPUT_RESISTANSE   0

#include <string.h>
#include <stdio.h>
#include <stdint.h>

extern const void *SensorVirtualTable(void);
extern const void *Sensor(void);
extern const void *SensorClass(void);

extern const void *PT100(void);
extern const void *PT100Class(void);
extern const void *AAGPDUPT1000(void);
extern const void *AAGPDUPT1000Class(void);
extern const void *Young41342(void);
extern const void *Young41342Class(void);
extern const void *Young05305VS(void);
extern const void *Young05305VSClass(void);
extern const void *Young05305VD(void);
extern const void *Young05305VDClass(void);
extern const void *Young61302V(void);
extern const void *Young61302VClass(void);
extern const void *Young41382VCR(void);
extern const void *Young41382VCRClass(void);
extern const void *Young41382VCT(void);
extern const void *Young41382VCTClass(void);
extern const void *SkyQualityMonitor(void);
extern const void *SkyQualityMonitorClass(void);
extern const void *WS100UMB(void);
extern const void *WS100UMBClass(void);
extern const void *PTB210(void);
extern const void *PTB210Class(void);
extern const void *HCD6817C(void);
extern const void *HCD6817CClass(void);


int sensor_read_data(void *_self, double *data, size_t size);
int sensor_read_raw_data(void *_self, void *data, size_t size);
void sensor_set_controller(void *_self, const void *controller);
void sensor_set_device(void *_self, const void *device);
const char *sensor_get_name(const void *_self);
unsigned int sensor_get_channel(const void *_self);
unsigned int sensor_get_type(const void *_self);
void sensor_set_channel(void *_self, unsigned int channel);
void sensor_set_type(void *_self, unsigned int type);
void sensor_format_put(void *_self, FILE *fp);

extern const void *__AWSVirtualTable(void);
extern const void *__AWS(void);
extern const void *__AWSClass(void);

void __aws_set_sensor(void *_self, const void *sensor, size_t index);
void *__aws_get_sensor(void *_self, size_t index);
int __aws_get_channel_by_name(const void *_self, const char *name, unsigned int *channel);
const char *__aws_get_name(const void *_self);
int __aws_get_temperature_by_name(void *_self, const char *name, double *temperature, size_t size);
int __aws_get_temperature_by_channel(void *_self, unsigned int channel, double *temperature, size_t size);
int __aws_get_wind_speed_by_name(void *_self, const char *name, double *wind_speed, size_t size);
int __aws_get_wind_speed_by_channel(void *_self, unsigned int channel, double *wind_speed, size_t size);
int __aws_get_wind_direction_by_name(void *_self, const char *name, double *wind_direction, size_t size);
int __aws_get_wind_direction_by_channel(void *_self, unsigned int channel, double *wind_direction, size_t size);
int __aws_get_relative_humidity_by_name(void *_self, const char *name, double *relative_humidity, size_t size);
int __aws_get_relative_humidity_by_channel(void *_self, unsigned int channel, double *relative_humidity, size_t size);
int __aws_get_air_pressure_by_name(void *_self, const char *name, double *air_pressure, size_t size);
int __aws_get_air_pressure_by_channel(void *_self, unsigned int channel, double *air_pressure, size_t size);
int __aws_get_sky_quality_by_name(void *_self, const char *name, double *sky_quality, size_t size);
int __aws_get_sky_quality_by_channel(void *_self, unsigned int channel, double *sky_quality, size_t size);
int __aws_get_precipitation_by_name(void *_self, const char *name, double *precipitation, size_t size);
int __aws_get_precipitation_by_channel(void *_self, unsigned int channel, double *precipitation, size_t size);
int __aws_get_raw_data_by_name(void *_self, const char *name, void *data, size_t size);
int __aws_get_raw_data_by_channel(void *_self, unsigned int channel, void *data, size_t size);
void __aws_data_log(void *_self, FILE *fp);
void __aws_data_field(void *_self, FILE *fp);
void __aws_status(void *_self, FILE *fp);
int __aws_wait(void *_self, double timeout);
int __aws_inspect(void *_self);

extern const void *KLAWS(void);
extern const void *KLAWSClass(void);

void *klaws_get_controller(void *_self, size_t index);
void klaws_set_controller(void *_self, const void *controller, size_t index);

extern const void *KLAWSController(void);
extern const void *KLAWSControllerClass(void);

void *klaws_controller_get_device(void *_self, size_t index);
void klaws_controller_set_device(void *_self, const void *device, size_t index);
void *klaws_controller_get_serial(void *_self);
void klaws_controller_lock(void *_self);
void klaws_controller_unlock(void *_self);
int klaws_controller_inspect(void *_self);

extern const void *KLAWSDevice(void);
extern const void *KLAWSDeviceClass(void);

void klaws_device_set_index(void *_self, void *serial);
uint16_t klaws_device_get_index(void *_self);
int klaws_device_inspect(void *_self, void *serial);

#endif /* aws_h */
