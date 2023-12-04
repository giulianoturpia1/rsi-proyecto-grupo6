/**
 * @file project-conf.h
 * @author Giuliano Turpia, Diego Fraga, Ignacio Valettute
 * @brief Esto es el encabezado de configuración para cambiar/establecer valores 
 * a nivel de preproceso en el nodo cliente.
 * @version 0.4
 * @date 09/11/2023
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef UDP_CLIENT_CFG
#define UDP_CLIENT_CFG

/*---------------------------------------------------------------------------*/
/* Defines Contiki-NG Logging Level */
/*---------------------------------------------------------------------------*/
#define LOG_CONF_LEVEL_6LOWPAN  LOG_LEVEL_NONE
#define LOG_CONF_LEVEL_TCPIP    LOG_LEVEL_NONE
#define LOG_CONF_LEVEL_IPV6     LOG_LEVEL_NONE
#define LOG_CONF_LEVEL_RPL      LOG_LEVEL_NONE
#define LOG_CONF_LEVEL_MAC      LOG_LEVEL_NONE
#define LOG_CONF_LEVEL_FRAMER   LOG_LEVEL_NONE

/*---------------------------------------------------------------------------*/
/* Defines Capa de Transporte */
/*---------------------------------------------------------------------------*/
#define UDP_PORT	5678
#define SEND_INTERVAL (5*CLOCK_SECOND)
#define WITH_SERVER_REPLY 1
#define NODE_AMOUNT 16 //Los nodos en realiad van de [1, NODE_AMOUNT]

/*---------------------------------------------------------------------------*/
/* Defines específicos del proyecto. */
/*---------------------------------------------------------------------------*/
#define CONFIG_VER_CANAL
#define CANAL_0 15
#define CANAL_1 25

#ifndef TSCH_CONF_DEFAULT_HOPPING_SEQUENCE
#define TSCH_CONF_DEFAULT_HOPPING_SEQUENCE (uint8_t[]){ 15, 25 }
#endif

#ifndef TSCH_CONF_EB_PERIOD
#define TSCH_CONF_EB_PERIOD (2*SEND_INTERVAL)
#endif

#ifndef TSCH_CONF_MAX_EB_PERIOD
#define TSCH_CONF_MAX_EB_PERIOD (2*SEND_INTERVAL)
#endif

// Increase the timeouts of the network

#ifndef TSCH_CONF_KEEPALIVE_TIMEOUT
#define TSCH_CONF_KEEPALIVE_TIMEOUT (60*CLOCK_SECOND)
#endif

#ifndef TSCH_CONF_MAX_KEEPALIVE_TIMEOUT
#define TSCH_CONF_MAX_KEEPALIVE_TIMEOUT (120*CLOCK_SECOND)
#endif

#ifndef TSCH_CONF_DESYNC_THRESHOLD
#define TSCH_CONF_DESYNC_THRESHOLD (2*TSCH_CONF_MAX_KEEPALIVE_TIMEOUT)
#endif

#endif //UDP_CLIENT_CFG