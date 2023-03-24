/* 
Implemente un programa que cree diez hijos usando fork(). 
El padre debe mantener la cuenta de cuántos hijos ha creado 
y cada hijo imprimirá su propio pid y el valor de número de 
hijos creado por su padre. 
Haz que cada hijo imprima la información diez veces. 
El padre debe atender adecuadamente la muerte de sus hijos 
sin bloquearse en un wait.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


int contador;
int pid_id;


int main(){
    for (contador=0;contador<10;contador++){ //Bucle que genera lo 10 hijos
        if((pid_id=(fork()))<0){ //Crera hijo
            perror("fork");
            exit (-1);
        }
        if (pid_id==0) { //Estoy en el hijo
            printf("Soy el Hijo y mi pid: %d y mi padre ha creado ya %d hijos\n", getpid(), contador);
            exit(0); //Termina el hijo
        }
        else {  //Estoy en el padre
        printf("Soy el padre y he creado a mi hijo %d\n",contador);
        }
    } // Ya tengo a mis 10 hijos funcionando
    
    if (pid_id != 0 ){ //Padre esperando a que mis hijos acaben
    printf("Soy el Padre: Estoy esperando que muera mi hijo: 1\n");
    wait(0);
    printf("Soy el Padre: Estoy esperando que muera mi hijo: 2\n");
    wait(0);
    printf("Soy el Padre: Estoy esperando que muera mi hijo: 3\n");
    wait(0);
    printf("Soy el Padre: Estoy esperando que muera mi hijo: 4\n");
    wait(0);
    printf("Soy el Padre: Estoy esperando que muera mi hijo: 5\n");
    wait(0);
    printf("Soy el Padre: Estoy esperando que muera mi hijo: 6\n");
    wait(0);
    printf("Soy el Padre: Estoy esperando que muera mi hijo: 7\n");
    wait(0);
    printf("Soy el Padre: Estoy esperando que muera mi hijo: 8\n");
    wait(0);
    printf("Soy el Padre: Estoy esperando que muera mi hijo: 9\n");
    wait(0);
    printf("Soy el Padre: Estoy esperando que muera mi hijo: 10\n");
    wait(0);
    printf("Soy el Padre: Ya han muerto todos los hijos\n");
}

}