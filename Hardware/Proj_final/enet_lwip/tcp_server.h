/*
 * tcp_server.h
 *
 *  Created on: Feb 11, 2018
 *      Author: chenm
 */

#ifndef TCP_SERVER_H_
#define TCP_SERVER_H_

#include <stdint.h>
#include "lwip/opt.h"
#include "lwip/err.h"
#include "lwip/pbuf.h"
#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"
#include "lwip/memp.h"
#include "lwip/mem.h"
//////////////////////////////////////////////////////////////////////////////////

#define TCP_SERVER_RX_BUFSIZE   2000    //����tcp server���������ݳ���
#define TCP_SERVER_PORT         8088    //����tcp server�Ķ˿�

//tcp����������״̬
enum tcp_server_states
{
    ES_TCPSERVER_NONE = 0,      //û������
    ES_TCPSERVER_ACCEPTED,      //�пͻ�����������
    ES_TCPSERVER_CLOSING,       //�����ر�����
};
//LWIP�ص�����ʹ�õĽṹ��
struct tcp_server_struct
{
    uint8_t state;               //��ǰ����״
    struct tcp_pcb *pcb;    //ָ��ǰ��pcb
    struct pbuf *p;         //ָ�����/�����pbuf
};

void tcp_server_test(void);//TCP Server���Ժ���
err_t tcp_server_accept(void *arg,struct tcp_pcb *newpcb,err_t err);
err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
void tcp_server_error(void *arg,err_t err);
err_t tcp_server_poll(void *arg, struct tcp_pcb *tpcb);
err_t tcp_server_sent(void *arg, struct tcp_pcb *tpcb, u16_t len);
void tcp_server_senddata(struct tcp_pcb *tpcb, struct tcp_server_struct *es);
void tcp_server_connection_close(struct tcp_pcb *tpcb, struct tcp_server_struct *es);
void tcp_server_remove_timewait(void);
void tcp_server_test(void);

#endif /* TCP_SERVER_H_ */
