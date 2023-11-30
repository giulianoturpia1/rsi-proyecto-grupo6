/**
 * @file udp-server.c
 * @author Giuliano Turpia, Diego Fraga, Ignacio Valettute
 * @version 0.4
 * @date 11/11/2023
 * 
 * @copyright Copyright (c) 2023
 * 
 */

/*---------------------------------------------------------------------------*/
/* Contiki-NG Includes Especificos */
/*---------------------------------------------------------------------------*/
#include <contiki.h>
#include <sys/log.h> //Logging 
#include <net/netstack.h> //Network Stack
#include <net/ipv6/simple-udp.h> //Transport Layer UDP 
#include <sys/node-id.h> //Basic Node Handling Header
#include <net/routing/routing.h> //Setear el nodo como root de la red RPL automaticamente
#include <dev/leds.h> //Led Management

/*---------------------------------------------------------------------------*/
/* Librerias C */
/*---------------------------------------------------------------------------*/
#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

/*---------------------------------------------------------------------------*/
/* App Particular Inclusions */
/*---------------------------------------------------------------------------*/
#include "project-conf.h"

/*---------------------------------------------------------------------------*/
/* Defines Logging Modulo */
/*---------------------------------------------------------------------------*/
#define LOG_MODULE "Server"
#define LOG_LEVEL LOG_LEVEL_INFO

/*---------------------------------------------------------------------------*/
/* Variables */
/*---------------------------------------------------------------------------*/
static struct simple_udp_connection udp_conn;
static radio_value_t ch_num;

/*---------------------------------------------------------------------------*/
/* Declaracion Proceso/AutoStart */
/*---------------------------------------------------------------------------*/
PROCESS(server_pr, "Server multicast");
AUTOSTART_PROCESSES(&server_pr);

/*---------------------------------------------------------------------------*/
/* Callback Server UDP */
/*---------------------------------------------------------------------------*/
static void udp_rx_callback(struct simple_udp_connection *c,
         const uip_ipaddr_t *sender_addr,
         uint16_t sender_port,
         const uip_ipaddr_t *receiver_addr,
         uint16_t receiver_port,
         const uint8_t *data,
         uint16_t datalen)
{   
    /* Variables */
    char strRx[36];

    /* Imprimir callback message en un string */
    snprintf(strRx, sizeof(strRx), "%d bytes received correctly.", datalen);

    /* Toggle LEDs */
    leds_toggle(LEDS_RED);

    /* Log mensaje recibido */
    LOG_INFO("Received message '%.*s' from ", datalen, (char *) data);
    LOG_INFO_6ADDR(sender_addr);
    LOG_INFO_("\n");

    /* Enviar mensaje de callback al nodo que le hablo */
    simple_udp_sendto(&udp_conn, strRx, strlen(strRx), sender_addr);
}

/*---------------------------------------------------------------------------*/
/* Proceso Server UDP */
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(server_pr, event, data){

    PROCESS_BEGIN();

    /* Registrar la conexion - UDP sin IP especificada */
    simple_udp_register(&udp_conn, UDP_PORT, NULL, UDP_PORT, udp_rx_callback);

    /* Inicializar el Node ID */
    node_id_init();

    /* Valores de Canal */
    NETSTACK_RADIO.set_value(RADIO_PARAM_CHANNEL, USABLE_RADIO_CHANNEL);
    NETSTACK_RADIO.get_value(RADIO_PARAM_CHANNEL, &ch_num);
    LOG_INFO("Rf channel: %d\n", ch_num);

    /* Defino al nodo como root de la red RPL */
    NETSTACK_ROUTING.root_start();

    leds_toggle(LEDS_GREEN);

    while(1){
        PROCESS_WAIT_EVENT();
    }
    PROCESS_END();
}