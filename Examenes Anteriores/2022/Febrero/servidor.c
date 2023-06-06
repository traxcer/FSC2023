/*=============================================
  servidor.c Examen Feb2022 Febrero
  =============================================
  Autor Manuel Eloy Gutiérrez
  Málaga a 13 de Mayo del 2023

  Objetivo:
  Monitorización Temperatura mediante TCP
  El servidor mide la temperatura de un sistema
  simulado aleatorio cada segundo y la envía a 
  los clientes que están conectados.

  Nota del Programador:
  Eljo formato multiusuario fork, debido a la
  dedicación del servidor de enviar la 
  monitorizacion a cada cliente mientras que 
  esté conectado a intervalos de 1 segundo.
===============================================*/
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>


#define PUERTO 2119
#define MAX_CLIENTES 5

//Variable Globales
int enviar=0;

//Prototipos
ssize_t write_n(int fd,char *b,size_t tam);

void manejador_alarma(int signo){
    signal(SIGALRM,manejador_alarma); //armo la alarma
    write(1,"Envio->",7);
    enviar=1;
}

int main(){
    uint16_t temperatura;
    uint32_t control;
    
    struct itimerval temporizador;
    struct itimerval tempozero;
    struct timeval tiempoini;
    struct timeval tiempoini_zero;
    struct timeval tiemporepe;
    struct timeval tiemporepe_zero;
    
    tiempoini.tv_sec=1;
    tiempoini.tv_usec=0;
    tiemporepe.tv_sec=1;
    tiemporepe.tv_usec=0;
    
    tiempoini_zero.tv_sec=0;
    tiempoini_zero.tv_usec=0;
    tiemporepe_zero.tv_sec=0;
    tiemporepe_zero.tv_usec=0;
    
    temporizador.it_value=tiempoini;
    temporizador.it_interval=tiemporepe;
   
    if(signal(SIGALRM,manejador_alarma)==SIG_ERR){; //armo el manejador de la alarma
        perror("signal");
        exit(-1);
    }
    signal(SIGPIPE,SIG_IGN);

    struct sockaddr_in dir_servidor, dir_cliente;
    socklen_t tam_dir_cliente;
    tam_dir_cliente=sizeof(dir_cliente);
    memset(&dir_servidor,0,sizeof(dir_servidor));
    
    int sd, n_sd;
    int tamaño_pdu=sizeof(uint16_t)+sizeof(uint32_t);
    char b[tamaño_pdu];
    char *aux;

    //1. atribuyo valores a los campos del servidor
    dir_servidor.sin_family=AF_INET;
    dir_servidor.sin_port = htons(PUERTO);
    dir_servidor.sin_addr.s_addr=INADDR_ANY; //espera desde cualquier dirección
    //2. obtenemos el descriptor del socket de conexiones
    if((sd=socket(PF_INET,SOCK_STREAM,0))<0){
        perror("Servidor TCP, creación de socket conexiones fallida\n");
        exit(1);
    }
    //3. Vinculo
    if (bind(sd,(struct sockaddr *)&dir_servidor,sizeof(dir_servidor))<0){
        perror("Servidor TCP: bind fallido\n");
        exit(1);
    }
    //4. Pongo el servidor en modo escucha
    if(listen(sd,MAX_CLIENTES)<0){
        perror("Servidor TCP: listen fallido\n");
        exit(1);
    }
    int r;
    
    //Bucle del servidor
    while(1){
        printf("Servidor TCP esperando conexiones...\n");
        if((n_sd=accept(sd,(struct sockaddr *)&dir_cliente, &tam_dir_cliente))<0){
                //if (errno==EINTR){
                //    errno=0;
                //    continue;
                //} else {
                    perror("Servidor TCP: accept fallido\n");
                    close (sd);
                    close (n_sd);
                    exit(1);
                }
        write(1, "Ya tengo un cliente conectado\n",31);
        printf("Aceptada Conexión: socket=%d, Cliente:%s\n",n_sd,inet_ntoa(dir_cliente.sin_addr));
        //Activo el temporizador
        if(setitimer(ITIMER_REAL,&temporizador,NULL)<0){ //activo el temporizador
            perror("Servidor TCP: setitimer");
            exit(1);
        }
        int fin=1;
        int pid=getpid();
        while (fin==1) {
            if (enviar==1){
                enviar=0; //resetea
                temperatura = rand() % 10; //valor aleatorio entre 0 y 9. rand() está en stdlib.h 
                control=pow(temperatura,3);
                printf("(pid:%d) Temperatura =%u Control=%u ",pid,temperatura, control);
                //empaqueto y envio
                aux=b;
                temperatura=htons(temperatura);
                control=htonl(control);
                memcpy(aux,&temperatura,sizeof(temperatura));
                aux += sizeof(temperatura);
                memcpy(aux,&control,sizeof(control));
                r = write_n(n_sd,b,tamaño_pdu);
                printf("(Enviados %d bytes)\n",r);
                if ((r<0) && (errno==EPIPE)){
                    errno=0;
                    fin=0;
                    if (close (n_sd)<0){
                        perror("Servidor TCP: error en close\n");
                        close (sd);
                        exit(-1);
                    }
                    tempozero.it_value=tiempoini_zero;
                    tempozero.it_interval=tiemporepe_zero;
                    setitimer(ITIMER_REAL,&tempozero,NULL); //desactivo el temporizador
                } 
                } //fin de la condición enviar
            } //fin del bucle de la conexión cliente
        } //fin bucle while servidor
        if(close (sd)<0){
            perror("Servidor TCP: error en close\n");
        }
    return 0;
} //Fin main

/*==================
Funciones auxiliares
===================*/

ssize_t write_n(int fd,char *b,size_t tam){
    int aescribir=tam;
    int total_escrito=0;
    int escrito;
    do{
        errno=0;
        escrito=write(fd,b+total_escrito,aescribir);
        if (escrito>0){
            aescribir -=escrito;
            total_escrito +=escrito;
        }
    } while (((escrito>0)&&(total_escrito<tam))||(errno==EINTR));
    if (total_escrito>0)
        return total_escrito;
    else
        return escrito;
}