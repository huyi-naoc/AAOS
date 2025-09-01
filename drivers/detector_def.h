//
//  detector_def.h
//  AAOS
//
//  Created by Hu Yi on 2021/11/19.
//  Copyright © 2021 NAOC. All rights reserved.
//

#ifndef detector_def_h
#define detector_def_h

#define DETECTOR_STATE_OFFLINE          0x00000001
#define DETECTOR_STATE_UNINITIALIZED    0x00000002
#define DETECTOR_STATE_IDLE             0x00000004
#define DETECTOR_STATE_EXPOSING         0x00000008
#define DETECTOR_STATE_READING          0x00000010
#define DETECTOR_STATE_MALFUNCTION      0x80000000

#define DETECTOR_OPTION_IGNORE_DEVMAL           0x8000
#define DETECTOR_OPTION_NOWAIT                  0x4000
#define DETECTOR_OPTION_ONESHOT                 0x2000

#define DETECTOR_OPTION_NOTIFY_LAST_FILLING         0x0002  /* expose function return when last frame is begun to read. */
#define DETECTOR_OPTION_NOTIFY_EACH_COMPLETION      0x0001  /* Each frame is saved in separated file. */
#define DETECTOR_OPTION_STRING_FORMART_PLAIN        0x0000
#define DETECTOR_OPTION_STRING_FORMART_JSON         0x0100
#define DETECTOR_OPTION_STRING_FORMART_LIBCONFIG    0x0200
#define DETECTOR_OPTION_STRING_FORMART_XML          0x0300
#define DETECTOR_OPTION_STRING_FORMART_YAML         0x0400



#define DETECTOR_PIXEL_FORMAT_MONO_8            1
#define DETECTOR_PIXEL_FORMAT_MONO_10           2
#define DETECTOR_PIXEL_FORMAT_MONO_10_PACKED    3
#define DETECTOR_PIXEL_FORMAT_MONO_12           4
#define DETECTOR_PIXEL_FORMAT_MONO_12_PACKED    5
#define DETECTOR_PIXEL_FORMAT_MONO_14           6
#define DETECTOR_PIXEL_FORMAT_MONO_14_PACKED    7
#define DETECTOR_PIXEL_FORMAT_MONO_16           8
#define DETECTOR_PIXEL_FORMAT_MONO_18           9
#define DETECTOR_PIXEL_FORMAT_MONO_18_PACKED    10
#define DETECTOR_PIXEL_FORMAT_MONO_24           11
#define DETECTOR_PIXEL_FORMAT_MONO_24_PACKED    12
#define DETECTOR_PIXEL_FORMAT_MONO_32           13
#define DETECTOR_PIXEL_FORMAT_MONO_64           14

static const char *pixel_format_string[] = {"MONO_8", "MONO_10", "MONO_10_PACKED", "MONO_12", "MONO_12_PACKED", "MONO_14", "MONO_14_PACKED", "MONO_16", "MONO_18", "MONO_18_PACKED", "MONO_24", "MONO_24_PACKED", "MONO_32"};

#endif /* detector_def_h */
