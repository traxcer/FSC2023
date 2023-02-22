/*
PRUEBA del uso de STAT

Implementa una función int estadof(char* f) que muestra por	pantalla la	información	disponible	de	un fichero a partir	de su nombre. 
La información debe incluir si el fichero es un fichero	regular	o es un directorio, así	como su tamaño en bytes.	
La función devuelve	0 en caso de éxito,	y -1 en caso de	que	haya algún error.
• Notas
       i. Utiliza la llamada al	sistema	stat(...).
      ii. Las macros para consultar	si un fichero es regular o es un directorio, a partir del struct stat {…} son	
          S_ISDIR() y S_ISREG(), y	tienen como	argumento el campo st_mode del struct stat.

Programador: Eloy Gutiérrez
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

int estadof(char *f);

int estadof(char *fichero){
    struct stat estado;
    if( (stat(fichero, &estado))<0 ){
        perror("stat");
        exit(1);
    }
    if (S_ISDIR(estado.st_mode))
        printf("Es un directorio\n");
    if (S_ISREG(estado.st_mode))
        printf("Es un fichero regular\n");
    
    printf("ID del Dispositivo: %i\n",estado.st_dev);
    printf("ID del Usuario Propietario: %i\n",estado.st_uid);
    printf("Tamaño del fichero en Bytes: %lli\n",estado.st_size);
    return 0;
}


int main(int argc, char* argv[]){
    if (argc < 2){
        printf("Usar: %s nombre_fichero\n", argv[0]);
        exit (1);
    }
    printf("Información del fichero con stat()\n");
    printf("Fichero a analizar: %s\n", argv[1]);
    estadof(argv[1]);    
    
    return 0;
}