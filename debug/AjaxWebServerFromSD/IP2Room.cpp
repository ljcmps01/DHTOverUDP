//#include "IP2Room.h"
#include <stdlib.h>
#include <string.h>
#include <SD.h>

int ipList[]={
  201,
  202,
  203
};

const int arrayLength=sizeof(ipList)/sizeof(ipList[0]);

char *salas[arrayLength]={
  "Bajo",
  "Nueva",
  "Bouchard"
};

char *getRoom(int IP){
  for(int i=0;i<arrayLength;i++){
    if(IP==ipList[i]){
      return salas[i];
    }
  }
  return "No Match";
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

int getIndex(int ip){
  for(int i=0;i<arrayLength;i++){
    if(ipList[i]==ip){
      return i;
    }
  }
  return -1;
}

char* parseLog(char *tiempo, int index, float grados){
  char logBuffer[30]="";
  char c_grados[7];
  dtostrf(grados,1,2,c_grados);
  
  strcat(logBuffer,tiempo);
  strcat(logBuffer," \t\t|\t ");
  strcat(logBuffer,salas[index]);
  strcat(logBuffer," \t\t|\t ");
  strcat(logBuffer,c_grados);

  char* outBuffer=logBuffer;
  return outBuffer;
}
