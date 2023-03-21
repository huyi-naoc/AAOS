//
//  pdu_r.h
//  AAOS
//
//  Created by Hu Yi on 2020/6/29.
//  Copyright © 2020 NAOC. All rights reserved.
//

#ifndef pdu_r_h
#define pdu_r_h

#include "device_r.h"
#include "object_r.h"
#include "virtual_r.h"
#include <pthread.h>

#define _PDU_PRIORITY_  _VIRTUAL_PRIORITY_ + 1

struct SwitchVirtualTable {
    struct VirtualTable _;
    struct Method status;
    struct Method turn_on;
    struct Method turn_off;
    struct Method get_voltage;
    struct Method get_current;
    struct Method get_voltage_current;
};

struct Switch {
    struct Object _;
    const void *_vtab;
    char *name;
    unsigned int channel;
    unsigned int type;
    void *pdu;
};

struct SwitchClass {
    struct Class _;
    struct Method get_name;
    struct Method get_channel;
    struct Method set_channel;
    struct Method get_type;
    struct Method set_type;
    struct Method get_pdu;
    struct Method set_pdu;
    struct Method turn_on;
    struct Method turn_off;
    struct Method get_voltage;
    struct Method get_current;
    struct Method get_voltage_current;
    struct Method status;
};

struct AAGSwitch {
    struct Switch _;
    unsigned int flag;
};

struct AAGSwitchClass {
    struct SwitchClass _;
};

struct __PDUVirtualTable {
    struct VirtualTable _;
    struct Method status;
    struct Method turn_on;
    struct Method turn_off;
    struct Method get_voltage;
    struct Method get_current;
    struct Method get_voltage_current;
};

struct __PDU {
    struct Device _;
    const void *_vtab;
    char *name;
    char *type;
    char *description;
    void **swicthes;
    unsigned char *switch_status;
    size_t n_swicth;
};

struct __PDUClass {
    struct DeviceClass _;
    struct Method get_name;
    struct Method set_type;
    struct Method get_switches;
    struct Method get_switch;
    struct Method set_switch;
    
    struct Method status;
    struct Method turn_on;
    struct Method turn_off;
    struct Method get_voltage;
    struct Method get_current;
    struct Method get_voltage_current;
    struct Method turn_on_by_name;
    struct Method turn_off_by_name;
    struct Method get_voltage_by_name;
    struct Method get_current_by_name;
    struct Method get_voltage_current_by_name;
    struct Method get_channel_by_name;
};

#define AAGPDU_CHANNEL_NUMBER 32

struct AAGPDU {
    struct __PDU _;
    void *serial;
    char *serial_address;
    char *serial_port;
    char *serial1;
    char *serial2;
    char *inspect1;
    char *inspect2;
    unsigned int flag;
    unsigned int serial_index_1;
    unsigned int serial_index_2;
    pthread_mutex_t mtx;
};

struct AAGPDUClass {
    struct __PDUClass _;
};

#endif /* pdu_r_h */
