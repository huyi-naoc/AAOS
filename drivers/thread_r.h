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

#include <pthread.h>

struct __ObservationThread {
    struct Object _;

    uint64_t tel_id;

    char *scheduler_addr;
    char *scheduler_port;
    char *scheduler;
    char *scheduler2;
    void *scheduler_client;
    bool has_scheduler;
    
    char *dome_addr;
    char *dome_port;
    char *dome;
    void *dome_client;
    bool has_dome;
    
    char *telescope_addr;
    char *telescope_port;
    char *telescope;
    void *telescope_client;
    bool has_telescope;
    
    char *detector_addr;
    char *detector_port;
    char *detector;
    void *detector_client;
    bool has_detector;
    
    char *pipeline_addr;
    char *pipeline_port;
    char *pipeline;
    void *pipeline_client;
    bool has_pipeline;
    
    unsigned int state;
    pthread_mutex_t mtx;
    pthread_cond_t cond;

    pthread_t tid;
};


struct __ObservationThreadClass {
    struct Class _;
    
    struct Method start;
    struct Method suspend;
    struct Method stop;
    struct Method cancel;
    struct Method resume;
    struct Method set_member;
    struct Method get_member;

    struct Method cycle;
}
#endif /* thread_r_h */
