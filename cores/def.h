//
//  def.h
//  AAOS
//
//  Created by huyi on 18/6/13.
//  Copyright © 2018年 National Astronomical Observatories, Chinese Academy of Sciences. All rights reserved.
//

#ifndef __def_h
#define __def_h

#include <limits.h>

#define min(a,b) (((a)<(b)?(a):(b)))
#define max(a,b) (((a)>(b)?(a):(b)))

#define ADDRSIZE 128
#define BUFSIZE  4096
#define COMMANDSIZE 256
#define DAEMONNAMESIZE 32
#define FILENAMESIZE NAME_MAX + 1
#define KEYSIZE 32
#define LABELSIZE 32
#define MD5SUMSIZE 33
#define NAMESPACESIZE 16
#define PATHSIZE PATH_MAX + 1
#define PORTSIZE 16
#define STATUSSIZE 32
#define SYMBOLSIZE 64
#define TTYSIZE 32
#define USERNAMESIZE 32
#define UUIDSIZE 64
#define NAMESIZE 256
#define SERVICENAMESIZE 32
#define TIMESTAMPSIZE   32
#define PASSWDSIZE      128

#define DEVPATHSIZE 32
#define DEVNAMESIZE 32

#define PROTO_SERIAL    2
#define PROTO_TELESCOPE 3
#define PROTO_LOG       4
#define PROTO_AWS       5
#define PROTO_PDU       6
#define PROTO_SMS       7
#define PROTO_DETECTOR  8
#define PROTO_THERMAL   9
#define PROTO_SCHEDULER 10
#define PROTO_DOME      11
#define PROTO_SYSTEM    0xFFFF   /* system error recovery */

#define SYSTEM_COMMAND_REGISTER 0xFFFF
#define SYSTEM_COMMAND_INSPECT  0xFFFE

#define PROTO_OPTION_MORE_PACKET 0x8000

#define SERIAL_RPC_PORT         "12000"
#define DETECTOR_RPC_PORT       "12001"

#define TEL_RPC_PORT            "13000"
#define AWS_RPC_PORT            "13001"
#define PDU_RPC_PORT            "13002"
#define SMS_RPC_PORT            "13003"
#define THM_RPC_PORT            "13004"
#define DET_RPC_PORT            "13005"
#define DOM_RPC_PORT            "13006"

#define SCHEDULER_RPC_SITE_PORT   "12500"
#define SCHEDULER_RPC_GLOBAL      "12501"


#define FMODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

enum {AAOS_OK, AAOS_EACCES, AAOS_EAGAIN, AAOS_EBADF, AAOS_EBADMSG, AAOS_EBUSY, AAOS_ECONNREFUSED, AAOS_ECONNRESET, AAOS_ECLOSED, AAOS_EEXIST, AAOS_EHOSTUNREACH, AAOS_EIO, AAOS_EINVAL, AAOS_EINTR, AAOS_ENETDOWN, AAOS_ENETUNREACH, AAOS_ENOENT, AAOS_ENOMEM, AAOS_ENOSPC, AAOS_ENOTDIR, AAOS_ENOTSUP, AAOS_EPERM, AAOS_EPIPE, AAOS_EPROTOWRONG, AAOS_ESRCH, AAOS_ETIMEDOUT, AAOS_EALREADY, AAOS_EBADCMD, AAOS_ECMDNOSUPPORT,  AAOS_ECMDTOOLONG, AAOS_ECANCELED, AAOS_EDEVNOTLOADED, AAOS_EDEVMAL, AAOS_EEMPTY, AAOS_EFAILED, AAOS_EFMTNOTSUP, AAOS_ENOTFOUND, AAOS_EMOREPACK, AAOS_ENOPTION, AAOS_EPWROFF, AAOS_EUNINIT, AAOS_ERROR};

#define MYDEBUG do {printf("%s %s %d\n", __FILE__, __func__, __LINE__); } while(0)

#define MYDEBUG2(x) do {printf("%s %s %d: %s\n", __FILE__, __func__, __LINE__, nameOf(classOf((x)))); } while(0);

#define AAOS_LEVEL0          102
#define AAOS_LEVEL1          103
#define AAOS_LEVEL2          104
#define AAOS_LEVEL2H         105
#define AAOS_LEVEL3          106
#define AAOS_LEVLE4          107

#ifndef _VIRTUAL_PRIORITY_
#define _VIRTUAL_PRIORITY_		100
#endif

#ifndef _NET_PRIORITY_
#define _NET_PRIORITY_          101
#endif

#ifndef _RPC_PRIORITY_
#define _RPC_PRIORITY_          102
#endif

#ifndef _SERIAL_PRIORITY_
#define _SERIAL_PRIORITY_       101
#endif

#ifndef _SERIAL_RPC_PRIORITY_
#define _SERIAL_RPC_PRIORITY_   103
#endif

#ifndef _DOME_PRIORITY_
#define _DOME_PRIORITY_         104
#endif

#ifndef _DETECTOR_RPC_PRIORITY_
#define _DETECTOR_RPC_PRIORITY_	105
#endif

#ifndef _DOME_RPC_PRIORITY_
#define _DOME_RPC_PRIORITY_     105
#endif


extern int load_sofa_library;
#endif /* def_h */
