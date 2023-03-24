// Ejemplo del libro sobre kill

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

int main(){
    int pid ;
    if ((pid = fork())==0){ //se genera proceso hijo
        while (1){ //bucle infinito
            printf("HIJO. PID= %d\n", getpid());
            sleep(1);
        }
    }
    sleep(10);
    printf("PADRE. Terminación del proceso %d\n", pid);
    kill (pid,SIGTERM);
    return 0;
}