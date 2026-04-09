//
//  log_rpc_r.h
//  AAOS
//
//  Created by huyi on 2025/12/23.
//

#ifndef log_rpc_r_h
#define log_rpc_r_h

#include "rpc_r.h"
#include <stdlib.h>
#include <pthread.h>

struct Log {
    struct RPC _;
    void *queue;
    size_t size;
};

struct LogClass {
    struct RPCClass _;
    struct Method get_index_by_facility;
    struct Method submit;
};

struct LogClient {
    struct RPCClient _;
};

struct LogClientClass {
    struct RPCClientClass _;
};

struct LogServer {
    struct RPCServer _;
    void *queue;
    int efd;
    unsigned long long count;
    size_t n_thread;
    size_t n_event;
    pthread_t *tids;
    double timeout;
};

struct LogServerClass {
    struct RPCServerClass _;
};

#endif /* log_rpc_r_h */
