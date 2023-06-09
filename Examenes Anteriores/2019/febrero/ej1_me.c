/* Ejercicio 1 Febrero del 2019
   Maquina de Estado

   Programador Manuel Eloy Gutiérrez 
*/
//Includes
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/time.h>
#include <fcntl.h>

//Estados
#define EST_LEYENDO 0
#define EST_CRITICO 1
//Eventos
#define EV_TIMEOUT 0
#define EV_ERROR 1
#define EV_WARNING 2

//Funciones accesorias
ssize_t read_n (int fd, void * buffer, size_t n) {
    char * p = (char *) buffer;
    int intentando_leer = n;
    int total_leidos = 0;
    int leidos;
    do {
        errno = 0;
        leidos = read (fd, p + total_leidos, intentando_leer);
        if (leidos > 0) {
            total_leidos += leidos;
            intentando_leer -= leidos;
        }
    } while (((leidos > 0) && (total_leidos < n)) || errno == EINTR);
    if (leidos < 0) {
        return leidos;
    } else {
    return total_leidos;
    }
}

ssize_t write_n (int fd, void * buffer, size_t n) {
    char * p = (char *) buffer;
    int intentando_escribir = n;
    int total_escritos = 0;
    int escritos;
    do {
        errno = 0;
        escritos = write (fd, p + total_escritos, intentando_escribir);
        if (escritos > 0) {
            total_escritos += escritos;
            intentando_escribir -= escritos;
        }
    } while (((escritos > 0) && (total_escritos < n)) || errno == EINTR);
    if (escritos < 0) {
        return escritos;
    } else {
        return total_escritos;
    }
}

int maximo (int a, int b) {
    if (a > b) {
        return a;
    } else {
        return b;
    }
}

//Variables globales
int fin = 0;
int errores = 0;
int fd_pipe [2];
int fd_fifo;
char *estado_txt[]={"LEYENDO","CRITICO"};

//manejadores para controlar las señales

void manejadora (int s) {
    uint32_t evento = EV_TIMEOUT;
    int e = write_n (fd_pipe[1], &evento, 4);
    if (e < 0) {
        perror ("Escribe en pipe");
        exit (1);
    }
    signal (s, manejadora);
}

int espera_evento () {
    uint32_t evento;
    fd_set conjunto;
    FD_ZERO (&conjunto);
    FD_SET (fd_fifo, &conjunto);
    FD_SET (fd_pipe[0], &conjunto);
    int max = maximo(fd_fifo, fd_pipe [0]);
    int r = select (max + 1, &conjunto, 0, 0, 0);
    if (r < 0) {
        if (errno == EINTR) {
    } else {
        perror ("Select");
        close (fd_fifo);
        close (fd_pipe [0]);
        close (fd_pipe [1]);
        exit (1);
        }
    }
    
if (FD_ISSET (fd_pipe [0], &conjunto)) {
    read_n(fd_pipe[0],&evento,sizeof(evento));
    return evento;
    } //fd_isset fd_pipe

if (FD_ISSET (fd_fifo, &conjunto)) {
    char letra[2];
    int leidos = read_n (fd_fifo, letra, 2);
    letra [leidos - 1] = '\0';
    printf ("Letra leida: %s \n", letra);
    if (leidos < 0) {
        perror ("Leyendo fifo");
        close (fd_fifo);
        close (fd_pipe [0]);
        close (fd_pipe [1]);
        exit (1);
    }
    if (leidos == 0) { //se ha cerrado el otro extremo de la fifo
        printf("Cierre detectado. Fin de la Máquina\n");
        close (fd_fifo);
        close (fd_pipe [0]);
        close (fd_pipe [1]);
        exit(1);
    }
    if (strcmp (letra, "E") == 0) {
        evento = EV_ERROR;
        errores++;
    } //comprobando si es una W
    if (strcmp(letra,"W")==0){
        evento = EV_WARNING;
    }
} //fin fd_isset fd_fifo


return evento;
} //fin espera evento


int main () {
    fd_fifo = open ("fsc_fifo", O_RDONLY);
    if (fd_fifo == -1) {
        perror ("Open");
        exit (1);
    }
    int resultado = pipe (fd_pipe);
    if (resultado < 0) {
        perror ("pipe");
        exit (1);
    }
    if (signal (SIGALRM, manejadora)==SIG_ERR){
        perror("signal SIGALRM");
        exit(1);
    }
    
    int estado = EST_LEYENDO;
    struct itimerval timer;
    timer.it_interval.tv_sec=0;
    timer.it_interval.tv_usec=0;
    printf("-----------------------------------------\n");
    printf("Máquina Activa\n");
    printf("-----------------------------------------\n");
    while (!fin) { //Bucle maquina de estado
        printf("------------------------\n");
        printf("ESTADO ACTUAL: %s\n",estado_txt[estado]);
        printf("------------------------\n");
        int evento = espera_evento ();
        
        switch (estado){
            case EST_LEYENDO:

                switch (evento){
                    case EV_ERROR:
                        if (errores==1){
                            printf("Se ha producido un primer Error.\n");
                            }
                        else if (errores==2){
                            estado = EST_CRITICO;
                            printf ("Cambio a estado->");
                            printf("%s\n",estado_txt[estado]);
                            printf("Se ha producido un segundo Error.\n");
                            //activo temporizador 3,5 segundos
                            timer.it_value.tv_sec=3;
                            timer.it_value.tv_usec=500000;
                            if(setitimer (ITIMER_REAL, &timer, 0)<0){
                                perror("setitimer");
                                close(fd_fifo);
                                close(fd_pipe[0]);
                                close(fd_pipe[1]);
                                exit(1);
                            }
                        }    
                    break;
                    case EV_WARNING:
                        printf ("Se ha producido un WARNING.\n");
                    break;
                    default:
                    break;
                }

            break; //EST_LEYENDO
            case EST_CRITICO:
                switch (evento){
                    case EV_ERROR:
                        printf("Ya se han producido %d errores.\n",errores);
                        printf ("Se termina la maquina de estado por esta condición");
                        close (fd_fifo);
                        close(fd_pipe[0]);
                        close(fd_pipe[1]);
                        exit(1);
                    break;
                    case EV_TIMEOUT:
                        printf("Ha saltado el TimeOur, reseteo errores\n");
                        errores=0; //reseteo errores, ha transcurrido el TimeOut
                        estado = EST_LEYENDO;
                        //desactivo temporizador
                        timer.it_value.tv_sec=0;
                        timer.it_value.tv_usec=0;
                        setitimer (ITIMER_REAL, &timer, 0);
                    break;
                    case EV_WARNING:
                        printf ("WARNING...\n");
                    break;
                    default:
                        printf ("Evento no esperado.");
                        exit (1);
                    break;
            }
            break;
            default:
            break;
        } //Fin del swith estados
    }//Fin del bucle while
return 0;
} //fin del main