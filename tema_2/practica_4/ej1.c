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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/select.h>

/* Prototipos */
int max(fd,maxfds);
void copia_fdset(fd_set *dst, fd_set *origen, int maxfd_mas_uno);
/* Funciones auxilares */
int max(fd,maxfds){
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
    int fd;
    if ((fd=open("tmp/fsc_chat",O_RDONLY))<0){
        perror("open fifo");
        exit(-1);
    }
    fd_set rfds,activo_rdfs;
    struct timeval tv;
    int retorno;

    /* Mira stdin (fd 0) para ver si hay datos del teclado. */
    FD_ZERO(&rfds);
    FD_SET(0, &rfds);
    int maxfds=0; //Descriptor ya abierto de lectura teclado
    FD_SET(fd, &rfds);
    maxfds=max(fd,maxfds);
    /* Espera hasta 3 segundos.*/
    tv.tv_sec = 3;
    tv.tv_usec = 0;
    while(1){
    copia_fdset(&activo_rdfs,&rfds,maxfds+1);
    retorno = select(maxfds+1, &activo_rdfs, NULL, NULL, &tv);
        if (retorno == -1){
            perror("select()");
            if (close(fd)<0){
                perror("close");
                exit(-1);
            }
            exit(-1);
        }
        const int MAXBUFFSIZE = 2048;
        char buffer[MAXBUFFSIZE];
        if(FD_ISSET(fd, &active_rfds)) {
            int leido=read(fd, buffer, MAXBUFFSIZE);
            write(1,buffer,leido);
        else
            printf(".");

        exit(1);
    } //Fin buble principal while
    return 0;
}





