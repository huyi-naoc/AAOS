//
//  device_r.h
//  AAOS
//
//  Created by Hu Yi on 2022/7/23.
//  Copyright © 2022 NAOC. All rights reserved.
//

#ifndef device_r_h
#define device_r_h

#include "object_r.h"
#include "virtual_r.h"

#include <pthread.h>

struct Device {
    struct Object _;
    const void *_vtab;
    unsigned int state;
    pthread_mutex_t mtx;
    pthread_cond_t cond;
};

struct DeviceClass {
    struct Class _;
    struct Method inspect;
    struct Method wait;
};

struct DeviceVirtualTable {
    struct VirtualTable _;
    struct Method inspect;
    struct Method wait;
};
#endif /* device_r_h */
