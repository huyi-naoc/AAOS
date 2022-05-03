//
//  aws_rpc_r.h
//  AAOS
//
//  Created by Hu Yi on 2020/1/16.
//  Copyright © 2020 NAOC. All rights reserved.
//

#ifndef aws_rpc_r_h
#define aws_rpc_r_h

#include "rpc_r.h"

#define _AWS_RPC_PRIORITY_ _RPC_PRIORITY_ + 1

struct AWS {
    struct RPC _;
};

struct AWSClass {
    struct RPCClass _;
    struct Method get_index_by_name;
    struct Method get_channel_by_name;
    struct Method get_temperature;
    struct Method get_wind_speed;
    struct Method get_wind_direction;
    struct Method get_air_pressure;
    struct Method get_relative_humidity;
    struct Method get_sky_quality;
    struct Method get_precipitation;
    struct Method get_data;
    struct Method get_raw_data;
    struct Method status;
    struct Method data_log;
    struct Method data_field;
    struct Method reg;
    struct Method inspect;
};

struct AWSClient {
    struct RPCClient _;
};

struct AWSClientClass {
    struct RPCClientClass _;
};

struct AWSServer {
    struct RPCServer _;
};

struct AWSServerClass {
    struct RPCServerClass _;
};

#endif /* aws_rpc_r_h */
