//
//  schedule.h
//  AAOS
//
//  Created by Hu Yi on 2023/12/11.
//  Copyright © 2023 NAOC. All rights reserved.
//

#ifndef schedule_h
#define schedule_h
#include "rpc.h"

#define SCHEDULER_STATUS_OK     0
#define SCHEDULER_STATUS_DELETE 1
#define SCHEDULER_STATUS_MASKED 2

#define SCHEDULER_GET_TASK_BY_ID                    1
#define SCHEDULER_GET_TASK_BY_TELESCOPE_ID          1
#define SCHEDULER_GET_TASK_BY_NAME                  2
#define SCHEDULER_GET_TASK_BY_TELESCOPE_NAME        2
#define SCHEDULER_LIST_TELESCOPE                    3
#define SCHEDULER_ADD_TELESCOPE                     4
#define SCHEDULER_DELETE_TELESCOPE_BY_ID            5
#define SCHEDULER_DELETE_TELESCOPE_BY_NAME          6
#define SCHEDULER_MASK_TELESCOPE_BY_ID              7
#define SCHEDULER_MASK_TELESCOPE_BY_NAME            8
#define SCHEDULER_UNMASK_TELESCOPE_BY_ID            9
#define SCHEDULER_UNMASK_TELESCOPE_BY_NAME          10
#define SCHEDULER_ADD_TARGET                        11
#define SCHEDULER_DELETE_TARGET                     12
#define SCHEDULER_MASK_TARGET                       13
#define SCHEDULER_UNMASK_TARGET                     14
#define SCHEDULER_DELETE_TARGET_BY_ID               15
#define SCHEDULER_DELETE_TARGET_BY_NAME             16
#define SCHEDULER_MASK_TARGET_BY_ID                 17
#define SCHEDULER_MASK_TARGET_BY_NAME               18
#define SCHEDULER_UNMASK_TARGET_BY_ID               19
#define SCHEDULER_UNMASK_TARGET_BY_NAME             20
#define SCHEDULER_SET_TARGET_PRIORITY               21
#define SCHEDULER_ADD_SITE                          22
#define SCHEDULER_DELETE_SITE_BY_ID                 23
#define SCHEDULER_DELETE_SITE_BY_NAME               24
#define SCHEDULER_MASK_SITE_BY_ID                   25
#define SCHEDULER_MASK_SITE_BY_NAME                 26
#define SCHEDULER_UNMASK_SITE_BY_ID                 27
#define SCHEDULER_UNMASK_SITE_BY_NAME               28
#define SCHEDULER_ADD_TASK_RECORD                   29
#define SCHEDULER_UPDATE_TASK_STATUS                30

#define SCHEDULER_TASK_BLOCK_ACK                  38
#define SCHEDULER_POP_TASK_BLOCK                  39
#define SCHEDULER_PUSH_TASK_BLOCK                 40


#define SCHEDULER_FORMAT_JSON           1

#define SCHEDULER_TYPE_GLOBAL                       1
#define SCHEDULER_TYPE_SITE                         2
#define SCHEDULER_TYPE_UNIT                         3


extern const void *__Scheduler(void);
extern const void *__SchedulerClass(void);
#endif /* schedule_h */
