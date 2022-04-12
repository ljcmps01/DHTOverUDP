/*
ClientUDPCommunication
Este programa se encarga de leer la informacion del DHT22
y transmitirla via UDP al servidor web (Arduino MEGA)

En caso de conectar la placa y no recibir los datos deseados,
Conectar la placa mediante USB a la computadora y revisar los registros
impresos en el monitor serial
 */


#include <Ethernet.h>
#include <EthernetUdp.h>
#include "DHT.h"
#include <stdlib.h>

#define INTERVALO 3 //Intervalo en segundos entre lectura y lectura del sensor

#define DHTPIN 2
#define DHTTYPE DHT22

#define ARRAY_SIZE 32

//La direccion IP y MAC deben ser UNICAS entre cada dispositivo
//Corroborar que ninguno de los valores coincida con ninguna de
//las maquinas que se encuentren en la isla de conexión
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEE
};
//El 3er numero de la IP debe coincidir con el proporcionado por el switch
IPAddress ip(192, 168, 20, 201);

unsigned int localPort = 26026;      // local port to listen on

//IP y puerto de comunicación del servidor web
char masterIP[]="192.168.20.200";
int masterPort=26026;

// buffers for receiving and sending data
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];  // buffer to hold incoming packet,
char replyBuffer[ARRAY_SIZE];




// Creo las instancias de la libreria UDP para la comunicacion con el servidor
// y el DHT para la lectura del sensor
EthernetUDP Udp;
DHT dht(DHTPIN,DHTTYPE);

void setup() {
  // Inicializacion de la conexion Ethernet
  Ethernet.begin(mac, ip);

  Serial.begin(9600);

  // Chequeo que el shield de Ethernet esté conectado correctamente
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("El Ethernet shield no esta disponible.");
    while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Cable Ethernet desconectado.");
  }

//  Inicializamos los componentes
  dht.begin();
  Udp.begin(localPort);
}

void loop() {
  //Leemos los datos del DHT22
  float temperature = dht.readTemperature();

  //Si lo recibido no es un numero
  if(isnan(temperature)){
    Serial.println(F("Lectura del sensor fallida"));//Lanzamos un aviso de error
  }
  //Caso contrario
  else{
    //Avisamos que se leyó correctamente
    Serial.println(F("Datos recibidos correctamente"));
    dtostrf(temperature,1,2,replyBuffer); //Guardamos la lectura en el buffer de salida en formato string
    Serial.print(F("Enviando: "));        //y mostramos lo que estamos por mandar
    Serial.println(replyBuffer);
    
    Udp.beginPacket(masterIP, masterPort);  //Comenzamos la comunicacion al servidor
    Udp.write(replyBuffer,ARRAY_SIZE);      //Escribimos el mensaje en el buffer
    Udp.endPacket();                        //Enviamos la informacion y cerramos la comunicación
  }

  
  
  delay(INTERVALO*1000);
}


/*
  Processing sketch to run with this example
 =====================================================

 // Processing UDP example to send and receive string data from Arduino
 // press any key to send the "Hello Arduino" message


 import hypermedia.net.*;

 UDP udp;  // define the UDP object


 void setup() {
 udp = new UDP( this, 6000 );  // create a new datagram connection on port 6000
 //udp.log( true ); 		// <-- printout the connection activity
 udp.listen( true );           // and wait for incoming message
 }

 void draw()
 {
 }

 void keyPressed() {
 String ip       = "192.168.1.177";	// the remote IP address
 int port        = 8888;		// the destination port

 udp.send("Hello World", ip, port );   // the message to send

 }

 void receive( byte[] data ) { 			// <-- default handler
 //void receive( byte[] data, String ip, int port ) {	// <-- extended handler

 for(int i=0; i < data.length; i++)
 print(char(data[i]));
 println();
 }
 */
