# Visualización del Comportamiento Dinámico de una Red TSCH/RPL a través de LEDs

## Proyecto de Redes de Sensores Inalámbricos - 2023

### Tabla de Contenidos

- [1. Resumen del Proyecto](#1-resumen-del-proyecto)
  * [1.1. Introducción](#11-introducción)
  * [1.2. Motivación](#12-motivación)
- [2. Instalación del Repositorio](#2-instalación-del-repositorio)
- [3. Modo de Operación](#3-modo-de-operación)
  * [3.1. Configuraciones de Línea de Comandos](#31-configuraciones-de-línea-de-comandos)
  * [4. Ejemplos](#4-ejemplos)

# 1. Resumen del Proyecto

## 1.1. Introducción

La idea principal del proyecto es visualizar de manera interactiva, utilizando LEDs, una red de sensores inalámbricos que utilizan TSCH/RPL. El objetivo es observar el funcionamiento de la red en diferentes etapas de su vida (creación de red, intercambio de mensajes, direccionamiento, entre otros). Los resultados podrían ser utilizados en un entorno interactivo para promover una mejor comprensión de conceptos, o en futuras iteraciones del curso RSI.

## 1.2. Motivación

En una red RPL que utiliza TSCH, existen ciertas etapas durante su vida, como el descubrimiento del DODAG, intercambio controlado de mensajes pasando por un nodo raíz, transmisión de mensajes basada en intervalos de tiempo (TSCH), entre otros. Estos eventos, aunque pueden durar un tiempo, hay eventos con tiempos asociados muy cortos, como por ejemplo la transmisión/recepción de mensajes. Hay varias opciones para un red red TSCH, pudiendo ser con Orchestra, o el camino de un mensaje UDP a través del DODAG y cómo se anuncian diferentes mensajes en una red (EB, DIO, DAO).

El objetivo, entonces, es tener un sistema que permita visualizar el funcionamiento de una red de este tipo utilizando LEDs.

# 2. Instalación del Proyecto

# 2.1. Instalación del Repositorio

Ejecuta los siguientes comandos, asegurándote de haber instalado previamente Contiki-NG (a través de Docker o de manera nativa) y configurado correctamente las credenciales de GitHub:

```
cd ~/contiki-ng && git clone git@github.com:giulianoturpia1/rsi-proyecto-grupo6.git
```

# 2.2. Instalación de Dependencias

Para funcionar correctamente los siguientes proyectos en caso de necesitar re-compilar, se deben de reemplazar ciertas secciones de codigo del sistema operativo de Contiki-NG. En particular, dentro de la carpeta `patch` del repositorio se encuentra un pequeño instructivo para instalarlo. Esencialmente se debe de copiar los contenidos de la carpeta `patch` dentro de la ubicacion de Contiki-NG (por lo general en `~/contiki-ng`).

# 3. Modo de Operación

## 3.1. Configuración de red
Por defecto, los nodos cliente y servidor operarán como una red RPL/TSCH regular. Típicamente, la red puede tener tantos nodos cliente como se necesite, pero un solo nodo servidor. Desde el inicio, el nodo servidor está configurado como la raíz del DODAG en la red RPL, ya que es el comportamiento que queremos replicar (modo coordinador). Si es necesario (pero no recomendado), la raíz puede cambiarse emitiendo los siguientes comandos en la shell de Contiki-NG del nuevo nodo raíz (a través de puertos serie):

```
make login PORT=<puerto>; el puerto podría ser: /dev/ttyACM0
rpl-set-root 1
```

Esto también lo pondrá automáticamente en modo coordinador TSCH.

## 3.2. Configuración de visualización

El proyecto permite alternar entre modo de visualización de transmisión/recepción (Tx/Rx) en la red TSCH como también modo de visualización de canal de red TSCH. Esto puede ser seteado desde el project-conf.h de cada nodo, lo que permite generar binarios compilados en los que se vea el canal de comunicación, como compilados donde se vea el estado del nodo.

### 3.2.1 Modo de visualización Tx/Rx
Este es el modo de visualización predeterminado de la red. Prende ambos LEDs (rojo y verde) si la radio se prende, y los apaga cuando la radio se apaga. En el momento que el nodo comienza a transmitir, el mismo hace parpadear su LED verde. Si el nodo está en proceso de recepción, parpadea el LED rojo.
Para el funcionamiento de este modo es necesario que no esté definida cualquiera de las siguientes macros:
- **CONFIG_VER_CANAL**
- **CANAL_0**
- **CANAL_1**
De no estar definido cualquiera de ellos, el binario compilado mostrará el estado Tx/Rx.

### 3.2.2 Modo de visualización de canal
Al seleccionar este modo, el nodo mostrará el estado de prendido/apagado con el LED rojo, mientras que con el LED verde indicará el canal por el cual se está comunicando. La red TSCH debe tener únicamente 2 canales por los cuales transmitir. En caso de estarse comunicando por el definido como *CANAL_0*, el LED verde se prenderá, mientras que si está comunicandose por el definido como *CANAL_1*, el LED verde se apagará.

Para activar este modo de funcionamiento, se deben definir las siguientes macros en el archivo *project-conf.h* del nodo en el cual se quiere activar:
- **CONFIG_VER_CANAL** No necesita tomar ningún valor en específico.
- **CANAL_0** Necesita tomar el valor de uno de los canales utilizados en la red TSCH.
- **CANAL_1** Necesita tomar el valor del otro canal definido por la red TSCH.

## 4. Ejemplos

Como se mencionó anteriormente, el proyecto consiste en tener varios recursos disponibles para visualizar ciertos comportamientos de una red TSCH/RPL. Refiriéndose a la sección [3. Modo de Operación](#3-modo-de-operación), hay varios proyectos disponibles para compilar y luego cargar en la red deseada de nodos, que son:

- Visualizar el Descubrimiento/Actualización de la Red.
- Visualizar el TX/RX de los Nodos.
- Visualizar el Salto de Canales de los Nodos.
- Visualizar Intervalos de Tiempo y Ranuras de Trama.