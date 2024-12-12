//
//  telescope.h
//  AAOS
//
//  Created by huyi on 18/6/12.
//  Copyright © 2018年 National Astronomical Observatories, Chinese Academy of Sciences. All rights reserved.
//

#ifndef __telescope_h
#define __telescope_h

#include <stdint.h>
#include <string.h>

#include "telescope_def.h"
#include "object.h"

int __telescope_release(void *_self);

int __telescope_status(void *_self, char *status_buffer, size_t status_buffer_size);
int __telescope_power_on(void *_self);
int __telescope_power_off(void *_self);
int __telescope_init(void *_self);
int __telescope_park(void *_self);
int __telescope_park_off(void *_self);
int __telescope_stop(void *_self);
int __telescope_go_home(void *_self);
int __telescope_move(void *_self, unsigned int direction, double duration);
int __telescope_try_move(void *_self, unsigned int direction, double duration);
int __telescope_timed_move(void *_self, unsigned int direction, double duration, double timeout);
int __telescope_slew(void *_self, double ra, double dec);
int __telescope_try_slew(void *_self, double ra, double dec);
int __telescope_timed_slew(void *_self, double ra, double dec, double timeout);
int __telescope_raw(void *_self, const void *raw_command, size_t size, size_t *write_size, void *results, size_t results_size, size_t *return_size);
int __telescope_set_move_speed(void *_self, double move_speed);
int __telescope_get_move_speed(void *_self, double *move_speed);
int __telescope_set_slew_speed(void *_self, double slew_speed_x, double slew_speed_y);
int __telescope_get_slew_speed(void *_self, double *slew_speed_x, double *slew_speed_y);
int __telescope_set_track_rate(void *_self, double track_rate_x, double track_rate_y);
int __telescope_get_track_rate(void *_self, double *track_rate_x, double *track_rate_y);
int __telescope_set_option(void *_self, uint16_t option);
int __telescope_inspect(void *_self);
int __telescope_wait(void *_self, double timeout);
int __telescope_switch_instrument(void *_self, const char *name);
int __telescope_switch_filter(void *_self, const char *name);
int __telescope_switch_detector(void *_self, const char *name);

const char *__telescope_get_name(const void *_self);

extern const void *__Telescope(void);
extern const void *__TelescopeClass(void);
extern const void *__TelescopeVirtualTable(void);
extern const void *__TelescopeVirtualTableClass(void);

extern const void *VirtualTelescope(void);
extern const void *VirtualTelescopeClass(void);

extern const void *APMount(void);
extern const void *APMountClass(void);

extern const void *ASCOMMount(void);
extern const void *ASCOMMountClass(void);

#endif /* __telescope_h */
