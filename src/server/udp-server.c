/**
 * @file udp-server.c
 * @author Giuliano Turpia, Diego Fraga, Ignacio Valettute
 * @brief 
 * @version 0.3
 * @date 11-11-2023
 * 
 * @copyright Copyright (c) 2023
 * 
 */

/*---------------------------------------------------------------------------*/
/*Contiki-NG Specific Includes*/
/*---------------------------------------------------------------------------*/
#include <contiki.h>
#include <sys/log.h>
#include <net/netstack.h>
#include <net/ipv6/simple-udp.h>
#include <dev/leds.h>
#include <sys/node-id.h>
#include <net/routing/routing.h> //To set the node as root automatically


/*---------------------------------------------------------------------------*/
/*C Imported Libs*/
/*---------------------------------------------------------------------------*/
#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

/*---------------------------------------------------------------------------*/
/* App particular inclusions */
/*---------------------------------------------------------------------------*/
#include "project-conf.h"
#define LOG_MODULE "Server"
#define LOG_LEVEL LOG_LEVEL_INFO

/*---------------------------------------------------------------------------*/
/*Variables, Callback Defines*/
/*---------------------------------------------------------------------------*/
static struct simple_udp_connection udp_conn;
static radio_value_t ch_num;

/*---------------------------------------------------------------------------*/
/*Declaracion Proceso/AutoStart*/
/*---------------------------------------------------------------------------*/
PROCESS(server_pr, "Server multicast");
AUTOSTART_PROCESSES(&server_pr);

/* rx_callback function */
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

    /* Toggle leds */
    leds_toggle(LEDS_RED);

    /* Log mensaje recibido */
    LOG_INFO("Received message '%.*s' from ", datalen, (char *) data);
    LOG_INFO_6ADDR(sender_addr);
    #if LLSEC802154_CONF_ENABLED
        LOG_INFO_(" LLSEC LV:%d", uipbuf_get_attr(UIPBUF_ATTR_LLSEC_LEVEL));
    #endif
    LOG_INFO_("\n");

    /* Enviar mensaje de callback al nodo que le hablo */
    simple_udp_sendto(&udp_conn, strRx, strlen(strRx), sender_addr);
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(server_pr, event, data){
    static char strTx[36];
    uip_ipaddr_t dest_ip_addr;

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
    //NETSTACK_ROUTING.root_start();

    /* Armo el mensaje a enviar */
    snprintf(strTx, sizeof(strTx), "Servidor del DODAG: %x", node_id);

    /* Generar direccion */
    uip_create_linklocal_allnodes_mcast(&dest_ip_addr);

    /* Log multicast IP */
    LOG_INFO("Sending multicast (IP: ");
    LOG_INFO_6ADDR(&dest_ip_addr);
    LOG_INFO(")\n");

    /* Enviar mensaje */
    simple_udp_sendto(&udp_conn, strTx, strlen(strTx), &dest_ip_addr);

    /* Toggle LEDs para indicar mensaje enviado */
    leds_toggle(LEDS_GREEN);

    while(1){
        PROCESS_WAIT_EVENT();
    }
    PROCESS_END();
}