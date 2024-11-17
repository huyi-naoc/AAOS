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

#include "wrapper.h"

const void *__ObservationThread(void*);
const void *__ObservationThreadClass(void*);
#endif /* thread_h */
