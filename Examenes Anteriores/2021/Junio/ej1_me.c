/* EJERCICIO 1 */
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
#include <sys/sysinfo.h>
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
    write(1,"Ha llegado EV_SIGUSR1\n",22);
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
    write(1,"Ha llegado EV_SIGUSR2\n",22);
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
    if (r < 0) {
        if (errno == EINTR) {
        errno = 0;
    }
    else {
        perror ("Select");
    exit (1);
    }
    }
    if (FD_ISSET (pipe_fd[0], &conjunto)) {
        int leidos = read_n (pipe_fd [0], &evento, 4);
        if (leidos < 0) {
            perror ("Leyendo\n");
            exit (1);
        }
        return evento;
    }
    if (FD_ISSET (fifo_fd, &conjunto)) {
        char buffer [512];
        int leidos;
        do {
            leidos = read (fifo_fd, buffer, 512);
            if (leidos < 0) {
            if (errno == EINTR) {
                errno = 0;
            }
            else {
                perror ("Leyendo\n");
                exit (1);
                }
            }
            buffer [leidos - 1] = '\0';
            if (strcmp (buffer, "data") == 0) {
                return EV_DATA;
            } else {
                printf ("Cadena no reconocida. Terminando...\n");
                exit (1);
                fin = 1;
            }
        } while (leidos > 0 && errno == EINTR);
    }
return -1;
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
    struct timeval t1 = {1, 500000};
    struct timeval t2 = {0,0};

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
                if (evento == EV_SIGUSR1) {
                printf ("datos enviados...\n");
                estado = EST_ENVIADO;
                timer.it_interval = t1;
                timer.it_value = t1;
                setitimer (ITIMER_REAL, &timer, 0);
                }
                break;
            case EST_ENVIADO:
                switch (evento){
                    case EV_SIGALARM:
                        estado = EST_ESP_ENV;
                        printf ("Timeout: volviendo al estado esperando evniar...\n");
                        break;
                    case EV_SIGUSR2:
                        estado = EST_OCIOSO;
                        printf ("Pasando a estado ocioso...\n");
                        timer.it_interval = t2;
                        timer.it_value = t2;
                        setitimer (ITIMER_REAL, &timer, 0);
                        break;
                    default:
                        printf ("Evento no esperado\n");
                        exit (1);
                        break;
                }
                break;
                default:
                    printf ("Estado no esprado");
                    exit (1);
                    break;
            }
        } //Fin del bucle while
return 0;
}
