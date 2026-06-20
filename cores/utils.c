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

void
parse_addr_port(const char *string, char *addr, size_t addr_size, char *port, size_t port_size, const char *default_addr, const char *default_port)
{
    char *s, *s2, *token, *token2;

    if (addr == NULL || port == NULL) {
        return;
    }

    memset(addr, '\0', addr_size);
    memset(port, '\0', port_size);

    if (string == NULL || strcmp(string, ":") == 0) {
        if (default_addr == NULL || default_port == NULL) {
            return;
        }
        snprintf(addr, addr_size, "%s", default_addr);
        snprintf(port, port_size, "%s", default_port);
    } else {
        s = (char *) malloc(strlen(string) + 1);
        snprintf(s, strlen(string) + 1, "%s", string);
        s2 = s;
        token = strsep(&s2, ":");
        token2 = strsep(&s2, ":");
        fprintf(stderr, "token: %s; token2: %s\n", token, token2);
        if (token == NULL) {
            if (default_addr == NULL || default_port == NULL) {
                return;
            }
            snprintf(addr, addr_size, "%s", default_addr);
            snprintf(port, port_size, "%s", default_port);
        } else {
            if (token2 == NULL) {
                if (default_port == NULL) {
                    return;
                }
                snprintf(addr, addr_size, "%s", token);
                snprintf(port, port_size, "%s", default_port);
            } else {
                if (strlen(token) == 0) {
                    if (default_addr == NULL) {
                        return;
                    }
                    snprintf(addr, addr_size, "%s", default_addr);
                    snprintf(port, port_size, "%s", token2);
                } else {
                    if (strlen(token2) == 0) {
                        if (default_port == NULL) {
                            return;
                        }
                        snprintf(addr, addr_size, "%s", token);
                        snprintf(port, port_size, "%s", default_port);
                    } else {
                        snprintf(addr, addr_size, "%s", token);
                        snprintf(port, port_size, "%s", token2);
                    }
                }
            }
        }
        free(s);
    }
}
