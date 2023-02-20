/* Escriba un programa en C que, dado el valor de la siguiente constante:
#define T 32
implemente la función
int longitudCadena(char c[]); 
que devuelve la longitud de la cadena de caracteres que se le pasa como argumento. 
Implemente también el main() correspondiente para probar localmente dicha función.
Prueba 1: Compilación
Prueba de funcionamiento
La función solicitada se invocará con llamadas como:
           int l = longitudCadena("hola"); //se espera que l = 4
           int l = longitudCadena(""); //se espera que l = 0
Nótese que este programa no espera salida alguna, y que las comprobaciones se realizarán
con llamadas a las funciones correspondientes, que deben tener el prototipo indicado o, 
en otro caso, habrá un error de compilación. No está permitido el uso de la librería "string.h".
*/

#include <stdio.h>
#define T 32

int longitudCadena(char c[]); 

int main(){
    char a[]="Hola mundo";
    printf("La longitud de a[] es %d\n",longitudCadena(a));
}

int longitudCadena(char c[]){
    int i=0;
    while (c[i]!= '\0')
        i++;
    return i;
} 