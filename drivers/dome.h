//
//  dome.h
//  AAOS
//
//  Created by huyi on 2025/4/27.
//

#ifndef dome_h
#define dome_h

#include <string.h>
#include "object.h"
#include "virtual.h"

#ifdef __cplusplus
extern "C" {
#endif

int __dome_init(void *_self);
int __dome_open_window(void *_self);
int __dome_stop_window(void *_self);
int __dome_close_window(void *_self);
int __dome_inspect(void *_self);
int __dome_register(void *_self, double timeout);
int __dome_status(void *_self, void *status_buffer, size_t size, size_t *length);
int __dome_get_window_position(void *_self, double *position);
int __dome_get_window_open_speed(void *_self, double *speed);
int __dome_set_window_open_speed(void *_self, double speed);
int __dome_get_window_close_speed(void *_self, double *speed);
int __dome_set_window_close_speed(void *_self, double speed);
int __dome_raw(void *_self, const void *write_buffer, size_t write_buffer_size, size_t *write_size, void *read_buffer, size_t read_buffer_size, size_t *read_size);
const char *__dome_get_name(const void *_self);

extern const void *__Dome(void);
extern const void *__DomeClass(void);
extern const void *__DomeVirtualTable(void);

extern const void *VirtualDome(void);
extern const void *VirtualDomeClass(void);

#ifdef __cplusplus
}
#endif

#endif /* dome_h */
