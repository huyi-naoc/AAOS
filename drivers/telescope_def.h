//
//  telescope_def.h
//  AAOS
//
//  Created by Hu Yi on 2019/7/26.
//  Copyright © 2019 NAOC. All rights reserved.
//

#ifndef telescope_def_h
#define telescope_def_h

#define TELESCOPE_STATE_POWERED_OFF     0
#define TELESCOPE_STATE_UNINITIALIZED   1
#define TELESCOPE_STATE_PARKED          2
#define TELESCOPE_STATE_SLEWING         3
#define TELESCOPE_STATE_MOVING          4
#define TELESCOPE_STATE_TRACKING        5
#define TELESCOPE_STATE_TRACKING_WAIT   6
#define TELESCOPE_STATE_SWITCING        7

#define TELESCOPE_STATE_MALFUNCTION     0x80000000

#define TELESCOPE_MOVE_EAST             1
#define TELESCOPE_MOVE_WEST             2
#define TELESCOPE_MOVE_NORTH            3
#define TELESCOPE_MOVE_SOUTH            4

#define TELESCOPE_OPTION_IGNORE_MALFUNCTION 0x4000
#define TELESCOPE_OPTION_DO_NOT_WAIT        0x0001
#define TELESCOPE_LOCK                      0x0002

#define TELESCOPE_EQUATORIAL            1
#define TELESCOPE_HORIZONTAL            2

#define TELESCOPE_TRACK_RATE_LUNAR     10000
#define TELESCOPE_TRACK_RATE_SOLAR     10002
#define TELESCOPE_TRACK_RATE_SIDEREAL  10004

#endif /* telescope_def_h */
