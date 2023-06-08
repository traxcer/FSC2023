/*
Ejercicio 1 Maquina de Estado Septiembre 2021

Programador Manuel Eloy Gutiérrez Oñate
*/


#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/select.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>


//Estados
#define IDLE 0
#define WAITING 1
#define CONNECTED 2

//Eventos
#define CONNECT_REQUEST 1 
#define CONNECT_CONFIRM 2
#define TIMEOUT 3
#define RESET 4

//variable globales
int fd_pipe[2];
int fd_fifo;
char *estado_txt[]={"IDLE","WAITING","CONNECTED"};


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

void msigusr1(int s){
    int evento;
    evento=CONNECT_REQUEST;
    //escribo el evento en la pipe
    if(write_n(fd_pipe[1],(char*)&evento, sizeof(evento))<0){
        perror("Error write_n rn msigusr1");
        exit(1);
    }
    signal(s, msigusr1);
}
void msigusr2(int s){
    int evento;
    evento=CONNECT_CONFIRM;
    //escribo el evento en la pipe
    if(write_n(fd_pipe[1],(char*)&evento, sizeof(evento))<0){
        perror("Error write_n en msigusr2");
        exit(1);
    }
    signal(s, msigusr2);
}

void alarma(int s){
    int evento;
    evento=TIMEOUT;
    //escribo el evento en la pipe
    if(write_n(fd_pipe[1],(char*)&evento, sizeof(evento))<0){
        perror("Error write_n en alarma");
        exit(1);
    }
    //rearmo manejador de alarma
    signal(s,alarma);
}
//espera eventos
int espera_evento(){
    fd_set conjunto;
    FD_ZERO (&conjunto);
    FD_SET (fd_fifo, &conjunto);
    FD_SET (fd_pipe[0], &conjunto);
    int maximo = fd_pipe[0] > fd_fifo ? fd_pipe[0]:fd_fifo;
    int r= select(maximo+1,&conjunto,NULL,NULL,NULL);
    if (r<0){
        if (errno==EINTR){ //Ha interrumpido una señal
            errno =0;
        } else {
            perror("select");
            close (fd_fifo);
            close (fd_pipe[0]);
            close (fd_pipe[1]);
            exit (1);
        }
    }
    
    if(FD_ISSET(fd_pipe[0],&conjunto)){
        int evento;
        if (read_n(fd_pipe[0],(char *)&evento,sizeof(evento))<0){
            perror ("read_n pipe");
            close (fd_fifo);
            close (fd_pipe[0]);
            close (fd_pipe[1]);
            exit(1);
        }
        return evento;
    }

    if (FD_ISSET(fd_fifo,&conjunto)){
        char b[512];
        int leidos;
        //lectura de la fifo, no puedo usar read_n, no se el tamaño a leer
        do{
            errno=0;
            leidos=read(fd_fifo,b,512);
        } while (errno==EINTR);
        if (leidos <0){
            perror("read de la fifo");
            close (fd_fifo);
            close (fd_pipe[0]);
            close (fd_pipe[1]);
            exit(1);
        }
        if (leidos ==0){
            printf("Recibidos 0 bytes por la fifo, salimos...\n");
            close (fd_fifo);
            close (fd_pipe[0]);
            close (fd_pipe[1]);
            exit(1);
        }
    return RESET;
    }

    return 0;
}

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

int estado= IDLE; //Estado inicial
printf("--------------------------------------------\n");
printf("Ejecutandose Maquina de estado (PID: %d\n",getpid());
printf("--------------------------------------------\n");
printf ("Estado (%d): %s\n",estado,estado_txt[estado]);
int evento_recibido;
struct itimerval timer;
timer.it_interval.tv_sec=0;
timer.it_interval.tv_usec=0;
timer.it_value.tv_sec=2;
timer.it_value.tv_usec=0;
    while (1){
            evento_recibido= espera_evento();
        switch (estado){
        case (IDLE):
            switch (evento_recibido){
                case CONNECT_REQUEST:
                    estado=WAITING;
                    //activamos temporizador 2 segundos
                    setitimer(ITIMER_REAL,&timer,NULL);
                    printf ("Estado (%d): %s\n",estado,estado_txt[estado]);
                    break;
                default:
                    break;
                } //fin switch eventos
            break; //fin estado IDLE 
        case (WAITING):
            switch (evento_recibido){
                case TIMEOUT:
                    estado=IDLE;//reseteo temporizador
                    timer.it_interval.tv_sec=0;
                    setitimer(ITIMER_REAL,&timer,NULL);
                    printf ("Estado (%d): %s\n",estado,estado_txt[estado]);
                    break;
                case CONNECT_CONFIRM:
                    //reseteo temporizador
                    timer.it_interval.tv_sec=0;
                    setitimer(ITIMER_REAL,&timer,NULL);
                    estado=CONNECTED;
                    //reseteo temporizador
                    timer.it_interval.tv_sec=0;
                    setitimer(ITIMER_REAL,&timer,NULL);
                    printf ("Estado (%d): %s\n",estado,estado_txt[estado]);
                    break;
                default:
                    break;
                } //fin switch eventos
            
            break; //fin estado WAITING
        case (CONNECTED):
            switch (evento_recibido){
                case RESET:
                estado=IDLE;
                printf ("Estado (%d): %s\n",estado,estado_txt[estado]);
            break; //fin
                default:
                break;
            }
        default:
            break;
        }
    } //fin while principal

    return 0; 
} //fin main