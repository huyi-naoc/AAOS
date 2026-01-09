//
//  __detector.c
//  AAOS
//
//  Created by huyi on 18/7/26.
//  Copyright © 2018年 National Astronomical Observatories, Chinese Academy of Sciences. All rights reserved.
//

#include "adt.h"
#include "def.h"
#include "detector_def.h"
#include "detector.h"
#include "detector_r.h"
#include "object.h"
#include "protocol.h"
#include "rpc.h"
#include "virtual.h"
#include "wrapper.h"

#include <fitsio2.h>
#include <cjson/cJSON.h>

struct DetectorDataFrame {
    void *buffer;
    struct timespec tp;
    size_t length;
    uint32_t pixel_format;
    size_t i;
    size_t n;
    size_t width;
    size_t height;
    size_t n_chip;
    size_t n_channel;
    size_t x_overscan;
    size_t y_overscan;
};

static void
DetectorDataFrame_cleanup(void *data)
{
    struct DetectorDataFrame *mydata = (struct DetectorDataFrame *) data;

    free(mydata->buffer);
    free(mydata);
}

static int
__Detector_default_pre_acquisition(void *_self, const char *filename, ...)
{
    struct __Detector *self = cast(__Detector(), _self);
    int ret = AAOS_OK, status = 0;
    struct timespec tp;
    struct tm tm_buf;
    char buf[TIMESTAMPSIZE];
    fitsfile *fptr;
    
    va_list ap;
    va_start(ap, filename);
    fptr = va_arg(ap, fitsfile *);
    va_end(ap);
    
    Clock_gettime(CLOCK_REALTIME, &tp);
    gmtime_r(&tp.tv_sec, &tm_buf);
    strftime(buf, TIMESTAMPSIZE, "%Y-%m-%d", &tm_buf);
    fits_update_key_str(fptr, "DATE-OBS", buf, NULL, &status);
    strftime(buf, TIMESTAMPSIZE, "%H:%m:%d", &tm_buf);
    snprintf(buf + strlen(buf), TIMESTAMPSIZE - strlen(buf), ".%03d", (int) floor(tp.tv_nsec / 1000000));
    fits_update_key_str(fptr, "TIME-OBS", buf, NULL, &status);
    
    return ret;
}

static void
__detector_json_string_to_header(fitsfile *fptr, const char *json_string)
{
    cJSON *root_json, *general_json, *telescope_json, *site_setting, *value_json;
    int status = 0;
    
    if ((root_json = cJSON_Parse(json_string)) == NULL) {
        return;
    }
    
    if ((general_json = cJSON_GetObjectItemCaseSensitive(root_json, "GENERAL-INFO")) != NULL) {
        if ((value_json = (cJSON_GetObjectItemCaseSensitive(general_json, "task_id"))) != NULL && cJSON_IsNumber(value_json)) {
            fits_update_key_lng(fptr, "TASK_ID", (long) value_json->valueint, NULL, &status);
        }
        if ((value_json = (cJSON_GetObjectItemCaseSensitive(general_json, "category"))) != NULL && cJSON_IsString(value_json) && value_json->valuestring != NULL) {
            fits_update_key_str(fptr, "CATEGORY", value_json->string, NULL, &status);
        }
        
    }
    
    if ((general_json = cJSON_GetObjectItemCaseSensitive(root_json, "TELESCOPE-INFO")) != NULL) {
        if ((value_json = (cJSON_GetObjectItemCaseSensitive(general_json, "telescop"))) != NULL && cJSON_IsString(value_json) && value_json->valuestring != NULL) {
            fits_update_key_str(fptr, "TELESCOP", value_json->string, NULL, &status);
        }
        if ((value_json = (cJSON_GetObjectItemCaseSensitive(general_json, "tel_id"))) != NULL && cJSON_IsNumber(value_json)) {
            fits_update_key_lng(fptr, "TEL_ID", (long) value_json->valueint, NULL, &status);
        }
        if ((value_json = (cJSON_GetObjectItemCaseSensitive(general_json, "ra"))) != NULL && cJSON_IsNumber(value_json)) {
            fits_update_key_fixdbl(fptr, "RA", value_json->valuedouble, 6, NULL, &status);
        }
        if ((value_json = (cJSON_GetObjectItemCaseSensitive(general_json, "dec"))) != NULL && cJSON_IsNumber(value_json)) {
            fits_update_key_fixdbl(fptr, "DEC", value_json->valuedouble, 6, NULL, &status);
        }
        if ((value_json = (cJSON_GetObjectItemCaseSensitive(general_json, "tel_ra"))) != NULL && cJSON_IsNumber(value_json)) {
            fits_update_key_fixdbl(fptr, "TEL_RA", value_json->valuedouble, 6, NULL, &status);
        }
        if ((value_json = (cJSON_GetObjectItemCaseSensitive(general_json, "tel_dec"))) != NULL && cJSON_IsNumber(value_json)) {
            fits_update_key_fixdbl(fptr, "TEL_DEC", value_json->valuedouble, 6, NULL, &status);
        }
        if ((value_json = (cJSON_GetObjectItemCaseSensitive(general_json, "instrume"))) != NULL && cJSON_IsString(value_json) && value_json->valuestring != NULL) {
            fits_update_key_str(fptr, "INSTRUME", value_json->string, NULL, &status);
        }
        if ((value_json = (cJSON_GetObjectItemCaseSensitive(general_json, "mode"))) != NULL && cJSON_IsString(value_json) && value_json->valuestring != NULL) {
            fits_update_key_str(fptr, "MODE", value_json->string, NULL, &status);
        }
        if ((value_json = (cJSON_GetObjectItemCaseSensitive(general_json, "detname"))) != NULL && cJSON_IsString(value_json) && value_json->valuestring != NULL) {
            fits_update_key_str(fptr, "DETNAME", value_json->string, NULL, &status);
        }
        if ((value_json = (cJSON_GetObjectItemCaseSensitive(general_json, "filter"))) != NULL && cJSON_IsString(value_json) && value_json->valuestring != NULL) {
            fits_update_key_str(fptr, "FILTER", value_json->string, NULL, &status);
        }
        if ((value_json = (cJSON_GetObjectItemCaseSensitive(general_json, "grating"))) != NULL && cJSON_IsString(value_json) && value_json->valuestring != NULL) {
            fits_update_key_str(fptr, "GRATING", value_json->string, NULL, &status);
        }
        if ((value_json = (cJSON_GetObjectItemCaseSensitive(general_json, "exptime"))) != NULL && cJSON_IsString(value_json)) {
            fits_update_key_fixdbl(fptr, "EXPTIME", value_json->valuedouble, 3, NULL, &status);
        }
    }
    
    if ((general_json = cJSON_GetObjectItemCaseSensitive(root_json, "TARGET-INFO")) != NULL) {
        if ((value_json = (cJSON_GetObjectItemCaseSensitive(general_json, "targname"))) != NULL && cJSON_IsString(value_json) && value_json->valuestring != NULL) {
            fits_update_key_str(fptr, "TARGNAME", value_json->string, NULL, &status);
        }
        if ((value_json = (cJSON_GetObjectItemCaseSensitive(general_json, "targ_id"))) != NULL && cJSON_IsNumber(value_json)) {
            fits_update_key_lng(fptr, "TARG_ID", (long) value_json->valueint, NULL, &status);
        }
        if ((value_json = (cJSON_GetObjectItemCaseSensitive(general_json, "targ_ra"))) != NULL && cJSON_IsNumber(value_json)) {
            fits_update_key_fixdbl(fptr, "TARG_RA", value_json->valuedouble, 6, NULL, &status);
        }
        if ((value_json = (cJSON_GetObjectItemCaseSensitive(general_json, "targ_dec"))) != NULL && cJSON_IsNumber(value_json)) {
            fits_update_key_fixdbl(fptr, "TARG_DEC", value_json->valuedouble, 6, NULL, &status);
        }
        if ((value_json = (cJSON_GetObjectItemCaseSensitive(general_json, "sun_alt"))) != NULL && cJSON_IsNumber(value_json)) {
            fits_update_key_fixdbl(fptr, "SUN_ALT", value_json->valuedouble, 6, NULL, &status);
        }
        if ((value_json = (cJSON_GetObjectItemCaseSensitive(general_json, "sun_az"))) != NULL && cJSON_IsNumber(value_json)) {
            fits_update_key_fixdbl(fptr, "SUN_AZ", value_json->valuedouble, 6, NULL, &status);
        }
        if ((value_json = (cJSON_GetObjectItemCaseSensitive(general_json, "moon_alt"))) != NULL && cJSON_IsNumber(value_json)) {
            fits_update_key_fixdbl(fptr, "MOON_ALT", value_json->valuedouble, 6, NULL, &status);
        }
        if ((value_json = (cJSON_GetObjectItemCaseSensitive(general_json, "moon_az"))) != NULL && cJSON_IsNumber(value_json)) {
            fits_update_key_fixdbl(fptr, "MOON_AZ", value_json->valuedouble, 6, NULL, &status);
        }
        if ((value_json = (cJSON_GetObjectItemCaseSensitive(general_json, "moonphas"))) != NULL && cJSON_IsNumber(value_json)) {
            fits_update_key_fixdbl(fptr, "MOONPHAS", value_json->valuedouble, 6, NULL, &status);
        }
        if ((value_json = (cJSON_GetObjectItemCaseSensitive(general_json, "source"))) != NULL && cJSON_IsString(value_json) && value_json->valuestring != NULL) {
            fits_update_key_longstr(fptr, "SOURCE", value_json->string, NULL, &status);
        }
    }
    if ((general_json = cJSON_GetObjectItemCaseSensitive(root_json, "SITE-INFO")) != NULL) {
        if ((value_json = (cJSON_GetObjectItemCaseSensitive(general_json, "sitename"))) != NULL && cJSON_IsString(value_json) && value_json->valuestring != NULL) {
            fits_update_key_str(fptr, "SITENAME", value_json->string, NULL, &status);
        }
        if ((value_json = (cJSON_GetObjectItemCaseSensitive(general_json, "site_id"))) != NULL && cJSON_IsNumber(value_json)) {
            fits_update_key_lng(fptr, "SITE_ID", (long) value_json->valueint, NULL, &status);
        }
        if ((value_json = (cJSON_GetObjectItemCaseSensitive(general_json, "site_lon"))) != NULL && cJSON_IsString(value_json) && value_json->valuestring != NULL) {
            fits_update_key_str(fptr, "SITE_LON", value_json->string, NULL, &status);
        }
        if ((value_json = (cJSON_GetObjectItemCaseSensitive(general_json, "site_lat"))) != NULL && cJSON_IsString(value_json) && value_json->valuestring != NULL) {
            fits_update_key_str(fptr, "SITE_LAT", value_json->string, NULL, &status);
        }
        if ((value_json = (cJSON_GetObjectItemCaseSensitive(general_json, "site_alt"))) != NULL && cJSON_IsNumber(value_json)) {
            fits_update_key_fixdbl(fptr, "SITE_ALT", value_json->valuedouble, 1, NULL, &status);
        }
        if ((value_json = (cJSON_GetObjectItemCaseSensitive(general_json, "sitetemp"))) != NULL && cJSON_IsNumber(value_json)) {
            fits_update_key_fixdbl(fptr, "SITETEMP", value_json->valuedouble, 2, NULL, &status);
        }
        if ((value_json = (cJSON_GetObjectItemCaseSensitive(general_json, "site_ws"))) != NULL && cJSON_IsNumber(value_json)) {
            fits_update_key_fixdbl(fptr, "WITE_WS", value_json->valuedouble, 2, NULL, &status);
        }
        if ((value_json = (cJSON_GetObjectItemCaseSensitive(general_json, "site_wd"))) != NULL && cJSON_IsNumber(value_json)) {
            fits_update_key_fixdbl(fptr, "SITE_WD", value_json->valuedouble, 2, NULL, &status);
        }
        if ((value_json = (cJSON_GetObjectItemCaseSensitive(general_json, "site_rh"))) != NULL && cJSON_IsNumber(value_json)) {
            fits_update_key_fixdbl(fptr, "SITE_RH", value_json->valuedouble, 2, NULL, &status);
        }
        if ((value_json = (cJSON_GetObjectItemCaseSensitive(general_json, "site_dp"))) != NULL && cJSON_IsNumber(value_json)) {
            fits_update_key_fixdbl(fptr, "SITE_DP", value_json->valuedouble, 2, NULL, &status);
        }
        if ((value_json = (cJSON_GetObjectItemCaseSensitive(general_json, "site_ap"))) != NULL && cJSON_IsNumber(value_json)) {
            fits_update_key_fixdbl(fptr, "SITE_AP", value_json->valuedouble, 2, NULL, &status);
        }
        if ((value_json = (cJSON_GetObjectItemCaseSensitive(general_json, "site_see"))) != NULL && cJSON_IsNumber(value_json)) {
            fits_update_key_fixdbl(fptr, "SITE_SEE", value_json->valuedouble, 2, NULL, &status);
        }
        if ((value_json = (cJSON_GetObjectItemCaseSensitive(general_json, "site_bgk"))) != NULL && cJSON_IsNumber(value_json)) {
            fits_update_key_fixdbl(fptr, "SITE_BGK", value_json->valuedouble, 2, NULL, &status);
        }
    }
    cJSON_free(root_json);
}

static int
__Detector_default_post_acquisition(void *_self, const char *filename, ...)
{
    struct __Detector *self = cast(__Detector(), _self);
    struct timespec tp;
    struct tm tm_buf;
    char buf[TIMESTAMPSIZE];
    
    va_list ap;
    va_start(ap, filename);
    fitsfile *fptr = va_arg(ap, fitsfile *);
    const char *string = va_arg(ap, const char *);
    unsigned int format = va_arg(ap, unsigned int);
    void *rpc = va_arg(ap, void *);
    va_end(ap);

    int status = 0, hdutype, ret = AAOS_OK;
    char *basec, *bname;
    
    basec = strdup(filename);
    bname = basename(basec);
    
    if (fptr != NULL) {
        fits_movabs_hdu(fptr, 1, &hdutype, &status);
        if (!(self->d_state.options&DETECTOR_OPTION_NOTIFY_EACH_COMPLETION)) {
            fits_update_key_lng(fptr, "NEXTEND", self->d_exp.success_frames, NULL, &status);
        } else {
            fits_update_key_lng(fptr, "NEXTEND", 1, NULL, &status);
        }

        Clock_gettime(CLOCK_REALTIME, &tp);
        gmtime_r(&tp.tv_sec, &tm_buf);
        strftime(buf, TIMESTAMPSIZE, "%Y-%m-%d", &tm_buf);
        fits_update_key_str(fptr, "DATE", buf, NULL, &status);
        strftime(buf, TIMESTAMPSIZE, "%H:%m:%d", &tm_buf);
        snprintf(buf + strlen(buf), TIMESTAMPSIZE - strlen(buf), ".%03d", (int) floor(tp.tv_nsec / 1000000));
        fits_update_key_str(fptr, "TIME", buf, NULL, &status);
        fits_update_key_dbl(fptr, "EXPTIME", self->d_param.exposure_time, 3, NULL, &status);
        
        fits_update_key_str(fptr, "FILENAME", bname, NULL, &status);
        if (string != NULL) {
            if (format == DETECTOR_OPTION_STRING_FORMART_JSON) {
                __detector_json_string_to_header(fptr, string);
            }
        }
    }

    if (rpc == NULL) {
        printf("%s\n", bname);
    } else {
        Pthread_mutex_lock(&self->d_exp.mtx);
        if ((rpc != self->d_exp.rpc && self->d_exp.rpc != NULL) || rpc == self->d_exp.rpc) {
            protobuf_set(rpc, PACKET_LENGTH, strlen(bname) + 1);
            protobuf_set(rpc, PACKET_BUF, bname, strlen(bname) + 1);
            ret = rpc_write(rpc);
        } else {
            ret = AAOS_OK;
        }
        Pthread_mutex_unlock(&self->d_exp.mtx);
    }
    
    free(basec);

    return ret;
}

void static
__Detector_default_name_convention(struct __Detector *self, char *buf, size_t size, ...)
{
    FILE *fp;
    static __thread char time_buf[TIMESTAMPSIZE];
    struct timespec tp;
    struct tm tm_buf;
    size_t i, n;
    va_list ap;

    va_start(ap, size);
    i = va_arg(ap, size_t);
    n = va_arg(ap, size_t);
    va_end(ap);
    
    fp = fmemopen(buf, size, "w");
    if (self->d_proc.image_directory != NULL) {
        fprintf(fp, "%s/", self->d_proc.image_directory);
    }
    if (self->d_proc.image_prefix != NULL) {
        fprintf(fp, "%s_", self->d_proc.image_prefix);
    }
    if (self->name != NULL) {
        fprintf(fp, "%s_", self->name);
    }
    if (i == 1 || n == 1) {
        Clock_gettime(CLOCK_REALTIME, &tp);
        gmtime_r(&tp.tv_sec, &tm_buf);
        strftime(time_buf, TIMESTAMPSIZE, "%Y%m%d%H%M%S", &tm_buf);
    }
    if (n == 1) {
        fprintf(fp, "%s.fits", time_buf);
    } else {
        fprintf(fp, "%s_%04lu.fits", time_buf, i);
    }
    
    fclose(fp);
}

/*
 * Detector virtual table.
 */
static void *
__DetectorVirtualTable_ctor(void *_self, va_list *app)
{
    struct __DetectorVirtualTable *self = super_ctor(__DetectorVirtualTable(), _self, app);
    Method selector;
    
    while ((selector = va_arg(*app, Method))) {
        const char *tag = va_arg(*app, const char *);
        Method method = va_arg(*app, Method);
        if (selector == (Method) __detector_power_on) {
            if (tag) {
                self->power_on.tag = tag;
                self->power_on.selector = selector;
            }
            self->power_on.method = method;
            continue;
        }
        if (selector == (Method) __detector_power_off) {
            if (tag) {
                self->power_off.tag = tag;
                self->power_off.selector = selector;
            }
            self->power_off.method = method;
            continue;
        }
        if (selector == (Method) __detector_init) {
            if (tag) {
                self->init.tag = tag;
                self->init.selector = selector;
            }
            self->init.method = method;
            continue;
        }
        if (selector == (Method) __detector_get_binning) {
            if (tag) {
                self->get_binning.tag = tag;
                self->get_binning.selector = selector;
            }
            self->get_binning.method = method;
            continue;
        }
        if (selector == (Method) __detector_set_binning) {
            if (tag) {
                self->set_binning.tag = tag;
                self->set_binning.selector = selector;
            }
            self->set_binning.method = method;
            continue;
        }
        if (selector == (Method) __detector_get_gain) {
            if (tag) {
                self->get_gain.tag = tag;
                self->get_gain.selector = selector;
            }
            self->get_gain.method = method;
            continue;
        }
        if (selector == (Method) __detector_set_gain) {
            if (tag) {
                self->set_gain.tag = tag;
                self->set_gain.selector = selector;
            }
            self->set_gain.method = method;
            continue;
        }
        if (selector == (Method) __detector_get_frame_rate) {
            if (tag) {
                self->get_frame_rate.tag = tag;
                self->get_frame_rate.selector = selector;
            }
            self->get_frame_rate.method = method;
            continue;
        }
        if (selector == (Method) __detector_set_frame_rate) {
            if (tag) {
                self->set_frame_rate.tag = tag;
                self->set_frame_rate.selector = selector;
            }
            self->set_frame_rate.method = method;
            continue;
        }
        if (selector == (Method) __detector_set_exposure_time) {
            if (tag) {
                self->set_exposure_time.tag = tag;
                self->set_exposure_time.selector = selector;
            }
            self->set_exposure_time.method = method;
            continue;
        }
        if (selector == (Method) __detector_get_exposure_time) {
            if (tag) {
                self->get_exposure_time.tag = tag;
                self->get_exposure_time.selector = selector;
            }
            self->get_exposure_time.method = method;
            continue;
        }
        if (selector == (Method) __detector_set_readout_rate) {
            if (tag) {
                self->set_readout_rate.tag = tag;
                self->set_readout_rate.selector = selector;
            }
            self->set_readout_rate.method = method;
            continue;
        }
        if (selector == (Method) __detector_get_readout_rate) {
            if (tag) {
                self->get_readout_rate.tag = tag;
                self->get_readout_rate.selector = selector;
            }
            self->get_readout_rate.method = method;
            continue;
        }
        if (selector == (Method) __detector_set_region) {
            if (tag) {
                self->set_region.tag = tag;
                self->set_region.selector = selector;
            }
            self->set_region.method = method;
            continue;
        }
        if (selector == (Method) __detector_get_region) {
            if (tag) {
                self->get_region.tag = tag;
                self->get_region.selector = selector;
            }
            self->get_region.method = method;
            continue;
        }
        if (selector == (Method) __detector_set_overscan) {
            if (tag) {
                self->set_overscan.tag = tag;
                self->set_overscan.selector = selector;
            }
            self->set_overscan.method = method;
            continue;
        }
        if (selector == (Method) __detector_get_overscan) {
            if (tag) {
                self->get_overscan.tag = tag;
                self->get_overscan.selector = selector;
            }
            self->get_overscan.method = method;
            continue;
        }
        if (selector == (Method) __detector_set_pixel_format) {
            if (tag) {
                self->set_pixel_format.tag = tag;
                self->set_pixel_format.selector = selector;
            }
            self->set_pixel_format.method = method;
            continue;
        }
        if (selector == (Method) __detector_get_pixel_format) {
            if (tag) {
                self->get_pixel_format.tag = tag;
                self->get_pixel_format.selector = selector;
            }
            self->get_pixel_format.method = method;
            continue;
        }
        if (selector == (Method) __detector_set_temperature) {
            if (tag) {
                self->set_temperature.tag = tag;
                self->set_temperature.selector = selector;
            }
            self->set_temperature.method = method;
            continue;
        }
        if (selector == (Method) __detector_get_temperature) {
            if (tag) {
                self->get_temperature.tag = tag;
                self->get_temperature.selector = selector;
            }
            self->get_temperature.method = method;
            continue;
        }
        if (selector == (Method) __detector_expose) {
            if (tag) {
                self->expose.tag = tag;
                self->expose.selector = selector;
            }
            self->expose.method = method;
            continue;
        }
        if (selector == (Method) __detector_raw) {
            if (tag) {
                self->raw.tag = tag;
                self->raw.selector = selector;
            }
            self->raw.method = method;
            continue;
        }
        if (selector == (Method) __detector_inspect) {
            if (tag) {
                self->inspect.tag = tag;
                self->inspect.selector = selector;
            }
            self->inspect.method = method;
            continue;
        }
        if (selector == (Method) __detector_wait) {
            if (tag) {
                self->wait.tag = tag;
                self->wait.selector = selector;
            }
            self->wait.method = method;
            continue;
        }
        if (selector == (Method) __detector_wait_for_completion) {
            if (tag) {
                self->wait_for_completion.tag = tag;
                self->wait_for_completion.selector = selector;
            }
            self->wait_for_completion.method = method;
            continue;
        }
        if (selector == (Method) __detector_status) {
            if (tag) {
                self->status.tag = tag;
                self->status.selector = selector;
            }
            self->status.method = method;
            continue;
        }
        if (selector == (Method) __detector_info) {
            if (tag) {
                self->info.tag = tag;
                self->info.selector = selector;
            }
            self->info.method = method;
            continue;
        }
        if (selector == (Method) __detector_abort) {
            if (tag) {
                self->abort.tag = tag;
                self->abort.selector = selector;
            }
            self->abort.method = method;
            continue;
        }
        if (selector == (Method) __detector_stop) {
            if (tag) {
                self->stop.tag = tag;
                self->stop.selector = selector;
            }
            self->stop.method = method;
            continue;
        }
        if (selector == (Method) __detector_get_temperature) {
            if (tag) {
                self->get_temperature.tag = tag;
                self->get_temperature.selector = selector;
            }
            self->get_temperature.method = method;
            continue;
        }
        if (selector == (Method) __detector_set_temperature) {
            if (tag) {
                self->set_temperature.tag = tag;
                self->set_temperature.selector = selector;
            }
            self->set_temperature.method = method;
            continue;
        }
        if (selector == (Method) __detector_enable_cooling) {
            if (tag) {
                self->enable_cooling.tag = tag;
                self->enable_cooling.selector = selector;
            }
            self->enable_cooling.method = method;
            continue;
        }
        if (selector == (Method) __detector_disable_cooling) {
            if (tag) {
                self->disable_cooling.tag = tag;
                self->disable_cooling.selector = selector;
            }
            self->disable_cooling.method = method;
            continue;
        }
        if (selector == (Method) __detector_set_directory) {
            if (tag) {
                self->set_directory.tag = tag;
                self->set_directory.selector = selector;
            }
            self->set_directory.method = method;
            continue;
        }
        if (selector == (Method) __detector_set_prefix) {
            if (tag) {
                self->set_prefix.tag = tag;
                self->set_prefix.selector = selector;
            }
            self->set_prefix.method = method;
            continue;
        }
        if (selector == (Method) __detector_set) {
            if (tag) {
                self->set.tag = tag;
                self->set.selector = selector;
            }
            self->set.method = method;
            continue;
        }
        if (selector == (Method) __detector_get) {
            if (tag) {
                self->get.tag = tag;
                self->get.selector = selector;
            }
            self->get.method = method;
            continue;
        }
    }
    
    return _self;
}

static void *
__DetectorVirtualTable_dtor(void *_self)
{
    return super_dtor(Object(), _self);
}

static const void *___DetectorVirtualTable;

static void
__DetectorVirtualTable_destroy(void)
{
    free((void *) ___DetectorVirtualTable);
}

static void
__DetectorVirtualTable_initialize(void)
{
    ___DetectorVirtualTable = new(VirtualTableClass(), "__DetectorVirtualTable", VirtualTable(), sizeof(struct __DetectorVirtualTable),
                                  ctor, "ctor", __DetectorVirtualTable_ctor,
                                  dtor, "dtor", __DetectorVirtualTable_dtor,
                                  (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(__DetectorVirtualTable_destroy);
#endif
}

const void *
__DetectorVirtualTable(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, __DetectorVirtualTable_initialize);
#endif
    
    return ___DetectorVirtualTable;
}

/*
 * Detector class method.
 */


void
__detector_get(void *_self, const char *keyname, ...)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    va_list ap;
    va_start(ap, keyname);
    
    if (isOf(class, __DetectorClass()) && class->get.method) {
        ((void (*)(void *, const char *, va_list *)) class->get.method)(_self, keyname, &ap);
    } else {
        forward(_self, (void *) 0, (Method) __detector_get, "get", _self, keyname, &ap);
    }
    va_end(ap);
}

static void
__Detector_get(void *_self, const char *keyname, va_list *app)
{
    struct __Detector *self = cast(__Detector(), _self);


    if (strcmp(keyname, "width") == 0) {
        uint32_t *width = va_arg(*app, uint32_t *);
        *width = self->d_cap.width;
    } else if (strcmp(keyname, "height") == 0) {
        uint32_t *heigth = va_arg(*app, uint32_t *);
        *heigth = self->d_cap.height;
    } else if (strcmp(keyname, "n_chip") == 0) {
        uint32_t *n_chip = va_arg(*app, uint32_t *);
        *n_chip = (uint32_t) self->d_cap.n_chip;
    } else if (strcmp(keyname, "x_n_chip") == 0) {
        uint32_t *x_n_chip = va_arg(*app, uint32_t *);
        *x_n_chip = (uint32_t) self->d_cap.x_n_chip;
    } else if (strcmp(keyname, "y_n_chip") == 0) {
        uint32_t *y_n_chip = va_arg(*app, uint32_t *);
        *y_n_chip = (uint32_t) self->d_cap.y_n_chip;
    } else if (strcmp(keyname, "flip_map") == 0) {
        bool **flip_map = va_arg(*app, bool **);
        uint32_t *x_n_chip = va_arg(*app, uint32_t *);
        uint32_t *y_n_chip = va_arg(*app, uint32_t *);
        *x_n_chip = (uint32_t) self->d_cap.x_n_chip;
        *y_n_chip = (uint32_t) self->d_cap.y_n_chip;
        if (*x_n_chip == 0 || *y_n_chip == 0 || self->d_cap.flip_map == NULL) {
            *flip_map = NULL;
        } else {    
            *flip_map = (bool *) Malloc(sizeof(bool) * *x_n_chip * *y_n_chip);
            memcpy(*flip_map, self->d_cap.flip_map, sizeof(bool) * *x_n_chip * *y_n_chip);
        }
    } else if (strcmp(keyname, "mirror_map") == 0) {
        bool **mirror_map = va_arg(*app, bool **);
        uint32_t *x_n_chip = va_arg(*app, uint32_t *);
        uint32_t *y_n_chip = va_arg(*app, uint32_t *);
        *x_n_chip = (uint32_t) self->d_cap.x_n_chip;
        *y_n_chip = (uint32_t) self->d_cap.y_n_chip;
        if (*x_n_chip == 0 || *y_n_chip == 0 || self->d_cap.mirror_map == NULL) {
            *mirror_map = NULL;
        } else { 
            *mirror_map = (bool *) Malloc(sizeof(bool) * *x_n_chip * *y_n_chip);    
            memcpy(*mirror_map, self->d_cap.flip_map, sizeof(bool) * *x_n_chip * *y_n_chip);
        }
    } else if (strcmp(keyname, "x_binning_min") == 0) {
        uint32_t *x_binning_min = va_arg(*app, uint32_t *);
        *x_binning_min = self->d_cap.x_binning_min;
    } else if (strcmp(keyname, "x_binning_max") == 0) {
        uint32_t *x_binning_max = va_arg(*app, uint32_t *);
        *x_binning_max = self->d_cap.x_binning_max;
    } else if (strcmp(keyname, "x_binning_array") == 0) {
        uint32_t **x_binning_array = va_arg(*app, uint32_t **);
        uint32_t *n_x_binning = va_arg(*app, uint32_t *);
        *n_x_binning = self->d_cap.n_x_binning;
        if (*n_x_binning == 0 || self->d_cap.x_binning_array == 0) {
            *x_binning_array = NULL;
        } else {    
            *x_binning_array = (uint32_t *) Malloc(sizeof(uint32_t) * *n_x_binning);
            memcpy(*x_binning_array, self->d_cap.x_binning_array, sizeof(uint32_t) * *n_x_binning);
        }
    } else if (strcmp(keyname, "y_binning_min") == 0) {
        uint32_t *y_binning_min = va_arg(*app, uint32_t *);
        *y_binning_min = self->d_cap.y_binning_min;
    } else if (strcmp(keyname, "y_binning_max") == 0) {
        uint32_t *y_binning_max = va_arg(*app, uint32_t *);
        *y_binning_max = self->d_cap.y_binning_max;
    } else if (strcmp(keyname, "y_binning_array") == 0) {
        uint32_t **y_binning_array = va_arg(*app, uint32_t **);
        uint32_t *n_y_binning = va_arg(*app, uint32_t *);
        *n_y_binning = self->d_cap.n_y_binning;
	if (*n_y_binning == 0 || self->d_cap.y_binning_array == 0) {
    	    *y_binning_array = NULL;
	} else {    
        *y_binning_array = (uint32_t *) Malloc(sizeof(uint32_t) * *n_y_binning);
	    memcpy(*y_binning_array, self->d_cap.x_binning_array, sizeof(uint32_t) * *n_y_binning);
	}
    } else if (strcmp(keyname, "") == 0) {
	
    }
}

void
__detector_set(void *_self, const char *keyname, ...)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    va_list ap;
    va_start(ap, keyname);
    
    if (isOf(class, __DetectorClass()) && class->set.method) {
        ((void (*)(void *, const char *, va_list *)) class->set.method)(_self, keyname, &ap);
    } else {
        forward(_self, (void *) 0, (Method) __detector_set, "set", _self, keyname, &ap);
    }
    va_end(ap);
}

static void
__Detector_set(void *_self, const char *keyname, va_list *app)
{
    struct __Detector *self = cast(__Detector(), _self);
    
    if (strcmp(keyname, "width") == 0) {
        uint32_t width = va_arg(*app, uint32_t);
        self->d_cap.width = width;
    } else if (strcmp(keyname, "height") == 0) {
        uint32_t height = va_arg(*app, uint32_t);
        self->d_cap.height = height;
    } else if (strcmp(keyname, "n_chip") == 0) {
        uint32_t n_chip = va_arg(*app, uint32_t);
        self->d_cap.n_chip = n_chip;
    } else if (strcmp(keyname, "x_n_chip") == 0) {
        uint32_t x_n_chip = va_arg(*app, uint32_t);
        self->d_cap.x_n_chip = x_n_chip;
    } else if (strcmp(keyname, "y_n_chip") == 0) {
        uint32_t y_n_chip = va_arg(*app, uint32_t);
        self->d_cap.y_n_chip = y_n_chip;
    } else if (strcmp(keyname, "flip_map") == 0) {
        bool *flip_map = va_arg(*app, bool *);
        uint32_t n_chip = va_arg(*app, uint32_t);
        self->d_cap.flip_map = Realloc(self->d_cap.flip_map, sizeof(bool) * n_chip);
        memcpy(self->d_cap.flip_map, flip_map, sizeof(bool) * n_chip);
    } else if (strcmp(keyname, "mirror_map") == 0) {
        bool *mirror_map = va_arg(*app, bool *);
        uint32_t n_chip = va_arg(*app, uint32_t);
        self->d_cap.mirror_map = Realloc(self->d_cap.mirror_map, sizeof(bool) * n_chip);
        memcpy(self->d_cap.mirror_map, mirror_map, sizeof(bool) * n_chip);
    } else if (strcmp(keyname, "binning_available") == 0) {
        bool binning_available = va_arg(*app, int);
        self->d_cap.binning_available = binning_available;
    } else if (strcmp(keyname, "x_binning_max") == 0) {
        double x_binning_max = va_arg(*app, double);
        self->d_cap.x_binning_max = x_binning_max;
    } else if (strcmp(keyname, "x_binning_min") == 0) {
        double x_binning_min = va_arg(*app, double);
        self->d_cap.x_binning_min = x_binning_min;
    } else if (strcmp(keyname, "x_binning_array") == 0) {
        double *x_binning_array = va_arg(*app, double *);
        uint32_t n_x_binning = va_arg(*app, uint32_t);
        self->d_cap.x_binning_array = Realloc(self->d_cap.x_binning_array, sizeof(double) * n_x_binning);
        memcpy(self->d_cap.x_binning_array, x_binning_array, sizeof(double) * n_x_binning);
        self->d_cap.n_x_binning = n_x_binning;
    } else if (strcmp(keyname, "y_binning_max") == 0) {
        double y_binning_max = va_arg(*app, double);
        self->d_cap.y_binning_max = y_binning_max;
    } else if (strcmp(keyname, "y_binning_min") == 0) {
        double y_binning_min = va_arg(*app, double);
        self->d_cap.y_binning_min = y_binning_min;
    } else if (strcmp(keyname, "y_binning_array") == 0) {
        double *y_binning_array = va_arg(*app, double *);
        uint32_t n_y_binning = va_arg(*app, uint32_t);
        self->d_cap.y_binning_array = Realloc(self->d_cap.y_binning_array, sizeof(double) * n_y_binning);
        memcpy(self->d_cap.y_binning_array, y_binning_array, sizeof(double) * n_y_binning);
        self->d_cap.n_y_binning = n_y_binning;
    } else if (strcmp(keyname, "offset_available") == 0) {
        bool offset_available = va_arg(*app, int);
        self->d_cap.offset_available = offset_available;
    } else if (strcmp(keyname, "x_offset_min") == 0) {
        uint32_t x_offset_min = va_arg(*app, uint32_t);
        self->d_cap.x_offset_min = x_offset_min;
    } else if (strcmp(keyname, "x_offset_max") == 0) {
        uint32_t x_offset_max = va_arg(*app, uint32_t);
        self->d_cap.x_offset_max = x_offset_max;
    } else if (strcmp(keyname, "y_offset_min") == 0) {
        uint32_t y_offset_min = va_arg(*app, uint32_t);
        self->d_cap.y_offset_min = y_offset_min;
    } else if (strcmp(keyname, "y_offset_max") == 0) {
        uint32_t y_offset_max = va_arg(*app, uint32_t);
        self->d_cap.y_offset_max = y_offset_max;
    } else if (strcmp(keyname, "image_width_min") == 0) {
        uint32_t image_width_min = va_arg(*app, uint32_t);
        self->d_cap.image_width_min = image_width_min;
    } else if (strcmp(keyname, "image_width_max") == 0) {
        uint32_t image_width_max = va_arg(*app, uint32_t);
        self->d_cap.image_width_max = image_width_max;
    } else if (strcmp(keyname, "image_height_min") == 0) {
        uint32_t image_height_min = va_arg(*app, uint32_t);
        self->d_cap.image_height_min = image_height_min;
    } else if (strcmp(keyname, "image_height_max") == 0) {
        uint32_t image_height_max = va_arg(*app, uint32_t);
        self->d_cap.image_height_max = image_height_max;
    } else if (strcmp(keyname, "frame_rate_available") == 0) {
        bool frame_rate_available = va_arg(*app, int);
        self->d_cap.frame_rate_available = frame_rate_available;
    } else if (strcmp(keyname, "frame_rate_min") == 0) {
        double frame_rate_min = va_arg(*app, double);
        self->d_cap.frame_rate_min = frame_rate_min;
    } else if (strcmp(keyname, "frame_rate_max") == 0) {
        double frame_rate_max = va_arg(*app, double);
        self->d_cap.frame_rate_max = frame_rate_max;
    } else if (strcmp(keyname, "exposure_time_available") == 0) {
        bool exposure_time_available = va_arg(*app, int);
        self->d_cap.exposure_time_available = exposure_time_available;
    } else if (strcmp(keyname, "exposure_time_min") == 0) {
        double exposure_time_min = va_arg(*app, double);
        self->d_cap.exposure_time_min = exposure_time_min;
    } else if (strcmp(keyname, "exposure_time_max") == 0) {
        double exposure_time_max = va_arg(*app, double);
        self->d_cap.exposure_time_max = exposure_time_max;
    } else if (strcmp(keyname, "gain_available") == 0) {
        bool gain_available = va_arg(*app, int);
        self->d_cap.gain_available = gain_available;
    } else if (strcmp(keyname, "gain_min") == 0) {
        double gain_min = va_arg(*app, double);
        self->d_cap.gain_min = gain_min;
    } else if (strcmp(keyname, "gain_max") == 0) {
        double gain_max = va_arg(*app, double);
        self->d_cap.gain_max = gain_max;
    } else if (strcmp(keyname, "gain_array") == 0) {
        double *gain_array = va_arg(*app, double *);
        uint32_t n_gain = va_arg(*app, uint32_t);
        self->d_cap.gain_array = Realloc(self->d_cap.gain_array, sizeof(double) * n_gain);
        memcpy(self->d_cap.gain_array, gain_array, sizeof(double) * n_gain);
        self->d_cap.n_gain = n_gain;
    } else if (strcmp(keyname, "pixel_format_available") == 0) {
        bool pixel_format_available = va_arg(*app, int);
        self->d_cap.pixel_format_available = pixel_format_available;
    } else if (strcmp(keyname, "pixel_format_array") == 0) {
        unsigned int *pixel_format_array = va_arg(*app, unsigned int *);
        uint32_t n_pixel_format = va_arg(*app, uint32_t);
        self->d_cap.pixel_format_array = Realloc(self->d_cap.pixel_format_array, sizeof(unsigned int) * n_pixel_format);
        memcpy(self->d_cap.pixel_format_array, pixel_format_array, sizeof(unsigned int) * n_pixel_format);
        self->d_cap.n_pixel_format = n_pixel_format;
    } else if (strcmp(keyname, "readout_rate_available") == 0) {
        bool readout_rate_available = va_arg(*app, int);
        self->d_cap.readout_rate_available = readout_rate_available;
    } else if (strcmp(keyname, "readout_rate_min") == 0) {
        double readout_rate_min = va_arg(*app, double);
        self->d_cap.readout_rate_min = readout_rate_min;
    } else if (strcmp(keyname, "readout_rate_max") == 0) {
        double readout_rate_max = va_arg(*app, double);
        self->d_cap.readout_rate_max = readout_rate_max;
    } else if (strcmp(keyname, "readout_rate_array") == 0) {
        double *readout_rate_array = va_arg(*app, double *);
        uint32_t n_readout = va_arg(*app, uint32_t);
        self->d_cap.readout_rate_array = Realloc(self->d_cap.readout_rate_array, sizeof(double) * n_readout);
        memcpy(self->d_cap.readout_rate_array, readout_rate_array, sizeof(double) * n_readout);
        self->d_cap.n_readout_rate = n_readout;
    } else if (strcmp(keyname, "cooling_available") == 0) {
        bool cooling_available = va_arg(*app, int);
        self->d_cap.cooling_available = cooling_available;
    } else if (strcmp(keyname, "cooling_temperature_min") == 0) {
        double cooling_temperature_min = va_arg(*app, double);
        self->d_cap.cooling_temperature_min = cooling_temperature_min;
    } else if (strcmp(keyname, "cooling_temperature_max") == 0) {
        double cooling_temperature_max = va_arg(*app, double);
        self->d_cap.cooling_temperature_max = cooling_temperature_max;
    } else if (strcmp(keyname, "binning") == 0) {
        uint32_t x_binning, y_binning;
        x_binning = va_arg(*app, uint32_t);
        y_binning = va_arg(*app, uint32_t);
        self->d_param.x_binning = x_binning;
        self->d_param.y_binning = y_binning;
    } else if (strcmp(keyname, "capture_mode") == 0) {
        uint32_t capture_mode;
        capture_mode = va_arg(*app, uint32_t);       
        self->d_param.capture_mode = capture_mode;
    } else if (strcmp(keyname, "gain") == 0) {
        double gain = va_arg(*app, double);
        self->d_param.gain = gain;
    } else if (strcmp(keyname, "pixel_format") == 0) {
        uint32_t pixel_format;
        pixel_format = va_arg(*app, uint32_t);       
        self->d_param.pixel_format = pixel_format;
    } else if (strcmp(keyname, "overscan") == 0) {
        uint32_t x_overscan, y_overscan;
        x_overscan = va_arg(*app, uint32_t);
        y_overscan = va_arg(*app, uint32_t);
        self->d_param.x_overscan = x_overscan;
        self->d_param.y_overscan = y_overscan;
    } else if (strcmp(keyname, "temperature") == 0) {
        double temperature = va_arg(*app, double);
        self->d_param.temperature = temperature;
    } else if (strcmp(keyname, "trigger_mode") == 0) {
        uint32_t trigger_mode;
        trigger_mode = va_arg(*app, uint32_t);       
        self->d_param.trigger_mode = trigger_mode;
    }
}

void
__detector_clear_option(void *_self)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->clear_option.method) {
        return ((void(*)(void *)) class->clear_option.method)(_self);
    } else {
        forward(_self, 0, (Method) __detector_clear_option, "clear_option", _self);
    }
}

static void
__Detector_clear_option(void *_self)
{
    struct __Detector *self = cast(__Detector(), _self);
    
    Pthread_mutex_lock(&self->d_state.mtx);
    self->d_state.options = 0;
    self->d_exp.notify_last_frame_filling = 0;
    self->d_exp.notify_each_frame_done = 0;
    Pthread_mutex_unlock(&self->d_state.mtx);
}

void
__detector_set_option(void *_self, uint16_t option)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->set_option.method) {
        return ((void(*)(void *, uint16_t)) class->set_option.method)(_self, option);
    } else {
        forward(_self, 0, (Method) __detector_set_option, "set_option", _self, option);
    }
}

static void
__Detector_set_option(void *_self, uint16_t option)
{
    struct __Detector *self = cast(__Detector(), _self);
    
    Pthread_mutex_lock(&self->d_state.mtx);
    if (option & DETECTOR_OPTION_NOTIFY_LAST_FILLING) {
        self->d_exp.notify_last_frame_filling = 1;
    }
    if (option & DETECTOR_OPTION_NOTIFY_EACH_COMPLETION) {
        self->d_exp.notify_each_frame_done = 1;
    }
    self->d_state.options |= option;
    Pthread_mutex_unlock(&self->d_state.mtx);
}

uint16_t
__detector_get_option(void *_self)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->get_option.method) {
        return ((uint16_t (*)(void *)) class->get_option.method)(_self);
    } else {
        uint16_t result;
        forward(_self, &result, (Method) __detector_get_option, "get_option", _self);
        return result;
    }
}

static uint16_t
__Detector_get_option(void *_self)
{
    struct __Detector *self = cast(__Detector(), _self);
    
    uint16_t result;
    
    Pthread_mutex_lock(&self->d_state.mtx);
    result = self->d_state.options;
    Pthread_mutex_unlock(&self->d_state.mtx);

    return result;
}

void
__detector_set_directory(void *_self, const char *directory)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->set_directory.method) {
        return ((void (*)(void *, const char *)) class->set_directory.method)(_self, directory);
    } else {
        forward(_self, 0, (Method) __detector_set_directory, "set_directory", _self, directory);
    }
}

static void
__Detector_set_directory(void *_self, const char *directory)
{
    struct __Detector *self = cast(__Detector(), _self);
    
    Pthread_mutex_lock(&self->d_state.mtx);
    while (self->d_state.state == DETECTOR_STATE_READING || self->d_state.state == DETECTOR_STATE_EXPOSING) {
        Pthread_cond_wait(&self->d_state.cond, &self->d_state.mtx);
    }
    if (self->d_proc.image_directory != NULL) {
        free(self->d_proc.image_directory);
    }
    self->d_proc.image_directory = (char *) Malloc(strlen(directory) + 1);
    snprintf(self->d_proc.image_directory, strlen(directory) + 1, "%s", directory);
    Pthread_mutex_unlock(&self->d_state.mtx);
}

void
__detector_get_directory(void *_self, char *directory, size_t size)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->get_directory.method) {
        return ((void (*)(void *, char *, size_t)) class->set_directory.method)(_self, directory, size);
    } else {
        forward(_self, 0, (Method) __detector_get_directory, "get_directory", _self, directory, size);
    }
}

static void
__Detector_get_directory(void *_self, char *directory, size_t size)
{
    struct __Detector *self = cast(__Detector(), _self);
    
    Pthread_mutex_lock(&self->d_state.mtx);
    snprintf(directory, size, "%s", self->d_proc.image_directory);
    Pthread_mutex_unlock(&self->d_state.mtx);
}

void
__detector_set_prefix(void *_self, const char *prefix)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->set_prefix.method) {
        return ((void (*)(void *, const char *)) class->set_prefix.method)(_self, prefix);
    } else {
        forward(_self, 0, (Method) __detector_set_prefix, "set_prefix", _self, prefix);
    }
}

static void
__Detector_set_prefix(void *_self, const char *prefix)
{
    struct __Detector *self = cast(__Detector(), _self);
    
    Pthread_mutex_lock(&self->d_state.mtx);
    while (self->d_state.state == DETECTOR_STATE_READING || self->d_state.state == DETECTOR_STATE_EXPOSING) {
        Pthread_cond_wait(&self->d_state.cond, &self->d_state.mtx);
    }
    if (self->d_proc.image_prefix != NULL) {
        free(self->d_proc.image_prefix);
    }
    self->d_proc.image_prefix = (char *) Malloc(strlen(prefix) + 1);
    snprintf(self->d_proc.image_prefix, strlen(prefix) + 1, "%s", prefix);
    Pthread_mutex_unlock(&self->d_state.mtx);
}

void
__detector_get_prefix(void *_self, char *prefix, size_t size)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->get_prefix.method) {
        return ((void (*)(void *, char *, size_t)) class->set_prefix.method)(_self, prefix, size);
    } else {
        forward(_self, 0, (Method) __detector_get_prefix, "get_prefix", _self, prefix, size);
    }
}

static void
__Detector_get_prefix(void *_self, char *prefix, size_t size)
{
    struct __Detector *self = cast(__Detector(), _self);
    
    Pthread_mutex_lock(&self->d_state.mtx);
    snprintf(prefix, size, "%s", self->d_proc.image_prefix);
    Pthread_mutex_unlock(&self->d_state.mtx);
}

void
__detector_set_template(void *_self, const char *template)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);

    if (isOf(class, __DetectorClass()) && class->set_template.method) {
        return ((void (*)(void *, const char *)) class->set_template.method)(_self, template);
    } else {
        forward(_self, 0, (Method) __detector_set_template, "set_template", _self, template);
    }
}

static void
__Detector_set_template(void *_self, const char *template)
{
    struct __Detector *self = cast(__Detector(), _self);
    
    Pthread_mutex_lock(&self->d_state.mtx);
    while (self->d_state.state == DETECTOR_STATE_READING || self->d_state.state == DETECTOR_STATE_EXPOSING) {
        Pthread_cond_wait(&self->d_state.cond, &self->d_state.mtx);
    }
    if (self->d_proc.tpl_filename != NULL) {
        free(self->d_proc.tpl_filename);
    }
    self->d_proc.tpl_filename = (char *) Malloc(strlen(template) + 1);
    snprintf(self->d_proc.tpl_filename, strlen(template) + 1, "%s", template);
    Pthread_mutex_unlock(&self->d_state.mtx);
}

void
__detector_get_template(void *_self, char *template, size_t size)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->get_template.method) {
        return ((void (*)(void *, char *, size_t)) class->set_template.method)(_self, template, size);
    } else {
        forward(_self, 0, (Method) __detector_get_prefix, "get_template", _self, template, size);
    }
}

static void
__Detector_get_template(void *_self, char *template, size_t size)
{
    struct __Detector *self = cast(__Detector(), _self);
    
    Pthread_mutex_lock(&self->d_state.mtx);
    snprintf(template, size, "%s", self->d_proc.tpl_filename);
    Pthread_mutex_unlock(&self->d_state.mtx);
}

void
__detector_set_pre_acquisition(void *_self, int (*pre_acquisition)(void *, const char *, ...))
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->set_pre_acquisition.method) {
        return ((void (*)(void *, int (*)(void *, const char *, ...))) class->set_pre_acquisition.method)(_self, pre_acquisition);
    } else {
        forward(_self, 0, (Method) __detector_set_pre_acquisition, "set_pre_acquisition", _self, pre_acquisition);
    }
}

static void
__Detector_set_pre_acquisition(void *_self, int (*pre_acquisition)(void *, const char *, ...))
{
    struct __Detector *self = cast(__Detector(), _self);
    
    Pthread_mutex_lock(&self->d_state.mtx);
    while (self->d_state.state == DETECTOR_STATE_READING || self->d_state.state == DETECTOR_STATE_EXPOSING) {
        Pthread_cond_wait(&self->d_state.cond, &self->d_state.mtx);
    }
    
    self->d_proc.pre_acquisition = pre_acquisition;
    Pthread_mutex_unlock(&self->d_state.mtx);
}

void
__detector_set_post_acquisition(void *_self, int (*post_acquisition)(void *, const char *, ...))
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->set_post_acquisition.method) {
        return ((void (*)(void *, int (*)(void *, const char *, ...))) class->set_post_acquisition.method)(_self, post_acquisition);
    } else {
        forward(_self, 0, (Method) __detector_set_post_acquisition, "set_post_acquisition", _self, post_acquisition);
    }
}

static void
__Detector_set_post_acquisition(void *_self, int (*post_acquisition)(void *, const char *, ...))
{
    struct __Detector *self = cast(__Detector(), _self);
    
    Pthread_mutex_lock(&self->d_state.mtx);
    while (self->d_state.state == DETECTOR_STATE_READING || self->d_state.state == DETECTOR_STATE_EXPOSING) {
        Pthread_cond_wait(&self->d_state.cond, &self->d_state.mtx);
    }
    self->d_proc.post_acquisition = post_acquisition;
    Pthread_mutex_unlock(&self->d_state.mtx);
}

void
__detector_set_name_convention(void *_self, void (*name_convention)(void *, char *, size_t, ...))
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->set_name_convention.method) {
        return ((void (*)(void *, void (*)(void *, char *, size_t, ...))) class->set_name_convention.method)(_self, name_convention);
    } else {
        forward(_self, 0, (Method) __detector_set_name_convention, "set_name_convention", _self, name_convention);
    }
}

static void
__Detector_set_name_convention(void *_self, void (*name_convention)(void *, char *, size_t, ...))
{
    struct __Detector *self = cast(__Detector(), _self);
    
    Pthread_mutex_lock(&self->d_state.mtx);
    while (self->d_state.state == DETECTOR_STATE_READING || self->d_state.state == DETECTOR_STATE_EXPOSING) {
        Pthread_cond_wait(&self->d_state.cond, &self->d_state.mtx);
    }
    self->d_proc.name_convention = name_convention;
    Pthread_mutex_unlock(&self->d_state.mtx);
}

int
__detector_wait(void *_self, double timeout)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->wait.method) {
        return ((int (*)(void *, double)) class->wait.method)(_self, timeout);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_wait, "wait", _self, timeout);
        return result;
    }
}

static int
__Detector_wait(void *_self, double timeout)
{
    struct __Detector *self = cast(__Detector(), _self);
    
    int ret = 0;
    struct timespec tp;
    
    tp.tv_sec = floor(timeout);
    tp.tv_nsec = (timeout - tp.tv_sec) * 1000000000;
    
    Pthread_mutex_lock(&self->d_state.mtx);
    while (self->d_state.state == DETECTOR_STATE_MALFUNCTION) {
        if (timeout > 0) {
            ret = Pthread_cond_timedwait(&self->d_state.cond, &self->d_state.mtx, &tp);
        } else {
            ret  = Pthread_cond_wait(&self->d_state.cond, &self->d_state.mtx);
        }
    }
    Pthread_mutex_unlock(&self->d_state.mtx);
    
    if (ret == 0) {
        return AAOS_OK;
    } else if (ret == ETIMEDOUT) {
        return AAOS_ETIMEDOUT;
    } else {
        return AAOS_ERROR;
    }
}

/*
 * Virtual functions of detector class.
 */
int
__detector_power_on(void *_self)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->power_on.method) {
        return ((int (*)(void *)) class->power_on.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_power_on, "power_on", _self);
        return result;
    }
}

static int
__Detector_power_on(void *_self)
{
    struct __Detector *self = cast(__Detector(), _self);
    
    Pthread_mutex_lock(&self->d_state.mtx);
    if (self->d_state.state != DETECTOR_STATE_MALFUNCTION) {
        self->d_state.state = DETECTOR_STATE_IDLE;
    }
    Pthread_mutex_unlock(&self->d_state.mtx);
    
    return AAOS_OK;
}

int
__detector_power_off(void *_self)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->power_off.method) {
        return ((int (*)(void *)) class->power_off.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_power_off, "power_off", _self);
        return result;
    }
}

static int
__Detector_power_off(void *_self)
{
    struct __Detector *self = cast(__Detector(), _self);
    
    Pthread_mutex_lock(&self->d_state.mtx);
    self->d_state.state = DETECTOR_STATE_OFFLINE;
    Pthread_mutex_unlock(&self->d_state.mtx);
    
    return AAOS_OK;
}

int
__detector_enable_cooling(void *_self)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->enable_cooling.method) {
        return ((int (*)(void *)) class->enable_cooling.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_enable_cooling, "enable_cooling", _self);
        return result;
    }
}

static int
__Detector_enable_cooling(void *_self)
{
    return AAOS_ENOTSUP;
}

int
__detector_disable_cooling(void *_self)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->disable_cooling.method) {
        return ((int (*)(void *)) class->disable_cooling.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_disable_cooling, "disable_cooling", _self);
        return result;
    }
}

static int
__Detector_disable_cooling(void *_self)
{
    return AAOS_ENOTSUP;
}

int
__detector_init(void *_self)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->init.method) {
        return ((int (*)(void *)) class->init.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_init, "init", _self);
        return result;
    }
}

static int
__Detector_init(void *_self, uint16_t options)
{
    struct __Detector *self = cast(__Detector(), _self);
    
    Pthread_mutex_lock(&self->d_state.mtx);
    if (self->d_state.state == DETECTOR_STATE_MALFUNCTION) {
        Pthread_mutex_unlock(&self->d_state.mtx);
        return AAOS_EDEVMAL;
    } else if (self->d_state.state == DETECTOR_STATE_OFFLINE) {
        Pthread_mutex_unlock(&self->d_state.mtx);
        return AAOS_EPWROFF;
    }
    Pthread_mutex_unlock(&self->d_state.mtx);
    
    return AAOS_OK;
}

int
__detector_set_exposure_time(void *_self, double exposure_time)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->set_exposure_time.method) {
        return ((int (*)(void *, double)) class->set_exposure_time.method)(_self, exposure_time);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_set_exposure_time, "set_exposure_time", _self, exposure_time);
        return result;
    }
}

static int
__Detector_set_exposure_time(void *_self, double exposure_time)
{
    struct __Detector *self = cast(__Detector(), _self);
    
    if (!self->d_cap.exposure_time_available) {
        return AAOS_ENOTSUP;
    }
    
    if (exposure_time > self->d_cap.exposure_time_max || exposure_time < self->d_cap.exposure_time_min) {
        return AAOS_EINVAL;
    }
    
    Pthread_mutex_lock(&self->d_state.mtx);
    self->d_param.exposure_time = exposure_time;
    Pthread_mutex_unlock(&self->d_state.mtx);
    
    return AAOS_OK;
}

int
__detector_get_exposure_time(void *_self, double *exposure_time)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->get_exposure_time.method) {
        return ((int (*)(void *, double *)) class->get_exposure_time.method)(_self, exposure_time);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_get_exposure_time, "get_exposure_time", _self, exposure_time);
        return result;
    }
}

static int
__Detector_get_exposure_time(void *_self, double *exposure_time)
{
    struct __Detector *self = cast(__Detector(), _self);

    Pthread_mutex_lock(&self->d_state.mtx);
    *exposure_time = self->d_param.exposure_time;
    Pthread_mutex_unlock(&self->d_state.mtx);
    
    return AAOS_OK;
}

int
__detector_set_frame_rate(void *_self, double frame_rate)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->set_frame_rate.method) {
        return ((int (*)(void *, double)) class->set_frame_rate.method)(_self, frame_rate);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_set_frame_rate, "set_frame_rate", _self, frame_rate);
        return result;
    }
}

static int
__Detector_set_frame_rate(void *_self, double frame_rate)
{
    struct __Detector *self = cast(__Detector(), _self);
    
    if (frame_rate > self->d_cap.frame_rate_max || frame_rate < self->d_cap.frame_rate_min) {
        return AAOS_EINVAL;
    }
    Pthread_mutex_lock(&self->d_state.mtx);
    self->d_param.frame_rate = frame_rate;
    Pthread_mutex_unlock(&self->d_state.mtx);
    
    return AAOS_OK;
}

int
__detector_get_frame_rate(void *_self, double *frame_rate)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->get_frame_rate.method) {
        return ((int (*)(void *, double *)) class->get_frame_rate.method)(_self, frame_rate);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_get_frame_rate, "get_frame_rate", _self, frame_rate);
        return result;
    }
}

static int
__Detector_get_frame_rate(void *_self, double *frame_rate)
{
    struct __Detector *self = cast(__Detector(), _self);
    
    if (!self->d_cap.frame_rate_available) {
        return AAOS_ENOTSUP;
    }
    
    Pthread_mutex_lock(&self->d_state.mtx);
    *frame_rate = self->d_param.frame_rate;
    Pthread_mutex_unlock(&self->d_state.mtx);
    
    return AAOS_OK;
}

int
__detector_set_gain(void *_self, double gain)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->set_gain.method) {
        return ((int (*)(void *, double)) class->set_gain.method)(_self, gain);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_set_gain, "set_gain", _self, gain);
        return result;
    }
}

static int
__Detector_set_gain(void *_self, double gain)
{
    struct __Detector *self = cast(__Detector(), _self);
    
    if (gain > self->d_cap.gain_max || gain < self->d_cap.gain_min) {
        return AAOS_EINVAL;
    }
    
    Pthread_mutex_lock(&self->d_state.mtx);
    self->d_param.frame_rate = gain;
    Pthread_mutex_unlock(&self->d_state.mtx);
    
    return AAOS_OK;
}

int
__detector_get_gain(void *_self, double *gain)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->get_gain.method) {
        return ((int (*)(void *, double *)) class->get_gain.method)(_self, gain);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_get_gain, "get_gain", _self, gain);
        return result;
    }
}

static int
__Detector_get_gain(void *_self, double *gain)
{
    struct __Detector *self = cast(__Detector(), _self);
    
    if (!self->d_cap.gain_available) {
        return AAOS_ENOTSUP;
    }
    
    Pthread_mutex_lock(&self->d_state.mtx);
    *gain = self->d_param.gain;
    Pthread_mutex_unlock(&self->d_state.mtx);
    
    return AAOS_OK;
}

int
__detector_get_temperature(void *_self, double *temperature)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->get_temperature.method) {
        return ((int (*)(void *, double *)) class->get_temperature.method)(_self, temperature);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_get_temperature, "get_temperature", _self, temperature);
        return result;
    }
}

static int
__Detector_get_temperature(void *_self, double *temperature)
{
    return AAOS_ENOTSUP;
}

int
__detector_set_temperature(void *_self, double temperature)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->set_temperature.method) {
        return ((int (*)(void *, double)) class->set_temperature.method)(_self, temperature);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_set_temperature, "set_temperature", _self, temperature);
        return result;
    }
}

static int
__Detector_set_temperature(void *_self, double temperature)
{
    struct __Detector *self = cast(__Detector(), _self);
    
    Pthread_mutex_lock(&self->d_state.mtx);
    self->d_param.temperature = temperature;
    Pthread_mutex_unlock(&self->d_state.mtx);
    
    return AAOS_OK;
}

int
__detector_set_pixel_format(void *_self, uint32_t pixel_format)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->set_pixel_format.method) {
        return ((int (*)(void *, uint32_t)) class->set_pixel_format.method)(_self, pixel_format);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_set_pixel_format, "set_pixel_format", _self, pixel_format);
        return result;
    }
}

int
__detector_get_pixel_format(void *_self, uint32_t *pixel_format)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->get_pixel_format.method) {
        return ((int (*)(void *, uint32_t *)) class->get_pixel_format.method)(_self, pixel_format);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_get_pixel_format, "get_pixel_format", _self, pixel_format);
        return result;
    }
}

int
__detector_set_overscan(void *_self, uint32_t x_overscan, uint32_t y_overscan)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->set_overscan.method) {
        return ((int (*)(void *, uint32_t, uint32_t)) class->set_overscan.method)(_self, x_overscan, y_overscan);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_set_overscan, "set_overscan", _self, x_overscan, y_overscan);
        return result;
    }
}

int
__detector_get_overscan(void *_self, uint32_t *x_overscan, uint32_t *y_overscan)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->get_overscan.method) {
        return ((int (*)(void *, uint32_t *, uint32_t *)) class->get_overscan.method)(_self, x_overscan, y_overscan);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_set_overscan, "set_overscan", _self, x_overscan, y_overscan);
        return result;
    }
}

int
__detector_set_capture_mode(void *_self, uint32_t capture_mode)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->set_capture_mode.method) {
        return ((int (*)(void *, uint32_t)) class->set_capture_mode.method)(_self, capture_mode);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_set_capture_mode, "set_capture_mode", _self, capture_mode);
        return result;
    }
}

int
__detector_get_capture_mode(void *_self, uint32_t *capture_mode)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->get_capture_mode.method) {
        return ((int (*)(void *, uint32_t *)) class->get_capture_mode.method)(_self, capture_mode);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_get_capture_mode, "get_capture_mode", _self, capture_mode);
        return result;
    }
}

int
__detector_set_trigger_mode(void *_self, uint32_t trigger_mode)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->set_trigger_mode.method) {
        return ((int (*)(void *, uint32_t)) class->set_trigger_mode.method)(_self, trigger_mode);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_set_trigger_mode, "set_trigger_mode", _self, trigger_mode);
        return result;
    }
}

int
__detector_get_trigger_mode(void *_self, uint32_t *trigger_mode)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->get_trigger_mode.method) {
        return ((int (*)(void *, uint32_t *)) class->get_trigger_mode.method)(_self, trigger_mode);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_get_capture_mode, "get_trigger_mode", _self, trigger_mode);
        return result;
    }
}

const char *
__detector_get_name(const void *_self)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->get_name.method) {
        return ((const char * (*)(const void *)) class->get_name.method)(_self);
    } else {
        const char *result;
        forward(_self, &result, (Method) __detector_get_name, "get_name", _self);
        return result;
    }
}

static const char *
__Detector_get_name(void *_self)
{
    struct __Detector *self = cast(__Detector(), _self);
    
    return self->name;
}

int
__detector_set_readout_rate(void *_self, double readout_rate)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->set_readout_rate.method) {
        return ((int (*)(void *, double)) class->set_readout_rate.method)(_self, readout_rate);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_set_readout_rate, "set_readout_rate", _self, readout_rate);
        return result;
    }
}

static int
__Detector_set_readout_rate(void *_self, double readout_rate)
{
    struct __Detector *self = cast(__Detector(), _self);
    
    if (readout_rate > self->d_cap.readout_rate_max || readout_rate < self->d_cap.readout_rate_min) {
        return AAOS_EINVAL;
    }
    Pthread_mutex_lock(&self->d_state.mtx);
    self->d_param.readout_rate = readout_rate;
    Pthread_mutex_unlock(&self->d_state.mtx);
    
    return AAOS_OK;
}

int
__detector_get_readout_rate(void *_self, double *readout_rate)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->get_readout_rate.method) {
        return ((int (*)(void *, double *)) class->get_readout_rate.method)(_self, readout_rate);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_get_readout_rate, "get_readout_rate", _self, readout_rate);
        return result;
    }
}

static int
__Detector_get_readout_rate(void *_self, double *readout_rate)
{
    struct __Detector *self = cast(__Detector(), _self);
    
    if (!self->d_cap.readout_rate_available) {
        return AAOS_ENOTSUP;
    }
    
    Pthread_mutex_lock(&self->d_state.mtx);
    *readout_rate = self->d_param.readout_rate;
    Pthread_mutex_unlock(&self->d_state.mtx);
    
    return AAOS_OK;
}

int
__detector_set_region(void *_self, uint32_t x_offset, uint32_t y_offset, uint32_t width, uint32_t height)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->set_region.method) {
        return ((int (*)(void *, uint32_t, uint32_t, uint32_t, uint32_t)) class->set_region.method)(_self, x_offset, y_offset, width, height);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_set_region, "set_region", _self, x_offset, y_offset, width, height);
        return result;
    }
}

static int
__Detector_set_region(void *_self, uint32_t x_offset, uint32_t y_offset, uint32_t width, uint32_t height)
{
    struct __Detector *self = cast(__Detector(), _self);
    size_t image_width, image_height;
    
    Pthread_mutex_lock(&self->d_state.mtx);
    
    image_width = self->d_cap.width / self->d_param.x_binning;
    image_height = self->d_cap.height / self->d_param.y_binning;
    
    if ((x_offset + width) >= image_width || (y_offset + height) >= image_height) {
        Pthread_mutex_unlock(&self->d_state.mtx);
        return AAOS_EINVAL;
    }
    
    self->d_param.x_offset = x_offset;
    self->d_param.y_offset = y_offset;
    self->d_param.image_width = width;
    self->d_param.image_height = height;
    
    Pthread_mutex_unlock(&self->d_state.mtx);
    
    return AAOS_OK;
}

int
__detector_get_region(void *_self, uint32_t *x_offset, uint32_t *y_offset, uint32_t *width, uint32_t *height)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->get_region.method) {
        return ((int (*)(void *, uint32_t *, uint32_t *, uint32_t *, uint32_t *)) class->get_region.method)(_self, x_offset, y_offset, width, height);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_get_region, "get_region", _self, x_offset, y_offset, width, height);
        return result;
    }
}

static int
__Detector_get_region(void *_self, uint32_t *x_offset, uint32_t *y_offset, uint32_t *width, uint32_t *height)
{
    struct __Detector *self = cast(__Detector(), _self);
    
    Pthread_mutex_lock(&self->d_state.mtx);
    
    *x_offset = (uint32_t) self->d_param.x_offset;
    *y_offset = (uint32_t) self->d_param.y_offset;
    *width = (uint32_t) self->d_param.image_width;
    *height = (uint32_t) self->d_param.image_height;
    
    Pthread_mutex_unlock(&self->d_state.mtx);
    
    return AAOS_OK;
}

int
__detector_set_binning(void *_self, uint32_t x_binning, uint32_t y_binning)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->set_binning.method) {
        return ((int (*)(void *, uint32_t, uint32_t)) class->set_binning.method)(_self, x_binning, y_binning);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_set_binning, "set_binning", _self, x_binning, y_binning);
        return result;
    }
}

static int
__Detector_set_binning(void *_self, uint32_t x_binning, uint32_t y_binning)
{
    struct __Detector *self = cast(__Detector(), _self);
    
    if (self->d_cap.x_binning_array) {
        size_t i;
        for (i = 0; i < self->d_cap.n_x_binning; i++) {
            if (x_binning == self->d_cap.x_binning_array[i]) {
                break;
            }
        }
        return AAOS_EINVAL;
    } else {
        if (x_binning < self->d_cap.x_binning_min || x_binning > self->d_cap.x_offset_max) {
            return AAOS_EINVAL;
        }
    }
    if (self->d_cap.y_binning_array) {
        size_t i;
        for (i = 0; i < self->d_cap.n_y_binning; i++) {
            if (y_binning == self->d_cap.y_binning_array[i]) {
                break;
            }
        }
        return AAOS_EINVAL;
    } else {
        if (y_binning < self->d_cap.y_binning_min || y_binning > self->d_cap.y_offset_max) {
            return AAOS_EINVAL;
        }
    }
    
    Pthread_mutex_lock(&self->d_state.mtx);
    self->d_param.x_binning = x_binning;
    self->d_param.y_binning = y_binning;
    Pthread_mutex_unlock(&self->d_state.mtx);
    
    return AAOS_OK;
}

int
__detector_get_binning(void *_self, uint32_t *x_binning, uint32_t *y_binnning)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->get_binning.method) {
        return ((int (*)(void *, uint32_t *, uint32_t *)) class->get_binning.method)(_self, x_binning, y_binnning);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_get_binning, "get_binning", _self, x_binning, y_binnning);
        return result;
    }
}

static int
__Detector_get_binning(void *_self, uint32_t *x_binning, uint32_t *y_binning)
{
    struct __Detector *self = cast(__Detector(), _self);
    
    Pthread_mutex_lock(&self->d_state.mtx);
    *x_binning = (uint32_t) self->d_param.x_binning;
    *y_binning = (uint32_t) self->d_param.y_binning;
    Pthread_mutex_unlock(&self->d_state.mtx);
    
    return AAOS_OK;
}

/*
 * Pure virtual functions.
 */
int
__detector_status(void *_self, void *res, size_t res_size, size_t *res_len)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->status.method) {
        return ((int (*)(void *, void *, size_t, size_t *)) class->status.method)(_self, res, res_size, res_len);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_status, "status", _self, res, res_size, res_len);
        return result;
    }
}

int
__detector_info(void *_self, void *res, size_t res_size, size_t *res_len)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->info.method) {
        return ((int (*)(void *, void *, size_t, size_t *)) class->info.method)(_self, res, res_size, res_len);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_info, "info", _self, res, res_size, res_len);
        return result;
    }
}

int
__detector_expose(void *_self, double exposure_time, uint32_t n_frame, ...)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    va_list ap;
    va_start(ap, n_frame);
    int result;

    if (isOf(class, __DetectorClass()) && class->expose.method) {
        result = ((int (*)(void *, double, uint32_t, va_list *)) class->expose.method)(_self, exposure_time, n_frame, &ap);
    } else {
        forward(_self, &result, (Method) __detector_expose, "expose", _self, exposure_time, n_frame, &ap);
    }

    va_end(ap);
    
    return result;
}

int __detector_raw(void *_self, const void *write_buffer, size_t write_buffer_size, size_t *write_size, void *read_buffer, size_t read_buffer_size, size_t *read_size)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->raw.method) {
        return ((int (*)(void *, const void *, size_t, size_t *, void *, size_t, size_t *)) class->expose.method)(_self, write_buffer, write_buffer_size, write_size, read_buffer, read_buffer_size, read_size);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_raw, "raw", _self, write_buffer, write_buffer_size, write_size, read_buffer, read_buffer_size, read_size);
        return result;
    }
}

int
__detector_inspect(void *_self)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->inspect.method) {
        return ((int (*)(void *)) class->inspect.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_inspect, "inspect", _self);
        return result;
    }
}

int
__detector_abort(void *_self)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->abort.method) {
        return ((int (*)(void *)) class->abort.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_abort, "abort", _self);
        return result;
    }
}

int
__detector_stop(void *_self)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->stop.method) {
        return ((int (*)(void *)) class->stop.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_stop, "stop", _self);
        return result;
    }
}

int
__detector_load(void *_self, ...)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    int result;
    va_list ap;
    
    va_start(ap, _self);
    if (isOf(class, __DetectorClass()) && class->load.method) {
        result = ((int (*)(void *, va_list *)) class->load.method)(_self, &ap);
    } else {
        forward(_self, &result, (Method) __detector_load, "load", _self, &ap);
    }
    va_end(ap);
    
    return result;
}

int
__detector_reload(void *_self)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->reload.method) {
        return ((int (*)(void *)) class->reload.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_reload, "reload", _self);
        return result;
    }
}

int
__detector_unload(void *_self)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->unload.method) {
        return ((int (*)(void *)) class->unload.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_unload, "unload", _self);
        return result;
    }
}

int
__detector_wait_for_completion(void *_self)
{
    const struct __DetectorClass *class = (const struct __DetectorClass *) classOf(_self);
    
    if (isOf(class, __DetectorClass()) && class->wait_for_completion.method) {
        return ((int (*)(void *)) class->wait_for_completion.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) __detector_wait_for_completion, "wait_for_completion", _self);
        return result;
    }
}

/*
 * class basic function
 */
static void
__Detector_forward(const void *_self, void *result, Method selector, const char *name, va_list *app)
{
    const struct __Detector *self = cast(__Detector(), _self);
    Method method = virtualTo(self->_vtab, name);
    va_list ap;
    
#ifdef va_copy
    va_copy(ap, *app);
#else
    ap = *app;
#endif
    
    void *obj = va_arg(*app, void *);
    
    if (selector == (Method) __detector_init || selector == (Method) __detector_power_on || selector == (Method) __detector_power_off || selector == (Method) __detector_inspect || selector == (Method) __detector_wait_for_completion || selector == (Method) __detector_reload || selector == (Method) __detector_unload || selector == (Method) __detector_abort || selector == (Method) __detector_stop || selector == (Method) __detector_enable_cooling || selector == (Method) __detector_disable_cooling) {
        *((int *) result) = ((int (*)(void *)) method)(obj);
    } else if (selector == (Method) __detector_get_gain || selector == (Method) __detector_get_frame_rate || selector == (Method) __detector_get_exposure_time || selector == (Method) __detector_get_readout_rate || selector == (Method) __detector_get_temperature) {
        double *value = va_arg(*app, double *);
        *((int *) result) = ((int (*)(void *, double *)) method)(obj, value);
    } else if (selector == (Method) __detector_set_gain || selector == (Method) __detector_set_frame_rate || selector == (Method) __detector_set_exposure_time || selector == (Method) __detector_set_readout_rate || selector == (Method) __detector_wait || selector == (Method) __detector_set_temperature) {
        double value = va_arg(*app, double);
        *((int *) result) = ((int (*)(void *, double)) method)(obj, value);
    } else if (selector == (Method) __detector_get_region) {
        uint32_t *x_offset, *y_offset, *width, *height;
        x_offset = va_arg(*app, uint32_t *);
        y_offset = va_arg(*app, uint32_t *);
        width = va_arg(*app, uint32_t *);
        height = va_arg(*app, uint32_t *);
        *((int *) result) = ((int (*)(void *, uint32_t *, uint32_t *, uint32_t *, uint32_t *)) method)(obj, x_offset, y_offset, width, height);
    } else if (selector == (Method) __detector_set_region) {
        double x_offset, y_offset, width, height;
        x_offset = va_arg(*app, uint32_t);
        y_offset = va_arg(*app, uint32_t);
        width = va_arg(*app, uint32_t);
        height = va_arg(*app, uint32_t);
        *((int *) result) = ((int (*)(void *, uint32_t, uint32_t, uint32_t, uint32_t)) method)(obj, x_offset, y_offset, width, height);
    } else if (selector == (Method) __detector_get_binning) {
        uint32_t *x_binning, *y_binning;
        x_binning = va_arg(*app, uint32_t *);
        y_binning = va_arg(*app, uint32_t *);
        *((int *) result) = ((int (*)(void *, uint32_t *, uint32_t *)) method)(obj, x_binning, y_binning);
    } else if (selector == (Method) __detector_set_binning) {
        uint32_t x_binning, y_binning;
        x_binning = va_arg(*app, uint32_t);
        y_binning = va_arg(*app, uint32_t);
        *((int *) result) = ((int (*)(void *, uint32_t, uint32_t)) method)(obj, x_binning, y_binning);
    } else if (selector == (Method) __detector_get_overscan) {
        uint32_t *x_overscan, *y_overscan;
        x_overscan = va_arg(*app, uint32_t *);
        y_overscan = va_arg(*app, uint32_t *);
        *((int *) result) = ((int (*)(void *, uint32_t *, uint32_t *)) method)(obj, x_overscan, y_overscan);
    } else if (selector == (Method) __detector_set_overscan) {
        uint32_t x_overscan, y_overscan;
        x_overscan = va_arg(*app, uint32_t);
        y_overscan = va_arg(*app, uint32_t);
        *((int *) result) = ((int (*)(void *, uint32_t, uint32_t)) method)(obj, x_overscan, y_overscan);
    } else if (selector == (Method) __detector_expose) {
        double exposure_time;
        uint32_t n_frame;
        va_list *app2;
        exposure_time = va_arg(*app, double);
        n_frame = va_arg(*app, uint32_t);
        app2 = va_arg(*app, va_list *);
        *((int *) result) = ((int (*)(void *, double, uint32_t, va_list *)) method)(obj, exposure_time, n_frame, app2);
    } else if (selector == (Method) __detector_raw) {
        const void *write_buffer = va_arg(*app, const void *);
        size_t write_buffer_size = va_arg(*app, size_t);
        size_t *write_size = va_arg(*app, size_t *);
        void *read_buffer = va_arg(*app, void *);
        size_t read_buffer_size = va_arg(*app, size_t);
        size_t *read_size = va_arg(*app, size_t *);
        *((int *) result) = ((int (*)(void *, const void *, size_t, size_t *, void *, size_t, size_t *)) method)(obj, write_buffer, write_buffer_size, write_size, read_buffer, read_buffer_size, read_size);
    } else if (selector == (Method) __detector_load) {
        va_list *myapp = va_arg(*app, va_list *);
        *((int *) result) = ((int (*)(void *, va_list *)) method)(obj, myapp);
    } else if (selector == (Method) __detector_status || selector == (Method) __detector_info) {
        char *buffer = va_arg(*app, char *);
        size_t size = va_arg(*app, size_t);
        size_t *res_len = va_arg(*app, size_t *);
        *((int *) result) = ((int (*)(void *, char *, size_t, size_t *)) method)(obj, buffer, size, res_len);
    } else if (selector == (Method) __detector_set_prefix || selector == (Method) __detector_set_prefix) {
        const char *value = va_arg(*app, const char *);
        *((int *) result) = ((int (*)(void *, const char *)) method)(obj, value);
    } else if (selector == (Method) __detector_set_pixel_format) {
        uint32_t pixel_format = va_arg(*app, uint32_t);
        *((int *) result) = ((int (*)(void *, uint32_t)) method)(obj, pixel_format);
    } else if (selector == (Method) __detector_get_pixel_format) {
        uint32_t *pixel_format = va_arg(*app, uint32_t *);
        *((int *) result) = ((int (*)(void *, uint32_t *)) method)(obj, pixel_format);
    } else {
        assert(0);
    }
    
#ifdef va_copy
    va_end(ap);
#endif
}

static void *
__Detector_ctor(void *_self, va_list *app)
{
    struct __Detector *self = super_ctor(__Detector(), _self, app);
    
    const char *s, *key, *value;
    
    s = va_arg(*app, const char *);
    self->name = (char *) Malloc(strlen(s) + 1);
    snprintf(self->name, strlen(s) + 1, "%s", s);
    self->d_cap.n_chip = 1;
    
    while ((key = va_arg(*app, const char *))) {
        if (strcmp(key, "description") == 0) {
            value = va_arg(*app, const char *);
            self->description = (char *) Malloc(strlen(value) + 1);
            snprintf(self->description, strlen(value) + 1, "%s", value);
            continue;
        }
        if (strcmp(key, "prefix") == 0) {
            value = va_arg(*app, const char *);
            self->d_proc.image_prefix = (char *) Malloc(strlen(value) + 1);
            snprintf(self->d_proc.image_prefix, strlen(value) + 1, "%s", value);
            continue;
        }
        if (strcmp(key, "directory") == 0) {
            value = va_arg(*app, const char *);
            self->d_proc.image_directory = (char *) Malloc(strlen(value) + 1);
            snprintf(self->d_proc.image_directory, strlen(value) + 1, "%s", value);
            continue;
        }
        if (strcmp(key, "template") == 0) {
            value = va_arg(*app, const char *);
            self->d_proc.tpl_filename = (char *) Malloc(strlen(value) + 1);
            snprintf(self->d_proc.tpl_filename, strlen(value) + 1, "%s", value);
            continue;
        }
        if (strcmp(key, "option") == 0) {
            self->d_state.options = va_arg(*app, uint32_t);
            continue;
        }
    }
    
    self->d_state.state = DETECTOR_STATE_OFFLINE;
    //self->d_state.options = DETECTOR_OPTION_NOTIFY_EACH_COMPLETION;
    //self->d_exp.notify_each_frame_done = 1;

    //self->d_proc.queue = new(ThreadsafeQueue(), free);
    Pthread_cond_init(&self->d_exp.cond, NULL);
    Pthread_mutex_init(&self->d_exp.mtx, NULL);
    Pthread_cond_init(&self->d_state.cond, NULL);
    Pthread_mutex_init(&self->d_state.mtx, NULL);
    Pthread_rwlock_init(&self->d_param.rwlock, NULL);
    
    return (void *) self;
}

static void *
__Detector_dtor(void *_self)
{
    struct __Detector *self = cast(__Detector(), _self);
    
    Pthread_cond_destroy(&self->d_state.cond);
    Pthread_mutex_destroy(&self->d_state.mtx);
    
    if (self->d_cap.x_binning_array != NULL) {
        free(self->d_cap.x_binning_array);
    }
    if (self->d_cap.y_binning_array != NULL) {
        free(self->d_cap.x_binning_array);
    }
    if (self->d_cap.gain_array != NULL) {
        free(self->d_cap.gain_array);
    }
    if (self->d_cap.readout_rate_array) {
        free(self->d_cap.readout_rate_array);
    }
    if (self->d_cap.trigger_mode_array != NULL) {
        free(self->d_cap.trigger_mode_array);
    }
    Pthread_rwlock_destroy(&self->d_param.rwlock);
    Pthread_cond_destroy(&self->d_exp.cond);
    Pthread_mutex_destroy(&self->d_exp.mtx);
    
    free(self->d_proc.tpl_filename);
    free(self->d_proc.image_prefix);
    free(self->d_proc.image_directory);
    
    if (self->d_proc.tpl_fptr != NULL) {
        int status = 0;
        fits_close_file(self->d_proc.tpl_fptr, &status);
    }
    if (self->d_proc.img_fptr != NULL) {
        int status = 0;
        fits_close_file(self->d_proc.img_fptr, &status);
    }
    if (self->d_proc.queue != NULL) {
        delete(self->d_proc.queue);
    }
    
    if (self->d_proc.img_filename != NULL) {
        free(self->d_proc.img_filename);
    }
    
    free(self->name);
    free(self->description);
    
    return super_dtor(__Detector(), _self);
}

static void *
__DetectorClass_ctor(void *_self, va_list *app)
{
    struct __DetectorClass *self = super_ctor(__DetectorClass(), _self, app);
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
        if (selector == (Method) __detector_clear_option) {
            if (tag) {
                self->clear_option.tag = tag;
                self->clear_option.selector = selector;
            }
            self->clear_option.method = method;
            continue;
        }
        if (selector == (Method) __detector_get_option) {
            if (tag) {
                self->get_option.tag = tag;
                self->get_option.selector = selector;
            }
            self->get_option.method = method;
            continue;
        }
        if (selector == (Method) __detector_set_option) {
            if (tag) {
                self->set_option.tag = tag;
                self->set_option.selector = selector;
            }
            self->set_option.method = method;
            continue;
        }
        if (selector == (Method) __detector_get_directory) {
            if (tag) {
                self->get_directory.tag = tag;
                self->get_directory.selector = selector;
            }
            self->get_directory.method = method;
            continue;
        }
        if (selector == (Method) __detector_set_directory) {
            if (tag) {
                self->set_directory.tag = tag;
                self->set_directory.selector = selector;
            }
            self->set_directory.method = method;
            continue;
        }
        if (selector == (Method) __detector_get_prefix) {
            if (tag) {
                self->get_prefix.tag = tag;
                self->get_prefix.selector = selector;
            }
            self->get_prefix.method = method;
            continue;
        }
        if (selector == (Method) __detector_set_prefix) {
            if (tag) {
                self->set_prefix.tag = tag;
                self->set_prefix.selector = selector;
            }
            self->set_prefix.method = method;
            continue;
        }
        if (selector == (Method) __detector_get_template) {
            if (tag) {
                self->get_template.tag = tag;
                self->get_template.selector = selector;
            }
            self->get_template.method = method;
            continue;
        }
        if (selector == (Method) __detector_set_template) {
            if (tag) {
                self->set_template.tag = tag;
                self->set_template.selector = selector;
            }
            self->set_template.method = method;
            continue;
        }
        if (selector == (Method) __detector_set_pre_acquisition) {
            if (tag) {
                self->set_pre_acquisition.tag = tag;
                self->set_pre_acquisition.selector = selector;
            }
            self->set_pre_acquisition.method = method;
            continue;
        }
        if (selector == (Method) __detector_set_post_acquisition) {
            if (tag) {
                self->set_post_acquisition.tag = tag;
                self->set_post_acquisition.selector = selector;
            }
            self->set_post_acquisition.method = method;
            continue;
        }
        if (selector == (Method) __detector_set_name_convention) {
            if (tag) {
                self->set_name_convention.tag = tag;
                self->set_name_convention.selector = selector;
            }
            self->set_name_convention.method = method;
            continue;
        }
        if (selector == (Method) __detector_power_on) {
            if (tag) {
                self->power_on.tag = tag;
                self->power_on.selector = selector;
            }
            self->power_on.method = method;
            continue;
        }
        if (selector == (Method) __detector_power_off) {
            if (tag) {
                self->power_off.tag = tag;
                self->power_off.selector = selector;
            }
            self->power_off.method = method;
            continue;
        }
        if (selector == (Method) __detector_init) {
            if (tag) {
                self->init.tag = tag;
                self->init.selector = selector;
            }
            self->init.method = method;
            continue;
        }
        if (selector == (Method) __detector_get_binning) {
            if (tag) {
                self->get_binning.tag = tag;
                self->get_binning.selector = selector;
            }
            self->get_binning.method = method;
            continue;
        }
        if (selector == (Method) __detector_set_binning) {
            if (tag) {
                self->set_binning.tag = tag;
                self->set_binning.selector = selector;
            }
            self->set_binning.method = method;
            continue;
        }
        if (selector == (Method) __detector_get_gain) {
            if (tag) {
                self->get_gain.tag = tag;
                self->get_gain.selector = selector;
            }
            self->get_gain.method = method;
            continue;
        }
        if (selector == (Method) __detector_set_gain) {
            if (tag) {
                self->set_gain.tag = tag;
                self->set_gain.selector = selector;
            }
            self->set_gain.method = method;
            continue;
        }
        if (selector == (Method) __detector_get_frame_rate) {
            if (tag) {
                self->get_frame_rate.tag = tag;
                self->get_frame_rate.selector = selector;
            }
            self->get_frame_rate.method = method;
            continue;
        }
        if (selector == (Method) __detector_set_frame_rate) {
            if (tag) {
                self->set_frame_rate.tag = tag;
                self->set_frame_rate.selector = selector;
            }
            self->set_frame_rate.method = method;
            continue;
        }
        if (selector == (Method) __detector_set_exposure_time) {
            if (tag) {
                self->set_exposure_time.tag = tag;
                self->set_exposure_time.selector = selector;
            }
            self->set_exposure_time.method = method;
            continue;
        }
        if (selector == (Method) __detector_get_exposure_time) {
            if (tag) {
                self->get_exposure_time.tag = tag;
                self->get_exposure_time.selector = selector;
            }
            self->get_exposure_time.method = method;
            continue;
        }
        if (selector == (Method) __detector_set_overscan) {
            if (tag) {
                self->set_overscan.tag = tag;
                self->set_overscan.selector = selector;
            }
            self->set_overscan.method = method;
            continue;
        }
        if (selector == (Method) __detector_get_overscan) {
            if (tag) {
                self->get_overscan.tag = tag;
                self->get_overscan.selector = selector;
            }
            self->get_overscan.method = method;
            continue;
        }
        if (selector == (Method) __detector_set_readout_rate) {
            if (tag) {
                self->set_readout_rate.tag = tag;
                self->set_readout_rate.selector = selector;
            }
            self->set_readout_rate.method = method;
            continue;
        }
        if (selector == (Method) __detector_get_readout_rate) {
            if (tag) {
                self->get_readout_rate.tag = tag;
                self->get_readout_rate.selector = selector;
            }
            self->get_readout_rate.method = method;
            continue;
        }
        if (selector == (Method) __detector_set_region) {
            if (tag) {
                self->set_region.tag = tag;
                self->set_region.selector = selector;
            }
            self->set_region.method = method;
            continue;
        }
        if (selector == (Method) __detector_get_region) {
            if (tag) {
                self->get_region.tag = tag;
                self->get_region.selector = selector;
            }
            self->get_region.method = method;
            continue;
        }
        if (selector == (Method) __detector_set_pixel_format) {
            if (tag) {
                self->set_pixel_format.tag = tag;
                self->set_pixel_format.selector = selector;
            }
            self->set_pixel_format.method = method;
            continue;
        }
        if (selector == (Method) __detector_get_pixel_format) {
            if (tag) {
                self->get_pixel_format.tag = tag;
                self->get_pixel_format.selector = selector;
            }
            self->get_pixel_format.method = method;
            continue;
        }
        if (selector == (Method) __detector_expose) {
            if (tag) {
                self->expose.tag = tag;
                self->expose.selector = selector;
            }
            self->expose.method = method;
            continue;
        }
        if (selector == (Method) __detector_raw) {
            if (tag) {
                self->raw.tag = tag;
                self->raw.selector = selector;
            }
            self->raw.method = method;
            continue;
        }
        if (selector == (Method) __detector_load) {
            if (tag) {
                self->load.tag = tag;
                self->load.selector = selector;
            }
            self->load.method = method;
            continue;
        }
        if (selector == (Method) __detector_reload) {
            if (tag) {
                self->reload.tag = tag;
                self->reload.selector = selector;
            }
            self->reload.method = method;
            continue;
        }
        if (selector == (Method) __detector_unload) {
            if (tag) {
                self->unload.tag = tag;
                self->unload.selector = selector;
            }
            self->unload.method = method;
            continue;
        }
        if (selector == (Method) __detector_inspect) {
            if (tag) {
                self->inspect.tag = tag;
                self->inspect.selector = selector;
            }
            self->inspect.method = method;
            continue;
        }
        if (selector == (Method) __detector_wait) {
            if (tag) {
                self->wait.tag = tag;
                self->wait.selector = selector;
            }
            self->wait.method = method;
            continue;
        }
        if (selector == (Method) __detector_wait_for_completion) {
            if (tag) {
                self->wait_for_completion.tag = tag;
                self->wait_for_completion.selector = selector;
            }
            self->wait_for_completion.method = method;
            continue;
        }
        if (selector == (Method) __detector_status) {
            if (tag) {
                self->status.tag = tag;
                self->status.selector = selector;
            }
            self->status.method = method;
            continue;
        }
        if (selector == (Method) __detector_info) {
            if (tag) {
                self->info.tag = tag;
                self->info.selector = selector;
            }
            self->info.method = method;
            continue;
        }
        if (selector == (Method) __detector_abort) {
            if (tag) {
                self->abort.tag = tag;
                self->abort.selector = selector;
            }
            self->abort.method = method;
            continue;
        }
        if (selector == (Method) __detector_stop) {
            if (tag) {
                self->stop.tag = tag;
                self->stop.selector = selector;
            }
            self->stop.method = method;
            continue;
        }
        if (selector == (Method) __detector_get_name) {
            if (tag) {
                self->get_name.tag = tag;
                self->get_name.selector = selector;
            }
            self->get_name.method = method;
            continue;
        }
        if (selector == (Method) __detector_get_temperature) {
            if (tag) {
                self->get_temperature.tag = tag;
                self->get_temperature.selector = selector;
            }
            self->get_temperature.method = method;
            continue;
        }
        if (selector == (Method) __detector_set_temperature) {
            if (tag) {
                self->set_temperature.tag = tag;
                self->set_temperature.selector = selector;
            }
            self->set_temperature.method = method;
            continue;
        }
        if (selector == (Method) __detector_enable_cooling) {
            if (tag) {
                self->enable_cooling.tag = tag;
                self->enable_cooling.selector = selector;
            }
            self->enable_cooling.method = method;
            continue;
        }
        if (selector == (Method) __detector_disable_cooling) {
            if (tag) {
                self->disable_cooling.tag = tag;
                self->disable_cooling.selector = selector;
            }
            self->disable_cooling.method = method;
            continue;
        }
        if (selector == (Method) __detector_set) {
            if (tag) {
                self->set.tag = tag;
                self->set.selector = selector;
            }
            self->set.method = method;
            continue;
        }
        if (selector == (Method) __detector_get) {
            if (tag) {
                self->get.tag = tag;
                self->get.selector = selector;
            }
            self->get.method = method;
            continue;
        }
    }
    
#ifdef va_copy
    va_end(ap);
#endif
    
    return (void *) self;
}

static const void *___DetectorClass;

static void
__DetectorClass_destroy(void)
{
    free((void *) ___DetectorClass);
}

static void
__DetectorClass_initialize(void)
{
    ___DetectorClass = new(Class(), "__DetectorClass", Class(), sizeof(struct __DetectorClass),
                         ctor, "ctor", __DetectorClass_ctor,
                         (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(__DetectorClass_destroy);
#endif
}

const void *
__DetectorClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, __DetectorClass_initialize);
#endif
    
    return ___DetectorClass;
}

static const void *___Detector;

static void
__Detector_destroy(void)
{
    free((void *)___Detector);
}

static void
__Detector_initialize(void)
{

    ___Detector = new(__DetectorClass(), "__Detector", Object(), sizeof(struct __Detector),
                      ctor, "ctor", __Detector_ctor,
                      dtor, "dtor", __Detector_dtor,
                      forward, "forward", __Detector_forward,
                      __detector_clear_option, "clear_option", __Detector_clear_option,
                      __detector_set_option, "set_option", __Detector_set_option,
                      __detector_get_option, "get_option", __Detector_get_option,
                      __detector_set_directory, "set_directory", __Detector_set_directory,
                      __detector_get_directory, "get_directory", __Detector_get_directory,
                      __detector_set_prefix, "set_prefix", __Detector_set_prefix,
                      __detector_get_prefix, "get_prefix", __Detector_get_prefix,
                      __detector_set_template, "set_template", __Detector_set_template,
                      __detector_get_template, "get_template", __Detector_get_template,
                      __detector_set_pre_acquisition, "set_pre_acquisition", __Detector_set_pre_acquisition,
                      __detector_set_post_acquisition, "set_pre_acquisition", __Detector_set_post_acquisition,
                      __detector_set_name_convention, "set_name_convention", __Detector_set_name_convention,
                      
                      __detector_wait, "wait", __Detector_wait,
                      __detector_power_on, "power_on", __Detector_power_on,
                      __detector_power_off, "power_off", __Detector_power_off,
                      __detector_enable_cooling, "enable_cooling", __Detector_enable_cooling,
                      __detector_disable_cooling, "disable_cooling", __Detector_disable_cooling,
                      __detector_init, "init", __Detector_init,
                      __detector_set_binning, "set_binning", __Detector_set_binning,
                      __detector_get_binning, "get_binning", __Detector_get_binning,
                      __detector_set_exposure_time, "set_exposure_time", __Detector_set_exposure_time,
                      __detector_get_exposure_time, "get_exposure_time", __Detector_get_exposure_time,
                      __detector_set_frame_rate, "set_frame_rate", __Detector_set_frame_rate,
                      __detector_get_frame_rate, "get_frame_rate", __Detector_get_frame_rate,
                      __detector_set_gain, "set_gain", __Detector_set_gain,
                      __detector_get_gain, "get_gain", __Detector_get_gain,
                      __detector_set_readout_rate, "set_readout_rate", __Detector_set_readout_rate,
                      __detector_get_readout_rate, "get_readout_rate", __Detector_get_readout_rate,
                      __detector_set_temperature, "set_temperature", __Detector_set_temperature,
                      __detector_get_temperature, "get_temperature", __Detector_get_temperature,
                      __detector_set_region, "set_region", __Detector_set_region,
                      __detector_get_region, "get_region", __Detector_get_region,
                      __detector_get_name, "get_name", __Detector_get_name,
                      __detector_get, "get", __Detector_get,
                      __detector_set, "set", __Detector_set,
                      
                      //__detector_raw, "raw", __Detector_raw,
                      //__detector_load, "load", __Detector_load,
                      //__detector_reload, "reload", __Detector_reload,
                      //__detector_unload, "unload", __Detector_unload,
                    (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(__Detector_destroy);
#endif
}

const void *
__Detector(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, __Detector_initialize);
#endif
    
    return ___Detector;
}

/*
 * Virtual detector.
 */

struct VirtualDetectorExposureArg {
    struct VirtualDetector *detector;
    void *rpc;
    void *string; /* serialized header data string. */
    int format; /* string format */
};

static const void *virtual_detector_virtual_table(void);

static void *
VirtualDetector_ctor(void *_self, va_list *app)
{
    struct VirtualDetector *self = super_ctor(VirtualDetector(), _self, app);
    
    self->_.d_state.state = DETECTOR_STATE_UNINITIALIZED;
    const char *name = va_arg(*app, const char *);

    self->_.d_cap.width = 1024;
    self->_.d_cap.height = 1024;
    self->_.d_cap.n_chip = 1;

    self->_.d_cap.binning_available = true;
    self->_.d_cap.n_x_binning = 2;
    self->_.d_cap.x_binning_array = (uint32_t *) Malloc(sizeof(size_t) * self->_.d_cap.n_x_binning);
    self->_.d_cap.x_binning_array[0] = 2;

    self->_.d_cap.x_binning_array[1] = 4;
    self->_.d_cap.n_y_binning = 2;
    self->_.d_cap.y_binning_array = (uint32_t *) Malloc(sizeof(size_t) * self->_.d_cap.n_y_binning);
    self->_.d_cap.y_binning_array[0] = 2;
    self->_.d_cap.y_binning_array[1] = 4;

    self->_.d_cap.offset_available = true;
    self->_.d_cap.x_offset_min = 0;
    self->_.d_cap.x_offset_max = 1024;
    self->_.d_cap.y_offset_min = 0;
    self->_.d_cap.y_offset_max = 1024;
    self->_.d_cap.image_width_min = 0;
    self->_.d_cap.image_width_max = 1024;
    self->_.d_cap.image_height_min = 0;
    self->_.d_cap.image_height_max = 1024;
    
    self->_.d_cap.frame_rate_available = true;
    self->_.d_cap.frame_rate_min = 0.;
    self->_.d_cap.frame_rate_max = 1. / 0.001;
    
    self->_.d_cap.exposure_time_available = true;
    self->_.d_cap.exposure_time_max = 7200.;
    self->_.d_cap.exposure_time_min = 0.;
    
    self->_.d_cap.gain_available = true;
    self->_.d_cap.n_gain = 2;
    self->_.d_cap.gain_array = (double *) Malloc(sizeof(double) * self->_.d_cap.n_gain);
    self->_.d_cap.gain_array[0] = 3.;
    self->_.d_cap.gain_array[1] = 7.;
    
    self->_.d_cap.pixel_format_available = false;
    
    self->_.d_cap.cooling_available = false;
    
    self->_.d_param.image_width = self->_.d_cap.width;
    self->_.d_param.image_height = self->_.d_cap.height;
    self->_.d_param.pixel_format = DETECTOR_PIXEL_FORMAT_MONO_16;
    self->_.d_param.gain = self->_.d_cap.gain_array[0];
    
    self->_.d_exp.last_frame_filling_flag = false;
    self->_.d_exp.stop_flag = false;
    self->_.d_exp.notify_each_frame_done = true;
    self->_.d_state.options |= DETECTOR_OPTION_NOTIFY_EACH_COMPLETION;
    //self->_.d_exp.notify_last_frame_filling = true;
    //self->_.d_state.options |= DETECTOR_OPTION_NOTIFY_LAST_FILLING;

    self->_._vtab= virtual_detector_virtual_table();
    
    return (void *) self;
}

static void *
VirtualDetector_dtor(void *_self)
{
    struct VirtualDetector *self = cast(VirtualDetector(), _self);
    
    return super_dtor(VirtualDetector(), _self);
}

static void *
VirtualDetectorClass_ctor(void *_self, va_list *app)
{
    struct VirtualDetectorClass *self = super_ctor(VirtualDetectorClass(), _self, app);
    
    self->_.raw.method = (Method) 0;
    self->_.init.method = (Method) 0;
    self->_.status.method = (Method) 0;
    
    self->_.get_binning.method = (Method) 0;
    self->_.set_binning.method = (Method) 0;
    self->_.set_exposure_time.method = (Method) 0;
    self->_.get_exposure_time.method = (Method) 0;
    self->_.set_frame_rate.method = (Method) 0;
    self->_.get_frame_rate.method = (Method) 0;
    self->_.set_gain.method = (Method) 0;
    self->_.get_gain.method = (Method) 0;
    self->_.set_region.method = (Method) 0;
    self->_.get_region.method = (Method) 0;
    self->_.inspect.method = (Method) 0;
self->_.reg.method = (Method) 0;
    //self->_.wait.method = (Method) 0;
    self->_.wait_for_completion.method = (Method) 0;
    self->_.raw.method = (Method) 0;
    self->_.expose.method = (Method) 0;
    self->_.status.method = (Method) 0;
    self->_.info.method = (Method) 0;
    self->_.stop.method = (Method) 0;
    self->_.abort.method = (Method) 0;
    
    return self;
}

static const void *_VirtualDetectorClass;

static void
VirtualDetectorClass_destroy(void)
{
    free((void *) _VirtualDetectorClass);
}

static void
VirtualDetectorClass_initialize(void)
{
    _VirtualDetectorClass = new(__DetectorClass(), "VirtualDetectorClass", __DetectorClass(), sizeof(struct VirtualDetectorClass),
                                ctor, "", VirtualDetectorClass_ctor,
                                (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(VirtualDetectorClass_destroy);
#endif
}

const void *
VirtualDetectorClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, VirtualDetectorClass_initialize);
#endif
    
    return _VirtualDetectorClass;
}

static const void *_VirtualDetector;

static void
VirtualDetector_destroy(void)
{
    free((void *)_VirtualDetector);
}

static void
VirtualDetector_initialize(void)
{
    _VirtualDetector = new(VirtualDetectorClass(), "VirtualDetector", __Detector(), sizeof(struct VirtualDetector),
                           ctor, "ctor", VirtualDetector_ctor,
                           dtor, "dtor", VirtualDetector_dtor,
                           (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(VirtualDetector_destroy);
#endif
}

const void *
VirtualDetector(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, VirtualDetector_initialize);
#endif

    return _VirtualDetector;
}

static int
VirtualDetector_power_on(void *_self)
{
    struct VirtualDetector *self = cast(VirtualDetector(), _self);
    
    unsigned int state;
    uint16_t options;

    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;

    if ((state&DETECTOR_STATE_MALFUNCTION) && !(options&DETECTOR_OPTION_IGNORE_DEVMAL)) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        return AAOS_EDEVMAL;
    }
    if (state == DETECTOR_STATE_OFFLINE) {
        self->_.d_state.state &= ~DETECTOR_STATE_MALFUNCTION;
        self->_.d_state.state |= DETECTOR_STATE_UNINITIALIZED;
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);

    return AAOS_OK;
}

static int
VirtualDetector_power_off(void *_self)
{
    struct VirtualDetector *self = cast(VirtualDetector(), _self);
    
    unsigned int state;
    uint16_t options;
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    if ((state&DETECTOR_STATE_MALFUNCTION) && !(options&DETECTOR_OPTION_IGNORE_DEVMAL)) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        return AAOS_EDEVMAL;
    }
    while (self->_.d_state.state == DETECTOR_STATE_READING || self->_.d_state.state == DETECTOR_STATE_EXPOSING) {
        Pthread_cond_wait(&self->_.d_state.cond, &self->_.d_state.mtx);
    }
    self->_.d_state.state = DETECTOR_STATE_OFFLINE;
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    return AAOS_OK;
}

static int
VirtualDetector_init(void *_self)
{
    struct VirtualDetector *self = cast(VirtualDetector(), _self);
    
    unsigned int state;
    uint16_t options;

    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    if ((state&DETECTOR_STATE_MALFUNCTION) && !(options&DETECTOR_OPTION_IGNORE_DEVMAL)) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        return AAOS_EDEVMAL;
    }
    state &= ~DETECTOR_STATE_MALFUNCTION;
    if (state == DETECTOR_STATE_UNINITIALIZED) {
        self->_.d_state.state &= ~DETECTOR_STATE_MALFUNCTION;
        self->_.d_state.state |= DETECTOR_STATE_IDLE;
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    return AAOS_OK;
}

static int
VirtualDetector_set_binning(void *_self, uint32_t x_binning, uint32_t y_binning)
{
    struct VirtualDetector *self = cast(VirtualDetector(), _self);
    
    size_t i;
    unsigned int state;
    uint16_t options;
    
    if (!self->_.d_cap.binning_available) {
        return AAOS_ENOTSUP;
    }
    
    if (self->_.d_cap.x_binning_array != NULL) {
        for (i = 0; i < self->_.d_cap.n_x_binning; i++) {
            if (x_binning == self->_.d_cap.x_binning_array[i]) {
                break;
            }
        }
        if (i == self->_.d_cap.n_x_binning) {
            return AAOS_EINVAL;
        }
    } else {
        if (x_binning > self->_.d_cap.x_binning_max || x_binning < self->_.d_cap.x_binning_min) {
            return AAOS_EINVAL;
        }
    }
    
    if (self->_.d_cap.y_binning_array != NULL) {
        for (i = 0; i < self->_.d_cap.n_y_binning; i++) {
            if (y_binning == self->_.d_cap.y_binning_array[i]) {
                break;
            }
        }
        if (i == self->_.d_cap.n_y_binning) {
            return AAOS_EINVAL;
        }
    } else {
        if (y_binning > self->_.d_cap.y_binning_max || y_binning < self->_.d_cap.y_binning_min) {
            return AAOS_EINVAL;
        }
    }
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    if ((state&DETECTOR_STATE_MALFUNCTION) && !(options&DETECTOR_OPTION_IGNORE_DEVMAL)) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        return AAOS_EDEVMAL;
    }
    state &= ~DETECTOR_STATE_MALFUNCTION;
    switch (state) {
        case DETECTOR_STATE_OFFLINE:
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EPWROFF;
            break;
        case DETECTOR_STATE_UNINITIALIZED:
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EUNINIT;
            break;
        case DETECTOR_STATE_READING:
        case DETECTOR_STATE_EXPOSING:
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EBUSY;
            break;
        default:
            break;
    }
    self->_.d_param.x_binning = x_binning;
    self->_.d_param.y_binning = y_binning;
    self->_.d_param.x_offset = 0;
    self->_.d_param.y_offset = 0;
    self->_.d_param.image_width = self->_.d_cap.width / self->_.d_param.x_binning;
    self->_.d_param.image_height = self->_.d_cap.height / self->_.d_param.y_binning;
    self->_.d_cap.width = self->_.d_param.image_width;
    self->_.d_cap.height = self->_.d_param.image_height;
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    return AAOS_OK;
}

static int
VirtualDetector_get_binning(void *_self, size_t *x_binning, size_t *y_binning)
{
    struct VirtualDetector *self = cast(VirtualDetector(), _self);
    
    unsigned int state;
    uint16_t options;
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    if ((state&DETECTOR_STATE_MALFUNCTION) && !(options&DETECTOR_OPTION_IGNORE_DEVMAL)) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        return AAOS_EDEVMAL;
    }
    state &= ~DETECTOR_STATE_MALFUNCTION;
    switch (state) {
        case DETECTOR_STATE_OFFLINE:
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EPWROFF;
            break;
        case DETECTOR_STATE_UNINITIALIZED:
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EUNINIT;
            break;
        case DETECTOR_STATE_READING:
        case DETECTOR_STATE_EXPOSING:
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EBUSY;
            break;
        default:
            break;
    }
    *x_binning = self->_.d_param.x_binning;
    *y_binning = self->_.d_param.y_binning;
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    return AAOS_OK;
}

static int
VirtualDetector_set_exposure_time(void *_self, double exposure_time)
{
    struct VirtualDetector *self = cast(VirtualDetector(), _self);
    
    size_t i;
    unsigned int state;
    uint16_t options;
    
    if (!self->_.d_cap.exposure_time_available) {
        return AAOS_ENOTSUP;
    }
    if (exposure_time > self->_.d_cap.exposure_time_max|| exposure_time < self->_.d_cap.exposure_time_min) {
        return AAOS_EINVAL;
    }
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    if ((state&DETECTOR_STATE_MALFUNCTION) && !(options&DETECTOR_OPTION_IGNORE_DEVMAL)) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        return AAOS_EDEVMAL;
    }
    state &= ~DETECTOR_STATE_MALFUNCTION;
    switch (state) {
        case DETECTOR_STATE_OFFLINE:
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EPWROFF;
            break;
        case DETECTOR_STATE_UNINITIALIZED:
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EUNINIT;
            break;
        case DETECTOR_STATE_READING:
        case DETECTOR_STATE_EXPOSING:
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EBUSY;
            break;
        default:
            break;
    }
    self->_.d_param.exposure_time = exposure_time;
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    return AAOS_OK;
}

static int
VirtualDetector_get_exposure_time(void *_self, double *exposure_time)
{
    struct VirtualDetector *self = cast(VirtualDetector(), _self);
    
    unsigned int state;
    uint16_t options;
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    if ((state&DETECTOR_STATE_MALFUNCTION) && !(options&DETECTOR_OPTION_IGNORE_DEVMAL)) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        return AAOS_EDEVMAL;
    }
    state &= ~DETECTOR_STATE_MALFUNCTION;
    switch (state) {
        case DETECTOR_STATE_OFFLINE:
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EPWROFF;
            break;
        case DETECTOR_STATE_UNINITIALIZED:
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EUNINIT;
            break;
        //case DETECTOR_STATE_READING:
        //case DETECTOR_STATE_EXPOSING:
        //    Pthread_mutex_unlock(&self->_.d_state.mtx);
        //    return AAOS_EBUSY;
        //    break;
        default:
            break;
    }
    *exposure_time = self->_.d_param.exposure_time;
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    return AAOS_OK;
}

static int
VirtualDetector_set_frame_rate(void *_self, double frame_rate)
{
    struct VirtualDetector *self = cast(VirtualDetector(), _self);
    
    size_t i;
    unsigned int state;
    uint16_t options;
    
    if (!self->_.d_cap.frame_rate_available) {
        return AAOS_ENOTSUP;
    }
    if (frame_rate > self->_.d_cap.frame_rate_max || frame_rate < self->_.d_cap.frame_rate_min) {
        return AAOS_EINVAL;
    }
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    if ((state&DETECTOR_STATE_MALFUNCTION) && !(options&DETECTOR_OPTION_IGNORE_DEVMAL)) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        return AAOS_EDEVMAL;
    }
    state &= ~DETECTOR_STATE_MALFUNCTION;
    switch (state) {
        case DETECTOR_STATE_OFFLINE:
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EPWROFF;
            break;
        case DETECTOR_STATE_UNINITIALIZED:
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EUNINIT;
            break;
        case DETECTOR_STATE_READING:
        case DETECTOR_STATE_EXPOSING:
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EBUSY;
            break;
        default:
            break;
    }
    self->_.d_param.frame_rate = frame_rate;
    //self->_.d_cap.exposure_time_min = 1. / frame_rate;
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    return AAOS_OK;
}

static int
VirtualDetector_get_frame_rate(void *_self, double *frame_rate)
{
    struct VirtualDetector *self = cast(VirtualDetector(), _self);
    
    unsigned int state;
    uint16_t options;
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    if ((state&DETECTOR_STATE_MALFUNCTION) && !(options&DETECTOR_OPTION_IGNORE_DEVMAL)) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        return AAOS_EDEVMAL;
    }
    state &= ~DETECTOR_STATE_MALFUNCTION;
    switch (state) {
        case DETECTOR_STATE_OFFLINE:
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EPWROFF;
            break;
        case DETECTOR_STATE_UNINITIALIZED:
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EUNINIT;
            break;
        //case DETECTOR_STATE_READING:
        //case DETECTOR_STATE_EXPOSING:
        //    Pthread_mutex_unlock(&self->_.d_state.mtx);
        //    return AAOS_EBUSY;
        //    break;
        default:
            break;
    }
    *frame_rate = self->_.d_param.frame_rate;
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    return AAOS_OK;
}

static int
VirtualDetector_set_gain(void *_self, double gain)
{
    struct VirtualDetector *self = cast(VirtualDetector(), _self);
    
    size_t i;
    unsigned int state;
    uint16_t options;
    
    if (!self->_.d_cap.gain_available) {
        return AAOS_ENOTSUP;
    }
    
    if (self->_.d_cap.gain_array != NULL) {
        for (i = 0; i < self->_.d_cap.n_gain; i++) {
            if (fabs(self->_.d_cap.gain_array[i] - gain) < 0.001) {
                gain = self->_.d_cap.gain_array[i];
                break;
            }
        }
        if (i == self->_.d_cap.n_gain) {
            return AAOS_EINVAL;
        }
    } else {
        if (gain > self->_.d_cap.gain_max || gain < self->_.d_cap.gain_min) {
            return AAOS_EINVAL;
        }
    }
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    if ((state&DETECTOR_STATE_MALFUNCTION) && !(options&DETECTOR_OPTION_IGNORE_DEVMAL)) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        return AAOS_EDEVMAL;
    }
    state &= ~DETECTOR_STATE_MALFUNCTION;
    switch (state) {
        case DETECTOR_STATE_OFFLINE:
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EPWROFF;
            break;
        case DETECTOR_STATE_UNINITIALIZED:
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EUNINIT;
            break;
        case DETECTOR_STATE_READING:
        case DETECTOR_STATE_EXPOSING:
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EBUSY;
            break;
        default:
            break;
    }
    self->_.d_param.gain = gain;
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    return AAOS_OK;
}

static int
VirtualDetector_get_gain(void *_self, double *gain)
{
    struct VirtualDetector *self = cast(VirtualDetector(), _self);
    
    unsigned int state;
    uint16_t options;
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    if ((state&DETECTOR_STATE_MALFUNCTION) && !(options&DETECTOR_OPTION_IGNORE_DEVMAL)) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        return AAOS_EDEVMAL;
    }
    state &= ~DETECTOR_STATE_MALFUNCTION;
    switch (state) {
        case DETECTOR_STATE_OFFLINE:
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EPWROFF;
            break;
        case DETECTOR_STATE_UNINITIALIZED:
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EUNINIT;
            break;
        //case DETECTOR_STATE_READING:
        //case DETECTOR_STATE_EXPOSING:
        //    Pthread_mutex_unlock(&self->_.d_state.mtx);
        //    return AAOS_EBUSY;
        //    break;
        default:
            break;
    }
    *gain = self->_.d_param.gain;
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    return AAOS_OK;
}

static int
VirtualDetector_set_pixel_format(void *_self, uint32_t pixel_format)
{
    struct VirtualDetector *self = cast(VirtualDetector(), _self);
    
    size_t i;
    unsigned int state;
    uint16_t options;
    
    if (!self->_.d_cap.pixel_format_available) {
        return AAOS_ENOTSUP;
    }
    
    if (self->_.d_cap.pixel_format_array != NULL) {
        for (i = 0; i < self->_.d_cap.n_pixel_format; i++) {
            if (self->_.d_cap.pixel_format_array[i] == pixel_format) {
                break;
            }
        }
        if (i == self->_.d_cap.n_pixel_format) {
            return AAOS_EINVAL;
        }
    } else {
        return AAOS_ENOENT;
    }
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    if ((state&DETECTOR_STATE_MALFUNCTION) && !(options&DETECTOR_OPTION_IGNORE_DEVMAL)) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        return AAOS_EDEVMAL;
    }
    state &= ~DETECTOR_STATE_MALFUNCTION;
    switch (state) {
        case DETECTOR_STATE_OFFLINE:
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EPWROFF;
            break;
        case DETECTOR_STATE_UNINITIALIZED:
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EUNINIT;
            break;
        case DETECTOR_STATE_READING:
        case DETECTOR_STATE_EXPOSING:
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EBUSY;
            break;
        default:
            self->_.d_param.pixel_format = pixel_format;
            break;
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    return AAOS_OK;
}

static int
VirtualDetector_get_pixel_format(void *_self, uint32_t *pixel_format)
{
    struct VirtualDetector *self = cast(VirtualDetector(), _self);
     
    Pthread_mutex_lock(&self->_.d_state.mtx);
    *pixel_format = self->_.d_param.pixel_format;
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    return AAOS_OK;
}

static int
VirtualDetector_set_readout_rate(void *_self, double readout_rate)
{
    struct VirtualDetector *self = cast(VirtualDetector(), _self);
    
    size_t i;
    unsigned int state;
    uint16_t options;
    
    if (!self->_.d_cap.readout_rate_available) {
        return AAOS_ENOTSUP;
    }
    
    if (self->_.d_cap.readout_rate_array != NULL) {
        for (i = 0; i < self->_.d_cap.n_readout_rate; i++) {
            if (fabs(self->_.d_cap.readout_rate_array[i] - readout_rate) < 0.001) {
                readout_rate = self->_.d_cap.readout_rate_array[i];
                break;
            }
        }
        if (i == self->_.d_cap.n_readout_rate) {
            return AAOS_EINVAL;
        }
    } else {
        if (readout_rate > self->_.d_cap.readout_rate_max || readout_rate < self->_.d_cap.readout_rate_min) {
            return AAOS_EINVAL;
        }
    }
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    if ((state&DETECTOR_STATE_MALFUNCTION) && !(options&DETECTOR_OPTION_IGNORE_DEVMAL)) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        return AAOS_EDEVMAL;
    }
    state &= ~DETECTOR_STATE_MALFUNCTION;
    switch (state) {
        case DETECTOR_STATE_OFFLINE:
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EPWROFF;
            break;
        case DETECTOR_STATE_UNINITIALIZED:
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EUNINIT;
            break;
        case DETECTOR_STATE_READING:
        case DETECTOR_STATE_EXPOSING:
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EBUSY;
            break;
        default:
            break;
    }
    self->_.d_param.readout_rate = readout_rate;
    switch (self->_.d_param.pixel_format) {
        case DETECTOR_PIXEL_FORMAT_MONO_8:
            self->_.d_cap.frame_rate_max = (readout_rate * 1000000.) / (self->_.d_param.image_width * self->_.d_param.image_height * 8);
            break;
        case DETECTOR_PIXEL_FORMAT_MONO_12:
            self->_.d_cap.frame_rate_max = (readout_rate * 1000000.) / (self->_.d_param.image_width * self->_.d_param.image_height * 12);
            break;
        case DETECTOR_PIXEL_FORMAT_MONO_14:
            self->_.d_cap.frame_rate_max = (readout_rate * 1000000.) / (self->_.d_param.image_width * self->_.d_param.image_height * 14);
            break;
        case DETECTOR_PIXEL_FORMAT_MONO_32:
            self->_.d_cap.frame_rate_max = (readout_rate * 1000000.) / (self->_.d_param.image_width * self->_.d_param.image_height * 32);
            break;
        case DETECTOR_PIXEL_FORMAT_MONO_64:
            self->_.d_cap.frame_rate_max = (readout_rate * 1000000.) / (self->_.d_param.image_width * self->_.d_param.image_height * 64);
            break;
        default:
            self->_.d_cap.frame_rate_max = (readout_rate * 1000000.) / (self->_.d_param.image_width * self->_.d_param.image_height * 16);
            break;
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    return AAOS_OK;
}

static int
VirtualDetector_get_readout_rate(void *_self, double *readout_rate)
{
    struct VirtualDetector *self = cast(VirtualDetector(), _self);
    
    unsigned int state;
    uint16_t options;
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    if ((state&DETECTOR_STATE_MALFUNCTION) && !(options&DETECTOR_OPTION_IGNORE_DEVMAL)) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        return AAOS_EDEVMAL;
    }
    state &= ~DETECTOR_STATE_MALFUNCTION;
    switch (state) {
        case DETECTOR_STATE_OFFLINE:
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EPWROFF;
            break;
        case DETECTOR_STATE_UNINITIALIZED:
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EUNINIT;
            break;
        //case DETECTOR_STATE_READING:
        //case DETECTOR_STATE_EXPOSING:
        //    Pthread_mutex_unlock(&self->_.d_state.mtx);
        //    return AAOS_EBUSY;
        //    break;
        default:
            break;
    }
    *readout_rate = self->_.d_param.readout_rate;
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    return AAOS_OK;
}

static int
VirtualDetector_set_region(void *_self, uint32_t x_offset, uint32_t y_offset, uint32_t width, uint32_t height)
{
    struct VirtualDetector *self = cast(VirtualDetector(), _self);
    
    size_t i;
    unsigned int state;
    uint16_t options;
    
    if (!self->_.d_cap.offset_available) {
        return AAOS_ENOTSUP;
    }
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    if (x_offset > self->_.d_cap.x_offset_max || x_offset < self->_.d_cap.x_offset_min || (x_offset + width) > self->_.d_cap.width || y_offset > self->_.d_cap.y_offset_max || y_offset < self->_.d_cap.y_offset_min || (y_offset + height) > self->_.d_cap.height) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        return AAOS_EINVAL;
    }
    
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    if ((state&DETECTOR_STATE_MALFUNCTION) && !(options&DETECTOR_OPTION_IGNORE_DEVMAL)) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        return AAOS_EDEVMAL;
    }
    state &= ~DETECTOR_STATE_MALFUNCTION;
    switch (state) {
        case DETECTOR_STATE_OFFLINE:
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EPWROFF;
            break;
        case DETECTOR_STATE_UNINITIALIZED:
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EUNINIT;
            break;
        case DETECTOR_STATE_READING:
        case DETECTOR_STATE_EXPOSING:
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EBUSY;
            break;
        default:
            break;
    }
    self->_.d_param.x_offset = x_offset;
    self->_.d_param.image_width = width;
    self->_.d_param.y_offset = y_offset;
    self->_.d_param.image_height = height;
    switch (self->_.d_param.pixel_format) {
        case DETECTOR_PIXEL_FORMAT_MONO_8:
            self->_.d_cap.frame_rate_max = (self->_.d_param.readout_rate * 1000000.) / (self->_.d_param.image_width * self->_.d_param.image_height * 8);
            break;
        case DETECTOR_PIXEL_FORMAT_MONO_12:
            self->_.d_cap.frame_rate_max = (self->_.d_param.readout_rate * 1000000.) / (self->_.d_param.image_width * self->_.d_param.image_height * 12);
            break;
        case DETECTOR_PIXEL_FORMAT_MONO_14:
            self->_.d_cap.frame_rate_max = (self->_.d_param.readout_rate * 1000000.) / (self->_.d_param.image_width * self->_.d_param.image_height * 14);
            break;
        case DETECTOR_PIXEL_FORMAT_MONO_32:
            self->_.d_cap.frame_rate_max = (self->_.d_param.readout_rate * 1000000.) / (self->_.d_param.image_width * self->_.d_param.image_height * 32);
            break;
        case DETECTOR_PIXEL_FORMAT_MONO_64:
            self->_.d_cap.frame_rate_max = (self->_.d_param.readout_rate * 1000000.) / (self->_.d_param.image_width * self->_.d_param.image_height * 64);
            break;
        default:
            self->_.d_cap.frame_rate_max = (self->_.d_param.readout_rate * 1000000.) / (self->_.d_param.image_width * self->_.d_param.image_height * 16);
            break;
    }
    
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    return AAOS_OK;
}

static int
VirtualDetector_get_region(void *_self, uint32_t *x_offset, uint32_t *y_offset, uint32_t *width, uint32_t *height)
{
    struct VirtualDetector *self = cast(VirtualDetector(), _self);
    
    unsigned int state;
    uint16_t options;
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    if ((state&DETECTOR_STATE_MALFUNCTION) && !(options&DETECTOR_OPTION_IGNORE_DEVMAL)) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        return AAOS_EDEVMAL;
    }
    state &= ~DETECTOR_STATE_MALFUNCTION;
    switch (state) {
        case DETECTOR_STATE_OFFLINE:
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EPWROFF;
            break;
        case DETECTOR_STATE_UNINITIALIZED:
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EUNINIT;
            break;
        //case DETECTOR_STATE_READING:
        //case DETECTOR_STATE_EXPOSING:
        //    Pthread_mutex_unlock(&self->_.d_state.mtx);
        //    return AAOS_EBUSY;
        //    break;
        default:
            break;
    }
    *x_offset = self->_.d_param.x_offset;
    *width = self->_.d_param.image_width;
    *y_offset = self->_.d_param.y_offset;
    *height = self->_.d_param.image_height;
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    return AAOS_OK;
}

static int
VirtualDetector_disable_cooling(void *_self)
{
    struct VirtualDetector *self = cast(VirtualDetector(), _self);
    
    unsigned int state;
    uint16_t options;
    
    if (!self->_.d_cap.cooling_available) {
        return AAOS_ENOTSUP;
    }
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    if ((state&DETECTOR_STATE_MALFUNCTION) && !(options&DETECTOR_OPTION_IGNORE_DEVMAL)) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        return AAOS_EDEVMAL;
    }
    self->_.d_param.is_cooling_enable = false;
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    return AAOS_OK;
}

static int
VirtualDetector_enable_cooling(void *_self)
{
    struct VirtualDetector *self = cast(VirtualDetector(), _self);
    
    unsigned int state;
    uint16_t options;
    
    if (!self->_.d_cap.cooling_available) {
        return AAOS_ENOTSUP;
    }
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    if ((state&DETECTOR_STATE_MALFUNCTION) && !(options&DETECTOR_OPTION_IGNORE_DEVMAL)) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        return AAOS_EDEVMAL;
    }
    self->_.d_param.is_cooling_enable = true;
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    return AAOS_OK;
}

static int
VirtualDetector_set_temperature(void *_self, double temperature)
{
    struct VirtualDetector *self = cast(VirtualDetector(), _self);
    
    size_t i;
    unsigned int state;
    uint16_t options;
    
    if (!self->_.d_cap.cooling_available) {
        return AAOS_ENOTSUP;
    }
    
    if (temperature > self->_.d_cap.cooling_temperature_max || temperature < self->_.d_cap.cooling_temperature_min) {
        return AAOS_EINVAL;
    }
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    if ((state&DETECTOR_STATE_MALFUNCTION) && !(options&DETECTOR_OPTION_IGNORE_DEVMAL)) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        return AAOS_EDEVMAL;
    }
    
    self->_.d_param.temperature = temperature;
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    return AAOS_OK;
}

static int
VirtualDetector_get_temperature(void *_self, double *temperature)
{
    struct VirtualDetector *self = cast(VirtualDetector(), _self);
    
    unsigned int state;
    uint16_t options;
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    if ((state&DETECTOR_STATE_MALFUNCTION) && !(options&DETECTOR_OPTION_IGNORE_DEVMAL)) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        return AAOS_EDEVMAL;
    }
    *temperature = self->_.d_param.temperature;
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    return AAOS_OK;
}

static double
VirtualDetector_readout_time_nl(struct VirtualDetector *self)
{
    double readout_time;
    
    switch (self->_.d_param.pixel_format) {
        case DETECTOR_PIXEL_FORMAT_MONO_8:
            readout_time = (self->_.d_param.image_width * self->_.d_param.image_height * 8) / (self->_.d_param.readout_rate * 1000000.);
            break;
        case DETECTOR_PIXEL_FORMAT_MONO_10_PACKED:
            readout_time = (self->_.d_param.image_width * self->_.d_param.image_height * 10) / (self->_.d_param.readout_rate * 1000000.);
            break;
        case DETECTOR_PIXEL_FORMAT_MONO_12_PACKED:
            readout_time = (self->_.d_param.image_width * self->_.d_param.image_height * 12) / (self->_.d_param.readout_rate * 1000000.);
            break;
        case DETECTOR_PIXEL_FORMAT_MONO_14_PACKED:
            readout_time = (self->_.d_param.image_width * self->_.d_param.image_height * 14) / (self->_.d_param.readout_rate * 1000000.);
            break;
        case DETECTOR_PIXEL_FORMAT_MONO_18_PACKED:
            readout_time = (self->_.d_param.image_width * self->_.d_param.image_height * 14) / (self->_.d_param.readout_rate * 1000000.);
            break;
        case DETECTOR_PIXEL_FORMAT_MONO_24_PACKED:
            readout_time = (self->_.d_param.image_width * self->_.d_param.image_height * 24) / (self->_.d_param.readout_rate * 1000000.);
            break;
        case DETECTOR_PIXEL_FORMAT_MONO_18:
        case DETECTOR_PIXEL_FORMAT_MONO_24:
        case DETECTOR_PIXEL_FORMAT_MONO_32:
            readout_time = (self->_.d_param.image_width * self->_.d_param.image_height * 32) / (self->_.d_param.readout_rate * 1000000.);
            break;
        case DETECTOR_PIXEL_FORMAT_MONO_64:
            readout_time = (self->_.d_param.image_width * self->_.d_param.image_height * 64) / (self->_.d_param.readout_rate * 1000000.);
            break;
        default:
            readout_time = (self->_.d_param.image_width * self->_.d_param.image_height * 16) / (self->_.d_param.readout_rate * 1000000.);
            break;
    }
    
    return readout_time;
}

static void
VirtualDetector_generate_frame(struct VirtualDetector *detector, void **data)
{
    size_t width = detector->_.d_param.image_width, height = detector->_.d_param.image_width, n_chip = detector->_.d_cap.n_chip;
    
    /*
    double gain = detector->_.d_param.gain, read_noise = detector->read_noise, bias = detector->bias_level;
     */
    switch (detector->_.d_param.pixel_format) {
        case DETECTOR_PIXEL_FORMAT_MONO_8:
            *data = Malloc(width * height * n_chip);
            break;
        case DETECTOR_PIXEL_FORMAT_MONO_10:
        case DETECTOR_PIXEL_FORMAT_MONO_10_PACKED:
        case DETECTOR_PIXEL_FORMAT_MONO_12:
        case DETECTOR_PIXEL_FORMAT_MONO_12_PACKED:
        case DETECTOR_PIXEL_FORMAT_MONO_14:
        case DETECTOR_PIXEL_FORMAT_MONO_14_PACKED:
        case DETECTOR_PIXEL_FORMAT_MONO_16:
            *data = Malloc(width * height * n_chip * 2);
            break;
        case DETECTOR_PIXEL_FORMAT_MONO_18:
        case DETECTOR_PIXEL_FORMAT_MONO_18_PACKED:
        case DETECTOR_PIXEL_FORMAT_MONO_24:
        case DETECTOR_PIXEL_FORMAT_MONO_24_PACKED:
        case DETECTOR_PIXEL_FORMAT_MONO_32:
            *data = Malloc(width * height * n_chip * 4);
            break;
        case DETECTOR_PIXEL_FORMAT_MONO_64:
            *data = Malloc(width * height * n_chip * 8);
            break;
        default:
            break;
    }
}

/*
static void *
VirtualDetector_do_expose(void *arg)
{
    //struct  VirtualDetector *detector = (struct  VirtualDetector *) arg;
    va_list *app = (va_list *) arg;
    struct VirtualDetector *detector = va_arg(*app, struct VirtualDetector *);
    
    size_t i, n = detector->_.d_exp.request_frames;
    double frame_length = 1. / detector->_.d_param.frame_rate;
    double readout_time = detector->_.d_param.image_width * detector->_.d_param.image_height / detector->_.d_param.readout_rate;
    double gap_time = frame_length - readout_time - detector->_.d_param.exposure_time;
    int bitpix = USHORT_IMG, datatype = TUSHORT;
    long naxes[2];
    
    char *filename = NULL;
    int status = 0;
    
    switch (detector->_.d_param.pixel_format) {
        case 8:
            bitpix = BYTE_IMG;
            datatype = TBYTE;
            break;
        case 16:
            bitpix = USHORT_IMG;
            datatype = TUSHORT;
            break;
        case 32:
            bitpix = ULONG_IMG;
            datatype = TULONG;
            break;
        case 64:
            bitpix = ULONGLONG_IMG;
            datatype = TULONGLONG;
            break;
        default:
            break;
    }
    
    naxes[0] = detector->_.d_param.image_width;
    naxes[1] = detector->_.d_param.image_width;

    if (!detector->_.d_exp.notify_each_frame_done) {
        filename = (char *) Malloc(FILENAMESIZE);
        if (detector->_.d_proc.tpl_fptr == NULL) {
            if (detector->_.d_proc.tpl_filename != NULL) {
                if (detector->_.d_proc.image_directory != NULL) {
                    snprintf(filename, FILENAMESIZE, "%s/%s", detector->_.d_proc.image_directory, detector->_.d_proc.tpl_filename);
                } else {
                    snprintf(filename, FILENAMESIZE, "%s", detector->_.d_proc.tpl_filename);
                }
                fitsfile *tpl_fptr, *img_fptr;
                fits_open_file(&tpl_fptr, filename, READONLY, &status);
                detector->_.d_proc.name_convention(detector, filename, FILENAMESIZE, app);
                if (status == 0) {
                    detector->_.d_proc.tpl_fptr = (void *) tpl_fptr;
                    fits_create_file(&img_fptr, filename, &status);
                    detector->_.d_proc.img_fptr = (void *) img_fptr;
                    fits_copy_file(tpl_fptr, img_fptr, 1, 1, 1, &status);
                } else {
                    fits_create_file(&img_fptr, filename, &status);
                    detector->_.d_proc.img_fptr = (void *) img_fptr;
                }
            } else {
                detector->_.d_proc.name_convention(detector, filename, FILENAMESIZE, app);
                fitsfile *tpl_fptr, *img_fptr;
                tpl_fptr = (fitsfile *) detector->_.d_proc.tpl_fptr;
                fits_create_file(&img_fptr, filename, &status);
                fits_copy_file(tpl_fptr, img_fptr, 1, 1, 1, &status);
                detector->_.d_proc.img_fptr = (void *) img_fptr;
            }
        } else {
            fitsfile *img_fptr, *tpl_fptr;
            tpl_fptr = (fitsfile *) detector->_.d_proc.tpl_fptr;
            detector->_.d_proc.name_convention(detector, filename, FILENAMESIZE, app);
            fits_create_file(&img_fptr, filename, &status);
            fits_copy_file(tpl_fptr, img_fptr, 1, 1, 1, &status);
            detector->_.d_proc.img_fptr = (void *) img_fptr;
        }
    }
    for (i = 1; i < n; i++) {
        void *data;
        fitsfile *img_fptr;
        Pthread_mutex_lock(&detector->mtx);
        if (detector->stop_flag) {
            Pthread_mutex_unlock(&detector->mtx);
            return NULL;
        }
        Pthread_mutex_unlock(&detector->mtx);
        
        
        Pthread_mutex_lock(&detector->_.d_state.mtx);
        detector->_.d_state.state = DETECTOR_STATE_EXPOSING;
        Pthread_mutex_unlock(&detector->_.d_state.mtx);
        Nanosleep(detector->_.d_param.exposure_time);
        
       
        Pthread_mutex_lock(&detector->_.d_state.mtx);
        detector->_.d_state.state = DETECTOR_STATE_READING;
        Pthread_mutex_unlock(&detector->_.d_state.mtx);
        Nanosleep(readout_time);
        
        
        if (detector->_.d_exp.notify_last_frame_filling && i == n - 1) {
            Pthread_mutex_lock(&detector->_.d_exp.mtx);
            detector->_.d_exp.last_frame_filling_flag = 1;
            Pthread_mutex_unlock(&detector->_.d_exp.mtx);
            Pthread_cond_signal(&detector->_.d_exp.cond);
        }
        VirtualDetector_generate_frame(detector, &data);
        if (detector->_.d_exp.notify_each_frame_done) {
            filename = (char *) Malloc(FILENAMESIZE);
            if (detector->_.d_proc.tpl_fptr == NULL) {
                if (detector->_.d_proc.image_directory != NULL) {
                    snprintf(filename, FILENAMESIZE, "%s/%s", detector->_.d_proc.image_directory, detector->_.d_proc.tpl_filename);
                } else {
                    snprintf(filename, FILENAMESIZE, "%s", detector->_.d_proc.tpl_filename);
                }
                fitsfile *tpl_fptr;
                fits_open_file(&tpl_fptr, filename, READONLY, &status);
                detector->_.d_proc.name_convention(detector, filename, FILENAMESIZE, app);
                if (status == 0) {
                    detector->_.d_proc.tpl_fptr = (void *) tpl_fptr;
                    fits_create_file(&img_fptr, filename, &status);
                    detector->_.d_proc.img_fptr = (void *) img_fptr;
                    fits_copy_file(tpl_fptr, img_fptr, 1, 1, 1, &status);
                } else {
                    fits_create_file(&img_fptr, filename, &status);
                    detector->_.d_proc.img_fptr = (void *) img_fptr;
                }
            } else {
                detector->_.d_proc.name_convention(detector, filename, FILENAMESIZE, app);
                fitsfile *tpl_fptr = (fitsfile *) detector->_.d_proc.tpl_fptr;
                fits_create_file(&img_fptr, filename, &status);
                fits_copy_file(tpl_fptr, img_fptr, 1, 1, 1, &status);
                detector->_.d_proc.img_fptr = (void *) img_fptr;
            }
            fits_create_img(img_fptr, bitpix, 2, naxes, &status);
            fits_write_img(img_fptr, datatype, 1, naxes[0] * naxes[1], (void *) data, &status);
            fits_close_file(img_fptr, &status);
            free(data);
            threadsafe_queue_push(detector->_.d_proc.queue, filename);
        } else {
            img_fptr = (fitsfile *) detector->_.d_proc.img_fptr;
            fits_create_img(img_fptr, bitpix, 2, naxes, &status);
            fits_write_img(img_fptr, datatype, 1, naxes[0] * naxes[1], (void *) data, &status);
            free(data);
            if (i == n - 1) {
                fits_close_file(img_fptr, &status);
                detector->_.d_proc.img_fptr = NULL;
                threadsafe_queue_push(detector->_.d_proc.queue, filename);
            }
        }
        if (gap_time > 0.00001) {
            Nanosleep(gap_time);
        }
        Pthread_mutex_lock(&detector->_.d_state.mtx);
        detector->_.d_state.state = DETECTOR_STATE_IDLE;
        Pthread_mutex_unlock(&detector->_.d_state.mtx);
    }
    return NULL;
}
*/

static void
VirtualDetector_do_expose_cleanup_fitsfile(void *arg)
{
    fitsfile *fptr = (fitsfile *) arg;
    int status = 0;
    
    if (fptr != NULL) {
        fits_close_file(fptr, &status);
    }

}

static void
VirtualDetector_do_expose_cleanup_data(void *arg)
{
    free(arg);
}

static void
VirtualDetector_do_expose_cleanup_cond_signal(void *arg)
{
    Pthread_cond_signal((pthread_cond_t *) arg);
    //free(arg);
}

static void
VirtualDetector_write_image(struct VirtualDetector *detector, fitsfile *fptr, void *data)
{
    size_t width = detector->_.d_param.image_width, height = detector->_.d_param.image_width, n_chip = detector->_.d_cap.n_chip, x_n_chip = detector->_.d_cap.x_n_chip, y_n_chip = detector->_.d_cap.y_n_chip;
    int status = 0, bitpix, datatype, naxis = 2;
    long naxes[2];
    struct timespec tp;
    struct tm tm_buf;
    char buf[TIMESTAMPSIZE];
    
    Clock_gettime(CLOCK_REALTIME, &tp);
    
    naxes[0] = width * x_n_chip;
    naxes[1] = height * y_n_chip;
    
    if (fptr != NULL) {
        switch (detector->_.d_param.pixel_format) {
            case DETECTOR_PIXEL_FORMAT_MONO_8:
                bitpix = BYTE_IMG;
                datatype = TBYTE;
                break;
            case DETECTOR_PIXEL_FORMAT_MONO_10:
            case DETECTOR_PIXEL_FORMAT_MONO_10_PACKED:
            case DETECTOR_PIXEL_FORMAT_MONO_12:
            case DETECTOR_PIXEL_FORMAT_MONO_12_PACKED:
            case DETECTOR_PIXEL_FORMAT_MONO_14:
            case DETECTOR_PIXEL_FORMAT_MONO_14_PACKED:
            case DETECTOR_PIXEL_FORMAT_MONO_16:
                bitpix = USHORT_IMG;
                datatype = TUSHORT;
                break;
            case DETECTOR_PIXEL_FORMAT_MONO_18:
            case DETECTOR_PIXEL_FORMAT_MONO_18_PACKED:
            case DETECTOR_PIXEL_FORMAT_MONO_24:
            case DETECTOR_PIXEL_FORMAT_MONO_24_PACKED:
            case DETECTOR_PIXEL_FORMAT_MONO_32:
                bitpix = ULONG_IMG;
                datatype = TUINT;
                break;
            case DETECTOR_PIXEL_FORMAT_MONO_64:
                bitpix = ULONGLONG_IMG;
                datatype = TULONGLONG;
                break;
            default:
                bitpix = USHORT_IMG;
                datatype = TUSHORT;
                break;
        }
        
        fits_create_img(fptr, bitpix, naxis, naxes, &status);
        if (status != 0) {
            fprintf(stderr, "fits_create_img error: %d\n", status);
        }
        gmtime_r(&tp.tv_sec, &tm_buf);
        strftime(buf, TIMESTAMPSIZE, "%Y-%m-%d", &tm_buf);
        fits_update_key_str(fptr, "DATE-OBS", buf, "end data of this frame", &status);
        strftime(buf, TIMESTAMPSIZE, "%H:%m:%d", &tm_buf);
        snprintf(buf + strlen(buf), TIMESTAMPSIZE - strlen(buf), ".%03d", (int) floor(tp.tv_nsec / 1000000));
        fits_update_key_str(fptr, "TIME-OBS", buf, "end data of this frame", &status);
        fits_write_img(fptr, datatype, 1, naxes[0] * naxes[1], data, &status);
        if (status != 0) {
            fprintf(stderr, "fits_write_img error: %d\n", status);
        }
    }
}

static void *
VirtualDetector_do_expose_thr(void *arg)
{
    struct VirtualDetectorExposureArg *myarg = (struct VirtualDetectorExposureArg *) arg;
    struct VirtualDetector *detector = myarg->detector;
    double readout_time = VirtualDetector_readout_time_nl(detector);
    uint16_t options = detector->_.d_state.options;
    char filename[FILENAMESIZE];
    uint32_t i;
    int status = 0;
    fitsfile *fptr;
    void *data = NULL;
    void *string = myarg->string;
    void *rpc = myarg->rpc;
    int format = myarg->format;

    free(arg);

    detector->_.d_exp.success_frames = 0;
    Pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
    pthread_cleanup_push(VirtualDetector_do_expose_cleanup_cond_signal, &detector->_.d_exp.cond);
    if (detector->_.d_state.options&DETECTOR_OPTION_NOTIFY_EACH_COMPLETION) {
        /*
         * All the requeted frames will be saved into seperated fits files.
         */
        for (i = 0; i < detector->_.d_exp.request_frames; i++) {
            /*
             *
             */
            fptr = NULL;
            Pthread_mutex_lock(&detector->_.d_state.mtx);
            detector->_.d_state.state &= DETECTOR_STATE_MALFUNCTION;
            detector->_.d_state.state |= DETECTOR_STATE_EXPOSING;
            Pthread_mutex_unlock(&detector->_.d_state.mtx);
	
            if (detector->_.d_proc.name_convention != NULL) {
                detector->_.d_proc.name_convention(detector, filename, FILENAMESIZE, i + 1, detector->_.d_exp.request_frames);
            } else {
                __Detector_default_name_convention((struct __Detector *)detector, filename, FILENAMESIZE, i + 1, detector->_.d_exp.request_frames);
            }
                      
            /*
             * Simulate exposure 
             */
            
            Pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
            Nanosleep(detector->_.d_param.exposure_time);
            Pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
            
            Pthread_mutex_lock(&detector->_.d_exp.mtx);
            detector->_.d_exp.count = i;
            if (detector->_.d_exp.notify_last_frame_filling && i == detector->_.d_exp.request_frames - 1) {
                detector->_.d_exp.last_frame_filling_flag = true;
                Pthread_cond_signal(&detector->_.d_exp.cond);
            }
            Pthread_mutex_unlock(&detector->_.d_exp.mtx);
            /*
             * Simulate read out
             */
            
            Pthread_mutex_lock(&detector->_.d_state.mtx);
            detector->_.d_state.state &= DETECTOR_STATE_MALFUNCTION;
            detector->_.d_state.state |= DETECTOR_STATE_READING;
            Pthread_mutex_unlock(&detector->_.d_state.mtx);
            if (detector->_.d_proc.tpl_fptr != NULL) {
                fits_create_file(&fptr, filename, &status);
                fits_copy_file(detector->_.d_proc.tpl_fptr, fptr, 0, 1, 1, &status);
                if (status != 0) {
                
                }
            } else {
                fits_create_template(&fptr, filename, detector->_.d_proc.tpl_filename, &status);
                if (status != 0) {
                }
            }

            if (detector->_.d_proc.pre_acquisition != NULL) {
                
            } else {
                __Detector_default_pre_acquisition((struct __Detector *)  detector, filename, fptr);
            }
            detector->_.d_proc.img_fptr = fptr;
            
            Pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
            Nanosleep(1./detector->_.d_param.frame_rate - detector->_.d_param.exposure_time);
            Pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
            
            VirtualDetector_generate_frame(detector, &data);
            detector->_.d_exp.success_frames++;

            VirtualDetector_write_image(detector, fptr, data);
            free(data);
            data = NULL;
	
            if (detector->_.d_proc.post_acquisition != NULL) {
		
            } else {
                __Detector_default_post_acquisition((struct __Detector *) detector, filename, fptr, string, format, rpc);
            }
	
            fits_close_file(fptr, &status);
            detector->_.d_proc.img_fptr = NULL;
            fptr = NULL;

            /*
             * Exposure has been stopped.
             */
            Pthread_mutex_lock(&detector->_.d_exp.mtx);
            if (detector->_.d_exp.stop_flag) {
                Pthread_mutex_unlock(&detector->_.d_exp.mtx);
                break;
            }
            Pthread_mutex_unlock(&detector->_.d_exp.mtx);
        }
    } else {
        fptr = NULL;
        if (detector->_.d_proc.name_convention != NULL) {
            detector->_.d_proc.name_convention(detector, filename, FILENAMESIZE, 0, 1);
        } else {
            __Detector_default_name_convention((struct __Detector *) detector, filename, FILENAMESIZE, 0, 1);
        }
	
        if (detector->_.d_proc.tpl_fptr != NULL) {
            fits_create_file(&fptr, filename, &status);
            fits_copy_file(detector->_.d_proc.tpl_fptr, fptr, 0, 1, 1, &status);
            if (status != 0) {

            }
        } else {
            fits_create_template(&fptr, filename, detector->_.d_proc.tpl_filename, &status);
            if (status != 0) {
	
            }
        }
        detector->_.d_proc.img_fptr = fptr;
        if (detector->_.d_proc.pre_acquisition != NULL) {
            
        } else {
            __Detector_default_pre_acquisition((struct __Detector *) detector, filename, fptr);
        }
        pthread_cleanup_push(VirtualDetector_do_expose_cleanup_fitsfile, fptr);
        for (i = 0; i < detector->_.d_exp.request_frames; i++) {
            Pthread_mutex_lock(&detector->_.d_state.mtx);
            detector->_.d_state.state &= DETECTOR_STATE_MALFUNCTION;
            detector->_.d_state.state |= DETECTOR_STATE_EXPOSING;
            Pthread_mutex_unlock(&detector->_.d_state.mtx);
            Pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
            Nanosleep(detector->_.d_param.exposure_time);
            Pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
            
            Pthread_mutex_lock(&detector->_.d_exp.mtx);
            detector->_.d_exp.count = i;

            if (detector->_.d_exp.notify_last_frame_filling && i == detector->_.d_exp.request_frames - 1) {
                detector->_.d_exp.last_frame_filling_flag = true;
                Pthread_cond_signal(&detector->_.d_exp.cond);
            }
            Pthread_mutex_unlock(&detector->_.d_exp.mtx);
            
            Pthread_mutex_lock(&detector->_.d_state.mtx);
            detector->_.d_state.state &= DETECTOR_STATE_MALFUNCTION;
            detector->_.d_state.state |= DETECTOR_STATE_READING;
            Pthread_mutex_unlock(&detector->_.d_state.mtx);
            Pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
            Nanosleep(1./detector->_.d_param.frame_rate - detector->_.d_param.exposure_time);
            Pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
            VirtualDetector_generate_frame(detector, &data);
            detector->_.d_exp.success_frames++;
            VirtualDetector_write_image(detector, fptr, data);
            free(data);
            data = NULL;
            Pthread_mutex_lock(&detector->_.d_exp.mtx);
            if (detector->_.d_exp.stop_flag) {
                Pthread_mutex_unlock(&detector->_.d_exp.mtx);
                /*
                 * TODO, close FITS file, and do post process.
                 */
                break;
            }
            Pthread_mutex_unlock(&detector->_.d_exp.mtx);
        }
        pthread_cleanup_pop(0);
        if (detector->_.d_proc.post_acquisition != NULL) {
			
        } else {
            __Detector_default_post_acquisition((struct __Detector *) detector, filename, fptr, string, format, rpc);
        }
        fits_close_file(fptr, &status);
        detector->_.d_proc.img_fptr = NULL;
    }
    pthread_cleanup_pop(0);

    return NULL;
}

static int
VirtualDetector_expose(void *_self, double exposure_time, uint32_t n_frame, va_list *app)
{
    struct VirtualDetector *self = cast(VirtualDetector(), _self);
    
    pthread_t tid;
    uint32_t i;
    unsigned int state;
    uint16_t options;
    char *filename, *string;
    void *retval;
    void *rpc;
    struct VirtualDetectorExposureArg *arg;
    int ret = AAOS_OK;
    
    rpc = va_arg(*app, void **);
    string = va_arg(*app, char *);

    arg = (struct VirtualDetectorExposureArg *) Malloc(sizeof(struct VirtualDetectorExposureArg));

    arg->detector = self;
    arg->rpc = rpc;
    arg->string = string;

    Pthread_mutex_lock(&self->_.d_exp.mtx);
    self->_.d_exp.last_frame_filling_flag = false;
    self->_.d_exp.stop_flag = false;
    self->_.d_exp.rpc = NULL;
    Pthread_mutex_unlock(&self->_.d_exp.mtx);
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    if (exposure_time > self->_.d_cap.exposure_time_max || exposure_time < self->_.d_cap.exposure_time_min) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        return AAOS_EINVAL;
    }
    if ((state&DETECTOR_STATE_MALFUNCTION) && !(options&DETECTOR_OPTION_IGNORE_DEVMAL)) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        return AAOS_EDEVMAL;
    }
    self->_.d_param.exposure_time = exposure_time;
    if ((self->_.d_state.state == DETECTOR_STATE_READING || self->_.d_state.state == DETECTOR_STATE_READING) && (options&DETECTOR_OPTION_NOWAIT)) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        return AAOS_EBUSY;
    }
    while ((self->_.d_state.state&(~DETECTOR_STATE_MALFUNCTION)) == DETECTOR_STATE_READING || (self->_.d_state.state&(~DETECTOR_STATE_MALFUNCTION)) == DETECTOR_STATE_EXPOSING) {
        Pthread_cond_wait(&self->_.d_state.cond, &self->_.d_state.mtx);
    }
    self->_.d_state.state |= DETECTOR_STATE_EXPOSING;
    self->_.d_param.exposure_time = exposure_time;
    if (exposure_time > 1. / self->_.d_param.frame_rate) {
        self->_.d_param.frame_rate = 1. / (exposure_time + VirtualDetector_readout_time_nl(self));
    }
    self->_.d_exp.request_frames = n_frame;
    Pthread_create(&tid, NULL, VirtualDetector_do_expose_thr, (void *) arg);
    self->_.d_state.tid = tid;
    Pthread_mutex_lock(&self->_.d_exp.mtx);
    self->_.d_exp.rpc = rpc;
    Pthread_mutex_unlock(&self->_.d_exp.mtx);
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    if (!self->_.d_exp.notify_last_frame_filling) {
        /*
         * Waiting for 
         */
        Pthread_join(tid, &retval);
        Pthread_mutex_lock(&self->_.d_state.mtx);
        self->_.d_state.state &= DETECTOR_STATE_MALFUNCTION;
        self->_.d_state.state |= DETECTOR_STATE_IDLE;
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        Pthread_cond_broadcast(&self->_.d_state.cond);
	
        if (retval == PTHREAD_CANCELED) {
            return AAOS_ECANCELED;
        }
        Pthread_mutex_lock(&self->_.d_exp.mtx);
        if (self->_.d_exp.stop_flag == true) {
            self->_.d_exp.stop_flag = false;
            Pthread_mutex_unlock(&self->_.d_exp.mtx);
            return AAOS_ECANCELED;
        }
        Pthread_mutex_unlock(&self->_.d_exp.mtx);
    } else {
        Pthread_mutex_lock(&self->_.d_exp.mtx);
        while (!self->_.d_exp.last_frame_filling_flag) {
            Pthread_cond_wait(&self->_.d_exp.cond, &self->_.d_exp.mtx);
        }
        if (self->_.d_exp.stop_flag == true) {
            self->_.d_exp.stop_flag = false;
            ret = AAOS_ECANCELED;
        }
        Pthread_mutex_unlock(&self->_.d_exp.mtx);
    }
    Pthread_mutex_lock(&self->_.d_exp.mtx);
    self->_.d_exp.rpc = NULL;
    Pthread_mutex_unlock(&self->_.d_exp.mtx);
    
    return ret;
}

static int
VirtualDetector_wait_for_completion(void *_self)
{
    struct VirtualDetector *self = cast(VirtualDetector(), _self);
    
    unsigned int state;
    uint16_t options;
    void *retval;
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    if ((state&DETECTOR_STATE_MALFUNCTION) && !(options&DETECTOR_OPTION_IGNORE_DEVMAL)) {
        return AAOS_EDEVMAL;
    }
    state &= ~DETECTOR_STATE_MALFUNCTION;
    
    if (state == DETECTOR_STATE_READING && self->_.d_exp.notify_last_frame_filling) {
        Pthread_join(self->_.d_state.tid, &retval);
        if (retval == PTHREAD_CANCELED) {
            return AAOS_ECANCELED;
        }
        Pthread_mutex_lock(&self->_.d_exp.mtx);
        if (self->_.d_exp.stop_flag == true) {
            self->_.d_exp.stop_flag = false;
            Pthread_mutex_unlock(&self->_.d_exp.mtx);
            return AAOS_ECANCELED;
        }
        Pthread_mutex_unlock(&self->_.d_exp.mtx);
        Pthread_mutex_lock(&self->_.d_state.mtx);
        self->_.d_state.state &= DETECTOR_STATE_MALFUNCTION;
        self->_.d_state.state |= DETECTOR_STATE_IDLE;
        Pthread_mutex_unlock(&self->_.d_state.mtx);
    }
    return AAOS_OK;
}

static int
VirtualDetector_stop(void *_self)
{
    struct VirtualDetector *self = cast(VirtualDetector(), _self);
    
    unsigned int state;
    uint16_t options;
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    if ((state&DETECTOR_STATE_MALFUNCTION) && !(options&DETECTOR_OPTION_IGNORE_DEVMAL)) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        return AAOS_EDEVMAL;
    }
    if ((self->_.d_state.state&(~DETECTOR_STATE_MALFUNCTION)) == DETECTOR_STATE_READING || (self->_.d_state.state&(~DETECTOR_STATE_MALFUNCTION)) == DETECTOR_STATE_EXPOSING) {
        Pthread_mutex_lock(&self->_.d_exp.mtx);
        self->_.d_exp.stop_flag = true;
        self->_.d_exp.last_frame_filling_flag = true;
        Pthread_cond_signal(&self->_.d_exp.cond);
        Pthread_mutex_unlock(&self->_.d_exp.mtx);
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    return AAOS_OK;
}

static int
VirtualDetector_abort(void *_self)
{
    struct VirtualDetector *self = cast(VirtualDetector(), _self);
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    if (self->_.d_state.state == DETECTOR_STATE_READING || self->_.d_state.state == DETECTOR_STATE_EXPOSING) {
        Pthread_cancel(self->_.d_state.tid);
        Pthread_mutex_lock(&self->_.d_exp.mtx);
        self->_.d_exp.last_frame_filling_flag = true;
        Pthread_cond_signal(&self->_.d_exp.cond);
        Pthread_mutex_unlock(&self->_.d_exp.mtx);
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    return AAOS_OK;
}

static int
VirtualDetector_inspect(void *_self)
{
    struct VirtualDetector *self = cast(VirtualDetector(), _self);
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    if (self->_.d_state.state&DETECTOR_STATE_MALFUNCTION) {
        self->_.d_state.state = DETECTOR_STATE_IDLE;
        Pthread_cond_broadcast(&self->_.d_state.cond);
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);

    return AAOS_OK;
}

static int
VirtualDetector_status(void *_self, void *res, size_t res_size, size_t *res_len)
{
    struct VirtualDetector *self = cast(VirtualDetector(), _self);
    
    FILE *fp;
    unsigned int state;
    bool is_cooling_enable;
    double exposure_time, frame_rate, gain, readout_rate, temperature;
    uint32_t x_offset, width, y_offset, height;
    int ret;
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    is_cooling_enable = self->_.d_param.is_cooling_enable;
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    fp = fmemopen(res, res_size, "w+");
    if (state&DETECTOR_STATE_MALFUNCTION) {
        fprintf(fp, "status:\t\tMalfunction\n");
    } else {
        fprintf(fp, "status:\t\tOK\n");
    }
    if (state&DETECTOR_STATE_EXPOSING) {
        fprintf(fp, "state:\t\tEXPOSING\n");
    } else if (state&DETECTOR_STATE_READING) {
        fprintf(fp, "state:\t\tREADING\n");
    } else {
        fprintf(fp, "state:\t\tIDLE\n");
    }

    if ((ret = VirtualDetector_get_region(self, &x_offset, &width, &y_offset, &height)) == AAOS_OK) {
        fprintf(fp, "region:\t\t%u %u %u %u\n", x_offset, width, y_offset, height);
    }
if ((ret = VirtualDetector_get_frame_rate(self, &frame_rate)) == AAOS_OK) {
        fprintf(fp, "framerate:\t%8.3f\n", frame_rate);
    }
if ((ret = VirtualDetector_get_exposure_time(self, &exposure_time)) == AAOS_OK) {
        fprintf(fp, "exptime:\t%8.3f\n", exposure_time);
    }
    if ((ret = VirtualDetector_get_gain(self, &gain)) == AAOS_OK) {
        fprintf(fp, "gain:\t\t%8.3f (e- per ADU)\n", gain);
    }
if ((ret = VirtualDetector_get_gain(self, &readout_rate)) == AAOS_OK) {
        fprintf(fp, "readrate:\t%8.3f (Mbps)\n", readout_rate);
    }
    if (is_cooling_enable && (ret = VirtualDetector_get_temperature(self, &temperature)) == AAOS_OK) {
	
	fprintf(fp, "settemp:\t%8.3f (Celsius degree)\n", temperature);
	fprintf(fp, "chiptemp:\t%8.3f (Celsius degree)\n", temperature);
    }
    fclose(fp);

if (res_len != NULL) {
	*res_len = strlen(res) + 1;
}
    
    return AAOS_OK;
}

static int
VirtualDetector_info(void *_self, void *res, size_t res_size, size_t *res_len)
{
    struct VirtualDetector *self = cast(VirtualDetector(), _self);
    
    FILE *fp;
    size_t i;
    
    size_t x_offset_min, x_offset_max, image_width_min, image_width_max, y_offset_min, y_offset_max, image_height_min, image_height_max;
    double frame_rate_min, frame_rate_max;
    

    fp = fmemopen(res, res_size, "w+");

    fprintf(fp, "name:\t\t%s\n", self->_.name);
    if (self->_.description != NULL) {
        fprintf(fp, "description:\t%s\n\n", self->_.description);
    }
    fprintf(fp, "format:\t\t%u x %u\n", self->_.d_cap.width, self->_.d_cap.height);
    fprintf(fp, "mosaic:\t\t%zd x %zd\n\n", self->_.d_cap.x_n_chip, self->_.d_cap.y_n_chip);

    if (self->_.d_cap.binning_available) {
        fprintf(fp, "binning:\tavailable\n");
        if (self->_.d_cap.x_binning_array != NULL) {
            fprintf(fp, "x_binning:\t[");
            for (i = 0; i < self->_.d_cap.n_x_binning - 1; i++) {
                fprintf(fp, "%ud, ", self->_.d_cap.x_binning_array[i]);
            }
            fprintf(fp, "%ud]\n", self->_.d_cap.x_binning_array[self->_.d_cap.n_x_binning - 1]);
        } else {
            fprintf(fp, "x_binning:\t%ud -- %ud\n", self->_.d_cap.x_binning_min, self->_.d_cap.x_binning_max);
        }
        if (self->_.d_cap.y_binning_array != NULL) {
            fprintf(fp, "y_binning:\t[");
            for (i = 0; i < self->_.d_cap.n_y_binning - 1; i++) {
                fprintf(fp, "%ud, ", self->_.d_cap.y_binning_array[i]);
            }
            fprintf(fp, "%ud]\n\n", self->_.d_cap.y_binning_array[self->_.d_cap.n_y_binning - 1]);
        } else {
            fprintf(fp, "y_binning:\t%ud -- %ud\n\n", self->_.d_cap.y_binning_min, self->_.d_cap.y_binning_max);
        }
    } else {
        fprintf(fp, "binning:\tunavailable\n\n");
    }
    
    if (self->_.d_cap.offset_available) {
        fprintf(fp, "RIO:\t\tavailable\n");
        Pthread_mutex_lock(&self->_.d_state.mtx);
        x_offset_min = self->_.d_cap.x_offset_min;
        x_offset_max = self->_.d_cap.x_offset_max;
        image_width_min = self->_.d_cap.image_width_min;
        image_width_max = self->_.d_cap.image_width_max;
        y_offset_min = self->_.d_cap.y_offset_min;
        y_offset_max = self->_.d_cap.y_offset_max;
        image_height_min = self->_.d_cap.image_height_min;
        image_height_max = self->_.d_cap.image_height_max;
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        fprintf(fp, "x_offset:\t%zd -- %zd\n", x_offset_min, x_offset_max);
        fprintf(fp, "width:\t\t%zd -- %zd\n", image_width_min, image_width_max);
        fprintf(fp, "y_offset:\t%zd -- %zd\n", y_offset_min, y_offset_max);
        fprintf(fp, "height:\t%zd -- %zd\n\n", image_height_min, image_height_max);	
    } else {
        fprintf(fp, "RIO:\t\tunavailable\n\n");
    }
    
    if (self->_.d_cap.pixel_format_available && self->_.d_cap.pixel_format_array != NULL) {
        fprintf(fp, "pixelformat:\tavailable\n");
        fprintf(fp, "pixelformat:\t[");
        for (i = 0; i < self->_.d_cap.n_pixel_format - 1; i++) {
            fprintf(fp, "%s, ", pixel_format_string[self->_.d_cap.pixel_format_array[i] -1]);
        }
        fprintf(fp, "%s]\n\n", pixel_format_string[self->_.d_cap.pixel_format_array[self->_.d_cap.n_pixel_format - 1] -1]);
    } else {
        fprintf(fp, "pixelformat:\tunavailable\n\n");
    }

    if (self->_.d_cap.frame_rate_available) {
        fprintf(fp, "framerate:\t\tavailable\n");
        Pthread_mutex_lock(&self->_.d_state.mtx);
        frame_rate_min = self->_.d_cap.frame_rate_min;
        frame_rate_max = self->_.d_cap.frame_rate_max;
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        fprintf(fp, "framerate:\t%9.3lf -- %9.3lf (fps)\n\n", frame_rate_min, frame_rate_max);
    } else {
        fprintf(fp, "framerate:\tunavailable\n\n");
    }

    if (self->_.d_cap.exposure_time_available) {
        fprintf(fp, "exptime:\tavailable\n");
        fprintf(fp, "exptime:\t%9.3lf -- %9.3lf seconds\n\n", self->_.d_cap.exposure_time_min, self->_.d_cap.exposure_time_max);	
    } else {
        fprintf(fp, "exptime:\t\tunavailable\n\n");
    }

    if (self->_.d_cap.gain_available) {
        fprintf(fp, "gain:\t\tavailable\n");
        if (self->_.d_cap.gain_array !=  NULL) {
            fprintf(fp, "gain:\t\t[");
            for (i = 0; i < self->_.d_cap.n_gain - 1; i++) {
                fprintf(fp, "%8.3f, ", self->_.d_cap.gain_array[i]);
            }
            fprintf(fp, "%8.3f] (e- per ADU)\n\n", self->_.d_cap.gain_array[self->_.d_cap.n_gain - 1]);
        } else {
            fprintf(fp, "gain:\t%8.3lf -- %8.3lf (e- per ADU)\n\n", self->_.d_cap.gain_min, self->_.d_cap.gain_max);
        }
    } else {
        fprintf(fp, "gain:\t\tunavailable\n\n");
    }
    

    if (self->_.d_cap.readout_rate_available && self->_.d_cap.readout_rate_array != NULL) {
        fprintf(fp, "readrate:\tavailable\n");
        if (self->_.d_cap.readout_rate_array !=  NULL) {
            fprintf(fp, "readrate:\t\t[");
            for (i = 0; i < self->_.d_cap.n_readout_rate - 1; i++) {
                fprintf(fp, "%8.3f, ", self->_.d_cap.readout_rate_array[i]);
            }
            fprintf(fp, "%8.3f] (Mbps)\n\n", self->_.d_cap.readout_rate_array[self->_.d_cap.n_readout_rate - 1]);
        } else {
            fprintf(fp, "readrate:\t%8.3lf -- %8.3lf (Mbps)\n\n", self->_.d_cap.readout_rate_min, self->_.d_cap.readout_rate_max);
        }
    } else {
        fprintf(fp, "readrate:\tunavailable\n\n");
    }

    if (self->_.d_cap.cooling_available) {
        fprintf(fp, "cooling:\tavailable\n");
    } else {
        fprintf(fp, "cooling:\tunavailable\n");
    }
    fclose(fp);

    if (res_len != NULL) {
        *res_len = strlen(res) + 1;
    }

    return AAOS_OK;
}

static const void *_virtual_detector_virtual_table;

static void
virtual_detector_virtual_table_destroy(void)
{
    delete((void *) _virtual_detector_virtual_table);
}

static void
virtual_detector_virtual_table_initialize(void)
{
    _virtual_detector_virtual_table = new(__DetectorVirtualTable(),
                                          __detector_init, "init", VirtualDetector_init,
                                          __detector_power_on, "power_on", VirtualDetector_power_on,
                                          __detector_power_off, "power_off", VirtualDetector_power_off,
                                          __detector_set_binning, "set_binning", VirtualDetector_set_binning,
                                          __detector_get_binning, "get_binning", VirtualDetector_get_binning,
                                          __detector_set_exposure_time, "set_exposure_time", VirtualDetector_set_exposure_time,
                                          __detector_get_exposure_time, "get_exposure_time", VirtualDetector_get_exposure_time,
                                          __detector_set_frame_rate, "set_frame_rate", VirtualDetector_set_frame_rate,
                                          __detector_get_frame_rate, "get_frame_rate", VirtualDetector_get_frame_rate,
                                          __detector_set_gain, "set_gain", VirtualDetector_set_gain,
                                          __detector_get_gain, "get_gain", VirtualDetector_get_gain,
                                          __detector_set_pixel_format, "set_pixel_format", VirtualDetector_set_pixel_format,
                                          __detector_get_pixel_format, "get_pixel_format", VirtualDetector_get_pixel_format,
                                          __detector_set_readout_rate, "set_readout_rate", VirtualDetector_set_readout_rate,
                                          __detector_get_readout_rate, "get_readout_rate", VirtualDetector_get_readout_rate,
                                          __detector_set_region, "set_region", VirtualDetector_set_region,
                                          __detector_get_region, "get_region", VirtualDetector_get_region,
                                          __detector_disable_cooling, "disable_cooling", VirtualDetector_disable_cooling,
                                          __detector_enable_cooling, "enable_cooling", VirtualDetector_enable_cooling,
                                          __detector_set_temperature, "set_temperature", VirtualDetector_set_temperature,
                                          __detector_get_temperature, "get_temperature", VirtualDetector_get_temperature,
                                          __detector_expose, "expose", VirtualDetector_expose,
                                          __detector_wait_for_completion, "wait_for_completion", VirtualDetector_wait_for_completion,
                                          __detector_abort, "abort", VirtualDetector_abort,
                                          __detector_stop, "stop", VirtualDetector_stop,
                                          __detector_status, "status",VirtualDetector_status,
									  __detector_info, "info",VirtualDetector_info,
                                          (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(virtual_detector_virtual_table_destroy);
#endif
}

static const void *
virtual_detector_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, virtual_detector_virtual_table_initialize);
#endif
    
    return _virtual_detector_virtual_table;
}

/*
 * SITIAN Camera
 */

#include "net.h"

static const char *pattern_sitian_cam = "^MSG=((ExpoTime:)|(HDR:[0-2])|(ObjectType:[0-4])|(GetTemp)|(SetTemp:-?([0-9]*.))|(SetDir:)|(MultiStart::)|(ExpoStart)|(ExpoStop)|(Status)|(ExpoAbort))\r\n$";
static regex_t preg_sitiancam;

static const void *sitiancam_virtual_table(void);

static void *
SitianCam_ctor(void *_self, va_list *app)
{
    struct SitianCam *self = super_ctor(SitianCam(), _self, app);
    
    const char *value;
    
    self->_.d_state.state = DETECTOR_STATE_UNINITIALIZED;
    value = va_arg(*app, const char *);
    self->address = (char *) Malloc(strlen(value) + 1);
    snprintf(self->address, strlen(value) + 1, "%s", value);
    value = va_arg(*app, const char *);
    self->port = (char *) Malloc(strlen(value) + 1);
    snprintf(self->port, strlen(value) + 1, "%s", value);
    
    self->_._vtab= sitiancam_virtual_table();
    
    return (void *) self;
}

static void *
SitianCam_dtor(void *_self)
{
    struct SitianCam *self = cast(SitianCam(), _self);
    
    free(self->address);
    free(self->port);
    
    if (self->camera != NULL) {
        delete(self->camera);
    }

    return super_dtor(SitianCam(), _self);
}

static void *
SitianCamClass_ctor(void *_self, va_list *app)
{
    struct SitianCamClass *self = super_ctor(SitianCamClass(), _self, app);
    
    self->_.raw.method = (Method) 0;
    self->_.init.method = (Method) 0;
    self->_.status.method = (Method) 0;
    
    self->_.inspect.method = (Method) 0;
    //self->_.wait.method = (Method) 0;
    //self->_.wait_for_completion.method = (Method) 0;
    self->_.raw.method = (Method) 0;
    self->_.expose.method = (Method) 0;
    
    return self;
}

static const void *_SitianCamClass;

static void
SitianCamClass_destroy(void)
{
    free((void *) _SitianCamClass);
}

static void
SitianCamClass_initialize(void)
{
    _SitianCamClass = new(__DetectorClass(), "SitianCamClass", __DetectorClass(), sizeof(struct SitianCamClass),
                          ctor, "", SitianCamClass_ctor,
                          (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(SitianCamClass_destroy);
#endif
}

const void *
SitianCamClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, SitianCamClass_initialize);
#endif
    
    return _SitianCamClass;
}

static const void *_SitianCam;

static void
SitianCam_destroy(void)
{
    free((void *)_SitianCam);
}

static void
SitianCam_initialize(void)
{
    _SitianCam = new(SitianCamClass(), "SitianCam", __Detector(), sizeof(struct SitianCam),
                     ctor, "ctor", SitianCam_ctor,
                     dtor, "dtor", SitianCam_dtor,
                     (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(SitianCam_destroy);
#endif
}

const void *
SitianCam(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, SitianCam_initialize);
#endif

    return _SitianCam;
}

static int
SitianCam_init(void *_self)
{
    struct SitianCam *self = cast(SitianCam(), _self);
    
    void *client;
    int ret = AAOS_OK;
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    switch (self->_.d_state.state) {
        case DETECTOR_STATE_UNINITIALIZED:
            client = new(TCPClient(), self->address, self->port);
            ret = tcp_client_connect(client, &self->camera);
            delete(client);
            break;
        case DETECTOR_STATE_MALFUNCTION:
            ret = AAOS_EDEVMAL;
            break;
        default:
            break;
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    return ret;
}

static int
SitianCam_raw(void *_self, const void *write_buffer, size_t write_buffer_size, size_t *write_size, void *read_buffer, size_t read_buffer_size, size_t *read_size)
{
    struct SitianCam *self = cast(SitianCam(), _self);
    
    int ret;
    
    if ((ret = tcp_socket_write(self->camera, write_buffer, write_buffer_size, write_size)) != AAOS_OK) {
        return ret;
    }
    
    if ((ret = tcp_socket_read_until(self->camera, read_buffer, read_buffer_size, read_size, "\r\n")) != AAOS_OK) {
        return ret;
    }
    
    return AAOS_OK;
}

static int
SitianCam_set_exposure_time(void *_self, double exposure_time)
{
    struct SitianCam *self = cast(SitianCam(), _self);
    
    int ret = AAOS_OK;
    char buffer[BUFSIZE];
    void *camera = self->camera;
    char tmp[256];
    int error_code;
    const char *s;
    
    memset(tmp, '\0', 256);
    snprintf(buffer, BUFSIZE, "MSG=ExpoTime:%.2f\r\n", exposure_time);
    Pthread_mutex_lock(&self->_.d_state.mtx);
    switch (self->_.d_state.state) {
        case DETECTOR_STATE_MALFUNCTION:
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EDEVMAL;
            break;
        case DETECTOR_STATE_UNINITIALIZED:
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EUNINIT;
            break;
        case DETECTOR_STATE_READING:
        case DETECTOR_STATE_EXPOSING:
            while (self->_.d_state.state == DETECTOR_STATE_EXPOSING || self->_.d_state.state == DETECTOR_STATE_READING) {
                Pthread_cond_wait(&self->_.d_state.cond, &self->_.d_state.mtx);
            }
            switch (self->_.d_state.state) {
                case DETECTOR_STATE_MALFUNCTION:
                    Pthread_mutex_unlock(&self->_.d_state.mtx);
                    return AAOS_EDEVMAL;
                    break;
                case DETECTOR_STATE_UNINITIALIZED:
                    Pthread_mutex_unlock(&self->_.d_state.mtx);
                    return AAOS_EUNINIT;
                    break;
                case DETECTOR_STATE_IDLE:
                    if ((ret = tcp_socket_write(camera, buffer, strlen(buffer), NULL)) != AAOS_OK) {
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        return ret;
                    }
                    if ((ret = tcp_socket_read_until(camera, buffer, BUFSIZE, NULL, "\r\n")) != AAOS_OK) {
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        return ret;
                    }
                    s = strchr(buffer, ':') + 1;
                    memcpy(tmp, s, strlen(s) - 2);
                    error_code = atoi(tmp);
                    if (error_code != 0) {
                        switch (error_code) {
                            case 1:
                                Pthread_mutex_unlock(&self->_.d_state.mtx);
                                return AAOS_EBADCMD;
                                break;
                            case 2:
                                Pthread_mutex_unlock(&self->_.d_state.mtx);
                                return AAOS_EINVAL;
                                break;
                            case 3:
                            case 4:
                                Pthread_mutex_unlock(&self->_.d_state.mtx);
                                return AAOS_EINTR;
                                break;
                            default:
                                Pthread_mutex_unlock(&self->_.d_state.mtx);
                                return AAOS_ERROR;
                                break;
                        }
                    }
                    self->_.d_param.exposure_time = exposure_time;
                    if (exposure_time > 1. / self->_.d_param.frame_rate) {
                        self->_.d_param.frame_rate = 1. / exposure_time;
                    }
                    break;
                default:
                    break;
            }
            break;
        case DETECTOR_STATE_IDLE:
            if ((ret = tcp_socket_write(camera, buffer, strlen(buffer), NULL)) != AAOS_OK) {
                Pthread_mutex_unlock(&self->_.d_state.mtx);
                return ret;
            }
            if ((ret = tcp_socket_read_until(camera, buffer, BUFSIZE, NULL, "\r\n")) != AAOS_OK) {
                Pthread_mutex_unlock(&self->_.d_state.mtx);
                return ret;
            }
            s = strchr(buffer, ':') + 1;
            memcpy(tmp, s, strlen(s) - 2);
            error_code = atoi(tmp);
            if (error_code != 0) {
                switch (error_code) {
                    case 1:
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        return AAOS_EBADCMD;
                        break;
                    case 2:
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        return AAOS_EINVAL;
                        break;
                    case 3:
                    case 4:
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        return AAOS_EINTR;
                        break;
                    default:
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        return AAOS_ERROR;
                        break;
                }
            }
            self->_.d_param.exposure_time = exposure_time;
            if (exposure_time > 1. / self->_.d_param.frame_rate) {
                self->_.d_param.frame_rate = 1. / exposure_time;
            }
            break;
        default:
            break;
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    return ret;
}

#define SITIAN_DETECTOR_GAIN_LOW        0.
#define SITIAN_DETECTOR_GAIN_HIGH       1.
#define SITIAN_DETECTOR_GAIN_LOW_HIGH   2.

static int
SitianCam_set_gain(void *_self, double gain)
{
    struct SitianCam *self = cast(SitianCam(), _self);
    
    int ret = AAOS_OK;
    char buffer[BUFSIZE];
    void *camera = self->camera;
    char tmp[256];
    int error_code;
    const char *s;
    
    memset(tmp, '\0', 256);
    if (gain != SITIAN_DETECTOR_GAIN_LOW || gain != SITIAN_DETECTOR_GAIN_HIGH || gain != SITIAN_DETECTOR_GAIN_LOW_HIGH) {
        return AAOS_EINVAL;
    }
    snprintf(buffer, BUFSIZE, "MSG=HDR:%d\r\n", (int) gain);
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    switch (self->_.d_state.state) {
        case DETECTOR_STATE_MALFUNCTION:
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EDEVMAL;
            break;
        case DETECTOR_STATE_UNINITIALIZED:
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EUNINIT;
            break;
        case DETECTOR_STATE_READING:
        case DETECTOR_STATE_EXPOSING:
            while (self->_.d_state.state == DETECTOR_STATE_EXPOSING || self->_.d_state.state == DETECTOR_STATE_READING) {
                Pthread_cond_wait(&self->_.d_state.cond, &self->_.d_state.mtx);
            }
            switch (self->_.d_state.state) {
                case DETECTOR_STATE_MALFUNCTION:
                    Pthread_mutex_unlock(&self->_.d_state.mtx);
                    return AAOS_EDEVMAL;
                    break;
                case DETECTOR_STATE_UNINITIALIZED:
                    Pthread_mutex_unlock(&self->_.d_state.mtx);
                    return AAOS_EUNINIT;
                    break;
                case DETECTOR_STATE_IDLE:
                    if ((ret = tcp_socket_write(camera, buffer, strlen(buffer), NULL)) != AAOS_OK) {
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        return ret;
                    }
                    if ((ret = tcp_socket_read_until(camera, buffer, BUFSIZE, NULL, "\r\n")) != AAOS_OK) {
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        return ret;
                    }
                    s = strchr(buffer, ':') + 1;
                    memcpy(tmp, s, strlen(s) - 2);
                    error_code = atoi(tmp);
                    if (error_code != 0) {
                        switch (error_code) {
                            case 1:
                                Pthread_mutex_unlock(&self->_.d_state.mtx);
                                return AAOS_EBADCMD;
                                break;
                            case 2:
                                Pthread_mutex_unlock(&self->_.d_state.mtx);
                                return AAOS_EINVAL;
                                break;
                            case 3:
                            case 4:
                                Pthread_mutex_unlock(&self->_.d_state.mtx);
                                return AAOS_EINTR;
                                break;
                            default:
                                Pthread_mutex_unlock(&self->_.d_state.mtx);
                                return AAOS_ERROR;
                                break;
                        }
                    }
                    self->_.d_param.gain = gain;
                    break;
                default:
                    break;
            }
            break;
        case DETECTOR_STATE_IDLE:
            if ((ret = tcp_socket_write(camera, buffer, strlen(buffer), NULL)) != AAOS_OK) {
                Pthread_mutex_unlock(&self->_.d_state.mtx);
                return ret;
            }
            if ((ret = tcp_socket_read_until(camera, buffer, BUFSIZE, NULL, "\r\n")) != AAOS_OK) {
                Pthread_mutex_unlock(&self->_.d_state.mtx);
                return ret;
            }
            s = strchr(buffer, ':') + 1;
            memcpy(tmp, s, strlen(s) - 2);
            error_code = atoi(tmp);
            if (error_code != 0) {
                switch (error_code) {
                    case 1:
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        return AAOS_EBADCMD;
                        break;
                    case 2:
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        return AAOS_EINVAL;
                        break;
                    case 3:
                    case 4:
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        return AAOS_EINTR;
                        break;
                    default:
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        return AAOS_ERROR;
                        break;
                }
            }
            self->_.d_param.gain = gain;
            break;
        default:
            break;
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    return ret;
}

static int
SitianCam_set_temperature(void *_self, double temperature)
{
    struct SitianCam *self = cast(SitianCam(), _self);
    
    int ret = AAOS_OK;
    char buffer[BUFSIZE];
    void *camera = self->camera;
    char tmp[256];
    int error_code;
    const char *s;
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    switch (self->_.d_state.state) {
        case DETECTOR_STATE_MALFUNCTION:
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EDEVMAL;
            break;
        case DETECTOR_STATE_UNINITIALIZED:
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EUNINIT;
            break;
        default:
            break;
    }
    self->_.d_param.temperature = temperature;
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    snprintf(buffer, BUFSIZE, "MSG=SetTemp:%.2f\r\n", temperature);
    
    if ((ret = tcp_socket_write(camera, buffer, strlen(buffer), NULL)) != AAOS_OK) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        return ret;
    }
    if ((ret = tcp_socket_read_until(camera, buffer, BUFSIZE, NULL, "\r\n")) != AAOS_OK) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        return ret;
    }
    
    s = strchr(buffer, ':') + 1;
    memset(tmp, '\0', 256);
    memcpy(tmp, s, strlen(s) - 2);
    error_code = atoi(tmp);
    if (error_code != 0) {
        switch (error_code) {
            case 1:
                return AAOS_EBADCMD;
                break;
            case 2:
                return AAOS_EINVAL;
                break;
            default:
                return AAOS_ERROR;
                break;
        }
    }
    
    return ret;
}

static int
SitianCam_get_temperature(void *_self, double *temperature)
{
    struct SitianCam *self = cast(SitianCam(), _self);
    
    int ret = AAOS_OK;
    char buffer[BUFSIZE];
    void *camera = self->camera;
    char tmp[256];
    int error_code;
    const char *s, *s2;
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    switch (self->_.d_state.state) {
        case DETECTOR_STATE_MALFUNCTION:
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EDEVMAL;
            break;
        case DETECTOR_STATE_UNINITIALIZED:
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EUNINIT;
            break;
        default:
            break;
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    snprintf(buffer, BUFSIZE, "MSG=GetTemp\r\n");
    
    if ((ret = tcp_socket_write(camera, buffer, strlen(buffer), NULL)) != AAOS_OK) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        return ret;
    }
    if ((ret = tcp_socket_read_until(camera, buffer, BUFSIZE, NULL, "\r\n")) != AAOS_OK) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        return ret;
    }
    
    s = strchr(buffer, ':') + 1;
    s2 = strchr(s, ':');
    memset(tmp, '\0', 256);
    memcpy(tmp, s, s2 - s - 1);
    error_code = atoi(tmp);
    if (error_code != 0) {
        switch (error_code) {
            case 1:
                return AAOS_EBADCMD;
                break;
            default:
                return AAOS_ERROR;
                break;
        }
    }
    s = s2 + 1;
    s2 = strchr(s, ':');
    memset(tmp, '\0', 256);
    memcpy(tmp, s, s2 - s - 1);
    *temperature = atof(tmp);
    
    return ret;
}

static int
SitianCam_set_directory(void *_self, const char *directory)
{
    struct SitianCam *self = cast(SitianCam(), _self);
    
    int ret = AAOS_OK;
    char buffer[BUFSIZE];
    void *camera = self->camera;
    char tmp[256];
    int error_code;
    const char *s;
    
    memset(tmp, '\0', 256);
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    switch (self->_.d_state.state) {
        case DETECTOR_STATE_MALFUNCTION:
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EDEVMAL;
            break;
        case DETECTOR_STATE_UNINITIALIZED:
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EUNINIT;
            break;
        case DETECTOR_STATE_READING:
        case DETECTOR_STATE_EXPOSING:
            while (self->_.d_state.state == DETECTOR_STATE_EXPOSING || self->_.d_state.state == DETECTOR_STATE_READING) {
                Pthread_cond_wait(&self->_.d_state.cond, &self->_.d_state.mtx);
            }
            switch (self->_.d_state.state) {
                case DETECTOR_STATE_MALFUNCTION:
                    Pthread_mutex_unlock(&self->_.d_state.mtx);
                    return AAOS_EDEVMAL;
                    break;
                case DETECTOR_STATE_UNINITIALIZED:
                    Pthread_mutex_unlock(&self->_.d_state.mtx);
                    return AAOS_EUNINIT;
                    break;
                case DETECTOR_STATE_IDLE:
                    if (self->_.d_proc.image_prefix != NULL) {
                        snprintf(buffer, BUFSIZE, "MSG=SetDir:%s/$%s_$fits\r\n", directory, self->_.d_proc.image_prefix);
                    } else {
                        snprintf(buffer, BUFSIZE, "MSG=SetDir:%s/test_$fits\r\n", directory);
                    }
                    if ((ret = tcp_socket_write(camera, buffer, strlen(buffer), NULL)) != AAOS_OK) {
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        return ret;
                    }
                    if ((ret = tcp_socket_read_until(camera, buffer, BUFSIZE, NULL, "\r\n")) != AAOS_OK) {
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        return ret;
                    }
                    s = strchr(buffer, ':') + 1;
                    memcpy(tmp, s, strlen(s) - 2);
                    error_code = atoi(tmp);
                    if (error_code != 0) {
                        switch (error_code) {
                            case 1:
                                Pthread_mutex_unlock(&self->_.d_state.mtx);
                                return AAOS_EBADCMD;
                                break;
                            case 2:
                                Pthread_mutex_unlock(&self->_.d_state.mtx);
                                return AAOS_EINVAL;
                                break;
                            case 3:
                            case 4:
                                Pthread_mutex_unlock(&self->_.d_state.mtx);
                                return AAOS_EINTR;
                                break;
                            default:
                                Pthread_mutex_unlock(&self->_.d_state.mtx);
                                return AAOS_ERROR;
                                break;
                        }
                    }
                    if (self->_.d_proc.image_directory != NULL) {
                        free(self->_.d_proc.image_directory);
                    }
                    self->_.d_proc.image_directory = (char *) Malloc(strlen(directory) + 1);
                    snprintf(self->_.d_proc.image_directory, strlen(directory) + 1, "%s", directory);
                    break;
                default:
                    break;
            }
            break;
        case DETECTOR_STATE_IDLE:
            if (self->_.d_proc.image_prefix != NULL) {
                snprintf(buffer, BUFSIZE, "MSG=SetDir:%s/$%s_$fits\r\n", directory, self->_.d_proc.image_prefix);
            } else {
                snprintf(buffer, BUFSIZE, "MSG=SetDir:%s/test_$fits\r\n", directory);
            }
            if ((ret = tcp_socket_write(camera, buffer, strlen(buffer), NULL)) != AAOS_OK) {
                Pthread_mutex_unlock(&self->_.d_state.mtx);
                return ret;
            }
            if ((ret = tcp_socket_read_until(camera, buffer, BUFSIZE, NULL, "\r\n")) != AAOS_OK) {
                Pthread_mutex_unlock(&self->_.d_state.mtx);
                return ret;
            }
            s = strchr(buffer, ':') + 1;
            memcpy(tmp, s, strlen(s) - 2);
            error_code = atoi(tmp);
            if (error_code != 0) {
                switch (error_code) {
                    case 1:
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        return AAOS_EBADCMD;
                        break;
                    case 2:
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        return AAOS_EINVAL;
                        break;
                    case 3:
                    case 4:
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        return AAOS_EINTR;
                        break;
                    default:
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        return AAOS_ERROR;
                        break;
                }
            }
            if (self->_.d_proc.image_directory != NULL) {
                free(self->_.d_proc.image_directory);
            }
            self->_.d_proc.image_directory = (char *) Malloc(strlen(directory) + 1);
            snprintf(self->_.d_proc.image_directory, strlen(directory) + 1, "%s", directory);
            break;
        default:
            break;
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    return ret;
}

static int
SitianCam_set_prefix(void *_self, const char *prefix)
{
    struct SitianCam *self = cast(SitianCam(), _self);
    
    int ret = AAOS_OK;
    char buffer[BUFSIZE];
    void *camera = self->camera;
    char tmp[256];
    int error_code;
    const char *s;
    
    memset(tmp, '\0', 256);
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    switch (self->_.d_state.state) {
        case DETECTOR_STATE_MALFUNCTION:
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EDEVMAL;
            break;
        case DETECTOR_STATE_UNINITIALIZED:
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EUNINIT;
            break;
        case DETECTOR_STATE_READING:
        case DETECTOR_STATE_EXPOSING:
            while (self->_.d_state.state == DETECTOR_STATE_EXPOSING || self->_.d_state.state == DETECTOR_STATE_READING) {
                Pthread_cond_wait(&self->_.d_state.cond, &self->_.d_state.mtx);
            }
            switch (self->_.d_state.state) {
                case DETECTOR_STATE_MALFUNCTION:
                    Pthread_mutex_unlock(&self->_.d_state.mtx);
                    return AAOS_EDEVMAL;
                    break;
                case DETECTOR_STATE_UNINITIALIZED:
                    Pthread_mutex_unlock(&self->_.d_state.mtx);
                    return AAOS_EUNINIT;
                    break;
                case DETECTOR_STATE_IDLE:
                    if (self->_.d_proc.image_directory != NULL) {
                        snprintf(buffer, BUFSIZE, "MSG=SetDir:%s/$%s_$fits\r\n", self->_.d_proc.image_directory, prefix);
                    } else {
                        snprintf(buffer, BUFSIZE, "MSG=SetDir:./$%s_$fits\r\n", prefix);
                    }
                    if ((ret = tcp_socket_write(camera, buffer, strlen(buffer), NULL)) != AAOS_OK) {
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        return ret;
                    }
                    if ((ret = tcp_socket_read_until(camera, buffer, BUFSIZE, NULL, "\r\n")) != AAOS_OK) {
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        return ret;
                    }
                    s = strchr(buffer, ':') + 1;
                    memcpy(tmp, s, strlen(s) - 2);
                    error_code = atoi(tmp);
                    if (error_code != 0) {
                        switch (error_code) {
                            case 1:
                                Pthread_mutex_unlock(&self->_.d_state.mtx);
                                return AAOS_EBADCMD;
                                break;
                            case 2:
                                Pthread_mutex_unlock(&self->_.d_state.mtx);
                                return AAOS_EINVAL;
                                break;
                            case 3:
                            case 4:
                                Pthread_mutex_unlock(&self->_.d_state.mtx);
                                return AAOS_EINTR;
                                break;
                            default:
                                Pthread_mutex_unlock(&self->_.d_state.mtx);
                                return AAOS_ERROR;
                                break;
                        }
                    }
                    if (self->_.d_proc.image_prefix != NULL) {
                        free(self->_.d_proc.image_prefix);
                    }
                    self->_.d_proc.image_prefix = (char *) Malloc(strlen(prefix) + 1);
                    snprintf(self->_.d_proc.image_prefix, strlen(prefix) + 1, "%s", prefix);
                    break;
                default:
                    break;
            }
            break;
        case DETECTOR_STATE_IDLE:
            if (self->_.d_proc.image_directory != NULL) {
                snprintf(buffer, BUFSIZE, "MSG=SetDir:%s/$%s_$fits\r\n", self->_.d_proc.image_directory, prefix);
            } else {
                snprintf(buffer, BUFSIZE, "MSG=SetDir:./$%s_$fits\r\n", prefix);
            }
            if ((ret = tcp_socket_write(camera, buffer, strlen(buffer), NULL)) != AAOS_OK) {
                Pthread_mutex_unlock(&self->_.d_state.mtx);
                return ret;
            }
            if ((ret = tcp_socket_read_until(camera, buffer, BUFSIZE, NULL, "\r\n")) != AAOS_OK) {
                Pthread_mutex_unlock(&self->_.d_state.mtx);
                return ret;
            }
            s = strchr(buffer, ':') + 1;
            memcpy(tmp, s, strlen(s) - 2);
            error_code = atoi(tmp);
            if (error_code != 0) {
                switch (error_code) {
                    case 1:
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        return AAOS_EBADCMD;
                        break;
                    case 2:
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        return AAOS_EINVAL;
                        break;
                    case 3:
                    case 4:
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        return AAOS_EINTR;
                        break;
                    default:
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        return AAOS_ERROR;
                        break;
                }
            }
            if (self->_.d_proc.image_prefix != NULL) {
                free(self->_.d_proc.image_prefix);
            }
            self->_.d_proc.image_prefix = (char *) Malloc(strlen(prefix) + 1);
            snprintf(self->_.d_proc.image_prefix, strlen(prefix) + 1, "%s", prefix);
            break;
        default:
            break;
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    return ret;
}

static int
SitianCam_expose(void *_self, double exposure_time, uint32_t n_frames, va_list *app)
{
    struct SitianCam *self = cast(SitianCam(), _self);
    
    int ret = AAOS_OK;
    char buffer[BUFSIZE];
    double frame_rate, frame_length;
    uint32_t i;
    void *camera = self->camera;
    size_t size;
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    switch (self->_.d_state.state) {
        case DETECTOR_STATE_MALFUNCTION:
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EDEVMAL;
            break;
        case DETECTOR_STATE_READING:
        case DETECTOR_STATE_EXPOSING:
            while (self->_.d_state.state != DETECTOR_STATE_IDLE) {
                Pthread_cond_wait(&self->_.d_state.cond, &self->_.d_state.mtx);
            }
            switch (self->_.d_state.state) {
                case DETECTOR_STATE_IDLE:
                    self->_.d_state.state = DETECTOR_STATE_EXPOSING;
                    break;
                case DETECTOR_STATE_MALFUNCTION:
                    Pthread_mutex_unlock(&self->_.d_state.mtx);
                    return AAOS_EDEVMAL;
                    break;
                case DETECTOR_STATE_UNINITIALIZED:
                    Pthread_mutex_unlock(&self->_.d_state.mtx);
                    return AAOS_EUNINIT;
                    break;
                default:
                    break;
            }
            break;
        case DETECTOR_STATE_UNINITIALIZED:
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EUNINIT;
            break;
        case DETECTOR_STATE_IDLE:
            self->_.d_state.state = DETECTOR_STATE_EXPOSING;
            break;
        default:
            break;
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);

    /*
    if ((ret = __detector_get_frame_rate(self, &frame_rate)) != AAOS_OK) {
        return ret;
    }
    */
    frame_rate = self->_.d_param.frame_rate;
    frame_length = 1. / frame_rate;
    if (frame_length < exposure_time) {
        /*
        if ((ret = __detector_set_frame_rate(self, 1. / exposure_time)) != AAOS_OK) {
            return ret;
        }
        frame_length = 1. / exposure_time;
         */
        frame_rate = 1. / exposure_time;
        frame_length = exposure_time;
    }
    
    snprintf(buffer, BUFSIZE, "MSG=MulStart:%.2f:%.2f:%ud\r\n", exposure_time, frame_length - exposure_time, n_frames);
    if ((ret = tcp_socket_write(camera, buffer, strlen(buffer), NULL)) != AAOS_OK) {
        return ret;
    }
    
    for (i = 0; i < n_frames; i++) {
        if ((ret = tcp_socket_read_until(camera, buffer, BUFSIZE, &size, "\r\n")) != AAOS_OK) {
            return ret;
        }
        const char *s = buffer, *s2;
        char tmp[FILENAMESIZE];
        unsigned int frame_no;
        int error_code;
        s = strchr(buffer, ':') + 1;
        s2 = strchr(s, ':');
        memset(tmp, '\0', FILENAMESIZE);
        memcpy(tmp, s, s2 - s - 1);
        error_code = atoi(tmp);
        if (error_code != 0) {
            switch (error_code) {
                case 1:
                    Pthread_mutex_lock(&self->_.d_state.mtx);
                    self->_.d_state.state = DETECTOR_STATE_IDLE;
                    Pthread_mutex_unlock(&self->_.d_state.mtx);
                    Pthread_cond_broadcast(&self->_.d_state.cond);
                    return AAOS_EBADCMD;
                    break;
                case 2:
                    Pthread_mutex_lock(&self->_.d_state.mtx);
                    self->_.d_state.state = DETECTOR_STATE_IDLE;
                    Pthread_mutex_unlock(&self->_.d_state.mtx);
                    Pthread_cond_broadcast(&self->_.d_state.cond);
                    return AAOS_EINVAL;
                    break;
                case 3:
                case 4:
                    Pthread_mutex_lock(&self->_.d_state.mtx);
                    self->_.d_state.state = DETECTOR_STATE_IDLE;
                    Pthread_mutex_unlock(&self->_.d_state.mtx);
                    Pthread_cond_broadcast(&self->_.d_state.cond);
                    return AAOS_EINTR;
                    break;
                default:
                    Pthread_mutex_lock(&self->_.d_state.mtx);
                    self->_.d_state.state = DETECTOR_STATE_IDLE;
                    Pthread_mutex_unlock(&self->_.d_state.mtx);
                    Pthread_cond_broadcast(&self->_.d_state.cond);
                    return AAOS_ERROR;
                    break;
            }
        }
        s = s2 + 1;
        s2 = strchr(s, ':');
        memset(tmp, '\0', FILENAMESIZE);
        memcpy(tmp, s, s2 - s - 1);
        frame_no = atoi(tmp);
        s = s2 + 1;
        memcpy(tmp, s, strlen(s) - 2);
        if (self->_.d_proc.post_acquisition != NULL) {
            self->_.d_proc.post_acquisition(self, tmp, app);
        } else {
            printf("%s\n", tmp);
        }
    }
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    self->_.d_state.state = DETECTOR_STATE_IDLE;
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    Pthread_cond_broadcast(&self->_.d_state.cond);
    
    return ret;
}

static const void *_sitiancam_virtual_table;

static void
sitiancam_virtual_table_destroy(void)
{

    delete((void *) _sitiancam_virtual_table);
}

static void
sitiancam_virtual_table_initialize(void)
{
    _sitiancam_virtual_table = new(__DetectorVirtualTable(),
                                   __detector_init, "init", SitianCam_init,
                                   //__detector_info, "info", GenICam_info,
                                   __detector_expose, "expose", SitianCam_expose,
                                   //__detector_set_binning, "set_binning", GenICam_set_binning,
                                   //__detector_get_binning, "get_binning", GenICam_get_binning,
                                   __detector_set_exposure_time, "set_exposure_time", SitianCam_set_exposure_time,
                                   //__detector_get_exposure_time, "get_exposure_time", GenICam_get_exposure_time,
                                   //__detector_set_frame_rate, "set_frame_rate", GenICam_set_frame_rate,
                                   //__detector_get_frame_rate, "get_frame_rate", GenICam_get_frame_rate,
                                   __detector_set_gain, "set_gain", SitianCam_set_gain,
                                   //__detector_get_gain, "get_gain", GenICam_get_gain,
                                   //__detector_set_region, "set_region", GenICam_set_region,
                                   //__detector_get_region, "get_region", GenICam_get_region,
                                   __detector_raw, "raw", SitianCam_raw,
                                   //__detector_inspect, "inspect", GenICam_inspect,
                                   //__detector_wait_for_completion, "wait_for_completion", GenICam_wait_for_last_completion,
                                   (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(sitiancam_virtual_table_destroy);
#endif
}

static const void *
sitiancam_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, sitiancam_virtual_table_initialize);
#endif
    
    return _sitiancam_virtual_table;
}

/*
 *
 *
 */

/*
 * GenICam
 * use libaravis
 * Tested camera:
 * ImperX, inc.
 */

#ifdef __USE_ARAVIS__

#include <aravis-0.8/arv.h>
#include <glib-2.0/glib.h>

static void
tp2str(struct timespec *tp, char *date_time, size_t size)
{
    time_t tloc = tp->tv_sec;
    int nloc = tp->tv_nsec / 1000000;
    struct tm res;
    size_t left;
    gmtime_r(&tloc, &res);
    strftime(date_time, size, "%Y-%m-%dT%H:%M:%S", &res);
    left = size - strlen(date_time);
    if (left > 1) {
    snprintf(date_time + strlen(date_time), left, ".%03d", nloc);
    }
}

static const void *genicam_virtual_table(void);

static void *
GenICam_ctor(void *_self, va_list *app)
{
    struct GenICam *self = super_ctor(GenICam(), _self, app);
    
    self->_.d_state.state = DETECTOR_STATE_UNINITIALIZED;
    const char *name = va_arg(*app, const char *);
    
    self->name = (char *) Malloc(strlen(name) + 1);
    snprintf(self->name, strlen(name) + 1, "%s", name);
    
    self->_._vtab= genicam_virtual_table();
    
    return (void *) self;
}

static void *
GenICam_dtor(void *_self)
{
    struct GenICam *self = cast(GenICam(), _self);
    
    free(self->name);
    
    if (ARV_IS_CAMERA(self->camera)) {
        g_object_unref(self->camera);
    }
    
    if (ARV_IS_STREAM(self->stream)) {
        g_object_unref(self->stream);
    }
    
    return super_dtor(GenICam(), _self);
}

static void *
GenICamClass_ctor(void *_self, va_list *app)
{
    struct GenICamClass *self = super_ctor(GenICamClass(), _self, app);
    
    self->_.raw.method = (Method) 0;
    self->_.init.method = (Method) 0;
    self->_.status.method = (Method) 0;
    
    self->_.get_binning.method = (Method) 0;
    self->_.set_binning.method = (Method) 0;
    self->_.set_exposure_time.method = (Method) 0;
    self->_.get_exposure_time.method = (Method) 0;
    self->_.set_frame_rate.method = (Method) 0;
    self->_.get_frame_rate.method = (Method) 0;
    self->_.set_gain.method = (Method) 0;
    self->_.get_gain.method = (Method) 0;
    self->_.set_region.method = (Method) 0;
    self->_.get_region.method = (Method) 0;
    self->_.inspect.method = (Method) 0;
    //self->_.wait.method = (Method) 0;
    self->_.wait_for_completion.method = (Method) 0;
    self->_.raw.method = (Method) 0;
    self->_.expose.method = (Method) 0;
    self->_.status.method = (Method) 0;
    self->_.info.method = (Method) 0;
    self->_.stop.method = (Method) 0;
    self->_.abort.method = (Method) 0;
    
    return self;
}

static const void *_GenICamClass;

static void
GenICamClass_destroy(void)
{
    free((void *) _GenICamClass);
}

static void
GenICamClass_initialize(void)
{
    _GenICamClass = new(__DetectorClass(), "GenICamClass", __DetectorClass(), sizeof(struct GenICamClass),
                          ctor, "", GenICamClass_ctor,
                          (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(GenICamClass_destroy);
#endif
}

const void *
GenICamClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, GenICamClass_initialize);
#endif
    
    return _GenICamClass;
}

static const void *_GenICam;

static void
GenICam_destroy(void)
{
    free((void *)_GenICam);
}

static void
GenICam_initialize(void)
{
    _GenICam = new(GenICamClass(), "GenICam", __Detector(), sizeof(struct GenICam),
                     ctor, "ctor", GenICam_ctor,
                     dtor, "dtor", GenICam_dtor,
                     (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(GenICam_destroy);
#endif
}

const void *
GenICam(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, GenICam_initialize);
#endif

    return _GenICam;
}

static int
GenICam_init(void *_self)
{
    struct GenICam *self = cast(GenICam(), _self);

    int ret = AAOS_OK;
    Pthread_mutex_lock(&self->_.d_state.mtx);
    if (self->_.d_state.state != DETECTOR_STATE_UNINITIALIZED) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        goto error;
    } else {
        ArvCamera *camera = NULL;
        GError *error = NULL;
        camera = arv_camera_new(self->name, &error);
        if (error != NULL) {
            g_clear_error(&error);
            ret = AAOS_ERROR;
        } else {
            self->camera = (void *) camera;
            self->_.d_state.state = DETECTOR_STATE_IDLE;
            
            if (arv_camera_is_binning_available(camera, NULL)) {
                gint min_ = 0, max_ = 0, x_binning = 0, y_binning  = 0;
                self->_.d_cap.binning_available = true;
                arv_camera_get_x_binning_bounds(camera, &min_, &max_, NULL);
                self->_.d_cap.x_binning_max = max_;
                self->_.d_cap.x_binning_min = min_;
                //increment = arv_camera_get_x_binning_increment(camera, NULL);
                arv_camera_get_y_binning_bounds(camera, &min_, &max_, NULL);
                self->_.d_cap.y_binning_max = max_;
                self->_.d_cap.y_binning_min = min_;
                //increment = arv_camera_get_y_binning_increment(camera, NULL);
                arv_camera_get_binning(camera, &x_binning, &y_binning, NULL);
                self->_.d_param.x_binning = x_binning;
                self->_.d_param.y_binning = y_binning;
            }
            
            {
                self->_.d_cap.offset_available = true;
                gint x, y;
                gint width, height;
                gint min_, max_;
                arv_camera_get_x_offset_bounds(camera, &min_, &max_, NULL);
                self->_.d_cap.x_offset_max = max_;
                self->_.d_cap.x_offset_min = min_;
                //increment = arv_camera_get_x_offset_increment(camera, NULL);
                arv_camera_get_y_offset_bounds(camera, &min_, &max_, NULL);
                self->_.d_cap.y_offset_max = max_;
                self->_.d_cap.y_offset_min = min_;
                //increment = arv_camera_get_y_offset_increment(camera, NULL);
                arv_camera_get_width_bounds(camera, &min_, &max_, NULL);
                self->_.d_cap.image_width_max = max_;
                self->_.d_cap.image_width_min = min_;
                //increment = arv_camera_get_width_increment(camera, NULL);
                arv_camera_get_height_bounds(camera, &min_, &max_, NULL);
                self->_.d_cap.image_height_max = max_;
                self->_.d_cap.image_height_min = min_;
                //increment = arv_camera_get_height_increment(camera, NULL);
                arv_camera_get_region(camera, &x, &y, &width, &height, NULL);
                self->_.d_param.image_width = width;
                self->_.d_param.image_height = height;
                self->_.d_param.x_offset = x;
                self->_.d_param.y_offset = y;
            }
            
            if (arv_camera_is_frame_rate_available(camera, NULL)) {
                self->_.d_cap.frame_rate_available = true;
                double min_ = 0., max_ = 0., frame_rate = 0.;
                arv_camera_get_frame_rate_bounds(camera, &min_, &max_, NULL);
                self->_.d_cap.frame_rate_max = max_;
                self->_.d_cap.frame_rate_min = min_;
                frame_rate = arv_camera_get_frame_rate(camera, NULL);
                self->_.d_param.frame_rate = frame_rate;
            }
            
            if (arv_camera_is_exposure_time_available(camera, NULL)) {
                self->_.d_cap.exposure_time_available = true;
                double min_ = 0., max_ = 0., exposure_time = 0.;
                arv_camera_get_exposure_time_bounds(camera, &min_, &max_, NULL);
                self->_.d_cap.exposure_time_max = max_;
                self->_.d_cap.exposure_time_min = min_;
                exposure_time = arv_camera_get_exposure_time(camera, NULL);
                self->_.d_param.exposure_time = exposure_time;
            }
            
            if (arv_camera_is_gain_available(camera, NULL)) {
                self->_.d_cap.gain_available = true;
                double min_ = 0., max_ = 0., gain = 0.;
                arv_camera_get_gain_bounds(camera, &min_, &max_, NULL);
                self->_.d_cap.gain_max = max_;
                self->_.d_cap.gain_min = min_;
                gain = arv_camera_get_gain(camera, NULL);
                self->_.d_param.gain = gain;
            }
        }
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
error:
    return ret;
}

static int
GenICam_raw(void *_self, const void *write_buffer, size_t write_buffer_size, size_t *write_size, void *read_buffer, size_t read_buffer_size, size_t *read_size)
{
    struct GenICam *self = cast(GenICam(), _self);
    ArvCamera *camera = (ArvCamera *) self->camera;
    int ret = AAOS_OK;
    
    GError *error = NULL;
    char cmd[16], type[32], feature[64];
    const char *s;
    gboolean available;
    
    s = (const char *) write_buffer;
    sscanf(s, "%s %s", cmd, feature);
    available = arv_camera_is_feature_available(camera, feature, &error);
    if (error == NULL) {
        if (!available) {
            ret = AAOS_EBADCMD;
            goto error;
        }
    } else {
        ret = AAOS_ERROR;
        goto error;
    }

    s = strstr(write_buffer, feature) + strlen(feature) + 1;
    if (strcmp(cmd, "get") == 0) {
        sscanf(s, "%s", type);
        if (strcmp(type, "int") == 0 || strcmp(type, "integer") == 0) {
            gint64 value = arv_camera_get_integer(camera, feature, &error);
            if (error == NULL) {
                snprintf(read_buffer, read_buffer_size, "%s: %ld", feature, value);
                if (read_size != NULL) {
                    *read_size = strlen(read_buffer) + 1;
                }
            } else {
                ret = AAOS_ERROR;
                goto error;
            }
        } else if (strcmp(type, "bool") == 0 || strcmp(type, "boolean") == 0) {
            gboolean value = arv_camera_get_boolean(camera, feature, &error);
            if (error == NULL) {
                if (value) {
                    
                    snprintf(read_buffer, read_buffer_size, "%s: TRUE\n", feature);
                } else {
                    snprintf(read_buffer, read_buffer_size, "%s: FALSE\n", feature);
                }
                if (read_size != NULL) {
                    *read_size = strlen(read_buffer) + 1;
                }
            } else {
                ret = AAOS_ERROR;
                goto error;
            }
        } else if (strcmp(type, "float") == 0 || strcmp(type, "double") == 0) {
            double value = arv_camera_get_float(camera, feature, &error);
            if (error == NULL) {
                snprintf(read_buffer, read_buffer_size, "%s: %f\n", feature, value);
                if (read_size != NULL) {
                    *read_size = strlen(read_buffer) + 1;
                }
            } else {
                ret = AAOS_ERROR;
                goto error;
            }
        } else if (strcmp(type, "enum") == 0 || strcmp(type, "enumeration") == 0) {
            guint n_values;
            gint64 *values = arv_camera_dup_available_enumerations(camera, feature, &n_values, &error);
            if (error == NULL) {
                guint i;
                char *s = read_buffer;
                ssize_t nleft = read_buffer_size;

                snprintf(s, nleft, "%s: [%ld",  feature, values[0]);
                nleft -= strlen(s);
                s += strlen(s);
                if (nleft > 0) {
                    for (i = 1; i < n_values; i++) {
                        snprintf(s, nleft, ", %ld", values[i]);
                        nleft -= strlen(s);
                        s += strlen(s);
                        if (nleft < 0) {
                            break;
                        }
                    }
                }
                if (nleft > 0) {
                    snprintf(s, nleft, "]\n");
                }
                if (read_size != NULL) {
                    *read_size = strlen(read_buffer) + 1;
                }
            } else {
                ret = AAOS_ERROR;
                goto error;
            }
    } else if (strcmp(type, "enum_as_str") == 0 || strcmp(type, "enumeration_as_string") == 0) {
            guint n_values;
            const char **values = arv_camera_dup_available_enumerations_as_strings(camera, feature, &n_values, &error);
            if (error == NULL) {
                guint i;
                gint left = read_buffer_size;
                char *idx = (char *) read_buffer;
                if (read_size != NULL) {
                    *read_size = 0;
                }
                for (i = 0; i < n_values; i++) {
                    if (left <= 0) {
                        break;
                    }
                    snprintf(idx, left, "%s", values[i]);
                    idx += strlen(idx) + 1;
                    left -= strlen(idx) + 1;
                    if (read_size != NULL) {
                        *read_size += strlen(idx) + 1;
                    }
                }
            } else {
                ret = AAOS_ERROR;
                goto error;
            }
        } else if (strcmp(type, "enum_as_name") == 0 || strcmp(type, "enumeration_as_name") == 0) {
            guint n_values;
            const char **values = arv_camera_dup_available_enumerations_as_display_names(camera, feature, &n_values, &error);
            if (error == NULL) {
                guint i;
                gint left = read_buffer_size;
                char *idx = (char *) read_buffer;
                if (read_size != NULL) {
                    *read_size = 0;
                }
                for (i = 0; i < n_values; i++) {
                    if (left <= 0) {
                        break;
                    }
                    snprintf(idx, left, "%s", values[i]);
                    idx += strlen(idx) + 1;
                    left -= strlen(idx) + 1;
                    if (read_size != NULL) {
                        *read_size += strlen(idx) + 1;
                    }
                }
            } else {
                ret = AAOS_ERROR;
                goto error;
            }
        } else if (strcmp(type, "int_incr") == 0 || strcmp(type, "integer_increment") == 0) {
            gint64 value = arv_camera_get_integer_increment(camera, feature, &error);
            if (error == NULL) {
                snprintf(read_buffer, read_buffer_size, "%s increment: %ld\n", feature, value);
                if (read_size != NULL) {
                    *read_size = strlen(read_buffer) + 1;
                }
            } else {
                ret = AAOS_ERROR;
                goto error;
            }
        } else if (strcmp(type, "float_incr") == 0 || strcmp(type, "double_incr") == 0 || strcmp(type, "float_increment") == 0 || strcmp(type, "double_increment") == 0) {
            double value = arv_camera_get_float_increment(camera, feature, &error);
            if (error == NULL) {
                snprintf(read_buffer, read_buffer_size, "%s increment: %f\n", feature, value);
                if (read_size != NULL) {
                    *read_size = strlen(read_buffer) + 1;
                }
            } else {
                ret = AAOS_ERROR;
                goto error;
            }
        } else if (strcmp(type, "float_bound") == 0 || strcmp(type, "double_bound") == 0) {
            double bounds[2];
            arv_camera_get_float_bounds(camera, feature, &bounds[0], &bounds[1], &error);
            if (error == NULL) {
                snprintf(read_buffer, read_buffer_size, "%s bounds: [%f, %f]\n", feature, bounds[0], bounds[1]);
                if (read_size != NULL) {
                    *read_size = strlen(read_buffer) + 1;
                }
            } else {
                ret = AAOS_ERROR;
                goto error;
            }
        } else if (strcmp(type, "int_bound") == 0 || strcmp(type, "integer_bound") == 0) {
            gint64 bounds[2];
            arv_camera_get_integer_bounds(camera, feature, &bounds[0], &bounds[1], &error);
            if (error == NULL) {
                snprintf(read_buffer, read_buffer_size, "%s bounds: [%ld, %ld]\n", feature, bounds[0], bounds[1]);
                if (read_size != NULL) {
                    *read_size = strlen(read_buffer) + 1;
                }
            } else {
                ret = AAOS_ERROR;
                goto error;
            }
        } else if (strcmp(type, "str") == 0 || strcmp(type, "string") == 0) {
            const char *value = arv_camera_get_string(camera, feature, &error);
            if (error == NULL) {
                snprintf(read_buffer, read_buffer_size, "%s", value);
                if (read_size != NULL) {
                    *read_size = strlen(read_buffer) + 1;
                }
            } else {
                ret = AAOS_ERROR;
                goto error;
            }
        }else {
            ret = AAOS_EBADCMD;
            goto error;
        }
    } else if (strcmp(cmd, "set") == 0) {
        sscanf(s, "%s", type);
        s = strstr(s, type) + strlen(type) + 1;
        if (strcmp(type, "bool") == 0 || strcmp(type, "boolean") == 0) {
            gboolean value;
            sscanf(s, "%d", &value);
            arv_camera_set_boolean(camera, feature, value, &error);
            if (error != NULL) {
                ret = AAOS_ERROR;
                goto error;
            }
        } else if (strcmp(type, "int") == 0 || strcmp(type, "integer") == 0) {
            gint64 value;
            sscanf(s, "%ld", &value);
            arv_camera_set_integer(camera, feature, value, &error);
            if (error != NULL) {
                ret = AAOS_ERROR;
                goto error;
            }
        } else if (strcmp(type, "double") == 0 || strcmp(type, "float") == 0) {
            double value;
            sscanf(s, "%lf", &value);
            arv_camera_set_float(camera, feature, value, &error);
            if (error != NULL) {
                ret = AAOS_ERROR;
                goto error;
            }
        } else if (strcmp(type, "str") == 0 || strcmp(type, "string") == 0) {
            while (*s == ' ' || *s == '\t')
                s++;
            arv_camera_set_string(camera, feature, s, &error);
            if (error != NULL) {
                ret = AAOS_ERROR;
                goto error;
            }
        } else {
            ret = AAOS_EBADCMD;
            goto error;
        }
        snprintf(read_buffer, read_buffer_size, "Set `%s` successfully\n", feature);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else if (strcmp(cmd, "execute") == 0) {
        arv_camera_execute_command(camera, feature, &error);
        if (error != NULL) {
            ret = AAOS_ERROR;
            goto error;
        }
        snprintf(read_buffer, read_buffer_size, "Execute `%s` successfully\n", feature);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else {
        ret = AAOS_EBADCMD;
        goto error;
    }
error:
    if (error != NULL) {
        g_clear_error(&error);
    }
    return ret;
}

/*
 * if exposure time is negative, set camera exposure duration to auto mode.
 *
 * RETURN VALUE
 *        On success, set_exposure_time() returns AAOS_OK; on error, it
 *        returns an error number.
 * ERRORS
 *        AAOS_EINVAL exposure_time is negative, but the camera do not
 *                    support auto exposure function.
 *        AAOS_EINVAL exposure_time is out of the camera's exposure time
 *                    time range.
 */

static int
GenICam_set_exposure_time_nl(void *_self, double exposure_time)
{
    struct GenICam *self = cast(GenICam(), _self);
    ArvCamera *camera = (ArvCamera *) self->camera;
    
    double exposure_time_us = 1000000. * exposure_time;
    ArvAuto auto_mode;
    gboolean auto_available = arv_camera_is_exposure_auto_available(camera, NULL);
    GError *error = NULL;
    int ret = AAOS_OK;

    if (exposure_time_us < 0) {
        if (auto_available) {
            auto_mode = arv_camera_get_exposure_time_auto(camera, &error);
            if (error != NULL) {
                ret = AAOS_ERROR;
                goto error;
            }
            if (auto_mode == ARV_AUTO_OFF) {
                arv_camera_set_exposure_time_auto(camera, ARV_AUTO_ONCE, &error);
                if (error != NULL) {
                    ret = AAOS_ERROR;
                    goto error;
                }
            }
        } else {
            ret = AAOS_ERROR;
            goto error;
        }
    } else {
        if (auto_available) {
            auto_mode = arv_camera_get_exposure_time_auto(camera, &error);
            if (error != NULL) {
                ret = AAOS_ERROR;
                goto error;
            }
            if (auto_mode != ARV_AUTO_OFF) {
                arv_camera_set_exposure_time_auto(camera, ARV_AUTO_ONCE, &error);
                if (error != NULL) {
                    ret = AAOS_ERROR;
                    goto error;
                }
            }
        }
        double min, max;
        arv_camera_get_exposure_time_bounds(camera, &min, &max, &error);
        if (error != NULL) {
            ret = AAOS_ERROR;
            goto error;
        }
        if (exposure_time_us > max || exposure_time_us < min) {
            double min, max;
            arv_camera_get_frame_rate_bounds(camera, &min, &max, &error);
            if (error != NULL) {
                ret = AAOS_ERROR;
                goto error;
            }
            if (exposure_time_us > 1000000. / min || exposure_time_us < 1000000. / max) {
                ret = AAOS_EINVAL;
                goto error;
            } else {
                arv_camera_set_frame_rate(camera, 1000000. / (exposure_time_us + 10), &error);
                if (error != NULL) {
                    ret = AAOS_ERROR;
                    goto error;
                }
                self->_.d_param.frame_rate = 1000000. / (exposure_time_us + 10);
            }
        }
        double current_exposure_time;
        current_exposure_time= arv_camera_get_exposure_time(camera, &error);
        if (error != NULL) {
            ret = AAOS_ERROR;
            goto error;
        }
        if (fabs(current_exposure_time -  exposure_time_us) > 1.) {
            Nanosleep(current_exposure_time / 1000000.);
            arv_camera_set_exposure_time(camera, exposure_time_us, &error);
            if (error != NULL) {
                ret = AAOS_ERROR;
                goto error;
            }
            self->_.d_param.exposure_time = exposure_time;
        }
    }
error:
    if (error != NULL) {
        g_clear_error(&error);
    }
    return ret;
}

static int
GenICam_set_exposure_time(void *_self, double exposure_time)
{
    struct GenICam *self = cast(GenICam(), _self);
    ArvCamera *camera = (ArvCamera *) self->camera;
    
    double exposure_time_us = 1000000. * exposure_time;
    ArvAuto auto_mode;
    gboolean auto_available = arv_camera_is_exposure_auto_available(camera, NULL);
    GError *error = NULL;
    int ret;
    double current_exposure_time;

    if (exposure_time_us < 0) {
        if (auto_available) {
            auto_mode = arv_camera_get_exposure_time_auto(camera, &error);
            if (error != NULL) {
                ret = AAOS_ERROR;
                goto error;
            }
            if (auto_mode == ARV_AUTO_OFF) {
                arv_camera_set_exposure_time_auto(camera, ARV_AUTO_ONCE, &error);
                if (error != NULL) {
                    ret = AAOS_ERROR;
                    goto error;
                }
            }
        } else {
            ret = AAOS_ERROR;
            goto error;
        }
    } else {
        if (auto_available) {
            auto_mode = arv_camera_get_exposure_time_auto(camera, &error);
            if (error != NULL) {
                ret = AAOS_ERROR;
                goto error;
            }
            if (auto_mode != ARV_AUTO_OFF) {
                arv_camera_set_exposure_time_auto(camera, ARV_AUTO_ONCE, &error);
                if (error != NULL) {
                    ret = AAOS_ERROR;
                    goto error;
                }
            }
        }
        double min, max;
        arv_camera_get_exposure_time_bounds(camera, &min, &max, &error);
        if (error != NULL) {
            ret = AAOS_ERROR;
            goto error;
        }
        
        Pthread_mutex_lock(&self->_.d_state.mtx);
        switch (self->_.d_state.state) {
            case DETECTOR_STATE_IDLE:
                if (exposure_time_us > max || exposure_time_us < min) {
                    double min, max;
                    arv_camera_get_frame_rate_bounds(camera, &min, &max, &error);
                    if (error != NULL) {
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        ret = AAOS_ERROR;
                        goto error;
                    }
                    if (exposure_time_us > 1000000. / min || exposure_time_us < 1000000. / max) {
                        ret = AAOS_EINVAL;
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        goto error;
                    } else {
                        arv_camera_set_frame_rate(camera, 1000000. / (exposure_time_us + 10), &error);
                        if (error != NULL) {
                            ret = AAOS_ERROR;
                            Pthread_mutex_unlock(&self->_.d_state.mtx);
                            goto error;
                        }
                        self->_.d_param.frame_rate = 1000000. / (exposure_time_us + 10);
                    }
                }
                
                current_exposure_time= arv_camera_get_exposure_time(camera, &error);
                if (error != NULL) {
                    ret = AAOS_ERROR;
                    Pthread_mutex_unlock(&self->_.d_state.mtx);
                    goto error;
                }
                if (fabs(current_exposure_time -  exposure_time_us) > 1.) {
                    Nanosleep(current_exposure_time / 1000000.);
                    arv_camera_set_exposure_time(camera, exposure_time_us, &error);
                    if (error != NULL) {
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        ret = AAOS_ERROR;
                        goto error;
                    }
                    self->_.d_param.exposure_time = exposure_time;
                    Pthread_mutex_unlock(&self->_.d_state.mtx);
                }
                break;
            case DETECTOR_STATE_EXPOSING:
            case DETECTOR_STATE_READING:
                while (self->_.d_state.state == DETECTOR_STATE_READING || self->_.d_state.state == DETECTOR_STATE_EXPOSING) {
                    Pthread_cond_wait(&self->_.d_state.cond, &self->_.d_state.mtx);
                }
                if (exposure_time_us > max || exposure_time_us < min) {
                    double min, max;
                    arv_camera_get_frame_rate_bounds(camera, &min, &max, &error);
                    if (error != NULL) {
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        ret = AAOS_ERROR;
                        goto error;
                    }
                    if (exposure_time_us > 1000000. / min || exposure_time_us < 1000000. / max) {
                        ret = AAOS_EINVAL;
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        goto error;
                    } else {
                        arv_camera_set_frame_rate(camera, 1000000. / (exposure_time_us + 10), &error);
                        if (error != NULL) {
                            ret = AAOS_ERROR;
                            Pthread_mutex_unlock(&self->_.d_state.mtx);
                            goto error;
                        }
                        self->_.d_param.frame_rate = 1000000. / (exposure_time_us + 10);
                    }
                }
                
                current_exposure_time= arv_camera_get_exposure_time(camera, &error);
                if (error != NULL) {
                    ret = AAOS_ERROR;
                    Pthread_mutex_unlock(&self->_.d_state.mtx);
                    goto error;
                }
                if (fabs(current_exposure_time -  exposure_time_us) > 1.) {
                    Nanosleep(current_exposure_time / 1000000.);
                    arv_camera_set_exposure_time(camera, exposure_time_us, &error);
                    if (error != NULL) {
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        ret = AAOS_ERROR;
                        goto error;
                    }
                    self->_.d_param.exposure_time = exposure_time;
                    Pthread_mutex_unlock(&self->_.d_state.mtx);
                }
                break;
            case DETECTOR_STATE_MALFUNCTION:
                ret = AAOS_EDEVMAL;
                break;
            default:
                break;
        }
        Pthread_mutex_unlock(&self->_.d_state.mtx);
    }
error:
    
    if (error != NULL) {
        g_clear_error(&error);
    }
    return ret;
}

static void
GenICam_name_convention(void *user_data, char *filename, size_t size, ...)
{
    struct GenICam *udata = (struct GenICam *) user_data;
    
    struct tm res;
    size_t left;
    struct timespec *tp;
    va_list ap;
    
    va_start(ap, size);
    tp = va_arg(ap, struct timespec *);
    va_end(ap);
    
    time_t tloc = tp->tv_sec;
    left = size;
    
    if (udata->_.d_proc.image_directory != NULL) {
        snprintf(filename, size, "%s", udata->_.d_proc.image_directory);
        left = size - strlen(filename);
        filename += strlen(filename);
    }
    
    if (udata->_.d_proc.image_prefix != NULL) {
        snprintf(filename, size, "%s", udata->_.d_proc.image_prefix);
        left = size - strlen(filename);
        filename += strlen(filename);
    }
    
    gmtime_r(&tloc, &res);
    
    if (left > 1) {
        strftime(filename, left, "%y%m%d_%H%M%S.fits", &res);
    }
}

static void
GenICam_process_image(void *user_data, ArvBuffer *buffer, char *filename, size_t size)
{
    struct GenICam *udata = (struct GenICam *) user_data;
    
    fitsfile *fptr = (fitsfile *) udata->_.d_proc.img_fptr;
    int status = 0;
    struct timespec tp;
    
    gint x, y, width, height;
    const void *data = arv_buffer_get_data(buffer, &size);
    size_t pixel_size = ARV_PIXEL_FORMAT_BIT_PER_PIXEL(arv_buffer_get_image_pixel_format(buffer));
    long naxes[2];
    int bitpix, datatype;
    char date_time[32];
    
    arv_buffer_get_image_region(buffer, &x, &y, &width, &height);
    switch (pixel_size) {
        case 8:
            bitpix = BYTE_IMG;
            datatype = TBYTE;
            break;
        case 16:
            bitpix = USHORT_IMG;
            datatype = TUSHORT;
            break;
        case 32:
            bitpix = ULONG_IMG;
            datatype = TULONG;
            break;
        case 64:
            bitpix = ULONGLONG_IMG;
            datatype = TULONGLONG;
            break;
        default:
            break;
    }
    naxes[0] = width;
    naxes[1] = height;
    
    int flag = 0;
    if (fptr == NULL) {
        flag = 1;
        Clock_gettime(CLOCK_REALTIME, &tp);
        GenICam_name_convention(user_data, filename, size, &tp);
        if (udata->_.d_proc.tpl_fptr == NULL) {
            if (udata->_.d_proc.tpl_filename != NULL) {
                char tmp[FLEN_FILENAME];
                if (udata->_.d_proc.image_directory != NULL) {
                    snprintf(tmp, FLEN_FILENAME, "%s/%s", udata->_.d_proc.image_directory, udata->_.d_proc.tpl_filename);
                } else {
                    snprintf(tmp, FLEN_FILENAME, "%s", udata->_.d_proc.tpl_filename);
                }
                fits_create_template(&fptr, filename, tmp, &status);
            } else {
                fits_create_file(&fptr, filename, &status);
                fits_copy_file(udata->_.d_proc.tpl_fptr, fptr, 1, 1, 1, &status);
            }
        }
    }
    /*
     * write to the file
     */
    Clock_gettime(CLOCK_REALTIME, &tp);
    tp2str(&tp, date_time, 32);
    fits_create_img(fptr, bitpix, 2, naxes, &status);
    fits_write_img(fptr, datatype, 1, width * height, (void *) data, &status);
    fits_update_key_longstr(fptr, "DATE-OBS", date_time, NULL, &status);
    
    /*
     * close file properly.
     */
    if (flag) {
        if (udata->_.d_exp.notify_each_frame_done) {
            fits_close_file(fptr, &status);
            udata->_.d_proc.img_fptr = NULL;
        } else {
            udata->_.d_proc.img_fptr = fptr;
            if (udata->_.d_exp.count == udata->_.d_exp.request_frames) {
                fits_close_file(fptr, &status);
                udata->_.d_proc.img_fptr = NULL;
            }
        }
    } else {
        if (!udata->_.d_exp.notify_each_frame_done && udata->_.d_exp.count == udata->_.d_exp.request_frames) {
                fits_close_file(fptr, &status);
                udata->_.d_proc.img_fptr = NULL;
        }
    }
}

static void
GenICam_stream_callback(void *user_data, ArvStreamCallbackType type, ArvBuffer *buffer)
{
    struct GenICam *udata = (struct GenICam *) user_data;

    switch (type) {
        case ARV_STREAM_CALLBACK_TYPE_INIT:
            udata->_.d_exp.count = 0;
            break;
        case ARV_STREAM_CALLBACK_TYPE_START_BUFFER:
            udata->_.d_exp.count++;
            /*
             * Make exposure function return when last frame filling start.
             */
        
            Pthread_mutex_lock(&udata->_.d_state.mtx);
            udata->_.d_state.state = DETECTOR_STATE_READING;
            Pthread_mutex_unlock(&udata->_.d_state.mtx);
            if (udata->_.d_exp.notify_last_frame_filling) {
                Pthread_mutex_lock(&udata->_.d_exp.mtx);
                if (udata->_.d_exp.count == udata->_.d_exp.request_frames && udata->_.d_exp.notify_last_frame_filling) {
                    udata->_.d_exp.last_frame_filling_flag = 1;
                    Pthread_cond_signal(&udata->_.d_exp.cond);
                }
                Pthread_mutex_unlock(&udata->_.d_exp.mtx);
            }
            break;
        case ARV_STREAM_CALLBACK_TYPE_BUFFER_DONE:
            Pthread_mutex_lock(&udata->_.d_state.mtx);
            udata->_.d_state.state = DETECTOR_STATE_EXPOSING;
            Pthread_mutex_unlock(&udata->_.d_state.mtx);
            if (udata->_.d_exp.notify_each_frame_done) {
                if (buffer != NULL) {
                    char *filename = (char *) Malloc(FLEN_FILENAME);
                    GenICam_process_image(user_data, buffer, filename, FLEN_FILENAME);
                    threadsafe_queue_push(udata->_.d_proc.queue, filename);
                }
            } else {
                if (buffer != NULL) {
                    if (udata->_.d_exp.count != 1) {
                        GenICam_process_image(user_data, buffer, NULL, 0);
                        if (udata->_.d_exp.count == udata->_.d_exp.request_frames) {
                            threadsafe_queue_push(udata->_.d_proc.queue, udata->_.d_proc.img_filename);
                            udata->_.d_proc.img_filename = NULL;
                        }
                    } else {
                        udata->_.d_proc.img_filename = (char *) Malloc(FLEN_FILENAME);
                        GenICam_process_image(user_data, buffer, udata->_.d_proc.img_filename, FLEN_FILENAME);
                    }
                }
            }
            break;
        case ARV_STREAM_CALLBACK_TYPE_EXIT:
            break;
    }
}

static int
GenICam_expose(void *_self, double exposure_time, uint32_t n_frames, va_list *app)
{
    struct GenICam *self = cast(GenICam(), _self);
    
    ArvCamera *camera = (ArvCamera *) self->camera;
    GError *error = NULL;
    ArvStream *stream = NULL;
    gint payload;
    int old_state, ret;
    char *filename;
    
    self->_.d_exp.count = 0;
    self->_.d_exp.request_frames = n_frames;
    
    /*
     * Do not allow cancel while set exposure time, since it is a fast
     * instruction.
     */
    Pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &old_state);

    g_return_val_if_fail (ARV_IS_CAMERA (camera), EXIT_FAILURE);
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    while (self->_.d_state.state == DETECTOR_STATE_EXPOSING || self->_.d_state.state == DETECTOR_STATE_READING) {
        Pthread_cond_wait(&self->_.d_state.cond, &self->_.d_state.mtx);
    }
    self->_.d_state.state = DETECTOR_STATE_EXPOSING;
    self->_.d_exp.request_frames =  n_frames;
    self->n_buffers = n_frames;
    self->callback_param = (void *) app;
    Pthread_mutex_unlock(&self->_.d_state.mtx);

    if ((ret = GenICam_set_exposure_time_nl(self, exposure_time)) != AAOS_OK) {
        Pthread_cond_broadcast(&self->_.d_state.cond);
        goto error;
    }

    stream = arv_camera_create_stream (camera, GenICam_stream_callback, self, &error);
    if (error != NULL) {
        g_clear_error(&error);
        Pthread_mutex_lock(&self->_.d_state.mtx);
        self->_.d_state.state = DETECTOR_STATE_IDLE;
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        Pthread_cond_broadcast(&self->_.d_state.cond);
        ret = AAOS_ERROR;
        goto error;
    }
    self->stream = (void *) stream;
    
    payload = arv_camera_get_payload (camera, &error);
    if (error != NULL) {
        g_clear_error(&error);
        Pthread_mutex_lock(&self->_.d_state.mtx);
        self->_.d_state.state = DETECTOR_STATE_IDLE;
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        Pthread_cond_broadcast(&self->_.d_state.cond);
        ret = AAOS_ERROR;
        goto error;
    }
    if (n_frames == 1) {
        arv_stream_push_buffer(stream, arv_buffer_new(payload, NULL));
        arv_camera_set_acquisition_mode(camera, ARV_ACQUISITION_MODE_SINGLE_FRAME, &error);
        if (error != NULL) {
            if (error->code == ARV_GC_ERROR_ENUM_ENTRY_NOT_FOUND) {
                arv_camera_set_acquisition_mode(camera, ARV_ACQUISITION_MODE_CONTINUOUS, &error);
                if (error != NULL) {
                    g_clear_error(&error);
                    Pthread_mutex_lock(&self->_.d_state.mtx);
                    self->_.d_state.state = DETECTOR_STATE_IDLE;
                    Pthread_mutex_unlock(&self->_.d_state.mtx);
                    Pthread_cond_broadcast(&self->_.d_state.cond);
                    ret = AAOS_ERROR;
                    goto error;
                }
            } else {
                g_clear_error(&error);
                Pthread_mutex_lock(&self->_.d_state.mtx);
                self->_.d_state.state = DETECTOR_STATE_IDLE;
                Pthread_mutex_unlock(&self->_.d_state.mtx);
                Pthread_cond_broadcast(&self->_.d_state.cond);
                ret = AAOS_ERROR;
                goto error;
            }
        }
    } else {
        int i;
        for (i = 0; i < self->n_buffers; i++) {
                arv_stream_push_buffer(stream, arv_buffer_new(payload, NULL));
        }
        arv_camera_set_acquisition_mode(camera, ARV_ACQUISITION_MODE_CONTINUOUS, &error);
        if (error != NULL) {
            g_clear_error(&error);
            Pthread_mutex_lock(&self->_.d_state.mtx);
            self->_.d_state.state = DETECTOR_STATE_IDLE;
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            Pthread_cond_broadcast(&self->_.d_state.cond);
            ret = AAOS_ERROR;
            goto error;
        }
    }
    
    Pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    arv_camera_start_acquisition(camera, &error);
    if (error != NULL) {
        g_clear_error(&error);
        Pthread_mutex_lock(&self->_.d_state.mtx);
        self->_.d_state.state = DETECTOR_STATE_IDLE;
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        Pthread_cond_broadcast(&self->_.d_state.cond);
        ret = AAOS_ERROR;
        goto error;
    }
    Pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
    
    if (self->_.d_exp.notify_each_frame_done) {
        size_t i, n = n_frames - 1;
        for (i = 0; i < n; i++) {
            filename = threadsafe_queue_wait_and_pop(self->_.d_proc.queue);
            if (self->_.d_proc.post_acquisition) {
                self->_.d_proc.post_acquisition(self, filename, self->callback_param);
            } else {
                printf("%s\n", filename);
            }
            free(filename);
        }
    }
    
    /*
     * return just after last frame's exposure completes.
     */
    if (self->_.d_exp.notify_last_frame_filling) {
        Pthread_mutex_lock(&self->_.d_exp.mtx);
        while (self->_.d_exp.last_frame_filling_flag == 0) {
            Pthread_cond_wait(&self->_.d_exp.cond, &self->_.d_exp.mtx);
        }
        self->_.d_exp.last_frame_filling_flag = 0;
        Pthread_mutex_unlock(&self->_.d_exp.mtx);
        return ret;
    }
    
    filename = threadsafe_queue_wait_and_pop(self->_.d_proc.queue);
    if (self->_.d_proc.post_acquisition) {
        self->_.d_proc.post_acquisition(self, filename, self->callback_param);
    } else {
        printf("%s\n", filename);
    }
    
    free(filename);
    
    arv_camera_stop_acquisition(camera, &error);
    while ((filename = threadsafe_queue_try_pop(self->_.d_proc.queue)) != NULL) {
        Unlink(filename);
        free(filename);
    }
    
error:
    Pthread_mutex_lock(&self->_.d_state.mtx);
    self->_.d_state.state = DETECTOR_STATE_IDLE;
    if (error != NULL) {
        g_clear_error(&error);
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        ret = AAOS_ERROR;
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    Pthread_cond_broadcast(&self->_.d_state.cond);

    if (stream != NULL && ARV_IS_STREAM(stream)) {
        g_object_unref(stream);
    }
    self->stream = NULL;
    Pthread_setcancelstate(old_state, NULL);
    return ret;
}

static int
GenICam_wait_for_last_completion(void *_self)
{
    struct GenICam *self = cast(GenICam(), _self);
    
    ArvCamera *camera = (ArvCamera *) self->camera;
    ArvStream *stream = (ArvStream *) self->stream;
    int ret = AAOS_OK;
    GError *error = NULL;
    char *filename;
    
    if (self->_.d_exp.notify_last_frame_filling) {
        filename = threadsafe_queue_wait_and_pop(self->_.d_proc.queue);
        if (self->_.d_proc.post_acquisition) {
            self->_.d_proc.post_acquisition(self, filename, self->callback_param);
        } else {
            printf("%s\n", filename);
        }
        free(filename);
    }

    arv_camera_stop_acquisition(camera, &error);
    while ((filename = threadsafe_queue_try_pop(self->_.d_proc.queue)) != NULL) {
        Unlink(filename);
        free(filename);
    }
    Pthread_mutex_lock(&self->_.d_state.mtx);
    self->_.d_state.state = DETECTOR_STATE_IDLE;
    if (error != NULL) {
        g_clear_error(&error);
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        ret = AAOS_ERROR;
        goto error;
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    Pthread_cond_broadcast(&self->_.d_state.cond);
    
error:
    if (stream != NULL && ARV_IS_STREAM(stream)) {
        g_object_unref(stream);
    }
    
    return ret;
}

static int
GenICam_set_region(void *_self, uint32_t x_offset, uint32_t y_offset, uint32_t width, uint32_t height)
{
    struct GenICam *self = cast(GenICam(), _self);
    
    ArvCamera *camera = (ArvCamera *) self->camera;
    GError *error = NULL;
    int ret = AAOS_OK;
    
    if (x_offset > self->_.d_cap.x_offset_max || x_offset < self->_.d_cap.x_offset_min || y_offset > self->_.d_cap.y_offset_max || y_offset < self->_.d_cap.y_offset_min || width > self->_.d_cap.image_width_max || width < self->_.d_cap.image_width_min || height > self->_.d_cap.image_height_max || height < self->_.d_cap.image_height_min) {
        return AAOS_EINVAL;
    }
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    switch (self->_.d_state.state) {
        case DETECTOR_STATE_IDLE:
            arv_camera_set_region(camera, x_offset, y_offset, width, height, &error);
            if (error != NULL) {
                Pthread_mutex_unlock(&self->_.d_state.mtx);
                ret = AAOS_ERROR;
                goto error;
            }
            self->_.d_param.x_offset = x_offset;
            self->_.d_param.y_offset = y_offset;
            self->_.d_param.image_width = width;
            self->_.d_param.image_height = height;
            break;
        case DETECTOR_STATE_EXPOSING:
        case DETECTOR_STATE_READING:
            while (self->_.d_state.state == DETECTOR_STATE_EXPOSING || self->_.d_state.state == DETECTOR_STATE_READING) {
                Pthread_cond_wait(&self->_.d_state.cond, &self->_.d_state.mtx);
            }
            switch (self->_.d_state.state) {
                case DETECTOR_STATE_MALFUNCTION:
                    ret = AAOS_EDEVMAL;
                    break;
                case DETECTOR_STATE_OFFLINE:
                case DETECTOR_STATE_UNINITIALIZED:
                    ret = AAOS_EUNINIT;
                    break;
                default:
                    arv_camera_set_region(camera, x_offset, y_offset, width, height, &error);
                    if (error != NULL) {
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        ret = AAOS_ERROR;
                        goto error;
                    }
                    self->_.d_param.x_offset = x_offset;
                    self->_.d_param.y_offset = y_offset;
                    self->_.d_param.image_width = width;
                    self->_.d_param.image_height = height;
                    break;
            }
            break;
        case DETECTOR_STATE_MALFUNCTION:
            ret = AAOS_EDEVMAL;
            break;
        default:
            break;
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
error:
    if (error != NULL) {
        g_clear_error(&error);
    }
    return ret;
}

static int
GenICam_get_region(void *_self, uint32_t *x_offset, uint32_t *y_offset, uint32_t *width, uint32_t *height)
{
    struct GenICam *self = cast(GenICam(), _self);
    
    ArvCamera *camera = (ArvCamera *) self->camera;
    GError *error = NULL;
    int ret = AAOS_OK;
    
    gint x, y, w, h;
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    switch (self->_.d_state.state) {
        case DETECTOR_STATE_IDLE:
            arv_camera_get_region(camera, &x, &y, &w, &h, &error);
            if (error != NULL) {
                Pthread_mutex_unlock(&self->_.d_state.mtx);
                ret = AAOS_ERROR;
                goto error;
            }
            *x_offset = (uint32_t) x;
            *y_offset = (uint32_t) y;
            *width = (uint32_t) w;
            *height = (uint32_t) h;
            self->_.d_param.x_offset = *x_offset;
            self->_.d_param.y_offset = *y_offset;
            self->_.d_param.image_width = *width;
            self->_.d_param.image_height = *height;
            break;
        case DETECTOR_STATE_EXPOSING:
        case DETECTOR_STATE_READING:
            while (self->_.d_state.state == DETECTOR_STATE_EXPOSING || self->_.d_state.state == DETECTOR_STATE_READING) {
                Pthread_cond_wait(&self->_.d_state.cond, &self->_.d_state.mtx);
            }
            switch (self->_.d_state.state) {
                case DETECTOR_STATE_MALFUNCTION:
                    ret = AAOS_EDEVMAL;
                    break;
                case DETECTOR_STATE_OFFLINE:
                case DETECTOR_STATE_UNINITIALIZED:
                    ret = AAOS_EUNINIT;
                    break;
                default:
                    arv_camera_get_region(camera, &x, &y, &w, &h, &error);
                    if (error != NULL) {
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        ret = AAOS_ERROR;
                        goto error;
                    }
                    *x_offset = (uint32_t) x;
                    *y_offset = (uint32_t) y;
                    *width = (uint32_t) w;
                    *height = (uint32_t) h;
                    self->_.d_param.x_offset = *x_offset;
                    self->_.d_param.y_offset = *y_offset;
                    self->_.d_param.image_width = *width;
                    self->_.d_param.image_height = *height;
                    break;
            }
            break;
        case DETECTOR_STATE_MALFUNCTION:
            ret = AAOS_EDEVMAL;
            break;
        default:
            break;
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
error:
    if (error != NULL) {
        g_clear_error(&error);
    }
    return ret;
}

static int
GenICam_set_binning(void *_self, uint32_t x_binning, uint32_t y_binning)
{
    struct GenICam *self = cast(GenICam(), _self);
    
    ArvCamera *camera = (ArvCamera *) self->camera;
    GError *error = NULL;
    int ret = AAOS_OK;
    
    if (x_binning > self->_.d_cap.x_binning_max || x_binning < self->_.d_cap.x_binning_min || y_binning > self->_.d_cap.y_binning_max || y_binning < self->_.d_cap.y_binning_min) {
        return AAOS_EINVAL;
    }
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    switch (self->_.d_state.state) {
        case DETECTOR_STATE_IDLE:
            arv_camera_set_binning(camera, x_binning, y_binning, &error);
            if (error != NULL) {
                Pthread_mutex_unlock(&self->_.d_state.mtx);
                ret = AAOS_ERROR;
                goto error;
            }
            self->_.d_param.x_binning = x_binning;
            self->_.d_param.y_binning = y_binning;
            break;
        case DETECTOR_STATE_EXPOSING:
        case DETECTOR_STATE_READING:
            while (self->_.d_state.state == DETECTOR_STATE_EXPOSING || self->_.d_state.state == DETECTOR_STATE_READING) {
                Pthread_cond_wait(&self->_.d_state.cond, &self->_.d_state.mtx);
            }
            switch (self->_.d_state.state) {
                case DETECTOR_STATE_MALFUNCTION:
                    ret = AAOS_EDEVMAL;
                    break;
                case DETECTOR_STATE_OFFLINE:
                case DETECTOR_STATE_UNINITIALIZED:
                    ret = AAOS_EUNINIT;
                    break;
                default:
                    arv_camera_set_binning(camera, x_binning, y_binning, &error);
                    if (error != NULL) {
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        ret = AAOS_ERROR;
                        goto error;
                    }
                    self->_.d_param.x_binning = x_binning;
                    self->_.d_param.y_binning = y_binning;
                    break;
            }
            break;
        case DETECTOR_STATE_MALFUNCTION:
            ret = AAOS_EDEVMAL;
            break;
        default:
            break;
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
error:
    if (error != NULL) {
        g_clear_error(&error);
    }
    return ret;
}

static int
GenICam_get_binning(void *_self, uint32_t *x_binning, uint32_t *y_binning)
{
    struct GenICam *self = cast(GenICam(), _self);
    
    ArvCamera *camera = (ArvCamera *) self->camera;
    GError *error = NULL;
    int ret = AAOS_OK;
    
    gint x, y;
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    switch (self->_.d_state.state) {
        case DETECTOR_STATE_IDLE:
            arv_camera_get_binning(camera, &x, &y, &error);
            if (error != NULL) {
                Pthread_mutex_unlock(&self->_.d_state.mtx);
                ret = AAOS_ERROR;
                goto error;
            }
            *x_binning = (uint32_t) x;
            *y_binning = (uint32_t) y;
            self->_.d_param.x_binning = *x_binning;
            self->_.d_param.y_binning = *y_binning;
            break;
        case DETECTOR_STATE_EXPOSING:
        case DETECTOR_STATE_READING:
            while (self->_.d_state.state == DETECTOR_STATE_EXPOSING || self->_.d_state.state == DETECTOR_STATE_READING) {
                Pthread_cond_wait(&self->_.d_state.cond, &self->_.d_state.mtx);
            }
            switch (self->_.d_state.state) {
                case DETECTOR_STATE_MALFUNCTION:
                    ret = AAOS_EDEVMAL;
                    break;
                case DETECTOR_STATE_OFFLINE:
                case DETECTOR_STATE_UNINITIALIZED:
                    ret = AAOS_EUNINIT;
                    break;
                default:
                    arv_camera_get_binning(camera, &x, &y, &error);
                    if (error != NULL) {
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        ret = AAOS_ERROR;
                        goto error;
                    }
                    *x_binning = (uint32_t) x;
                    *y_binning = (uint32_t) y;
                    self->_.d_param.x_binning = *x_binning;
                    self->_.d_param.y_binning = *y_binning;
                    break;
            }
            break;
        case DETECTOR_STATE_MALFUNCTION:
            ret = AAOS_EDEVMAL;
            break;
        default:
            break;
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
error:
    if (error != NULL) {
        g_clear_error(&error);
    }
    return ret;
}

static int
GenICam_set_frame_rate(void *_self, double frame_rate)
{
    struct GenICam *self = cast(GenICam(), _self);
    
    ArvCamera *camera = (ArvCamera *) self->camera;
    GError *error = NULL;
    int ret = AAOS_OK;
    
    if (frame_rate > self->_.d_cap.frame_rate_max || frame_rate < self->_.d_cap.frame_rate_min) {
        return AAOS_EINVAL;
    }
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    switch (self->_.d_state.state) {
        case DETECTOR_STATE_IDLE:
            arv_camera_set_frame_rate(camera, frame_rate, &error);
            if (error != NULL) {
                Pthread_mutex_unlock(&self->_.d_state.mtx);
                ret = AAOS_ERROR;
                goto error;
            }
            self->_.d_param.frame_rate = frame_rate;
            break;
        case DETECTOR_STATE_EXPOSING:
        case DETECTOR_STATE_READING:
            while (self->_.d_state.state == DETECTOR_STATE_EXPOSING || self->_.d_state.state == DETECTOR_STATE_READING) {
                Pthread_cond_wait(&self->_.d_state.cond, &self->_.d_state.mtx);
            }
            switch (self->_.d_state.state) {
                case DETECTOR_STATE_MALFUNCTION:
                    ret = AAOS_EDEVMAL;
                    break;
                case DETECTOR_STATE_OFFLINE:
                case DETECTOR_STATE_UNINITIALIZED:
                    ret = AAOS_EUNINIT;
                    break;
                default:
                    arv_camera_set_frame_rate(camera, frame_rate, &error);
                    if (error != NULL) {
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        ret = AAOS_ERROR;
                        goto error;
                    }
                    self->_.d_param.frame_rate = frame_rate;
                    break;
            }
            break;
        case DETECTOR_STATE_MALFUNCTION:
            ret = AAOS_EDEVMAL;
            break;
        default:
            break;
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
error:
    if (error != NULL) {
        g_clear_error(&error);
    }
    return ret;
}

static int
GenICam_get_frame_rate(void *_self, double *frame_rate)
{
    struct GenICam *self = cast(GenICam(), _self);
    
    ArvCamera *camera = (ArvCamera *) self->camera;
    GError *error = NULL;
    int ret = AAOS_OK;
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    switch (self->_.d_state.state) {
        case DETECTOR_STATE_IDLE:
            *frame_rate = arv_camera_get_frame_rate(camera, &error);
            if (error != NULL) {
                Pthread_mutex_unlock(&self->_.d_state.mtx);
                ret = AAOS_ERROR;
                goto error;
            }
            self->_.d_param.frame_rate = *frame_rate;
            break;
        case DETECTOR_STATE_EXPOSING:
        case DETECTOR_STATE_READING:
            while (self->_.d_state.state == DETECTOR_STATE_EXPOSING || self->_.d_state.state == DETECTOR_STATE_READING) {
                Pthread_cond_wait(&self->_.d_state.cond, &self->_.d_state.mtx);
            }
            switch (self->_.d_state.state) {
                case DETECTOR_STATE_MALFUNCTION:
                    ret = AAOS_EDEVMAL;
                    break;
                case DETECTOR_STATE_OFFLINE:
                case DETECTOR_STATE_UNINITIALIZED:
                    ret = AAOS_EUNINIT;
                    break;
                default:
                    *frame_rate = arv_camera_get_frame_rate(camera, &error);
                    if (error != NULL) {
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        ret = AAOS_ERROR;
                        goto error;
                    }
                    self->_.d_param.frame_rate = *frame_rate;
                    break;
            }
            break;
        case DETECTOR_STATE_MALFUNCTION:
            ret = AAOS_EDEVMAL;
            break;
        default:
            break;
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
error:
    if (error != NULL) {
        g_clear_error(&error);
    }
    return ret;
}

static int
GenICam_set_gain(void *_self, double gain)
{
    struct GenICam *self = cast(GenICam(), _self);
    
    ArvCamera *camera = (ArvCamera *) self->camera;
    GError *error = NULL;
    int ret = AAOS_OK;
    
    if (gain > self->_.d_cap.gain_max || gain < self->_.d_cap.gain_min) {
        return AAOS_EINVAL;
    }
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    switch (self->_.d_state.state) {
        case DETECTOR_STATE_IDLE:
            arv_camera_set_gain(camera, gain, &error);
            if (error != NULL) {
                Pthread_mutex_unlock(&self->_.d_state.mtx);
                ret = AAOS_ERROR;
                goto error;
            }
            self->_.d_param.gain = gain;
            break;
        case DETECTOR_STATE_EXPOSING:
        case DETECTOR_STATE_READING:
            while (self->_.d_state.state == DETECTOR_STATE_EXPOSING || self->_.d_state.state == DETECTOR_STATE_READING) {
                Pthread_cond_wait(&self->_.d_state.cond, &self->_.d_state.mtx);
            }
            switch (self->_.d_state.state) {
                case DETECTOR_STATE_MALFUNCTION:
                    ret = AAOS_EDEVMAL;
                    break;
                case DETECTOR_STATE_OFFLINE:
                case DETECTOR_STATE_UNINITIALIZED:
                    ret = AAOS_EUNINIT;
                    break;
                default:
                    arv_camera_set_gain(camera, gain, &error);
                    if (error != NULL) {
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        ret = AAOS_ERROR;
                        goto error;
                    }
                    self->_.d_param.gain = gain;
                    break;
            }
            break;
        case DETECTOR_STATE_MALFUNCTION:
            ret = AAOS_EDEVMAL;
            break;
        default:
            break;
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
error:
    if (error != NULL) {
        g_clear_error(&error);
    }
    return ret;
}

static int
GenICam_get_gain(void *_self, double *gain)
{
    struct GenICam *self = cast(GenICam(), _self);
    
    ArvCamera *camera = (ArvCamera *) self->camera;
    GError *error = NULL;
    int ret = AAOS_OK;
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    switch (self->_.d_state.state) {
        case DETECTOR_STATE_IDLE:
            *gain = arv_camera_get_gain(camera, &error);
            if (error != NULL) {
                Pthread_mutex_unlock(&self->_.d_state.mtx);
                ret = AAOS_ERROR;
                goto error;
            }
            self->_.d_param.gain = *gain;
            break;
        case DETECTOR_STATE_EXPOSING:
        case DETECTOR_STATE_READING:
            while (self->_.d_state.state == DETECTOR_STATE_EXPOSING || self->_.d_state.state == DETECTOR_STATE_READING) {
                Pthread_cond_wait(&self->_.d_state.cond, &self->_.d_state.mtx);
            }
            switch (self->_.d_state.state) {
                case DETECTOR_STATE_MALFUNCTION:
                    ret = AAOS_EDEVMAL;
                    break;
                case DETECTOR_STATE_OFFLINE:
                case DETECTOR_STATE_UNINITIALIZED:
                    ret = AAOS_EUNINIT;
                    break;
                default:
                    *gain = arv_camera_get_gain(camera, &error);
                    if (error != NULL) {
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        ret = AAOS_ERROR;
                        goto error;
                    }
                    self->_.d_param.gain = *gain;
                    break;
            }
            break;
        case DETECTOR_STATE_MALFUNCTION:
            ret = AAOS_EDEVMAL;
            break;
        default:
            break;
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
error:
    if (error != NULL) {
        g_clear_error(&error);
    }
    return ret;
}

static int
GenICam_get_exposure_time(void *_self, double *exposure_time)
{
    struct GenICam *self = cast(GenICam(), _self);
    
    ArvCamera *camera = (ArvCamera *) self->camera;
    GError *error = NULL;
    int ret = AAOS_OK;
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    switch (self->_.d_state.state) {
        case DETECTOR_STATE_IDLE:
            *exposure_time = arv_camera_get_exposure_time(camera, &error);
            if (error != NULL) {
                Pthread_mutex_unlock(&self->_.d_state.mtx);
                ret = AAOS_ERROR;
                goto error;
            }
            self->_.d_param.exposure_time = *exposure_time;
            break;
        case DETECTOR_STATE_EXPOSING:
        case DETECTOR_STATE_READING:
            while (self->_.d_state.state == DETECTOR_STATE_EXPOSING || self->_.d_state.state == DETECTOR_STATE_READING) {
                Pthread_cond_wait(&self->_.d_state.cond, &self->_.d_state.mtx);
            }
            switch (self->_.d_state.state) {
                case DETECTOR_STATE_MALFUNCTION:
                    ret = AAOS_EDEVMAL;
                    break;
                case DETECTOR_STATE_OFFLINE:
                case DETECTOR_STATE_UNINITIALIZED:
                    ret = AAOS_EUNINIT;
                    break;
                default:
                    *exposure_time = arv_camera_get_exposure_time(camera, &error);
                    if (error != NULL) {
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        ret = AAOS_ERROR;
                        goto error;
                    }
                    self->_.d_param.exposure_time = *exposure_time;
                    break;
            }
            break;
        case DETECTOR_STATE_MALFUNCTION:
            ret = AAOS_EDEVMAL;
            break;
        default:
            break;
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
error:
    if (error != NULL) {
        g_clear_error(&error);
    }
    return ret;
}

static int
GenICam_inspect(void *_self)
{
    struct GenICam *self = cast(GenICam(), _self);
    
    ArvCamera *camera = (ArvCamera *) self->camera;
    GError *error = NULL;
    int ret = AAOS_OK;
    
    arv_camera_get_string(camera, "Vendor", &error);
    Pthread_mutex_lock(&self->_.d_state.mtx);
    if (error != NULL) {
        self->_.d_state.state = DETECTOR_STATE_MALFUNCTION;
        ret = AAOS_EDEVMAL;
        g_clear_error(&error);
    } else {
        self->_.d_state.state = DETECTOR_STATE_IDLE;
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    Pthread_cond_broadcast(&self->_.d_state.cond);
    
    return ret;
}

static int
GenICam_info(void *_self, void *res, size_t res_size, size_t *res_len)
{
    struct GenICam *self = cast(GenICam(), _self);
    
    ArvCamera *camera = (ArvCamera *) self->camera;
    GError *error = NULL;
    FILE *fp;
    int ret = AAOS_OK;
    
    fp = fmemopen(res, res_size, "w+");
    fprintf(fp, "Vendor:\t\t%s\n", arv_camera_get_vendor_name(camera, NULL));
    fprintf(fp, "Model:\t\t%s\n", arv_camera_get_model_name(camera, NULL));
    fprintf(fp, "Serial:\t\t%s\n", arv_camera_get_device_serial_number(camera, NULL));
    fprintf(fp, "DeviceID:\t%s\n", arv_camera_get_device_id(camera, NULL));
    fprintf(fp, "DeviceUserID:\t%s\n", arv_camera_get_device_id(camera, NULL));
    
    gint width, height;
    arv_camera_get_sensor_size(camera, &width, &height, NULL);
    fprintf(fp, "\nSensor format:\t%dx%d\n", width, height);

    gint x, y;
    arv_camera_get_region(camera, &x, &y, &width, &height, NULL);
    fprintf(fp, "Current region:\t[%d, %d, %d, %d]\n", x, y, width, height);
    gint min, max, increment;
    arv_camera_get_x_offset_bounds(camera, &min, &max, NULL);
    increment = arv_camera_get_x_offset_increment(camera, NULL);
    fprintf(fp, "X-Offset range:\t[%d, %d, %d]\n", min, max, increment);
    arv_camera_get_y_offset_bounds(camera, &min, &max, NULL);
    increment = arv_camera_get_y_offset_increment(camera, NULL);
    fprintf(fp, "Y-Offset range:\t[%d, %d, %d]\n", min, max, increment);
    arv_camera_get_width_bounds(camera, &min, &max, NULL);
    increment = arv_camera_get_width_increment(camera, NULL);
    fprintf(fp, "Width range:\t[%d, %d, %d]\n", min, max, increment);
    arv_camera_get_height_bounds(camera, &min, &max, NULL);
    increment = arv_camera_get_height_increment(camera, NULL);
    fprintf(fp, "Height range:\t[%d, %d, %d]\n", min, max, increment);
    
    guint n;
    const char **available_pixel_formats = arv_camera_dup_available_pixel_formats_as_display_names(camera, &n, NULL);
    if (available_pixel_formats != NULL) {
        guint i;
        fprintf(fp, "\nPixel format:\t%s\n", available_pixel_formats[0]);
        for (i = 1; i < n; i++) {
            fprintf(fp, "\t\t%s\n", available_pixel_formats[i]);
        }
        g_clear_pointer (&available_pixel_formats, g_free);
    } else {
        fprintf(fp, "\nPixel format:\tnot available\n");
    }
    fprintf(fp, "Current:\t%s\n", arv_camera_get_pixel_format_as_string(camera, NULL));

    if (arv_camera_is_frame_rate_available(camera, NULL)) {
        fprintf(fp, "\nFramerate:\tavailable\n");
        double min = 0., max = 0., frame_rate = 0.;
        arv_camera_get_frame_rate_bounds(camera, &min, &max, NULL);
        fprintf(fp, "Range:\t\t[%.3f, %.3f] Hz\n", min, max);
        frame_rate = arv_camera_get_frame_rate(camera, NULL);
        fprintf(fp, "Current:\t%.6f Hz\n", frame_rate);
    } else {
        fprintf(fp, "\nFramerate:\tnot available\n");
    }
                
    if (arv_camera_is_exposure_time_available(camera, NULL)) {
        fprintf(fp, "\nExposureTime:\tavailable\n");
        double min = 0., max = 0., exposure_time = 0.;
        arv_camera_get_exposure_time_bounds(camera, &min, &max, NULL);
        fprintf(fp, "Range:\t\t[%.0f, %.0f] us\n", min, max);
        exposure_time = arv_camera_get_exposure_time(camera, NULL);
        fprintf(fp, "Current:\t%.0f us\n", exposure_time);
    } else {
        fprintf(fp, "\nExposureTime:\tnot available\n");
    }

    if (arv_camera_is_gain_available(camera, NULL)) {
        fprintf(fp, "\nGain:\t\tavailable\n");
        double min = 0., max = 0., gain = 0.;
        arv_camera_get_gain_bounds(camera, &min, &max, NULL);
        fprintf(fp, "Range:\t\t[%.2f, %.2f] e-/ADU\n", min, max);
        gain = arv_camera_get_gain(camera, NULL);
        fprintf(fp, "Current:\t%.2f e-/ADU\n", gain);
    } else {
        fprintf(fp, "\nGain\t:\tnot available\n");
    }
    
    if (arv_camera_is_binning_available(camera, NULL)) {
        fprintf(fp, "\nBinning:\tavailable\n");
        gint min = 0, max = 0, increment = 0, x_binning = 0, y_binning  = 0;
        arv_camera_get_x_binning_bounds(camera, &min, &max, NULL);
        increment = arv_camera_get_x_binning_increment(camera, NULL);
        fprintf(fp, "Range:\t\thorizontal %d, %d, %d\n", min, max, increment);
        arv_camera_get_y_binning_bounds(camera, &min, &max, NULL);
        increment = arv_camera_get_y_binning_increment(camera, NULL);
        fprintf(fp, "Range:\t\tvertical   %d, %d, %d\n", min, max, increment);
        arv_camera_get_binning(camera, &x_binning, &y_binning, NULL);
        fprintf(fp, "Current:\t%dx%d\n", x_binning, y_binning);
    } else {
        fprintf(fp, "\nExposureTime:\tnot available\n");
    }

    fclose(fp);
    if (res_len != NULL) {
        *res_len = strlen(res) + 1;
    }
    return ret;
}

static const void *_genicam_virtual_table;

static void
genicam_virtual_table_destroy(void)
{
    delete((void *) _genicam_virtual_table);
}

static void
genicam_virtual_table_initialize(void)
{
    _genicam_virtual_table = new(__DetectorVirtualTable(),
                                 __detector_init, "init", GenICam_init,
                                 __detector_info, "info", GenICam_info,
                                 __detector_expose, "expose", GenICam_expose,
                                 __detector_set_binning, "set_binning", GenICam_set_binning,
                                 __detector_get_binning, "get_binning", GenICam_get_binning,
                                 __detector_set_exposure_time, "set_exposure_time", GenICam_set_exposure_time,
                                 __detector_get_exposure_time, "get_exposure_time", GenICam_get_exposure_time,
                                 __detector_set_frame_rate, "set_frame_rate", GenICam_set_frame_rate,
                                 __detector_get_frame_rate, "get_frame_rate", GenICam_get_frame_rate,
                                 __detector_set_gain, "set_gain", GenICam_set_gain,
                                 __detector_get_gain, "get_gain", GenICam_get_gain,
                                 __detector_set_region, "set_region", GenICam_set_region,
                                 __detector_get_region, "get_region", GenICam_get_region,
                                 __detector_raw, "raw", GenICam_raw,
                                 __detector_inspect, "inspect", GenICam_inspect,
                                 __detector_wait_for_completion, "wait_for_completion", GenICam_wait_for_last_completion,
                                 (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(genicam_virtual_table_destroy);
#endif
}

static const void *
genicam_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, genicam_virtual_table_initialize);
#endif
    
    return _genicam_virtual_table;
}
#endif

/*
 * USTC cameras.
 */

#define USTC_CCD_SUCCESS                    20001
#define USTC_CCD_SCAN_ERROR                 20002
#define USTC_CCD_DEVICE_NOT_FOUND           20003
#define USTC_CCD_FORMAT_ERROR               20004
#define USTC_CCD_DEVICE_BUSY                20005
#define USTC_CCD_ARG_ERROR                  20006
#define USTC_CCD_ACCESS_ERROR               20007
#define USTC_CCD_NOT_INIT                   20008
#define USTC_CCD_SOFTWARE_BUG               20009
#define USTC_CCD_OUTOFRANGE                 20010
#define USTC_CCD_TEMPERATURE_ERROR          20011
#define USTC_CCD_TEMPERATURE_STABILIZED     20012
#define USTC_CCD_TEMPERATURE_NOT_REACHED    20013
#define USTC_CCD_TEMPERATURE_OFF            20014
#define USTC_CCD_NO_IMAGE                   20015
#define USTC_CCD_TIMEOUT                    20016
#define USTC_CCD_ACQURING                   20017
#define USTC_CCD_IMAGE_ALREADY_TRANSFERED   20018
#define USTC_CCD_CONFIG_NOT_FOUND           20019
#define USTC_CCD_NOT_IMPLEMENTED            20020
#define USTC_CCD_IO_ERROR                   20021
#define USTC_CCD_ACCESS_ABORT               20022

typedef struct {
    uint8_t Model_Enum;
    uint8_t MB_ID;
    uint8_t Year;
    uint8_t Month;
    uint8_t Day;
    uint8_t Hour;
    uint8_t version;
    uint8_t version_aa;
    uint8_t id;
    uint8_t checksum;
} PIXELX_SERIAL;

static const void *ustc_camera_virtual_table(void);

static void *
USTCCamera_ctor(void *_self, va_list *app)
{
    struct USTCCamera *self = super_ctor(USTCCamera(), _self, app);
    
    const char *s;

    self->_.d_state.state = (DETECTOR_STATE_OFFLINE|DETECTOR_STATE_UNINITIALIZED);
    s = va_arg(*app, const char *);
    self->so_path = (char *) Malloc(strlen(s) + 1);
    snprintf(self->so_path, strlen(s) + 1, "%s", s);
    self->log_level = va_arg(*app, unsigned int);
    self->which = va_arg(*app, unsigned int);
    Pthread_mutex_init(&self->mtx, NULL);
    Pthread_cond_init(&self->cond, NULL);
    self->dlh = dlopen(self->so_path, RTLD_LAZY | RTLD_LOCAL);
    
    self->_._vtab= ustc_camera_virtual_table();
    
    return (void *) self;
}

static void *
USTCCamera_dtor(void *_self)
{
    struct USTCCamera *self = cast(USTCCamera(), _self);
    
    Pthread_mutex_destroy(&self->mtx);
    Pthread_cond_destroy(&self->cond);
    free(self->so_path);
    dlclose(self->dlh);

    return super_dtor(USTCCamera(), _self);
}

static void *
USTCCameraClass_ctor(void *_self, va_list *app)
{
    struct USTCCameraClass *self = super_ctor(USTCCameraClass(), _self, app);
    
    self->_.raw.method = (Method) 0;
    self->_.init.method = (Method) 0;
    
    self->_.get_binning.method = (Method) 0;
    self->_.set_binning.method = (Method) 0;
    self->_.set_exposure_time.method = (Method) 0;
    self->_.get_exposure_time.method = (Method) 0;
    self->_.set_frame_rate.method = (Method) 0;
    self->_.get_frame_rate.method = (Method) 0;
    self->_.set_gain.method = (Method) 0;
    self->_.get_gain.method = (Method) 0;
    self->_.set_region.method = (Method) 0;
    self->_.get_region.method = (Method) 0;
    self->_.inspect.method = (Method) 0;
    //self->_.wait.method = (Method) 0;
    self->_.wait_for_completion.method = (Method) 0;
    self->_.raw.method = (Method) 0;
    self->_.expose.method = (Method) 0;
    self->_.status.method = (Method) 0;
    self->_.info.method = (Method) 0;
    self->_.stop.method = (Method) 0;
    self->_.abort.method = (Method) 0;
    self->_.power_on.method = (Method) 0;
    self->_.power_off.method = (Method) 0;
    
    return self;
}

static const void *_USTCCameraClass;

static void
USTCCameraClass_destroy(void)
{
    free((void *) _USTCCameraClass);
}

static void
USTCCameraClass_initialize(void)
{
    _USTCCameraClass = new(__DetectorClass(), "USTCCameraClass", __DetectorClass(), sizeof(struct USTCCameraClass),
                           ctor, "", USTCCameraClass_ctor,
                           (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(USTCCameraClass_destroy);
#endif
}

const void *
USTCCameraClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, USTCCameraClass_initialize);
#endif
    
    return _USTCCameraClass;
}

static const void *_USTCCamera;

static void
USTCCamera_destroy(void)
{
    free((void *)_USTCCamera);
}

static void
USTCCamera_initialize(void)
{
    _USTCCamera = new(USTCCameraClass(), "USTCCamera", __Detector(), sizeof(struct USTCCamera),
                      ctor, "ctor", USTCCamera_ctor,
                      dtor, "dtor", USTCCamera_dtor,
                      (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(USTCCamera_destroy);
#endif
}

const void *
USTCCamera(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, USTCCamera_initialize);
#endif

    return _USTCCamera;
}

static int
ustc_error_mapping(int ustc_error)
{
switch (ustc_error) {
        case USTC_CCD_SUCCESS:
            return AAOS_OK;
            break;
        case USTC_CCD_ARG_ERROR:
        case USTC_CCD_OUTOFRANGE:
            return AAOS_EINVAL;
            break;
        case USTC_CCD_NOT_INIT:
            return AAOS_EUNINIT;
            break;
        case USTC_CCD_ACCESS_ERROR:
            return AAOS_ECONNREFUSED;
            break;
        case USTC_CCD_TIMEOUT:
            return AAOS_ETIMEDOUT;
            break;
        case USTC_CCD_DEVICE_BUSY:
            return AAOS_EBUSY;
            break;
        case USTC_CCD_IO_ERROR:
            return AAOS_EIO;
            break;
        case USTC_CCD_NOT_IMPLEMENTED:
            return AAOS_ENOTSUP;
            break;
        case USTC_CCD_CONFIG_NOT_FOUND:
            return AAOS_ENOENT;
            break;
        case USTC_CCD_ACCESS_ABORT:
            return AAOS_EINTR;
            break;
        case USTC_CCD_DEVICE_NOT_FOUND:
            return AAOS_ENOTFOUND;
            break;
        default:
            return AAOS_ERROR;
            break;
    }
    
    return AAOS_OK;
}

static int
USTCCamera_raw(void *_self, const void *write_buffer, size_t write_buffer_size, size_t *write_size, void *read_buffer, size_t read_buffer_size, size_t *read_size)
{
struct USTCCamera *self = cast(USTCCamera(), _self);

FILE *fp = fmemopen((void *) write_buffer, write_buffer_size, "r+");
char func_name[NAMESIZE];
int ret = AAOS_OK;

fscanf(fp, "%s", func_name);

if (strcmp(func_name, "Initialize") == 0 ) {
        unsigned int (*Initialize) (unsigned int , unsigned int);
	unsigned int which, log_level;
	Initialize = dlsym(self->dlh, "Initialize");
        if ((ret = fscanf(fp, "%u %u", &log_level, &which)) < 2 ){
		ret = AAOS_EBADCMD;
            goto error;
        } else {
		ret = Initialize(log_level, which);
		ret = ustc_error_mapping(ret);
            if (read_size != NULL) {
                *read_size = 0;
            }
        }
} else if (strcmp(func_name, "ShutDown") == 0 ) {
        unsigned int (*ShutDown) (void);
        ShutDown = dlsym(self->dlh, "ShutDown");
        ret = ShutDown();
        ret = ustc_error_mapping(ret);
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "CameraReset") == 0 ) {
        unsigned int (*CameraReset) (void);
        CameraReset = dlsym(self->dlh, "CameraReset");
        ret = CameraReset();
        ret = ustc_error_mapping(ret);
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "CancelWait") == 0 ) {
        unsigned int (*CancelWait) (void);
        CancelWait = dlsym(self->dlh, "CancelWait");
        ret = CancelWait();
        ret = ustc_error_mapping(ret);
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "GetExposureInterval") == 0 ) {
        double interval = -1.00;
        unsigned int (*GetExposureInterval) (double *);
        GetExposureInterval = dlsym(self->dlh, "GetExposureInterval");
        ret = GetExposureInterval(&interval);
        ret = ustc_error_mapping(ret);
        snprintf(read_buffer, read_buffer_size, "%.2f", interval);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else if (strcmp(func_name, "GetExposureTime") == 0 ) {
        double exposure_time = -1.00;
        unsigned int (*GetExposureTime) (double *);
        GetExposureTime = dlsym(self->dlh, "GetExposureTime");
        ret = GetExposureTime(&exposure_time);
        ret = ustc_error_mapping(ret);
        snprintf(read_buffer, read_buffer_size, "%.2f", exposure_time);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else if (strcmp(func_name, "GetLDCMode") == 0 ) {
        uint8_t mode = 255;
        unsigned int (*GetLDCMode) (uint8_t *);
        GetLDCMode = dlsym(self->dlh, "GetLDCMode");
        ret = GetLDCMode(&mode);
        ret = ustc_error_mapping(ret);
        snprintf(read_buffer, read_buffer_size, "%d", mode);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else if (strcmp(func_name, "GetNumReadoutSpeed") == 0 ) {
        unsigned int (*GetNumReadoutSpeed) (void);
        GetNumReadoutSpeed = dlsym(self->dlh, "GetNumReadoutSpeed");
        snprintf(read_buffer, read_buffer_size, "%d", GetNumReadoutSpeed());
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else if (strcmp(func_name, "GetTriggerMode") == 0 ) {
        uint8_t mode = 255;
        unsigned int (*GetTriggerMode) (uint8_t *);
        GetTriggerMode = dlsym(self->dlh, "GetTriggerMode");
        ret = GetTriggerMode(&mode);
        ret = ustc_error_mapping(ret);
        snprintf(read_buffer, read_buffer_size, "%d", mode);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else if (strcmp(func_name, "SetContinuousCapture") == 0 ) {
        unsigned int (*SetContinuousCapture) (uint16_t);
        unsigned int n_frame;
        SetContinuousCapture = dlsym(self->dlh, "SetContinuousCapture");
        if ((ret = fscanf(fp, "%u", &n_frame)) < 1 ){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            ret = SetContinuousCapture(n_frame);
            ret = ustc_error_mapping(ret);
            if (read_size != NULL) {
                *read_size = 0;
            }
        }
    } else if (strcmp(func_name, "SetEraseCount") == 0 ) {
        unsigned int (*SetEraseCount) (uint8_t);
        uint8_t count;
        SetEraseCount = dlsym(self->dlh, "SetEraseCount");
        if ((ret = fscanf(fp, "%c", &count)) < 1 ){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            ret = SetEraseCount(count);
            ret = ustc_error_mapping(ret);
            if (read_size != NULL) {
                *read_size = 0;
            }
        }
    } else if (strcmp(func_name, "SetExposureInterval") == 0 ) {
        unsigned int (*SetExposureInterval) (double);
	double interval;
	SetExposureInterval = dlsym(self->dlh, "SetExposureInterval");
	if ((ret = fscanf(fp, "%lf", &interval)) < 1 ){
		ret = AAOS_EBADCMD;
            goto error;
        } else {
		ret = SetExposureInterval(interval);
		ret = ustc_error_mapping(ret);
            if (read_size != NULL) {
                *read_size = 0;
            }
        }
} else if (strcmp(func_name, "SetExposureTime") == 0 ) {
        unsigned int (*SetExposureTime) (double);
	double exposure_time;
	SetExposureTime = dlsym(self->dlh, "SetExposureTime");
	if ((ret = fscanf(fp, "%lf", &exposure_time)) < 1 ){
		ret = AAOS_EBADCMD;
            goto error;
        } else {
		ret = SetExposureTime(exposure_time);
		ret = ustc_error_mapping(ret);
            if (read_size != NULL) {
                *read_size = 0;
            }
        }
} else if (strcmp(func_name, "SetEraseCount") == 0 ) {
        unsigned int (*SetEraseCount) (uint8_t);
        uint8_t count;
        SetEraseCount = dlsym(self->dlh, "SetEraseCount");
        if ((ret = fscanf(fp, "%c", &count)) < 1 ){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            ret = SetEraseCount(count);
            ret = ustc_error_mapping(ret);
            if (read_size != NULL) {
                *read_size = 0;
            }
        }
    } else if (strcmp(func_name, "SetPreAmpGain") == 0 ) {
        unsigned int (*SetPreAmpGain) (uint8_t);
        uint8_t gain;
        if ((ret = fscanf(fp, "%c", &gain)) < 1 ){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            if (gain > 1) {
                ret = AAOS_EINVAL;
                goto error;
            }
            SetPreAmpGain = dlsym(self->dlh, "SetPreAmpGain");
            ret = SetPreAmpGain(gain);
            ret = ustc_error_mapping(ret);
            if (read_size != NULL) {
                *read_size = 0;
            }
        }
} else if (strcmp(func_name, "SetShutter") == 0 ) {
        unsigned int (*SetShutter) (uint8_t);
        uint8_t mode;
        if ((ret = fscanf(fp, "%c", &mode)) < 1 ){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            if (mode > 2) {
                ret = AAOS_EINVAL;
                goto error;
            }
            SetShutter = dlsym(self->dlh, "SetShutter");
            ret = SetShutter(mode);
            ret = ustc_error_mapping(ret);
            if (read_size != NULL) {
                *read_size = 0;
            }
        }
    } else if (strcmp(func_name, "SetTriggerMode") == 0 ) {
        unsigned int (*SetTriggerMode) (uint8_t);
        uint8_t mode;
        if ((ret = fscanf(fp, "%c", &mode)) < 1 ){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            if (mode > 3) {
                ret = AAOS_EINVAL;
                goto error;
            }
            SetTriggerMode = dlsym(self->dlh, "SetTriggerMode");
            ret = SetTriggerMode(mode);
            ret = ustc_error_mapping(ret);
            if (read_size != NULL) {
                *read_size = 0;
            }
        }
    } else if (strcmp(func_name, "StopExposure") == 0 ) {
        unsigned int (*StopExposure) (void);
	StopExposure = dlsym(self->dlh, "StopExposure");
	ret = StopExposure();
	ret = ustc_error_mapping(ret);
        if (read_size != NULL) {
            *read_size = 0;
        }
} else if (strcmp(func_name, "StartExposure") == 0 ) {
        unsigned int (*StartExposure) (void);
        StartExposure = dlsym(self->dlh, "StartExposure");
        ret = StartExposure();
        ret = ustc_error_mapping(ret);
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "WaitForAcquisition") == 0 ) {
        unsigned int (*WaitForAcquisition) (void);
        WaitForAcquisition = dlsym(self->dlh, "WaitForAcquisition");
        ret = WaitForAcquisition();
        ret = ustc_error_mapping(ret);
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "WaitForAcquisitionTimeOut") == 0 ) {
        unsigned int (*WaitForAcquisitionTimeOut) (int);
        int timeout;
        if ((ret = fscanf(fp, "%d", &timeout)) < 1 ){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            if (timeout < 0) {
                ret = AAOS_EINVAL;
                goto error;
            }
            WaitForAcquisitionTimeOut = dlsym(self->dlh, "WaitForAcquisitionTimeOut");
            ret = WaitForAcquisitionTimeOut(timeout);
            ret = ustc_error_mapping(ret);
            if (read_size != NULL) {
                *read_size = 0;
            }
        }
    } else if (strcmp(func_name, "DetectorPowerOn") == 0 ) {
        unsigned int (*DetectorPowerON) (void);
        DetectorPowerON = dlsym(self->dlh, "DetectorPowerOn");
        ret = DetectorPowerON();
        ret = ustc_error_mapping(ret);
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "DetectorPowerOff") == 0 ) {
        unsigned int (*DetectorPowerOFF) (void);
        DetectorPowerOFF = dlsym(self->dlh, "DetectorPowerOff");
        ret = DetectorPowerOFF();
        ret = ustc_error_mapping(ret);
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "Controller_temperature") == 0 ) {
        unsigned int (*Controller_temperature) (double *);
        double temperature = 9999.00;
        Controller_temperature = dlsym(self->dlh, "Controller_temperature");
        ret = Controller_temperature(&temperature);
        ret = ustc_error_mapping(ret);
        snprintf(read_buffer, read_buffer_size, "%.2f", temperature);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else if (strcmp(func_name, "ControllerFan") == 0 ) {
        unsigned int (*ControllerFan) (uint8_t);
        unsigned int level;
        if ((ret = fscanf(fp, "%u", &level)) < 1 ){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            ControllerFan = dlsym(self->dlh, "ControllerFan");
            ret = ControllerFan(level);
            ret = ustc_error_mapping(ret);
            if (read_size != NULL) {
                *read_size = 0;
            }
        }
    } else if (strcmp(func_name, "ControllerHeat") == 0 ) {
        unsigned int (*ControllerHeat) (uint8_t);
        unsigned int heatpwm;
        if ((ret = fscanf(fp, "%u", &heatpwm)) < 1 ){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            ControllerHeat = dlsym(self->dlh, "ControllerHeat");
            ret = ControllerHeat(heatpwm);
            ret = ustc_error_mapping(ret);
            if (read_size != NULL) {
                *read_size = 0;
            }
        }
    } else if (strcmp(func_name, "CoolerOff") == 0 ) {
        unsigned int (*CoolerOff) (void);
        CoolerOff = dlsym(self->dlh, "CoolerOff");
        ret = CoolerOff();
        ret = ustc_error_mapping(ret);
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "CoolerOn") == 0 ) {
        unsigned int (*CoolerOn) (void);
        CoolerOn = dlsym(self->dlh, "CoolerOn");
        ret = CoolerOn();
        ret = ustc_error_mapping(ret);
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "entemp") == 0 ) {
        unsigned int (*entemp) (double *);
        double temperature = 9999.00;
        entemp = dlsym(self->dlh, "entemp");
        ret = entemp(&temperature);
        ret = ustc_error_mapping(ret);
        snprintf(read_buffer, read_buffer_size, "%.2f", temperature);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else if (strcmp(func_name, "get_power") == 0 ) {
        unsigned int (*get_power) (double *);
        double power = -1.00;
        get_power = dlsym(self->dlh, "get_power");
        ret = get_power(&power);
        ret = ustc_error_mapping(ret);
        snprintf(read_buffer, read_buffer_size, "%.2f", power);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else if (strcmp(func_name, "GetBitDepth") == 0 ) {
        unsigned int (*GetBitDepth) (int *);
        int depth = -1;
        GetBitDepth = dlsym(self->dlh, "GetBitDepth");
        ret = GetBitDepth(&depth);
        ret = ustc_error_mapping(ret);
        snprintf(read_buffer, read_buffer_size, "%d", depth);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else if (strcmp(func_name, "GetCameraReady") == 0 ) {
        unsigned int (*GetCameraReady) (uint8_t *);
        uint8_t stat_;
        GetCameraReady = dlsym(self->dlh, "GetCameraReady");
        ret = GetCameraReady(&stat_);
        ret = ustc_error_mapping(ret);
        snprintf(read_buffer, read_buffer_size, "%u", stat_);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else if (strcmp(func_name, "GetChipNum") == 0) {
        unsigned int (*GetChipNum) (int *);
        int chipnum = 1;
        GetChipNum = dlsym(self->dlh, "GetChipNum");
        ret = GetChipNum(&chipnum);
        ret = ustc_error_mapping(ret);
        snprintf(read_buffer, read_buffer_size, "%d", chipnum);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else if (strcmp(func_name, "GetCoolerStatus") == 0 ) {
        unsigned int (*GetCoolerStatus) (uint8_t *);
        uint8_t coolstat;
        GetCoolerStatus = dlsym(self->dlh, "GetCoolerStatus");
        ret = GetCoolerStatus(&coolstat);
        ret = ustc_error_mapping(ret);
        snprintf(read_buffer, read_buffer_size, "%u", coolstat);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else if (strcmp(func_name, "GetCurrentByChannel") == 0) {
        unsigned int (*GetCurrentByChannel) (int, float *);
        int chan;
        float cur = -1.00;
        if ((ret = fscanf(fp, "%d", &chan)) < 1 ){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            if (chan < 0 || chan > 3) {
                ret = AAOS_EINVAL;
                goto error;
            } else {
                GetCurrentByChannel = dlsym(self->dlh, "GetCurrentByChannel");
                ret = GetCurrentByChannel(chan, &cur);
                snprintf(read_buffer, read_buffer_size, "%.2f", cur);
                if (read_size != NULL) {
                    *read_size = strlen(read_buffer) + 1;
                }
            }
        }
    } else if (strcmp(func_name, "GetDetector") == 0 ) {
        unsigned int (*GetDetector) (int *, int *);
        int width = -1, height = -1;
        GetDetector = dlsym(self->dlh, "GetDetector");
        ret = GetDetector(&width, &height);
        ret = ustc_error_mapping(ret);
        snprintf(read_buffer, read_buffer_size, "%d %d", width, height);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else if (strcmp(func_name, "GetExposureROI") == 0 ) {
        unsigned int (*GetExposureROI) (int *, int *, int *, int *);
        int rowStartNum = -1, rowKeepNum = -1, colStartNum = -1, colKeepNum = -1;
        GetExposureROI = dlsym(self->dlh, "GetExposureROI");
        ret = GetExposureROI(&rowStartNum, &rowKeepNum, &colStartNum, &colKeepNum);
        ret = ustc_error_mapping(ret);
        snprintf(read_buffer, read_buffer_size, "%d %d %d %d", rowStartNum, rowKeepNum, colStartNum, colKeepNum);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else if (strcmp(func_name, "GetFanSpeed") == 0 ) {
        unsigned int (*GetFanSpeed) (uint8_t *);
        uint8_t speed = 255;
        GetFanSpeed = dlsym(self->dlh, "GetFanSpeed");
        ret = GetFanSpeed(&speed);
        ret = ustc_error_mapping(ret);
        snprintf(read_buffer, read_buffer_size, "%u", speed);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else if (strcmp(func_name, "GetGain") == 0 ) {
        unsigned int (*GetGain) (float *, float *);
        float high = -1.00, low = -1.00;
        GetGain = dlsym(self->dlh, "GetGain");
        ret = GetGain(&high, &low);
        ret = ustc_error_mapping(ret);
        snprintf(read_buffer, read_buffer_size, "%.2f %.2f", high, low);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else if (strcmp(func_name, "GetHeatPWM") == 0 ) {
        unsigned int (*GetHeatPWM) (uint8_t *);
        uint8_t heatpwm = 255;
        GetHeatPWM = dlsym(self->dlh, "GetHeatPWM");
        ret = GetHeatPWM(&heatpwm);
        ret = ustc_error_mapping(ret);
        snprintf(read_buffer, read_buffer_size, "%u", heatpwm);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else if (strcmp(func_name, "GetPumpCur") == 0 ) {
        unsigned int (*GetPumpCur) (double *);
        double cur = -1.00;
        GetPumpCur = dlsym(self->dlh, "GetPumpCur");
        ret = GetPumpCur(&cur);
        ret = ustc_error_mapping(ret);
        snprintf(read_buffer, read_buffer_size, "%.2f", cur);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else if (strcmp(func_name, "GetPumpMaxPower") == 0 ) {
        unsigned int (*GetPumpMaxPower) (double *);
        double power = -1.00;
        GetPumpMaxPower = dlsym(self->dlh, "GetPumpMaxPower");
        ret = GetPumpMaxPower(&power);
        ret = ustc_error_mapping(ret);
        snprintf(read_buffer, read_buffer_size, "%.2f", power);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else if (strcmp(func_name, "GetPumpTargetVol") == 0 ) {
        unsigned int (*GetPumpTargetVol) (double *);
        double voltage = -1.00;
        GetPumpTargetVol = dlsym(self->dlh, "GetPumpTargetVol");
        ret = GetPumpTargetVol(&voltage);
        ret = ustc_error_mapping(ret);
        snprintf(read_buffer, read_buffer_size, "%.2f", voltage);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else if (strcmp(func_name, "GetPumpUint") == 0 ) {
        unsigned int (*GetPumpUint) (int *);
        int unit = -1;
        GetPumpUint = dlsym(self->dlh, "GetPumpUint");
        ret = GetPumpUint(&unit);
        ret = ustc_error_mapping(ret);
        snprintf(read_buffer, read_buffer_size, "%d", unit);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else if (strcmp(func_name, "GetPumpVacuum") == 0 ) {
        unsigned int (* GetPumpVacuum) (double *);
        double vac = -1.00;
        GetPumpVacuum = dlsym(self->dlh, " GetPumpVacuum");
        ret =  GetPumpVacuum(&vac);
        ret = ustc_error_mapping(ret);
        snprintf(read_buffer, read_buffer_size, "%.2f", vac);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else if (strcmp(func_name, "GetPumpVol") == 0 ) {
        unsigned int (*GetPumpVol) (double *);
        double voltage = -1.00;
        GetPumpVol = dlsym(self->dlh, "GetPumpVol");
        ret = GetPumpVol(&voltage);
        ret = ustc_error_mapping(ret);
        snprintf(read_buffer, read_buffer_size, "%.2f", voltage);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else if (strcmp(func_name, "GetTcbTemp") == 0 ) {
        char temp_name[32];
        if ((ret = fscanf(fp, "%s", temp_name)) < 1 ){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            unsigned int (*GetTcbTemp) (const char *, float *);
            float temperature = 9999.00;
            GetTcbTemp = dlsym(self->dlh, "GetTcbTemp");
            ret = GetTcbTemp(temp_name, &temperature);
            ret = ustc_error_mapping(ret);
            snprintf(read_buffer, read_buffer_size, "%.2f", temperature);
            if (read_size != NULL) {
                *read_size = strlen(read_buffer) + 1;
            }
        }
    } else if (strcmp(func_name, "GetTemperature") == 0 ) {
        char temp_name[32];
        if ((ret = fscanf(fp, "%s", temp_name)) < 1 ){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            unsigned int (*GetTemperature) (const char *, float *);
            float temperature = 9999.00;
            GetTemperature = dlsym(self->dlh, "GetTemperature");
            ret = GetTemperature(temp_name, &temperature);
            ret = ustc_error_mapping(ret);
            snprintf(read_buffer, read_buffer_size, "%.2f", temperature);
            if (read_size != NULL) {
                *read_size = strlen(read_buffer) + 1;
            }
        }
    } else if (strcmp(func_name, "GetVacuum") == 0 ) {
        int pr;
        if ((ret = fscanf(fp, "%d", &pr)) < 1 ){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            unsigned int (*GetVacuum) (int, double *);
            double press = -1.00;
            GetVacuum = dlsym(self->dlh, "GetVacuum");
            ret = GetVacuum(pr, &press);
            ret = ustc_error_mapping(ret);
            snprintf(read_buffer, read_buffer_size, "%.2f", press);
            if (read_size != NULL) {
                *read_size = strlen(read_buffer) + 1;
            }
        }
    } else if (strcmp(func_name, "GetVideoMode") == 0 ) {
        unsigned int (*GetVideoMode) (uint16_t *);
        uint16_t mode = 65535;
        GetVideoMode = dlsym(self->dlh, "GetVideoMode");
        ret = GetVideoMode(&mode);
        ret = ustc_error_mapping(ret);
        snprintf(read_buffer, read_buffer_size, "%u", mode);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else if (strcmp(func_name, "GetVoltageByChannel") == 0) {
        unsigned int (*GetVoltageByChannel) (int, float *);
        int chan;
        float voltage = -1.00;
        if ((ret = fscanf(fp, "%d", &chan)) < 1 ){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            if (chan < 0 || chan > 3) {
                ret = AAOS_EINVAL;
                goto error;
            } else {
                GetVoltageByChannel = dlsym(self->dlh, "GetVoltageByChannel");
                ret = GetVoltageByChannel(chan, &voltage);
                snprintf(read_buffer, read_buffer_size, "%.2f", voltage);
                if (read_size != NULL) {
                    *read_size = strlen(read_buffer) + 1;
                }
            }
        }
    } else if (strcmp(func_name, "hottemp") == 0 ) {
        unsigned int (*hottemp) (double *);
        double temperature = 9999.00;
        hottemp = dlsym(self->dlh, "hottemp");
        ret = hottemp(&temperature);
        ret = ustc_error_mapping(ret);
        snprintf(read_buffer, read_buffer_size, "%.2f", temperature);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else if (strcmp(func_name, "ListCameras") == 0 ) {
        unsigned int (*ListCameras) (int *);
        int camera_num = -1;
        ListCameras = dlsym(self->dlh, "ListCameras");
        ret = ListCameras(&camera_num);
        ret = ustc_error_mapping(ret);
        snprintf(read_buffer, read_buffer_size, "%d", camera_num);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else if (strcmp(func_name, "Motor_temperature") == 0 ) {
        unsigned int (*Motor_temperature) (double *);
        double temperature = 9999.00;
        Motor_temperature = dlsym(self->dlh, "Motor_temperature");
        ret = Motor_temperature(&temperature);
        ret = ustc_error_mapping(ret);
        snprintf(read_buffer, read_buffer_size, "%.2f", temperature);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else if (strcmp(func_name, "PumpOff") == 0 ) {
        unsigned int (*PumpOff) (void);
        PumpOff = dlsym(self->dlh, "PumpOff");
        ret = PumpOff();
        ret = ustc_error_mapping(ret);
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "PumpOn") == 0 ) {
        unsigned int (*PumpOn) (void);
        PumpOn = dlsym(self->dlh, "PumpOn");
        ret = PumpOn();
        ret = ustc_error_mapping(ret);
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "PumpProtect") == 0) {
        unsigned int (*PumpProtect) (bool);
        unsigned int onoff_;
        bool onoff;
        if ((ret = fscanf(fp, "%u", &onoff_)) < 1 ){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            if (onoff_) {
                onoff = true;
            } else {
                onoff = false;
            }
            PumpProtect = dlsym(self->dlh, "PumpProtect");
            ret = PumpProtect(onoff);
            ret = ustc_error_mapping(ret);
            if (read_size != NULL) {
                *read_size = 0;
            }
        }
    } else if (strcmp(func_name, "read_cooler_status") == 0 ) {
        unsigned int (*read_cooler_status) (int *);
        int error = -1;
        read_cooler_status = dlsym(self->dlh, "read_cooler_status");
        ret = read_cooler_status(&error);
        ret = ustc_error_mapping(ret);
        snprintf(read_buffer, read_buffer_size, "%d", error);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else if (strcmp(func_name, "RetransNthImg") == 0) {
        unsigned int (*RetransNthImg) (uint16_t);
        uint16_t idx;
        if ((ret = fscanf(fp, "%hu", &idx)) < 1 ){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            RetransNthImg = dlsym(self->dlh, "RetransNthImg");
            ret = RetransNthImg(idx);
            ret = ustc_error_mapping(ret);
            if (read_size != NULL) {
                *read_size = 0;
            }
        }
    } else if (strcmp(func_name, "ROIDisable") == 0) {
        unsigned int (*ROIDisable) (void);
        ROIDisable = dlsym(self->dlh, "ROIDisable");
        ret = ROIDisable();
        ret = ustc_error_mapping(ret);
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "ROIEnable") == 0) {
        unsigned int (*ROIEnable) (uint16_t, uint16_t, uint16_t, uint16_t);
        uint16_t rowStartNum, rowKeepNum, colStartNum, colKeepNum;
        if ((ret = fscanf(fp, "%hu %hu %hu %hu", &rowStartNum, &rowKeepNum, &colStartNum, &colKeepNum)) < 4 ){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            ROIEnable = dlsym(self->dlh, "ROIEnable");
            ret = ROIEnable(rowStartNum, rowKeepNum, colStartNum, colKeepNum);
            ret = ustc_error_mapping(ret);
            if (read_size != NULL) {
                *read_size = 0;
            }
        }
    } else if (strcmp(func_name, "Runningtime") == 0 ) {
        unsigned int (*Runningtime) (double *);
        double running_time = -1.00;
        Runningtime = dlsym(self->dlh, "Runningtime");
        ret = Runningtime(&running_time);
        ret = ustc_error_mapping(ret);
        snprintf(read_buffer, read_buffer_size, "%.2f", running_time);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else if (strcmp(func_name, " Save_configuration") == 0) {
        unsigned int (* Save_configuration) (void);
        Save_configuration = dlsym(self->dlh, " Save_configuration");
        ret =  Save_configuration();
        ret = ustc_error_mapping(ret);
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "set_channel_votlage") == 0) {
        unsigned int (*set_channel_votlage) (int, float);
        int chan;
        float voltage;
        if ((ret = fscanf(fp, "%d %f", &chan, &voltage)) < 2 ){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            if (chan < 1 || chan > 3) {
                ret = AAOS_EINVAL;
                goto error;
            }
            set_channel_votlage = dlsym(self->dlh, "set_channel_votlage");
            ret = set_channel_votlage(chan, voltage);
            ret = ustc_error_mapping(ret);
            if (read_size != NULL) {
                *read_size = 0;
            }
        }
    } else if (strcmp(func_name, "set_cooltemp") == 0) {
        unsigned int (*set_cooltemp) (float);
        float temperature;
        if ((ret = fscanf(fp, "%f", &temperature)) < 1){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            set_cooltemp = dlsym(self->dlh, "set_cooltemp");
            ret = set_cooltemp(temperature);
            ret = ustc_error_mapping(ret);
            if (read_size != NULL) {
                *read_size = 0;
            }
        }
    } else if (strcmp(func_name, "set_pid") == 0) {
        unsigned int (*set_pid) (int, int);
        int chan, status;
        if ((ret = fscanf(fp, "%d %d", &chan, &status)) < 2 ){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            if (chan < 1 || chan > 3) {
                ret = AAOS_EINVAL;
                goto error;
            }
            if (status != 0 && status != 1) {
                ret = AAOS_EINVAL;
                goto error;
            }
            set_pid = dlsym(self->dlh, "set_pid");
            ret = set_pid(chan, status);
            ret = ustc_error_mapping(ret);
            if (read_size != NULL) {
                *read_size = 0;
            }
        }
    } else if (strcmp(func_name, "set_power") == 0) {
        unsigned int (*set_power) (int, int);
        int chan, status;
        if ((ret = fscanf(fp, "%d %d", &chan, &status)) < 2 ){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            if (chan != 1 && chan != 2) {
                ret = AAOS_EINVAL;
                goto error;
            }
            if (status != 0 && status != 1) {
                ret = AAOS_EINVAL;
                goto error;
            }
            set_power = dlsym(self->dlh, "set_power");
            ret = set_power(chan, status);
            ret = ustc_error_mapping(ret);
            if (read_size != NULL) {
                *read_size = 0;
            }
        }
    } else if (strcmp(func_name, "set_tcb_target_temp") == 0) {
        unsigned int (*set_tcb_target_temp) (int, int);
        int chan, temperature;
        if ((ret = fscanf(fp, "%d %d", &chan, &temperature)) < 2 ){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            if (chan < 1 || chan > 3) {
                ret = AAOS_EINVAL;
                goto error;
            }
            set_tcb_target_temp = dlsym(self->dlh, "set_tcb_target_temp");
            ret = set_tcb_target_temp(chan, temperature);
            ret = ustc_error_mapping(ret);
            if (read_size != NULL) {
                *read_size = 0;
            }
        }
    } else if (strcmp(func_name, "SetGain") == 0) {
        unsigned int (*SetGain) (float, float);
        float high, low;
        if ((ret = fscanf(fp, "%f %f", &high, &low)) < 2 ){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            SetGain = dlsym(self->dlh, "SetGain");
            ret = SetGain(high, low);
            ret = ustc_error_mapping(ret);
            if (read_size != NULL) {
                *read_size = 0;
            }
        }
    } else if (strcmp(func_name, "SetHeatPWM") == 0) {
        unsigned int (*SetHeatPWM) (int, uint8_t *);
        int chan;
        uint8_t heatpwm;
        if ((ret = fscanf(fp, "%d %c", &chan, &heatpwm)) < 2 ){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            SetHeatPWM = dlsym(self->dlh, "SetHeatPWM");
            ret = SetHeatPWM(chan, &heatpwm);
            ret = ustc_error_mapping(ret);
            if (read_size != NULL) {
                *read_size = 0;
            }
        }
    } else if (strcmp(func_name, "SetPumpMaxPower") == 0) {
        unsigned int (*SetPumpMaxPower) (double);
        double power;
        if ((ret = fscanf(fp, "%lf", &power)) < 1){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            SetPumpMaxPower = dlsym(self->dlh, "SetPumpMaxPower");
            if (power < 10. || power > 40.) {
                ret =AAOS_EINVAL;
                goto error;
            }
            ret = SetPumpMaxPower(power);
            ret = ustc_error_mapping(ret);
            if (read_size != NULL) {
                *read_size = 0;
            }
        }
    } else if (strcmp(func_name, "SetPumpTargetVol") == 0) {
        unsigned int (*SetPumpTargetVol) (double);
        double voltage;
        if ((ret = fscanf(fp, "%lf", &voltage)) < 1){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            SetPumpTargetVol = dlsym(self->dlh, "SetPumpMaxPower");
            if (voltage < 3000. || voltage > 7000.) {
                ret =AAOS_EINVAL;
                goto error;
            }
            ret = SetPumpTargetVol(voltage);
            ret = ustc_error_mapping(ret);
            if (read_size != NULL) {
                *read_size = 0;
            }
        }
    } else if (strcmp(func_name, "SetPumpUnit") == 0) {
        unsigned int (*SetPumpUnit) (int);
        int unit;
        if ((ret = fscanf(fp, "%d", &unit)) < 1){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            SetPumpUnit = dlsym(self->dlh, "SetPumpUnit");
            if (unit < 0 || unit > 2) {
                ret =AAOS_EINVAL;
                goto error;
            }
            ret = SetPumpUnit(unit);
            ret = ustc_error_mapping(ret);
            if (read_size != NULL) {
                *read_size = 0;
            }
        }
    } else if (strcmp(func_name, "SetRtnsMode") == 0) {
        unsigned int (*SetRtnsMode) (uint16_t);
        uint16_t mode;
        if ((ret = fscanf(fp, "%hu", &mode)) < 1){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            SetRtnsMode = dlsym(self->dlh, "SetRtnsMode");
            if (mode > 1) {
                ret =AAOS_EINVAL;
                goto error;
            }
            ret = SetRtnsMode(mode);
            ret = ustc_error_mapping(ret);
            if (read_size != NULL) {
                *read_size = 0;
            }
        }
    } else if (strcmp(func_name, "SetVacuumUnit") == 0) {
        unsigned int (*SetVacuumUnit) (int);
        int unit;
        if ((ret = fscanf(fp, "%d", &unit)) < 1){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            SetVacuumUnit = dlsym(self->dlh, "SetVacuumUnit");
            if (unit < 0 || unit > 2) {
                ret =AAOS_EINVAL;
                goto error;
            }
            ret = SetVacuumUnit(unit);
            ret = ustc_error_mapping(ret);
            if (read_size != NULL) {
                *read_size = 0;
            }
        }
    } else if (strcmp(func_name, "VideoMode") == 0) {
        unsigned int (*VideoMode) (uint16_t);
        uint16_t mode;
        if ((ret = fscanf(fp, "%hu", &mode)) < 1){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            VideoMode = dlsym(self->dlh, "VideoMode");
            if (mode > 1) {
                ret =AAOS_EINVAL;
                goto error;
            }
            ret = VideoMode(mode);
            ret = ustc_error_mapping(ret);
            if (read_size != NULL) {
                *read_size = 0;
            }
        }
    } else if (strcmp(func_name, "SetSerialNumber") == 0) {
        unsigned int (*SetSerialNumber) (PIXELX_SERIAL *);
        uint8_t Model_Enum, MB_ID, Year, Month, Day, Hour, version, version_aa, id, checksum;
        if ((ret = fscanf(fp, "%c %c %c %c %c %c %c %c %c %c", &Model_Enum, &MB_ID, &Year, &Month, &Day, &Hour, &version, &version_aa, &id, &checksum)) < 10){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            PIXELX_SERIAL serial_no;
            serial_no.Model_Enum = Model_Enum;
            serial_no.MB_ID = MB_ID;
            serial_no.Year = Year;
            serial_no.Month = Month;
            serial_no.Day = Day;
            serial_no.Hour = Hour;
            serial_no.version = version;
            serial_no.version_aa = version_aa;
            serial_no.id = id;
            serial_no.checksum = checksum;
            SetSerialNumber = dlsym(self->dlh, "SetSerialNumber");
            ret = SetSerialNumber(&serial_no);
            ret = ustc_error_mapping(ret);
            if (read_size != NULL) {
                *read_size = 0;
            }
        }
    } else if (strcmp(func_name, "GetSerialNumber") == 0) {
        unsigned int (*GetSerialNumber) (PIXELX_SERIAL *);
        PIXELX_SERIAL serial_no;
            
        GetSerialNumber = dlsym(self->dlh, "GetSerialNumber");
        ret = GetSerialNumber(&serial_no);
        ret = ustc_error_mapping(ret);
        snprintf(read_buffer, read_buffer_size, "%03d %03d %02d %02d %02d %02d %03d %03d %03d %03d", serial_no.Model_Enum, serial_no.MB_ID, serial_no.Year, serial_no.Month, serial_no.Day, serial_no.Hour, serial_no.version, serial_no.version_aa, serial_no.id, serial_no.checksum);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else if (strcmp(func_name, "GetTemperatureDiff") == 0) {
        unsigned int (*GetTemperatureDiff) (double *, double *);
        double cold_diff, hot_diff;
        GetTemperatureDiff = dlsym(self->dlh, "GetTemperatureDiff");
        ret = GetTemperatureDiff(&cold_diff, &hot_diff);
        ret = ustc_error_mapping(ret);
        snprintf(read_buffer, read_buffer_size, "%.2f %.2f", cold_diff, hot_diff);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
    } else if (strcmp(func_name, "CamErrorName") == 0) {
        const char * (*CamErrorName) (int);
        int errorcode;
        if ((ret = fscanf(fp, "%d", &errorcode)) < 1){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            CamErrorName = dlsym(self->dlh, "CamErrorName");
            snprintf(read_buffer, read_buffer_size, "%s", CamErrorName(errorcode));
            if (read_size != NULL) {
                *read_size = strlen(read_buffer) + 1;
            }
        }
    } else if (strcmp(func_name, "CamErrorStrerror") == 0) {
        const char * (*CamErrorStrerror) (int);
        int errorcode;
        if ((ret = fscanf(fp, "%d", &errorcode)) < 1){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            CamErrorStrerror = dlsym(self->dlh, "CamErrorStrerror");
            snprintf(read_buffer, read_buffer_size, "%s", CamErrorStrerror(errorcode));
            if (read_size != NULL) {
                *read_size = strlen(read_buffer) + 1;
            }
        }
    } else if (strcmp(func_name, "SaveAsFITS") == 0) {
        unsigned int (*SaveAsFITS) (const char *);
        char path[PATHSIZE];
        if ((ret = fscanf(fp, "%s", path)) < 1 ){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            SaveAsFITS = dlsym(self->dlh, "SaveAsFITS");
            ret = SaveAsFITS(path);
            ret = ustc_error_mapping(ret);
            if (read_size != NULL) {
                *read_size = 0;
            }
        }
    } else if (strcmp(func_name, "SaveAsFITSAccum") == 0) {
        unsigned int (*SaveAsFITSAccum) (const char *);
        char path[PATHSIZE];
        if ((ret = fscanf(fp, "%s", path)) < 1 ){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            SaveAsFITSAccum = dlsym(self->dlh, "SaveAsFITSAccum");
            ret = SaveAsFITSAccum(path);
            ret = ustc_error_mapping(ret);
            if (read_size != NULL) {
                *read_size = 0;
            }
        }
    } else if (strcmp(func_name, "SaveNthAsFITS") == 0) {
        unsigned int (*SaveNthAsFITS) (const char *, uint16_t);
        char path[PATHSIZE];
        uint16_t idx;
        if ((ret = fscanf(fp, "%s %hu", path, &idx)) < 2 ){
            ret = AAOS_EBADCMD;
            goto error;
        } else {
            SaveNthAsFITS = dlsym(self->dlh, "SaveNthAsFITS");
            ret = SaveNthAsFITS(path, idx);
            ret = ustc_error_mapping(ret);
            if (read_size != NULL) {
                *read_size = 0;
            }
        }
    }
    else {
	ret = AAOS_EBADCMD;
}
    
error:
fclose(fp);
return ret;
}

static int
USTCCamera_abort(void *_self)
{
    struct USTCCamera *self = cast(USTCCamera(), _self);
    
    int ret = AAOS_OK;
    
    unsigned int (*StopExposure)(void) = dlsym(self->dlh, "StopExposure");
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    if ((self->_.d_state.state&DETECTOR_STATE_EXPOSING) || (self->_.d_state.state&DETECTOR_STATE_READING)) {
        Pthread_mutex_lock(&self->mtx);
        ret = StopExposure();
        Pthread_mutex_unlock(&self->mtx);
        if (ret == USTC_CCD_SUCCESS) {
            ret = AAOS_OK;
            self->_.d_state.state |= DETECTOR_STATE_IDLE;
        } else {
            ret = ustc_error_mapping(ret);
        }
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    return ret;
}

static int
USTCCamera_stop(void *_self)
{
    return AAOS_ENOTSUP;
}

static int
USTCCamera_set_frame_rate(void *_self, double frame_rate)
{
    struct USTCCamera *self = cast(USTCCamera(), _self);
    
    unsigned int (*SetExposureInterval)(double) = dlsym(self->dlh, "SetExposureInterval");
    int ret;

    Pthread_mutex_lock(&self->_.d_state.mtx);
    Pthread_mutex_lock(&self->mtx);
    ret = SetExposureInterval(1./frame_rate);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        self->_.d_param.frame_rate = 1./frame_rate;
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
return ustc_error_mapping(ret);
}

static int
USTCCamera_get_frame_rate(void *_self, double *frame_rate)
{
    struct USTCCamera *self = cast(USTCCamera(), _self);
    
    unsigned int (*GetExposureInterval)(double *) = dlsym(self->dlh, "GetExposureInterval");
    int ret;
    
    Pthread_mutex_lock(&self->mtx);
    ret = GetExposureInterval(frame_rate);
    *frame_rate = 1. / *frame_rate;
    Pthread_mutex_unlock(&self->mtx);
    
return ustc_error_mapping(ret);
}

static int
USTCCamera_set_exposure_time(void *_self, double exposure_time)
{
    struct USTCCamera *self = cast(USTCCamera(), _self);
    
    unsigned int (*SetExposureTime)(double) = dlsym(self->dlh, "SetExposureTime");
    int ret;
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    Pthread_mutex_lock(&self->mtx);
    ret = SetExposureTime(exposure_time);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        self->_.d_param.exposure_time = exposure_time;
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    return ustc_error_mapping(ret);
}

static int
USTCCamera_get_exposure_time(void *_self, double *exposure_time)
{
    struct USTCCamera *self = cast(USTCCamera(), _self);
    
    unsigned int (*GetExposureInterval)(double *) = dlsym(self->dlh, "GetExposureInterval");
    int ret;
    
    Pthread_mutex_lock(&self->mtx);
    ret = GetExposureInterval(exposure_time);
    Pthread_mutex_unlock(&self->mtx);
    
    return ustc_error_mapping(ret);
}

static int
USTCCamera_enable_cooling(void *_self)
{
    struct USTCCamera *self = cast(USTCCamera(), _self);
    
    unsigned int (*CoolerOn)(void) = dlsym(self->dlh, "CoolerOn");
    int ret;
    
    Pthread_mutex_lock(&self->mtx);
    ret = CoolerOn();
    Pthread_mutex_unlock(&self->mtx);
    
return ustc_error_mapping(ret);
}

static int
USTCCamera_disable_cooling(void *_self)
{
    struct USTCCamera *self = cast(USTCCamera(), _self);
    
    unsigned int (*CoolerOff)(void) = dlsym(self->dlh, "CoolerOff");
    int ret;
    
    Pthread_mutex_lock(&self->mtx);
    ret = CoolerOff();
    Pthread_mutex_unlock(&self->mtx);
    
    return ustc_error_mapping(ret);
}

static int
USTCCamera_set_temperature(void *_self, double temperature)
{
    struct USTCCamera *self = cast(USTCCamera(), _self);
    
    unsigned int (*Coolertemp)(double) = dlsym(self->dlh, "set_cooltemp");
    int ret;
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    Pthread_mutex_lock(&self->mtx);
    ret = Coolertemp(temperature);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        self->_.d_param.temperature = temperature;
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    return ustc_error_mapping(ret);
}

static int
USTCCamera_get_temperature(void *_self, double *temperature)
{
    struct USTCCamera *self = cast(USTCCamera(), _self);
    
    unsigned int (*Coolertemp)(double *) = dlsym(self->dlh, "Coolertemp");
    int ret;
    
    Pthread_mutex_lock(&self->mtx);
    ret = Coolertemp(temperature);
    Pthread_mutex_unlock(&self->mtx);
    
    return ustc_error_mapping(ret);
}

static int
USTCCamera_set_region(void *_self, uint32_t x_offset, uint32_t y_offset, uint32_t width, uint32_t height)
{
    struct USTCCamera *self = cast(USTCCamera(), _self);
    
    int ret;
    
    int (*ROIEnable)(uint16_t, uint16_t, uint16_t, uint16_t) = dlsym(self->dlh, "ROIEnable");
    if (ROIEnable == NULL) {
        return AAOS_ENOTSUP;
    }
    Pthread_mutex_lock(&self->_.d_state.mtx);
    while (self->_.d_state.state == DETECTOR_STATE_EXPOSING || self->_.d_state.state == DETECTOR_STATE_READING) {
        Pthread_cond_wait(&self->_.d_state.cond, &self->_.d_state.mtx);
    }
    Pthread_mutex_lock(&self->mtx);
    ret = ROIEnable(x_offset, width, y_offset, height);
    Pthread_mutex_unlock(&self->mtx);
    ret = ustc_error_mapping(ret);
    if (ret == AAOS_OK) {
        self->_.d_param.image_width = width;
        self->_.d_param.image_height = height;
        self->_.d_param.x_offset = x_offset;
        self->_.d_param.y_offset = y_offset;
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
return ret;
}

static int
USTCCamera_get_region(void *_self, uint32_t *x_offset, uint32_t *y_offset, uint32_t *width, uint32_t *height)
{
    struct USTCCamera *self = cast(USTCCamera(), _self);
    
    int ret;
    uint16_t xo, yo, w, h;
    
    unsigned int (*GetExposureROI)(uint16_t *, uint16_t *, uint16_t *, uint16_t *) = dlsym(self->dlh, "GetExposureROI");
    if (GetExposureROI == NULL) {
        return AAOS_ENOTSUP;
    }
    Pthread_mutex_lock(&self->_.d_state.mtx);
    Pthread_mutex_lock(&self->mtx);
    ret = GetExposureROI(&xo, &w, &yo, &h);
    Pthread_mutex_unlock(&self->mtx);
    ret = ustc_error_mapping(ret);
    if (ret == AAOS_OK) {
        *x_offset = xo;
        *y_offset = yo;
        *width = w;
        *height = h;
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    return ret;
}

static int 
USTCCamera_set_gain(void *_self, double gain)
{
    struct USTCCamera *self = cast(USTCCamera(), _self);
    
    int ret;
    unsigned int (*SetPreAmpGain)(uint8_t) = dlsym(self->dlh, "SetPreAmpGain");
    
    if (self->_.d_cap.gain_available) {
        if (self->_.d_cap.gain_array) {
            size_t i, n = self->_.d_cap.n_gain;
            for (i = 0; i < n; i++) {
                double gain_ = self->_.d_cap.gain_array[i];
                if (fabs(gain - gain_) < 0.0000001 && i == 0) {
                    Pthread_mutex_lock(&self->_.d_state.mtx);
                    Pthread_mutex_lock(&self->mtx);
                    ret = SetPreAmpGain(1);
                    Pthread_mutex_unlock(&self->mtx);
                    self->_.d_param.gain = 5.0;
                    Pthread_mutex_unlock(&self->_.d_state.mtx);
                    goto error;
                } else if (fabs(gain - gain_) < 0.0000001 && i == 1) {
                    
                    Pthread_mutex_lock(&self->_.d_state.mtx);
                    Pthread_mutex_lock(&self->mtx);
                    ret = SetPreAmpGain(0);
                    Pthread_mutex_unlock(&self->mtx);
                    self->_.d_param.gain = 10.0;
                    Pthread_mutex_unlock(&self->_.d_state.mtx);
                    goto error;
                }
            }
            return AAOS_EINVAL;
        } else {
            return AAOS_EINVAL;
        }
    } else {
        return AAOS_ENOTSUP;
    }
    
error:
return ustc_error_mapping(ret);
}

static int 
USTCCamera_get_gain(void *_self, double *gain)
{
    struct USTCCamera *self = cast(USTCCamera(), _self);
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    *gain = self->_.d_param.gain;
    Pthread_mutex_unlock(&self->_.d_state.mtx);

return AAOS_OK;
}

static int 
USTCCamera_set_readout_speed(void *_self, double readout_speed)
{
return AAOS_ENOTSUP;
}

static int 
USTCCamera_get_readout_speed(void *_self, double *readout_speed)
{
return AAOS_ENOTSUP;
}

static int 
USTCCamera_power_on(void *_self)
{
    struct USTCCamera *self = cast(USTCCamera(), _self);
    
    unsigned int (*DetectorPowerOn)(void) = dlsym(self->dlh, "DetectorPowerOn");
    int ret;

    Pthread_mutex_lock(&self->_.d_state.mtx);
    if (!(self->_.d_state.state&DETECTOR_STATE_OFFLINE)) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        return AAOS_OK;
    } else if (self->_.d_state.state&DETECTOR_STATE_UNINITIALIZED) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        return AAOS_EUNINIT;
    }
    Pthread_mutex_lock(&self->mtx);
    ret = DetectorPowerOn();
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        self->_.d_state.state = DETECTOR_STATE_IDLE;
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    return ustc_error_mapping(ret);
}

static int 
USTCCamera_power_off(void *_self)
{
    struct USTCCamera *self = cast(USTCCamera(), _self);
    
    unsigned int (*DetectorPowerOff)(void) = dlsym(self->dlh, "DetectorPowerOff");
    int ret;
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    if (self->_.d_state.state&DETECTOR_STATE_OFFLINE) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        return AAOS_OK;
    } else if (self->_.d_state.state&DETECTOR_STATE_UNINITIALIZED) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        return AAOS_EUNINIT;
    }
    Pthread_mutex_lock(&self->mtx);
    ret = DetectorPowerOff();
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        self->_.d_state.state |= DETECTOR_STATE_OFFLINE;
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    return ustc_error_mapping(ret);
}

static int
USTCCamera_init(void *_self)
{
    struct USTCCamera *self = cast(USTCCamera(), _self);
    
    int cam_num, chipnum = 0, width = 0, height = 0;
    float high = -1., low = -1.;
    int ret;
    
    unsigned int (*ListCameras) (int *) = dlsym(self->dlh, "ListCameras");
    unsigned int (*Initialize) (unsigned int , unsigned int) = dlsym(self->dlh, "Initialize");
    unsigned int (*DetectorPowerON) (void) = dlsym(self->dlh, "DetectorPowerON");
    unsigned int (*GetChipNum) (int *) = dlsym(self->dlh, "GetChipNum");
    unsigned int (*GetDetector) (int *, int *) = dlsym(self->dlh, "GetDetector");
    unsigned int (*GetGain) (float *, float *) = dlsym(self->dlh, "GetGain");
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    if (!(self->_.d_state.state&DETECTOR_STATE_UNINITIALIZED)) {
        ret = USTC_CCD_SUCCESS;
        goto power_on;
    }
    Pthread_mutex_lock(&self->mtx);
    ret = ListCameras(&cam_num);
    Pthread_mutex_unlock(&self->mtx);
    if (ret != USTC_CCD_SUCCESS) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        goto error;
    }
    Pthread_mutex_lock(&self->mtx);
    ret = Initialize(self->log_level, self->which);
    Pthread_mutex_lock(&self->mtx);
    if (ret != USTC_CCD_SUCCESS) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        goto error;
    }
    self->_.d_state.state &= ~DETECTOR_STATE_UNINITIALIZED;
power_on:
    if (!(self->_.d_state.state&DETECTOR_STATE_OFFLINE)) {
        ret = USTC_CCD_SUCCESS;
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        goto error;
    }
    Pthread_mutex_lock(&self->mtx);
    ret = DetectorPowerON();
    Pthread_mutex_lock(&self->mtx);
    self->_.d_state.state &= ~DETECTOR_STATE_OFFLINE;
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    self->_.d_cap.binning_available = TRUE;
    self->_.d_cap.cooling_available = TRUE;
    self->_.d_cap.exposure_time_available = TRUE;
    self->_.d_cap.frame_rate_available = TRUE;
    self->_.d_cap.gain_available = TRUE;
    self->_.d_cap.gain_available = TRUE;
    self->_.d_cap.n_gain = 2;
    self->_.d_cap.gain_array = (double *) Malloc(2 * sizeof(double));

    Pthread_mutex_lock(&self->mtx);
    if ((ret = GetChipNum(&chipnum)) != USTC_CCD_SUCCESS) {
        Pthread_mutex_unlock(&self->mtx);
        goto error;
    }
    self->_.d_cap.n_chip = chipnum;
    if ((ret = GetDetector(&width, &height)) != USTC_CCD_SUCCESS) {
        Pthread_mutex_unlock(&self->mtx);
        goto error;
    }
    self->_.d_cap.width = width;
    self->_.d_cap.height = height;
    if ((ret = GetGain(&high, &low)) != USTC_CCD_SUCCESS) {
        Pthread_mutex_unlock(&self->mtx);
        goto error;
    }
    self->_.d_cap.gain_array[0] = low;
    self->_.d_cap.gain_array[1] = high;
    Pthread_mutex_unlock(&self->mtx);
    
error:
    return ustc_error_mapping(ret);
}

struct USTCCameraFrameProcess {
    struct USTCCamera *camera;
    unsigned char *buf;
    char *json_string;
    void *rpc;
    struct timespec tp;
    size_t nth;
    size_t n;
};

void static
USTCCamera_name_convention(struct USTCCamera *self, char *buf, size_t size, ...)
{
    FILE *fp;
    char time_buf[TIMESTAMPSIZE];
    struct timespec tp;
    struct tm tm_buf;
    size_t i, n;
    va_list ap;
    
    va_start(ap, size);
    i = va_arg(ap, size_t);
    n = va_arg(ap, size_t);
    va_end(ap);
    
    fp = fmemopen(buf, size, "rw+");
    if (self->_.d_proc.image_directory != NULL) {
        fprintf(fp, "%s/", self->_.d_proc.image_directory);
    }
    if (self->_.d_proc.image_prefix != NULL) {
        fprintf(fp, "%s_", self->_.d_proc.image_prefix);
    }
    if (self->_.name != NULL) {
        fprintf(fp, "%s_", self->_.name);
    }
    Clock_gettime(CLOCK_REALTIME, &tp);
    gmtime_r(&tp.tv_sec, &tm_buf);
    if (n == 1) {
        strftime(time_buf, TIMESTAMPSIZE, "%Y%m%d%H%M%S.fits", &tm_buf);
        fprintf(fp, "%s", time_buf);
    } else {
        strftime(time_buf, TIMESTAMPSIZE, "%Y%m%d%H%M%S", &tm_buf);
        fprintf(fp, "%s_%04lu.fits", time_buf, i + 1);
    }
    
    fclose(fp);
}

void static
USTCCamera_post_acquisition(struct USTCCamera *self, const char *pathname, ...)
{
    va_list ap;
    
    va_start(ap, pathname);
    void *rpc = va_arg(ap, void *);
    va_end(ap);
    if (rpc != NULL) {
        protobuf_set(rpc, PACKET_LENGTH, pathname, strlen(pathname) + 1);
        rpc_write(rpc);
    }
}

void static
USTCCamera_write_image(struct USTCCamera *self, void *buf, struct timespec *tp, size_t nth, size_t n_frame, char *string, void *rpc)
{
    char pathname[PATHSIZE], date_obs[TIMESTAMPSIZE], time_obs[TIMESTAMPSIZE], *s;
    struct tm time_buf;
    fitsfile *fptr;
    int status = 0;
    int naxis = 2;
    long naxes[2], nelements;
    unsigned short *array = buf;
    double gain, settemp = 9999.00;
    size_t i, n_chip = self->_.d_cap.n_chip;
    
    gmtime_r(&tp->tv_sec, &time_buf);
    strftime(date_obs, TIMESTAMPSIZE, "%Y-%m-%d", &time_buf);
    strftime(time_obs, TIMESTAMPSIZE, "%H:%M:%S", &time_buf);
    s = time_obs + strlen(time_obs);
    snprintf(s, TIMESTAMPSIZE - strlen(time_obs), "%06d", (int) tp->tv_nsec / 1000);
    
    naxes[0] = self->_.d_param.image_width;
    naxes[1] = self->_.d_param.image_height;
    if (self->_.d_proc.name_convention == NULL) {
        USTCCamera_name_convention(self, pathname, PATHSIZE, nth, n_frame);
    } else {
        self->_.d_proc.name_convention(self, pathname, PATHSIZE, nth, n_frame);
    }
    
    nelements = naxes[0] * naxes[1];
    fits_create_template(&fptr, pathname, self->_.d_proc.tpl_filename, &status);
    
    //fits_update_key_lng(fptr, "NEXTEND", 3, "Number of standard extensions", &status);
    fits_update_key_longstr(fptr, "DATE-OBS", date_obs, NULL, &status);
    fits_update_key_longstr(fptr, "TIME-OBS", time_obs, NULL, &status);
    USTCCamera_get_gain(self, &gain);
    fits_update_key_dbl(fptr, "GAIN", gain, 3, NULL, &status);
    Pthread_mutex_lock(&self->_.d_state.mtx);
    settemp = self->_.d_param.temperature;
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    fits_update_key_dbl(fptr, "SETTEMP", settemp, 2, NULL, &status);
    
    double controller_temperature = 9999.00, cool_temp = 9999.00, en_temp = 9999.0, power = -1.00, press = -1.00, hot_temp = 9999.00, motor_temperature = 9999.00;
    uint8_t coolstat = 255, speed = 255, heatpwm = 255;
    float cur2 = -1.00, vol2 = -1.00, temp = 9999.00;
    
    Pthread_mutex_lock(&self->mtx);
    unsigned int (*Controller_temperature)(double *) = dlsym(self->dlh, "Controller_temperature");
    unsigned int (*Coolertemp)(double *) = dlsym(self->dlh, "Coolertemp");
    unsigned int (*entemp)(double *) = dlsym(self->dlh, "entemp");
    unsigned int (*get_power)(double *) = dlsym(self->dlh, "get_power");
    unsigned int (*GetCoolerStatus)(uint8_t *) = dlsym(self->dlh, "GetCoolerStatus");
    unsigned int (*GetCurrentByChannel)(int, float *) = dlsym(self->dlh, "GetCurrentByChannel");
    unsigned int (*GetFanSpeed)(uint8_t *) = dlsym(self->dlh, "GetFanSpeed");
    unsigned int (*GetHeatPWM)(uint8_t *) = dlsym(self->dlh, "GetHeatPWM");
    unsigned int (*GetTemperature)(const char *, float *) = dlsym(self->dlh, "GetTemperature");
    unsigned int (*GetVacuum)(int, double *) = dlsym(self->dlh, "GetVacuum");
    unsigned int (*GetVoltageByChannel)(int, float *) = dlsym(self->dlh, "GetVoltageByChannel");
    unsigned int (*hottemp)(double *) = dlsym(self->dlh, "hottemp");
    unsigned int (*Motor_temperature)(double *) = dlsym(self->dlh, "Motor_temperature");
    
    Controller_temperature(&controller_temperature);
    fits_update_key_dbl(fptr, "CTRLTEMP", controller_temperature, 2, NULL, &status);
    Coolertemp(&cool_temp);
    fits_update_key_dbl(fptr, "COOLTEMP", cool_temp, 2, NULL, &status);
    entemp(&en_temp);
    fits_update_key_dbl(fptr, "ENVTEMP", en_temp, 2, NULL, &status);
    get_power(&power);
    fits_update_key_dbl(fptr, "POWER", power, 2, NULL, &status);
    GetCoolerStatus(&coolstat);
    fits_update_key_lng(fptr, "COOLSTAT", coolstat, NULL, &status);
    GetCurrentByChannel(0, &cur2);
    cur2 = -1.00;
    fits_update_key_flt(fptr, "PB24V_I", cur2, 2, NULL, &status);
    cur2 = -1.00;
    GetCurrentByChannel(1, &cur2);
    fits_update_key_flt(fptr, "PB12V_I", cur2, 2, NULL, &status);
    cur2 = -1.00;
    GetCurrentByChannel(2, &cur2);
    fits_update_key_flt(fptr, "PB5V_I", cur2, 2, NULL, &status);
    cur2 = -1.00;
    GetCurrentByChannel(3, &cur2);
    fits_update_key_flt(fptr, "PB6V_I", cur2, 2, NULL, &status);
    GetFanSpeed(&speed);
    fits_update_key_lng(fptr, "FANSPEED", speed, NULL, &status);
    GetHeatPWM(&heatpwm);
    fits_update_key_lng(fptr, "HEATPWM", heatpwm, NULL, &status);
    GetTemperature("D1", &temp);
    fits_update_key_flt(fptr, "D1TEMP", temp, 2, NULL, &status);
    temp = 9999.00;
    GetTemperature("D2", &temp);
    fits_update_key_flt(fptr, "D2TEMP", temp, 2, NULL, &status);
    temp = 9999.00;
    GetTemperature("PT1", &temp);
    fits_update_key_flt(fptr, "PT1TEMP", temp, 2, NULL, &status);
    temp = 9999.00;
    GetTemperature("PT2", &temp);
    fits_update_key_flt(fptr, "PT2TEMP", temp, 2, NULL, &status);
    temp = 9999.00;
    GetTemperature("PT3", &temp);
    fits_update_key_flt(fptr, "PT3TEMP", temp, 2, NULL, &status);
    temp = 9999.00;
    GetTemperature("PT4", &temp);
    fits_update_key_flt(fptr, "PT4TEMP", temp, 2, NULL, &status);
    GetVacuum(1, &press);
    fits_update_key_dbl(fptr, "PR1", press, 2, NULL, &status);
    press = -1.00;
    GetVacuum(2, &press);
    fits_update_key_dbl(fptr, "PR2", press, 2, NULL, &status);
    press = -1.00;
    GetVacuum(3, &press);
    fits_update_key_dbl(fptr, "PR3", press, 2, NULL, &status);
    press = -1.00;
    GetVacuum(4, &press);
    fits_update_key_dbl(fptr, "PR4", press, 2, NULL, &status);
    press = -1.00;
    GetVacuum(5, &press);
    fits_update_key_dbl(fptr, "PR5", press, 2, NULL, &status);
    GetVoltageByChannel(0, &vol2);
    fits_update_key_flt(fptr, "PB24V_V", vol2, 2, NULL, &status);
    vol2 = -1.00;
    GetVoltageByChannel(1, &vol2);
    fits_update_key_flt(fptr, "PB12V_V", vol2, 2, NULL, &status);
    vol2 = -1.00;
    GetVoltageByChannel(2, &vol2);
    fits_update_key_flt(fptr, "PB5V_V", vol2, 2, NULL, &status);
    vol2 = -1.00;
    GetVoltageByChannel(3, &vol2);
    fits_update_key_flt(fptr, "PB6V_V", vol2, 2, NULL, &status);
    hottemp(&hot_temp);
    fits_update_key_dbl(fptr, "HOTTEMP", hot_temp, 2, NULL, &status);
    Motor_temperature(&motor_temperature);
    fits_update_key_dbl(fptr, "MTRREMP", motor_temperature, 2, NULL, &status);
    Pthread_mutex_unlock(&self->mtx);
    
    if (string != NULL) {
        
    }
    
    for (i = 0; i < n_chip; i++) {
        fits_create_img(fptr, USHORT_IMG, naxis, naxes, &status);
        fits_update_key_str(fptr, "EXTNAME", "raw", "extension name", &status);
        fits_update_key_lng(fptr, "EXTVER", i + 1, "extension version", &status);
        fits_write_img(fptr, TSHORT, 1, nelements, array + i * nelements, &status);
    }
    fits_close_file(fptr, &status);
    
    if (self->_.d_proc.post_acquisition == NULL) {
        USTCCamera_post_acquisition(self, pathname, rpc);
    } else {
        self->_.d_proc.post_acquisition(self, pathname, rpc);
    }
}

static void *
USTCCamera_image_process(void *arg)
{
    struct USTCCamera *self = ((struct USTCCameraFrameProcess *) arg)->camera;
    unsigned char *buf = ((struct USTCCameraFrameProcess *) arg)->buf;
    char *string = ((struct USTCCameraFrameProcess *) arg)->json_string;
    struct timespec *tp = &((struct USTCCameraFrameProcess *) arg)->tp;
    size_t i = ((struct USTCCameraFrameProcess *) arg)->nth, n = ((struct USTCCameraFrameProcess *) arg)->n;
    void *rpc = ((struct USTCCameraFrameProcess *) arg)->rpc;
    USTCCamera_write_image(self, buf, tp, i, n, string, rpc);
    free(buf);
    free(arg);
    if (i == n - 1) {
        free(string);
    }
    
    return NULL;
}

static int
USTCCamera_expose(void *_self, double exposure_time, uint32_t n_frame, va_list *app)
{
    struct USTCCamera *self = cast(USTCCamera(), _self);
    
    double frame_rate;
    uint32_t i, j, n;
    unsigned char *buf;
    pthread_t *tids;
    int ret = AAOS_OK;
    void *rpc = va_arg(*app, char *);
    char *json_string = va_arg(*app, char *);
    unsigned int (*SetExposureInterval)(double) = dlsym(self->dlh, "SetExposureInterval");
    unsigned int (*SetExposureTime)(double)= dlsym(self->dlh, "SetExposureTime");
    unsigned int (*SetContinuousCapture)(uint16_t) = dlsym(self->dlh, "SetContinuousCapture");
    unsigned int (*StartExposure)(void) = dlsym(self->dlh, "StartExposure");
    unsigned int (*StopExposure)(void) = dlsym(self->dlh, "StopExposure");
    unsigned int (*GetImage)(void *, int) = dlsym(self->dlh, "GetImage");
    unsigned int (*WaitForAcquisition)(void) = dlsym(self->dlh, "WaitForAcquisition");
    unsigned int (*SetEraseCount)(uint8_t) = dlsym(self->dlh, "SetEraseCount");
    unsigned int (*GetAcquisitionStatus)(void) = dlsym(self->dlh, "GetAcquisitionStatus");
    
    if (exposure_time < 0.) {
        return AAOS_EINVAL;
    }
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    switch (self->_.d_state.state) {
        case DETECTOR_STATE_IDLE:
        {
            self->_.d_param.exposure_time = exposure_time;
            self->_.d_param.frame_rate = 1. / exposure_time;
            Pthread_mutex_lock(&self->mtx);
            ret = SetExposureInterval(exposure_time);
            ret = ustc_error_mapping(ret);
            if (ret != AAOS_OK) {
                goto error2;
            }
            ret = SetExposureTime(exposure_time);
            ret = ustc_error_mapping(ret);
            if (ret != AAOS_OK) {
                goto error2;
            }
            ret = SetContinuousCapture(n_frame);
            ret = ustc_error_mapping(ret);
            if (ret != AAOS_OK) {
                goto error2;
            }
            ret = SetEraseCount(self->erase_count);
            ret = ustc_error_mapping(ret);
            if (ret != AAOS_OK) {
                goto error2;
            }
            ret = StartExposure();
            ret = ustc_error_mapping(ret);
            if (ret != AAOS_OK) {
                goto error2;
            }
            Pthread_mutex_unlock(&self->mtx);
            self->_.d_state.state = DETECTOR_STATE_EXPOSING;
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            
            tids = (pthread_t *) Malloc(sizeof(pthread_t));
            for (i = 0; i < n_frame; i++) {
                Nanosleep(exposure_time);
                Pthread_mutex_lock(&self->mtx);
                ret = GetAcquisitionStatus();
                Pthread_mutex_unlock(&self->mtx);
                if (ret == USTC_CCD_SUCCESS) {
                    Pthread_mutex_lock(&self->_.d_state.mtx);
                    self->_.d_state.state = DETECTOR_STATE_EXPOSING;
                    Pthread_mutex_unlock(&self->_.d_state.mtx);
                    n = (uint32_t) (self->_.d_param.image_width * self->_.d_param.image_height * self->_.d_cap.n_chip);
                    buf = (unsigned char *) Malloc(n * 2);
                    Pthread_mutex_lock(&self->mtx);
                    ret = GetImage(buf, n * 2);
                    Pthread_mutex_unlock(&self->mtx);
                    if (ret == USTC_CCD_SUCCESS) {
                        struct USTCCameraFrameProcess *arg = (struct USTCCameraFrameProcess *) Malloc(sizeof(struct USTCCameraFrameProcess));
                        arg->camera = self;
                        arg->buf = buf;
                        arg->json_string = json_string;
                        arg->n = n_frame;
                        arg->nth = i;
                        arg->rpc = rpc;
                        Clock_gettime(CLOCK_REALTIME, &arg->tp);
                        Pthread_create(&tids[i], NULL, USTCCamera_image_process, (void *) arg);
                    } else {
                        Pthread_mutex_lock(&self->_.d_state.mtx);
                        self->_.d_state.state = DETECTOR_STATE_IDLE;
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        free(buf);
                        free(json_string);
                        for (j = 0; j < i; j++) {
                            Pthread_join(tids[j], NULL);
                        }
                        free(tids);
                        return AAOS_ECANCELED;
                    }
                } else if (ret == USTC_CCD_ACQURING) {
                    Pthread_mutex_lock(&self->_.d_state.mtx);
                    self->_.d_state.state = DETECTOR_STATE_READING;
                    Pthread_mutex_unlock(&self->_.d_state.mtx);
                    Pthread_mutex_lock(&self->mtx);
                    ret = WaitForAcquisition();
                    Pthread_mutex_unlock(&self->mtx);
                    if (ret == USTC_CCD_SUCCESS) {
                        Pthread_mutex_lock(&self->_.d_state.mtx);
                        self->_.d_state.state = DETECTOR_STATE_EXPOSING;
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        n = (uint32_t) (self->_.d_param.image_width * self->_.d_param.image_height * self->_.d_cap.n_chip);
                        buf = (unsigned char *) Malloc(n * 2);
                        Pthread_mutex_lock(&self->mtx);
                        ret = GetImage(buf, n * 2);
                        Pthread_mutex_unlock(&self->mtx);
                        if (ret == USTC_CCD_SUCCESS) {
                            struct USTCCameraFrameProcess *arg = (struct USTCCameraFrameProcess *) Malloc(sizeof(struct USTCCameraFrameProcess));
                            arg->camera = self;
                            arg->buf = buf;
                            arg->json_string = json_string;
                            arg->nth = i;
                            arg->n = n_frame;
                            arg->rpc = rpc;
                            Clock_gettime(CLOCK_REALTIME, &arg->tp);
                            Pthread_create(&tids[i], NULL, USTCCamera_image_process, (void *) arg);
                        } else {
                            Pthread_mutex_lock(&self->_.d_state.mtx);
                            self->_.d_state.state = DETECTOR_STATE_IDLE;
                            Pthread_mutex_unlock(&self->_.d_state.mtx);
                            free(buf);
                            free(json_string);
                            for (j = 0; j < i; j++) {
                                Pthread_join(tids[j], NULL);
                            }
                            free(tids);
                            return AAOS_ECANCELED;
                        }
                    } else {
                        Pthread_mutex_lock(&self->_.d_state.mtx);
                        self->_.d_state.state = DETECTOR_STATE_IDLE;
                        Pthread_mutex_unlock(&self->_.d_state.mtx);
                        free(json_string);
                        free(tids);
                        return AAOS_ECANCELED;
                    }
                } else {
                    Pthread_mutex_lock(&self->_.d_state.mtx);
                    self->_.d_state.state = DETECTOR_STATE_IDLE;
                    Pthread_mutex_unlock(&self->_.d_state.mtx);
                    free(json_string);
                    free(tids);
                    return AAOS_ECANCELED;
                }
            }
            for (i = 0; i < n_frame; i++) {
                Pthread_join(tids[i], NULL);
            }
            Pthread_mutex_lock(&self->_.d_state.mtx);
            Pthread_mutex_lock(&self->mtx);
            ret = StopExposure();
            Pthread_mutex_unlock(&self->mtx);
            self->_.d_state.state = DETECTOR_STATE_IDLE;
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            ret = ustc_error_mapping(ret);
            return ret;
        }
            break;
        case DETECTOR_STATE_UNINITIALIZED:
            ret = AAOS_EUNINIT;
            goto error;
            break;
        case DETECTOR_STATE_MALFUNCTION:
            ret = AAOS_EDEVMAL;
            goto error;
        case DETECTOR_STATE_READING:
        case DETECTOR_STATE_EXPOSING:
            ret = AAOS_EBUSY;
            goto error;
        default:
            ret = AAOS_ERROR;
            goto error;
            break;
    }
error2:
    Pthread_mutex_unlock(&self->mtx);
error:
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    return ret;
}

static int
USTCCamera_status(void *_self, void *res, size_t res_size, size_t *res_len)
{
    struct USTCCamera *self = cast(USTCCamera(), _self);
    
    cJSON *root_json, *detector_json, *dewars_json, *pump_json, *electronics_json;
    char *json_string = NULL;
    uint8_t readystat, coolstat, speed, heatpwm;
    uint16_t rowStartNum, rowKeepNum, colStartNum, colKeepNum;
    int chan;
    double running_time, power, temperature, en_temp, hot_temp, press, cur, vol;
    float vol2, cur2, temp;
    int ret;
    
    unsigned int (*GetAcquisitionStatus)(void) = dlsym(self->dlh, "GetAcquisitionStatus");
    unsigned int (*GetCameraReady)(uint8_t *) = dlsym(self->dlh, "GetCameraReady");
    unsigned int (*Runningtime)(double *) = dlsym(self->dlh, "Runningtime");
    unsigned int (*GetExposureROI)(uint16_t *, uint16_t *, uint16_t *, uint16_t *) = dlsym(self->dlh, "GetExposureROI");
    unsigned int (*GetCoolerStatus)(uint8_t *) = dlsym(self->dlh, "GetCoolerStatus");
    unsigned int (*get_power)(double *) = dlsym(self->dlh, "get_power");
    unsigned int (*Controller_temperature)(double *) = dlsym(self->dlh, "Controller_temperature");
    unsigned int (*entemp)(double *) = dlsym(self->dlh, "entemp");
    unsigned int (*hottemp)(double *) = dlsym(self->dlh, "hottemp");
    unsigned int (*GetVacuum)(int, double *) = dlsym(self->dlh, "ControllerHeat");
    unsigned int (*GetPumpVol)(double *) = dlsym(self->dlh, "GetPumpVol");
    unsigned int (*GetPumpCur)(double *) = dlsym(self->dlh, "GetPumpCur");
    unsigned int (*GetPumpTargetVol)(double *) = dlsym(self->dlh, "GetPumpTargetVol");
    unsigned int (*GetCurrentByChannel)(int, float *) = dlsym(self->dlh, "GetCurrentByChannel");
    unsigned int (*GetVoltageByChannel)(int, float *) = dlsym(self->dlh, "GetVoltageByChannel");
    unsigned int (*GetFanSpeed)(uint8_t *) = dlsym(self->dlh, "GetFanSpeed");
    unsigned int (*GetHeatPWM)(uint8_t *) = dlsym(self->dlh, "GetHeatPWM");
    unsigned int (*GetTemperature)(const char *, float *) = dlsym(self->dlh, "GetTemperature");
    
    root_json = cJSON_CreateObject();
    
    detector_json = cJSON_CreateObject();
    cJSON_AddItemToObject(root_json, "Detector", detector_json);
    Pthread_mutex_lock(&self->mtx);
    ret = GetAcquisitionStatus();
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_ACQURING) {
        cJSON_AddStringToObject(detector_json, "STATE", "EXPOSING");
    } else {
        unsigned int state;
        Pthread_mutex_lock(&self->_.d_state.mtx);
        state = self->_.d_state.state;
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        switch (state) {
            case DETECTOR_STATE_IDLE:
                cJSON_AddStringToObject(detector_json, "STATE", "IDLE");
                break;
            case DETECTOR_STATE_OFFLINE:
                cJSON_AddStringToObject(detector_json, "STATE", "POWER OFF");
                break;
            case DETECTOR_STATE_MALFUNCTION:
                cJSON_AddStringToObject(detector_json, "STATE", "MALFUNCTION");
                goto error;
                break;
            case DETECTOR_STATE_UNINITIALIZED:
                cJSON_AddStringToObject(detector_json, "STATE", "UNINITIALIZED");
                goto error;
                break;
            case DETECTOR_STATE_READING:
                cJSON_AddStringToObject(detector_json, "STATE", "READING");
                break;
            default:
                cJSON_AddStringToObject(detector_json, "STATE", "UNKNOWN");
                goto error;
                break;
        }
    }
    Pthread_mutex_lock(&self->mtx);
    ret = GetCameraReady(&readystat);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(detector_json, "ReadyState", readystat);
    } else {
        cJSON_AddNumberToObject(detector_json, "ReadyState", -1);
    }
    Pthread_mutex_lock(&self->mtx);
    ret = Runningtime(&running_time);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(detector_json, "RunningTime", running_time);
    } else {
        cJSON_AddNumberToObject(detector_json, "RunningTime", -1);
    }
    Pthread_mutex_lock(&self->mtx);
    ret = GetExposureROI(&rowStartNum, &rowKeepNum, &colStartNum, &colKeepNum);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(detector_json, "RIO-X", rowStartNum);
        cJSON_AddNumberToObject(detector_json, "RIO-Y", colStartNum);
        cJSON_AddNumberToObject(detector_json, "RIO-W", rowKeepNum);
        cJSON_AddNumberToObject(detector_json, "RIO-H", colKeepNum);
    } else {
        cJSON_AddNumberToObject(detector_json, "RIO-X", -1);
        cJSON_AddNumberToObject(detector_json, "RIO-Y", -1);
        cJSON_AddNumberToObject(detector_json, "RIO-W", -1);
        cJSON_AddNumberToObject(detector_json, "RIO-H", -1);
    }

    dewars_json = cJSON_CreateObject();
    cJSON_AddItemToObject(root_json, "Dewars", dewars_json);
    Pthread_mutex_lock(&self->mtx);
    ret = GetCoolerStatus(&coolstat);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(dewars_json, "CoolerStatus", coolstat);
    } else {
        cJSON_AddNumberToObject(dewars_json, "CoolerStatus", -1);
    }
    Pthread_mutex_lock(&self->mtx);
    ret = get_power(&power);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(dewars_json, "CoolerPower", power);
    } else {
        cJSON_AddNumberToObject(dewars_json, "CoolerPower", -1.);
    }
    Pthread_mutex_lock(&self->mtx);
    ret = Controller_temperature(&temperature);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(dewars_json, "ControllerTemp", temperature);
    } else {
        cJSON_AddNumberToObject(dewars_json, "ControllerTemp", 9999.0);
    }
    Pthread_mutex_lock(&self->mtx);
    ret = entemp(&en_temp);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(dewars_json, "AmbientTemp", en_temp);
    } else {
        cJSON_AddNumberToObject(dewars_json, "AmbientTemp", 9999.0);
    }
    Pthread_mutex_lock(&self->mtx);
    ret = hottemp(&hot_temp);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(dewars_json, "HotEndTemp", hot_temp);
    } else {
        cJSON_AddNumberToObject(dewars_json, "HotEndTemp", 9999.0);
    }
    Pthread_mutex_lock(&self->mtx);
    Pthread_mutex_lock(&self->mtx);
    ret = GetFanSpeed(&speed);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(dewars_json, "FanSpeed", speed);
    } else {
        cJSON_AddNumberToObject(dewars_json, "FanSpeed", -1);
    }
    Pthread_mutex_lock(&self->mtx);
    ret = GetHeatPWM(&heatpwm);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(dewars_json, "HeatPWM", heatpwm);
    } else {
        cJSON_AddNumberToObject(dewars_json, "HeatPWM", -1);
    }
    
    pump_json = cJSON_CreateObject();
    cJSON_AddItemToObject(root_json, "Pump", pump_json);
    Pthread_mutex_lock(&self->mtx);
    ret = GetVacuum(1, &press);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(pump_json, "PR1", press);
    } else {
        cJSON_AddNumberToObject(pump_json, "PR1", -1.);
    }
    Pthread_mutex_lock(&self->mtx);
    ret = GetVacuum(2, &press);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(pump_json, "PR2", press);
    } else {
        cJSON_AddNumberToObject(pump_json, "PR2", -1.);
    }
    Pthread_mutex_lock(&self->mtx);
    ret = GetVacuum(3, &press);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(pump_json, "PR3", press);
    } else {
        cJSON_AddNumberToObject(pump_json, "PR3", -1.);
    }
    Pthread_mutex_lock(&self->mtx);
    ret = GetVacuum(4, &press);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(pump_json, "PR4", press);
    } else {
        cJSON_AddNumberToObject(pump_json, "PR4", -1.);
    }
    Pthread_mutex_lock(&self->mtx);
    ret = GetVacuum(5, &press);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(pump_json, "PR5", press);
    } else {
        cJSON_AddNumberToObject(pump_json, "PR5", -1.);
    }
    Pthread_mutex_lock(&self->mtx);
    ret = GetPumpVol(&vol);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(pump_json, "PumpVoltage", vol);
    } else {
        cJSON_AddNumberToObject(pump_json, "PumpVoltage", -1.);
    }
    Pthread_mutex_lock(&self->mtx);
    ret = GetPumpCur(&cur);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(pump_json, "PumpCurrent", cur);
    } else {
        cJSON_AddNumberToObject(pump_json, "PumpCurrent", -1.);
    }
    Pthread_mutex_lock(&self->mtx);
    ret = GetPumpTargetVol(&vol);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(pump_json, "PumpTargetVoltage", vol);
    } else {
        cJSON_AddNumberToObject(pump_json, "PumpTargetVoltage", -1.);
    }
    
    electronics_json = cJSON_CreateObject();
    cJSON_AddItemToObject(root_json, "Elesctronics", pump_json);
    Pthread_mutex_lock(&self->mtx);
    ret = GetVoltageByChannel(0, &vol2);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(electronics_json, "PB24V Voltage", vol2);
    } else {
        cJSON_AddNumberToObject(electronics_json, "PB24V Voltage", -1.);
    }
    Pthread_mutex_lock(&self->mtx);
    ret = GetCurrentByChannel(0, &cur2);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(electronics_json, "PB24V Current", cur2);
    } else {
        cJSON_AddNumberToObject(electronics_json, "PB24V Current", -1.);
    }
    Pthread_mutex_lock(&self->mtx);
    ret = GetVoltageByChannel(1, &vol2);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(electronics_json, "PB12V Voltage", vol2);
    } else {
        cJSON_AddNumberToObject(electronics_json, "PB12V Voltage", -1.);
    }
    Pthread_mutex_lock(&self->mtx);
    ret = GetCurrentByChannel(1, &cur2);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(electronics_json, "PB12V Current", cur2);
    } else {
        cJSON_AddNumberToObject(electronics_json, "PB12V Current", -1.);
    }
    Pthread_mutex_lock(&self->mtx);
    ret = GetVoltageByChannel(2, &vol2);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(electronics_json, "PB5V Voltage", vol2);
    } else {
        cJSON_AddNumberToObject(electronics_json, "PB5V Voltage", -1.);
    }
    Pthread_mutex_lock(&self->mtx);
    ret = GetCurrentByChannel(2, &cur2);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(electronics_json, "PB5V Current", cur2);
    } else {
        cJSON_AddNumberToObject(electronics_json, "PB5V Current", -1.);
    }
    Pthread_mutex_lock(&self->mtx);
    ret = GetVoltageByChannel(3, &vol2);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(electronics_json, "PB6V Voltage", vol2);
    } else {
        cJSON_AddNumberToObject(electronics_json, "PB6V Voltage", -1.);
    }
    Pthread_mutex_lock(&self->mtx);
    ret = GetCurrentByChannel(3, &cur2);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(electronics_json, "PB6V Current", cur2);
    } else {
        cJSON_AddNumberToObject(electronics_json, "PB6V Current", -1.);
    }
    Pthread_mutex_lock(&self->mtx);
    ret = GetTemperature("D1", &temp);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(electronics_json, "D1Temp", temp - 273.15);
    } else {
        cJSON_AddNumberToObject(electronics_json, "D1Temp", 9999.0);
    }
    Pthread_mutex_lock(&self->mtx);
    ret = GetTemperature("D2", &temp);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(electronics_json, "D2Temp", temp - 273.15);
    } else {
        cJSON_AddNumberToObject(electronics_json, "D2Temp", 9999.0);
    }
    Pthread_mutex_lock(&self->mtx);
    ret = GetTemperature("PT1", &temp);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(electronics_json, "PT1", temp - 273.15);
    } else {
        cJSON_AddNumberToObject(electronics_json, "PT1", 9999.0);
    }
    Pthread_mutex_lock(&self->mtx);
    ret = GetTemperature("PT2", &temp);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(electronics_json, "PT2", temp - 273.15);
    } else {
        cJSON_AddNumberToObject(electronics_json, "PT2", 9999.0);
    }
    Pthread_mutex_lock(&self->mtx);
    ret = GetTemperature("PT3", &temp);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(electronics_json, "PT3", temp - 273.15);
    } else {
        cJSON_AddNumberToObject(electronics_json, "PT3", 9999.0);
    }
    Pthread_mutex_lock(&self->mtx);
    ret = GetTemperature("PT4", &temp);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(electronics_json, "PT4", temp - 273.15);
    } else {
        cJSON_AddNumberToObject(electronics_json, "PT4", 9999.0);
    }

error:
    json_string = cJSON_Print(root_json);
    cJSON_Delete(root_json);
    snprintf(res, res_size, "%s", json_string);
    if (res_len != NULL) {
        *res_len = strlen(res) + 1;
    }
    free(json_string);
    return AAOS_OK;
}

static int
USTCCamera_info(void *_self, void *res, size_t res_size, size_t *res_len)
{
    struct USTCCamera *self = cast(USTCCamera(), _self);
    
    cJSON *root_json;
    char *json_string = NULL;
    PIXELX_SERIAL serial_no;
    int chipnum, width, height;
    
    int ret;
    
    unsigned int(*GetSerialNumber)(PIXELX_SERIAL *) = dlsym(self->dlh, "GetSerialNumber");
    unsigned int(*GetChipNum)(int *) = dlsym(self->dlh, "GetChipNum");
    unsigned int(*GetDetector)(int *, int *) = dlsym(self->dlh, "GetDetector");
    
    root_json = cJSON_CreateObject();
    cJSON_AddStringToObject(root_json, "Name", self->_.name);
    if (self->_.description != NULL) {
        cJSON_AddStringToObject(root_json, "Description", self->_.description);
    }
    
    Pthread_mutex_lock(&self->mtx);
    ret = GetSerialNumber(&serial_no);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        char buf[TIMESTAMPSIZE];
        cJSON_AddNumberToObject(root_json, "Model_Enum", serial_no.Model_Enum);
        cJSON_AddNumberToObject(root_json, "MB_ID", serial_no.MB_ID);
        cJSON_AddNumberToObject(root_json, "version", serial_no.version);
        cJSON_AddNumberToObject(root_json, "version_aa", serial_no.version_aa);
        cJSON_AddNumberToObject(root_json, "ID", serial_no.id);
        cJSON_AddNumberToObject(root_json, "Checksum", serial_no.checksum);
        snprintf(buf, TIMESTAMPSIZE, "%04u-%02u-%02u %02u", serial_no.Year, serial_no.Month, serial_no.Day, serial_no.Hour);
        cJSON_AddStringToObject(root_json, "Date", buf);
        
    }
    Pthread_mutex_lock(&self->mtx);
    ret = GetChipNum(&chipnum);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(root_json, "ChipNumber", chipnum);
    } else {
        cJSON_AddNumberToObject(root_json, "ChipNumber", 1);
    }
    Pthread_mutex_lock(&self->mtx);
    ret = GetDetector(&width, &height);
    Pthread_mutex_unlock(&self->mtx);
    if (ret == USTC_CCD_SUCCESS) {
        cJSON_AddNumberToObject(root_json, "Width", width);
        cJSON_AddNumberToObject(root_json, "Height", height);
    } else {
        cJSON_AddNumberToObject(root_json, "Width", self->_.d_cap.width);
        cJSON_AddNumberToObject(root_json, "Height", self->_.d_cap.height);
    }
    
error:
    json_string = cJSON_Print(root_json);
    cJSON_Delete(root_json);
    snprintf(res, res_size, "%s", json_string);
    if (res_len != NULL) {
        *res_len = strlen(res) + 1;
    }
    cJSON_Delete(root_json);
    free(json_string);
    return AAOS_OK;
}

static const void *_ustc_camera_virtual_table;

static void
ustc_camera_virtual_table_destroy(void)
{
    delete((void *) _ustc_camera_virtual_table);
}

static void
ustc_camera_virtual_table_initialize(void)
{
    _ustc_camera_virtual_table = new(__DetectorVirtualTable(),
                                     __detector_init, "init", USTCCamera_init,
                                     __detector_info, "info", USTCCamera_info,
                                     __detector_status, "status", USTCCamera_status,
                                     __detector_expose, "expose", USTCCamera_expose,
                                     __detector_power_on, "power_on", USTCCamera_power_on,
                                     __detector_power_off, "power_off", USTCCamera_power_off,
                                     //__detector_set_binning, "set_binning", USTCCamera_set_binning,
                                     //__detector_get_binning, "get_binning", USTCCamera_get_binning,
                                     __detector_set_exposure_time, "set_exposure_time", USTCCamera_set_exposure_time,
                                     __detector_get_exposure_time, "get_exposure_time", USTCCamera_get_exposure_time,
                                     __detector_set_frame_rate, "set_frame_rate", USTCCamera_set_frame_rate,
                                     __detector_get_frame_rate, "get_frame_rate", USTCCamera_get_frame_rate,
                                     __detector_set_gain, "set_gain", USTCCamera_set_gain,
                                     __detector_get_gain, "get_gain", USTCCamera_get_gain,
                                     __detector_set_region, "set_region", USTCCamera_set_region,
                                     __detector_get_region, "get_region", USTCCamera_get_region,
                                     __detector_raw, "raw", USTCCamera_raw,
                                     //__detector_inspect, "inspect", USTCCamera_inspect,
                                     //__detector_wait_for_completion, "wait_for_completion", USTCCamera_wait_for_last_completion,
                                     (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(ustc_camera_virtual_table_destroy);
#endif
}

static const void *
ustc_camera_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, ustc_camera_virtual_table_initialize);
#endif
    
    return _ustc_camera_virtual_table;
}

/*
 * ASI Camera
 * https://www.asiastro.cn
 */
#ifdef __USE_ASI_CAMERA__

#include "ASICamera2.h"

static int
asi_error_mapping(int asi_error)
{
    int ret = AAOS_ERROR;
    
    switch (asi_error) {
        case ASI_SUCCESS:
        case ASI_ERROR_INVALID_SEQUENCE:
            ret = AAOS_OK;
            break;
        case ASI_ERROR_INVALID_INDEX:
        case ASI_ERROR_INVALID_ID:
        case ASI_ERROR_INVALID_CONTROL_TYPE:
        case ASI_ERROR_INVALID_FILEFORMAT:
        case ASI_ERROR_INVALID_SIZE:
        case ASI_ERROR_OUTOF_BOUNDARY:
        case ASI_ERROR_INVALID_MODE:
            ret = AAOS_EINVAL;
            break;
        case ASI_ERROR_CAMERA_CLOSED:
            ret = AAOS_ECLOSED;
            break;
        case ASI_ERROR_CAMERA_REMOVED:
            ret = AAOS_ENOTFOUND;
            break;
        case ASI_ERROR_INVALID_PATH:
            ret = AAOS_ENOENT;
            break;
        case ASI_ERROR_INVALID_IMGTYPE:
        case ASI_ERROR_GPS_NOT_SUPPORTED:
            ret = AAOS_ENOTSUP;
            break;
        case ASI_ERROR_TIMEOUT:
            ret = AAOS_ETIMEDOUT;
            break;
        case ASI_ERROR_BUFFER_TOO_SMALL:
            ret = AAOS_ENOMEM;
            break;
        case ASI_ERROR_VIDEO_MODE_ACTIVE:
        case ASI_ERROR_EXPOSURE_IN_PROGRESS:
            ret = AAOS_EBUSY;
            break;
        default:
            break;
	}
    return ret;
}

void static
ASICamera_name_convention(void *_self, char *buf, size_t size, ...)
{
    struct __Detector *self = cast(__Detector(), _self);

    FILE *fp;
    static __thread char time_buf[TIMESTAMPSIZE];
    struct timespec *tp;
    struct tm tm_buf;
    size_t i, n;
    va_list ap;

    va_start(ap, size);
    i = va_arg(ap, size_t);
    n = va_arg(ap, size_t);
    tp = va_arg(ap, struct timespec *);
    va_end(ap);
    
    fp = fmemopen(buf, size, "w");
    if (self->d_proc.image_directory != NULL) {
        fprintf(fp, "%s/", self->d_proc.image_directory);
    }
    if (self->d_proc.image_prefix != NULL) {
        fprintf(fp, "%s_", self->d_proc.image_prefix);
    }
    if (self->name != NULL) {
        fprintf(fp, "%s_", self->name);
    }
    if (i == 1 || n == 1) {
        gmtime_r(&tp->tv_sec, &tm_buf);
        strftime(time_buf, TIMESTAMPSIZE, "%Y%m%d%H%M%S", &tm_buf);
    }
    if (n == 1) {
        fprintf(fp, "%s.fits", time_buf);
    } else {
        fprintf(fp, "%s_%04lu.fits", time_buf, i);
    }
    
    fclose(fp);
}

static int
ASICamera_pre_acquisition(void *_self, const char *filename, ...)
{
    struct __Detector *self = cast(__Detector(), _self);
    int ret = AAOS_OK, status = 0;
    struct timespec *tp;
    struct tm tm_buf;
    char buf[TIMESTAMPSIZE];
    fitsfile *fptr;
    
    va_list ap;
    va_start(ap, filename);
    fptr = va_arg(ap, fitsfile *);
    tp = va_arg(ap, struct timespec *);
    va_end(ap);
    
    gmtime_r(&tp->tv_sec, &tm_buf);
    strftime(buf, TIMESTAMPSIZE, "%Y-%m-%d", &tm_buf);
    fits_update_key_str(fptr, "DATE-OBS", buf, NULL, &status);
    strftime(buf, TIMESTAMPSIZE, "%H:%m:%d", &tm_buf);
    snprintf(buf + strlen(buf), TIMESTAMPSIZE - strlen(buf), ".%03d", (int) floor(tp->tv_nsec / 1000000));
    fits_update_key_str(fptr, "TIME-OBS", buf, NULL, &status);
    fits_update_key_log(fptr, "COOLING", self->d_param.is_cooling_enable, NULL, &status);
    fits_update_key_fixdbl(fptr, "COOLTEMP", self->d_param.temperature, 2, NULL, &status);
    
    return ret;
}

static const void *asi_camera_virtual_table(void);

static void *
ASICamera_ctor(void *_self, va_list *app)
{
    struct ASICamera *self = super_ctor(ASICamera(), _self, app);
    
    const char *s;

    self->_.d_state.state = (DETECTOR_STATE_OFFLINE|DETECTOR_STATE_UNINITIALIZED);
    self->camera_id = -1;
    self->camera_index = -1;
    s = va_arg(*app, const char *);
    self->so_path = (char *) Malloc(strlen(s) + 1);
    snprintf(self->so_path, strlen(s) + 1, "%s", s);
    Pthread_mutex_init(&self->mtx, NULL);
    Pthread_cond_init(&self->cond, NULL);
    self->dlh = dlopen(self->so_path, RTLD_LAZY | RTLD_LOCAL);
    self->capture_mode = DETECTOR_CAPTURE_MODE_VIDEO;
#ifdef DEBUG
    if (self->dlh == NULL) {
        fprintf(stderr, "%s %s %d: dlopen \"%s\" error.\n", __FILE__, __func__, __LINE__ - 3, self->so_path);
    }
#endif
    self->_._vtab= asi_camera_virtual_table();
    self->_.d_proc.name_convention = ASICamera_name_convention;
    self->_.d_proc.pre_acquisition = ASICamera_pre_acquisition;
    self->_.d_proc.post_acquisition = __Detector_default_post_acquisition;
    self->_.d_proc.queue = new(ThreadsafeQueue(), DetectorDataFrame_cleanup);
    
    return (void *) self;
}

static void *
ASICamera_dtor(void *_self)
{
    struct ASICamera *self = cast(ASICamera(), _self);
    
    Pthread_mutex_destroy(&self->mtx);
    Pthread_cond_destroy(&self->cond);
    free(self->so_path);
    dlclose(self->dlh);

    return super_dtor(ASICamera(), _self);
}

static void *
ASICameraClass_ctor(void *_self, va_list *app)
{
    struct ASICameraClass *self = super_ctor(ASICameraClass(), _self, app);
    
    self->_.raw.method = (Method) 0;
    self->_.init.method = (Method) 0;
    
    self->_.disable_cooling.method = (Method) 0;
    self->_.enable_cooling.method = (Method) 0;
    self->_.get_binning.method = (Method) 0;
    self->_.set_binning.method = (Method) 0;
    self->_.set_exposure_time.method = (Method) 0;
    self->_.get_exposure_time.method = (Method) 0;
    self->_.set_frame_rate.method = (Method) 0;
    self->_.get_frame_rate.method = (Method) 0;
    self->_.set_gain.method = (Method) 0;
    self->_.get_gain.method = (Method) 0;
    self->_.set_region.method = (Method) 0;
    self->_.get_region.method = (Method) 0;
    self->_.set_temperature.method = (Method) 0;
    self->_.get_temperature.method = (Method) 0;
    self->_.inspect.method = (Method) 0;
    //self->_.wait.method = (Method) 0;
    self->_.wait_for_completion.method = (Method) 0;
    self->_.raw.method = (Method) 0;
    self->_.expose.method = (Method) 0;
    self->_.status.method = (Method) 0;
    self->_.info.method = (Method) 0;
    self->_.stop.method = (Method) 0;
    self->_.abort.method = (Method) 0;
    self->_.power_on.method = (Method) 0;
    self->_.power_off.method = (Method) 0;
    
    return self;
}

static const void *_ASICameraClass;

static void
ASICameraClass_destroy(void)
{
    free((void *) _ASICameraClass);
}

static void
ASICameraClass_initialize(void)
{
    _ASICameraClass = new(__DetectorClass(), "ASICameraClass", __DetectorClass(), sizeof(struct ASICameraClass),
                           ctor, "", ASICameraClass_ctor,
                           (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(ASICameraClass_destroy);
#endif
}

const void *
ASICameraClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, ASICameraClass_initialize);
#endif
    
    return _ASICameraClass;
}

static const void *_ASICamera;

static void
ASICamera_destroy(void)
{
    free((void *)_ASICamera);
}

static void
ASICamera_initialize(void)
{
    _ASICamera = new(ASICameraClass(), "ASICamera", __Detector(), sizeof(struct ASICamera),
                     ctor, "ctor", ASICamera_ctor,
                     dtor, "dtor", ASICamera_dtor,
                     (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(ASICamera_destroy);
#endif
}

const void *
ASICamera(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, ASICamera_initialize);
#endif

    return _ASICamera;
}

static int
ASICamera_raw(void *_self, const void *write_buffer, size_t write_buffer_size, size_t *write_size, void *read_buffer, size_t read_buffer_size, size_t *read_size)
{
    struct ASICamera *self = cast(ASICamera(), _self);
    
    FILE *fp;
    char func_name[COMMANDSIZE];
    int ret;
    
    if ((fp = fmemopen((void *) write_buffer, write_buffer_size, "r")) == NULL) {
        return AAOS_ERROR;
    }
    
    if ((ret = fscanf(fp, "%s", &func_name)) != 1) {
        return AAOS_ERROR;
    }
    
    if ((strcmp(func_name, "ASIGetNumOfConnectedCameras")) == 0) {
        int (*ASIGetNumOfConnectedCameras)(void);
        int n;
        cJSON *root_json;
        char *string;
        
        ASIGetNumOfConnectedCameras = dlsym(self->dlh, "ASIGetNumOfConnectedCameras");
        n = ASIGetNumOfConnectedCameras();
        root_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(root_json, "n_camera", (double) n);
        string = cJSON_Print(root_json);
        cJSON_Delete(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        free(string);
        if (read_size != NULL) {
            *read_size = strlen((char *) read_buffer);
        }
    } else if ((strcmp(func_name, "ASICameraCheck")) == 0) {
        ASI_BOOL result;
        ASI_BOOL (*ASICameraCheck)(int, int);
        cJSON *root_json;
        char *string;
        
        ASICameraCheck = dlsym(self->dlh, "ASICameraCheck");
        result = ASICameraCheck(0x03C3, self->camera_id);
        root_json = cJSON_CreateObject();
        if (result) {
            cJSON_AddStringToObject(root_json, "CameraStatus", "OK");
        } else {
            cJSON_AddStringToObject(root_json, "CameraStatus", "ERROR");
        }
        string = cJSON_Print(root_json);
        cJSON_Delete(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        free(string);
        if (read_size != NULL) {
            *read_size = strlen((char *) read_buffer);
        }
    } else if ((strcmp(func_name, "ASIGetCameraProperty")) == 0) {
        int i;
	int camera_index;
        ASI_CAMERA_INFO camera_info;
        ASI_ERROR_CODE (*ASIGetCameraProperty)(ASI_CAMERA_INFO *, int);
        ASIGetCameraProperty = dlsym(self->dlh, "ASIGetCameraProperty");
        cJSON *root_json, *array_json;
        char *string;
        
        if ((ret = (fscanf(fp, "%d", &camera_index))) != 1) {
            ret = AAOS_EBADCMD;
            goto error;
        }
        if ((ret = ASIGetCameraProperty(&camera_info, camera_index)) !=  ASI_SUCCESS) {
            ret = asi_error_mapping(ret);
            goto error;
        }
        root_json = cJSON_CreateObject();
        cJSON_AddStringToObject(root_json, "Name", camera_info.Name);
        cJSON_AddNumberToObject(root_json, "CameraID", (double) camera_info.CameraID);
        cJSON_AddNumberToObject(root_json, "MaxHeight", (double) camera_info.MaxHeight);
        cJSON_AddNumberToObject(root_json, "MaxWidth", (double) camera_info.MaxWidth);
        cJSON_AddNumberToObject(root_json, "IsColorCam", (double) camera_info.IsColorCam);
        cJSON_AddNumberToObject(root_json, "BayerPattern", (double) camera_info.BayerPattern);
        array_json = cJSON_CreateIntArray(camera_info.SupportedBins, 16);
        cJSON_AddItemToObject(root_json, "SupportedBins", array_json);
        array_json = cJSON_CreateIntArray(camera_info.SupportedVideoFormat, 8);
        cJSON_AddItemToObject(root_json, "SupportedVideoFormat", array_json);
        cJSON_AddNumberToObject(root_json, "PixelSize", camera_info.PixelSize);
        cJSON_AddNumberToObject(root_json, "MechanicalShutter", (double) camera_info.MechanicalShutter);
        cJSON_AddNumberToObject(root_json, "ST4Port", (double) camera_info.ST4Port);
        cJSON_AddNumberToObject(root_json, "IsCoolerCam", (double) camera_info.IsCoolerCam);
        cJSON_AddNumberToObject(root_json, "IsUSB3Host", (double) camera_info.IsUSB3Host);
        cJSON_AddNumberToObject(root_json, "IsUSB3Camera", (double) camera_info.IsUSB3Camera);
        cJSON_AddNumberToObject(root_json, "ElecPerADU", (double) camera_info.ElecPerADU);
        cJSON_AddNumberToObject(root_json, "BitDepth", (double) camera_info.BitDepth);
        string = cJSON_Print(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
        cJSON_Delete(root_json);
        free(string);
    } else if ((strcmp(func_name, "ASIGetCameraPropertyByID")) == 0) {
        ASI_CAMERA_INFO camera_info;
        ASI_ERROR_CODE (*ASIGetCameraPropertyByID)(int, ASI_CAMERA_INFO *);
        ASIGetCameraPropertyByID = dlsym(self->dlh, "ASIGetCameraProperty");
        cJSON *root_json, *array_json;
        char *string;
        
        if ((ret = ASIGetCameraPropertyByID(self->camera_id, &camera_info)) !=  ASI_SUCCESS) {
            ret = asi_error_mapping(ret);
            goto error;
        }
        root_json = cJSON_CreateObject();
        cJSON_AddStringToObject(root_json, "Name", camera_info.Name);
        cJSON_AddNumberToObject(root_json, "CameraID", (double) camera_info.CameraID);
        cJSON_AddNumberToObject(root_json, "MaxHeight", (double) camera_info.MaxHeight);
        cJSON_AddNumberToObject(root_json, "MaxWidth", (double) camera_info.MaxWidth);
        cJSON_AddNumberToObject(root_json, "IsColorCam", (double) camera_info.IsColorCam);
        cJSON_AddNumberToObject(root_json, "BayerPattern", (double) camera_info.BayerPattern);
        array_json = cJSON_CreateIntArray(camera_info.SupportedBins, 16);
        cJSON_AddItemToObject(root_json, "SupportedBins", array_json);
        array_json = cJSON_CreateIntArray(camera_info.SupportedVideoFormat, 8);
        cJSON_AddItemToObject(root_json, "SupportedVideoFormat", array_json);
        cJSON_AddNumberToObject(root_json, "PixelSize", camera_info.PixelSize);
        cJSON_AddNumberToObject(root_json, "MechanicalShutter", (double) camera_info.MechanicalShutter);
        cJSON_AddNumberToObject(root_json, "ST4Port", (double) camera_info.ST4Port);
        cJSON_AddNumberToObject(root_json, "IsCoolerCam", (double) camera_info.IsCoolerCam);
        cJSON_AddNumberToObject(root_json, "IsUSB3Host", (double) camera_info.IsUSB3Host);
        cJSON_AddNumberToObject(root_json, "IsUSB3Camera", (double) camera_info.IsUSB3Camera);
        cJSON_AddNumberToObject(root_json, "ElecPerADU", (double) camera_info.ElecPerADU);
        cJSON_AddNumberToObject(root_json, "BitDepth", (double) camera_info.BitDepth);
        string = cJSON_Print(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
        cJSON_Delete(root_json);
        free(string);
    } else if ((strcmp(func_name, "ASIOpenCamera")) == 0) {
        ASI_ERROR_CODE (*ASIOpenCamera)(int);
                
        ASIOpenCamera = dlsym(self->dlh, "ASIOpenCamera");
        if ((ret = ASIOpenCamera(self->camera_id)) !=  ASI_SUCCESS) {
            ret = asi_error_mapping(ret);
            goto error;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if ((strcmp(func_name, "ASIInitCamera")) == 0) {
        ASI_ERROR_CODE (*ASIInitCamera)(int);
        
        ASIInitCamera = dlsym(self->dlh, "ASIInitCamera");
        if ((ret = ASIInitCamera(self->camera_id)) !=  ASI_SUCCESS) {
            ret = asi_error_mapping(ret);
            goto error;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if ((strcmp(func_name, "ASICloseCamera")) == 0) {
        ASI_ERROR_CODE (*ASICloseCamera)(int);
        
        ASICloseCamera = dlsym(self->dlh, "ASICloseCamera");
        if ((ret = ASICloseCamera(self->camera_id)) !=  ASI_SUCCESS) {
            ret = asi_error_mapping(ret);
            goto error;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if ((strcmp(func_name, "ASIGetNumOfControls")) == 0) {
        int controls[32];
        cJSON *root_json, *array_json;
        char *string;
	int i;

        for (i = 0; i < 32; i++) {
            controls[i] = -1;
        }
        ASI_ERROR_CODE (*ASIGetNumOfControls)(int, int *);
        ASIGetNumOfControls = dlsym(self->dlh, "ASIGetNumOfControls");
        if ((ret = ASIGetNumOfControls(self->camera_id, controls)) !=  ASI_SUCCESS) {
            ret = asi_error_mapping(ret);
            goto error;
        }
        root_json = cJSON_CreateObject();
        array_json = cJSON_CreateArray();
        for (i = 0; i < 32; i++) {
            if (controls[i] == -1) {
                break;
            }
            cJSON_AddItemToArray(array_json, cJSON_CreateNumber((double) controls[i]));
        }
        cJSON_AddItemToObject(root_json, "controls", array_json);
        string = cJSON_Print(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
        free(string);
        cJSON_Delete(root_json);
    } else if ((strcmp(func_name, "ASIGetControlCaps")) == 0) {
        int control_index;
        ASI_CONTROL_CAPS control_caps;
        cJSON *root_json, *array_json;
        char *string;
        ASI_ERROR_CODE (*ASIGetControlCaps)(int, int, ASI_CONTROL_CAPS *);
        
        if ((ret = (fscanf(fp, "%d", &control_index))) != 1) {
            ret = AAOS_EBADCMD;
            goto error;
        }
        ASIGetControlCaps = dlsym(self->dlh, "ASIGetControlCaps");
        if ((ret = ASIGetControlCaps(self->camera_id, control_index, &control_caps)) !=  ASI_SUCCESS) {
            ret = asi_error_mapping(ret);
            goto error;
        }
        root_json = cJSON_CreateObject();
        cJSON_AddStringToObject(root_json, "Name", control_caps.Name);
        cJSON_AddStringToObject(root_json, "Description", control_caps.Description);
        cJSON_AddNumberToObject(root_json, "MaxValue", (double) control_caps.MaxValue);
        cJSON_AddNumberToObject(root_json, "MinValue", (double) control_caps.MaxValue);
        cJSON_AddNumberToObject(root_json, "DefaultValue", (double) control_caps.DefaultValue);
        cJSON_AddNumberToObject(root_json, "IsAutoSupported", (double) control_caps.IsAutoSupported);
        cJSON_AddNumberToObject(root_json, "IsWritable", (double) control_caps.IsWritable);
        cJSON_AddItemToObject(root_json, "controls", array_json);
        string = cJSON_Print(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
        free(string);
        cJSON_Delete(root_json);
    } else if ((strcmp(func_name, "ASIGetControlValue")) == 0) {
        int control_type;
        long value;
        ASI_BOOL is_auto;
        cJSON *root_json;
        char *string;
        ASI_ERROR_CODE (*ASIGetControlValue)(int, int, long *, ASI_BOOL *);
        
        if ((ret = (fscanf(fp, "%d", &control_type))) != 1) {
            ret = AAOS_EBADCMD;
            goto error;
        }
        ASIGetControlValue = dlsym(self->dlh, "ASIGetControlValue");
        if ((ret = ASIGetControlValue(self->camera_id, control_type, &value, &is_auto)) !=  ASI_SUCCESS) {
            ret = asi_error_mapping(ret);
            goto error;
        }
        root_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(root_json, "Value", (double) value);
        cJSON_AddNumberToObject(root_json, "IsAuto", (double) is_auto);
        string = cJSON_Print(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
        free(string);
        cJSON_Delete(root_json);
    } else if ((strcmp(func_name, "ASISetControlValue")) == 0) {
        int control_type;
        long value;
        ASI_BOOL is_auto;
        ASI_ERROR_CODE (*ASISetControlValue)(int, int, long, ASI_BOOL);
        
        if ((ret = (fscanf(fp, "%d %ld %d", &control_type, &value, &is_auto))) != 3) {
            ret = AAOS_EBADCMD;
            goto error;
        }
        if (is_auto) {
            is_auto = ASI_TRUE;
        }
        ASISetControlValue = dlsym(self->dlh, "ASISetControlValue");
        if ((ret = ASISetControlValue(self->camera_id, control_type, value, is_auto)) !=  ASI_SUCCESS) {
            ret = asi_error_mapping(ret);
            goto error;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if ((strcmp(func_name, "ASIGetROIFormat")) == 0) {
        int width, height, binning, img_type;
        cJSON *root_json;
        char *string;
        ASI_ERROR_CODE (*ASIGetROIFormat)(int, int *, int *, int *, ASI_IMG_TYPE *);
        
        ASIGetROIFormat = dlsym(self->dlh, "ASIGetROIFormat");
        if ((ret = ASIGetROIFormat(self->camera_id, &width, &height, &binning, &img_type)) !=  ASI_SUCCESS) {
            ret = asi_error_mapping(ret);
            goto error;
        }
        root_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(root_json, "Width", (double) width);
        cJSON_AddNumberToObject(root_json, "Height", (double) height);
        cJSON_AddNumberToObject(root_json, "Binning", (double) binning);
        cJSON_AddNumberToObject(root_json, "Img_type", (double) img_type);
        string = cJSON_Print(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
        free(string);
        cJSON_Delete(root_json);
    } else if ((strcmp(func_name, "ASISetROIFormat")) == 0) {
        int width, height, binning, img_type;
        ASI_ERROR_CODE (*ASISetROIFormat)(int, int, int, int, ASI_IMG_TYPE);
        
        if ((ret = (fscanf(fp, "%d %d %d %d", &width, &height, &binning, &img_type))) != 5) {
            ret = AAOS_EBADCMD;
            goto error;
        }
        ASISetROIFormat = dlsym(self->dlh, "ASISetROIFormat");
        if ((ret = ASISetROIFormat(self->camera_id, width, height, binning, img_type)) !=  ASI_SUCCESS) {
            ret = asi_error_mapping(ret);
            goto error;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if ((strcmp(func_name, "ASIGetStartPos")) == 0) {
        int x, y;
        cJSON *root_json;
        char *string;
        ASI_ERROR_CODE (*ASIGetStartPos)(int, int *, int *);
        
        ASIGetStartPos = dlsym(self->dlh, "ASIGetStartPos");
        if ((ret = ASIGetStartPos(self->camera_id, &x, &y)) !=  ASI_SUCCESS) {
            ret = asi_error_mapping(ret);
            goto error;
        }
        root_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(root_json, "X_Offset", (double) x);
        cJSON_AddNumberToObject(root_json, "Y_Offset", (double) y);
        string = cJSON_Print(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
        free(string);
        cJSON_Delete(root_json);
    } else if ((strcmp(func_name, "ASISetStartPos")) == 0) {
        int x, y;
        ASI_ERROR_CODE (*ASISetStartPos)(int, int, int);
        
        if ((ret = (fscanf(fp, "%d %d", &x, &y))) != 3) {
            ret = AAOS_EBADCMD;
            goto error;
        }
        ASISetStartPos = dlsym(self->dlh, "ASISetStartPos");
        if ((ret = ASISetStartPos(self->camera_id, x, y)) !=  ASI_SUCCESS) {
            ret = asi_error_mapping(ret);
            goto error;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if ((strcmp(func_name, "ASIGetDroppedFrames")) == 0) {
        int n_dropped;
        cJSON *root_json;
        char *string;
        ASI_ERROR_CODE (*ASIGetDroppedFrames)(int, int *);
        
        ASIGetDroppedFrames = dlsym(self->dlh, "ASIGetDroppedFrames");
        if ((ret = ASIGetDroppedFrames(self->camera_id, &n_dropped)) !=  ASI_SUCCESS) {
            ret = asi_error_mapping(ret);
            goto error;
        }
        root_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(root_json, "DropperFrames", (double) n_dropped);
        string = cJSON_Print(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
        free(string);
        cJSON_Delete(root_json);
    } else if ((strcmp(func_name, "ASIEnableDarkSubtract")) == 0) {
        char dark_ref[PATHSIZE];
        ASI_ERROR_CODE (*ASIEnableDarkSubtract)(int, char *);
        
        if ((ret = (fscanf(fp, "%s", dark_ref))) != 1) {
            ret = AAOS_EBADCMD;
            goto error;
        }
        ASIEnableDarkSubtract = dlsym(self->dlh, "ASIEnableDarkSubtract");
        if ((ret = ASIEnableDarkSubtract(self->camera_id, dark_ref)) !=  ASI_SUCCESS) {
            ret = asi_error_mapping(ret);
            goto error;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if ((strcmp(func_name, "ASIDisableDarkSubtract")) == 0) {
        ASI_ERROR_CODE (*ASIDisableDarkSubtract)(int);

        ASIDisableDarkSubtract = dlsym(self->dlh, "ASIDisableDarkSubtract");
        if ((ret = ASIDisableDarkSubtract(self->camera_id)) !=  ASI_SUCCESS) {
            ret = asi_error_mapping(ret);
            goto error;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if ((strcmp(func_name, "ASIStartVideoCapture")) == 0) {
        ASI_ERROR_CODE (*ASIStartVideoCapture)(int);

        ASIStartVideoCapture = dlsym(self->dlh, "ASIStartVideoCapture");
        if ((ret = ASIStartVideoCapture(self->camera_id)) !=  ASI_SUCCESS) {
            ret = asi_error_mapping(ret);
            goto error;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if ((strcmp(func_name, "ASIStopVideoCapture")) == 0) {
        ASI_ERROR_CODE (*ASIStopVideoCapture)(int);

        ASIStopVideoCapture = dlsym(self->dlh, "ASIStopVideoCapture");
        if ((ret = ASIStopVideoCapture(self->camera_id)) !=  ASI_SUCCESS) {
            ret = asi_error_mapping(ret);
            goto error;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if ((strcmp(func_name, "ASIGetVideoData")) == 0) {
        int wait_time_ms;
        int width, height, binning, img_type;
        ASI_ERROR_CODE (*ASIGetROIFormat)(int, int *, int *, int *, ASI_IMG_TYPE *);
        ASI_ERROR_CODE (*ASIGetVideoData)(int, unsigned char *, long, int);
        
        ASIGetROIFormat = dlsym(self->dlh, "ASIGetROIFormat");
        if ((ret = ASIGetROIFormat(self->camera_id, &width, &height, &binning, &img_type)) !=  ASI_SUCCESS) {
            ret = asi_error_mapping(ret);
            goto error;
        }
        switch (img_type) {
            case ASI_IMG_RAW8:
                if (read_buffer_size < width * height) {
                    ret = AAOS_ENOMEM;
                    goto error;
                }
                if (read_size != NULL) {
                    *read_size = width * height;
                }
                break;
            case ASI_IMG_RGB24:
                if (read_buffer_size < width * height * 3) {
                    ret = AAOS_ENOMEM;
                    goto error;
                }
                if (read_size != NULL) {
                    *read_size = width * height * 3;
                }
                break;
            case ASI_IMG_RAW16:
                if (read_buffer_size < width * height * 2) {
                    ret = AAOS_ENOMEM;
                    goto error;
                }
                if (read_size != NULL) {
                    *read_size = width * height * 2;
                }
                break;
            default:
                if (read_size != NULL) {
                    *read_size = width * height;
                }
                break;
        }
        if ((ret = (fscanf(fp, "%d", &wait_time_ms))) != 1) {
            ret = AAOS_EBADCMD;
            goto error;
        }
        ASIGetVideoData = dlsym(self->dlh, "ASIGetVideoData");
        if ((ret = ASIGetVideoData(self->camera_id, (unsigned char *) read_buffer, (long) read_buffer_size, wait_time_ms)) !=  ASI_SUCCESS) {
            ret = asi_error_mapping(ret);
            goto error;
        }
    } else if ((strcmp(func_name, "ASIGetVideoDataGPS")) == 0) {
        int wait_time_ms, nread;;
        int width, height, binning, img_type;
        ASI_GPS_DATA gps_data;
        ASI_ERROR_CODE (*ASIGetROIFormat)(int, int *, int *, int *, ASI_IMG_TYPE *);
        ASI_ERROR_CODE (*ASIGetVideoDataGPS)(int, unsigned char *, long, int, ASI_GPS_DATA *);
        ASIGetROIFormat = dlsym(self->dlh, "ASIGetROIFormat");
        
        if ((ret = ASIGetROIFormat(self->camera_id, &width, &height, &binning, &img_type)) !=  ASI_SUCCESS) {
            ret = asi_error_mapping(ret);
            goto error;
        }
        switch (img_type) {
            case ASI_IMG_RAW8:
                if (read_buffer_size < width * height) {
                    ret = AAOS_ENOMEM;
                    goto error;
                }
                nread = width * height + sizeof(ASI_GPS_DATA);
                
                break;
            case ASI_IMG_RGB24:
                if (read_buffer_size < width * height * 3) {
                    ret = AAOS_ENOMEM;
                    goto error;
                }
                nread = width * height * 3 + sizeof(ASI_GPS_DATA);
                break;
            case ASI_IMG_RAW16:
                if (read_buffer_size < width * height * 2) {
                    ret = AAOS_ENOMEM;
                    goto error;
                }
                nread = width * height * 2 + sizeof(ASI_GPS_DATA);
                break;
            default:
                nread = width * height;
                break;
        }
        if ((ret = (fscanf(fp, "%d", &wait_time_ms))) != 1) {
            ret = AAOS_EBADCMD;
            goto error;
        }
        ASIGetVideoDataGPS = dlsym(self->dlh, "ASIGetVideoDataGPS");
        if ((ret = ASIGetVideoDataGPS(self->camera_id, (unsigned char *) read_buffer, (long) read_buffer_size, wait_time_ms, &gps_data)) !=  ASI_SUCCESS) {
            ret = asi_error_mapping(ret);
            goto error;
        }
        memcpy((unsigned char *) read_buffer + nread - sizeof(ASI_GPS_DATA) , &gps_data, sizeof(ASI_GPS_DATA));
        if (read_size != NULL) {
            *read_size = nread;
        }
    } else if ((strcmp(func_name, "ASIPulseGuideOn")) == 0) {
        int direction;
        ASI_ERROR_CODE (*ASIPulseGuideOn)(int, ASI_GUIDE_DIRECTION);
        
        if ((ret = (fscanf(fp, "%d", &direction))) != 1) {
            ret = AAOS_EBADCMD;
            goto error;
        }
        ASIPulseGuideOn = dlsym(self->dlh, "ASIPulseGuideOn");
        if ((ret = ASIPulseGuideOn(self->camera_id, direction)) !=  ASI_SUCCESS) {
            ret = asi_error_mapping(ret);
            goto error;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if ((strcmp(func_name, "ASIStartExposure")) == 0) {
        ASI_ERROR_CODE (*ASIStartExposure)(int);

        ASIStartExposure = dlsym(self->dlh, "ASIStartExposure");
        if ((ret = ASIStartExposure(self->camera_id)) !=  ASI_SUCCESS) {
            ret = asi_error_mapping(ret);
            goto error;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if ((strcmp(func_name, "ASIStopExposure")) == 0) {
        ASI_ERROR_CODE (*ASIStopExposure)(int);
 
        ASIStopExposure = dlsym(self->dlh, "ASIStopExposure");
        if ((ret = ASIStopExposure(self->camera_id)) !=  ASI_SUCCESS) {
            ret = asi_error_mapping(ret);
            goto error;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if ((strcmp(func_name, "ASIGetExpStatus")) == 0) {
        ASI_EXPOSURE_STATUS exposure_status;
	ASI_ERROR_CODE (*ASIGetExpStatus)(int, ASI_EXPOSURE_STATUS *);
        ASIGetExpStatus = dlsym(self->dlh, "ASIGetExpStatus");
	cJSON *root_json;
        char *string;

        if ((ret = ASIGetExpStatus(self->camera_id, &exposure_status)) != ASI_SUCCESS) {
            ret = asi_error_mapping(ret);
            goto error;
        }
        root_json = cJSON_CreateObject();
		cJSON_AddNumberToObject(root_json, "Status", (double) exposure_status);
		string = cJSON_Print(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
        free(string);
        cJSON_Delete(root_json);
    } else if ((strcmp(func_name, "ASIGetDataAfterExp")) == 0) {
        int wait_time_ms;
        int width, height, binning, img_type;
        ASI_ERROR_CODE (*ASIGetROIFormat)(int, int *, int *, int *, ASI_IMG_TYPE *);
        ASI_ERROR_CODE (*ASIGetDataAfterExp)(int, unsigned char *, long);
        ASIGetROIFormat = dlsym(self->dlh, "ASIGetROIFormat");
        
        if ((ret = ASIGetROIFormat(self->camera_id, &width, &height, &binning, &img_type)) !=  ASI_SUCCESS) {
            ret = asi_error_mapping(ret);
            goto error;
        }
        switch (img_type) {
            case ASI_IMG_RAW8:
                if (read_buffer_size < width * height) {
                    ret = AAOS_ENOMEM;
                    goto error;
                }
                if (read_size != NULL) {
                    *read_size = width * height;
                }
                break;
            case ASI_IMG_RGB24:
                if (read_buffer_size < width * height * 3) {
                    ret = AAOS_ENOMEM;
                    goto error;
                }
                if (read_size != NULL) {
                    *read_size = width * height * 3;
                }
                break;
            case ASI_IMG_RAW16:
                if (read_buffer_size < width * height * 2) {
                    ret = AAOS_ENOMEM;
                    goto error;
                }
                if (read_size != NULL) {
                    *read_size = width * height * 2;
                }
                break;
            default:
                if (read_size != NULL) {
                    *read_size = width * height;
                }
                break;
        }
        ASIGetDataAfterExp = dlsym(self->dlh, "ASIGetDataAfterExp");
        if ((ret = ASIGetDataAfterExp(self->camera_id, (unsigned char *) read_buffer, (long) read_buffer_size)) !=  ASI_SUCCESS) {
            ret = asi_error_mapping(ret);
            goto error;
        }
    } else if ((strcmp(func_name, "ASIGetDataAfterExpGPS")) == 0) {
        int nread;;
        int width, height, binning, img_type;
        ASI_GPS_DATA gps_data;
        ASI_ERROR_CODE (*ASIGetROIFormat)(int, int *, int *, int *, ASI_IMG_TYPE *);
        ASI_ERROR_CODE (*ASIGetDataAfterExpGPS)(int, unsigned char *, long, ASI_GPS_DATA *);
        ASIGetROIFormat = dlsym(self->dlh, "ASIGetROIFormat");
        
        if ((ret = ASIGetROIFormat(self->camera_id, &width, &height, &binning, &img_type)) !=  ASI_SUCCESS) {
            ret = asi_error_mapping(ret);
            goto error;
        }
        switch (img_type) {
            case ASI_IMG_RAW8:
                if (read_buffer_size < width * height) {
                    ret = AAOS_ENOMEM;
                    goto error;
                }
                nread = width * height + sizeof(ASI_GPS_DATA);
                
                break;
            case ASI_IMG_RGB24:
                if (read_buffer_size < width * height * 3) {
                    ret = AAOS_ENOMEM;
                    goto error;
                }
                nread = width * height * 3 + sizeof(ASI_GPS_DATA);
                break;
            case ASI_IMG_RAW16:
                if (read_buffer_size < width * height * 2) {
                    ret = AAOS_ENOMEM;
                    goto error;
                }
                nread = width * height * 2 + sizeof(ASI_GPS_DATA);
                break;
            default:
                nread = width * height;
                break;
        }
        ASIGetDataAfterExpGPS = dlsym(self->dlh, "ASIGetDataAfterExpGPS");
        if ((ret = ASIGetDataAfterExpGPS(self->camera_id, (unsigned char *) read_buffer, (long) read_buffer_size, &gps_data)) !=  ASI_SUCCESS) {
            ret = asi_error_mapping(ret);
            goto error;
        }
        memcpy((unsigned char *) read_buffer + nread - sizeof(ASI_GPS_DATA) , &gps_data, sizeof(ASI_GPS_DATA));
        if (read_size != NULL) {
            *read_size = nread;
        }
    } else if (strcmp(func_name, "ASIGetID") == 0) {
	ASI_ID asi_id;
	ASI_ERROR_CODE (*ASIGetID)(int, ASI_ID *);
	
	ASIGetID = dlsym(self->dlh, "ASIGetID");
        cJSON *root_json;
        char *string;
	if ((ret = ASIGetID(self->camera_id, &asi_id)) !=  ASI_SUCCESS) {
            ret = asi_error_mapping(ret);
            goto error;
        }
        root_json = cJSON_CreateObject();
        cJSON_AddStringToObject(root_json, "ASI_ID", asi_id.id);
        string = cJSON_Print(root_json);
        cJSON_Delete(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        free(string);

	if (read_size != NULL) {
            *read_size = strlen((char *) read_buffer) + 1;
        }
    } else if (strcmp(func_name, "ASISetID") == 0) {
		ASI_ID asi_id; 
		ASI_ERROR_CODE (*ASISetID)(int, ASI_ID);
        
		ASISetID = dlsym(self->dlh, "ASISetID");
		if ((ret = (fscanf(fp, "%d", &asi_id))) != 1) {
            ret = AAOS_EBADCMD;
            goto error;
        }
		if ((ret = ASISetID(self->camera_id, asi_id)) !=  ASI_SUCCESS) {
            ret = asi_error_mapping(ret);
            goto error;
        }
		if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "ASIGetGainOffset") == 0) {
		int Offset_HighestDR, Offset_UnityGain, Gain_LowestRN, Offset_LowestRN;
		ASI_ERROR_CODE (*ASIGetGainOffset)(int, int *, int *, int *, int *);
		ASIGetGainOffset = dlsym(self->dlh, "ASIGetGainOffset");
		cJSON *root_json;
		char *string;

		if ((ret = ASIGetGainOffset(self->camera_id, &Offset_HighestDR, &Offset_UnityGain, &Gain_LowestRN, &Offset_LowestRN)) !=  ASI_SUCCESS) {
            ret = asi_error_mapping(ret);
            goto error;
        }
		root_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(root_json, "Offset_HighestDR", (double) Offset_HighestDR);
        cJSON_AddNumberToObject(root_json, "Offset_UnityGain", (double) Offset_UnityGain);
        cJSON_AddNumberToObject(root_json, "Gain_LowestRN", (double) Gain_LowestRN);
        cJSON_AddNumberToObject(root_json, "Offset_LowestRN", (double) Offset_LowestRN);
        string = cJSON_Print(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
        free(string);
        cJSON_Delete(root_json);
    } else if (strcmp(func_name, "ASIGetLMHGainOffset") == 0) {
		int LGain, MGain, HGain, HOffset;
		ASI_ERROR_CODE (*ASIGetLMHGainOffset)(int, int *, int *, int *, int *);
		ASIGetLMHGainOffset = dlsym(self->dlh, "ASIGetLMHGainOffset");
		cJSON *root_json;
		char *string;
	
		if ((ret = ASIGetLMHGainOffset(self->camera_id, &LGain, &MGain, &HGain, &HOffset)) !=  ASI_SUCCESS) {
            ret = asi_error_mapping(ret);
            goto error;
        }
		root_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(root_json, "LGain", (double) LGain);
        cJSON_AddNumberToObject(root_json, "MGain", (double) MGain);
        cJSON_AddNumberToObject(root_json, "HGain", (double) HGain);
        cJSON_AddNumberToObject(root_json, "HOffset", (double) HOffset);
        string = cJSON_Print(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
        free(string);
        cJSON_Delete(root_json);
    } else if (strcmp(func_name, "ASIGetSDKVersion") == 0) {
		const char *version;
		char * (*ASIGetSDKVersion)(void);
        cJSON *root_json;
        char *string;
        
		ASIGetSDKVersion = dlsym(self->dlh, "ASIGetSDKVersion");
		if ((version = ASIGetSDKVersion()) == NULL) {
			ret = AAOS_ERROR;
			goto error;
		}
        root_json = cJSON_CreateObject();
        cJSON_AddStringToObject(root_json, "SDKVersion", version);
        string = cJSON_Print(root_json);
        cJSON_Delete(root_json);
		snprintf(read_buffer, read_buffer_size, "%s", version);
        free(string);
        if (read_size != NULL) {
            *read_size = strlen((char *) read_buffer) + 1;
        }
    } else if (strcmp(func_name, "ASIGetCameraSupportMode") == 0) {
		int i;
		ASI_SUPPORTED_MODE supported_mode;
		ASI_ERROR_CODE (*ASIGetCameraSupportMode)(int, ASI_SUPPORTED_MODE *);
		cJSON *root_json, *array_json;
		char *string;
		
        ASIGetCameraSupportMode = dlsym(self->dlh, "ASI_SUPPORTED_MODE");
		if ((ret = ASIGetCameraSupportMode(self->camera_id, &supported_mode)) !=  ASI_SUCCESS) {
            ret = asi_error_mapping(ret);
            goto error;
        }
		root_json = cJSON_CreateObject();
		array_json = cJSON_CreateArray();
		for (i = 0; i < 16; i++) {
			if (supported_mode.SupportedCameraMode[i] == ASI_MODE_END) {
				break;
			}
			cJSON_AddItemToArray(array_json, cJSON_CreateNumber((double) supported_mode.SupportedCameraMode[i]));
		}
        cJSON_AddItemToObject(root_json, "SupportedMode", array_json);
        string = cJSON_Print(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
        free(string);
        cJSON_Delete(root_json);
    } else if (strcmp(func_name, "ASIGetMode") == 0) {
		ASI_CAMERA_MODE camera_mode;
		ASI_ERROR_CODE (*ASIGetMode)(int, ASI_CAMERA_MODE *);
		ASIGetMode = dlsym(self->dlh, "ASIGetMode");
		cJSON *root_json;
		char *string;

		if ((ret = ASIGetMode(self->camera_id, &camera_mode)) !=  ASI_SUCCESS) {
            ret = asi_error_mapping(ret);
            goto error;
        }
		root_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(root_json, "CameraMode", (double) camera_mode);
        string = cJSON_Print(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
        free(string);
        cJSON_Delete(root_json);
    } else if (strcmp(func_name, "ASISetMode") == 0) {
		int camera_mode;
		ASI_ERROR_CODE (*ASISetMode)(int, ASI_CAMERA_MODE);
        
		ASISetMode = dlsym(self->dlh, "ASISetMode");
		if ((ret = (fscanf(fp, "%d",  &camera_mode))) != 1) {
            ret = AAOS_EBADCMD;
            goto error;
        }
        if ((ret = ASISetMode(self->camera_id, camera_mode)) !=  ASI_SUCCESS) {
            ret = asi_error_mapping(ret);
            goto error;
        }
		if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "ASISendSoftTrigger") == 0) {
		ASI_BOOL is_start;
		ASI_ERROR_CODE (*ASISendSoftTrigger)(int, ASI_BOOL);
        
		ASISendSoftTrigger = dlsym(self->dlh, "ASISendSoftTrigger");
		if ((ret = (fscanf(fp, "%d", &is_start))) != 1) {
            ret = AAOS_EBADCMD;
            goto error;
        }
		if (is_start) {
			is_start = ASI_TRUE;
		}
		if ((ret = ASISendSoftTrigger(self->camera_id, is_start)) !=  ASI_SUCCESS) {
            ret = asi_error_mapping(ret);
            goto error;
        }
		if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "ASIGetSerialNumber") == 0) {
	ASI_SN camera_sn;
	ASI_ERROR_CODE (*ASIGetSerialNumber)(int, ASI_SN *);
        cJSON *root_json;
        char *string;
		
	ASIGetSerialNumber = dlsym(self->dlh, "ASIGetSerialNumber");
		
	if ((ret = ASIGetSerialNumber(self->camera_id, &camera_sn)) !=  ASI_SUCCESS) {
            ret = asi_error_mapping(ret);
            goto error;
        }
        root_json = cJSON_CreateObject();
        cJSON_AddStringToObject(root_json, "SerialNumber", camera_sn.id);
        string = cJSON_Print(root_json);
        cJSON_Delete(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        free(string);
        if (read_size != NULL) {
            *read_size = strlen((char *) read_buffer) + 1;
        }
    } else if (strcmp(func_name, "ASISetTriggerOutputIOConf") == 0) {
	int pin, is_pin_high;
	long delay, duration;
	ASI_ERROR_CODE (*ASISetTriggerOutputIOConf)(int, ASI_TRIG_OUTPUT_PIN, ASI_BOOL, long, long);

	ASISetTriggerOutputIOConf = dlsym(self->dlh, "ASISetTriggerOutputIOConf");
	if ((ret = (fscanf(fp, "%d %d %ld %ld", &pin, &is_pin_high, &delay, &duration))) != 4) {
            ret = AAOS_EBADCMD;
	    goto error;
        }
	if (is_pin_high) {
	    is_pin_high = ASI_TRUE;
	}
	if ((ret = ASISetTriggerOutputIOConf(self->camera_id, pin, is_pin_high, delay, duration)) !=  ASI_SUCCESS) {
	    ret = asi_error_mapping(ret);
	    goto error;
	}
	if (read_size != NULL) {
	    *read_size = 0;
	}
    } else if (strcmp(func_name, "ASIGPSGetData") == 0) {
	ASI_GPS_DATA start_gps_data, end_gps_data;
	ASI_ERROR_CODE (*ASIGPSGetData)(int, ASI_GPS_DATA *, ASI_GPS_DATA *);
	cJSON *root_json, *gps_json, *datetime_json;
	char *string;
        
	ASIGPSGetData = dlsym(self->dlh, "ASIGPSGetData");
        if ((ret = ASIGPSGetData(self->camera_id, &start_gps_data, &end_gps_data)) !=  ASI_SUCCESS) {
            ret = asi_error_mapping(ret);
            goto error;
        }
        root_json = cJSON_CreateObject();
	datetime_json = cJSON_CreateObject();
	cJSON_AddNumberToObject(datetime_json, "Year", (double) start_gps_data.Datetime.Year);
	cJSON_AddNumberToObject(datetime_json, "Month", (double) start_gps_data.Datetime.Month);
	cJSON_AddNumberToObject(datetime_json, "Day", (double) start_gps_data.Datetime.Day);
	cJSON_AddNumberToObject(datetime_json, "Hour", (double) start_gps_data.Datetime.Hour);
	cJSON_AddNumberToObject(datetime_json, "Minute", (double) start_gps_data.Datetime.Minute);
	cJSON_AddNumberToObject(datetime_json, "Second", (double) start_gps_data.Datetime.Second);
	cJSON_AddNumberToObject(datetime_json, "Msecond", (double) start_gps_data.Datetime.Msecond);
	cJSON_AddNumberToObject(datetime_json, "Usecond", (double) start_gps_data.Datetime.Usecond);
	gps_json = cJSON_CreateObject();
	cJSON_AddItemToObject(gps_json, "Datetime", datetime_json);
	cJSON_AddNumberToObject(gps_json, "Latitude", start_gps_data.Latitude);
	cJSON_AddNumberToObject(gps_json, "Longitude", start_gps_data.Longitude);
	cJSON_AddNumberToObject(gps_json, "Altitude", (double) start_gps_data.Altitude);
	cJSON_AddNumberToObject(gps_json, "SatelliteNum", (double) start_gps_data.SatelliteNum);
	cJSON_AddItemToObject(gps_json, "StartGPSData", gps_json);
	datetime_json = cJSON_CreateObject();
	cJSON_AddNumberToObject(datetime_json, "Year", (double) end_gps_data.Datetime.Year);
	cJSON_AddNumberToObject(datetime_json, "Month", (double) end_gps_data.Datetime.Month);
	cJSON_AddNumberToObject(datetime_json, "Day", (double) end_gps_data.Datetime.Day);
	cJSON_AddNumberToObject(datetime_json, "Hour", (double) end_gps_data.Datetime.Hour);
	cJSON_AddNumberToObject(datetime_json, "Minute", (double) end_gps_data.Datetime.Minute);
	cJSON_AddNumberToObject(datetime_json, "Second", (double) end_gps_data.Datetime.Second);
	cJSON_AddNumberToObject(datetime_json, "Msecond", (double) end_gps_data.Datetime.Msecond);
	cJSON_AddNumberToObject(datetime_json, "Usecond", (double) end_gps_data.Datetime.Usecond);
	gps_json = cJSON_CreateObject();
	cJSON_AddItemToObject(gps_json, "Datetime", datetime_json);
	cJSON_AddNumberToObject(gps_json, "Latitude", end_gps_data.Latitude);
	cJSON_AddNumberToObject(gps_json, "Longitude", end_gps_data.Longitude);
	cJSON_AddNumberToObject(gps_json, "Altitude", (double) end_gps_data.Altitude);
	cJSON_AddNumberToObject(gps_json, "SatelliteNum", (double) end_gps_data.SatelliteNum);
	cJSON_AddItemToObject(gps_json, "EndGPSData", gps_json);
	string = cJSON_Print(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        if (read_size != NULL) {
            *read_size = strlen(read_buffer) + 1;
        }
        free(string);
        cJSON_Delete(root_json);
    } else {
        ret = AAOS_EBADCMD;
    }
error:
    fclose(fp);
    return ret;
    
}

static int
ASICamera_init(void *_self)
{
    struct ASICamera *self = cast(ASICamera(), _self);
    
    int (*ASIGetNumOfConnectedCameras)(void);
    ASI_CAMERA_INFO camera_info;
    ASI_ERROR_CODE (*ASIGetCameraProperty)(ASI_CAMERA_INFO *, int);
    ASI_ERROR_CODE (*ASIGetCameraPropertyByID)(int, ASI_CAMERA_INFO *);
    ASI_ERROR_CODE (*ASIOpenCamera)(int);
    ASI_ERROR_CODE (*ASIInitCamera)(int);
    ASI_ERROR_CODE (*ASIGetNumOfControls)(int, int *);
    ASI_CONTROL_CAPS control_caps;
    ASI_ERROR_CODE (*ASIGetControlCaps)(int, int, ASI_CONTROL_CAPS *);
    ASI_ERROR_CODE (*ASIGetStartPos)(int, int *, int *);
    ASI_ERROR_CODE (*ASIGetROIFormat)(int, int *, int *, int *, ASI_IMG_TYPE *);
    ASI_ERROR_CODE (*ASIGetControlValue)(int, ASI_CONTROL_TYPE, long *, ASI_BOOL *);
    ASI_ERROR_CODE (*ASISetControlValue)(int, ASI_CONTROL_TYPE, long, ASI_BOOL);
    ASI_ERROR_CODE (*ASISetROIFormat)(int, int, int, int, ASI_IMG_TYPE);
    ASI_ERROR_CODE (*ASIGetCameraSupportMode)(int, ASI_SUPPORTED_MODE*);
    ASI_ERROR_CODE (*ASISetCameraMode)(int, ASI_CAMERA_MODE);
    ASI_ERROR_CODE (*ASIGetCameraMode)(int, ASI_CAMERA_MODE *);

    int i, n, x, y, w, h, b;
    ASI_IMG_TYPE t;
    ASI_BOOL B;
    ASI_SUPPORTED_MODE supported_mode;
    ASI_CAMERA_MODE mode;
    long v;
    int ret;
    
    ASIGetNumOfConnectedCameras = dlsym(self->dlh, "ASIGetNumOfConnectedCameras");
    ASIGetCameraProperty = dlsym(self->dlh, "ASIGetCameraProperty");
    ASIGetCameraPropertyByID = dlsym(self->dlh, "ASIGetCameraPropertyByID");
    ASIOpenCamera = dlsym(self->dlh, "ASIOpenCamera");
    ASIInitCamera = dlsym(self->dlh, "ASIInitCamera");
    ASIGetNumOfControls = dlsym(self->dlh, "ASIGetNumOfControls");
    ASIGetControlCaps = dlsym(self->dlh, "ASIGetControlCaps");
    ASIGetStartPos = dlsym(self->dlh, "ASIGetStartPos");
    ASIGetROIFormat = dlsym(self->dlh, "ASIGetROIFormat");
    ASIGetControlValue = dlsym(self->dlh, "ASIGetControlValue");
    ASISetControlValue = dlsym(self->dlh, "ASISetControlValue");
    ASISetROIFormat = dlsym(self->dlh, "ASISetROIFormat");
    ASIGetCameraSupportMode = dlsym(self->dlh, "ASIGetCameraSupportMode");
    ASISetCameraMode = dlsym(self->dlh, "ASISetCameraMode");
    ASIGetCameraMode = dlsym(self->dlh, "ASIGetCameraMode");
    
    if (self->camera_id < 0) {
        n = ASIGetNumOfConnectedCameras();
        if (n == 0) {
#ifdef DEBUG
	    fprintf(stderr, "%s %s %d --- ASIGetNumOfConnectedCameras: no ASI camera has been found\n", __FILE__, __func__, __LINE__ - 2);
#endif
            ret = AAOS_ENOTFOUND;
            goto error;
        }
        if (self->name == NULL) {
            if ((ret = ASIGetCameraProperty(&camera_info, 0)) != ASI_SUCCESS) {
#ifdef DEBUG
		fprintf(stderr, "%s %s %d --- ASIGetCameraProperty error: %d\n", __FILE__, __func__, __LINE__ - 2, ret);
#endif
                ret = asi_error_mapping(ret);
                goto error;
            }
            self->camera_index = 0;
            self->camera_id = camera_info.CameraID;
            self->name = (char *) Malloc(strlen(camera_info.Name) + 1);
            snprintf(self->name, strlen(camera_info.Name) + 1, "%s", camera_info.Name);
        } else {
            ret = AAOS_ENOTFOUND;
            for (i = 0; i < n; i++) {
                if ((ret = ASIGetCameraProperty(&camera_info, i)) != ASI_SUCCESS) {
                    ret = asi_error_mapping(ret);
                    goto error;
                }
                if (strcmp(self->name, camera_info.Name) == 0) {
                    self->camera_index = i;
                    self->camera_id = camera_info.CameraID;
                    ret = AAOS_OK;
                }
            }
            if (ret == AAOS_ENOTFOUND) {
                goto error;
            }
        }
    } else {
        if ((ret = ASIGetCameraPropertyByID(self->camera_id, &camera_info)) != ASI_SUCCESS) {
            ret = asi_error_mapping(ret);
            goto error;
        }
    }
    
    if ((ret = ASIOpenCamera(self->camera_id)) != ASI_SUCCESS) {
        ret = asi_error_mapping(ret);
        goto error;
    }
    
    if ((ret = ASIInitCamera(self->camera_id)) != ASI_SUCCESS) {
        ret = asi_error_mapping(ret);
        goto error;
    }
    
    self->_.d_cap.width = camera_info.MaxWidth;
    self->_.d_cap.height = camera_info.MaxHeight;

    if (camera_info.SupportedBins[0] == 0) {
        self->_.d_cap.binning_available = false;
    } else {
        n = 0;
        for (i = 0; i < 16; i++) {
            if (camera_info.SupportedBins[i] == 0) {
                break;
            }
            n++;
        }
        self->_.d_cap.n_x_binning = n;
        self->_.d_cap.n_y_binning = n;
        self->_.d_cap.x_binning_array = (uint32_t *) Malloc(sizeof(uint32_t) * n);
        self->_.d_cap.y_binning_array = (uint32_t *) Malloc(sizeof(uint32_t) * n);
        for (i = 0; i < n; i++) {
            self->_.d_cap.x_binning_array[i] = camera_info.SupportedBins[i];
            self->_.d_cap.y_binning_array[i] = camera_info.SupportedBins[i];
        }
    }

    if (camera_info.SupportedVideoFormat[0] == ASI_IMG_END) {
        self->_.d_cap.pixel_format_available == false;
    } else {
        n = 0;
        for (i = 0; i < 8; i++) {
            if (camera_info.SupportedVideoFormat[i] == ASI_IMG_END) {
                break;
            }
            n++;
        }
        self->_.d_cap.n_pixel_format = n;
        self->_.d_cap.pixel_format_array = (uint32_t *) Malloc(sizeof(uint32_t) * n);
        for (i = 0; i < n; i++) {
            switch (camera_info.SupportedVideoFormat[i]) {
                case ASI_IMG_RAW8:
                    self->_.d_cap.pixel_format_array[i] = DETECTOR_PIXEL_FORMAT_MONO_8;
                    break;
                case ASI_IMG_RGB24:
                    self->_.d_cap.pixel_format_array[i] = DETECTOR_PIXEL_FORMAT_RGB_24;
                    break;
                case ASI_IMG_RAW16:
                    self->_.d_cap.pixel_format_array[i] = DETECTOR_PIXEL_FORMAT_MONO_16;
                    self->_.d_param.pixel_format = DETECTOR_PIXEL_FORMAT_MONO_16;
                    break;
                default:
                    self->_.d_cap.pixel_format_array[i] = DETECTOR_PIXEL_FORMAT_UNKNOWN;
                    break;
            }
        }
    }
    
    if (camera_info.IsCoolerCam) {
        self->_.d_cap.cooling_available = true;
    } else {
        self->_.d_cap.cooling_available = false;
    }

    if ((ret = ASIGetNumOfControls(self->camera_id, &n)) == ASI_SUCCESS) {
        for (i = 0; i < n; i++) {
            if ((ret = ASIGetControlCaps(self->camera_id, i, &control_caps)) != ASI_SUCCESS) {
                continue;
            }
            switch (control_caps.ControlType) {
                case ASI_GAIN:
                    if (control_caps.IsWritable) {
                        self->_.d_cap.gain_available = true;
                    } else {
                        self->_.d_cap.gain_available = false;
                    }
                    self->_.d_cap.gain_min = control_caps.MinValue;
                    self->_.d_cap.gain_max = control_caps.MaxValue;
                    self->_.d_param.gain = control_caps.DefaultValue;
                    break;
                case ASI_EXPOSURE:
                    if (control_caps.IsWritable) {
                        self->_.d_cap.exposure_time_available = true;
                    } else {
                        self->_.d_cap.exposure_time_available = false;
                    }
                    self->_.d_cap.exposure_time_min = control_caps.MinValue / 1000.;
                    self->_.d_cap.exposure_time_max = control_caps.MaxValue / 1000.;
                    self->_.d_param.exposure_time = control_caps.DefaultValue / 1000.;
                    break;
                case ASI_TARGET_TEMP:
                    self->_.d_cap.cooling_temperature_min = control_caps.MinValue;
                    self->_.d_cap.cooling_temperature_max = control_caps.MaxValue;
                    //self->_.d_param.temperature = control_caps.DefaultValue;
                    break;
               case ASI_HARDWARE_BIN:
                    if (control_caps.IsWritable) {
                        self->_.d_cap.binning_available = true;
                    } else {
                        self->_.d_cap.binning_available = false;
                    }
                    self->_.d_cap.x_binning_min = control_caps.MinValue;
                    self->_.d_cap.x_binning_max = control_caps.MaxValue;
                    self->_.d_cap.y_binning_min = control_caps.MinValue;
                    self->_.d_cap.y_binning_max = control_caps.MaxValue;
                    self->_.d_param.x_binning = control_caps.DefaultValue;
                    self->_.d_param.y_binning = control_caps.DefaultValue;
                    break;
                default:
                    break;
            }
        }
    }
    
    if ((ret = ASIGetStartPos(self->camera_id, &x, &y)) != ASI_SUCCESS) {
        ret = asi_error_mapping(ret);
        goto error;
    }
    self->_.d_param.x_offset = x;
    self->_.d_param.y_offset = y;
    if ((ret = ASIGetROIFormat(self->camera_id, &w, &h, &b, &t)) != ASI_SUCCESS) {
#ifdef DEBUG
        fprintf(stderr, "%s %s %d --- ASIGetROIFormat error: %d\n", __FILE__, __func__, __LINE__ - 2, ret);
#endif
        ret = asi_error_mapping(ret);
        goto error;
    }
    self->_.d_param.image_width = w;
    self->_.d_param.image_height = h;
    self->_.d_param.x_binning = b;
    self->_.d_param.y_binning = b;
    /*
    switch (t) {
        case ASI_IMG_RAW8:
            self->_.d_param.pixel_format = DETECTOR_PIXEL_FORMAT_MONO_8;
            break;
        case ASI_IMG_RGB24:
            self->_.d_param.pixel_format = DETECTOR_PIXEL_FORMAT_RGB_24;
            break;
        case ASI_IMG_RAW16:
	    self->_.d_param.pixel_format = DETECTOR_PIXEL_FORMAT_MONO_16;
            break;
        default:
            self->_.d_param.pixel_format = DETECTOR_PIXEL_FORMAT_UNKNOWN;
            break;
    }
    */
    
    if (self->_.d_param.pixel_format == DETECTOR_PIXEL_FORMAT_MONO_16) {
        t = ASI_IMG_RAW16;
    }

    if ((ret = ASISetROIFormat(self->camera_id, w, h, b, t)) != ASI_SUCCESS) {
#ifdef DEBUG
	fprintf(stderr, "%s %s %d --- ASISetROIFormat error: %d\n", __FILE__, __func__, __LINE__ - 2, ret);
#endif
        ret = asi_error_mapping(ret);
        goto error;
    }
    
    if ((ret = ASIGetControlValue(self->camera_id, ASI_COOLER_ON, &v, &B)) == ASI_SUCCESS) {
        if (v) {
            self->_.d_param.is_cooling_enable = true;
        } else {
            self->_.d_param.is_cooling_enable = false;
        }
    }
    
    if (camera_info.IsTriggerCam) {
        self->_.d_cap.trigger_available = true;
        if ((ret = ASIGetCameraSupportMode(self->camera_id, &supported_mode)) != ASI_SUCCESS){
            goto error;
        }
        n = 0;
        if (supported_mode.SupportedCameraMode[0] != ASI_MODE_END) {
            for (i = 0; i < 16; i++) {
                if (supported_mode.SupportedCameraMode[i] != ASI_MODE_END) {
                    break;
                }
                n++;
            }
            self->_.d_cap.trigger_mode_array = (uint32_t *) Malloc(sizeof(uint32_t) * n);
            for (i = 0; i < n; i++) {
                switch (supported_mode.SupportedCameraMode[i]) {
                    case ASI_MODE_NORMAL:
                        self->_.d_cap.trigger_mode_array[i] = DETECTOR_TRIGGER_MODE_DEFAULT;
                        break;
                    case ASI_MODE_TRIG_SOFT_EDGE:
                        self->_.d_cap.trigger_mode_array[i] = DETECTOR_TRIGGER_MODE_SOFTWARE_EDGE;
                        break;
                    case ASI_MODE_TRIG_SOFT_LEVEL:
                        self->_.d_cap.trigger_mode_array[i] = DETECTOR_TRIGGER_MODE_SOFTWARE_LEVEL;
                        break;
                    case ASI_MODE_TRIG_HIGH_LEVEL:
                        self->_.d_cap.trigger_mode_array[i] = DETECTOR_TRIGGER_MODE_HARDWARE_LEVEL_HIGH;
                        break;
                    case ASI_MODE_TRIG_LOW_LEVEL:
                        self->_.d_cap.trigger_mode_array[i] = DETECTOR_TRIGGER_MODE_HARDWARE_LEVEL_LOW;
                        break;
                    case ASI_MODE_TRIG_RISE_EDGE:
                        self->_.d_cap.trigger_mode_array[i] = DETECTOR_TRIGGER_MODE_HARDWARE_EDGE_RISE;
                        break;
                    case ASI_MODE_TRIG_FALL_EDGE:
                        self->_.d_cap.trigger_mode_array[i] = DETECTOR_TRIGGER_MODE_HARDWARE_EDGE_FALL;
                        break;
                    default:
                        break;
                }
            }
        }
        
        if ((ret = ASISetCameraMode(self->camera_id, ASI_MODE_NORMAL)) != ASI_SUCCESS) {
#ifdef DEBUG
            fprintf(stderr, "%s %s %d --- ASISetCameraMode error: %d\n", __FILE__, __func__, __LINE__ - 2, ret);
#endif
            ret = asi_error_mapping(ret);
            goto error;
        }
    } else {
        self->_.d_cap.trigger_available = false;
    }
    
    if (self->_.d_cap.cooling_available && self->_.d_param.temperature < 50.) {
        v = ASI_TRUE;
        if ((ret = ASISetControlValue(self->camera_id, ASI_COOLER_ON, ASI_TRUE, ASI_FALSE)) != ASI_SUCCESS) {
#ifdef DEBUG
            fprintf(stderr, "%s %s %d --- ASISetControlValue error: %d\n", __FILE__, __func__, __LINE__ - 2, ret);
#endif
            ret = asi_error_mapping(ret);
            goto error;
        }
        self->_.d_param.is_cooling_enable = true;
        v = (long) self->_.d_param.temperature;
        if ((ret = ASISetControlValue(self->camera_id, ASI_TARGET_TEMP, v, ASI_FALSE)) != ASI_SUCCESS) {
#ifdef DEBUG
            fprintf(stderr, "%s %s %d --- ASISetControlValue error: %d\n", __FILE__, __func__, __LINE__ - 2, ret);
#endif
            ret = asi_error_mapping(ret);
            goto error;
        }
    }
    Pthread_mutex_lock(&self->_.d_state.mtx);
    self->_.d_state.state = DETECTOR_STATE_IDLE; 
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
error:
    return ret;
}

static int
ASICamera_set_exposure_time_nl(struct ASICamera *_self, double exposure_time)
{
    struct ASICamera *self = cast(ASICamera(), _self); 

    unsigned int state;
    uint16_t options;
    ASI_ERROR_CODE (*ASISetControlValue)(int, ASI_CONTROL_TYPE, long, ASI_BOOL);
    long exposure_time_us = (long) exposure_time * 1000000.;
    int ret;
    
    ASISetControlValue = dlsym(self->dlh, "ASISetControlValue");
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    
    if (exposure_time > self->_.d_cap.exposure_time_max || exposure_time < self->_.d_cap.exposure_time_min) {
        ret = AAOS_EINVAL;
        goto error;
    }
    if ((state&DETECTOR_STATE_MALFUNCTION) && !(options&DETECTOR_OPTION_IGNORE_DEVMAL)) {
        ret = AAOS_EDEVMAL;
        goto error;
    }
    if ((state&DETECTOR_STATE_EXPOSING) || (state&DETECTOR_STATE_READING)) {
        if (options&DETECTOR_OPTION_NOWAIT) {
            ret = AAOS_EBUSY;
            goto error;
        }
    }
    if ((ret = ASISetControlValue(self->camera_id, ASI_EXPOSURE, exposure_time_us, ASI_FALSE)) != ASI_SUCCESS) {
        ret = asi_error_mapping(ret);
        goto error;
    }
error:
    self->_.d_param.exposure_time = exposure_time;
    
    return ret;
}

static int
ASICamera_get_exposure_time(void *_self, double *exposure_time)
{
    struct ASICamera *self = cast(ASICamera(), _self);

    unsigned int state;
    uint16_t options;
    ASI_ERROR_CODE (*ASIGetControlValue)(int, ASI_CONTROL_TYPE, long *, ASI_BOOL *);
    long exposure_time_ms;
    ASI_BOOL is_auto;
    int ret;

    ASIGetControlValue = dlsym(self->dlh, "ASIGetControlValue");

    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    if ((state&DETECTOR_STATE_MALFUNCTION) && !(options&DETECTOR_OPTION_IGNORE_DEVMAL)) {
        ret = AAOS_EDEVMAL;
        goto error;
    } else if (state&DETECTOR_STATE_OFFLINE) {
        ret = AAOS_EPWROFF;
        goto error;
    } else if (state&DETECTOR_STATE_UNINITIALIZED) {
        ret = AAOS_EUNINIT;
        goto error;
    } else if ((state&DETECTOR_STATE_EXPOSING) || (state&DETECTOR_STATE_READING)) {
        if (options&DETECTOR_OPTION_NOWAIT) {
            ret = AAOS_EBUSY;
            goto error;
        }
        while ((state&DETECTOR_STATE_EXPOSING) || (state&DETECTOR_STATE_READING)) {
            Pthread_cond_wait(&self->_.d_state.cond, &self->_.d_state.mtx);
        }
    }
    if ((ret = ASIGetControlValue(self->camera_id, ASI_EXPOSURE, &exposure_time_ms, &is_auto)) != ASI_SUCCESS) {
        ret = asi_error_mapping(ret);
        goto error;
    }
    *exposure_time = exposure_time_ms / 1000.;
error:
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    return ret;

} 

static int
ASICamera_set_exposure_time(void *_self, double exposure_time)
{
    struct ASICamera *self = cast(ASICamera(), _self);
    
    unsigned int state;
    uint16_t options;
    ASI_ERROR_CODE (*ASISetControlValue)(int, ASI_CONTROL_TYPE, long, ASI_BOOL);
    long exposure_time_us = (long) exposure_time * 1000000.;
    int ret;
    
    ASISetControlValue = dlsym(self->dlh, "ASISetControlValue");
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    if (exposure_time > self->_.d_cap.exposure_time_max || exposure_time < self->_.d_cap.exposure_time_min) {
        ret = AAOS_EINVAL;
        goto error;
    }
    if ((state&DETECTOR_STATE_MALFUNCTION) && !(options&DETECTOR_OPTION_IGNORE_DEVMAL)) {
        ret = AAOS_EDEVMAL;
        goto error;
    } else if (state&DETECTOR_STATE_OFFLINE) {
        ret = AAOS_EPWROFF;
        goto error;
    } else if (state&DETECTOR_STATE_UNINITIALIZED) {
        ret = AAOS_EUNINIT;
        goto error;
    } else if ((state&DETECTOR_STATE_EXPOSING) || (state&DETECTOR_STATE_READING)) {
        if (options&DETECTOR_OPTION_NOWAIT) {
            ret = AAOS_EBUSY;
            goto error;
        }
        while ((state&DETECTOR_STATE_EXPOSING) || (state&DETECTOR_STATE_READING)) {
            Pthread_cond_wait(&self->_.d_state.cond, &self->_.d_state.mtx);
        }
    }
    if ((ret = ASISetControlValue(self->camera_id, ASI_EXPOSURE, exposure_time_us, ASI_FALSE)) != ASI_SUCCESS) {
        ret = asi_error_mapping(ret);
        goto error;
    }
    self->_.d_param.exposure_time = exposure_time;
error:
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    return ret;
}

static void
ASICamera_info_json(struct ASICamera *self, void *buffer, size_t size, ASI_CAMERA_INFO *camera_info)
{
	
    cJSON *root_json, *asi_json, *capability_json, *array_json;
    char *string = NULL;
    size_t i;
	
    root_json = cJSON_CreateObject();
    cJSON_AddStringToObject(root_json, "name", self->_.name);
    if (self->_.description != NULL) {
	cJSON_AddStringToObject(root_json, "description", self->_.description);
    }
	
	asi_json = cJSON_CreateObject();
	cJSON_AddStringToObject(asi_json, "name", camera_info->Name);
	cJSON_AddNumberToObject(asi_json, "camera_index", self->camera_index);
	cJSON_AddNumberToObject(asi_json, "camera_id", self->camera_id);
	if (camera_info->IsColorCam) {
	    cJSON_AddStringToObject(asi_json, "ColorCamera", "true");
	    switch (camera_info->BayerPattern) {
			case ASI_BAYER_RG:
			    cJSON_AddStringToObject(asi_json, "BayerPattern", "ASI_BAYER_RG");
		        break;
			case ASI_BAYER_BG:
			    cJSON_AddStringToObject(asi_json, "BayerPattern", "ASI_BAYER_BG");
		        break;
			case ASI_BAYER_GR:
			    cJSON_AddStringToObject(asi_json, "BayerPattern", "ASI_BAYER_GR");
			    break;
			case ASI_BAYER_GB:
			    cJSON_AddStringToObject(asi_json, "BayerPattern", "ASI_BAYER_GB");
		        break;
			default:
			    cJSON_AddStringToObject(asi_json, "BayerPattern", "Unknow");
		        break;	
	    }
	} else {
		cJSON_AddStringToObject(asi_json, "ColorCamera", "false");	
	}
	if (camera_info->MechanicalShutter) {
	    cJSON_AddStringToObject(asi_json, "MechanicalShutter", "true");	
	} else {
		cJSON_AddStringToObject(asi_json, "MechanicalShutter", "false");	
	}
	if (camera_info->ST4Port) {
	    cJSON_AddStringToObject(asi_json, "ST4Port", "true");	
	} else {
		cJSON_AddStringToObject(asi_json, "ST4Port", "false");	
	}
    if (camera_info->IsCoolerCam) {
        cJSON_AddStringToObject(asi_json, "CoolerCamera", "true");
    } else {
        cJSON_AddStringToObject(asi_json, "CoolerCamera", "false");
    }
	if (camera_info->IsUSB3Host) {
	    cJSON_AddStringToObject(asi_json, "IsUSB3Host", "true");
	} else {
		cJSON_AddStringToObject(asi_json, "IsUSB3Host", "false");
	}
    if (camera_info->IsUSB3Camera) {
        cJSON_AddStringToObject(asi_json, "IsUSB3Camera", "true");
    } else {
        cJSON_AddStringToObject(asi_json, "IsUSB3Camera", "false");
    }
    if (camera_info->IsTriggerCam) {
        cJSON_AddStringToObject(asi_json, "IsTriggerCam", "true");
    } else {
        cJSON_AddStringToObject(asi_json, "IsTriggerCam", "false");
    }
    cJSON_AddNumberToObject(asi_json, "ElecPerADU", camera_info->ElecPerADU);
    cJSON_AddNumberToObject(asi_json, "BitDepth", camera_info->BitDepth);
    
	
	cJSON_AddItemToObject(root_json, "ASICamera", asi_json);
 	
	capability_json = cJSON_CreateObject();
	cJSON_AddNumberToObject(capability_json, "width", self->_.d_cap.width);
	cJSON_AddNumberToObject(capability_json, "heigth", self->_.d_cap.height);
	cJSON_AddNumberToObject(capability_json, "x_n_chip", self->_.d_cap.x_n_chip);
	cJSON_AddNumberToObject(capability_json, "y_n_chip", self->_.d_cap.y_n_chip);
	cJSON_AddNumberToObject(capability_json, "n_chip", self->_.d_cap.n_chip);
	if (self->_.d_cap.flip_map != NULL) {
		
	}
	if (self->_.d_cap.mirror_map != NULL) {
		
	}	
	
	if (self->_.d_cap.binning_available) {
		cJSON_AddStringToObject(capability_json, "binning_available", "true");
        if (self->_.d_cap.x_binning_array != NULL && self->_.d_cap.y_binning_array != NULL) {
            char binning_buffer[BUFSIZE];
            FILE *fp;
			size_t i;
            fp = fmemopen(binning_buffer, BUFSIZE, "w");
			for (i = 0; i < self->_.d_cap.n_x_binning - 1; i++) {
				fprintf(fp, "(%d,%d) ", self->_.d_cap.x_binning_array[i], self->_.d_cap.y_binning_array[i]);
			}
			fprintf(fp, "(%d,%d)", self->_.d_cap.x_binning_array[self->_.d_cap.n_x_binning - 1], self->_.d_cap.y_binning_array[self->_.d_cap.n_x_binning - 1]);
			cJSON_AddStringToObject(capability_json, "binning_array", binning_buffer);
        } else {
			cJSON_AddNumberToObject(capability_json, "x_binning_min", self->_.d_cap.x_binning_min);
			cJSON_AddNumberToObject(capability_json, "x_binning_max", self->_.d_cap.x_binning_max);
			cJSON_AddNumberToObject(capability_json, "y_binning_max", self->_.d_cap.y_binning_min);
			cJSON_AddNumberToObject(capability_json, "y_binning_max", self->_.d_cap.y_binning_max);
        }		
	} else {
		cJSON_AddStringToObject(capability_json, "binning_available", "false");
	}
	
	if (self->_.d_cap.offset_available) {
		cJSON_AddStringToObject(capability_json, "offset_available", "true");
	} else {
		cJSON_AddStringToObject(capability_json, "offset_available", "false");
	}
	
	if (self->_.d_cap.gain_available) {
		cJSON_AddStringToObject(capability_json, "gain_available", "true");
		if (self->_.d_cap.gain_array == NULL) {
			cJSON_AddNumberToObject(capability_json, "gain_min", self->_.d_cap.gain_min);
			cJSON_AddNumberToObject(capability_json, "gain_max", self->_.d_cap.gain_max);
		} else {
			array_json = cJSON_CreateArray();
			for (i = 0; i < self->_.d_cap.n_gain; i++) {
				cJSON_AddItemToArray(array_json, cJSON_CreateNumber(self->_.d_cap.gain_array[i]));
			}
			cJSON_AddItemToObject(capability_json, "gain_array", array_json);
		}
	} else {
		cJSON_AddStringToObject(capability_json, "gain_available", "false");
	}
	if (self->_.d_cap.auto_gain_available) {
		cJSON_AddStringToObject(capability_json, "auto_gain_available", "true");
		cJSON_AddNumberToObject(capability_json, "auto_gain_min", self->_.d_cap.auto_gain_min);
		cJSON_AddNumberToObject(capability_json, "auto_gain_max", self->_.d_cap.auto_gain_max);
	} else {
		cJSON_AddStringToObject(capability_json, "auto_gain_available", "false");
	}
	
	if (self->_.d_cap.exposure_time_available) {
		cJSON_AddStringToObject(capability_json, "exposure_time_available", "true");
		if (self->_.d_cap.exposure_time_array == NULL) {
			cJSON_AddNumberToObject(capability_json, "exposure_time_min", self->_.d_cap.exposure_time_min);
			cJSON_AddNumberToObject(capability_json, "exposure_time_max", self->_.d_cap.exposure_time_max);
		} else {
			array_json = cJSON_CreateArray();
			for (i = 0; i < self->_.d_cap.n_exposure_time; i++) {
				cJSON_AddItemToArray(array_json, cJSON_CreateNumber(self->_.d_cap.exposure_time_array[i]));
			}
			cJSON_AddItemToObject(capability_json, "exposure_time_array", array_json);
		}
	} else {
		cJSON_AddStringToObject(capability_json, "exposure_time_available", "false");
	}
	if (self->_.d_cap.auto_frame_rate_available) {
		cJSON_AddStringToObject(capability_json, "auto_exposure_time_available", "true");
		cJSON_AddNumberToObject(capability_json, "auto_exposure_time_min", self->_.d_cap.auto_exposure_time_min);
		cJSON_AddNumberToObject(capability_json, "auto_exposure_time_max", self->_.d_cap.auto_exposure_time_max);
	} else {
		cJSON_AddStringToObject(capability_json, "auto_exposure_time_available", "false");
	}
	
	if (self->_.d_cap.frame_rate_available) {
		cJSON_AddStringToObject(capability_json, "frame_rate_available", "true");
		if (self->_.d_cap.frame_rate_array == NULL) {
			cJSON_AddNumberToObject(capability_json, "frame_rate_min", self->_.d_cap.frame_rate_min);
			cJSON_AddNumberToObject(capability_json, "frame_rate_max", self->_.d_cap.frame_rate_max);
		} else {
			array_json = cJSON_CreateArray();
			for (i = 0; i < self->_.d_cap.n_frame_rate; i++) {
				cJSON_AddItemToArray(array_json, cJSON_CreateNumber(self->_.d_cap.frame_rate_array[i]));
			}
			cJSON_AddItemToObject(capability_json, "frame_rate_array", array_json);
		}
	} else {
		cJSON_AddStringToObject(capability_json, "frame_rate_available", "false");
	}
	if (self->_.d_cap.auto_frame_rate_available) {
		cJSON_AddStringToObject(capability_json, "auto_frame_rate_available", "true");
		cJSON_AddNumberToObject(capability_json, "auto_frame_rate_min", self->_.d_cap.auto_frame_rate_min);
		cJSON_AddNumberToObject(capability_json, "auto_frame_rate_max", self->_.d_cap.auto_frame_rate_max);
	} else {
		cJSON_AddStringToObject(capability_json, "auto_frame_rate_available", "false");
	}
	
	if (self->_.d_cap.pixel_format_available) {
		cJSON_AddStringToObject(capability_json, "pixel_format_available", "true");
		array_json = cJSON_CreateArray();
		for (i = 0; i < self->_.d_cap.n_pixel_format; i++) {
			switch (self->_.d_cap.pixel_format_array[i]) {
				case DETECTOR_PIXEL_FORMAT_MONO_8:
				    cJSON_AddItemToArray(array_json, cJSON_CreateString("MONO_8"));
				    break;
				case DETECTOR_PIXEL_FORMAT_MONO_10:
				    cJSON_AddItemToArray(array_json, cJSON_CreateString("MONO_10"));
				    break;
				case DETECTOR_PIXEL_FORMAT_MONO_10_PACKED:
				    cJSON_AddItemToArray(array_json, cJSON_CreateString("MONO_10_PACKED"));
				    break;
				case DETECTOR_PIXEL_FORMAT_MONO_12:
				    cJSON_AddItemToArray(array_json, cJSON_CreateString("MONO_12"));
				    break;
				case DETECTOR_PIXEL_FORMAT_MONO_12_PACKED:
				    cJSON_AddItemToArray(array_json, cJSON_CreateString("MONO_12_PACKED"));
				    break;
				case DETECTOR_PIXEL_FORMAT_MONO_14:
				    cJSON_AddItemToArray(array_json, cJSON_CreateString("MONO_14"));
				    break;
				case DETECTOR_PIXEL_FORMAT_MONO_14_PACKED:
				    cJSON_AddItemToArray(array_json, cJSON_CreateString("MONO_14_PACKED"));
				    break;
				case DETECTOR_PIXEL_FORMAT_MONO_16:
				    cJSON_AddItemToArray(array_json, cJSON_CreateString("MONO_16"));
				    break;
				case DETECTOR_PIXEL_FORMAT_MONO_18:
				    cJSON_AddItemToArray(array_json, cJSON_CreateString("MONO_18"));
				    break;
				case DETECTOR_PIXEL_FORMAT_MONO_18_PACKED:
				    cJSON_AddItemToArray(array_json, cJSON_CreateString("MONO_18_PACKED"));
				    break;
				case DETECTOR_PIXEL_FORMAT_MONO_24:
				    cJSON_AddItemToArray(array_json, cJSON_CreateString("MONO_24"));
				    break;
				case DETECTOR_PIXEL_FORMAT_MONO_24_PACKED:
				    cJSON_AddItemToArray(array_json, cJSON_CreateString("MONO_24_PACKED"));
				    break;
				case DETECTOR_PIXEL_FORMAT_MONO_32:
				    cJSON_AddItemToArray(array_json, cJSON_CreateString("MONO_32"));
				    break;
				case DETECTOR_PIXEL_FORMAT_MONO_64:
				    cJSON_AddItemToArray(array_json, cJSON_CreateString("MONO_64"));
				    break;
				case DETECTOR_PIXEL_FORMAT_RGB_24:
				    cJSON_AddItemToArray(array_json, cJSON_CreateString("RGB_24"));
				    break;
				default:
				    break;
			}
		}
		cJSON_AddItemToObject(capability_json, "pixel_format_array", array_json);
	} else {
		cJSON_AddStringToObject(capability_json, "pixel_format_available", "false");
	}
	
	if (self->_.d_cap.readout_rate_available) {
		cJSON_AddStringToObject(capability_json, "readout_rate_available", "true");
		if (self->_.d_cap.readout_rate_array == NULL) {
			cJSON_AddNumberToObject(capability_json, "readout_rate_min", self->_.d_cap.readout_rate_min);
			cJSON_AddNumberToObject(capability_json, "readout_rate_max", self->_.d_cap.readout_rate_max);
		} else {
			array_json = cJSON_CreateArray();
			for (i = 0; i < self->_.d_cap.n_readout_rate; i++) {
				cJSON_AddItemToArray(array_json, cJSON_CreateNumber(self->_.d_cap.readout_rate_array[i]));
			}
			cJSON_AddItemToObject(capability_json, "readout_rate_array", array_json);
		}
	} else {
		cJSON_AddStringToObject(capability_json, "readout_rate_available", "false");
	}
	if (self->_.d_cap.auto_readout_rate_available) {
		cJSON_AddStringToObject(capability_json, "auto_readout_rate_available", "true");
		cJSON_AddNumberToObject(capability_json, "auto_readout_rate_min", self->_.d_cap.auto_readout_rate_min);
		cJSON_AddNumberToObject(capability_json, "auto_readout_rate_max", self->_.d_cap.auto_readout_rate_max);
	} else {
		cJSON_AddStringToObject(capability_json, "auto_readout_rate_available", "false");
	}
	
	if (self->_.d_cap.cooling_available) {
		cJSON_AddStringToObject(capability_json, "cooling_available", "true");
		cJSON_AddNumberToObject(capability_json, "cooling_temperature_min", self->_.d_cap.cooling_temperature_min);
		cJSON_AddNumberToObject(capability_json, "cooling_temperature_max", self->_.d_cap.cooling_temperature_max);
			
	} else {
		cJSON_AddStringToObject(capability_json, "cooling_available", "false");
	}
	if (self->_.d_cap.auto_cooling_available) {
		cJSON_AddStringToObject(capability_json, "auto_cooling_available", "true");
		cJSON_AddNumberToObject(capability_json, "auto_cooling_temperature_min", self->_.d_cap.auto_cooling_temperature_min);
		cJSON_AddNumberToObject(capability_json, "auto_cooling_temperature_max", self->_.d_cap.auto_cooling_temperature_max);
	} else {
		cJSON_AddStringToObject(capability_json, "auto_cooling_available", "false");
	}
	cJSON_AddItemToObject(root_json, "capability", capability_json);
    
    string = cJSON_Print(root_json);
	cJSON_Delete(root_json);
    snprintf(buffer, size, "%s", string);
    free(string);
}

static int
ASICamera_info(void *_self, void *buffer, size_t size)
{
	struct ASICamera *self = cast(ASICamera(), _self);
	
	ASI_CAMERA_INFO camera_info;
	int ret = AAOS_OK;
	
	ASI_ERROR_CODE (*ASIGetCameraProperty)(ASI_CAMERA_INFO *, int);
	
	ASIGetCameraProperty = dlsym(self->dlh, "ASIGetCameraProperty");
	
	if ((ret = ASIGetCameraProperty(&camera_info, self->camera_index)) != ASI_SUCCESS) {
	    ret = asi_error_mapping(ret);
	    goto error;
	}
	ASICamera_info_json(self, buffer, size, &camera_info);
	
error:
    return ret;
}

static void
ASICamera_status_json(struct ASICamera *self, void *buffer, size_t size)
{
    unsigned int state, options;
    uint32_t image_width, image_height, x_offset, y_offset, x_binning, y_binning;
    double exposure_time, frame_rate, gain, readout_rate, cooling_temperature;
    bool auto_exposure_time_enable, auto_frame_rate_enable, auto_gain_enable, auto_readout_rate_enable, auto_cooling_enable;
    long value;
    ASI_BOOL is_auto;
    cJSON *root_json;
    char *string;
    
    ASI_ERROR_CODE (*ASIGetControlValue)(int, ASI_CONTROL_TYPE, long *, ASI_BOOL *);
    ASIGetControlValue = dlsym(self->dlh, "ASIGetControlValue");
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    image_width = self->_.d_param.image_width;
    image_height = self->_.d_param.image_height;
    x_offset = self->_.d_param.x_offset;
    y_offset = self->_.d_param.y_offset;
    x_binning = self->_.d_param.x_binning;
    y_binning = self->_.d_param.y_binning;
    auto_exposure_time_enable = self->_.d_param.auto_exposure_time_enable;
    exposure_time = self->_.d_param.exposure_time;
    auto_frame_rate_enable = self->_.d_param.auto_frame_rate_enable;
    frame_rate = self->_.d_param.frame_rate;
    auto_gain_enable = self->_.d_param.auto_gain_enable;
    gain = self->_.d_param.gain;
    auto_readout_rate_enable = self->_.d_param.auto_readout_rate_enable;
    readout_rate = self->_.d_param.readout_rate;
    auto_cooling_enable = self->_.d_param.auto_cooling_enable;
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    root_json = cJSON_CreateObject();
    cJSON_AddStringToObject(root_json, "name", self->_.name);
    if (state&DETECTOR_STATE_MALFUNCTION) {
        cJSON_AddStringToObject(root_json, "status", "MALFUNCTION");
    }
    switch (state&(~DETECTOR_STATE_MALFUNCTION)) {
        case DETECTOR_STATE_IDLE:
            cJSON_AddStringToObject(root_json, "state", "IDLE");
            break;
        case DETECTOR_STATE_OFFLINE:
            cJSON_AddStringToObject(root_json, "state", "OFFLINE");
            break;
        case DETECTOR_STATE_UNINITIALIZED:
            cJSON_AddStringToObject(root_json, "state", "UNINITIALIZED");
            break;
        case DETECTOR_STATE_EXPOSING:
            cJSON_AddStringToObject(root_json, "state", "EXPOSING");
            break;
        case DETECTOR_STATE_READING:
            cJSON_AddStringToObject(root_json, "state", "READING");
            break;
        default:
            break;
    }
    
    cJSON_AddNumberToObject(root_json, "image_width", (double) image_width);
    cJSON_AddNumberToObject(root_json, "image_height", (double) image_height);
    cJSON_AddNumberToObject(root_json, "x_offset", (double) x_offset);
    cJSON_AddNumberToObject(root_json, "y_offset", (double) y_offset);
    cJSON_AddNumberToObject(root_json, "x_binning", (double) x_binning);
    cJSON_AddNumberToObject(root_json, "y_binning", (double) y_binning);
    if (self->_.d_cap.auto_exposure_time_available) {
        if (auto_exposure_time_enable) {
            cJSON_AddStringToObject(root_json, "auto_exposure_time_enable", "true");
        } else {
            cJSON_AddStringToObject(root_json, "auto_exposure_time_enable", "false");
        }
    }
    cJSON_AddNumberToObject(root_json, "exposure_time", exposure_time);
    if (self->_.d_cap.auto_frame_rate_available) {
        if (auto_frame_rate_enable) {
            cJSON_AddStringToObject(root_json, "auto_frame_rate_enable", "true");
        } else {
            cJSON_AddStringToObject(root_json, "auto_frame_rate_enable", "false");
        }
    }
    cJSON_AddNumberToObject(root_json, "frame_rate", frame_rate);
    if (self->_.d_cap.auto_frame_rate_available) {
        if (auto_frame_rate_enable) {
            cJSON_AddStringToObject(root_json, "auto_frame_rate_enable", "true");
        } else {
            cJSON_AddStringToObject(root_json, "auto_frame_rate_enable", "false");
        }
    }
    cJSON_AddNumberToObject(root_json, "gain", gain);
    if (self->_.d_cap.auto_gain_available) {
        if (auto_gain_enable) {
            cJSON_AddStringToObject(root_json, "auto_gain_enable", "true");
        } else {
            cJSON_AddStringToObject(root_json, "auto_gain_enable", "false");
        }
    }
    if (self->_.d_cap.auto_readout_rate_available) {
        if (auto_readout_rate_enable) {
            cJSON_AddStringToObject(root_json, "auto_readout_rate_enable", "true");
        } else {
            cJSON_AddStringToObject(root_json, "auto_readout_rate_enable", "false");
        }
    }
    cJSON_AddNumberToObject(root_json, "readout_rate", readout_rate);

    if (self->_.d_param.is_cooling_enable) {
	cJSON_AddStringToObject(root_json, "cooling_enabled", "true");
    } else {
	cJSON_AddStringToObject(root_json, "cooling_enabled", "false");
    }

    if (self->_.d_cap.auto_cooling_available) {
        if (auto_cooling_enable) {
            cJSON_AddStringToObject(root_json, "auto_cooling_enabled", "true");
        } else {
            cJSON_AddStringToObject(root_json, "auto_cooling_enabled", "false");
        }
    }

    if (self->_.d_cap.cooling_available && self->_.d_param.is_cooling_enable && !self->_.d_cap.auto_cooling_available) {
        cJSON_AddNumberToObject(root_json, "setting_temperature", self->_.d_param.temperature);
    }
    
    if (ASIGetControlValue(self->camera_id, ASI_TEMPERATURE, &value, &is_auto) == ASI_SUCCESS) {
        cJSON_AddNumberToObject(root_json, "actual_temperature", value/10.);
    }
    
    string = cJSON_Print(root_json);
    cJSON_Delete(root_json);
    snprintf(buffer, size, "%s", string);
    free(string);
}

static int
ASICamera_status(void *_self, void *buffer, size_t size)
{
    struct ASICamera *self = cast(ASICamera(), _self);
    
    ASICamera_status_json(self, buffer, size);
    
    return AAOS_OK;
}

struct ASICameraExposureArg {
    struct ASICamera *detector;
    void *rpc;
    void *string; /* serialized header data string. */
    unsigned int format; /* string format */
};

static void *
ASICamera_process_image_thr(void *arg)
{
    struct ASICameraExposureArg *myarg = (struct ASICameraExposureArg *) arg;
    struct ASICamera *detector = myarg->detector;
    void *string = myarg->string;
    void *rpc = myarg->rpc;
    int format = myarg->format;
    uint16_t options = detector->_.d_state.options;
    
    struct DetectorDataFrame *data;
    size_t i, n, width, height;
    char filename[FILENAMESIZE];
    char buf[TIMESTAMPSIZE];
    fitsfile *fptr = NULL;
    int bitpix, datatype, naxis = 2, status = 0;
    long naxes[2];
    uint32_t pixel_format;
    struct timespec tp;
    struct tm tm_buf;
    long value;
    ASI_BOOL is_auto;
    ASI_ERROR_CODE (*ASIGetControlValue)(int, ASI_CONTROL_TYPE, long *, ASI_BOOL *);
    
    ASIGetControlValue = dlsym(detector->dlh, "ASIGetControlValue");
    
    for (; ;) {
        data = threadsafe_queue_wait_and_pop(detector->_.d_proc.queue);
        if (data != NULL) {
            if (data->buffer != NULL) {
                free(data->buffer);
                free(data);
            } else {
                break;
            }
        }
    }
    
    n = data->n;
    width = data->width;
    height = data->height;
    pixel_format = data->pixel_format;
    naxes[0] = width;
    naxes[1] = height;
    
    tp.tv_sec = data->tp.tv_sec;
    tp.tv_nsec = data->tp.tv_nsec;
    switch (pixel_format) {
        case DETECTOR_PIXEL_FORMAT_MONO_8:
            bitpix = BYTE_IMG;
            datatype = TBYTE;
            break;
        case DETECTOR_PIXEL_FORMAT_MONO_16:
            bitpix = USHORT_IMG;
            datatype = TUSHORT;
        default:
            break;
    }
    
    if (!(options&DETECTOR_OPTION_NOTIFY_EACH_COMPLETION)) {
        detector->_.d_proc.name_convention(detector, filename, FILENAMESIZE, 1, 1, &tp);
        if (detector->_.d_proc.tpl_fptr != NULL) {
            fits_create_file(&fptr, filename, &status);
            if (status != 0) {
#ifdef DEBUG
                fprintf(stderr, "%s %s %d --- fits_create_file error: %d\n", __FILE__, __func__, __LINE__ - 2, status);
#endif
            }
            fits_copy_file(detector->_.d_proc.tpl_fptr, fptr, 0, 1, 1, &status);
            if (status != 0) {
#ifdef DEBUG
                fprintf(stderr, "%s %s %d --- fits_copy_file error: %d\n", __FILE__, __func__, __LINE__ - 2, status);
#endif
            }
        } else {
            fits_create_template(&fptr, filename, detector->_.d_proc.tpl_filename, &status);
            if (status != 0) {
#ifdef DEBUG
                fprintf(stderr, "%s %s %d --- fits_create_template error: %d\n", __FILE__, __func__, __LINE__ - 2, status);
#endif
            }
        }
        detector->_.d_proc.pre_acquisition(detector, filename, fptr, &data->tp);
        detector->_.d_proc.img_fptr = fptr;
    }
    
    for (i = 0; i < n; i++) {
        data = threadsafe_queue_wait_and_pop(detector->_.d_proc.queue);
        if (data == NULL) {
            break;
        }
        if (data->buffer == NULL) {
            free(data);
            break;
        }
        if (options&DETECTOR_OPTION_NOTIFY_EACH_COMPLETION) {
            detector->_.d_proc.name_convention(detector, filename, FILENAMESIZE, i + 1, n, &tp);
            if (detector->_.d_proc.tpl_fptr != NULL) {
                fits_create_file(&fptr, filename, &status);
                if (status != 0) {
#ifdef DEBUG
                    fprintf(stderr, "%s %s %d --- fits_create_file error: %d\n", __FILE__, __func__, __LINE__ - 2, status);
#endif
                }
                fits_copy_file(detector->_.d_proc.tpl_fptr, fptr, 0, 1, 1, &status);
                if (status != 0) {
#ifdef DEBUG
                    fprintf(stderr, "%s %s %d --- fits_copy_file error: %d\n", __FILE__, __func__, __LINE__ - 2, status);
#endif
                }
            } else {
                fits_create_template(&fptr, filename, detector->_.d_proc.tpl_filename, &status);
                if (status != 0) {
#ifdef DEBUG
                    fprintf(stderr, "%s %s %d --- fits_create_template error: %d\n", __FILE__, __func__, __LINE__ - 2, status);
#endif
                }
            }
            detector->_.d_proc.pre_acquisition(detector, filename, fptr, &data->tp);
            detector->_.d_proc.img_fptr = fptr;
        }
        
        
        fits_create_img(fptr, bitpix, naxis, naxes, &status);
        if (status != 0) {
#ifdef DEBUG
            fprintf(stderr, "%s %s %d --- fits_create_img error: %d\n", __FILE__, __func__, __LINE__ - 2, status);
#endif
        }
        gmtime_r(&data->tp.tv_sec, &tm_buf);
        strftime(buf, TIMESTAMPSIZE, "%Y-%m-%d", &tm_buf);
        fits_update_key_str(fptr, "DATE-OBS", buf, "end date of this frame", &status);
        strftime(buf, TIMESTAMPSIZE, "%H:%M:%S", &tm_buf);
        snprintf(buf + strlen(buf), TIMESTAMPSIZE - strlen(buf), ".%03d", (int) floor(data->tp.tv_nsec / 1000000));
        fits_update_key_str(fptr, "TIME-OBS", buf, "end time of this frame", &status);
        if (ASIGetControlValue(detector->camera_id, ASI_GAIN, &value, &is_auto) == ASI_SUCCESS) {
            fits_update_key_fixflt(fptr, "GAIN", value/1., 2, "commanded gain of CMOS", &status);
        }
        if (ASIGetControlValue(detector->camera_id, ASI_TEMPERATURE, &value, &is_auto) == ASI_SUCCESS) {
            fits_update_key_fixflt(fptr, "CHIPTEMP", value/10., 1, "chip temperature (in Celsius degree)", &status);
        }
        if (ASIGetControlValue(detector->camera_id, ASI_TEMPERATURE, &value, &is_auto) == ASI_SUCCESS) {
            fits_update_key_fixflt(fptr, "CHIPTEMP", value/10., 1, "chip temperature (in Celsius degree)", &status);
        }
        fits_update_key_str(fptr, "EXTNAME", "RAW", "extension name", &status);
        fits_update_key_lng(fptr, "EXTVER", i + 1, "extension version number", &status);
        fits_write_img(fptr, datatype, 1, naxes[0] * naxes[1], data->buffer, &status);
        if (status != 0) {
#ifdef DEBUG
            fprintf(stderr, "%s %s %d --- fits_write_img error: %d\n", __FILE__, __func__, __LINE__ - 2, status);
#endif
        }
        
        if (options&DETECTOR_OPTION_NOTIFY_EACH_COMPLETION) {
            detector->_.d_proc.post_acquisition(detector, filename, fptr, string, format, rpc);
            fits_close_file(fptr, &status);
            if (status != 0) {
#ifdef DEBUG
                fprintf(stderr, "fits_close_file error: %d\n", status);
#endif
            }
            detector->_.d_proc.img_fptr = NULL;
        }
        free(data->buffer);
        free(data);
    }
    
    if (!(options&DETECTOR_OPTION_NOTIFY_EACH_COMPLETION)) {
	detector->_.d_proc.post_acquisition(detector, filename, fptr, string, format, rpc);
        fits_close_file(fptr, &status);
        if (status != 0) {
#ifdef DEBUG
            fprintf(stderr, "fits_close_file error: %d\n", status);
#endif
        }
        detector->_.d_proc.img_fptr = NULL;
    }
    
    return NULL;
}

static int
ASICamera_expose_video(struct ASICamera *self, double exposure_time, uint32_t n_frame, va_list *app)
{
    unsigned int state;
    uint16_t options;
    uint32_t i;
    struct DetectorDataFrame *data;
    long length;
    int ret = AAOS_OK;
    void *rpc;
    char *string;
    int format;
    struct ASICameraExposureArg *arg;
    void *retval;
    
    rpc = va_arg(*app, void *);
    string = va_arg(*app, char *);
    
    ASI_ERROR_CODE (*ASIStartVideoCapture)(int);
    ASI_ERROR_CODE (*ASIGetVideoData)(int, unsigned char *, long, int);
    ASI_ERROR_CODE (*ASIStopVideoCapture)(int);

    ASIStartVideoCapture = dlsym(self->dlh, "ASIStartVideoCapture");
    ASIGetVideoData = dlsym(self->dlh, "ASIGetVideoData");
    ASIStopVideoCapture = dlsym(self->dlh, "ASIStopVideoCapture");
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    if ((state&DETECTOR_STATE_MALFUNCTION) && !(options&DETECTOR_OPTION_IGNORE_DEVMAL)) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        return AAOS_EDEVMAL;
    }
    
    if ((state&DETECTOR_STATE_EXPOSING) || (state&DETECTOR_STATE_READING)) {
        if (options&DETECTOR_OPTION_NOWAIT) {
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EBUSY;
        }
        while ((state&DETECTOR_STATE_EXPOSING) || (state&DETECTOR_STATE_READING)) {
            Pthread_cond_wait(&self->_.d_state.cond, &self->_.d_state.mtx);
        }
    } else if (state&DETECTOR_STATE_OFFLINE) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        return AAOS_EPWROFF;
    } else if (state&DETECTOR_STATE_UNINITIALIZED) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        return AAOS_EUNINIT;
    }
    if ((ret = ASICamera_set_exposure_time_nl(self, exposure_time)) != AAOS_OK) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        return ret;
    }
    state = DETECTOR_STATE_EXPOSING;
    self->_.d_state.state = (self->_.d_state.state&DETECTOR_STATE_MALFUNCTION) | state;
    
    Pthread_mutex_lock(&self->_.d_exp.mtx);
    self->_.d_exp.rpc = rpc;
    self->_.d_exp.request_frames = n_frame;
    self->_.d_exp.success_frames = 0;
    self->_.d_exp.count = 0;
    Pthread_mutex_unlock(&self->_.d_exp.mtx);
    arg = (struct ASICameraExposureArg *) Malloc(sizeof(struct ASICameraExposureArg));
    arg->detector = self;
    arg->rpc = rpc;
    arg->string = string;
    arg->format = format;
    Pthread_create(&self->_.d_state.tid, NULL, ASICamera_process_image_thr, (void *) arg);
    
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    switch (self->_.d_param.pixel_format) {
        case DETECTOR_PIXEL_FORMAT_MONO_8:
            length = self->_.d_param.image_width * self->_.d_param.image_height;
            break;
        case DETECTOR_PIXEL_FORMAT_MONO_16:
            length = self->_.d_param.image_width * self->_.d_param.image_height * 2;
            break;
        case DETECTOR_PIXEL_FORMAT_RGB_24:
            length = self->_.d_param.image_width * self->_.d_param.image_height * 24;
            break;
        default:
            break;
    }

    data = (struct DetectorDataFrame *) Malloc(sizeof(struct DetectorDataFrame));
    data->buffer = NULL;
    data->pixel_format = self->_.d_param.pixel_format;
    data->width = self->_.d_param.image_width;
    data->height = self->_.d_param.image_height;
    data->n = n_frame;
    data->i = 1;
    data->length = length;
    Clock_gettime(CLOCK_REALTIME, &data->tp);
    threadsafe_queue_push(self->_.d_proc.queue, data);
    
    if ((ret = ASIStartVideoCapture(self->camera_id)) != ASI_SUCCESS) {
        ret = asi_error_mapping(ret);
        threadsafe_queue_push(self->_.d_proc.queue, NULL);
        Pthread_join(self->_.d_state.tid, NULL);
        Pthread_mutex_lock(&self->_.d_exp.mtx);
        self->_.d_exp.rpc = NULL;
        self->_.d_exp.stop_flag = true;
        Pthread_mutex_unlock(&self->_.d_exp.mtx);
        Pthread_mutex_lock(&self->_.d_state.mtx);
        self->_.d_state.state = (state&DETECTOR_STATE_MALFUNCTION) | DETECTOR_STATE_IDLE;
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        Pthread_cond_broadcast(&self->_.d_state.cond);
        threadsafe_queue_push(self->_.d_proc.queue, NULL);
        Pthread_join(self->_.d_state.tid, &retval);
        Pthread_mutex_lock(&self->_.d_exp.mtx);
        self->_.d_exp.stop_flag = false;
        Pthread_mutex_unlock(&self->_.d_exp.mtx);
        free(arg);
        
        return ret;
    }

    for (i = 0; i < n_frame; i++) {
        Pthread_mutex_lock(&self->_.d_exp.mtx);
        if (self->_.d_exp.stop_flag) {
#ifdef DEBUG
            fprintf(stderr, "%s %s %d: exposure has been stopped.\n", __FILE__, __func__, __LINE__);
#endif
            self->_.d_exp.stop_flag = false;
            Pthread_mutex_unlock(&self->_.d_exp.mtx);
            Pthread_cond_broadcast(&self->_.d_exp.cond);
            ret = AAOS_ECANCELED;
            break;
        }
        Pthread_mutex_unlock(&self->_.d_exp.mtx);
        data = (struct DetectorDataFrame *) Malloc(sizeof(struct DetectorDataFrame));
        data->buffer = (unsigned char *) Malloc(length);
        data->pixel_format = self->_.d_param.pixel_format;
        data->width = self->_.d_param.image_width;
        data->height = self->_.d_param.image_height;
        data->n = n_frame;
        data->i = i + 1;
        data->length = length;
        if ((ret = ASIGetVideoData(self->camera_id, data->buffer, length, 2000 * exposure_time + 500)) != ASI_SUCCESS) {
#ifdef DEBUG
            fprintf(stderr, "%s %s %d --- ASIGetVideoData: error %d.\n", __FILE__, __func__, __LINE__ - 2, ret);
#endif
            free(data->buffer);
            free(data);
            if (ret == ASI_ERROR_CAMERA_CLOSED || ret == ASI_ERROR_TIMEOUT) {
                ret = AAOS_ECANCELED;
            } else {
                ret = asi_error_mapping(ret);
            }
            break;
        }
        self->_.d_exp.success_frames++;
        self->_.d_exp.count++;
        Clock_gettime(CLOCK_REALTIME, &data->tp);
        threadsafe_queue_push(self->_.d_proc.queue, data);
    }

    threadsafe_queue_push(self->_.d_proc.queue, NULL);
    Pthread_join(self->_.d_state.tid, &retval);
    
    Pthread_mutex_lock(&self->_.d_exp.mtx);
    if (!(self->_.d_exp.stop_flag&&ret==AAOS_ECANCELED)) {
        ASIStopVideoCapture(self->camera_id);
    }
    self->_.d_exp.stop_flag = false;	
    self->_.d_exp.rpc = NULL;
    Pthread_mutex_unlock(&self->_.d_exp.mtx);
    free(arg);
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    self->_.d_state.state = (state&DETECTOR_STATE_MALFUNCTION) | DETECTOR_STATE_IDLE;
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    Pthread_cond_broadcast(&self->_.d_state.cond);

    if (retval == PTHREAD_CANCELED) {
        ret = AAOS_ECANCELED;
    }

    return ret;
}

static int
ASICamera_expose_snapshot(struct ASICamera *_self, double exposure_time, uint32_t n_frame, va_list *app)
{
    struct ASICamera *self = cast(ASICamera(), _self);
    
    unsigned int state;
    uint16_t options;
    uint32_t i;
    struct timespec tp;
    ASI_EXPOSURE_STATUS status;
    unsigned char *buffer = NULL;
    long length;
    int ret;
    void *rpc;
    char *string;
    pthread_t tid;
    struct DetectorDataFrame *data;
	
    rpc = va_arg(*app, void *);
    string = va_arg(*app, char *);
    
    tp.tv_sec = floor(exposure_time - 0.2);
    tp.tv_nsec = (exposure_time - 0.2 - tp.tv_sec) * 1000000000.;
    
    ASI_ERROR_CODE (*ASIStartExposure)(int, ASI_BOOL);
    ASI_ERROR_CODE (*ASIGetExpStatus)(int, ASI_EXPOSURE_STATUS *);
    ASI_ERROR_CODE (*ASIGetDataAfterExp)(int, unsigned char *, long);
    ASIStartExposure = dlsym(self->dlh, "ASIStartExposure");
    ASIGetExpStatus = dlsym(self->dlh, "ASIGetExpStatus");
    ASIGetDataAfterExp = dlsym(self->dlh, "ASIGetDataAfterExp");
    
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    
    if ((state&DETECTOR_STATE_MALFUNCTION) && !(options&DETECTOR_OPTION_IGNORE_DEVMAL)) {
        ret = AAOS_EDEVMAL;
        goto error;
    }
    if ((state&DETECTOR_STATE_EXPOSING) || (state&DETECTOR_STATE_READING)) {
        if (options&DETECTOR_OPTION_NOWAIT) {
            ret = AAOS_EBUSY;
            goto error;
        }
        while ((state&DETECTOR_STATE_EXPOSING) || (state&DETECTOR_STATE_READING)) {
            Pthread_cond_wait(&self->_.d_state.cond, &self->_.d_state.mtx);
        }
    } else if (state&DETECTOR_STATE_OFFLINE) {
        ret = AAOS_EPWROFF;
        goto error;
    } else if (state&DETECTOR_STATE_UNINITIALIZED) {
        ret = AAOS_EUNINIT;
        goto error;
    }
    if ((ret = ASICamera_set_exposure_time_nl(self, exposure_time)) != AAOS_OK) {
        goto error;
    }
    state = DETECTOR_STATE_EXPOSING;
    self->_.d_state.state = (self->_.d_state.state&DETECTOR_STATE_MALFUNCTION)|state;
    Pthread_mutex_unlock(&self->_.d_state.mtx);
	
    switch (self->_.d_param.pixel_format) {
        case DETECTOR_PIXEL_FORMAT_MONO_8:
	        length = self->_.d_param.image_width * self->_.d_param.image_height;
	        break;
        case DETECTOR_PIXEL_FORMAT_MONO_16:
            length = self->_.d_param.image_width * self->_.d_param.image_height * 2;
	        break;
        case DETECTOR_PIXEL_FORMAT_RGB_24:
            length = self->_.d_param.image_width * self->_.d_param.image_height * 24;
	        break;
        default:
            break;		
    }
	
    data = (struct DetectorDataFrame *) Malloc(sizeof(struct DetectorDataFrame));
    data->buffer = NULL;
    data->pixel_format = self->_.d_param.pixel_format;
    data->width = self->_.d_param.image_width;
    data->height = self->_.d_param.image_height;
    data->n = n_frame;
    data->i = 1;
    data->length = length;
    Clock_gettime(CLOCK_REALTIME, &data->tp);
    threadsafe_queue_push(self->_.d_proc.queue, data);
	
    for (i = 0; i < n_frame; i++) {
        Clock_gettime(CLOCK_REALTIME, &tp);
        if ((ret = ASIStartExposure(self->camera_id, ASI_FALSE)) != ASI_SUCCESS) {
#ifdef DEBUG
			fprintf(stderr, "%s %s %d -- ASIStartExposure error: %d\n", __FILE__, __func__, __LINE__ - 2, ret);
#endif
            threadsafe_queue_push(self->_.d_proc.queue, NULL);
            ret = asi_error_mapping(ret);
            goto error2;
        }
        Clock_nanosleep(CLOCK_MONOTONIC, 0, &tp, NULL);
        while (true) {
            if ((ret = ASIGetExpStatus(self->camera_id, &status)) != ASI_SUCCESS) {
#ifdef DEBUG
		fprintf(stderr, "%s %s %d -- ASIGetExpStatus error: %d\n", __FILE__, __func__, __LINE__ - 2, ret);
#endif
                threadsafe_queue_push(self->_.d_proc.queue, NULL);
                ret = asi_error_mapping(ret);
                goto error2;
            }
            if (status == ASI_EXP_WORKING) {
                continue;
            } else {
                break;
            }
        }
        data = (struct DetectorDataFrame *) Malloc(sizeof(struct DetectorDataFrame));
        data->i = i + 1;
        data->length = length;
        buffer = (unsigned char *) Malloc(length);
        data->buffer = buffer;
        data->tp.tv_sec = tp.tv_sec;
        data->tp.tv_nsec = tp.tv_nsec;
        if ((ret = ASIGetDataAfterExp(self->camera_id, buffer, length)) != ASI_SUCCESS) {
            threadsafe_queue_push(self->_.d_proc.queue, NULL);
            ret = asi_error_mapping(ret);
            goto error2;
        }
    }
    threadsafe_queue_push(self->_.d_proc.queue, NULL);
error2:
    Pthread_mutex_lock(&self->_.d_state.mtx);
error:
    state = DETECTOR_STATE_IDLE;
    self->_.d_state.state = (self->_.d_state.state&DETECTOR_STATE_MALFUNCTION)|state;
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    ret;
}

static int
ASICamera_expose(void *_self, double exposure_time, uint32_t n_frame, va_list *app)
{
    struct ASICamera *self = cast(ASICamera(), _self);

    if (self->capture_mode == DETECTOR_CAPTURE_MODE_VIDEO) {
        return ASICamera_expose_video(self, exposure_time, n_frame, app);
    } else {
        return ASICamera_expose_snapshot(self, exposure_time, n_frame, app);
    }
}

static int
ASICamera_stop(void *_self)
{
    struct ASICamera *self = cast(ASICamera(), _self);
    
    unsigned int state;
    uint16_t options;

    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    if ((state&DETECTOR_STATE_EXPOSING) || (state&DETECTOR_STATE_READING)) {
        Pthread_mutex_lock(&self->_.d_exp.mtx);
        self->_.d_exp.stop_flag = true;
        while (self->_.d_exp.stop_flag) {
            Pthread_cond_wait(&self->_.d_exp.cond, &self->_.d_exp.mtx);
        }
        Pthread_mutex_unlock(&self->_.d_exp.mtx);
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    return AAOS_OK;
}

static int
ASICamera_abort(void *_self)
{
    struct ASICamera *self = cast(ASICamera(), _self);
    
    unsigned int state;
    uint16_t options;
    int ret = AAOS_OK;
    ASI_ERROR_CODE (*ASIStopVideoCapture)(int);

    ASIStopVideoCapture = dlsym(self->dlh, "ASIStopVideoCapture");
   
    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    if ((state&DETECTOR_STATE_EXPOSING) || (state&DETECTOR_STATE_READING)) {
	
        if ((ret = ASIStopVideoCapture(self->camera_id)) != ASI_SUCCESS) {
#ifdef DEBUG
	
            fprintf(stderr, "%s %s %d: ASIStopVideoCapture error.\n", __FILE__, __func__, __LINE__ - 2);
#endif
	        ret = asi_error_mapping(ret);
	        goto error;
	    }
	    Pthread_mutex_lock(&self->_.d_exp.mtx);
	    self->_.d_exp.stop_flag = true;
        Pthread_mutex_unlock(&self->_.d_exp.mtx);
    }

error:
    Pthread_mutex_unlock(&self->_.d_state.mtx);

    return ret;
}

static int
ASICamera_get_binning(void *_self, uint32_t *x_binning, uint32_t *y_binning)
{
    struct ASICamera *self = cast(ASICamera(), _self);
    
    ASI_ERROR_CODE (*ASIGetROIFormat)(int, int *, int *, int *, ASI_IMG_TYPE *);
    
    int w, h, b, ret = AAOS_OK;
    ASI_IMG_TYPE t;
    unsigned int state;
    uint16_t options;
    
    ASIGetROIFormat = dlsym(self->dlh, "ASIGetROIFormat");
    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    if ((state&DETECTOR_STATE_MALFUNCTION) && !(options&DETECTOR_OPTION_IGNORE_DEVMAL)) {
        ret = AAOS_EDEVMAL;
        goto error;
    } else if (state&DETECTOR_STATE_UNINITIALIZED) {
        ret = AAOS_EUNINIT;
        goto error;
    } else if (state&DETECTOR_STATE_OFFLINE) {
        ret = AAOS_EPWROFF;
        goto error;
    }  
    if ((ret = ASIGetROIFormat(self->camera_id, &w, &h, &b, &t)) != ASI_SUCCESS) {
        ret = asi_error_mapping(ret);
        goto error;
    }
    *x_binning = b;
    *y_binning = b;
error:
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    return ret;
}

static int
ASICamera_set_binning(void *_self, uint32_t x_binning, uint32_t y_binning)
{
    struct ASICamera *self = cast(ASICamera(), _self);
    
    int w, h, b, ret = AAOS_OK;
    ASI_IMG_TYPE t;
    unsigned int state;
    uint16_t options;
    
    ASI_ERROR_CODE (*ASIGetROIFormat)(int, int *, int *, int *, ASI_IMG_TYPE *);
    ASI_ERROR_CODE (*ASISetROIFormat)(int, int, int, int, ASI_IMG_TYPE);
    
    ASIGetROIFormat = dlsym(self->dlh, "ASIGetROIFormat");
    ASISetROIFormat = dlsym(self->dlh, "ASISetROIFormat");
    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    if ((state&DETECTOR_STATE_MALFUNCTION) && !(options&DETECTOR_OPTION_IGNORE_DEVMAL)) {
        ret = AAOS_EDEVMAL;
        goto error;
    } else if ((state&DETECTOR_STATE_READING) || (state&DETECTOR_STATE_EXPOSING)) {
        if (options&DETECTOR_OPTION_NOWAIT) {
            ret = AAOS_EBUSY;
            goto error;
        }
        while ((state&DETECTOR_STATE_EXPOSING) || (state&DETECTOR_STATE_READING)) {
            Pthread_cond_wait(&self->_.d_state.cond, &self->_.d_state.mtx);
        }
    } else if (state&DETECTOR_STATE_UNINITIALIZED) {
        ret = AAOS_EUNINIT;
        goto error;
    } else if (state&DETECTOR_STATE_OFFLINE) {
        ret = AAOS_EPWROFF;
        goto error;
    } 
    if ((ret = ASIGetROIFormat(self->camera_id, &w, &h, &b, &t)) != ASI_SUCCESS) {
        ret = asi_error_mapping(ret);
        goto error;
    }
    
    b = (int) x_binning;
    w = self->_.d_cap.width / b;
    h = self->_.d_cap.height / b;
    self->_.d_param.image_width = w;
    self->_.d_param.image_height = h;
    if (w%8 != 0 || h%2 != 0) {
        ret = AAOS_EINVAL;
        goto error;
    }
    if ((ret = ASISetROIFormat(self->camera_id, w, h, b, t)) != ASI_SUCCESS) {
        ret = asi_error_mapping(ret);
        goto error;
    }
error:
    self->_.d_param.x_binning = x_binning;
    self->_.d_param.y_binning = y_binning;
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    return ret;
}

static int
ASICamera_get_region(void *_self, uint32_t *x_offset, uint32_t *y_offset, uint32_t *width, uint32_t *height)
{
    struct ASICamera *self = cast(ASICamera(), _self);
    
    int w, h, b, x, y, ret = AAOS_OK;
    ASI_IMG_TYPE t;
    unsigned int state;
    uint16_t options;
    
    ASI_ERROR_CODE (*ASIGetROIFormat)(int, int *, int *, int *, ASI_IMG_TYPE *);
    ASI_ERROR_CODE (*ASIGetStartPos)(int, int *, int *);
    
    ASIGetROIFormat = dlsym(self->dlh, "ASIGetROIFormat");
    ASIGetStartPos = dlsym(self->dlh, "ASIGetStartPos");
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    if ((state&DETECTOR_STATE_MALFUNCTION) && !(options&DETECTOR_OPTION_IGNORE_DEVMAL)) {
        ret = AAOS_EDEVMAL;
        goto error;
    }
    if (state&DETECTOR_STATE_UNINITIALIZED) {
        ret = AAOS_EUNINIT;
        goto error;
    } else if (state&DETECTOR_STATE_OFFLINE) {
        ret = AAOS_EPWROFF;
        goto error;
    }
    if ((ret = ASIGetROIFormat(self->camera_id, &w, &h, &b, &t)) != ASI_SUCCESS) {
        ret = asi_error_mapping(ret);
        goto error;
    }
    if ((ret = ASIGetStartPos(self->camera_id, &x, &y)) != ASI_SUCCESS) {
        ret = asi_error_mapping(ret);
        goto error;
    }
    *x_offset = x;
    *y_offset = y;
    *width = w;
    *height = h;
error:
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    return ret;
}

static int
ASICamera_set_region(void *_self, uint32_t x_offset, uint32_t y_offset, uint32_t width, uint32_t height)
{
    struct ASICamera *self = cast(ASICamera(), _self);
    
    int w, h, b, x, y, ret = AAOS_OK;
    ASI_IMG_TYPE t;
    unsigned int state;
    uint16_t options;
    
    ASI_ERROR_CODE (*ASIGetROIFormat)(int, int *, int *, int *, ASI_IMG_TYPE *);
    ASI_ERROR_CODE (*ASISetROIFormat)(int, int, int, int, ASI_IMG_TYPE);
    ASI_ERROR_CODE (*ASISetStartPos)(int, int, int);
    
    ASIGetROIFormat = dlsym(self->dlh, "ASIGetROIFormat");
    ASISetROIFormat = dlsym(self->dlh, "ASISetROIFormat");
    ASISetStartPos = dlsym(self->dlh, "ASISetStartPos");
    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    if ((state&DETECTOR_STATE_MALFUNCTION) && !(options&DETECTOR_OPTION_IGNORE_DEVMAL)) {
        ret = AAOS_EDEVMAL;
        goto error;
    } else if ((state&DETECTOR_STATE_READING) || (state&DETECTOR_STATE_EXPOSING)) {
        if (options&DETECTOR_OPTION_NOWAIT) {
            ret = AAOS_EBUSY;
            goto error;
        }
        while ((state&DETECTOR_STATE_EXPOSING) || (state&DETECTOR_STATE_READING)) {
            Pthread_cond_wait(&self->_.d_state.cond, &self->_.d_state.mtx);
        }
    } else if (state&DETECTOR_STATE_UNINITIALIZED) {
        ret = AAOS_EUNINIT;
        goto error;
    } else if (state&DETECTOR_STATE_OFFLINE) {
        ret = AAOS_EPWROFF;
        goto error;
    }
    if ((ret = ASIGetROIFormat(self->camera_id, &w, &h, &b, &t)) != ASI_SUCCESS) {
        ret = asi_error_mapping(ret);
        goto error;
    }
    
    w = self->_.d_cap.width / b;
    h = self->_.d_cap.height / b;
    self->_.d_param.image_width = width;
    self->_.d_param.image_height = height;
    if (w%8 != 0 || h%2 != 0) {
        ret = AAOS_EINVAL;
        goto error;
    }
    if ((ret = ASISetROIFormat(self->camera_id, w, h, b, t)) != ASI_SUCCESS) {
        ret = asi_error_mapping(ret);
        goto error;
    }
    if ((ret = ASISetStartPos(self->camera_id, x, y)) != ASI_SUCCESS) {
        ret = asi_error_mapping(ret);
        goto error;
    }

    self->_.d_param.x_offset = x_offset;
    self->_.d_param.y_offset = y_offset;
    self->_.d_param.image_width = width;
    self->_.d_param.image_height = height;
error:
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    return ret;
}

static int 
ASICamera_set_gain(void *_self, double gain)
{
    struct ASICamera *self = cast(ASICamera(), _self);

    unsigned int state;
    uint16_t options;
    long value = (long) gain;
    int ret = AAOS_OK;
    ASI_ERROR_CODE (*ASISetControlValue)(int, ASI_CONTROL_TYPE, long, ASI_BOOL);

    ASISetControlValue = dlsym(self->dlh, "ASISetControlValue");
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    if (self->_.d_state.state&DETECTOR_STATE_MALFUNCTION) {
        ret = AAOS_EDEVMAL;
        goto error;
    } else if ((state&DETECTOR_STATE_READING) || (state&DETECTOR_STATE_EXPOSING)) {
        if (options&DETECTOR_OPTION_NOWAIT) {
            ret = AAOS_EBUSY;
            goto error;
        }
        while ((state&DETECTOR_STATE_EXPOSING) || (state&DETECTOR_STATE_READING)) {
            Pthread_cond_wait(&self->_.d_state.cond, &self->_.d_state.mtx);
        }
    } else if (state&DETECTOR_STATE_UNINITIALIZED) {
        ret = AAOS_EUNINIT;
        goto error;
    } else if (state&DETECTOR_STATE_OFFLINE) {
        ret = AAOS_EPWROFF;
        goto error;
    }
    if ((ret = ASISetControlValue(self->camera_id, ASI_GAIN, value, ASI_FALSE)) != ASI_SUCCESS) {
        ret = asi_error_mapping(ret);
        goto error;
    }
	
    self->_.d_param.gain = gain;
error:
    Pthread_mutex_unlock(&self->_.d_state.mtx);

    return ret;
}

static int 
ASICamera_get_gain(void *_self, double *gain)
{
    struct ASICamera *self = cast(ASICamera(), _self);

    unsigned int state;
    uint16_t options;
    long value;
    ASI_BOOL is_auto;
    int ret = AAOS_OK;
    ASI_ERROR_CODE (*ASIGetControlValue)(int, ASI_CONTROL_TYPE, long *, ASI_BOOL *);

    ASIGetControlValue = dlsym(self->dlh, "ASIGetControlValue");
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    if (self->_.d_state.state&DETECTOR_STATE_MALFUNCTION) {
        ret = AAOS_EDEVMAL;
        goto error;
    } else if (state&DETECTOR_STATE_UNINITIALIZED) {
        ret = AAOS_EUNINIT;
        goto error;
    } else if (state&DETECTOR_STATE_OFFLINE) {
        ret = AAOS_EPWROFF;
        goto error;
    }
    if ((ret = ASIGetControlValue(self->camera_id, ASI_GAIN, &value, &is_auto)) != ASI_SUCCESS) {
        ret = asi_error_mapping(ret);
        goto error;
    }
    *gain = (double) value;
error:
    Pthread_mutex_unlock(&self->_.d_state.mtx);

    return ret;
}

static int
ASICamera_enable_cooling(void *_self)
{
    struct ASICamera *self = cast(ASICamera(), _self);
    
    unsigned int state;
    uint16_t options;
    int ret = AAOS_OK;
    ASI_ERROR_CODE (*ASISetControlValue)(int, ASI_CONTROL_TYPE, long, ASI_BOOL);

    ASISetControlValue = dlsym(self->dlh, "ASISetControlValue");

    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    if (!self->_.d_cap.cooling_available) {
        ret = AAOS_ENOTSUP;
    }
    if (self->_.d_state.state&DETECTOR_STATE_MALFUNCTION) {
        ret = AAOS_EDEVMAL;
	goto error;
    } else if (state&DETECTOR_STATE_UNINITIALIZED) {
        ret = AAOS_EUNINIT;
        goto error;
    } else if (state&DETECTOR_STATE_OFFLINE) {
        ret = AAOS_EPWROFF;
        goto error;
    }
    if ((ret = ASISetControlValue(self->camera_id, ASI_COOLER_ON, ASI_FALSE, ASI_FALSE)) != ASI_SUCCESS) {
#ifdef DEBUG
        fprintf(stderr, "%s %s %d --- ASISetControlValue error: %d\n", __FILE__, __func__, __LINE__ - 2, ret);
#endif
        ret = asi_error_mapping(ret);
        goto error;
    }
    self->_.d_param.is_cooling_enable = true;
error:
    Pthread_mutex_unlock(&self->_.d_state.mtx);

    return ret;

}

static int
ASICamera_disable_cooling(void *_self)
{
    struct ASICamera *self = cast(ASICamera(), _self);

    unsigned int state;
    uint16_t options;
    int ret = AAOS_OK;
    ASI_ERROR_CODE (*ASISetControlValue)(int, ASI_CONTROL_TYPE, long , ASI_BOOL);
    
    ASISetControlValue = dlsym(self->dlh, "ASISetControlValue");

    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    if (!self->_.d_cap.cooling_available) {
        ret = AAOS_ENOTSUP;
    }
    if (self->_.d_state.state&DETECTOR_STATE_MALFUNCTION) {
        ret = AAOS_EDEVMAL;
        goto error;
    } else if (state&DETECTOR_STATE_UNINITIALIZED) {
        ret = AAOS_EUNINIT;
        goto error;
    } else if (state&DETECTOR_STATE_OFFLINE) {
        ret = AAOS_EPWROFF;
        goto error;
    }
    if ((ret = ASISetControlValue(self->camera_id, ASI_COOLER_ON, ASI_FALSE, ASI_FALSE)) != ASI_SUCCESS) {
#ifdef DEBUG
        fprintf(stderr, "%s %s %d --- ASISetControlValue error: %d\n", __FILE__, __func__, __LINE__ - 2, ret);
#endif
        ret = asi_error_mapping(ret);
        goto error;
    }
    self->_.d_param.is_cooling_enable = false;
error:
    Pthread_mutex_unlock(&self->_.d_state.mtx);

    return ret;
}

static int
ASICamera_get_temperature(void *_self, double *temperature)
{
    struct ASICamera *self = cast(ASICamera(), _self);

    unsigned int state;
    uint16_t options;
    long value;
    ASI_BOOL is_auto;
    int ret = AAOS_OK;
    ASI_ERROR_CODE (*ASIGetControlValue)(int, ASI_CONTROL_TYPE, long *, ASI_BOOL *);

    ASIGetControlValue = dlsym(self->dlh, "ASIGetControlValue");

    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    if (self->_.d_state.state&DETECTOR_STATE_MALFUNCTION) {
        ret = AAOS_EDEVMAL;
        goto error;
    } else if (state&DETECTOR_STATE_UNINITIALIZED) {
        ret = AAOS_EUNINIT;
        goto error;
    } else if (state&DETECTOR_STATE_OFFLINE) {
        ret = AAOS_EPWROFF;
        goto error;
    }
    if ((ret = ASIGetControlValue(self->camera_id, ASI_TEMPERATURE, &value, &is_auto)) != ASI_SUCCESS) {
        ret = asi_error_mapping(ret);
        goto error;
    }
    *temperature = (double) value / 10.;
error:
    Pthread_mutex_unlock(&self->_.d_state.mtx);

    return ret;
}

static int
ASICamera_set_temperature(void *_self, double temperature)
{
    struct ASICamera *self = cast(ASICamera(), _self);

    unsigned int state;
    uint16_t options;
    long value = (long) temperature;
    int ret = AAOS_OK;
    ASI_ERROR_CODE (*ASISetControlValue)(int, ASI_CONTROL_TYPE, long, ASI_BOOL);

    ASISetControlValue = dlsym(self->dlh, "ASISetControlValue");
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    if (self->_.d_state.state&DETECTOR_STATE_MALFUNCTION) {
        ret = AAOS_EDEVMAL;
        goto error;
    } else if (state&DETECTOR_STATE_UNINITIALIZED) {
        ret = AAOS_EUNINIT;
        goto error;
    } else if (state&DETECTOR_STATE_OFFLINE) {
        ret = AAOS_EPWROFF;
        goto error;
    }
    if ((ret = ASISetControlValue(self->camera_id, ASI_TARGET_TEMP, value, ASI_FALSE)) != ASI_SUCCESS) {
        ret = asi_error_mapping(ret);
        goto error;
    }
    
    self->_.d_param.temperature = temperature;
error:
    Pthread_mutex_unlock(&self->_.d_state.mtx);

    return ret;
}

static const void *_asi_camera_virtual_table;

static void
asi_camera_virtual_table_destroy(void)
{
    delete((void *) _asi_camera_virtual_table);
}

static void
asi_camera_virtual_table_initialize(void)
{
    _asi_camera_virtual_table = new(__DetectorVirtualTable(),
	                                __detector_disable_cooling, "disable_cooling", ASICamera_disable_cooling,
	                                __detector_enable_cooling, "enable_cooling", ASICamera_enable_cooling,
                                    __detector_init, "init", ASICamera_init,
                                    __detector_info, "info", ASICamera_info,
                                    __detector_status, "status", ASICamera_status,
                                    __detector_expose, "expose", ASICamera_expose,
                                    __detector_stop, "stop", ASICamera_stop,
                                    __detector_abort, "abort", ASICamera_abort,
                                    //__detector_power_on, "power_on", ASICamera_power_on,
                                    //__detector_power_off, "power_off", ASICamera_power_off,
                                    __detector_set_binning, "set_binning", ASICamera_set_binning,
                                    __detector_get_binning, "get_binning", ASICamera_get_binning,
                                    __detector_set_exposure_time, "set_exposure_time", ASICamera_set_exposure_time,
                                    __detector_get_exposure_time, "get_exposure_time", ASICamera_get_exposure_time,
                                    //__detector_set_frame_rate, "set_frame_rate", ASICamera_set_frame_rate,
                                    //__detector_get_frame_rate, "get_frame_rate", ASICamera_get_frame_rate,
                                    __detector_set_gain, "set_gain", ASICamera_set_gain,
                                    __detector_get_gain, "get_gain", ASICamera_get_gain,
                                    __detector_set_region, "set_region", ASICamera_set_region,
                                    __detector_get_region, "get_region", ASICamera_get_region,
                                    __detector_set_temperature, "set_temperature", ASICamera_set_temperature,
                                    __detector_get_temperature, "get_temperature", ASICamera_get_temperature,
                                    __detector_raw, "raw", ASICamera_raw,
                                     //__detector_inspect, "inspect", USTCCamera_inspect,
                                    (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(asi_camera_virtual_table_destroy);
#endif
}

static const void *
asi_camera_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, asi_camera_virtual_table_initialize);
#endif
    
    return _asi_camera_virtual_table;
}

#endif

/*
 * Leading InGaAs infrared camera.
 * http://leadingoe.com.cn
 */
#ifdef __USE_LEADING_CAMERA__

static const void *leading_camera_virtual_table(void);

static void *
LeadingCamera_ctor(void *_self, va_list *app)
{
    struct LeadingCamera *self = super_ctor(LeadingCamera(), _self, app);
    
    const char *s;
    
    bool (*EnumerateDevices_C_API)(int *, void **, void **);
    bool (*connectCamera_C_API)(void *, int *);
    void* (*LeadingCameraGetDeviceByName)(const char *, int, void **, void **, int *);
    bool (*releaseCameras_C_API)(void **, void **);
    void (*LeadingCameraCopyInfo)(void *, void *, char *, char *, char *, char *, char *, char *, char *, char *, char *);
    int n_camera;

    self->_.d_state.state = (DETECTOR_STATE_OFFLINE|DETECTOR_STATE_UNINITIALIZED);
    self->camera_index = -1;
    s = va_arg(*app, const char *);
    self->so_path = (char *) Malloc(strlen(s) + 1);
    snprintf(self->so_path, strlen(s) + 1, "%s", s);
    Pthread_mutex_init(&self->mtx, NULL);
    Pthread_cond_init(&self->cond, NULL);
    self->dlh = dlopen(self->so_path, RTLD_LAZY | RTLD_LOCAL);
#ifdef DEBUG
    if (self->dlh == NULL) {
        fprintf(stderr, "%s %s %d: dlopen \"%s\" error.\n", __FILE__, __func__, __LINE__ - 3, self->so_path);
        return NULL;
    }
#endif
    
    EnumerateDevices_C_API = dlsym(self->dlh, "EnumerateDevices_C_API");
    connectCamera_C_API = dlsym(self->dlh, "connectCamera_C_API");
    LeadingCameraGetDeviceByName = dlsym(self->dlh, "LeadingCameraGetDeviceByName");
    LeadingCameraCopyInfo = dlsym(self->dlh, "LeadingCameraCopyInfo");
    if (EnumerateDevices_C_API(&n_camera, self->device_list, self->interface_list)) {
        self->n_camera = n_camera;
        void *leading_camera;
        if (self->camera_index < 0) {
            if (self->_.name != NULL) {
                if ((leading_camera = LeadingCameraGetDeviceByName(self->_.name, n_camera, self->device_list, self->interface_list, &self->camera_index)) == NULL) {
                    free(self->so_path);
                    dlclose(self->dlh);
                    Pthread_mutex_destroy(&self->mtx);
                    Pthread_cond_destroy(&self->cond);
                    super_dtor(LeadingCamera(), self);
                    releaseCameras_C_API(self->device_list, self->interface_list);
                    return NULL;
                }
                if (!connectCamera_C_API(leading_camera, &self->camera_id)) {
                    free(self->so_path);
                    dlclose(self->dlh);
                    Pthread_mutex_destroy(&self->mtx);
                    Pthread_cond_destroy(&self->cond);
                    super_dtor(LeadingCamera(), self);
                    releaseCameras_C_API(self->device_list, self->interface_list);
                    return NULL;
                }
            } else {
                free(self->so_path);
                dlclose(self->dlh);
                Pthread_mutex_destroy(&self->mtx);
                Pthread_cond_destroy(&self->cond);
                super_dtor(LeadingCamera(), self);
                releaseCameras_C_API(self->device_list, self->interface_list);
                return NULL;
            }
        } else {
            if (self->camera_index >= n_camera) {
                free(self->so_path);
                dlclose(self->dlh);
                Pthread_mutex_destroy(&self->mtx);
                Pthread_cond_destroy(&self->cond);
                super_dtor(LeadingCamera(), self);
                releaseCameras_C_API(self->device_list, self->interface_list);
                return NULL;
            }
            leading_camera = self->device_list[self->camera_index];
            if (!connectCamera_C_API(leading_camera, &self->camera_id)) {
                free(self->so_path);
                dlclose(self->dlh);
                Pthread_mutex_destroy(&self->mtx);
                Pthread_cond_destroy(&self->cond);
                super_dtor(LeadingCamera(), self);
                releaseCameras_C_API(self->device_list, self->interface_list);
                return NULL;
            }
        }
    } else {
        free(self->so_path);
        dlclose(self->dlh);
        Pthread_mutex_destroy(&self->mtx);
        Pthread_cond_destroy(&self->cond);
        super_dtor(LeadingCamera(), self);
        return NULL;
    }
    
    LeadingCameraCopyInfo(self->device_list[self->camera_index], self->interface_list[self->camera_index], self->camera_info.model_name, self->camera_info.device_version, self->camera_info.manufacture_spec_info, self->camera_info.serial_number, self->camera_info.user_define_name, self->camera_info.ip, self->camera_info.mask, self->camera_info.gateway, self->camera_info.description);
    self->_._vtab= leading_camera_virtual_table();
    //self->_.d_proc.name_convention = ASICamera_name_convention;
    //self->_.d_proc.pre_acquisition = ASICamera_pre_acquisition;
    //self->_.d_proc.post_acquisition = __Detector_default_post_acquisition;
    self->_.d_proc.queue = new(ThreadsafeQueue(), DetectorDataFrame_cleanup);
    
    return (void *) self;
}

static void *
LeadingCamera_dtor(void *_self)
{
    struct LeadingCamera *self = cast(LeadingCamera(), _self);
    
    bool (*releaseCameras_C_API)(void **, void **);
    
    releaseCameras_C_API = dlsym(self->dlh, "releaseCameras_C_API");
    if (self->device_list != NULL && self->interface_list != NULL) {
        releaseCameras_C_API(self->device_list[self->camera_index], self->interface_list[self->camera_index]);
    }
    Pthread_mutex_destroy(&self->mtx);
    Pthread_cond_destroy(&self->cond);
    free(self->so_path);
    dlclose(self->dlh);

    return super_dtor(LeadingCamera(), _self);
}


static void *
LeadingCameraClass_ctor(void *_self, va_list *app)
{
    struct LeadingCameraClass *self = super_ctor(LeadingCameraClass(), _self, app);
    
    self->_.raw.method = (Method) 0;
    self->_.init.method = (Method) 0;
    
    self->_.disable_cooling.method = (Method) 0;
    self->_.enable_cooling.method = (Method) 0;
    self->_.get_binning.method = (Method) 0;
    self->_.set_binning.method = (Method) 0;
    self->_.set_exposure_time.method = (Method) 0;
    self->_.get_exposure_time.method = (Method) 0;
    self->_.set_frame_rate.method = (Method) 0;
    self->_.get_frame_rate.method = (Method) 0;
    self->_.set_gain.method = (Method) 0;
    self->_.get_gain.method = (Method) 0;
    self->_.set_region.method = (Method) 0;
    self->_.get_region.method = (Method) 0;
    self->_.set_temperature.method = (Method) 0;
    self->_.get_temperature.method = (Method) 0;
    self->_.inspect.method = (Method) 0;
    //self->_.wait.method = (Method) 0;
    self->_.wait_for_completion.method = (Method) 0;
    self->_.raw.method = (Method) 0;
    self->_.expose.method = (Method) 0;
    self->_.status.method = (Method) 0;
    self->_.info.method = (Method) 0;
    self->_.stop.method = (Method) 0;
    self->_.abort.method = (Method) 0;
    self->_.power_on.method = (Method) 0;
    self->_.power_off.method = (Method) 0;
    
    return self;
}

static void *_LeadingCameraClass;

static void
LeadingCameraClass_destroy(void)
{
    free((void *) _LeadingCameraClass);
}

static void
LeadingCameraClass_initialize(void)
{
    _LeadingCameraClass = new(__DetectorClass(), "LeadingCameraClass", __DetectorClass(), sizeof(struct LeadingCameraClass),
                              ctor, "", LeadingCameraClass_ctor,
                              (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(LeadingCameraClass_destroy);
#endif
}

const void *
LeadingCameraClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, LeadingCameraClass_initialize);
#endif

    return _LeadingCameraClass;
}

static void *_LeadingCamera;

static void
LeadingCamera_destroy(void)
{
    free((void *) _LeadingCamera);
}

static void
LeadingCamera_initialize(void)
{
    _LeadingCamera = new(LeadingCameraClass(), "LeadingCamera", __Detector(), sizeof(struct LeadingCamera),
                         ctor, "ctor", LeadingCamera_ctor,
                         dtor, "dtor", LeadingCamera_dtor,
                         (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(LeadingCamera_destroy);
#endif
}

const void *
LeadingCamera(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, LeadingCamera_initialize);
#endif

    return _LeadingCamera;
}

static int
LeadingCamera_raw(void *_self, const void *write_buffer, size_t write_buffer_size, size_t *write_size, void *read_buffer, size_t read_buffer_size, size_t *read_size)
{
    struct LeadingCamera *self = cast(LeadingCamera(), _self);
    
    FILE *fp;
    char func_name[COMMANDSIZE];
    int ret = AAOS_OK;
    
    if ((fp = fmemopen((void *) write_buffer, write_buffer_size, "r")) == NULL) {
        return AAOS_ERROR;
    }
    
    if ((ret = fscanf(fp, "%s", &func_name)) != 1) {
        fclose(fp);
        return AAOS_ERROR;
    }
    
    if (strcmp(func_name, "EnumerateDevices") == 0) {
        void (*LeadingCameraCopyInfo)(void *, void *, char *, char *, char *, char *, char *, char *, char *, char *, char *);
        int i, n_camera;
        void **device_list, **interface_list;
        char *string, model_name[32], device_version[32], manufacture_spec_info[48], serial_number[16], user_defined_name[16], ip[16], mask[16], gateway[16], description[132];
        cJSON *root_json, *camera_array_json, *camera_json;
        LeadingCameraCopyInfo = dlsym(self->dlh, "LeadingCameraCopyInfo");
        root_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(root_json, "n_camera", n_camera);
        camera_array_json = cJSON_CreateArray();
        for (i = 0; i < self->n_camera; i++) {
            LeadingCameraCopyInfo(self->device_list[i], self->interface_list[i], model_name, device_version, manufacture_spec_info, serial_number, user_defined_name, ip, mask, gateway, description);
            camera_json = cJSON_CreateObject();
            cJSON_AddStringToObject(camera_json, "ModelName", model_name);
            cJSON_AddStringToObject(camera_json, "DeviceVersion", device_version);
            cJSON_AddStringToObject(camera_json, "ManufactureSpecInfo", manufacture_spec_info);
            cJSON_AddStringToObject(camera_json, "SerialNumber", serial_number);
            cJSON_AddStringToObject(camera_json, "UserDefinedName", user_defined_name);
            cJSON_AddStringToObject(camera_json, "IP", ip);
            cJSON_AddStringToObject(camera_json, "Mask", mask);
            cJSON_AddStringToObject(camera_json, "Gateway", gateway);
            cJSON_AddStringToObject(camera_json, "Description", description);
            cJSON_AddItemToArray(camera_array_json, camera_json);
        }
        cJSON_AddItemToObject(root_json, "cameras", camera_array_json);
        string = cJSON_Print(root_json);
        cJSON_Delete(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        free(string);
        if (read_size != NULL) {
            *read_size = strlen((char *) read_buffer);
        }
        
    } else if (strcmp(func_name, "GigEIpConfiguration") == 0) {
        bool (*GigEIpConfiguration_C_API)(unsigned char *, unsigned int, unsigned int, unsigned int, unsigned char *, unsigned int);
        unsigned char mac[8], user_name[16];
        unsigned int ip, mask, gateway, is_save;
        
        GigEIpConfiguration_C_API = dlsym(self->dlh, "GigEIpConfiguration_C_API");
        if (fscanf(fp, "%s %u %u %u %s %u", mac, &ip, &mask, &gateway, user_name, &is_save) != 6) {
            fclose(fp);
            return AAOS_EINVAL;
        }
        if (!GigEIpConfiguration_C_API(mac, ip, mask, gateway, user_name, is_save)) {
            fclose(fp);
            return AAOS_ENOTFOUND;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "startCameraCapture") == 0) {
        bool (*startCameraCapture_C_API)(int);
    
        startCameraCapture_C_API = dlsym(self->dlh, "startCameraCapture_C_API");
        if (!startCameraCapture_C_API(self->camera_id)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "stopCameraCapture") == 0) {
        bool (*stopCameraCapture_C_API)(int);
        
        stopCameraCapture_C_API = dlsym(self->dlh, "stopCameraCapture_C_API");
        if (!stopCameraCapture_C_API(self->camera_id)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "setCameraHeartBeat") == 0) {
        bool (*setCameraHeartBeat_C_API)(int, unsigned int);
        unsigned int msec;
        
        setCameraHeartBeat_C_API = dlsym(self->dlh, "setCameraHeartBeat_C_API");
        if (fscanf(fp, "%u", &msec) != 1) {
            fclose(fp);
            return AAOS_EBADCMD;
        }
        if (!setCameraHeartBeat_C_API(self->camera_id, msec)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "sendCommandToCamera") == 0) {
        bool (*sendCommandToCamera_C_API)(int, unsigned char *);
        unsigned char command[COMMANDSIZE];
        
        sendCommandToCamera_C_API = dlsym(self->dlh, "sendCommandToCamera_C_API");
        if (fscanf(fp, "%s", &command) != 1) {
            fclose(fp);
            return AAOS_EINVAL;
        }
        if (!sendCommandToCamera_C_API(self->camera_id, command)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "sendCustomCmdToCamera") == 0) {
        bool (*sendCustomCmdToCamera_C_API)(int, unsigned char *, unsigned int);
        unsigned char command[COMMANDSIZE];

        sendCustomCmdToCamera_C_API = dlsym(self->dlh, "sendCustomCmdToCamera_C_API");
        if (fscanf(fp, "%s", command) != 1) {
            fclose(fp);
            return AAOS_EINVAL;
        }
        if (!sendCustomCmdToCamera_C_API(self->camera_id, command, strlen(command))) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "getCameraImageDigit") == 0) {
        int (*getCameraImageDigit_C_API)(int);
        int digits;
        char *string;
        cJSON *root_json;
        
        getCameraImageDigit_C_API = dlsym(self->dlh, "getCameraImageDigit_C_API");
        digits = getCameraImageDigit_C_API(self->camera_id);
        root_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(root_json, "CameraImageDigit", digits);
        string = cJSON_Print(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        free(string);
        cJSON_Delete(root_json);
        if (read_size != NULL) {
            *read_size = strlen((char *) read_buffer);
        }
    } else if (strcmp(func_name, "getMaxResolution") == 0) {
        bool (*getMaxResolution_C_API)(int, unsigned int *, unsigned int *);
        unsigned int width, height;
        char *string;
        cJSON *root_json;
        
        getMaxResolution_C_API = dlsym(self->dlh, "getMaxResolution_C_API");
        if (!getMaxResolution_C_API(self->camera_id, &width, &height)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        root_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(root_json, "MaxWidth", width);
        cJSON_AddNumberToObject(root_json, "MaxHeight", height);
        string = cJSON_Print(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        free(string);
        cJSON_Delete(root_json);
        if (read_size != NULL) {
            *read_size = strlen((char *) read_buffer);
        }
    } else if (strcmp(func_name, "setImageFormat") == 0) {
        bool (*setImageFormat_C_API)(int, unsigned char);
        unsigned char uc_value;
        
        setImageFormat_C_API = dlsym(self->dlh, "setImageFormat_C_API");
        if (fscanf(fp, "%c", &uc_value) != 1) {
            fclose(fp);
            return AAOS_EBADCMD;
        }
        if (!setImageFormat_C_API(self->camera_id, uc_value)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "getImageFormat") == 0) {
        bool (*getImageFormat_C_API)(int, unsigned char *);
        unsigned char uc_value;
        char *string;
        cJSON *root_json;
        
        getImageFormat_C_API = dlsym(self->dlh, "getImageFormat_C_API");
        if (!getImageFormat_C_API(self->camera_id, &uc_value)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        root_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(root_json, "ImageFormat", uc_value);
        string = cJSON_Print(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        free(string);
        cJSON_Delete(root_json);
        if (read_size != NULL) {
            *read_size = strlen((char *) read_buffer);
        }
    } else if (strcmp(func_name, "setPseudoColorMode") == 0) {
        bool (*setPseudoColorMode_C_API)(int, unsigned char);
        unsigned char uc_value;
        
        setPseudoColorMode_C_API = dlsym(self->dlh, "setPseudoColorMode_C_API");
        if (fscanf(fp, "%c", &uc_value) != 1) {
            fclose(fp);
            return AAOS_EBADCMD;
        }
        if (!setPseudoColorMode_C_API(self->camera_id, uc_value)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "getPseudoColorMode") == 0) {
        bool (*getPseudoColorMode_API)(int, unsigned char *);
        unsigned char uc_value;
        char *string;
        cJSON *root_json;
        
        getPseudoColorMode_API = dlsym(self->dlh, "getPseudoColorMode_API");
        if (!getPseudoColorMode_API(self->camera_id, &uc_value)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        root_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(root_json, "PseudoColorMode", uc_value);
        string = cJSON_Print(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        free(string);
        cJSON_Delete(root_json);
        if (read_size != NULL) {
            *read_size = strlen((char *) read_buffer);
        }
    } else if (strcmp(func_name, "setCrossLineShow") == 0) {
        bool (*setCrossLineShow_C_API)(int, bool, unsigned char, unsigned char);
        bool b_value;
        unsigned char uc_value, uc_value2;
        
        setCrossLineShow_C_API = dlsym(self->dlh, "setPseudoColorMode_C_API");
        if (fscanf(fp, "%d %c %c", &b_value, &uc_value, &uc_value2) != 3) {
            fclose(fp);
            return AAOS_EBADCMD;
        }
        if (!setCrossLineShow_C_API(self->camera_id, b_value, uc_value, uc_value2)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "setTriggerMode") == 0) {
        bool (*setTriggerMode_C_API)(int, unsigned char);
        unsigned char uc_value;
        
        setTriggerMode_C_API = dlsym(self->dlh, "setTriggerMode_C_API");
        if (fscanf(fp, "%c", &uc_value) != 1) {
            fclose(fp);
            return AAOS_EBADCMD;
        }
        if (!setTriggerMode_C_API(self->camera_id, uc_value)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "getTriggerMode") == 0) {
        bool (*getTriggerMode_C_API)(int, unsigned char *);
        unsigned char uc_value;
        char *string;
        cJSON *root_json;
        
        getTriggerMode_C_API = dlsym(self->dlh, "getTriggerMode_C_API");
        if (!getTriggerMode_C_API(self->camera_id, &uc_value)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        root_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(root_json, "TriggerMode", uc_value);
        string = cJSON_Print(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        free(string);
        cJSON_Delete(root_json);
        if (read_size != NULL) {
            *read_size = strlen((char *) read_buffer);
        }
    } else if (strcmp(func_name, "setTriggerLevel") == 0) {
        bool (*setTriggerLevel_C_API)(int, bool);
        bool b_value;
        
        setTriggerLevel_C_API = dlsym(self->dlh, "setTriggerLevel_C_API");
        if (fscanf(fp, "%d", &b_value) != 1) {
            fclose(fp);
            return AAOS_EBADCMD;
        }
        if (!setTriggerLevel_C_API(self->camera_id, b_value)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "getTriggerLevel") == 0) {
        bool (*getTriggerLevel_C_API)(int, unsigned char *);
        bool b_value;
        char *string;
        cJSON *root_json;
        
        getTriggerLevel_C_API = dlsym(self->dlh, "getTriggerLevel_C_API");
        if (!getTriggerLevel_C_API(self->camera_id, &b_value)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        root_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(root_json, "TriggerLevel", b_value);
        string = cJSON_Print(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        free(string);
        cJSON_Delete(root_json);
        if (read_size != NULL) {
            *read_size = strlen((char *) read_buffer);
        }
    } else if (strcmp(func_name, "setTriggerEdge") == 0) {
        bool (*setTriggerEdge_C_API)(int, bool);
        bool b_value;
        
        setTriggerEdge_C_API = dlsym(self->dlh, "setTriggerEdge_C_API");
        if (fscanf(fp, "%d", &b_value) != 1) {
            fclose(fp);
            return AAOS_EBADCMD;
        }
        if (!setTriggerEdge_C_API(self->camera_id, b_value)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "getTriggerEdge") == 0) {
        bool (*getTriggerEdge_C_API)(int, bool *);
        bool b_value;
        char *string;
        cJSON *root_json;
        
        getTriggerEdge_C_API = dlsym(self->dlh, "getTriggerEdge_C_API");
        if (!getTriggerEdge_C_API(self->camera_id, &b_value)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        root_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(root_json, "TriggerEdge", b_value);
        string = cJSON_Print(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        free(string);
        cJSON_Delete(root_json);
        if (read_size != NULL) {
            *read_size = strlen((char *) read_buffer);
        }
    } else if (strcmp(func_name, "setTriggerSignal") == 0) {
        bool (*setTriggerSignal_C_API)(int, unsigned char);
        unsigned char uc_value;
        
        setTriggerSignal_C_API = dlsym(self->dlh, "setTriggerSignal_C_API");
        if (fscanf(fp, "%c", &uc_value) != 1) {
            fclose(fp);
            return AAOS_EBADCMD;
        }
        if (!setTriggerSignal_C_API(self->camera_id, uc_value)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "getTriggerSignal") == 0) {
        bool (*getTriggerSignal_C_API)(int, unsigned char *);
        unsigned char uc_value;
        char *string;
        cJSON *root_json;
        
        getTriggerSignal_C_API = dlsym(self->dlh, "getTriggerSignal_C_API");
        if (!getTriggerSignal_C_API(self->camera_id, &uc_value)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        root_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(root_json, "TriggerSignal", uc_value);
        string = cJSON_Print(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        free(string);
        cJSON_Delete(root_json);
        if (read_size != NULL) {
            *read_size = strlen((char *) read_buffer);
        }
    } else if (strcmp(func_name, "setFrameFrequency") == 0) {
        bool (*setFrameFrequency_C_API)(int, unsigned int);
        unsigned int ui_value;
        
        setFrameFrequency_C_API = dlsym(self->dlh, "setFrameFrequency_C_API");
        if (fscanf(fp, "%u", &ui_value) != 1) {
            fclose(fp);
            return AAOS_EBADCMD;
        }
        if (!setFrameFrequency_C_API(self->camera_id, ui_value)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "getFrameFrequency") == 0) {
        bool (*getFrameFrequency_C_API)(int, unsigned int *);
        int camera_id;
        unsigned int ui_value;
        char *string;
        cJSON *root_json;
        
        getFrameFrequency_C_API = dlsym(self->dlh, "getFrameFrequency_C_API");
        if (fscanf(fp, "%d", &camera_id) != 1) {
            fclose(fp);
            return AAOS_EINVAL;
        }
        if (!getFrameFrequency_C_API(self->camera_id, &ui_value)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        root_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(root_json, "FrameFrequency", ui_value);
        string = cJSON_Print(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        free(string);
        cJSON_Delete(root_json);
        if (read_size != NULL) {
            *read_size = strlen((char *) read_buffer);
        }
    } else if (strcmp(func_name, "setTestImageMode") == 0) {
        bool (*setTestImageMode_C_API)(int, unsigned char);
        unsigned char uc_value;
        
        setTestImageMode_C_API = dlsym(self->dlh, "setTestImageMode_C_API");
        if (fscanf(fp, "%c", &uc_value) != 1) {
            fclose(fp);
            return AAOS_EINVAL;
        }
        if (!setTestImageMode_C_API(self->camera_id, uc_value)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "setExposureTime") == 0) {
        bool (*setExposureTime_C_API)(int, unsigned int);
        unsigned int ui_value;
        
        setExposureTime_C_API = dlsym(self->dlh, "setExposureTime_C_API");
        if (fscanf(fp, "%u", &ui_value) != 1) {
            fclose(fp);
            return AAOS_EBADCMD;
        }
        if (!setExposureTime_C_API(self->camera_id, ui_value)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "getExposureTime") == 0) {
        bool (*getExposureTime_C_API)(int, unsigned int *);
        unsigned int ui_value;
        char *string;
        cJSON *root_json;
        
        getExposureTime_C_API = dlsym(self->dlh, "getExposureTime_C_API");
        if (!getExposureTime_C_API(self->camera_id, &ui_value)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        root_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(root_json, "ExposureTime", ui_value);
        string = cJSON_Print(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        free(string);
        cJSON_Delete(root_json);
        if (read_size != NULL) {
            *read_size = strlen((char *) read_buffer);
        }
    } else if (strcmp(func_name, "setExposureMode") == 0) {
        bool (*setExposureMode_C_API)(int, bool);
        bool b_value;
        
        setExposureMode_C_API = dlsym(self->dlh, "setExposureMode_C_API");
        if (fscanf(fp, "%d", &b_value) != 1) {
            fclose(fp);
            return AAOS_EBADCMD;
        }
        if (!setExposureMode_C_API(self->camera_id, b_value)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "getExposureMode") == 0) {
        bool (*getExposureMode_C_API)(int, bool *);
        bool b_value;
        char *string;
        cJSON *root_json;
        
        getExposureMode_C_API = dlsym(self->dlh, "getExposureMode_C_API");
        if (!getExposureMode_C_API(self->camera_id, &b_value)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        root_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(root_json, "ExposureMode", b_value);
        string = cJSON_Print(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        free(string);
        cJSON_Delete(root_json);
        if (read_size != NULL) {
            *read_size = strlen((char *) read_buffer);
        }
    } else if (strcmp(func_name, "setExposeIntensity") == 0) {
        bool (*setExposeIntensity_C_API)(int, unsigned char);
        unsigned char uc_value;
        
        setExposeIntensity_C_API = dlsym(self->dlh, "setExposeIntensity_C_API");
        if (fscanf(fp, "%c", &uc_value) != 2) {
            fclose(fp);
            return AAOS_EBADCMD;
        }
        if (!setExposeIntensity_C_API(self->camera_id, uc_value)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "getExposeIntensity") == 0) {
        bool (*getExposeIntensity_C_API)(int, unsigned char *);
        unsigned char uc_value;
        char *string;
        cJSON *root_json;
        
        getExposeIntensity_C_API = dlsym(self->dlh, "getExposeIntensity_C_API");
        if (!getExposeIntensity_C_API(self->camera_id, &uc_value)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        root_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(root_json, "ExposeIntensity", uc_value);
        string = cJSON_Print(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        free(string);
        cJSON_Delete(root_json);
        if (read_size != NULL) {
            *read_size = strlen((char *) read_buffer);
        }
    } else if (strcmp(func_name, "setExposeRegion") == 0) {
        bool (*setExposeRegion_C_API)(int, unsigned char);
        unsigned char uc_value;
        
        setExposeRegion_C_API = dlsym(self->dlh, "setExposeRegion_C_API");
        if (fscanf(fp, "%c", &uc_value) != 1) {
            fclose(fp);
            return AAOS_EBADCMD;
        }
        if (!setExposeRegion_C_API(self->camera_id, uc_value)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "getExposeRegion") == 0) {
        bool (*getExposeRegion_C_API)(int, unsigned char *);
        unsigned char uc_value;
        char *string;
        cJSON *root_json;
        
        getExposeRegion_C_API = dlsym(self->dlh, "getExposeRegion_C_API");
        if (!getExposeRegion_C_API(self->camera_id, &uc_value)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        root_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(root_json, "ExposeRegion", uc_value);
        string = cJSON_Print(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        free(string);
        cJSON_Delete(root_json);
        if (read_size != NULL) {
            *read_size = strlen((char *) read_buffer);
        }
    } else if (strcmp(func_name, "setAutoExposeTimeLimitState") == 0) {
        bool (*setAutoExposeTimeLimitState_C_API)(int, bool);
        bool b_value;
        
        setAutoExposeTimeLimitState_C_API = dlsym(self->dlh, "setAutoExposeTimeLimitState_C_API");
        if (fscanf(fp, "%d", &b_value) != 1) {
            fclose(fp);
            return AAOS_EBADCMD;
        }
        if (!setAutoExposeTimeLimitState_C_API(self->camera_id, b_value)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "getAutoExposeTimeLimitState") == 0) {
        bool (*getAutoExposeTimeLimitState_C_API)(int, bool *);
        bool b_value;
        char *string;
        cJSON *root_json;
        
        getAutoExposeTimeLimitState_C_API = dlsym(self->dlh, "getAutoExposeTimeLimitState_C_API");
        if (!getAutoExposeTimeLimitState_C_API(self->camera_id, &b_value)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        root_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(root_json, "AutoExposeTimeLimitState", b_value);
        string = cJSON_Print(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        free(string);
        cJSON_Delete(root_json);
        if (read_size != NULL) {
            *read_size = strlen((char *) read_buffer);
        }
    } else if (strcmp(func_name, "setAutoExposeMinTime") == 0) {
        bool (*setAutoExposeMinTime_C_API)(int, unsigned int);
        unsigned int ui_value;
        
        setAutoExposeMinTime_C_API = dlsym(self->dlh, "setAutoExposeMinTime_C_API");
        if (fscanf(fp, "%u", &ui_value) != 1) {
            fclose(fp);
            return AAOS_EINVAL;
        }
        if (!setAutoExposeMinTime_C_API(self->camera_id, ui_value)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "getAutoExposeMinTime") == 0) {
        bool (*getAutoExposeMinTime_C_API)(int, unsigned int *);
        unsigned int ui_value;
        char *string;
        cJSON *root_json;
        
        getAutoExposeMinTime_C_API = dlsym(self->dlh, "getAutoExposeMinTime_C_API");
        if (!getAutoExposeMinTime_C_API(self->camera_id, &ui_value)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        root_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(root_json, "AutoExposeMinTime", ui_value);
        string = cJSON_Print(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        free(string);
        cJSON_Delete(root_json);
        if (read_size != NULL) {
            *read_size = strlen((char *) read_buffer);
        }
    } else if (strcmp(func_name, "setAutoExposeMaxTime") == 0) {
        bool (*setAutoExposeMaxTime_C_API)(int, unsigned int);
        unsigned int ui_value;
        
        setAutoExposeMaxTime_C_API = dlsym(self->dlh, "setAutoExposeMaxTime_C_API");
        if (fscanf(fp, "%u", &ui_value) != 1) {
            fclose(fp);
            return AAOS_EBADCMD;
        }
        if (!setAutoExposeMaxTime_C_API(self->camera_id, ui_value)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "getAutoExposeMaxTime") == 0) {
        bool (*getAutoExposeMaxTime_C_API)(int, unsigned int *);
        unsigned int ui_value;
        char *string;
        cJSON *root_json;
        
        getAutoExposeMaxTime_C_API = dlsym(self->dlh, "getAutoExposeMaxTime_C_API");
        if (!getAutoExposeMaxTime_C_API(self->camera_id, &ui_value)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        root_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(root_json, "AutoExposeMaxTime", ui_value);
        string = cJSON_Print(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        free(string);
        cJSON_Delete(root_json);
        if (read_size != NULL) {
            *read_size = strlen((char *) read_buffer);
        }
    } else if (strcmp(func_name, "getSensorADAverage") == 0) {
        bool (*getSensorADAverage_C_API)(int, unsigned int *);
        unsigned int ui_value;
        char *string;
        cJSON *root_json;
        
        getSensorADAverage_C_API = dlsym(self->dlh, "getSensorADAverage_C_API");
        if (!getSensorADAverage_C_API(self->camera_id, &ui_value)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        root_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(root_json, "SensorADAverage", ui_value);
        string = cJSON_Print(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        free(string);
        cJSON_Delete(root_json);
        if (read_size != NULL) {
            *read_size = strlen((char *) read_buffer);
        }
    } else if (strcmp(func_name, "setGainMode") == 0) {
        bool (*setGainMode_C_API)(int, bool);
        bool b_value;
        
        setGainMode_C_API = dlsym(self->dlh, "setGainMode_C_API");
        if (fscanf(fp, "%d", &b_value) != 2) {
            fclose(fp);
            return AAOS_EBADCMD;
        }
        if (!setGainMode_C_API(self->camera_id, b_value)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "getGainMode") == 0) {
        bool (*getGainMode_C_API)(int, bool *);
        bool b_value;
        char *string;
        cJSON *root_json;
        
        getGainMode_C_API = dlsym(self->dlh, "getGainMode_C_API");
        if (!getGainMode_C_API(self->camera_id, &b_value)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        root_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(root_json, "GainMode", b_value);
        string = cJSON_Print(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        free(string);
        cJSON_Delete(root_json);
        if (read_size != NULL) {
            *read_size = strlen((char *) read_buffer);
        }
    } else if (strcmp(func_name, "setGainValue") == 0) {
        bool (*setGainValue_C_API)(int, unsigned char);
        unsigned char uc_value;
        
        setGainValue_C_API = dlsym(self->dlh, "setGainValue_C_API");
        if (fscanf(fp, "%c", &uc_value) != 1) {
            fclose(fp);
            return AAOS_EBADCMD;
        }
        if (!setGainValue_C_API(self->camera_id, uc_value)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "getGainValue") == 0) {
        bool (*getGainValue_C_API)(int, unsigned char *);
        unsigned char uc_value;
        char *string;
        cJSON *root_json;
        
        getGainValue_C_API = dlsym(self->dlh, "getGainValue_C_API");
        if (!getGainValue_C_API(self->camera_id, &uc_value)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        root_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(root_json, "GainValue", uc_value);
        string = cJSON_Print(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        free(string);
        cJSON_Delete(root_json);
        if (read_size != NULL) {
            *read_size = strlen((char *) read_buffer);
        }
    } else if (strcmp(func_name, "setImageEnhanceMode") == 0) {
        bool (*setImageEnhanceMode_C_API)(int, unsigned char);
        unsigned char uc_value;
        
        setImageEnhanceMode_C_API = dlsym(self->dlh, "setImageEnhanceMode_C_API");
        if (fscanf(fp, "%c", &uc_value) != 2) {
            fclose(fp);
            return AAOS_EBADCMD;
        }
        if (!setImageEnhanceMode_C_API(self->camera_id, uc_value)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "getImageEnhanceMode") == 0) {
        bool (*getImageEnhanceMode_C_API)(int, unsigned char *);
        unsigned char uc_value;
        char *string;
        cJSON *root_json;
        
        getImageEnhanceMode_C_API = dlsym(self->dlh, "getImageEnhanceMode_C_API");
        if (!getImageEnhanceMode_C_API(self->camera_id, &uc_value)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        root_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(root_json, "ImageEnhanceMode", uc_value);
        string = cJSON_Print(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        free(string);
        cJSON_Delete(root_json);
        if (read_size != NULL) {
            *read_size = strlen((char *) read_buffer);
        }
    } else if (strcmp(func_name, "setImageEnhanceBrightness") == 0) {
        bool (*setImageEnhanceBrightnessC_API)(int, unsigned char);
        unsigned char uc_value;
        
        setImageEnhanceBrightnessC_API = dlsym(self->dlh, "setImageEnhanceBrightnessC_API");
        if (fscanf(fp, "%c", &uc_value) != 1) {
            fclose(fp);
            return AAOS_EBADCMD;
        }
        if (!setImageEnhanceBrightnessC_API(self->camera_id, uc_value)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "getImageEnhanceBrightness") == 0) {
        bool (*getImageEnhanceBrightness_C_API)(int, unsigned char *);
        unsigned char uc_value;
        char *string;
        cJSON *root_json;
        
        getImageEnhanceBrightness_C_API = dlsym(self->dlh, "getImageEnhanceBrightness_C_API");
        if (!getImageEnhanceBrightness_C_API(self->camera_id, &uc_value)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        root_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(root_json, "ImageEnhanceBrightness", uc_value);
        string = cJSON_Print(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        free(string);
        cJSON_Delete(root_json);
        if (read_size != NULL) {
            *read_size = strlen((char *) read_buffer);
        }
    } else if (strcmp(func_name, "setImageEnhanceContrast") == 0) {
        bool (*setImageEnhanceContrast_C_API)(int, unsigned char);
        unsigned char uc_value;
        
        setImageEnhanceContrast_C_API = dlsym(self->dlh, "setImageEnhanceContrast_C_API");
        if (fscanf(fp, "%c", &uc_value) != 1) {
            fclose(fp);
            return AAOS_EBADCMD;
        }
        if (!setImageEnhanceContrast_C_API(self->camera_id, uc_value)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "getImageEnhanceContrast") == 0) {
        bool (*getImageEnhanceContrast_C_API)(int, unsigned char *);
        unsigned char uc_value;
        char *string;
        cJSON *root_json;
        
        getImageEnhanceContrast_C_API = dlsym(self->dlh, "getImageEnhanceContrast_C_API");
        if (!getImageEnhanceContrast_C_API(self->camera_id, &uc_value)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        root_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(root_json, "ImageEnhanceContrast", uc_value);
        string = cJSON_Print(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        free(string);
        cJSON_Delete(root_json);
        if (read_size != NULL) {
            *read_size = strlen((char *) read_buffer);
        }
    } else if (strcmp(func_name, "setImageDetailEnhance") == 0) {
        bool (*setImageDetailEnhance_C_API)(int, unsigned char);
        unsigned char uc_value;
        
        setImageDetailEnhance_C_API = dlsym(self->dlh, "setImageDetailEnhance_C_API");
        if (fscanf(fp, "%c", &uc_value) != 1) {
            fclose(fp);
            return AAOS_EBADCMD;
        }
        if (!setImageDetailEnhance_C_API(self->camera_id, uc_value)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "getImageDetailEnhance") == 0) {
        bool (*getImageDetailEnhance_C_API)(int, unsigned char *);
        unsigned char uc_value;
        char *string;
        cJSON *root_json;
        
        getImageDetailEnhance_C_API = dlsym(self->dlh, "getImageDetailEnhance_C_API");
        if (!getImageDetailEnhance_C_API(self->camera_id, &uc_value)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        root_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(root_json, "ImageDetailEnhance", uc_value);
        string = cJSON_Print(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        free(string);
        cJSON_Delete(root_json);
        if (read_size != NULL) {
            *read_size = strlen((char *) read_buffer);
        }
    } else if (strcmp(func_name, "setImageNioseFilter") == 0) {
        bool (*setImageNioseFilter_C_API)(int, unsigned char);
        unsigned char uc_value;
        
        setImageNioseFilter_C_API = dlsym(self->dlh, "setImageNioseFilter_C_API");
        if (fscanf(fp, "%c", &uc_value) != 2) {
            fclose(fp);
            return AAOS_EBADCMD;
        }
        if (!setImageNioseFilter_C_API(self->camera_id, uc_value)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "getImageNioseFilter") == 0) {
        bool (*getImageNioseFilter_C_API)(int, unsigned char *);
        unsigned char uc_value;
        char *string;
        cJSON *root_json;
        
        getImageNioseFilter_C_API = dlsym(self->dlh, "getImageNioseFilter_C_API");
        if (!getImageNioseFilter_C_API(self->camera_id, &uc_value)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        root_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(root_json, "ImageNioseFilter", uc_value);
        string = cJSON_Print(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        free(string);
        cJSON_Delete(root_json);
        if (read_size != NULL) {
            *read_size = strlen((char *) read_buffer);
        }
    } else if (strcmp(func_name, "setGammaCorrectionMode") == 0) {
        bool (*setGammaCorrectionMode_C_API)(int, unsigned char);
        unsigned char uc_value;
        
        setGammaCorrectionMode_C_API = dlsym(self->dlh, "setGammaCorrectionMode_C_API");
        if (fscanf(fp, "%c", &uc_value) != 1) {
            fclose(fp);
            return AAOS_EINVAL;
        }
        if (!setGammaCorrectionMode_C_API(self->camera_id, uc_value)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "getGammaCorrectionMode") == 0) {
        bool (*getGammaCorrectionMode_C_API)(int, unsigned char *);
        unsigned char uc_value;
        char *string;
        cJSON *root_json;
        
        getGammaCorrectionMode_C_API = dlsym(self->dlh, "getGammaCorrectionMode_C_API");
        if (!getGammaCorrectionMode_C_API(self->camera_id, &uc_value)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        root_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(root_json, "GammaCorrectionMode", uc_value);
        string = cJSON_Print(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        free(string);
        cJSON_Delete(root_json);
        if (read_size != NULL) {
            *read_size = strlen((char *) read_buffer);
        }
    } else if (strcmp(func_name, "setGammaCorrectionValue") == 0) {
        bool (*setGammaCorrectionValue_C_API)(int, unsigned char);
        unsigned char uc_value;
        
        setGammaCorrectionValue_C_API = dlsym(self->dlh, "setGammaCorrectionValue_C_API");
        if (fscanf(fp, "%c", &uc_value) != 2) {
            fclose(fp);
            return AAOS_EBADCMD;
        }
        if (!setGammaCorrectionValue_C_API(self->camera_id, uc_value)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "getGammaCorrectionValue") == 0) {
        bool (*getGammaCorrectionValue_C_API)(int, unsigned char *);
        unsigned char uc_value;
        char *string;
        cJSON *root_json;
        
        getGammaCorrectionValue_C_API = dlsym(self->dlh, "getGammaCorrectionValue_C_API");
        if (!getGammaCorrectionValue_C_API(self->camera_id, &uc_value)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        root_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(root_json, "GammaCorrectionValue", uc_value);
        string = cJSON_Print(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        free(string);
        cJSON_Delete(root_json);
        if (read_size != NULL) {
            *read_size = strlen((char *) read_buffer);
        }
    } else if (strcmp(func_name, "setNoUniformCorrectionState") == 0) {
        bool (*setNoUniformCorrectionState_C_API)(int, bool);
        bool b_value;
        
        setNoUniformCorrectionState_C_API = dlsym(self->dlh, "setNoUniformCorrectionState_C_API");
        if (fscanf(fp, "%d", &b_value) != 1) {
            fclose(fp);
            return AAOS_EBADCMD;
        }
        if (!setNoUniformCorrectionState_C_API(self->camera_id, b_value)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "getNoUniformCorrectionState") == 0) {
        bool (*getNoUniformCorrectionState_C_API)(int, bool *);
        bool b_value;
        char *string;
        cJSON *root_json;
        
        getNoUniformCorrectionState_C_API = dlsym(self->dlh, "getNoUniformCorrectionState_C_API");
        if (!getNoUniformCorrectionState_C_API(self->camera_id, &b_value)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        root_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(root_json, "NoUniformCorrectionState", b_value);
        string = cJSON_Print(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        free(string);
        cJSON_Delete(root_json);
        if (read_size != NULL) {
            *read_size = strlen((char *) read_buffer);
        }
    } else if (strcmp(func_name, "setBadPointCorrectionState") == 0) {
        bool (*setBadPointCorrectionState_C_API)(int, bool);
        bool b_value;
        
        setBadPointCorrectionState_C_API = dlsym(self->dlh, "setBadPointCorrectionState_C_API");
        if (fscanf(fp, "%d", &b_value) != 2) {
            fclose(fp);
            return AAOS_EBADCMD;
        }
        if (!setBadPointCorrectionState_C_API(self->camera_id, b_value)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "getBadPointCorrectionState") == 0) {
        bool (*getBadPointCorrectionState_C_API)(int, bool *);
        bool b_value;
        char *string;
        cJSON *root_json;
        
        getBadPointCorrectionState_C_API = dlsym(self->dlh, "getBadPointCorrectionState_C_API");
        if (!getBadPointCorrectionState_C_API(self->camera_id, &b_value)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        root_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(root_json, "BadPointCorrectionState", b_value);
        string = cJSON_Print(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        free(string);
        cJSON_Delete(root_json);
        if (read_size != NULL) {
            *read_size = strlen((char *) read_buffer);
        }
    } else if (strcmp(func_name, "setBadPointPosition") == 0) {
        bool (*setBadPointPosition_C_API)(int, unsigned short, unsigned short);
        unsigned short us_value, us_value2;
        
        setBadPointPosition_C_API = dlsym(self->dlh, "setBadPointPosition_C_API");
        if (fscanf(fp, "%hu %hu", &us_value, &us_value2) != 2) {
            fclose(fp);
            return AAOS_EBADCMD;
        }
        if (!setBadPointPosition_C_API(self->camera_id, us_value, us_value2)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "setROIZoonOffset") == 0) {
        bool (*setROIZoonOffset_C_API)(int, unsigned short, unsigned short);
        unsigned short us_value, us_value2;
        
        setROIZoonOffset_C_API = dlsym(self->dlh, "setROIZoonOffset_C_API");
        if (fscanf(fp, "%hu %hu", &us_value, &us_value2) != 3) {
            fclose(fp);
            return AAOS_EBADCMD;
        }
        if (!setROIZoonOffset_C_API(self->camera_id, us_value, us_value2)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "getROIZoonOffset") == 0) {
        bool (*getROIZoonOffset_C_API)(int, unsigned short *, unsigned short *);
        unsigned short us_value, us_value2;
        char *string;
        cJSON *root_json;
        
        getROIZoonOffset_C_API = dlsym(self->dlh, "getROIZoonOffset_C_API");
        if (!getROIZoonOffset_C_API(self->camera_id, &us_value, &us_value2)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        root_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(root_json, "ROIZoonOffsetX", us_value);
        cJSON_AddNumberToObject(root_json, "ROIZoonOffsetY", us_value2);
        string = cJSON_Print(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        free(string);
        cJSON_Delete(root_json);
        if (read_size != NULL) {
            *read_size = strlen((char *) read_buffer);
        }
    } else if (strcmp(func_name, "setROIZoonSize") == 0) {
        bool (*setROIZoonSize_C_API)(int, unsigned short, unsigned short);
        unsigned short us_value, us_value2;
        
        setROIZoonSize_C_API = dlsym(self->dlh, "setROIZoonSize_C_API");
        if (fscanf(fp, "%hu %hu", &us_value, &us_value2) != 3) {
            fclose(fp);
            return AAOS_EBADCMD;
        }
        if (!setROIZoonSize_C_API(self->camera_id, us_value, us_value2)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "getROIZoonSize") == 0) {
        bool (*getROIZoonSize_C_API)(int, unsigned short *, unsigned short *);
        unsigned short us_value, us_value2;
        char *string;
        cJSON *root_json;
        
        getROIZoonSize_C_API = dlsym(self->dlh, "getROIZoonSize_C_API");
        if (!getROIZoonSize_C_API(self->camera_id, &us_value, &us_value2)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        root_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(root_json, "ImageWidth", us_value);
        cJSON_AddNumberToObject(root_json, "ImageHeight", us_value2);
        string = cJSON_Print(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        free(string);
        cJSON_Delete(root_json);
        if (read_size != NULL) {
            *read_size = strlen((char *) read_buffer);
        }
    } else if (strcmp(func_name, "setPalNtscType") == 0) {
        bool (*setPalNtscType_C_API)(int, bool);
        bool b_value;
        
        setPalNtscType_C_API = dlsym(self->dlh, "setPalNtscType_C_API");
        if (fscanf(fp, "%d", &b_value) != 1) {
            fclose(fp);
            return AAOS_EBADCMD;
        }
        if (!setPalNtscType_C_API(self->camera_id, b_value)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "getPalNtscType") == 0) {
        bool (*getPalNtscType_C_API)(int, bool *);
        bool b_value;
        char *string;
        cJSON *root_json;
        
        getPalNtscType_C_API = dlsym(self->dlh, "getPalNtscType_C_API");
        if (!getPalNtscType_C_API(self->camera_id, &b_value)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        root_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(root_json, "PalNtscType", b_value);
        string = cJSON_Print(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        free(string);
        cJSON_Delete(root_json);
        if (read_size != NULL) {
            *read_size = strlen((char *) read_buffer);
        }
    } else if (strcmp(func_name, "RestoreFactorySetting") == 0) {
        bool (*RestoreFactorySetting_C_API)(int);
        
        RestoreFactorySetting_C_API = dlsym(self->dlh, "RestoreFactorySetting_C_API");
        if (!RestoreFactorySetting_C_API(self->camera_id)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "SaveUserSetting") == 0) {
        bool (*SaveUserSetting_C_API)(int);
        
        SaveUserSetting_C_API = dlsym(self->dlh, "SaveUserSetting_C_API");
        if (!SaveUserSetting_C_API(self->camera_id)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "getFpaTemp") == 0) {
        bool (*getFpaTemp_C_API)(int, double *);
        double d_value;
        char *string;
        cJSON *root_json;
        
        getFpaTemp_C_API = dlsym(self->dlh, "getFpaTemp_C_API");
        if (!getFpaTemp_C_API(self->camera_id, &d_value)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        root_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(root_json, "FpaTemp", d_value);
        string = cJSON_Print(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        free(string);
        cJSON_Delete(root_json);
        if (read_size != NULL) {
            *read_size = strlen((char *) read_buffer);
        }
    } else if (strcmp(func_name, "getShellTmp") == 0) {
        bool (*getShellTmp_C_API)(int, unsigned int *);
        unsigned int ui_value;
        char *string;
        cJSON *root_json;
        
        getShellTmp_C_API = dlsym(self->dlh, "getShellTmp_C_API");
        if (!getShellTmp_C_API(self->camera_id, &ui_value)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        root_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(root_json, "ShellTmp", ui_value);
        string = cJSON_Print(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        free(string);
        cJSON_Delete(root_json);
        if (read_size != NULL) {
            *read_size = strlen((char *) read_buffer);
        }
    } else if (strcmp(func_name, "getImageBoardTmp") == 0) {
        bool (*getImageBoardTmp_C_API)(int, unsigned int *);
        unsigned int ui_value;
        char *string;
        cJSON *root_json;
        
        getImageBoardTmp_C_API = dlsym(self->dlh, "getImageBoardTmp_C_API");
        if (!getImageBoardTmp_C_API(self->camera_id, &ui_value)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        root_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(root_json, "ImageBoardTmp", ui_value);
        string = cJSON_Print(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        free(string);
        cJSON_Delete(root_json);
        if (read_size != NULL) {
            *read_size = strlen((char *) read_buffer);
        }
    } else if (strcmp(func_name, "getSignalBoardTmp") == 0) {
        bool (*getSignalBoardTmp_C_API)(int, unsigned int *);
        unsigned int ui_value;
        char *string;
        cJSON *root_json;
        
        getSignalBoardTmp_C_API = dlsym(self->dlh, "getSignalBoardTmp_C_API");
        if (!getSignalBoardTmp_C_API(self->camera_id, &ui_value)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        root_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(root_json, "SignalBoardTmp", ui_value);
        string = cJSON_Print(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        free(string);
        cJSON_Delete(root_json);
        if (read_size != NULL) {
            *read_size = strlen((char *) read_buffer);
        }
    } else if (strcmp(func_name, "setTecCtrlTempMode") == 0) {
        bool (*setTecCtrlTempMode_C_API)(int, unsigned char);
        unsigned char uc_value;
        
        setTecCtrlTempMode_C_API = dlsym(self->dlh, "setTecCtrlTempMode_C_API");
        if (fscanf(fp, "%c", &uc_value) != 1) {
            fclose(fp);
            return AAOS_EBADCMD;
        }
        if (!setTecCtrlTempMode_C_API(self->camera_id, uc_value)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "getTecCtrlTempMode") == 0) {
        bool (*getTecCtrlTempMode_C_API)(int, unsigned char *);
        unsigned char uc_value;
        char *string;
        cJSON *root_json;
        
        getTecCtrlTempMode_C_API = dlsym(self->dlh, "getTecCtrlTempMode_C_API");
        if (!getTecCtrlTempMode_C_API(self->camera_id, &uc_value)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        root_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(root_json, "TecCtrlTempMode", uc_value);
        string = cJSON_Print(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        free(string);
        cJSON_Delete(root_json);
        if (read_size != NULL) {
            *read_size = strlen((char *) read_buffer);
        }
    } else if (strcmp(func_name, "setTecCtrlTemp") == 0) {
        bool (*setTecCtrlTemp_C_API)(int, unsigned char);
        unsigned char uc_value;
        
        setTecCtrlTemp_C_API = dlsym(self->dlh, "setTecCtrlTemp_C_API");
        if (fscanf(fp, "%c", &uc_value) != 2) {
            fclose(fp);
            return AAOS_EBADCMD;
        }
        if (!setTecCtrlTemp_C_API(self->camera_id, uc_value)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "getTecCtrlTemp") == 0) {
        bool (*getTecCtrlTemp_C_API)(int, double *, unsigned char *);
        unsigned char uc_value;
        double d_value;
        char *string;
        cJSON *root_json;
        
        getTecCtrlTemp_C_API = dlsym(self->dlh, "getTecCtrlTemp_C_API");
        if (!getTecCtrlTemp_C_API(self->camera_id, &d_value, &uc_value)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        root_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(root_json, "TecCtrlTemp", d_value);
        cJSON_AddNumberToObject(root_json, "TapPosition", uc_value);
        string = cJSON_Print(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        free(string);
        cJSON_Delete(root_json);
        if (read_size != NULL) {
            *read_size = strlen((char *) read_buffer);
        }
    } else if (strcmp(func_name, "getExtFunctionInfo") == 0) {
        bool (*getExtFunctionInfo_C_API)(int, unsigned int *);
        unsigned int ui_value;
        char *string;
        cJSON *root_json;
        
        getExtFunctionInfo_C_API = dlsym(self->dlh, "getExtFunctionInfo_C_API");
        if (!getExtFunctionInfo_C_API(self->camera_id, &ui_value)) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        root_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(root_json, "ExtFunctionInfo", ui_value);
        string = cJSON_Print(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        free(string);
        cJSON_Delete(root_json);
        if (read_size != NULL) {
            *read_size = strlen((char *) read_buffer);
        }
    } else {
        ret = AAOS_EBADCMD;
    }
    
error:
    fclose(fp);
    return ret;
}

static int
LeadingCamera_init(void *_self)
{
    struct LeadingCamera *self = cast(LeadingCamera(), _self);
    
    bool (*getMaxResolution_C_API)(int, unsigned int*, unsigned int *);
    bool (*getAutoExposeMinTime_C_API)(int, unsigned int *);
    bool (*getAutoExposeMaxTime_C_API)(int, unsigned int *);
    bool (*setImageFormat_C_API)(int, unsigned char);
    bool (*setTriggerMode_C_API)(int, unsigned char);
    bool (*setExposureMode_C_API)(int, bool);
    bool (*setExposeRegion_C_API)(int, unsigned char);
    bool (*setGainMode_C_API)(int, bool);
    bool (*getGainValue_C_API)(int, unsigned char *);
    bool (*getROIZoonOffset_C_API)(int, unsigned short *, unsigned short *);
    bool (*getROIZoonSize_C_API)(int, unsigned short *, unsigned short *);
    bool (*setTecCtrlTempMode_C_API)(int, unsigned char);
    bool (*getTecCtrlTemp_C_API)(int, double *, unsigned char *);
    
    unsigned short x_offset, y_offset, image_width, image_height;
    unsigned int width, height, auto_exposure_time_min, auto_exposure_time_max;
    unsigned char gain, tap;
    
    getMaxResolution_C_API = dlsym(self->dlh, "getMaxResolution_C_API");
    getAutoExposeMinTime_C_API = dlsym(self->dlh, "getAutoExposeMinTime_C_API");
    getAutoExposeMaxTime_C_API = dlsym(self->dlh, "getAutoExposeMaxTime_C_API");
    setImageFormat_C_API = dlsym(self->dlh, "setImageFormat_C_API");
    setTriggerMode_C_API= dlsym(self->dlh, "setTriggerMode_C_API");
    setExposureMode_C_API = dlsym(self->dlh, "setExposureMode_C_API");
    setExposeRegion_C_API = dlsym(self->dlh, "setExposeRegion_C_API");
    setGainMode_C_API = dlsym(self->dlh, "setGainMode_C_API");
    getGainValue_C_API = dlsym(self->dlh, "getGainValue_C_API");
    getROIZoonOffset_C_API = dlsym(self->dlh, "getROIZoonOffset_C_API");
    getROIZoonSize_C_API = dlsym(self->dlh, "getROIZoonSize_C_API");
    setTecCtrlTempMode_C_API = dlsym(self->dlh, "setTecCtrlTempMode_C_API");
    
    if (getMaxResolution_C_API(self->camera_id, &width, &height)) {
        self->_.d_cap.width = (size_t) width;
        self->_.d_cap.height = (size_t) height;
    }
    
    self->_.d_cap.pixel_format_available = true;
    self->_.d_cap.n_pixel_format = 3;
    self->_.d_cap.pixel_format_array = (uint32_t *) Malloc(3 * sizeof(uint32_t));
    self->_.d_cap.pixel_format_array[0] = DETECTOR_PIXEL_FORMAT_MONO_8;
    self->_.d_cap.pixel_format_array[1] = DETECTOR_PIXEL_FORMAT_MONO_14;
    self->_.d_cap.pixel_format_array[2] = DETECTOR_PIXEL_FORMAT_YUV422;
    if (setImageFormat_C_API(self->camera_id, 1)) {
        self->_.d_param.pixel_format = DETECTOR_PIXEL_FORMAT_MONO_14;
    }
    
    self->_.d_cap.trigger_available = true;
    self->_.d_cap.n_trigger_mode = 5;
    self->_.d_cap.trigger_mode_array = (uint32_t *) Malloc(5 * sizeof(uint32_t));
    self->_.d_cap.trigger_mode_array[1] = DETECTOR_TRIGGER_MODE_HARDWARE_EDGE_FALL;
    self->_.d_cap.trigger_mode_array[2] = DETECTOR_TRIGGER_MODE_HARDWARE_EDGE_RISE;
    self->_.d_cap.trigger_mode_array[3] = DETECTOR_TRIGGER_MODE_HARDWARE_LEVEL_HIGH;
    self->_.d_cap.trigger_mode_array[4] = DETECTOR_TRIGGER_MODE_HARDWARE_LEVEL_LOW;
    if (setTriggerMode_C_API(self->camera_id, 0)) {
        self->_.d_param.trigger_mode = DETECTOR_TRIGGER_MODE_DEFAULT;
    }
    
    self->_.d_cap.auto_exposure_time_available = true;
    if (getAutoExposeMinTime_C_API(self->camera_id, &auto_exposure_time_min)) {
        self->_.d_cap.auto_exposure_time_min = auto_exposure_time_min/1000000.;
    }
    if (getAutoExposeMaxTime_C_API(self->camera_id, &auto_exposure_time_max)) {
        self->_.d_cap.auto_exposure_time_max = auto_exposure_time_max/1000000.;
    }
    if (setExposureMode_C_API(self->camera_id, false)) {
        self->_.d_param.auto_exposure_time_enable = false;
    }
    
    self->_.d_cap.frame_rate_available = true;
    
    self->_.d_cap.gain_available = true;
    self->_.d_cap.auto_gain_available = true;
    if (setGainMode_C_API(self->camera_id, false)) {
        self->_.d_param.auto_gain_enable = false;
    }
    if (getGainValue_C_API(self->camera_id, &gain)) {
        switch (gain) {
            case 0:
                self->_.d_param.gain = 1.;
                break;
            case 1:
                self->_.d_param.gain = 2.;
                break;
            case 2:
                self->_.d_param.gain = 3.;
                break;
            default:
                break;
        }
    }
    
    self->_.d_cap.offset_available = true;
    setExposeRegion_C_API(self->camera_id, 0);
    if (getROIZoonOffset_C_API(self->camera_id, &x_offset, &y_offset)) {
        self->_.d_param.x_offset = x_offset;
        self->_.d_param.y_offset = y_offset;
    }
    if (getROIZoonSize_C_API(self->camera_id, &image_width, &image_height)) {
        self->_.d_param.image_width = image_width;
        self->_.d_param.image_height = image_height;
    }
    
    self->_.d_cap.cooling_available = true;
    self->_.d_cap.auto_cooling_available = true;
    if (setTecCtrlTempMode_C_API(self->camera_id, 1)) {
        self->_.d_param.auto_cooling_enable = false;
        self->_.d_param.is_cooling_enable = true;
    }
    getTecCtrlTemp_C_API(self->camera_id, &self->_.d_param.temperature, &tap);
    
    return AAOS_OK;
}

static int
LeadingCamera_status_json(struct LeadingCamera *self, void *buffer, size_t size)
{
    unsigned int state, options;
    uint32_t image_width, image_height, x_offset, y_offset, x_binning, y_binning, pixel_format;
    double exposure_time, frame_rate, gain, readout_rate, cooling_temperature;
    bool auto_exposure_time_enable, auto_frame_rate_enable, auto_gain_enable, auto_readout_rate_enable, auto_cooling_enable;
    cJSON *root_json;
    char *string;
    bool (*getTecCtrlTemp_C_API)(int, double *, unsigned char *);
    unsigned char uc_value;
    double d_value;
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    Pthread_rwlock_rdlock(&self->_.d_param.rwlock);
    image_width = self->_.d_param.image_width;
    image_height = self->_.d_param.image_height;
    x_offset = self->_.d_param.x_offset;
    y_offset = self->_.d_param.y_offset;
    x_binning = self->_.d_param.x_binning;
    y_binning = self->_.d_param.y_binning;
    auto_exposure_time_enable = self->_.d_param.auto_exposure_time_enable;
    exposure_time = self->_.d_param.exposure_time;
    auto_frame_rate_enable = self->_.d_param.auto_frame_rate_enable;
    frame_rate = self->_.d_param.frame_rate;
    auto_gain_enable = self->_.d_param.auto_gain_enable;
    gain = self->_.d_param.gain;
    auto_readout_rate_enable = self->_.d_param.auto_readout_rate_enable;
    readout_rate = self->_.d_param.readout_rate;
    pixel_format = self->_.d_param.pixel_format;
    auto_cooling_enable = self->_.d_param.auto_cooling_enable;
    Pthread_rwlock_unlock(&self->_.d_param.rwlock);
    
    
    root_json = cJSON_CreateObject();
    cJSON_AddStringToObject(root_json, "name", self->_.name);
    if (state&DETECTOR_STATE_MALFUNCTION) {
        cJSON_AddStringToObject(root_json, "status", "MALFUNCTION");
    }
    switch (state&(~DETECTOR_STATE_MALFUNCTION)) {
        case DETECTOR_STATE_IDLE:
            cJSON_AddStringToObject(root_json, "state", "IDLE");
            break;
        case DETECTOR_STATE_OFFLINE:
            cJSON_AddStringToObject(root_json, "state", "OFFLINE");
            break;
        case DETECTOR_STATE_UNINITIALIZED:
            cJSON_AddStringToObject(root_json, "state", "UNINITIALIZED");
            break;
        case DETECTOR_STATE_EXPOSING:
            cJSON_AddStringToObject(root_json, "state", "EXPOSING");
            break;
        case DETECTOR_STATE_READING:
            cJSON_AddStringToObject(root_json, "state", "READING");
            break;
        default:
            break;
    }
    
    cJSON_AddNumberToObject(root_json, "image_width", (double) image_width);
    cJSON_AddNumberToObject(root_json, "image_height", (double) image_height);
    cJSON_AddNumberToObject(root_json, "x_offset", (double) x_offset);
    cJSON_AddNumberToObject(root_json, "y_offset", (double) y_offset);
    if (self->_.d_cap.binning_available) {
        cJSON_AddNumberToObject(root_json, "x_binning", (double) x_binning);
        cJSON_AddNumberToObject(root_json, "y_binning", (double) y_binning);
    }
    if (self->_.d_cap.auto_exposure_time_available) {
        if (auto_exposure_time_enable) {
            cJSON_AddStringToObject(root_json, "auto_exposure_time_enable", "true");
        } else {
            cJSON_AddStringToObject(root_json, "auto_exposure_time_enable", "false");
        }
    }
    cJSON_AddNumberToObject(root_json, "exposure_time", exposure_time);
    if (self->_.d_cap.auto_frame_rate_available) {
        if (auto_frame_rate_enable) {
            cJSON_AddStringToObject(root_json, "auto_frame_rate_enable", "true");
        } else {
            cJSON_AddStringToObject(root_json, "auto_frame_rate_enable", "false");
        }
    }
    cJSON_AddNumberToObject(root_json, "frame_rate", frame_rate);
    if (self->_.d_cap.auto_frame_rate_available) {
        if (auto_frame_rate_enable) {
            cJSON_AddStringToObject(root_json, "auto_frame_rate_enable", "true");
        } else {
            cJSON_AddStringToObject(root_json, "auto_frame_rate_enable", "false");
        }
    }
    cJSON_AddNumberToObject(root_json, "gain", gain);
    if (self->_.d_cap.auto_gain_available) {
        if (auto_gain_enable) {
            cJSON_AddStringToObject(root_json, "auto_gain_enable", "true");
        } else {
            cJSON_AddStringToObject(root_json, "auto_gain_enable", "false");
        }
    }
    if (self->_.d_cap.auto_readout_rate_available) {
        if (auto_readout_rate_enable) {
            cJSON_AddStringToObject(root_json, "auto_readout_rate_enable", "true");
        } else {
            cJSON_AddStringToObject(root_json, "auto_readout_rate_enable", "false");
        }
    }
    cJSON_AddNumberToObject(root_json, "readout_rate", readout_rate);
    cJSON_AddStringToObject(root_json, "pixel_format", pixel_format_string[pixel_format]);

    if (self->_.d_param.is_cooling_enable) {
    cJSON_AddStringToObject(root_json, "cooling_enabled", "true");
    } else {
    cJSON_AddStringToObject(root_json, "cooling_enabled", "false");
    }

    if (self->_.d_cap.auto_cooling_available) {
        if (auto_cooling_enable) {
            cJSON_AddStringToObject(root_json, "auto_cooling_enabled", "true");
        } else {
            cJSON_AddStringToObject(root_json, "auto_cooling_enabled", "false");
        }
    }

    if (self->_.d_cap.cooling_available && self->_.d_param.is_cooling_enable && !self->_.d_cap.auto_cooling_available) {
        cJSON_AddNumberToObject(root_json, "setting_temperature", self->_.d_param.temperature);
    }
    
    getTecCtrlTemp_C_API = dlsym(self->dlh, "getTecCtrlTemp_C_API");
    if (getTecCtrlTemp_C_API(self->camera_id, &d_value, &uc_value)) {
        cJSON_AddNumberToObject(root_json, "actual_temperature", d_value);
    }
    
    string = cJSON_Print(root_json);
    cJSON_Delete(root_json);
    snprintf(buffer, size, "%s", string);
    free(string);
    
    return AAOS_OK;
}

static int
LeadingCamera_status(void *_self, void *buffer, size_t size)
{
    struct LeadingCamera *self = cast(LeadingCamera(), _self);
    
    LeadingCamera_status_json(self, buffer, size);
    
    return AAOS_OK;
}

static void
LeadingCamera_info_json(struct LeadingCamera *self, void *buffer, size_t size)
{
    cJSON *root_json, *leading_json, *capability_json, *array_json;
    char *string = NULL;
    size_t i;
    void (*LeadingCameraCopyInfo)(void *, void *, char *, char *, char *, char *, char *, char *, char *, char *, char *);
    char model_name[32], device_version[32], manufacture_spec_info[48], serial_number[16], user_defined_name[16], ip[16], mask[16], gateway[16], description[132];
    
    root_json = cJSON_CreateObject();
    cJSON_AddStringToObject(root_json, "name", self->_.name);
    if (self->_.description != NULL) {
        cJSON_AddStringToObject(root_json, "description", self->_.description);
    }
    
    LeadingCameraCopyInfo = dlsym(self->dlh, "LeadingCameraCopyInfo");
    LeadingCameraCopyInfo(self->device_list[self->camera_index], self->interface_list[self->camera_index], model_name, device_version, manufacture_spec_info, serial_number, user_defined_name, ip, mask, gateway, description);
    leading_json = cJSON_CreateObject();
    cJSON_AddNumberToObject(leading_json, "camera_index", self->camera_index);
    cJSON_AddNumberToObject(leading_json, "camera_id", self->camera_id);
    cJSON_AddStringToObject(leading_json, "ModelName", model_name);
    cJSON_AddStringToObject(leading_json, "DeviceVersion", device_version);
    cJSON_AddStringToObject(leading_json, "ManufactureSpecInfo", manufacture_spec_info);
    cJSON_AddStringToObject(leading_json, "SerialNumber", serial_number);
    cJSON_AddStringToObject(leading_json, "UserDefinedName", user_defined_name);
    cJSON_AddStringToObject(leading_json, "IP", ip);
    cJSON_AddStringToObject(leading_json, "Mask", mask);
    cJSON_AddStringToObject(leading_json, "Gateway", gateway);
    cJSON_AddStringToObject(leading_json, "Description", description);
    cJSON_AddItemToObject(root_json, "LeadingCamera", leading_json);
     
    capability_json = cJSON_CreateObject();
    cJSON_AddNumberToObject(capability_json, "width", self->_.d_cap.width);
    cJSON_AddNumberToObject(capability_json, "heigth", self->_.d_cap.height);
    cJSON_AddNumberToObject(capability_json, "x_n_chip", self->_.d_cap.x_n_chip);
    cJSON_AddNumberToObject(capability_json, "y_n_chip", self->_.d_cap.y_n_chip);
    cJSON_AddNumberToObject(capability_json, "n_chip", self->_.d_cap.n_chip);
    if (self->_.d_cap.flip_map != NULL) {
        
    }
    if (self->_.d_cap.mirror_map != NULL) {
        
    }
    
    if (self->_.d_cap.binning_available) {
        cJSON_AddStringToObject(capability_json, "binning_available", "true");
        if (self->_.d_cap.x_binning_array != NULL && self->_.d_cap.y_binning_array != NULL) {
            char binning_buffer[BUFSIZE];
            FILE *fp;
            size_t i;
            fp = fmemopen(binning_buffer, BUFSIZE, "w");
            for (i = 0; i < self->_.d_cap.n_x_binning - 1; i++) {
                fprintf(fp, "(%d,%d) ", self->_.d_cap.x_binning_array[i], self->_.d_cap.y_binning_array[i]);
            }
            fprintf(fp, "(%d,%d)", self->_.d_cap.x_binning_array[self->_.d_cap.n_x_binning - 1], self->_.d_cap.y_binning_array[self->_.d_cap.n_x_binning - 1]);
            cJSON_AddStringToObject(capability_json, "binning_array", binning_buffer);
        } else {
            cJSON_AddNumberToObject(capability_json, "x_binning_min", self->_.d_cap.x_binning_min);
            cJSON_AddNumberToObject(capability_json, "x_binning_max", self->_.d_cap.x_binning_max);
            cJSON_AddNumberToObject(capability_json, "y_binning_max", self->_.d_cap.y_binning_min);
            cJSON_AddNumberToObject(capability_json, "y_binning_max", self->_.d_cap.y_binning_max);
        }
    } else {
        cJSON_AddStringToObject(capability_json, "binning_available", "false");
    }
    
    if (self->_.d_cap.offset_available) {
        cJSON_AddStringToObject(capability_json, "offset_available", "true");
    } else {
        cJSON_AddStringToObject(capability_json, "offset_available", "false");
    }
    
    if (self->_.d_cap.gain_available) {
        cJSON_AddStringToObject(capability_json, "gain_available", "true");
        if (self->_.d_cap.gain_array == NULL) {
            cJSON_AddNumberToObject(capability_json, "gain_min", self->_.d_cap.gain_min);
            cJSON_AddNumberToObject(capability_json, "gain_max", self->_.d_cap.gain_max);
        } else {
            array_json = cJSON_CreateArray();
            for (i = 0; i < self->_.d_cap.n_gain; i++) {
                cJSON_AddItemToArray(array_json, cJSON_CreateNumber(self->_.d_cap.gain_array[i]));
            }
            cJSON_AddItemToObject(capability_json, "gain_array", array_json);
        }
    } else {
        cJSON_AddStringToObject(capability_json, "gain_available", "false");
    }
    if (self->_.d_cap.auto_gain_available) {
        cJSON_AddStringToObject(capability_json, "auto_gain_available", "true");
        cJSON_AddNumberToObject(capability_json, "auto_gain_min", self->_.d_cap.auto_gain_min);
        cJSON_AddNumberToObject(capability_json, "auto_gain_max", self->_.d_cap.auto_gain_max);
    } else {
        cJSON_AddStringToObject(capability_json, "auto_gain_available", "false");
    }
    
    if (self->_.d_cap.exposure_time_available) {
        cJSON_AddStringToObject(capability_json, "exposure_time_available", "true");
        if (self->_.d_cap.exposure_time_array == NULL) {
            cJSON_AddNumberToObject(capability_json, "exposure_time_min", self->_.d_cap.exposure_time_min);
            cJSON_AddNumberToObject(capability_json, "exposure_time_max", self->_.d_cap.exposure_time_max);
        } else {
            array_json = cJSON_CreateArray();
            for (i = 0; i < self->_.d_cap.n_exposure_time; i++) {
                cJSON_AddItemToArray(array_json, cJSON_CreateNumber(self->_.d_cap.exposure_time_array[i]));
            }
            cJSON_AddItemToObject(capability_json, "exposure_time_array", array_json);
        }
    } else {
        cJSON_AddStringToObject(capability_json, "exposure_time_available", "false");
    }
    if (self->_.d_cap.auto_frame_rate_available) {
        cJSON_AddStringToObject(capability_json, "auto_exposure_time_available", "true");
        cJSON_AddNumberToObject(capability_json, "auto_exposure_time_min", self->_.d_cap.auto_exposure_time_min);
        cJSON_AddNumberToObject(capability_json, "auto_exposure_time_max", self->_.d_cap.auto_exposure_time_max);
    } else {
        cJSON_AddStringToObject(capability_json, "auto_exposure_time_available", "false");
    }
    
    if (self->_.d_cap.frame_rate_available) {
        cJSON_AddStringToObject(capability_json, "frame_rate_available", "true");
        if (self->_.d_cap.frame_rate_array == NULL) {
            cJSON_AddNumberToObject(capability_json, "frame_rate_min", self->_.d_cap.frame_rate_min);
            cJSON_AddNumberToObject(capability_json, "frame_rate_max", self->_.d_cap.frame_rate_max);
        } else {
            array_json = cJSON_CreateArray();
            for (i = 0; i < self->_.d_cap.n_frame_rate; i++) {
                cJSON_AddItemToArray(array_json, cJSON_CreateNumber(self->_.d_cap.frame_rate_array[i]));
            }
            cJSON_AddItemToObject(capability_json, "frame_rate_array", array_json);
        }
    } else {
        cJSON_AddStringToObject(capability_json, "frame_rate_available", "false");
    }
    if (self->_.d_cap.auto_frame_rate_available) {
        cJSON_AddStringToObject(capability_json, "auto_frame_rate_available", "true");
        cJSON_AddNumberToObject(capability_json, "auto_frame_rate_min", self->_.d_cap.auto_frame_rate_min);
        cJSON_AddNumberToObject(capability_json, "auto_frame_rate_max", self->_.d_cap.auto_frame_rate_max);
    } else {
        cJSON_AddStringToObject(capability_json, "auto_frame_rate_available", "false");
    }
    
    if (self->_.d_cap.pixel_format_available) {
        cJSON_AddStringToObject(capability_json, "pixel_format_available", "true");
        array_json = cJSON_CreateArray();
        for (i = 0; i < self->_.d_cap.n_pixel_format; i++) {
            switch (self->_.d_cap.pixel_format_array[i]) {
                case DETECTOR_PIXEL_FORMAT_MONO_8:
                    cJSON_AddItemToArray(array_json, cJSON_CreateString("MONO_8"));
                    break;
                case DETECTOR_PIXEL_FORMAT_MONO_10:
                    cJSON_AddItemToArray(array_json, cJSON_CreateString("MONO_10"));
                    break;
                case DETECTOR_PIXEL_FORMAT_MONO_10_PACKED:
                    cJSON_AddItemToArray(array_json, cJSON_CreateString("MONO_10_PACKED"));
                    break;
                case DETECTOR_PIXEL_FORMAT_MONO_12:
                    cJSON_AddItemToArray(array_json, cJSON_CreateString("MONO_12"));
                    break;
                case DETECTOR_PIXEL_FORMAT_MONO_12_PACKED:
                    cJSON_AddItemToArray(array_json, cJSON_CreateString("MONO_12_PACKED"));
                    break;
                case DETECTOR_PIXEL_FORMAT_MONO_14:
                    cJSON_AddItemToArray(array_json, cJSON_CreateString("MONO_14"));
                    break;
                case DETECTOR_PIXEL_FORMAT_MONO_14_PACKED:
                    cJSON_AddItemToArray(array_json, cJSON_CreateString("MONO_14_PACKED"));
                    break;
                case DETECTOR_PIXEL_FORMAT_MONO_16:
                    cJSON_AddItemToArray(array_json, cJSON_CreateString("MONO_16"));
                    break;
                case DETECTOR_PIXEL_FORMAT_MONO_18:
                    cJSON_AddItemToArray(array_json, cJSON_CreateString("MONO_18"));
                    break;
                case DETECTOR_PIXEL_FORMAT_MONO_18_PACKED:
                    cJSON_AddItemToArray(array_json, cJSON_CreateString("MONO_18_PACKED"));
                    break;
                case DETECTOR_PIXEL_FORMAT_MONO_24:
                    cJSON_AddItemToArray(array_json, cJSON_CreateString("MONO_24"));
                    break;
                case DETECTOR_PIXEL_FORMAT_MONO_24_PACKED:
                    cJSON_AddItemToArray(array_json, cJSON_CreateString("MONO_24_PACKED"));
                    break;
                case DETECTOR_PIXEL_FORMAT_MONO_32:
                    cJSON_AddItemToArray(array_json, cJSON_CreateString("MONO_32"));
                    break;
                case DETECTOR_PIXEL_FORMAT_MONO_64:
                    cJSON_AddItemToArray(array_json, cJSON_CreateString("MONO_64"));
                    break;
                case DETECTOR_PIXEL_FORMAT_RGB_24:
                    cJSON_AddItemToArray(array_json, cJSON_CreateString("RGB_24"));
                    break;
                case DETECTOR_PIXEL_FORMAT_YUV422:
                    cJSON_AddItemToArray(array_json, cJSON_CreateString("YUV_422"));
                    break;
                default:
                    break;
            }
        }
        cJSON_AddItemToObject(capability_json, "pixel_format_array", array_json);
    } else {
        cJSON_AddStringToObject(capability_json, "pixel_format_available", "false");
    }
    
    if (self->_.d_cap.readout_rate_available) {
        cJSON_AddStringToObject(capability_json, "readout_rate_available", "true");
        if (self->_.d_cap.readout_rate_array == NULL) {
            cJSON_AddNumberToObject(capability_json, "readout_rate_min", self->_.d_cap.readout_rate_min);
            cJSON_AddNumberToObject(capability_json, "readout_rate_max", self->_.d_cap.readout_rate_max);
        } else {
            array_json = cJSON_CreateArray();
            for (i = 0; i < self->_.d_cap.n_readout_rate; i++) {
                cJSON_AddItemToArray(array_json, cJSON_CreateNumber(self->_.d_cap.readout_rate_array[i]));
            }
            cJSON_AddItemToObject(capability_json, "readout_rate_array", array_json);
        }
    } else {
        cJSON_AddStringToObject(capability_json, "readout_rate_available", "false");
    }
    if (self->_.d_cap.auto_readout_rate_available) {
        cJSON_AddStringToObject(capability_json, "auto_readout_rate_available", "true");
        cJSON_AddNumberToObject(capability_json, "auto_readout_rate_min", self->_.d_cap.auto_readout_rate_min);
        cJSON_AddNumberToObject(capability_json, "auto_readout_rate_max", self->_.d_cap.auto_readout_rate_max);
    } else {
        cJSON_AddStringToObject(capability_json, "auto_readout_rate_available", "false");
    }
    
    if (self->_.d_cap.cooling_available) {
        cJSON_AddStringToObject(capability_json, "cooling_available", "true");
        cJSON_AddNumberToObject(capability_json, "cooling_temperature_min", self->_.d_cap.cooling_temperature_min);
        cJSON_AddNumberToObject(capability_json, "cooling_temperature_max", self->_.d_cap.cooling_temperature_max);
            
    } else {
        cJSON_AddStringToObject(capability_json, "cooling_available", "false");
    }
    if (self->_.d_cap.auto_cooling_available) {
        cJSON_AddStringToObject(capability_json, "auto_cooling_available", "true");
        cJSON_AddNumberToObject(capability_json, "auto_cooling_temperature_min", self->_.d_cap.auto_cooling_temperature_min);
        cJSON_AddNumberToObject(capability_json, "auto_cooling_temperature_max", self->_.d_cap.auto_cooling_temperature_max);
    } else {
        cJSON_AddStringToObject(capability_json, "auto_cooling_available", "false");
    }
    cJSON_AddItemToObject(root_json, "capability", capability_json);
    
    string = cJSON_Print(root_json);
    cJSON_Delete(root_json);
    snprintf(buffer, size, "%s", string);
    free(string);
}

static int
LeadingCamera_info(void *_self, void *buffer, size_t size)
{
    struct LeadingCamera *self = cast(LeadingCamera(), _self);
    
    LeadingCamera_info_json(self, buffer, size);
    
    return AAOS_OK;
}

static int
LeadingCamera_set_exposure_time(void *_self, double exposure_time)
{
    struct LeadingCamera *self = cast(LeadingCamera(), _self);
    
    unsigned int state;
    uint16_t options;
    bool (*setExposureTime_C_API)(int, unsigned int);
    unsigned int exposure_time_us = (unsigned int) exposure_time * 1000000.;
    int ret;
    
    setExposureTime_C_API = dlsym(self->dlh, "setExposureTime_C_API");
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    if (exposure_time > self->_.d_cap.exposure_time_max || exposure_time < self->_.d_cap.exposure_time_min) {
        ret = AAOS_EINVAL;
        goto error;
    }
    if ((state&DETECTOR_STATE_MALFUNCTION) && !(options&DETECTOR_OPTION_IGNORE_DEVMAL)) {
        ret = AAOS_EDEVMAL;
        goto error;
    } else if (state&DETECTOR_STATE_OFFLINE) {
        ret = AAOS_EPWROFF;
        goto error;
    } else if (state&DETECTOR_STATE_UNINITIALIZED) {
        ret = AAOS_EUNINIT;
        goto error;
    } else if ((state&DETECTOR_STATE_EXPOSING) || (state&DETECTOR_STATE_READING)) {
        if (options&DETECTOR_OPTION_NOWAIT) {
            ret = AAOS_EBUSY;
            goto error;
        }
        while ((state&DETECTOR_STATE_EXPOSING) || (state&DETECTOR_STATE_READING)) {
            Pthread_cond_wait(&self->_.d_state.cond, &self->_.d_state.mtx);
        }
    }
    if (!setExposureTime_C_API(self->camera_id, exposure_time_us)) {
        ret = AAOS_ERROR;
        goto error;
    }
    Pthread_rwlock_wrlock(&self->_.d_param.rwlock);
    self->_.d_param.exposure_time = exposure_time;
    Pthread_rwlock_unlock(&self->_.d_param.rwlock);
error:
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    return ret;
}

static int
LeadingCamera_get_exposure_time(void *_self, double *exposure_time)
{
    struct LeadingCamera *self = cast(LeadingCamera(), _self);
    
    unsigned int state;
    uint16_t options;
    bool (*getExposureTime_C_API)(int, unsigned int *);
    unsigned int exposure_time_us;
    int ret;
    
    getExposureTime_C_API = dlsym(self->dlh, "setExposureTime_C_API");
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    if ((state&DETECTOR_STATE_MALFUNCTION) && !(options&DETECTOR_OPTION_IGNORE_DEVMAL)) {
        ret = AAOS_EDEVMAL;
        goto error;
    } else if (state&DETECTOR_STATE_OFFLINE) {
        ret = AAOS_EPWROFF;
        goto error;
    } else if (state&DETECTOR_STATE_UNINITIALIZED) {
        ret = AAOS_EUNINIT;
        goto error;
    }
    if (!getExposureTime_C_API(self->camera_id, &exposure_time_us)) {
        ret = AAOS_ERROR;
        goto error;
    }
    *exposure_time = exposure_time_us / 1000000.;
error:
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    return ret;
}


static int
LeadingCamera_set_exposure_time_nl(void *_self, double exposure_time)
{
    struct LeadingCamera *self = cast(LeadingCamera(), _self);
    
    bool (*setExposureTime_C_API)(int, unsigned int);
    int ret = AAOS_OK;
    unsigned int exposure_time_us = (unsigned int) exposure_time * 1000000;
    
    setExposureTime_C_API = dlsym(self->dlh, "setExposureTime_C_API");
    
    if (exposure_time > self->_.d_cap.exposure_time_max || exposure_time < self->_.d_cap.exposure_time_min) {
        ret = AAOS_EINVAL;
        goto error;
    }
    
    if (!setExposureTime_C_API(self->camera_id, exposure_time_us)) {
        ret = AAOS_EDEVMAL;
        goto error;
    }
    
    Pthread_rwlock_wrlock(&self->_.d_param.rwlock);
    self->_.d_param.exposure_time = exposure_time;
    Pthread_rwlock_unlock(&self->_.d_param.rwlock);
    
error:
    return ret;
}

static int
LeadingCamera_set_frame_rate(void *_self, double frame_rate)
{
    struct LeadingCamera *self = cast(LeadingCamera(), _self);
    
    unsigned int state;
    uint16_t options;
    bool (*setFrameFrequency_C_API)(int, unsigned int);
    int ret = AAOS_OK;
    
    setFrameFrequency_C_API = dlsym(self->dlh, "setFrameFrequency_C_API");
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    if ((state&DETECTOR_STATE_MALFUNCTION) && !(options&DETECTOR_OPTION_IGNORE_DEVMAL)) {
        ret = AAOS_EDEVMAL;
        goto error;
    } else if (state&DETECTOR_STATE_OFFLINE) {
        ret = AAOS_EPWROFF;
        goto error;
    } else if (state&DETECTOR_STATE_UNINITIALIZED) {
        ret = AAOS_EUNINIT;
        goto error;
    } else if ((state&DETECTOR_STATE_EXPOSING) || (state&DETECTOR_STATE_READING)) {
        if (options&DETECTOR_OPTION_NOWAIT) {
            ret = AAOS_EBUSY;
            goto error;
        }
        while ((state&DETECTOR_STATE_EXPOSING) || (state&DETECTOR_STATE_READING)) {
            Pthread_cond_wait(&self->_.d_state.cond, &self->_.d_state.mtx);
        }
    }
    if (!setFrameFrequency_C_API(self->camera_id, (unsigned int) frame_rate)) {
        ret = AAOS_EDEVMAL;
        goto error;
    }
    Pthread_rwlock_wrlock(&self->_.d_param.rwlock);
    self->_.d_param.frame_rate = frame_rate;
    Pthread_rwlock_unlock(&self->_.d_param.rwlock);
error:
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    return ret;
}

static int
LeadingCamera_get_frame_rate(void *_self, double *frame_rate)
{
    struct LeadingCamera *self = cast(LeadingCamera(), _self);
    
    unsigned int state;
    uint16_t options;
    bool (*getFrameFrequency_C_API)(int, unsigned int *);
    unsigned int ui_value;
    int ret = AAOS_OK;
    
    getFrameFrequency_C_API = dlsym(self->dlh, "getFrameFrequency_C_API");
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    if ((state&DETECTOR_STATE_MALFUNCTION) && !(options&DETECTOR_OPTION_IGNORE_DEVMAL)) {
        ret = AAOS_EDEVMAL;
        goto error;
    } else if (state&DETECTOR_STATE_OFFLINE) {
        ret = AAOS_EPWROFF;
        goto error;
    } else if (state&DETECTOR_STATE_UNINITIALIZED) {
        ret = AAOS_EUNINIT;
        goto error;
    }
    if (!getFrameFrequency_C_API(self->camera_id, &ui_value)) {
        ret = AAOS_EDEVMAL;
        goto error;
    }
    *frame_rate = (double) ui_value;
error:
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    return ret;
}

static int
LeadingCamera_set_gain(void *_self, double gain)
{
    struct LeadingCamera *self = cast(LeadingCamera(), _self);
    
    unsigned int state;
    uint16_t options;
    bool (*setGainValue_C_API)(int, unsigned char);
    int ret = AAOS_OK;
    unsigned char uc_value = 0;
    size_t i, n = self->_.d_cap.n_gain;
    
    setGainValue_C_API = dlsym(self->dlh, "setGainValue_C_API");
    
    for (i = 0; i < n; i++) {
        if (fabs(gain - self->_.d_cap.gain_array[i]) < 0.0001) {
            uc_value = i + 1;
            break;
        }
    }
    if (uc_value == 0) {
        return AAOS_EINVAL;
    }
    uc_value++;
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    if ((state&DETECTOR_STATE_MALFUNCTION) && !(options&DETECTOR_OPTION_IGNORE_DEVMAL)) {
        ret = AAOS_EDEVMAL;
        goto error;
    } else if (state&DETECTOR_STATE_OFFLINE) {
        ret = AAOS_EPWROFF;
        goto error;
    } else if (state&DETECTOR_STATE_UNINITIALIZED) {
        ret = AAOS_EUNINIT;
        goto error;
    } else if ((state&DETECTOR_STATE_EXPOSING) || (state&DETECTOR_STATE_READING)) {
        if (options&DETECTOR_OPTION_NOWAIT) {
            ret = AAOS_EBUSY;
            goto error;
        }
        while ((state&DETECTOR_STATE_EXPOSING) || (state&DETECTOR_STATE_READING)) {
            Pthread_cond_wait(&self->_.d_state.cond, &self->_.d_state.mtx);
        }
    }
    if (!setGainValue_C_API(self->camera_id, uc_value)) {
        ret = AAOS_EDEVMAL;
        goto error;
    }
    Pthread_rwlock_wrlock(&self->_.d_param.rwlock);
    self->_.d_param.gain = gain;
    Pthread_rwlock_unlock(&self->_.d_param.rwlock);
error:
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    return ret;
}

static int
LeadingCamera_get_gain(void *_self, double *gain)
{
    struct LeadingCamera *self = cast(LeadingCamera(), _self);
    
    unsigned int state;
    uint16_t options;
    bool (*getGainValue_C_API)(int, unsigned char *);
    unsigned char uc_value;
    int ret = AAOS_OK;
    
    getGainValue_C_API = dlsym(self->dlh, "getGainValue_C_API");
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    if ((state&DETECTOR_STATE_MALFUNCTION) && !(options&DETECTOR_OPTION_IGNORE_DEVMAL)) {
        ret = AAOS_EDEVMAL;
        goto error;
    } else if (state&DETECTOR_STATE_OFFLINE) {
        ret = AAOS_EPWROFF;
        goto error;
    } else if (state&DETECTOR_STATE_UNINITIALIZED) {
        ret = AAOS_EUNINIT;
        goto error;
    }
    if (!getGainValue_C_API(self->camera_id, &uc_value)) {
        ret = AAOS_EDEVMAL;
        goto error;
    }
    *gain = self->_.d_cap.gain_array[uc_value];
error:
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    return ret;
}

static int
LeadingCamera_set_region(void *_self, uint32_t x_offset, uint32_t y_offset, uint32_t width, uint32_t height)
{
    struct LeadingCamera *self = cast(LeadingCamera(), _self);
    
    unsigned int state;
    uint16_t options;
    bool (*setROIZoonOffset_C_API)(int, unsigned short, unsigned short);
    bool (*setROIZoonSize_C_API)(int, unsigned short, unsigned short);
    int ret = AAOS_OK;
    unsigned short us_value, us_value2, us_value3, us_value4;
    
    us_value = (unsigned short) x_offset;
    us_value2 = (unsigned short) y_offset;
    us_value3 = (unsigned short) width;
    us_value4 = (unsigned short) height;
    
    setROIZoonOffset_C_API = dlsym(self->dlh, "setROIZoonOffset_C_API");
    setROIZoonSize_C_API = dlsym(self->dlh, "setROIZoonSize_C_API");
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    if ((state&DETECTOR_STATE_MALFUNCTION) && !(options&DETECTOR_OPTION_IGNORE_DEVMAL)) {
        ret = AAOS_EDEVMAL;
        goto error;
    } else if (state&DETECTOR_STATE_OFFLINE) {
        ret = AAOS_EPWROFF;
        goto error;
    } else if (state&DETECTOR_STATE_UNINITIALIZED) {
        ret = AAOS_EUNINIT;
        goto error;
    } else if ((state&DETECTOR_STATE_EXPOSING) || (state&DETECTOR_STATE_READING)) {
        if (options&DETECTOR_OPTION_NOWAIT) {
            ret = AAOS_EBUSY;
            goto error;
        }
        while ((state&DETECTOR_STATE_EXPOSING) || (state&DETECTOR_STATE_READING)) {
            Pthread_cond_wait(&self->_.d_state.cond, &self->_.d_state.mtx);
        }
    }
    if (!setROIZoonOffset_C_API(self->camera_id, us_value, us_value2)) {
        ret = AAOS_EDEVMAL;
        goto error;
    }
    if (!setROIZoonSize_C_API(self->camera_id, us_value3, us_value4)) {
        ret = AAOS_EDEVMAL;
        goto error;
    }
    Pthread_rwlock_wrlock(&self->_.d_param.rwlock);
    self->_.d_param.x_offset = x_offset;
    self->_.d_param.y_offset = y_offset;
    self->_.d_param.image_width = width;
    self->_.d_param.image_height = height;
    Pthread_rwlock_unlock(&self->_.d_param.rwlock);
error:
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    return ret;
}

static int
LeadingCamera_get_region(void *_self, uint32_t *x_offset, uint32_t *y_offset, uint32_t *width, uint32_t *height)
{
    struct LeadingCamera *self = cast(LeadingCamera(), _self);
    
    unsigned int state;
    uint16_t options;
    bool (*getROIZoonOffset_C_API)(int, unsigned short *, unsigned short *);
    bool (*getROIZoonSize_C_API)(int, unsigned short *, unsigned short *);
    int ret = AAOS_OK;
    unsigned short us_value, us_value2, us_value3, us_value4;
    
    getROIZoonOffset_C_API = dlsym(self->dlh, "getROIZoonOffset_C_API");
    getROIZoonSize_C_API = dlsym(self->dlh, "getROIZoonSize_C_API");
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    if ((state&DETECTOR_STATE_MALFUNCTION) && !(options&DETECTOR_OPTION_IGNORE_DEVMAL)) {
        ret = AAOS_EDEVMAL;
        goto error;
    } else if (state&DETECTOR_STATE_OFFLINE) {
        ret = AAOS_EPWROFF;
        goto error;
    } else if (state&DETECTOR_STATE_UNINITIALIZED) {
        ret = AAOS_EUNINIT;
        goto error;
    }
    if (!getROIZoonOffset_C_API(self->camera_id, &us_value, &us_value2)) {
        ret = AAOS_EDEVMAL;
        goto error;
    }
    if (!getROIZoonSize_C_API(self->camera_id, &us_value3, &us_value4)) {
        ret = AAOS_EDEVMAL;
        goto error;
    }
    *x_offset = (uint32_t) us_value;
    *y_offset = (uint32_t) us_value2;
    *width = (uint32_t) us_value3;
    *height = (uint32_t) us_value4;
error:
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    return ret;
}

static int
LeadingCamera_set_binning(void *_self, uint32_t x_binning, uint32_t y_binning)
{
    return AAOS_ENOTSUP;
}

static int
LeadingCamera_get_binning(void *_self, uint32_t *x_binning, uint32_t *y_binning)
{
    return AAOS_ENOTSUP;
}

static int
LeadingCamera_set_readout_rate(void *_self, double readout_rate)
{
    return AAOS_ENOTSUP;
}

static int
LeadingCamera_get_readout_rate(void *_self, double readout_rate)
{
    return AAOS_ENOTSUP;
}

static int
LeadingCamera_enable_cooling(void *_self)
{
    struct LeadingCamera *self = cast(LeadingCamera(), _self);
    
    unsigned int state;
    uint16_t options;
    bool (*setTecCtrlTempMode_C_API)(int, unsigned char);
    int ret = AAOS_OK;
    
    setTecCtrlTempMode_C_API = dlsym(self->dlh, "setTecCtrlTempMode_C_API");
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    if ((state&DETECTOR_STATE_MALFUNCTION) && !(options&DETECTOR_OPTION_IGNORE_DEVMAL)) {
        ret = AAOS_EDEVMAL;
        goto error;
    } else if (state&DETECTOR_STATE_OFFLINE) {
        ret = AAOS_EPWROFF;
        goto error;
    } else if (state&DETECTOR_STATE_UNINITIALIZED) {
        ret = AAOS_EUNINIT;
        goto error;
    }
    if (!setTecCtrlTempMode_C_API(self->camera_id, 1)) {
        ret = AAOS_EDEVMAL;
        goto error;
    }
error:
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    return ret;
}

static int
LeadingCamera_disable_cooling(void *_self)
{
    struct LeadingCamera *self = cast(LeadingCamera(), _self);
    
    unsigned int state;
    uint16_t options;
    bool (*setTecCtrlTempMode_C_API)(int, unsigned char *);
    int ret = AAOS_OK;
    
    setTecCtrlTempMode_C_API = dlsym(self->dlh, "setTecCtrlTempMode_C_API");
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    if ((state&DETECTOR_STATE_MALFUNCTION) && !(options&DETECTOR_OPTION_IGNORE_DEVMAL)) {
        ret = AAOS_EDEVMAL;
        goto error;
    } else if (state&DETECTOR_STATE_OFFLINE) {
        ret = AAOS_EPWROFF;
        goto error;
    } else if (state&DETECTOR_STATE_UNINITIALIZED) {
        ret = AAOS_EUNINIT;
        goto error;
    }
    if (!setTecCtrlTempMode_C_API(self->camera_id, 0)) {
        ret = AAOS_EDEVMAL;
        goto error;
    }
error:
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    return ret;
}

static int
LeadingCamera_set_temperature(void *_self, double temperature)
{
    struct LeadingCamera *self = cast(LeadingCamera(), _self);
    
    unsigned int state;
    uint16_t options;
    bool (*setTecCtrlTemp_C_API)(int, unsigned char);
    int ret = AAOS_OK;
    unsigned char uc_value;
    
    setTecCtrlTemp_C_API = dlsym(self->dlh, "setTecCtrlTemp_C_API");
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    if ((state&DETECTOR_STATE_MALFUNCTION) && !(options&DETECTOR_OPTION_IGNORE_DEVMAL)) {
        ret = AAOS_EDEVMAL;
        goto error;
    } else if (state&DETECTOR_STATE_OFFLINE) {
        ret = AAOS_EPWROFF;
        goto error;
    } else if (state&DETECTOR_STATE_UNINITIALIZED) {
        ret = AAOS_EUNINIT;
        goto error;
    }
    uc_value = 255;
    /*
     * TODO: convert temperature to Leading camera's TapPosition
     */
    if (!setTecCtrlTemp_C_API(self->camera_id, uc_value)) {
        ret = AAOS_EDEVMAL;
        goto error;
    }
error:
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    return ret;
}

static int
LeadingCamera_get_temperature(void *_self, double temperature)
{
    struct LeadingCamera *self = cast(LeadingCamera(), _self);
    
    unsigned int state;
    uint16_t options;
    bool (*getTecCtrlTemp_C_API)(int, double *, unsigned char *);
    int ret = AAOS_OK;
    unsigned char uc_value;
    
    getTecCtrlTemp_C_API = dlsym(self->dlh, "getTecCtrlTemp_C_API");
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    if ((state&DETECTOR_STATE_MALFUNCTION) && !(options&DETECTOR_OPTION_IGNORE_DEVMAL)) {
        ret = AAOS_EDEVMAL;
        goto error;
    } else if (state&DETECTOR_STATE_OFFLINE) {
        ret = AAOS_EPWROFF;
        goto error;
    } else if (state&DETECTOR_STATE_UNINITIALIZED) {
        ret = AAOS_EUNINIT;
        goto error;
    }
    if (!getTecCtrlTemp_C_API(self->camera_id, &temperature, &uc_value)) {
        ret = AAOS_EDEVMAL;
        goto error;
    }
error:
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    return ret;
}

static int
LeadingCamera_set_pixel_format(void *_self, uint32_t pixel_format)
{
    struct LeadingCamera *self = cast(LeadingCamera(), _self);
    
    unsigned int state;
    uint16_t options;
    bool (*setImageFormat_C_API)(int, unsigned char);
    int ret = AAOS_EINVAL;
    size_t i, n = self->_.d_cap.n_pixel_format;
    unsigned char uc_value = 1;
    
    setImageFormat_C_API = dlsym(self->dlh, "setImageFormat_C_API");
    
    for (i = 0; i < n; i++) {
        if (pixel_format == self->_.d_cap.pixel_format_array[i]) {
            ret = AAOS_OK;
            switch (pixel_format) {
                case DETECTOR_PIXEL_FORMAT_MONO_8:
                    uc_value = 0;
                    break;
                case DETECTOR_PIXEL_FORMAT_MONO_14:
                    uc_value = 1;
                case DETECTOR_PIXEL_FORMAT_YUV422:
                    uc_value = 2;
                    break;
                default:
                    break;
            }
            break;
        }
    }
    if (ret == AAOS_EINVAL) {
        return AAOS_EINVAL;
    }
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    if ((state&DETECTOR_STATE_MALFUNCTION) && !(options&DETECTOR_OPTION_IGNORE_DEVMAL)) {
        ret = AAOS_EDEVMAL;
        goto error;
    } else if (state&DETECTOR_STATE_OFFLINE) {
        ret = AAOS_EPWROFF;
        goto error;
    } else if (state&DETECTOR_STATE_UNINITIALIZED) {
        ret = AAOS_EUNINIT;
        goto error;
    } else if ((state&DETECTOR_STATE_EXPOSING) || (state&DETECTOR_STATE_READING)) {
        if (options&DETECTOR_OPTION_NOWAIT) {
            ret = AAOS_EBUSY;
            goto error;
        }
        while ((state&DETECTOR_STATE_EXPOSING) || (state&DETECTOR_STATE_READING)) {
            Pthread_cond_wait(&self->_.d_state.cond, &self->_.d_state.mtx);
        }
    }
    if (!setImageFormat_C_API(self->camera_id, uc_value)) {
        ret = AAOS_EDEVMAL;
        goto error;
    }
    Pthread_rwlock_wrlock(&self->_.d_param.rwlock);
    self->_.d_param.pixel_format = pixel_format;
    Pthread_rwlock_unlock(&self->_.d_param.rwlock);
error:
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    return ret;
}

static int
LeadingCamera_get_pixel_format(void *_self, uint32_t *pixel_format)
{
    struct LeadingCamera *self = cast(LeadingCamera(), _self);
    
    unsigned int state;
    uint16_t options;
    bool (*getImageFormat_C_API)(int, unsigned char *);
    unsigned char uc_value;
    int ret = AAOS_OK;
    
    getImageFormat_C_API = dlsym(self->dlh, "getImageFormat_C_API");
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    if ((state&DETECTOR_STATE_MALFUNCTION) && !(options&DETECTOR_OPTION_IGNORE_DEVMAL)) {
        ret = AAOS_EDEVMAL;
        goto error;
    } else if (state&DETECTOR_STATE_OFFLINE) {
        ret = AAOS_EPWROFF;
        goto error;
    } else if (state&DETECTOR_STATE_UNINITIALIZED) {
        ret = AAOS_EUNINIT;
        goto error;
    }
    if (!getImageFormat_C_API(self->camera_id, &uc_value)) {
        ret = AAOS_EDEVMAL;
        goto error;
    }
    switch (uc_value) {
        case 0:
            *pixel_format = DETECTOR_PIXEL_FORMAT_MONO_8;
            break;
        case 1:
            *pixel_format = DETECTOR_PIXEL_FORMAT_MONO_14;
            break;
        case 2:
            *pixel_format =DETECTOR_PIXEL_FORMAT_YUV422;
            break;
        default:
            break;
    }
error:
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    return ret;
    
}

struct LeadingCameraExposureArg {
    struct LeadingCamera *detector;
    void *rpc;
    void *string; /* serialized header data string. */
    unsigned int format; /* string format */
};

static void *
LeadingCamera_process_image_thr(void *arg)
{
    struct LeadingCameraExposureArg *myarg = (struct LeadingCameraExposureArg *) arg;
    struct LeadingCamera *detector = myarg->detector;
    void *string = myarg->string;
    void *rpc = myarg->rpc;
    int format = myarg->format;
    uint16_t options = detector->_.d_state.options;
    
    struct DetectorDataFrame *data;
    size_t i, n, width, height;
    char filename[FILENAMESIZE];
    char buf[TIMESTAMPSIZE];
    fitsfile *fptr = NULL;
    int bitpix, datatype, naxis = 2, status = 0;
    long naxes[2];
    uint32_t pixel_format;
    struct timespec tp;
    struct tm tm_buf;
    long value;
    
    bool (*getGainValue_C_API)(int, unsigned char *);
    bool (*getGainMode_C_API)(int, bool *);
    
    bool (*getTriggerMode_C_API)(int, unsigned char *);
    bool (*getTriggerLevel_C_API)(int, bool *);
    bool (*getTriggerEdge_C_API)(int, bool *);
    
    bool (*getImageEnhanceMode_C_API)(int, unsigned char *);
    bool (*getImageEnhanceBrightness_C_API)(int, unsigned char *);
    bool (*getImageEnhanceContrast_C_API)(int, unsigned char *);
    bool (*getImageDetailEnhance_C_API)(int, unsigned char *);
    bool (*getImageNioseFilter_C_API)(int, unsigned char *);
    bool (*getGammaCorrectionMode_C_API)(int, unsigned char *);
    bool (*getGammaCorrectionValue_C_API)(int, unsigned char *);
    bool (*getNoUniformCorrectionState_C_API)(int, bool *);
    bool (*getBadPointCorrectionState_C_API)(int, bool *);
    bool (*getROIZoonOffset_C_API)(int, unsigned short *, unsigned short *);
    bool (*getROIZoonSize_C_API)(int, unsigned short *, unsigned short *);
    bool (*getPalNtscType_C_API)(int, bool *);
    bool (*getFpaTemp_C_API)(int, double *);
    bool (*getShellTmp_C_API)(int, unsigned int *);
    bool (*getSignalBoardTmp_C_API)(int, unsigned int *);
    bool (*getTecCtrlTempMode_C_API)(int, unsigned char *);
    bool (*getTecCtrlTemp_C_API)(int, double *, unsigned char *);
    
    double d_value;
    unsigned char uc_value;
    unsigned int ui_value;
    unsigned short us_value, us_value2;
    bool b_value;
    
    getTriggerMode_C_API = dlsym(detector->dlh, "getTriggerMode_C_API");
    getGainMode_C_API = dlsym(detector->dlh, "getGainMode_C_API");
    
    getTriggerLevel_C_API = dlsym(detector->dlh, "getTriggerLevel_C_API");
    getTriggerEdge_C_API = dlsym(detector->dlh, "getTriggerEdge_C_API");
    getGainValue_C_API = dlsym(detector->dlh, "getGainValue_C_API");
    getImageEnhanceMode_C_API = dlsym(detector->dlh, "getImageEnhanceMode_C_API");
    getImageEnhanceBrightness_C_API = dlsym(detector->dlh, "getImageEnhanceBrightness_C_API");
    getImageEnhanceContrast_C_API = dlsym(detector->dlh, "getImageEnhanceContrast_C_API");
    getImageDetailEnhance_C_API = dlsym(detector->dlh, "getImageDetailEnhance_C_API");
    getImageNioseFilter_C_API = dlsym(detector->dlh, "getImageNioseFilter_C_API");
    getGammaCorrectionMode_C_API = dlsym(detector->dlh, "getGammaCorrectionMode_C_API");
    getGammaCorrectionValue_C_API = dlsym(detector->dlh, "getGammaCorrectionValue_C_API");
    getNoUniformCorrectionState_C_API = dlsym(detector->dlh, "getNoUniformCorrectionState_C_API");
    getBadPointCorrectionState_C_API = dlsym(detector->dlh, "getBadPointCorrectionState_C_API");
    getROIZoonOffset_C_API = dlsym(detector->dlh, "getROIZoonOffset_C_API");
    getROIZoonSize_C_API = dlsym(detector->dlh, "getROIZoonSize_C_API");
    getPalNtscType_C_API = dlsym(detector->dlh, "getPalNtscType_C_API");
    getFpaTemp_C_API = dlsym(detector->dlh, "getFpaTemp_C_API");
    getShellTmp_C_API = dlsym(detector->dlh, "getShellTmp_C_API");
    getSignalBoardTmp_C_API = dlsym(detector->dlh, "getSignalBoardTmp_C_API");
    getTecCtrlTempMode_C_API = dlsym(detector->dlh, "getTecCtrlTempMode_C_API");
    getTecCtrlTemp_C_API = dlsym(detector->dlh, "getTecCtrlTemp_C_API");
    
    for (; ;) {
        data = threadsafe_queue_wait_and_pop(detector->_.d_proc.queue);
        if (data != NULL) {
            if (data->buffer != NULL) {
                free(data->buffer);
                free(data);
            } else {
                break;
            }
        }
    }
    
    n = data->n;
    width = data->width;
    height = data->height;
    pixel_format = data->pixel_format;
    naxes[0] = width;
    naxes[1] = height;
    
    tp.tv_sec = data->tp.tv_sec;
    tp.tv_nsec = data->tp.tv_nsec;
    switch (pixel_format) {
        case DETECTOR_PIXEL_FORMAT_MONO_8:
            bitpix = BYTE_IMG;
            datatype = TBYTE;
            break;
        case DETECTOR_PIXEL_FORMAT_MONO_14:
            bitpix = USHORT_IMG;
            datatype = TUSHORT;
        default:
            break;
    }
    
    if (!(options&DETECTOR_OPTION_NOTIFY_EACH_COMPLETION)) {
        detector->_.d_proc.name_convention(detector, filename, FILENAMESIZE, 1, 1, &tp);
        if (detector->_.d_proc.tpl_fptr != NULL) {
            fits_create_file(&fptr, filename, &status);
            if (status != 0) {
#ifdef DEBUG
                fprintf(stderr, "%s %s %d --- fits_create_file error: %d\n", __FILE__, __func__, __LINE__ - 2, status);
#endif
            }
            fits_copy_file(detector->_.d_proc.tpl_fptr, fptr, 0, 1, 1, &status);
            if (status != 0) {
#ifdef DEBUG
                fprintf(stderr, "%s %s %d --- fits_copy_file error: %d\n", __FILE__, __func__, __LINE__ - 2, status);
#endif
            }
        } else {
            fits_create_template(&fptr, filename, detector->_.d_proc.tpl_filename, &status);
            if (status != 0) {
#ifdef DEBUG
                fprintf(stderr, "%s %s %d --- fits_create_template error: %d\n", __FILE__, __func__, __LINE__ - 2, status);
#endif
            }
        }
        detector->_.d_proc.pre_acquisition(detector, filename, fptr, &data->tp);
        detector->_.d_proc.img_fptr = fptr;
    }
    
    for (i = 0; i < n; i++) {
        Pthread_mutex_lock(&detector->_.d_exp.mtx);
        if (detector->_.d_exp.stop_flag) {
            Pthread_mutex_unlock(&detector->_.d_exp.mtx);
            break;
        }
        Pthread_mutex_unlock(&detector->_.d_exp.mtx);
        data = threadsafe_queue_wait_and_pop(detector->_.d_proc.queue);
        if (data == NULL) {
            break;
        }
        if (data->buffer == NULL) {
            free(data);
            break;
        }
        if (options&DETECTOR_OPTION_NOTIFY_EACH_COMPLETION) {
            detector->_.d_proc.name_convention(detector, filename, FILENAMESIZE, i + 1, n, &tp);
            if (detector->_.d_proc.tpl_fptr != NULL) {
                fits_create_file(&fptr, filename, &status);
                if (status != 0) {
#ifdef DEBUG
                    fprintf(stderr, "%s %s %d --- fits_create_file error: %d\n", __FILE__, __func__, __LINE__ - 2, status);
#endif
                }
                fits_copy_file(detector->_.d_proc.tpl_fptr, fptr, 0, 1, 1, &status);
                if (status != 0) {
#ifdef DEBUG
                    fprintf(stderr, "%s %s %d --- fits_copy_file error: %d\n", __FILE__, __func__, __LINE__ - 2, status);
#endif
                }
            } else {
                fits_create_template(&fptr, filename, detector->_.d_proc.tpl_filename, &status);
                if (status != 0) {
#ifdef DEBUG
                    fprintf(stderr, "%s %s %d --- fits_create_template error: %d\n", __FILE__, __func__, __LINE__ - 2, status);
#endif
                }
            }
            detector->_.d_proc.pre_acquisition(detector, filename, fptr, &data->tp);
            detector->_.d_proc.img_fptr = fptr;
        }
        
        
        fits_create_img(fptr, bitpix, naxis, naxes, &status);
        if (status != 0) {
#ifdef DEBUG
            fprintf(stderr, "%s %s %d --- fits_create_img error: %d\n", __FILE__, __func__, __LINE__ - 2, status);
#endif
        }
        gmtime_r(&data->tp.tv_sec, &tm_buf);
        strftime(buf, TIMESTAMPSIZE, "%Y-%m-%d", &tm_buf);
        fits_update_key_str(fptr, "DATE-OBS", buf, "end date of this frame", &status);
        strftime(buf, TIMESTAMPSIZE, "%H:%M:%S", &tm_buf);
        snprintf(buf + strlen(buf), TIMESTAMPSIZE - strlen(buf), ".%03d", (int) floor(data->tp.tv_nsec / 1000000));
        fits_update_key_str(fptr, "TIME-OBS", buf, "end time of this frame", &status);
        
        if (getTriggerMode_C_API(detector->camera_id, &uc_value)) {
            switch (uc_value) {
                case 0:
                    fits_update_key_str(fptr, "TRIGMODE", "default", "trigger mode", &status);
                    break;
                case 1:
                    fits_update_key_str(fptr, "TRIGMODE", "auto", "trigger mode", &status);
                    break;
                case 2:
                    if (getTriggerLevel_C_API(detector->camera_id, &b_value)) {
                        if (b_value) {
                            fits_update_key_str(fptr, "TRIGMODE", "hardware level low", "trigger mode", &status);
                        } else {
                            fits_update_key_str(fptr, "TRIGMODE", "hardware level high", "trigger mode", &status);
                        }
                    }
                    break;
                case 3:
                    if (getTriggerEdge_C_API(detector->camera_id, &b_value)) {
                        if (b_value) {
                            fits_update_key_str(fptr, "TRIGMODE", "hardware edge fall", "trigger mode", &status);
                        } else {
                            fits_update_key_str(fptr, "TRIGMODE", "hardware edge rise", "trigger mode", &status);
                        }
                    }
                    break;
                default:
                    break;
            }
        }
        
        if (getGainMode_C_API(detector->camera_id, &b_value)) {
            fits_update_key_log(fptr, "GAINMODE", b_value, "whether auto gain enable", &status);
        }
        if (getGainValue_C_API(detector->camera_id, &uc_value)) {
            fits_update_key_fixflt(fptr, "GAIN", (float) uc_value, 2, "commanded gain of detector", &status);
        }
        if (getImageEnhanceMode_C_API(detector->camera_id, &uc_value)) {
            fits_update_key_lng(fptr, "IMGENH", (long) uc_value, "image enhance mode, 0:off -- 1:hist -- 2:linear -- 3:mixed", &status);
        }
        if (getImageEnhanceBrightness_C_API(detector->camera_id, &uc_value)) {
            fits_update_key_lng(fptr, "BRTENH", (long) uc_value, "image brightness enhance coefficiency", &status);
        }
        if (getImageEnhanceContrast_C_API(detector->camera_id, &uc_value)) {
            fits_update_key_lng(fptr, "CTRENH", (long) uc_value, "image contrast enhance coefficiency", &status);
        }
        if (getImageDetailEnhance_C_API(detector->camera_id, &uc_value)) {
            fits_update_key_lng(fptr, "DTENH", (long) uc_value, "image detail enhance coefficiency, 0 means disbaled", &status);
        }
        if (getImageNioseFilter_C_API(detector->camera_id, &uc_value)) {
            fits_update_key_lng(fptr, "NFILTER", (long) uc_value, "noise filter coefficiency, 0 means disbaled", &status);
        }
        if (getGammaCorrectionMode_C_API(detector->camera_id, &uc_value)) {
            switch (uc_value) {
                case 0:
                    fits_update_key_lng(fptr, "GMODE", 0, "gamma correction mode, 0:off -- 1:manual -- 2:auto", &status);
                    break;
                case 1:
                    fits_update_key_lng(fptr, "GMODE", 1, "gamma correction mode, 0:off -- 1:manual -- 2:auto", &status);
                    if (getGammaCorrectionValue_C_API(detector->camera_id,  &uc_value)) {
                        fits_update_key_fixdbl(fptr, "GAMMA", (double) uc_value, 2, "gamma value", &status);
                    }
                    break;
                case 2:
                    fits_update_key_lng(fptr, "GMODE", 2, "gamma correction mode, 0:off -- 1:manual -- 2:auto", &status);
                    if (getGammaCorrectionValue_C_API(detector->camera_id,  &uc_value)) {
                        fits_update_key_fixdbl(fptr, "GAMMA", (double) uc_value, 2, "gamma value", &status);
                    }
                    break;
                default:
                    break;
            }
        }
        if (getNoUniformCorrectionState_C_API(detector->camera_id, &b_value)) {
            fits_update_key_log(fptr, "NONEUNI", b_value, "whether none uniformaty correction enabled", &status);
        }
        if (getBadPointCorrectionState_C_API(detector->camera_id, &b_value)) {
            fits_update_key_log(fptr, "BADPOINT", b_value, "whether bad point correction enabled", &status);
        }
        if (getROIZoonOffset_C_API(detector->camera_id, &us_value, &us_value2)) {
            fits_update_key_lng(fptr, "X_OFFSET", (long) us_value, "X offset of ROI", &status);
            fits_update_key_lng(fptr, "Y_OFFSET", (long) us_value, "Y offset of ROI", &status);
        }
        if (getROIZoonSize_C_API(detector->camera_id, &us_value, &us_value2)) {
            fits_update_key_lng(fptr, "IMGWIDTH", (long) us_value, "width of ROI", &status);
            fits_update_key_lng(fptr, "IMGHEIGH", (long) us_value, "height of ROI", &status);
        }
        if (getPalNtscType_C_API(detector->camera_id, &b_value)) {
            fits_update_key_log(fptr, "ENCMODE", b_value, "color encoding mode, T:PAL -- F:NTSC", &status);
        }
        if (getFpaTemp_C_API(detector->camera_id, &d_value)) {
            fits_update_key_fixflt(fptr, "FPATEMP", d_value, 2, "actual FPA temperature of detector", &status);
        }
        if (getShellTmp_C_API(detector->camera_id, &ui_value)) {
            fits_update_key_fixflt(fptr, "SHELTEMP", (double) ui_value, 2, "shell temperature of detector", &status);
        }
        if (getSignalBoardTmp_C_API(detector->camera_id, &ui_value)) {
            fits_update_key_fixflt(fptr, "SIGTEMP", (double) ui_value, 2, "signal board temperature of detector", &status);
        }
        if (getTecCtrlTempMode_C_API(detector->camera_id, &uc_value)) {
            switch (uc_value) {
                case 0:
                    fits_update_key_lng(fptr, "TEMPMODE", 0, "0:off -- 1:manual -- 2:auto", &status);
                    break;
                case 1:
                    fits_update_key_lng(fptr, "TEMPMODE", 1, "0:off -- 1:manual -- 2:auto", &status);
                    if (getTecCtrlTemp_C_API(detector->camera_id, &d_value, &uc_value)) {
                        fits_update_key_fixdbl(fptr, "SETTEMP", d_value, 2, "commanded temperature of TEC", &status);
                        fits_update_key_lng(fptr, "TAPPOS", (long) uc_value, "tapPosition of ommanded temperature of TEC", &status);
                    }
                    break;
                case 2:
                    fits_update_key_lng(fptr, "TEMPMODE", 2, "0:off -- 1:manual -- 2:auto", &status);
                    break;
                default:
                    break;
            }
        }
        fits_update_key_str(fptr, "EXTNAME", "RAW", "extension name", &status);
        fits_update_key_lng(fptr, "EXTVER", i + 1, "extension version number", &status);
        fits_write_img(fptr, datatype, 1, naxes[0] * naxes[1], data->buffer, &status);
        if (status != 0) {
#ifdef DEBUG
            fprintf(stderr, "%s %s %d --- fits_write_img error: %d\n", __FILE__, __func__, __LINE__ - 2, status);
#endif
        }
        
        if (options&DETECTOR_OPTION_NOTIFY_EACH_COMPLETION) {
            detector->_.d_proc.post_acquisition(detector, filename, fptr, string, format, rpc);
            fits_close_file(fptr, &status);
            if (status != 0) {
#ifdef DEBUG
                fprintf(stderr, "fits_close_file error: %d\n", status);
#endif
            }
            detector->_.d_proc.img_fptr = NULL;
        }
        free(data->buffer);
        free(data);
    }
    
    if (!(options&DETECTOR_OPTION_NOTIFY_EACH_COMPLETION)) {
        detector->_.d_proc.post_acquisition(detector, filename, fptr, string, format, rpc);
        fits_close_file(fptr, &status);
        if (status != 0) {
#ifdef DEBUG
            fprintf(stderr, "fits_close_file error: %d\n", status);
#endif
        }
        detector->_.d_proc.img_fptr = NULL;
    }
    
    return NULL;
}

static void
LeadingCamera_frame_capture_cb(unsigned char *frame_buffer, int w, int h, int t, void *p)
{
    struct LeadingCamera *detector = (struct LeadingCamera *) p;
    
    unsigned int state;
    struct DetectorDataFrame *data;
    uint32_t i, n;
    
    Pthread_mutex_lock(&detector->_.d_state.mtx);
    state = detector->_.d_state.state;
    if (!(state&DETECTOR_STATE_EXPOSING) || !(state&DETECTOR_STATE_READING)) {
        Pthread_mutex_unlock(&detector->_.d_state.mtx);
        return;
    }
    i = detector->_.d_exp.count;
    detector->_.d_exp.count++;
    detector->_.d_exp.success_frames++;
    n = detector->_.d_exp.request_frames;
    Pthread_mutex_unlock(&detector->_.d_state.mtx);
    
    data = (struct DetectorDataFrame *) Malloc(sizeof(struct DetectorDataFrame));
    data->i = (size_t) i;
    data->n = (size_t) n;
    data->width = w;
    data->height = h;
    Clock_gettime(CLOCK_REALTIME, &data->tp);
    switch (t) {
        case 0:
            data->pixel_format = DETECTOR_PIXEL_FORMAT_MONO_8;
            data->buffer = Malloc(w*h);
            memcpy(data->buffer, frame_buffer, w*h);
            break;
        case 1:
            data->pixel_format = DETECTOR_PIXEL_FORMAT_MONO_14;
            data->buffer = Malloc(2*w*h);
            memcpy(data->buffer, frame_buffer, 2*w*h);
            break;
        case 2:
            data->pixel_format = DETECTOR_PIXEL_FORMAT_YUV422;
            data->buffer = Malloc(2*w*h);
            memcpy(data->buffer, frame_buffer, 2*w*h);
            break;
        default:
            break;
    }
    threadsafe_queue_push(detector->_.d_proc.queue, data);
    if (i == n - 1) {
        threadsafe_queue_push(detector->_.d_proc.queue, NULL);
    }
}

static int
LeadingCamera_expose(void *_self, double exposure_time, uint32_t n_frame, va_list *app)
{
    struct LeadingCamera *self = cast(LeadingCamera(), _self);
    
    unsigned int state;
    uint16_t options;
    uint32_t i;
    long length;
    int ret = AAOS_OK;
    void *rpc;
    char *string;
    int format;
    struct DetectorDataFrame *data;
    struct LeadingCameraExposureArg *arg;
    void *retval;
    
    rpc = va_arg(*app, void *);
    string = va_arg(*app, char *);
    
    bool (*startCameraCapture_C_API)(int);
    bool (*stopCameraCapture_C_API)(int);

    startCameraCapture_C_API = dlsym(self->dlh, "startCameraCapture_C_API");
    stopCameraCapture_C_API = dlsym(self->dlh, "stopCameraCapture_C_API");
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    if ((state&DETECTOR_STATE_MALFUNCTION) && !(options&DETECTOR_OPTION_IGNORE_DEVMAL)) {
    
    Pthread_mutex_unlock(&self->_.d_state.mtx);
        return AAOS_EDEVMAL;
    }
    
    if ((state&DETECTOR_STATE_EXPOSING) || (state&DETECTOR_STATE_READING)) {
        if (options&DETECTOR_OPTION_NOWAIT) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EBUSY;
        }
        while ((state&DETECTOR_STATE_EXPOSING) || (state&DETECTOR_STATE_READING)) {
            Pthread_cond_wait(&self->_.d_state.cond, &self->_.d_state.mtx);
        }
    } else if (state&DETECTOR_STATE_OFFLINE) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        return AAOS_EPWROFF;
    } else if (state&DETECTOR_STATE_UNINITIALIZED) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        return AAOS_EUNINIT;
    }
    if ((ret = LeadingCamera_set_exposure_time_nl(self, exposure_time)) != AAOS_OK) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        return ret;
    }
    state = DETECTOR_STATE_EXPOSING;
    self->_.d_state.state = (self->_.d_state.state&DETECTOR_STATE_MALFUNCTION) | state;
    
    Pthread_mutex_lock(&self->_.d_exp.mtx);
    self->_.d_exp.rpc = rpc;
    self->_.d_exp.request_frames = n_frame;
    self->_.d_exp.success_frames = 0;
    self->_.d_exp.count = 0;
    Pthread_mutex_unlock(&self->_.d_exp.mtx);
    arg = (struct LeadingCameraExposureArg *) Malloc(sizeof(struct LeadingCameraExposureArg));
    arg->detector = self;
    arg->rpc = rpc;
    arg->string = string;
    arg->format = format;
    Pthread_create(&self->_.d_state.tid, NULL, LeadingCamera_process_image_thr, (void *) arg);
    
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    data = (struct DetectorDataFrame *) Malloc(sizeof(struct DetectorDataFrame));
    data->buffer = NULL;
    data->pixel_format = self->_.d_param.pixel_format;
    data->width = self->_.d_param.image_width;
    data->height = self->_.d_param.image_height;
    data->n = n_frame;
    data->i = 1;
    data->length = length;
    Clock_gettime(CLOCK_REALTIME, &data->tp);
    threadsafe_queue_push(self->_.d_proc.queue, data);
    
    if (!startCameraCapture_C_API(self->camera_id)) {
        ret = AAOS_EDEVMAL;
        threadsafe_queue_push(self->_.d_proc.queue, NULL);
        Pthread_join(self->_.d_state.tid, NULL);
        Pthread_mutex_lock(&self->_.d_exp.mtx);
        self->_.d_exp.rpc = NULL;
        self->_.d_exp.stop_flag = true;
        Pthread_mutex_unlock(&self->_.d_exp.mtx);
        Pthread_mutex_lock(&self->_.d_state.mtx);
        self->_.d_state.state = (state&DETECTOR_STATE_MALFUNCTION) | DETECTOR_STATE_IDLE;
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        Pthread_cond_broadcast(&self->_.d_state.cond);
        threadsafe_queue_push(self->_.d_proc.queue, NULL);
        Pthread_join(self->_.d_state.tid, &retval);
        Pthread_mutex_lock(&self->_.d_exp.mtx);
        self->_.d_exp.stop_flag = false;
        Pthread_mutex_unlock(&self->_.d_exp.mtx);
        free(arg);
        return ret;
    }
    Pthread_join(self->_.d_state.tid, &retval);
    
    Pthread_mutex_lock(&self->_.d_exp.mtx);
    if (self->_.d_exp.stop_flag) {
        ret = AAOS_ECANCELED;
    } else {
        if (!stopCameraCapture_C_API(self->camera_id)) {
            ret = AAOS_EDEVMAL;
        }
        
    }
    self->_.d_exp.stop_flag = false;
    self->_.d_exp.rpc = NULL;
    Pthread_mutex_unlock(&self->_.d_exp.mtx);
    free(arg);
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    self->_.d_state.state = (state&DETECTOR_STATE_MALFUNCTION) | DETECTOR_STATE_IDLE;
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    Pthread_cond_broadcast(&self->_.d_state.cond);

    if (retval == PTHREAD_CANCELED) {
        ret = AAOS_ECANCELED;
    }
    
    return ret;
}

static const void *_leading_camera_virtual_table;

static void
leading_camera_virtual_table_destroy(void)
{
    delete((void *) _leading_camera_virtual_table);
}

static void
leading_camera_virtual_table_initialize(void)
{
    _leading_camera_virtual_table = new(__DetectorVirtualTable(),
                                        __detector_disable_cooling, "disable_cooling", LeadingCamera_disable_cooling,
                                        __detector_enable_cooling, "enable_cooling", LeadingCamera_enable_cooling,
                                        __detector_init, "init", LeadingCamera_init,
                                        __detector_info, "info", LeadingCamera_info,
                                        __detector_status, "status", LeadingCamera_status,
                                        __detector_expose, "expose", LeadingCamera_expose,
                                        //__detector_stop, "stop", ASICamera_stop,
                                        //__detector_abort, "abort", ASICamera_abort,
                                        //__detector_power_on, "power_on", ASICamera_power_on,
                                        //__detector_power_off, "power_off", ASICamera_power_off,
                                        __detector_set_binning, "set_binning", LeadingCamera_set_binning,
                                        __detector_get_binning, "get_binning", LeadingCamera_get_binning,
                                        __detector_set_exposure_time, "set_exposure_time", LeadingCamera_set_exposure_time,
                                        __detector_get_exposure_time, "get_exposure_time", LeadingCamera_get_exposure_time,
                                        __detector_set_frame_rate, "set_frame_rate", LeadingCamera_set_frame_rate,
                                        __detector_get_frame_rate, "get_frame_rate", LeadingCamera_get_frame_rate,
                                        __detector_set_gain, "set_gain", LeadingCamera_set_gain,
                                        __detector_get_gain, "get_gain", LeadingCamera_get_gain,
                                        __detector_set_region, "set_region", LeadingCamera_set_region,
                                        __detector_get_region, "get_region", LeadingCamera_get_region,
                                        __detector_set_temperature, "set_temperature", LeadingCamera_set_temperature,
                                        __detector_get_temperature, "get_temperature", LeadingCamera_get_temperature,
                                        __detector_raw, "raw", LeadingCamera_raw,
                                        //__detector_inspect, "inspect", USTCCamera_inspect,
                                        (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(leading_camera_virtual_table_destroy);
#endif
}

static const void *
leading_camera_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, leading_camera_virtual_table_initialize);
#endif
    
    return _leading_camera_virtual_table;
}

#endif

 #ifdef __USE_QHY_CAMAERA__

static int
qhy_error_mapping(int error_code)
{
    int ret;

    switch (error_code) {
        case -1:
            ret = AAOS_ERROR;
        break;
    default:
        break;
    }

    return ret;
}

static const void *qhy_camera_virtual_table(void);

static void *
QHYCamera_ctor(void *_self, va_list *app)
{
    struct QHYCamera *self = super_ctor(QHYCamera(), _self, app);
    
    const char *s;

    self->_.d_state.state = (DETECTOR_STATE_OFFLINE|DETECTOR_STATE_UNINITIALIZED);
    memset(self->camera_id, '\0', 40);
    self->camera_index = -1;
    s = va_arg(*app, const char *);
    self->so_path = (char *) Malloc(strlen(s) + 1);
    snprintf(self->so_path, strlen(s) + 1, "%s", s);
    Pthread_mutex_init(&self->mtx, NULL);
    Pthread_cond_init(&self->cond, NULL);
    self->dlh = dlopen(self->so_path, RTLD_LAZY | RTLD_LOCAL);
    self->capture_mode = DETECTOR_CAPTURE_MODE_VIDEO;
#ifdef DEBUG
    if (self->dlh == NULL) {
        fprintf(stderr, "%s %s %d: dlopen \"%s\" error.\n", __FILE__, __func__, __LINE__ - 3, self->so_path);
    }
#endif
    self->_._vtab= qhy_camera_virtual_table();
    //self->_.d_proc.name_convention = ASICamera_name_convention;
    //self->_.d_proc.pre_acquisition = ASICamera_pre_acquisition;
    //self->_.d_proc.post_acquisition = __Detector_default_post_acquisition;
    self->_.d_proc.queue = new(ThreadsafeQueue(), DetectorDataFrame_cleanup);
    
    return (void *) self;
}

static void *
QHYCamera_dtor(void *_self)
{
    struct QHYCamera *self = cast(QHYCamera(), _self);
    
    Pthread_mutex_destroy(&self->mtx);
    Pthread_cond_destroy(&self->cond);
    free(self->so_path);
    dlclose(self->dlh);

    return super_dtor(QHYCamera(), _self);
}

static void *
QHYCameraClass_ctor(void *_self, va_list *app)
{
    struct QHYCameraClass *self = super_ctor(QHYCameraClass(), _self, app);
    
    self->_.raw.method = (Method) 0;
    self->_.init.method = (Method) 0;
    
    self->_.disable_cooling.method = (Method) 0;
    self->_.enable_cooling.method = (Method) 0;
    self->_.get_binning.method = (Method) 0;
    self->_.set_binning.method = (Method) 0;
    self->_.set_capture_mode.method = (Method) 0;
    self->_.get_capture_mode.method = (Method) 0;
    self->_.set_exposure_time.method = (Method) 0;
    self->_.get_exposure_time.method = (Method) 0;
    self->_.set_frame_rate.method = (Method) 0;
    self->_.get_frame_rate.method = (Method) 0;
    self->_.set_gain.method = (Method) 0;
    self->_.get_gain.method = (Method) 0;
    self->_.set_overscan.method = (Method) 0;
    self->_.get_overscan.method = (Method) 0;
    self->_.set_pixel_format.method = (Method) 0;
    self->_.get_pixel_format.method = (Method) 0;
    self->_.set_region.method = (Method) 0;
    self->_.get_region.method = (Method) 0;
    self->_.set_temperature.method = (Method) 0;
    self->_.get_temperature.method = (Method) 0;
    self->_.set_trigger_mode.method = (Method) 0;
    self->_.get_trigger_mode.method = (Method) 0;
    self->_.inspect.method = (Method) 0;
    //self->_.wait.method = (Method) 0;
    self->_.wait_for_completion.method = (Method) 0;
    self->_.raw.method = (Method) 0;
    self->_.expose.method = (Method) 0;
    self->_.status.method = (Method) 0;
    self->_.info.method = (Method) 0;
    self->_.stop.method = (Method) 0;
    self->_.abort.method = (Method) 0;
    self->_.power_on.method = (Method) 0;
    self->_.power_off.method = (Method) 0;
    
    return self;
}

static const void *_QHYCameraClass;

static void
QHYCameraClass_destroy(void)
{
    free((void *) _QHYCameraClass);
}

static void
QHYCameraClass_initialize(void)
{
    _QHYCameraClass = new(__DetectorClass(), "QHYCameraClass", __DetectorClass(), sizeof(struct QHYCameraClass),
                           ctor, "", QHYCameraClass_ctor,
                           (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(QHYCameraClass_destroy);
#endif
}

const void *
QHYCameraClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, QHYCameraClass_initialize);
#endif
    
    return _QHYCameraClass;
}

static const void *_QHYCamera;

static void
QHYCamera_destroy(void)
{
    free((void *)_QHYCamera);
}

static void
QHYCamera_initialize(void)
{
    _QHYCamera = new(QHYCameraClass(), "QHYCamera", __Detector(), sizeof(struct QHYCamera),
                     ctor, "ctor", QHYCamera_ctor,
                     dtor, "dtor", QHYCamera_dtor,
                     (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(QHYCamera_destroy);
#endif
}

const void *
QHYCamera(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, QHYCamera_initialize);
#endif

    return _QHYCamera;
}

static int
QHYCamera_raw(void *_self, const void *write_buffer, size_t write_buffer_size, size_t *write_size, void *read_buffer, size_t read_buffer_size, size_t *read_size)
{
    struct QHYCamera *self = cast(QHYCamera(), _self);
    
    FILE *fp;
    char func_name[COMMANDSIZE];
    int ret = AAOS_OK;
    uint32_t qhy_ret;
   
    
    if ((fp = fmemopen((void *) write_buffer, write_buffer_size, "r")) == NULL) {
        return AAOS_ERROR;
    }
    
    if ((ret = fscanf(fp, "%s", &func_name)) != 1) {
        flose(fp);
        return AAOS_ERROR;
    }
    
    if (strcmp(func_name, "C_InitQHYCCDResource") == 0) {
        uint32_t (*C_InitQHYCCDResource)(void);
        C_InitQHYCCDResource = dlsym(self->dlh, "C_InitQHYCCDResource");
        qhy_ret = C_InitQHYCCDResource();
        if (read_size != NULL) {
            *read_size = 0;
        }
    } esle if (strcmp(func_name, "C_ReleaseQHYCCDResource") == 0) {
        uint32_t (*C_ReleaseQHYCCDResource)(void);
        C_ReleaseQHYCCDResource = dlsym(self->dlh, "C_ReleaseQHYCCDResource");
        qhy_ret = C_ReleaseQHYCCDResource();
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "C_ScanQHYCCD") == 0) {
        uint32_t  (*C_ScanQHYCCD)(void);
        uint32_t n_camera;
        cJSON *root_json;
        char *string;

        C_ScanQHYCCD = dlsym(self->dlh, "C_ScanQHYCCD");
        root_json = cJSON_CreateObject();
        n_camera  = C_ScanQHYCCD();
        cJSON_AddNumberToObject(root_json, "n_camera", n_camera);
        string = cJSON_Print(root_json);
        cJSON_Delete(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        free(string);
        if (read_size != NULL) {
            *read_size = strlen((char *) read_buffer);
        }
    } else if (strcmp(func_name, "C_GetQHYCCDId") == 0) {
        uint32_t (*C_GetQHYCCDId)(uint32_t, char *);
        uint32_t index;
        char qhy_id[40];
        cJSON *root_json;
        char *string;

        C_GetQHYCCDId = dlsym(self->dlh,"C_OpenQHYCCD");
        memset(qhy_id, '\0', 40);
        if (fscanf(fp, "%u", &index) != 1) {
            fclose(fp);
            return AAOS_EBADCMD;
        }
        if ((qhy_ret = C_GetQHYCCDId(index, qhy_id)) == -1) {
            fclose(fp);
            return AAOS_ENOTFOUND;
        }
        root_json = cJSON_CreateObject();
        cJSON_AddStringToObject(root_json, "QHYId", qhy_id);
        string = cJSON_Print(root_json);
        cJSON_Delete(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        free(string);
        if (read_size != NULL) {
            *read_size = strlen((char *) read_buffer);
        }
    } else if (strcmp(func_name, "C_OpenQHYCCD") == 0) {
        void * (*C_OpenQHYCCD)(char *);
        char qhy_id[40];
        
        C_OpenQHYCCD = dlsym(self->dlh, "C_OpenQHYCCD");
        memset(qhy_id, '\0', 40);
        if (fscanf(fp, "%s", qhy_id) != 1) {
            fclose(fp);
            return AAOS_EBADCMD;
        }
        if ((self->camera_handle = C_OpenQHYCCD(qhy_id)) == NULL) {
            fclose(fp);
            return AAOS_ERROR;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "C_CloseQHYCCD") == 0) {
        uint32_t (*C_CloseQHYCCD)(char *);
        C_CloseQHYCCD = dlsym(self->dlh, "C_CloseQHYCCD");
        if ((qhy_ret = C_CloseQHYCCD(self->camera_handle)) == -1) {
            fclose(fp);
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "C_GetQHYCCDReadModeNumbe") == 0) {
        uint32_t (*C_GetQHYCCDReadModeNumbe)(void *, uint32_t *);
        uint32_t n_mode;
        cJSON *root_json;
        char *string;

        C_GetQHYCCDReadModeNumbe = dlsym(self->dlh, "C_GetQHYCCDReadModeNumbe");
        if ((qhy_ret = C_GetQHYCCDReadModeNumbe(self->camera_handle, &n_mode)) == -1) {
            fclose(fp);
            return AAOS_ERROR;
        }
        root_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(root_json, "n_read_mode", n_mode);
        string = cJSON_Print(root_json);
        cJSON_Delete(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        free(string);
        if (read_size != NULL) {
            *read_size = strlen((char *) read_buffer);
        }
    } else if (strcmp(func_name, "C_GetQHYCCDReadModeName") == 0) {
        uint32_t (*C_GetQHYCCDReadModeName)(void *, uint32_t, char *);
        uint32_t index;
        char name[40];
        cJSON *root_json;
        char *string;

        C_GetQHYCCDReadModeName = dlsym(self->dlh, "C_GetQHYCCDReadModeName");
        memset(name, '\0', 40);
        if (fscanf(fp, "%u", &index) != 1) {
            fclose(fp);
            return AAOS_EBADCMD;
        }
        if ((qhy_ret = C_GetQHYCCDReadModeName(self->camera_handle, index, name)) == -1) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        root_json = cJSON_CreateObject();
        cJSON_AddStringToObject(root_json, "ModeName", name);
        string = cJSON_Print(root_json);
        cJSON_Delete(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        free(string);
        if (read_size != NULL) {
            *read_size = strlen((char *) read_buffer);
        }
    } else if (strcmp(func_name, "C_SetQHYCCDReadModeName") == 0) {
        uint32_t (*C_SetQHYCCDReadModeName)(void *, uint32_t);
        uint32_t index;
        
        C_SetQHYCCDReadModeName = dlsym(self->dlh, "C_SetQHYCCDReadModeName");
        if (fscanf(fp, "%u", &index) != 1) {
            fclose(fp);
            return AAOS_EBADCMD;
        }
        if ((qhy_ret = C_GetQHYCCDReadModeName(self->camera_handle, index)) == -1) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "C_SetQHYCCDStreamMode") == 0) {
        uint32_t (*C_SetQHYCCDStreamMode)(void *, uint32_t);
        uint32_t mode;

        C_SetQHYCCDStreamMode = dlsym(self->dlh, "C_SetQHYCCDStreamMode");
        if (fscanf(fp, "%u", &mode) != 1) {
            fclose(fp);
            return AAOS_EBADCMD;
        }
        if ((qhy_ret = C_SetQHYCCDStreamMode(self->camera_handle, mode)) == -1) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    }  else if (strcmp(func_name, "C_InitQHYCCD") == 0) {
        uint32_t (*C_InitQHYCCD)(void *);

        C_InitQHYCCD = dlsym(self->dlh, "C_InitQHYCCD");
        if ((qhy_ret = C_InitQHYCCD(self->camera_handle)) == -1) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "C_SetQHYCCDDebayerOnOff") == 0) {
        uint32_t (*C_SetQHYCCDDebayerOnOff)(void *, bool);
        bool onoff;

        C_SetQHYCCDDebayerOnOff = dlsym(self->dlh, "C_SetQHYCCDDebayerOnOff");
        if (fscanf(fp, "%d", &onoff) != 1) {
            fclose(fp);
            return AAOS_EBADCMD;
        }
        if ((qhy_ret = C_SetQHYCCDDebayerOnOff(self->camera_handle, onoff)) == -1) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, " C_SetQHYCCDParam_Bits") == 0) {
        uint32_t (* C_SetQHYCCDParam_Bits)(void *, double);
        double bits;

        C_SetQHYCCDParam_Bits = dlsym(self->dlh, "C_SetQHYCCDParam_Bits");
        if (fscanf(fp, "%lf", &bits) != 1) {
            fclose(fp);
            return AAOS_EBADCMD;
        }
        if ((qhy_ret = C_SetQHYCCDParam_Bits(self->camera_handle, bits)) == -1) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, " C_SetQHYCCDBinMode") == 0) {
        uint32_t (* C_SetQHYCCDBinMode)(void *, uint32_t, uint32_t);
        uint32_t x_binning, y_binning;

        C_SetQHYCCDBinMode = dlsym(self->dlh, "C_SetQHYCCDBinMode");
        if (fscanf(fp, "%u %u", &x_binning, &y_binning) != 2) {
            fclose(fp);
            return AAOS_EBADCMD;
        }
        if ((qhy_ret = C_SetQHYCCDBinMode(self->camera_handle, x_binning, y_binning)) == -1) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, " C_SetQHYCCDResolution") == 0) {
        uint32_t (* C_SetQHYCCDResolution)(void *, uint32_t, uint32_t, uint32_t, uint32_t);
        uint32_t x_offset, y_offset, width, height;

        C_SetQHYCCDResolution = dlsym(self->dlh, "C_SetQHYCCDResolution");
        if (fscanf(fp, "%u %u %u %u", &x_offset, &y_offset, &width, &height) != 4) {
            fclose(fp);
            return AAOS_EBADCMD
        }
        if ((qhy_ret = C_SetQHYCCDResolution(self->camera_handle, x_offset, y_offset, width, height)) == -1) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "C_GetQHYCCDChipInfo") == 0) {
        uint32_t (*C_GetQHYCCDChipInfo)(void *, double*, double*, uint32_t *, uint32_t *, double *, double *, uint32_t *);
        double chip_width, chip_height, pixel_width, pixel_height;
        uint32_t width, height, bits_per_pixel;
        cJSON *root_json;
        char *string;

        C_GetQHYCCDChipInfo = dlsym(self->dlh, "C_GetQHYCCDChipInfo");
        if ((qhy_ret = C_GetQHYCCDChipInfo(self->camera_handle, &chip_width,&chip_height, &width, &height, &pixel_width, &pixel_height, &bits_per_pixel)) == -1) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        root_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(root_json, "ChipWidth", chip_width);
        cJSON_AddNumberToObject(root_json, "ChipHeight", chip_height);
        cJSON_AddNumberToObject(root_json, "Width", width);
        cJSON_AddNumberToObject(root_json, "Height", height);
        cJSON_AddNumberToObject(root_json, "PixelWidth", pixel_width);
        cJSON_AddNumberToObject(root_json, "PixelHeight", pixel_height);
        cJSON_AddNumberToObject(root_json, "BitsPerPixel", bits_per_pixel);
        string = cJSON_Print(root_json);
        cJSON_Delete(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        free(string);
        if (read_size != NULL) {
            *read_size = strlen((char *) read_buffer);
        }
    } else if (strcmp(func_name, "C_GetQHYCCDEffectiveArea") == 0) {
        uint32_t (*C_GetQHYCCDEffectiveArea)(void *,  uint32_t *, uint32_t *, uint32_t *, uint32_t *);
        uint32_t  x_offset, y_offset, image_width, image_height;
        cJSON *root_json;
        char *string;

        C_GetQHYCCDEffectiveArea = dlsym(self->dlh, "C_GetQHYCCDEffectiveArea");
        if ((qhy_ret =  C_GetQHYCCDEffectiveArea(self->camera_handle, &x_offset, &y_offset, &image_width, &image_height)) == -1) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        root_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(root_json, "X_Offset", x_offset);
        cJSON_AddNumberToObject(root_json, "Y_Offset", y_offset);
        cJSON_AddNumberToObject(root_json, "ImageWidth", image_width);
        cJSON_AddNumberToObject(root_json, "ImageHeight", image_height);
        string = cJSON_Print(root_json);
        cJSON_Delete(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        free(string);
        if (read_size != NULL) {
            *read_size = strlen((char *) read_buffer);
        }
    } else if (strcmp(func_name, "C_GetQHYCCDOverScanArea") == 0) {
        uint32_t (*C_GetQHYCCDOverScanArea)(void *,  uint32_t *, uint32_t *, uint32_t *, uint32_t *);
        uint32_t  oversacan_x_offset, overscan_y_offset, overscan_width, overscan_height;
        cJSON *root_json;
        char *string;

        C_GetQHYCCDOverScanArea = dlsym(self->dlh, "C_GetQHYCCDOverScanArea");
        if ((qhy_ret =  C_GetQHYCCDOverScanArea(self->camera_handle, &overscan_x_offset, &overscan_y_offset, &overscan_width, &overscan_height)) == -1) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        root_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(root_json, "Overscan_X_Offset", overscan_x_offset);
        cJSON_AddNumberToObject(root_json, "Overscan_Y_Offset", overscan_y_offset);
        cJSON_AddNumberToObject(root_json, "OverscanWidth", overscan_width);
        cJSON_AddNumberToObject(root_json, "OverscanHeight", overscan_height);
        string = cJSON_Print(root_json);
        cJSON_Delete(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        free(string);
        if (read_size != NULL) {
            *read_size = strlen((char *) read_buffer);
        }
    } else if (strcmp(func_name, "C_GetQHYCCDMemLength") == 0) {
        uint32_t (*C_GetQHYCCDMemLength)(void *);
        cJSON *root_json;
        char *string;

        C_GetQHYCCDMemLength = dlsym(self->dlh, "C_GetQHYCCDMemLength");
        if ((qhy_ret =  C_GetQHYCCDMemLength(self->camera_handle)) == -1) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        root_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(root_json, "MemoryLength", qhy_ret);
        string = cJSON_Print(root_json);
        cJSON_Delete(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        free(string);
        if (read_size != NULL) {
            *read_size = strlen((char *) read_buffer);
        }
    } else if (strcmp(func_name, "C_SetQHYCCDParam_Exposure") == 0) {
        uint32_t (*C_SetQHYCCDParam_Exposure)(void *, double);
        double exposure_time;

        C_SetQHYCCDParam_Exposure = dlsym(self->dlh, "C_SetQHYCCDParam_Exposure");
        if (fscanf(fp, "%lf", &exposure_time) != 1) {
            fclose(fp);
            return AAOS_EBADCMD;
        }
        if ((qhy_ret = C_SetQHYCCDParam_Exposure(self->camera_handle, exposure_time)) == -1) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "C_SetQHYCCDParam_Gain") == 0) {
        uint32_t (*C_SetQHYCCDParam_Gain)(void *, double);
        double gain;

        C_SetQHYCCDParam_Gain = dlsym(self->dlh, "C_SetQHYCCDParam_Gain");
        if (fscanf(fp, "%lf", &gain) != 1) {
            fclose(fp);
            return AAOS_EBADCMD;
        }
        if ((qhy_ret = C_SetQHYCCDParam_Gain(self->camera_handle, gain)) == -1) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "C_SetQHYCCDParam_Offset") == 0) {
        uint32_t (*C_SetQHYCCDParam_Offset)(void *, double);
        double offset;

        C_SetQHYCCDParam_Offset = dlsym(self->dlh, "C_SetQHYCCDParam_Offset");
        if (fscanf(fp, "%lf", &offset) != 1) {
            fclose(fp);
            return AAOS_EBADCMD;
        }
        if ((qhy_ret = C_SetQHYCCDParam_Offset(self->camera_handle, offset)) == -1) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "C_SetQHYCCDParam_Traffic") == 0) {
        uint32_t (*C_SetQHYCCDParam_Traffic)(void *, double);
        double traffic;

        C_SetQHYCCDParam_Traffic = dlsym(self->dlh, "C_SetQHYCCDParam_Traffic");
        if (fscanf(fp, "%lf", &traffic) != 1) {
            fclose(fp);
            return AAOS_EBADCMD;
        }
        if ((qhy_ret = C_SetQHYCCDParam_Traffic(self->camera_handle, traffic)) == -1) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "C_SetQHYCCDParam_WBR") == 0) {
        uint32_t (*C_SetQHYCCDParam_WBR)(void *, double);
        double wbr;

        C_SetQHYCCDParam_WBR = dlsym(self->dlh, "C_SetQHYCCDParam_WBR");
        if (fscanf(fp, "%lf", &wbr) != 1) {
            fclose(fp);
            return AAOS_EBADCMD;
        }
        if ((qhy_ret = C_SetQHYCCDParam_WBR(self->camera_handle, wbr)) == -1) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "C_SetQHYCCDParam_WBG") == 0) {
        uint32_t (*C_SetQHYCCDParam_WBG)(void *, double);
        double wbg;

        C_SetQHYCCDParam_WBG = dlsym(self->dlh, "C_SetQHYCCDParam_WBG");
        if (fscanf(fp, "%lf", &wbg) != 1) {
            fclose(fp);
            return AAOS_EBADCMD;
        }
        if ((qhy_ret = C_SetQHYCCDParam_WBG(self->camera_handle, wbg)) == -1) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "C_SetQHYCCDParam_WBB") == 0) {
        uint32_t (*C_SetQHYCCDParam_WBB)(void *, double);
        double wbb;

        C_SetQHYCCDParam_WBB = dlsym(self->dlh, "C_SetQHYCCDParam_WBB");
        if (fscanf(fp, "%lf", &wbb) != 1) {
            fclose(fp);
            return AAOS_EBADCMD;
        }
        if ((qhy_ret = C_SetQHYCCDParam_WBB(self->camera_handle, wbb)) == -1) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "C_SetQHYCCDParam_Brightness") == 0) {
        uint32_t (*C_SetQHYCCDParam_Brightness)(void *, double);
        double brightness;

        C_SetQHYCCDParam_Brightness = dlsym(self->dlh, "C_SetQHYCCDParam_Brightness");
        if (fscanf(fp, "%lf", &brightness) != 1) {
            fclose(fp);
            return AAOS_EBADCMD;
        }
        if ((qhy_ret = C_SetQHYCCDParam_Brightness(self->camera_handle, brightness)) == -1) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "C_SetQHYCCDParam_Contrast") == 0) {
        uint32_t (*C_SetQHYCCDParam_Contrast)(void *, double);
        double contrast;

        C_SetQHYCCDParam_Contrast = dlsym(self->dlh, "C_SetQHYCCDParam_Contrast");
        if (fscanf(fp, "%lf", &contrast) != 1) {
            fclose(fp);
            return AAOS_EBADCMD;
        }
        if ((qhy_ret = C_SetQHYCCDParam_Contrast(self->camera_handle, contrast)) == -1) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "C_SetQHYCCDParam_Gamma") == 0) {
        uint32_t (*C_SetQHYCCDParam_Gamma)(void *, double);
        double gamma;

        C_SetQHYCCDParam_Gamma = dlsym(self->dlh, "C_SetQHYCCDParam_Gamma");
        if (fscanf(fp, "%lf", &gamma) != 1) {
            fclose(fp);
            return AAOS_EBADCMD;
        }
        if ((qhy_ret = C_SetQHYCCDParam_Gamma(self->camera_handle, gamma)) == -1) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "C_ExpQHYCCDSingleFrame") == 0) {
        uint32_t (*C_ExpQHYCCDSingleFrame)(void *);
       
        C_ExpQHYCCDSingleFrame = dlsym(self->dlh, "C_ExpQHYCCDSingleFrame");
        if ((qhy_ret = C_ExpQHYCCDSingleFrame(self->camera_handle)) == -1) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "C_CancelQHYCCDExposingAndReadout") == 0) {
        uint32_t (*C_CancelQHYCCDExposingAndReadout)(void *);
       
        C_CancelQHYCCDExposingAndReadout = dlsym(self->dlh, "C_CancelQHYCCDExposingAndReadout");
        if ((qhy_ret = C_CancelQHYCCDExposingAndReadout(self->camera_handle)) == -1) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "C_BeginQHYCCDLive") == 0) {
        uint32_t (*C_BeginQHYCCDLive)(void *);
       
        C_BeginQHYCCDLive = dlsym(self->dlh, "C_BeginQHYCCDLive");
        if ((qhy_ret = C_BeginQHYCCDLive(self->camera_handle)) == -1) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "C_StopQHYCCDLive") == 0) {
        uint32_t (*C_StopQHYCCDLive)(void *);
       
        C_StopQHYCCDLive = dlsym(self->dlh, "C_StopQHYCCDLive");
        if ((qhy_ret = C_StopQHYCCDLive(self->camera_handle)) == -1) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "C_GetQHYCCDTrigerInterfaceNumber") == 0) {
        uint32_t (*C_GetQHYCCDTrigerInterfaceNumber)(void *, uint32_t);
        uint32_t trigger_interface_number;
        cJSON *root_json;
        char *string;

        C_GetQHYCCDTrigerInterfaceNumber = dlsym(self->dlh, "C_GetQHYCCDTrigerInterfaceNumber");
        if ((qhy_ret =  C_GetQHYCCDMemLength(self->camera_handle, &trigger_interface_number)) == -1) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        root_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(root_json, "TriggerInterfaceNumber", trigger_interface_number);
        string = cJSON_Print(root_json);
        cJSON_Delete(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        free(string);
        if (read_size != NULL) {
            *read_size = strlen((char *) read_buffer);
        }
    } else if (strcmp(func_name, " C_GetQHYCCDTrigerInterfaceName") == 0) {
        uint32_t (*C_GetQHYCCDTrigerInterfaceName)(void *, uint32_t, char *);
        uint32_t index;
        char name[40];
        cJSON *root_json;
        char *string;

        C_GetQHYCCDTrigerInterfaceName = dlsym(self->dlh, " C_GetQHYCCDTrigerInterfaceName");
        memset(name, '\0', 40);
        if (fscanf(fp, "%u", &index) != 1) {
            fclose(fp);
            return AAOS_EBADCMD;
        }
        if ((qhy_ret =  C_GetQHYCCDTrigerInterfaceName(self->camera_handle, index, name)) == -1) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        root_json = cJSON_CreateObject();
        cJSON_AddStringToObject(root_json, "TriggerInterfaceName", name);
        string = cJSON_Print(root_json);
        cJSON_Delete(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        free(string);
        if (read_size != NULL) {
            *read_size = strlen((char *) read_buffer);
        }
    } else if (strcmp(func_name, "C_SetQHYCCDTrigerInterface") == 0) {
        uint32_t (*C_SetQHYCCDTrigerInterface)(void *, uint32_t);
        uint32_t index;
       
        C_SetQHYCCDTrigerInterface = dlsym(self->dlh, "C_SetQHYCCDTrigerInterface");
        if (fscanf(fp, "%u", &index) != 1) {
            fclose(fp);
            return AAOS_EBADCMD;
        }
        if ((qhy_ret = C_SetQHYCCDTrigerInterface(self->camera_handle, index)) == -1) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "C_SetQHYCCDTrigerFunction") == 0) {
        uint32_t (* C_SetQHYCCDTrigerFunction)(void *, bool);
        bool onoff;

        C_SetQHYCCDTrigerFunction = dlsym(self->dlh, "C_SetQHYCCDTrigerFunction");
        if (fscanf(fp, "%d", &onoff) != 1) {
            fclose(fp);
            return AAOS_EBADCMD;
        }
        if ((qhy_ret =  C_SetQHYCCDTrigerFunction(self->camera_handle, onoff)) == -1) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "C_EnableQHYCCDTrigerOut") == 0) {
        uint32_t (*C_EnableQHYCCDTrigerOut)(void *);
        
         C_EnableQHYCCDTrigerOut = dlsym(self->dlh, "C_EnableQHYCCDTrigerOut");
       
        if ((qhy_ret = C_EnableQHYCCDTrigerOut(self->camera_handle, index)) == -1) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "C_EnableQHYCCDBurstMode") == 0) {
        uint32_t (*C_EnableQHYCCDBurstMode)(void *, bool);
        bool onoff;

        C_EnableQHYCCDBurstMode = dlsym(self->dlh, "C_EnableQHYCCDBurstMode");
        if (fscanf(fp, "%d", &onoff) != 1) {
            fclose(fp);
            return AAOS_EBADCMD;
        }
        if ((qhy_ret = C_EnableQHYCCDBurstMode(self->camera_handle, onoff)) == -1) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "C_SetQHYCCDBurstModeStartEnd") == 0) {
        uint32_t (*C_SetQHYCCDBurstModeStartEnd)(void *, uint32_t, uint32_t);
        uint32_t start, end;

        C_SetQHYCCDBurstModeStartEnd = dlsym(self->dlh, "C_SetQHYCCDBurstModeStartEnd");
        if (fscanf(fp, "%u %u", &start, &end) != 2 || start + 2 >= end) {
            fclose(fp);
            return AAOS_EBADCMD;
        }
        if ((qhy_ret = C_SetQHYCCDBurstModeStartEnd(self->camera_handle, start, end)) == -1) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "C_SetQHYCCDBurstModePatchNumber") == 0) {
        uint32_t (*C_SetQHYCCDBurstModePatchNumber)(void *, uint32_t);
        uint32_t patch_number;

        C_SetQHYCCDBurstModePatchNumber = dlsym(self->dlh, "C_SetQHYCCDBurstModePatchNumber");
        if (fscanf(fp, "%u", &patch_number) != 1) {
            fclose(fp);
            return AAOS_EBADCMD;
        }
        if ((qhy_ret = C_SetQHYCCDBurstModePatchNumber(self->camera_handle, patch_number)) == -1) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "C_SetQHYCCDBurstIDLE") == 0) {
        uint32_t (*C_SetQHYCCDBurstIDLE)(void *);

        C_SetQHYCCDBurstIDLE = dlsym(self->dlh, "C_SetQHYCCDBurstIDLE");
        if ((qhy_ret = C_SetQHYCCDBurstIDLE(self->camera_handle)) == -1) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "C_ReleaseQHYCCDBurstIDLE") == 0) {
        uint32_t (*C_ReleaseQHYCCDBurstIDLE)(void *);

        C_ReleaseQHYCCDBurstIDLE = dlsym(self->dlh, "C_ReleaseQHYCCDBurstIDLE");
        if ((qhy_ret = C_ReleaseQHYCCDBurstIDLE(self->camera_handle)) == -1) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else if (strcmp(func_name, "C_GetQHYCCDParam_CurTempture") == 0) {
        double (*C_GetQHYCCDParam_CurTempture)(void *);
        cJSON *root_json;
        char *string;
        double temperature;

        C_GetQHYCCDParam_CurTempture = dlsym(self->dlh, "C_GetQHYCCDParam_CurTempture");
        temperature = C_GetQHYCCDParam_CurTempture(self->camera_handle);
        root_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(root_json, "CameraTemperature", temperature);
        string = cJSON_Print(root_json);
        cJSON_Delete(root_json);
        snprintf(read_buffer, read_buffer_size, "%s", string);
        free(string);
        if (read_size != NULL) {
            *read_size = strlen((char *) read_buffer);
        }
    } else if (strcmp(func_name, "C_SetQHYCCDParam_CoolerPWM") == 0) {
        uint32_t (*C_SetQHYCCDParam_CoolerPWM)(void *, double);
        double pwm;

        C_SetQHYCCDParam_CoolerPWM = dlsym(self->dlh, "C_SetQHYCCDParam_CoolerPWM");
        if (fscanf(fp, "%lf", &pwm) != 1) {
            fclose(fp);
            return AAOS_EBADCMD;
        }
        if ((qhy_ret = C_SetQHYCCDParam_CoolerPWM(self->camera_handle, pwm)) == -1) {
            fclose(fp);
            return AAOS_EDEVMAL;
        }
        if (read_size != NULL) {
            *read_size = 0;
        }
    } else {
        ret = AAOS_EBADCMD;
    }

    fclose(fp);
    return ret;
}

static int
QHYCamera_int(void *_self)
{
    struct QHYCamera *self = cast(QHYCamera(), _self);

    uint32_t state;
    uint16_t options;
    int ret = AAOS_OK;
    uint32_t (*C_ScanQHYCCD)(void);
    uint32_t (*C_GetQHYCCDId)(int, char *);
    void * (*C_OpenQHYCCD)(char *);
    uint32_t (*C_InitQHYCCD)(void *);
    uint32_t (*C_CloseQHYCCD)(void *);
    uint32_t (*C_GetQHYCCDInfo)(void *, double *, double *, uint32_t *, uint32_t *, double *, double *, uint32_t);
    uint32_t (*C_SetQHYCCDBinMode)(void *, uint32_t, uint32_t);
    uint32_t (*C_SetQHYCCDResolution)(void *, uint32_t, uint32_t, uint32_t, uint32_t);
    uint32_t (*C_SetQHYCCDParam_Gain)(void *, double);
    uint32_t (*C_SetQHYCCDParam_TargetTemperature)(void *, double);
    uint32_t (*C_SetQHYCCDParam_Brightness)(void *, double);
    uint32_t (*C_SetQHYCCDParam_Contrast)(void *, double);
    uint32_t (*C_SetQHYCCDParam_Gamma)(void *, double);
    uint32_t qhy_ret;
    uint32_t i, width, height, bits_per_pixel;
    double chip_width, chip_height, pixel_width, pixel_height;


    C_ScanQHYCCD = dlsym(self->dlh, "C_ScanQHYCCD");
    C_GetQHYCCDId = dlsym(self->dlh, "C_GetQHYCCDId");
    C_OpenQHYCCD = dlsym(self->dlh, "C_OpenQHYCCD"); 
    C_InitQHYCCD = dlsym(self->dlh, "C_InitQHYCCD"); 
    C_CloseQHYCCD = dlsym(self->dlh, "C_CloseQHYCCD");
    C_GetQHYCCDInfo = dlsym(self->dlh, "C_GetQHYCCDInfo");
    C_SetQHYCCDBinMode = dlsym(self->dlh, "C_SetQHYCCDBinMode");
    C_SetQHYCCDResolution = dlsym(self->dlh, "C_SetQHYCCDResolution");
    C_SetQHYCCDParam_Gain = dlysm(self->dlh, "C_SetQHYCCDParam_Gain");
    C_SetQHYCCDParam_TargetTemperature = dlsym(self->dlh, "C_SetQHYCCDParam_TargetTemperature");
    C_SetQHYCCDParam_Brightness = dlsym(self->dlh, "C_SetQHYCCDParam_Brightness");
    C_SetQHYCCDParam_Contrast = dlsym(self->dlh, "C_SetQHYCCDParam_Contrast");
    C_SetQHYCCDParam_Gamma = dlsym(self->dlh, "C_SetQHYCCDParam_Gamma");


    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;, 
    options = self->_.d_state.options;
    if ((state&DETECTOR_STATE_MALFUNCTION) && !(options&DETECTOR_OPTION_IGNORE_DEVMAL)) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        return AAOS_EDEVMAL;
    }
    if (state&DETECTOR_STATE_OFFLINE) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        return AAOS_EPWROFF;
    } else if (state&DETECTOR_STATE_UNINITIALIZED) {
        if ((qhy_ret = C_ScanQHYCCD()) == -1) {
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EDEVMAL;
        }
        self->n_camera = qhy_ret;

        if (self->camera_index != 0) {
            if (self->camera_index > qhy_ret) {
                Pthread_mutex_unlock(&self->_.d_state.mtx);
                return AAOS_ENOTFOUND;
            }
            memset(self->camera_id, '\0', 40);
            if ((qhy_ret = C_GetQHYCCDId(self->camera_index - 1, self->camera_id)) == -1) {
                Pthread_mutex_unlock(&self->_.d_state.mtx);
                return AAOS_EDEVMAL;
            }
        } else {
            for (i = 0; i < qhy_ret; i++) {
                memset(self->camera_id, '\0', 40);
                if ((qhy_ret = C_GetQHYCCDId(i, self->camera_id)) == -1) {
                    Pthread_mutex_unlock(&self->_.d_state.mtx);
                    return AAOS_EDEVMAL;
                }
                if (memcmp(self->camera_id, self->_.name) == 0) {
                    self->camera_index = i + 1;
                    break;   
                }
            }
            if (i == qhy_ret - 1) {
                Pthread_mutex_unlock(&self->_.d_state.mtx);
                return AAOS_ENOTFOUND;
            }
        }
        if ((self->camera_handle = C_OpenQHYCCD(self->camera_id)) == NULL) {
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EDEVMAL;
        }
        if ((qhy_ret = C_InitQHYCCD(self->camera_handle)) == -1) {
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            C_CloseQHYCCD(self->camera_handle);
            return AAOS_EDEVMAL;
        }
        if ((qhy_ret = C_GetQHYCCDInfo(self->camera_handle, &chip_width, &chip_height, &width, &height, &pixel_width, &pixel_height, &bits_per_pixel)) == -1) {
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            C_CloseQHYCCD(self->camera_handle);
            return AAOS_EDEVMAL;
        }
        self->_.d_cap.width = width;
        self->_.d_cap.height = height;
        if ((qhy_ret = C_SetQHYCCDBinMode(self->camera_handle, 1, 1)) == -1) {
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            C_CloseQHYCCD(self->camera_handle);
            return AAOS_EDEVMAL;
        }
        self->_.d_param.x_binning = 1;
        self->_.d_param.y_binning = 1;

        if ((qhy_ret = C_SetQHYCCDResolution(self->camera_handle, 0, 0, width, height)) == -1) {
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            C_CloseQHYCCD(self->camera_handle);
            return AAOS_EDEVMAL;
        }
        self->_.d_param.image_width = width;
        self->_.d_param.image_height = height;
        self->_d.param.x_offset = 0;
        self->_d.param.y_offset = 0;
        switch (bits_per_pixel) {
            case 8:
                self->_.d_param.pixel_format = DETECTOR_PIXEL_FORMAT_MONO_8;
                break;
            case 16:
                self->_.d_param.pixel_format = DETECTOR_PIXEL_FORMAT_MONO_16;
                break;
            case 24: 
                self->_.d_param.pixel_format = DETECTOR_PIXEL_FORMAT_RGB_24;
                break;
            default:
                break;
        }
        if (self->d_param.gain > 0.) {
            if ((qhy_ret = C_SetQHYCCDParam_Gain(self->camera_handle, self->_d.param.gain)) == -1) {
                Pthread_mutex_unlock(&self->_.d_state.mtx);
                C_CloseQHYCCD(self->camera_handle);
                return AAOS_EDEVMAL;
            }
        }
        if (self->_d.param.temperature < 50.) {
            if ((qhy_ret = C_SetQHYCCDParam_TargetTemperature(self->camera_handle, self->_d.param.temperature)) == -1) {
                Pthread_mutex_unlock(&self->_.d_state.mtx);
                C_CloseQHYCCD(self->camera_handle);
                return AAOS_EDEVMAL;
            }    
        }
        if ((qhy_ret = C_SetQHYCCDParam_Brightness(self->camera_handle, 0)) == -1) {
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            C_CloseQHYCCD(self->camera_handle);
            return AAOS_EDEVMAL;
        }
        if ((qhy_ret = C_SetQHYCCDParam_Contrast(self->camera_handle, 0.)) == -1) {
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            C_CloseQHYCCD(self->camera_handle);
            return AAOS_EDEVMAL;
        }
        if ((qhy_ret = C_SetQHYCCDParam_Gamma(self->camera_handle, 0.)) == -1) {
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            C_CloseQHYCCD(self->camera_handle);
            return AAOS_EDEVMAL;
        }
        
        state = DETECTOR_STATE_IDLE;
        self->state &= state;
        Pthread_mutex_unlock(&self->_.d_state.mtx);
    } else {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        return AAOS_OK;
    }
}

static int
QHYCamera_set_binning(void *_self, uint32_t x_binning, uint32_t y_binning)
{
    struct QHYCamera *self = cast(QHYCamera(), _self);

    uint32_t (*C_SetQHYCCDBinMode)(void *, uint32_t, uint32_t);
    uint32_t state;
    uint16_t options;
    int ret = AAOS_OK;
    uint32_t qhy_ret;
    size_t i;

    if (!self->_.d_cap.binning_avaliable) {
        return AAOS_ENOTSUP;
    }
    if (self->_.d_cap.x_binning_array != NULL) {
        for (i = 0; i < self->_.d_cap.n_x_binning; i++) {
            if (x_binning == self->_.d_cap.x_binning_array[i]) {
                break;
            }
        }
        if (i == self->_.d_cap.n_x_binning) {
            return AAOS_EINVAL;
        }
    } else {
        if (x_binning < self->_.d_cap.x_binning_min || x_binning > self->_.d_cap.x_binning_max) {
            return AAOS_EINVAL;
        }
    }
    if (self->_.d_cap.y_binning_array != NULL) {
        for (i = 0; i < self->_.d_cap.n_y_binning; i++) {
            if (y_binning == self->_.d_cap.y_binning_array[i]) {
                break;
            }
        }
        if (i == self->_.d_cap.n_y_binning) {
            return AAOS_EINVAL;
        }
    } else {
        if (y_binning < self->_.d_cap.y_binning_min || x_binning > self->_.d_cap.y_binning_max) {
            return AAOS_EINVAL;
        }
    }

    C_SetQHYCCDBinMode = dlysm(self->dlh, "C_SetQHYCCDBinMode");
    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    if ((state&DETECTOR_STATE_MALFUNCTION) && !(options&DETECTOR_OPTION_IGNORE_DEVMAL)) {
        ret = AAOS_EDEVMAL;
        goto error;
    } else if ((state&DETECTOR_STATE_READING) || (state&DETECTOR_STATE_EXPOSING)) {
        if (options&DETECTOR_OPTION_NOWAIT) {
            ret = AAOS_EBUSY;
            goto error;
        }
        while ((state&DETECTOR_STATE_EXPOSING) || (state&DETECTOR_STATE_READING)) {
            Pthread_cond_wait(&self->_.d_state.cond, &self->_.d_state.mtx);
        }
    } else if (state&DETECTOR_STATE_UNINITIALIZED) {
        ret = AAOS_EUNINIT;
        goto error;
    } else if (state&DETECTOR_STATE_OFFLINE) {
        ret = AAOS_EPWROFF;
        goto error;
    }

    Pthread_rwlock_wrlock(&self->_.d_param.rwlock);
    if ((qhy_ret = C_SetQHYCCDResolution(self->camera_handle, x_offset, y_offset, width, height)) == -1) {  
        ret = AAOS_EDEVMAL;
        Pthread_rwlock_unlock(&self->_.d_param.rwlock);
        goto error;
    }
    self->_.d_param.x_binning = x_binning;
    self->_.d_param.y_binning = y_binning;
    Pthread_rwlock_unlock(&self->_.d_param.rwlock);

error:
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    return ret;
}

static int
QHYCamera_get_binning(void *_self, uint32_t *x_binning, uint32_t *y_binning)
{
    struct QHYCamera *self = cast(QHYCamera(), _self);

    Pthread_rwlock_rdlock(&self->_.d_param.rwlock);
    *x_binning = self->_.d_param.x_binning;
    *y_binning = self->_.d_param.y_binning;
    Pthread_rwlock_unlock(&self->_.d_param.rwlock);

    return AAOS_OK;
}

static int
QHYCamera_set_exposure_time(void *_self, double exposure_time)
{
    struct QHYCamera *self = cast(QHYCamera(), _self);

    uint32_t (*C_SetQHYCCDParam_Exposure)(void *, double);
    uint32_t state;
    uint16_t options;
    int ret = AAOS_OK;
    uint32_t qhy_ret;

    C_SetQHYCCDParam_Exposure = dlysm(self->dlh, "C_SetQHYCCDParam_Exposure");
    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    if ((state&DETECTOR_STATE_MALFUNCTION) && !(options&DETECTOR_OPTION_IGNORE_DEVMAL)) {
        ret = AAOS_EDEVMAL;
        goto error;
    } else if (state&DETECTOR_STATE_UNINITIALIZED) {
        ret = AAOS_EUNINIT;
        goto error;
    } else if (state&DETECTOR_STATE_OFFLINE) {
        ret = AAOS_EPWROFF;
        goto error;
    }
    if (!self->_.d_cap.cooling_available) {
        ret = AAOS_ENOTSUP;
        goto error;
    }
    Pthread_rwlock_wrlock(self->_.d_param.rwlock);
    if ((qhy_ret = C_SetQHYCCDParam_Exposure(self->camera_handle, exposure_time * 1000000.)) == -1) {  
        ret = AAOS_EDEVMAL;
        goto error;
    }
    self->_.d_param.exposure_time = exposure_time;
    Pthread_rwlock_unlock(self->_.d_param.rwlock);

error:
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    return ret;
}

static int
QHYCamera_get_exposure_time(void *_self, double *exposure_time)
{
    struct QHYCamera *self = cast(QHYCamera(), _self);

    Pthread_rwlock_rdlock(&self->_.d_param.rwlock);
    *exposure_time = self->_.d_param.exposure_time;
    Pthread_rwlock_unlock(&self->_.d_param.rwlock);

    return AAOS_OK;
}

static int
QHYCamera_set_frame_rate(void *_self, double frame_rate)
{
    return AAOS_ENOTSUP;
}

static int
QHYCamera_get_frame_rate(void *_self, double *frame_rate)
{
    return AAOS_ENOTSUP;
}

static int
QHYCamera_set_gain(void *_self, double gain)
{
    struct QHYCamera *self = cast(QHYCamera(), _self);

    uint32_t (*C_SetQHYCCDParam_Gain)(void *, double);
    uint32_t state;
    uint16_t options;
    int ret = AAOS_OK;
    uint32_t qhy_ret;

    C_SetQHYCCDParam_Gain = dlysm(self->dlh, "C_SetQHYCCDParam_Gain");
    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    if ((state&DETECTOR_STATE_MALFUNCTION) && !(options&DETECTOR_OPTION_IGNORE_DEVMAL)) {
        ret = AAOS_EDEVMAL;
        goto error;
    } else if ((state&DETECTOR_STATE_READING) || (state&DETECTOR_STATE_EXPOSING)) {
        if (options&DETECTOR_OPTION_NOWAIT) {
            ret = AAOS_EBUSY;
            goto error;
        }
        while ((state&DETECTOR_STATE_EXPOSING) || (state&DETECTOR_STATE_READING)) {
            Pthread_cond_wait(&self->_.d_state.cond, &self->_.d_state.mtx);
        }
    } else if (state&DETECTOR_STATE_UNINITIALIZED) {
        ret = AAOS_EUNINIT;
        goto error;
    } else if (state&DETECTOR_STATE_OFFLINE) {
        ret = AAOS_EPWROFF;
        goto error;
    }
    Pthread_rwlock_wrlock(self->_.d_param.rwlock);
    if ((qhy_ret = C_SetQHYCCDParam_Gain(self->camera_handle, gain)) == -1) {  
        ret = AAOS_EDEVMAL;
        goto error;
    }
    self->_.d_param.gain = gain;
    Pthread_rwlock_unlock(self->_.d_param.rwlock);

error:
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    return ret;
}

static int
QHYCamera_get_gain(void *_self, double *gain)
{
    struct QHYCamera *self = cast(QHYCamera(), _self);

    Pthread_rwlock_rdlock(&self->_.d_param.rwlock);
    *gain = self->_.d_param.gain;
    Pthread_rwlock_unlock(&self->_.d_param.rwlock);

    return AAOS_OK;
}

static int
QHYCamera_set_overscan(void *_self, uint32_t x_overscan, uint32_t y_overscan)
{
    return AAOS_ENOTSUP;
}

static int
QHYCamera_get_overscan(void *_self, uint32_t *x_overscan, uint32_t *y_overscan)
{
    return AAOS_ENOTSUP;
}

static int
QHYCamera_set_pixel_format(void *_self, uint32_t pixel_format)
{
    struct QHYCamera *self = cast(QHYCamera(), _self);

    uint32_t (*C_SetQHYCCDParam_Bits)(void *, double);
    double bits = 0.;
    uint32_t state;
    uint16_t options;
    size_t i;
    int ret = AAOS_OK;
    uint32_t qhy_ret;

    C_SetQHYCCDParam_Bits = dlysm(self->dlh, "C_SetQHYCCDParam_Bits");
    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    if ((state&DETECTOR_STATE_MALFUNCTION) && !(options&DETECTOR_OPTION_IGNORE_DEVMAL)) {
        ret = AAOS_EDEVMAL;
        goto error;
    } else if ((state&DETECTOR_STATE_READING) || (state&DETECTOR_STATE_EXPOSING)) {
        if (options&DETECTOR_OPTION_NOWAIT) {
            ret = AAOS_EBUSY;
            goto error;
        }
        while ((state&DETECTOR_STATE_EXPOSING) || (state&DETECTOR_STATE_READING)) {
            Pthread_cond_wait(&self->_.d_state.cond, &self->_.d_state.mtx);
        }
    } else if (state&DETECTOR_STATE_UNINITIALIZED) {
        ret = AAOS_EUNINIT;
        goto error;
    } else if (state&DETECTOR_STATE_OFFLINE) {
        ret = AAOS_EPWROFF;
        goto error;
    }
    if (!self->_.d_cap.pixel_format_available) {
        ret = AAOS_ENOTSUP;
        goto error;
    }
    for (i = 0; i < self->_.d_cap.n_pixel_format; i ++) {
        if (pixel_format == self->_.d_cap.pixel_format[i]) {
            switch (pixel_format) {
                case DETECTOR_PIXEL_FORMAT_MONO_8:
                    bits = 8.;
                    break;
                case DETECTOR_PIXEL_FORMAT_MONO_16:
                    bits = 16.;
                    break;
                case DETECTOR_PIXEL_FORMAT_RGB_24:
                    bits = 24.;
                    break;
                default:
                    break;
            }
            break;
        }
    }
    if (bits == 0.) {
        ret = AAOS_EINVAL;
        goto error;
    }
    Pthread_rwlock_wrlock(self->_.d_param.rwlock);
    if ((qhy_ret = C_SetQHYCCDParam_Bits(self->camera_handle, bits)) == -1) {  
        ret = AAOS_EDEVMAL;
        goto error;
    }
    self->_.d_param.gain = gain;
    Pthread_rwlock_unlock(self->_.d_param.rwlock);

error:
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    return ret;
}

static int
QHYCamera_get_pixel_format(void *_self, uint32_t *pixel_format)
{
    struct QHYCamera *self = cast(QHYCamera(), _self);

    Pthread_rwlock_rdlock(&self->_.d_param.rwlock);
    *pixel_format = self->_.d_param.pixel_format;
    Pthread_rwlock_unlock(&self->_.d_param.rwlock);

    return AAOS_OK;
}

static int
QHYCamera_set_region(void *_self, uint32_t x_offset, uint32_t y_offset, uint32_t width, uint32_t height)
{
    struct QHYCamera *self = cast(QHYCamera(), _self);

    uint32_t (*C_SetQHYCCDResolution)(void *, uint32_t, uint32_t, uint32_t, uint32_t);
    uint32_t state;
    uint16_t options;
    int ret = AAOS_OK;
    uint32_t qhy_ret;

    C_SetQHYCCDResolution = dlysm(self->dlh, "C_SetQHYCCDResolution");
    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    if ((state&DETECTOR_STATE_MALFUNCTION) && !(options&DETECTOR_OPTION_IGNORE_DEVMAL)) {
        ret = AAOS_EDEVMAL;
        goto error;
    } else if ((state&DETECTOR_STATE_READING) || (state&DETECTOR_STATE_EXPOSING)) {
        if (options&DETECTOR_OPTION_NOWAIT) {
            ret = AAOS_EBUSY;
            goto error;
        }
        while ((state&DETECTOR_STATE_EXPOSING) || (state&DETECTOR_STATE_READING)) {
            Pthread_cond_wait(&self->_.d_state.cond, &self->_.d_state.mtx);
        }
    } else if (state&DETECTOR_STATE_UNINITIALIZED) {
        ret = AAOS_EUNINIT;
        goto error;
    } else if (state&DETECTOR_STATE_OFFLINE) {
        ret = AAOS_EPWROFF;
        goto error;
    }
    if ((x_offset + width) > self->_.d_cap.width / self->_.d_param.x_binning || (y_offset + width) > self->_.d_cap.height / self->_.d_param.y_binning) {
        ret = AAOS_EINVAL;
        goto error;
    }

    Pthread_rwlock_wrlock(self->_.d_param.rwlock);
    if ((qhy_ret = C_SetQHYCCDResolution(self->camera_handle, x_offset, y_offset, width, height)) == -1) {  
        ret = AAOS_EDEVMAL;
        goto error;
    }
    self->_.d_param.x_offset = x_offset;
    self->_.d_param.y_offset = y_offset;
    self->_.d_param.image_width = width;
    self->_.d_param.image_height = height;
    Pthread_rwlock_unlock(self->_.d_param.rwlock);

error:
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    return ret;
}

static int
QHYCamera_get_region(void *_self, uint32_t *x_offset, uint32_t *y_offset, uint32_t *width, uint32_t *height)
{
    struct QHYCamera *self = cast(QHYCamera(), _self);

    uint32_t (*C_GetQHYCCDEffectiveArea)(void *, uint32_t *, uint32_t *, uint32_t *, uint32_t *);
    uint32_t state;
    uint16_t options;
    int ret = AAOS_OK;
    uint32_t qhy_ret;

    C_GetQHYCCDEffectiveArea = dlysm(self->dlh, "_GetQHYCCDEffectiveArea");
    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    if ((state&DETECTOR_STATE_MALFUNCTION) && !(options&DETECTOR_OPTION_IGNORE_DEVMAL)) {
        ret = AAOS_EDEVMAL;
        goto error;
    } else if (state&DETECTOR_STATE_UNINITIALIZED) {
        ret = AAOS_EUNINIT;
        goto error;
    } else if (state&DETECTOR_STATE_OFFLINE) {
        ret = AAOS_EPWROFF;
        goto error;
    }

    if ((qhy_ret = C_GetQHYCCDEffectiveArea(self->camera_handle, x_offset, y_offset, width, height)) == -1) {  
        ret = AAOS_EDEVMAL;
        goto error;
    }

error:
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    return ret;
}

static int
QHYCamera_set_temperature(void *_self, double temperature)
{
    struct QHYCamera *self = cast(QHYCamera(), _self);

    uint32_t (*C_SetQHYCCDParam_TargetTemperature)(void *, double);
    uint32_t state;
    uint16_t options;
    int ret = AAOS_OK;
    uint32_t qhy_ret;

    C_SetQHYCCDParam_TargetTemperature = dlysm(self->dlh, "C_SetQHYCCDParam_TargetTemperature");
    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    if ((state&DETECTOR_STATE_MALFUNCTION) && !(options&DETECTOR_OPTION_IGNORE_DEVMAL)) {
        ret = AAOS_EDEVMAL;
        goto error;
    } else if (state&DETECTOR_STATE_UNINITIALIZED) {
        ret = AAOS_EUNINIT;
        goto error;
    } else if (state&DETECTOR_STATE_OFFLINE) {
        ret = AAOS_EPWROFF;
        goto error;
    }
    if (!self->_.d_cap.cooling_available) {
        ret = AAOS_ENOTSUP;
        goto error;
    }
    Pthread_rwlock_wrlock(self->_.d_param.rwlock);
    if ((qhy_ret = C_SetQHYCCDParam_TargetTemperature(self->camera_handle, temperature)) == -1) {  
        ret = AAOS_EDEVMAL;
        goto error;
    }
    self->_.d_param.temperature = temperature;
    Pthread_rwlock_unlock(self->_.d_param.rwlock);

error:
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    return ret;
}

static int
QHYCamera_get_temperature(void *_self, double *temperature)
{
    struct QHYCamera *self = cast(QHYCamera(), _self);

    double (*C_GetQHYCCDParam_CurTempture)(void *);
    uint32_t state;
    uint16_t options;
    int ret = AAOS_OK;
    uint32_t qhy_ret;

    C_GetQHYCCDParam_CurTempture = dlysm(self->dlh, "C_GetQHYCCDParam_CurTempture");
    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    if ((state&DETECTOR_STATE_MALFUNCTION) && !(options&DETECTOR_OPTION_IGNORE_DEVMAL)) {
        ret = AAOS_EDEVMAL;
        goto error;
    } else if (state&DETECTOR_STATE_UNINITIALIZED) {
        ret = AAOS_EUNINIT;
        goto error;
    } else if (state&DETECTOR_STATE_OFFLINE) {
        ret = AAOS_EPWROFF;
        goto error;
    }

    *temperature = C_GetQHYCCDParam_CurTempture(self->camera_handle);
error:
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    return ret;
}

static int
QHYCamera_set_trigger_mode(void *_self, uint32_t trigger_mode)
{
    return AAOS_ENOTSUP;
    /*
    struct QHYCamera *self = cast(QHYCamera(), _self);

    uint32_t (*C_SetQHYCCDParam_Bits)(void *, double);
    double bits = 0.;
    uint32_t state;
    uint16_t options;
    size_t i;
    int ret = AAOS_OK;
    uint32_t qhy_ret;

    C_SetQHYCCDParam_Bits = dlysm(self->dlh, "C_SetQHYCCDParam_Bits");
    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    if ((state&DETECTOR_STATE_MALFUNCTION) && !(options&DETECTOR_OPTION_IGNORE_DEVMAL)) {
        ret = AAOS_EDEVMAL;
        goto error;
    } else if ((state&DETECTOR_STATE_READING) || (state&DETECTOR_STATE_EXPOSING)) {
        if (options&DETECTOR_OPTION_NOWAIT) {
            ret = AAOS_EBUSY;
            goto error;
        }
        while ((state&DETECTOR_STATE_EXPOSING) || (state&DETECTOR_STATE_READING)) {
            Pthread_cond_wait(&self->_.d_state.cond, &self->_.d_state.mtx);
        }
    } else if (state&DETECTOR_STATE_UNINITIALIZED) {
        ret = AAOS_EUNINIT;
        goto error;
    } else if (state&DETECTOR_STATE_OFFLINE) {
        ret = AAOS_EPWROFF;
        goto error;
    }
    if (!self->_.d_cap.pixel_format_available) {
        ret = AAOS_ENOTSUP;
        goto error;
    }
    for (i = 0; i < self->_.d_cap.n_pixel_format; i ++) {
        if (pixel_format == self->_.d_cap.pixel_format[i]) {
            switch (pixel_format) {
                case DETECTOR_PIXEL_FORMAT_MONO_8:
                    bits = 8.;
                    break;
                case DETECTOR_PIXEL_FORMAT_MONO_16:
                    bits = 16.;
                    break;
                case DETECTOR_PIXEL_FORMAT_RGB_24:
                    bits = 24.;
                    break;
                default:
                    break;
            }
            break;
        }
    }
    if (bits == 0.) {
        ret = AAOS_EINVAL;
        goto error;
    }
    Pthread_rwlock_wrlock(self->_.d_param.rwlock);
    if ((qhy_ret = C_SetQHYCCDParam_Bits(self->camera_handle, bits)) == -1) {  
        ret = AAOS_EDEVMAL;
        goto error;
    }
    self->_.d_param.gain = gain;
    Pthread_rwlock_unlock(self->_.d_param.rwlock);

error:
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    return ret;
*/
}

static int
QHYCamera_get_trigger_mode(void *_self, uint32_t *trigger_mode)
{
    /*
    struct QHYCamera *self = cast(QHYCamera(), _self);

    Pthread_rwlock_rdlock(&self->_.d_param.rwlock);
    *pixel_format = self->_.d_param.pixel_format;
    Pthread_rwlock_unlock(&self->_.d_param.rwlock);
    */
    return AAOS_ENOTSUP;
}

static int
QHYCamera_stop(void *_self)
{
    struct QHYCamera *self = cast(QHYCamera(), _self);
    
    unsigned int state;
    uint16_t options;

    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    if ((state&DETECTOR_STATE_EXPOSING) || (state&DETECTOR_STATE_READING)) {
        Pthread_mutex_lock(&self->_.d_exp.mtx);
        self->_.d_exp.stop_flag = true;
        while (self->_.d_exp.stop_flag) {
            Pthread_cond_wait(&self->_.d_exp.cond, &self->_.d_exp.mtx);
        }
        Pthread_mutex_unlock(&self->_.d_exp.mtx);
    }
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    
    return AAOS_OK;
}

static int 
QHYCamera_abort(void *_self)
{
    struct QHYCamera *self = cast(QHYCamera(), _self);
    
    uint32_t state;
    uint16_t options;
    int ret = AAOS_OK;
    uint32_t (*abort_func)(void *);
    uint32_t qhy_ret;

    Pthread_rwlock_rdlock(self->_.d_param.rwlock)
    switch (self->_.d_param.capture_mode)  {
        case DETECTOR_CAPTURE_MODE_SNAPSHOT:
            abort_func = dlsym(self->dlh, "C_CancelQHYCCDExposingAndReadout");
            break;
        case DETECTOR_CAPTUTE_MODE_MULTIFRAME:
            abort_func = dlsym(self->dlh, "C_StopQHYCCDLive");
            break;
        case DETECTOR_CAPTUTE_MODE_VIDEO:
            abort_func = dlsym(self->dlh, "C_StopQHYCCDLive");
            break;
        default:
            abort_func = dlsym(self->dlh, "C_StopQHYCCDLive");
            break;
    }

    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    if ((state&DETECTOR_STATE_EXPOSING) || (state&DETECTOR_STATE_READING)) {

        if ((qhy_ret = abort_func(self->camera_handles)) == -1) {
#ifdef DEBUG
            fprintf(stderr, "%s %s %d: abort_func error.\n", __FILE__, __func__, __LINE__ - 2);
#endif
	        ret = AAOS_EDEVMAL;
	        goto error;
	    }
	    Pthread_mutex_lock(&self->_.d_exp.mtx);
	    self->_.d_exp.stop_flag = true;
        Pthread_mutex_unlock(&self->_.d_exp.mtx);
    }

error:
    Pthread_mutex_unlock(&self->_.d_state.mtx);

    return ret;

}

static int 
QHYCamera_info_json(struct QHYCamera *self, void *buffer, size_t size)
{	
    cJSON *root_json, *qhy_json, *capability_json, *array_json;
    char *string = NULL;
    size_t i;
	
    root_json = cJSON_CreateObject();
    cJSON_AddStringToObject(root_json, "name", self->_.name);
    if (self->_.description != NULL) {
        cJSON_AddStringToObject(root_json, "description", self->_.description);
    }
	
	qhy_json = cJSON_CreateObject();
	cJSON_AddStringToObject(qhy_json, "name", camera_info->Name);
    cJSON_AddNumberToObject(qhy_json, "camera_index", self->camera_index);
	cJSON_AddStringToObject(qhy_json, "camera_index", self->camera_id);
	
	capability_json = cJSON_CreateObject();
	cJSON_AddNumberToObject(capability_json, "width", self->_.d_cap.width);
	cJSON_AddNumberToObject(capability_json, "heigth", self->_.d_cap.height);
	cJSON_AddNumberToObject(capability_json, "x_n_chip", self->_.d_cap.x_n_chip);
	cJSON_AddNumberToObject(capability_json, "y_n_chip", self->_.d_cap.y_n_chip);
	cJSON_AddNumberToObject(capability_json, "n_chip", self->_.d_cap.n_chip);
    cJSON_AddNumberToObject(capability_json, "n_channel", self->_.d_cap.n_channel);

	if (self->_.d_cap.flip_map != NULL) {

	}
	if (self->_.d_cap.mirror_map != NULL) {
		
	}	
	
	if (self->_.d_cap.binning_available) {
		cJSON_AddStringToObject(capability_json, "binning_available", "true");
        if (self->_.d_cap.x_binning_array != NULL && self->_.d_cap.y_binning_array != NULL) {
            char binning_buffer[BUFSIZE];
            FILE *fp;
			size_t i;
            fp = fmemopen(binning_buffer, BUFSIZE, "w");
			for (i = 0; i < self->_.d_cap.n_x_binning - 1; i++) {
				fprintf(fp, "(%d,%d) ", self->_.d_cap.x_binning_array[i], self->_.d_cap.y_binning_array[i]);
			}
			fprintf(fp, "(%d,%d)", self->_.d_cap.x_binning_array[self->_.d_cap.n_x_binning - 1], self->_.d_cap.y_binning_array[self->_.d_cap.n_x_binning - 1]);
			cJSON_AddStringToObject(capability_json, "binning_array", binning_buffer);
        } else {
			cJSON_AddNumberToObject(capability_json, "x_binning_min", self->_.d_cap.x_binning_min);
			cJSON_AddNumberToObject(capability_json, "x_binning_max", self->_.d_cap.x_binning_max);
			cJSON_AddNumberToObject(capability_json, "y_binning_max", self->_.d_cap.y_binning_min);
			cJSON_AddNumberToObject(capability_json, "y_binning_max", self->_.d_cap.y_binning_max);
        }		
	} else {
		cJSON_AddStringToObject(capability_json, "binning_available", "false");
	}
	
	if (self->_.d_cap.offset_available) {
		cJSON_AddStringToObject(capability_json, "ROI_available", "true");
	} else {
		cJSON_AddStringToObject(capability_json, "ROI_available", "false");
	}
	
	if (self->_.d_cap.gain_available) {
		cJSON_AddStringToObject(capability_json, "gain_available", "true");
		if (self->_.d_cap.gain_array == NULL) {
			cJSON_AddNumberToObject(capability_json, "gain_min", self->_.d_cap.gain_min);
			cJSON_AddNumberToObject(capability_json, "gain_max", self->_.d_cap.gain_max);
		} else {
			array_json = cJSON_CreateArray();
			for (i = 0; i < self->_.d_cap.n_gain; i++) {
				cJSON_AddItemToArray(array_json, cJSON_CreateNumber(self->_.d_cap.gain_array[i]));
			}
			cJSON_AddItemToObject(capability_json, "gain_array", array_json);
		}
	} else {
		cJSON_AddStringToObject(capability_json, "gain_available", "false");
	}
	if (self->_.d_cap.auto_gain_available) {
		cJSON_AddStringToObject(capability_json, "auto_gain_available", "true");
		cJSON_AddNumberToObject(capability_json, "auto_gain_min", self->_.d_cap.auto_gain_min);
		cJSON_AddNumberToObject(capability_json, "auto_gain_max", self->_.d_cap.auto_gain_max);
	} else {
		cJSON_AddStringToObject(capability_json, "auto_gain_available", "false");
	}
	
	if (self->_.d_cap.exposure_time_available) {
		cJSON_AddStringToObject(capability_json, "exposure_time_available", "true");
		if (self->_.d_cap.exposure_time_array == NULL) {
			cJSON_AddNumberToObject(capability_json, "exposure_time_min", self->_.d_cap.exposure_time_min);
			cJSON_AddNumberToObject(capability_json, "exposure_time_max", self->_.d_cap.exposure_time_max);
		} else {
			array_json = cJSON_CreateArray();
			for (i = 0; i < self->_.d_cap.n_exposure_time; i++) {
				cJSON_AddItemToArray(array_json, cJSON_CreateNumber(self->_.d_cap.exposure_time_array[i]));
			}
			cJSON_AddItemToObject(capability_json, "exposure_time_array", array_json);
		}
	} else {
		cJSON_AddStringToObject(capability_json, "exposure_time_available", "false");
	}
	if (self->_.d_cap.auto_exposure_available) {
		cJSON_AddStringToObject(capability_json, "auto_exposure_time_available", "true");
		cJSON_AddNumberToObject(capability_json, "auto_exposure_time_min", self->_.d_cap.auto_exposure_time_min);
		cJSON_AddNumberToObject(capability_json, "auto_exposure_time_max", self->_.d_cap.auto_exposure_time_max);
	} else {
		cJSON_AddStringToObject(capability_json, "auto_exposure_time_available", "false");
	}
	
	if (self->_.d_cap.frame_rate_available) {
		cJSON_AddStringToObject(capability_json, "frame_rate_available", "true");
		if (self->_.d_cap.frame_rate_array == NULL) {
			cJSON_AddNumberToObject(capability_json, "frame_rate_min", self->_.d_cap.frame_rate_min);
			cJSON_AddNumberToObject(capability_json, "frame_rate_max", self->_.d_cap.frame_rate_max);
		} else {
			array_json = cJSON_CreateArray();
			for (i = 0; i < self->_.d_cap.n_frame_rate; i++) {
				cJSON_AddItemToArray(array_json, cJSON_CreateNumber(self->_.d_cap.frame_rate_array[i]));
			}
			cJSON_AddItemToObject(capability_json, "frame_rate_array", array_json);
		}
	} else {
		cJSON_AddStringToObject(capability_json, "frame_rate_available", "false");
	}
	if (self->_.d_cap.auto_frame_rate_available) {
		cJSON_AddStringToObject(capability_json, "auto_frame_rate_available", "true");
		cJSON_AddNumberToObject(capability_json, "auto_frame_rate_min", self->_.d_cap.auto_frame_rate_min);
		cJSON_AddNumberToObject(capability_json, "auto_frame_rate_max", self->_.d_cap.auto_frame_rate_max);
	} else {
		cJSON_AddStringToObject(capability_json, "auto_frame_rate_available", "false");
	}
	
	if (self->_.d_cap.pixel_format_available) {
		cJSON_AddStringToObject(capability_json, "pixel_format_available", "true");
		array_json = cJSON_CreateArray();
		for (i = 0; i < self->_.d_cap.n_pixel_format; i++) {
			switch (self->_.d_cap.pixel_format_array[i]) {
				case DETECTOR_PIXEL_FORMAT_MONO_8:
				    cJSON_AddItemToArray(array_json, cJSON_CreateString("MONO_8"));
				    break;
				case DETECTOR_PIXEL_FORMAT_MONO_10:
				    cJSON_AddItemToArray(array_json, cJSON_CreateString("MONO_10"));
				    break;
				case DETECTOR_PIXEL_FORMAT_MONO_10_PACKED:
				    cJSON_AddItemToArray(array_json, cJSON_CreateString("MONO_10_PACKED"));
				    break;
				case DETECTOR_PIXEL_FORMAT_MONO_12:
				    cJSON_AddItemToArray(array_json, cJSON_CreateString("MONO_12"));
				    break;
				case DETECTOR_PIXEL_FORMAT_MONO_12_PACKED:
				    cJSON_AddItemToArray(array_json, cJSON_CreateString("MONO_12_PACKED"));
				    break;
				case DETECTOR_PIXEL_FORMAT_MONO_14:
				    cJSON_AddItemToArray(array_json, cJSON_CreateString("MONO_14"));
				    break;
				case DETECTOR_PIXEL_FORMAT_MONO_14_PACKED:
				    cJSON_AddItemToArray(array_json, cJSON_CreateString("MONO_14_PACKED"));
				    break;
				case DETECTOR_PIXEL_FORMAT_MONO_16:
				    cJSON_AddItemToArray(array_json, cJSON_CreateString("MONO_16"));
				    break;
				case DETECTOR_PIXEL_FORMAT_MONO_18:
				    cJSON_AddItemToArray(array_json, cJSON_CreateString("MONO_18"));
				    break;
				case DETECTOR_PIXEL_FORMAT_MONO_18_PACKED:
				    cJSON_AddItemToArray(array_json, cJSON_CreateString("MONO_18_PACKED"));
				    break;
				case DETECTOR_PIXEL_FORMAT_MONO_24:
				    cJSON_AddItemToArray(array_json, cJSON_CreateString("MONO_24"));
				    break;
				case DETECTOR_PIXEL_FORMAT_MONO_24_PACKED:
				    cJSON_AddItemToArray(array_json, cJSON_CreateString("MONO_24_PACKED"));
				    break;
				case DETECTOR_PIXEL_FORMAT_MONO_32:
				    cJSON_AddItemToArray(array_json, cJSON_CreateString("MONO_32"));
				    break;
				case DETECTOR_PIXEL_FORMAT_MONO_64:
				    cJSON_AddItemToArray(array_json, cJSON_CreateString("MONO_64"));
				    break;
				case DETECTOR_PIXEL_FORMAT_RGB_24:
				    cJSON_AddItemToArray(array_json, cJSON_CreateString("RGB_24"));
				    break;
				default:
				    break;
			}
		}
		cJSON_AddItemToObject(capability_json, "pixel_format_array", array_json);
	} else {
		cJSON_AddStringToObject(capability_json, "pixel_format_available", "false");
	}
	
	if (self->_.d_cap.readout_rate_available) {
		cJSON_AddStringToObject(capability_json, "readout_rate_available", "true");
		if (self->_.d_cap.readout_rate_array == NULL) {
			cJSON_AddNumberToObject(capability_json, "readout_rate_min", self->_.d_cap.readout_rate_min);
			cJSON_AddNumberToObject(capability_json, "readout_rate_max", self->_.d_cap.readout_rate_max);
		} else {
			array_json = cJSON_CreateArray();
			for (i = 0; i < self->_.d_cap.n_readout_rate; i++) {
				cJSON_AddItemToArray(array_json, cJSON_CreateNumber(self->_.d_cap.readout_rate_array[i]));
			}
			cJSON_AddItemToObject(capability_json, "readout_rate_array", array_json);
		}
	} else {
		cJSON_AddStringToObject(capability_json, "readout_rate_available", "false");
	}
	if (self->_.d_cap.auto_readout_rate_available) {
		cJSON_AddStringToObject(capability_json, "auto_readout_rate_available", "true");
		cJSON_AddNumberToObject(capability_json, "auto_readout_rate_min", self->_.d_cap.auto_readout_rate_min);
		cJSON_AddNumberToObject(capability_json, "auto_readout_rate_max", self->_.d_cap.auto_readout_rate_max);
	} else {
		cJSON_AddStringToObject(capability_json, "auto_readout_rate_available", "false");
	}
	
	if (self->_.d_cap.cooling_available) {
		cJSON_AddStringToObject(capability_json, "cooling_available", "true");
		cJSON_AddNumberToObject(capability_json, "cooling_temperature_min", self->_.d_cap.cooling_temperature_min);
		cJSON_AddNumberToObject(capability_json, "cooling_temperature_max", self->_.d_cap.cooling_temperature_max);
			
	} else {
		cJSON_AddStringToObject(capability_json, "cooling_available", "false");
	}
	if (self->_.d_cap.auto_cooling_available) {
		cJSON_AddStringToObject(capability_json, "auto_cooling_available", "true");
		cJSON_AddNumberToObject(capability_json, "auto_cooling_temperature_min", self->_.d_cap.auto_cooling_temperature_min);
		cJSON_AddNumberToObject(capability_json, "auto_cooling_temperature_max", self->_.d_cap.auto_cooling_temperature_max);
	} else {
		cJSON_AddStringToObject(capability_json, "auto_cooling_available", "false");
	}
	cJSON_AddItemToObject(root_json, "capability", capability_json);
    
    string = cJSON_Print(root_json);
	cJSON_Delete(root_json);
    snprintf(buffer, size, "%s", string);
    free(string);
}

static int
QHYCamera_info(void *_self, void *buffer, size_t size)
{
    struct QHYCamera *self = cast(QHYCamera(), _self);
    
    QHYCamera_info_json(self, buffer, size);
    
    return AAOS_OK;
}

static int
QHYCamera_status_json(struct QHYCamera self, void *buffer, size_t size)
{
    unsigned int state, options;
    uint32_t image_width, image_height, x_offset, y_offset, x_binning, y_binning, x_overscan, y_overscan, capture_mode, trigger_mode;
    double exposure_time, frame_rate, gain, readout_rate, cooling_temperature;
    bool auto_exposure_time_enable, auto_frame_rate_enable, auto_gain_enable, auto_readout_rate_enable, auto_cooling_enable;
    long value;
    cJSON *root_json;
    char *string;
    double (*C_GetQHYCCDParam_CurTempture)(void *);

    C_GetQHYCCDParam_CurTempture = dlsym(self->dlh, "C_GetQHYCCDParam_CurTempture");
    
    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;
    options = self->_.d_state.options;
    Pthread_mutex_unlock(&self->_.d_state.mtx);

    Pthread_rwlock_rdlock(self->_.d_param.rwlock);
    image_width = self->_.d_param.image_width;
    image_height = self->_.d_param.image_height;
    x_offset = self->_.d_param.x_offset;
    y_offset = self->_.d_param.y_offset;
    x_binning = self->_.d_param.x_binning;
    y_binning = self->_.d_param.y_binning;
    x_overscan = self->_.d_param.x_overscan;
    y_overscan = self->_.d_param.y_overscan;
    auto_exposure_time_enable = self->_.d_param.auto_exposure_time_enable;
    exposure_time = self->_.d_param.exposure_time;
    auto_frame_rate_enable = self->_.d_param.auto_frame_rate_enable;
    frame_rate = self->_.d_param.frame_rate;
    auto_gain_enable = self->_.d_param.auto_gain_enable;
    gain = self->_.d_param.gain;
    auto_readout_rate_enable = self->_.d_param.auto_readout_rate_enable;
    readout_rate = self->_.d_param.readout_rate;
    auto_cooling_enable = self->_.d_param.auto_cooling_enable;
    capture_mode = self->_.d_param.capture_mode;
    overscan_x = self->_.d_param.overscan_x;
    overscan_y = self->_.d_param.overscan_y;
    trigger_mode = self->_.d_param.trigger_mode;
    Pthread_rwlock_unlock(self->_.d_param.rwlock);
    
    
    root_json = cJSON_CreateObject();
    cJSON_AddStringToObject(root_json, "name", self->_.name);
    if (state&DETECTOR_STATE_MALFUNCTION) {
        cJSON_AddStringToObject(root_json, "status", "MALFUNCTION");
    }
    switch (state&(~DETECTOR_STATE_MALFUNCTION)) {
        case DETECTOR_STATE_IDLE:
            cJSON_AddStringToObject(root_json, "state", "IDLE");
            break;
        case DETECTOR_STATE_OFFLINE:
            cJSON_AddStringToObject(root_json, "state", "OFFLINE");
            break;
        case DETECTOR_STATE_UNINITIALIZED:
            cJSON_AddStringToObject(root_json, "state", "UNINITIALIZED");
            break;
        case DETECTOR_STATE_EXPOSING:
            cJSON_AddStringToObject(root_json, "state", "EXPOSING");
            break;
        case DETECTOR_STATE_READING:
            cJSON_AddStringToObject(root_json, "state", "READING");
            break;
        default:
            break;
    }
    
    cJSON_AddNumberToObject(root_json, "image_width", (double) image_width);
    cJSON_AddNumberToObject(root_json, "image_height", (double) image_height);
    cJSON_AddNumberToObject(root_json, "x_offset", (double) x_offset);
    cJSON_AddNumberToObject(root_json, "y_offset", (double) y_offset);
    cJSON_AddNumberToObject(root_json, "x_binning", (double) x_binning);
    cJSON_AddNumberToObject(root_json, "y_binning", (double) y_binning);
    if (self->_.d_cap.overscan_available) {
        cJSON_AddNumberToObject(root_json, "x_overscan", (double) x_overscan);
        cJSON_AddNumberToObject(root_json, "y_overscan", (double) y_overscan);

    }

    if (self->_.d_cap.auto_exposure_time_available) {
        if (auto_exposure_time_enable) {
            cJSON_AddStringToObject(root_json, "auto_exposure_time_enable", "true");
        } else {
            cJSON_AddStringToObject(root_json, "auto_exposure_time_enable", "false");
            cJSON_AddNumberToObject(root_json, "exposure_time", exposure_time);
        }
    } else { 
        cJSON_AddNumberToObject(root_json, "exposure_time", exposure_time);
    }
    
    if (self->_.d_cap.auto_frame_rate_available) {
        if (auto_frame_rate_enable) {
            cJSON_AddStringToObject(root_json, "auto_frame_rate_enable", "true");
        } else {
            cJSON_AddStringToObject(root_json, "auto_frame_rate_enable", "false");
            if (self->_.d_cap.frame_rate_available) {
                cJSON_AddNumberToObject(root_json, "frame_rate", frame_rate);
            } else {
                cJSON_AddNumberToObject(root_json, "frame_rate", 1. / exposure_time);
            }
        }
    } else {
        if (self->_.d_cap.frame_rate_available) {
            cJSON_AddNumberToObject(root_json, "frame_rate", frame_rate);
        } else {
            cJSON_AddNumberToObject(root_json, "frame_rate", 1. / exposure_time);
        }
    }

    if (self->_.d_cap.auto_gain_available) {
        if (auto_gain_enable) {
            cJSON_AddStringToObject(root_json, "auto_gain_enable", "true");
        } else {
            cJSON_AddStringToObject(root_json, "auto_gain_enable", "false");
            cJSON_AddNumberToObject(root_json, "gain", gain);
        }
    } else {
        cJSON_AddNumberToObject(root_json, "gain", gain);
    }

    if (self->_.d_cap.auto_readout_rate_available) {
        if (auto_readout_rate_enable) {
            cJSON_AddStringToObject(root_json, "auto_readout_rate_enable", "true");
        } else {
            cJSON_AddStringToObject(root_json, "auto_readout_rate_enable", "false");
            cJSON_AddNumberToObject(root_json, "readout_rate", readout_rate);
        }
    } else {
        cJSON_AddNumberToObject(root_json, "readout_rate", readout_rate);
    }
    
    if (self->_.d_param.is_cooling_enable) {
        cJSON_AddStringToObject(root_json, "cooling_enabled", "true");
    } else {
        cJSON_AddStringToObject(root_json, "cooling_enabled", "false");
    }

    if (self->_.d_cap.auto_cooling_available) {
        if (auto_cooling_enable) {
            cJSON_AddStringToObject(root_json, "auto_cooling_enabled", "true");
        } else {
            cJSON_AddStringToObject(root_json, "auto_cooling_enabled", "false");
        }
    }

    if (self->_.d_cap.cooling_available && self->_.d_param.is_cooling_enable && !self->_.d_cap.auto_cooling_available) {
        cJSON_AddNumberToObject(root_json, "setting_temperature", self->_.d_param.temperature);
    }
    
    cJSON_AddNumberToObject(root_json, "actual_temperature", C_GetQHYCCDParam_CurTempture(self->camera_handle));
    
    if (self->_.d_cap.capture_mode_available) {
        switch (capture_mode) {
            case DETECTOR_CAPTURE_MODE_SNAPSHOT:
                cJSON_AddStringToObject(root_json, "capture_mode", "snapshot");
                break;
            case DETECTOR_CAPTURE_MODE_MUTLIFRAME:
                cJSON_AddStringToObject(root_json, "capture_mode", "multiframe");
                break;
            case DETECTOR_CAPTURE_MODE_VIDEO:
                cJSON_AddStringToObject(root_json, "capture_mode", "video");
                break;
            default:
                break;
        }
    } else {
        cJSON_AddStringToObject(root_json, "capture_mode", "default");
    }

    if (self->_.d_cap.trigger_mode_available) {
        switch (trigger_mode) {
            case DETECTOR_TRIGGER_MODE_DEFAULT:
                cJSON_AddStringToObject(root_json, "trigger_mode", "deault");
                break;
            case DETECTOR_TRIGGER_MODE_SOFTWARE_EDGE:
                cJSON_AddStringToObject(root_json, "trigger_mode", "software_edge");
                break;
            case DETECTOR_TRIGGER_MODE_SOFTWARE_LEVEL:
                cJSON_AddStringToObject(root_json, "trigger_mode", "software_level");
                break;
            case DETECTOR_TRIGGER_MODE_HARDWARE_LEVEL_HIGH:
                cJSON_AddStringToObject(root_json, "trigger_mode", "hardware_level_high");
                break;
            case DETECTOR_TRIGGER_MODE_HARDWARE_LEVEL_LOW:
                cJSON_AddStringToObject(root_json, "trigger_mode", "hardware_level_low");
                break;
            case DETECTOR_TRIGGER_MODE_HARDWARE_EDGE_RISE:
                cJSON_AddStringToObject(root_json, "trigger_mode", "hardware_edge_rise");
                break;
            case DETECTOR_TRIGGER_MODE_HARDWARE_EDGE_FALL:
                cJSON_AddStringToObject(root_json, "trigger_mode", "hardware_edge_fall");
                break;
            default:
                break;
        }
    } else {
        cJSON_AddStringToObject(root_json, "trigger_mode", "default");
    }
    
    string = cJSON_Print(root_json);
    cJSON_Delete(root_json);
    snprintf(buffer, size, "%s", string);
    free(string);

    return AAOS_OK;
}

static int
QHYCamera_status(void *_self, void *buffer, size_t size)
{
    struct QHYCamera *self = cast(QHYCamera(), _self);

    int ret;
    
    ret = QHYCamera_status_json(self, buffer, size);
    
    return AAOS_OK;
}

struct QHYCameraExposureArg {
    struct QHYCamera *detector;
    void *rpc;
    void *string; /* serialized header data string. */
    unsigned int format; /* string format */
};

static void *
QHYCamera_process_image_thr(void *arg)
{
    struct QHYCameraExposureArg *myarg = (struct QHYCameraExposureArg *) arg;
    struct QHYCamera *detector = myarg->detector;
    void *string = myarg->string;
    void *rpc = myarg->rpc;
    int format = myarg->format;
    uint16_t options = detector->_.d_state.options;
    
    struct DetectorDataFrame *data;
    size_t i, n, width, height;
    char filename[FILENAMESIZE];
    char buf[TIMESTAMPSIZE];
    fitsfile *fptr = NULL;
    int bitpix, datatype, naxis = 2, status = 0;
    long naxes[2];
    uint32_t pixel_format;
    struct timespec tp;
    struct tm tm_buf;

    uint32_t (*C_GetQHYCCDParam_CurTempture)(void *, double *);
    double temperature;

    C_GetQHYCCDParam_CurTempture = dlsym(detector->dlh, "C_GetQHYCCDParam_CurTempture");

    for (; ;) {
        data = threadsafe_queue_wait_and_pop(detector->_.d_proc.queue);
        if (data != NULL) {
            if (data->buffer != NULL) {
                free(data->buffer);
                free(data);
            } else {
                break;
            }
        }
    }
    
    n = data->n;
    width = data->width;
    height = data->height;
    pixel_format = data->pixel_format;
    naxes[0] = width;
    naxes[1] = height;
    
    tp.tv_sec = data->tp.tv_sec;
    tp.tv_nsec = data->tp.tv_nsec;
    /*
     * TODO:
     * RGB data
     */
    switch (pixel_format) {
        case DETECTOR_PIXEL_FORMAT_MONO_8:
            bitpix = BYTE_IMG;
            datatype = TBYTE;
            break;
        case DETECTOR_PIXEL_FORMAT_MONO_16:
            bitpix = USHORT_IMG;
            datatype = TUSHORT;
        default:
            break;
    }
    
    if (!(options&DETECTOR_OPTION_NOTIFY_EACH_COMPLETION)) {
        detector->_.d_proc.name_convention(detector, filename, FILENAMESIZE, 1, 1, &tp);
        if (detector->_.d_proc.tpl_fptr != NULL) {
            fits_create_file(&fptr, filename, &status);
            if (status != 0) {
#ifdef DEBUG
                fprintf(stderr, "%s %s %d --- fits_create_file error: %d\n", __FILE__, __func__, __LINE__ - 2, status);
#endif
            }
            fits_copy_file(detector->_.d_proc.tpl_fptr, fptr, 0, 1, 1, &status);
            if (status != 0) {
#ifdef DEBUG
                fprintf(stderr, "%s %s %d --- fits_copy_file error: %d\n", __FILE__, __func__, __LINE__ - 2, status);
#endif
            }
        } else {
            fits_create_template(&fptr, filename, detector->_.d_proc.tpl_filename, &status);
            if (status != 0) {
#ifdef DEBUG
                fprintf(stderr, "%s %s %d --- fits_create_template error: %d\n", __FILE__, __func__, __LINE__ - 2, status);
#endif
            }
        }
        detector->_.d_proc.pre_acquisition(detector, filename, fptr, &data->tp);
        detector->_.d_proc.img_fptr = fptr;
    }
    
    for (i = 0; i < n; i++) {
        data = threadsafe_queue_wait_and_pop(detector->_.d_proc.queue);
        if (data == NULL) {
            break;
        }
        if (data->buffer == NULL) {
            free(data);
            break;
        }
        if (options&DETECTOR_OPTION_NOTIFY_EACH_COMPLETION) {
            detector->_.d_proc.name_convention(detector, filename, FILENAMESIZE, i + 1, n, &tp);
            if (detector->_.d_proc.tpl_fptr != NULL) {
                fits_create_file(&fptr, filename, &status);
                if (status != 0) {
#ifdef DEBUG
                    fprintf(stderr, "%s %s %d --- fits_create_file error: %d\n", __FILE__, __func__, __LINE__ - 2, status);
#endif
                }
                fits_copy_file(detector->_.d_proc.tpl_fptr, fptr, 0, 1, 1, &status);
                if (status != 0) {
#ifdef DEBUG
                    fprintf(stderr, "%s %s %d --- fits_copy_file error: %d\n", __FILE__, __func__, __LINE__ - 2, status);
#endif
                }
            } else {
                fits_create_template(&fptr, filename, detector->_.d_proc.tpl_filename, &status);
                if (status != 0) {
#ifdef DEBUG
                    fprintf(stderr, "%s %s %d --- fits_create_template error: %d\n", __FILE__, __func__, __LINE__ - 2, status);
#endif
                }
            }
            detector->_.d_proc.pre_acquisition(detector, filename, fptr, &data->tp);
            detector->_.d_proc.img_fptr = fptr;
        }
        
        
        fits_create_img(fptr, bitpix, naxis, naxes, &status);
        if (status != 0) {
#ifdef DEBUG
            fprintf(stderr, "%s %s %d --- fits_create_img error: %d\n", __FILE__, __func__, __LINE__ - 2, status);
#endif
        }
        gmtime_r(&data->tp.tv_sec, &tm_buf);
        strftime(buf, TIMESTAMPSIZE, "%Y-%m-%d", &tm_buf);
        fits_update_key_str(fptr, "DATE-OBS", buf, "end date of this frame", &status);
        strftime(buf, TIMESTAMPSIZE, "%H:%M:%S", &tm_buf);
        snprintf(buf + strlen(buf), TIMESTAMPSIZE - strlen(buf), ".%03d", (int) floor(data->tp.tv_nsec / 1000000));
        fits_update_key_str(fptr, "TIME-OBS", buf, "end time of this frame", &status);
        /*
         *
         */
        if ((qhy_ret = C_GetQHYCCDParam_CurTempture(detector->camera_handle, &temperature)) != -1) {
            fits_update_key_fixfbl(fptr, "CHIPTEMP", temperature, 2, NULL, &status);
        }··
        fits_update_key_fixdbl(fptr, "GAIN", detector->_.d_param.gain, 2, NULL, &status);
        fits_update_key_lng(fptr, "X_OFFSET", detector->_.d_param.x_offset, NULL, &status);
        fits_update_key_lng(fptr, "Y_OFFSET", detector->_.d_param.y_offset, NULL, &status);

        fits_update_key_str(fptr, "EXTNAME", "RAW", "extension name", &status);
        fits_update_key_lng(fptr, "EXTVER", i + 1, "extension version number", &status);
        fits_write_img(fptr, datatype, 1, naxes[0] * naxes[1], data->buffer, &status);
        if (status != 0) {
#ifdef DEBUG
            fprintf(stderr, "%s %s %d --- fits_write_img error: %d\n", __FILE__, __func__, __LINE__ - 2, status);
#endif
        }
        
        if (options&DETECTOR_OPTION_NOTIFY_EACH_COMPLETION) {
            detector->_.d_proc.post_acquisition(detector, filename, fptr, string, format, rpc);
            fits_close_file(fptr, &status);
            if (status != 0) {
#ifdef DEBUG
                fprintf(stderr, "fits_close_file error: %d\n", status);
#endif
            }
            detector->_.d_proc.img_fptr = NULL;
        }
        free(data->buffer);
        free(data);
    }
    
    if (!(options&DETECTOR_OPTION_NOTIFY_EACH_COMPLETION)) {
        detector->_.d_proc.post_acquisition(detector, filename, fptr, string, format, rpc);
        fits_close_file(fptr, &status);
        if (status != 0) {
#ifdef DEBUG
            fprintf(stderr, "fits_close_file error: %d\n", status);
#endif
        }
        detector->_.d_proc.img_fptr = NULL;
    }
    
    return NULL;
}

int static
QHYCamera_expose_video(struct QHYCamera *self, double exposure_time, uint32_t n_frame, va_list *app)
{
    uint32_t (*C_SetQHYCCDParam_Exposure)(void *, double);
    uint32_t (*C_BeginQHYCCDLive)(void *);
    uint32_t (*C_GetQHYCCDLiveFrame)(void *, uint32_t *, uint32_t *, uint32_t *, uint32_t *, uint8_t *);
    uint32_t (*C_GetQHYCCDMemLength)(void *);
    uint32_t (*C_StopQHYCCDLive)(void *);
    void *rpc;
    char *string;
    uint32_t state;
    uint16_t options;
    int ret = AAOS_OK;
    uint32_t qhy_ret, width, height, bits_per_pixel, channel;
    uint32_t i;
    struct DetectorDataFrame *data;
    uint32_t data_length;
    void *retval;
    int format = 0;
    bool abort_flag = false;

    rpc = va_arg(*app, void *);
    string = va_arg(*app, char *);

    C_SetQHYCCDParam_Exposure = dlsym(self->dhl, "C_SetQHYCCDParam_Exposure");
    C_BeginQHYCCDLive = dlsym(self->dlh, "C_BeginQHYCCDLive");
    C_GetQHYCCDLiveFrame = dlsym(self->dlh, "C_GetQHYCCDLiveFrame");
    C_StopQHYCCDLive = dlsym(self->dlh, "C_StopQHYCCDLive");
    C_GetQHYCCDMemLength = dlsym(self->dlh, "C_GetQHYCCDMemLength");

    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = self->_.d_state.state;, 
    options = self->_.d_state.options;
    if ((state&DETECTOR_STATE_MALFUNCTION) && !(options&DETECTOR_OPTION_IGNORE_DEVMAL)) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        return AAOS_EDEVMAL;
    }
    
    if ((state&DETECTOR_STATE_EXPOSING) || (state&DETECTOR_STATE_READING)) {
        if (options&DETECTOR_OPTION_NOWAIT) {
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            return AAOS_EBUSY;
        }
        while ((state&DETECTOR_STATE_EXPOSING) || (state&DETECTOR_STATE_READING)) {
            Pthread_cond_wait(&self->_.d_state.cond, &self->_.d_state.mtx);
        }
    } else if (state&DETECTOR_STATE_OFFLINE) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        return AAOS_EPWROFF;
    } else if (state&DETECTOR_STATE_UNINITIALIZED) {
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        return AAOS_EUNINIT;
    }
    if ((data_length = C_GetQHYCCDMemLength(self->camera_handle)) == -1) {
#ifdef DEBUG
        fprintf(stderr, "%s %s %d --- C_GetQHYCCDMemLength error\n", __FILE__, __func__, __LINE__ - 2);
#endif
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        return AAOS_EDEVMAL;
    }
    if ((qhy_ret = C_SetQHYCCDParam_Exposure(self->camera_handle, exposure_time * 1000000.)) == -1) {
#ifdef DEBUG
        fprintf(stderr, "%s %s %d --- C_SetQHYCCDParam_Exposure error\n", __FILE__, __func__, __LINE__ - 2);
#endif
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        return AAOS_EDEVMAL;
    }
    self->_.d_param.exposure_time = exposure_time;
    if ((qhy_ret = C_BeginQHYCCDLive(self->camera_handle)) == -1) {
#ifdef DEBUG
        fprintf(stderr, "%s %s %d --- C_BeginQHYCCDLive error\n", __FILE__, __func__, __LINE__ - 2);
#endif
        Pthread_mutex_unlock(&self->_.d_state.mtx);
        return AAOS_EDEVMAL;
    }
    state = DETECTOR_STATE_EXPOSING;
    self->_.d_state.state = state&self->_.d_state.state;
    arg = (struct QHYCameraExposureArg *) Malloc(sizeof(struct QHYCameraExposureArg));
    arg->detector = self;
    arg->rpc = rpc;
    arg->string = string;
    arg->format = format;
    Pthread_create(&self->_.d_state.tid, NULL, QHYCamera_process_image_thr, NULL);
    Pthread_mutex_unlock(&self->_.d_state.mtx);

    data = (struct DetectorDataFrame *) Malloc(sizeof(struct DetectorDataFrame));
    Clock_gettime(CLOCK_REALETIME, &data->tp);
    data->buffer = NULL;
    data->i = 0;
    data->n = n_frame;
    data->width = self->_.d_param.image_width;
    data->height = self->_.d_param.image_height;
    data->pixel_format = self->_.d_param.pixel_format;
    threadsafe_queue_push(data);

    for (i = 0; i < n_frame; i++) {
        Pthread_mutex_lock(&self->_.d_exp.mtx);
        if (self->_.d_exp.stop_flag) {
            C_StopQHYCCDLive(self->camera_handle);
            self->_.d_exp.stop_flag = false;
            Pthread_cond_broadcast(&self->_.d_exp.cond);
            Pthread_mutex_unlock(&self->_.d_exp.mtx);
            ret = AAOS_ECANCELED;
            break;
        }
        Pthread_mutex_unlock(&self->_.d_exp.mtx);
        data = (struct DetectorDataFrame *) Malloc(sizeof(struct DetectorDataFrame));
        Clock_gettime(CLOCK_REALETIME, &data->tp);
        data->buffer = NULL;
        data->i = i;
        data->n = n_frame;
        data->width = self->_.d_param.image_width;
        data->height = self->_.d_param.image_height;
        data->pixel_format = self->_.d_param.pixel_format;
        data->buffer = Malloc(data_length);
        /*
        switch (data->pixel_format) {
            case DETECTOR_PIXEL_FORMAT_MONO_8:
                data->buffer = Malloc(sizeof(uint8_t) * data->width * data->height);
                break;
            case DETECTOR_PIXEL_FORMAT_MONO_16:
                data->buffer = Malloc(sizeof(uint8_t) * data->width * data->height * 2);
                break;
            case DETECTOR_PIXEL_FORMAT_RGB_24:
                data->buffer = Malloc(sizeof(uint8_t) * data->width * data->height * 3);
                break;
            default:
                break;
        }
        */
        threadsafe_queue_push(data);
        if ((qhy_ret = C_GetQHYCCDLiveFrame(self->camera_handle, &width, &height, &bits_per_pixel, &channel, self->buffer)) == -1) {
            free(data->buffer);
            free(data);
            Pthread_mutex_lock(&self->_.d_exp.mtx);
            if (self->_.d_exp.stop_flag) {
                self->_.d_exp.stop_flag = false;
                abort_flag = true;
                ret = AAOS_ECANCELED;
            } else {
                ret = AAOS_EDEVMAL;
            }
            Pthread_mutex_unlock(&self->_.d_exp.mtx);
            break;
            /*
            threadsafe_queue_push(NULL);
            Pthread_join(self->_.d_state.tid, retval);
            free(arg);
            Pthread_mutex_lock(&self->_.d_state.mtx);
            state = DETECTOR_STATE_IDLE;
            self->_.d_state.state &= state;
            Pthread_mutex_unlock(&self->_.d_state.mtx);
            Pthread_cond_broadcast(&self->_.d_state.cond);
            return AAOS_EDEVMAL;
            */
        }
        threadsafe_queue_push(data);
    }
    if (!abort_flag) {
        C_StopQHYCCDLive(self->camera_handle);
    }
    threadsafe_queue_push(NULL);
    Pthread_join(self->_.d_state.tid, &retval);
    free(arg);
    Pthread_mutex_lock(&self->_.d_state.mtx);
    state = DETECTOR_STATE_IDLE;
    self->_.d_state.state &= state;
    Pthread_mutex_unlock(&self->_.d_state.mtx);
    Pthread_cond_broadcast(&self->_.d_state.cond);

    return ret;
}

int static
QHYCamera_expose_multiframe(struct QHYCamera *self, double exposure_time, uint32_t n_frame, va_list *app)
{
    return AAOS_OK;
}


int static
QHYCamera_expose_snapshot(struct QHYCamera *self, double exposure_time, uint32_t n_frame, va_list *app)
{
    return AAOS_OK;
}

int static
QHYCamera_expose(void *_self, double exposure_time, uint32_t n_frame, va_list *app)
{
    struct QHYCamera *self = cast(QHYCamera(), _self);

    int capture_mode;

    capture_mode = self->capture_mode;

    switch (capture_mode) {
    case DETECTOR_CAPTURE_MODE_SNAPSHOT:
        /* code */
        return QHYCamera_expose_snapshot(self, exposuretime, n_frame, app);
        break;
    case DETECTOR_CAPTURE_MODE_VIDEO:
        return QHYCamera_expose_video(self, exposuretime, n_frame, app);
        break;
    case DETECTOR_CAPTUTE_MODE_MULTIFRAME:
        return QHYCamera_expose_multiframe(self, exposuretime, n_frame, app);
        break;
    default:
        break;
    }

    return AAOS_OK;
}

static const void *_qhy_camera_virtual_table;

static void
qhy_camera_virtual_table_destroy(void)
{
    delete((void *) _qhy_camera_virtual_table);
}

static void
qhy_camera_virtual_table_initialize(void)
{
    _qhy_camera_virtual_table = new(__DetectorVirtualTable(),
                                    __detector_init, "init", QHYCamera_init,
                                    __detector_info, "info", QHYCamera_info,
                                    __detector_status, "status", QHYCamera_status,
                                    __detector_expose, "expose", QHYCamera_expose,
                                    __detector_stop, "stop", QHYCamera_stop,
                                    __detector_abort, "abort", QHYCamera_abort,
                                    //__detector_power_on, "power_on", ASICamera_power_on,
                                    //__detector_power_off, "power_off", ASICamera_power_off,
                                    __detector_set_binning, "set_binning", QHYCamera_set_binning,
                                    __detector_get_binning, "get_binning", QHYCamera_get_binning,
                                    __detector_set_capture_mode, "set_capture_mode", QHYCamera_set_capture_mode,
                                    __detector_get_capture_mode, "get_capture_mode", QHYCamera_get_capture_mode,
                                    __detector_set_exposure_time, "set_exposure_time", QHYCamera_set_exposure_time,
                                    __detector_get_exposure_time, "get_exposure_time", QHYCamera_get_exposure_time,
                                    //__detector_set_frame_rate, "set_frame_rate", ASICamera_set_frame_rate,
                                    //__detector_get_frame_rate, "get_frame_rate", ASICamera_get_frame_rate,
                                    __detector_set_gain, "set_gain", QHYCamera_set_gain,
                                    __detector_get_gain, "get_gain", QHYCamera_get_gain,
                                    __detector_set_overscan, "set_overscan", QHYCamera_set_overscan,
                                    __detector_get_overscan, "get_overscan", QHYCamera_get_overscan,
                                    __detector_set_pixel_format, "set_pixel_format", QHYCamera_set_pixel_format,
                                    __detector_get_pixel_format, "get_pixel_format", QHYCamera_get_pixel_format,
                                    __detector_set_region, "set_region", QHYCamera_set_region,
                                    __detector_get_region, "get_region", QHYCamera_get_region,
                                    __detector_set_temperature, "set_temperature", QHYCamera_set_temperature,
                                    __detector_get_temperature, "get_temperature", QHYCamera_get_temperature,
                                    __detector_set_trigger_mode, "set_trigger_mode", QHYCamera_set_trigger_mode,
                                    __detector_get_trigger_mode, "get_trigger_mode", QHYCamera_get_trigger_mode,

                                    __detector_raw, "raw", QHYCamera_raw,
                                     //__detector_inspect, "inspect", USTCCamera_inspect,
                                    (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(qhy_camera_virtual_table_destroy);
#endif
}

static const void *
asi_camera_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, asi_camera_virtual_table_initialize);
#endif
    
    return _asi_camera_virtual_table;
}

#endif


#ifdef _USE_COMPILER_ATTRIBUTION_

static void __destructor__(void) __attribute__ ((destructor(_DETECTOR_PRIORITY_)));

static void
__destructor__(void)
{
    USTCCamera_destroy();
    USTCCameraClass_destroy();
    ustc_camera_virtual_table_destroy();
    
#ifdef __USE_QHY_CAMERA__
    QHYCamera_destroy();
    QHYCameraClass_destroy();
    qhy_camera_virtual_table_destroy();
#endif

#ifdef __USE_LEADING_CAMERA__
    LeadingCamera_destroy();
    LeadingCameraClass_destroy();
    leading_camera_virtual_table_destroy();
#endif
    
#ifdef __USE_ASI_CAMERA__
    ASICamera_destroy();
    ASICameraClass_destroy();
    asi_camera_virtual_table_destroy();
#endif
    
#ifdef __USE_ARAVIS__
    GenICam_destroy();
    GenICamClass_destroy();
    genicam_virtual_table_destroy();
#endif
    __Detector_destroy();
    __DetectorClass_destroy();
    __DetectorVirtualTable_destroy();
}

static void __constructor__(void) __attribute__ ((constructor(_DETECTOR_PRIORITY_)));

static void
__constructor__(void)
{
    __DetectorVirtualTable_initialize();
    __DetectorClass_initialize();
    __Detector_initialize();
    
    ustc_camera_virtual_table_initialize();
    USTCCameraClass_initialize();
    USTCCamera_initialize();

#ifdef __USE_ARAVIS__
    genicam_virtual_table_initialize();
    GenICamClass_initialize();
    GenICam_initialize();
#endif
    
#ifdef __USE_ASI_CAMERA__
    asi_camera_virtual_table_initialize();
    ASICameraClass_initialize();
    ASICamera_initialize();
#endif
    
#ifdef __USE_LEADING_CAMERA__
    leading_camera_virtual_table_initialize();
    LeadingCameraClass_initialize();
    LeadingCamera_initialize();
#endif
    
#ifdef __USE_QHY_CAMERA__
    qhy_camera_virtual_table_initialize();
    QHYCameraClass_initialize();
    QHYCamera_initialize();
#endif
}
#endif
