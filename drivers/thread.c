//
//  thread.c
//  AAOS
//
//  Created by huyi on 2024/11/15.
//

#include "def.h"
#include "detector_rpc.h"
#include "rpc.h"
#include "scheduler_def.h"
#include "scheduler_rpc.h"
#include "telescope_rpc.h"
#include "thread.h"
#include "thread_r.h"
#include "utils.h"
#include "wrapper.h"

#include <cjson/cJSON.h>

int
__observation_thread_cycle(void *_self)
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
    int ret = AAOS_OK;
    
    if (self->has_scheduler) {
        if (self->scheduler_client == NULL) {
            if (self->scheduler_addr != NULL && self->scheduler_addr != NULL) {
                self->scheduler_client = new(SchedulerClient(), self->scheduler_addr, self->scheduler_port);
            } else {
                return AAOS_EINVAL;
            }
        }
        
        if (self->scheduler == NULL) {
            if ((ret = rpc_client_connect(self->scheduler_client, &self->scheduler_client)) == AAOS_OK) {
                return AAOS_OK;
            } else {
                return ret;
            }
        } else {
            return AAOS_OK;
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
    int ret;
    
    if (self->has_telescope) {
        if (self->telescope_client == NULL) {
            if (self->telescope_addr != NULL && self->telescope_addr != NULL) {
                self->telescope_client = new(TelescopeClient(), self->telescope_addr, self->telescope_port);
            } else {
                return AAOS_EINVAL;
            }
        }
        
        if (self->telescope == NULL) {
            if ((ret = rpc_client_connect(self->telescope_client, &self->telescope_client)) == AAOS_OK) {
                return AAOS_OK;
            } else {
                return ret;
            }
        } else {
            return AAOS_OK;
        }
    } else {
        return AAOS_OK;
    }
}

static int
__ObservationThread_check_detector(struct __ObservationThread *self)
{
    int ret;
    
    if (self->has_detector) {
        
        if (self->detector_client == NULL) {
            if (self->detector_addr != NULL && self->detector_addr != NULL) {
                self->detector_client = new(DetectorClient(), self->detector_addr, self->detector_port);
            } else {
                return AAOS_EINVAL;
            }
        }
        
        if (self->detector == NULL) {
            if ((ret = rpc_client_connect(self->detector_client, &self->detector)) == AAOS_OK) {
                return AAOS_OK;
            } else {
                return ret;
            }
        } else {
            return AAOS_OK;
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
    
    int ret = AAOS_OK;
    
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
    
    Pthread_mutex_lock(&self->mtx);
    while (self->state == OT_STATE_STOP || self->state == OT_STATE_CANCEL) {
        Pthread_cond_wait(&self->cond, &self->mtx);
        ret = AAOS_ECANCELED;
        goto end;
    }
    Pthread_mutex_unlock(&self->mtx);
    
    char buf[BUFSIZE];
    if (((ret = scheduler_get_task_by_telescope_id(self->scheduler, self->tel_id, buf, BUFSIZE, NULL, NULL))) != AAOS_OK) {
        /*
         * 
         */
    }
    
    cJSON *root_json, *telescope_json, *target_json, *value_json, *ra_json, *dec_json;
    bool is_daytime = false, is_badweather = false;
    double ra, dec, exptime = 60.;
    uint32_t nframes = 1;
    struct timespec tp;
    char *detname;

    /*
     * Bad weather and daytime.
     */
    if ((root_json = cJSON_Parse(buf)) == NULL) {
#ifdef DEBUG
        fprintf(stderr, "%s %s %d: Bad format of SIU, `%s`.\n", __FILE__, __func__, __LINE__, buf);
#endif
        return AAOS_EBADCMD;
    }

    if (((target_json = cJSON_GetObjectItemCaseSensitive(root_json, "TARGET-INFO"))) == NULL) {
#ifdef DEBUG
        fprintf(stderr, "%s %s %d: SIU does not contain `TARGET-INFO`.\n", __FILE__, __func__, __LINE__);
#endif
        return AAOS_EBADCMD;
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
    
    Pthread_mutex_lock(&self->mtx);
    while (self->state == OT_STATE_STOP || self->state == OT_STATE_CANCEL) {
        Pthread_cond_wait(&self->cond, &self->mtx);
        if (self->state == OT_STATE_CANCEL) {
            ret = AAOS_ECANCELED;
        }
        goto end;
    }
    Pthread_mutex_unlock(&self->mtx);

    if (self->has_telescope) {
        if ((telescope_json = cJSON_GetObjectItemCaseSensitive(root_json, "TELESCOPE-INFO")) == NULL) {
            return AAOS_ERROR;
        }
        if ((value_json = cJSON_GetObjectItemCaseSensitive(telescope_json, "telescop")) == NULL) {
            return AAOS_ERROR;
        }
        if ((ret = telescope_get_index_by_name(self->telescope, value_json->valuestring)) != AAOS_OK) {
            return AAOS_ERROR;
        }
        /*
         * Change instrument, filter, detector.
         */
        if ((value_json = cJSON_GetObjectItemCaseSensitive(telescope_json, "instrume")) != NULL) {
            telescope_switch_instrument(self->telescope, value_json->valuestring);
        }
        if ((value_json = cJSON_GetObjectItemCaseSensitive(telescope_json, "filter")) != NULL) {
            telescope_switch_filter(self->telescope, value_json->valuestring);
        }
        if ((value_json = cJSON_GetObjectItemCaseSensitive(telescope_json, "detname")) != NULL) {
            telescope_switch_detector(self->telescope, value_json->valuestring);
            detname = value_json->valuestring;
        }
        if ((value_json = cJSON_GetObjectItemCaseSensitive(telescope_json, "exptime")) != NULL) {
            exptime = value_json->valuedouble;
        }
        if ((value_json = cJSON_GetObjectItemCaseSensitive(telescope_json, "nframes")) != NULL) {
            nframes = value_json->valueint;
        }
        
        ra_json = cJSON_GetObjectItemCaseSensitive(target_json, "targ_ra");
        dec_json = cJSON_GetObjectItemCaseSensitive(target_json, "targ_dec");
        if (ra_json != NULL && dec_json != NULL && cJSON_IsNumber(ra_json) && cJSON_IsNumber(dec_json)) {
            ra = ra_json->valuedouble;
            dec = dec_json->valuedouble;
            Pthread_mutex_lock(&self->mtx);
            self->state = OT_STATE_SLEW;
            Pthread_mutex_unlock(&self->mtx);
            if ((ret = telescope_slew(self->telescope, ra, dec)) != AAOS_OK) {
                return ret;
            }
        }
    }
    
    Pthread_mutex_lock(&self->mtx);
    while (self->state == OT_STATE_STOP || self->state == OT_STATE_CANCEL) {
        Pthread_cond_wait(&self->cond, &self->mtx);
        ret = AAOS_ECANCELED;
        goto end;
    }
    Pthread_mutex_unlock(&self->mtx);
    
    if (self->has_detector) {
        if ((ret = detector_get_index_by_name(self->detector, detname)) != AAOS_OK) {
            return AAOS_ERROR;
        }
        Pthread_mutex_lock(&self->mtx);
        self->state = OT_STATE_EXPOSE;
        Pthread_mutex_unlock(&self->mtx);
        if ((ret = detector_expose(self->detector, exptime, nframes, NULL)) != AAOS_OK) {
            return AAOS_ERROR;
        }
    }
    
    Pthread_mutex_lock(&self->mtx);
end:
    self->state = OT_STATE_IDLE;
    Pthread_mutex_unlock(&self->mtx);
    
    return ret;
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
__ObservationThread_start(void *_self)
{
    struct __ObservationThread *self = cast(__ObservationThread(), _self);
    
    Pthread_mutex_lock(&self->mtx);
    self->tid = pthread_self();
    Pthread_mutex_unlock(&self->mtx);
    int ret;
    for (; ;) {
        ret = __observation_thread_cycle(_self);
    }
    
    return AAOS_OK;
}

static void *
__ObservationThread_thr(void *_self)
{
    int ret;
    
    ret = __ObservationThread_start(_self);
    
    return NULL;
}

int
__observation_thread_stop(void *_self)
{
    const struct __ObservationThreadClass *class = (const struct __ObservationThreadClass *) classOf(_self);
    
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
    const struct __ObservationThreadClass *class = (const struct __ObservationThreadClass *) classOf(_self);
    
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
    if (self->state == OT_STATE_SUSPEND || self->state == OT_STATE_CANCEL || self->state == OT_STATE_STOP) {
        self->state = OT_STATE_IDLE;
    }
    Pthread_mutex_unlock(&self->mtx);
    Pthread_cond_signal(&self->cond);

    return AAOS_OK;
}

int
__observation_thread_cancel(void *_self)
{
    const struct __ObservationThreadClass *class = (const struct __ObservationThreadClass *) classOf(_self);
    
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
    if (isOf(class, __ObservationThreadClass()) && class->get_member.method) {
        ((void (*)(void *)) class->get_member.method)(_self);
    } else {
        forward(_self, (void *) 0, (Method) __observation_thread_get_member, "get_member", _self, name, &ap);
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
    if (isOf(class, __ObservationThreadClass()) && class->set_member.method) {
        ((void (*)(void *)) class->set_member.method)(_self);
    } else {
        forward(_self, (void *) 0, (Method) __observation_thread_set_member, "set_member", _self, name, &ap);
    }
    va_end(ap);
}

static void
__ObservationThread_set_member(void *_self, const char *name, va_list *app)
{
    struct __ObservationThread *self = cast(__ObservationThread(), _self);
    
    int ret;
    if (strcmp(name, "scheduler") == 0) {
        const char *addr, *port;
        addr = va_arg(*app, const char *);
        port = va_arg(*app, const char *);
        if (addr == NULL && port == NULL) {
            if (self->scheduler_addr != NULL && self->scheduler_port != NULL && (ret = rpc_client_connect(self->scheduler_client, &self->scheduler2)) == AAOS_OK) {

            }
            return;
        }
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
        addr = va_arg(*app, const char *);
        port = va_arg(*app, const char *);
        self->dome_addr = (char *) Realloc(self->dome_addr, strlen(addr) + 1);
        self->dome_port = (char *) Realloc(self->dome_port, strlen(port) + 1);
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
        addr = va_arg(*app, const char *);
        port = va_arg(*app, const char *);
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
        addr = va_arg(*app, const char *);
        port = va_arg(*app, const char *);
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
        addr = va_arg(*app, const char *);
        port = va_arg(*app, const char *);
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


static void *
__ObservationThread_ctor(void *_self, va_list *app)
{
    struct __ObservationThread *self = super_ctor(__ObservationThread(), _self, app);
    
    /*
    const char *s, *key, *value;
    
    s = va_arg(*app, const char *);
    if (s) {
        self->name = (char *) Malloc(strlen(s) + 1);
        snprintf(self->name, strlen(s) + 1, "%s", s);
    }
    s = va_arg(*app, const char *);
    if (s) {
        self->path = (char *) Malloc(strlen(s) + 1);
        snprintf(self->path, strlen(s) + 1, "%s", s);
    }
    
    self->read_timeout = READTIMEOUT;
    self->write_timeout = WRITETIMEOUT;
    
    while ((key = va_arg(*app, const char *))) {
        if (strcmp(key, "description") == 0) {
            value = va_arg(*app, const char *);
            if (value) {
                self->description = (char *) Malloc(strlen(value) + 1);
                snprintf(self->description, strlen(value) + 1, "%s", value);
            }
            
            continue;
        } else if (strcmp(key, "read_timeout") == 0) {
            self->read_timeout = va_arg(*app, double);
            continue;
        } else if (strcmp(key, "write_timeout") == 0) {
            self->write_timeout = va_arg(*app, double);
            continue;
        } else if (strcmp(key, "read_buffer_size") == 0) {
            self->read_buffer_size = va_arg(*app, size_t);
            self->read_buffer = Malloc(self->read_buffer_size);
            if (!self->write_buffer) {
                self->write_buffer = self->read_buffer;
                self->write_buffer_size = self->read_buffer_size;
            }
            continue;
        } else if (strcmp(key, "write_buffer_size") == 0) {
            self->write_buffer_size = va_arg(*app, size_t);
            self->write_buffer = Malloc(self->write_buffer_size);
            if (!self->read_buffer) {
                self->read_buffer = self->write_buffer;
                self->read_buffer_size = self->write_buffer_size;
            }
            continue;
        }
         
    }*/
    
    return (void *) self;
}

static void *
__ObservationThread_dtor(void *_self)
{
    struct __ObservationThread *self = cast(__ObservationThread(), _self);
    
    if (self->has_dome) {
        if (self->dome) {
            delete(self->dome);
        }
        if (self->dome_client) {
            delete(self->dome_client);
        }
        free(self->dome_name);
        free(self->dome_addr);
        free(self->dome_port);
    }
    
    if (self->has_detector) {
        if (self->detector) {
            delete(self->detector);
        }
        if (self->detector_client) {
            delete(self->detector_client);
        }
        free(self->detector_name);
        free(self->detector_addr);
        free(self->detector_port);
    }
    
    if (self->has_scheduler) {
        if (self->detector) {
            delete(self->detector);
        }
        if (self->detector_client) {
            delete(self->detector_client);
        }
        free(self->detector_name);
        free(self->detector_addr);
        free(self->detector_port);
        
    }
    
    free(self->detector_addr);
    free(self->detector_port);
    if (self->detector_client) {
        delete(self->detector_client);
    }
    
    return super_dtor(__ObservationThread(), _self);
}


static void *
__ObservationThreadClass_ctor(void *_self, va_list *app)
{
    struct __ObservationThreadClass *self = super_ctor(__ObservationThreadClass(), _self, app);
    Method selector;
    
#ifdef va_copy
    va_list ap;
    va_copy(ap, *app);
#else
    va_list ap = *app;
#endif
    
    while ((selector = va_arg(ap, Method))) {
        const char *tag = va_arg(ap, const char *);
        Method method = va_arg(ap, Method);
        if (selector == (Method) __observation_thread_start) {
            if (tag) {
                self->start.tag = tag;
                self->start.selector = selector;
            }
            self->start.method = method;
            continue;
        }
        if (selector == (Method) __observation_thread_stop) {
            if (tag) {
                self->stop.tag = tag;
                self->stop.selector = selector;
            }
            self->stop.method = method;
            continue;
        }
        if (selector == (Method) __observation_thread_suspend) {
            if (tag) {
                self->suspend.tag = tag;
                self->suspend.selector = selector;
            }
            self->suspend.method = method;
            continue;
        }
        if (selector == (Method) __observation_thread_cancel) {
            if (tag) {
                self->cancel.tag = tag;
                self->cancel.selector = selector;
            }
            self->cancel.method = method;
            continue;
        }
        if (selector == (Method) __observation_thread_cycle) {
            if (tag) {
                self->cycle.tag = tag;
                self->cycle.selector = selector;
            }
            self->cycle.method = method;
            continue;
        }
        
    }
    
#ifdef va_copy
    va_end(ap);
#endif
    
    return (void *) self;
}

static const void *___ObservationThreadClass;

static void
__ObservationThreadClass_destroy(void)
{
    free((void *) ___ObservationThreadClass);
}

static void
__ObservationThreadClass_initialize(void)
{
    ___ObservationThreadClass = new(Class(), "__ObservationThreadClass", Class(), sizeof(struct __ObservationThreadClass),
                                    ctor, "ctor", __ObservationThreadClass_ctor,
                                    (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(__ObservationThreadClass_destroy);
#endif
}

const void *
__ObservationThreadClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, __ObservationThreadClass_initialize);
#endif
    
    return ___ObservationThreadClass;
}

static const void *___ObservationThread;

static void
__ObservationThread_destroy(void)
{
    free((void *)___ObservationThread);
}

static void
__ObservationThread_initialize(void)
{
    ___ObservationThread = new(__ObservationThreadClass(), "__ObservationThread", Object(), sizeof(struct __ObservationThread),
                               ctor, "ctor", __ObservationThread_ctor,
                               dtor, "dtor", __ObservationThread_dtor,
                               
                               (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(__ObservationThread_destroy);
#endif
}

const void *
__ObservationThread(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, __ObservationThread_initialize);
#endif
    
    return ___ObservationThread;
}

