/* Ejemplo 3 de Maquinas de Estado
señales y temporizadores

Implementado por Manuel Eloy Gutiérrez
Málaga a 28 de Marzo del 2023
*/

#include <stdio.h>
#include <signal.h>
#include <errno.h>

/* Defino las constantes para los estados*/
#define E1 0
#define E2 1
#define E3 2

/* Defino las constantes para los eventos*/
#define EV1 0 /*SIGUSR1*/
#define EV2 1 /*SIGUSR2*/
#define EV3 2 /*TIMEOUT*/

//Prototipos
int read_n(int fd,char *b,int n);


int p[2]; /*Una pipe global para encolar los eventos*/
char *strestados[]={"E1","E2","E3"};
int evento_recibido;

/*manejadores de las señales*/
int m1(int signo){

}
int m2(int signo){

}
int temporizador(int signo){

}

void espera_evento(){
int leidos=read_n(p[0],&evento_recibido, sizeof(int)); //Leo evento
}

int main(){
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
        exit(-1);
    }
    if (signal(SIGUSR2,m2)<0){
        perror("signal SIGUSR2");
        exit(-1);
    }
    if (signal(SIGALRM,temporizador)<0){
        perror("signal SIGALRM");
        exit(-1);
    }

    
    while (fin==0){ /* Bucle Principal */
        espera_evento();



    }



return 0;
}


/* Funciones accesorias */
int read_n(int fd,char *b,int n){
    int a_leer=n;
    int total_leido=0;
    int leidos;
    do{
        errno=0;
        leidos=read(fd,b+total_leido,a_leer);
        if (leidos>0){
            total_leido += leidos;
            a_leer -= leidos;
        }
    } while (((leidos>0)&&(total_leido<n))||(errno=EINTR));
    if (leidos<0)
        return leidos;
    else
        return total_leido;
}