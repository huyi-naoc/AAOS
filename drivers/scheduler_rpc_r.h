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
};

struct SchedulerClass {
    struct RPCClass _;
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

    struct Method register_thread;
    struct Method add_task_record;
    struct Method update_task_record;
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

#endif /* scheduler_rpc_r_h */
