/* 
Implementa la función int listdir(char *d) que	muestre	un directorio empleando	la función estadof del ejercicio anterior.

• Implementa la función usando funciones de la biblioteca de C (es decir, usando opendir y readdir).

Programador: Manuel Eloy Gutiérrez 
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

//Prototipos
int estadof(char *f);
int listdir(char *directorio);

int listdir(char *directorio){
    DIR *dir = opendir(directorio);
    if (!dir){
        printf("error al inentar abrir el directorio\n");
        return -1;
    }
    struct dirent *entry;
    printf("Contenidos del directorio;\n");
    while ((entry=readdir(dir)) != NULL){
        printf("Nombre de fichero: %s\n", entry->d_name);
        estadof(entry->d_name);
        printf("\n\n");
    }
    if (closedir(dir)==-1){
        printf("problema al cerrar el directorio\n");
        return -1;
    }
    return 0;
}

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
    printf("Información del directorio\n");
    printf("Directorio a analizar: %s\n", argv[1]);
    listdir(argv[1]);
    return 0;
}