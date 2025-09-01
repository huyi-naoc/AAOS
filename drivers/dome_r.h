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
	double open_speed;
	double close_speed;
	double position; /* in percent unit*/
};

struct __DomeClass {
    struct Class _;
    struct Method init;
    struct Method open;
    struct Method stop;
    struct Method close;
    struct Method status;
    struct Method raw;
    struct Method inspect;
    struct Method reg;
	struct Method get_position;
	struct Method get_open_speed;
	struct Method set_open_speed;
	struct Method get_close_speed;
	struct Method set_close_speed;
    struct Method get_name;
};

struct __DomeVirtualTable {
    struct VirtualTable _;
    struct Method init;
    struct Method open;
    struct Method stop;
    struct Method close;
    struct Method status;
    struct Method raw;
    struct Method inspect;
    struct Method reg;
	struct Method get_position;
	struct Method get_open_speed;
	struct Method set_open_speed;
	struct Method get_close_speed;
	struct Method set_close_speed;
};

struct VirtualDome {
	struct __Dome _;
	double last_open_time;
	double last_close_time;
	pthread_t tid;
};

struct VirtualDomeClass {
	struct __DomeClass _;
};

#endif /* dome_r_h */
