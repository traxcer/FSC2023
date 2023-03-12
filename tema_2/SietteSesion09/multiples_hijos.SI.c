#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>

#define T 16

void m(int signo) {
    //devolvemos los recursos de nuestro hijo al sistema
    wait(0);
    write(1,"Hijo terminado\n", 15);
    signal(SIGCHLD,m);
}

int main(int argc, char * argv[]) {
    //Programa que lee desde un descriptor de fichero
    //y por cada lectura, crea un hijo

    if (argc < 2) {
        printf("Uso: %s <tuberia>\n", argv[0]);
        exit(1);
    }

    // abrir la tuberia
    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror("open");
        exit(1);
    }

    if (signal(SIGCHLD, m) == SIG_ERR) {
        perror("signal");
        exit(1);
    }

    pid_t pid;
    int leidos;
    char b[T];

    //bucle infinito que atiende a un servicio
    while(1) {
        //bloqueamos a la espera de un cliente
        do {
            errno = 0;
            leidos = read(fd, b, T);
            b[leidos - 1] = '\0';
            printf("b = %s (%d)\n", b, leidos);
        } while ((leidos < 0) && (errno ==EINTR));
        
        //creamos un hijo para atender al cliente

        pid = fork();
        if (pid < 0) {
            perror("fork");
            exit(1);
        } else if (pid == 0) {
            //zona exclusiva del hijo
            printf("Hijo %d: %s\n", getpid(), b);
            sleep(30);
            exit(0);
        } else {
            //zona exclusiva del padre
            //wait(0); //no se generan hijos concurrentes
            //exit(0); //Para generar huérfanos
        }
    }

    if (close(fd) < 0) {
        perror("close");
        exit(1);
    }

    return 0;
}
