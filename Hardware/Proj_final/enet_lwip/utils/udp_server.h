/*
 * udp_server.h
 *
 *  Created on: Feb 11, 2018
 *      Author: chenm
 */

#ifndef UTILS_UDP_SERVER_H_
#define UTILS_UDP_SERVER_H_

#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "lwip/tcp.h"

#define UDP_DEMO_RX_BUFSIZE     2000
#define UDP_DEMO_PORT           8888


void udp_server_init(void);
void udp_server_recv(void *arg,struct udp_pcb *upcb,struct pbuf *p,struct ip_addr *addr,u16_t port);
void udp_server_senddata(struct udp_pcb *upcb);
void udp_server_connection_close(struct udp_pcb *upcb);


#endif /* UTILS_UDP_SERVER_H_ */
