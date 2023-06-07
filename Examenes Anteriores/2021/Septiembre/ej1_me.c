/*
Ejercicio 1 Maquina de Estado Septiembre 2021

Programador Manuel Eloy Gutiérrez Oñate
*/


#include <stdio.h>
#include <errno.h>
#include <signal.h>-
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


//Estados
#define IDLE 1
#define WAITING 2
#define CONNECTED 3

//Eventos
#define CONNECT_REQUEST 1 
#define CONNECT_CONFIRM 2
#define TIMEOUT 3
#define RESET 4

//variable globales
int fd_pipe[2];
int fd_fifo;


//Prototipos
int read_n(int fd,char *b, int n);

// funciones adicionales
int read_n(int fd,char *b, int n){
    int a_leer=n;
    int tot_leido=0;
    int leido;
    do {
        errno=0;
        leido=read(fd,b+tot_leido,a_leer);
            if (leido>0){
                tot_leido+=leido;
                a_leer-=leido;
            }
        }
    while (((leido>0)&&(tot_leido<n))||(errno==EINTR));
    if (leido <0)
        return leido;
    else
        return tot_leido;
}
int write_n(int fd,char *b, int n){
    int a_escribir=n;
    int tot_escrito=0;
    int escrito;
    do {
        errno=0;
        escrito=write(fd,b+tot_escrito,a_escribir);
            if (escrito>0){
                tot_escrito+=escrito;
                a_escribir-=escrito;
            }
        }
    while (((escrito>0)&&(tot_escrito<n))||(errno==EINTR));
    if (escrito<0)
        return escrito;
    else
        return tot_escrito;
}

//manejadores de señales
int msigusr1(int s){
    int evento;
    evento=CONNECT_REQUEST;

    signal(s, msigusr1);
    return evento;
}
void msigusr1(int s){
    int evento;
    evento=CONNECT_REQUEST;
    //escribo el evento en la pipe
    if(write_n(fd_pipe[1],evento, sizeof(evento))<0){
        perror("Error write_n rn msigusr1");
        exit(1);
    }
    signal(s, msigusr1);
}
void msigusr2(int s){
    int evento;
    evento=CONNECT_CONFIRM;
    //escribo el evento en la pipe
    if(write_n(fd_pipe[1],evento, sizeof(evento))<0){
        perror("Error write_n en msigusr2");
        exit(1);
    }
    signal(s, msigusr2);
}

void alarma(int s){
    int evento;
    evento=TIMEOUT;
    //escribo el evento en la pipe
    if(write_n(fd_pipe[1],evento, sizeof(evento))<0){
        perror("Error write_n en alarma");
        exit(1);
    }
    signal(s, msigusr1);
}
//espera eventos


//función principal
int main(){
    //abro la pipe y la fifo
    if (pipe(fd_pipe)<0){
        perror("Error creando la pipe");
        exit(1);
    }
    if ((fd_fifo=open("fsc_maquina",O_RDONLY))<0){
        perror("Error abriendo la fifo");
        exit(1);
    }
    //armo las señales
    if (signal(SIGUSR1, msigusr1)==SIG_ERR){
        perror("signal sigusr1");
        exit(1);
    }
    if (signal(SIGUSR2, msigusr2)==SIG_ERR){
        perror("signal sigusr2");
        exit(1);
    }
    if (signal(SIGALRM, alarma)==SIG_ERR){
        perror("signal sigalrm");
        exit(1);
    }
int fin=0;
    while (fin==0){
        

    } //fi n while principal

    return 0; 
} //fin main