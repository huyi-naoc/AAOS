//
//  rpc_r.h
//  AAOS
//
//  Created by huyi on 2018/11/8.
//  Copyright © 2018年 National Astronomical Observatories, Chinese Academy of Sciences. All rights reserved.
//

#ifndef rpc_r_h
#define rpc_r_h

#include "net_r.h"
#include "virtual_r.h"

#define _RPC_PRIORITY_ _VIRTUAL_PRIORITY_ + 1

struct RPC {
    struct TCPSocket _;
    const void *_vtab;
    unsigned int option;
    void *protobuf;
};

struct RPCClass {
    struct TCPSocketClass _;
    struct Method read;
    struct Method write;
    struct Method call;
    struct Method process;
    struct Method execute;
    struct Method inspect;
    struct Method reg;
};

struct RPCVirtualTable {
    struct VirtualTable _;
    struct Method call;
    struct Method process;
    struct Method execute;
    struct Method inspect;
    struct Method reg;
};

struct RPCClient {
    struct TCPClient _;
    const void *_vtab;
    int options;
};

struct RPCClientClass {
    struct TCPClientClass _;
    struct Method connect;
};

struct RPCClientVirtualTable {
    struct VirtualTable _;
    struct Method connect;
};

struct RPCServer {
    struct TCPServer _;
    const void *_vtab;
    int option;
};

struct RPCServerClass {
    struct TCPServerClass _;
    struct Method accept;
    struct Method start;
};

struct RPCServerVirtualTable {
    struct VirtualTable _;
    struct Method accept;
    struct Method start;
};

#endif /* rpc_r_h */
