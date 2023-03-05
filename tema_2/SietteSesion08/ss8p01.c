#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>

#define _POSIX_SOURCE
#define T 8
#define S 1

void alarma(int signo){
    printf("A saltado la ALARMA para escribir\n");
}

void m(int signo){//Manejadora señal SIGUSR1
    printf("Ha llegado la señal SIGUSR\n");
    signal(SIGUSR1,SIG_IGN);
    signal(SIGALRM,alarma);
}

int main(int argc, char *argv[]){
    int fdr, fdw, leido, escrito;
    char buffer[T];

    if (argc < 2){
        printf("Usar: %s <origen> <destino>\n",argv[0]);
        return 0;
    }
    printf("Mi pid:%d\n",getpid()); //Muestro el PID
    
    if ((fdr= open(argv[1],O_RDONLY))<0){ //Error en la apertura del fichero origen
        perror("open lectura");
        exit (-1);
    }
    if ((fdw= open(argv[2],O_WRONLY | O_TRUNC | O_CREAT, 0664))<0){ //Error en la apertura del fichero destino
        perror("open escritura");
        exit (-1);
    }
    while((leido=read(fdr,buffer,T))>0){ //Lee datos
        if (signal(SIGUSR1,m) == SIG_ERR) { //Captura de señal
            perror("SEÑAL");
            exit(-1);
            }
        printf("==> Leido (%d) bytes: %s\n",leido,buffer);
        pause(); //espera la señal SIGUSR1
        alarm(S);
        pause(); //espera la alarma
        if ((escrito=write(fdw,buffer,leido))<0){
            perror("write");
            exit(-1);
        }
        printf("<== Escrito (%d) bytes: %s\n",escrito,buffer);
    }
    close(fdr);
    close(fdw);
}
