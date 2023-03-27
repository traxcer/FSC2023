/*
Elabore un programa donde un padre le transmita el contenido de un 
fichero de texto a un hijo utilizando una pipe. La misión del hijo 
es recibir los datos de ese fichero y convertir todas sus mayúsculas
a minúsculas para grabarlo con el nombre que decida el padre 
(NOTA: mira la función tolower()). 
El hijo recibirá del padre el nombre del fichero donde se grabarán 
los datos, porque el padre primero llama a fork() y luego pide el 
nombre del fichero de entrada y el de salida al usuario por teclado. 
El fichero de entrada lo abre el padre. El de salida lo abre el hijo 
cuando el padre le pasa el nombre a través de la pipe.
*/

#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <wait.h>

#define T 25
#define buffer 255

int read_n(int fd, void *b, int n);

int main(){

int p[2];
if (pipe(p)<0){
    perror("pipe");
    exit(1);
}

pid_t pid;
pid=fork();
//=====================================================================
if (pid==0){ //Hijo
    int leidos, tamanio;
    char destino[T];
    if (close(p[1])<0){
        perror("(HIJO) Close de Escritura");
        exit(1);
    }
    if ((leidos=read_n(p[0],&tamanio,sizeof(int)))<0){
        perror ("read_n tamanio");
        exit(1);
    }
    if ((leidos=read_n(p[0],destino,tamanio))<0){ //leo de fifo
        perror ("read_n destino");
        exit(1);
    }
    printf("(HIJO) lee (%d) contenido (%s)\n",tamanio,destino);
    exit(0);
//=====================================================================
} else if (pid <0){ //Error
    perror("fork");
    exit(1);
} else { //Padre
//=====================================================================
    int tecleado;
    //char mensaje[buffer];
    char origen[T];
    char destino [T];
    //int fds;
    if (close(p[0])<0){
        perror("(PADRE) Close de Lectura");
        exit(1);
    }
    printf("(PADRE) Introduzca el nombre del fichero de origen: ");
    if((tecleado=read(1,origen,25))<0){
        perror("(PADRE) read nombre origen");
        exit(1);
    }
    origen[tecleado]='\0';

    printf("(PADRE) Introduzca el nombre del fichero de destino: ");
    if((tecleado=read(1,destino,25))<0){
        perror("(PADRE) read nombre destino");
        exit(1);
    }
    destino[tecleado]='\0';
    write(p[1],&tecleado,sizeof(tecleado));
    write(p[1],destino,tecleado); //EScribo en fifo destino
    //fds=open(origen,O_RDONLY); //Abrimos origen en el padre
    //close (fds)
    wait(NULL);
}
//=====================================================================
return 0;
}





/* Funciones auxilares */
int read_n(int fd, void *b, int n){
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
