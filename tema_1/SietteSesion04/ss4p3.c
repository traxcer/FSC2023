/* 
Escriba un programa en C que, implemente la función

   void sustituir(char * input, char ** output); 

sustituye todos cada dígito de la cadena de entrada input por una secuencia 
de asteriscos que empieza y termina por el carácter | y que contiene tantos asteriscos 
como el valor del dígito. Por ejemplo, si la función recibe como entrada la cadena "a1B", 
el resultado será "a|*|B".

Implemente también el programa principal main() de forma que la cadena de entrada a la 
función se reciba como argumento del programa.
Prueba 1: Compilación
Prueba de funcionamiento
Varios ejemplos de ejecución de este programa, llamando a la función sustituir() serían:
              >./sustituirDigitos
               Introduzca una cadena: hola
               salida = hola
              >./sustituirDigitos
               Introduzca una cadena: 1b2c0
               salida = |*|b|**|c||
              >./sustituirDigitos
               Introduzca una cadena: a13b
               salida = a|*||***|b
Aunque se muestra la posible salida del programa, las pruebas se realizarán con llamadas 
a la función sustitir(), que debe tener el prototipo indicado o, en otro caso, habrá un 
error de compilación. No está permitido el uso de la librería "string.h".
*/
#include <stdio.h>
#include <stdlib.h>

void sustituir(char * input, char ** output);
int damemoria(char *c);
void sust(char **p, char c);
int tamanioCadena(char c[]);
void concatena(char c1[], char c2[]); 

int main(int argc, char *argv[]){
    if(argc<2){
        printf("Uso: %s cadena\n",argv[0]);
        exit(-1);
    }
    char *c;
    sustituir(argv[1],&c);
    printf("Introduzca una cadena: %s\n",argv[1]);
    printf("Salida: %s\n",c);
    return 0;
}

void sustituir(char * input, char ** output){
    int longitud = tamanioCadena(input);
    int n = damemoria(input);
    *output =(char*) calloc(n,sizeof(char));
    for(int i=0;i<longitud;i++){
        sust(output,input[i]);
    }
}
int damemoria(char *c){
    int mem = 0,cnt=0;
    char *p;
    p=c;
    while(p[cnt] != '\0'){
        if(p[cnt] == '0' || p[cnt] == '1' || p[cnt] == '2' || p[cnt] == '3' || p[cnt] == '4' || p[cnt] == '5' || p[cnt] == '6' || p[cnt] == '7' || p[cnt] == '8' || p[cnt] == '9'){
            mem = mem + atoi(&p[cnt]) + 2;
        }else{
            mem++;
        }
        cnt++;
    }
    return mem+1;
}

void sust(char **p, char c){
    int n;
    char cc[]="a";
    if(c == '0' || c == '1' || c == '2' || c == '3' || c == '4' || c == '5' || c == '6' || c == '7' || c == '8' || c == '9'){
        n = atoi(&c);
        concatena(*p, "|");
        for(int i=0;i<n;i++){
            concatena(*p, "*");
        }
        concatena(*p, "|");
    }else{
        cc[0] = c;
        concatena(*p, cc);
        
    }
}
int tamanioCadena(char c[]){
    char *p;
    int res = 0;
    p = c;
    while(p[res] != '\0'){
        res++;
    }
    return res;
}
void concatena(char c1[], char c2[]){
    int l = tamanioCadena(c1);
    int cnt = 0;
    char *p1,*p2;
    p1 = c1;
    p2 = c2;
    while(p2[cnt] != '\0'){
        p1[l] = p2[cnt];
        cnt++;
        l++;
    }
}