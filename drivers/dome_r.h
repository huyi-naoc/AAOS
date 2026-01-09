//
//  dome_r.h
//  AAOS
//
//  Created by huyi on 2025/4/27.
//

#ifndef dome_r_h
#define dome_r_h

#include "object_r.h"
#include "virtual_r.h"
#include <string.h>
#include <pthread.h>

struct DomeState {
    pthread_mutex_t mtx;
    pthread_cond_t cond;
    unsigned int state;
};

struct __Dome {
    struct Object _;
    const void *_vtab;
    char *name;
    char *description;
    struct DomeState d_state;
    double window_open_speed;
    double window_close_speed;
    double window_position;
    double slew_speed;
    double track_speed;
	bool slew_available;
    
};

struct __DomeClass {
    struct Class _;
    struct Method init;
    struct Method open_window;
    struct Method stop_window;
    struct Method close_window;
    struct Method status;
    struct Method raw;
    struct Method inspect;
    struct Method reg;
	struct Method get_window_position;
	struct Method get_window_open_speed;
	struct Method set_window_open_speed;
	struct Method get_window_close_speed;
	struct Method set_window_close_speed;
    struct Method go_home;
	struct Method slew;
	struct Method park;
	struct Method park_off;
    struct Method get_name;
};

struct __DomeVirtualTable {
    struct VirtualTable _;
    struct Method init;
    struct Method open_window;
    struct Method stop_window;
    struct Method close_window;
    struct Method status;
    struct Method raw;
    struct Method inspect;
    struct Method reg;
	struct Method get_window_position;
	struct Method get_window_open_speed;
	struct Method set_window_open_speed;
	struct Method get_window_close_speed;
	struct Method set_window_close_speed;
    struct Method go_home;
    struct Method slew;
	struct Method park;
	struct Method park_off;
};

struct VirtualDome {
	struct __Dome _;
	double last_window_open_time;
	double last_window_close_time;
	pthread_t tid;
};

struct VirtualDomeClass {
	struct __DomeClass _;
};

#endif /* dome_r_h */
