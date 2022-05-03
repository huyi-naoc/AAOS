//
//  ascom_r.h
//  AAOS
//
//  Created by Hu Yi on 2019/11/15.
//  Copyright © 2019 NAOC. All rights reserved.
//

#ifndef ascom_r_h
#define ascom_r_h

#include "object_r.h"
#define _ASCOM_PRIORITY_ 102

struct ASCOM {
    struct Object _;
    char *address;
    char *port;
    char *version;
    char *device_type;
    unsigned int device_number;
};

struct ASCOMClass {
    struct Class _;
    struct Method get;
    struct Method put;
};


#endif /* ascom_r_h */
