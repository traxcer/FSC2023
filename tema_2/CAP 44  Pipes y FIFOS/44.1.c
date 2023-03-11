#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>


int main()  {

int filedes[2];

    if (pipe(filedes) == -1){              /* Crea la pipe */
        perror("pipe");
        exit(0);
    }
    
    switch (fork()) {                     /* Create a child process */
        case -1:
            perror("pipe");
            exit (0);
        case 0:  /* Hijo */
            if (close(filedes[1]) == -1){  /* Cierra el extremo no usado*/
                perror("close");
                exit(0);
            }
                /* El hijo lee de la pipe */
                char b[255];
                read(filedes[0],b,255);
                printf("SOY EL HIJO: %s",b);
                exit(0); //Termino el hijo
            break;
        default: /* Padre */
            if (close(filedes[0]) == -1){ /* Cierra el extremo no usado*/
            perror("close");
                exit(0);
            }
        /* Elpadre escribe a la pipe */
        write(filedes[1],"Hola hijo, esto lo he mandado desde la pipe\n",44);
        wait(0); //espera termine el hijo
        break;
    }

}