//
//  detector_r.h
//  AAOS
//
//  Created by huyi on 18/7/26.
//  Copyright © 2018年 National Astronomical Observatories, Chinese Academy of Sciences. All rights reserved.
//

#ifndef _detector_r_h
#define _detector_r_h

#include "object_r.h"
#include "virtual_r.h"
#include <pthread.h>

#define _DETECTOR_PRIORITY_ _VIRTUAL_PRIORITY_ + 1

struct DetectorState {
    uint32_t state;
    uint16_t options;
    pthread_mutex_t mtx;
    pthread_cond_t cond;
    pthread_t tid;
};

struct DetectorCapability {
    size_t width;               //full image width without binning, with overscan
    size_t height;              //full image height without binning, with overscan
    
    bool binning_available;
    size_t x_binning_min;
    size_t x_binning_max;
    size_t *x_binning_array;
    size_t n_x_binning;
    size_t y_binning_min;
    size_t y_binning_max;
    size_t *y_binning_array;
    size_t n_y_binning;
    
    bool offset_available;
    size_t x_offset_min;
    size_t x_offset_max;
    size_t y_offset_min;
    size_t y_offset_max;
    size_t image_width_min;
    size_t image_width_max;
    size_t image_height_min;
    size_t image_height_max;
    
    bool frame_rate_available;
    double frame_rate_min; /* changing with ROI and binning */
    double frame_rate_max; /* changing with ROI and binning */
    
    bool exposure_time_available;
    double exposure_time_min; /* changing with frame rate */
    double exposure_time_max; /* changing with frame rate */
    
    bool gain_available;
    double gain_min;
    double gain_max;
    double *gain_array;
    size_t n_gain;
    
    bool readout_rate_available;
    double readout_rate_min;
    double readout_rate_max;
    double *readout_rate_array;
    size_t n_readout;
    
    bool cooling_available;
    double cooling_temperature_min;
    double colling_temperature_max;
};

struct DetectorParameter {
    size_t x_binning;       //current horizontal binning
    size_t y_binning;       //current vertical binning
    size_t x_offset;        //current horizontal offset
    size_t y_offset;        //current vertical offset
    size_t image_width;     //current image width
    size_t image_height;    //current image height
    int pixel_format;       //current image format;
    double gain;            //current ga
    double frame_rate;      //current frame rate
    double exposure_time;   //curret exposure time
    double readout_rate;    //current read out speed
    double temperature;     //setting temperature
};

struct DetectorExposureControl {
    int exposure_mode;
    size_t success_frames;
    size_t request_frames;
    size_t count;
    pthread_mutex_t mtx;
    pthread_cond_t cond;
    int notify_last_frame_filling;
    int last_frame_filling_flag;
    int notify_each_frame_done; /* each frame data save as a single fits file */
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
    struct Device _;
    const void *_vtab;
    char *name;
    char *description;
    struct DetectorState d_state;
    struct DetectorCapability d_cap;
    struct DetectorParameter d_param;
    struct DetectorExposureControl d_exp;
    struct DetectorFrameProcess d_proc;
};

struct __DetectorClass {
    struct DeviceClass _;
    /*
     * Camera controlling methods, virtual or pure virtual methods.
     */
    struct Method abort;
    struct Method disable_cooling;
    struct Method enable_cooling;
    struct Method expose;
    struct Method get_binning;
    struct Method get_exposure_time;
    struct Method get_gain;
    struct Method get_readout_rate;
    struct Method get_region;
    struct Method get_temperature;
    struct Method info;
    struct Method init;
    struct Method inspect;
    struct Method load;
    struct Method power_on;
    struct Method power_off;
    struct Method raw;
    struct Method reload;
    struct Method set_binning;
    struct Method set_exposure_time;
    struct Method set_frame_rate;
    struct Method get_frame_rate;
    struct Method set_gain;
    struct Method set_readout_rate;
    struct Method set_region;
    struct Method set_temperature;
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
    struct Method get_binning;
    struct Method get_exposure_time;
    struct Method get_gain;
    struct Method get_readout_rate;
    struct Method get_region;
    struct Method get_temperature;
    struct Method info;
    struct Method init;
    struct Method inspect;
    struct Method load;
    struct Method power_on;
    struct Method power_off;
    struct Method raw;
    struct Method reload;
    struct Method set_binning;
    struct Method set_exposure_time;
    struct Method set_frame_rate;
    struct Method get_frame_rate;
    struct Method set_gain;
    struct Method set_readout_rate;
    struct Method set_region;
    struct Method set_temperature;
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
    int stop_flag;
    pthread_mutex_t mtx;
    pthread_cond_t cond;
    double ra;
    double dec;
    double pixelscale;
    double read_noise;
    double bias_level;
    double sat_level;
};

struct VirtualDetectorClass {
    struct __DetectorClass _;
};

#endif /* detector_r_h */
