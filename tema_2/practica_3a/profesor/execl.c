#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>

int main(int argc, char* argv[]){
	
	int pidHijo;
	int childNumber = 0;

	while (childNumber < 10) 
	{
		if ((pidHijo = fork()) == -1)
		{
			perror("No hay recursos");
			return -1;
		} 
		else if (pidHijo == 0) /*Codigo del hijo*/
		{
			if ((int)getpid() % 2 == 0) 
			{	
				printf("El hijo con pid %d va a ejecutar un date\n", getpid());
				execl("/bin/date", "date", 0);
				perror("fallo en execl()!\n\n");
			} else 
			{
				printf("El hijo con pid %d va a ejecutar un pwd\n", getpid());
				execl("/bin/pwd", "pwd", 0);
				perror("fallo en execl()!\n\n");
			}
		}
		else 
		{
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
