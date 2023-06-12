#define _POSIX_SOURCE
/*=============================================
  maquina.c Examen Feb2022 Febrero
  =============================================
  Autor Manuel Eloy Gutiérrez
  Málaga a 15 de Mayo del 2023

  Objetivo:
  Máquina de estados que gestiona la monitorización 
  de temperaturas  de  un  determinado  dispositivo  
  Opera mediante instrucciones que llegan por una 
  fifo
  Nota del Programador:
  
===============================================*/
#include <stdio.h>
#include <fcntl.h> 
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <signal.h>


//Defino los estados
#define IDLE 0
#define MONITORIZANDO 1
#define ESPERANDO 2

//Defino los eventos
#define START 0
#define STOP 1
#define CONT 2
#define FIN 3

//variable globales
int fd_fifo;
int fd_pipe[2];
int lecturas=0;
int leer=0;
int fin=0;
char *estado_txt[]={"IDLE","MONITORIZANDO","ESPERANDO"};
#define T 128

//funciones auxiliares
int read_n(int fd, char *buffer, int n){
    int a_leer=n;
    int total_leido=0;
    int leidos;
    do{
        errno=0;
        leidos= read(fd,buffer+total_leido,a_leer);
        if (leidos>0){
            a_leer-=leidos;
            total_leido+=leidos;
        }
    } while ( ((leidos>0) && (total_leido<n)) ||(errno==EINTR));
    if (leidos<0)
        return leidos;
    else
        return total_leido;
}
int write_n(int fd, char *buffer, int n){
    int a_escribir=n;
    int total_escrito=0;
    int escritos;
    do{
        errno=0;
        escritos= read(fd,buffer+total_escrito,a_escribir);
        if (escritos>0){
            a_escribir-=escritos;
            total_escrito+=escritos;
        }
    } while ( ((escritos>0) && (total_escrito<n)) ||(errno==EINTR));
    if (escritos<0)
        return escritos;
    else
        return total_escrito;
}

void maneja_hijo(int s){
    wait(0);
    signal(s,maneja_hijo);
}
void temporizador_medicion(int s){
    signal(s,temporizador_medicion); //reactiva la captura del temp
    leer=1;
}

int espera_evento(){
    //Prepara select para controlar los descriptores
    fd_set conjunto;
    FD_ZERO(&conjunto);
    FD_SET(fd_fifo,&conjunto);
    //FD_SET(fd_pipe[0],&conjunto);
    //int maximo = fd_fifo > fd_pipe[0] ? fd_fifo:fd_pipe[0];
    int maximo=fd_fifo;
    int r= select(maximo+1,&conjunto,NULL, NULL, NULL);
    if (r<0){
        perror("select");
        close (fd_fifo);
        //close (fd_pipe[0]);
        //close (fd_pipe[1]);
        exit(1);
    }
    char mensaje[T];
    if (FD_ISSET(fd_fifo,&conjunto)){
        int leidos= read(fd_fifo, mensaje,T); 
        printf("Leido de la fifo: %d",leidos);
        if (leidos<0){
            perror("read");
            exit(1);
        }
        mensaje[leidos-1]='\0'; //convierte en cadena
        printf("Mensaje recibido: %s\n",mensaje);
        if (strncmp(mensaje,"/start ",7)==0){
            lecturas=atoi(&mensaje[7]);
        return START;
        }
        if (strcmp(mensaje,"/stop")==0){
            return STOP;
        }
        if (strcmp(mensaje,"/cont")==0){
            return CONT;
        }
        if (strcmp(mensaje,"/fin")==0){
            return FIN;
        }
    }
return -1;
}


int main(int argc, char * argv[]){
    if (argc<2){
        printf("Uso: %s <fifo>\n", argv[0]);
        exit (1);
    }
    pid_t pid;
    signal(SIGCHLD, maneja_hijo); //Asigna manejador a la captura de la señal

    //creamos la pipe
      //if ((pipe(fd_pipe))<0){
      //  perror("pipe");
      //  exit(1);
    //}

    //abrimos la fifo

    if((fd_fifo=open(argv[1],O_RDONLY))<0){
        perror("open");
        exit(1);
    }
    
	int estado = IDLE;

    while (fin==0){
        printf("Estado: %s\n", estado_txt[estado]);
        write(1,"Esperando Comando...\n",21);
        int evento_recibido= espera_evento();
        if (evento_recibido==-1)
            printf("Comando no reconocido\n");
        switch (estado)
        {
        case IDLE:
            switch (evento_recibido)
            {
            case START:
                estado=MONITORIZANDO;
                printf("Estado: Monitorizando...\n");
            break; //fin IDLE<-START
            default:
            break;
            } //fin del swith estado
        break;//case IDLE
        case MONITORIZANDO:
            if (evento_recibido==STOP){
                if((kill(pid,SIGSTOP))<0){
                    perror("SIGstop");
                    kill(pid,SIGKILL);
                    close (fd_fifo);
                    //close (fd_pipe[0]);
                    //close (fd_pipe[1]);
                    exit(1);
                }
                estado=ESPERANDO;
            }
            if (evento_recibido==FIN){
                if((kill(pid,SIGKILL))<0){
                    perror("SIGkill");
                    close (fd_fifo);
                    //close (fd_pipe[0]);
                    //close (fd_pipe[1]);
                    exit(1);
                }
                lecturas=0;
                estado=IDLE;
            }
            break; //monitorizando
        case ESPERANDO:
            if (evento_recibido==CONT){
                if((kill(pid,SIGCONT))<0){
                    perror("kill");
                    close (fd_fifo);
                    //close (fd_pipe[0]);
                    //close (fd_pipe[1]);
                    exit(1);
                }
                estado=MONITORIZANDO;
            }
            break;
        
        default:
            break;
        } //fin del switch estado
    if (estado==MONITORIZANDO){ //Aqui se crea el Hijo que simula las lecturas
        pid = fork();
        if (pid==0){ //Proceso hijo
            struct itimerval timerhijo;
            timerhijo.it_value.tv_sec=1;
            timerhijo.it_value.tv_usec=0;
            timerhijo.it_interval.tv_sec=1;
            timerhijo.it_interval.tv_usec=0;
            setitimer(ITIMER_REAL,&timerhijo,NULL); //activa temporizador
            while (lecturas>0){
                if (leer==1){
                    uint16_t temperatura=rand()%10;
                    printf("Proceso Hijo (Monitorización %d): %d",lecturas,temperatura);
                    lecturas --; //decrementa las lecturas pedidas
                }
                leer=0;
            }
            exit(1); //termina el hijo
        } 
        //Aqui estamos en el padre que sigue atendiendo la fifo
    }
    } // del bucle while (fin==0)


return 0;
} //fin del main

