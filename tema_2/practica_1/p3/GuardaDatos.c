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
    ssize_t fd;
    int escritos, tamanio;
    char buffer[T];
    
//Control argumentos
    if (argc<2){
        printf("Usar: %s <nombfichero\n",argv[0]);
        exit (1);
    }

//Datos
    int x = 7;
    int array_enteros[4]={0x00, 0x01, 0x02, 0x03};
    struct Estructura est;
    est.a = 1; est.b = 2.0; est.c = '3';

//Apertura, si o existe lo crea y si existe lo trunca, y preprepara para escribir
    fd=open(argv[1],O_CREAT | O_TRUNC | O_WRONLY, 0644);
    if (fd<0){
        perror("open");
        exit (1);
    }

//Genero un buffer, compacto, para escribir en el fichero.
    //Grabo el contenido de memoria ocupada por cada dato
    memcpy(buffer,&x,sizeof(int));
    tamanio=sizeof(int); //para acumular el tamaño de lo usado en el buffer
    memcpy(buffer+tamanio,array_enteros,sizeof(int)*4);
    tamanio+=(sizeof(int)*4);  
    memcpy(buffer+tamanio,&est.a,sizeof(est.a));
    tamanio+=sizeof(est.a);    
    memcpy(buffer+tamanio,&est.b,sizeof(est.b));
    tamanio+=sizeof(est.b);  
    memcpy(buffer+tamanio,&est.c,sizeof(est.c));
    tamanio+=sizeof(est.c);

//grabacion
    if((escritos=write(fd,buffer,tamanio))<0){
        perror("write");
        exit (1);
    }
    if(close(fd)<0){
        perror("close");
        exit(1);
    }
    printf("Grabados correctamente (bytes): %d\n",tamanio);

return 0;
}

