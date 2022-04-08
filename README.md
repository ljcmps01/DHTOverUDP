# DHTOverUDP
Sistema de control de temperatura de sala a traves de UDP

## Daily LOG
### 08/04
* Programa base de registro a tarjeta SD
* Funcion getRoom, IP->Sala
* Se agregó los archivos de la libreria de funciones al programa master

Al incluir la lectura de datos por SD se alcanzó una ocupación de memoria dinamica del 81% haciendo que el programa sea inutilizable
Crear nueva rama de desarrollo experimental para continuar con esa sección

### 07/04
* Programas demo de configuracion de las IP -> Sala
* Funcion para obtener sectores de la IP en tipo INT

Se retiro la funcionalidad del reloj NTP del programa Master por problemas de recepcion y cruce de datos al obtener la informacion del servidor NTP y el cliente
Investigar formas alternativas de obtener la hora real


### 06/04

## Pendientes
* Agregar funcionalidad de humedad en cliente y servidor (lectura, trasmision y recepción)
* Agregar lectura de hora real
* Hacer lectura del HTML desde la SD para optimizar espacio en memoria
