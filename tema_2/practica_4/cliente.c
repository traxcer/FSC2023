/*

*/
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define FIFO_NAME "/tmp/fsc_chat"

int main(int argc, char *argv[]) {
    int fd_fifo, pid_servidor;
    char mensaje[BUFSIZ];

    if (argc < 2) {
        fprintf(stderr, "Error: Debe pasar como argumento el pid del servidor de chat\n");
        exit(1);
    }

    pid_servidor = atoi(argv[1]);

    // Enviamos la señal SIGUSR1 al servidor para que abra la fifo
    kill(pid_servidor, SIGUSR1);

    // Esperamos a que el servidor abra la fifo
    sleep(1);

    // Abrimos la fifo para escribir
    fd_fifo = open(FIFO_NAME, O_WRONLY);
    if (fd_fifo == -1) {
        perror("Error al abrir la fifo");
        exit(1);
    }

    // Bucle de lectura de teclado y envío por fifo
    while (1) {
        printf("Escribe un mensaje: ");
        fgets(mensaje, BUFSIZ, stdin);

        // Quitamos el caracter de nueva línea al final del mensaje
        mensaje[strcspn(mensaje, "\n")] = '\0';

        // Si el mensaje es "exit", salimos del bucle y cerramos la fifo
        if (strcmp(mensaje, "exit") == 0) {
            close(fd_fifo);
            exit(0);
        }

        // Escribimos el mensaje en la fifo
        write(fd_fifo, mensaje, strlen(mensaje));
    }

    return 0;
}



