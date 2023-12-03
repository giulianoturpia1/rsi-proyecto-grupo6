# Visualización del Comportamiento Dinámico de una Red TSCH/RPL a través de LEDs

## Proyecto de Redes de Sensores Inalámbricos - 2023

### Tabla de Contenidos

- [1. Resumen del Proyecto](#1-resumen-del-proyecto)
  * [1.1. Introducción](#11-introducción)
  * [1.2. Motivación](#12-motivación)
- [2. Instalación del Proyecto](#2-instalación-del-proyecto)
  * [2.1. Instalación del Repositorio](#21-instalación-del-repositorio)
  * [2.2. Instalación de Dependencias](#22-instalación-de-dependencias)
- [3. Configuración y Carga del Proyecto](#3-configuración-y-carga-del-proyecto)
  * [3.1. Configuración](#31-configuración)
    + [3.1.1. Configuración de Visualización](#311-configuración-de-visualización)
      - [3.1.1.1. Visualización Tx y Rx](#3111-visualización-tx-y-rx)
      - [3.1.1.2. Visualización de Canal](#3112-visualización-de-canal)
    + [3.1.2. Configuración en Tiempo de Ejecución](#312-configuración-en-tiempo-de-ejecución)
  * [3.2. Cargado en los Nodos](#32-cargado-en-los-nodos)
      - [3.2.1. Nodo Cliente](#321-nodo-cliente)
      - [3.2.2. Nodo Servidor](#322-nodo-servidor)
  * [4. Ejemplos](#4-ejemplos)

# 1. Resumen del Proyecto

## 1.1. Introducción

La idea principal del proyecto es visualizar de manera interactiva, utilizando LEDs, una red de sensores inalámbricos que utilizan TSCH/RPL. El objetivo es observar el funcionamiento de la red en diferentes etapas de su vida (creación de red, intercambio de mensajes, direccionamiento, entre otros). Los resultados podrían ser utilizados en un entorno interactivo para promover una mejor comprensión de conceptos, o en futuras iteraciones del curso RSI.

## 1.2. Motivación

En una red RPL que utiliza TSCH, existen ciertas etapas durante su vida, como el descubrimiento del DODAG, intercambio controlado de mensajes pasando por un nodo raíz, transmisión de mensajes basada en intervalos de tiempo (TSCH), entre otros. Estos eventos, aunque pueden durar un tiempo, hay eventos con tiempos asociados muy cortos, como por ejemplo la transmisión/recepción de mensajes. Hay varias opciones para un red red TSCH, pudiendo ser con Orchestra, o el camino de un mensaje UDP a través del DODAG y cómo se anuncian diferentes mensajes en una red (EB, DIO, DAO).

El objetivo, entonces, es tener un sistema que permita visualizar el funcionamiento de una red de este tipo utilizando LEDs.

# 2. Instalación del Proyecto

## 2.1. Instalación del Repositorio

Ejecuta los siguientes comandos, asegurándote de haber instalado previamente Contiki-NG (a través de Docker o de manera nativa) y configurado correctamente las credenciales de GitHub:

```
cd ~/contiki-ng && git clone git@github.com:giulianoturpia1/rsi-proyecto-grupo6.git
```

La versión más reciente de Contiki-NG utilizada es del dia 30/11/2023 y corresponde con el commit Hash `3754d008846b2e0798f606e71714b298fb9a7eb9` del repositorio de Contiki-NG.

## 2.2. Instalación de Dependencias

Para funcionar correctamente los siguientes proyectos en caso de necesitar re-compilar, se deben de reemplazar ciertas secciones de código del sistema operativo de Contiki-NG. En particular, dentro de la carpeta `scripts` del repositorio se encuentra un pequeño script ejecutable que permite actualizar todo lo necesario. Esencialmente el script copia los contenidos de la carpeta `patch` dentro de la ubicación de Contiki-NG (por lo general en `~/contiki-ng`) y se realizan ciertas actualizaciones para correr de manera correcta los programas.

El script `patch-setup.bash` toma dos parámetros los cual deben de pasarse correctamente para realizar el parche y poder luego generar los proyectos:

+ `CONTIKI_PATH`: Es en donde se encuentra la carpeta de `contiki-ng` y donde se va a aplicar el parche. Si la ubicación del repositorio es `/home/user/contiki-ng`, `CONTIKI_PATH`=/home/user a modo de ejemplificar.

+ `TSCH_STRETCH`: Es el factor de enlentecimiento de los *timeslots* de la red, factor que es aplicado a cada tiempo asociado al *timeslot*. El factor debe de ser mayor o igual a 1 siempre. Se recomienda utilizar un factor dentro de [10-15], pues timeslots mayores no permiten que la red se arme correctamente. 

Una posible ejecución de este script es la siguiente(asumiendo donde se encuentra `contiki-ng`). Para este ejemplo tomaremos que la ubicación es `/home/user/contiki-ng`

```
cd /home/$USER/contiki-ng/rsi-proyecto-grupo6/scripts && ./patch-setup.bash /home/$USER 10
```

Luego de ejecutado el script, el parche debería de correctamente actualizar los archivos con el fin de poder correr el proyecto del repositorio.

# 3. Configuración y Carga del Proyecto

En esencia se deben de realizar ciertas etapas previas para poder compilar y cargar en los nodos los archivos del proyecto. En particular, se debe de realizar una configuración del comportamiento visual del sistema, y otra sobre las características de la red TSCH/RPL, como el modo de guardado de tabla de ruteo en RPL o *Orchestra* en TSCH.

## 3.1. Configuración

### 3.1.1. Configuración de Visualización

El proyecto permite alternar entre modo de visualización de transmisión/recepción (Tx/Rx) en la red TSCH y el canal actual de la red TSCH. Esto puede ser seteado desde el `project-conf.h` de cada nodo. Existe un script en el directorio `/scripts` del mismo repositorio el cual actualiza estas opciones de visualización, tomando un argumento:

+ `VIEW_CHANNEL`: Toma como valores `VIEW_CHANNEL` o `VIEW_TX_RX`, dependiendo de lo que se quiera observar de la red.

Una posible ejecución de este script es la siguiente(asumiendo donde se encuentra `contiki-ng`):

```
cd /home/$USER/contiki-ng/rsi-proyecto-grupo6/scripts && ./visual-options.bash VIEW_TX_RX
```

Luego de ejecutado el script, el comportamiento de los LEDs de tanto los clientes como los servidores, va a seguir el lineamiento mencionado en las subsecciones siguientes.

#### 3.1.1.1. Visualización Tx y Rx
Este es el modo de visualización predeterminado de la red. Prende ambos LEDs (rojo y verde) si la radio se prende, y los apaga cuando la radio se apaga. En el momento que el nodo comienza a transmitir, el mismo hace parpadear el LED verde. Si el nodo está en proceso de recepción, parpadea el LED rojo.

#### 3.1.1.2. Visualización de Canal
Al seleccionar este modo, el nodo mostrará el estado de prendido/apagado con el LED rojo, mientras que con el LED verde indicará el canal por el cual se está comunicando. La red TSCH debe tener únicamente 2 canales por los cuales transmitir. En caso de estarse comunicando por el definido como *CANAL_0* (20 por defecto), el LED verde se prenderá, mientras que si está comunicándose por el definido como *CANAL_1* (25 por defecto), el LED verde se apagará.

### 3.1.2. Configuración en Tiempo de Ejecución
Por defecto, los nodos cliente y servidor operan como una red RPL/TSCH regular. Típicamente, la red puede tener tantos nodos cliente como se necesite, pero un solo nodo servidor. Desde el inicio, el nodo servidor está configurado como la raíz del DODAG en la red RPL, ya que es el comportamiento que queremos replicar (modo coordinador). Si es necesario (pero no recomendado), la raíz puede cambiarse emitiendo los siguientes comandos en la shell de Contiki-NG del nuevo nodo raíz (a través de puertos serie):

```
make login PORT=<puerto>; el puerto podría ser: /dev/ttyACM0
rpl-set-root 1
```

Esto también lo pondrá automáticamente en modo coordinador TSCH.

## 3.2. Cargado en los Nodos

### 3.2.1. Nodos Cliente

### 3.2.2. Nodo Servidor
