/*
Implemente un programa C con tres procesos que ejecutan concurrentemente: 
un padre y dos hijos de forma indefinida. 
El proceso hijo1, cada 2 segundos, hace dos cosas: 
envía la señal SIGUSR1 al proceso padre y muestra la cadena Soy el 
proceso hijo1 con pid = 7763: papá, despierta a mi hermano. 

El proceso padre atiende SIGUSR1 enviando esta señal al hijo2 e imprimiendo 
por pantalla: Soy el proceso padre con pid = 7760: ya voy!. 

El proceso hijo2, al recibir la señal, muestra la cadena: 
Soy el proceso hijo2 con pid = 7764: ya estoy despierto. 
Los pid mostrados en el ejemplo son arbitrarios, y se deben obtener usando 
la función getpid().

Nota 1: por razones "ajenas a mi voluntad", las pruebas no funcionan cuando 
se usa printf(). Para que se pasen correctamente, hay que usar la función 
write() para escribir por pantalla. Así, el equivalente de

printf("Soy proceso hijo1 con PID=%d: papá, despierta a mi hermano\n",getpid());
sería:

#define T 256
char msg[T];
sprintf(msg,"Soy proceso hijo1 con PID=%d: papá, despierta a mi hermano\n",getpid());
write(1,msg,strlen(msg));

Nota 2: El uso de la función kill(pid_t pid, int signo) saca un warning, 
independientemente de las librerías incluidas. 
Hay dos formas de solucionarlo: compilar sin -std=c99, que es lo que se hace 
en Siette, o bien incluir el siguiente #define _POSIX_SOURCE al principio del 
código.
*/

#define _POSIX_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <signal.h>
#include <unistd.h>

#define T 256

int pid_hijo1;
int pid_hijo2;
int pid_padre;

void manejador_hijo2SIGUSR1(int sig){
    signal(sig,manejador_hijo2SIGUSR1);
    char msg[T];    
    sprintf(msg,"Soy el proceso hijo2 con pid = %d: ya estoy despierto.\n",getpid());
    write(1,msg,strlen(msg));
}   

void manejador_Alarma(int sig){
    //Maneja la alarma del hijo1 cada 2 segundos
    signal(sig,manejador_Alarma); //rearmo la alarma del temporizador de nuevo
    char msg[T];    
    sprintf(msg,"Soy proceso hijo1 con PID=%d: papá, despierta a mi hermano\n",getpid());
    write(1,msg,strlen(msg));
    kill(getppid(), SIGUSR1); //llama al padre
}
void manejador_padreSIGUSR1(int sig){
    signal(sig,manejador_padreSIGUSR1);
    char msg[T];    
    sprintf(msg,"Soy el proceso padre con PID = %d: ya voy!.\n",getpid());
    write(1,msg,strlen(msg));
    kill(pid_hijo2, SIGUSR1); //llama al hijo2
} 

int pid_padre;

int main(){
    
    /* El padre crea dos procesos hijos */
    if((pid_hijo1 = fork())<0){
        perror("fork");
        exit (-1);
        } else if (pid_hijo1 == 0){ //Estoy en el hijo1
            //Activo un temporizador a 2 segundos en el hijo 1
            struct itimerval temporizador;
            struct timeval tiempoini;
            struct timeval tiemporepe;
            tiempoini.tv_sec=2;
            tiempoini.tv_usec=0;
            tiemporepe.tv_sec=2;
            tiemporepe.tv_usec=0;
            temporizador.it_value=tiempoini;
            temporizador.it_interval=tiemporepe;
            signal(SIGALRM,manejador_Alarma); //armo la alarma
            setitimer(ITIMER_REAL,&temporizador,NULL); //activo el temporizador
            while(1){ //iteración infinita
                pause();   /*Espera señal*/
            }
    }
    if((pid_hijo2 = fork())<0){
        perror("fork");
        exit (-1);
            } else if (pid_hijo2 == 0){ //Estoy en el hijo2
            signal(SIGUSR1,manejador_hijo2SIGUSR1);
            while(1){ //iteración infinita  
                pause();
            }
        }
    
    if (pid_hijo1 !=0){ //Estoy en el padre
        //signal(SIGUSR1,manejador_SIGUSR1); //armo el manejador
        signal(SIGUSR1,manejador_padreSIGUSR1);
        while(1){
            pause();
        }
    }
    return 0;
}