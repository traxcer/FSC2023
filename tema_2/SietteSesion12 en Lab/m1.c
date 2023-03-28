/* Implementacion de maquina de estado
Ejemplo SEÑALES COMO EVENTOS
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

/* 1. pongo en constantes los eventos y los estados */

#define E1 0
#define E2 1

#define E_USR1 0
#define E_USR2 1
#define E_INT 2

int espera_Evento();

/* variable global para almacenar la señal recibida
    se asigna en los manejadose, pero perderiamos señales
    por tanto es mejor usar una pipe para encolar las
    señales 
*/
int evento_recibido;

void m1(int signo){
    evento_recibido=E_USR1;
    signal(SIGUSR1,m1);
}
void m2(int signo){
    evento_recibido=E_USR2;
    signal(SIGUSR1,m2);
}
void m3(int signo){
    evento_recibido=E_INT;
    // signal(SIGUSR1,m1); no es necesario, salimos de la maquina
}

int espera_evento(){
    pause();
    return evento_recibido;
}

int main(int argc, char *argv[]){

    if (signal(SIGUSR1,m1)  ==SIG_ERR){
        perror("signal m1");
        exit(1);
    }
    if (signal(SIGUSR2,m2)  ==SIG_ERR){
        perror("signal m2");
        exit(1);
    }
    if (signal(SIGINT,m3)  ==SIG_ERR){
        perror("signal m2");
        exit(1);
    }

    printf("PID:%d\n",getpid());

    int estado=E1;
    int fin=0;

    while(fin==0){
        printf("Esperando evento en estado %d\n", estado);
        int evento= espera_evento();
        printf("Evento: %d\n", evento);

        switch (estado){
            case E1:
                switch (evento){ //switch anidado, como salen 3 flechas de E1 necesito 3 case
                    case E_USR1:
                    break;
                    case E_USR2:
                        printf("Transito de E1 a E2\n");
                        estado=E2;
                    break;
                    case E_INT:
                        printf("Saliendo de la MAquina\n");
                        fin=1;
                    break;
                    default:
                        printf("Estado no esperado\n");
                        fin=1;
                    break;
                }        
            break;

            case E2:
            switch (evento){ //switch anidado, como salen 3 flechas de E1 necesito 3 case
                    case E_USR1:
                        printf("Transito de E2 a E1\n");
                        estado=E1;
                    break;
                    case E_USR2:
                    break;
                    case E_INT:
                        printf("Saliendo de la Maquina");
                        fin=1;
                    break;
                    default:
                        printf("Estado no esperado\n");
                        fin=1;
                    break;
                }        
            break;
            default:
                printf("Estado no esperado\n");
                fin=1;
            break;
        }
    }

    return 0;
}