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

//Defino los estados
#define IDLE 0
#define MONITORIZANDO 1
#define ESPERANDO 2

//Defino los eventos
#define START 0
#define STOP 1
#define CONT 2
#define FIN 3

int espera_evento(int fd,char *b);

#define T 128

int p[2]

int espera_evento(int fd, char *mensaje){
    
    int leidos= read(fd, mensaje,T);  //Bloquea esperando evento de la fifo
    if (leidos<0){
        perror("read");
        exit(1);
    }
    mensaje[leidos-1]='\0';

    if (strncmp(mensaje,"/START ",7)){
        return START;
    }

}


int main(int argc, char * argv[]){
    if (argc<2){
        printf("Uso: %s <fifo>\n", argv[0]);
        exit (1);
    }
    
    //creamos la pipe
    if (pipe(p)<0){
        perror("pipe");
        exit(1);
    }

    //abrimos la fifo
    int fd;
    if((fd=open(argv[1],O_RDONLY))<0){
        perror("open");
        exit(1);
    }

    struct itimerval temporizador;
    temporizador.it_value.tv_sec=1;
    temporizador.it_value.tv_usec=0;
    temporizador.it_interval.tv_sec=1;
    temporizador.it_interval.tv_usec=0;
    
	int estado = IDLE;
	int evento_recibido;
    int fin=0;
	char mensaje[T];

    fd_set cjt, cjtrab;
    FD_ZERO(&cjt);
    FD_SET(fd,&cjt)

    while (fin==0){
        printf("Estado: %d, Esperando Comando...", estado);
        int evento_recibido= espera_evento(fd, mensaje);
    
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
            }
            break;//case IDLE
        case MONITORIZANDO:
            /* code */
            break;
        case ESPERANDO:
            /* code */
            break;
        
        default:
            break;
        }
        
    
    
    } // del bucle while (fin==0)


return 0;
} //fin del main
