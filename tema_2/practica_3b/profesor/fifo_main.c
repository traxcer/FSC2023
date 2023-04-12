#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>

void esperaHijos(int sig)
{
	wait(0);
	signal(SIGCHLD, esperaHijos);
}

int main(int argc, char* argv[])
{
	
	int pidHijo;
	char fich[512];
	signal(SIGCHLD, esperaHijos);

	while (1) 
	{
		printf("Introduzca fichero a imprimir: ");
		scanf("%s", fich);
		if ((pidHijo = fork()) == -1)
		{
			perror("No hay recursos");
			return -1;
		} 
		else if (pidHijo == 0) /*Codigo del hijo*/
		{
				execl("fifo_cliente_impresion", "fifo_cliente_impresion", fich, 0);
				perror("fallo en execl()!\n\n");
		}
		else 
		{
		}
	}
	return 0;
}
