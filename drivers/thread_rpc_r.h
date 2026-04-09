//
//  thread_rpc_r.h
//  AAOS
//
//  Created by Hu Yi on 2026/2/21.
//

#ifndef thread_rpc_r_h
#define thread_rpc_r_h

#include "rpc_r.h"

struct ObservationThread {
    struct RPC _;
};

struct ObservationThreadClass {
    struct RPCClass _;
    struct Method get_index_by_name;
    struct Method start;
    struct Method suspend;
    struct Method stop;
    struct Method cancel;
    struct Method resume;
    struct Method terminate;
};

struct ObservationThreadClient {
    struct RPCClient _;
};

struct ObservationThreadClientClass {
    struct RPCClientClass _;
};

struct ObservationThreadServer {
    struct RPCServer _;
};

struct ObservationThreadServerClass {
    struct RPCServerClass _;
};

#endif /* thread_rpc_r_h */
