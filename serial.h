
int serial_read(void *_self, void *data, size_t size, size_t *read_size);
int serial_write(void *_self, const void *data, size_t size);
int serial_init(void *_self);

int serial_aws_temperature(void *_self, unsigned int channel, void *temperature, unsigned int options);
int serial_aws_wind_speed(void *_self, unsigned int channel, void *wind_speed, unsigned int options);
int serial_aws_wind_direction(void *_self, unsigned int channel, void *wind_direction, unsigned int options);
int serial_aws_air_pressure(void *_self, unsigned int channel, void *air_pressure, unsigned int options);
int serial_aws_relative_humidity(void *_self, unsigned int channel, void *relative_humidity, unsigned int options);
int serial_aws_raw(void *_self, void *command, size_t command_size, void *result, size_t result_size);

int serial_pdu_temprature(void *_self, unsigned int channel, void *temperature, unsigned int options);
int serial_pdu_voltage_current(void *_self, unsigned int channel, void *voltage, void *current, unsigned int options);
int serial_pdu_turn_on(void *_self, unsigned int channel);
int serial_pdu_turn_off(void *_self, unsigned int channel);
int serial_pdu_raw(void *_self, void *command, size_t command_size, void *result, size_t result_size);                                   
