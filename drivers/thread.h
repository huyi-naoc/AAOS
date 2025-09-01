//
//  thread.h
//  AAOS
//
//  Created by huyi on 2024/11/15.
//

#ifndef thread_h
#define thread_h

#define OT_STATE_SLEW       0x0001
#define OT_STATE_EXPOSE     0x0002
#define OT_STATE_SUSPEND    0x0004
#define OT_STATE_CANCEL     0x0008
#define OT_STATE_STOP       0x0010
#define OT_STATE_IDLE       0x8000

#ifdef __cplusplus
extern "C" {
#endif

int __observation_thread_cycle(void *_sef);
int __observation_thread_start(void *_self);
int __observation_thread_stop(void *_self);
int __observation_thread_suspend(void *_self);
int __observation_thread_cancel(void *_self);
int __observation_thread_resume(void *_self);

const void *__ObservationThread(void);
const void *__ObservationThreadClass(void);

#ifdef __cplusplus
}
#endif
#endif /* thread_h */
