//
//  serial_rpc_r.h
//  AAOS
//
//  Created by huyi on 2018/11/26.
//  Copyright © 2018 National Astronomical Observatories, Chinese Academy of Sciences. All rights reserved.
//

#ifndef serial_rpc_r_h
#define serial_rpc_r_h

#include "rpc_r.h"

#define _SERIAL_RPC_PRIORITY_ _RPC_PRIORITY_ + 1

struct Serial {
    struct RPC _;
};

struct SerialClass {
    struct RPCClass _;
    struct Method get_index_by_name;
    struct Method get_index_by_path;
    struct Method raw;
    struct Method load;
    struct Method reload;
    struct Method unload;
    struct Method info;
    struct Method inspect;
    struct Method reg;
};

struct SerialClient {
    struct RPCClient _;
};

struct SerialClientClass {
    struct RPCClientClass _;
};

struct SerialServer {
    struct RPCServer _;
};

struct SerialServerClass {
    struct RPCServerClass _;
};
#endif /* serial_rpc_r_h */
