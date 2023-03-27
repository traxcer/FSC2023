# Ejercicios problematicos de clase 

#### Preparatorio para el examen del tema 2
---
## Preguntas sobre: memoria
---
- ¿Que imprime éste código por pantalla?
  ```
  #include <stdio.h>
  int main(){
    int a[]={10,20};
    int *p1=&a[0];
    int *p2=&a[1];
    int n= p2-p1;
    printf("n=%d\n",n);
    return 0;
  }
  ```
  Mostrará n=1, por la aritmética de punteros.
---
- ¿Que imprime éste código por pantalla?
  ```
  #include <stdio.h>
  int main(int argc, char *argv[]){
    char c1[]="fsc";
    char c2[]="fsc";
    if (c1==c2)
        printf("Iguales\n");
    else
        printf("Diferentes\n");
    return 0;
  }
  ```
  Mostrará diferentes, ya que c1 y c2 son dos punteros constantes a una cadena de caracteres con diferentes valores de posición de memoria.
---
- ¿Que imprime éste código por pantalla en una máquina de 64 bits?
  ```
  int main(){
    char *s="R2D2";
    printf("%d, %d, %d\n", sizeof(s),sizeof(*s),sizeof(s[0]));
    return 0;
  }
  ```
  imprimirá: 8,1,1
  sizeof(s), es el tamaño que ocupa el puntero en memoria, normalmente ocupa 16 bits (o sea 2 bytes), como ésta maquina es de 64 bits, en éste caso ocupará 8 bytes. Y sizeof(*s) se refiere al tamaño en bytes del valor contenido (un carácter), o sea 1 byte y sizeof(s[0]) se refiere a lo mismo.
---
- ¿Que imprime éste código por pantalla?
  ```
  void intercambia(int *p,int *q){
    int *aux;
    aux=p;
    p=q;
    q=aux;
  }

  int main(){
    int x=10;
    int y=20;
    intercambio(&x,&y);
    printf("x=%d; y=%d\n",x,y);
    return 0;
  }
  ```
  Imprimirá: x=10; y=20
  No intercambia loa valores pues en la función lo que se intercamia son dos copias de los valores mandados.
---
## Preguntas sobre: ficheros
---
- ¿Que llamada es correcta a open() para abrir "f.txt" y crearlo si no existe, y si existe, borrar su contenido?
  ```
  open("f.txt"O_WRONLY|O_CREAT|TRUNC, 0644);
  ```
---
- Si "fd" es el descriptor de fichero con el que hemos abierto "data.txt" para lectura, cuyo contenido es "0123456789" (sin comillas), y b es un array de char ¿Qué sale por pantalla?
  ```
  int r1=read(fd,b,20);
  int r2=read(fd,b,5);
  
  printf("r1=%d; r2=&d\n",r1,r2);
  ```
  Mostrará r1=10; r2=0
  Pues la primera lectura ya lee todo el contenido del fichero.
---
- Si "data.txt" contiene es "0123456789" (sin comillas), ¿Cuál sería el contenido del fichero al ejecutar el código?
  ```
  if ((fd=open("data.txt", O_WRONLY))<0){
    perror("open");
    exit(-1);
  }
  int w=write(fd,"abc",3);
  ```
  Contendrá: abc3456789
---  
- Si "data.txt" contiene es "0123456789" (sin comillas), ¿Cuál sería el contenido del fichero al ejecutar el código?
  ```
  if ((fd=open("data.txt", O_WRONLY|APPEND))<0){
    perror("open");
    exit(-1);
  }
  int w=write(fd,"abc",3);
  ```
  Contendrá: 1233456789abc
---
- Si "data.txt" contiene es "0123456789" (sin comillas), ¿Cuál sería el contenido del fichero al ejecutar el código?
  ```
  if ((fd=open("data.txt", O_WRONLY|TRUNC))<0){
    perror("open");
    exit(-1);
  }
  int w=write(fd,"abc",3);
  ```
  Contendrá: abc
---
- ¿Qué se muestra por pantalla?
  ```
  if ((fd=open("data.txt", O_WRONLY))<0){
    perror("open");
    exit(-1);
  }
  int w=write(fd,"abc",3);
  ```
  Contendrá: abc3456789
---
- ¿Qué se muestra por pantalla?
  ```
  int main(){
    char *s= "hola mundo!";
    char *p=s;
    printf("%c %c %s\n, *(p+1),s[1],p+5)
    return 0;
  }
  ```
  Contendrá: o o mundo!
---
- Sea fd el descriptor de un fichero cuyo contenido es "abcdefghij" (10 bytes)¿Qué se muestra por pantalla?
  ```
    #define T 10
    char b[T+1]="0123456789"
    int r1=read(fd,b,3);
    printf("r1= %d; b=%s\n",r1,b);
  ```
  Contendrá: r1=3 b=abc3456789
---

- Sea fd el descriptor de un fichero cuyo contenido es "abcdefghij" (10 bytes)¿Qué se muestra por pantalla?
  ```
    #define T 10
    char b[T+1]="0123456789"
    int r1=read(fd,b,3);
    int r2=read(fd,b+r1,3);
    printf("r2= %d; b=%s\n",r1,b);
  ```
  Contendrá: r2=3 b=abcdef6789
---
- Sea fd el descriptor de un fichero cuyo contenido es "abcdefghij" (10 bytes)¿Qué se muestra por pantalla?
  ```
    #define T 10
    char b[T+1]="0123456789"
    int r1=read(fd,b,T);
    int r2=read(fd,b,T);
    printf("r2= %d; b=%s\n",r1,b);
  ```
  Contendrá: r2=0 b=abcefghij
---
## Preguntas sobre: señales 
---
- Sea una llamada a la función signal(), con la señal s y el manejador m, signal(s,m). 
  - Respuesta correcta: Ejecuta que hacer ante la señal s.
---
- ¿Qué muestra el programa por pantalla si se pulsa Ctrl+C un segundo despues de iniciarse el programa?
  ```
    void m(int signo){
        write(1,"Catched!\n",9)
    }
    int main(){
        signal(SIGINT,m);
        printf("saliendo...\n");
    }
    
  ```
  Mostrará: saliendo...
---
- ¿Qué muestra el programa por pantalla si se pulsa Ctrl+C un segundo despues de iniciarse el programa?
  ```
    void m(int signo){
        write(1,"Catched!\n",9)
    }
    int main(){
        pause();
        printf("saliendo...\n");
    }
    
  ```
  Mostrará: ^C
---
## Preguntas sobre: procesos (fork y execl)
---
- ¿Cual es la salida por pantalla del proceso hijo?
  ```
  int main(int argc, char *argv[]){
    pid_t pid;
    int v=3;
    if((pid=fork())<0){
        perror("fork");
        exit(-1);
    } else if(pid==0){
        v++;
        printf("v=%d ", v);
    } else {
        v--;
        printf("v=%d",v);
    }
    v+=10;
    printf("v= %d\n",v);
    return 0;
  }
  ```
  Mostrará: v=4 v= 14
---

