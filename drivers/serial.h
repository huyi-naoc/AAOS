//
//  serial.h
//  AAOS
//
//  Created by huyi on 2018/10/25.
//  Copyright © 2018 National Astronomical Observatories, Chinese Academy of Sciences. All rights reserved.
//

#ifndef serial_h
#define serial_h

#include <string.h>
#include "object.h"
#include "virtual.h"

/*
#define SERIAL_STATE_UNINITIALIZED 1
#define SERIAL_STATE_READY 2
#define SERIAL_STATE_UNLOADED 3
 */

int __serial_init(void *_self);
unsigned int __serial_set_option(void *_self, unsigned int option);
int __serial_read(void *_self, void *read_buffer, size_t buffer_size, size_t *read_size);
int __serial_write(void *_self, const void *write_buffer, size_t buffer_size, size_t *write_size);
int __serial_read2(void *_self);
int __serial_write2(void *_self);
int __serial_read3(void *_self, void *read_buffer, size_t buffer_size, size_t *read_size);
int __serial_set_command(void *_self, const void *data, size_t size);
int __serial_raw(void *_self, const void *write_buffer, size_t write_buffer_size, size_t *write_size, void *read_buffer, size_t read_buffer_size, size_t *read_size);
int __serial_raw2(void *_self);
int __serial_get_fd(const void *_self);
int __serial_get_result(const void *_self, void **result, size_t *size);
int __serial_feed_dog(void *_self);
int __serial_validate(const void *_self, const void *command, size_t size);
int __serial_reload(void *_self);
int __serial_load(void *_self, ...);
int __serial_unload(void *_self);
int __serial_inspect(void *_self);
int __serial_wait(void *_self, double timeout);
int __serial_set_state(void *_self, unsigned int state);
int __serial_set_inspect(void *_self, const void *inspect, size_t size);
int __serial_get_inspect(void *_self, void *inspect, size_t size);

const char *__serial_get_name(const void *_self);
const char *__serial_get_path(const void *_self);

extern const void *__Serial(void);
extern const void *__SerialClass(void);
extern const void *__SerialVirtualTable(void);

extern const void *JZDSerial(void);
extern const void *JZDSerialClass(void);

extern const void *SQMSerial(void);
extern const void *SQMSerialClass(void);

extern const void *WS100UMBSerial(void);
extern const void *WS100UMBSerialClass(void);

extern const void *AAGPDUSerial(void);
extern const void *AAGPDUSerialClass(void);

extern const void *APMountSerial(void);
extern const void *APMountSerialClass(void);

extern const void *HCD6817CSerial(void);
extern const void *HCD6817CSerialClass(void);

extern const void *RDSSSerial(void);
extern const void *RDSSSerialClass(void);

int sms_serial_send(void *_self, const char *phone_number, const char *message);
int sms_serial_recv(void *_self, unsigned int number, void *buf, size_t size, size_t *read_size);
int sms_serial_del(void *_self, unsigned int number);
extern const void *SMSSerial(void);
extern const void *SMSSerialClass(void);

extern const void *KLTPSerial(void);
extern const void *KLTPSerialClass(void);
#endif /* serial_h */
