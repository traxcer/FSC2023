/* GuardaDatos.c

Manuel Eloy Gutiérrez
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define T 255

struct Estructura{
    int a;
    float b;
    char c;
};

int main(int argc, char *argv[]){

    int fd, escritos, tamanio;
    char buffer[T];
    
//Control argumentos
    if (argc<2){
        printf("Usar: %s <nombfichero\n",argv[0]);
        exit (1);
    }

//Datos
    int x = 7;
    int array_enteros[4]={0x00, 0x01, 0x02, 0x03};
    struct Estructura es;
    es.a = 1; es.b = 2.0; es.c = '3';

//Apertura y grabación de datos en el fichero

    fd=open(argv[1],O_CREAT | O_TRUNC | O_WRONLY, 0644);
    if (fd<0){
        perror("open");
        exit (1);
    }
//Genero un buffer b, compacto.
    memcpy(buffer,&x,sizeof(int));
    tamanio=sizeof(int);
    buffer[tamanio]='\0';
    printf("buffer: %s, x ocupa(%d)..acumulado:%d\n",buffer,(int)sizeof(int),tamanio);

    memcpy(buffer+sizeof(int),array_enteros,sizeof(int)*4);
    tamanio+=(4*sizeof(int));
    buffer[tamanio]='\0';
    printf("buffer: %s, array_enteros ocupa(%d)..acumulado:%d\n",buffer,(int)sizeof(int)*4,tamanio);
    
    memcpy(buffer+sizeof(int)+sizeof(int)*4,&es.a,(int)sizeof(es.a));
    tamanio+=(int)sizeof(es.a);
    printf("es.a ocupa(%ld)..acumulado:%d\n",sizeof(es.a),tamanio);
    
    memcpy(buffer+sizeof(int)+sizeof(int)*4+sizeof(es.a),&es.b,sizeof(es.b));
    tamanio+=sizeof(es.b);
    printf("es.b ocupa(%ld)..acumulado:%d\n",sizeof(es.b),tamanio);
    
    memcpy(buffer+sizeof(int)+sizeof(int)*4+sizeof(es.a+sizeof(es.b)),&es.c,sizeof(es.c));
    tamanio+=sizeof(es.c);
    printf("es.a ocupa(%ld)..acumulado:%d\n",sizeof(es.c),tamanio);

//grabacion
    if((escritos=write(fd,buffer,tamanio))<0){
        perror("write");
        exit (1);
    }
    if(close(fd)<0){
        perror("close");
        exit(1);
    }
    buffer[tamanio]='\0';
    printf("grabado en formato char : %s\n",buffer);
    printf("Grabados correctamente (bytess): %d\n",tamanio);
    

    return 0;
}

