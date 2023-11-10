/**
 * @file project-conf.h
 * @author Giuliano Turpia, Diego Fraga, Ignacio Valettute
 * @brief This is the config header for changing/setting values at pre
 * process level in the server node
 * @version 0.2
 * @date 09/11/2023
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef SERVER_CFG
#define SERVER_CFG

/***********************Contiki-NG related includes to allow TSCH timing manipulation***********************/
#include "contiki.h"
#include "net/mac/tsch/tsch.h"

/***********************Logging Level***********************/
#define LOG_CONF_LEVEL_6LOWPAN  LOG_LEVEL_NONE
#define LOG_CONF_LEVEL_TCPIP    LOG_LEVEL_NONE
#define LOG_CONF_LEVEL_IPV6     LOG_LEVEL_NONE
#define LOG_CONF_LEVEL_RPL      LOG_LEVEL_NONE
#define LOG_CONF_LEVEL_MAC      LOG_LEVEL_NONE
#define LOG_CONF_LEVEL_FRAMER   LOG_LEVEL_NONE

/***********************Transport-Layer Defines***********************/

#define UDP_PORT	5678
#define SEND_INTERVAL (3*CLOCK_SECOND)
#define WITH_SERVER_REPLY 1
#define USABLE_RADIO_CHANNEL 16

/***********************Configuration Parameters for RPL Network***********************/
#define QUEBUF_CONF_NUM 4
#define NBR_TABLE_CONF_MAX_NEIGHBORS 25
#define UIP_CONF_BUFFER_SIZE 240
#define NETSTACK_MAX_ROUTE_ENTRIES 30
#define SICSLOWPAN_CONF_FRAG 1

/***********************Time-slot Timing Changes************************/
/*
    The changes are meant to be used in remote type of nodes, the platform being zoul.
    There is a default value set present in os/net/mac/tsch/tsch-timeslot-timing.c called
    tsch_timeslot_timing_us_10000 which is used as a default value for the TSCH timing before porting to a specific platform.
    The following timeslot timings are based off of the cc26x0-cc13x0 cpu family and the remote family(in particular remote-revb)

    The first timings are for the platform CC1350 dev-kit, doing so because it is the platform available for testing(and a reference set of values not available, discused next).
    If we look inside the "contiki-conf.h"(arch/platform/cc26x0-cc13x0/contiki-conf.h), inside has #include "cc13xx-cc26xx-def.h", 
    where we found that the timing used is the one present in "cc13xx-50kbps-tsch.c".
    However, the type of nodes used DEPEND on these timings, in particular as mentioned in the paper(Figure 6-30 in IEEE 802.15.4-2015 Standard.pdf).

    On the other hand, the second timings are for the platform Zoul. By looking in the "contiki-conf.h"(arch/platform/zoul/contiki-conf.h) that
    is the conf used for either revb or reva, it uses "radio_tsch_timeslot_timing()", referenced in "platform.c". This takes the timings
    from the NETSTACK_RADIO but it does not seem to use any set of specific values, so in turn the following should work.

    The idea is to scale by a factor each time, to see how it affects TSCH/RPL performance and to allow the
    creation of the RPL network.
*/

#define SCALING_FACTOR 100

#define LED_NET_TSCH_DEFAULT_TS_CCA_OFFSET         1800*SCALING_FACTOR
#define LED_NET_TSCH_DEFAULT_TS_CCA                128*SCALING_FACTOR
#define LED_NET_TSCH_DEFAULT_TS_TX_OFFSET          3800*SCALING_FACTOR
#define LED_NET_TSCH_DEFAULT_TS_RX_OFFSET          (LED_NET_TSCH_DEFAULT_TS_TX_OFFSET - (TSCH_CONF_RX_WAIT / 2))*SCALING_FACTOR
#define LED_NET_TSCH_DEFAULT_TS_RX_ACK_DELAY       2000*SCALING_FACTOR
#define LED_NET_TSCH_DEFAULT_TS_TX_ACK_DELAY       3000*SCALING_FACTOR
#define LED_NET_TSCH_DEFAULT_TS_RX_WAIT            TSCH_CONF_RX_WAIT*SCALING_FACTOR
#define LED_NET_TSCH_DEFAULT_TS_ACK_WAIT           3000*SCALING_FACTOR
#define LED_NET_TSCH_DEFAULT_TS_RX_TX              192*SCALING_FACTOR
#define LED_NET_TSCH_DEFAULT_TS_MAX_ACK            10000*SCALING_FACTOR
#define LED_NET_TSCH_DEFAULT_TS_MAX_TX             21600*SCALING_FACTOR

/*Timeslot length*/
#define LED_NET_TSCH_DEFAULT_TS_TIMESLOT_LENGTH  40000*SCALING_FACTOR

/*TSCH timeslot timing (microseconds to seconds depending from the factor)*/
const tsch_timeslot_timing_usec tsch_timing_led_net = {
  LED_NET_TSCH_DEFAULT_TS_CCA_OFFSET,
  LED_NET_TSCH_DEFAULT_TS_CCA,
  LED_NET_TSCH_DEFAULT_TS_TX_OFFSET,
  LED_NET_TSCH_DEFAULT_TS_RX_OFFSET,
  LED_NET_TSCH_DEFAULT_TS_RX_ACK_DELAY,
  LED_NET_TSCH_DEFAULT_TS_TX_ACK_DELAY,
  LED_NET_TSCH_DEFAULT_TS_RX_WAIT,
  LED_NET_TSCH_DEFAULT_TS_ACK_WAIT,
  LED_NET_TSCH_DEFAULT_TS_RX_TX,
  LED_NET_TSCH_DEFAULT_TS_MAX_ACK,
  LED_NET_TSCH_DEFAULT_TS_MAX_TX,
  LED_NET_TSCH_DEFAULT_TS_TIMESLOT_LENGTH,
};

/*Define as extern as the modules can use it*/
extern const uint16_t tsch_timing_led_net[];

/*TSCH timeslot made active*/
#ifndef TSCH_CONF_DEFAULT_TIMESLOT_TIMING
#define TSCH_CONF_DEFAULT_TIMESLOT_TIMING tsch_timing_led_net
#endif
#endif // CONF_TAREA_1

