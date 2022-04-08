#ifndef IP2Room.h
#define IP2Room.h

//Le paso la ip completa de tipo char y el #orden 
//que requiero para identificar el dispositivo
//devuelve el #orden
//int getID(char *IP, int index);

//Le paso el #orden y me devuelve a que sala pertenece
char* getRoom(int IP);

//funcion de escritura de archivo
int dataLog(char* data, char* filename);
#endif
