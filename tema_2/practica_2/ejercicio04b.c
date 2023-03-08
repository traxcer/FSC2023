#define _POSIX_CODE

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>


/* Ejercicio 4.a

a) Escribe un programa que, periódicamente, (3,3 segundos de CPU) 
imprima un “.” en la pantalla. Protégelo contra Ctrl-C. 

(NOTA: para pararlo, usa Ctrl-Z y luego mátalo por línea de comandos)
*/

void m (int signo);
int imprimo=0;


void m (int signo){
    signal(SIGALRM,m);
    imprimo =1;
}

int main(){
    
    /*Protejo contra CTRL+C*/
    signal(SIGINT,SIG_IGN);
    
    struct itimerval temporizo; /* asigno valores*/
    temporizo.it_interval.tv_sec=3;
    temporizo.it_interval.tv_usec=300000;
    temporizo.it_value.tv_sec=3;
    temporizo.it_value.tv_usec=300000;
    
    /* Ajusto el Timer*/
    if((setitimer(ITIMER_REAL, &temporizo,NULL))<0){
        perror("signal");
        exit(1);
    }
    
    if((signal (SIGALRM,m))<0){
        perror("signal");
        exit(1);
    }
    
    while (1){
        pause();
        if (imprimo==1){
            write(1,".",1);
            imprimo=0;
        }
    }
return 0;
}