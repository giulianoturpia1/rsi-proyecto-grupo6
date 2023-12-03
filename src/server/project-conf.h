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
#define SEND_INTERVAL (5*CLOCK_SECOND)
#define WITH_SERVER_REPLY 1

/*---------------------------------------------------------------------------*/
/* Defines específicos del proyecto. */
/*---------------------------------------------------------------------------*/
#define CONFIG_VER_CANAL
#define CANAL_0 16
#define CANAL_1 17

#ifndef TSCH_CONF_DEFAULT_HOPPING_SEQUENCE
#define TSCH_CONF_DEFAULT_HOPPING_SEQUENCE (uint8_t[]){ 16, 17 }
#endif

#ifndef TSCH_CONF_EB_PERIOD
#define TSCH_CONF_EB_PERIOD 2*SEND_INTERVAL
#endif

#ifndef TSCH_CONF_MAX_EB_PERIOD
#define TSCH_CONF_MAX_EB_PERIOD 2*SEND_INTERVAL
#endif

#endif // SERVER_CFG

