//
//  aws_r.h
//  AAOS
//
//  Created by huyi on 2018/10/29.
//  Copyright © 2018 National Astronomical Observatories, Chinese Academy of Sciences. All rights reserved.
//

#ifndef aws_r_h
#define aws_r_h


#include "object_r.h"
#include "virtual_r.h"
#include "rpc.h"
#include <pthread.h>

#define _AWS_PRIORITY_  _VIRTUAL_PRIORITY_ + 1

struct Sensor {
    struct Object _;
    const void *_vtab;
    char *name;         /* Sensor name */
    char *model;        /* model name */
    char *command;      /* command */
    char *format;       /* Output format */
    char *description;  /* Description */
    unsigned int type;
    unsigned int channel;
    void *controller;   /* Controller */
    void *device;       /* Device */
};

struct SensorClass {
    struct Class _;
    struct Method set_controller;
    struct Method set_device;
    struct Method read_data;
    struct Method read_raw_data;
    struct Method set_channel;
    struct Method get_channel;
    struct Method set_type;
    struct Method get_type;
    struct Method get_name;
    struct Method format_put;
};

struct SensorVirtualTable {
    struct VirtualTable _;
    struct Method read_data;
    struct Method read_raw_data;
    struct Method set_controller;
    struct Method set_device;
    struct Method format_put;
};

struct PT100 {
    struct Sensor _;
    int output_type;
};

struct PT100Class {
    struct SensorClass _;
};

struct AAGPDUPT1000 {
    struct Sensor _;
    int output_type;
};

struct AAGPDUPT1000Class {
    struct SensorClass _;
};


struct Young41342 {
    struct Sensor _;
    int output_type;
};

struct Young41342Class {
    struct SensorClass _;
};

struct Young05305VS {
    struct Sensor _;
};

struct  Young05305VSClass {
    struct SensorClass _;
};

struct Young05305VD {
    struct Sensor _;
    double offset; /* installation offset to the north direction, N->E->S->W */
};

struct  Young05305VDClass {
    struct SensorClass _;
};

struct Young61302V {
    struct Sensor _;
};

struct Young61302VClass {
    struct SensorClass _;
};

struct  Young41382VCR {
    struct Sensor _;
};

struct  Young41382VCRClass {
    struct SensorClass _;
};

struct Young41382VCT {
    struct Sensor _;
};

struct Young41382VCTClass {
    struct SensorClass _;
};

struct SkyQualityMonitor {
    struct Sensor _;
    double extinction; /* extinction caused by window shield */
};

struct SkyQualityMonitorClass {
    struct SensorClass _;
};

struct WS100UMB {
    struct Sensor _;
};

struct WS100UMBClass {
    struct SensorClass _;
};

struct PTB210 {
    struct Sensor _;
    int output_type;
};

struct PTB210Class {
    struct SensorClass _;
};

struct HCD6817C {
    struct Sensor _;
    unsigned int *functions;
    size_t n_function;
};

struct HCD6817CClass {
    struct SensorClass _;
};


struct __AWS {
    struct Object _;
    const void *_vtab;
    char *name;
    char *description;
    char *delimiter;
    char *newline;
    void **sensors;
    pthread_mutex_t mtx;
    pthread_cond_t cond;
    int state;
    size_t n_sensors;
};

struct __AWSClass {
    struct Class _;
    struct Method set_sensor;
    struct Method get_sensor;
    struct Method get_channel_by_name;
    struct Method get_temperature_by_name;
    struct Method get_wind_speed_by_name;
    struct Method get_wind_direction_by_name;
    struct Method get_air_pressure_by_name;
    struct Method get_relative_humidity_by_name;
    struct Method get_sky_quality_by_name;
    struct Method get_precipitation_by_name;
    struct Method get_raw_data_by_name;
    struct Method get_temperature_by_channel;
    struct Method get_wind_speed_by_channel;
    struct Method get_wind_direction_by_channel;
    struct Method get_air_pressure_by_channel;
    struct Method get_relative_humidity_by_channel;
    struct Method get_sky_quality_by_channel;
    struct Method get_precipitation_by_channel;
    struct Method get_raw_data_by_channel;
    struct Method get_name;
    struct Method data_log;
    struct Method data_field;
    struct Method status;
    struct Method inspect;
    struct Method wait;
};

struct __AWSVirtualTable {
    struct VirtualTable _;
    struct Method get_temperature_by_name;
    struct Method get_wind_speed_by_name;
    struct Method get_wind_direction_by_name;
    struct Method get_air_pressure_by_name;
    struct Method get_relative_humidity_by_name;
    struct Method get_sky_quality_by_name;
    struct Method get_precipitation_by_name;
    struct Method get_temperature_by_channel;
    struct Method get_wind_speed_by_channel;
    struct Method get_wind_direction_by_channel;
    struct Method get_air_pressure_by_channel;
    struct Method get_relative_humidity_by_channel;
    struct Method get_sky_quality_by_channel;
    struct Method get_precipitation_by_channel;
    struct Method data_log;
    struct Method inspect;
};

struct KLAWSDevice {
    struct Object _;
    char *name;
    int critical;
    uint16_t index;
};

struct KLAWSDeviceClass {
    struct Class _;
    struct Method get_index;
    struct Method set_index;
    struct Method inspect;
};

struct KLAWSController {
    struct Object _;
    void *serial;
    char *address;
    char *port;
    struct KLAWSDevice **devices;
    size_t n_device;
    int critical;
    pthread_mutex_t mtx;
};

struct KLAWSControllerClass {
    struct Class _;
    struct Method lock;
    struct Method unlock;
    struct Method get_device;
    struct Method set_device;
    struct Method get_serial;
    struct Method inspect;
};

struct KLAWS {
    struct __AWS _;
    void **sensors;
    void **controllers;
    size_t n_controller;
    size_t n_deivce;
};

struct KLAWSClass {
    struct __AWSClass _;
    struct Method set_node;
    struct Method set_controller;
    struct Method get_controller;
    struct Method inspect;
};

#endif /* aws_r_h */
