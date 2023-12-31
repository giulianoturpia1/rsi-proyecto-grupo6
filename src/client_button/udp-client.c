/**
 * @file udp-client.c
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
#include <dev/button-hal.h> //Button Management
#include <leds.h> //LED handling
#include <net/netstack.h> //Network Stack
#include <net/routing/routing.h>
#include <simple-udp.h> //Transport Layer UDP 
#include <sys/log.h> //Logging 
#include <sys/ctimer.h>
#include <sys/node-id.h> //Basic Node Handling Header
#include <sys/etimer.h> //Event Timer
#include <sys/timer.h>
#include <uip.h> //IPV6 IP target

/*---------------------------------------------------------------------------*/
/* Librerias C */
/*---------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/*---------------------------------------------------------------------------*/
/* App Particular Inclusions */
/*---------------------------------------------------------------------------*/
#include "project-conf.h"

/*---------------------------------------------------------------------------*/
/* Definicion de macro de configuracion. */
/*---------------------------------------------------------------------------*/
#ifdef CONFIG_VER_CANAL
#ifdef CANAL_0
#ifdef CANAL_1
#define _CONFIG_VER_CANAL
#endif // CANAL_1
#endif // CANAL_0
#endif // CONFIG_VER_CANAL 

/*---------------------------------------------------------------------------*/
/* Defines Logging Modulo */
/*---------------------------------------------------------------------------*/
#define LOG_MODULE "Client"
#define LOG_LEVEL LOG_LEVEL_INFO

#ifndef STRETCH
#define STRETCH 10
#endif

#ifndef DEFAULT_RADIO_DIV
#define DEFAULT_RADIO_DIV 100
#endif

typedef enum estados
{
    OFF,
    ON
} estados_t ;

/*---------------------------------------------------------------------------*/
/* Variables */
/*---------------------------------------------------------------------------*/
static struct simple_udp_connection udp_client;
static process_event_t  event_count_button;
static process_event_t  radio_on_ev;
static process_event_t  radio_off_ev;
static process_event_t  radio_tx_ev;
#ifndef _CONFIG_VER_CANAL
static process_event_t  radio_rx_ev;
#endif

#ifndef _CONFIG_VER_CANAL
static struct ctimer blink_timer;
/* Período durante el cual los LEDs blinkean. */
static const uint32_t BLINK_TIMEOUT = (STRETCH*CLOCK_SECOND)/(DEFAULT_RADIO_DIV);

/* El LED prende y apaga cada x ms */
static const uint32_t SIMPLE_BLINK_TIMEOUT = BLINK_TIMEOUT/5;
#endif

static uint8_t auxPost;
static int (* on_function)(void);
static int (* off_function)(void);
static int (* tx_function)(unsigned short);
#ifndef _CONFIG_VER_CANAL
static int (* rx_function)(void);

static struct timer timer_general;
#endif

/*---------------------------------------------------------------------------*/
/* Declaracion Proceso/AutoStart */
/*---------------------------------------------------------------------------*/
PROCESS(udp_client_process, "UDP client");
PROCESS(node_select_process, "Node Selections");
PROCESS(radio_sniffer_pr, "Radio Sniffer");

AUTOSTART_PROCESSES(&udp_client_process, &node_select_process, &radio_sniffer_pr);

/*---------------------------------------------------------------------------*/
/* Callback Cliente UDP */
/*---------------------------------------------------------------------------*/
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

/* Handler de encendido. */
int on_handler(void)
{
    if (!on_function)
    {
        return 0;
    }

    LOG_DBG("Handler on\n");
    process_post(&radio_sniffer_pr, radio_on_ev, NULL);

    return on_function();
}

/* Handler de función de apagado. */
int off_handler(void)
{
    if (!off_function)
    {
        return 0;
    }

    LOG_DBG("Handler off\n");
    process_post(&radio_sniffer_pr, radio_off_ev, NULL);

    return off_function();
}

/* Función de manejo de transmisión por radio. */
int tx_handler(unsigned short length)
{
    if (!tx_function)
    {
        return 0;
    }

    static int channel;
    LOG_DBG("Handler tx\n");

    NETSTACK_RADIO.get_value(RADIO_PARAM_CHANNEL, &channel);
    process_post(&radio_sniffer_pr, radio_tx_ev, &channel);

    return tx_function(length);
}

#ifndef _CONFIG_VER_CANAL
/* Función de manejo de recepción de radio. */
int rx_handler(void)
{
    static int channel;

    if (!rx_function)
    {
        return 0;
    }

    LOG_DBG("Handler rx\n");

    NETSTACK_RADIO.get_value(RADIO_PARAM_CHANNEL, &channel);
    process_post(&radio_sniffer_pr, radio_rx_ev, &channel);

    return rx_function();
}

/* Función de blinking de LED verde (dedicada a evento tx) */
static void tx_blink_timer_callback(void *ptr)
{
    estados_t* estado = (estados_t*)(ptr);

    do
    {
        if (timer_expired(&timer_general))
        {
        leds_off(LEDS_ALL);
            break;
        }else{
            ctimer_set(&blink_timer, SIMPLE_BLINK_TIMEOUT, tx_blink_timer_callback, estado);
        }

        leds_toggle(LEDS_GREEN);

    }while(0);
}

/* Función de blinking de LED rojo (dedicado a evento rx) */
static void rx_blink_timer_callback(void *ptr)
{
    estados_t* estado = (estados_t*)(ptr);

    do
    {
        if (timer_expired(&timer_general))
        {
            leds_off(LEDS_ALL);
            break;
        }else{
            ctimer_set(&blink_timer, SIMPLE_BLINK_TIMEOUT, rx_blink_timer_callback, estado);
        }

        leds_toggle(LEDS_RED);

    }while(0);
}
#endif // _CONFIG_VER_CANAL

/*---------------------------------------------------------------------------*/
/* Proceso Cliente UDP */
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
    #if CONTIKI_TARGET_COOJA || CONTIKI_TARGET_NATIVE
    uip_ip6addr(&empty_addr, 0xFD00, 0, 0, 0,0x200, 0, 0, 0);
    uip_ip6addr(&dest_ipaddr, 0xFD00, 0, 0, 0,0x200, 0, 0, 0);
    #else
    uip_ip6addr(&empty_addr, 0xFD00, 0, 0, 0,0x212, 0x4B00, 0xD2E, 0);
    uip_ip6addr(&dest_ipaddr, 0xFD00, 0, 0, 0,0x212, 0x4B00, 0xD2E, 0);
    #endif
    /* Registrar la conexion - UDP sin IP especificada */
    simple_udp_register(&udp_client, UDP_PORT, NULL, UDP_PORT, udp_rx_callback);

    /* Inicializar el Node ID */
    node_id_init();

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
            uint8_t targetNode = *(uint8_t*)data;
            LOG_INFO("Correctamente seleccionado: Nodo %d\n", targetNode);
            #if CONTIKI_TARGET_COOJA || CONTIKI_TARGET_NATIVE
            uip_ip6addr(&dest_ipaddr, 0xFD00, 0, 0, 0,0x200 + targetNode, targetNode, targetNode, targetNode);
            #else
            uip_ip6addr(&dest_ipaddr, 0xFD00, 0, 0, 0,0x212, 0x4B00, 0xD2E, targetNode);
            #endif
        }
    }
    PROCESS_END();
}
/*---------------------------------------------------------------------------*/
/* Proceso Seleccion de Nodo */
/*---------------------------------------------------------------------------*/

PROCESS_THREAD(node_select_process, ev, data)
{

    PROCESS_BEGIN();
    
    /* Variables */
    static struct etimer eTimerButton;
    static uint8_t buttonPressCount = 0;

    /* Registrar el evento */
    event_count_button = process_alloc_event();

    etimer_set(&eTimerButton, 2*CLOCK_SECOND);// <---- Definir el tiempo de espera para el boton
    
    while (1)
    {
        PROCESS_WAIT_EVENT();

        if(ev == button_hal_press_event){
            etimer_restart(&eTimerButton); //Was previously done with restart
            buttonPressCount++;
            buttonPressCount = (buttonPressCount % (NODE_AMOUNT + 1));
            buttonPressCount = (buttonPressCount == 0) ? 1 : buttonPressCount; 
            LOG_INFO("Seleccionado: Nodo %d\n", buttonPressCount);
        }
        if(etimer_expired(&eTimerButton) && buttonPressCount != 0){ //Si se presiono el boton y se espero el tiempo de espera, pues cada evento hace que entre al process
            auxPost = buttonPressCount;
            process_post(&udp_client_process,event_count_button, &auxPost);
            
            LOG_INFO("Objetivo actualizado a: Nodo %d\n", auxPost);
            buttonPressCount = 0;
        }
    }
    PROCESS_END();

}

PROCESS_THREAD(radio_sniffer_pr, ev, data)
{
    #ifndef _CONFIG_VER_CANAL
    static estados_t estado = OFF;
    #else
    static radio_value_t canal;
    #endif


    PROCESS_BEGIN();

    /* Alocar eventos. */
    radio_on_ev = process_alloc_event();
    radio_off_ev = process_alloc_event();
    radio_tx_ev = process_alloc_event();
    #ifndef _CONFIG_VER_CANAL
    radio_rx_ev = process_alloc_event();
    #endif

    /* Guardar funciones correspondientes al handler. */
    on_function = NETSTACK_RADIO.on;
    off_function = NETSTACK_RADIO.off;
    tx_function = NETSTACK_RADIO.transmit;
    #ifndef _CONFIG_VER_CANAL
    rx_function = NETSTACK_RADIO.receiving_packet;
    #endif

    /* Reasignar funciones colocando handlers definidos para el proyecto. */
    NETSTACK_RADIO.on = on_handler;
    NETSTACK_RADIO.off = off_handler;
    NETSTACK_RADIO.transmit = tx_handler;
    #ifndef _CONFIG_VER_CANAL
    NETSTACK_RADIO.receiving_packet = rx_handler;
    #endif

    leds_off(LEDS_ALL);

    while (1)
    {
        PROCESS_WAIT_EVENT();
        if (ev == radio_on_ev)
        {
            /* Handler para cuando radio se enciende. */
            #ifndef _CONFIG_VER_CANAL
            estado = ON;
            leds_on(LEDS_ALL);
            #else
            leds_on(LEDS_RED);
            #endif
        }

        if (ev == radio_off_ev)
        {
            /* Handler para cuando la radio se apaga. */
            #ifndef _CONFIG_VER_CANAL
            estado = OFF;
            leds_off(LEDS_ALL);
            #else
            leds_off(LEDS_RED);
            #endif
        }

        if (ev == radio_tx_ev)
        {
            /* Handler para cuando se transmite algo. */
            /* El canal por el cual se transmite está en (*data) */
            #ifndef _CONFIG_VER_CANAL
            leds_off(LEDS_GREEN);
            ctimer_set(&blink_timer, SIMPLE_BLINK_TIMEOUT, tx_blink_timer_callback, &estado);
            timer_set(&timer_general, BLINK_TIMEOUT);
            #else
            leds_on(LEDS_RED);
            NETSTACK_RADIO.get_value(RADIO_PARAM_CHANNEL, &canal);
            if (canal == CANAL_1)
            {
                leds_off(LEDS_GREEN);
            }else{
                leds_on(LEDS_GREEN);
            }
            #endif
        }

        #ifndef _CONFIG_VER_CANAL
        if (ev == radio_rx_ev)
        {
            /* Handler para cuando se recibe un paquete. */
            /* El canal por el cual se transmite está en (*data) */
            leds_off(LEDS_RED);
            ctimer_set(&blink_timer, SIMPLE_BLINK_TIMEOUT, rx_blink_timer_callback, &estado);
            timer_set(&timer_general, BLINK_TIMEOUT);
        }
        #endif
    }

    PROCESS_END();
}