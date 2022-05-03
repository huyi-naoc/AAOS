//
//  ascom.h
//  AAOS
//
//  Created by Hu Yi on 2019/11/15.
//  Copyright © 2019 NAOC. All rights reserved.
//

#ifndef ascom_h
#define ascom_h

#ifdef __cplusplus
extern "C"
{
#endif

#include <string.h>
#include <stdbool.h>

int ascom_put(void *_self, const char *command, const char *request_data, char *response_data, size_t *size);
int ascom_get(void *_self, const char *command, char *response_data, size_t *size);

int ascom_get_bool_value(const char *response, bool *value);
int ascom_get_integer_value(const char *response, int *value);
int ascom_get_double_value(const char *response, double *value);
int ascom_get_string_value(const char *response, char *value, size_t size);
int ascom_get_error_code(const char *response, int *error_code);

extern const void *ASCOM(void);
extern const void *ASCOMClass(void);

#ifdef __cplusplus
}
#endif

#endif /* ascom_h */
