//#include "IP2Room.h"
#include <stdlib.h>
#include <string.h>

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

int getID(char *IP, int index){
	int int_ip[4];
	int c=0;
	char aux[3];
	int cAux=0;

	for(int i=0;i<strlen(IP);i++){
		if(IP[i]!='.'){
			if(cAux<3){
				aux[cAux]=IP[i];
				cAux++;
			}
			else cAux=0;
		}
		else{
		int_ip[c]=atoi(aux);
		c++;
		for(int j=0;j<strlen(aux);j++)aux[j]="";
		cAux=0;
		}
	}

	int_ip[c]=atoi(aux);
	return int_ip[index-1];
}

char *getRoom(int IP){
  for(int i=0;i<arrayLength;i++){
    if(IP==ipList[i]){
      return salas[i];
    }
  }
  return "No Match";
}
