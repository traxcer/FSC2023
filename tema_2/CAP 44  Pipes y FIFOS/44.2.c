#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUF_SIZE 10

int main(int argc,char *argv[]){
    int fdp[2];
    char buff[BUF_SIZE];
    ssize_t numRead;

    /* Chequea parámetros de llamada */
    if (argc!=2 || strcmp(argv[1], "--help")==0){
        printf("%s cadena\n",argv[0]);
        exit(1);
    }

    if (pipe(fdp)==-1){ /* Crea la pipe */
        perror("pipe");
        exit(1);
    }

    switch (fork()){
        case -1:
            perror("fork");
            exit (1);
        case 0: /* Hijo, lee de la pipe */
            if (close(fdp[1])==-1){
                perror("close hijo");
                exit(1);  
            }
            for(;;){
                numRead=read(fdp[0],buff,BUF_SIZE);
                    if(numRead==-1){
                        perror("read");
                        exit(1);
                    }
                    if (numRead==0)
                        break;
                    printf("El hijo lee:\n");
                    if((write(1,buff,numRead))!= numRead){
                        perror("write hijo");
                        exit(1);
                    }
                    
            }
            write(1,"\n",1);
            if(close(fdp[0])<0){
                perror("close");
                exit(1);
            }
            printf("El hijo ha terminado\n");
            exit(1); /* Termino el hijo */
        default: /*Padre, escribe en la pipe */
            if(close(fdp[0])==-1){
                perror("close padre");
                exit(1);  
            }
            printf("El padre escribe en la pipe: %s\n", argv[1]);
            if (write(fdp[1], argv[1], strlen(argv[1]))!= strlen(argv[1])){
                perror("write padre");
                exit(1);  
            }
            printf("Padre esperando al hijo\n");
            wait(NULL); /* Espera que termine el hijo*/
    }

    return 0;
}