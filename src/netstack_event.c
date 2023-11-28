
#include <contiki.h>
#include <sys/log.h>
#include <os/lib/sensors.h>
// #include <batmon-sensor.h>
#include <dev/leds.h>
#include <stdio.h>
#include "net/netstack.h"

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

static int (* on_function)(void);
static int (* off_function)(void);
static int (* tx_function)(unsigned short);
static int (* rx_function)(void);

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

PROCESS_THREAD(radio_sniffer_pr, ev, data){

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
        }

        if (ev == radio_off_ev)
        {
            /* Handler para cuando la radio se apaga. */
        }

        if (ev == radio_tx_ev)
        {
            /* Handler para cuando se transmite algo. */
            /* El canal por el cual se transmite está en (*data) */
        }

        if (ev == radio_rx_ev)
        {
            /* Handler para cuando se recibe un paquete. */
            /* El canal por el cual se transmite está en (*data) */
        }
    }

    PROCESS_END();
}