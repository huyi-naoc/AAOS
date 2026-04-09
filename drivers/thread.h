//
//  thread.h
//  AAOS
//
//  Created by huyi on 2024/11/15.
//

#ifndef thread_h
#define thread_h

#define OT_STATE_OPEN_DOME  0x0001
#define OT_STATE_SLEW       0x0002
#define OT_STATE_EXPOSE     0x0004
#define OT_STATE_SUSPEND    0x0008
#define OT_STATE_CANCEL     0x0010
#define OT_STATE_STOP       0x0020
#define OT_STATE_TERMINATE  0x0040
#define OT_STATE_IDLE       0x8000


#define OT_FLAG_L0          0x0001
#define OT_FLAG_L1          0x0002
#define OT_FLAG_BAD_WEATHER 0x0004
#define OT_FLAG_TEST        0x0008

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int __observation_thread_cycle(void *_sef);
int __observation_thread_start(void *_self);
int __observation_thread_terminate(void *_self);
int __observation_thread_stop(void *_self, uint32_t flag);
int __observation_thread_suspend(void *_self, uint32_t flag);
int __observation_thread_cancel(void *_self, uint32_t flag);
int __observation_thread_resume(void *_self);
void __observation_thread_set_member(void *_self, const char *, ...);
void __observation_thread_get_member(void *_self, const char *, ...);
const char *__observation_thread_get_name(void *_self);

const void *__ObservationThread(void);
const void *__ObservationThreadClass(void);

#ifdef __cplusplus
}
#endif
#endif /* thread_h */
