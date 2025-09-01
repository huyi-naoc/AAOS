//
//  pdu.h
//  AAOS
//
//  Created by Hu Yi on 2020/6/29.
//  Copyright © 2020 NAOC. All rights reserved.
//

#ifndef pdu_h
#define pdu_h

#include <string.h>

#define SWITCH_STATUS_ON        0
#define SWITCH_STATUS_OFF       1
#define SWITCH_STATUS_UNKNOWN   2

#ifdef __cplusplus
extern "C" {
#endif

extern const void *SwitchVirtualTable(void);

int switch_turn_on(void *_self);
int switch_turn_off(void *_self);
int switch_get_current(void *_self, double *current);
int switch_get_voltage(void *_self, double *voltage);
int switch_get_voltage_current(void *_self, double *voltage, double *current);
int switch_status(void *_self, unsigned char *status);

const char *switch_get_name(const void *_self);
unsigned int switch_get_channel(const void *_self);
void switch_set_channel(void *_self, unsigned int channel);
unsigned int switch_get_type(const void *_self);
void switch_set_type(void *_self, unsigned int type);
void switch_set_pdu(void *_self, const void *pdu);

extern const void *Switch(void);
extern const void *SwitchClass(void);

extern const void *AAGSwitch(void);
extern const void *AAGSwitchClass(void);

const char *__pdu_get_name(const void *_self);
void **__pdu_get_switches(const void *_self);
void __pdu_set_switch(void *_self, const void *myswitch, size_t index);
void *__pdu_get_switch(void *_self, size_t index);

void __pdu_set_type(void *_self, const char *type);
int __pdu_turn_on(void *_self, unsigned int channel);
int __pdu_turn_off(void *_self, unsigned int channel);
int __pdu_get_current(void *_self, unsigned int channel, double *current);
int __pdu_get_voltage(void *_self, unsigned int channel, double *voltage);
int __pdu_get_voltage_current(void *_self, unsigned int channel, double *voltage, double *current);
int __pdu_status(void *_self, unsigned char *status, size_t size);
int __pdu_turn_on_by_name(void *_self, const char *name);
int __pdu_turn_off_by_name(void *_self, const char *name);
int __pdu_get_current_by_name(void *_self, const char *name, double *current);
int __pdu_get_voltage_by_name(void *_self, const char *name, double *voltage);
int __pdu_get_voltage_current_by_name(void *_self, const char *name, double *voltage, double *current);
int __pdu_get_channel_by_name(const void *_self, const char *name, unsigned int *channel);

extern const void *__PDUVirtualTable(void);
extern const void *__PDU(void);
extern const void *__PDUClass(void);

extern const void *AAGPDU(void);
extern const void *AAGPDUClass(void);

#ifdef __cplusplus
}
#endif

#endif /* pdu_h */
