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

int thermal_unit_turn_on(void *_self);
int thermal_unit_turn_off(void *_self);
int thermal_unit_get_temperature(const void *_self, double *temperature);
int thermal_unit_status(void *_self, void *buffer, size_t size);
void *thermal_unit_thermal_control(void *_self);

extern const void *ThermalUnit(void);
extern const void *ThermalUnitClass(void);
extern const void *ThermalUnitVirtualTable(void);

extern const void *KLCAMThermalUnit(void);
extern const void *KLCAMThermalUnitClass(void);

extern const void *KLCAMSimpleThermalUnit(void);
extern const void *KLCAMSimpleThermalUnitClass(void);

#define THERMAL_UNIT_STATE_ON   1
#define THERMAL_UNIT_STATE_OFF  0

#endif /* thermal_h */
