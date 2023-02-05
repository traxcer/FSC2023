#include <stdio.h>

#define T 21

int sumaMenores (int n, int a[T], int size);
int ordena (int matriz[T]);

int main(){
    int a1[T]={2, 106, 93,145,78,62,190,42,142,26,192,91,82,47,164,111,36,71,168,61,23};
    int m1 = sumaMenores(12, a1, 21);
    printf("Prueba 1:\n");
    if (m1==621)
        printf("Se ha obtenido el valor esperado m=621\n");
    else
        printf("No se ha obtenido el valor esperado\n");
    
    int a2[T]={153, 15, 168, 152, 46, 69, 142, 192, 84, 22, 132, 78, 0, 120, 114, 71, 135, 170, 56, 82, 17};
    int m2 = sumaMenores(18, a2, 21);
        printf("Prueba 2:\n");
    if (m2==1488)
        printf("Se ha obtenido el valor esperado m=1488\n");
    else
        printf("No se ha obtenido el valor esperado\n");

    int m3 = sumaMenores(15, a2, 21);
        printf("Prueba 3:\n");
    if (m3==-1)
        printf("Se ha obtenido el valor esperado m=-1\n");
    else
        printf("No se ha obtenido el valor esperado\n");
    return 0;
}

//devuelve la suma de los n menores
//si a es mayor que size o algun valor de a <1 devuelve -1
int sumaMenores (int n, int a[T], int size){
    if (n>size || ordena(a)==-1)
        return -1;
    printf("Matriz ordenada\n");
    for (int i=0;i<T;i++)
        printf("%d, ",a[i]);
    printf("\n");
    int suma=0;
    for(int i=0;i<n;i++)
        suma+=a[i];
    return suma;
}

int ordena (int matriz[T]){
    int tmp;
    int ya=1;
    while (ya==1){
        ya=0;
        for (int i=0;i<T;i++){    
            if (matriz[i]<0)
                return -1;
            if (matriz[i] > matriz[i+1]){
                ya=1;
                tmp=matriz[i];
                matriz[i]=matriz[i+1];
                matriz[i+1]=tmp;
            }
        }
    }
    return 0;
}