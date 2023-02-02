/*
Tarea de Seguimiento de la Sessión 01

Tarea 2

Implementar:
int taylor_serie_exp(double x, int orden);

devuelve:
            
    En v devuelve el valor de aproximación del polinomio de Taylor de orden dado.

UMA, Málaga a 2 de Febrero del 2023
Programador: Manuel Eloy Gutiérrez
*/


#include <stdio.h>
#include <math.h> //para pow (Potencia de una función)
//powf(float x, float y); x elevado a la potencia y
#include <stdlib.h>

//prototipo
int taylor_serie_geometrica(double *v, double x, int orden);

int main(){
    double *v;
    v=malloc(sizeof(double));
    double x=-7.204568;
    int r =taylor_serie_exp(x,4);
    printf("Devuelve: %d\n",r); 
    return 0;
}

int taylor_serie_exp(double x, int orden){ 
    *v=0;
    for (double i=0;i<(double)orden+1;i++){
        printf("Orden %f: Potencia de x:%f elevado a y:%f es:%f ",i+1,x,i,pow(x,i));
        
        *v=*v+powf(x,i);
        printf(" Suma acumulada:%f\n",*v);
    }
    return 0;
}

int factorial(double n){
    double fact= 0; 
    for (int c = 1; c <= n; c++){
        fact = fact * c;
    }
    return fact;
}