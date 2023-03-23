//
//  thermal_rpc.h
//  AAOS
//
//  Created by Hu Yi on 2023/3/22.
//  Copyright © 2023 NAOC. All rights reserved.
//

#ifndef thermal_rpc_h
#define thermal_rpc_h

#include "rpc.h"

#include <stdio.h>
#include <string.h>

int thermal_unit_get_index_by_name(void *_self, const char *name);
int thermal_unit_status(void *_self, void *buffer, size_t size, size_t *res_size);
int thermal_unit_info(void *_self, FILE *fp);

extern const void *ThermalUnit(void);
extern const void *ThermalUnitClass(void);

extern const void *ThermalUnitClient(void);
extern const void *ThermalUnitClientClass(void);

extern const void *ThermalUnitServer(void);
extern const void *ThermalUnitServerClass(void);

#endif /* thermal_rpc_h */
