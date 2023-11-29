/**
 * @file udp-client.c
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
#include <sys/log.h> //Logging 
#include <net/netstack.h> //Network Stack
#include <dev/leds.h>
#include <simple-udp.h> //Transport Layer UDP 
#include <sys/node-id.h> //Basic Node Handling Header
#include <net/routing/routing.h>
#include <dev/button-hal.h>//Button Management
#include <dev/leds.h> //Led Management
#include <uip.h> //To setup the IPV6 ip target

/*---------------------------------------------------------------------------*/
/*C Imported Libs*/
/*---------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>

/*---------------------------------------------------------------------------*/
/* App particular inclusions */
/*---------------------------------------------------------------------------*/
#include "project-conf.h"
#define LOG_MODULE "Client"
#define LOG_LEVEL LOG_LEVEL_DBG

/*---------------------------------------------------------------------------*/
/*Variables, Callback Defines*/
/*---------------------------------------------------------------------------*/
static struct simple_udp_connection udp_client;
static process_event_t event_count_button;
static radio_value_t ch_num;
static struct etimer eTimerButton;

/*---------------------------------------------------------------------------*/
/*Declaracion Proceso/AutoStart*/
/*---------------------------------------------------------------------------*/
PROCESS(udp_client_process, "UDP client");
PROCESS(node_select_process, "Node Selections");

AUTOSTART_PROCESSES(&udp_client_process, &node_select_process);

/* rx_callback definicion */
static void
udp_rx_callback(struct simple_udp_connection *  c,
                const uip_ipaddr_t *            sender_addr,
                uint16_t                        sender_port,
                const uip_ipaddr_t *            receiver_addr,
                uint16_t                        receiver_port,
                const uint8_t *                 data,
                uint16_t                        datalen)
{
    
    LOG_INFO("Respuesta recibida '%.*s' de ", datalen, (char *) data);
    LOG_INFO_6ADDR(sender_addr);
    LOG_INFO_("\n");
}


/*---------------------------------------------------------------------------*/
/*UDP CLIENT PROCESS*/
/*---------------------------------------------------------------------------*/

PROCESS_THREAD(udp_client_process, ev, data)
{
    PROCESS_BEGIN();

    /* Variables */
    static struct etimer periodic_timer;
    static uip_ipaddr_t dest_ipaddr;
    static uip_ipaddr_t empty_addr;
    static char str[36];

    /* Crear la IP vacia del servidor */
    uip_ip6addr(&empty_addr, 0xFD00, 0, 0, 0,0x200, 0, 0, 0);
    uip_ip6addr(&dest_ipaddr, 0xFD00, 0, 0, 0,0x200, 0, 0, 0);

    /* Registrar la conexion - UDP sin IP especificada */
    simple_udp_register(&udp_client, UDP_PORT, NULL, UDP_PORT, udp_rx_callback);

     /* Inicializar el Node ID */
    node_id_init();

    /* Valores de Canal */
    NETSTACK_RADIO.set_value(RADIO_PARAM_CHANNEL, USABLE_RADIO_CHANNEL);
    NETSTACK_RADIO.get_value(RADIO_PARAM_CHANNEL, &ch_num);
    LOG_INFO("Rf channel: %d\n", ch_num);

    while(1){
        /* Timer set y wait. */
        etimer_set(&periodic_timer, SEND_INTERVAL);
        PROCESS_WAIT_EVENT();
        if(etimer_expired(&periodic_timer)){

            if (NETSTACK_ROUTING.node_is_reachable() && !uip_ipaddr_cmp(&dest_ipaddr, &empty_addr)) { //En caso de que este en la red y llegue a destino
                /* Generar el payload */
                snprintf(str, sizeof(str), "Este es el nodo %x.", node_id);

                /* Enviar mensaje para Log*/
                LOG_INFO("Enviando mensaje a: ");
                LOG_INFO_6ADDR(&dest_ipaddr);
                LOG_INFO_("\n");

                /* Enviar mensaje */
                simple_udp_sendto(&udp_client, str, strlen(str), &dest_ipaddr);
            } else if (uip_ipaddr_cmp(&dest_ipaddr, &empty_addr))
            {
                LOG_INFO("No se ha seleccionado un nodo destino\n");

            } else if(!NETSTACK_ROUTING.node_is_reachable() && NETSTACK_ROUTING.node_has_joined){ //En caso de que este en la red pero no llegue a destino
                LOG_INFO("Nodo no llega a destino en la Red TSCH/RPL aun\n");
                LOG_INFO("Se intenta alcanzar el nodo: ");
                LOG_INFO_6ADDR(&dest_ipaddr);
                LOG_INFO_("\n");
            }
        }
        if(ev == event_count_button){ //En caso de que se quiera un nodo en particular
            uint8_t targetNode = *(int8_t*)data;
            LOG_INFO("Correctamente seleccionado: Nodo %d\n", targetNode);
            uip_ip6addr(&dest_ipaddr, 0xFD00, 0, 0, 0,0x200 + targetNode, targetNode, targetNode, targetNode);
        }
    }
    PROCESS_END();
}
/*---------------------------------------------------------------------------*/
/*NODE SELECT PROCESS*/
/*---------------------------------------------------------------------------*/

PROCESS_THREAD(node_select_process, ev, data){

    PROCESS_BEGIN();
    
    /* Variables */
    static int8_t buttonPressCount = 0;

    /* Registrar el evento */
    event_count_button = process_alloc_event();

    etimer_set(&eTimerButton, 2*CLOCK_SECOND);// <---- Definir el tiempo de espera para el boton
    
    while (1)
    {
        PROCESS_WAIT_EVENT();

        if(ev == button_hal_press_event){
            etimer_restart(&eTimerButton); //Was previously done with restart
            buttonPressCount++;
            LOG_INFO("Seleccionado: Nodo %d\n", buttonPressCount);
        }
        if(etimer_expired(&eTimerButton) && buttonPressCount != 0){
            int8_t auxPost = buttonPressCount;
            process_post(&udp_client_process,event_count_button, &auxPost);
            
            LOG_INFO("Objetivo actualizado a: Nodo %d\n", buttonPressCount);
            buttonPressCount = 0;
        }
    }
    PROCESS_END();

}