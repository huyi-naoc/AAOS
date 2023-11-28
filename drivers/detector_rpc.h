//
//  detector_rpc.h
//  AAOS
//
//  Created by Hu Yi on 2021/11/16.
//  Copyright © 2021 NAOC. All rights reserved.
//

#ifndef detector_rpc_h
#define detector_rpc_h

/**
 * @file detector_rpc.h
 */

#include "rpc.h"
#include <stddef.h>
#include <stdint.h>

#define DETECTOR_COMMAND_RAW                    1
#define DETECTOR_COMMAND_EXPOSE                 2
#define DETECTOR_COMMAND_STATUS                 3
#define DETECTOR_COMMAND_POWER_ON               4
#define DETECTOR_COMMAND_POWER_OFF              5
#define DETECTOR_COMMAND_LOAD                   6
#define DETECTOR_COMMAND_RELOAD                 7
#define DETECTOR_COMMAND_UNLOAD                 8
#define DETECTOR_COMMAND_WAIT_FOR_COMPLETION    9

#define DETECTOR_COMMAND_SET_OPTION             10
#define DETECTOR_COMMAND_GET_OPTION             11
#define DETECTOR_COMMAND_CLEAR_OPTION           12
#define DETECTOR_COMMAND_SET_BINNING            13
#define DETECTOR_COMMAND_GET_BINNING            14
#define DETECTOR_COMMAND_SET_EXPOSURE_TIME      15
#define DETECTOR_COMMAND_GET_EXPOSURE_TIME      16
#define DETECTOR_COMMAND_SET_FRAME_RATE         17
#define DETECTOR_COMMAND_GET_FRAME_RATE         18
#define DETECTOR_COMMAND_SET_GAIN               19
#define DETECTOR_COMMAND_GET_GAIN               20
#define DETECTOR_COMMAND_SET_READOUT_RATE       21
#define DETECTOR_COMMAND_GET_READOUT_RATE       22
#define DETECTOR_COMMAND_SET_REGION             23
#define DETECTOR_COMMAND_GET_REGION             24
#define DETECTOR_COMMAND_SET_TEMPERATURE        25
#define DETECTOR_COMMAND_GET_TEMPERATURE        26

#define DETECTOR_COMMAND_SET_DIRECTORY          27
#define DETECTOR_COMMAND_GET_DIRECTORY          28
#define DETECTOR_COMMAND_SET_PREFIX             29
#define DETECTOR_COMMAND_GET_PREFIX             30
#define DETECTOR_COMMAND_SET_TEMPLATE           31
#define DETECTOR_COMMAND_GET_TEMPLATE           32

#define DETECTOR_COMMAND_INFO                   33
#define DETECTOR_COMMAND_STOP                   34
#define DETECTOR_COMMAND_ABORT                  35

#define DETECTOR_COMMAND_DELETE_IMAGE           36
#define DETECTOR_COMMAND_DELETE_ALL_IMAGE       37
#define DETECTOR_COMMAND_LIST_IMAGE             38
#define DETECTOR_COMMAND_GET_IMAGE              39

#define DETECTOR_COMMAND_COOLING_ENABLE         40
#define DETECTOR_COMMAND_COOLING_DISABLE        41

#define DETECTOR_COMMAND_GET_INDEX_BY_NAME      42

/**
 * Expose method of detector object.
 * @details Expose. If the detector is in \b DETECTOR_STATE_EXPOSING or \b DETECTOR_STATE_READING state, this method will wait until the detector is in a state other than \b EXPOSING or \b READING. Expose method will change the frame rate, if setting frame rate is available for the detector and the current frame length ( 1 / \b frame_rate ) is shorter than \b exposure_time. The setting of frame rate by this method will be preserved for the successive calls. While this method is executing, it will change the detector in the state of \b EXPOSING or \b READING. After it returns with \b AAOS_OK, it will set the state of the detector to \b DETECTOR_STATE_READING  or \b DETECTOR_STATE_IDLE, depending opon whether the option of \b DETECTOR_OPTION_NOTIFY_LAST_FILLING is set or not. When expose is started to execute, it will open and close the shutter at proper time points if the detector has a physical or electronic shutter.
 * @param[in,out] _self detector object.
 * @param[in] exposure_time exposure time in seconds.
 * @param[in] n_frames number of frames.
 * @param[in] image_callback callback function. If \b image_callback is \b NULL, print the image filename(s) to the standard output, else image_callback will be invoked after each frame is completed.
 * @retval AAOS_OK
 * No errors.
 * @retval AAOS_EINVAL
 * Input parameter \b exposure_time and/or \b n_frames is out of range.
 * @retval AAOS_EINTER
 * Exposing is stopped or abort before all  the requested frames are completed.
 * @retval AAOS_EDEVMAL
 * Detector is in MALFUNCTION state.
 */
int detector_expose(void *_self, double exposure_time, uint32_t n_frames, void (*image_callback)(void *_self, const char *filename, ...), ...);

/**
 * Wait for last frame completion method of detector object.
 * @details If the option of \b DETECTOR_OPTION_NOTIFY_LAST_FILLING is not set, it will return immediately. Otherwise, if the detector is in \b DETECTOR_STATE_READING, it will wait for a frame completed, and then set the detector to \b DETECTOR_STATE_IDLE.
 * @param[in,out] _self detector object.
 * @param[in] image_callback callback function. If \b image_callback is \b NULL, print the image filename(s) to the standard output, else image_callback will be invoked after the last frame is completed.
 * @retval AAOS_OK
 * No errors.
 * @retval AAOS_EALREADY
 * No frame is to be waited.
 * @retval AAOS_ENOTSUP
 * Wait for last frame completion is not supported by the specific detector.
 * @retval AAOS_EINTER
 * Exposing is stopped or abort before the last frame are completed.
 * @retval AAOS_EDEVMAL
 * Detector is in MALFUNCTION state.
 */
int detector_wait_for_completion(void *_self, void (*image_callback)(void *_self, const char *filename, ...));

/**
 * Abort exposing method of detector object.
 * @details If the detector is not in \b DETECTOR_STATE_EXPOSING or \b DETECTOR_STATE_READING, abort method will do nothing and return \b AAOS_OK. Otherwise, it will abort the current frame, and no files will be generated, meanwhile an expose or wait_for_completion call by another thread or process will return immediately with a return value of \b AAOS_EINTER.
 * @param[in,out] _self detector object.
 * @retval AAOS_OK
 * No errors.
 * @retval AAOS_ENOTSUP
 * Abort exposing method is not supported by the underline detector.
 */
int detector_abort(void *_self);

/**
 * Stop exposing method of detector object.
 * @param[in,out] _self detector object.
 * @retval AAOS_OK
 * No errors.
 * @retval AAOS_ENOTSUP
 * Stop exposing method is not supported by the underline detector.
 */
int detector_stop(void *_self);

/**
 * Staus method of detector object.
 * @param[in,out] _self detector object.
 * @param[in] res a pointer to restore the result.
 * @param[in] res_size size of \b res.
 * @param[in] res_len data length of \b res. If \b res_len is \b NULL, do nothing.
 * @retval AAOS_OK
 * No errors.
 * @retval AAOS_EAAOS_EDEVMAL
 * Detector is in MALFUNCTION state.
 */
int detector_status(void *_self, void *res, size_t res_size, size_t *res_len);

/**
 * Staus method of detector object.
 * @param[in,out] _self detector object.
 * @param[in] res a pointer to restore the result.
 * @param[in] res_size size of \b res.
 * @param[in] res_len data length of \b res. If \b res_len is \b NULL, do nothing.
 * @retval AAOS_OK
 * No errors.
 * @retval AAOS_EAAOS_EDEVMAL
 * Detector is in MALFUNCTION state.
 */
int detector_info(void *_self, void *res, size_t res_size, size_t *res_len);

/**
 * Power on method of detector object.
 * @param[in,out] _self detector object.
 * @retval AAOS_OK
 * No errors.
 * @retval AAOS_EDEVMAL
 * Detector is in MALFUNCTION state.
 * @retval AAOS_ENOTSUP
 * Power on method is not supported by the underline detector.
 */
int detector_power_on(void *_self);

/**
 * Power off method of detector object.
 * @param[in,out] _self detector object.
 * @retval AAOS_OK
 * No errors.
 * @retval AAOS_EDEVMAL
 * Detector is in MALFUNCTION state.
 * @retval AAOS_ENOTSUP
 * Power off method is not supported by the underline detector.
 */
int detector_power_off(void *_self);

/**
 * Enable cooling method of detector object.
 * @param[in,out] _self detector object.
 * @retval AAOS_OK
 * No errors.
 * @retval AAOS_EDEVMAL
 * Detector is in MALFUNCTION state.
 * @retval AAOS_ENOTSUP
 * Enable cooling method is not supported by the underline detector.
 */
int detector_cooling_enable(void *_self);

/**
 * Disable cooling method of detector object.
 * @param[in,out] _self detector object.
 * @retval AAOS_OK
 * No errors.
 * @retval AAOS_EDEVMAL
 * Detector is in MALFUNCTION state.
 * @retval AAOS_ENOTSUP
 * Disable coolingmethod is not supported by the underline detector.
 */
int detector_cooling_disable(void *_self);

/**
 * Set binning method of detector object.
 * @details Set the binning of a detector. If the detector is in \b EXPOSING or \b READING state, this method will wait until the detector is in a state other than \b EXPOSING or \b READING .
 * @param[in,out] _self detector object.
 * @param[in] x_binning binning value for horizontal direction.
 * @param[in] y_binning binning value for virtical direction.
 * @retval AAOS_OK
 * No errors.
 * @retval AAOS_EINVAL
 * Input parameter \b x_binning and/or \b y_binning is out of range.
 * @retval AAOS_EDEVMAL
 * Detector is in MALFUNCTION state.
 * @retval AAOS_ENOTSUP
 * Set binning method is not supported by the underline detector.
 */
int detector_set_binning(void *_self, uint32_t x_binning, uint32_t y_binning);

/**
 * Get binning method of detector object.
 * @param[in,out] _self detector object.
 * @param[out] x_binning binning value for horizontal direction.
 * @param[out] y_binning binning value for virtical direction.
 * @retval AAOS_OK
 * No errors.
 * @retval AAOS_EDEVMAL
 * Detector is in MALFUNCTION state.
 * @retval AAOS_ENOTSUP
 * Get binning method is not supported by the underline detector.
 */
int detector_get_binning(void *_self, uint32_t *x_binning, uint32_t *y_binning);

/**
 * Get binning method of detector object.
 * @param[in,out] _self detector object.
 * @param[out] x_binning binning value for horizontal direction.
 * @param[out] y_binning binning value for virtical direction.
 * @retval AAOS_OK
 * No errors.
 * @retval AAOS_EDEVMAL
 * Detector is in MALFUNCTION state.
 * @retval AAOS_ENOTSUP
 * Get binning method is not supported by the underline detector.
 */
int detector_get_binning(void *_self, uint32_t *x_binning, uint32_t *y_binning);

/**
 * Set exposure time method of detector object.
 * @param[in,out] _self detector object.
 * @param[in] exposure_time  exposure time in seconds.
 * @retval AAOS_OK
 * No errors.
 * @retval AAOS_EINVAL
 * Input parameter \b exposure_time is out of range.
 * @retval AAOS_EDEVMAL
 * Detector is in MALFUNCTION state.
 * @retval AAOS_ENOTSUP
 * Set exposure time  method is not supported by the underline detector.
 */
int detector_set_exposure_time(void *_self, double exposure_time);

/**
 * Get exposure time method of detector object.
 * @param[in,out] _self detector object.
 * @param[out] exposure_time  exposure time in seconds.
 * @retval AAOS_OK
 * No errors.
 * @retval AAOS_EDEVMAL
 * Detector is in MALFUNCTION state.
 * @retval AAOS_ENOTSUP
 * Set exposure time  method is not supported by the underline detector.
 */
int detector_get_exposure_time(void *_self, double *exposure_time);

/**
 * Set frame rate method of detector object.
 * @param[in,out] _self detector object.
 * @param[in] frame_rate  frame rate in Hz.
 * @retval AAOS_OK
 * No errors.
 * @retval AAOS_EINVAL
 * Input parameter \b frame_rate is out of range.
 * @retval AAOS_EDEVMAL
 * Detector is in MALFUNCTION state.
 * @retval AAOS_ENOTSUP
 * Set frame rate method is not supported by the underline detector.
 */
int detector_set_frame_rate(void *_self, double frame_rate);

/**
 * Get frame rate method of detector object.
 * @param[in,out] _self detector object.
 * @param[out] frame_rate frame rate in Hz.
 * @retval AAOS_OK
 * No errors.
 * @retval AAOS_EDEVMAL
 * Detector is in MALFUNCTION state.
 * @retval AAOS_ENOTSUP
 * Get frame rate method is not supported by the underline detector.
 */
int detector_get_frame_rate(void *_self, double *frame_rate);

/**
 * Set gain method of detector object.
 * @param[in,out] _self detector object.
 * @param[in] gain  gain in e-/ADU.
 * @retval AAOS_OK
 * No errors.
 * @retval AAOS_EINVAL
 * Input parameter \b gain is out of range.
 * @retval AAOS_EDEVMAL
 * Detector is in MALFUNCTION state.
 * @retval AAOS_ENOTSUP
 * Set gain method is not supported by the underline detector.
 */
int detector_set_gain(void *_self, double gain);

/**
 * Get gain method of detector object.
 * @param[in,out] _self detector object.
 * @param[out] gain  gain in e-/ADU.
 * @retval AAOS_OK
 * No errors.
 * @retval AAOS_EDEVMAL
 * Detector is in MALFUNCTION state.
 * @retval AAOS_ENOTSUP
 * Get gain  method is not supported by the underline detector.
 */
int detector_get_gain(void *_self, double *gain);

/**
 * Set readout rate method of detector object.
 * @param[in,out] _self detector object.
 * @param[in] readout_rate  gain in kHz.
 * @retval AAOS_OK
 * No errors.
 * @retval AAOS_EINVAL
 * Input parameter \b readout_rate is out of range.
 * @retval AAOS_EDEVMAL
 * Detector is in MALFUNCTION state.
 * @retval AAOS_ENOTSUP
 * Set gain method is not supported by the underline detector.
 */
int detector_set_readout_rate(void *_self, double readout_rate);

/**
 * Get readout rate method of detector object.
 * @param[in,out] _self detector object.
 * @param[out] readout_rate  readout rate in kHz.
 * @retval AAOS_OK
 * No errors.
 * @retval AAOS_EDEVMAL
 * Detector is in MALFUNCTION state.
 * @retval AAOS_ENOTSUP
 * Get gain  method is not supported by the underline detector.
 */
int detector_get_readout_rate(void *_self, double *readout_rate);

/**
 * Set temperature method of detector object.
 * @param[in,out] _self detector object.
 * @param[in] temperature  temperature in celsius degree.
 * @retval AAOS_OK
 * No errors.
 * @retval AAOS_EINVAL
 * Input parameter \b temperature is out of range.
 * @retval AAOS_EDEVMAL
 * Detector is in MALFUNCTION state.
 * @retval AAOS_ENOTSUP
 * Set temperature method is not supported by the underline detector.
 */
int detector_set_temperature(void *_self, double temperature);

/**
 * Get readout rate method of detector object.
 * @param[in,out] _self detector object.
 * @param[out] temperature temperature in celsius degree.
 * @retval AAOS_OK
 * No errors.
 * @retval AAOS_EDEVMAL
 * Detector is in MALFUNCTION state.
 * @retval AAOS_ENOTSUP
 * Get temperature method is not supported by the underline detector.
 */
int detector_get_temperature(void *_self, double *temperature);

/**
 * Set I/O region method of detector object.
 * @param[in,out] _self detector object.
 * @param[in] x_offset  Horizontal offset in pixel.
 * @param[in] y_offset  Vertical offset in pixel.
 * @param[in] width  Image width in pixel.
 * @param[in] height  Image height in pixel.
 * @retval AAOS_OK
 * No errors.
 * @retval AAOS_EINVAL
 * Input parameter \b x_offset , \b y_offset , \b width and/or \b height is out of range.
 * @retval AAOS_EDEVMAL
 * Detector is in MALFUNCTION state.
 * @retval AAOS_ENOTSUP
 * Set I/O region method is not supported by the underline detector.
 */
int detector_set_region(void *_self, uint32_t x_offset, uint32_t y_offset, uint32_t width, uint32_t height);

/**
 * Get I/O region method of detector object.
 * @param[in,out] _self detector object.
 * @param[out] x_offset  Horizontal offset in pixel.
 * @param[out] y_offset  Vertical offset in pixel.
 * @param[out] width  Image width in pixel.
 * @param[out] height  Image height in pixel.
 * @retval AAOS_OK
 * No errors.
 * @retval AAOS_EDEVMAL
 * Detector is in MALFUNCTION state.
 * @retval AAOS_ENOTSUP
 * Get I/O region method is not supported by the underline detector.
 */
int detector_get_region(void *_self, uint32_t *x_offset, uint32_t *y_offset, uint32_t *width, uint32_t *height);

/**
 * Set the prefix of the images method of detector object.
 * @param[in,out] _self detector object.
 * @param[in] prefix Image name prefix.
 * @retval AAOS_OK
 * No errors.
 */
int detector_set_prefix(void *_self, const char *prefix);

/**
 * Get the prefix of the images method of detector object.
 * @param[in,out] _self detector object.
 * @param[out] prefix Image name prefix.
 * @param[in] size size of \b prefix.
 * @retval AAOS_OK
 * No errors.
 */
int detector_get_prefix(void *_self, char *prefix, size_t size);

/**
 * Set the directory of the images to store method of detector object.
 * @param[in,out] _self detector object.
 * @param[in] directory Image name prefix.
 * @retval AAOS_OK
 * No errors.
 */
int detector_set_directory(void *_self, const char *directory);

/**
 * Get the directory of the images to store method of detector object.
 * @param[in,out] _self detector object.
 * @param[out] directory Image name prefix.
 * @param[in] size size of \b prefix.
 * @retval AAOS_OK
 * No errors.
 */
int detector_get_directory(void *_self, char *directory, size_t size);

/**
 * Set the template file name method of detector object.
 * @param[in,out] _self detector object.
 * @param[in] tpl name of the template file.
 * @retval AAOS_OK
 * No errors.
 */
int detector_set_template(void *_self, const char *tpl);

/**
 * Get the template file name method of detector object.
 * @param[in,out] _self detector object.
 * @param[out] tpl name of the template file.
 * @param[in] size size of \b tpl.
 * @retval AAOS_OK
 * No errors.
 */
int detector_get_template(void *_self, char *tpl, size_t size);

/**
 * Set options method of detector object.
 * @param[in,out] _self detector object.
 * @param[in] option name of the template file.
 * @retval AAOS_OK
 * No errors.
 */
int detector_set_option(void *_self, uint16_t option);

/**
 * Get option method of detector object.
 * @param[in,out] _self detector object.
 * @param[out] option name of the template file.
 * @retval AAOS_OK
 * No errors.
 */
int detector_get_option(void *_self, uint16_t *option);

/**
 * Clear option method of detector object.
 * @param[in,out] _self detector object.
 * @retval AAOS_OK
 * No errors.
 */
int detector_clear_option(void *_self);

/**
 * Reload method of detector object.
 * @param[in,out] _self detector object.
 * @retval AAOS_OK
 * No errors.
 * @retval AAOS_ENOTSUP
 * Reload method is not supported by the specific detector.
 */
int detector_reload(void *_self);

/**
 * Unload method of detector object.
 * @param[in,out] _self detector object.
 * @retval AAOS_OK
 * No errors.
 * @retval AAOS_ENOTSUP
 * Unload method is not supported by the specific detector.
 */
int detector_unload(void *_self);

/**
 * Load method of detector object.
 * @param[in,out] _self detector object.
 * @retval AAOS_OK
 * No errors.
 * @retval AAOS_ENOTSUP
 * Load method is not supported by the specific detector.
 */
int detector_load(void *_self, ...);

/**
 * Delete image method of detector object.
 * @param[in,out] _self detector object.
 * @param[in] filename image file name
 * @retval AAOS_OK
 * No errors.
 * @retval AAOS_EEXIST
 * The file \b filename doest not exist in the image directory.
 * @retval AAOS_ENOTSUP
 * Delete method is not supported by the underline detector.
 */
int detector_delete_image(void *_self, const char *filename);

/**
 * Delete all the images method.
 * @param[in,out] _self detector object.
 * @retval AAOS_OK
 * No errors.
 * @retval AAOS_ENOTSUP
 * Delete method is not supported by the underline detector.
 */
int detector_delete_all_image(void *_self);

/**
 * Get image method of detector object.
 * @param[in,out] _self detector object.
 * @param[in] filename image file name
 * @retval AAOS_OK
 * No errors.
 * @retval AAOS_EEXIST
 * The file \b filename doest not exist in the image directory.
 * @retval AAOS_ENOTSUP
 * Delete method is not supported by the underline detector.
 */
int detector_get_image(void *_self, const char *filename);

/**
 * List all the images method.
 * @details List all the images in the image directory except template files. Only files whose filename has a prefix of get_prefix, a suffix of ".fits" or ".fits.fz" are list. Filenames in the filelist are delimited by a newline character.
 * @param[in,out] _self detector object.
 * @param[in,out] filelist filename list
 * @param[in] size Size of filelist.
 * @retval AAOS_OK
 * No errors.
 * @retval AAOS_ENOTSUP
 * Delete method is not supported by the underline detector.
 */
int detector_list_image(void *_self, char *filelist, size_t size);

/**
 * Raw method of detector object.
 * @details Send a raw command to the detector. Different camera has different set of raw commands, see the detector's SDK manual or communiication protocol. This method is intended to provide an expert to finely control the detector. Raw method may change the state of the detector silently, which causes the state of the detector inconsistency.
 * @param[in,out] _self detector object.
 * @param[in] cmd raw command string.
 * @param[in] cmd_size command string size.
 * @param[in] res a pointer to restore the result.
 * @param[in] res_size size of \b res.
 * @param[in] res_len data length of \b res. If \b res_len is \b NULL, do nothing.
 * @retval AAOS_OK
 * No errors.
 * @retval AAOS_EBADCMD
 * \b cmd is wrong.
 * @retval AAOS_EAAOS_EDEVMAL
 * Detector is in MALFUNCTION state.
 * @retval AAOS_ENOTSUP
 * Raw method is not supported by the specific detector..
 */
int detector_raw(void *_self, const void *cmd, size_t cmd_size, void *res, size_t res_size, size_t *res_len);

/**
 * Get index  method of detector object.
 * @param[in,out] _self detector object.
 * @param[in] name detector name.
 * @retval AAOS_OK
 * No errors.
 * @retval AAOS_ENOTDOUND
 * Detector \b name is not found.
 */
int detector_get_index_by_name(void *_self, const char *name);

const void *Detector(void);
const void *DetectorClass(void);

extern const void *DetectorClient(void);
extern const void *DetectorClientClass(void);

extern const void *DetectorServer(void);
extern const void *DetectorServerClass(void);

void **detectors;
size_t n_detector;
void *detector_list;

#endif /* detector_rpc_h */
