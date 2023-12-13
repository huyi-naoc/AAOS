//
//  schedule.h
//  AAOS
//
//  Created by Hu Yi on 2023/12/11.
//  Copyright © 2023 NAOC. All rights reserved.
//

#ifndef schedule_h
#define schedule_h
/**
 *  Get object to observe.
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
scheduler_get_object_by_id(void *_self, int identifier, char *result, size_t size, size_t *length, int *type);

int
scheduler_get_object_by_name(void *_self, const char *name, char *result, size_t size, size_t *length, int *type);

int
scheduler_list_telescope(void *_self, char *result, size_t size, size_t length length, int *type);

int
scheduler_add_telescope(void *_self, ...);

int
scheduler_delete_telescope_by_name(void *_self, int identifier);

int
scheduler_delete_telescope_by_id(void *_self, const char *name);

int
scheduler_bind_target_to_telescope_by_id(void *_self, int target_id, int identifier);

int
scheduler_unbind_target_to_telescope_by_name(void *_self, int target_id, const char *name);

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

int
scheduler_add_target(void *_self, const char *target, int type);

int
scheduler_delete_target(void *_self, int target_id);

int
scheduler_list_target(void *_self, char *result, size_t size, size_t *length, int *type);
#endif /* schedule_h */
