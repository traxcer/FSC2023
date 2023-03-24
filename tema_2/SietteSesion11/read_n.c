/* Función propia read_n

Salvaguarda lecturas ante la aparición de señales.
*/

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>

#define T 255

int read_n(int fd,char *buffer, int tamanio);
void m (int signo);

int read_n(int fd,char *buffer, int tamanio){

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

void m (int signo){
    write(1,"CAPTURADA\n",11);
    if (signal(SIGUSR1,m)<0){
        perror ("signal");
        exit (1);
    }
}

int main(){
    /* Prueba:
    Lee caracteres por teclado con read_n y lo interrumpño y retomo 
    tras una señal.
    */
    char b[T];
    pid_t pid;
    pid=getpid();

    if (signal(SIGUSR1,m)<0){
        perror ("signal");
        exit (1);
    }

    do{ 
        printf("Introduzca un texto por teclado, con fin, termina (pid:%d):\n", pid);
        int leido = read(0,b,T);
        b[leido-1]='\0';
    
    } while (strcmp(b,"fin")!=0);

return 0;
}