//
//  log_rpc_r.h
//  AAOS
//
//  Created by huyi on 2025/12/23.
//

#ifndef log_rpc_r_h
#define log_rpc_r_h

#include "rpc_r.h"

struct Log {
    struct RPC _;
    void *queue;
    size_t size;
};

struct LogClass {
    struct RPCClass _;
    struct Method commit;
};


#endif /* log_rpc_r_h */
