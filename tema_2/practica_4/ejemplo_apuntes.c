/*
    Ejemplo de select() de los apuntes
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/select.h>

#define T 255

void copia_fdset (fd_set *dst, fd_set *src, int maximo_mas_uno);

/* Funciones auxilares*/
void copia_fdset (fd_set *dst, fd_set *src, int maximo_mas_uno){
    for(int i=0;i<maximo_mas_uno;i++){
        if (FD_ISSET(i,src))
            FD_SET(i,dst);
    }
}


int main(){

int f1=open("fifo1",O_RDONLY);
int f2=open("fifo2",O_RDONLY);

fd_set rfd_a, rfd;

FD_ZERO(&rfd);
FD_SET(f1, &rfd);
FD_SET(f2, &rfd);

int maximo = f1 > f2 ? f1:f2;
struct timeval t,t_a;
t.tv_sec=2;
t.tv_usec=750000;
int r;
char b[T];
int leidos;

while(1){
copia_fdset (&rfd_a, &rfd, maximo+1);
t_a=t; //Ajusta el timer
if((r= select(maximo+1,&rfd_a, NULL, NULL, &t_a))<0){
    perror("select");
    close (f2); close (f1);exit(-1);
}
if (r==0){
    // se realiza la accion requerida, read bloquearia
}
if(FD_ISSET(f1,&rfd_a)){
    leidos = read(f1,b,T);
}
if(FD_ISSET(f2,&rfd_a)){
    leidos = read(f2,b,T);
}
write(1,b,leidos);
}
return 0;
} //Cierra main