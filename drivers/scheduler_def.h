//
//  scheduler_def.h
//  AAOS
//
//  Created by huyi on 2024/11/8.
//  Copyright © 2024 NAOC. All rights reserved.
//

#ifndef scheduler_def_h
#define scheduler_def_h

#define SCHEDULER_STATUS_OK             0
#define SCHEDULER_STATUS_DELETE         1
#define SCHEDULER_STATUS_MASKED         2
#define SCHEDULER_STATUS_SUCCESS        0
#define SCHEDULER_STATUS_FAIL           1
#define SCHEDULER_STATUS_COMPLETE       2
#define SCHEDULER_STATUS_INCOMPLETE     3
#define SCHEDULER_STATUS_EXECUTE        4
#define SCHEDULER_STATUS_GENERATE       5

#define SCHEDULER_GET_TASK_BY_TELESCOPE_ID          1
#define SCHEDULER_GET_TASK_BY_TELESCOPE_NAME        2
#define SCHEDULER_LIST_TELESCOPE                    3
#define SCHEDULER_ADD_TELESCOPE                     4
#define SCHEDULER_DELETE_TELESCOPE_BY_ID            5
#define SCHEDULER_DELETE_TELESCOPE_BY_NAME          6
#define SCHEDULER_MASK_TELESCOPE_BY_ID              7
#define SCHEDULER_MASK_TELESCOPE_BY_NAME            8
#define SCHEDULER_UNMASK_TELESCOPE_BY_ID            9
#define SCHEDULER_UNMASK_TELESCOPE_BY_NAME          10
#define SCHEDULER_LIST_TARGET                       11
#define SCHEDULER_ADD_TARGET                        12
#define SCHEDULER_DELETE_TARGET                     13
#define SCHEDULER_MASK_TARGET                       14
#define SCHEDULER_UNMASK_TARGET                     15
#define SCHEDULER_DELETE_TARGET_BY_ID               16
#define SCHEDULER_DELETE_TARGET_BY_NAME             17
#define SCHEDULER_MASK_TARGET_BY_ID                 18
#define SCHEDULER_MASK_TARGET_BY_NAME               19
#define SCHEDULER_UNMASK_TARGET_BY_ID               20
#define SCHEDULER_UNMASK_TARGET_BY_NAME             21
#define SCHEDULER_SET_TARGET_PRIORITY               22
#define SCHEDULER_LIST_SITE                         23
#define SCHEDULER_ADD_SITE                          24
#define SCHEDULER_DELETE_SITE_BY_ID                 25
#define SCHEDULER_DELETE_SITE_BY_NAME               26
#define SCHEDULER_MASK_SITE_BY_ID                   27
#define SCHEDULER_MASK_SITE_BY_NAME                 28
#define SCHEDULER_UNMASK_SITE_BY_ID                 29
#define SCHEDULER_UNMASK_SITE_BY_NAME               30
#define SCHEDULER_ADD_TASK_RECORD                   31
#define SCHEDULER_UPDATE_TASK_STATUS                32

#define SCHEDULER_UPDATE_STATUS                     33
#define SCHEDULER_TASK_BLOCK_ACK                    38
#define SCHEDULER_POP_TASK_BLOCK                    39
#define SCHEDULER_PUSH_TASK_BLOCK                   40


#define SCHEDULER_UPDATE_TASK_RECORD                49
#define SCHEDULER_SITE_INIT                         50
#define SCHEDULER_TELESCOPE_INIT                    51
#define SCHEDULER_TARGET_INIT                       52
#define SCHEDULER_TASK_RECORD_INIT                  53

#define SCHEDULER_SITE_MANAGE                       54
#define SCHEDULER_TELESCOPE_MANAGE                  55


#define SCHEDULER_FORMAT_JSON           1

#define SCHEDULER_TYPE_GLOBAL                       1
#define SCHEDULER_TYPE_SITE                         2
#define SCHEDULER_TYPE_UNIT                         3
#define SCHEDULER_TYPE_UNKNOWN                      4


#endif /* scheduler_def_h */
