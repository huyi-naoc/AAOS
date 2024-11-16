//
//  thread_r.h
//  AAOS
//
//  Created by huyi on 2024/11/15.
//

#ifndef thread_r_h
#define thread_r_h

#include "object_r.h"
#include <stdbool.h>
#include <stdint.h>

struct __ObservationThread {
    struct Object _;

    uint64_t identifier;
    
    char *scheduler_addr;
    char *scheduler_port;
    char *scheduler;
    char *dome_addr;
    char *dome_port;
    void *dome;
    char *telescope_addr;
    char *telescope_port;
    char *telescope;
    char *detector_addr;
    char *detector_port;
    char *detector;
    char *pipeline_addr;
    char *pipeline_port;
    char *pipeline;
    bool has_scheduler;
    bool has_dome;
    bool has_telescope;
    bool has_detector;
};
#endif /* thread_r_h */
