/**
 * @file project-conf.h
 * @author Giuliano Turpia, Diego Fraga, Ignacio Valettute
 * @brief Esto es el encabezado de configuración para cambiar/establecer valores 
 * a nivel de preproceso en el nodo cliente.
 * @version 0.3
 * @date 09/11/2023
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef UDP_CLIENT_CFG
#define UDP_CLIENT_CFG

/***********************Logging Level***********************/
#define LOG_CONF_LEVEL_6LOWPAN  LOG_LEVEL_NONE
#define LOG_CONF_LEVEL_TCPIP    LOG_LEVEL_NONE
#define LOG_CONF_LEVEL_IPV6     LOG_LEVEL_NONE
#define LOG_CONF_LEVEL_RPL      LOG_LEVEL_NONE
#define LOG_CONF_LEVEL_MAC      LOG_LEVEL_NONE
#define LOG_CONF_LEVEL_FRAMER   LOG_LEVEL_NONE

/***********************Transport-Layer Defines***********************/

#define UDP_PORT	5678
#define SEND_INTERVAL (4*CLOCK_SECOND)
#define WITH_SERVER_REPLY 1
#define USABLE_RADIO_CHANNEL 16

/***********************Parametros de Configuracion para RPL Network***********************/
/*#define QUEBUF_CONF_NUM 4
#define NBR_TABLE_CONF_MAX_NEIGHBORS 25
#define UIP_CONF_BUFFER_SIZE 240
#define NETSTACK_MAX_ROUTE_ENTRIES 30
#define SICSLOWPAN_CONF_FRAG 1*/
#define NODE_AMOUNT 16 //Note that the Actual Node Amount ranges from [1, NODE_AMOUNT]

#endif //UDP_CLIENT_CFG