#include <stdio.h>

#define T 5

struct PDU{
    size_t length;
    int *payload;
};

size_t memoria(struct PDU a[T]); 
void iniciar(struct PDU a[T]);

int main(){
    struct PDU a[T];
    size_t tamanio_real=0;
    iniciar(a);
    for (int i=0;i<5;i++)
        tamanio_real+=(a[i].length*sizeof(int));
    tamanio_real+=sizeof(int)*5;
    size_t tamanio = memoria(a);
    printf("Tamaño real datos: %lu Tamaño ocupado en la memoria: %lu\n",tamanio_real,tamanio);
    return 0;
}

void iniciar(struct PDU a[T]){
    a[0].length = 18;
    a[1].length = 14;
    a[2].length = 2;
    a[3].length = 7;
    a[4].length = 19;
}
size_t memoria(struct PDU a[T]){
    size_t result = 0;
    for(int i=0;i<T;i++){
        result = result + (a[i].length)*4 + sizeof(a[i].length)  ;
    }
    return result;
}
