#include "rpc_r.h"

struct Scheduler {
    struct RPC _;
    unsigned int type;
    void *global_list;
    void *site_list;
    void *unit_list;
};

struct SchedulerClass {
    struct RPCClass _;
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