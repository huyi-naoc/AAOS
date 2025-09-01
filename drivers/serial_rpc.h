//
//  serial_rpc.h
//  AAOS
//
//  Created by huyi on 2018/11/26.
//  Copyright © 2018 National Astronomical Observatories, Chinese Academy of Sciences. All rights reserved.
//

#ifndef serial_rpc_h
#define serial_rpc_h

#include "rpc.h"

#define SERIAL_COMMAND_RAW 1
#define SERIAL_COMMAND_INFO 2
#define SERIAL_COMMAND_LOAD 3
#define SERIAL_COMMAND_RELOAD 4
#define SERIAL_COMMAND_UNLOAD 5
#define SERIAL_COMMAND_GET_INDEX_BY_NAME 6
#define SERIAL_COMMAND_GET_INDEX_BY_PATH 7
#define SERIAL_COMMAND_INSPECT           0xFFFE
#define SERIAL_COMMAND_REGISTER          0xFFFF

#define SERIAL_OPTION_BINARY    1

extern void **serials;
extern size_t n_serial;
extern void *serial_list;

#ifdef __cplusplus
extern "C" {
#endif

void start_feed_dog(double seconds);

int serial_raw(void *_self, const void *cmd, size_t cmd_size, void *res, size_t res_size, size_t *res_length);
int serial_info(void *_self, void *res, size_t res_size, size_t *res_length);
int serial_get_index_by_name(void *_self, const char *name);
int serial_get_index_by_path(void *_self, const char *path);
int serial_inspect(void *_self);
int serial_register(void *_self, double timeout);

extern const void *Serial(void);
extern const void *SerialClass(void);

extern const void *SerialClient(void);
extern const void *SerialClientClass(void);

extern const void *SerialServer(void);
extern const void *SerialServerClass(void);
#ifdef __cplusplus
}
#endif

#endif /* serial_rpc_h */
