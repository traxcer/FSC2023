/*
Basándose en el ejercicio 2, consiga la misma funcionalidad a 
través de dos programas independientes, que se ejecutarán a la 
vez en el sistema. En esta ocasión, hay que sustituir las pipes 
por fifos. 
Uno de los programas es el que hace la lectura por teclado y el 
envío a la fifo. 
El otro recibe por la fifo y muestra por pantalla. 
El nombre de la fifo puede obtenerse como argumento a los programas. 
Crea la fifo por consola, previamente.*/

#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define T 8

int main(int argc, char *argv[]){
ssize_t fd;
int tecleado;
char mensaje[T];

if (argc<2){
    printf("Usar: %s <nombfifo>\n",argv[0]);
}

    if ((fd=open(argv[1], O_WRONLY))< 0){ //abro fifo escritura
        perror("open fifo");
        exit(1);
    }
do{
    //lee del teclado
    if((tecleado=read(0,&mensaje,T))<0){
        perror("read teclado");
        exit(1);
    }
    /* Escribe en la pipe mensaje*/
    if ((write(fd,mensaje,tecleado))<0){
            perror("write en pipe");
            exit(1);
        }
    mensaje[tecleado-1]='\0';
} while (strcmp(mensaje,"fin")!=0);

return 0;
}





/* Funciones auxilares */
ssize_t read_n(int fd, void *b, size_t n){
int a_leer=n;
int t_leido=0;
int leido;

do{
    errno=0;
    leido = read (fd,b,a_leer);
    if (leido >=0){
        a_leer-=leido;
        t_leido+=leido;
    }
} while (((leido > 0)&&(t_leido < n))|| (errno==EINTR));
if (leido < 0)
    return leido;
else
    return t_leido;
}
