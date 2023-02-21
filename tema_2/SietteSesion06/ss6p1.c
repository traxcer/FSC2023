/*
Escriba una función en C que tome como argumento un nombre de fichero, 
y tres enteros pasados por dirección como argumentos de salida, y cuente 
el número de caracteres en minúscula, en mayúscula y dígitos que aparecen 
en el fichero. La función debe tener el siguiente prototipo:
       void contar(char * filename, int * minusculas, int * mayusculas, int * digitos);
Implemente también el programa main necesario para probar su funcionamiento.
Prueba 1: Compilación
Prueba de funcionamiento
Dada un fichero regular en disco llamado "datos.txt" con el siguiente contenido:
           aAbBcCdD1233.;-fgh
la función tenga devuelva los siguientes valores:
           //llamada a la función
           int minusculas, mayusculas, digitos;
          contar("datos.txt", &minusculas, &mayusculas, &digitos);
           //se espera que minusculas = 7; mayusculas = 4; digitos = 4;
Nótese que este programa no espera salida alguna, y que las comprobaciones se realizarán con llamadas a función correspondiente, que deben tener el prototipo indicado.
*/

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#define T 255
//Prototipos de funciones
void contar(char * filename, int * minusculas, int * mayusculas, int * digitos);

int main(int argc,char * argv[]){
    if (argc<2){
        printf("Atención: Debe usar %s nombfichero\n",argv[0]);
        return 0;
    }
    int minusculas, mayusculas, digitos;
    contar("datos.txt", &minusculas, &mayusculas, &digitos);
    printf("Minusculas: %d\nMayusculas: %d\nDigitos: %d\n", minusculas, mayusculas, digitos);
}
    
void contar(char * filename, int * minusculas, int * mayusculas, int * digitos){
    char buf[T];
    int fd, m=0, M=0, d=0; 
    ssize_t leido;

    if((fd=open(filename,O_RDONLY))<0){
        perror("open");
        exit(-1);
    }
    while((leido = read(fd,buf,T))>0){ //bucle de lectura y contabilizacion
        
        for (int i=0;i<leido;i++){
            if ((buf[i]>='a') && (buf[i]<='z')) m++;
            if ((buf[i]>='A') && (buf[i]<='Z')) M++;
            if ((buf[i]>='0') && (buf[i]<='9')) d++;
        }
    }
    if (leido==-1){ //Error leyendo
            perror("read");
            exit(-1);
        }
    if (close(fd)<0){ // Leido completo, cierra
        perror("close");
        exit(-1);
    }
    *minusculas=m;
    *mayusculas=M;
    *digitos=d;   
}