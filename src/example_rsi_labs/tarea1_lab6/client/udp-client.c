/**
 * @file udp-client.c
 * @author Giuliano Turpia, Diego Fraga, Ignacio Valettute
 * @brief 
 * @version 0.1
 * @date 2023-08-29
 * 
 * @copyright Copyright (c) 2023
 * 
 */

/*---------------------------------------------------------------------------*/
/*Contiki-NG Specific Includes*/
/*---------------------------------------------------------------------------*/
#include "contiki.h"    
#include "dev/leds.h"
#include "simple-udp.h" //Transport Layer UDP 
#include "sys/node-id.h" //Basic Node Handling Header
#include "net/netstack.h"
#include "net/routing/routing.h"


/*---------------------------------------------------------------------------*/
/*C Imported Libs*/
/*---------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//System Logging and other lib
#include "project-conf.h"
#include "sys/log.h" 
#define LOG_MODULE "Client"
#define LOG_LEVEL LOG_LEVEL_DBG


/*---------------------------------------------------------------------------*/
/*Variables, Callback Defines*/
/*---------------------------------------------------------------------------*/
static struct simple_udp_connection udp_client;
static uip_ipaddr_t target_ip_addr;
static radio_value_t ch_num;

/*---------------------------------------------------------------------------*/
/*Process Declaration/AutoStart*/
/*---------------------------------------------------------------------------*/
PROCESS(udp_client_process, "UDP client");
AUTOSTART_PROCESSES(&udp_client_process);

/* rx_callback definition */
static void
udp_rx_callback(struct simple_udp_connection *  c,
                const uip_ipaddr_t *            sender_addr,
                uint16_t                        sender_port,
                const uip_ipaddr_t *            receiver_addr,
                uint16_t                        receiver_port,
                const uint8_t *                 data,
                uint16_t                        datalen)
{
    
    LOG_INFO("Received request '%.*s' from ", datalen, (char *) data);
    LOG_INFO_6ADDR(sender_addr);
    LOG_INFO_("\n");

    /* Set the ip-address to send data to the server node. */
    //uip_ipaddr_copy(&target_ip_addr,sender_addr);

}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_client_process, ev, data)
{
    static struct etimer periodic_timer;
    static char str[225];
    //static const int algo = 16;

    PROCESS_BEGIN();

    // Register the connection - WITH NO SPECIFIC IPV6 ADDRESS
    simple_udp_register(&udp_client, UDP_PORT, NULL, UDP_PORT, udp_rx_callback);

    //Initialize Node Id
    node_id_init();

    //Channel Values
    NETSTACK_RADIO.set_value(RADIO_PARAM_CHANNEL, USABLE_RADIO_CHANNEL);
    NETSTACK_RADIO.get_value(RADIO_PARAM_CHANNEL, &ch_num);
    LOG_INFO("Rf channel: %d\n", ch_num);
     
    uip_ip6addr(&target_ip_addr, 0xfe80, 0, 0, 0, 0xc30c, 0, 0, 1);
    while(1){
        //LOG_DBG("Loop");
        /* Timer set and wait. */
        etimer_set(&periodic_timer, SEND_INTERVAL);
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));

        // Generate the string
        //for(int i = 0; i < algo; i++)
        //{
        snprintf(str, sizeof(PAYLOAD), PAYLOAD);
        //}

        //LOG_INFO("Sending payload to server\n");
        simple_udp_sendto(&udp_client, str, strlen(str), &target_ip_addr);

    }
    PROCESS_END();
}