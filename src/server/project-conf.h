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

/***********************Logging Level***********************/
#define LOG_CONF_LEVEL_6LOWPAN  LOG_LEVEL_NONE
#define LOG_CONF_LEVEL_TCPIP    LOG_LEVEL_NONE
#define LOG_CONF_LEVEL_IPV6     LOG_LEVEL_NONE
#define LOG_CONF_LEVEL_RPL      LOG_LEVEL_NONE
#define LOG_CONF_LEVEL_MAC      LOG_LEVEL_NONE
#define LOG_CONF_LEVEL_FRAMER   LOG_LEVEL_NONE

/***********************Transport-Layer Defines***********************/

#define UDP_PORT	5678
#define SEND_INTERVAL (10*CLOCK_SECOND)
#define WITH_SERVER_REPLY 1
#define USABLE_RADIO_CHANNEL 16

/***********************Parametros de Configuracion para RPL Network***********************/
/*#define QUEBUF_CONF_NUM 4
#define NBR_TABLE_CONF_MAX_NEIGHBORS 25
#define UIP_CONF_BUFFER_SIZE 240
#define NETSTACK_MAX_ROUTE_ENTRIES 30
#define SICSLOWPAN_CONF_FRAG 1*/

#endif // SERVER_CFG

