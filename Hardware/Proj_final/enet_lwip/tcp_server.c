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
#include "driverlib/eeprom.h"

//TCP Server receive buffer
uint8_t tcp_server_recvbuf[TCP_SERVER_RX_BUFSIZE];
//TCP Server send buffer
char tcp_server_sendbuf[TCP_SERVER_RX_BUFSIZE];

extern int bl_cnt;


uint8_t tcp_server_flag;

uint8_t receive_flag = 0;
uint8_t send_flag = 0;
uint8_t connection_flag = 0;



//TCP Server test
void tcp_server_test(void)
{
    err_t err;
    struct tcp_pcb *tcppcbnew;
    struct tcp_pcb *tcppcbconn;

    uint8_t res=0;
    tcppcbnew=tcp_new();
    if(tcppcbnew)
    {
        err=tcp_bind(tcppcbnew,IP_ADDR_ANY,TCP_SERVER_PORT);
        if(err==ERR_OK)
        {
            tcppcbconn=tcp_listen(tcppcbnew);
            tcp_accept(tcppcbconn,tcp_server_accept);
        }else res=1;
    }else res=1;

    if(res == 1){
        tcp_server_connection_close(tcppcbnew,0);
        tcp_server_connection_close(tcppcbconn,0);
        memset(tcppcbnew,0,sizeof(struct tcp_pcb));
        memset(tcppcbconn,0,sizeof(struct tcp_pcb));
    }

}


err_t tcp_server_accept(void *arg,struct tcp_pcb *newpcb,err_t err)
{
    err_t ret_err;
    struct tcp_server_struct *es;
    LWIP_UNUSED_ARG(arg);
    LWIP_UNUSED_ARG(err);
    tcp_setprio(newpcb,TCP_PRIO_MIN);
    es=(struct tcp_server_struct*)mem_malloc(sizeof(struct tcp_server_struct));
    if(es!=NULL)
    {
        es->state=ES_TCPSERVER_ACCEPTED;
        es->pcb=newpcb;
        es->p=NULL;

        tcp_arg(newpcb,es);
        tcp_recv(newpcb,tcp_server_recv);
        tcp_err(newpcb,tcp_server_error);
        tcp_poll(newpcb,tcp_server_poll,1);
        tcp_sent(newpcb,tcp_server_sent);

        //tcp_server_flag|=1<<5;

        ret_err=ERR_OK;
    }else ret_err=ERR_MEM;
    return ret_err;
}

err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    err_t ret_err;
    uint32_t data_len = 0;
    struct pbuf *q;
    struct tcp_server_struct *es;
    LWIP_ASSERT("arg != NULL",arg != NULL);
    es=(struct tcp_server_struct *)arg;
    if(p==NULL)
    {
        es->state=ES_TCPSERVER_CLOSING;
        es->p=p;
        ret_err=ERR_OK;
    }else if(err!=ERR_OK)
    {
        if(p)pbuf_free(p);
        ret_err=err;
    }else if(es->state==ES_TCPSERVER_ACCEPTED)
    {
        if(p!=NULL)
        {
            memset(tcp_server_recvbuf,0,TCP_SERVER_RX_BUFSIZE);
            for(q=p;q!=NULL;q=q->next)
            {
                if(q->len > (TCP_SERVER_RX_BUFSIZE-data_len)) MEMCPY(tcp_server_recvbuf+data_len,q->payload,(TCP_SERVER_RX_BUFSIZE-data_len));//拷贝数据
                else MEMCPY(tcp_server_recvbuf+data_len,q->payload,q->len);
                data_len += q->len;
                if(data_len > TCP_SERVER_RX_BUFSIZE) break;
            }
            UARTprintf("%s",tcp_server_recvbuf);
            if(tcp_server_recvbuf[0] == '1')
                receive_flag = 1;

            tcp_recved(tpcb,p->tot_len);
            pbuf_free(p);
            //free(tcp_server_sendbuf);
            ret_err=ERR_OK;
        }
    }else
    {
        tcp_recved(tpcb,p->tot_len);
        es->p=NULL;
        pbuf_free(p);
        ret_err=ERR_OK;
    }
    return ret_err;
}

void tcp_server_error(void *arg,err_t err)
{
    LWIP_UNUSED_ARG(err);
    UARTprintf("tcp error:%x\r\n",(uint32_t)arg);
    if(arg!=NULL)mem_free(arg);
}

// Non utilisé
err_t tcp_server_poll(void *arg, struct tcp_pcb *tpcb)
{
    err_t ret_err;
    struct tcp_server_struct *es;
    es=(struct tcp_server_struct *)arg;
    if(es!=NULL)
    {
        UARTprintf("\nNormal\n");
        if(receive_flag == 1){
            UARTprintf("\nSuper\n");

            static uint32_t eepromAddr;
            EEPROMRead(&eepromAddr,0,4);
            if(eepromAddr == 0){
                char* str_nodata = "No Data";
                //tcp_server_sendbuf = malloc(strlen(str_nodata)+1);
                strcpy(tcp_server_sendbuf,str_nodata);
            } else {
                //tcp_server_sendbuf = malloc(8*sizeof(char)*eepromAddr+1);
                *tcp_server_sendbuf = '\0';
                if(eepromAddr != 4294967295)
                {
                    int var = 0;
                    uint32_t temp;
                    char bufferConvert[10];
                    for (var = 4; var <= eepromAddr; var+=4) {
                        EEPROMRead(&temp,var,4);
                        //UARTprintf("%d ",temp);
                        sprintf(bufferConvert,"%d",temp);
                        UARTprintf("%s",bufferConvert);
                        strcat(tcp_server_sendbuf,bufferConvert);
                        strcat(tcp_server_sendbuf,",");
                    }
                    EEPROMMassErase();
                    uint32_t zero = 0;
                    EEPROMProgram(&zero, 0, 4);
                    EEPROMRead(&eepromAddr,0,4);
                }
            }

            es->p=pbuf_alloc(PBUF_TRANSPORT,strlen((char*)tcp_server_sendbuf),PBUF_POOL);//申请内存
            pbuf_take(es->p,(char*)tcp_server_sendbuf,strlen((char*)tcp_server_sendbuf));
            tcp_server_senddata(tpcb,es);
            if(es->p!=NULL)pbuf_free(es->p);    //释放内存

            receive_flag = 0;
        } else if(es->state==ES_TCPSERVER_CLOSING)
        {
            tcp_server_connection_close(tpcb,es);
        }
        ret_err=ERR_OK;
    }else
    {
        tcp_abort(tpcb);
        ret_err=ERR_ABRT;
    }
    return ret_err;
}


err_t tcp_server_sent(void *arg, struct tcp_pcb *tpcb, uint16_t len)
{
    struct tcp_server_struct *es;
    LWIP_UNUSED_ARG(len);
    es = (struct tcp_server_struct *) arg;
    if(es->p)tcp_server_senddata(tpcb,es);
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
            es->p=ptr->next;
            if(es->p)pbuf_ref(es->p);
            pbuf_free(ptr);
            tcp_recved(tpcb,plen);
        }else if(wr_err==ERR_MEM)es->p=ptr;
     }
}
//关闭tcp连接
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
