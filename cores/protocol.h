//
//  protocol.h
//  AAOS
//
//  Created by huyi on 2018/11/8.
//  Copyright © 2018 National Astronomical Observatories, Chinese Academy of Sciences. All rights reserved.
//

#ifndef protocol_h
#define protocol_h

#include "object.h"

#define PACKET_PROTOCOL 1
#define PACKET_INDEX 2
#define PACKET_COMMAND 3
#define PACKET_OPTION 4
#define PACKET_CHANNEL 5
#define PACKET_ERRORCODE 6
#define PACKET_LENGTH 7
#define PACKET_U16F0 8
#define PACKET_U16F1 9
#define PACKET_U16F2 10
#define PACKET_U16F3 11
#define PACKET_U16F4 12
#define PACKET_U16F5 13
#define PACKET_U16F6 14
#define PACKET_U16F7 15
#define PACKET_U32F0 16
#define PACKET_U32F1 17
#define PACKET_U32F2 18
#define PACKET_U32F3 19
#define PACKET_FF0 20
#define PACKET_FF1 21
#define PACKET_FF2 22
#define PACKET_FF3 23
#define PACKET_DF0 24
#define PACKET_DF1 25
#define PACKET_STR 26
#define PACKET_BUF 27
#define PACKET_SIZE 28
#define PACKET_U64F0 29
#define PACKET_U64F1 30

#define PACKETHEADERSIZE 32
#define PACKETPARAMETERSIZE 16

#ifdef __cplusplus
extern "C" {
#endif

void protobuf_set(void *_self, unsigned int field, ...);
void protobuf_get(const void *_self, unsigned int field, ...);
int protobuf_reallocate(void *_self, size_t size);
size_t protobuf_payload(const void *_self);
void *protobuf_header(const void *_self);

extern const void *ProtoBuf(void);
extern const void *ProtoBufClass(void);
#ifdef __cplusplus
}
#endif

#endif /* protocol_h */
