/*
Ejercicio 1 Maquina de Estado Septiembre 2021

Programador Manuel Eloy Gutiérrez Oñate
*/


#include <stdio.h>
#include <errno.h>


//Estados
#define IDLE 1
#define WAITING 2
#define CONNECTED 3

//Eventos
#define CONNECT_REQUEST 1 
#define CONNECT_CONFIRM 2
#define TIMEOUT 3
#define RESET 4


//Prototipos
int read_n(int fd,char *b, int n);

// funciones adicionales
int read_n(int fd,char *b, int n){
    int a_leer=n;
    int tot_leido=0;
    int leido;
    do {
        errno=0;
        leido=read(fd,b+tot_leido,a_leer);
            if (leido>0){
                tot_leido+=leido;
                a_leer-=leido;
            }
        }
    while (((leido>0)&&(tot_leido<n))||(errno==EINTR));
    if (leido <0)
        return leido;
    else
        return tot_leido;
}
int write_n(int fd,char *b, int n){
    int a_escribir=n;
    int tot_escrito=0;
    int escrito;
    do {
        errno=0;
        escrito=write(fd,b+tot_escrito,a_escribir);
            if (escrito>0){
                tot_escrito+=escrito;
                a_escribir-=escrito;
            }
        }
    while (((escrito>0)&&(tot_escrito<n))||(errno==EINTR));
    if (escrito<0)
        return escrito;
    else
        return tot_escrito;
}

//manejadores de señales


//espera eventos


//funcioón principal
int main(){




    return 0; 
} //fin main