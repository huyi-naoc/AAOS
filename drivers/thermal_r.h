//
//  thermal_r.h
//  AAOS
//
//  Created by Hu Yi on 2023/2/25.
//  Copyright © 2023 NAOC. All rights reserved.
//

#ifndef thermal_r_h
#define thermal_r_h

#include "object_r.h"
#include "virtual_r.h"

#include <pthread.h>

struct __ThermalUnit {
    struct Object _;
    const void *_vtab;
    char *name;
    char *description;
    double highest;
    double lowest;
    double period;
    unsigned int state;
    pthread_mutex_t mtx;
};

struct __ThermalUnitClass {
    struct Class _;
    struct Method get_temperature;
    struct Method get_name;
    struct Method status;
    struct Method thermal_control;
    struct Method turn_on;
    struct Method turn_off;
};

struct __ThermalUnitVirtualTable {
    struct VirtualTable _;
    struct Method get_temperature;
    struct Method status;
    struct Method thermal_control;
    struct Method turn_on;
    struct Method turn_off;
};

/*
struct KLCAMThermalUnit {
    struct __ThermalUnit _;
    char *external_addr;
    char *external_port;
    char *external_aws;
    char *external_sensor;
    char *external_addr2;
    char *external_port2;
    char *external_switch;
    char *external_channel;
    char *internal_addr;
    char *internal_port;
    char *internal_aws;
    char *internal_sensor;
    char *internal_addr2;
    char *internal_port2;
    char *internal_switch;
    char *internal_channel;
};

struct KLCAMThermalUnitClass {
    struct __ThermalUnitClass _;
};
 */

struct SimpleThermalUnit {
    struct __ThermalUnit _;
    char *temp_cmd;
    char *turn_on_cmd;
    char *turn_off_cmd;
};

struct SimpleThermalUnitClass {
    struct __ThermalUnitClass _;
};

struct KLCAMSimpleThermalUnit {
    struct __ThermalUnit _;
    char *temp_cmd;         /* get temperature command, internal */
    char *turn_on_cmd;      /* turn on heater(s) command, internal */
    char *turn_off_cmd;     /* turn off command, internal */
    char *temp_cmd2;        /* turn on command, external */
    char *turn_on_cmd2;     /* turn off command, external */
    char *turn_off_cmd2;    /* turn off command, external */
    char *temp_cmd3;        /* get ambient temperature command */
    double threshold;       /* thershold for turn on/off external heater(s) */
    unsigned int state;     /* external heater state */
    pthread_mutex_t mtx;
    
};

struct KLCAMSimpleThermalUnitClass {
    struct __ThermalUnitClass _;
};


#endif /* thermal_r_h */
