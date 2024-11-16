//
//  thread.c
//  AAOS
//
//  Created by huyi on 2024/11/15.
//

#include "def.h"
#include "scheduler_rpc.h"
#include "thread.h"
#include "thread_r.h"
#include "wrapper.h"


static
ObservationThread_cycle(void *_self)
{
    struct ObservationThread *self = cast(ObservationThread(), _self);

    int ret;

    double ra, dec;

    if (self->scheduler == NULL && self->has_scheduler) {
        /*
         * connect to scheduling system.
         */
    } else {
        return;
    }

    if (self->dome == NULL && self->has_dome) {
        /*
         * connect to dome driver.
         */
    } else {
        return;
    }

    if (self->telescope == NULL && self->has_telescope) {
        /*
         * connect to telescope driver.
         */
    } else {
        return;
    }

    if (self->telescope == NULL && self->has_detector) {
        /*
         * connect to detector driver.
         */
    } else {
        return;
    }

    if (self->has_scheduler && (ret = scheduler_get_task_by_telescope_id(self->scheduler, self->identifier)) != AAOS_OK) {
        return;
    }

    if (self->has_telescope && (ret = telescope_slew(self->telescope, ra, dec)) != AAOS_OK) {
        
    }

    if (self->has_detector && (ret = scheduler_get_task_by_telescope_id(self->scheduler, self->identifier)) != AAOS_OK) {
        
    }

}