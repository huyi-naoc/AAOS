//
//  pdu_rpc.h
//  AAOS
//
//  Created by Hu Yi on 2020/7/16.
//  Copyright © 2020 NAOC. All rights reserved.
//

#ifndef pdu_rpc_h
#define pdu_rpc_h

#include "rpc.h"

#define PDU_COMMAND_TURN_ON                 1
#define PDU_COMMAND_TURN_OFF                2
#define PDU_COMMAND_GET_VOLTAGE             3
#define PDU_COMMAND_GET_CURRENT             4
#define PDU_COMMAND_GET_VOLTAGE_CURRENT     5
#define PDU_COMMAND_STATUS                  6

#define PDU_COMMAND_GET_INDEX_BY_NAME       23
#define PDU_COMMAND_GET_CHANNEL_BY_NAME     24

void **pdus;
size_t n_pdu;

#ifdef __cplusplus
extern "C" {
#endif

int pdu_turn_on(void *_self);
int pdu_turn_off(void *_self);
int pdu_get_current(void *_self, double *current);
int pdu_get_voltage(void *_self, double *voltage);
int pdu_get_voltage_current(void *_self, double *voltage, double *current);
int pdu_status(void *_self, unsigned char *status, size_t size);

int pdu_turn_on_by_channle(void *_self, unsigned int channel);
int pdu_turn_off_by_channle(void *_self, unsigned int channel);
int pdu_get_current_by_channle(void *_self, unsigned int channel, double *current);
int pdu_get_voltage_by_channle(void *_self, unsigned int channel, double *voltage);
int pdu_get_voltage_current_by_channel(void *_self, unsigned int channel, double *voltage, double *current);
int pdu_turn_on_by_name(void *_self, const char *name);
int pdu_turn_off_by_name(void *_self, const char *name);
int pdu_get_current_by_name(void *_self, const char *name, double *current);
int pdu_get_voltage_by_name(void *_self, const char *name, double *voltage);
int pdu_get_voltage_current_by_name(void *_self, const char *name, double *voltage, double *current);

int pdu_get_index_by_name(void *_self, const char *name);
int pdu_get_channel_by_name(void *_self, const char *name);

extern const void *PDU(void);
extern const void *PDUClass(void);

extern const void *PDUClient(void);
extern const void *PDUClientClass(void);

extern const void *PDUServer(void);
extern const void *PDUServerClass(void);

#ifdef __cplusplus
}
#endif

#endif /* pdu_rpc_h */
