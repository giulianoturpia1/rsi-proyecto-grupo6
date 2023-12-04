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

# 1. Resumen del Proyecto

## 1.1. Introducción

La idea principal del proyecto es visualizar de manera interactiva, utilizando LEDs, una red de sensores inalámbricos que utilizan TSCH/RPL. El objetivo es observar el funcionamiento de la red en diferentes etapas de su vida (creación de red, intercambio de mensajes, direccionamiento, entre otros). Los resultados podrían ser utilizados en un entorno interactivo para promover una mejor comprensión de conceptos, o en futuras iteraciones del curso RSI.

## 1.2. Motivación

En una red RPL que utiliza TSCH, existen ciertas etapas durante su vida, como el descubrimiento del DODAG, intercambio controlado de mensajes pasando por un nodo raíz, transmisión de mensajes basada en intervalos de tiempo (TSCH), entre otros. Estos eventos, aunque pueden durar un tiempo, hay eventos con tiempos asociados muy cortos, como por ejemplo la transmisión/recepción de mensajes. Hay varias opciones para un red red TSCH, pudiendo ser con Orchestra, o el camino de un mensaje UDP a través del DODAG y cómo se anuncian diferentes mensajes en una red (EB, DIO, DAO).

El objetivo, entonces, es tener un sistema que permita visualizar el funcionamiento de una red de este tipo utilizando LEDs.

# 2. Instalación del Proyecto

## 2.1. Instalación del Repositorio

Ejecuta los siguientes comandos, asegurándote de haber instalado previamente Contiki-NG (a través de Docker o de manera nativa) y configurado correctamente las credenciales de GitHub. En una terminal ejecutar:

```
sudo apt-get install make
cd ~/contiki-ng && git clone git@github.com:giulianoturpia1/rsi-proyecto-grupo6.git
```

Destacar que el directorio de Contiki-NG se eligio en ese lugar por conveniencia, pudiendo ser posible elegir otra ubicacion, al seguir el tutorial de instalacion de Contiki-NG. Se recomienda la seccion de instalacion de Docker.
A su vez, es necesario tener el entorno `./contiker` en el directorio de Contiki-NG. Para esto, se deben ejecutar los siguiente comandos en una terminal:

```
cd ~/contiki-ng && wget https://rsi.pages.fing.edu.uy/laboratorios-rsi/Entorno-NG/contiker
chmod a+x contiker
```

La versión más reciente de Contiki-NG utilizada es del dia 30/11/2023 y corresponde con el commit Hash `3754d008846b2e0798f606e71714b298fb9a7eb9` del repositorio de Contiki-NG.

## 2.2. Instalación de Dependencias

Para funcionar correctamente los siguientes proyectos en caso de necesitar re-compilar, se deben de reemplazar ciertas secciones de código del sistema operativo de Contiki-NG. En particular, dentro de la carpeta `scripts` del repositorio se encuentra un pequeño script ejecutable que permite actualizar todo lo necesario. Esencialmente el script copia los contenidos de la carpeta `patch` dentro de la ubicación de Contiki-NG (por lo general en `~/contiki-ng`) y se realizan ciertas actualizaciones para correr de manera correcta los programas.

El script `patch-setup.bash` toma dos parámetros los cual deben de pasarse correctamente para realizar el parche y poder luego generar los proyectos:

+ `CONTIKI_PATH`: Es en donde se encuentra la carpeta de `contiki-ng` y donde se va a aplicar el parche. Si la ubicación del repositorio es `/home/user/contiki-ng`, `CONTIKI_PATH=/home/user` a modo de ejemplificar.

+ `TSCH_STRETCH`: Es el factor de enlentecimiento de los *timeslots* de la red, factor que es aplicado a cada tiempo asociado al *timeslot*. El factor debe de ser mayor o igual a 1 siempre. Se recomienda utilizar un factor dentro de [10-15], pues timeslots mayores no permiten que la red se arme correctamente.

Una posible ejecución de este script es la siguiente(asumiendo donde se encuentra `contiki-ng`). Para este ejemplo tomaremos que la ubicación es `/home/user/contiki-ng`

```
cd /home/$USER/contiki-ng/rsi-proyecto-grupo6/scripts && ./patch-setup.bash /home/$USER 10
```

Luego de ejecutado el script, el parche debería de correctamente actualizar los archivos con el fin de poder correr el proyecto del repositorio. Independientemente de la plataforma que se eliga. Tener en cuenta que se amplifican los tiempos de cada plataforma, con lo cual el efecto en una puede ser diferente a otra.

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

El proyecto consiste como se mencionaba brevemente en una red TSCH/RPL en la cual se espera tener un nodo *servidor* y varios nodos *cliente*. La red TSCH/RPL por defecto utiliza **TSCH** como protocolo en sub-capa MAC y utiliza la implementación de **RPL-Lite** de Contiki-NG, la cual no almacena en los nodos las tablas de forwarding (el **root** se encarga de indicar el destino de los paquetes IPv6).
El nodo servidor actúa como **root** del DODAG (Destination-Oriented, Directed, Acyclic Graph) en RPL. Los mensajes van a ser enviados mediante *UDP* unicast a cualquiera de los nodos de la red, donde cada uno es capaz de detectar si le llega una respuesta correcta, mostrando en sus logs locales mediante *LOG_INFO*. Esto es así pues tener ida y vuelta en una red de varios nodos (especial como la del proyecto) puede saturarse con muchos mensajes a la vez.

Los nodos **cliente** son capaces de enviar mensajes a cualquier nodo, inclusive el servidor(el cual tiene una respuesta de logs como se mencionó previamente), apretando los botones disponibles en las plataformas, en donde la cantidad de veces presionados dictan el nodo de la red el cual quieren enviar un mensaje. Dicha cantidad dicta el `NODEID` o identificador del nodo (que debe de configurarse al cargarse). Se permiten enviar mensajes hasta un máximo de 16 nodos diferentes en la red (incluyendo al nodo *server*).

Tener en cuenta que se debe de realizar el acondicionamiento del directorio de Contiki-NG y configuracion antes de esta sección, ver [2.2. Instalación de Dependencias](#22-instalación-de-dependencias) y [3. Configuración y Carga del Proyecto](#3-configuración-y-carga-del-proyecto).

Mencionado esto, las plataformas que funcionan para el proyecto son las siguientes:

+ Launchpad CC1350/CC2650 de Texas Instruments:
  * TARGET=cc26x0-cc13x0
  * BOARD=launchpad/cc1350 o BOARD=launchpad/cc2250

+ Nodos Remote de Zolertia (Zoul)
  * TARGET=zoul
  * BOARD=remote-reva o BOARD=remote-revb

**Nota:** Podría llegar a ser necesario modificar algunos valores por defecto de la red TSCH/RPL que no se modifican en este proyecto, pues pueden existir problemas de utilización de memoria (como cantidad de vecinos maximos, tamaño de la trama, etc).

Dado lo mencionado hasta el momento, es posible cargar los nodos *cliente* y *servidor* para poder visualizar la red TSCH/RPL utilizando una serie de comandos para cargarlos, dentro del contenedor de Contiki-NG, `./contiker`.

Para poder cargar los códigos de los nodos cliente y servidor, se deben de realizar los siguientes pasos, procurando que **todos** los nodos de la red deben de tener los mismo parámetros configurados (sea cliente o servidor):

1. Conectar el dispositivo mediante USB a la computadora, asegurando que no hay ninguna instancia de `./contiker` corriendo.

2. Entrar al directorio de Contiki-NG y ejecutar el contenedor de Contiki `./contiker` (si se usa Docker, recomendado). Se recomienda instalarlo en `/home/$USER` (como se ha mostrado repetidas veces en este documento):

```
cd ~/contiki-ng && ./contiker
```

3. Dirigirse a las carpetas correspondientes del nodo **cliente** o **servidor**.

### 3.2.1. Nodos Cliente

4. Para ello ejecutar, en la misma terminal de los pasos 1. y 2.:

```
cd ~/contiki-ng/rsi-proyecto-grupo6/src/client_button
```

5. Ejecutar el comando `make motelist-all`, para ver el puerto por el cual debe de cargarse al nodo.
La salida del comando debe de ser similar a la siguiente:

```
Port      	Serial	VID 	PID 	Product                                	Vendor      	 
------------  --------  ------  ------  -----------------------------------------  -----------------
/dev/ttyACM0  L400A87W  0x0451  0xBEF3  XDS110 (03.00.00.26) Embed with CMSIS-DAP  Texas Instruments
/dev/ttyACM1  L400A87W  0x0451  0xBEF3  XDS110 (03.00.00.26) Embed with CMSIS-DAP  Texas Instruments
```

En este caso corresponde a un nodo launchpad CC1350. Utilizar el primer puerto mostrado por el comando, en este caso `/dev/ttyACM0`, que en general siguen la convención `/dev/ttyACM*`, donde `*` es un número.

6. Entrar su nodo en modo programación. Dependiendo de su plataforma, consultar con el manual del nodo a utilizar. En general se deben de presionar una combinación de botones en el nodo, o utilizar un programa para realizarlo.

7. Cargar el programa en su nodo, teniendo en cuenta las siguientes opciones obligatorias y opcionales:

  * Obligatorias
	+ `TARGET=xxxxx`
	+ `BOARD=xxxxxx`
	+ `NODEID=xx`

Son campos los cuales corresponden los primeros dos a las plataformas mencionadas arriba, mientras que el campo `NODEID` debe de valer entre 2 y 16 (el 1 es reservado para el *server*).

  * Opcionales
	+ `MAKE_WITH_ORCHESTRA=x`
	+ `MAKE_WITH_STORING_ROUTING=x`
 
Estas opciones dictan si utilizar solución del "scheduler" autónomo para TSCH, **Orchestra**(ver este [link](https://docs.contiki-ng.org/en/develop/doc/programming/Orchestra.html)) que utiliza RPL. A su vez, se puede configurar si cada nodo utiliza su tabla de ruteo o no, mediante la segunda opción. Por defecto estas opciones se encuentran deshabilitadas, pudiendo activarlas colocando un `1` en donde dice `x`.

Se carga al nodo, con la siguiente sintaxis de comando(en la misma terminal de los pasos previos) y el puerto del paso 5.:

```
make TARGET=xxxxx BOARD=xxxxx NODEID=xx MAKE_WITH_ORCHESTRA=x MAKE_WITH_STORING_ROUTING=x udp-client.upload PORT=/dev/ttyACM0
```

Un ejemplo posible para cargar a un launchpad puede ser el siguiente:

```
make TARGET=cc26x0-cc13x0 BOARD=launchpad/cc1350 NODEID=4 MAKE_WITH_ORCHESTRA=1 MAKE_WITH_STORING_ROUTING=0 udp-client.upload PORT=/dev/ttyACM0
```

### 3.2.2. Nodo Servidor

4. Para ello ejecutar, en la misma terminal de los pasos 1. y 2.:

```
cd ~/contiki-ng/rsi-proyecto-grupo6/src/server
```

5. Ejecutar el comando `make motelist-all`, para ver el puerto por el cual debe de cargarse al nodo.
La salida del comando debe de ser similar a la siguiente:

```
Port      	Serial	VID 	PID 	Product                                	Vendor      	 
------------  --------  ------  ------  -----------------------------------------  -----------------
/dev/ttyACM0  L400A87W  0x0451  0xBEF3  XDS110 (03.00.00.26) Embed with CMSIS-DAP  Texas Instruments
/dev/ttyACM1  L400A87W  0x0451  0xBEF3  XDS110 (03.00.00.26) Embed with CMSIS-DAP  Texas Instruments
```

En este caso corresponde a un nodo launchpad CC1350. Utilizar el primer puerto mostrado por el comando, en este caso `/dev/ttyACM0`, que en general siguen la convención `/dev/ttyACM*`, donde `*` es un número.

6. Entrar su nodo en modo programacion. Dependiendo de su plataforma, consultar con el manual del nodo a utilizar. En general se deben de presionar una combinacion de botones en el nodo, o utilizar un programa para realizarlo.

7. Cargar el programa en su nodo, teniendo en cuenta las siguientes opciones obligatorias y opcionales:

  * Obligatorias
	+ `TARGET=xxxxx`
	+ `BOARD=xxxxxx`
	+ `NODEID=xx`

Son campos los cuales corresponden los primeros dos a las plataformas mencionadas arriba, mientras que el campo `NODEID` debe de valer 1.

  * Opcionales
	+ `MAKE_WITH_ORCHESTRA=x`
	+ `MAKE_WITH_STORING_ROUTING=x`
 
Estas opciones dictan si utilizar solución del "scheduler" autónomo para TSCH, **Orchestra**(ver este [link](https://docs.contiki-ng.org/en/develop/doc/programming/Orchestra.html)) que utiliza RPL. A su vez, se puede configurar si cada nodo utiliza su tabla de ruteo o no, mediante la segunda opcion. Por defecto estas opciones se encuentran deshabilitadas, pudiendo activarlas colocando un `1` en donde dice `x`.

Se carga al nodo, con la siguiente sintaxis de comando(en la misma terminal de los pasos previos) y el puerto del paso 5.:

```
make TARGET=xxxxx BOARD=xxxxx NODEID=xx MAKE_WITH_ORCHESTRA=x MAKE_WITH_STORING_ROUTING=x udp-server.upload PORT=/dev/ttyACM0
```

Un ejemplo posible para cargar a un launchpad puede ser el siguiente:

```
make TARGET=cc26x0-cc13x0 BOARD=launchpad/cc1350 NODEID=1 MAKE_WITH_ORCHESTRA=1 MAKE_WITH_STORING_ROUTING=0 udp-server.upload PORT=/dev/ttyACM0
```
