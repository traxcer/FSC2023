#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>


int main(int argc, char* argv[]){
	int pidHijo;
    int childNumber = 1;

	while (childNumber <= 10) {
		if ((pidHijo = fork()) == -1){
			perror("No hay recursos");
			return -1;
		} 
		else if (pidHijo == 0){ /*Codigo del hijo*/
			for (int i=0; i<10; i++)
				printf("Soy el hijo %d. Mi padre ha creado: %d hijos\n", getpid(), childNumber);
			exit(0);
		}
		else{ /*Código del padre*/
			childNumber++;
		}
	}
	/*En este caso, decido esperar a todos mis hijos aqui en vez de usar el manejador
	porque puede que el padre termine antes de que lo hagan los hijos */
	int i;
	for(i=0;i<10;i++){
		wait(0);
	}
	return 0;
}
