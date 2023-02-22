/* Una de las principales misiones de un fichero es la de guardar contenidos
 y configuraciones de un programa para recuperarlos la siguiente vez que se 
 ejecute, y así no se pierden.
Implemente un programa GuardaDatos.c que toma como argumento el nombre de un 
fichero nuevo donde almacenar una serie de datos, y guarda el contenido de 
las siguientes variables:
 
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
	ssize_t escribe;
	int x = 7;
	int array_enteros[4] = {0x00,0x01,0x02,0x03};
	struct Estructura est;
	est.a = 1; est.b = 2.0; est.c = '3';
	//Guardamos los datos
	if ((fd = (open(argv[1],O_WRONLY | O_TRUNC | O_CREAT, 0664)))<2){
		perror("Error en open");
		exit (1);
	}
	do{
		escribe= write(fd,&x,sizeof(int));
	} while(escribe<(ssize_t)sizeof(int));
	do{
		escribe= write(fd,array_enteros, 4*sizeof(int));
	} while(escribe< 4*((ssize_t)sizeof(int)));

/* 
if write(fd,&est,sizeof(est)!=sizeof(est))
	{
	perror("Error en la escritura de la estructura");
	return -1;
	}
*/

	do{
		escribe= write(fd,&est.a, sizeof(est.a));
	} while(escribe<(ssize_t)sizeof(est.a));
	do{
		escribe= write(fd,&est.b, sizeof(est.b));
	} while(escribe<(ssize_t)sizeof(est.b));
	do{
		escribe= write(fd,&est.c, sizeof(est.c));
	} while(escribe<(ssize_t)sizeof(est.c));

close (fd);
return 0;

}