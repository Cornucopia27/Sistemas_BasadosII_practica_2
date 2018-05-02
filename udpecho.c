/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

#include "udpecho.h"

#include "lwip/opt.h"

#if LWIP_NETCONN

#include "lwip/api.h"
#include "lwip/sys.h"

#include "FreeRTOS.h"
#include "task.h"
#include "fsl_pit.h"
#include "fsl_dac.h"

#define PACKET_SIZE 500
#define PACKET_COMPARISON 2


uint8_t counter = 0;
uint16_t ping_buffer[PACKET_SIZE] = {2048};
uint16_t pong_buffer[PACKET_SIZE] = {2048};
uint8_t array_counter = 0;

bool pit_flag = false;

void PIT0_IRQHandler()
{
	PIT_ClearStatusFlags(PIT, kPIT_Chnl_0, kPIT_TimerFlag);
	if(true == pit_flag)
	{
		DAC_SetBufferValue(DAC0, 0U,(ping_buffer[counter]));
	}
	else
	{
		DAC_SetBufferValue(DAC0, 0U,(pong_buffer[counter]));
	}

	counter = (counter < (PACKET_SIZE - PACKET_COMPARISON)) ? counter + 1 : 0;
}

static void
server_thread(void *arg)
{
	struct netconn *conn;
	struct netbuf *buf;

	char *msg;
	uint16_t *data;

//	TickType_t last_tick = 0;

	uint16_t len;

	LWIP_UNUSED_ARG(arg);
	conn = netconn_new(NETCONN_UDP);
	netconn_bind(conn, IP_ADDR_ANY, 50005);
	//LWIP_ERROR("udpecho: invalid conn", (conn != NULL), return;);

	while (1)
	{
		netconn_recv(conn, &buf);
//		netbuf_data(buf, (void**)&data, &len);
		if(false == pit_flag)
		{
			pit_flag = true;
			counter = 0;
			netbuf_copy(buf, ping_buffer, sizeof(ping_buffer));
		}
		else
		{
			pit_flag = false;
			counter = 0;
			netbuf_copy(buf, pong_buffer, sizeof(pong_buffer));
		}
		netbuf_delete(buf);
		PIT_StartTimer(PIT, kPIT_Chnl_0);

	}
}

/*-----------------------------------------------------------------------------------*/
//static void
//client_thread(void *arg)
//{
//	ip_addr_t dst_ip;
//	struct netconn *conn;
//	struct netbuf *buf;
//
//	LWIP_UNUSED_ARG(arg);
//	conn = netconn_new(NETCONN_UDP);
//	//LWIP_ERROR("udpecho: invalid conn", (conn != NULL), return;);
//
//	char *msg = "Hello loopback!";
//	buf = netbuf_new();
//	netbuf_ref(buf,msg,10);
//
//	IP4_ADDR(&dst_ip, 127, 0, 0, 1);
//
//	while (1)
//	{
//		netconn_sendto(conn, buf, &dst_ip, 50000);
//		vTaskDelay(1000);
//	}
//}
/*-----------------------------------------------------------------------------------*/
void
udpecho_init(void)
{
//	sys_thread_new("client", client_thread, NULL, 300, 1);
	sys_thread_new("server", server_thread, NULL, 300, 2);

}

#endif /* LWIP_NETCONN */
