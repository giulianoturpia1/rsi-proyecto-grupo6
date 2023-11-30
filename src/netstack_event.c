/**
* @file Este archivo define los eventos y procesos necesarios para
*       manejar correctamente el encendido, apagado, transmición y
*       recepción de la radio del sistema. Para esto define hanclers
*       y modifica el radio_driver para hacer de sniffer.
* @author Diego Fraga, Giuliano Turpia, Ignacio Valetutte.
*/
#include <contiki.h>
#include <sys/log.h>
#include <sys/ctimer.h>
#include <dev/leds.h>
#include <stdio.h>
#include <net/netstack.h>

/* Definición de procesos. */
PROCESS(radio_sniffer_pr, "Radio on off");

/* Definición de eventos. */
static process_event_t radio_on_ev;
static process_event_t radio_off_ev;
static process_event_t radio_tx_ev;
static process_event_t radio_rx_ev;

AUTOSTART_PROCESSES(&radio_sniffer_pr);

#define LOG_MODULE "Radio_sniffer"
#define LOG_LEVEL LOG_LEVEL_DBG

#ifndef ENL_FACTOR
#define ENL_FACTOR 100
#endif

#ifndef DEFAULT_RADIO_DIV
#define DEFAULT_RADIO_DIV 100
#endif

static const uint32_t BLINK_TIMEOUT = ENL_FACTOR*(CLOCK_SECOND)/(DEFAULT_RADIO_DIV);

static int (* on_function)(void);
static int (* off_function)(void);
static int (* tx_function)(unsigned short);
static int (* rx_function)(void);

typedef enum estados
{
    OFF,
    ON
} estados_t ;

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

static void blink_timer_callback(void *ptr)
{
    estados_t estado = *(estados_t*)(ptr);
    do
    {
        /* Dependiendo del estado de los LEDs, vuelve al estado original. */
        if (estado == OFF)
        {
            leds_off(LEDS_ALL);
            break;
        }
        if (estado == ON)
        {
            leds_on(LEDS_ALL);
            break;
        }
        estado = OFF;
    } while (0);

}

PROCESS_THREAD(radio_sniffer_pr, ev, data)
{
    static struct ctimer blink_timer;
    static estados_t estado = OFF;

    PROCESS_BEGIN();

    /* Alocar eventos. */
    radio_on_ev = process_alloc_event();
    radio_off_ev = process_alloc_event();
    radio_tx_ev = process_alloc_event();
    radio_rx_ev = process_alloc_event();

    /* Guardar funciones correspondientes al handler. */
    on_function = NETSTACK_RADIO.on;
    off_function = NETSTACK_RADIO.off;
    tx_function = NETSTACK_RADIO.transmit;
    rx_function = NETSTACK_RADIO.receiving_packet;

    /* Reasignar funciones colocando handlers definidos para el proyecto. */
    NETSTACK_RADIO.on = on_handler;
    NETSTACK_RADIO.off = off_handler;
    NETSTACK_RADIO.transmit = tx_handler;
    NETSTACK_RADIO.receiving_packet = rx_handler;

    while (1)
    {
        PROCESS_WAIT_EVENT();
        if (ev == radio_on_ev)
        {
            /* Handler para cuando radio se enciende. */
            estado = ON;
            leds_on(LEDS_ALL);

        }

        if (ev == radio_off_ev)
        {
            /* Handler para cuando la radio se apaga. */
            estado = OFF;
            leds_off(LEDS_ALL);
        }

        if (ev == radio_tx_ev)
        {
            /* Handler para cuando se transmite algo. */
            /* El canal por el cual se transmite está en (*data) */
            leds_off(LEDS_GREEN);
            ctimer_set(&blink_timer, BLINK_TIMEOUT, blink_timer_callback, &estado);
        }

        if (ev == radio_rx_ev)
        {
            /* Handler para cuando se recibe un paquete. */
            /* El canal por el cual se transmite está en (*data) */
            leds_off(LEDS_RED);
            ctimer_set(&blink_timer, BLINK_TIMEOUT, blink_timer_callback, &estado);
        }
    }

    PROCESS_END();
}