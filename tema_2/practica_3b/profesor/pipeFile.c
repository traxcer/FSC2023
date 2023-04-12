#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h> 
#include <ctype.h>

#define SIZE 512
void esperaHijos(int sig)
{
	wait(0);
	signal(SIGCHLD, esperaHijos);
}

int main(int argc, char* argv[])
{
	
	int pidHijo;

	signal(SIGCHLD, esperaHijos);
	int p[2];
	pipe(p);

	if ((pidHijo = fork()) == -1)
	{
		perror("No hay recursos");
		return -1;
	} 
	else if (pidHijo == 0) /*Codigo del hijo*/
	{
		//Cerramos el lado de escritura de la pipe
		close(p[1]);
		int fdTarget;
		char buffer[SIZE];

		//Supongamos que hacemos una parada aquí con sleep
		//El padre seguiría escribiendo datos en la pipe, pero el hijo cogería bien
		//el nombre del fichero destino, porque el nombre del fichero contiene un carácter fin de cadena
		//y por tanto cuando el hijo hace open(buffer,...), buffer realmente tiene <nombrefichero\0> más
		//bytesRead - (strlen(buffer)+1) bytes de lo que haya enviado el padre justo después del nombre del fichero
		//sleep(15);
		
		//Primer hemos de leer el nombre del fichero destino para poder abrirlo
		//NOTA: en general, usamos sizeof(buffer) para indicar el tamaño, pero es nos sirve sólo si el buffer se ha reservado
		//estáticamente (es decir, si lo hemos declarado como una cadena de caracteres). Si lo declaramos como un puntero y reservamos memoria
		//sizeof(buffer) dará 4 (en arquitecturas de 32 bits) u 8 (en arquitecturas de 64 bits). Por tanto, en general es mejor usar una constante SIZE
		//como último argumento de read y write
		int bytesRead;
		if ((bytesRead = read(p[0], (char*)buffer, SIZE)) == 0) 
		{	
			perror("Problem in read in child\n");
			exit(-1);
		}
		fdTarget = open(buffer, O_WRONLY | O_TRUNC | O_CREAT, 0770);
		if (fdTarget < 0)
		{
			perror("Problem opening target file\n");
			exit(-1);
		}
	
		int bytesWritten;
		//Antes de leer más información, hemos de asegurarnos de que buffer no tiene más bytes
		if (bytesRead > strlen(buffer) + 1) 
		{
			printf("Entramos aqui\n");
			for (int i = strlen(buffer) + 1; i < bytesRead; i++)
			{
				buffer[i] = tolower(buffer[i]);
			}
			bytesWritten = write(fdTarget, (char*)(buffer+(strlen(buffer)+1)), bytesRead - (strlen(buffer) + 1));
			if (bytesWritten < 0)
			{
				perror("Error with write in the child\n");
				exit(-1);
			}
		}

		while((bytesRead = read(p[0], (char*)buffer, SIZE)) != 0)
		{
			for(int i = 0; i < bytesRead; i++)
			{
				buffer[i] = tolower(buffer[i]);
			}
			bytesWritten = write(fdTarget, (char*)buffer, bytesRead);
			if(bytesWritten < 0)
			{
				perror("Error with write in the child\n");
				exit (-1);
			}
		}
		
		close(fdTarget);
		close(p[0]);
		exit(0);
	}
	else 
	{
		/*Código del padre*/
		//Cerramos el lado de lectura de la pipe
		close(p[0]);
		
		char source[SIZE], target[SIZE];
		char buffer[SIZE];

		printf("Introduzca nombre de fichero origen y destino: ");
		scanf("%s %s", source, target);
	
		//Abrimos el fichero
		int fdSource = open(source, O_RDONLY);
		if (fdSource < 0)
		{
			perror("Problem opening source file\n");
			exit(-1);
		}
		
		//Enviamos el nombre del fichero destino al hijo
		int bytesWritten;
		bytesWritten = write(p[1], target, strlen(target)+1);
		if (bytesWritten < 0)
		{
			perror("Error with write in the father\n");
			exit(-1);
		}
		

		int bytesRead;
		while((bytesRead = read(fdSource, (char*)buffer, SIZE)) != 0)
		{
			bytesWritten = write(p[1], (char*)buffer, bytesRead);
			if (bytesWritten < 0)
			{
				perror("Error in write in father\n");
				exit(-1);
			}
			printf("The father sent %d\n", bytesWritten);
		}
		close(p[1]);
		close(fdSource);

	}
	

	return 0;
}
