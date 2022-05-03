//
//  virtual.h
//  AAOS
//
//  Created by huyi on 2018/7/10.
//  Copyright © 2018年 National Astronomical Observatories, Chinese Academy of Sciences. All rights reserved.
//

#ifndef __virtual_h
#define __virtual_h

#include "object.h"

Method virtualTo(const void *_self, const char *tag);

extern const void *VirtualTable(void);
extern const void *VirtualTableClass(void);

#endif /* virtual_h */
