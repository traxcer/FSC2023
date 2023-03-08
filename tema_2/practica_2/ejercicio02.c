/* Compila con -ansi y sin éster flag y observa el comportamiento 
   de los procesos!
AÑADIR codigo en los puntos 1, 2 y 3
NOTA: cuando se compila en ansi, los comentarios // no los acepta,
hay que ponerlos entre barra-asterisco  asterisco-barra
*/

#include <signal.h>
#include <unistd.h>
#include <sys/time.h>

/* Definición de tipo puntero a función que devuelve void y recibe
   como argumento un entero*/
typedef void (*sighandler_t)(int);
/* Defino una variable global a ese tipo*/
sighandler_t puntero_a_funcion;

void manejador1(int s);
void manejador2(int s); /*prototipo para que no se queje el compilador*/

void manejador1(int s){
    write(1,"manejador1\n",11);
    /*2. Registra el manejador2 para que atienda SIGINT (Ctrl-C),
    guarda en la variable puntero_a_funcion lo que devuelve signal */
    puntero_a_funcion=signal(SIGINT,manejador2);
}
void manejador2(int s){
    write(1,"manejador2\n",11);
    /*3. Registra puntero_a_funcion para que atienda a SIGINT (Ctrl-C)*/
    signal(SIGINT,puntero_a_funcion);
}

int main(){
    /*1. Registra el manejador1 para que atienda a SIGINT (Ctrl-C)*/
    signal(SIGINT,manejador1);
    while(1) /*menos recursos si se pone pause() en el cuerpo de while*/
        pause();
    return 0;
}
