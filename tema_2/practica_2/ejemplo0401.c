#include <stdio.h>
#include <signal.h> //para signal
#include <unistd.h> //para write

void manejador(int num_senial){
    /*tratamiento de la señal*/
    signal(SIGINT, SIG_IGN); //SIG_IGN, ignora la señal
    write(1,"\nHas pulsado CTRL+C pero No!, no me salgo,\n",44);
    write(1,"pero ya ha desarmado la captura de señal\n",42);
    write(1,"la proxima vez sigo sin salir porque ahora la IGNORO\n",54);
    return; /*vuelta al programa*/
}


int main(){
    signal(SIGINT, manejador); //SIGINT: Señal de terminación, CTRL+C
    
    /*Codigo ejecutado indefinidamente para que se ejecute la señal*/
    while(1);
}

