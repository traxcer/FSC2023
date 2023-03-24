/*
Implemente un programa que genere un proceso hijo encargado de mostrar por 
pantalla la información que le suministra su padre. El padre obtiene la información 
del usuario por teclado. Los procesos terminan cuando se introduce por teclado la 
cadena "fin\n" (es decir, "fin" más la pulsación de la tecla INTRO). Utilice una 
pipe para el trasiego de información entre padre e hijo. 
*/

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

#define T 50

int main(){
    /* Preparo pipe para canalizar la comunicación PADRE->HIJO */
    int p[2];
    if ((pipe(p))<0){
        perror("pipe");
        exit(1);
    }

    pid_t pid;
    if((pid=fork())<0){
        perror("fork");
        exit(1);
    } else if (pid==0){ /* Proceso HIJO*/
    if((close (p[1]))<0){
        perror("Close Escritura Hijo");
        exit(1);
    }
    char mensaje[T];
    int leidos;
    leidos=read(p[0],mensaje,T);
    do{
    //write (1,"Mensaje leido (HIJO):\n",23);
    write(1,mensaje,leidos);
    } while ((leidos=read(p[0],mensaje,T))>0);
    if (leidos<0){
        perror("leidos");
        exit(1);
    }
    exit(1); /* Termina el hijo */
    
    } else { /* Proceso PADRE */
    if((close (p[0]))<0){
        perror("Close Lectura Padre");
        exit(1);
    }
    char mensaje[T];
    int teclado;
        do{
            //write (1,"Introduce un Mensaje (PADRE):\n",31);
            if((teclado=read(0,&mensaje,T))<0){
                perror("read teclado");
                exit(1);
            }
        /* Escribe en tuberia el MSG*/
        if ((write(p[1],mensaje,teclado))<0){
            perror("write del padre en pipe");
            exit(1);
        }
        mensaje[teclado-1]='\0';
        } while (strcmp(mensaje,"fin")!=0);

        if((close (p[1]))<0){
            perror("Close Escritura del Padre");
            exit(1);
        }   
    wait(NULL); /* Espera al hijo */
    }

}