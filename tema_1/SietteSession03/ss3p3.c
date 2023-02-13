/*  Escriba un programa en C que, dado el valor de la siguiente constante:

#define T 32
implemente la función

   void concatenar(char c1[], char c2[]); 
que concatena las cadenas de caracteres c1 y c2, y almacena el resultado en c1. 
Implemente también el main() correspondiente para probar localmente dicha función.
Prueba 1: Compilación
Prueba de funcionamiento
La función solicitada se invocará con llamadas como:
           char c1[32] = "hola";
           char c2[32] = "adios";
           concatenar(c1,c2); //se espera que c1 = "holaadios";
           char c1[32] = "";
           char c2[32] = "adios";
           concatenar(c1,c2); //se espera que c1 = "adios";
Nótese que este programa no espera salida alguna, y que las comprobaciones se 
realizarán con llamadas a la función correspondiente, que deben tener el prototipo 
indicado o, en otro caso, habrá un error de compilación. 
No está permitido el uso de la librería "string.h".
*/

#include <stdio.h>
#define T 32

void concatenar(char c1[], char c2[]);

int main(){
    char c1[32] = "hola";
    char c2[32] = "adios";
    concatenar(c1,c2);
    printf("c1 concatenada = %s\n",c1); //se espera que c1 = "holaadios";
}

void concatenar(char c1[], char c2[]){
    int i=0;
    int j=0;
    while (c1[i]!='\0')
        i++;
    while (c2[j]!='\0'){
        c1[i]=c2[j];
        i++;
        j++;
    }
    c1[i]='\0'; //añade finalizador de cadena al final
}