#!/bin/bash

# This script is used to patch the Contiki-NG source code to support the project modifications

# Inputs
CONTIKI_PATH="$1"
if [[ -z "$CONTIKI_PATH" || ! -d "$CONTIKI_PATH" ]]; then
    echo "Contiki-NG path is not a valid value, retry the script with a valid directory"
    exit 1
fi
TSCH_STRETCH="$2"
if [[ -z "$TSCH_STRETCH" || $TSCH_STRETCH -eq 0 ]]; then
    echo "Time stretch is not a valid value, retry the script with a valid value"
    exit 1
fi
SCRIPT_PATH="$(dirname "$0")"
cd $SCRIPT_PATH/..
PROJECT_PATH="$(pwd)"

# Paths
CLIENT_PATH="$PROJECT_PATH/src/client_button"
SERVER_PATH="$PROJECT_PATH/src/server"
TSCH_LOCAL_PATH="$PROJECT_PATH/patch/contiki-ng/os/net/mac/tsch/"
CC13XX_CC26XX_PATH="$PROJECT_PATH/patch/contiki-ng/arch/cpu/cc26x0-cc13x0"
# Files
CLIENT_FILE="$CLIENT_PATH/*.c"
SERVER_FILE="$SERVER_PATH/*.c"
TSCH_TIMING_FILE="$TSCH_LOCAL_PATH/tsch-timeslot-timing.c"
CC13XX_CC26XX_TIMING_FILE="$CC13XX_CC26XX_PATH/rf-core/cc13xx-50kbps-tsch.c"

echo "Patching Contiki-NG Source code, updating stretch values..."

# Patching Local Files by updating the STRETCH value
sed -i "s/#define STRETCH [0-9]*\b/#define STRETCH $TSCH_STRETCH/g" $CLIENT_FILE
sed -i "s/#define STRETCH [0-9]*\b/#define STRETCH $TSCH_STRETCH/g" $SERVER_FILE
sed -i "s/#define STRETCH [0-9]*\b/#define STRETCH $TSCH_STRETCH/g" $TSCH_TIMING_FILE
sed -i "s/#define STRETCH [0-9]*\b/#define STRETCH $TSCH_STRETCH/g" $CC13XX_CC26XX_TIMING_FILE


echo "Copying files to Contiki-NG source code..."

cp -r "$PROJECT_PATH/patch/contiki-ng" $CONTIKI_PATH 