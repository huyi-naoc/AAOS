//
//  utils.c
//  AAOS
//
//  Created by huyi on 2024/11/13.
//
#include <stdio.h>
#include <string.h>
#include "utils.h"


void
ct_to_iso_str(char *buf, size_t size)
{
    struct timespec tp;
    struct tm time_buf;
    long usec;

    clock_gettime(CLOCK_REALTIME, &tp);

    gmtime_r(&tp.tv_sec, &time_buf);
    usec = (long) (tp.tv_nsec / 1000);

    strftime(buf, size, "%Y-%m-%dT%H:%M:%S", &time_buf);
    buf += strlen(buf);
    size -= strlen(buf);

    snprintf(buf, size, ".%06ldZ", usec);
}

double
iso_str_to_t(const char *buf)
{
    struct tm time_buf;
    double timestamp = .0;

    size_t len = strlen(buf);

    strptime(buf, "%Y-%m-%dT%H:%M:%S", &time_buf);

    if (len > 8 && buf[len-1] == 'Z' && buf[len-5] == '.') {
        char tmp[8];
        memcpy(tmp, buf + len - 9, 6);
        tmp[6] = '\0';
        timestamp = atoi(tmp) / 1000000.;
    }

    timestamp += timegm(&time_buf);
    
    return timestamp;
}

void
iso_str_to_tp(const char *buf, struct timespec *tp)
{
    struct tm time_buf;
    long nsec = 0;
    size_t len = strlen(buf);

    strptime(buf, "%Y-%m-%dT%H:%M:%S", &time_buf);
    if (len > 8 && buf[len-1] == 'Z' && buf[len-5] == '.') {
        char tmp[8];
        memcpy(tmp, buf + len - 9, 6);
        tmp[6] = '\0';
        nsec = atoi(tmp) * 1000;
    }

    tp->tv_sec = timegm(&time_buf);
    tp->tv_nsec = nsec;
}
