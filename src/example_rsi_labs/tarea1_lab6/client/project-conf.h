/**
 * @file project-conf.h
 * @author Giuliano Turpia, Diego Fraga, Ignacio Valettute
 * @brief This is the config header for changing/setting values at pre
 * process level
 * @version 0.1
 * @date 2023-08-29
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef UDP_CLIENT_CFG
#define UDP_CLIENT_CFG

//Logging Level
#define LOG_CONF_LEVEL_6LOWPAN  LOG_LEVEL_DBG
#define LOG_CONF_LEVEL_TCPIP    LOG_LEVEL_NONE
#define LOG_CONF_LEVEL_IPV6     LOG_LEVEL_NONE
#define LOG_CONF_LEVEL_RPL      LOG_LEVEL_NONE
#define LOG_CONF_LEVEL_MAC      LOG_LEVEL_NONE
#define LOG_CONF_LEVEL_FRAMER   LOG_LEVEL_NONE

/*Transport-Layer Defines*/

#define UDP_PORT	5678
#define SEND_INTERVAL (3*CLOCK_SECOND)
#define WITH_SERVER_REPLY 1
#define USABLE_RADIO_CHANNEL 16

//Configuration Parameters
#define QUEBUF_CONF_NUM 4
#define NBR_TABLE_CONF_MAX_NEIGHBORS 25
#define UIP_CONF_BUFFER_SIZE 240
#define NETSTACK_MAX_ROUTE_ENTRIES 30
#define SICSLOWPAN_CONF_FRAG 1


#define PAYLOAD "In the heart of the jungle, Hugo, a hippo, felt the weight of solitude, tears streaming down his cheeks under the silver moon's gentle gaze."

#endif //UDP_CLIENT_CFG