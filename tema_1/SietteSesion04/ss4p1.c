#include <stdio.h>

void asteriscos(int n);

int main(){
	int array [100],i=0;
    int cuenta[10]={0,0,0,0,0,0,0,0,0,0}; //inicializo cuenta de digitos

    printf("Introduzca una secuencia de números entre 0 y 9 terminada en número negativo: ");
    do 
    {
		scanf("%d",&array[i]); //introduce lo leido en scanf en un array al pulsar [ENTER]
        i++;
    } while(array[i-1]>=0); //mientras sean positivos, si es negativo termina de tomar datos
   
    i--; //Numero de elementos del array
    
    for (int n=0;n<i;n++) //recorro el array, indice n y genero cuenta con el total de cada digito
        cuenta[array[n]]++;
    
    for (int i=0;i<10;i++)
    {
        printf("%d ",i);
        asteriscos(cuenta[i]);
        printf("\n");
    }
    return 0;
}

void asteriscos(int n){
    int i;
    for (i=0;i<n;i++){
        printf("*");
    }
}