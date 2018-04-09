/*
 * tcp_server.c
 *
 *  Created on: Feb 11, 2018
 *      Author: chenm
 */
#include <stdbool.h>
#include "tcp_server.h"
#include "lwip/opt.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"
#include "driverlib/sysctl.h"
#include "utils/uartstdio.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

//TCP Server receive buffer
uint8_t tcp_server_recvbuf[TCP_SERVER_RX_BUFSIZE];
//TCP Server send buffer
const uint8_t *tcp_server_sendbuf="TCP Server send data\r\n";


uint8_t tcp_server_flag;

uint8_t receive_flag = 0;
uint8_t send_flag = 0;
uint8_t connection_flag = 0;


//TCP Server test
void tcp_server_test(void)
{
    err_t err;
    struct tcp_pcb *tcppcbnew;      //����һ��TCP���������ƿ�
    struct tcp_pcb *tcppcbconn;     //����һ��TCP���������ƿ�

    uint8_t res=0;
    tcppcbnew=tcp_new();    //����һ���µ�pcb
    if(tcppcbnew)           //�����ɹ�
    {
        err=tcp_bind(tcppcbnew,IP_ADDR_ANY,TCP_SERVER_PORT);    //������IP��ָ���Ķ˿ںŰ���һ��,IP_ADDR_ANYΪ�󶨱������е�IP��ַ
        if(err==ERR_OK) //�����
        {
            tcppcbconn=tcp_listen(tcppcbnew);           //����tcppcb�������״̬
            tcp_accept(tcppcbconn,tcp_server_accept);   //��ʼ��LWIP��tcp_accept�Ļص�����
        }else res=1;
    }else res=1;

    if(res == 1){
        tcp_server_connection_close(tcppcbnew,0);//�ر�TCP Server����
        tcp_server_connection_close(tcppcbconn,0);//�ر�TCP Server����
        memset(tcppcbnew,0,sizeof(struct tcp_pcb));
        memset(tcppcbconn,0,sizeof(struct tcp_pcb));
    }

}

//lwIP tcp_accept()�Ļص�����
err_t tcp_server_accept(void *arg,struct tcp_pcb *newpcb,err_t err)
{
    err_t ret_err;
    struct tcp_server_struct *es;
    LWIP_UNUSED_ARG(arg);
    LWIP_UNUSED_ARG(err);
    tcp_setprio(newpcb,TCP_PRIO_MIN);//�����´�����pcb���ȼ�
    es=(struct tcp_server_struct*)mem_malloc(sizeof(struct tcp_server_struct)); //�����ڴ�
    if(es!=NULL) //�ڴ����ɹ�
    {
        es->state=ES_TCPSERVER_ACCEPTED;    //��������
        es->pcb=newpcb;
        es->p=NULL;

        tcp_arg(newpcb,es);
        tcp_recv(newpcb,tcp_server_recv);   //��ʼ��tcp_recv()�Ļص�����
        tcp_err(newpcb,tcp_server_error);   //��ʼ��tcp_err()�ص�����
        tcp_poll(newpcb,tcp_server_poll,1); //��ʼ��tcp_poll�ص�����
        tcp_sent(newpcb,tcp_server_sent);   //��ʼ�����ͻص�����

        //tcp_server_flag|=1<<5;              //����пͻ���������

        ret_err=ERR_OK;
    }else ret_err=ERR_MEM;
    return ret_err;
}
//lwIP tcp_recv()�����Ļص�����
err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    err_t ret_err;
    uint32_t data_len = 0;
    struct pbuf *q;
    struct tcp_server_struct *es;
    LWIP_ASSERT("arg != NULL",arg != NULL);
    es=(struct tcp_server_struct *)arg;
    if(p==NULL) //�ӿͻ��˽��յ�������
    {
        es->state=ES_TCPSERVER_CLOSING;//��Ҫ�ر�TCP ������
        es->p=p;
        ret_err=ERR_OK;
    }else if(err!=ERR_OK)   //�ӿͻ��˽��յ�һ���ǿ�����,��������ĳ��ԭ��err!=ERR_OK
    {
        if(p)pbuf_free(p);  //�ͷŽ���pbuf
        ret_err=err;
    }else if(es->state==ES_TCPSERVER_ACCEPTED)  //��������״̬
    {
        if(p!=NULL)  //����������״̬���ҽ��յ������ݲ�Ϊ��ʱ�����ӡ����
        {
            memset(tcp_server_recvbuf,0,TCP_SERVER_RX_BUFSIZE);  //���ݽ��ջ���������
            for(q=p;q!=NULL;q=q->next)  //����������pbuf����
            {
                //�ж�Ҫ������TCP_SERVER_RX_BUFSIZE�е������Ƿ����TCP_SERVER_RX_BUFSIZE��ʣ��ռ䣬�������
                //�Ļ���ֻ����TCP_SERVER_RX_BUFSIZE��ʣ�೤�ȵ����ݣ�����Ļ��Ϳ������е�����
                if(q->len > (TCP_SERVER_RX_BUFSIZE-data_len)) MEMCPY(tcp_server_recvbuf+data_len,q->payload,(TCP_SERVER_RX_BUFSIZE-data_len));//��������
                else MEMCPY(tcp_server_recvbuf+data_len,q->payload,q->len);
                data_len += q->len;
                if(data_len > TCP_SERVER_RX_BUFSIZE) break; //����TCP�ͻ��˽�������,����
            }
            UARTprintf("%s",tcp_server_recvbuf);

            es->p=pbuf_alloc(PBUF_TRANSPORT,strlen((char*)tcp_server_sendbuf),PBUF_POOL);//�����ڴ�
            pbuf_take(es->p,(char*)tcp_server_sendbuf,strlen((char*)tcp_server_sendbuf));
            tcp_server_senddata(tpcb,es);       //��ѯ��ʱ����Ҫ���͵�����

            tcp_recved(tpcb,p->tot_len);//���ڻ�ȡ��������,֪ͨLWIP���Ի�ȡ��������
            pbuf_free(p);   //�ͷ��ڴ�
            ret_err=ERR_OK;
        }
    }else//�������ر���
    {
        tcp_recved(tpcb,p->tot_len);//���ڻ�ȡ��������,֪ͨLWIP���Ի�ȡ��������
        es->p=NULL;
        pbuf_free(p); //�ͷ��ڴ�
        ret_err=ERR_OK;
    }
    return ret_err;
}
//lwIP tcp_err�����Ļص�����
void tcp_server_error(void *arg,err_t err)
{
    LWIP_UNUSED_ARG(err);
    UARTprintf("tcp error:%x\r\n",(uint32_t)arg);
    if(arg!=NULL)mem_free(arg);//�ͷ��ڴ�
}

// Non utilis��
err_t tcp_server_poll(void *arg, struct tcp_pcb *tpcb)
{
    err_t ret_err;
    struct tcp_server_struct *es;
    es=(struct tcp_server_struct *)arg;
    if(es!=NULL)
    {
        if(es->state==ES_TCPSERVER_CLOSING)//��Ҫ�ر�����?ִ�йرղ���
            {
                tcp_server_connection_close(tpcb,es);//�ر�����
            }
        ret_err=ERR_OK;
    }else
    {
        tcp_abort(tpcb);//��ֹ����,ɾ��pcb���ƿ�
        ret_err=ERR_ABRT;
    }
    return ret_err;
}


err_t tcp_server_sent(void *arg, struct tcp_pcb *tpcb, uint16_t len)
{
    struct tcp_server_struct *es;
    LWIP_UNUSED_ARG(len);
    es = (struct tcp_server_struct *) arg;
    if(es->p)tcp_server_senddata(tpcb,es);//��������
    return ERR_OK;
}


void tcp_server_senddata(struct tcp_pcb *tpcb, struct tcp_server_struct *es)
{
    struct pbuf *ptr;
    uint16_t plen;
    err_t wr_err=ERR_OK;
     while((wr_err==ERR_OK)&&es->p&&(es->p->len<=tcp_sndbuf(tpcb)))
     {
        ptr=es->p;
        wr_err=tcp_write(tpcb,ptr->payload,ptr->len,1);
        if(wr_err==ERR_OK)
        {
            plen=ptr->len;
            es->p=ptr->next;            //ָ����һ��pbuf
            if(es->p)pbuf_ref(es->p);   //pbuf��ref��һ
            pbuf_free(ptr);
            tcp_recved(tpcb,plen);      //����tcp���ڴ�С
        }else if(wr_err==ERR_MEM)es->p=ptr;
     }
}
//�ر�tcp����
void tcp_server_connection_close(struct tcp_pcb *tpcb, struct tcp_server_struct *es)
{
    tcp_close(tpcb);
    tcp_arg(tpcb,NULL);
    tcp_sent(tpcb,NULL);
    tcp_recv(tpcb,NULL);
    tcp_err(tpcb,NULL);
    tcp_poll(tpcb,NULL,0);
    if(es)mem_free(es);
}
