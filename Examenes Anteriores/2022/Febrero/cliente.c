#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <math.h>

#define PORT 2119

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usar: %s <IP address>\n", argv[0]);
        exit(1);
    }
    struct sockaddr_in servaddr;
    int sockfd, n;
    int tam_buffer=sizeof(uint16_t)+sizeof(uint32_t);
    char buffer[tam_buffer];
    char *aux;
    uint16_t temperatura;
    uint32_t control;
    
    //1. Creo el socket de conexiones TCP
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    //2. Cargo campos para conectar
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr=inet_addr(argv[1]);
    socklen_t tam_servaddr=sizeof(servaddr);
    
    //3. Conecto al servidor
    if (connect (sockfd, (struct sockaddr *)&servaddr,tam_servaddr)<0){
        perror("connect");
        close(sockfd);
        exit(-1);
    }
    printf("Conectado al servidor...\n");
    do{
    n = recv(sockfd, buffer, tam_buffer, 0);
    aux=buffer;
    memcpy(&temperatura,aux,sizeof(uint16_t));
    temperatura=ntohs(temperatura);
    aux += sizeof(uint16_t);
    memcpy(&control,aux,sizeof(uint32_t));
    control=ntohs(control);
    if (pow(temperatura,3)!=control)
        printf ("error al recibir la temperatura");
    else
    
    printf("Mensaje de recibido: Temperatura= %d Control=%u\n", temperatura,control);
    } while(1);
    close(sockfd);

    return 0;
}
