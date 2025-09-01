//
//  dome_rpc_r.h
//  AAOS
//
//  Created by huyi on 2025/4/30.
//

#ifndef dome_rpc_r_h
#define dome_rpc_r_h

#include "rpc_r.h"

struct Dome {
    struct RPC _;
};

struct DomeClass {
    struct RPCClass _;
    struct Method get_index_by_name;
	struct Method get_name_by_index;
    struct Method init;
    struct Method open;
    struct Method close;
    struct Method stop;
    struct Method status;
    struct Method raw;
    struct Method get_open_speed;
    struct Method set_open_speed;
    struct Method get_close_speed;
    struct Method set_close_speed;
    struct Method get_position;
    struct Method reg;
    struct Method inspect;
};

struct DomeClient {
    struct RPCClient _;
};

struct DomeClientClass {
    struct RPCClientClass _;
};

struct DomeServer {
    struct RPCServer _;
};

struct DomeServerClass {
    struct RPCServerClass _;
};

#endif /* dome_rpc_r_h */
