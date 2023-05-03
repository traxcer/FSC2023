#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include "write_n.c"

#define T 255


int main(){

struct PDU {
    uint16_t size;
    char payload[T];
} datos;
datos.size=3;
char mypayload[T];
memcpy (&datos.payload,"a1b",datos.size);

ssize_t canal=open("prueba.txt", O_WRONLY|O_CREAT|O_TRUNC, 0644);
memcpy(&mypayload,&datos.payload,datos.size);
mypayload[datos.size+1]='\0';

printf("datos.size= %d datos.payload=%s\n",datos.size,mypayload);
uint16_t tam=htons(datos.size);
if((write_n(canal,(char *) &tam,sizeof(uint16_t)))<0){
    perror("write_n datos.size");
    close (canal);
    exit(-1);
}
if((write_n(canal,(char *) &datos.payload,datos.size))<0){
    perror("write_n datos.size");
    close (canal);
    exit(-1);
}
close (canal);
printf("fin\n");
return 0;
}