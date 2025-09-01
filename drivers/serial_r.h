//
//  serial_r.h
//  AAOS
//
//  Created by huyi on 2018/10/25.
//  Copyright © 2018 National Astronomical Observatories, Chinese Academy of Sciences. All rights reserved.
//

#ifndef serial_r_h
#define serial_r_h

#include "object_r.h"
#include "virtual_r.h"
#include <pthread.h>

//#define _SERIAL_PRIORITY_ _VIRTUAL_PRIORITY_ + 1

struct __Serial {
    struct Object _;
    const void *_vtab;
    char *name;
    char *path;
    char *description;
    char *inspect;
    void *read_buffer; /* internal buffer, not use yet */
    void *write_buffer; /* internal buffer, not use yet */
    size_t read_buffer_size;
    size_t write_buffer_size;
    size_t read_size;
    size_t write_size;
    double read_timeout;
    double write_timeout;
    int fd;
    unsigned int state;
    unsigned int option;
    pthread_mutex_t mtx;
    pthread_cond_t cond;
};

struct __SerialClass {
    struct Class _;
    struct Method init;
    struct Method load;
    struct Method unload;
    struct Method reload;
    struct Method read; /* normal read*/
    struct Method write;
    struct Method read2; /* use internal buffer */
    struct Method read3; /* read for noncanonical mode */
    struct Method write2;
    struct Method get_result;
    struct Method set_command;
    struct Method set_option;
    struct Method get_option;
    struct Method raw;
    struct Method raw_nl;
    struct Method raw2;
    struct Method validate;
    struct Method get_fd;
    struct Method get_name;
    struct Method get_path;
    struct Method feed_dog;
    struct Method inspect;
    struct Method set_state;
    struct Method get_state;
    struct Method set_inspect;
    struct Method get_inspect;
    struct Method wait;
};

struct __SerialVirtualTable {
    struct VirtualTable _;
    struct Method init;
    struct Method raw;
    struct Method validate;
    struct Method feed_dog;
    struct Method inspect;
};

struct JZDSerial {
    struct __Serial _;
};

struct JZDSerialClass {
    struct __SerialClass _;
};

struct SQMSerial {
    struct __Serial _;
};

struct SQMSerialClass {
    struct __SerialClass _;
};

struct WS100UMBSerial {
    struct __Serial _;
};

struct WS100UMBSerialClass {
    struct __SerialClass _;
};

struct AAGPDUSerial {
    struct __Serial _;
};

struct AAGPDUSerialClass {
    struct __SerialClass _;
};

struct APMountSerial {
    struct __Serial _;
    double timeout;
};

struct APMountSerialClass {
    struct __SerialClass _;
};

struct HCD6817CSerial {
    struct __Serial _;
    double timeout;
};

struct HCD6817CSerialClass {
    struct __SerialClass _;
};

struct RDSSSerial {
    struct __Serial _;
    void *queue;
    void *queue2;
};

struct RDSSSerialClass {
    struct __SerialClass _;
};

struct SMSSerial {
    struct __Serial _;
    double timeout;
};

struct SMSSerialClass {
    struct __SerialClass _;
    struct Method send;
    struct Method recv;
    struct Method del;
};

struct KLTPSerial {
    struct __Serial _;
    size_t output_len;
    unsigned char *read_buf;
    int flag;
    unsigned int data_flag;
    pthread_mutex_t data_flag_mtx;
    pthread_t tid; /* read thread */
    //pthread_mutex_t mtx;
    //pthread_cond_t cond;
    pthread_t tid2; /* process thread */
    //void *queue;
    void *data_queue; /* data queue */
    void *cmd_queue; /* command queue */
    size_t size;    /* maximum number of elements in circular queue for data collection */
    size_t size2;
    size_t length; /* data length of each element */
    char *prefix;
    char *directory;
    char *fmt;
    unsigned int model;
};

struct KLTPSerialClass {
    struct __SerialClass _;
};
#endif /* serial_r_h */
