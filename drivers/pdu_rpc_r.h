//
//  pdu_rpc_r.h
//  AAOS
//
//  Created by Hu Yi on 2020/7/16.
//  Copyright © 2020 NAOC. All rights reserved.
//

#ifndef pdu_rpc_r_h
#define pdu_rpc_r_h

#define _PDU_RPC_PRIORITY_ _RPC_PRIORITY_ + 1

#include "rpc_r.h"

struct PDU {
    struct RPC _;
};

struct PDUClass {
    struct RPCClass _;
    struct Method get_index_by_name;
    struct Method get_channel_by_name;
    struct Method get_voltage;
    struct Method get_current;
    struct Method get_voltage_current;
    struct Method status;
    struct Method turn_on;
    struct Method turn_off;
};

struct PDUClient {
    struct RPCClient _;
};

struct PDUClientClass {
    struct RPCClientClass _;
};

struct PDUServer {
    struct RPCServer _;
};

struct PDUServerClass {
    struct RPCServerClass _;
};


#endif /* pdu_rpc_r_h */
