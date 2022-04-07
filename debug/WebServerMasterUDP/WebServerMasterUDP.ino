/*
 * NO FUNCIONA LINEA 111 y 179 
 * pareciera que el error era por que el buffer de entrada es de tipo byte
 * cuando tendria que ser de tipo char
 * 
 * Probar con el char casteado al inicio de la declaracion
 * 
 * Probar cambiando el tipo de variable del buffer de entrada
  Web Server

 A simple web server that shows the value of the analog input pins.
 using an Arduino Wiznet Ethernet shield.

 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 * Analog inputs attached to pins A0 through A5 (optional)

 created 18 Dec 2009
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe
 modified 02 Sept 2015
 by Arturo Guadalupi
 
 */

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

#define NTP_PACKET_SIZE 48

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEE
};
IPAddress ip(192, 168, 20, 200);


//Set global variables for NTP requests
unsigned int localPort = 8888;       // local port to listen for UDP packets

const char timeServer[] = "time.nist.gov"; // time.nist.gov NTP server

//const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message

byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

unsigned int horas;
unsigned int minutos;
unsigned int segundos;
// A UDP instance to let us send and receive packets over UDP
EthernetUDP Udp;


// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("Ethernet WebServer Example");

  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);

  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }

  // start the server
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());

  Udp.begin(localPort);
}


void loop() {
  sendNTPpacket(timeServer);
  delay(100);
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    //Si el mensaje proviene del puerto 26026
    //Se trata de una lectura de DHT de un cliente
    if(Udp.remotePort()==26026){
      Serial.print("Received packet of size ");
      Serial.println(packetSize);
      Serial.print("From ");
      IPAddress remote = Udp.remoteIP();
      for (int i=0; i < 4; i++) {
        Serial.print(remote[i], DEC);
        if (i < 3) {
          Serial.print(".");
        }
      }
      Serial.print(", port ");
      Serial.println(Udp.remotePort());
  
      // read the packet into packetBufffer
      Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
      Serial.println("Contents:");
      Serial.println((char)packetBuffer);
    }

    //Caso contrario
    //Es la respuesta del servidor NTP
    //Con la hora UTC actual
    else{
      Udp.read(packetBuffer, NTP_PACKET_SIZE);
  
      //MOVER A FUNCION?
      unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
      unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
      // combine the four bytes (two words) into a long integer
      // this is NTP time (seconds since Jan 1 1900):
      unsigned long secsSince1900 = highWord << 16 | lowWord;
      const unsigned long seventyYears = 2208988800UL;
      // subtract seventy years:
      unsigned long epoch = secsSince1900 - seventyYears;
     // get the hour, minute and second:
      horas=(epoch  % 86400L) / 3600-3; // print the hour (86400 equals secs per day)
      minutos=(epoch  % 3600) / 60; // print the minute (3600 equals secs per minute)
      segundos=epoch % 60; // print the second
    }
  }

  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<header>");
          
          client.println("<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\">");
          client.println("</header>");
          client.println("<html>");
          // output the value of each analog input pin
          //for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
          //  int sensorReading = analogRead(analogChannel);
          //  client.print("analog input ");
          //  client.print(analogChannel);
          //  client.print(" is ");
          //  client.print(sensorReading);
          //  client.println("<br />");
          //}
          while(horas==-1);
          client.print(horas);
          client.print(":");
          client.print(minutos);
          client.print(":");
          client.print(segundos);
          client.print("| Sala |");
          horas=-1;
          client.print((char)packetBuffer);
          client.print("°C ");
          client.print("<br />");
          client.print("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}


// send an NTP request to the time server at the given address
void sendNTPpacket(const char * address) {
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); // NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}
