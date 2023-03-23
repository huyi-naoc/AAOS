//
//  thermal_rpc_r.h
//  AAOS
//
//  Created by Hu Yi on 2023/3/22.
//  Copyright © 2023 NAOC. All rights reserved.
//

#ifndef thermal_rpc_r_h
#define thermal_rpc_r_h

#include "rpc_r.h"

struct ThermalUnit {
    struct RPC _;
};

struct ThermalUnitClass {
    struct RPCClass _;
    struct Method get_index_by_name;
    
    struct Method status;
    struct Method info;
};

struct ThermalUnitClient {
    struct RPCClient _;
};

struct ThermalUnitClientClass {
    struct RPCClientClass _;
};

struct ThermalUnitServer {
    struct RPCServer _;
};

struct ThermalUnitServerClass {
    struct RPCServerClass _;
};

#endif /* thermal_rpc_r_h */
