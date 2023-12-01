/**
 * @file project-conf.h
 * @author Giuliano Turpia, Diego Fraga, Ignacio Valettute
 * @brief Esto es el encabezado de configuración para cambiar/establecer valores 
 * a nivel de preproceso en el nodo server.
 * @version 0.3
 * @date 09/11/2023
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef SERVER_CFG
#define SERVER_CFG

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
#define SEND_INTERVAL (10*CLOCK_SECOND)
#define WITH_SERVER_REPLY 1
#define USABLE_RADIO_CHANNEL 16

#endif // SERVER_CFG

