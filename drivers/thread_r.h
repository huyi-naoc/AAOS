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

    

    char *name;
    char *description;

    char *scheduler_addr;
    char *scheduler_port;
    void *scheduler_client;
    void *scheduler;        /* For requesting observation tasks */
    void *scheduler2;       /* For registering a thread to the site scheduling system */
    bool has_scheduler;
    
    char *dome_addr;
    char *dome_port;
    char *dome_name;
    void *dome_client;
    void *dome;
    uint16_t dome_index;
    uint64_t dome_identifier;
    bool has_dome;
    
    char *telescope_addr;
    char *telescope_port;
    char *telescope_name;
    void *telescope_client;
    uint16_t telescope_index;
    uint64_t telescope_identifier;
    void *telescope;
    bool has_telescope;
    
    char *detector_addr;
    char *detector_port;
    char *detector_name;
    void *detector_client;
    void *detector;
    bool has_detector;
    
    char *pipeline_addr;
    char *pipeline_port;
    void *pipeline_client;
    void *pipeline;
    bool has_pipeline;

    char *aws_addr;
    char *aws_port;
    char *aws_name;
    void *aws_client;
    void *aws;
    char **aws_keyname;
    char **aws_keyvalue;
    size_t n_aws_keypair;
    bool has_aws;
    
    pthread_rwlock_t scheduler_rwlock;
    pthread_rwlock_t dome_rwlock;
    pthread_rwlock_t telescope_rwlock;
    pthread_rwlock_t detector_rwlock;
    pthread_rwlock_t aws_rwlock;
    pthread_rwlock_t pipeline_rwlock;


    unsigned int state;
    unsigned int flag;
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
    struct Method terminate;
    struct Method set_member;
    struct Method get_member;
    struct Method cycle;
    struct Method get_name;
};
#endif /* thread_r_h */
