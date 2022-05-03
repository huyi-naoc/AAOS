//
//  telescope_rpc_r.h
//  AAOS
//
//  Created by Hu Yi on 2019/7/9.
//  Copyright © 2019 NAOC. All rights reserved.
//

#ifndef telescope_rpc_r_h
#define telescope_rpc_r_h

#include "rpc_r.h"

#define _TELESCOPE_RPC_PRIORITY_ _RPC_PRIORITY_ + 1

struct Telescope {
    struct RPC _;
};

struct TelescopeClass {
    struct RPCClass _;
    struct Method get_index_by_name;
    struct Method set_option;

    struct Method status;
    struct Method power_on;
    struct Method power_off;
    struct Method init;
    struct Method park;
    struct Method park_off;
    struct Method stop;
    struct Method go_home;
    struct Method move;
    struct Method try_move;
    struct Method timed_move;
    struct Method slew;
    struct Method try_slew;
    struct Method timed_slew;
    struct Method raw;
    struct Method release;
    struct Method set_slew_speed;
    struct Method get_slew_speed;
    struct Method set_move_speed;
    struct Method get_move_speed;
    struct Method set_track_rate;
    struct Method get_track_rate;
    
};

struct TelescopeClient {
    struct RPCClient _;
};

struct TelescopeClientClass {
    struct RPCClientClass _;
};

struct TelescopeServer {
    struct RPCServer _;
};

struct TelescopeServerClass {
    struct RPCServerClass _;
};

#endif /* telescope_rpc_r_h */
