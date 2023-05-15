#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <math.h>
#include <errno.h>

#define PORT 2119

ssize_t read_n(int fd,char *b,size_t tam);

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

    //4. select para controlar entrada por teclado de fin y datos del servidor
    fd_set cjto, cjto_trab;
    FD_ZERO(&cjto);
    FD_SET(0,&cjto);
    FD_SET(sockfd,&cjto);
    int max_desc=sockfd;
    int fin=0;
    do{
        memcpy(&cjto_trab,&cjto,sizeof(cjto));
        int r=select(max_desc+1,&cjto_trab,NULL,NULL,NULL);
        if (r<0){
            perror("Cliente TCP: select");
            close(sockfd);
            exit(-1);
        }
        if(FD_ISSET(sockfd,&cjto_trab)){
            n = read_n(sockfd, buffer, tam_buffer);
            printf("(Recibidos %d bytes) ",n);
            aux=buffer;
            memcpy(&temperatura,aux,sizeof(uint16_t));
            temperatura=ntohs(temperatura);
            aux += sizeof(uint16_t);
            memcpy(&control,aux,sizeof(uint32_t));
            control=ntohl(control);
            if (pow(temperatura,3)!=control){
                printf ("error al recibir la temperatura");
            } else {
                printf("Mensaje recibido: Temperatura= %d Control=%u\n", temperatura,control);
            }
        }
        if(FD_ISSET(0,&cjto_trab)){
            read(0,buffer,20);
            if (strncmp("fin",buffer,3)==0){
                printf("El cliente ha cerrado la conexión\n");
                close (sockfd);
                fin=1;
            } else {
                printf("Comando no reconocido\n");
            }
        }
    } while (fin==0);
return 0;
}

/*==================
Funciones auxiliares
===================*/

ssize_t read_n(int fd,char *b,size_t tam){
    int aescribir=tam;
    int total_escrito=0;
    int escrito;
    do{
        errno=0;
        escrito=read(fd,b+total_escrito,aescribir);
        if (escrito>0){
            aescribir -=escrito;
            total_escrito +=escrito;
        }
    } while (((escrito>0)&&(total_escrito<tam))||(errno==EINTR));
    if (total_escrito>0)
        return total_escrito;
    else
        return escrito;
}