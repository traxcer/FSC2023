/*  Programa ejemplo de procesos del libro:
    Programación avanzada en Unix

Manuel E. Gutiérrez.
22 Abril 2021
*/
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

#define NUM_PROCESOS 5
int I=0;


void codigo_del_proceso(int id){
    int i;
    for (i=0;i<50;i++)
        printf("Proceso %d: i= %d, I = %d\n",id, i,I++);
    exit (id); //Sale del proceso con el valor de id
    
}

int main(){
    int p;
    int id [NUM_PROCESOS]= {1,2,3,4,5};
    int pid;
    int salida;

    for (p=0; p <  NUM_PROCESOS ;p++){
        pid=fork();
        if (pid==-1){ //Error al crear el hijo
            perror("Error al crear el proceso");
            exit(-1);
        } else if (pid==0)//Codigo proceso hijo
            codigo_del_proceso(id[p]);//le pasamos el numero de proceso creado
    }
    //Ya esto lo ejecuta el padre. El hijo sale en la funcion llamada
    for (p=0;p<NUM_PROCESOS;p++){
        pid= wait (&salida);
        printf("Proceso %d con id = %x terminado\n", pid, salida >> 8);
    }
}
