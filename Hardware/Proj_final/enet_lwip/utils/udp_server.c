/*
 * udp_server.c
 *
 *  Created on: Feb 11, 2018
 *      Author: chenm
 */
#include "udp_server.h"

void udp_server_init(void){
    err_t err;
    struct udp_pcb *udpPcb;
    struct ip_addr remoteIpAddr;


}


void udp_server_recv(void *arg,struct udp_pcb *upcb,struct pbuf *p,struct ip_addr *addr,u16_t port){

}

void udp_server_senddata(struct udp_pcb *upcb){

}

void udp_server_connection_close(struct udp_pcb *upcb){

}




