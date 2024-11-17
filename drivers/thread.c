//
//  thread.c
//  AAOS
//
//  Created by huyi on 2024/11/15.
//

#include "def.h"
#include "detector_rpc.h"
#include "scheduler_rpc.h"
#include "telescope_rpc.h"
#include "thread.h"
#include "thread_r.h"
#include "wrapper.h"

#include <cjson/cJSON.h>

int
__observation_thread_cycle(void *_sef)
{
    const struct __ObservationThreadClass *class = (const struct __ObservationThreadClass *) classOf(_self);
    
    if (isOf(class, __ObservationThreadClass()) && class->cycle.method) {
        return ((int (*)(void *)) class->cycle.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) __observation_thread_cycle, "cycle", _self);
        return result;
    }
}

static int
__ObservationThread_check_scheduler(struct __ObservationThread *self)
{
    int ret;
    if (self->has_scheduler) {
        if (self->scheduler_client == NULL && self->scheduler_addr != NULL && self->scheduler_addr != NULL) {
            self->scheduler_client = new(SchedulerClient(), self->scheduler_addr, self->scheduler_port);
        } else {
            return AAOS_EINVAL;
        }
        
        if (self->scheduler == NULL && (ret = rpc_connect(self->scheduler_client, &self->scheduler)) == AAOS_OK) {
            
        } else {
            return ret;
        }
    } else {
        return AAOS_OK;
    }
}

/*
static int
__ObservationThread_check_dome(struct __ObservationThread *self)
{
    if (self->has_dome) {
        if (self->dome_client == NULL && self->dome_addr != NULL && self->dome_addr != NULL) {
            self->dome_client = new(DomeClient(), self->dome_addr, self->dome_port);
        } else {
            return AAOS_EINVAL;
        }
        
        if (self->dome == NULL && (ret = rpc_connect(self->dome_client, &self->dome)) == AAOS_OK) {
            
        } else {
            return ret;
        }
        
    } else {
        return AAOS_OK;
    }
}
*/

static int
__ObservationThread_check_telescope(struct __ObservationThread *self)
{
    if (self->has_telescope) {
        if (self->telescope_client == NULL && self->telescope_addr != NULL && self->telescope_addr != NULL) {
            self->telescope_client = new(TelescopeClient(), self->telescope_addr, self->telescope_port);
        } else {
            return AAOS_EINVAL;
        }
        
        if (self->scheduler == NULL && (ret = rpc_connect(self->telescope_client, &self->telescope)) == AAOS_OK) {
            
        } else {
            return ret;
        }
        
    } else {
        return AAOS_OK;
    }
}

static int
__ObservationThread_check_detector(struct __ObservationThread *self)
{
    if (self->has_detector) {
        if (self->detector_client == NULL && self->detector_addr != NULL && self->detector_addr != NULL) {
            self->detector_client = new(DetectorClient(), self->detector_addr, self->detector_port);
        } else {
            return AAOS_EINVAL;
        }
        
        if (self->detector == NULL && (ret = rpc_connect(self->detector_client, &self->detector)) == AAOS_OK) {
            
        } else {
            return ret;
        }
        
    } else {
        return AAOS_OK;
    }
}

/*
static int
__ObservationThread_check_pipeline(struct __ObservationThread *self)
{
    if (self->has_pipeline) {
        if (self->pipeline_client == NULL && self->pipeline_addr != NULL && self->pipeline_addr != NULL) {
            self->pipeline_client = new(PipelineClient(), self->pipeline_addr, self->pipeline_port);
        } else {
            return AAOS_EINVAL;
        }
        
        if (self->pipeline == NULL && (ret = rpc_connect(self->pipeline_client, &self->pipeline)) == AAOS_OK) {
            
        } else {
            return ret;
        }
        
    } else {
        return AAOS_OK;
    }
}
*/

int
__ObservationThread_cycle(void *_self)
{
    struct __ObservationThread *self = cast(__ObservationThread(), _self);

    Pthread_mutex_lock(&self->mtx);
    while (self->state == OT_STATE_CANCEL || self->state == OT_STATE_STOP || self->state == OT_STATE_SUSPEND) {
        pthread_cond_wait(&self->cond, &self->mtx);
    }
    Pthread_mutex_unlock(&self->mtx);
    
    int ret;
    
    if (self->has_scheduler && (ret = __ObservationThread_check_scheduler(self)) != AAOS_OK) {

    }
    /*
    if (self->has_dome && (ret = __ObservationThread_check_dome(self)) != AAOS_OK) {
        
    }
    */
    if (self->has_telescope && (ret = __ObservationThread_check_telescope(self)) != AAOS_OK) {
        
    }
    
    if (self->has_detector && (ret = __ObservationThread_check_detector(self)) != AAOS_OK) {
        
    }
    /*
    if (self->has_pipeline && (ret = __ObservationThread_check_pipeline(self)) != AAOS_OK) {
        
    }
    */
    char buf[BUFSIZE];
    
    if (((ret = scheduler_get_target_by_telescope_id(self->scheduler, self->tel_id, buf, BUFSIZE, NULL, NULL))) != AAOS_OK) {    
        /*
         * 
         */
    }
    
    cJSON *root_json, *telescope_json, *target_json, *value_json, *ra_json, *dec_json;
    bool is_daytime = false, is_badweather = false;
    double ra, dec, exptime;
    uint32_t nframes;
    struct timespec tp;
    char *detname;

    /*
     * Bad weather and daytime.
     */
    if ((root_json = cJSON_Parse(buf)) == NULL) {
        return AAOS_ERROR;
    }

    if (((target_json = cJSON_GetObjectItemCaseSensitive(root_json, "TARGET-INFO"))) == NULL) {
        return AAOS_ERROR;
    }
     value_json = cJSON_GetObjectItemCaseSensitive(target_json, "targname");
    if (value_json != NULL && cJSON_IsString(value_json)) {
        if (strcasecmp(value_json->valuestring, "bad wearther") == 0) {
            is_badweather = true;
        }
        if (strcasecmp(value_json->valuestring, "daytime") == 0) {
            is_daytime = true;
        }
    }
    value_json = cJSON_GetObjectItemCaseSensitive(target_json, "expired");
    if (value_json != NULL && cJSON_IsString(value_json)) {
        iso_str_to_tp(value_json->valuestring, &tp);
    }
    if (is_daytime | is_badweather) {
#ifdef MACOSX
        Clock_nanosleep(CLOCK_REALTIME, 1, &tp, NULL);
#endif
#ifdef LINUX
        Clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &tp, NULL);
#endif
        cJSON_Delete(root_json);
        return AAOS_OK;
    }

    if (self->has_telescope) {
        if ((telescope_json = cJSON_GetObjectItemCaseSensitive(root_json, "TELESCOPE-INFO")) == NULL) {
            return AAOS_ERROR;
        }
        if ((value_json = cJSON_GetObjectItemCaseSensitive(telescope_json, "telescop")) == NULL ) {
            return AAOS_ERROR;
        }
        if ((ret = telescope_get_index_by_name(self->telescope, value_json->valuestring)) != AAOS_OK) {
            return AAOS_ERROR;
        }
        /*
         * Change instrument, filter, detector.
         */
        if ((value_json = cJSON_GetObjectItemCaseSensitive(telescope_json, "instrume")) != NULL ) {

        }
        if ((value_json = cJSON_GetObjectItemCaseSensitive(telescope_json, "filter")) != NULL ) {

        }
        if ((value_json = cJSON_GetObjectItemCaseSensitive(telescope_json, "detname")) != NULL ) {
            detname = value_json->valuestring;
        }
        if ((value_json = cJSON_GetObjectItemCaseSensitive(telescope_json, "exptime")) != NULL ) {
            exptime = value_json->valuedouble;
        }
        if ((value_json = cJSON_GetObjectItemCaseSensitive(telescope_json, "nframes")) != NULL ) {
            nframes = value_json->valueint;
        }
        
        ra_json = cJSON_GetObjectItemCaseSensitive(target_json, "targ_ra");
        dec_json = cJSON_GetObjectItemCaseSensitive(target_json, "targ_dec");
        if (ra_json != NULL && dec_json != NULL && cJSON_IsNumber(ra_json) && cJSON_IsNumber(dec_json)) {
            ra = ra_json->valuedoubel;
            dec = dec_json->valuedouble;
            if ((ret = telescope_slew(self->telescope, ra, dec)) != AAOS_OK) {
                return ret;
            }
        }
    }
    
    if (self->has_detector) {
        if ((ret = detector_get_index_by_name(self->detector, detname)) != AAOS_OK) {
            return AAOS_ERROR;
        }
        if ((ret = detector_expose(self->detector, exptime, nframes), NULL) != AAOS_OK) {
            return AAOS_ERROR;
        }
    }

    return AAOS_OK;
}

int
__observation_thread_start(void *_self)
{
    const struct __ObservationThreadClass *class = (const struct __ObservationThreadClass *) classOf(_self);
    
    if (isOf(class, __ObservationThreadClass()) && class->start.method) {
        return ((int (*)(void *)) class->start.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) __observation_thread_start, "start", _self);
        return result;
    }
}

static int
__ObserbationThread_start(void *_self)
{
    struct __ObservationThread *self = cast(__ObservationThread(), _self);

    int ret;
    for (; ;) {
        ret = __observation_thread_cycle(_self);
    }

    return AAOS_OK;
}

int
__observation_thread_stop(void *_self)
{
    if (isOf(class, __ObservationThreadClass()) && class->stop.method) {
        return ((int (*)(void *)) class->stop.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) __observation_thread_stop, "stop", _self);
        return result;
    }
}

static int
__ObserbationThread_stop(void *_self)
{
    struct __ObservationThread *self = cast(__ObservationThread(), _self);

    Pthread_mutex_lock(&self->mtx);
    self->state = OT_STATE_STOP;
    Pthread_mutex_lock(&self->mtx);
    return AAOS_OK;
}

int
__observation_thread_resume(void *_self)
{
    if (isOf(class, __ObservationThreadClass()) && class->resume.method) {
        return ((int (*)(void *)) class->resume.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) __observation_thread_resume, "resume", _self);
        return result;
    }
}

static int
__ObserbationThread_resume(void *_self)
{
    struct __ObservationThread *self = cast(__ObservationThread(), _self);

    Pthread_mutex_lock(&self->mtx);
    if (self->state == OT_STATE_SUSPEND || slef->state == OT_STATE_CANCEL || self->state == OT_STATE_STOP) {
        self->state = OT_STATE_IDLE;
    }
    Pthread_mutex_unlock(&self->mtx);
    Pthread_cond_signal(&self->cond);

    return AAOS_OK;
}

int
__observation_thread_cancel(void *_self)
{
    if (isOf(class, __ObservationThreadClass()) && class->cancel.method) {
        return ((int (*)(void *)) class->cancel.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) __observation_thread_cancel, "cancel", _self);
        return result;
    }
}

void
__observation_thread_get_member(void *_self, const char *name, ...)
{
    const struct __ObservationThreadClass *class = (const struct __ObservationThreadClass *) classOf(_self);
    
    va_list ap;
    va_start(ap, name);
    if (isOf(class, __SchedulerClass()) && class->get_member.method) {
        ((void (*)(void *)) class->get_member.method)(_self);
    } else {
        forward(_self, 0, (Method) __observation_thread_set_member, "get_member", _self, name, &ap);
    }
    va_end(ap);
}

static void
__ObservationThread_get_member(void *_self, const char *name, va_list *app)
{
    struct __ObservationThread *self = cast(__ObservationThread(), _self);

    if (strcmp(name, "scheduler") == 0) {
        void **value = va_arg(*app, void **);
        *value = self->scheduler2;
        return;
    }
}

void
__observation_thread_set_member(void *_self, const char *name, ...)
{
    const struct __ObservationThreadClass *class = (const struct __ObservationThreadClass *) classOf(_self);
    
    va_list ap;
    va_start(ap, name);
    if (isOf(class, __SchedulerClass()) && class->set_member.method) {
        ((void (*)(void *)) class->set_member.method)(_self);
    } else {
        forward(_self, 0, (Method) __observation_thread_set_member, "set_member", _self, name, &ap);
    }
    va_end(ap);
}

static void
__ObservationThread_set_member(void *_self, const char *name, va_list *app)
{
    struct __ObservationThread *self = cast(__ObservationThread(), _self);

    if (strcmp(name, "scheduler") == 0) {
        const char *addr, *port;
        addr = va_arg(addr, const char *);
        port = va_arg(port, const char *);
        self->scheduler_addr = (char *) Realloc(self->scheduler_addr, strlen(addr) + 1);
        self->scheduler_port = (char *) Realloc(self->scheduler_port, strlen(port) + 1);
        snprintf(self->scheduler_addr, strlen(addr) + 1, "%s", addr);
        snprintf(self->scheduler_port, strlen(port) + 1, "%s", port);
        if (self->scheduler_client != NULL) {
            delete(self->scheduler_client);
        }
        if (self->scheduler2 != NULL) {
            delete(self->scheduler2);
        }
        self->scheduler_client = new(SchedulerClient(), addr, port);
        if ((ret = rpc_client_connect(self->scheduler_client, &self->scheduler2)) == AAOS_OK) {

        }
        return;
    }

    if (strcmp(name, "dome") == 0) {
        const char *addr, *port;
        addr = va_arg(addr, const char *);
        port = va_arg(port, const char *);
        self->dome_addr = (char *) Realloc(self->dome_addr);
        self->dome_port = (char *) Realloc(self->dome_port);
        snprintf(self->dome_addr, strlen(addr) + 1, "%s", addr);
        snprintf(self->dome_port, strlen(port) + 1, "%s", port);
        
        /*
        if (self->dome_client != NULL) {
            delete(self->dome_client);
        }
        self->dome_client = new(DomeClient(), addr, port);
        */
        return;
    }

    if (strcmp(name, "telescope") == 0) {
        const char *addr, *port;
        addr = va_arg(addr, const char *);
        port = va_arg(port, const char *);
        self->telescope_addr = (char *) Realloc(self->telescope_addr, strlen(addr) + 1);
        self->telescope_port = (char *) Realloc(self->telescope_port, strlen(port) + 1);
        snprintf(self->telescope_addr, strlen(addr) + 1, "%s", addr);
        snprintf(self->telescope_port, strlen(port) + 1, "%s", port);
        if (self->telescope_client != NULL) {
            delete(self->telescope_client);
        }
        self->telescope_client = new(TelescopeClient(), addr, port);
        return;
    }

    if (strcmp(name, "detector") == 0) {
        const char *addr, *port;
        addr = va_arg(addr, const char *);
        port = va_arg(port, const char *);
        self->detector_addr = (char *) Realloc(self->detector_addr, strlen(addr) + 1);
        self->detector_port = (char *) Realloc(self->detector_port, strlen(port) + 1);
        snprintf(self->detector_addr, strlen(addr) + 1, "%s", addr);
        snprintf(self->detector_port, strlen(port) + 1, "%s", port);
        if (self->detector_client != NULL) {
            delete(self->detector_client);
        }
        self->detector_client = new(DetectorClient(), addr, port);
        return;
    }

    if (strcmp(name, "pipeline") == 0) {
        const char *addr, *port;
        addr = va_arg(addr, const char *);
        port = va_arg(port, const char *);
        self->pipeline_addr = (char *) Realloc(self->pipeline_addr, strlen(addr) + 1);
        self->pipeline_port = (char *) Realloc(self->pipeline_port, strlen(port) + 1);
        snprintf(self->pipeline_addr, strlen(addr) + 1, "%s", addr);
        snprintf(self->pipeline_port, strlen(port) + 1, "%s", port);
        /*
        if (self->pipeline_client != NULL) {
            delete(self->pipeline_client);
        }
        self->pipeline_client = new(TelescopeClient(), addr, port);
        */
        return;
    }
}
