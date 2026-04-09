//
//  thread_rpc.h
//  AAOS
//
//  Created by Hu Yi on 2026/2/21.
//

#ifndef thread_rpc_h
#define thread_rpc_h

#include <stdint.h>
#include "rpc.h"


#define OT_COMMAND_GET_INDEX_BY_NAME        1
#define OT_COMMAND_START                    2
#define OT_COMMAND_TERMINATE                3
#define OT_COMMAND_RESUME                   4
#define OT_COMMAND_SUSPEND                  5
#define OT_COMMAND_STOP                     6
#define OT_COMMAND_CANCEL                   7

#ifdef __cplusplus
extern "C" {
#endif

int observation_thread_get_index_by_name(void *_self, const char *name);
int observation_thread_start(void *_self);
int observation_thread_resume(void *_self);
int observation_thread_terminate(void *_self);
int observation_thread_cancel(void *_self, uint32_t flag);
int observation_thread_suspend(void *_self, uint32_t flag);
int observation_thread_stop(void *_self, uint32_t flag);

extern const void *ObservationThread(void);
extern const void *ObservationThreadClass(void);

extern const void *ObservationThreadClient(void);
extern const void *ObservationThreadClientClass(void);

extern const void *ObservationThreadServer(void);
extern const void *ObservationThreadServerClass(void);


#ifdef __cplusplus
}
#endif

#endif /* thread_rpc_h */
