#include <stdio.h>
#include <signal.h>
#include <unistd.h>


/* Constantes: Estados y Eventos */

#define E1 0
#define E2 1
#define E3 2

#define EV_SIGUSR1 0
#define EV_SIGUSR2 1

int evento_recibido; //Variable global

/* Funciones Accesorias */
void m1(int signo);
void m1(int signo);
void espera_evento();

void espera_evento(){
    printf("Esperando evento...\n");
    pause();
}

void m1(int signo){
    signal (SIGUSR1, m1);
    evento_recibido=EV_SIGUSR1;
    write(1,"Evento SIGUSR1 recibido\n",24);
}
void m2(int signo){
    signal (SIGUSR2, m2);
    write(1,"Evento SIGUSR2 recibido\n",24);
    evento_recibido=EV_SIGUSR2;
    
}

int main(){
    char *strestado[]={"E1","E2","E3"};
    int fin=0;
    int estado=E1;
    signal (SIGUSR1, m1);
    signal (SIGUSR2, m2);
    int contador=0;
    printf("PID Programa (%d)\n",getpid());
    printf("Estado Inicial: %s\n",strestado[estado]);

    while (fin==0){ //Bucle principal
        printf("Estado: %s\n",strestado[estado]);
        espera_evento();
            switch (estado)
            {
            case E1:
                switch (evento_recibido)
                {
                    case EV_SIGUSR1:                        
                        printf("Estado: %s", strestado[estado]);
                        estado=E1;
                        printf("-> Estado %s\n",strestado[estado]);
                    break;
                    
                    case EV_SIGUSR2:
                        estado=E2;
                        printf("Transición de Estado %s -> a Estado %s\n",strestado[E1],strestado[estado]);
                    break;
                
                default:
                    break;
                }
                break;
            case E2:
                switch (evento_recibido)
                {
                    case EV_SIGUSR1:
                        estado=E1;
                        contador=0;
                        printf("Transición de Estado %s -> a Estado %s (Contador:%d)\n",strestado[E2],strestado[estado],contador);
                    break;
                    
                    case EV_SIGUSR2:
                        estado=E2;
                        contador++;
                        printf("Transición de Estado %s -> a Estado %s (Contador:%d)\n",strestado[E2],strestado[estado],contador);
                        if (contador>2){
                            contador=0;
                            estado=E3;
                        }
                    break;
                
                default:
                    break;
                }
                break;
            case E3:
                switch (evento_recibido)
                {
                    case EV_SIGUSR1:
                        estado=E1;
                        printf("Transición de Estado %s -> a Estado %s\n",strestado[E3],strestado[estado]);
                    break;
                    
                    case EV_SIGUSR2:
                        estado=E1;
                        printf("Transición de Estado %s -> a Estado %s\n",strestado[E3],strestado[estado]);
                    break;               
                default:
                    break;
                }
                break;

            default:
                break;
            }


    }
    return 0;
}