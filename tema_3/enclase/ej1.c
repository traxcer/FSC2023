#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]){

    uint16_t v=0x3132;
    char almacen [2];
    memcpy(almacen,&v,sizeof(short));
    printf("a[0]=%c (%p)\n", almacen[0], &almacen[0]);
    printf("a[1]=%c (%p)\n", almacen[1], &almacen[1]);

    /* Una opcion para cambiar el orden muy ineficiente
       seria el trasponer los datos con una variable
       auxiliar, 
       
       schar aux=a[1];
       memcpy(&a[1],&a[0],sizeof(char));
       memcpy(&a[0],&aux,sizeof(char));
       
       
       pero es mucho mas eficaz usar la
       familia de funciones:
       para enviar a la red
       htons (16)
       htohi
       para recibir de la red
       ntohs (16 bits)
       ntohi (32 bits)
    
    */

    return 0;
}