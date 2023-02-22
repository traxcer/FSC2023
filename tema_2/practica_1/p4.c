/*Implementa una función
int copiaFichero(char *forigen, char *fdestino) 
que copie un fichero a otro, suponiendo que los nombres de ambos se pasan 
como relativos al directorio en el que se ejecuta el programa. Utiliza esta 
función en un programa, llamado cp_fsc, similar al comando “cp” del sistema 
operativo, que se ejecute como:
cp_fsc nombre_forigen nombre_fdestino
*/

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

int main (int argc,char * argv[]){
    int fd_origen, fd_destino, ndatos;
    char buffer[BUFSIZ];
    //Chequeo de argumentos
    if (argc<3){
        fprintf(stderr,"Usar: %s <origen> <destino>\n",argv[0]);
        exit (1);
    }
    //control apertura de ficheros
    if((fd_origen=open(argv[1],O_RDONLY))==-1){
        perror("open");
        exit (-1);
    }
    if((fd_destino=open(argv[2],O_WRONLY|O_TRUNC|O_CREAT,0666))==-1){
        perror("open");
        exit (-1);
    }
    while((ndatos=read(fd_origen,buffer, sizeof (buffer)))>0)
        write(fd_destino,buffer,ndatos);
close (fd_destino);
close (fd_origen);
return 0;
}