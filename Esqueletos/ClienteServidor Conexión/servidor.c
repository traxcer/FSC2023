/*
Programador Manuel Eloy Gutiérrez

SERVIDOR

Objeto del programa:
Esqueleto funcional para la conexión del un cliente a un servidor
por medio de socket, a falta de implementar la lógica del programa.

Málaga a 15 de Junio del 2023
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>

#define PUERTO 4950
#define T 250

int main(){

int sd, n_sd;
struct sockaddr_in dir_server, dir_client;

memset(&dir_server,0,sizeof(dir_server));

dir_server.sin_family=AF_INET;
dir_server.sin_port=htons(PUERTO);
dir_server.sin_addr.s_addr=INADDR_ANY;

socklen_t tam_dir_client=sizeof(dir_client);
socklen_t tam_dir_server=sizeof(dir_server);

if((sd=socket(AF_INET,SOCK_STREAM,0))<0){
    perror("SERVER: socket");
    exit(1);
}
if(bind(sd,(struct sockaddr*)&dir_server,tam_dir_server)<0){
    perror("SERVER: bind");
    close(sd);
    exit(1);
}
if(listen(sd,5)<0){
    perror("SERVER: listen");
    close(sd);
    exit(1);
}
printf("Esperando conexión.\n");
if((n_sd = accept(sd,(struct sockaddr *)&dir_client,&tam_dir_client))<0){
    perror("SERVER: accept");
    close(sd);
    exit(1);
}
    printf("Se ha conectado el cliente. Socket del cliente:%d\n",n_sd);
    char b[T];
    while(1){
        read(n_sd,b,T);
        printf("Leido del Cliente: %s",b);
    }
    return 0;
}