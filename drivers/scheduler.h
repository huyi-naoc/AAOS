//
//  scheduler.h
//  AAOS
//
//  Created by Hu Yi on 2023/12/11.
//  Copyright © 2023 NAOC. All rights reserved.
//

#ifndef scheduler_h
#define scheduler_h

#include <stdarg.h>
#include <stdint.h>
#include <string.h>

#include "object.h"

#ifndef PI
#define PI 3.141592653589793
#endif

int __scheduler_init(void *_self);
int __scheduler_get_task_by_telescope_id(void *_self, uint64_t identifier, char *result, size_t size, size_t *length, unsigned int *type);
int __scheduler_get_task_by_telescope_name(void *_self, const char *name, char *result, size_t size, size_t *length, unsigned int *type);
int __scheduler_pop_task_block(void *_self);
int __scheduler_push_task_block(void *_self, const char *block_buf, unsigned int type);

int __scheduler_update_status(void *_self, const char *string, unsigned int type);

int __scheduler_list_site(void *_self, char *buf, size_t size, unsigned int *type);
int __scheduler_add_site(void *_self, const char *info, unsigned int type);
int __scheduler_delete_site_by_id(void *_self, uint64_t identifier);
int __scheduler_delete_site_by_name(void *_self, const char *name);
int __scheduler_mask_site_by_id(void *_self, uint64_t identifier);
int __scheduler_mask_site_by_name(void *_self, const char *name);
int __scheduler_unmask_site_by_id(void *_self, uint64_t identifier);
int __scheduler_unmask_site_by_name(void *_self, const char *name);

int __scheduler_list_telescope(void *_self, char *buf, size_t size, unsigned int *type);
int __scheduler_add_telescope(void *_self, const char *info, unsigned int type);
int __scheduler_delete_telescope_by_id(void *_self, uint64_t identifier);
int __scheduler_delete_telescope_by_name(void *_self, const char *name);
int __scheduler_mask_telescope_by_id(void *_self, uint64_t identifier);
int __scheduler_mask_telescope_by_name(void *_self, const char *name);
int __scheduler_unmask_telescope_by_id(void *_self, uint64_t identifier);
int __scheduler_unmask_telescope_by_name(void *_self, const char *name);

int __scheduler_list_target(void *_self, char *buf, size_t size, unsigned int *type);
int __scheduler_add_target(void *_self, const char *info, unsigned int type);
int __scheduler_delete_target_by_id(void *_self, uint64_t identifier, uint32_t nside);
int __scheduler_delete_target_by_name(void *_self, const char *name);
int __scheduler_mask_target_by_id(void *_self, uint64_t identifier, uint32_t nside);
int __scheduler_mask_target_by_name(void *_self, const char *name);
int __scheduler_unmask_target_by_id(void *_self, uint64_t identifier, uint32_t nside);
int __scheduler_unmask_target_by_name(void *_self, const char *name);

int __scheduler_add_task_record(void *_self, int status, const char *info, unsigned int type);
int __scheduler_update_task_record(void *_self, uint64_t identifier, const char *info);

void __scheduler_set_member(void *self, const char *name, ...);

int __scheduler_register_thread(void *_self, uint64_t identifier, void *thread);

void *__scheduler_site_manage_thr(void *_self);
void *__scheduler_telescope_manage_thr(void *_self);



extern const void *__Scheduler(void);
extern const void *__SchedulerClass(void);
#endif /* scheduler_h */
