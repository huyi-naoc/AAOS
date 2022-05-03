//
//  protocol_r.h
//  AAOS
//
//  Created by huyi on 2018/11/8.
//  Copyright © 2018 National Astronomical Observatories, Chinese Academy of Sciences. All rights reserved.
//

#ifndef protocol_r_h
#define protocol_r_h

#include <stdint.h>
#include "object_r.h"

#define CARRIERSIZE 16
#define DEFAULTPACKETSIZE 992

#define _PROTOCOL_PRIORITY_ 102

struct Packet {
    uint16_t protocol;
    uint16_t index;
    uint16_t command;
    uint16_t option;
    uint16_t channel;
    uint16_t errorcode;
    uint32_t length;
    union Carrier {
        struct Uint16_8 {
            uint16_t field0;
            uint16_t field1;
            uint16_t field2;
            uint16_t field3;
            uint16_t field4;
            uint16_t field5;
            uint16_t field6;
            uint16_t field7;
        } uint16_8;
        struct Uint32_4 {
            uint32_t field0;
            uint32_t field1;
            uint32_t field2;
            uint32_t field3;
        } uint32_4;
        struct Uint64_2 {
            uint64_t field0;
            uint64_t field1;
        } uint64_2;
        struct Float_4 {
            float field0;
            float field1;
            float field2;
            float field3;
        } float_4;
        struct Double_2 {
            double field0;
            double field1;
        } double_2;
        char string_1[CARRIERSIZE];
    } carrier;
    char buf[];
};

/*
 * Greeting Google's protobuf ^_^
 */

struct ProtoBuf {
    struct Object _;
    struct Packet *packet;
    size_t packet_size;
};

struct ProtoBufClass {
    struct Class _;
    struct Method set;
    struct Method get;
    struct Method reallocate;
    struct Method payload;
    struct Method header;
};

#endif /* protocol_r_h */
