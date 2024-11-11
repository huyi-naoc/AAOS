//
//  scheduler_rpc.h
//  AAOS
//
//  Created by Hu Yi on 2021/11/26.
//  Copyright © 2021 NAOC. All rights reserved.
//

#ifndef scheduler_rpc_h
#define scheduler_rpc_h

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

int scheduler_get_task_by_telescope_id(void *_self, uint64_t identifier, char *result, size_t size, size_t *length, unsigned int *type);
int scheduler_get_task_by_telescope_name(void *_self, const char *name, char *result, size_t size, size_t *length, unsigned int *type);

int scheduler_update_status(void *_self, const char *string, unsigned int type);

int scheduler_list_site(void *_self, char *result, size_t size, size_t *length, unsigned int *type);
int scheduler_add_site(void *_self, const char *info, unsigned int type);
int scheduler_delete_site_by_id(void *_self, uint64_t identifier);
int scheduler_delete_site_by_name(void *_self, const char *name);
int scheduler_mask_site_by_id(void *_self, uint64_t identifier);
int scheduler_mask_site_by_name(void *_self, const char *name);
int scheduler_unmask_site_by_id(void *_self, uint64_t identifier);
int scheduler_unmask_site_by_name(void *_self, const char *name);


int scheduler_list_telescope(void *_self, char *result, size_t size, size_t *length, unsigned int *type);
int scheduler_add_telescope(void *_self, const char *info, unsigned int type);
int scheduler_delete_telescope_by_id(void *_self, uint64_t identifier);
int scheduler_delete_telescope_by_name(void *_self, const char *name);
int scheduler_mask_telescope_by_id(void *_self, uint64_t identifier);
int scheduler_mask_telescope_by_name(void *_self, const char *name);
int scheduler_unmask_telescope_by_id(void *_self, uint64_t identifier);
int scheduler_unmask_telescope_by_name(void *_self, const char *name);

int scheduler_list_target(void *_self, char *result, size_t size, size_t *length, unsigned int *type);
int scheduler_add_target(void *_self, const char *info, unsigned int type);
int scheduler_delete_target_by_id(void *_self, uint64_t identifier, uint32_t nside);
int scheduler_delete_target_by_name(void *_self, const char *name);
int scheduler_mask_target_by_id(void *_self, uint64_t identifier, uint32_t nside);
int scheduler_mask_target_by_name(void *_self, const char *name);
int scheduler_unmask_target_by_id(void *_self, uint64_t identifier, uint32_t nside);
int scheduler_unmask_target_by_name(void *_self, const char *name);

int scheduler_add_task_record(void *_self, const char *info, unsigned int type);
int scheduler_update_task_record(void *_self, uint64_t identifier, const char *info, unsigned int type);

extern const void *Scheduler(void);
extern const void *SchedulerClass(void);

extern const void *SchedulerClient(void);
extern const void *SchedulerClientClass(void);

extern const void *SchedulerServer(void);
extern const void *SchedulerServerClass(void);

extern void *scheduler;


#endif /* scheduler_rpc_h */
