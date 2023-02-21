/* GuardaDatos.c

Manuel Eloy Gutiérrez
*/

#include <stdio.h>

struct Estructura{
    int a;
    float b;
    char c;
};



int man(int argc, char *argv[]){
//Control argumentos
    if (argc<2){
        printf("Usar: %s <nombfichero",argv[0]);
        exit (1);
    }

//Datos
struct Estructura es;
es.a = 1; es.b = 2.0; es.c = '3';
int x = 7;
int array_enteros[4]={0x00, 0x01, 0x02, 0x03};





    return 0;
}

