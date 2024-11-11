
//
//  scheduler_r.h
//  AAOS
//
//  Created by huyi on 2024/11/6.
//  Copyright © 2024 NAOC. All rights reserved.
//

#ifndef scheduler_r_h
#define scheduler_r_h

#include "object_r.h"

#include <stdint.h>
#include <string.h>
#include <pthread.h>

struct __Scheduler {
    struct Object _;
    
    unsigned int type;
    bool standalone;
    char *description;

    char *db_host;
    char *db_user;
    char *db_passwd;
    char *db_name;
    char *site_db_table;
    char *telescope_db_table;
    char *target_db_table;
    char *task_db_table;

    void *site_list;
    void *telescope_list;
    void *target_list;

    char *global_addr;
    char *global_port;
    void *site;

    char *site_addr;
    char *site_port;
    void *telescope;

    char *ipc_model;
    char *algorithm;
    char *algorithm_standalone;
    char *sock_file;

    uint64_t max_site_id;
    uint64_t max_telescope_id;
    uint64_t max_task_id;
    pthread_mutex_t cnt_mtx;
    
    size_t max_task_in_block;
};

struct __SchedulerClass {
    struct Class _;
    struct Method init;
    struct Method set_member;
    /*
     * scheduling method.
     */
    struct Method get_task_by_telescope_id;     /* site/unit scheduler. */  
    struct Method get_task_by_telescope_name;   /* site/unit scheduler. */   
    struct Method pop_task_block;               /* global scheduler. */
    struct Method push_task_block;              /* site scheduler. */

    struct Method update_status;
     /*
      * management method.
      */
    struct Method list_site;                    /* global/site scheduler. */
    struct Method add_site;
    struct Method delete_site_by_id;
    struct Method delete_site_by_name;
    struct Method mask_site_by_id;
    struct Method mask_site_by_name;
    struct Method unmask_site_by_id;
    struct Method unmask_site_by_name;

    struct Method list_telescope;               /* global/site scheduler. */
    struct Method add_telescope;
    struct Method delete_telescope_by_id;
    struct Method delete_telescope_by_name;
    struct Method mask_telescope_by_id;
    struct Method mask_telescope_by_name;
    struct Method unmask_telescope_by_id;
    struct Method unmask_telescope_by_name;

    struct Method list_target;                  /* global/site scheduler. */
    struct Method add_target;
    struct Method delete_target_by_id;
    struct Method delete_target_by_name;
    struct Method mask_target_by_id;
    struct Method mask_target_by_name;
    struct Method unmask_target_by_id;
    struct Method unmask_target_by_name;

    struct Method add_task_record;
    struct Method update_task_record;

    struct Method site_manage_thr;
    struct Method telescope_manage_thr;
    
};

#endif /* scheduler_r_h */