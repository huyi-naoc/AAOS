//
//  detector_r.h
//  AAOS
//
//  Created by huyi on 18/7/26.
//  Copyright © 2018年 National Astronomical Observatories, Chinese Academy of Sciences. All rights reserved.
//

#ifndef _detector_r_h
#define _detector_r_h

#include "device_r.h"
#include "object_r.h"
#include "virtual_r.h"
#include <pthread.h>

//#define _DETECTOR_PRIORITY_ _VIRTUAL_PRIORITY_ + 1

struct DetectorState {
    uint32_t state;
    uint16_t options;
    pthread_mutex_t mtx;
    pthread_cond_t cond;
    pthread_t tid;
};

struct DetectorCapability {
    uint32_t width;               //full image width without binning, with overscan
    uint32_t height;              //full image height without binning, with overscan
    size_t x_n_chip;
    size_t y_n_chip;
    size_t n_chip;              //number of chips (number of MOSAIC chips, not number of readout channels)
    size_t n_channel;           //number of readout channels, overscan
    bool *flip_map;
    bool *mirror_map;
    
    bool binning_available;
    uint32_t x_binning_min;
    uint32_t x_binning_max;
    uint32_t *x_binning_array;
    uint32_t n_x_binning;
    uint32_t y_binning_min;
    uint32_t y_binning_max;
    uint32_t *y_binning_array;
    uint32_t n_y_binning;
    
    bool offset_available;
    uint32_t x_offset_min;
    uint32_t x_offset_max;
    uint32_t y_offset_min;
    uint32_t y_offset_max;
    uint32_t image_width_min;
    uint32_t image_width_max;
    uint32_t image_height_min;
    uint32_t image_height_max;
    
    bool frame_rate_available;
    double frame_rate_min; /* may change with ROI and binning */
    double frame_rate_max; /* may change with ROI and binning */
    double *frame_rate_array;
    size_t n_frame_rate;
    bool auto_frame_rate_available;
    double auto_frame_rate_min;
    double auto_frame_rate_max;
    
    bool exposure_time_available;
    double exposure_time_min; /* may change with ROI and binning */
    double exposure_time_max; /* may change with ROI and binning */
    double *exposure_time_array;
    size_t n_exposure_time;
    bool auto_exposure_time_available;
    double auto_exposure_time_min;
    double auto_exposure_time_max;
    
    bool gain_available;
    double gain_min;
    double gain_max;
    double *gain_array;
    size_t n_gain;
    bool auto_gain_available;
    double auto_gain_min;
    double auto_gain_max;

    bool pixel_format_available;
    uint32_t *pixel_format_array;
    size_t n_pixel_format;
    
    bool readout_rate_available;
    double readout_rate_min;
    double readout_rate_max;
    double *readout_rate_array;
    size_t n_readout_rate;
    bool auto_readout_rate_available;
    double auto_readout_rate_min;
    double auto_readout_rate_max;

    bool cooling_available;
    double cooling_temperature_min;
    double cooling_temperature_max;
    bool auto_cooling_available;
    double auto_cooling_temperature_min;
    double auto_cooling_temperature_max;
    
    bool trigger_available;
    uint32_t *trigger_mode_array;
    size_t n_trigger_mode;

    bool capture_mode_available;
    uint32_t *capture_mode_array;
    size_t n_capture_mode;

    bool overscan_available;
    uint32_t x_overscan_min;
    uint32_t x_overscan_max;
    uint32_t *x_overscan_array;
    size_t n_x_overscan;
    uint32_t y_overscan_min;
    uint32_t y_overscan_max;
    uint32_t *y_overscan_array;
    size_t n_y_overscan;
};

struct DetectorParameter {
    uint32_t x_binning;           //current horizontal binning
    uint32_t y_binning;           //current vertical binning
    uint32_t x_offset;            //current horizontal offset
    uint32_t y_offset;            //current vertical offset
    uint32_t image_width;         //current image width
    uint32_t image_height;        //current image height
    uint32_t pixel_format;        //current image pixel format
    uint32_t trigger_mode;        //current trigger mode
    uint32_t capture_mode;
    uint32_t x_overscan;
    uint32_t y_overscan;
    double gain;                //current gain
    double frame_rate;          //current frame rate
    double exposure_time;       //curret exposure time
    double readout_rate;        //current read out speed
    double temperature;         //current setting temperature
    bool auto_gain_enable;
    bool auto_exposure_time_enable;
    bool auto_frame_rate_enable;
    bool auto_readout_rate_enable;
    bool is_cooling_enable;
    bool auto_cooling_enable;
    pthread_rwlock_t rwlock;
};

struct DetectorExposureControl {
    int exposure_mode;
    uint32_t success_frames;
    uint32_t request_frames;
    uint32_t count;
    pthread_mutex_t mtx;
    pthread_cond_t cond;
    void *rpc;
    char *extra_data;
    unsigned int extra_data_format;
    bool stop_flag;
    bool notify_last_frame_filling;
    bool last_frame_filling_flag;
    bool notify_each_frame_done;        /* each frame data save as a single fits file */
};

struct DetectorFrameProcess {
    char *image_prefix;
    char *image_directory;
    void *tpl_fptr;
    char *tpl_filename;
    void *queue;
    void *img_fptr;
    char *img_filename;
    int (*pre_acquisition)(void *, const char *, ...);
    int (*post_acquisition)(void *, const char *, ...);
    void (*name_convention)(void *, char *, size_t, ...);
};

struct __Detector {
    struct Object _;
    const void *_vtab;
    char *name;
    char *description;
    unsigned int data_format;
    struct DetectorState d_state;
    struct DetectorCapability d_cap;
    struct DetectorParameter d_param;
    struct DetectorExposureControl d_exp;
    struct DetectorFrameProcess d_proc;
};

struct __DetectorClass {
    struct Class _;
    /*
     * Camera controlling methods, virtual or pure virtual methods.
     */
    struct Method abort;
    struct Method disable_cooling;
    struct Method enable_cooling;
    struct Method expose;
    struct Method get;
    struct Method get_binning;
    struct Method get_capture_mode;
    struct Method get_exposure_time;
    struct Method get_gain;
    struct Method get_overscan;
    struct Method get_pixel_format;
    struct Method get_readout_rate;
    struct Method get_region;
    struct Method get_temperature;
    struct Method get_trigger_mode;
    struct Method info;
    struct Method init;
    struct Method inspect;
    struct Method load;
    struct Method power_on;
    struct Method power_off;
    struct Method raw;
    struct Method reg;
    struct Method reload;
    struct Method set;
    struct Method set_binning;
    struct Method set_capture_mode;
    struct Method set_exposure_time;
    struct Method set_frame_rate;
    struct Method get_frame_rate;
    struct Method set_gain;
    struct Method set_overscan; 
    struct Method set_pixel_format;
    struct Method set_readout_rate;
    struct Method set_region;
    struct Method set_temperature;
    struct Method set_trigger_mode;
    struct Method status;
    struct Method stop;
    struct Method unload;
    struct Method wait;
    struct Method wait_for_completion;
    /*
     * image related methods, usually not virtual methods.
     */
    struct Method clear_option;
    struct Method get_directory;
    struct Method get_option;
    struct Method get_name;
    struct Method get_prefix;
    struct Method get_template;
    struct Method set_directory;
    struct Method set_name_convention;
    struct Method set_option;
    struct Method set_pre_acquisition;
    struct Method set_prefix;
    struct Method set_post_acquisition;
    struct Method set_template;
    
};

struct __DetectorVirtualTable {
    struct VirtualTable _;
    struct Method abort;
    struct Method disable_cooling;
    struct Method enable_cooling;
    struct Method expose;
    struct Method get;
    struct Method get_binning;
    struct Method get_capture_mode;
    struct Method get_exposure_time;
    struct Method get_gain;
    struct Method get_overscan;
    struct Method get_pixel_format;
    struct Method get_readout_rate;
    struct Method get_region;
    struct Method get_temperature;
    struct Method get_trigger_mode;
    struct Method info;
    struct Method init;
    struct Method inspect;
    struct Method load;
    struct Method power_on;
    struct Method power_off;
    struct Method raw;
    struct Method reg;
    struct Method reload;
    struct Method set;
    struct Method set_binning;
    struct Method set_capture_mode;
    struct Method set_exposure_time;
    struct Method set_frame_rate;
    struct Method get_frame_rate;
    struct Method set_gain;
    struct Method set_overscan;
    struct Method set_pixel_format;
    struct Method set_readout_rate;
    struct Method set_region;
    struct Method set_temperature;
    struct Method set_trigger_mode;
    struct Method status;
    struct Method stop;
    struct Method unload;
    struct Method wait;
    struct Method wait_for_completion;
    struct Method set_directory;
    struct Method set_prefix;
};

struct GenICam {
    struct __Detector _;
    void *camera;           /* Aravis's camera object */
    void *stream;           /* Aravis's stream object */
    char *name;             /* connection name */
    void *callback_param;   
    size_t n_buffers;       /* number of buffers when create a stream; */
};

struct GenICamClass {
    struct __DetectorClass _;
};

struct AndorCMOS {
    struct __Detector _;
    void *at_handle;
    char *name;
};

struct AndorCMOSClass {
    struct __DetectorClass _;
};

struct SitianCam {
    struct __Detector _;
    void *camera;
    char *port;
    char *address;
};

struct SitianCamClass {
    struct __DetectorClass _;
};

struct VirtualDetector {
    struct __Detector _;
};

struct VirtualDetectorClass {
    struct __DetectorClass _;
};

struct USTCCamera {
    struct __Detector _;
    unsigned int log_level;
    unsigned int which;
    pthread_mutex_t mtx;
    pthread_cond_t cond;
    char *so_path;
	void *dlh;
    size_t erase_count;
};

struct USTCCameraClass {
    struct __DetectorClass _;
};

#ifdef __USE_ASI_CAMERA__
struct ASICamera {
    struct __Detector _;
    int camera_index;
    int camera_id;
    pthread_mutex_t mtx;
    pthread_cond_t cond;
    char *name;
    char *so_path;
    void *dlh;
    int capture_mode;
};

struct ASICameraClass {
    struct __DetectorClass _;
};
#endif

#ifdef __USE_LEADING_CAMERA__

struct LeadingCameraInfo {
    char ip[16];
    char mask[16];
    char gateway[16];
    char description[132];
    
    char model_name[32];
    char device_version[32];
    char manufacture_spec_info[48];
    char serial_number[16];
    char user_define_name[16];
};

struct LeadingCamera {
    struct __Detector _;
    int camera_index;
    int camera_id;
    struct LeadingCameraInfo camera_info;
    void **device_list;
    void **interface_list;
    size_t n_camera;
    pthread_mutex_t mtx;
    pthread_cond_t cond;
    char *so_path;
    void *dlh;
};

struct LeadingCameraClass {
    struct __DetectorClass _;
};
#endif

#ifdef __USE_QHY_CAMERA__

struct QHYCamera {
    struct __Detector _;
    int camera_index;
    char camera_id[40];
    void *camera_handle;
    char *so_path;
    void *dlh;
    int capture_mode;
};

struct QHYCameraClass {
    struct __DetectorClass _;
};

#endif

#endif /* detector_r_h */
