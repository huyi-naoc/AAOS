//
//  dome_rpc.h
//  AAOS
//
//  Created by huyi on 2025/4/29.
//

#ifndef dome_rpc_h
#define dome_rpc_h

#include "rpc.h"
#include <stddef.h>
#include <stdint.h>

#define DOME_COMMAND_GET_INDEX_BY_NAME          1
#define DOME_COMMAND_GET_NAME_BY_INDEX			2
#define DOME_COMMAND_INIT						3
#define DOME_COMMAND_OPEN						4
#define DOME_COMMAND_CLOSE						5
#define DOME_COMMAND_STOP						6
#define DOME_COMMAND_INSPECT					7
#define DOME_COMMAND_REGISTER					8
#define DOME_COMMAND_STATUS						9
#define DOME_COMMAND_GET_POSITION				10
#define DOME_COMMAND_GET_OPEN_SPEED				11
#define DOME_COMMAND_SET_OPEN_SPEED				12
#define DOME_COMMAND_GET_CLOSE_SPEED			13
#define DOME_COMMAND_SET_CLOSE_SPEED			14
#define DOME_COMMAND_RAW						15

#ifdef __cplusplus
extern "C" {
#endif

int dome_init(void *_self);
int dome_open(void *_self);
int dome_close(void *_self);
int dome_stop(void *_self);
int dome_inspect(void *_self);
int dome_register(void *_self, double timeout);
int dome_status(void *_self, void *status_buffer, size_t status_size, size_t *status_length);
int dome_get_position(void *_self, double *position);
int dome_get_open_speed(void *_self, double *speed);
int dome_set_open_speed(void *_self, double speed);
int dome_get_close_speed(void *_self, double *speed);
int dome_set_close_speed(void *_self, double speed);
int dome_raw(void *_self, const void *write_buffer, size_t write_buffer_size, size_t *write_size, void *read_buffer, size_t read_buffer_size, size_t *read_size);
int dome_get_index_by_name(void *_self, const char *name);
int dome_get_name_by_index(void *_self, uint16_t index, char *name, size_t size);

const void *Dome(void);
const void *DomeClass(void);

extern const void *DomeClient(void);
extern const void *DomeClientClass(void);

extern const void *DomeServer(void);
extern const void *DomeServerClass(void);

#ifdef __cplusplus
}
#endif

void **domes;
size_t n_dome;
void *dome_list;

#endif /* dome_rpc_h */
