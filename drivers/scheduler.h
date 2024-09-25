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

#define SCHEDULER_GET_TASK_BY_ID                    1
#define SCHEDULER_GET_TASK_BY_NAME                  2
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

#define SCHEDULER_PUSH_TARGET_BLOCK                 30


#define SCHEDULER_FORMAT_JSON           1

#define SCHEDULER_TYPE_GLOBAL                       1
#define SCHEDULER_TYPE_SITE                         2
#define SCHEDULER_TYPE_UNIT                         3


struct SchedulerInfo {
    char *addr;
    char *port;
    char *name;
    void *scheduler;
}; 


/**
 *  Get task to observe.
 *  @param[in,out] _self scheduler object.
 *  @param[in] identifier telescope identifier.
 *  @param[in, out] result schedule result, should allocate enough memory before calling this function.
 *  @param[in] size the size of result.
 *  @param[out] length the length of result, can be NULL.
 *  @param[out] type the type of result, can be STR_INTPR_JSON, STR_INTPR_LIBCONFIG, STR_INTPR_XML, STR_INTPR_INI, STR_INTPR_PLAIN.
 *  @retval AAOS_OK
 *  No errors.
 *  @retval AAOS_NOTFOUND
 *  The entry for \b identifier does not found.
 */
int 
scheduler_get_task_by_id(void *_self, int identifier, char *result, size_t size, size_t *length, int *type);

/**
 *  Get task to observe.
 *  @param[in,out] _self scheduler object.
 *  @param[in] name telescope name.
 *  @param[in,out] result schedule result, should allocate enough memory before calling this function.
 *  @param[in] size the size of result.
 *  @param[out] length the length of result, can be NULL.
 *  @param[out] type the type of result, can be STR_INTPR_JSON, STR_INTPR_LIBCONFIG, STR_INTPR_XML, STR_INTPR_INI, STR_INTPR_PLAIN.
 *  @retval AAOS_OK
 *  No errors.
 *  @retval AAOS_NOTFOUND
 *  The entry for \b name does not found.
 */
int 
scheduler_get_task_by_name(void *_self, const char *name, char *result, size_t size, size_t *length, int *type);

/**
 *  List telescope scheduled by the scheduler. If the scheduler is a global scheduler, list all the telescope in the array. If the scheudler is a site scheduler, 
 *  list all the telecope on that site. If the scheduler is the unit/node scheduler , list the unit/node.
 *  @param[in,out] _self scheduler object.
 *  @param[in,out] result result that contains telescope list.
 *  @param[in] size the size of result.
 *  @param[out] length the length of result, can be NULL.
 *  @param[out] type the type of result, can be STR_INTPR_JSON, STR_INTPR_LIBCONFIG, STR_INTPR_XML, STR_INTPR_INI, STR_INTPR_PLAIN.
 *  @retval AAOS_OK
 *  No errors.
 */
int 
scheduler_list_telescope(void *_self, char *result, size_t size, size_t length, int *type);

int 
scheduler_add_telescope(void *_self, ...);

int 
scheduler_delete_telescope_by_id(void *_self, int identifier);

int 
scheduler_delete_telescope_by_name(void *_self, const char *name);

int 
scheduler_bind_target_to_telescope_by_id(void *_self, int target_id, int identifier);

int
scheduler_unbind_target_to_telescope_by_name(void *_self, int target_id, const char *name);

/*
int
scheduler_get_all_statistics_by_id(void *_self, int identifier, char *result, size_t size, size_t *length, int *type);

int
scheduler_get_all_statistics_by_name(void *_self, const char *name, char *result, size_t size, size_t *length, int *type);

int
scheduler_get_target_all_statistics(void *_self, int target_id, char *result, size_t size, size_t *length, int *type);

int
scheduler_get_target_statistics_by_id(void *_self, int target_id, int identifier, char *result, size_t size, size_t *length, int *type);

int
scheduler_get_target_statistics_by_name(void *_self, int target_id, const char *name, char *result, size_t size, size_t *length, int *type);

int
scheduler_get_parameter(void *_self, int parameter, ...);

int
scheduler_set_parameter(void *_self, int parameter, ...);
*/

int scheduler_add_target(void *_self, ...);
int scheduler_delete_target(void *_self, ...);
int scheduler_mask_target(void *_self, ...);
int scheduler_unmask_target(void *_self, ...);

int scheduler_delete_target_by_id(void *_self, int identifier);
int scheduler_mask_target_by_id(void *_self, int identifier);
int scheduler_unmask_target_by_id(void *_self, int identifier);
int scheduler_delete_target_by_name(void *_self, const char *name);
int scheduler_mask_target_by_name(void *_self, const char *name);
int scheduler_unmask_target_by_name(void *_self, const char *name);

int scheduler_push_target_block_by_name(void *_self, const char *name, const char *target_block, int type);
//int scheduler_list_target(void *_self, char *result, size_t size, size_t *length, int *type);



extern const void *Scheduler(void);
extern const void *SchedulerClass(void);


extern const void *SchedulerClient(void);
extern const void *SchedulerClassClientClass(void);

extern const void *SchedulerServer(void);
extern const void *SchedulerServerClass(void);
#endif /* schedule_h */
