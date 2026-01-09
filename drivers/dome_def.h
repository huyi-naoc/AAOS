//
//  dome_def.h
//  AAOS
//
//  Created by huyi on 2025/4/27.
//

#ifndef dome_def_h
#define dome_def_h

#define DOME_STATE_WINDOW_CLOSED    0x0000
#define DOME_STATE_WINDOW_OPENED    0x0001
#define DOME_STATE_WINDOW_OPENING   0x0002
#define DOME_STATE_WINDOW_CLOSING   0x0004
#define DOME_STATE_WINDOW_STOPPED   0x0008

#define DOME_STATE_SLEWING          0x0010
#define DOME_STATE_TRACKING         0x0020
#define DOME_STATE_PARKED           0x0040

#define DOME_STATE_UNINITIALIZED    0x4000
#define DOME_STATE_MALFUNCTION      0x8000

#endif /* dome_def_h */
