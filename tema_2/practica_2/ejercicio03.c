#define _POSIX_SOURCE

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

/* Ejercicio 3

Implementa un programa inmune al Ctrl-C durante los segundos que se 
introduzcan como argumento. 
Transcurridos éstos, se volverá a habilitar la posibilidad de que se 
cierre el programa con Ctrl-C.
*/

void m (int signo);
void alarma (int signo);

void m (int signo){
    write(1,"\nSe está IGNORANDO CTRL+C\n",27);
    if((signal(SIGINT,m))<0){
        perror ("signal");
        exit (1);
    }
}

void alarma (int signo){
    write(1,"\nOLE!: Ya se puede usar CTRL+C\n",32);
    if((signal(SIGINT,SIG_DFL))<0){
        perror ("signal");
        exit (1);
    }

}

int main(int argc, char *argv[]){
    if (argc<2){
        printf("Uso: %s <segundos>\n", argv[0]);
        exit (1);
    }
    char msg[50];
    sprintf(msg,"Se va a IGNORAR CTRL+C por %s segundos\n", argv[1]);
    write(1,msg,strlen(msg));

    if ((signal (SIGINT, m))<0) //Ignoramos ya CTRL+C para que no se cuele
    {
        perror ("signal");
        exit (1);
    }

    if((signal (SIGALRM,alarma))<0) //armo la alarma
    {
        perror ("signal");
        exit (1);
    }
    
    if((alarm (atoi(argv[1])))<0) //activamos la alarma
    {
        perror ("signal");
        exit (1);
    }
    
    while (1){ //Bucle infinito
        pause(); //bloqueo, espero señal, para no acaparar la CPU
    }

}