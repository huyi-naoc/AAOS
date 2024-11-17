//
//  utils.h
//  AAOS
//
//  Created by huyi on 2024/11/13.
//

#ifndef utils_h
#define utils_h

#include <stdlib.h>
#include <time.h>

void ct_to_iso_str(char *buf, size_t size);
double iso_str_to_t(const char *buf);
void iso_str_to_tp(const char *buf, struct timespec *tp);

#endif /* utils_h */
