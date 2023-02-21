/* GuardaDatos.c

Manuel Eloy Gutiérrez
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define T 255

struct Estructura{
    int a;
    float b;
    char c;
};




int man(int argc, char *argv[]){

    int fd, escritos;
    int b[T];
    
//Control argumentos
    if (argc<2){
        printf("Usar: %s <nombfichero",argv[0]);
        exit (1);
    }

//Datos
    struct Estructura es;
    es.a = 1; es.b = 2.0; es.c = '3';
    int x = 7;
    int array_enteros[4]={0x00, 0x01, 0x02, 0x03};

//Apertura y grabación de datos en el fichero

    fd=open(argv[1],O_CREAT | O_APPEND | O_WRONLY, 644);
    if (fd<0){
        perror("open");
        exit (1);
    }


    return 0;
}

