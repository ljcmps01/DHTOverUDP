/*
  Programa base de carga de datos del DHT 22
  cada X intervalo
*/

#include <SPI.h>
#include <SD.h>
#include "DHT.h"

#define DHTPIN 2
#define DHTTYPE DHT22

#define INTERVALO 5000 // En Milisegundos

DHT dht(DHTPIN, DHTTYPE);

const int chipSelect = 4;

int dataLog(char* data, char* filename);

unsigned long cargaPrevia=0;


void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");
  dht.begin();
}

void loop() {
  float temperatura=dht.readTemperature();
  char temperatura_c[5];

  dtostrf(temperatura,1,2,temperatura_c);

  Serial.println(temperatura_c);
  if(millis()-cargaPrevia>INTERVALO-1){
    cargaPrevia=millis();
    Serial.println("carga de datos");
    dataLog(temperatura_c,"datalog.txt");  
  }
  delay(1000);
  
}


int dataLog(char* data, char* filename){
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open(filename, FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(data);
    dataFile.close();
    // print to the serial port too:
    Serial.println(data);
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.print("error opening ");
    Serial.println(filename);
  }
}
