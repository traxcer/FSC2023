#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define T 32

int main() {
    char c1[5] = "Hola";
    
    //Ejemplo: longitud de una cadena
    int l1 = strlen(c1);
    printf("strlen(\"%s\") = %d\n", c1, l1);
    l1 = strlen(c1+1);
    printf("strlen(\"%s\") = %d\n", c1+1, l1);

    //Ejemplo con problema: copias de cadenas
    char c2[4];
    strcpy(c2,c1);

    //¿Qué sacan esos printfs?//
    printf("strlen(\"%s\") = %d\n", c1, strlen(c1));
    printf("strlen(\"%s\") = %d\n", c2, strlen(c2));

    //Ejemplo con problema: concatenación de cadenas
    char dest[5] = "hola";
    char src[32] = " y adios muy buenas";
    
    //¿Qué le pasa al valor de l1 en la siguiente secuencia de instrucciones?
    printf("l1 = %d\n", l1);
    strcat(dest,src);
    printf("dest + src = %s\n", dest);
    printf("l1 = %d\n", l1);


    return 0;
}