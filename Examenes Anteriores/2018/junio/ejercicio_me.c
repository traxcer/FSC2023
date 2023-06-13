/* EJERCICIO 1 Maquina de Estados*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

//ESTADOS y EVENTOS
#define EST_ESPERA_FIN1 0
#define EST_ESPERA_FIN2 1
#define EV_FIN 0
#define EV_NO_FIN 1
#define EV_TIMEOUT 2

//función epera_evento
    int espera_evento (int fd) {
    int evento = -1;
    int leidos;
    //int escritos;
    char buffer [512];
    fd_set conjunto;
    FD_ZERO (&conjunto);
    FD_SET (0, &conjunto);
    FD_SET (fd, &conjunto);
    struct timeval tiempo = {5, 500};
    //Select
    int resultado = select (fd + 1, &conjunto, 0, 0, &tiempo);
    if (resultado == -1) {
        perror ("Error al hacer select");
        exit (1);
    }
    if (resultado == 0) {
        evento = EV_TIMEOUT;
    } else { 
    if (FD_ISSET (0, &conjunto)) {
        memset (&tiempo, 0, sizeof (tiempo));
        leidos = read (0, buffer, 512);
        if (leidos < 0) {
            perror ("Error al leer");
            exit (1);
        }
        if (leidos == 0) {
        FD_CLR (0, &conjunto);
        }
        if (leidos > 0) {
        buffer [leidos] = '\0';
        char * q = strstr (buffer, "fin");
        if (q == NULL) {
            evento = EV_NO_FIN;
        } else {
        evento = EV_FIN;
        }
        }
    } // FD_ISSET
    if (FD_ISSET (fd, &conjunto)) {
        memset (&tiempo, 0, sizeof (tiempo));
        leidos = read (fd, buffer, 512);
        if (leidos < 0) {
            exit (1);
        }
        if (leidos == 0) {
        FD_CLR (fd, &conjunto);
        }
        if (leidos > 0) {
            buffer [leidos] = '\0';
            char * q = strstr (buffer, "fin");
            if (q == NULL) {
                evento = EV_NO_FIN;
            } else {
            evento = EV_FIN;
        }
        }
    } // FD_ISSET
    } // RESULTADO > 0
    return evento;
}

int main () {
    int estado = EST_ESPERA_FIN1;
    int evento;
    int fin = 0;
    int fd = open ("fsc_fifo", O_RDONLY);
    if (fd < 0) {
        perror ("Error al abrir la fifo");
    }
    while (!fin) {
        evento = espera_evento (fd);
        if (evento < 0) {
            printf ("Evento no reconocido");
        }
        switch (estado) {
            case EST_ESPERA_FIN1 :
                switch (evento) {
                    case EV_NO_FIN:
                    break;
                    case EV_FIN:
                        estado = EST_ESPERA_FIN2; printf ("Ha entrado\n");
                    break;
                    case EV_TIMEOUT:
                    break;
                }
            break;
            case EST_ESPERA_FIN2 :
                switch (evento){
                    case EV_NO_FIN:
                    break;
                    case EV_FIN:
                        fin = 1; printf ("Ha entrado2\n");
                    break;
                    case EV_TIMEOUT:
                        estado = EST_ESPERA_FIN1;
                    break;
                } // switch evento
            break;
        } //switch estado
    } // bucle while
return 0;
}
