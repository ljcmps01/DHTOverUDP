/*--------------------------------------------------------------
  Programa:      AjaxWebServerFromSD

  Descripcion:  Arduino web server que muestra el estado de
                cada sala (temperatura) recibiendo informacion
                de los clientes DHT (Arduino Uno)
                La pagina se crea en base a la tarjeta SD.
                La pagina web contiene codigo JavaScript que usa
                Ajax y XML para actualizar la informacion en la web.
                Los clientes se comunican con está placa mandando 
                su informacion a traves de UDP
  
  Hardware:     Shield de Ethernet acoplado a un Arduino MEGA
                Conectar el DHT encima de la siguiente forma
                     __ __ __ __ ___ ___
     Arduino  5V____|   |   |   |   |   |\Izquierda                     
                    |___|___|___|___|___| \                   
            Data____|   |   |   |   |   |  \
                    |___|___|___|___|___|   |Derecha
                ____|   |   |   |   |   |   |
                    |___|___|___|___|___|  /
             GND____|   |   |   |   |   | /
                    |___|___|___|___|___|/

--------------------------------------------------------------*/

#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>
#include "DHT.h"
#include <EthernetUdp.h>

#include "IP2Room.h"

// Tamaño del buffer usado para recibir el pedido HTTP
#define REQ_BUF_SZ   50

// Configuracion del DHT
#define DHTPIN 2
#define DHTTYPE DHT22

#define INTERVALO 10

// Direccion MAC del Master
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 20, 200); // Direccion IP, cambiar segun switch
EthernetServer server(80);  // Habilitamos el servidor web en el puerto 80

//Configuracion UDP
unsigned int localPort = 26026;   //Puerto de escucha
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];  // buffer de paquetes entrantes,

//Preparo los archivos y variables de la web
File webFile;                     // Archivo de la pagina web en la SD
char HTTP_req[REQ_BUF_SZ] = {0};  // buffered HTTP request stored as null terminated string
char req_index = 0;               // index into HTTP_req buffer

DHT dht(DHTPIN, DHTTYPE);

EthernetUDP Udp;

extern int ipList[];

float tempSalas[SIZE]={0};

unsigned long millisPrevio=0;

void setup(){    
    Serial.begin(9600);       
    
    if (!SD.begin(4)) {
        Serial.println("ERROR - Inicializacion del modulo SD fallida");
        return;    // Inicializacion del modulo SD fallida
    }
    Serial.println("EXITO - Tarjeta SD inicializada correctamente.");
    // Busco el archivo index.htm
    if (!SD.exists("/WebPage/index.htm")) {
        Serial.println("ERROR - No se encontro el archivo index.htm!");
        return;
    }
    Serial.println("SUCCESS - Archivo index.htm encontrado.");
    
    Ethernet.begin(mac, ip);  // Inicializar la conexion ethernet con 
                              //la direccion mac e ip proporcionadas

    server.begin();           // inicializar el servidor web
    Serial.print("Servidor inicializado en: ");
    Serial.println(Ethernet.localIP());

    Udp.begin(localPort);     //Inicilizar la escucha por el puerto UDP
    dht.begin();
}



void loop()
{   
    float temperatura;
    int packetSize=Udp.parsePacket();
    int id;

    unsigned long millisActual=millis();

    /*----------------------------------------------------------------
     *            INICIO DE LECTURA DE PAQUETE DE DATOS
     ----------------------------------------------------------------*/
    if(packetSize){
      Serial.print("Se recibio un paquete del tamaño ");
      Serial.println(packetSize);
      Serial.print("de la IP:  ");
      IPAddress remote = Udp.remoteIP();
      for (int i=0; i < 4; i++) {
        Serial.print(remote[i], DEC);
        if (i < 3) {
          Serial.print(".");
        }
      }
      
      Serial.print(", puerto ");
      Serial.println(Udp.remotePort());
      id=remote[3];
      
      Serial.print("ID: ");
      Serial.println(id);
      Serial.print("indice: ");
      Serial.println(getIndex(id));
      Serial.print("sala: ");
      Serial.println(getRoom(id));
      
      // read the packet into packetBufffer
      Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
      temperatura=atof(packetBuffer);
      tempSalas[getIndex(id)]=temperatura;
      Serial.println("Contenido:");
      Serial.println(temperatura);
    }
    delay(10);
    /*----------------------------------------------------------------
     *            FIN DE LECTURA DE PAQUETE DE DATOS
     ----------------------------------------------------------------*/

    if(millisActual - millisPrevio>=INTERVALO*1000){
      millisPrevio=millisActual;
      webFile = SD.open("DATALOG.txt",FILE_WRITE);
      if (dataLog) {
          char hora[6];
          
          Serial.println("Escribiendo Datalog.txt...");
          for(int i=0;i<SIZE;i++){
            char* buff=parseLog(itoa(millisActual,hora,6),i,tempSalas[i]);                
            Serial.println(buff);
            webFile.print(buff);
            webFile.print("#");            
            webFile.println("----------------------------------------------#");
          }
          // close the file:
          webFile.close();
          Serial.println("Listo.");
        } 
        else {
          // if the file didn't open, print an error:
          Serial.println("error abriendo el Datalog");
        }
    }

    /*----------------------------------------------------------------
     *            INICIO DE PAGINA WEB
     ----------------------------------------------------------------*/

    EthernetClient client=server.available();
    if (client) {  // Si hay clientes...
        boolean currentLineIsBlank = true;
        while (client.connected()) {
            if (client.available()) {   // Esperamos a que haya informacion del cliente
                char c = client.read(); // Leo el primer byte del cliente
                if (req_index < (REQ_BUF_SZ - 1)) {
                    HTTP_req[req_index] = c;          // Guardamos el pedido HTTP
                    req_index++;
                }
                //Llegamos al final del pedido
                if (c == '\n' && currentLineIsBlank) {
                    // Enviamos un header de respuesta HTTP estandar
                    client.println("HTTP/1.1 200 OK");
                    // El resto del header depende de lo que requiera el cliente
                    // variando entre un pedido de pagina web o Ajax
                    
                    // Si es un pedido Ajax --> Envio archivo XML
                    if (StrContains(HTTP_req, "ajax_inputs")) {
                        
                        client.println("Content-Type: text/xml");
                        client.println("Connection: keep-alive");
                        client.println();
                        // Enviamos el archivo XML con las entradas obtenidas por UDP
                        XML_response(client,temperatura);
                    }
                    
                    else {  // Caso contrario, enviar la pagina web
                        
                        client.println("Content-Type: text/html");
                        client.println("Connection: close");
                        client.println("Refresh: 10");
                        client.println();
                        // Envio la pagina web
                        webFile = SD.open("/WebPage/index.htm");        // Abro el archivo index
                        if (webFile) {
                            while(webFile.available()) {
                                client.write(webFile.read()); // Envio la pagina web al cliente
                            }
                            webFile.close(); //Cierro el archivo
                        }
                        webFile = SD.open("DATALOG.txt");
                        if (webFile) {
                            int archivosLeidos=0;
                            int flagArchivos=1;
                          
                            while(webFile.available()) {
                              if(flagArchivos){
                              char caracter=webFile.read();
                              if(caracter!='#'){
                                if(archivosLeidos<24*2){
                                  client.write(caracter); // Envio la pagina web al cliente
                                  
                                }
                                else{
                                  flagArchivos=0;
                                  break;
                                }
                                
                              }
                              else{
                                archivosLeidos++;  
                                client.write("<br>");
                              }
                            }
                          }
                          webFile.close(); //Cierro el archivo
                        }
                        client.write("</div>");
                        client.write("</body>");
                        client.write("</html>");
                    }
                    // Muestro en el monitor serial el pedido HTTP del cliente
                    Serial.println(HTTP_req);
                    // Reseteo el index y el buffer HTTP
                    req_index = 0;
                    StrClear(HTTP_req, REQ_BUF_SZ);
                    break;
                }
                // Cada linea recibida del cliente termina con \r\n
                if (c == '\n') {
                    // Recibi el ultimo caracter de la linea
                    currentLineIsBlank = true;
                } 
                else if (c != '\r') {
                    // Se recibio otro caracter del cliente
                    currentLineIsBlank = false;
                }
            } // end if (client.available())
        } // end while (client.connected())
        delay(1);      // Damos un poco de tiempo al cliente para procesar la informacion
        client.stop(); // Cerramos al conexion
    } // end if (client)
}

// Envio el archivo XML con las ultimas temperaturas leidas de cada sala
void XML_response(EthernetClient cl,float t)
{
    cl.print("<?xml version = \"1.0\" ?>");
    cl.print("<inputs>");
    for(int i=0;i<SIZE;i++){
      switch(i){
        case 0:    
          cl.print("<lecturaBajo>");
          if(!tempSalas[i]){
            cl.print("...");  
          }
          else{
            cl.print(tempSalas[i]);
            cl.print("°C");
          }
          cl.print("</lecturaBajo>");
          //read A1
          
          break;
          
        case 1:    
          cl.print("<lecturaNueva>");
          if(!tempSalas[i]){
            cl.print("...");  
          }
          else{
            cl.print(tempSalas[i]);
            cl.print("°C");
          }
          cl.print("</lecturaNueva>");
          //read A1
          
          break;
          
        case 2:    
          cl.print("<lecturaBouchard>");
          if(!tempSalas[i]){
            cl.print("...");  
          }
          else{
            cl.print(tempSalas[i]);
            cl.print("°C");
          }
          cl.print("</lecturaBouchard>");
          
          break;
      }
    }
    cl.print("</inputs>");    
}

// Settea cada caracter de la string a 0, es decir la limpiamos
void StrClear(char *str, char length)
{
    for (int i = 0; i < length; i++) {
        str[i] = 0;
    }
}

// Busca la existencia de sfind en str
// return 1 si sfind existe en str
// return 0 si sfind NO existe en str
char StrContains(char *str, char *sfind)
{
    char found = 0;
    char index = 0;
    char len;

    len = strlen(str);
    
    if (strlen(sfind) > len) {
        return 0;
    }
    while (index < len) {
        if (str[index] == sfind[found]) {
            found++;
            if (strlen(sfind) == found) {
                return 1;
            }
        }
        else {
            found = 0;
        }
        index++;
    }

    return 0;
}
