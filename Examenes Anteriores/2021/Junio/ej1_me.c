/* EJERCICIO 2 Máquina de Estados */
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/* Estados */
#define EST_OCIOSO 1
#define EST_ESP_ENV 2
#define EST_ENVIADO 3
/* Eventos */
#define EV_DATA 1
#define EV_SIGALARM 2
#define EV_SIGUSR1 3
#define EV_SIGUSR2 4

int fin = 0;
int pipe_fd [2];
int fifo_fd;

/* Funciones auxiliares */
ssize_t write_n (int fd, void * buffer, size_t n) {
    char * p = (char *) buffer;
    int escritos;
    int intentado_escribir = n;
    int total_escritos = 0;
    do {
        errno = 0;
        escritos = write (fd, p + total_escritos, intentado_escribir);
        if (escritos > 0) {
            total_escritos += escritos;
            intentado_escribir -= escritos;
        }
    } while (((escritos > 0) && (total_escritos < n)) || (errno == EINTR));
    if (escritos < 0) {
        return escritos;
    }
    else {
        return total_escritos;
    }
}

ssize_t read_n (int fd, void * buffer, size_t n) {
    char * p = (char *) buffer;
    int escritos;
    int intentado_escribir = n;
    int total_escritos = 0;
    do {
        errno = 0;
        escritos = read (fd, p + total_escritos, intentado_escribir);
        if (escritos > 0) {
            total_escritos += escritos;
            intentado_escribir -= escritos;
        }
    } while (((escritos > 0) && (total_escritos < n))|| (errno == EINTR));
    if (escritos < 0) {
        return escritos;
    }
        else {
        return total_escritos;
    }
}

void manejadora_alarm (int s) {
    int evento = EV_SIGALARM;
    int escritos = write_n (pipe_fd [1], &evento, 4);
    if (escritos<0){
        perror("manejadora_alrm write_n ");
        close (pipe_fd[0]);
        close (pipe_fd[1]);
        close(fifo_fd);
        exit(-1);
    }
    signal (s, manejadora_alarm);
}

void manejadora_usr1 (int s) {
    int evento = EV_SIGUSR1;
    int escritos = write_n (pipe_fd [1], &evento, 4);//escribe entero
    if (escritos<0){
        perror("manejadora_alrm write_n ");
        close (pipe_fd[0]);
        close (pipe_fd[1]);
        close(fifo_fd);
        exit(-1);
    }
    signal (s, manejadora_usr1);
}
void manejadora_usr2 (int s) {
    int evento = EV_SIGUSR2;
    int escritos = write_n (pipe_fd [1], &evento, 4);
    if (escritos<0){
        perror("manejadora_alrm write_n ");
        close (pipe_fd[0]);
        close (pipe_fd[1]);
        close(fifo_fd);
        exit(-1);
    }
    signal (s, manejadora_usr2);
}

int maximo (int a, int b) {
    if (a > b) {
        return a;
    } else {
        return b;
    }
}

int espera_evento () {
    int evento;
    fd_set conjunto;
    FD_ZERO (&conjunto); //limpia el conjunto
    FD_SET (fifo_fd, &conjunto);
    FD_SET (pipe_fd [0], &conjunto);
    int max_fd = maximo (pipe_fd [0], fifo_fd);
    int r = select (max_fd + 1, &conjunto, 0, 0, 0);
    //chequeo la llamada al sistema select() que no haya fallado
    if (r < 0) {
        if (errno == EINTR) {
        errno = 0;
    }
    else {
        perror ("Select");
        //libera descriptores
        close(fifo_fd);
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        //salimos de la ejecución
        exit (1);
        }
    }
    /* Se chequea el resultado de select tras desbloquearse
     tras detectarse dats en alguno de los descriptores que
     se controlan */
    if (FD_ISSET (pipe_fd[0], &conjunto)) { //datos para leer en la pipe
        /* leo el comando que hay en la pipe, uso la lectura protegida con
        con read_n, para proteger la lectura de eventuales señales */
        int leidos = read_n (pipe_fd[0], &evento, 4);
        if (leidos < 0) {
            perror ("Leyendo\n");
            exit (1);
        }
        return evento; // devuelve el comando leido 
    }
    if (FD_ISSET (fifo_fd, &conjunto)) { //datos para leer de la fifo
        char buffer [512];
        int leidos;
        /*  leo la cadena que hay en la fifo, desconozco la longitud de
        la cadena a leer, hago un do...while y controlo que no interfiera
        una señal con errno==EINTR */
        do {
            errno=0;
            leidos = read (fifo_fd, buffer, 512);
            if (leidos < 0) {
                if (errno == EINTR) {
                    errno = 0;
                    continue; //continua tras comprobar que es una señal
                }
            else { //no es una señal, es un error y se sale liberando recursos
                perror ("Leyendo\n");
                close(fifo_fd);
                close(pipe_fd[0]);
                close(pipe_fd[1]);
                exit (1);
                }
            }
            
            buffer [leidos - 1] = '\0'; //convierte en cadena
            //(Ej: leidos=2. b[0] y b[1], entonces b[2-1]='\0', sustitute al ENTER que es el último caracter)
            if (strcmp (buffer, "data") == 0) { //comparamos el comando
                evento= EV_DATA;
                return evento; // devuelve el comando leido 
            } else {
                printf ("Cadena no reconocida. Se considera un error y se sale de la Máquina.\n");
                close(fifo_fd);
                close(pipe_fd[0]);
                close(pipe_fd[1]);
                exit (1);
            }
        } while (leidos > 0); //mientras se haya leido se sigue leyendo
    }
return 0;
}
/* main */

int main () {
    /* Arma la captura de señales */
    signal (SIGALRM, manejadora_alarm);
    signal (SIGUSR1, manejadora_usr1);
    signal (SIGUSR2, manejadora_usr2);
    /* crea la pipe para encolar las señales */
    int r = pipe (pipe_fd);
    if (r < 0) {
        perror("pipe");
        exit (1);
    }
    /* abre la fifo*/
    fifo_fd = open ("fsc_fifo", O_RDONLY);
    if (fifo_fd < 0) {
        perror ("fifo");
        exit (1);
    }
    printf ("Pid de mi proceso: %d\n", getpid ());
    /* Estado inicial y ajuste temporizador */
    int estado = EST_OCIOSO;
    struct itimerval timer;
    timer.it_interval.tv_sec=0; //no es periodido, pongo a 0
    timer.it_interval.tv_usec=0;

    while (!fin) { //Bucle principal de la maquina de estados
        int evento = espera_evento ();
        if (evento < 0) {
            printf ("Error espera evento");
            exit (1);
        }
        switch (estado) {
            case EST_OCIOSO:
                if (evento == EV_DATA) {
                    estado = EST_ESP_ENV;
                    printf ("Cambiando a estado esperando enviar...\n");
                    } else {
                    printf ("Evento no esperado");
                    }
            break;
            
            case EST_ESP_ENV:
                if(evento== EV_SIGUSR1){
                    printf ("enviando datos...\n");
                    timer.it_value.tv_sec=1;
                    timer.it_value.tv_usec=500000;
                    estado = EST_ENVIADO;
                    setitimer (ITIMER_REAL, &timer, 0); //activo temporizador    
                }
            break;
            
            case EST_ENVIADO:
                switch (evento){
                    case EV_SIGALARM:
                        estado = EST_ESP_ENV;
                        printf ("TimeOut (1.5s): volviendo al estado esperando enviar...\n");
                        break;
                    case EV_SIGUSR2:
                        estado = EST_OCIOSO;
                        printf ("Enviado Ok. Pasando a estado ocioso...\n");
                        timer.it_value.tv_sec=0; //pongo el temporizador a 0
                        timer.it_value.tv_usec=0;
                        setitimer (ITIMER_REAL, &timer, 0);
                        break;
                    default:
                        printf ("Evento no esperado\n");
                        break;
                }
                break;
                default:
                    printf ("Estado no esperado");
                    close(fifo_fd);
                    close(pipe_fd[0]);
                    close(pipe_fd[1]);
                    exit (1);
                    break;
            break;
            }
        } //Fin del bucle while
return 0;
}
