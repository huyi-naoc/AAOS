//
//  scheduler_rpc_r.h
//  AAOS
//
//  Created by huyi on 2024/11/6.
//  Copyright © 2024 NAOC. All rights reserved.
//

#ifndef scheduler_rpc_r_h
#define scheduler_rpc_r_h

#include "rpc_r.h"

struct Scheduler {
    struct RPC _;
    unsigned int type;
    void *global_list;
    void *site_list;
    void *unit_list;
    char *db_host;
    char *db_name;
    char *db_password;
    char *site_info_table;
    char *unit_info_table;
    char *target_info_table;
    char *task_info_table;
    char *ipc_model;
    char *socket_file;
    char *algorithm;
    char *algorithm_path;
};

struct __SchedulerClass {
    struct RPCClass _;
    struct Method init;
    struct Method get_task_by_id;
    struct Method get_task_by_name;
    struct Method push_task_block;
    struct Method list_telescope;
    struct Method add_telescope;
    struct Method delete_telescope_by_id;
    struct Method mask_telescope_by_id;
    struct Method unmask_telescope_by_id;
    struct Method delete_telescope_by_name;
    struct Method mask_telescope_by_name;
    struct Method unmask_telescope_by_name; 
    struct Method add_target;
    struct Method delete_target;
    struct Method mask_target;
    struct Method unmask_target;
    struct Method delete_target_by_id;
    struct Method mask_target_by_id;
    struct Method unmask_target_by_id;
    struct Method delete_target_by_name;
    struct Method mask_target_by_name;
    struct Method unmask_target_by_name;
    struct Method report_task_status_by_id;
    struct Method get_task_status_by_id;
    struct Method start_observation_by_id;
    struct Method stop_observation_by_id;
    struct Method cancel_observation_by_id;
    struct Method resume_observation_by_id;
    struct Method end_observation_by_id;
    struct Method start_observation_by_name;
    struct Method stop_observation_by_name;
    struct Method cancel_observation_by_name;
    struct Method resume_observation_by_name;
    struct Method end_observation_by_name;
    struct Method stat_all;
    struct Method stat_by_telescope_id;
    struct Method stat_by_telescope_name;
    struct Method stat_by_target_id;
    struct Method stat_by_target_name;
    struct Method push_task_block;
    struct Method report_task_status;
    struct Method report_site_info;
};

struct SchedulerClient {
    struct RPCClient _;
};

struct SchedulerClientClass {
    struct RPCClientClass _;
};

struct SchedulerServer {
    struct RPCServer _;
};

struct SchedulerServerClass {
    struct RPCServerClass _;
};

struct __Scheduler {
    struct Object _;
    unsigned int type;
    size_t max_task_in_block;
    uint16_t max_site_id;
    uint16_t max_telescope_id;
    uint32_t max_task_id;
    uint16_t site_id;
    uint16_t telescope_id;

    char *db_host;
    char *db_name;
    char *db_password;
    char *site_info_table;
    char *unit_info_table;
    char *target_info_table;
    char *task_info_table;
    char *ipc_model;
    char *socket_file;
    char *algorithm;
    char *algorithm_path;

    void *global_list;
    void *site_list;
    void *telescope_list;
};


#endif /* scheduler_rpc_r_h */
