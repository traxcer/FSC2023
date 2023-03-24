/* 
Implemente una función que emule a system(char *comando). Se llamará  
fsc_system,  y  tendrá  el  mismo  argumento  que  la  original.  
Nuestro  fsc_system creará  un  hijo  del  proceso  que  mutará  
(usando  alguna  versión  de  exec-)  para ejecutar  una  shell  con  
el  comando  suministrado  como  argumento  de  entrada. 
Tras la creación del hijo, la función fsc_system se bloqueará en un 
wait() hasta que finalice la ejecución del comando en el hijo. 
NOTA: consulte la documentación de teoría disponible sobre exec-.
*/


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int fsc_system(char *comando);

int main(int argc, char *argv[]){
    if (argc < 2){
        printf("Usar: %s <comando\n", argv[0]);
        exit (1);
    } else
        fsc_system(argv[1]);

    return 0;
}


int fsc_system(char *comando){
pid_t pid;
    if((pid=fork())<0){ /* Error al crear el hijo */
        perror("fork");
        exit(1);
    } else if (pid==0){ /* Estamos en el HIJO*/
        execl("/usr/bin/bash","bach","-c", comando,NULL);
        perror("execl");
        exit(1);
    } else{ /* Estamos en el PADRE */

        wait(NULL);
    }

return 0;
}