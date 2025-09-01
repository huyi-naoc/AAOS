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

//#define  _NET_PRIORITY_ _VIRTUAL_PRIORITY_ + 1

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
    struct Method accept2;
};

struct TCPServer {
    struct Object _;
    const void *_vtab;
    char *address;
    char *path;
    char *port;
    int backlog;
    int lfd;
    int lfd2;
    size_t n_threads;
    size_t max_events;
    unsigned int option; /* for future use */
    double timeout;
};

struct TCPServerClass {
    struct Class _;
    struct Method accept;
    struct Method accept2;
    struct Method start;
    struct Method get_lfd;
    struct Method get_lfds;
    struct Method get_option;
    struct Method set_option;
    struct Method set_address;
    struct Method set_path;
};

struct UDSClientVirtualTable {
    struct VirtualTable _;
    struct Method connect;
};

struct UDSClient {
    struct Object _;
    const void *_vtab;
    char *path;
};

struct UDSClientClass {
    struct Class _;
    struct Method connect;
};

struct UDSServerVirtualTable {
    struct VirtualTable _;
    struct Method accept;
};

struct UDSServer {
    struct Object _;
    const void *_vtab;
    char *path;
    int lfd;
    size_t n_threads;
    size_t max_events;
    unsigned int option; /* for future use */
    double timeout;
};

struct UDSServerClass {
    struct Class _;
    struct Method accept;
    struct Method start;
    struct Method get_lfd;
    struct Method get_option;
    struct Method set_option;
};


#endif /* net_r_h */
