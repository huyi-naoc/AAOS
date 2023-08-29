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

int tcp_socket_get_sockfd(const void *_self);
int tcp_socket_get_host_info(void *_self, char *addr, size_t addr_size, char *port, size_t port_size);
int tcp_socket_get_peer_info(void *_self, char *addr, size_t addr_size, char *port, size_t port_size);
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
int tcp_server_accept(void *_self, void **client);
void tcp_server_start(void *_self);

extern const void *TCPServer(void);
extern const void *TCPServerClass(void);
extern const void *TCPServerVirtualTable(void);

#endif /* net_h */
