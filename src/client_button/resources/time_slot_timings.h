/**
 * @file time_slot_timings.h
 * @author Giuliano Turpia, Diego Fraga, Ignacio Valettute
 * @brief Archivo utilizado para guardar los timings asociados a TSCH
 * @version 0.2
 * @date 09/11/2023
 * 
 * @copyright Copyright (c) 2023
 * 
 */

/*---------------------------------------------------------------------------*/
/* Contiki-NG Includes Especificos - Manipular TSCH */
/*---------------------------------------------------------------------------*/
#include "contiki.h"
#include "net/mac/tsch/tsch.h"
#include "net/mac/tsch/tsch-types.h"

/*---------------------------------------------------------------------------*/
/* Time-slot Cambios */
/*---------------------------------------------------------------------------*/
/*
    Los cambios están destinados a ser utilizados en nodos "remote", siendo la plataforma "zoul".
    Hay un valor predeterminado establecido presente en "os/net/mac/tsch/tsch-timeslot-timing.c" llamado "tsch_timeslot_timing_us_10000" que se utiliza como valor predeterminado 
    para el tiempo TSCH antes de la adaptación a una plataforma específica.
    Los siguientes tiempos de ranura se basan en la familia de CPU cc26x0-cc13x0 y la familia remote (en particular remote-revb).

    Los tiempos son para la plataforma CC1350 dev-kit, debido a que es la plataforma disponible para pruebas (y un conjunto de valores de referencia no disponibles, discutidos a continuación).
    Si miramos dentro de "contiki-conf.h" (arch/platform/cc26x0-cc13x0/contiki-conf.h), en su interior tiene #include "cc13xx-cc26xx-def.h", donde encontramos que el tiempo utilizado 
    es el que está presente en "cc13xx-50kbps-tsch.c".
    Sin embargo, el tipo de nodos utilizados DEPENDE de estos tiempos, en particular como se menciona en el artículo 
    (Figura 6-30 en IEEE 802.15.4-2015 Standard.pdf).

    Por otro lado, los segundos tiempos son para la plataforma Zoul. Al mirar en "contiki-conf.h" (arch/platform/zoul/contiki-conf.h), que es el conf utilizado para revb o reva, utiliza "radio_tsch_timeslot_timing()", referenciado en "platform.c". 
    Esto toma los tiempos de NETSTACK_RADIO pero no parece usar ningún conjunto de valores específicos, por lo que a su vez lo siguiente debería funcionar.
    La idea es escalar por un factor cada vez, para ver cómo afecta al rendimiento de TSCH/RPL y permitir la creación de la red RPL.

    Se realizaron varios cambios en los archivos de tsch de Contiki_NG, ya que los valores máximos de tiempo no eran suficientes para ser establecidos en uint16_t, por lo que se cambiaron a uint32_t. Los archivos modificados fueron:

    + tsch.c -> líneas 96 y 99 respectivamente a
      * static const uint32_t *tsch_default_timing_us;
      * uint32_t tsch_timing_us[tsch_ts_elements_count];

    + tsch-timeslot-timing.c -> varias líneas para implementar el factor STRETCH

    + tsch-types.h -> línea 145 a
      * typedef uint32_t tsch_timeslot_timing_usec[tsch_ts_elements_count];

    Para las plataformas en particular se debe de acceder como se menciona arriba y modificar los valores de uint16_t:
    + arch/cpu/cc26x0-cc13x0/rf-core/cc13xx-50kbps-tsch.h -> línea 39 a
      * const uint32_t tsch_timing_cc13xx_50kbps[tsch_ts_elements_count];
*/

#define SCALING_FACTOR 10

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


/*---------------------------------------------------------------------------*/
/* Time-slot Cambios(microsegundos a segundos dependiendo del factor) */
/*---------------------------------------------------------------------------*/
#define LED_NET_TSCH_DEFAULT_TS_TIMESLOT_LENGTH  40000*SCALING_FACTOR

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

/*Definir como extern pues Contiki-NG lo precisa*/
extern const uint32_t tsch_timing_led_net[];