//
//  net.h
//  AAOS
//
//  Created by huyi on 2018/11/8.
//  Copyright © 2018年 National Astronomical Observatories, Chinese Academy of Sciences. All rights reserved.
//

#ifndef net_h
#define net_h

#include "object.h"
#include "virtual.h"

#define TCPSOCKET_OPTION_DO_NOT_RESTART_ON_SIGNAL 0x01
#define TCPSOCKET_OPTION_NOBLOCKING 0x02

#define TCPSERVER_OPTION_BLOCK_PERTHREAD            0x0001
#define TCPSERVER_OPTION_NONBLOCK_PERTHREAD         0x0002
#define TCPSERVER_OPTION_BLOCK_PRETHEADED           0x0004
#define TCPSERVER_OPTION_NONBLOCK_PRETHEADED        0x0008

#define TCPSERVER_OPTION_TCP                        0x0100
#define TCPSERVER_OPTION_UDS                        0x0200

#define TCPSERVER_OPTION_DEFAULT (TCPSERVER_OPTION_BLOCK_PERTHREAD | TCPSERVER_OPTION_TCP | TCPSERVER_OPTION_UDS)

#ifdef __cpluspus
extern "C" {
#endif

int tcp_socket_get_sockfd(const void *_self);
int tcp_socket_read(void *_self, void *read_buffer, size_t request_size, size_t *read_size);
int tcp_socket_read_until(void *_self, void *read_buffer, size_t request_size, size_t *read_size, const char *delim);
int tcp_socket_write(void *_self, const void *write_buffer, size_t request_size, size_t *write_size);

extern const void *TCPSocket(void);
extern const void *TCPSocketClass(void);
extern const void *TCPSocketVirtualTable(void);

int tcp_client_connect(const void *_self, void **client);

extern const void *TCPClient(void);
extern const void *TCPClientClass(void);
extern const void *TCPClientVirtualTable(void);

int tcp_server_get_lfd(const void *_self);
void tcp_server_get_lfds(const void *_self, int *lfds);
int tcp_server_set_option(void *_self, unsigned int option);
int tcp_server_get_option(const void *_self, unsigned int *option);
int tcp_server_accept(void *_self, void **client);
int tcp_server_accept2(void *_self, void **client);
void tcp_server_start(void *_self);
void tcp_server_set_path(void *_self, const char *path);
void tcp_server_set_addrss(void *_self, const char *address);

extern const void *TCPServer(void);
extern const void *TCPServerClass(void);
extern const void *TCPServerVirtualTable(void);

int uds_client_connect(const void *_self, void **client);

extern const void *UDSClient(void);
extern const void *UDSClientClass(void);
extern const void *UDSClientVirtualTable(void);

int uds_server_get_lfd(const void *_self);
int uds_server_get_option(const void *_self, unsigned int *option);
int uds_server_set_option(void *_self, unsigned int option);
int uds_server_accept(void *_self, void **client);
void uds_server_start(void *_self);

extern const void *UDSServer(void);
extern const void *UDSServerClass(void);
extern const void *UDSServerVirtualTable(void);

#ifdef  __cplusplus
}
#endif
#endif /* net_h */
