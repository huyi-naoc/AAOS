//
//  detector_rpc_r.h
//  AAOS
//
//  Created by Hu Yi on 2021/11/16.
//  Copyright © 2021 NAOC. All rights reserved.
//

#ifndef detector_rpc_r_h
#define detector_rpc_r_h

#include "rpc_r.h"

struct Detector {
    struct RPC _;
};

struct DetectorClass {
    struct RPCClass _;
    struct Method abort;
    struct Method stop;
    struct Method status;
    struct Method info;
    struct Method init;
    struct Method power_on;
    struct Method power_off;
    struct Method expose;
    struct Method set_binning;
    struct Method get_binning;
    struct Method set_exposure_time;
    struct Method get_exposure_time;
    struct Method set_frame_rate;
    struct Method get_frame_rate;
    struct Method set_gain;
    struct Method get_gain;
    struct Method set_pixel_format;
    struct Method get_pixel_format;
    struct Method set_readout_rate;
    struct Method get_readout_rate;
    struct Method set_region;
    struct Method get_region;
    struct Method set_temperature;
    struct Method get_temperature;
    struct Method enable_cooling;
    struct Method disable_cooling;
    
    struct Method load;
    struct Method reload;
    struct Method unload;
    struct Method wait_for_completion;
    struct Method raw;
    
    struct Method set_prefix;
    struct Method get_prefix;
    struct Method set_directory;
    struct Method get_directory;
    struct Method set_template;
    struct Method get_template;
    struct Method set_option;
    struct Method get_option;
    struct Method clear_option;
    
    struct Method get_image;
    struct Method delete_image;
    struct Method delete_all_image;
    struct Method list_image;
    
    struct Method get_index_by_name;
};

struct DetectorClient {
    struct RPCClient _;
};

struct DetectorClientClass {
    struct RPCClientClass _;
};

struct DetectorServer {
    struct RPCServer _;
};

struct DetectorServerClass {
    struct RPCServerClass _;
};

#endif /* detector_rpc_r_h */
