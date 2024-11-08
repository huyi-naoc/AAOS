//
//  scheduler_rpc.h
//  AAOS
//
//  Created by Hu Yi on 2021/11/26.
//  Copyright © 2021 NAOC. All rights reserved.
//

#ifndef scheduler_rpc_h
#define scheduler_rpc_h

/**
 * @file scheduler_rpc.h
 */

#include <stddef.h>
#include <stdint.h>

/**
 * Get a scheduling information unit (SIU) by the telescope's name.
 * @param[in] _self scheduler object.
 * @param[in] name telescope name.
 * @param[in] options options that controls the format of scheduling result.
 * @param[in,out] result buffer that stores scheduling result.
 * @param[in] size size of result.
 * @param[in,out] length length of result.
 * @retval AAOS_OK
 * No errors.
 * @retval AAOS_NOTFOUND
 * The entry for \b name does not found.
 */
int scheduler_request_by_name(void *_self, const char *name, unsigned int options, void *result, size_t size, size_t *length);

/**
 * Get a scheduling information unit (SIU) by the telescope's identifier.
 * @param[in] _self scheduler object.
 * @param[in] identifier telescope indentifier.
 * @param[in] options options that controls the format of scheduling result.
 * @param[in,out] result buffer that stores scheduling result.
 * @param[in] size size of result.
 * @param[in] length length of result.
 * @retval AAOS_OK
 * No errors.
 * @retval AAOS_NOTFOUND
 * The entry for \b identifier does not found.
 */
int scheduler_request_by_id(void *_self, uint32_t identifier, unsigned int options, void *result, size_t size, size_t *length);

/**
 * Add a telescope to the global scheduler.
 * @param[in] _self scheduler object.
 * @param[in] options options that specify the format of telescope.
 * @param[in] telescope telescope information.
 * @param[in] length length of \b telecope .
 * @retval AAOS_OK
 *  No errors.
 */
int scheduler_add_telescope(void *_self, unsigned int options, const void *telescope, size_t length);

/**
 * Delete a telescope to the global scheduler.
 * @param[in] _self scheduler object.
 * @param[in] name telescope name.
 * @retval AAOS_OK
 *  No errors.
 * @retval AAOS_NOTFOUND
 * The entry for \b name does not found.
 */
int scheduler_delete_telescope_by_name(void *_self, const char *name);

/**
 * Delete a telescope to the global scheduler.
 * @param[in] _self scheduler object.
 * @param[in] identifier telescope name.
 * @retval AAOS_OK
 *  No errors.
 * @retval AAOS_NOTFOUND
 * The entry for \b identifier does not found.
 */
int scheduler_delete_telescope_by_id(void *_self, uint32_t identifier);

/**
 * Mask a telescope to the site scheduler.
 * @param[in] _self scheduler object.
 * @param[in] name telescope name.
 * @retval AAOS_OK
 *  No errors.
 * @retval AAOS_NOTFOUND
 * The entry for \b name does not found.
 */
int scheduler_mask_telescope_by_name(void *_self, const char *name);

/**
 * Mask a telescope to the site scheduler.
 * @param[in] _self scheduler object.
 * @param[in] identifier telescope name.
 * @retval AAOS_OK
 *  No errors.
 * @retval AAOS_NOTFOUND
 * The entry for \b identifier does not found.
 */
int scheduler_mask_telescope_by_id(void *_self, uint32_t identifier);

/**
 * Unmask a telescope to the site scheduler.
 * @param[in] _self scheduler object.
 * @param[in] name telescope name.
 * @retval AAOS_OK
 *  No errors.
 * @retval AAOS_NOTFOUND
 * The entry for \b name does not found.
 */
int scheduler_unmask_telescope_by_name(void *_self, const char *name);

/**
 * Unmask a telescope to the site scheduler.
 * @param[in] _self scheduler object.
 * @param[in] identifier telescope name.
 * @retval AAOS_OK
 *  No errors.
 * @retval AAOS_NOTFOUND
 * The entry for \b identifier does not found.
 */
int scheduler_unmask_telescope_by_id(void *_self, uint32_t identifier);

/**
 * List all the telescopes scheduled by the scheduler.
 * @param[in] _self scheduler object.
 * @param[in,out] telescopes a list of telescopes, should be delete by the caller to prevent from memory leak.
 * @retval AAOS_OK
 * No errors.
 */
int scheduler_list_telescope(void *_self, void **telescopes);

/**
 * Add a target to the scheduler.
 * @param[in] _self  scheduler object.
 * @param[in] options scheduler options.
 * @param[in] target target information.
 * @param[in] length length of target.
 *
 */
int scheduler_add_target(void *_self, unsigned int options, const void *target, size_t length);
int scheduler_delete_target(void *_self, uint32_t indentifier);
int scheduler_mask_target(void *_self, uint32_t indentifier);
int scheduler_unmask_target(void *_self, uint32_t indentifier);
int scheduler_list_target(void *_self, void *targets);

int scheduler_report_task_status(void *_self, uint32_t status);
int scheduler_report_task_quality(void *_self, unsigned int options, const void *quality, size_t length);

int scheduler_create_thread(void *_self);
int scheduler_cancel_thread(void *_self);
int scheduler_suspend_thread(void *_self);
int scheduler_resume_thread(void *_self);


#endif /* scheduler_rpc_h */
