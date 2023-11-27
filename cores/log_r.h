//
//  log_r.h
//  AAOS
//
//  Created by Hu Yi on 2019/5/24.
//  Copyright © 2019 National Astronomical Observatories, Chinese Academy of Sciences. All rights reserved.
//

#ifndef log_r_h
#define log_r_h

#include "object_r.h"
#include <pthread.h>

#include "rpc_r.h"

#define _LOG_RPC_PRIORITY_ _RPC_PRIORITY_ + 1

#define MAX_LEVEL_LENGTH    16

struct __Log {
    const struct Object _;
    pthread_mutex_t mtx;
    int fds[MAX_LEVEL_LENGTH];
    char *facility;
    char *work_directory;
};

struct __LogClass {
    const struct Class _;
    struct Method write;
    struct Method get_facility;
};

#endif /* log_r_h */
