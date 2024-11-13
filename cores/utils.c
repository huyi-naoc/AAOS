//
//  utils.c
//  AAOS
//
//  Created by huyi on 2024/11/13.
//

#include "utils.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

void
ct_to_iso_str(char *buf, size_t size)
{
    struct timespec tp;
    struct tm time_buf;
    long usec;

    clock_gettime(CLOCK_REALTIME, &tp);

    gmtime_r(&tp.tv_sec, &time_buf);
    usec = (long) (tp.tv_nsec / 1000000);

    strftime(buf, size, "%Y-%m-%dT%H:%M:%S", &time_buf);
    buf += strlen(buf);
    size -= strlen(buf);

    snprintf(buf, size, ".%03dZ", usec);
}

double
iso_str_to_t(const char *buf)
{
    struct tm time_buf;
    double timestamp;

    size_t len = strlen(buf);

    strptime(buf, "%Y-%m-%dT%H:%M:%S", &time_buf);

    if (len > 5 && buf[len-1] == 'Z' && buf[len-5] == '.') {
        char tmp[4];
        tmp[0] = buf[len-4];
        tmp[1] = buf[len-3];
        tmp[2] = buf[len-2];
        tmp[0] = '\0';

    }
    return timestamp;
}