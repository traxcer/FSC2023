/* Ejemplo 3 de Maquinas de Estado
señales y temporizadores

Implementado por Manuel Eloy Gutiérrez
Málaga a 28 de Marzo del 2023
*/

#include <stdio.h>
#include <signal.h>

/* Defino las constantes para los estados*/
#define E1 0
#define E2 1
#define E3 2

/* Defino las constantes para los eventos*/
#define EV1 0 /*SIGUSR1*/
#define EV2 0 /*SIGUSR2*/
#define EV3 0 /*TIMEOUT*/

int p[2]; /*Una pipe global para encolar los eventos*/

/*manejadores de las señales*/
int m1(int signo){

}

main(){
    int estado=E1; /* Estado inicial */
    int fin=0;
    /*preparo la pipe para el encolado de señales */
    if (pipe(p)<0){
        perror("pipe");
        exit(1);
    }
    /*preparo los manejadores para que capturen las señales*/
    if (signal(SIGUSR1,m1)<0){
        perror("signal SIGUSR1");
        exit(1);
    }
    if (signal(SIGUSR2,m2)<0){
        perror("signal SIGUSR1");
        exit(1);
    }
    if (signal(SIGALRM,m3)<0){
        perror("signal SIGUSR1");
        exit(1);
    }

    
    while (fin==0){ /* Bucle Principal */




    }



return 0;
}