//
//  daemon.h
//  AAOS
//
//  Created by Hu Yi on 2019/6/28.
//  Copyright © 2019 National Astronomical Observatories, Chinese Academy of Sciences. All rights reserved.
//

#ifndef daemon_h
#define daemon_h

#include <stdarg.h>
#include "def.h"
#include "object.h"

#define DAEMON_USERNAME_FIELD   1
#define DAEMON_ROOTDIR_FIELD    2
#define DAEMON_LOCKFILE_FIELD   3
#define DAEMON_NAME_FIELD       4
#define DAEMON_DAEMONIZE_FIELD  5

void daemon_set(void *_self, unsigned int field, ...);
void daemon_start(const void *_self);
void daemon_stop(const void *_self);
void daemon_reload(const void *_self);
void daemon_reload(const void *_self);

extern const void *Daemon(void);
extern const void *DaemonClass(void);

#endif /* daemon_h */
