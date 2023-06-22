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
    dirserver.sin_addr.s_addr = ip_servidor;
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
    
    //Como pueden llegar datos de teclado o del servidor, uso select
    fd_set conjunto, cjtotrabajo;
    FD_ZERO(&conjunto);
    FD_SET(0, &conjunto);
    FD_SET(sd, &conjunto);
    int max = 0 >sd ? 0:sd;
    int fin=0;
    uint8_t tipop;    
    char nop;
    uint8_t numoperandos;
    uint16_t operandos[T];
    char operacion[T];
    char *aux;
    int escrito;
    struct respuestas{
        uint8_t tipo_de_respuesta;
        int32_t resultado;
    }respuesta;

    //Bucle de comandos
    while (fin==0){
        printf("Introduzca tipo de operación (PID=%d) ('f', sincomillas para terminar)\n",getpid());
        memcpy(&cjtotrabajo,&conjunto,sizeof(conjunto));
        int r= select(max+1, &cjtotrabajo, NULL, NULL, NULL);
        if (r<0){
            perror ("Cliente TCP: select");
            close (sd);
            exit(1);
        }
        if (FD_ISSET(0,&cjtotrabajo)){
            write(1,"Leyendo de teclado el tipo de operación (s/m), ",47);
            if((read(0,&tipop,2)) <0){
                perror("read tipo");
                close (sd);
                exit(1);
            }
            printf("Leido (tipop): %c\n",tipop);
            if(tipop=='f'){ //comprueba si lo primero se que ha leido del teclado es una f
                close(sd);
                exit(1);
            }
            //ya tengo el tipo de operación
            write(1,"Numero de operandos: ",21);
            if((read(0,&nop,2))<0){
                perror("read numoperandos");
                close (sd);
                exit(1);
            }
            numoperandos=atoi(&nop);
            printf("Leido (numop): %d\n",numoperandos);
            for(int i=0;i<numoperandos;i++){
                printf("Introduzca operando (%d):",i+1);
                if((read(0,&operandos[i],sizeof(uint16_t)+1))<0){
                    perror("read numoperandos");
                    close (sd);
                    exit(1);
                }
            } //fin del bucle for de operandos
        
            //Envio petición al servidor
            aux=operacion;
            memcpy(aux,&tipop, sizeof(uint8_t)); 
            aux+= sizeof(uint8_t);
            memcpy(aux,&numoperandos, sizeof(uint8_t));
            aux+= sizeof(uint8_t);
            for(int i=0;i<numoperandos;i++){ //compacta operandos
                memcpy(aux,&operandos[i], sizeof(uint16_t));
                aux+=sizeof(uint16_t);
            } //fin del bucle for de operandos

            //envio el tamaño
            uint16_t tam_pack=2*sizeof(uint8_t)+numoperandos*sizeof(uint16_t);
            uint16_t ntam_pack=htons(tam_pack);
            escrito=write_n(sd, (char*) &ntam_pack, sizeof(uint16_t));
            
            //envio la operación
            escrito=write_n(sd, operacion,tam_pack);
            if (escrito<0){
                perror("write_n");
                close (sd);
                exit(1);
            } else 
                printf("Escritos %d bytes al servidor\n",escrito);
            } //fin del if datos en teclado a leer
 
            if (FD_ISSET(sd,&cjtotrabajo)){
                printf("He recibido algo del servidor\n");
                read(sd,&respuesta.tipo_de_respuesta,sizeof(respuesta.tipo_de_respuesta));
                read(sd,&respuesta.resultado,sizeof(respuesta.resultado));
                respuesta.resultado=ntohl(respuesta.resultado);
                if (respuesta.tipo_de_respuesta==1){
                    printf("Recibida respuesta del servidor\n");
                    printf("Tipo: %d\n",respuesta.tipo_de_respuesta);
                    printf("Resultado: %d", respuesta.resultado);
                }
                if (respuesta.tipo_de_respuesta==0){
                    printf("Operación no soportada por el servidor\n");
                }
            } //sin de FD_ISSET sd
    
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