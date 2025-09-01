//
//  rpc.h
//  AAOS
//
//  Created by huyi on 2018/11/8.
//  Copyright © 2018年 National Astronomical Observatories, Chinese Academy of Sciences. All rights reserved.
//

#ifndef rpc_h
#define rpc_h

#include "net.h"
#include "protocol.h"

/*
 * That rpc_call returns a positive number means a failure occurs on the server side, otherwise, means a network failure.
 */
#ifdef __cplusplus
extern "C" {
#endif

int rpc_call(void *_self);
int rpc_read(void *_self);
int rpc_write(void *_self);
int rpc_process(void *_self);
int rpc_execute(void *_self);
int rpc_register(void *_self, double timeout);
int rpc_inspect(void *_self);

const void *RPC(void);
const void *RPCClass(void);
const void *RPCVirtualTable(void);

int rpc_client_connect(void *_self, void **client);

extern const void *RPCClient(void);
extern const void *RPCClientClass(void);
extern const void *RPCClientVirtualTable(void);

int rpc_server_accept(void *_self, void **client);
int rpc_server_accept2(void *_self, void **client);
void rpc_server_start(void *_self);

extern const void *RPCServer(void);
extern const void *RPCServerClass(void);
extern const void *RPCServerVirtualTable(void);

#ifdef __cplusplus
}
#endif

#define RPC_PER_THREAD      0x8000
#define RPC_PRE_THREADED    0x4000
#define RPC_TCP             0x0001
#define RPC_UNIX_DOMAIN     0x0002

#define RPC_DEFAULT         (RPC_PER_THREAD|RPC_TCP)

#endif /* rpc_h */
