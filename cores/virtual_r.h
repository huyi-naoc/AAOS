//
//  virtual_r.h
//  AAOS
//
//  Created by huyi on 2018/7/10.
//  Copyright © 2018年 National Astronomical Observatories, Chinese Academy of Sciences. All rights reserved.
//

#ifndef __virtual_r_h
#define __virtual_r_h

#include "object_r.h"

//#define _VIRTUAL_PRIORITY_ 102

struct VirtualTable {
    const struct Object _;
    struct Method dummy;
};

struct VirtualTableClass {
    const struct Class _;
    struct Method virtualTo;
};

#endif /* virtual_r_h */
