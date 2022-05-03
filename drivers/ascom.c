//
//  ascom.c
//  AAOS
//
//  Created by Hu Yi on 2019/11/15.
//  Copyright © 2019 NAOC. All rights reserved.
//

#include <curl/curl.h>

#include "ascom.h"
#include "ascom_r.h"
#include "def.h"
#include "wrapper.h"

struct MemoryStruct {
    char *memory;
    size_t *size;
};

static void
ASCOM_cleanup(void *arg)
{
    CURL *curl_handle = (CURL *) arg;
    
    curl_easy_cleanup(curl_handle);
}


static size_t
ReadMemoryCallback(void *data, size_t size, size_t nmemb, void *userp)
{
    char *s = (char *) userp;
    
    if (s == NULL) {
        return size * nmemb;
    }
    
    size_t realsize = strlen(s) + 1;

    if (realsize > size * nmemb) {
        realsize = size * nmemb;
    }
    
    memcpy(data, s, realsize);
    
    return realsize;
}

static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;
    
    char *ptr = realloc(mem->memory, *mem->size + realsize + 1);
    
    if (ptr == NULL) {
        return 0;
    }
 
    mem->memory = ptr;
 
    memcpy(&mem->memory[*mem->size], contents, realsize);
  
    *mem->size += realsize;
  
    mem->memory[*mem->size] = 0;
 
    return realsize;
}

int
ascom_get(void *_self, const char *command, char *response_data, size_t *size)
{
    const struct ASCOMClass *class = (const struct ASCOMClass *) classOf(_self);
    
    if (isOf(class, ASCOMClass()) && class->get.method) {
        return ((int (*)(void *, const char *, char *, size_t *)) class->get.method)(_self, command, response_data, size);
    } else {
        int result;
        forward(_self, &result, (Method) ascom_get, "get", _self, command, response_data, size);
        return result;
    }
}

static int
ASCOM_get(void *_self, const char *command, void *response_data, size_t *size)
{
    struct ASCOM *self = cast(ASCOM(), _self);
    
    char URL[BUFSIZE];
    CURL *curl_handle = NULL;
    CURLcode res;
    struct MemoryStruct user_data;
   
    int ret = AAOS_OK;
    
    if (self->port) {
        snprintf(URL, BUFSIZE, "%s:%s/api/%s/%s/%ud/%s", self->address, self->port, self->version, self->device_type, self->device_number, command);
    } else {
        snprintf(URL, BUFSIZE, "%s/api/%s/%s/%ud/%s", self->address, self->version, self->device_type, self->device_number, command);
    }
    
    curl_handle = curl_easy_init();
    pthread_cleanup_push(ASCOM_cleanup, curl_handle);
    if (curl_handle == NULL) {
        ret = AAOS_ERROR;
        goto error;
    }
    
    curl_easy_setopt(curl_handle, CURLOPT_URL, URL);
    
    user_data.memory = response_data;
    if (size != NULL) {
        user_data.size = size;
    } else {
        user_data.size = 0;
    }
    
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *) &user_data);
    
    res = curl_easy_perform(curl_handle);
    
    response_data = user_data.memory;
    
    if (res != CURLE_OK) {
        ret = AAOS_ERROR;
        goto error;
    }
    
    pthread_cleanup_pop(0);
    
error:
    curl_easy_cleanup(curl_handle);
    
    return ret;
}

int
ascom_put(void *_self, const char *command, const char *request_data, char *response_data, size_t *size)
{
    const struct ASCOMClass *class = (const struct ASCOMClass *) classOf(_self);
    
    if (isOf(class, ASCOMClass()) && class->put.method) {
        return ((int (*)(void *, const char *, const char *, char *, size_t *)) class->put.method)(_self, command, request_data, response_data, size);
    } else {
        int result;
        forward(_self, &result, (Method) ascom_get, "put", _self, command, request_data, response_data, size);
        return result;
    }
}

static int
ASCOM_put(void *_self, const char *command, const char *request_data, char *response_data, size_t *size)
{
    struct ASCOM *self = cast(ASCOM(), _self);
    
    char URL[BUFSIZE];
    CURL *curl_handle;
    CURLcode res;
    struct MemoryStruct user_data;
   
    int ret = AAOS_OK;
    
    if (self->port) {
        snprintf(URL, BUFSIZE, "%s:%s/api/%s/%s/%ud/%s", self->address, self->port, self->version, self->device_type, self->device_number, command);
    } else {
        snprintf(URL, BUFSIZE, "%s/api/%s/%s/%ud/%s", self->address, self->version, self->device_type, self->device_number, command);
    }
    
    
    curl_handle = curl_easy_init();
    pthread_cleanup_push(ASCOM_cleanup, curl_handle);
    if (curl_handle == NULL) {
        ret = AAOS_ERROR;
        goto error;
    }
    
    curl_easy_setopt(curl_handle, CURLOPT_URL, URL);
    curl_easy_setopt(curl_handle, CURLOPT_READFUNCTION, ReadMemoryCallback);
    curl_easy_setopt(curl_handle, CURLOPT_READDATA, request_data);
    curl_easy_setopt(curl_handle, CURLOPT_UPLOAD, 1L);
    curl_easy_setopt(curl_handle, CURLOPT_PUT, 1L);
    
    
    user_data.memory = response_data;
    if (size != NULL) {
        user_data.size = size;
    } else {
        user_data.size = 0;
    }
    
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *) &user_data);
    
    res = curl_easy_perform(curl_handle);
    
    response_data = user_data.memory;
    
    if (res != CURLE_OK) {
        ret = AAOS_ERROR;
        goto error;
    }
    
    pthread_cleanup_pop(0);
error:
    curl_easy_cleanup(curl_handle);
    
    return ret;
}

static void *
ASCOM_ctor(void *_self, va_list *app)
{
    struct ASCOM *self = super_ctor(ASCOM(), _self, app);
    
    const char *s;
    
    s = va_arg(*app, const char *);
    if (s) {
        self->address = (char *) Malloc(strlen(s) + 1);
        snprintf(self->address, strlen(s) + 1, "%s", s);
    }
    s = va_arg(*app, const char *);
    if (s) {
        self->port = (char *) Malloc(strlen(s) + 1);
        snprintf(self->port, strlen(s) + 1, "%s", s);
    }
    s = va_arg(*app, const char *);
    if (s) {
        self->version = (char *) Malloc(strlen(s) + 1);
        snprintf(self->version, strlen(s) + 1, "%s", s);
    }
    s = va_arg(*app, const char *);
    if (s) {
        self->device_type = (char *) Malloc(strlen(s) + 1);
        snprintf(self->device_type, strlen(s) + 1, "%s", s);
    }
    self->device_number = va_arg(*app, unsigned int);
    
    
    return (void *) self;
}

static void *
ASCOM_dtor(void *_self)
{
    struct ASCOM *self = cast(ASCOM(), _self);
    
   
    free(self->address);
    free(self->port);
    free(self->version);
    free(self->device_type);
    
    return super_dtor(ASCOM(), _self);
}

static void *
ASCOMClass_ctor(void *_self, va_list *app)
{
    struct ASCOMClass *self = super_ctor(ASCOMClass(), _self, app);
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
        if (selector == (Method) ascom_put) {
            if (tag) {
                self->put.tag = tag;
                self->put.selector = selector;
            }
            self->put.method = method;
            continue;
        }
        if (selector == (Method) ascom_get) {
            if (tag) {
                self->get.tag = tag;
                self->get.selector = selector;
            }
            self->get.method = method;
            continue;
        }
    }
    
#ifdef va_copy
    va_end(ap);
#endif
    
    return (void *) self;
}

static const void *_ASCOMClass;

static void
ASCOMClass_destroy(void)
{
    free((void *) _ASCOMClass);
}

static void
ASCOMClass_initialize(void)
{
    _ASCOMClass = new(Class(), "ASCOMClass", Class(), sizeof(struct ASCOMClass),
                        ctor, "ctor", ASCOMClass_ctor,
                        (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(ASCOMClass_destroy);
#endif
}

const void *
ASCOMClass(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, ASCOMClass_initialize);
#endif
    
    return _ASCOMClass;
}

static const void *_ASCOM;

static void
ASCOM_destroy(void)
{
    free((void *)_ASCOM);
}

static void
ASCOM_initialize(void)
{
    _ASCOM = new(ASCOMClass(), "ASCOM", Object(), sizeof(struct ASCOM),
                 ctor, "ctor", ASCOM_ctor,
                 dtor, "dtor", ASCOM_dtor,
                 ascom_get, "get", ASCOM_get,
                 ascom_put, "put", ASCOM_put,
                 (void *) 0);
#ifndef _USE_COMPILER_ATTRIBUTION_
    atexit(ASCOM_destroy);
#endif
}

const void *
ASCOM(void)
{
#ifndef _USE_COMPILER_ATTRIBUTION_
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    Pthread_once(&once_control, ASCOM_initialize);
#endif
    
    return _ASCOM;
}

#ifdef _USE_COMPILER_ATTRIBUTION_
static void __destructor__(void) __attribute__ ((destructor(_ASCOM_PRIORITY_)));

static void
__destructor__(void)
{
    Ascom_destroy();
    AscomClass_destroy();
}

static void __constructor__(void) __attribute__ ((constructor(_ASCOM_PRIORITY_)));

static void
__constructor__(void)
{
    AscomClass_initialize();
    Ascom_initialize();
}
#endif

#include <cjson/cJSON.h>

int
ascom_get_bool_value(const char *response, bool *value)
{
    int status = AAOS_OK, error_code;
    cJSON *response_json = cJSON_Parse(response), *error_code_cjson, *value_json;
    
    if (response_json == NULL) {
        status = AAOS_ERROR;
        goto error;
    }
    
    error_code_cjson = cJSON_GetObjectItemCaseSensitive(response_json, "ErrorNumber");
    if (!(cJSON_IsNumber(error_code_cjson))) {
        goto error;
    }
    error_code = error_code_cjson->valueint;
    if (error_code != AAOS_OK) {
        goto error;
    }
    
    value_json = cJSON_GetObjectItemCaseSensitive(response_json, "Value");
    if (!(cJSON_IsNumber(value_json))) {
        goto error;
    }
    *value = value_json->valueint;
    
error:
    cJSON_Delete(response_json);
    return status;
    
}

int
ascom_get_integer_value(const char *response, int *value)
{
    int status = AAOS_OK, error_code;
    cJSON *response_json = cJSON_Parse(response), *error_code_cjson, *value_json;
    
    if (response_json == NULL) {
        status = AAOS_ERROR;
        goto error;
    }
    
    error_code_cjson = cJSON_GetObjectItemCaseSensitive(response_json, "ErrorNumber");
    if (!(cJSON_IsNumber(error_code_cjson))) {
        goto error;
    }
    error_code = error_code_cjson->valueint;
    if (error_code != AAOS_OK) {
        goto error;
    }
    
    value_json = cJSON_GetObjectItemCaseSensitive(response_json, "Value");
    if (!(cJSON_IsNumber(value_json))) {
        goto error;
    }
    *value = value_json->valueint;
    
error:
    cJSON_Delete(response_json);
    return status;
}

int
ascom_get_double_value(const char *response, double *value)
{
    int status = AAOS_OK, error_code;
    cJSON *response_json = cJSON_Parse(response), *error_code_cjson, *value_json;
    
    if (response_json == NULL) {
        status = AAOS_ERROR;
        goto error;
    }
    
    error_code_cjson = cJSON_GetObjectItemCaseSensitive(response_json, "ErrorNumber");
    if (!(cJSON_IsNumber(error_code_cjson))) {
        goto error;
    }
    error_code = error_code_cjson->valueint;
    if (error_code != AAOS_OK) {
        goto error;
    }
    
    value_json = cJSON_GetObjectItemCaseSensitive(response_json, "Value");
    if (!(cJSON_IsNumber(value_json))) {
        goto error;
    }
    *value = value_json->valuedouble;
    
error:
    cJSON_Delete(response_json);
    return status;
}

int
ascom_get_string_value(const char *response, char *value, size_t size)
{
    int status = AAOS_OK, error_code;
    cJSON *response_json = cJSON_Parse(response), *error_code_cjson, *value_json;
    
    if (response_json == NULL) {
        status = AAOS_ERROR;
        goto error;
    }
    
    error_code_cjson = cJSON_GetObjectItemCaseSensitive(response_json, "ErrorNumber");
    if (!(cJSON_IsNumber(error_code_cjson))) {
        goto error;
    }
    error_code = error_code_cjson->valueint;
    if (error_code != AAOS_OK) {
        goto error;
    }
    
    value_json = cJSON_GetObjectItemCaseSensitive(response_json, "Value");
    if (!(cJSON_IsNumber(value_json))) {
        goto error;
    }
    
    snprintf(value, size, "%s", value_json->valuestring);
    
error:
    cJSON_Delete(response_json);
    return status;
}

int
ascom_get_error_code(const char *response, int *error_code)
{
    int status = AAOS_OK;
    cJSON *response_json = cJSON_Parse(response), *error_code_cjson;
    
    if (response_json == NULL) {
        status = AAOS_ERROR;
        goto error;
    }
    
    error_code_cjson = cJSON_GetObjectItemCaseSensitive(response_json, "ErrorNumber");
    if (!(cJSON_IsNumber(error_code_cjson))) {
        goto error;
    }
    *error_code = error_code_cjson->valueint;
    
error:
    cJSON_Delete(response_json);
    return status;
}

/*
 * *data will be reallocated, must be free properly somewhere else.
 */

/*
int
ascom_get(void *_self, const char *command, void **data, size_t *size)
{
    const struct ASCOMClass *class = (const struct ASCOMClass *) classOf(_self);
    
    if (isOf(class, ASCOMClass()) && class->get.method) {
        return ((int (*)(void *, const char *, void **, size_t *)) class->get.method)(_self, command, data, size);
    } else {
        int result;
        forward(_self, &result, (Method) ascom_get, "get", _self, command, data, size);
        return result;
    }
}
*/
/*
 * this call back function comes from curl example, not mine.
 */
/*
static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;
    
    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    
    if (ptr == NULL) {
        return 0;
    }
 
    mem->memory = ptr;
 
    memcpy(&(mem->memory[mem->size]), contents, realsize);
  
    mem->size += realsize;
  
    mem->memory[mem->size] = 0;
 
    return realsize;
}

static int
ASCOM_get(void *_self, const char *command, void **data, size_t *size)
{
    struct ASCOM *self = cast(ASCOM(), _self);
    
    char URL[BUFSIZE];
    CURL *curl_handle;
    CURLcode res;
    struct MemoryStruct user_data;
    struct curl_slist *chunk = NULL;
    int ret = AAOS_OK;
    
    if (self->port) {
        snprintf(URL, BUFSIZE, "%s:%s/api/%s/%s/%ud/", self->address, self->port, self->version, self->device_type, self->device_number);
    } else {
        snprintf(URL, BUFSIZE, "%s/api/%s/%s/%ud/", self->address, self->version, self->device_type, self->device_number);
    }
    
    
    curl_handle = curl_easy_init();
    if (curl_handle == NULL) {
        ret = AAOS_ERROR;
        goto error;
    }
    chunk = curl_slist_append(chunk, "Accept:application/json");
    curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, chunk);
    curl_easy_setopt(curl_handle, CURLOPT_URL, URL);
    
    user_data.memory = *data;
    if (size != NULL) {
        user_data.size = *size;
    } else {
        user_data.size = 0;
    }
    
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *) &user_data);
    
    res = curl_easy_perform(curl_handle);
    if (res != CURLE_OK) {
        ret = AAOS_ERROR;
        goto error;
    }

error:
    curl_easy_cleanup(curl_handle);
    if (chunk != NULL) {
        curl_slist_free_all(chunk);
    }
    return ret;
}
*/
/*
 * *data will be reallocated, must be free properly somewhere else.
 */

/*
int
ascom_put(void *_self, const char *command, const char *parameters, void **data, size_t *size)
{
    const struct ASCOMClass *class = (const struct ASCOMClass *) classOf(_self);
    
    if (isOf(class, ASCOMClass()) && class->put.method) {
        return ((int (*)(void *, const char *, const char *, void **, size_t *)) class->put.method)(_self, command, parameters, data, size);
    } else {
        int result;
        forward(_self, &result, (Method) ascom_get, "get", _self, command, parameters, data, size);
        return result;
    }
}
 */
/*
static int
ASCOM_put(void *_self, const char *command, const char *parameters, void **data, size_t *size)
{
    struct ASCOM *self = cast(ASCOM(), _self);
    
    char URL[BUFSIZE];
    CURL *curl_handle;
    CURLcode res;
    struct MemoryStruct user_data;
    struct curl_slist *chunk = NULL;
    int ret = AAOS_OK;
    
    if (self->port) {
        snprintf(URL, BUFSIZE, "%s:%s/api/%s/%s/%ud/", self->address, self->port, self->version, self->device_type, self->device_number);
    } else {
        snprintf(URL, BUFSIZE, "%s/api/%s/%s/%ud/", self->address, self->version, self->device_type, self->device_number);
    }
    
    
    curl_handle = curl_easy_init();
    if (curl_handle == NULL) {
        ret = AAOS_ERROR;
        goto error;
    }
    chunk = curl_slist_append(chunk, "Content-type:application/x-www-form-urlencoded");
    curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, chunk);
    curl_easy_setopt(curl_handle, CURLOPT_URL, URL);
    
    user_data.memory = *data;
    if (size != NULL) {
        user_data.size = *size;
    } else {
        user_data.size = 0;
    }
    
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *) &user_data);
    
    res = curl_easy_perform(curl_handle);
    if (res != CURLE_OK) {
        ret = AAOS_ERROR;
        goto error;
    }

error:
    curl_easy_cleanup(curl_handle);
    if (chunk != NULL) {
        curl_slist_free_all(chunk);
    }
    return ret;
}
*/
