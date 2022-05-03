//
//  serial_rpc.c
//  AAOS
//
//  Created by huyi on 2018/11/26.
//  Copyright © 2018 National Astronomical Observatories, Chinese Academy of Sciences. All rights reserved.
//

#include "adt.h"
#include "def.h"
#include "protocol.h"
#include "serial.h"
#include "serial_rpc.h"
#include "serial_rpc_r.h"
#include "wrapper.h"

void **serials;
size_t n_serial;
void *serial_list;

static bool
serial_find_by_name_if(void *serial, va_list *app)
{
    const char *name, *myname;
    
    myname = __serial_get_name(serial);
#ifdef va_copy
    va_list ap;
    va_copy(ap, *app);
    name = va_arg(ap, const char *);
    va_end(ap);
#else
    name = va_arg(*app, const char *);
#endif
    if (strcmp(name, myname) == 0) {
        return true;
    } else {
        return false;
    }
}

static int
get_index_by_name(const char *name, int *index)
{
    size_t i;
    const char *s;
    *index = 0;
    
    for (i = 0; i < n_serial; i++) {
        s = __serial_get_name(serials[i]);
        if (strcmp(name, s) == 0) {
            *index = (int) i + 1;
            return AAOS_OK;
        }
    }
    
    if (*index == 0) {
        if (threadsafe_list_find_first_if(serial_list, serial_find_by_name_if, name) != NULL) {
            return AAOS_OK;
        }
    }

    return AAOS_ENOTFOUND;
}

static void *
get_serial_by_name(const char *name)
{
    size_t i;
    const char *s;
    void *serial = NULL;
    
    for (i = 0; i < n_serial; i++) {
        s = __serial_get_name(serials[i]);
        if (strcmp(name, s) == 0) {
            return  serials[i];
        }
    }
    
    if (serial == NULL) {
        serial = threadsafe_list_find_first_if(serial_list, serial_find_by_name_if, name);
    }
    
    return serial;
}

static bool
serial_find_by_path_if(void *serial, va_list *app)
{
    const char *path, *mypath;
    
    mypath = __serial_get_path(serial);
#ifdef va_copy
    va_list ap;
    va_copy(ap, *app);
    path = va_arg(ap, const char *);
    va_end(ap);
#else
    path = va_arg(*app, const char *);
#endif
    if (strcmp(path, mypath) == 0) {
        return true;
    } else {
        return false;
    }
}

static int
get_index_by_path(const char *path, int *index)
{
    size_t i;
    const char *s;
    *index = 0;
    
    for (i = 0; i < n_serial; i++) {
        s = __serial_get_path(serials[i]);
        if (strcmp(path, s) == 0) {
            *index = (int) i + 1;
            return AAOS_OK;
        }
    }
    
    if (*index == 0) {
        if (threadsafe_list_find_first_if(serial_list, serial_find_by_path_if, path) != NULL) {
            return AAOS_OK;
        }
    }
    
    return AAOS_ENOTFOUND;
}

static void *
get_serial_by_path(const char *path)
{
    size_t i;
    const char *s;
    void *serial = NULL;
    
    for (i = 0; i < n_serial; i++) {
        s = __serial_get_path(serials[i]);
        if (strcmp(path, s) == 0) {
            return serials[i];
        }
    }
    
    if (serial == NULL) {
        serial = threadsafe_list_find_first_if(serial_list, serial_find_by_path_if, path);
    }
    
    return serial;
}

static void *
get_serial_by_index(int index)
{
    if (index > 0 && index <= n_serial) {
        return serials[index - 1];
    } else {
        return NULL;
    }
}

/*
 * Serial class
 */

// private function

static void
Serial_get_result(void *protobuf, int command, ...)
{
    va_list ap;
    va_start(ap, command);
    switch (command) {
        case SERIAL_COMMAND_INFO:
        case SERIAL_COMMAND_RAW:
        {
            uint32_t length;
            void *res = va_arg(ap, void *);
            size_t res_size = va_arg(ap, size_t);
            size_t *res_length = va_arg(ap, size_t *);
            protobuf_get(protobuf, PACKET_LENGTH, &length);
            if (length == 0) {
                char *s;
                protobuf_get(protobuf, PACKET_STR, &s);
                if (s != res) {
                    snprintf(res, res_size, "%s", s);
                }
                if (res_length != NULL) {
                    *res_length = strlen(res);
                }
            } else {
                char *buf;
                protobuf_get(protobuf, PACKET_BUF, &buf, NULL);
                if (buf != res) {
                    snprintf(res, res_size, "%s", buf);
                }
                if (res_length != NULL) {
                    *res_length = strlen(res);
                }
            }
        }
            break;
        default:
            break;
    }
    
    va_end(ap);
}

inline static int
Serial_protocol_check(void *_self)
{
    uint16_t protocol;
    protobuf_get(_self, PACKET_PROTOCOL, &protocol);
    if (protocol != PROTO_SERIAL && protocol != PROTO_SYSTEM) {
        protobuf_set(_self, PACKET_ERRORCODE, AAOS_EPROTOWRONG);
        protobuf_set(_self, PACKET_LENGTH, 0);
        return AAOS_EPROTOWRONG;
    } else {
        return AAOS_OK;
    }
}

int
serial_get_index_by_name(void *_self, const char *name)
{
    const struct SerialClass *class = (const struct SerialClass *) classOf(_self);
    
    if (isOf(class, SerialClass()) && class->get_index_by_name.method) {
        return ((int (*)(void *, const char *)) class->get_index_by_name.method)(_self, name);
    } else {
        int result;
        forward(_self, &result, (Method) serial_get_index_by_name, "get_index_by_name", _self, name);
        return result;
    }
}

static int
Serial_get_index_by_name(void *_self, const char *name)
{
    struct Serial *self = cast(Serial(), _self);
    size_t length;
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_SERIAL);
    protobuf_set(self, PACKET_COMMAND, SERIAL_COMMAND_GET_INDEX_BY_NAME);
    protobuf_set(self, PACKET_INDEX, 0);
    
    length = strlen(name);
    if (length < PACKETPARAMETERSIZE) {
        char *s;
        protobuf_get(self, PACKET_STR, &s);
        if (s != name) {
            snprintf(s, PACKETPARAMETERSIZE, "%s", name);
        }
        protobuf_set(self, PACKET_LENGTH, 0);
    } else {
        char *buf;
        protobuf_get(self, PACKET_BUF, &buf, NULL);
        if (name != buf) {
            protobuf_set(self, PACKET_BUF, name, length + 1);
        }
        uint32_t len = (uint32_t) length;
        protobuf_set(self, PACKET_LENGTH, len + 1);
    }
    
    return rpc_call(self);
}

int
serial_inspect(void *_self)
{
    const struct SerialClass *class = (const struct SerialClass *) classOf(_self);
    
    if (isOf(class, SerialClass()) && class->inspect.method) {
        return ((int (*)(void *)) class->inspect.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) serial_inspect, "inspect", _self);
        return result;
    }
}

static int
Serial_inspect(void *_self)
{
    struct Serial *self = cast(Serial(), _self);
    void *protobuf = self->_.protobuf;
    
    protobuf_set(protobuf, PACKET_PROTOCOL, PROTO_SERIAL);
    protobuf_set(protobuf, PACKET_COMMAND, SERIAL_COMMAND_INSPECT);
    protobuf_set(self, PACKET_LENGTH, 0);
    
    return rpc_call(self);
}

int
serial_get_index_by_path(void *_self, const char *name)
{
    const struct SerialClass *class = (const struct SerialClass *) classOf(_self);
    
    if (isOf(class, SerialClass()) && class->get_index_by_path.method) {
        return ((int (*)(void *, const char *)) class->get_index_by_path.method)(_self, name);
    } else {
        int result;
        forward(_self, &result, (Method) serial_get_index_by_path, "get_index_by_path", _self, name);
        return result;
    }
}

static int
Serial_get_index_by_path(void *_self, const char *name)
{
    struct Serial *self = cast(Serial(), _self);
    
    void *protobuf = self->_.protobuf;
    int ret;
    size_t length;
    
    protobuf_set(protobuf, PACKET_PROTOCOL, PROTO_SERIAL);
    protobuf_set(protobuf, PACKET_COMMAND, SERIAL_COMMAND_GET_INDEX_BY_PATH);
    
    length = strlen(name);
    if (length < PACKETPARAMETERSIZE) {
        char *s;
        protobuf_get(protobuf, PACKET_STR, &s);
        if (s != name) {
            snprintf(s, PACKETPARAMETERSIZE, "%s", name);
        }
        protobuf_set(protobuf, PACKET_LENGTH, 0);
    } else {
        size_t payload = protobuf_payload(protobuf);
        char *buf;
        protobuf_get(protobuf, PACKET_BUF, &buf, NULL);
        if (name != buf) {
            if (payload < length + 1) {
                if ((ret = protobuf_reallocate(protobuf, length + 1)) != AAOS_OK) {
                    return ret;
                }
                payload = length + 1;
                protobuf_get(protobuf, PACKET_BUF, &buf, NULL);
            }
            snprintf(buf, payload, "%s", name);
        }
        uint32_t len = (uint32_t) length;
        protobuf_set(protobuf, PACKET_LENGTH, len);
    }
    
    return rpc_call(self);
}

int
serial_info(void *_self, void *res, size_t res_size, size_t *res_length)
{
    const struct SerialClass *class = (const struct SerialClass *) classOf(_self);
    
    if (isOf(class, SerialClass()) && class->info.method) {
        return ((int (*)(void *, void *, size_t, size_t *)) class->info.method)(_self, res, res_size, res_length);
    } else {
        int result;
        forward(_self, &result, (Method) serial_info, "info", _self, res, res_size, res_length);
        return result;
    }
}

static int
Serial_info(void *_self, void *res, size_t res_size, size_t *res_length)
{
    struct Serial *self = cast(Serial(), _self);
    
    void *protobuf = self->_.protobuf;
    int ret;
    
    protobuf_set(protobuf, PACKET_PROTOCOL, PROTO_SERIAL);
    protobuf_set(protobuf, PACKET_COMMAND, SERIAL_COMMAND_INFO);
    
    if ((ret = rpc_call(self)) == AAOS_OK) {
        Serial_get_result(protobuf, SERIAL_COMMAND_INFO, res, res_size, res_length);
    }
    
    return ret;
}

int
serial_raw(void *_self, const void *cmd, size_t cmd_size, void *res, size_t res_size, size_t *res_length)
{
    const struct SerialClass *class = (const struct SerialClass *) classOf(_self);
    
    if (isOf(class, SerialClass()) && class->raw.method) {
        return ((int (*)(void *, const void *cmd, size_t, void *, size_t, size_t *)) class->raw.method)(_self, cmd, cmd_size, res, res_size, res_length);
    } else {
        int result;
        forward(_self, &result, (Method) serial_raw, "raw", _self, cmd, cmd_size, res, res_size, res_length);
        return result;
    }
}

static int
Serial_raw(void *_self, const void *cmd, size_t cmd_size, void *res, size_t res_size, size_t *res_length)
{
    struct Serial *self = cast(Serial(), _self);
    uint16_t index;
    uint16_t option;
    int ret;
    
    if (cmd == NULL || res == NULL) {
        return -1 * AAOS_EINVAL;
    }
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_SERIAL);
    protobuf_set(self, PACKET_COMMAND, SERIAL_COMMAND_RAW);
    protobuf_get(self, PACKET_INDEX, &index);
    protobuf_get(self, PACKET_OPTION, &option);
    /*
     * binary command, usually inn non-canonical mode, should not use PACKET_STR field.
     */
    if (cmd_size < PACKETPARAMETERSIZE && index != 0 && !(option & SERIAL_OPTION_BINARY)) {
        char *s;
        protobuf_get(self, PACKET_STR, &s);
        if (s != cmd) {
            memcpy(s, cmd, cmd_size);
            s[cmd_size] = '\0';
        }
        protobuf_set(self, PACKET_LENGTH, 0);
    } else {
        uint32_t length = (uint32_t) cmd_size;
        protobuf_set(self, PACKET_LENGTH, length);
        char *buf;
        protobuf_get(self, PACKET_BUF, &buf, NULL);
        size_t payload = protobuf_payload(self);
        if (payload < cmd_size) {
            return AAOS_ERROR;
        }
        if (buf != cmd) {
            memcpy(buf, cmd, cmd_size);
            if (!(option & SERIAL_OPTION_BINARY)) {
                buf[cmd_size] = '\0';
            }
        }
    }
    if ((ret = rpc_call(self)) == AAOS_OK) {
        uint32_t length;
        protobuf_get(self, PACKET_LENGTH, &length);
        if (length == 0) {
            char *s;
            protobuf_get(self, PACKET_STR, &s);
            if (s != res) {
                snprintf(res, res_size, "%s", s);
            }
            if (res_length != NULL) {
                *res_length = strlen(res);
            }
        } else {
            char *buf;
            protobuf_get(self, PACKET_BUF, &buf, NULL);
            if (buf != res) {
                snprintf(res, res_size, "%s", buf);
            }
            if (res_length != NULL) {
                *res_length = strlen(res);
            }
        }
    }
    return ret;
}

int
serial_register(void *_self, double timeout)
{
    const struct SerialClass *class = (const struct SerialClass *) classOf(_self);
    
    if (isOf(class, SerialClass()) && class->reg.method) {
        return ((int (*)(void *, double)) class->reg.method)(_self, timeout);
    } else {
        int result;
        forward(_self, &result, (Method) serial_register, "register", _self, timeout);
        return result;
    }
}

static int
Serial_register(void *_self, double timeout)
{
    struct Serial *self = cast(Serial(), _self);
    
    void *protobuf = self->_.protobuf;
    
    protobuf_set(protobuf, PACKET_PROTOCOL, PROTO_SYSTEM);
    protobuf_set(protobuf, PACKET_COMMAND, SERIAL_COMMAND_REGISTER);
    protobuf_set(protobuf, PACKET_DF0, timeout);
    
    return rpc_call(_self);
}

/*
 * Virtual function for serial rpc.
 */
static const void *serial_virtual_table(void);

static int
Serial_execute_info(struct Serial *self)
{
    size_t i;
    void *buf;
    size_t size;
    FILE *fp;
    
    size = protobuf_payload(self);
    protobuf_get(self, PACKET_BUF, &buf, NULL);
    if ((fp = fmemopen(buf, size, "w")) == NULL) {
        return AAOS_ERROR;
    }
    for (i = 0; i < n_serial; i++) {
        puto(serials[i], fp);
    }
    fclose(fp);
    return AAOS_OK;
}

/*
 * if a bad command is given, just return AAOS_OK,
 * but set the error code AAOS_EBADCMD.
 */

static int
Serial_execute_raw(struct Serial *self)
{
    char *command;
    int ret;
    uint16_t index;
    uint16_t option;
    uint32_t length;
    
    /*
     * If payload is zero, just use STR field as the input raw command; otherwise, use payload as the input
     */
    
    protobuf_get(self, PACKET_LENGTH, &length);
    if (length == 0) {
        protobuf_get(self, PACKET_STR, &command);
    } else {
        protobuf_get(self, PACKET_BUF, &command, NULL);
    }
    
    protobuf_get(self, PACKET_INDEX, &index);
    protobuf_get(self, PACKET_OPTION, &option);
    
    if (index == 0) {
        /*
         * If index is zero, means STR field contains device name or device path, payload must none zero.
         */
        if (length == 0) {
            return AAOS_EBADCMD;
        } else {
            char *s, *buf;
            void *serial;
            int idx;
            size_t payload, read_size, length;
            protobuf_get(self, PACKET_STR, &s);
            
            if ((get_index_by_name(s, &idx)) != AAOS_OK) {
                get_index_by_path(s, &idx);
            }
            index = (uint16_t) idx;
            protobuf_set(self, PACKET_INDEX, &index);
            if ((serial = get_serial_by_name(s)) == NULL && (serial = get_serial_by_path(s)) == NULL) {
                protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
                protobuf_set(self, PACKET_LENGTH, 0);
                return AAOS_ENOTFOUND;
            }
            protobuf_get(self, PACKET_BUF, &buf, &length);
            if (length == 0) {
                length = (uint32_t) strlen(command);
            }
            payload = protobuf_payload(self);
            if ((ret = __serial_raw(serial, command, length, NULL, buf, payload, &read_size)) != AAOS_OK) {
                return ret;
            }
            /*
             * If STR field is large enough and not in binary mode, use it.
             */
            if (read_size < PACKETPARAMETERSIZE && !(option & SERIAL_OPTION_BINARY)) {
                memcpy(s, buf, read_size);
                protobuf_set(self, PACKET_LENGTH, 0);
            } else {
                length = (uint32_t) read_size;
                protobuf_set(self, PACKET_LENGTH, length);
            }
            protobuf_set(self, PACKET_ERRORCODE, AAOS_OK);
        }
    } else {
        char *s, *buf;
        void *serial;
        if ((serial = get_serial_by_index((int) index)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
        size_t payload, read_size;
        protobuf_get(self, PACKET_LENGTH, &length);
        if (length == 0) {
            length = (uint32_t) strlen(command);
        }
        payload = protobuf_payload(self);
        protobuf_get(self, PACKET_BUF, &buf, NULL);
        
        if ((ret = __serial_raw(serial, command, length, NULL, buf, payload, &read_size)) != AAOS_OK) {
            return ret;
        }
        if (read_size < PACKETPARAMETERSIZE && !(option & SERIAL_OPTION_BINARY)) {
            protobuf_get(self, PACKET_STR, &s);
            memcpy(s, buf, read_size);
            protobuf_set(self, PACKET_LENGTH, 0);
        } else {
            length = (uint32_t) read_size;
            protobuf_set(self, PACKET_LENGTH, length);
        }
        protobuf_set(self, PACKET_ERRORCODE, AAOS_OK);
    }
    return ret;
}

static int
Serial_execute_unload(struct Serial *self)
{
    uint16_t index;
    void *serial;
    
    protobuf_get(self, PACKET_INDEX, &index);
    
    if (index == 0) {
        uint32_t length;
        char *s;
        int idx;
        protobuf_get(self, PACKET_LENGTH, &length);
        if (length == 0) {
            protobuf_get(self, PACKET_STR, &s);
        } else {
            protobuf_get(self, PACKET_BUF, &s, NULL);
        }
        if ((get_index_by_name(s, &idx)) != AAOS_OK) {
            get_index_by_path(s, &idx);
        }
        index = (uint16_t) idx;
        protobuf_set(self, PACKET_INDEX, &index);
        if ((serial = get_serial_by_name(s)) == NULL && (serial = get_serial_by_path(s)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    } else {
        if ((serial = get_serial_by_index((int) index)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    }
    
    return __serial_unload(serial);
}

static int
Serial_execute_register(struct Serial *self)
{
    void *protobuf = self->_.protobuf;
    
    uint16_t index;
    void *serial;
    double timeout;
    int ret;
    
    protobuf_get(protobuf, PACKET_INDEX, &index);
    protobuf_get(protobuf, PACKET_DF0, &timeout);
    
    if ((serial = get_serial_by_index((int) index)) == NULL) {
        protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
        protobuf_set(self, PACKET_LENGTH, 0);
        return AAOS_ENOTFOUND;
    }
    
    if (__serial_inspect(serial) != AAOS_OK) {
        ret = __serial_wait(serial, timeout);
        protobuf_set(self, PACKET_ERRORCODE, AAOS_ETIMEDOUT);
        protobuf_set(self, PACKET_LENGTH, 0);
        return ret;
    }
    
    return AAOS_OK;
}

static int
Serial_execute_reload(struct Serial *self)
{
    uint16_t index;
    void *serial;
    
    protobuf_get(self, PACKET_INDEX, &index);
    
    if (index == 0) {
        uint32_t length;
        char *s;
        int idx;
        protobuf_get(self, PACKET_LENGTH, &length);
        if (length == 0) {
            protobuf_get(self, PACKET_STR, &s);
        } else {
            protobuf_get(self, PACKET_BUF, &s, NULL);
        }
        if ((get_index_by_name(s, &idx)) != AAOS_OK) {
            get_index_by_path(s, &idx);
        }
        index = (uint16_t) idx;
        protobuf_set(self, PACKET_INDEX, &index);
        if ((serial = get_serial_by_name(s)) == NULL && (serial = get_serial_by_path(s)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    } else {
        if ((serial = get_serial_by_index((int) index)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    }
    
    return __serial_reload(serial);
}

static int
Serial_execute_load(struct Serial *self)
{
    uint16_t index;
    void *serial;
    
    protobuf_get(self, PACKET_INDEX, &index);
    
    if (index == 0) {
        uint32_t length;
        char *s;
        int idx;
        protobuf_get(self, PACKET_LENGTH, &length);
        if (length == 0) {
            protobuf_get(self, PACKET_STR, &s);
        } else {
            protobuf_get(self, PACKET_BUF, &s, NULL);
        }
        if ((get_index_by_name(s, &idx)) != AAOS_OK) {
            get_index_by_path(s, &idx);
        }
        index = (uint16_t) idx;
        protobuf_set(self, PACKET_INDEX, &index);
        if ((serial = get_serial_by_name(s)) == NULL && (serial = get_serial_by_path(s)) == NULL) {
            /*
             * create a new obj
             */
        }
    } else {
        if ((serial = get_serial_by_index((int) index)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    }
    
    return __serial_load(serial);
}

static int
Serial_execute_inspect(struct Serial *self)
{
    uint16_t index;
    void *serial;
    
    protobuf_get(self, PACKET_INDEX, &index);
    
    if (index == 0) {
        uint32_t length;
        char *s;
        int idx;
        protobuf_get(self, PACKET_LENGTH, &length);
        if (length == 0) {
            protobuf_get(self, PACKET_STR, &s);
        } else {
            protobuf_get(self, PACKET_BUF, &s, NULL);
        }
        if ((get_index_by_name(s, &idx)) != AAOS_OK) {
            get_index_by_path(s, &idx);
        }
        index = (uint16_t) idx;
        protobuf_set(self, PACKET_INDEX, &index);
        if ((serial = get_serial_by_name(s)) == NULL && (serial = get_serial_by_path(s)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    } else {
        if ((serial = get_serial_by_index((int) index)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
    }
    
    return __serial_inspect(serial);
}

/*
 * if serial command is illegal
 */
static int
Serial_execute_default(struct Serial *self)
{
    return AAOS_EBADCMD;
}

static int
Serial_execute_get_index_by_name(struct Serial *self)
{
   
    char *name;
    int index, ret;
    uint16_t idx;
    uint32_t length;

    protobuf_get(self, PACKET_LENGTH, &length);
    /*
     * if lenght == 0, use str field.
     */
    if (length == 0) {
        protobuf_get(self, PACKET_STR, &name);
    } else {
        protobuf_get(self, PACKET_BUF, &name, NULL);
    }
    
    if ((ret = get_index_by_name(name, &index)) != AAOS_OK) {
        return ret;
    } else {
        idx = (uint16_t) index;
        protobuf_set(self, PACKET_INDEX, idx);
        protobuf_set(self, PACKET_LENGTH, 0);
    }

    return AAOS_OK;
}

static int
Serial_execute_get_index_by_path(struct Serial *self)
{
    void *protobuf = self->_.protobuf;
    char *path;
    int index, ret;
    uint32_t length;
    
    protobuf_get(protobuf, PACKET_LENGTH, &length);
    if (length == 0) {
        protobuf_get(protobuf, PACKET_STR, &path);
    } else {
        protobuf_get(protobuf, PACKET_BUF, &path, NULL);
    }
    if ((ret = get_index_by_path(path, &index)) != AAOS_OK) {
        
        return ret;
    } else {
        uint16_t idx;
        idx = (uint16_t) index;
        protobuf_set(self, PACKET_INDEX, idx);
        protobuf_set(protobuf, PACKET_LENGTH, 0);
    }
    
    return AAOS_OK;
}


static int
Serial_execute(void *_self)
{
    struct Serial *self = cast(Serial(), _self);
    uint16_t command;
    
    if (Serial_protocol_check(self) != AAOS_OK) {
        return AAOS_EPROTOWRONG;
    }
    protobuf_get(self, PACKET_COMMAND, &command);
    switch (command) {
        case SERIAL_COMMAND_GET_INDEX_BY_NAME:
            return Serial_execute_get_index_by_name(self);
            break;
        case SERIAL_COMMAND_GET_INDEX_BY_PATH:
            return Serial_execute_get_index_by_path(self);
            break;
        case SERIAL_COMMAND_INFO:
            return Serial_execute_info(self);
            break;
        case SERIAL_COMMAND_RAW:
            return Serial_execute_raw(self);
            break;
        case SERIAL_COMMAND_UNLOAD:
            return Serial_execute_unload(self);
            break;
        case SERIAL_COMMAND_LOAD:
            return Serial_execute_load(self);
            break;
        case SERIAL_COMMAND_RELOAD:
            return Serial_execute_reload(self);
            break;
        case SYSTEM_COMMAND_INSPECT:
            return Serial_execute_inspect(self);
            break;
        case SYSTEM_COMMAND_REGISTER:
            return Serial_execute_register(self);
            break;
        default:
            return Serial_execute_default(self);
            break;
    }
}

static void *
Serial_ctor(void *_self, va_list *app)
{
    struct Serial *self = super_ctor(Serial(), _self, app);
    
    self->_._vtab = serial_virtual_table();
    
    return (void *) self;
}

static void *
Serial_dtor(void *_self)
{
    //struct RPC *self = cast(RPC(), _self);
    
    return super_dtor(Serial(), _self);
}

static void *
SerialClass_ctor(void *_self, va_list *app)
{
    struct SerialClass *self = super_ctor(SerialClass(), _self, app);
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
        
        if (selector == (Method) serial_get_index_by_name) {
            if (tag) {
                self->get_index_by_name.tag = tag;
                self->get_index_by_name.selector = selector;
            }
            self->get_index_by_name.method = method;
            continue;
        }
        if (selector == (Method) serial_get_index_by_path) {
            if (tag) {
                self->get_index_by_path.tag = tag;
                self->get_index_by_path.selector = selector;
            }
            self->get_index_by_path.method = method;
            continue;
        }
        if (selector == (Method) serial_raw) {
            if (tag) {
                self->raw.tag = tag;
                self->raw.selector = selector;
            }
            self->raw.method = method;
            continue;
        }
        
        if (selector == (Method) serial_inspect) {
            if (tag) {
                self->inspect.tag = tag;
                self->inspect.selector = selector;
            }
            self->inspect.method = method;
            continue;
        }
        if (selector == (Method) serial_register) {
            if (tag) {
                self->reg.tag = tag;
                self->reg.selector = selector;
            }
            self->reg.method = method;
            continue;
        }
    }
    
#ifdef va_copy
    va_end(ap);
#endif
    
    self->_.execute.method = (Method) 0;
    
    return self;
}

static void *_SerialClass;

static void
SerialClass_destroy(void)
{
    free((void *) _SerialClass);
}

static void
SerialClass_initialize(void)
{
    _SerialClass = new(RPCClass(), "SerialClass", RPCClass(), sizeof(struct SerialClass),
                        ctor, "ctor", SerialClass_ctor,
                        (void *) 0);
    
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(SerialClass_destroy);
#endif
}

const void *
SerialClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, SerialClass_initialize);
#endif
    
    return _SerialClass;
}

static void *_Serial;

static void
Serial_destroy(void)
{
    free((void *) _Serial);
}

static void
Serial_initialize(void)
{
    _Serial = new(SerialClass(), "Serial", RPC(), sizeof(struct Serial),
                  ctor, "ctor", Serial_ctor,
                  dtor, "dtor", Serial_dtor,
                  serial_raw, "raw", Serial_raw,
                  serial_inspect, "inspect", Serial_inspect,
                  serial_register, "register", Serial_register,
                  serial_info, "info", Serial_info,
                  serial_get_index_by_name, "get_index_by_name", Serial_get_index_by_name,
                  serial_get_index_by_path, "get_index_by_path", Serial_get_index_by_path,
                  (void *) 0);
    
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(Serial_destroy);
#endif

}

const void *
Serial(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, Serial_initialize);
#endif
    
    return _Serial;
}

static const void *_serial_virtual_table;

static void
serial_virtual_table_destroy(void)
{
    delete((void *) _serial_virtual_table);
}

static void
serial_virtual_table_initialize(void)
{
    _serial_virtual_table = new(RPCVirtualTable(),
                                rpc_execute, "execute", Serial_execute,
                                (void *)0);
    
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(serial_virtual_table_destroy);
#endif
}

static const void *
serial_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, serial_virtual_table_initialize);
#endif
    
    return _serial_virtual_table;
}

/*
 * Serial client class
 */

static const void *serial_client_virtual_table(void);

static
int SerialClient_connect(void *_self, void **client)
{
    struct SerialClient *self = cast(SerialClient(), _self);
    
    int cfd = Tcp_connect(self->_._.address, self->_._.port, NULL, NULL);
    
    if (cfd < 0) {
        *client = NULL;
        return AAOS_ERROR;
    } else {
        *client = new(Serial(), cfd);
    }
    
    protobuf_set(*client, PACKET_PROTOCOL, PROTO_SERIAL);
    
    return AAOS_OK;
}

static void *
SerialClient_ctor(void *_self, va_list *app)
{
    struct SerialClient *self = super_ctor(SerialClient(), _self, app);
    
    self->_._vtab = serial_client_virtual_table();
    
    return (void *) self;
}

static void *
SerialClient_dtor(void *_self)
{
    //struct RPC *self = cast(RPC(), _self);
    
    return super_dtor(SerialClient(), _self);
}

static void *
SerialClientClass_ctor(void *_self, va_list *app)
{
    struct SerialClientClass *self = super_ctor(SerialClientClass(), _self, app);
    
    self->_.connect.method = (Method) 0;
    
    return self;
}

static void *_SerialClientClass;

static void
SerialClientClass_destroy(void)
{
    free((void *) _SerialClientClass);
}

static void
SerialClientClass_initialize(void)
{
    _SerialClientClass = new(RPCClientClass(), "SerialClientClass", RPCClientClass(), sizeof(struct SerialClientClass),
                             ctor, "ctor", SerialClientClass_ctor,
                             (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(SerialClientClass_destroy);
#endif
}

const void *
SerialClientClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, SerialClientClass_initialize);
#endif
    
    return _SerialClientClass;
}

static void *_SerialClient;

static void
SerialClient_destroy(void)
{
    free((void *) _SerialClient);
}

static void
SerialClient_initialize(void)
{
    _SerialClient = new(SerialClientClass(), "SerialClient", RPCClient(), sizeof(struct SerialClient),
                        ctor, "ctor", SerialClient_ctor,
                        dtor, "dtor", SerialClient_dtor,
                        (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(SerialClient_destroy);
#endif
}

const void *
SerialClient(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, SerialClient_initialize);
#endif
    
    return _SerialClient;
}

static const void *_serial_client_virtual_table;

static void
serial_client_virtual_table_destroy(void)
{
    delete((void *) _serial_client_virtual_table);
}

static void
serial_client_virtual_table_initialize(void)
{
    _serial_client_virtual_table = new(RPCClientVirtualTable(),
                                       rpc_client_connect, "connect", SerialClient_connect,
                                       (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(serial_client_virtual_table_destroy);
#endif
}

static const void *
serial_client_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, serial_client_virtual_table_initialize);
#endif
    
    return _serial_client_virtual_table;
}

/*
 * Serial server class
 */

static const void *serial_server_virtual_table(void);

static int
SerialServer_accept(void *_self, void **client)
{
    struct RPCServer *self = cast(RPCServer(), _self);
    
    int lfd, cfd;
    
    lfd = tcp_server_get_lfd(self);
    cfd = Accept(lfd, NULL, NULL);
    if (cfd < 0) {
        *client = NULL;
        return AAOS_ERROR;
    } else {
        if ((*client = new(Serial(), cfd)) == NULL) {
            Close(cfd);
            return AAOS_ERROR;
        }
        return AAOS_OK;
    }
}

static void *
SerialServer_ctor(void *_self, va_list *app)
{
    struct SerialServer *self = super_ctor(SerialServer(), _self, app);
    
    self->_._vtab = serial_server_virtual_table();
    
    return (void *) self;
}

static void *
SerialServer_dtor(void *_self)
{
    //struct RPC *self = cast(RPC(), _self);
    
    return super_dtor(SerialServer(), _self);
}

static void *
SerialServerClass_ctor(void *_self, va_list *app)
{
    struct SerialServerClass *self = super_ctor(SerialServerClass(), _self, app);
    
    self->_.accept.method = (Method) 0;
    
    return self;
}

static void *_SerialServerClass;

static void
SerialServerClass_destroy(void)
{
    free((void *) _SerialServerClass);
}

static void
SerialServerClass_initialize(void)
{
    _SerialServerClass = new(RPCServerClass(), "SerialServerClass", RPCServerClass(), sizeof(struct SerialServerClass),
                             ctor, "ctor", SerialServerClass_ctor,
                             (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(SerialServerClass_destroy);
#endif
}

const void *
SerialServerClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, SerialServerClass_initialize);
#endif
    
    return _SerialServerClass;
}

static void *_SerialServer;

static void
SerialServer_destroy(void)
{
    free((void *) _SerialServer);
}

static void
SerialServer_initialize(void)
{
    _SerialServer = new(SerialServerClass(), "SerialServer", RPCServer(), sizeof(struct SerialServer),
                        ctor, "ctor", SerialServer_ctor,
                        dtor, "dtor", SerialServer_dtor,
                        (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(SerialServer_destroy);
#endif
}

const void *
SerialServer(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, SerialServer_initialize);
#endif
    
    return _SerialServer;
}

static const void *_serial_server_virtual_table;

static void
serial_server_virtual_table_destroy(void)
{
    delete((void *) _serial_server_virtual_table);
}

static void
serial_server_virtual_table_initialize(void)
{

    _serial_server_virtual_table = new(RPCServerVirtualTable(),
                                       rpc_server_accept, "accept", SerialServer_accept,
                                       (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(serial_server_virtual_table_destroy);
#endif
}

static const void *
serial_server_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, serial_server_virtual_table_initialize);
#endif
    
    return _serial_server_virtual_table;
}

#ifdef _USE_COMPILER_ATTRIBUTION_
static void __destructor__(void) __attribute__ ((destructor(_SERIAL_RPC_PRIORITY_)));

static void
__destructor__(void)
{
    SerialServer_destroy();
    SerialServerClass_destroy();
    serial_server_virtual_table_destroy();
    SerialClient_destroy();
    SerialClientClass_destroy();
    serial_client_virtual_table_destroy();
    Serial_destroy();
    SerialClass_destroy();
    serial_virtual_table_destroy();
}

static void __constructor__(void) __attribute__ ((constructor(_SERIAL_RPC_PRIORITY_)));

static void
__constructor__(void)
{
    serial_virtual_table_initialize();
    SerialClass_initialize();
    Serial_initialize();
    serial_client_virtual_table_initialize();
    SerialClientClass_initialize();
    SerialClient_initialize();
    serial_server_virtual_table_initialize();
    SerialServerClass_initialize();
    SerialServer_initialize();
}
#endif

/*
 * feed dog function.
 */

static void *
feed_dog_thr(void *arg)
{
    double seconds = *((double *) arg);
    free(arg);
    Pthread_detach(pthread_self());
    size_t i;
    
    for (; ;) {
        Nanosleep(seconds);
        for (i = 0; i < n_serial; i++) {
            __serial_feed_dog(serials[i]);
        }
    }
    return NULL;
}

void
start_feed_dog(double seconds)
{
    if (seconds < 0.) {
        return;
    }
    double *arg = (double *) Malloc(sizeof(double));
    *arg = seconds;
    pthread_t tid;
    Pthread_create(&tid, NULL, feed_dog_thr, arg);
}

/*
 * Compiler-dependant initializer.
 */
#ifdef _USE_COMPILER_ATTRIBUTION_
static void __destructor__(void) __attribute__ ((destructor(_SERIAL_RPC_PRIORITY_)));

static void
__destructor__(void)
{
    serial_virtual_table_initialize();
    SerialClass_initialize();
    Serial_initialize();
    serial_server_virtual_table_initialize();
    SerialServerClass_initialize();
    SerialServer_initialize();
    serial_client_virtual_table_initialize();
    SerialClientClass_initialize();
    SerialClient_initialize();
}

static void __constructor__(void) __attribute__ ((constructor(_SERIAL_RPC_PRIORITY_)));

static void
__constructor__(void)
{
    SerialClient_destroy();
    SerialClientClass_destroy();
    serial_client_virtual_table_destroy();
    SerialServer_destroy();
    SerialServerClass_destroy();
    serial_server_virtual_table_destroy();
    Serial_destroy();
    SerialClass_destroy();
    serial_virtual_table_destroy();
}
#endif
