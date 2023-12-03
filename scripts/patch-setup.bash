#!/bin/bash

# El siguiente script se utiliza para parchear el código fuente de Contiki-NG y así dar soporte a las modificaciones del proyecto,
# además de actualizar el valor de extensión de tiempo.

echo "patch-setup.bash: Iniciando parcheado de Contiki-NG..."

##### Inputs #####
CONTIKI_PATH="$1"
if [[ -z "$CONTIKI_PATH" || ! -d "$CONTIKI_PATH" ]]; then
    echo "    * La ubicacion pasada de Contiki-NG no es valida, ejecute el script con un directorio valido"
    exit 1
fi
TSCH_STRETCH="$2"
if [[ -z "$TSCH_STRETCH" || $TSCH_STRETCH -eq 0 ]]; then
    echo "    * El estiramiento temporal no posee un valor valido, ejecute el script con un directorio valido"
    exit 1
fi
SCRIPT_PATH="$(dirname "$0")"
cd $SCRIPT_PATH/..
PROJECT_PATH="$(pwd)"

##### Paths #####
CLIENT_PATH="$PROJECT_PATH/src/client_button"
SERVER_PATH="$PROJECT_PATH/src/server"
TSCH_LOCAL_PATH="$PROJECT_PATH/patch/contiki-ng/os/net/mac/tsch/"
CC13XX_CC26XX_PATH="$PROJECT_PATH/patch/contiki-ng/arch/cpu/cc26x0-cc13x0"

##### Files #####
CLIENT_FILE="$CLIENT_PATH/*.c"
SERVER_FILE="$SERVER_PATH/*.c"
TSCH_TIMING_FILE="$TSCH_LOCAL_PATH/tsch-timeslot-timing.c"
CC13XX_CC26XX_TIMING_FILE="$CC13XX_CC26XX_PATH/rf-core/cc13xx-50kbps-tsch.c"

echo "    + Modificando STRETCH en archivos locales..."
sed -i "s|#define STRETCH [0-9]*\b|#define STRETCH $TSCH_STRETCH|g" $CLIENT_FILE
sed -i "s|#define STRETCH [0-9]*\b|#define STRETCH $TSCH_STRETCH|g" $SERVER_FILE
sed -i "s|#define STRETCH [0-9]*\b|#define STRETCH $TSCH_STRETCH|g" $TSCH_TIMING_FILE
sed -i "s|#define STRETCH [0-9]*\b|#define STRETCH $TSCH_STRETCH|g" $CC13XX_CC26XX_TIMING_FILE

echo "    + Aplicando patch al código fuente de Contiki-NG..."
cp -r "$PROJECT_PATH/patch/contiki-ng" $CONTIKI_PATH


# /*---------------------------------------------------------------------------*/
# /* Time-slot Cambios */
# /*---------------------------------------------------------------------------*/
# /*
#     Los cambios están destinados a ser utilizados en nodos "remote", siendo la plataforma "zoul".
#     Hay un valor predeterminado establecido presente en "os/net/mac/tsch/tsch-timeslot-timing.c" llamado "tsch_timeslot_timing_us_10000" que se utiliza como valor predeterminado 
#     para el tiempo TSCH antes de la adaptación a una plataforma específica.

#     Los tiempos varian de plataforma en plataforma, pero fisicamente se van a probar en CC1350 dev-kit,
#     debido a que es la plataforma disponible para pruebas (y un conjunto de valores de referencia no disponibles, discutidos a continuación).
#     Si miramos dentro de "contiki-conf.h" (arch/platform/cc26x0-cc13x0/contiki-conf.h), en su interior tiene #include "cc13xx-cc26xx-def.h", donde encontramos que el tiempo utilizado 
#     es el que está presente en "cc13xx-50kbps-tsch.c".
#     Sin embargo, el tipo de nodos utilizados DEPENDE de estos tiempos, en particular como se menciona en el artículo 
#     (Figura 6-30 en IEEE 802.15.4-2015 Standard.pdf).

#     Los tiempos para la plataforma Zoul son diferentes. Al mirar en "contiki-conf.h" (arch/platform/zoul/contiki-conf.h), que es el conf utilizado para revb o reva, utiliza "radio_tsch_timeslot_timing()", referenciado en "platform.c". 
#     Esto toma los tiempos de NETSTACK_RADIO pero no parece usar ningún conjunto de valores específicos, por lo que a su vez lo siguiente debería funcionar.
#     La idea es escalar por un factor cada vez, para ver cómo afecta al rendimiento de TSCH/RPL y permitir la creación de la red RPL.

#     Se realizaron varios cambios en los archivos de tsch de Contiki_NG, ya que los valores máximos de tiempo no eran suficientes para ser establecidos en uint16_t, por lo que se cambiaron a uint32_t. 
#     Los archivos modificados fueron:

#     + tsch.c -> líneas 96 y 99 respectivamente a
#       * static const uint32_t *tsch_default_timing_us;
#       * uint32_t tsch_timing_us[tsch_ts_elements_count];
#     + tsch-timeslot-timing.c -> varias líneas para implementar el factor STRETCH
#     + tsch-types.h -> línea 145
#       * typedef uint32_t tsch_timeslot_timing_usec[tsch_ts_elements_count];

#     + arch/cpu/cc26x0-cc13x0/rf-core/cc13xx-50kbps-tsch.h -> línea 39
#       * const uint32_t tsch_timing_cc13xx_50kbps[tsch_ts_elements_count];
#     + arch/cpu/cc26x0-cc13x0/rf-core/cc13xx-50kbps-tsch.c -> varias líneas para implementar el factor STRETCH

#     + arch/platform/zoul/contiki-conf.h -> línea 64
#       * uint32_t *radio_tsch_timeslot_timing(void);
#     + arch/platform/zoul/platform.c -> líneas 295 y 297
#       * uint32_t *radio_tsch_timeslot_timing(void)
        # *   uint32_t *
        #     radio_tsch_timeslot_timing(void) {
        #     uint32_t *ret;
        #     /* Get and return pointer to TSCH timings in usec */
        #     NETSTACK_RADIO.get_object(RADIO_CONST_TSCH_TIMING, &ret, sizeof(ret));
        #     return ret;
        #     }
# */
