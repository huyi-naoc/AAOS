//
//  thread.c
//  AAOS
//
//  Created by huyi on 2024/11/15.
//

#include "aws_rpc.h"
#include "def.h"
#include "detector_rpc.h"
#include "dome_rpc.h"
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

struct DomeArg {
    void *dome;
    double ra;
    double dec;
};

static void *
dome_initialize_thr(void *arg)
{
    dome_open_window(arg);
    
    return NULL;
}

static void *
dome_finalize_thr(void *arg)
{
    dome_close_window(arg);
    
    return NULL;
}

static void *
slew_dome_thr(void *arg)
{
    struct DomeArg *myarg = (struct DomeArg *) arg;

    dome_slew(myarg->dome, myarg->ra, myarg->dec);

    return NULL;
}

static void *
telescope_initialize_thr(void *arg)
{
    return NULL;
}

static void *
telescope_finalize_thr(void *arg)
{
    return NULL;
}

static void
__ObservationThread_check_before_cycle(struct __ObservationThread *self)
{
    Pthread_rwlock_wrlock(&self->dome_rwlock);
    if (self->has_dome && self->dome_client != NULL && self->dome == NULL) {
        rpc_client_connect(self->dome_client, &self->dome);
    }
    Pthread_rwlock_unlock(&self->dome_rwlock);
    
    Pthread_rwlock_wrlock(&self->scheduler_rwlock);
    if (self->has_scheduler && self->scheduler_client != NULL && self->scheduler == NULL) {
        rpc_client_connect(self->scheduler_client, &self->scheduler);
    }
    Pthread_rwlock_unlock(&self->scheduler_rwlock);
    
    Pthread_rwlock_wrlock(&self->telescope_rwlock);
    if (self->has_telescope && self->telescope_client != NULL && self->telescope == NULL) {
        rpc_client_connect(self->telescope_client, &self->telescope);
    }
    Pthread_rwlock_unlock(&self->telescope_rwlock);
    
    Pthread_rwlock_wrlock(&self->detector_rwlock);
    if (self->has_detector && self->detector_client != NULL && self->detector == NULL) {
        rpc_client_connect(self->detector_client, &self->detector);
    }
    Pthread_rwlock_unlock(&self->detector_rwlock);
    
    Pthread_rwlock_wrlock(&self->aws_rwlock);
    if (self->has_aws && self->aws_client != NULL && self->aws == NULL) {
        rpc_client_connect(self->aws_client, &self->aws);
    }
    Pthread_rwlock_unlock(&self->aws_rwlock);
    
    Pthread_rwlock_wrlock(&self->pipeline_rwlock);
    if (self->has_pipeline && self->pipeline_client != NULL && self->pipeline == NULL) {
        rpc_client_connect(self->pipeline_client, &self->aws);
    }
    Pthread_rwlock_unlock(&self->pipeline_rwlock);
}

static void
__ObservationThread_format_aws_data(struct __ObservationThread *self, cJSON *site_json)
{
    size_t i;
    cJSON *value_json;
    double value;
    int ret;
    
    if (self->aws_keyname == NULL || self->aws_keyvalue == NULL) {
        return;
    }

    for (i = 0; i < self->n_aws_keypair; i++) {
        if (self->aws_keyname[i] != NULL && self->aws_keyvalue != NULL && (ret = aws_get_data_by_name(self->aws, self->aws_name, self->aws_keyvalue[i], &value, 1)) == AAOS_OK) {
            if ((value_json = cJSON_GetObjectItemCaseSensitive(site_json, self->aws_keyname[i])) == NULL) {
                cJSON_AddNumberToObject(site_json, self->aws_keyname[i], value);
            } else {
                cJSON_SetNumberValue(value_json, value);
            }
        }
    }
}

static void
ObservationThread_image_callback(void *detector, const char *name, va_list *app)
{
    
}

static int
__ObservationThread_cycle(void *_self)
{
    struct __ObservationThread *self = cast(__ObservationThread(), _self);
    
    char buf[BUFSIZE], status[BUFSIZE];
    cJSON *root_json = NULL, *general_json, *telescope_json, *target_json, *site_json, *value_json, *ra_json, *dec_json;
    cJSON *status_json;
    bool is_daytime = false, is_badweather = false;
    double ra, dec, exptime = 60.;
    uint32_t nframes = 1;
    uint64_t tel_id = 0, task_id = 0;
    struct timespec tp;
    char *detname;
    int ret = AAOS_OK;
    pthread_t dome_tid = pthread_self();
    struct DomeArg *dome_arg = NULL;
    char *header = NULL;
    
    
    Pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
    
    __ObservationThread_check_before_cycle(self);

    Pthread_mutex_lock(&self->mtx);
    while (self->state == OT_STATE_CANCEL || self->state == OT_STATE_STOP || self->state == OT_STATE_SUSPEND) {
        Pthread_cond_wait(&self->cond, &self->mtx);
        Pthread_mutex_unlock(&self->mtx);
        return AAOS_OK;
    }
    Pthread_mutex_unlock(&self->mtx);
    
    Pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_testcancel();
    Pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
    
    Pthread_rwlock_rdlock(&self->dome_rwlock);
    if (self->has_dome && self->dome != NULL) {
        dome_open_window(self->dome);
    }
    Pthread_rwlock_unlock(&self->dome_rwlock);
    
    Pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_testcancel();
    Pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
    
    memset(buf, '\0', BUFSIZE);
    Pthread_rwlock_rdlock(&self->scheduler_rwlock);
    if (self->has_scheduler && self->scheduler != NULL) {
        Pthread_rwlock_rdlock(&self->telescope_rwlock);
        if (self->telescope_identifier != 0) {
            if (((ret = scheduler_get_task_by_telescope_id(self->scheduler, self->telescope_identifier, buf, BUFSIZE, NULL, NULL))) != AAOS_OK) {
                if (ret == AAOS_EPIPE) {
                    delete(self->scheduler);
                    self->scheduler = NULL;
                }
                Pthread_rwlock_unlock(&self->telescope_rwlock);
                Pthread_rwlock_unlock(&self->scheduler_rwlock);
                goto end;
            }
        } else {
            if (((ret = scheduler_get_task_by_telescope_name(self->scheduler, self->telescope_name, buf, BUFSIZE, NULL, NULL))) != AAOS_OK) {
                if (ret == AAOS_EPIPE) {
                    delete(self->scheduler);
                    self->scheduler = NULL;
                }
                Pthread_rwlock_unlock(&self->telescope_rwlock);
                Pthread_rwlock_unlock(&self->scheduler_rwlock);
                goto end;
            }
        }
        
        Pthread_rwlock_unlock(&self->telescope_rwlock);
    }
    Pthread_rwlock_unlock(&self->scheduler_rwlock);

    if (buf[0] != '\0') {
        if ((root_json = cJSON_Parse(buf)) == NULL) {
#ifdef DEBUG
            fprintf(stderr, "%s %s %d: Bad format of SIU, `%s`.\n", __FILE__, __func__, __LINE__ - 2, buf);
#endif
            return AAOS_EBADMSG;
        }
        if (((target_json = cJSON_GetObjectItemCaseSensitive(root_json, "TARGET-INFO"))) == NULL) {
#ifdef DEBUG
            fprintf(stderr, "%s %s %d: SIU does not contain `TARGET-INFO`.\n", __FILE__, __func__, __LINE__ - 2);
#endif
            return AAOS_EBADMSG;
        }
        if ((general_json = cJSON_GetObjectItemCaseSensitive(root_json, "GENERAL-INFO")) != NULL && (value_json = cJSON_GetObjectItemCaseSensitive(general_json, "task_id")) != NULL && cJSON_IsNumber(value_json)) {
            task_id = (uint64_t) value_json->valuedouble;
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
        if (self->state == OT_STATE_CANCEL || self->state == OT_STATE_STOP || self->state == OT_STATE_SUSPEND) {
            Pthread_rwlock_rdlock(&self->scheduler_rwlock);
            if (task_id != 0 && self->scheduler != NULL) {
                if ((ret = scheduler_update_task_status(self->scheduler, task_id, SCHEDULER_STATUS_INCOMPLETE)) != AAOS_OK) {
#ifdef DEBUG
                    fprintf(stderr, "%s %s %d: scheduler_update_task_status error, %d\n", __FILE__, __func__, __LINE__ - 2, ret);
#endif
                }
            }
            Pthread_rwlock_unlock(&self->scheduler_rwlock);
            Pthread_mutex_unlock(&self->mtx);
            if (root_json != NULL) {
                cJSON_Delete(root_json);
            }
            return AAOS_ECANCELED;
        }
        Pthread_mutex_unlock(&self->mtx);
    }
    
    Pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_testcancel();
    Pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
    
    if (task_id != 0) {
        Pthread_rwlock_rdlock(&self->scheduler_rwlock);
        if (self->scheduler != NULL && (ret = scheduler_update_task_status(self->scheduler, task_id, SCHEDULER_STATUS_EXECUTE)) != AAOS_OK) {
#ifdef DEBUG
            fprintf(stderr, "%s %s %d: scheduler_update_task_status error, %d\n", __FILE__, __func__, __LINE__ - 2, ret);
#endif
        }
        Pthread_rwlock_unlock(&self->scheduler_rwlock);
    }
    
    Pthread_rwlock_rdlock(&self->telescope_rwlock);
    if (self->has_telescope) {
        if ((telescope_json = cJSON_GetObjectItemCaseSensitive(root_json, "TELESCOPE-INFO")) == NULL) {
            Pthread_rwlock_rdlock(&self->scheduler_rwlock);
            if (task_id != 0) {
                if (self->scheduler != NULL && (ret = scheduler_update_task_status(self->scheduler, task_id, SCHEDULER_STATUS_INCOMPLETE)) != AAOS_OK) {
#ifdef DEBUG
                    fprintf(stderr, "%s %s %d: scheduler_update_task_status error, %d\n", __FILE__, __func__, __LINE__ - 2, ret);
#endif
                }
            }
            Pthread_rwlock_unlock(&self->scheduler_rwlock);
            Pthread_rwlock_unlock(&self->telescope_rwlock);
            if (root_json != NULL) {
                cJSON_Delete(root_json);
            }
            return AAOS_EBADMSG;
        }
        if ((value_json = cJSON_GetObjectItemCaseSensitive(telescope_json, "telescop")) == NULL) {
            Pthread_rwlock_rdlock(&self->scheduler_rwlock);
            if (task_id != 0) {
                if (self->scheduler != NULL && (ret = scheduler_update_task_status(self->scheduler, task_id, SCHEDULER_STATUS_INCOMPLETE)) != AAOS_OK) {
#ifdef DEBUG
                    fprintf(stderr, "%s %s %d: scheduler_update_task_status error, %d\n", __FILE__, __func__, __LINE__ - 2, ret);
#endif
                }
            }
            Pthread_rwlock_unlock(&self->scheduler_rwlock);
            Pthread_rwlock_unlock(&self->telescope_rwlock);
            if (root_json != NULL) {
                cJSON_Delete(root_json);
            }
            return AAOS_EBADMSG;
        }
        if ((value_json = cJSON_GetObjectItemCaseSensitive(telescope_json, "exptime")) != NULL) {
            exptime = value_json->valuedouble;
        }
        if ((value_json = cJSON_GetObjectItemCaseSensitive(telescope_json, "nframes")) != NULL) {
            nframes = value_json->valueint;
        }
        if ((value_json = cJSON_GetObjectItemCaseSensitive(telescope_json, "instrume")) != NULL) {
            ret = telescope_switch_instrument(self->telescope, value_json->valuestring);
        }
        if ((value_json = cJSON_GetObjectItemCaseSensitive(telescope_json, "filter")) != NULL) {
            ret = telescope_switch_filter(self->telescope, value_json->valuestring);
        }
        if ((value_json = cJSON_GetObjectItemCaseSensitive(telescope_json, "detname")) != NULL) {
            ret = telescope_switch_detector(self->telescope, value_json->valuestring);
            detname = value_json->valuestring;
        }
        ra_json = cJSON_GetObjectItemCaseSensitive(target_json, "targ_ra");
        dec_json = cJSON_GetObjectItemCaseSensitive(target_json, "targ_dec");
        if (ra_json != NULL && dec_json != NULL && cJSON_IsNumber(ra_json) && cJSON_IsNumber(dec_json)) {
            ra = ra_json->valuedouble;
            dec = dec_json->valuedouble;
            Pthread_mutex_lock(&self->mtx);
            self->state = OT_STATE_SLEW;
            Pthread_mutex_unlock(&self->mtx);
            
            Pthread_rwlock_rdlock(&self->dome_rwlock);
            if (self->has_dome && self->dome != NULL) {
                dome_arg = (struct DomeArg *) Malloc(sizeof(struct DomeArg));
                dome_arg->ra = ra;
                dome_arg->dec = dec;
                dome_arg->dome = self->dome;
                Pthread_create(&dome_tid, NULL, slew_dome_thr, dome_arg);
            }
            Pthread_rwlock_unlock(&self->dome_rwlock);
            if (self->telescope != NULL && (ret = telescope_slew(self->telescope, ra, dec)) != AAOS_OK) {
                if (ret == AAOS_EPIPE) {
                    delete(self->telescope);
                    self->telescope = NULL;
                }
                Pthread_rwlock_rdlock(&self->scheduler_rwlock);
                if (task_id != 0) {
                    if (self->scheduler != NULL && (ret = scheduler_update_task_status(self->scheduler, task_id, SCHEDULER_STATUS_INCOMPLETE)) != AAOS_OK) {
#ifdef DEBUG
                        fprintf(stderr, "%s %s %d: scheduler_update_task_status error, %d\n", __FILE__, __func__, __LINE__ - 2, ret);
#endif
                    }
                }
                Pthread_rwlock_unlock(&self->scheduler_rwlock);
                
                Pthread_mutex_lock(&self->mtx);
                if (self->state == OT_STATE_CANCEL) {
                    /*
                     * TODO, close dome, go home.
                     */
                }
                if (self->state== OT_STATE_CANCEL || self->state == OT_STATE_STOP || self->state == OT_STATE_SUSPEND) {
                    Pthread_mutex_unlock(&self->mtx);
                    Pthread_rwlock_unlock(&self->telescope_rwlock);
                    if (root_json != NULL) {
                        cJSON_Delete(root_json);
                    }
                    if (pthread_equal(dome_tid, pthread_self()) == 0) {
                        Pthread_join(dome_tid, NULL);
                        free(dome_arg);
                    }
                    return ret;
                }
                Pthread_mutex_unlock(&self->mtx);
                Pthread_rwlock_unlock(&self->telescope_rwlock);
                if (root_json != NULL) {
                    cJSON_Delete(root_json);
                }
                if (pthread_equal(dome_tid, pthread_self()) == 0) {
                    Pthread_join(dome_tid, NULL);
                    free(dome_arg);
                }
                goto end;
            } else {
                Pthread_mutex_lock(&self->mtx);
                if (self->state == OT_STATE_CANCEL) {
                    /*
                     * TODO, close dome, go home.
                     */
                }
                if (self->state== OT_STATE_CANCEL || self->state == OT_STATE_STOP || self->state == OT_STATE_SUSPEND) {
                    Pthread_mutex_unlock(&self->mtx);
                    Pthread_rwlock_unlock(&self->telescope_rwlock);
                    if (root_json != NULL) {
                        cJSON_Delete(root_json);
                    }
                    if (pthread_equal(dome_tid, pthread_self()) == 0) {
                        Pthread_join(dome_tid, NULL);
                        free(dome_arg);
                    }
                    return ret;
                }
                Pthread_mutex_unlock(&self->mtx);
            }
            if (self->dome != NULL && (ret = dome_status(self->dome, status, BUFSIZE, NULL)) != AAOS_OK) {
                status_json = cJSON_Parse(status);
                if (root_json == NULL) {
                    root_json = cJSON_CreateObject();
                }
                cJSON_AddItemToObject(root_json, "DOME-STATUS", status_json);
            }
            if (self->telescope != NULL && (ret = telescope_status(self->telescope, status, BUFSIZE, NULL)) != AAOS_OK) {
                status_json = cJSON_Parse(status);
                if (root_json == NULL) {
                    root_json = cJSON_CreateObject();
                }
                cJSON_AddItemToObject(root_json, "TELESCOPE-STATUS", status_json);
            }
        } else {
            Pthread_rwlock_rdlock(&self->scheduler_rwlock);
            if (task_id != 0) {
                if (self->scheduler != NULL && (ret = scheduler_update_task_status(self->scheduler, task_id, SCHEDULER_STATUS_INCOMPLETE)) != AAOS_OK) {
#ifdef DEBUG
                    fprintf(stderr, "%s %s %d: scheduler_update_task_status error, %d\n", __FILE__, __func__, __LINE__ - 2, ret);
#endif
                }
            }
            Pthread_rwlock_unlock(&self->scheduler_rwlock);
            Pthread_rwlock_unlock(&self->telescope_rwlock);
            if (root_json != NULL) {
                cJSON_Delete(root_json);
            }
            if (pthread_equal(dome_tid, pthread_self()) == 0) {
                Pthread_join(dome_tid, NULL);
                free(dome_arg);
            }
            ret = AAOS_EBADMSG;
            goto end;
        }
    }
    Pthread_rwlock_unlock(&self->telescope_rwlock);
    
    if (pthread_equal(dome_tid, pthread_self()) == 0) {
        Pthread_join(dome_tid, NULL);
        free(dome_arg);
    }
    
    Pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_testcancel();
    Pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
    
    Pthread_rwlock_rdlock(&self->aws_rwlock);
    if (self->has_aws && self->aws != NULL) {
        if (root_json == NULL) {
            root_json = cJSON_CreateObject();
            site_json = cJSON_CreateObject();
            cJSON_AddItemToObject(root_json, "SITE-INFO", site_json);
        } else {
            if ((site_json = cJSON_GetObjectItemCaseSensitive(root_json, "SITE-INFO")) == NULL) {
                site_json = cJSON_CreateObject();
                cJSON_AddItemToObject(root_json, "SITE-INFO", site_json);
            }
        }
        __ObservationThread_format_aws_data(self, site_json);
    }
    Pthread_rwlock_unlock(&self->aws_rwlock);
    
    if (root_json != NULL) {
        header = cJSON_Print(root_json);
    }

    Pthread_rwlock_rdlock(&self->detector_rwlock);
    if (self->has_detector) {
        if (self->detector != NULL && (ret = detector_get_index_by_name(self->detector, detname)) != AAOS_OK) {
            if (task_id != 0) {
                Pthread_rwlock_rdlock(&self->scheduler_rwlock);
                if (self->scheduler != NULL && (ret = scheduler_update_task_status(self->scheduler, task_id, SCHEDULER_STATUS_INCOMPLETE)) != AAOS_OK) {
#ifdef DEBUG
                    fprintf(stderr, "%s %s %d: scheduler_update_task_status error, %d\n", __FILE__, __func__, __LINE__ - 2, ret);
#endif
                }
                Pthread_rwlock_unlock(&self->scheduler_rwlock);
            }
            Pthread_rwlock_unlock(&self->detector_rwlock);
            if (root_json != NULL) {
                cJSON_Delete(root_json);
            }
            free(header);
            return ret;
        }
        Pthread_mutex_lock(&self->mtx);
        self->state = OT_STATE_EXPOSE;
        Pthread_mutex_unlock(&self->mtx);
        protobuf_set(self->detector, PACKET_BUF, strlen(header) + 1);
        if (self->detector != NULL && (ret = detector_expose(self->detector, exptime, nframes, NULL, NULL)) != AAOS_OK) {
#ifdef DEBUG
            fprintf(stderr, "%s %s %d: detector_expose eorr, %d\n", __FILE__, __func__, __LINE__ - 2, ret);
#endif
            if (ret == AAOS_EPIPE) {
                delete(self->detector);
                self->detector = NULL;
            }
            if (task_id != 0) {
                Pthread_rwlock_rdlock(&self->scheduler_rwlock);
                if (self->scheduler != NULL && (ret = scheduler_update_task_status(self->scheduler, task_id, SCHEDULER_STATUS_INCOMPLETE)) != AAOS_OK) {
#ifdef DEBUG
                    fprintf(stderr, "%s %s %d: scheduler_update_task_status error, %d\n", __FILE__, __func__, __LINE__ - 2, ret);
#endif
                }
                Pthread_rwlock_unlock(&self->scheduler_rwlock);
            }
            Pthread_mutex_lock(&self->mtx);
            if (self->state == OT_STATE_CANCEL) {
                    /*
                     * TODO, close dome, go home.
                     */
            }
            if (self->state== OT_STATE_CANCEL || self->state == OT_STATE_STOP || self->state == OT_STATE_SUSPEND) {
                Pthread_mutex_unlock(&self->mtx);
                Pthread_rwlock_unlock(&self->detector_rwlock);
                if (root_json != NULL) {
                    cJSON_Delete(root_json);
                }
                free(header);
                return ret;
            }
            Pthread_mutex_unlock(&self->mtx);
            Pthread_rwlock_unlock(&self->detector_rwlock);
            if (root_json != NULL) {
                cJSON_Delete(root_json);
            }
            free(header);
            goto end;
        } else {
            if (task_id != 0) {
                Pthread_rwlock_rdlock(&self->scheduler_rwlock);
                if (self->scheduler != NULL && (ret = scheduler_update_task_status(self->scheduler, task_id, SCHEDULER_STATUS_COMPLETE)) != AAOS_OK) {
#ifdef DEBUG
                    fprintf(stderr, "%s %s %d: scheduler_update_task_status error, %d\n", __FILE__, __func__, __LINE__ - 2, ret);
#endif
                }
                Pthread_rwlock_unlock(&self->scheduler_rwlock);
            }
        }
    }
    Pthread_rwlock_unlock(&self->detector_rwlock);
    
    if (root_json != NULL) {
        cJSON_Delete(root_json);
    }
    free(header);
    
end:
    Pthread_mutex_lock(&self->mtx);
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


static void *
__ObservationThread_thr(void *_self)
{
    int ret;
        
    for (; ;) {
        ret = __observation_thread_cycle(_self);
    }

    return NULL;
}

static int
__ObservationThread_start(void *_self)
{
    struct __ObservationThread *self = cast(__ObservationThread(), _self);
    
    int ret;
    
    Pthread_mutex_lock(&self->mtx);
    if (pthread_kill(self->tid, 0) == ESRCH) {
        self->tid = pthread_self();
        self->state = OT_STATE_IDLE;
    } else {
        Pthread_mutex_unlock(&self->mtx);
        return AAOS_EALREADY;
    }
    Pthread_mutex_unlock(&self->mtx);

    Pthread_create(&self->tid, NULL, __ObservationThread_thr, _self);
    
    return AAOS_OK;
}



int
__observation_thread_stop(void *_self, uint32_t flag)
{
    const struct __ObservationThreadClass *class = (const struct __ObservationThreadClass *) classOf(_self);
    
    if (isOf(class, __ObservationThreadClass()) && class->stop.method) {
        return ((int (*)(void *, uint32_t)) class->stop.method)(_self, flag);
    } else {
        int result;
        forward(_self, &result, (Method) __observation_thread_stop, "stop", _self, flag);
        return result;
    }
}

static int
__ObservationThread_stop(void *_self, uint32_t flag)
{
    struct __ObservationThread *self = cast(__ObservationThread(), _self);
    
    void *telescope = NULL, *detector = NULL;
    int ret;
    
    Pthread_mutex_lock(&self->mtx);
	if (self->state == OT_STATE_SLEW) {
		if (self->has_telescope && self->telescope_client != NULL && (ret = rpc_client_connect(self->telescope_client, &telescope)) == AAOS_OK) {
            telescope_stop(telescope);
            delete(telescope);
		}
	} else if (self->state == OT_STATE_EXPOSE) {
		if (self->has_detector && self->detector_client != NULL && (ret = rpc_client_connect(self->detector_client, &detector)) == AAOS_OK) {
            if (flag == OT_FLAG_L0) {
                detector_abort(self->detector);
            } else if (flag == OT_FLAG_L1) {
                detector_stop(self->detector);
            }
            delete(detector);
		}
	}
    self->state = OT_STATE_STOP;
    self->flag = flag;
    Pthread_mutex_lock(&self->mtx);

    return AAOS_OK;
}

int
__observation_thread_suspend(void *_self, uint32_t flag)
{
    const struct __ObservationThreadClass *class = (const struct __ObservationThreadClass *) classOf(_self);
    
    if (isOf(class, __ObservationThreadClass()) && class->suspend.method) {
        return ((int (*)(void *, uint32_t)) class->suspend.method)(_self, flag);
    } else {
        int result;
        forward(_self, &result, (Method) __observation_thread_suspend, "suspend", _self, flag);
        return result;
    }
}

int
__ObservationThread_suspend(void *_self, uint32_t flag)
{
    struct __ObservationThread *self = cast(__ObservationThread(), _self);
    
    Pthread_mutex_lock(&self->mtx);
    if (self->state != OT_STATE_CANCEL && self->state != OT_STATE_STOP) {
        self->state = OT_STATE_SUSPEND;
    }
    self->flag = flag;
    Pthread_mutex_unlock(&self->mtx);
    
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
__ObservationThread_resume(void *_self)
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
__observation_thread_cancel(void *_self, uint32_t flag)
{
    const struct __ObservationThreadClass *class = (const struct __ObservationThreadClass *) classOf(_self);
    
    if (isOf(class, __ObservationThreadClass()) && class->cancel.method) {
        return ((int (*)(void *, uint32_t)) class->cancel.method)(_self, flag);
    } else {
        int result;
        forward(_self, &result, (Method) __observation_thread_cancel, "cancel", _self, flag);
        return result;
    }
}

static int
__ObservationThread_cancel(void *_self, uint32_t flag)
{
    struct __ObservationThread *self = cast(__ObservationThread(), _self);
    
    void *telescope = NULL, *detector = NULL;
    int ret;
    
    Pthread_mutex_lock(&self->mtx);
    if (self->state == OT_STATE_SLEW) {
        if (self->has_telescope && self->telescope_client != NULL && (ret = rpc_client_connect(self->telescope_client, &telescope)) == AAOS_OK) {
            telescope_stop(telescope);
            delete(telescope);
        }
    } else if (self->state == OT_STATE_EXPOSE) {
        if (self->has_detector && self->detector_client != NULL && (ret = rpc_client_connect(self->detector_client, &detector)) == AAOS_OK) {
            if (flag == OT_FLAG_L0) {
                if ((ret = detector_abort(self->detector)) == AAOS_ENOTSUP) {
                    detector_stop(self->detector);
                }
            } else if (flag == OT_FLAG_L1) {
                detector_stop(self->detector);
            }
            delete(detector);
        }
    }
    self->state = OT_STATE_CANCEL;
    self->flag = flag;
    Pthread_mutex_unlock(&self->mtx);

    return AAOS_OK;
}

int
__observation_thread_terminate(void *_self)
{
    const struct __ObservationThreadClass *class = (const struct __ObservationThreadClass *) classOf(_self);
    
    if (isOf(class, __ObservationThreadClass()) && class->cancel.method) {
        return ((int (*)(void *)) class->cancel.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) __observation_thread_terminate, "terminal", _self);
        return result;
    }
}

static int
__ObservationThread_terminate(void *_self)
{
    struct __ObservationThread *self = cast(__ObservationThread(), _self);
    
    int ret = AAOS_OK;
    
    if (Pthread_cancel(self->tid) == 0) {
        Pthread_join(self->tid, NULL);
        Pthread_mutex_lock(&self->mtx);
        self->state = OT_STATE_TERMINATE;
        Pthread_mutex_unlock(&self->mtx);
    } else {
        ret = AAOS_ESRCH;
    }

    return ret;
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
    } else if (strcmp(name, "dome") == 0) {
		void **value = va_arg(*app, void **);
		*value = self->dome;
		return;
    } else if (strcmp(name, "telescope") == 0) {
		void **value = va_arg(*app, void **);
		*value = self->telescope;
		return;
    } else if (strcmp(name, "detetcor") == 0) {
		void **value = va_arg(*app, void **);
		*value = self->detector;
		return;
    } else if (strcmp(name, "pipeline") == 0) {
		void **value = va_arg(*app, void **);
		*value = self->pipeline;
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
        Pthread_rwlock_wrlock(&self->scheduler_rwlock);
        if (addr != NULL) {
            self->scheduler_addr = (char *) Realloc(self->scheduler_addr, strlen(addr) + 1);
            snprintf(self->scheduler_addr, strlen(addr) + 1, "%s", addr);
        } else {
            if (self->scheduler_addr == NULL) {
                self->scheduler_addr = (char *) Realloc(self->scheduler_addr, ADDRSIZE);
                snprintf(self->scheduler_addr, ADDRSIZE, "localahost");
            }
        }
        if (port != NULL) {
            self->scheduler_port = (char *) Realloc(self->scheduler_port, strlen(port) + 1);
            snprintf(self->scheduler_port, strlen(port) + 1, "%s", port);
        } else {
            if (self->scheduler_port == NULL) {
                self->scheduler_port = (char *) Realloc(self->scheduler_port, PORTSIZE);
                snprintf(self->scheduler_port, PORTSIZE, "%s", SCHEDULER_RPC_SITE_PORT);
            }
        }
        if (self->scheduler_client != NULL) {
            delete(self->scheduler_client);
        }
        self->scheduler_client = new(SchedulerClient(), self->scheduler_addr, self->scheduler_port);
        if (self->scheduler != NULL) {
            delete(self->scheduler);
        }
        if (self->scheduler2 != NULL) {
            delete(self->scheduler2);
        }
        rpc_client_connect(self->scheduler_client, &self->scheduler);
        rpc_client_connect(self->scheduler_client, &self->scheduler2);
        self->has_scheduler = true;
        Pthread_rwlock_unlock(&self->scheduler_rwlock);
    } else if (strcmp(name, "dome") == 0) {
        const char *addr, *port, *dome_name;
        addr = va_arg(*app, const char *);
        port = va_arg(*app, const char *);
        dome_name = va_arg(*app, const char *);
        Pthread_rwlock_wrlock(&self->dome_rwlock);
        if (addr != NULL) {
            self->dome_addr = (char *) Realloc(self->dome_addr, strlen(addr) + 1);
            snprintf(self->dome_addr, strlen(addr) + 1, "%s", addr);
        } else {
            if (self->dome_addr == NULL) {
                self->dome_addr = (char *) Realloc(self->dome_addr, ADDRSIZE);
                snprintf(self->dome_addr, ADDRSIZE, "localahost");
            }
        }
        if (port != NULL) {
            self->dome_port = (char *) Realloc(self->dome_port, strlen(port) + 1);
            snprintf(self->dome_port, strlen(port) + 1, "%s", port);
        } else {
            if (self->dome_port == NULL) {
                self->dome_port = (char *) Realloc(self->dome_port, PORTSIZE);
                snprintf(self->dome_port, PORTSIZE, "%s", DOM_RPC_PORT);
            }
        }
        if (dome_name != NULL) {
            self->dome_name = (char *) Realloc(self->dome_name, strlen(dome_name) + 1);
            snprintf(self->dome_name, strlen(dome_name) + 1, "%s", dome_name);
        }
        if (self->dome_client != NULL) {
            delete(self->dome_client);
        }
        self->dome_client = new(DomeClient(), self->dome_addr, self->dome_port);
        if (self->dome != NULL) {
            delete(self->dome);
        }
        if (rpc_client_connect(self->dome_client, &self->dome) == AAOS_OK) {
            if (self->dome_name != NULL) {
                dome_get_index_by_name(self->dome, self->dome_name);
            } else {
                protobuf_set(self->dome, PACKET_INDEX, 1);
            }
        }
        self->has_dome = true;
        Pthread_rwlock_unlock(&self->dome_rwlock);
    } else if (strcmp(name, "telescope") == 0) {
        const char *addr, *port, *telescope_name;
        uint64_t telescope_identifier;
        addr = va_arg(*app, const char *);
        port = va_arg(*app, const char *);
        telescope_name = va_arg(*app, const char *);
        telescope_identifier = va_arg(*app, uint64_t);
        Pthread_rwlock_wrlock(&self->telescope_rwlock);
        if (addr != NULL) {
            self->telescope_addr = (char *) Realloc(self->telescope_addr, strlen(addr) + 1);
            snprintf(self->telescope_addr, strlen(addr) + 1, "%s", addr);
        } else {
            if (self->telescope_addr == NULL) {
                self->telescope_addr = (char *) Realloc(self->telescope_addr, ADDRSIZE);
                snprintf(self->telescope_addr, ADDRSIZE, "localahost");
            }
        }
        if (port != NULL) {
            self->telescope_port = (char *) Realloc(self->telescope_port, strlen(port) + 1);
            snprintf(self->telescope_port, strlen(port) + 1, "%s", port);
        } else {
            if (self->telescope_port == NULL) {
                self->telescope_port = (char *) Realloc(self->telescope_port, PORTSIZE);
                snprintf(self->telescope_port, PORTSIZE, "%s", TEL_RPC_PORT);
            }
        }
        if (telescope_name != NULL) {
            self->telescope_name = (char *) Realloc(self->telescope_name, strlen(telescope_name) + 1);
            snprintf(self->telescope_name, strlen(telescope_name) + 1, "%s", telescope_name);
        }
        self->telescope_identifier = telescope_identifier;
        if (self->telescope_client != NULL) {
            delete(self->telescope_client);
        }
        self->telescope_client = new(DomeClient(), self->telescope_addr, self->telescope_port);
        if (self->telescope != NULL) {
            delete(self->telescope);
        }
        if (rpc_client_connect(self->telescope_client, &self->telescope) == AAOS_OK) {
            if (self->telescope_name != NULL) {
                telescope_get_index_by_name(self->telescope, self->telescope_name);
            } else {
                protobuf_set(self->telescope, PACKET_INDEX, 1);
            }
        }
        self->has_telescope = true;
        Pthread_rwlock_unlock(&self->telescope_rwlock);
    } else if (strcmp(name, "detector") == 0) {
        const char *addr, *port, *detector_name;
        addr = va_arg(*app, const char *);
        port = va_arg(*app, const char *);
        detector_name = va_arg(*app, const char *);
        Pthread_rwlock_wrlock(&self->detector_rwlock);
        if (addr != NULL) {
            self->detector_addr = (char *) Realloc(self->detector_addr, strlen(addr) + 1);
            snprintf(self->detector_addr, strlen(addr) + 1, "%s", addr);
        } else {
            if (self->detector_addr == NULL) {
                self->detector_addr = (char *) Realloc(self->detector_addr, ADDRSIZE);
                snprintf(self->detector_addr, ADDRSIZE, "localahost");
            }
        }
        if (port != NULL) {
            self->detector_port = (char *) Realloc(self->detector_port, strlen(port) + 1);
            snprintf(self->detector_port, strlen(port) + 1, "%s", port);
        } else {
            if (self->detector_port == NULL) {
                self->detector_port = (char *) Realloc(self->detector_port, PORTSIZE);
                snprintf(self->detector_port, PORTSIZE, "%s", DET_RPC_PORT);
            }
        }
        if (detector_name != NULL) {
            self->detector_name = (char *) Realloc(self->detector_name, strlen(detector_name) + 1);
            snprintf(self->detector_name, strlen(detector_name) + 1, "%s", detector_name);
        }
        if (self->detector_client != NULL) {
            delete(self->detector_client);
        }
        self->detector_client = new(DomeClient(), self->detector_addr, self->detector_port);
        if (self->detector != NULL) {
            delete(self->detector);
        }
        if (rpc_client_connect(self->detector_client, &self->detector) == AAOS_OK) {
            if (self->detector_name != NULL) {
                detector_get_index_by_name(self->detector, self->detector_name);
            } else {
                protobuf_set(self->detector, PACKET_INDEX, 1);
            }
        }
        self->has_detector = true;
        Pthread_rwlock_unlock(&self->detector_rwlock);
    } else if (strcmp(name, "aws") == 0) {
        const char *addr, *port, *aws_name;
        addr = va_arg(*app, const char *);
        port = va_arg(*app, const char *);
        aws_name = va_arg(*app, const char *);
        
        Pthread_rwlock_wrlock(&self->aws_rwlock);
        if (addr != NULL) {
            self->aws_addr = (char *) Realloc(self->aws_addr, strlen(addr) + 1);
            snprintf(self->aws_addr, strlen(addr) + 1, "%s", addr);
        } else {
            if (self->aws_addr == NULL) {
                self->aws_addr = (char *) Realloc(self->aws_addr, ADDRSIZE);
                snprintf(self->aws_addr, ADDRSIZE, "localahost");
            }
        }
        if (port != NULL) {
            self->aws_port = (char *) Realloc(self->aws_port, strlen(port) + 1);
            snprintf(self->detector_port, strlen(port) + 1, "%s", port);
        } else {
            if (self->aws_port == NULL) {
                self->aws_port = (char *) Realloc(self->aws_port, PORTSIZE);
                snprintf(self->aws_port, PORTSIZE, "%s", AWS_RPC_PORT);
            }
        }
        if (aws_name != NULL) {
            self->aws_name = (char *) Realloc(self->aws_name, strlen(aws_name) + 1);
            snprintf(self->aws_name, strlen(aws_name) + 1, "%s", aws_name);
        }
        if (self->aws_client != NULL) {
            delete(self->aws_client);
        }
        self->aws_client = new(AWSClient(), self->aws_addr, self->aws_port);
        if (self->aws != NULL) {
            delete(self->aws);
        }
        if (rpc_client_connect(self->aws_client, &self->aws) == AAOS_OK) {
            if (self->aws_name != NULL) {
                aws_get_index_by_name(self->aws, self->aws_name);
            } else {
                protobuf_set(self->aws, PACKET_INDEX, 1);
            }
        }
        self->aws_keyname = va_arg(*app, char **);
        self->aws_keyvalue = va_arg(*app, char **);
        self->n_aws_keypair = va_arg(*app, size_t);   
        self->has_aws = true;
        Pthread_rwlock_unlock(&self->aws_rwlock);
    } else if (strcmp(name, "pipeline") == 0) {
        const char *addr, *port;
        addr = va_arg(*app, const char *);
        port = va_arg(*app, const char *);
        Pthread_rwlock_wrlock(&self->pipeline_rwlock);
        if (addr != NULL) {
            self->pipeline_addr = (char *) Realloc(self->pipeline_addr, strlen(addr) + 1);
            snprintf(self->pipeline_addr, strlen(addr) + 1, "%s", addr);
        } else {
            if (self->pipeline_addr == NULL) {
                self->pipeline_addr = (char *) Realloc(self->pipeline_addr, ADDRSIZE);
                snprintf(self->pipeline_addr, ADDRSIZE, "localahost");
            }
        }
        if (port != NULL) {
            self->pipeline_port = (char *) Realloc(self->pipeline_port, strlen(port) + 1);
            snprintf(self->pipeline_port, strlen(port) + 1, "%s", port);
        } else {
            if (self->pipeline_port == NULL) {
                self->pipeline_port = (char *) Realloc(self->pipeline_port, PORTSIZE);
                snprintf(self->pipeline_port, PORTSIZE, "%s", PIP_RPC_PORT);
            }
        }
        if (self->pipeline_client != NULL) {
            delete(self->pipeline_client);
        }
        self->pipeline_client = new(SchedulerClient(), self->pipeline_addr, self->pipeline_port);
        if (self->pipeline != NULL) {
            delete(self->pipeline);
        }
        rpc_client_connect(self->pipeline_client, &self->pipeline);
        self->has_pipeline = true;
        Pthread_rwlock_unlock(&self->pipeline_rwlock);
    }
}

const char *
__observation_thread_get_name(void *_self)
{
    const struct __ObservationThreadClass *class = (const struct __ObservationThreadClass *) classOf(_self);
    
    if (isOf(class, __ObservationThreadClass()) && class->get_name.method) {
        return ((const char * (*)(void *)) class->get_name.method)(_self);
    } else {
        const char *result;
        forward(_self, &result, (Method) __observation_thread_get_name, "get_name", _self);
        return result;
    }
}

const char *
__ObservationThread_get_name(void *_self)
{
    struct __ObservationThread *self = cast(__ObservationThread(), _self);

    return self->name;
}

static void *
__ObservationThread_ctor(void *_self, va_list *app)
{
    struct __ObservationThread *self = super_ctor(__ObservationThread(), _self, app);

    const char *s, *key, *value, *value2, *value3;

    s = va_arg(*app, const char *);
    if (s) {
        self->name = (char *) Malloc(strlen(s) + 1);
        snprintf(self->name, strlen(s) + 1, "%s", s);
    }

    while ((key = va_arg(*app, const char *))) {
        if (strcmp(key, "description") == 0) {
            value = va_arg(*app, const char *);
            if (value) {
                self->description = (char *) Malloc(strlen(value) + 1);
                snprintf(self->description, strlen(value) + 1, "%s", value);
            }   
            continue;
        }
        if (strcmp(key, "scheduler") == 0) {
            value = va_arg(*app, const char *);
            value2 = va_arg(*app, const char *);
            if (value) {
                self->scheduler_addr = (char *) Malloc(strlen(value) + 1);
                snprintf(self->scheduler_addr, strlen(value) + 1, "%s", value);
            } else {
                self->scheduler_addr = (char *) Malloc(ADDRSIZE);
                snprintf(self->scheduler_addr, ADDRSIZE, "localhost");
            }
            if (value2) {
                self->scheduler_port = (char *) Malloc(strlen(value2) + 1);
                snprintf(self->scheduler_port, strlen(value2) + 1, "%s", value2);
            } else {
                self->scheduler_port = (char *) Malloc(PORTSIZE);
                snprintf(self->scheduler_port, PORTSIZE, "%s", SCHEDULER_RPC_SITE_PORT);
            }
            self->has_scheduler = true;
            continue;
        }
        if (strcmp(key, "telescope") == 0) {
            value = va_arg(*app, const char *);
            value2 = va_arg(*app, const char *);
            value3 = va_arg(*app, const char *);
            if (value) {
                self->telescope_addr = (char *) Malloc(strlen(value) + 1);
                snprintf(self->telescope_addr, strlen(value) + 1, "%s", value);
            } else {
                self->telescope_addr = (char *) Malloc(ADDRSIZE);
                snprintf(self->telescope_addr, ADDRSIZE, "localhost");
            }
            if (value2) {
                self->telescope_port = (char *) Malloc(strlen(value2) + 1);
                snprintf(self->telescope_port, strlen(value2) + 1, "%s", value2);
            } else {
                self->telescope_port = (char *) Malloc(PORTSIZE);
                snprintf(self->telescope_port, PORTSIZE, "%s", TEL_RPC_PORT);
            }
            if (value3) {
                self->telescope_port = (char *) Malloc(strlen(value3) + 1);
                snprintf(self->telescope_name, strlen(value3) + 1, "%s", value3);
            } 
            self->has_telescope = true;
            continue;
        }
        if (strcmp(key, "dome") == 0) {
            value = va_arg(*app, const char *);
            value2 = va_arg(*app, const char *);
            value3 = va_arg(*app, const char *);
            if (value) {
                self->dome_addr = (char *) Malloc(strlen(value) + 1);
                snprintf(self->dome_addr, strlen(value) + 1, "%s", value);
            } else {
                self->dome_addr = (char *) Malloc(ADDRSIZE);
                snprintf(self->dome_addr, ADDRSIZE, "localhost");
            }
            if (value2) {
                self->dome_port = (char *) Malloc(strlen(value2) + 1);
                snprintf(self->dome_port, strlen(value2) + 1, "%s", value2);
            } else {
                self->dome_port = (char *) Malloc(PORTSIZE);
                snprintf(self->dome_port, PORTSIZE, "%s", DOM_RPC_PORT);
            }
            if (value3) {
                self->dome_name = (char *) Malloc(strlen(value3) + 1);
                snprintf(self->dome_name, strlen(value3) + 1, "%s", value3);
            } 
            self->has_dome = true;
            continue;
        }
        if (strcmp(key, "detector") == 0) {
            value = va_arg(*app, const char *);
            value2 = va_arg(*app, const char *);
            value3 = va_arg(*app, const char *);
            if (value) {
                self->detector_addr = (char *) Malloc(strlen(value) + 1);
                snprintf(self->detector_addr, strlen(value) + 1, "%s", value);
            } else {
                self->detector_addr = (char *) Malloc(ADDRSIZE);
                snprintf(self->detector_addr, ADDRSIZE, "localhost");
            }
            if (value2) {
                self->detector_port = (char *) Malloc(strlen(value2) + 1);
                snprintf(self->detector_port, strlen(value2) + 1, "%s", value2);
            } else {
                self->detector_port = (char *) Malloc(PORTSIZE);
                snprintf(self->detector_port, PORTSIZE, "%s", DET_RPC_PORT);
            }
            if (value3) {
                self->detector_name = (char *) Malloc(strlen(value3) + 1);
                snprintf(self->detector_name, strlen(value3) + 1, "%s", value3);
            } 
            self->has_detector = true;
            continue;
        }
        if (strcmp(key, "pipeline") == 0) {
            value = va_arg(*app, const char *);
            value2 = va_arg(*app, const char *);
            if (value) {
                self->pipeline_addr = (char *) Malloc(strlen(value) + 1);
                snprintf(self->pipeline_addr, strlen(value) + 1, "%s", value);
            } else {
                self->pipeline_addr = (char *) Malloc(ADDRSIZE);
                snprintf(self->pipeline_addr, ADDRSIZE, "localhost");
            }
            if (value2) {
                self->pipeline_port = (char *) Malloc(strlen(value2) + 1);
                snprintf(self->pipeline_port, strlen(value2) + 1, "%s", value2);
            } else {
                self->pipeline_port = (char *) Malloc(PORTSIZE);
                snprintf(self->pipeline_port, PORTSIZE, "%s", PIP_RPC_PORT);
            }
            self->has_pipeline = true;
            continue;
        }
        if (strcmp(key, "aws") == 0) {
            value = va_arg(*app, const char *);
            value2 = va_arg(*app, const char *);
            value3 = va_arg(*app, const char *);
            if (value) {
                self->aws_addr = (char *) Malloc(strlen(value) + 1);
                snprintf(self->aws_addr, strlen(value) + 1, "%s", value);
            } else {
                self->aws_addr = (char *) Malloc(ADDRSIZE);
                snprintf(self->aws_addr, ADDRSIZE, "localhost");
            }
            if (value2) {
                self->aws_port = (char *) Malloc(strlen(value2) + 1);
                snprintf(self->aws_port, strlen(value2) + 1, "%s", value2);
            } else {
                self->aws_port = (char *) Malloc(PORTSIZE);
                snprintf(self->aws_port, PORTSIZE, "%s", AWS_RPC_PORT);
            }
            if (value3) {
                self->aws_name = (char *) Malloc(strlen(value3) + 1);
                snprintf(self->aws_name, strlen(value3) + 1, "%s", value3);
            } 
            self->has_aws = true;
            continue;
        }
    }

    self->state = OT_STATE_TERMINATE;
    
    Pthread_rwlock_init(&self->scheduler_rwlock, NULL);
    Pthread_rwlock_init(&self->dome_rwlock, NULL);
    Pthread_rwlock_init(&self->telescope_rwlock, NULL);
    Pthread_rwlock_init(&self->detector_rwlock, NULL);
    Pthread_rwlock_init(&self->aws_rwlock, NULL);
    Pthread_rwlock_init(&self->pipeline_rwlock, NULL);
    
    Pthread_cond_init(&self->cond, NULL);
    Pthread_mutex_init(&self->mtx, NULL);

    return (void *) self;
}

static void *
__ObservationThread_dtor(void *_self)
{
    struct __ObservationThread *self = cast(__ObservationThread(), _self);
    
    size_t i, n = self->n_aws_keypair;
    
    Pthread_mutex_destroy(&self->mtx);
    Pthread_cond_destroy(&self->cond);

    Pthread_rwlock_destroy(&self->pipeline_rwlock);
    Pthread_rwlock_destroy(&self->aws_rwlock);
    Pthread_rwlock_destroy(&self->detector_rwlock);
    Pthread_rwlock_destroy(&self->telescope_rwlock);
    Pthread_rwlock_destroy(&self->dome_rwlock);
    Pthread_rwlock_destroy(&self->scheduler_rwlock);

    if (self->aws_client) {
        delete(self->aws_client);
    }
    if (self->aws) {
        delete(self->aws);
    }
    free(self->aws_addr);
    free(self->aws_port);
    free(self->aws_name);
    if (self->aws_keyname != NULL) {
        for (i = 0; i < n; i++) {
            free(self->aws_keyname[i]);
        }
        free(self->aws_keyname);
    }
    if (self->aws_keyvalue != NULL) {
        for (i = 0; i < n; i++) {
            free(self->aws_keyvalue[i]);
        }
        free(self->aws_keyvalue);
    }
    
    if (self->dome_client) {
        delete(self->dome_client);
    }
    if (self->dome) {
        delete(self->dome);
    }
    free(self->dome_addr);
    free(self->dome_port);
    free(self->dome_name);

    if (self->detector) {
        delete(self->detector);
    }
    if (self->detector_client) {
        delete(self->detector_client);
    }
    free(self->detector_addr);
    free(self->detector_port);
    free(self->detector_name);

    if (self->scheduler) {
        delete(self->scheduler);
    }
    if (self->scheduler2) {
        delete(self->scheduler2);
    }
    free(self->scheduler_addr);
    free(self->scheduler_port);
    
    free(self->description);
    free(self->name);

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
        if (selector == (Method) __observation_thread_terminate) {
            if (tag) {
                self->terminate.tag = tag;
                self->terminate.selector = selector;
            }
            self->terminate.method = method;
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
        if (selector == (Method) __observation_thread_get_name) {
            if (tag) {
                self->get_name.tag = tag;
                self->get_name.selector = selector;
            }
            self->get_name.method = method;
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
                               
                               __observation_thread_cycle, "cycle", __ObservationThread_cycle,
                               __observation_thread_start, "start", __ObservationThread_start,
                               __observation_thread_cancel, "cancel", __ObservationThread_cancel,
                               __observation_thread_stop, "stop", __ObservationThread_stop,
                               __observation_thread_suspend, "suspend", __ObservationThread_suspend,
                               __observation_thread_resume, "resume", __ObservationThread_resume,
                               __observation_thread_set_member, "set_member", __ObservationThread_set_member,
                               __observation_thread_get_member, "get_member", __ObservationThread_get_member,
                               __observation_thread_get_name, "get_name", __ObservationThread_get_name, 
                               
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

#ifdef _USE_COMPILER_ATTRIBUTION_

static void __destructor__(void) __attribute__ ((destructor(_THREAD_PRIORITY_)));

static void
__destructor__(void)
{
    __ObservationThread_destroy();
    __ObservationThreadClass_destroy();
}

static void
__constructor__(void)
{
    __ObservationThreadClass_initialize();
    __ObservationThread_initialize();
}

#endif

