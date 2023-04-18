/* Pŕactica 4 : Select 

Programa de Chat, atento a:
-mensajes que un usuario quiere mandar por teclado
-recepción de mensajes de otros usuarios (a través de fifo)
 fifo: "/tmp/fsc_chat"

En principio sólo recepción

Programador: Manuel Elou Gutiérrez
Málaga a 12 de Abril del 2023
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/select.h>

#define MAXBUFFSIZE 2048

/* Prototipos */
int max(int fd,int maxfds);
void copia_fdset(fd_set *dst, fd_set *origen, int maxfd_mas_uno);
/* Funciones auxilares */
int max(int fd,int maxfds){
    if (fd>maxfds)
        return fd;
    else
        return maxfds;
}
void copia_fdset(fd_set *dst, fd_set *origen, int maxfd_mas_uno){
    FD_ZERO (dst);
    for (int i=0;i <maxfd_mas_uno;i++){
        if(FD_ISSET(i,origen))
            FD_SET(i,dst);
        }
}

int main(){
    printf("Comenzamos...\n");
    char buffer[MAXBUFFSIZE];
    int fd;
    if ((fd=open("/tmp/fsc_chat",O_RDONLY))<0){
        perror("open fifo");
        exit(-1);
    }
    
    fd_set rfds,activo_rfds;
    int r;
    struct timeval at,t;

    FD_ZERO(&rfds); //limpia, pone todo a 0
    FD_SET(0,&rfds);
    FD_SET(fd, &rfds);
    int maximo = 0 > fd ? 0:fd;
    
    printf ("maxfds: %d\n",maximo);
    while(1){
        t.tv_sec=3; //Ajusta el timer
        t.tv_usec=0;
        at=t;
        copia_fdset (&activo_rfds, &rfds, maximo+1);
        if((r= select(maximo+1,&activo_rfds, NULL, NULL, &at))<0){
            perror("select");
            close (fd); exit(-1);
        }
        at=t;
        if(FD_ISSET(fd, &activo_rfds)) {
            maximo = 0 > fd ? 0:fd;
            int leido=read(fd, buffer, MAXBUFFSIZE);
            write(1,"\nEn fifo: ",10);
            write(1,buffer,leido);
            } else 
            write(1,".",1);

    } //Fin buble principal while
    return 0;
}
