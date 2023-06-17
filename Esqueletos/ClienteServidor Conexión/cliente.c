/*
Programador Manuel Eloy Gutiérrez

CLIENTE

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

int main(int argc, char const *argv[])
{
    if (argc<3){
        printf("Usar: %s <dirIP> <Puerto>\n",argv[0]);
        exit (1);
    }
    struct sockaddr_in dir_server;
    socklen_t tam_dir_server=sizeog(dir_server);
    memset(&dir_server,0,tam_dir_server);
    dir_server.sin_port=htons(atoi(argv[2]));
    dir_server.sin_family=AF_INET;
    int dirserver;
    if ((dirserver = inet_addr(argv[1])) < 0) {
        printf("Error al convertir la IP %s\n", argv[1]);
exit(1);
}
    
    int sd=socket(AF_INET,SOCK_STREAM,0);
    printf("Conectando al servidor...\n");    
    if((connect(sd,(struct sockaddr *)&dir_server,tam_dir_server))<0){
        perror("CLIENTE: connect");
        close (sd);
        exit(1);
    }
    printf("Conectado\n");
    
return 0;
}
