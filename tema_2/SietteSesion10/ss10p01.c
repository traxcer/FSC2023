#define _POSIX_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>

int pid_hijo;

void alarma(int signo);

void alarma(int signo){
    kill(pid_hijo,SIGKILL);
}

int main(int argc, char *argv[]){

if (argc<3){ /* Check parametros */
    printf("Usar:%s <segundos> <cmd>\n",argv[0]);
    exit (1);
}
int pid;
signal (SIGALRM, alarma);  /*Manejador para la alarma*/
alarm(atoi(argv[1])); /* Activo el temporizador de ejecución */

if ((pid=fork())<0){ /*Error en la llamada al sistema*/
    perror ("fork");
    exit (1);
} else if (pid==0){  /*proceso Hijo ejecuta cmd*/
    execl("/bin/bash","bash", "-c", argv[2],NULL);
    exit (0);
} else { /* Padre */
    pid_hijo=pid;
    wait(0); /* Espero al hijo */
}
return 0;
}