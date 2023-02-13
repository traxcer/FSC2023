/*
Tarea de Seguimiento de la Sessión 01

Tarea 1

Implementar:
int taylor_serie_geometrica(double *v, double x, int orden);

devuelve:
            -1 si -1=< x >= 1
            0  en otro caso
    En v devuelve el valor de aproximación del polinomio de Taylor de orden dado.

UMA, Málaga a 1 de Febrero del 2023
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
    double x=0.99985;
    int r =taylor_serie_geometrica(v,x,5);
    printf("Devuelve: %d\n",r); 
    return 0;
}

int taylor_serie_geometrica(double *v, double x, int orden){
    if ((x< -1 || x>1)) 
        return -1;
    *v=0;
    for (double i=0;i<(double)orden+1;i++){
        printf("Orden %f: Potencia de x:%f elevado a y:%f es:%f ",i+1,x,i,pow(x,i));
        *v=*v+powf(x,i);
        printf(" Suma acumulada:%f\n",*v);
    }
    return 0;
}