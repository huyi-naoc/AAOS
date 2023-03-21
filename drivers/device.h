//
//  device.h
//  AAOS
//
//  Created by Hu Yi on 2022/7/23.
//  Copyright © 2022 NAOC. All rights reserved.
//

#ifndef device_h
#define device_h

#include "object.h"
#include "virtual.h"

#define DEVICE_OK           0
#define DEVICE_MALFUNCTION  1

int device_inspect(void *_self);
int device_wait(void *_self, double timeout);


extern const void *Device(void);
extern const void *DeviceClass(void);
extern const void *DeviceVirtualTable(void);


#endif /* device_h */
