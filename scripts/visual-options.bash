#!/bin/bash

# El siguiente script se utiliza para parchear el código local y determinar las opciones
# de visualización de CANAL y TX/RX.

echo "visual-options.bash: Iniciando parcheado de opciones de visualizacion..."


##### Inputs #####
VIEW_CHANNEL="$1"
if [[ -z "$VIEW_CHANNEL" || ! "$VIEW_CHANNEL" =~ ^(VIEW_CHANNEL|VIEW_TX_RX)$ ]]; then
    echo "    * La opcion pasada al script no posee un valor valido"
    echo "    * Los valores posibles son: VIEW_CHANNEL, VIEW_TX_RX"
    exit 1
fi
SCRIPT_PATH="$(dirname "$0")"
cd $SCRIPT_PATH/..
PROJECT_PATH="$(pwd)"

##### Patching #####
echo "    + Modificando codigo para visualizar CANAL o TX/RX..."
# Parcheando archivos locales al actualizar las opciones de visualización de CANAL y TX/RX
if [ "$VIEW_CHANNEL" == "VIEW_CHANNEL"  ]; then
    project_conf_files=$(find "$PROJECT_PATH/src" -name "project-conf.h")
    for file in $project_conf_files; do
        sed -i "s|.*#define CONFIG_VER_CANAL\b|#define CONFIG_VER_CANAL|g" $file
    done
else # Es VIEW_TX_RX
    project_conf_files=$(find "$PROJECT_PATH/src" -name "project-conf.h")
    for file in $project_conf_files; do
        sed -i "s|.*#define CONFIG_VER_CANAL\b|//#define CONFIG_VER_CANAL|g" $file
    done
fi