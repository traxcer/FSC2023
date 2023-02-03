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

//prototipos
double taylor_serie_exp(double x, int orden);
double factorial(double n);

int main(){
    double r =taylor_serie_exp(-7.204568,4);
    printf("Devuelve al final: %f\n",r); 
    return 0;
}

double taylor_serie_exp(double x, int orden){ 
    double resultado=0;
    for (double i=0;i<(double)orden+1;i++){
        printf("Orden %f: Valor ultimo sumando: %f / %f ",i,powf(x,i),factorial(i));
        resultado=resultado+(powf(x,i)/factorial(i));
        printf(" Suma acumulada:%f\n",resultado);
    }
    return resultado;
}

double factorial(double n){
    double fact= 0; 
    for (int c = 1; c <= n; c++){
        fact = fact * (double)c;
    }
    return fact;
}