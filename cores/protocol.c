//
//  protocol.c
//  AAOS
//
//  Created by huyi on 2018/11/8.
//  Copyright © 2018 National Astronomical Observatories, Chinese Academy of Sciences. All rights reserved.
//

#include "def.h"
#include "protocol_r.h"
#include "protocol.h"
#include "wrapper.h"

void
protobuf_set(void *_self, unsigned int field, ...)
{
    const struct ProtoBufClass *class = (const struct ProtoBufClass*) classOf(_self);
    va_list ap;
    va_start(ap, field);
    
    if (isOf(class, ProtoBufClass()) && class->set.method) {
        ((void (*)(void *, unsigned int, va_list *)) class->set.method)( _self, field, &ap);
    } else {
        forward(_self, 0, (Method) protobuf_set, "set", _self, field, &ap);
    }
    va_end(ap);
}

static void
ProtoBuf_set(void *_self, unsigned int field, va_list *app)
{
    struct ProtoBuf *self = cast(ProtoBuf(), _self);
    
    switch (field) {
        case PACKET_PROTOCOL:
        {
            uint32_t value = va_arg(*app, uint32_t);
            self->packet->protocol = (uint16_t) value;
        }
            break;
        case PACKET_INDEX:
        {
            uint32_t value = va_arg(*app, uint32_t);
            self->packet->index = (uint16_t) value;
        }
            break;
        case PACKET_COMMAND:
        {
            uint32_t value = va_arg(*app, uint32_t);
            self->packet->command = (uint16_t) value;
        }
            break;
        case PACKET_OPTION:
        {
            uint32_t value = va_arg(*app, uint32_t);
            self->packet->option = (uint16_t) value;
        }
            break;
        case PACKET_CHANNEL:
        {
            uint32_t value = va_arg(*app, uint32_t);
            self->packet->channel = (uint16_t) value;
        }
            break;
        case PACKET_ERRORCODE:
        {
            uint32_t value = va_arg(*app, uint32_t);
            self->packet->errorcode = (uint16_t) value;
        }
            break;
        case PACKET_LENGTH:
        {
            uint32_t value = va_arg(*app, uint32_t);
            self->packet->length = value;
        }
            break;
        case PACKET_U16F0:
        {
            uint32_t value = va_arg(*app, uint32_t);
            self->packet->carrier.uint16_8.field0 = (uint16_t) value;
        }
            break;
        case PACKET_U16F1:
        {
            uint32_t value = va_arg(*app, uint32_t);
            self->packet->carrier.uint16_8.field1 = (uint16_t) value;
        }
            break;
        case PACKET_U16F2:
        {
            uint32_t value = va_arg(*app, uint32_t);
            self->packet->carrier.uint16_8.field2 = (uint16_t) value;
        }
            break;
        case PACKET_U16F3:
        {
            uint32_t value = va_arg(*app, uint32_t);
            self->packet->carrier.uint16_8.field3 = (uint16_t) value;
        }
            break;
        case PACKET_U16F4:
        {
            uint32_t value = va_arg(*app, uint32_t);
            self->packet->carrier.uint16_8.field4 = (uint16_t) value;
        }
            break;
        case PACKET_U16F5:
        {
            uint32_t value = va_arg(*app, uint32_t);
            self->packet->carrier.uint16_8.field5 = (uint16_t) value;
        }
            break;
        case PACKET_U16F6:
        {
            uint32_t value = va_arg(*app, uint32_t);
            self->packet->carrier.uint16_8.field6 = (uint16_t) value;
        }
            break;
        case PACKET_U16F7:
        {
            uint32_t value = va_arg(*app, uint32_t);
            self->packet->carrier.uint16_8.field7 = (uint16_t) value;
        }
            break;
        case PACKET_U32F0:
        {
            uint32_t value = va_arg(*app, uint32_t);
            self->packet->carrier.uint32_4.field0 = value;
        }
            break;
        case PACKET_U32F1:
        {
            uint32_t value = va_arg(*app, uint32_t);
            self->packet->carrier.uint32_4.field1 = value;
        }
            break;
        case PACKET_U32F2:
        {
            uint32_t value = va_arg(*app, uint32_t);
            self->packet->carrier.uint32_4.field2 = value;
        }
            break;
        case PACKET_U32F3:
        {
            uint32_t value = va_arg(*app, uint32_t);
            self->packet->carrier.uint32_4.field3 = value;
        }
            break;
        case PACKET_U64F0:
        {
            uint64_t value = va_arg(*app, uint64_t);
            self->packet->carrier.uint64_2.field0 = value;
        }
            break;
        case PACKET_U64F1:
        {
            uint64_t value = va_arg(*app, uint64_t);
            self->packet->carrier.uint64_2.field1 = value;
        }
            
        case PACKET_FF0:
        {
            double value = va_arg(*app, double);
            self->packet->carrier.float_4.field0 = (float) value;
        }
            break;
        case PACKET_FF1:
        {
            double value = va_arg(*app, double);
            self->packet->carrier.float_4.field1 = (float) value;
        }
            break;
        case PACKET_FF2:
        {
            double value = va_arg(*app, double);
            self->packet->carrier.float_4.field2 = (float) value;
        }
            break;
        case PACKET_FF3:
        {
            double value = va_arg(*app, double);
            self->packet->carrier.float_4.field3 = (float) value;
        }
            break;
        case PACKET_DF0:
        {
            double value = va_arg(*app, double);
            self->packet->carrier.double_2.field0 = value;
        }
            break;
        case PACKET_DF1:
        {
            double value = va_arg(*app, double);
            self->packet->carrier.double_2.field1 = value;
        }
            break;
        case PACKET_STR:
        {
            const char *value = va_arg(*app, const char *);
            size_t size = strlen(value);
            if (size > CARRIERSIZE - 1) {
                return;
            }
            snprintf(self->packet->carrier.string_1, CARRIERSIZE, "%s", value);
        }
            break;
        case PACKET_BUF:
        {
            const void *value = va_arg(*app, const void *);
            size_t size = va_arg(*app, size_t);
            if (self->packet->buf != value) {
                if (self->packet_size < size) {
                    protobuf_reallocate(self, size);
                }
            
                memcpy(self->packet->buf, value, size);
            }
            self->packet->length = (uint32_t) size;
        }
            break;
        default:
            break;
    }
}

/*
 * If field is PACKET_BUF, the type of `size` parameter must be size_t * or NULL !!!!
 */

void
protobuf_get(const void *_self, unsigned int field, ...)
{
    const struct ProtoBufClass *class = (const struct ProtoBufClass*) classOf(_self);
    va_list ap;
    va_start(ap, field);
    
    if (isOf(class, ProtoBufClass()) && class->get.method) {
        ((void (*)(const void *, unsigned int, va_list *)) class->get.method)( _self, field, &ap);
    } else {
        forward(_self, 0, (Method) protobuf_get, "get", _self, field, &ap);
    }
    va_end(ap);
}

static void
ProtoBuf_get(const void *_self, unsigned int field, va_list *app)
{
    const struct ProtoBuf *self = cast(ProtoBuf(), _self);
    
    void *value = va_arg(*app, void *);
    
    switch (field) {
        case PACKET_PROTOCOL:
            memcpy(value, &self->packet->protocol, sizeof(uint16_t));
            break;
        case PACKET_INDEX:
            memcpy(value, &self->packet->index, sizeof(uint16_t));
            break;
        case PACKET_COMMAND:
            memcpy(value, &self->packet->command, sizeof(uint16_t));
            break;
        case PACKET_OPTION:
            memcpy(value, &self->packet->option, sizeof(uint16_t));
            break;
        case PACKET_CHANNEL:
            memcpy(value, &self->packet->channel, sizeof(uint16_t));
            break;
        case PACKET_ERRORCODE:
            memcpy(value, &self->packet->errorcode, sizeof(uint16_t));
            break;
        case PACKET_LENGTH:
            memcpy(value, &self->packet->length, sizeof(uint32_t));
            break;
        case PACKET_U16F0:
            memcpy(value, &self->packet->carrier.uint16_8.field0, sizeof(uint16_t));
            break;
        case PACKET_U16F1:
            memcpy(value, &self->packet->carrier.uint16_8.field1, sizeof(uint16_t));
            break;
        case PACKET_U16F2:
            memcpy(value, &self->packet->carrier.uint16_8.field2, sizeof(uint16_t));
            break;
        case PACKET_U16F3:
            memcpy(value, &self->packet->carrier.uint16_8.field3, sizeof(uint16_t));
            break;
        case PACKET_U16F4:
            memcpy(value, &self->packet->carrier.uint16_8.field4, sizeof(uint16_t));
            break;
        case PACKET_U16F5:
            memcpy(value, &self->packet->carrier.uint16_8.field5, sizeof(uint16_t));
            break;
        case PACKET_U16F6:
            memcpy(value, &self->packet->carrier.uint16_8.field6, sizeof(uint16_t));
            break;
        case PACKET_U16F7:
            memcpy(value, &self->packet->carrier.uint16_8.field7, sizeof(uint16_t));
            break;
        case PACKET_U32F0:
            memcpy(value, &self->packet->carrier.uint32_4.field0, sizeof(uint32_t));
            break;
        case PACKET_U32F1:
            memcpy(value, &self->packet->carrier.uint32_4.field1, sizeof(uint32_t));
            break;
        case PACKET_U32F2:
            memcpy(value, &self->packet->carrier.uint32_4.field2, sizeof(uint32_t));
            break;
        case PACKET_U32F3:
            memcpy(value, &self->packet->carrier.uint32_4.field3, sizeof(uint32_t));
            break;
        case PACKET_U64F0:
            memcpy(value, &self->packet->carrier.uint64_2.field0, sizeof(uint64_t));
            break;
        case PACKET_U64F1:
            memcpy(value, &self->packet->carrier.uint64_2.field1, sizeof(uint64_t));
            break;
        case PACKET_FF0:
            memcpy(value, &self->packet->carrier.float_4.field0, sizeof(float));
            break;
        case PACKET_FF1:
            memcpy(value, &self->packet->carrier.float_4.field1, sizeof(float));
            break;
        case PACKET_FF2:
            memcpy(value, &self->packet->carrier.float_4.field2, sizeof(float));
            break;
        case PACKET_FF3:
            memcpy(value, &self->packet->carrier.float_4.field3, sizeof(float));
            break;
        case PACKET_DF0:
            memcpy(value, &self->packet->carrier.double_2.field0, sizeof(double));
            break;
        case PACKET_DF1:
            memcpy(value, &self->packet->carrier.double_2.field1, sizeof(double));
            break;
        case PACKET_STR:
            *((char **) value) = self->packet->carrier.string_1;
            break;
        case PACKET_BUF:
        {
            *((void **) value) = self->packet->buf;
            size_t *size = va_arg(*app, size_t *);
            if (size) {
                *size = (size_t) self->packet->length;
            }
        }
            break;
        case PACKET_SIZE:
            memcpy(value, &self->packet_size, sizeof(size_t));
            break;
        default:
            break;
    }
}

int
protobuf_reallocate(void *_self, size_t size)
{
    const struct ProtoBufClass *class = (const struct ProtoBufClass*) classOf(_self);
    
    int result;
    
    if (isOf(class, ProtoBufClass()) && class->reallocate.method) {
        result = ((int (*)(void *, size_t)) class->reallocate.method)( _self, size);
    } else {
        forward(_self, &result, (Method) protobuf_reallocate, "reallocate", _self, size);
    }
    
    return result;
}

static int
ProtoBuf_reallocate(void *_self, size_t size)
{
    struct ProtoBuf *self = cast(ProtoBuf(), _self);
    
    void *packet;
    
    packet = Realloc(self->packet, size + sizeof(struct Packet));
    if (!packet) {
        return AAOS_ENOMEM;
    }
    self->packet = packet;
    self->packet_size = size;
    
    return AAOS_OK;
}

size_t
protobuf_payload(const void *_self)
{
    const struct ProtoBufClass *class = (const struct ProtoBufClass*) classOf(_self);
    
    size_t result;
    
    if (isOf(class, ProtoBufClass()) && class->payload.method) {
        result = ((size_t (*)(const void *)) class->payload.method)( _self);
    } else {
        forward(_self, &result, (Method) protobuf_payload, "payload", _self);
    }
    
    return result;
}

static size_t
ProtoBuf_payload(const void *_self)
{
    struct ProtoBuf *self = cast(ProtoBuf(), _self);
    
    return self->packet_size;
}

void *
protobuf_header(const void *_self)
{
    const struct ProtoBufClass *class = (const struct ProtoBufClass*) classOf(_self);
    
    void *result;
    
    if (isOf(class, ProtoBufClass()) && class->header.method) {
        result = ((void * (*)(const void *)) class->header.method)( _self);
    } else {
        forward(_self, &result, (Method) protobuf_header, "header", _self);
    }
    
    return result;
}

static void *
ProtoBuf_header(const void *_self)
{
    struct ProtoBuf *self = cast(ProtoBuf(), _self);
    
    return (void *) self->packet;
}

static void *
ProtoBuf_cctor(void *_self, const void *_from)
{
    struct ProtoBuf *self = super_cctor(ProtoBuf(), _self);
    struct ProtoBuf *from = cast(ProtoBuf(), _from);
    
    self->packet_size = from->packet_size;
    self->packet = (struct Packet *) Malloc(sizeof(struct Packet) + self->packet_size);
    memcpy(self->packet, from->packet, self->packet_size);
    
    return (void *) self;
}

static void *
ProtoBuf_ctor(void *_self, va_list *app)
{
    struct ProtoBuf *self = super_ctor(ProtoBuf(), _self, app);
    
    size_t size = va_arg(*app, size_t);
    
    if (size == 0) {
        self->packet_size = DEFAULTPACKETSIZE;
    } else {
        self->packet_size = size;
    }
    
    self->packet = (struct Packet *) Malloc(sizeof(struct Packet) + self->packet_size);
    memset(self->packet, '\0', sizeof(struct Packet) + self->packet_size);
    
    return (void *) self;
}

static void *
ProtoBuf_dtor(void *_self)
{
    struct ProtoBuf *self = cast(ProtoBuf(), _self);
    
    free(self->packet);
    
    return super_dtor(ProtoBuf(), _self);
}

static void *
ProtoBufClass_ctor(void *_self, va_list *app)
{
    struct ProtoBufClass *self = super_ctor(ProtoBufClass(), _self, app);
    Method selector;
    
#ifdef va_copy
    va_list ap;
    va_copy(ap, *app);
#else
    va_list ap = *app;
#endif
    
    while ((selector = va_arg(ap, Method))) {
        const char *tag = va_arg(ap, const char *);
        Method method = va_arg(ap, Method);
        if (selector == (Method) protobuf_set) {
            if (tag) {
                self->set.tag = tag;
                self->set.selector = selector;
            }
            self->set.method = method;
            continue;
        }
        if (selector == (Method) protobuf_get) {
            if (tag) {
                self->get.tag = tag;
                self->get.selector = selector;
            }
            self->get.method = method;
            continue;
        }
        if (selector == (Method) protobuf_reallocate) {
            if (tag) {
                self->reallocate.tag = tag;
                self->reallocate.selector = selector;
            }
            self->reallocate.method = method;
            continue;
        }
        if (selector == (Method) protobuf_header) {
            if (tag) {
                self->header.tag = tag;
                self->header.selector = selector;
            }
            self->header.method = method;
            continue;
        }
        if (selector == (Method) protobuf_payload) {
            if (tag) {
                self->payload.tag = tag;
                self->payload.selector = selector;
            }
            self->payload.method = method;
            continue;
        }
    }
    
#ifdef va_copy
    va_end(ap);
#endif
    
    return (void *) self;
}

static const void *_ProtoBufClass;

static void
ProtoBufClass_destroy(void)
{
    free((void *) _ProtoBufClass);
}

static void
ProtoBufClass_initialize(void)
{
    _ProtoBufClass = new(Class(), "ProtoBufClass", Class(), sizeof(struct ProtoBufClass),
                         ctor, "ctor", ProtoBufClass_ctor,
                         (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(ProtoBufClass_destroy);
#endif
}

const void *
ProtoBufClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, ProtoBufClass_initialize);
#endif
    
    return _ProtoBufClass;
}

static const void *_ProtoBuf;

static void
ProtoBuf_destroy(void)
{
    free((void *)_ProtoBuf);
}

static void
ProtoBuf_initialize(void)
{
    _ProtoBuf = new(ProtoBufClass(), "ProtoBuf", Object(), sizeof(struct ProtoBuf),
                    ctor, "ctor", ProtoBuf_ctor,
                    cctor, "cctor", ProtoBuf_cctor,
                    dtor, "dtor", ProtoBuf_dtor,
                    protobuf_set, "set", ProtoBuf_set,
                    protobuf_get, "get", ProtoBuf_get,
                    protobuf_reallocate, "reallocate", ProtoBuf_reallocate,
                    protobuf_payload, "payload", ProtoBuf_payload,
                    protobuf_header, "header", ProtoBuf_header,
                    (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(ProtoBuf_destroy);
#endif
}

const void *
ProtoBuf(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, ProtoBuf_initialize);
#endif
    
    return _ProtoBuf;
}

#ifdef _USE_COMPILER_ATTRIBUTION_

static void __destructor__(void) __attribute__ ((destructor(_PROTOCOL_PRIORITY_)));

static void
__destructor__(void)
{
    ProtoBuf_destroy();
    ProtoBufClass_destroy();
}

static void __constructor__(void) __attribute__ ((constructor(_PROTOCOL_PRIORITY_)));

static void
__constructor__(void)
{
    ProtoBufClass_initialize();
    ProtoBuf_initialize();
}

#endif
