//
//  telescope_r.h
//  AAOS
//
//  Created by huyi on 18/6/12.
//  Copyright © 2018年 National Astronomical Observatories, Chinese Academy of Sciences. All rights reserved.
//

#ifndef __telescope_r_h
#define __telescope_r_h

#include <stdint.h>
#include <pthread.h>
#include "object_r.h"
#include "virtual_r.h"

#define _TELESCOPE_PRIORITY_ _VIRTUAL_PRIORITY_ + 1

struct TelescopeState {
    unsigned int state;
    pthread_mutex_t mtx;
    pthread_cond_t cond;
};

struct __Telescope {
    const struct Object _;
    const void *_vtab;
    char *name;
    char *description;
    
    double location_lon; /* longtitude of the observation, degree */
    double location_lat; /* latitude of the observation, degree */
    double location_ele; /* altitude of the observation, meter */
    double gmt_offset;
    
    double track_rate_x; /* unit 15 arcseconds per second */
    double track_rate_y; /* unit 15 arcseconds per second */
    double ra;           /* current ra, degree */
    double dec;          /* current dec, degree */
    double alt;          /* current altitude of the telescope, degree */
    double az;           /* current azumith of the telescope, degree */

    double move_speed;
    unsigned int move_direction;
    double move_duration;
    
    double ra_from;
    double dec_from;
    double ra_to;
    double dec_to;
    double slew_speed_x;
    double slew_speed_y;
    int slew_direction_x;
    int slew_direction_y;
    
    double last_slew_begin_time;  /* slew begin time */
    double last_track_begin_time; /* track begin time */
    double last_move_begin_time;  /* move begin time */
    double last_park_begin_time;  /* park begin time */
    
    uint16_t option;
    
    pthread_t tid;
    
    struct TelescopeState t_state;
};

struct __TelescopeClass {
    const struct Class _;
    struct Method release;
    struct Method get_name;
    struct Method set_option;
    
    struct Method status;
    struct Method power_on;
    struct Method power_off;
    struct Method init;
    struct Method park;
    struct Method park_off;
    struct Method stop;
    struct Method go_home;
    struct Method move;
    struct Method try_move;
    struct Method timed_move;
    struct Method slew;
    struct Method try_slew;
    struct Method timed_slew;
    struct Method raw;
    struct Method set_slew_speed;
    struct Method get_slew_speed;
    struct Method set_move_speed;
    struct Method get_move_speed;
    struct Method set_track_rate;
    struct Method get_track_rate;

    struct Method switch_filter;
    struct Method switch_instrument;
    struct Method switch_detector;
    
    struct Method inspect;
    struct Method wait;
};

struct __TelescopeVirtualTable {
    struct VirtualTable _;
    struct Method status;
    struct Method power_on;
    struct Method power_off;
    struct Method init;
    struct Method park;
    struct Method park_off;
    struct Method stop;
    struct Method go_home;
    struct Method move;
    struct Method try_move;
    struct Method timed_move;
    struct Method slew;
    struct Method try_slew;
    struct Method timed_slew;
    struct Method raw;
    struct Method set_slew_speed;
    struct Method get_slew_speed;
    struct Method set_move_speed;
    struct Method get_move_speed;
    struct Method set_track_rate;
    struct Method get_track_rate;

    struct Method switch_filter;
    struct Method switch_instrument;
    struct Method switch_detector;
    
    struct Method inspect;
    struct Method wait;
    
};

struct VirtualTelescope {
    struct __Telescope _;
};

struct VirtualTelescopeClass {
    struct __TelescopeClass _;
};

struct APMount {
    struct __Telescope _;
    int type;
    char *serial_server_address;
    char *serial_server_port;
    char *serial_name;
    char *serial_name2;
    void *serial_rpc;
    
};

struct APMountClass {
    struct __TelescopeClass _;
};

struct ASCOMMount {
    struct __Telescope _;
    void *ascom;
    bool can_slew_async;
    bool can_slew_sync;
    bool can_park;
};

struct ASCOMMountClass {
    struct __TelescopeClass _;
};

struct SYSU80 {
    struct __Telescope _;
    char *address; /* Windows server IP */
    char *port; /* Windows server port, default is 51042 */
    char **instruments;
    size_t n_instrument;
    size_t default_instrument;
    size_t current_instrument;
};

struct SYSU80Class {
    struct __TelescopeClass _;
};
#endif /* telescope_r_h */
