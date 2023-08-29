//
//  net_r.h
//  AAOS
//
//  Created by huyi on 2018/11/8.
//  Copyright © 2018年 National Astronomical Observatories, Chinese Academy of Sciences. All rights reserved.
//

#ifndef net_r_h
#define net_r_h

#include "object_r.h"
#include "virtual_r.h"

#define  _NET_PRIORITY_ _VIRTUAL_PRIORITY_ + 1

struct TCPSocketVirtualTable {
    struct VirtualTable _;
    struct Method read;
    struct Method write;
};

struct TCPSocket {
    struct Object _;
    const void *_vtab;
    int sockfd;
    unsigned int option;
    double timeout;
};

struct TCPSocketClass {
    struct Class _;
    struct Method read;
    struct Method write;
    struct Method read_until;
    struct Method get_sockfd;
    struct Method set_option;
    struct Method get_option;
    struct Method get_host_info;
    struct Method get_peer_info;
};

struct TCPClientVirtualTable {
    struct VirtualTable _;
    struct Method connect;
};

struct TCPClient {
    struct Object _;
    const void *_vtab;
    char *address;
    char *port;
};

struct TCPClientClass {
    struct Class _;
    struct Method connect;
};

struct TCPServerVirtualTable {
    struct VirtualTable _;
    struct Method accept;
};

struct TCPServer {
    struct Object _;
    const void *_vtab;
    char *address;
    char *port;
    int backlog;
    unsigned int option; /* for future use */
    int lfd;
};

struct TCPServerClass {
    struct Class _;
    struct Method accept;
    struct Method start;
    struct Method get_lfd;
};

#endif /* net_r_h */
