/* Contiki inclusions */
#include <contiki.h>
#include <sys/log.h>
#include <net/netstack.h>
#include <net/ipv6/simple-udp.h>
#include <dev/leds.h>
#include <sys/node-id.h>
#include "net/routing/routing.h"

/* C std inclusions */
#include <stdint.h>
#include <inttypes.h>

/* App particular inclusions */
#include "project-conf.h"

#define LOG_MODULE "Server"
#define LOG_LEVEL LOG_LEVEL_INFO

static struct simple_udp_connection udp_conn;

/* Definition of reception callback function. */
static void udp_rx_callback(struct simple_udp_connection *c,
         const uip_ipaddr_t *sender_addr,
         uint16_t sender_port,
         const uip_ipaddr_t *receiver_addr,
         uint16_t receiver_port,
         const uint8_t *data,
         uint16_t datalen)
{   /* Variables */
    char strRx[36];

    /* Print callback message into a string. */
    snprintf(strRx, sizeof(strRx), "%d bytes received correctly.", datalen);

    /* Toggle leds. */
    leds_toggle(LEDS_RED);

    /* Log received message. */
    LOG_INFO("Received message '%.*s' from ", datalen, (char *) data);
    LOG_INFO_6ADDR(sender_addr);
    #if LLSEC802154_CONF_ENABLED
        LOG_INFO_(" LLSEC LV:%d", uipbuf_get_attr(UIPBUF_ATTR_LLSEC_LEVEL));
    #endif
    LOG_INFO_("\n");

    /* Callback message send. */
    simple_udp_sendto(&udp_conn, strRx, strlen(strRx), sender_addr);
}

PROCESS(server_pr, "Server multicast");
AUTOSTART_PROCESSES(&server_pr);

PROCESS_THREAD(server_pr, event, data){
    static char strTx[36];
    static radio_value_t ch_num;
    uip_ipaddr_t dest_ip_addr;

    PROCESS_BEGIN();

    /* Initialize connection */
    simple_udp_register(&udp_conn, UDP_PORT, NULL, UDP_PORT, udp_rx_callback);

    /* Initialize Node Id */
    node_id_init();

    NETSTACK_RADIO.set_value(RADIO_PARAM_CHANNEL, USABLE_RADIO_CHANNEL);
    NETSTACK_RADIO.get_value(RADIO_PARAM_CHANNEL, &ch_num);
    LOG_INFO("Rf channel: %d\n", ch_num);

    /* Defino al nodo como root de la red rpl */
    NETSTACK_ROUTING.root_start();

    /* Build strings */
    snprintf(strTx, sizeof(strTx), "Servidor del grupo 6: %x", node_id);

    /* Generate address */
    uip_create_linklocal_allnodes_mcast(&dest_ip_addr);

    /* Log multicast IP */
    LOG_INFO("Sending multicast (IP: ");
    LOG_INFO_6ADDR(&dest_ip_addr);
    LOG_INFO(")\n");

    /* Send the message */
    simple_udp_sendto(&udp_conn, strTx, strlen(strTx), &dest_ip_addr);

    /* Toggle leds in order to indicate a sent message. */
    leds_toggle(LEDS_GREEN);

    while(1){
        PROCESS_WAIT_EVENT();
    }
    PROCESS_END();
}