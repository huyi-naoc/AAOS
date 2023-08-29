//
//  log.c
//  AAOS
//
//  Created by Hu Yi on 2019/5/24.
//  Copyright © 2019 National Astronomical Observatories, Chinese Academy of Sciences. All rights reserved.
//

#include "adt.h"
#include "def.h"
#include "log.h"
#include "log_def.h"
#include "wrapper.h"

void 
write_log_data_to_file(FILE *fp, const char *facility, struct LogData *log_data)
{
    struct tm tm_buf;
    char buf[COMMANDSIZE];

    gmtime_r(log_data->tp.tv_sec, &tm_buf);
    strftime(buf, COMMANDSIZE, "%Y-%m-%d %H:%M:%S", &time_buf);
    fprintf(fp, "%s -- %s -- %s\n", facility, buf, log_data->data);
    fflush(fp);
}
