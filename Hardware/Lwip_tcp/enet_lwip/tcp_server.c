/*
 * tcp_server.c
 *
 *  Created on: Feb 11, 2018
 *      Author: chenm
 */

#include "tcp_server.h"

u8_t tcp_server_recvbuf[TCP_SERVER_RX_BUFSIZE];
u8_t *tcp_server_sendbuf="I knew I could do it!\r\n";


//TCP Server Flag
//bit7:0,No data to be sent; 1,data to be sent
//bit6:0,No data received; 1 data received.
//bit5:0 No client; 1 Client.
//bit4~0 Reserved
u8_t tcp_server_flag;

void tcp_server_test(void){
    err_t err;
    struct tcp_pcb *tcpPcbNew;
    struct tcp_pcb *tcpPcbConn;

    u8_t *tbuf;
    u8_t key;
    u8_t res=0;
    u8_t t=0;

    sprintf((char*)tbuf,"Server IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);//������IP
    LCD_ShowString(30,130,210,16,16,tbuf);
        sprintf((char*)tbuf,"Server Port:%d",TCP_SERVER_PORT);//�������˿ں�
        LCD_ShowString(30,150,210,16,16,tbuf);
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
        POINT_COLOR=BLUE;//��ɫ����
        while(res==0)
        {
            key=KEY_Scan(0);
            if(key==WKUP_PRES)break;
            if(key==KEY0_PRES)//KEY0������,��������
            {
                tcp_server_flag|=1<<7;//���Ҫ��������
            }
            if(tcp_server_flag&1<<6)//�Ƿ��յ�����?
            {
                LCD_Fill(30,210,lcddev.width-1,lcddev.height-1,WHITE);//����һ������
                LCD_ShowString(30,210,lcddev.width-30,lcddev.height-210,16,tcp_server_recvbuf);//��ʾ���յ�������
                tcp_server_flag&=~(1<<6);//��������Ѿ���������.
            }
            if(tcp_server_flag&1<<5)//�Ƿ�������?
            {
                sprintf((char*)tbuf,"Client IP:%d.%d.%d.%d",lwipdev.remoteip[0],lwipdev.remoteip[1],lwipdev.remoteip[2],lwipdev.remoteip[3]);//�ͻ���IP
                LCD_ShowString(30,170,230,16,16,tbuf);
                POINT_COLOR=RED;
                LCD_ShowString(30,190,lcddev.width-30,lcddev.height-190,16,"Receive Data:");//��ʾ��Ϣ
                POINT_COLOR=BLUE;//��ɫ����
            }else if((tcp_server_flag&1<<5)==0)
            {
                LCD_Fill(30,170,lcddev.width-1,lcddev.height-1,WHITE);//����
            }
            lwip_periodic_handle();
            lwip_pkt_handle();
            delay_ms(2);
            t++;
            if(t==200)
            {
                t=0;
                LED0=!LED0;
            }
        }
        tcp_server_connection_close(tcppcbnew,0);//�ر�TCP Server����
        tcp_server_connection_close(tcppcbconn,0);//�ر�TCP Server����
        tcp_server_remove_timewait();
        memset(tcppcbnew,0,sizeof(struct tcp_pcb));
        memset(tcppcbconn,0,sizeof(struct tcp_pcb));
        myfree(SRAMIN,tbuf);
        LCD_Clear(WHITE);           //����
        POINT_COLOR = RED;
        LCD_ShowString(30,30,200,16,16,"WARSHIP STM32F103");
        LCD_ShowString(30,50,200,16,16,"TCP Server Test");
        LCD_ShowString(30,70,200,16,16,"ATOM@ALIENTEK");

        POINT_COLOR=BLUE;
        LCD_ShowString(30,90,200,16,16,"Connect break��");
        LCD_ShowString(30,110,200,16,16,"KEY1:Connect");
}


err_t tcp_server_accept(void *arg,struct tcp_pcb *newpcb,err_t err){

}

err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err){

}

void tcp_server_error(void *arg,err_t err){

}

err_t tcp_server_poll(void *arg, struct tcp_pcb *tpcb){

}

err_t tcp_server_sent(void *arg, struct tcp_pcb *tpcb, u16_t len){

}

void tcp_server_senddata(struct tcp_pcb *tpcb, struct tcp_server_struct *es){

}

void tcp_server_connection_close(struct tcp_pcb *tpcb, struct tcp_server_struct *es){

}

void tcp_server_remove_timewait(void){

}




