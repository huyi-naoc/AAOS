//
//  detector.h
//  AAOS
//
//  Created by huyi on 18/7/26.
//  Copyright © 2018年 National Astronomical Observatories, Chinese Academy of Sciences. All rights reserved.
//

#ifndef detector_h
#define detector_h

#include <stddef.h>
#include <stdint.h>

#include "object.h"
#include "virtual.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern const void *__Detector(void);
extern const void *__DetectorClass(void);
extern const void *__DetectorVirtualTable(void);

int __detector_status(void *_self, void *res, size_t res_size, size_t *res_len);
int __detector_info(void *_self, void *res, size_t res_size, size_t *res_len);
int __detector_abort(void *_self);
int __detector_stop(void *_self);
int __detector_power_on(void *_self);
int __detector_power_off(void *_self);
int __detector_enable_cooling(void *_self);
int __detector_disable_cooling(void *_self);
int __detector_init(void *_self);
int __detector_inspect(void *_self);
int __detector_register(void *_self, double timeout);
int __detector_wait(void *_self, double timeout);
int __detector_wait_for_completion(void *_self);

void __detector_set(void *_self, const char *keyname, ...);
void __detector_get(void *_self, const char *keyname, ...);
void __detector_set_option(void *_self, uint16_t option);
uint16_t __detector_get_option(void *_self);
void __detector_clear_option(void *_self);
void __detector_set_directory(void *_self, const char *directory);
void __detector_get_directory(void *_self, char *directory, size_t size);
void __detector_set_prefix(void *_self, const char *prefix);
void __detector_get_prefix(void *_self, char *prefix, size_t size);
void __detector_set_template(void *_self, const char *tpl);
void __detector_get_template(void *_self, char *tpl, size_t size);

void __detector_set_pre_acquisition(void *_self, int (*pre_acquisition)(void *, const char *, ...));
void __detector_set_post_acquisition(void *_self, int (*post_acquisition)(void *, const char *, ...));
void __detector_set_name_convention(void *_self, void (*name_convention)(void *, char *, size_t, ...));

int __detector_set_capture_mode(void *_self, uint32_t capture_mode);
int __detector_get_capture_mode(void *_self, uint32_t *capture_mode);
int __detector_set_trigger_mode(void *_self, uint32_t trigger_mode);
int __detector_get_trigger_mode(void *_self, uint32_t *trigger_mode);
int __detector_set_overscan(void *_self, uint32_t x_overscan, uint32_t y_overscan);
int __detector_get_overscan(void *_self, uint32_t *x_overscan, uint32_t *y_overscan);

int __detector_set_region(void *_self, uint32_t x_offset, uint32_t y_offset, uint32_t width, uint32_t height);
int __detector_get_region(void *_self, uint32_t *x_offset, uint32_t *y_offset, uint32_t *width, uint32_t *height);
int __detector_set_binning(void *_self, uint32_t x_binning, uint32_t y_binning);
int __detector_get_binning(void *_self, uint32_t *x_binning, uint32_t *y_binning);

int __detector_set_frame_rate(void *_self, double frame_rate);
int __detector_get_frame_rate(void *_self, double *frame_rate);
int __detector_set_exposure_time(void *_self, double exposure_time);
int __detector_get_exposure_time(void *_self, double *exposure_time);
int __detector_set_pixel_format(void *_self, uint32_t pixel_format);
int __detector_get_pixel_format(void *_self, uint32_t *pixel_format);
int __detector_set_readout_rate(void *_self, double readout_rate);
int __detector_get_readout_rate(void *_self, double *readout_rate);
int __detector_set_gain(void *_self, double gain);
int __detector_get_gain(void *_self, double *gain);
int __detector_set_temperature(void *_self, double temperature);
int __detector_get_temperature(void *_self, double *temperature);
int __detector_reload(void *_self);
int __detector_load(void *_self, ...);
int __detector_unload(void *_self);
int __detector_raw(void *_self, const void *write_buffer, size_t write_buffer_size, size_t *write_size, void *read_buffer, size_t read_buffer_size, size_t *read_size);
int __detector_expose(void *_self, double exposure_time, uint32_t n_frame, ...);

const char *__detector_get_name(const void *_self);

#ifdef __USE_ARAVIS__
extern const void *GenICam(void);
extern const void *GenICamClass(void);
#endif

extern const void *SitianCam(void);
extern const void *SitianCamClass(void);

extern const void *VirtualDetector(void);
extern const void *VirtualDetectorClass(void);

extern const void *USTCCamera(void);
extern const void *USTCCameraClass(void);

#ifdef __USE_ASI_CAMERA__
extern const void *ASICamera(void);
extern const void *ASICameraClass(void);
#endif

#ifdef __USE_LEADING_CAMERA__
extern const void *LeadingCamera(void);
extern const void *LeadingCameraClass(void);
#endif

#ifdef __USE_QHY_CAMERA__
extern const void *QHYCamera(void);
extern const void *QHYCameraClass(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* detector_h */
