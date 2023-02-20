/* 
Escriba un programa en C que implemente las siguiente función:

   void eliminarVocales(char * c); 

que elimina todas las vocales de la cadena de caracteres a la que se le pasa como argumento. 
Implemente también el programa main necesario para probar su funcionamiento.
Prueba 1: Compilación
Prueba de funcionamiento

A continuación se muestran algunos ejemplos del funcionamiento de la función:
           //c = "hola y adios";
           eliminarVocales(c);
           //se espera que c = "hl y ds";
           //c = "cdfg";
           eliminarVocales(c);
           //se espera que c = "cdfg"; es decir, si no hay vocales, la cadena no se modifica
Nótese que este programa no espera salida alguna, y que las comprobaciones se realizarán con llamadas a función correspondiente, que deben tener el prototipo indicado o, en otro caso, habrá un error de compilación.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Prototipos
void eliminarVocales(char * c);

int main (){
    char palabra[50]="hola y adios";
    eliminarVocales(palabra);
    printf("%s\n",palabra);
    strcpy(palabra,"cdfg");
    printf("%s\n",palabra);
    return 0;
}

void eliminarVocales(char *c){
char nueva[50];
int i=0,j=0;
do
    {
    if (c[i]!='a' && c[i]!='e' && c[i]!='i' && c[i]!='o' && c[i]!='u' && c[i]!='A' && c[i]!='E' && c[i]!='I' && c[i]!='O' && c[i]!='U')        
        {
        nueva[j]=c[i];
        j++;
        }
    i++;
} while(c[i]!='\0');
nueva[j]='\0';
strcpy (c,nueva);
}