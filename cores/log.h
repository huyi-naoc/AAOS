//
//  log.h
//  AAOS
//
//  Created by Hu Yi on 2019/5/24.
//  Copyright © 2019 National Astronomical Observatories, Chinese Academy of Sciences. All rights reserved.
//

#ifndef log_h
#define log_h

#include <time.h>

const char *__log_get_facility(void *_self);
int __log_write(void *_self, unsigned int level, const char *message);

extern const void *__Log(void);
extern const void *__LogClass(void);

#endif /* log_h */
