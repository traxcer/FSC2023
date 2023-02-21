#include <stdio.h>

int main (){
    int i;
    /* En el array sys_errlist hay una descripción corta asociada a cada número
       de error, sys_nerr es el total de elementos del array sys_errlist. */
    for (i = 0; i < sys_nerr; i++)
      printf ("%d: %s\n", i, sys_errlist [i]);
return 0;
}