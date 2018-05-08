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
#include "tcpecho.h"
#include "stdint.h"
#include "lwip/opt.h"
#include "fsl_pit.h"

#if LWIP_NETCONN

#include "lwip/sys.h"
#include "lwip/api.h"
#define  PAUSAR			0x44 	//D
#define  REPRODUCIR		0x52	//R
#define  ESTADISTICAS	0x53	//S
#define  PUERTO			0x50	//P
/*-----------------------------------------------------------------------------------*/
bool port_Flag  = false;
char port[5];	/* Arreglo variable para guardar el puerto introducido por el usuario */

static void 
tcpecho_thread(void *arg)
{
  struct netconn *conn, *newconn;
  err_t err;
  LWIP_UNUSED_ARG(arg);

  /* Create a new connection identifier. */
  /* Bind connection to well known port number 7. */
#if LWIP_IPV6
  conn = netconn_new(NETCONN_TCP_IPV6);
  netconn_bind(conn, IP6_ADDR_ANY, 50017);
#else /* LWIP_IPV6 */
  conn = netconn_new(NETCONN_TCP);
  netconn_bind(conn, IP_ADDR_ANY, 50017);
#endif /* LWIP_IPV6 */
  LWIP_ERROR("tcpecho: invalid conn", (conn != NULL), return;);

  /* Tell connection to go into listening mode. */
  netconn_listen(conn);

  while (1) {

    /* Grab new connection. */
    err = netconn_accept(conn, &newconn);
    /*printf("accepted new connection %p\n", newconn);*/
    /* Process the new connection. */
    if (err == ERR_OK) {
      struct netbuf *buf;
      void *data;
      u16_t len;
      char dummy[] = "Detener/Reproducir audio: [D] o [R] \r\n Seleccionar puerto fuente de audio: [P] \r\n Desplegar estadisticas de la comunicacion: [S]"; //120 caracteres
      u16_t len_dummy = sizeof(dummy) ;
      netconn_write(newconn, dummy, len_dummy, NETCONN_COPY);		/* Enviar mensaje de menú principal */

      char detener[] = "\r\nDeteniendo el Audio";
      u16_t len_det = sizeof(detener);
      char reproducir[] = "\r\nReproduciendo el Audio";
      u16_t len_rep = sizeof(detener);
      char comp_data[1];
      char estadisticas[] = "\r\nMostrando estadisticas";
      u16_t len_stat = sizeof(estadisticas);
      char puerto[] = "\r\nEscribe el puerto (50000 en adelante):";
      u16_t len_port = sizeof(puerto);
      


    // BLOQUE DE RECEPCIÓN Y HANDLE DE DATOS INTRODUCIDOS POR EL USUARIO EN EL SOCKET CLIENTE DEL SMARTPHONE
      while ((err = netconn_recv(newconn, &buf)) == ERR_OK) {	//si recibe
        /*printf("Recved\n");*/
        do {
             netbuf_data(buf, &data, &len);
             err = netconn_write(newconn, data, len, NETCONN_COPY);

             netbuf_copy(buf,comp_data,1); /* Copio el búfer recibido en mi arreglo de prueba */

             if(true == port_Flag)
             {
            	 netbuf_copy(buf, port, 5);
            	 netconn_write(newconn, port, 5, NETCONN_COPY);
            	 port_Flag = false;
             }

             switch(*comp_data)
             {
             case PAUSAR:
            	 netconn_write(newconn, detener, len_det, NETCONN_COPY);
            	 PIT_StopTimer(PIT, kPIT_Chnl_0);
            	 break;

             case REPRODUCIR:
            	 netconn_write(newconn, reproducir, len_rep, NETCONN_COPY);
            	 PIT_StartTimer(PIT, kPIT_Chnl_0);
                 break;

             case ESTADISTICAS:
            	 netconn_write(newconn, estadisticas, len_stat, NETCONN_COPY);
                 break;

             case PUERTO:
            	 netconn_write(newconn, puerto, len_port, NETCONN_COPY);
            	 port_Flag = true;
            	 break;


             }

#if 0
            if (err != ERR_OK) {
              printf("tcpecho: netconn_write: error \"%s\"\n", lwip_strerr(err));
            }
#endif
        } while (netbuf_next(buf) >= 0);
        netbuf_delete(buf);
      }
      /*printf("Got EOF, looping\n");*/ 
      /* Close connection and discard connection identifier. */
      netconn_close(newconn);
      netconn_delete(newconn);
    }
  }
}
/*-----------------------------------------------------------------------------------*/
void
tcpecho_init(void)
{
  sys_thread_new("tcpecho_thread", tcpecho_thread, NULL, DEFAULT_THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);
}
/*-----------------------------------------------------------------------------------*/
uint16_t get_Port()
{
	uint16_t casted_port = (uint16_t)*port;
	return casted_port;
}
#endif /* LWIP_NETCONN */
