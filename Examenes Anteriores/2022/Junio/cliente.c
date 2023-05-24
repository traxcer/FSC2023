/*
Programa cliente.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <unistd.h>
#include <errno.h>


//#define PUERTO 4950
#define T 255

//prototipos funciones auxiliares
int write_n(int fd, char *b, int n);

int main(int argc, char *argv[]){

    if (argc<3){
        printf("Uso: %s <IP> <PUERTO>\n",argv[0]);
        exit(1);
    }

    struct sockaddr_in dirserver;
    memset(&dirserver,0, sizeof(dirserver));
    dirserver.sin_family=AF_INET;
    dirserver.sin_port = htons(atoi(argv[2]));
    
    uint32_t ip_servidor;
	if ((ip_servidor = inet_addr(argv[1])) < 0) {
		printf("Error al convertir la IP %s\n", argv[1]);
		exit(1);
	}
    dirserver.sin_addr.s_addr = inet_addr(argv[1]);
    socklen_t tam_dirserver= sizeof(dirserver);
    //creamos el socket
    int sd=socket(PF_INET, SOCK_STREAM,0);
    if (sd<0) {
        perror ("Cliente TCP: socket");
        exit(1);
    }
    //Conectamos con el servidor
    if ((connect(sd,(struct sockaddr *) &dirserver,tam_dirserver))<0){
        perror("Cliente TCP: connect");
        close (sd);
        exit(1);
    }
    
    //Como pueden llegar datos de teclao o del servidor, uso select
    fd_set conjunto, cjtotrabajo;
    FD_ZERO(&conjunto);
    FD_SET(0, &conjunto);
    FD_SET(sd, &conjunto);
    int max = 0 >sd ? 0:sd;
    int fin=0;
    uint8_t tipop, numoperandos =0;
    uint16_t operandos[T];
    char operacion[T];
    char *aux;

    //Bucle de comandos
    while (fin==0){
        printf("Introduzca tipo de operación ('f', sincomillas para terminar)\n");
        memcpy(&cjtotrabajo,&conjunto,sizeof(conjunto));
        int r= select(max+1, &cjtotrabajo, NULL, NULL, NULL);
        if (r<0){
            perror ("Cliente TCP: select");
            close (sd);
            exit(1);
        }
        if (FD_ISSET(0,&cjtotrabajo)){
            if((read(0,&tipop,sizeof(u_int8_t))) <0){
                perror("read tipo");
                close (sd);
                exit(1);
            }
            printf("Numero de operandos: ");
            if((read(0,&numoperandos,sizeof(u_int8_t)))<0){
                perror("read numoperandos");
                close (sd);
                exit(1);
            }
            for(int i=0;i<numoperandos;i++){
                printf("Introduzca operando %d",i+1);
                if((read(0,&operandos[i],sizeof(u_int16_t)))<0){
                    perror("read numoperandos");
                    close (sd);
                    exit(1);
                }
            } //fin del bucle for de operandos
            //Envio petición al servidor
            aux=operacion;
            memcpy(aux,&tipop, sizeof(uint8_t)); //al ser 1 byte no requiere hton
            aux+= sizeof(uint8_t);
            memcpy(aux,&numoperandos, sizeof(uint8_t)); //al ser 1 byte no requiere hton
            aux+= sizeof(uint8_t);
            for(int i=0;i<numoperandos;i++){ //compacta operandos
                operandos[i]=htons(operandos[i]);
                memcpy(aux,&operandos[i], sizeof(uint16_t));
                aux+=sizeof(uint16_t);
            } //fin del bucle for de operandos
            int tam_pack=2*sizeof(uint8_t)+numoperandos*sizeof(uint16_t);
            int escrito=write_n(sd, operacion,tam_pack);
            if (escrito<0){
                perror("write_n");
                close (sd);
                exit(1);
            } else 
                printf("Escritos %d bytes al servidor\n",tam_pack);
            } //fin del if datos en teclado a leer
    
    } //fin bucle de comandos

    return 0;
} //fin del main





// Funciones auxiliares
int write_n(int fd, char *b, int n){
    int aescribir=n;
    int total_escrito=0;
    int escritos;
    do{
        errno=0;
        if ((escritos=write(fd,b+total_escrito,aescribir))>0){
            aescribir-=escritos;
            total_escrito+=escritos;
        }
    } while(((escritos>0)&&(total_escrito<n))||(errno==EINTR));//fin do while
    if (total_escrito==n)
        return total_escrito;
    else
        return escritos;
}