//
//  dome_rpc.c
//  AAOS
//
//  Created by huyi on 2025/4/29.
//

#include "adt.h"
#include "def.h"
#include "protocol.h"
#include "dome.h"
#include "dome_rpc.h"
#include "dome_rpc_r.h"
#include "wrapper.h"


static bool
dome_find_by_name_if(void *dome, va_list *app)
{
    const char *name, *myname;
    
    myname = __dome_get_name(dome);
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
    
    for (i = 0; i < n_dome; i++) {
        s = __dome_get_name(domes[i]);
        if (strcmp(name, s) == 0) {
            *index = (int) i + 1;
            return AAOS_OK;
        }
    }
    
    if (*index == 0 && dome_list != NULL) {
        if (threadsafe_list_find_first_if(dome_list, dome_find_by_name_if, name) != NULL) {
            return AAOS_OK;
        }
    }

    return AAOS_ENOTFOUND;
}

static void *
get_dome_by_name(const char *name)
{
    size_t i;
    const char *s;
    void *dome = NULL;
    
    for (i = 0; i < n_dome; i++) {
        s = __dome_get_name(domes[i]);
        if (strcmp(name, s) == 0) {
            return  domes[i];
        }
    }
    
    if (dome == NULL) {
        dome = threadsafe_list_find_first_if(dome_list, dome_find_by_name_if, name);
    }
    
    return dome;
}

static void *
get_dome_by_index(int index)
{
    if (index > 0 && index <= n_dome) {
        return domes[index - 1];
    } else {
        return NULL;
    }
}

inline static int
Dome_protocol_check(void *_self)
{
    uint16_t protocol;
    protobuf_get(_self, PACKET_PROTOCOL, &protocol);
    if (protocol != PROTO_DOME && protocol != PROTO_SYSTEM) {
        protobuf_set(_self, PACKET_ERRORCODE, AAOS_EPROTOWRONG);
        protobuf_set(_self, PACKET_LENGTH, 0);
        return AAOS_EPROTOWRONG;
    } else {
        return AAOS_OK;
    }
}

int
dome_get_index_by_name(void *_self, const char *name)
{
    const struct DomeClass *class = (const struct DomeClass *) classOf(_self);
    
    if (isOf(class, DomeClass()) && class->get_index_by_name.method) {
        return ((int (*)(void *, const char *)) class->get_index_by_name.method)(_self, name);
    } else {
        int result;
        forward(_self, &result, (Method) dome_get_index_by_name, "dome_index_by_name", _self, name);
        return result;
    }
}

static int
Dome_get_index_by_name(void *_self, const char *name)
{
    struct Dome *self = cast(Dome(), _self);
    
    size_t length;
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_DOME);
    protobuf_set(self, PACKET_COMMAND, DOME_COMMAND_GET_INDEX_BY_NAME);
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
dome_get_name_by_index(void *_self, uint16_t index, char *name, size_t size)
{
    const struct DomeClass *class = (const struct DomeClass *) classOf(_self);
    
    if (isOf(class, DomeClass()) && class->get_name_by_index.method) {
        return ((int (*)(void *, uint16_t, char *, size_t)) class->get_name_by_index.method)(_self, index, name, size);
    } else {
        int result;
        forward(_self, &result, (Method) dome_get_name_by_index, "get_name_by_index", _self, index, name, size);
        return result;
    }
}

static int
Dome_get_name_by_index(void *_self, uint16_t index, char *name, size_t size)
{
    struct Dome *self = cast(Dome(), _self);
	
    int ret;
    uint32_t length;
    char *buf;
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_DOME);
    protobuf_set(self, PACKET_COMMAND, DOME_COMMAND_GET_NAME_BY_INDEX);
    protobuf_set(self, PACKET_INDEX, index);

    if ((ret = rpc_call(self)) == AAOS_OK) {
        protobuf_get(self, PACKET_LENGTH, &length);
        if (length == 0) {
            protobuf_get(self, PACKET_STR, &buf);
        } else {
            protobuf_get(self, PACKET_BUF, &buf, NULL);
        }
        snprintf(name, size, "%s", buf);
    }
	
    return ret;
}

int
dome_init(void *_self)
{
	const struct DomeClass *class = (const struct DomeClass *) classOf(_self);
    
    if (isOf(class, DomeClass()) && class->init.method) {
        return ((int (*)(void *)) class->init.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) dome_init, "init", _self);
        return result;
    }
}

static int
Dome_init(void *_self)
{
    struct Dome *self = cast(Dome(), _self);
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_DOME);
    protobuf_set(self, PACKET_COMMAND, DOME_COMMAND_INIT);
    
    return rpc_call(self);
}

int
dome_open_window(void *_self)
{
	const struct DomeClass *class = (const struct DomeClass *) classOf(_self);
    
    if (isOf(class, DomeClass()) && class->open_window.method) {
        return ((int (*)(void *)) class->open_window.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) dome_open_window, "open_window", _self);
        return result;
    }
}

static int
Dome_open_window(void *_self)
{
    struct Dome *self = cast(Dome(), _self);
	
    protobuf_set(self, PACKET_PROTOCOL, PROTO_DOME);
    protobuf_set(self, PACKET_COMMAND, DOME_COMMAND_OPEN_WINDOW);
    
    return rpc_call(self);
}

int
dome_close_window(void *_self)
{
	const struct DomeClass *class = (const struct DomeClass *) classOf(_self);
    
    if (isOf(class, DomeClass()) && class->close_window.method) {
        return ((int (*)(void *)) class->close_window.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) dome_close_window, "close_window", _self);
        return result;
    }
}

static int
Dome_close_window(void *_self)
{
    struct Dome *self = cast(Dome(), _self);
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_DOME);
    protobuf_set(self, PACKET_COMMAND, DOME_COMMAND_CLOSE_WINDOW);
    
    return rpc_call(self);
}

int
dome_stop_window(void *_self)
{
	const struct DomeClass *class = (const struct DomeClass *) classOf(_self);
    
    if (isOf(class, DomeClass()) && class->stop_window.method) {
        return ((int (*)(void *)) class->stop_window.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) dome_stop_window, "stop_window", _self);
        return result;
    }
}

static int
Dome_stop_window(void *_self)
{
    struct Dome *self = cast(Dome(), _self);
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_DOME);
    protobuf_set(self, PACKET_COMMAND, DOME_COMMAND_STOP_WINDOW);
    
    return rpc_call(self);
}

int
dome_inspect(void *_self)
{
	const struct DomeClass *class = (const struct DomeClass *) classOf(_self);
    
    if (isOf(class, DomeClass()) && class->inspect.method) {
        return ((int (*)(void *)) class->inspect.method)(_self);
    } else {
        int result;
        forward(_self, &result, (Method) dome_inspect, "inspect", _self);
        return result;
    }
}

static int
Dome_inspect(void *_self)
{
    struct Dome *self = cast(Dome(), _self);
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_DOME);
    protobuf_set(self, PACKET_COMMAND, DOME_COMMAND_INSPECT);
    
    return rpc_call(self);
}

int
dome_register(void *_self, double timeout)
{
	const struct DomeClass *class = (const struct DomeClass *) classOf(_self);
    
    if (isOf(class, DomeClass()) && class->reg.method) {
        return ((int (*)(void *, double)) class->reg.method)(_self, timeout);
    } else {
        int result;
        forward(_self, &result, (Method) dome_register, "register", _self, timeout);
        return result;
    }
}

static int
Dome_register(void *_self, double timeout)
{
    struct Dome *self = cast(Dome(), _self);
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_DOME);
    protobuf_set(self, PACKET_COMMAND, DOME_COMMAND_REGISTER);
    protobuf_set(self, PACKET_DF0, timeout);
    
    return rpc_call(self);
}

int 
dome_status(void *_self, void *status_buffer, size_t status_size, size_t *status_length)
{
	const struct DomeClass *class = (const struct DomeClass *) classOf(_self);
    
    if (isOf(class, DomeClass()) && class->status.method) {
        return ((int (*)(void *, void *, size_t, size_t *)) class->status.method)(_self, status_buffer, status_size, status_length);
    } else {
        int result;
        forward(_self, &result, (Method) dome_status, "status", _self, status_buffer, status_size, status_length);
        return result;
    }
}

static int
Dome_status(void *_self, void *status_buffer, size_t status_size, size_t *status_length)
{
    struct Dome *self = cast(Dome(), _self);
    
    int ret;
    void *buf;
    size_t length;

    protobuf_set(self, PACKET_PROTOCOL, PROTO_DOME);
    protobuf_set(self, PACKET_COMMAND, DOME_COMMAND_STATUS);
	
    if ((ret = rpc_call(self)) == AAOS_OK) {
	protobuf_get(self, PACKET_BUF, &buf, &length);
	if (buf != status_buffer) {
	    memcpy(status_buffer, buf, min(length, status_size));
	}
	if (status_length != NULL) {
	    *status_length = min(length, status_size);
	}
    }
    
    return ret;
}

int
dome_get_window_position(void *_self, double *position)
{
	const struct DomeClass *class = (const struct DomeClass *) classOf(_self);
    
    if (isOf(class, DomeClass()) && class->get_window_position.method) {
        return ((int (*)(void *, double *)) class->get_window_position.method)(_self, position);
    } else {
        int result;
        forward(_self, &result, (Method) dome_get_window_position, "get_window_position", _self, position);
        return result;
    }
}

static int
Dome_get_window_position(void *_self, double *position)
{
    struct Dome *self = cast(Dome(), _self);
    
	int ret;
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_DOME);
    protobuf_set(self, PACKET_COMMAND, DOME_COMMAND_GET_WINDOW_POSITION);
    
    if ((ret = rpc_call(self)) == AAOS_OK) {
		protobuf_get(self, PACKET_DF0, position);
    }
	
	return ret;
}

int
dome_get_window_open_speed(void *_self, double *speed)
{
	const struct DomeClass *class = (const struct DomeClass *) classOf(_self);
    
    if (isOf(class, DomeClass()) && class->get_window_open_speed.method) {
        return ((int (*)(void *, double *)) class->get_window_open_speed.method)(_self, speed);
    } else {
        int result;
        forward(_self, &result, (Method) dome_get_window_open_speed, "get_window_open_speed", _self, speed);
        return result;
    }
}

static int
Dome_get_window_open_speed(void *_self, double *speed)
{
    struct Dome *self = cast(Dome(), _self);
    
	int ret;
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_DOME);
    protobuf_set(self, PACKET_COMMAND, DOME_COMMAND_GET_WINDOW_OPEN_SPEED);
						
    if ((ret = rpc_call(self)) == AAOS_OK) {
		protobuf_get(self, PACKET_DF0, speed);
    }
	
	return ret;
}

int
dome_set_window_open_speed(void *_self, double speed)
{
	const struct DomeClass *class = (const struct DomeClass *) classOf(_self);
    
    if (isOf(class, DomeClass()) && class->set_window_open_speed.method) {
        return ((int (*)(void *, double)) class->set_window_open_speed.method)(_self, speed);
    } else {
        int result;
        forward(_self, &result, (Method) dome_set_window_open_speed, "set_window_open_speed", _self, speed);
        return result;
    }
}

static int
Dome_set_window_open_speed(void *_self, double speed)
{
    struct Dome *self = cast(Dome(), _self);
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_DOME);
    protobuf_set(self, PACKET_COMMAND, DOME_COMMAND_SET_WINDOW_OPEN_SPEED);
	protobuf_set(self, PACKET_DF0, speed);
    
	return rpc_call(self);
}

int
dome_get_window_close_speed(void *_self, double *speed)
{
	const struct DomeClass *class = (const struct DomeClass *) classOf(_self);
    
    if (isOf(class, DomeClass()) && class->get_window_close_speed.method) {
        return ((int (*)(void *, double *)) class->get_window_close_speed.method)(_self, speed);
    } else {
        int result;
        forward(_self, &result, (Method) dome_get_window_close_speed, "get_window_close_speed", _self, speed);
        return result;
    }
}

static int
Dome_get_window_close_speed(void *_self, double *speed)
{
    struct Dome *self = cast(Dome(), _self);
    
	int ret;
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_DOME);
    protobuf_set(self, PACKET_COMMAND, DOME_COMMAND_GET_WINDOW_CLOSE_SPEED);
						
    if ((ret = rpc_call(self)) == AAOS_OK) {
		protobuf_get(self, PACKET_DF0, speed);
    }
	
	return ret;
}

int
dome_set_window_close_speed(void *_self, double speed)
{
	const struct DomeClass *class = (const struct DomeClass *) classOf(_self);
    
    if (isOf(class, DomeClass()) && class->set_window_close_speed.method) {
        return ((int (*)(void *, double)) class->set_window_close_speed.method)(_self, speed);
    } else {
        int result;
        forward(_self, &result, (Method) dome_set_window_close_speed, "set_window_close_speed", _self, speed);
        return result;
    }
}

static int
Dome_set_window_close_speed(void *_self, double speed)
{
    struct Dome *self = cast(Dome(), _self);
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_DOME);
    protobuf_set(self, PACKET_COMMAND, DOME_COMMAND_SET_WINDOW_CLOSE_SPEED);
	protobuf_set(self, PACKET_DF0, speed);
    
	return rpc_call(self);
}

int
dome_raw(void *_self, const void *write_buffer, size_t write_buffer_size, size_t *write_size, void *read_buffer, size_t read_buffer_size, size_t *read_size)
{
	const struct DomeClass *class = (const struct DomeClass *) classOf(_self);
    
    if (isOf(class, DomeClass()) && class->raw.method) {
        return ((int (*)(void *, const void *, size_t, size_t *, void *, size_t, size_t *)) class->raw.method)(_self, write_buffer, write_buffer_size, write_size, read_buffer, read_buffer_size, read_size);
    } else {
        int result;
        forward(_self, &result, (Method) dome_raw, "raw", _self, write_buffer, write_buffer_size, write_size, read_buffer, read_buffer_size, read_size);
        return result;
    }
}

static int 
Dome_raw(void *_self, const void *write_buffer, size_t write_buffer_size, size_t *write_size, void *read_buffer, size_t read_buffer_size, size_t *read_size)
{
	struct Dome *self = cast(Dome(), _self);
	
    void *buf;
    size_t size; 
	uint32_t length;
    int ret;
    
    protobuf_set(self, PACKET_PROTOCOL, PROTO_DOME);
    protobuf_set(self, PACKET_COMMAND, DOME_COMMAND_RAW);
    protobuf_set(self, PACKET_BUF, write_buffer, write_buffer_size);
    
    if ((ret = rpc_call(self)) != AAOS_OK) {
        return ret;
    }

    protobuf_get(self, PACKET_LENGTH, &length);
    
    if (length == 0) {
        protobuf_get(self, PACKET_STR, &buf);
        size = PACKETPARAMETERSIZE;
    } else {
        protobuf_get(self, PACKET_BUF, &buf, &size);
    }
    
    if (read_buffer != buf) {
		memcpy(read_buffer, buf, min(read_buffer_size, size));	
    }
	if (read_size != NULL) {	
		*read_size = min(read_buffer_size, size);
	}
    
    return AAOS_OK;
}

static int
Dome_execute_get_index_by_name(struct Dome *self)
{
    char *name;
    int index, ret;
    uint32_t length;
    
    protobuf_get(self, PACKET_LENGTH, &length);
    
    if (length == 0) {
        protobuf_get(self, PACKET_STR, &name);
    } else {
        protobuf_get(self, PACKET_BUF, &name, NULL);
    }
    
    if ((ret = get_index_by_name(name, &index)) != AAOS_OK) {
        return ret;
    } else {
        uint16_t idx = (uint16_t) index;
        protobuf_set(self, PACKET_INDEX, idx);
        protobuf_set(self, PACKET_LENGTH, 0);
    }
    
    return AAOS_OK;
}

static int
Dome_execute_init(struct Dome *self)
{
    uint16_t index;
    void *dome;
    
    protobuf_get(self, PACKET_INDEX, &index);

    if ((dome = get_dome_by_index(index)) == NULL) {
        return AAOS_ENOTFOUND;
    }
        
    protobuf_set(self, PACKET_LENGTH, 0);

    return __dome_init(dome);
}

static int
Dome_execute_open_window(struct Dome *self)
{
    uint16_t index;
    void *dome;
    
    protobuf_get(self, PACKET_INDEX, &index);

    if ((dome = get_dome_by_index(index)) == NULL) {
        return AAOS_ENOTFOUND;
    }
        
    protobuf_set(self, PACKET_LENGTH, 0);

    return __dome_open_window(dome);
}

static int
Dome_execute_close_window(struct Dome *self)
{
    uint16_t index;
    void *dome;
    
    protobuf_get(self, PACKET_INDEX, &index);

    if ((dome = get_dome_by_index(index)) == NULL) {
        return AAOS_ENOTFOUND;
    }
        
    protobuf_set(self, PACKET_LENGTH, 0);

    return __dome_close_window(dome);
}

static int
Dome_execute_stop_window(struct Dome *self)
{
    uint16_t index;
    void *dome;
    
    protobuf_get(self, PACKET_INDEX, &index);

    if ((dome = get_dome_by_index(index)) == NULL) {
        return AAOS_ENOTFOUND;
    }
        
    protobuf_set(self, PACKET_LENGTH, 0);

    return __dome_stop_window(dome);
}

static int
Dome_execute_inspect(struct Dome *self)
{
    uint16_t index;
    void *dome;
    
    protobuf_get(self, PACKET_INDEX, &index);

    if ((dome = get_dome_by_index(index)) == NULL) {
        return AAOS_ENOTFOUND;
    }
        
    protobuf_set(self, PACKET_LENGTH, 0);

    return __dome_inspect(dome);
}

static int
Dome_execute_register(struct Dome *self)
{
    uint16_t index;
    void *dome;
    double timeout;
    
    protobuf_get(self, PACKET_INDEX, &index);

    if ((dome = get_dome_by_index(index)) == NULL) {
        return AAOS_ENOTFOUND;
    }
	
    protobuf_get(self, PACKET_DF0, &timeout);
    protobuf_set(self, PACKET_LENGTH, 0);

    return __dome_register(dome, timeout);
}

static int
Dome_execute_status(struct Dome *self)
{
    
    int ret;
    void *dome;
    void *buf;
    size_t size, length;
    uint32_t len;
    uint16_t index;
    
    protobuf_get(self, PACKET_INDEX, &index);
    
    if ((dome = get_dome_by_index(index)) == NULL) {
        return AAOS_ENOTFOUND;
    }
    
    size = protobuf_payload(self);
    protobuf_get(self, PACKET_BUF, &buf, NULL);
    
    if ((ret = __dome_status(dome, buf, size, &length)) == AAOS_OK) {
        len = (uint32_t) length;
        protobuf_set(self, PACKET_LENGTH, len);
    }
    
    return ret;
}

static int
Dome_execute_raw(struct Dome *self)
{
    char *command;
    int ret;
    uint16_t index;
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
    
    if (index == 0) {
        /*
         * If index is zero, means STR field contains device name or device path, payload must none zero.
         */
        if (length == 0) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_EBADCMD);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_EBADCMD;
        } else {
            char *s, *buf;
            void *dome;
            int idx;
            size_t payload, read_size;
            protobuf_get(self, PACKET_STR, &s);
            if ((get_index_by_name(s, &idx)) != AAOS_OK) {
                protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
                protobuf_set(self, PACKET_LENGTH, 0);
                return AAOS_ENOTFOUND;
            }
            index = (uint16_t) idx;
            protobuf_set(self, PACKET_INDEX, &index);
            dome = get_dome_by_index(idx);
            protobuf_get(self, PACKET_BUF, &buf, NULL);
            payload = protobuf_payload(self);
            if ((ret = __dome_raw(dome, command, strlen(command), NULL, buf, payload, &read_size)) != AAOS_OK) {
                return ret;
            }
            /*
             * If STR field is enough, use it.
             */
            if (read_size < PACKETPARAMETERSIZE) {
                memcpy(s, buf, read_size);
                protobuf_set(self, PACKET_LENGTH, 0);
            } else {
                length = (uint32_t) read_size;
                protobuf_set(self, PACKET_LENGTH, length);
            }
        }
    } else {
        char *s, *buf;
        void *dome;
        if ((dome = get_dome_by_index((int) index)) == NULL) {
            protobuf_set(self, PACKET_ERRORCODE, AAOS_ENOTFOUND);
            protobuf_set(self, PACKET_LENGTH, 0);
            return AAOS_ENOTFOUND;
        }
        size_t payload, read_size, write_size;
        protobuf_get(self, PACKET_LENGTH, &length);
        payload = protobuf_payload(self);
        protobuf_get(self, PACKET_BUF, &buf, NULL);
        if ((ret = __dome_raw(dome, command, strlen(command), &write_size, buf, payload, &read_size)) != AAOS_OK) {
            return ret;
        }
        protobuf_set(self, PACKET_ERRORCODE, AAOS_OK);
        if (read_size < PACKETPARAMETERSIZE) {
            protobuf_get(self, PACKET_STR, &s);
            memcpy(s, buf, read_size);
            protobuf_set(self, PACKET_LENGTH, 0);
        } else {
            length = (uint32_t) read_size;
            protobuf_set(self, PACKET_LENGTH, length);
        }
    }
    return ret;
}

static int
Dome_execute_get_window_position(struct Dome *self)
{
    int  ret;
    void *dome;
    double position;
    uint16_t index;
    
    protobuf_get(self, PACKET_INDEX, &index);

    if ((dome = get_dome_by_index(index)) == NULL) {
        return AAOS_ENOTFOUND;
    }

    protobuf_set(self, PACKET_LENGTH, 0);
	
	if ((ret = __dome_get_window_position(dome, &position)) == AAOS_OK) {
		protobuf_set(self, PACKET_DF0, position);
	}

    return ret;
}

static int
Dome_execute_get_window_open_speed(struct Dome *self)
{
    int ret;
    void *dome;
    double speed;
    uint16_t index;
    
    protobuf_get(self, PACKET_INDEX, &index);

    if ((dome = get_dome_by_index(index)) == NULL) {
        return AAOS_ENOTFOUND;
    }

    protobuf_set(self, PACKET_LENGTH, 0);
	
    if ((ret = __dome_get_window_open_speed(dome, &speed)) == AAOS_OK) {
        protobuf_set(self, PACKET_DF0, speed);
    }

    return ret;
}

static int
Dome_execute_set_window_open_speed(struct Dome *self)
{
    int ret;
    void *dome;
    double speed;
    uint16_t index;
    
    protobuf_get(self, PACKET_INDEX, &index);

    if ((dome = get_dome_by_index(index)) == NULL) {
        return AAOS_ENOTFOUND;
    }

    protobuf_get(self, PACKET_DF0, &speed);
    protobuf_set(self, PACKET_LENGTH, 0);
	
    return __dome_set_window_open_speed(dome, speed);
}

static int
Dome_execute_get_window_close_speed(struct Dome *self)
{
    int  ret;
    void *dome;
    double speed;
    uint16_t index;
    
    protobuf_get(self, PACKET_INDEX, &index);

    if ((dome = get_dome_by_index(index)) == NULL) {
        return AAOS_ENOTFOUND;
    }

    protobuf_set(self, PACKET_LENGTH, 0);
	
    if ((ret = __dome_get_window_close_speed(dome, &speed)) == AAOS_OK) {
        protobuf_set(self, PACKET_DF0, speed);
    }

    return ret;
}

static int
Dome_execute_set_window_close_speed(struct Dome *self)
{
    int ret;
    void *dome;
    double speed;
    uint16_t index;
    
    protobuf_get(self, PACKET_INDEX, &index);

    if ((dome = get_dome_by_index(index)) == NULL) {
        return AAOS_ENOTFOUND;
    }

    protobuf_get(self, PACKET_DF0, &speed);
    protobuf_set(self, PACKET_LENGTH, 0);
	
    return __dome_set_window_close_speed(dome, speed);
}

static int
Dome_execute_get_name_by_index(struct Dome *self)
{
    const char *name;
    int ret;
    void *dome;
    double speed;
    uint16_t index;
    
    protobuf_get(self, PACKET_INDEX, &index);

    if ((dome = get_dome_by_index(index)) == NULL) {
        return AAOS_ENOTFOUND;
    }
    
    name = __dome_get_name(dome);
    
    if (strlen(name) + 1 <PACKETPARAMETERSIZE) {
        protobuf_set(self, PACKET_STR, name);
        protobuf_set(self, PACKET_LENGTH, 0);
    } else {
        protobuf_set(self, PACKET_BUF, name, strlen(name) + 1);
    }
    
    return AAOS_OK;
}

static int
Dome_execute_default(struct Dome *self)
{
    return AAOS_EBADCMD;
}

static int
Dome_execute(void *_self)
{
    struct Dome *self = cast(Dome(), _self);
    uint16_t command;
    int ret;

    if (Dome_protocol_check(self) != AAOS_OK) {
        return AAOS_EPROTOWRONG;
    }
    
    protobuf_get(self, PACKET_COMMAND, &command);
    
    switch (command) {
        case DOME_COMMAND_GET_INDEX_BY_NAME:
            ret = Dome_execute_get_index_by_name(self);
            break;
        case DOME_COMMAND_GET_NAME_BY_INDEX:
            ret = Dome_execute_get_name_by_index(self);
            break;
        case DOME_COMMAND_STATUS:
            ret = Dome_execute_status(self);
            break;
        case DOME_COMMAND_INIT:
            ret = Dome_execute_init(self);
            break;
        case DOME_COMMAND_OPEN_WINDOW:
            ret = Dome_execute_open_window(self);
            break;
        case DOME_COMMAND_CLOSE_WINDOW:
            ret = Dome_execute_close_window(self);
            break;
        case DOME_COMMAND_STOP_WINDOW:
            ret = Dome_execute_stop_window(self);
            break;
        case DOME_COMMAND_INSPECT:
            ret = Dome_execute_inspect(self);
            break;
        case DOME_COMMAND_REGISTER:
            ret = Dome_execute_register(self);
            break;
        case DOME_COMMAND_GET_WINDOW_POSITION:
            ret = Dome_execute_get_window_position(self);
            break;
        case DOME_COMMAND_GET_WINDOW_OPEN_SPEED:
            ret = Dome_execute_get_window_open_speed(self);
            break;
        case DOME_COMMAND_SET_WINDOW_OPEN_SPEED:
            ret = Dome_execute_set_window_open_speed(self);
            break;
        case DOME_COMMAND_GET_WINDOW_CLOSE_SPEED:
            ret = Dome_execute_get_window_close_speed(self);
            break;
        case DOME_COMMAND_SET_WINDOW_CLOSE_SPEED:
            ret = Dome_execute_set_window_close_speed(self);
            break;
        case DOME_COMMAND_RAW:
            ret = Dome_execute_raw(self);
            break;
        default:
            return Dome_execute_default(self);
            break;
    }
    
    return ret;
}

static const void *dome_virtual_table(void);

static void *
Dome_ctor(void *_self, va_list *app)
{
    struct Dome *self = super_ctor(Dome(), _self, app);
    
    self->_._vtab = dome_virtual_table();
    
    return (void *) self;
}

static void *
Dome_dtor(void *_self)
{
    //struct RPC *self = cast(RPC(), _self);
    
    return super_dtor(Dome(), _self);
}

static void *
DomeClass_ctor(void *_self, va_list *app)
{
    struct DomeClass *self = super_ctor(DomeClass(), _self, app);
    Method selector;
    
    self->_.execute.method = (Method) 0;
    
#ifdef va_copy
    va_list ap;
    va_copy(ap, *app);
#else
    va_list ap = *app;
#endif
    
    while ((selector = va_arg(ap, Method))) {
        const char *tag = va_arg(ap, const char *);
        Method method = va_arg(ap, Method);
        
        if (selector == (Method) dome_status) {
            if (tag) {
                self->status.tag = tag;
                self->status.selector = selector;
            }
            self->status.method = method;
            continue;
        }
        if (selector == (Method) dome_get_index_by_name) {
            if (tag) {
                self->get_index_by_name.tag = tag;
                self->get_index_by_name.selector = selector;
            }
            self->get_index_by_name.method = method;
            continue;
        }
        if (selector == (Method) dome_get_name_by_index) {
            if (tag) {
                self->get_name_by_index.tag = tag;
                self->get_name_by_index.selector = selector;
            }
            self->get_name_by_index.method = method;
            continue;
        }
        if (selector == (Method) dome_raw) {
            if (tag) {
                self->raw.tag = tag;
                self->raw.selector = selector;
            }
            self->raw.method = method;
            continue;
        }
        if (selector == (Method) dome_open_window) {
            if (tag) {
                self->open_window.tag = tag;
                self->open_window.selector = selector;
            }
            self->open_window.method = method;
            continue;
        }
        if (selector == (Method) dome_close_window) {
            if (tag) {
                self->close_window.tag = tag;
                self->close_window.selector = selector;
            }
            self->close_window.method = method;
            continue;
        }
        if (selector == (Method) dome_stop_window) {
            if (tag) {
                self->stop_window.tag = tag;
                self->stop_window.selector = selector;
            }
            self->stop_window.method = method;
            continue;
        }
        if (selector == (Method) dome_get_window_position) {
            if (tag) {
                self->get_window_position.tag = tag;
                self->get_window_position.selector = selector;
            }
            self->get_window_position.method = method;
            continue;
        }
        if (selector == (Method) dome_get_window_open_speed) {
            if (tag) {
                self->get_window_open_speed.tag = tag;
                self->get_window_open_speed.selector = selector;
            }
            self->get_window_open_speed.method = method;
            continue;
        }
        if (selector == (Method) dome_set_window_open_speed) {
            if (tag) {
                self->set_window_open_speed.tag = tag;
                self->set_window_open_speed.selector = selector;
            }
            self->set_window_open_speed.method = method;
            continue;
        }
        if (selector == (Method) dome_get_window_close_speed) {
            if (tag) {
                self->get_window_close_speed.tag = tag;
                self->get_window_close_speed.selector = selector;
            }
            self->get_window_close_speed.method = method;
            continue;
        }
        if (selector == (Method) dome_set_window_close_speed) {
            if (tag) {
                self->set_window_close_speed.tag = tag;
                self->set_window_close_speed.selector = selector;
            }
            self->set_window_close_speed.method = method;
            continue;
        }
        if (selector == (Method) dome_inspect) {
            if (tag) {
                self->inspect.tag = tag;
                self->inspect.selector = selector;
            }
            self->inspect.method = method;
            continue;
        }
        if (selector == (Method) dome_register) {
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
    return (void *) self;
    
}

static void *_DomeClass;

static void
DomeClass_destroy(void)
{
    free((void *) _DomeClass);
}

static void
DomeClass_initialize(void)
{
    _DomeClass = new(RPCClass(), "DomeClass", RPCClass(), sizeof(struct DomeClass),
                     ctor, "ctor", DomeClass_ctor,
                     (void *) 0);
    
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(DomeClass_destroy);
#endif
    
}

const void *
DomeClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, DomeClass_initialize);
#endif
    
    return _DomeClass;
}

static void *_Dome;

static void
Dome_destroy(void)
{
    free((void *) _Dome);
}

static void
Dome_initialize(void)
{
    _Dome = new(DomeClass(), "Dome", RPC(), sizeof(struct Dome),
                ctor, "ctor", Dome_ctor,
                dtor, "dtor", Dome_dtor,
                dome_get_index_by_name, "get_index_by_name", Dome_get_index_by_name,
                dome_get_name_by_index, "get_name_by_index", Dome_get_name_by_index,
                dome_init, "init", Dome_init,
                dome_open_window, "open_window", Dome_open_window,
                dome_close_window, "close", Dome_close_window,
                dome_stop_window, "stop", Dome_stop_window,
                dome_inspect, "inspect", Dome_inspect,
                dome_register, "register", Dome_register,
                dome_get_window_position, "get_window_position", Dome_get_window_position,
                dome_get_window_open_speed, "get_window_open_speed", Dome_get_window_open_speed,
                dome_set_window_open_speed, "set_window_open_speed", Dome_set_window_open_speed,
                dome_get_window_close_speed, "get_window_close_speed", Dome_get_window_close_speed,
                dome_set_window_close_speed, "set_window_close_speed", Dome_set_window_close_speed,
                dome_status, "status", Dome_status,
                (void *) 0);
    
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(Dome_destroy);
#endif
    
}

const void *
Dome(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, Dome_initialize);
#endif
    
    return _Dome;
}

static const void *_dome_virtual_table;

static void
dome_virtual_table_destroy(void)
{
    delete((void *) _dome_virtual_table);
}


static void
dome_virtual_table_initialize(void)
{
    _dome_virtual_table = new(RPCVirtualTable(),
                              rpc_execute, "execute", Dome_execute,
                              (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(dome_virtual_table_destroy);
#endif
    
}

static const void *
dome_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, dome_virtual_table_initialize);
#endif
    
    return _dome_virtual_table;
}

static const void *dome_client_virtual_table(void);

static
int DomeClient_connect(void *_self, void **client)
{
    struct DomeClient *self = cast(DomeClient(), _self);
    
    int ret = AAOS_OK;
    int cfd;
    
    if (self->_._.address != NULL && Access(self->_._.address, F_OK) == 0) {
        cfd = Un_stream_connect(self->_._.address);
    } else {
        cfd = Tcp_connect(self->_._.address, self->_._.port, NULL, NULL);
    }
    
    if (cfd < 0) {
        switch (errno) {
            case ECONNREFUSED:
                ret = AAOS_ECONNREFUSED;
                break;
            case ENETUNREACH:
                ret = AAOS_ENETUNREACH;
                break;
            case ETIMEDOUT:
                ret = AAOS_ETIMEDOUT;
                break;
            default:
                ret = AAOS_ERROR;
                break;
        }
    }
    
    *client = new(Dome(), cfd);
    protobuf_set(*client, PACKET_PROTOCOL, PROTO_DOME);
    
    return ret;
}

static void *
DomeClient_ctor(void *_self, va_list *app)
{
    struct DomeClient *self = super_ctor(DomeClient(), _self, app);
    
    self->_._vtab = dome_client_virtual_table();
    
    return (void *) self;
}

static void *
DomeClient_dtor(void *_self)
{
    //struct RPC *self = cast(RPC(), _self);
    
    return super_dtor(DomeClient(), _self);
}

static void *
DomeClientClass_ctor(void *_self, va_list *app)
{
    struct DomeClientClass *self = super_ctor(DomeClientClass(), _self, app);
    
    self->_.connect.method = (Method) 0;
    
    return self;
}

static void *_DomeClientClass;

static void
DomeClientClass_destroy(void)
{
    free((void *) _DomeClientClass);
}

static void
DomeClientClass_initialize(void)
{
    _DomeClientClass = new(RPCClientClass(), "DomeClientClass", RPCClientClass(), sizeof(struct DomeClientClass),
                           ctor, "ctor", DomeClientClass_ctor,
                           (void *) 0);
    
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(DomeClientClass_destroy);
#endif
    
}

const void *
DomeClientClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, DomeClientClass_initialize);
#endif
    
    return _DomeClientClass;
}

static void *_DomeClient;

static void
DomeClient_destroy(void)
{
    free((void *) _DomeClient);
}

static void
DomeClient_initialize(void)
{
    _DomeClient = new(DomeClientClass(), "DomeClient", RPCClient(), sizeof(struct DomeClient),
                      ctor, "ctor", DomeClient_ctor,
                      dtor, "dtor", DomeClient_dtor,
                      (void *) 0);
    
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(DomeClient_destroy);
#endif
}

const void *
DomeClient(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, DomeClient_initialize);
#endif
    
    return _DomeClient;
}

static const void *_dome_client_virtual_table;

static void
dome_client_virtual_table_destroy(void)
{
    delete((void *) _dome_client_virtual_table);
}

static void
dome_client_virtual_table_initialize(void)
{
    _dome_client_virtual_table = new(RPCClientVirtualTable(),
                                     rpc_client_connect, "connect", DomeClient_connect,
                                     (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(dome_client_virtual_table_destroy);
#endif
    
}

static const void *
dome_client_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, dome_client_virtual_table_initialize);
#endif
    
    return _dome_client_virtual_table;
}

/*
 * Dome server class
 */

static const void *dome_server_virtual_table(void);

static int
DomeServer_accept(void *_self, void **client)
{
    struct RPCServer *self = cast(RPCServer(), _self);
    
    int lfd, cfd;
    
    lfd = tcp_server_get_lfd(self);
    cfd = Accept(lfd, NULL, NULL);
    if (cfd < 0) {
        *client = NULL;
        return AAOS_ERROR;
    } else {
        if ((*client = new(Dome(), cfd)) == NULL) {
            Close(cfd);
            return AAOS_ERROR;
        }
        return AAOS_OK;
    }
}

static int
DomeServer_accept2(void *_self, void **client)
{
    struct RPCServer *self = cast(RPCServer(), _self);
    
    int lfd, cfd, lfds[2];
    
    tcp_server_get_lfds(self, lfds);
    lfd = lfds[1];
    
    cfd = Accept(lfd, NULL, NULL);
    if (cfd < 0) {
        *client = NULL;
        return AAOS_ERROR;
    } else {
        if ((*client = new(Dome(), cfd)) == NULL) {
            Close(cfd);
            return AAOS_ERROR;
        }
        return AAOS_OK;
    }
}

static void *
DomeServer_ctor(void *_self, va_list *app)
{
    struct DomeServer *self = super_ctor(DomeServer(), _self, app);
    
    self->_._vtab = dome_server_virtual_table();
    
    return (void *) self;
}

static void *
DomeServer_dtor(void *_self)
{
    //struct RPC *self = cast(RPC(), _self);
    
    return super_dtor(DomeServer(), _self);
}

static void *
DomeServerClass_ctor(void *_self, va_list *app)
{
    struct DomeServerClass *self = super_ctor(DomeServerClass(), _self, app);
    
    self->_.accept.method = (Method) 0;
    self->_.accept2.method = (Method) 0;
    
    return self;
}

static void *_DomeServerClass;

static void
DomeServerClass_destroy(void)
{
    free((void *) _DomeServerClass);
}

static void
DomeServerClass_initialize(void)
{
    _DomeServerClass = new(RPCServerClass(), "DomeServerClass", RPCServerClass(), sizeof(struct DomeServerClass),
                           ctor, "ctor", DomeServerClass_ctor,
                           (void *) 0);
    
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(DomeServerClass_destroy);
#endif
}

const void *
DomeServerClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, DomeServerClass_initialize);
#endif
    
    return _DomeServerClass;
}

static void *_DomeServer;

static void
DomeServer_destroy(void)
{
    free((void *) _DomeServer);
}

static void
DomeServer_initialize(void)
{
    _DomeServer = new(DomeServerClass(), "DomeServer", RPCServer(), sizeof(struct DomeServer),
                      ctor, "ctor", DomeServer_ctor,
                      dtor, "dtor", DomeServer_dtor,
                      (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(DomeServer_destroy);
#endif
}

const void *
DomeServer(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_once(&once, DomeServer_initialize);
#endif
    
    return _DomeServer;
}

static const void *_dome_server_virtual_table;

static void
dome_server_virtual_table_destroy(void)
{
    delete((void *) _dome_server_virtual_table);
}

static void
dome_server_virtual_table_initialize(void)
{
    _dome_server_virtual_table = new(RPCServerVirtualTable(),
                                     rpc_server_accept, "accept", DomeServer_accept,
                                     rpc_server_accept2, "accept2", DomeServer_accept2,
                                     (void *)0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(dome_server_virtual_table_destroy);
#endif
}

static const void *
dome_server_virtual_table(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, dome_server_virtual_table_initialize);
#endif
    
    return _dome_server_virtual_table;
}

#ifdef _USE_COMPILER_ATTRIBUTION_

static void __destructor__(void) __attribute__ ((destructor(_DOME_RPC_PRIORITY_)));

static void
__destructor__(void)
{
    DomeServer_destroy();
    DomeServerClass_destroy();
    dome_server_virtual_table_destroy();
    DomeClient_destroy();
    DomeClientClass_destroy();
    dome_client_virtual_table_destroy();
    Dome_destroy();
    DomeClass_destroy();
    dome_virtual_table_destroy();
}

static void __constructor__(void) __attribute__ ((constructor(_DOME_RPC_PRIORITY_)));

static void
__constructor__(void)
{
    dome_virtual_table_initialize();
    DomeClass_initialize();
    Dome_initialize();
    dome_client_virtual_table_initialize();
    DomeClientClass_initialize();
    DomeClient_initialize();
    dome_server_virtual_table_initialize();
    DomeServerClass_initialize();
    DomeServer_initialize();
}
#endif
