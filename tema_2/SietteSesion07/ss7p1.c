/*
Implemente un programa en C que mantenga un registro de la llegada de las señales 
SIGUSR1, SIGUSR2 y SIGINT. Los detalles de funcionamiento serían:
Ante la llegada de cualquier señal, se escribe en un fichero denominado reg.dat 
tanto el orden como la señal correspondiente.
El fichero, que debe tener obligatoriamente ese nombre, si no existe, se crea, y 
si existe, se borra su contenido.
El programa corre indefinidamente hasta que llega SIGINT, en cuyo caso, además de 
escribir en disco el número orden y la señal, cierra el fichero y termina la ejecución.
Se recomienda el uso de sprintf() para generar las cadenas que se han de escribir en disco.
    char b[T];
    int v = 10;
    sprintf(b,"El valor de v es %d", v);
    // b = "El valor de v es 10"
Prueba 1: Compilación
Prueba de funcionamiento
Si la secuencia de señales SIGUSR1, SIGUSR2 y SIGINT con la que se prueba el código es la siguiente:
./registroEventos
#Llega SIGUSR2
#Llega SIGUSR1
#Llega SIGUSR1
#Llega SIGUSR2
#Llega SIGINT
El fichero de salida reg.dat debe tener el siguiente contenido:
1: SIGUSR2
2: SIGUSR1
3: SIGUSR1
4: SIGUSR2
5: SIGINT
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

//Prototipos
void manejadorSIGINT(int sig);
void manejadorSIGUSR1(int sig);
void manejadorSIGUSR2(int sig);

//Variables globales
int fd;
int contador=0;
char texto[20];

void manejadorSIGINT(int sig) {
    signal(sig,manejadorSIGINT); //vuelvo a registrar
    write(1,"Capturado SIGINT\n",17);
    contador++;
    sprintf(texto,"%d: SIGINT\n",contador);
    write(fd,texto,strlen(texto));
}
void manejadorSIGUSR1(int sig){
    signal(sig,manejadorSIGUSR1); //vuelvo a registrar
    write(1,"Capturado SIGUSR1\n",17);
    contador++;
    sprintf(texto,"%d: SIGUSR1\n",contador);
    write(fd,texto,strlen(texto));
}

void manejadorSIGUSR2(int sig){ 
    signal(sig,manejadorSIGUSR2);//vuelvo a registrar
    write(1,"Capturado SIGUSR2\n",17);
    contador++;
    sprintf(texto,"%d: SIGUSR2\n",contador);
    write(fd,texto,strlen(texto));
}

int main(){

    if (signal(SIGINT,manejadorSIGINT)==SIG_ERR){
        perror("Signal SIGINT");
        exit (-1);
    }
    if (signal(SIGUSR1,manejadorSIGUSR1)==SIG_ERR){
        perror("Signal SIGUSR1");
        exit (-1);
    }
    if (signal(SIGUSR2,manejadorSIGUSR2)==SIG_ERR){
        perror("SignalSIGUSR2");
        exit (-1);
    }
    //Preparamos el registro que guardará las señales.
    if((fd=open("reg.dat", O_WRONLY|O_APPEND|O_CREAT, 0666))<3){
        perror("Abriendo reg.dat");
        exit (-1);
    }
    while(1){  //bucle infinito
            printf("Esperando la llegada de señales\n");
        pause();
    }
}