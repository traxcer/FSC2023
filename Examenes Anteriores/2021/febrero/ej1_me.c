/*
Ejercicio 3 Maquina de Estados
Examen de Febrero del 2021

Programador: Manuel Eloy Gutiérrez Oñate
*/


//Includes
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

//Variable globales
int fd_fifo;
char*estados_txt[]={"IDLE","EJECUTANDO","ESPERANDO"};

//Estados
#define IDLE 0
#define  EJECUTANDO 1
#define ESPERANDO 2
//Eventos
#define CMD 0
#define STOP 1
#define CONT 2
#define FIN 3

int main (){

    int estado=IDLE; //estado inicial
    int fd_fifo=open("fsc_fifo", O_RDONLY);
    


    return 0;
} //fin main