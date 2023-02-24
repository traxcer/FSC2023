/* Una de las principales misiones de un fichero es la de guardar contenidos 
y configuraciones de un programa para recuperarlos la siguiente vez que se 
ejecute, y así no se pierden.
Implemente un programa LeeDatos.c que toma como argumento el nombre de un 
fichero donde están almacenados los datos de GuardaDatos.c, lea del fichero 
el valor de las variables y las muestre por pantalla.
Las variables son las siguientes:
 
struct Estructura{
	int a;
	float b;
	char c;
};
int x = 7;
int array_enteros[4] = {0x00,0x01,0x02,0x03};
struct Estructura est;
est.a = 1; est.b = 2.0; est.c = '3';
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


struct Estructura{
	int a ;
	float b;
	char c;
};

int main(int argc,char * argv[]) {
	if (argc < 2){
		printf("Usar: %s <nombrefichero.dat>\n",argv[0]);
		exit(1);
	}
	int fd;
	ssize_t leer;
    int x;
	int array_enteros[4];
	struct Estructura est;
	
	//Leemos los datos
	if ((fd = (open(argv[1],O_RDONLY))) <2){
		perror("Error en open");
		exit (1);
	}
	do{
		leer= read(fd,&x,sizeof(x));
        if (leer ==-1){
            perror("Error de Lectura");
            close (fd);
            exit(1);
        }
	} while(leer<(ssize_t)sizeof(x));
    printf("El valor leido de x: %d\n",x);
	do{
		leer= read(fd,array_enteros, 4*sizeof(int));
        if (leer ==-1){
            perror("Error de Lectura");
            close (fd);
            exit(1);
        }
	} while(leer< 4*((ssize_t)sizeof(int)));
    printf("El valor leido de array_enteros: %d %d %d %d\n",array_enteros[0],array_enteros[1],array_enteros[2],array_enteros[3]);
	
    /* En ejecicio de otro año está así.
        struct Estructura z;
        if (read(fd,&z,sizeof(z))!=sizeof(z))
        {
            perror("Error leyendo la estructura");
            return -1;
        } printf( "Valores-> z.a: %d z.d: %f z.c: %c");
    
    */
    
    
    do{
		leer= read(fd,&est.a, sizeof(est.a));
        if (leer ==-1){
            perror("Error de Lectura");
            close (fd);
            exit(1);
        }
	} while(leer<(ssize_t)sizeof(est.a));
    printf("El valor leido de est.a: %d\n",est.a);
	
    do{
		leer= read(fd,&est.b, sizeof(est.b));
        if (leer ==-1){
            perror("Error de Lectura");
            close (fd);
            exit(1);
        }
	} while(leer<(ssize_t)sizeof(est.b));
    printf("El valor leido de est.b: %f\n",est.b);
	
    do{
		leer= read(fd,&est.c, sizeof(est.c));
        if (leer ==-1){
            perror("Error de Lectura");
            close (fd);
            exit(1);
        }
	} while(leer<(ssize_t)sizeof(est.c));
    printf("El valor leido de est: %c\n",est.c);
return 0;

}