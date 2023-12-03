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
#include <sys/ctimer.h>
#include <sys/timer.h>

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
#define LOG_MODULE "Server"
#define LOG_LEVEL LOG_LEVEL_INFO

typedef enum estados
{
    OFF,
    ON
} estados_t ;

/*---------------------------------------------------------------------------*/
/* Variables */
/*---------------------------------------------------------------------------*/
static struct simple_udp_connection udp_conn;

/*---------------------------------------------------------------------------*/
/* Declaracion Proceso/AutoStart */
/*---------------------------------------------------------------------------*/
PROCESS(server_pr, "Server multicast");
PROCESS(radio_sniffer_pr, "Radio sniffer");

AUTOSTART_PROCESSES(&server_pr, &radio_sniffer_pr);

/* Definición de eventos. */
static process_event_t radio_on_ev;
static process_event_t radio_off_ev;
static process_event_t radio_tx_ev;
#ifndef _CONFIG_VER_CANAL
static process_event_t radio_rx_ev;
#endif

static int (* on_function)(void);
static int (* off_function)(void);
static int (* tx_function)(unsigned short);
#ifndef _CONFIG_VER_CANAL
static int (* rx_function)(void);
#endif

#ifndef _CONFIG_VER_CANAL
static struct timer timer_general;
static struct ctimer blink_timer;
#endif

#ifndef STRETCH
#define STRETCH 8
#endif

#ifndef DEFAULT_RADIO_DIV
#define DEFAULT_RADIO_DIV 1
#endif

#ifndef _CONFIG_VER_CANAL
/* Período durante el cual los LEDs blinkean. */
static const uint32_t BLINK_TIMEOUT = (2*CLOCK_SECOND)/(DEFAULT_RADIO_DIV);

/* El LED prende y apaga cada 200 ms. */
static const uint32_t SIMPLE_BLINK_TIMEOUT = CLOCK_SECOND/5;
#endif

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

    /* Log mensaje recibido */
    LOG_INFO("Received message '%.*s' from ", datalen, (char *) data);
    LOG_INFO_6ADDR(sender_addr);
    LOG_INFO_("\n");

    /* Enviar mensaje de callback al nodo que le hablo */
    simple_udp_sendto(&udp_conn, strRx, strlen(strRx), sender_addr);
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

/* Función de manejo de recepción de radio. */
#ifndef _CONFIG_VER_CANAL
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
    LOG_DBG("TX timer callback\n");
    do
    {
        if (timer_expired(&timer_general))
        {
            LOG_DBG("Timer general expirado, estado: %d\n", *estado);
            if (*estado == OFF)
            {
                leds_off(LEDS_GREEN);
            }

            if (*estado == ON)
            {
                leds_on(LEDS_GREEN);
            }
            break;
        }else{
            ctimer_set(&blink_timer, SIMPLE_BLINK_TIMEOUT/2, tx_blink_timer_callback, estado);
        }

        leds_toggle(LEDS_GREEN);

    }while(0);
}

/* Función de blinking de LED rojo (dedicado a evento rx) */
static void rx_blink_timer_callback(void *ptr)
{
    estados_t* estado = (estados_t*)(ptr);
    LOG_DBG("RX timer callback. Rmnn: %ld\n", timer_remaining(&timer_general));
    do
    {
        if (timer_expired(&timer_general))
        {
            LOG_DBG("Timer general expirado, estado: %d\n", *estado);
            if (*estado == OFF)
            {
                leds_off(LEDS_RED);
            }

            if (*estado == ON)
            {
                leds_on(LEDS_RED);
            }
            ctimer_stop(&blink_timer);
            break;
        }else{
            ctimer_set(&blink_timer, SIMPLE_BLINK_TIMEOUT/2, rx_blink_timer_callback, estado);
        }
        leds_toggle(LEDS_RED);

    }while(0);
}
#endif // _CONFIG_VER_CANAL

/*---------------------------------------------------------------------------*/
/* Proceso Server UDP */
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(server_pr, event, data){

    PROCESS_BEGIN();

    /* Registrar la conexion - UDP sin IP especificada */
    simple_udp_register(&udp_conn, UDP_PORT, NULL, UDP_PORT, udp_rx_callback);

    /* Inicializar el Node ID */
    node_id_init();

    /* Defino al nodo como root de la red RPL */
    NETSTACK_ROUTING.root_start();

    while(1){
        PROCESS_WAIT_EVENT();
    }
    PROCESS_END();
}

/*---------------------------------------------------------------------------*/
/* Proceso de radio sniff */
/*---------------------------------------------------------------------------*/
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
    #endif // _CONFIG_VER_CANAL

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
            #ifndef _CONFIG_VER_CANAL
            leds_off(LEDS_GREEN);
            ctimer_set(&blink_timer, SIMPLE_BLINK_TIMEOUT/2, tx_blink_timer_callback, &estado);
            timer_set(&timer_general, BLINK_TIMEOUT/2);
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
            leds_off(LEDS_RED);
            timer_set(&timer_general, BLINK_TIMEOUT/2);
            ctimer_set(&blink_timer, SIMPLE_BLINK_TIMEOUT, rx_blink_timer_callback, &estado);
        }
        #endif
    }

    PROCESS_END();
}
