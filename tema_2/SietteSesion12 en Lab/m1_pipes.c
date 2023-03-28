/* Implementacion de maquina de estado
Ejemplo SEÑALES COMO EVENTOS
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>

/* 1. pongo en constantes los eventos y los estados */

#define E1 0
#define E2 1

#define E_USR1 0
#define E_USR2 1
#define E_INT 2

int p[2];

int espera_Evento();
int read_n(int fd,void *buffer, int tamanio);

/* variable global para almacenar la señal recibida
    se asigna en los manejadose, pero perderiamos señales
    por tanto es mejor usar una pipe para encolar las
    señales (hecho en la version 2)
*/
int evento_recibido;

void m1(int signo){
    int evento=E_USR1;
    if(write(p[1],&evento,sizeof(int))!= sizeof(int)){
        perror("write e_usr1");
        exit(1);
    }
    signal(SIGUSR1,m1);
}
void m2(int signo){
    int evento=E_USR2;
    if(write(p[1],&evento,sizeof(int))!= sizeof(int)){
        perror("write e_usr2");
        exit(1);
    }
    signal(SIGUSR1,m2);
}
void m3(int signo){
    int evento=E_INT;
    if(write(p[1],&evento,sizeof(int))!= sizeof(int)){
        perror("write e_int");
        exit(1);
    }
    evento_recibido=E_INT;
    // signal(SIGUSR1,m1); no es necesario, salimos de la maquina
}

/*=====================================================
  Función principal espera_evento()
  =====================================================*/
int espera_evento(){
    int evento;
    //si no uso read_n voy a tener problemas
    //if (read(p[0],&evento,sizeof(int))!= sizeof(int)){
    if (read_n(p[0],&evento,sizeof(int))!= sizeof(int)){ //lee evento de la pipe
        perror("read evento");
        exit(1);
    }
    return evento;
}

int main(int argc, char *argv[]){
    /* Armo los manejadores de las señales a controlar */
    if (signal(SIGUSR1,m1)  == SIG_ERR){
        perror("signal m1");
        exit(1);
    }
    if (signal(SIGUSR2,m2)  == SIG_ERR){
        perror("signal m2");
        exit(1);
    }
    if (signal(SIGINT, m3)  == SIG_ERR){
        perror("signal m3");
        exit(1);
    }

    /* Creo una tuberia para encolar la señales, uso p global*/
    if (pipe(p)<0){
        perror("pipe");
        exit(1);
    }

    printf("PID (%d) de mi programa\n",getpid());

    int estado=E1; //Estado inicial
    int fin=0; //condición del buble principal

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
                        printf("Saliendo de la Maquina\n");
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
                    printf("Transito de E1 a E2\n");
                        estado=E2;
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

/*===============================================================================*/
/* Funciones auxiliares                                                          */
/*===============================================================================*/
int read_n(int fd,void *buffer, int tamanio){

    int a_leer=tamanio;
    int t_leido=0;
    int leido;

    do{
    errno=0;
    leido=read(fd, buffer+t_leido, a_leer);
        if (leido>=0){
            a_leer-=leido;
            t_leido+=leido;
        }
    } while (((leido>0) && (t_leido<tamanio)) || errno == EINTR);

    if (t_leido>0){
        return t_leido;
    } else {
        return leido;
    }
}