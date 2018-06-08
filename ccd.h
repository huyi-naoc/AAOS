
int ccd_expose(void *_self, double exposure_time, size_t frames);
int ccd_set_frame_rate(void *_self, double frame_rate);
int ccd_get_frame_rate(void *_self, double *frame_rate);
int ccd_set_exposure_time(void *_self, double exposure_time);
int ccd_get_exposure_time(void *_self, double *exposure_time);
int ccd_set_region(void *_self, size_t x_offset, size_t y_offset, size_t width, size_t height);
int ccd_get_region(void *_self, size_t *x_offset, size_t *y_offset, size_t *width, size_t *height);
int ccd_set_binning(void *_self, size_t x_binning, size_t x_binning);
int ccd_get_binning(void *_self, size_t *x_binning, size_t *y_binning);
int ccd_shutter(void *_self, unsigned int options);
int ccd_pipeline(void *_self, unsigned int options);
int ccd_storage(void *_self, unsigned int options);
int ccd_power_on(void *_self);
int ccd_power_off(void *_self);
int ccd_init(void *_self);
int ccd_raw(void *_self, void *command, size_t command_size, void *result, size_t result_size);
