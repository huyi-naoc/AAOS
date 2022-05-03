//
//  daemon_r.h
//  AAOS
//
//  Created by Hu Yi on 2019/6/28.
//  Copyright © 2019 National Astronomical Observatories, Chinese Academy of Sciences. All rights reserved.
//

#ifndef daemon_r_h
#define daemon_r_h

#include "def.h"
#include "object_r.h"

#define _DAEMON_PRIORITY_ 102

struct Daemon {
    const struct Object _;
    bool is_daemonized;
    char *daemon_name;
    char *root_directory;
    char *lock_file;
    char *username;
    
};

struct DaemonClass {
    const struct Class _;
    struct Method start;
    struct Method stop;
    struct Method restart;
    struct Method reload;
    struct Method set;
};

#endif /* daemon_r_h */
