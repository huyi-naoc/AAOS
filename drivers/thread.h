//
//  thread.h
//  AAOS
//
//  Created by huyi on 2024/11/15.
//

#ifndef thread_h
#define thread_h

#define OT_STATE_SLEW       1
#define OT_STATE_EXPOSE     2
#define OT_STATE_SUSPEND    3
#define OT_STATE_CANCEL     4
#define OT_STATE_STOP       5
#define OT_STATE_IDLE       6


int __observation_thread_cycle(void *_sef);
int __observation_thread_start(void *_self);
int __observation_thread_stop(void *_self);
int __observation_thread_suspend(void *_self);
int __observation_thread_cancel(void *_self);
int __observation_thread_resume(void *_self);

const void *__ObservationThread(void);
const void *__ObservationThreadClass(void);
#endif /* thread_h */
