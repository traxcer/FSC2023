#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define FIFO_NAME "fsc_fifo"
//Estados
#define E_INIT 1
#define E_READY 2
#define E_PLAYING 3
//Eventos
#define EV_TEARDOWN 1
#define EV_SETUP 2
#define EV_PAUSE 3
#define EV_PLAY 4
#define EV_TIMEOUT 5

//Variables globales
int p[2]; //pipe


int main(){
    int fdf;
    int fin = 0;
    int estado= E_INIT;
    int evento;

    //Abro la fifo
    if((fdf=open(FIFO_NAME,O_RDONLY))<0){
        perror("OPEN FIFO");
        exit(-1);
    }
    while (fin==0){ //Bucle princial de la maquina
        switch (estado)
        {
        case E_INIT:
            switch (evento)
            {
            case EV_TEARDOWN:
                
                break;
            case EV_SETUP:
                
                break;
            
            default:
                break;
            } // fin control eventos en estado E_INIT
            break;
        
        default:
            break;
        }

    } //fin bucle principal de la maquina
    return 0;
}  //fin main