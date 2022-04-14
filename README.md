# DHTOverUDP
Sistema de control de temperatura de sala a traves de UDP

## Daily LOG

### 13/04
* Se integró la funcion de escritura de registros
Hay que corregir la funcion de modo que cada nueva lectura se haga al comienzo del archivo y no al final (posiblemente con file.seek(0)) 

### 12/04
* Nuevo programa de Servidor Web, utilizando Ajax para las variables dinamicas del servidor
* Este nuevo programa carga el HTML desde la SD, ahorrando un significativo espacio en memoria y utiliza el Arduino MEGA

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
* Agregar lectura de hora real con DS3231
* 13/04 ~~Agregar registro en SD de la lectura leida~~
* 12/04 ~~Hacer lectura del HTML desde la SD para optimizar espacio en memoria~~
