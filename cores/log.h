//
//  log.h
//  AAOS
//
//  Created by Hu Yi on 2019/5/24.
//  Copyright © 2019 National Astronomical Observatories, Chinese Academy of Sciences. All rights reserved.
//

#ifndef log_h
#define log_h

struct LogData {
    unsigned int level;
    struct timespec tp;
    char data[];
};

void *log_queue;

void write_log_data_to_file(int fd, const char *facility, struct LogData *log_data);

#endif /* log_h */
