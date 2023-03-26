/*
Realice una variación del ejercicio anterior donde esta vez el hijo 
será el que ponga fin a los dos procesos cuando alcance la cuenta de 
X caracteres leídos por la  pipe  (este  valor  se  puede  obtener  
como  argumento  al  programa).  En  esta ocasión, se creará una 
pipe adicional para que el hijo le pueda transmitir al padre si éste 
debe continuar leyendo mensajes del usuario por teclado o debe 
finalizar.  
*/

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <errno.h>

#define T 255

/* Prototipos */
int read_n(int fd,void *b, int n);

/* Función principal */
int main(int argc, char *argv[]){
if (argc<2){
    printf("Usar: %s <int a leer>\n", argv[0]);
    exit(1);
}

/* Preparo las pipe para canalizar la comunicación (ph)PADRE -> HIJO 
    y la comunicación inversa (hp)HIJO -> PADRE */
    int ph[2], hp[2]; /**/
    if ((pipe(ph))<0){
        perror("pipe ph");
        exit(1);
    }
    if ((pipe(hp))<0){
        perror("pipe hp");
        exit(1);
    }

    pid_t pid;
    if((pid=fork())<0){
        perror("fork");
        exit(1);
    } else if (pid==0){ /* Proceso HIJO*/
    int t_leidos=0;
    if((close (ph[1]))<0){
        perror("Close Escritura Padre->Hijo");
        exit(1);
    }
    if((close (hp[0]))<0){
        perror("Close Lectura Hijo->Padre");
        exit(1);
    }
    char mensaje[T];
    char texto[T];
    int leidos;
    leidos=read(ph[0],mensaje,T);
    do{
        //write (1,"Mensaje leido (HIJO):\n",23);
        t_leidos+=leidos;
        sprintf(texto,"(HIJO)%s (t_leido)%d", mensaje,t_leidos);
        write(1,texto,strlen(texto));
        if (t_leidos <= atoi(argv[1]))
            write(hp[1],"y",1);
        else
            write(hp[1],"n",1);

    } while ((leidos=read(ph[0],mensaje,T))>0);
    if (leidos<0){
        perror("leidos");
        exit(1);
    }
    if((close (hp[1]))<0){
        perror("Close Escritura Hijo->Padre");
        exit(1);
    }
    if((close (ph[0]))<0){
        perror("Close Lectura Padre->Hijo");
        exit(1);
    }

    exit(1); /* Termina el hijo */
    
    } else { /* Proceso PADRE */
    if((close (ph[0]))<0){
        perror("Close Lectura Padre->Hijo");
        exit(1);
    }
    if((close (hp[1]))<0){
        perror("Close Escritura Hijo->Padre");
        exit(1);
    }
    char mensaje[T];
    char msg[T];
    int teclado;
    char msgctr='y';
        do{
            //write (1,"Introduce un Mensaje (PADRE):\n",31);
            if((teclado=read(0,&mensaje,T))<0){
                perror("read teclado");
                exit(1);
            }
            mensaje[teclado]='\0'; /*convierte en string*/
            /* Escribe en tuberia el MSG*/
            if ((write(ph[1],mensaje,teclado-1))<0){
                perror("write del padre escribiendo P->H");
                exit(1);
            }
            if (read_n(hp[0],&msgctr,1)<0){
                perror("read_n");
                exit(1);
            }
            sprintf(msg,"(PADRE)%s (ctrl)%c\n",mensaje,msgctr);
            write(1,msg,strlen(msg));
        } while (msgctr=='y');
        if((close (ph[1]))<0){
            perror("(PADRE) Close Escritura Padre->Hijo");
            exit(1);
        }   
        if((close (hp[0]))<0){
            perror("(PADRE) Close Lectura Hijo->Padre");
            exit(1);
        }   
    wait(NULL); /* Espera al hijo */
    }
return 0; /* Fin del main() */
}


/* Funciones Auxiliares */
int read_n(int fd,void *b, int n){
    int a_leer=n;
    int t_leido=0;
    int leido;
    do
    {
        errno=0;
        leido=read(fd,b+t_leido,a_leer);
        if (leido >=0){
            a_leer-=leido;
            t_leido+=leido;
        }
    } while ( ((leido>0)&&(t_leido<n))|| errno==EINTR);
    if (t_leido > 0)
        return t_leido;
    return leido;

}