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
#include <string.h>

struct Estructura{
	int a ;
	float b;
	char c;
};

#define T 255

int main(int argc,char * argv[]) {
	if (argc < 2){
		printf("Usar: %s <nombrefichero>\n",argv[0]);
		exit(1);
	}
	int fd;
	ssize_t leer;
    int x;
	int array_enteros[4];
	struct Estructura est;
    char buffer[T];
	
	//Leemos los datos
	if ((fd = (open(argv[1],O_RDONLY))) <2){
		perror("Error en open");
		exit (1);
	}
	leer= read(fd,buffer,T-1);
    if (leer <0){
            perror("Error de Lectura");
            close (fd);
            exit(1);
        }
    if(close(fd)<0){
        perror("close");
        exit(1);
    }
	buffer[T]='\0';
	printf("El buffer leido (leido=%ld) es: %s\n",leer,buffer);
    
	//volcamos en la variablers lo leido en el buffer
    memcpy(&x,buffer,sizeof(int));

    memcpy(array_enteros,buffer+sizeof(int),sizeof(int)*4);

    memcpy(&est.a,buffer+sizeof(int)+sizeof(int)*4,sizeof(est.a));
    
    memcpy(&est,buffer+sizeof(int)+sizeof(int)*4+sizeof(est.a),sizeof(est.b));
    
    memcpy(&est,buffer+sizeof(int)+sizeof(int)*4+sizeof(est.a)+sizeof(est.b),sizeof(est.c));

    printf("El valor leido de x: %d\n",x);
    printf("El valor leido de array_enteros: %d %u %d %d\n",array_enteros[0],array_enteros[1],array_enteros[2],array_enteros[3]);
	
  
    
    
return 0;

}
