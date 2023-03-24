/* 
Implemente un programa que cree 10 hijos. 
Si un hijo detecta que su pid es par, mostrará la fecha y hora del sistema. 
Si el pid del hijo es impar, éste mostrará el directorio actual en el que nos encontramos. 
Debes usar alguna función de la familia exec.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

int contador;
int pid_id;

void manejador_sigchld(int sig){
    signal(SIGCHLD,manejador_sigchld); //un hijo a acabado
    wait(0); //El padre libera los recursos.
}


int main(){
    signal (SIGCHLD,manejador_sigchld);
    for (contador=0;contador<10;contador++){ //Bucle que genera lo 10 hijos
        if((pid_id=(fork()))<0){ //Crera hijo
            perror("fork");
            exit (-1);
        }
        if (pid_id==0) { //Estoy en el hijo
            if (getpid()%2==0){
                //Tiene el pid par mostrará la fecha y hora del sistema.
                printf("Soy el hijo %d con PID:%d, par, y por tanto muestro fecha SO.\n",contador,getpid());
                execl("/usr/bin/date","date",NULL);
            } else {
                //Impar mostrará directorio actual
                printf("Soy el hijo %d con PID:%d, impar, y por tanto muestro directorio actual.\n",contador,getpid());
                execl("/usr/bin/ls","ls",NULL);
            }
            exit(0); //Termina el hijo
        }
        else {  //Estoy en el padre
        printf("Soy el padre y he creado a mi hijo %d\n",contador);
        }
    } // Ya tengo a mis 10 hijos funcionando
    
    if (pid_id != 0 ){ //Padre esperando a que mis hijos acaben
        printf("Soy el Padre: Estoy esperando que muera mi hijo: 1\n");
        wait(0);
        printf("Soy el Padre: Estoy esperando que muera mi hijo: 2\n");
        wait(0);
        printf("Soy el Padre: Estoy esperando que muera mi hijo: 3\n");
        wait(0);
        printf("Soy el Padre: Estoy esperando que muera mi hijo: 4\n");
        wait(0);
        printf("Soy el Padre: Estoy esperando que muera mi hijo: 5\n");
        wait(0);
        printf("Soy el Padre: Estoy esperando que muera mi hijo: 6\n");
        wait(0);
        printf("Soy el Padre: Estoy esperando que muera mi hijo: 7\n");
        wait(0);
        printf("Soy el Padre: Estoy esperando que muera mi hijo: 8\n");
        wait(0);
        printf("Soy el Padre: Estoy esperando que muera mi hijo: 9\n");
        wait(0);
        printf("Soy el Padre: Estoy esperando que muera mi hijo: 10\n");
        wait(0);
        printf("Soy el Padre: Ya han muerto todos los hijos\n");
    }

}