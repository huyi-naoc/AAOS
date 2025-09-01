//
//  thermal.h
//  AAOS
//
//  Created by Hu Yi on 2023/2/25.
//  Copyright © 2023 NAOC. All rights reserved.
//

#ifndef thermal_h
#define thermal_h

#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

int __thermal_unit_turn_on(void *_self);
int __thermal_unit_turn_off(void *_self);
int __thermal_unit_get_temperature(const void *_self, double *temperature);
int __thermal_unit_status(void *_self, void *buffer, size_t size, size_t *res_len);
void *__thermal_unit_thermal_control(void *_self);
const char *__thermal_unit_get_name(const void *_self);

extern const void *__ThermalUnit(void);
extern const void *__ThermalUnitClass(void);
extern const void *__ThermalUnitVirtualTable(void);

extern const void *KLCAMThermalUnit(void);
extern const void *KLCAMThermalUnitClass(void);

extern const void *SimpleThermalUnit(void);
extern const void *SimpleThermalUnitClass(void);

extern const void *KLCAMSimpleThermalUnit(void);
extern const void *KLCAMSimpleThermalUnitClass(void);

#ifdef __cplusplus
}
#endif

#endif /* thermal_h */
