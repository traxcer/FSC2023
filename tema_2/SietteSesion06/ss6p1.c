/*
Descripción:
Función que toma como argumento un nombre de fichero, y tres 
enteros pasados por dirección como argumentos de salida, y 
cuente el número de caracteres en minusculas, mayúsculas y 
digitos que aparecden en el fichero.

Programador Manuel Eloy Gutiérrez
Session 6 Programa 1
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdlib.h>

#define T 255

void contar(char *filename, int *minusculas, int *mayusculas, int *digitos);


int main (int argc, char *argv[]){
    if (argc<2){
        printf("Uso: %s <filename>", argv[1]);
        return 0;
    }

int *mayusculas, *minusculas, *digitos;
//Inicia contadores
*minusculas=0;
*mayusculas=0;
*digitos=0;

    return (0);
}

void contar(char *filename, int *minusculas, int *mayusculas, int *digitos){
    ssize_t fd, leidos;
    char b[T];

    
    if((fd=open(filename,O_RDONLY))==-1){
        perror("open:");
        exit (1);
    }
    leidos=read(fd,b,T);
    while (leidos>0){
        for (int i=0;i<leidos;i++){
            if (b[i]>='a' && b[i]<='z')
                *minusculas++;
            if (b[i]>='A' && b[i]<='Z')
                *mayusculas++;
            if (b[i]>='0' && b[i]<='9')
                *digitos++;
        }
        leidos=read(fd,b,T);
    }
    if (leidos<0){ //Error de llamada al sistema
        perror("read:");
        exit (1);
    }
}
