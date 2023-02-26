/*
Codifica un programa ls_fsc que acepte la opción -l y que muestre el directorio en curso empleando 
el ejecutable original ls del sistema y la función system(). 
(Consulta el manual para el uso de la función system()).
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define T 50

int main (int argc, char *argv[]){
    if (argc<2){
        printf("uso: %s -ls, para mostrar el directorio en curso\n",argv[0]);
        exit (1);
    }
    char comando[T]="ls ";
    strcat (comando, argv[1]);
    
    system(comando);

    return 0;
}