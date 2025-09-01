//
//  utils.h
//  AAOS
//
//  Created by huyi on 2024/11/13.
//

#ifndef utils_h
#define utils_h

#include <stdlib.h>
#define _XOPEN_SOURCE
#define __USE_XOPEN
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif
void ct_to_iso_str(char *buf, size_t size);
double iso_str_to_t(const char *buf);
void iso_str_to_tp(const char *buf, struct timespec *tp);

#ifdef __cplusplus
}
#endif

#endif /* utils_h */
