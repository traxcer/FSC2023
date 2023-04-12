/*
Ejercicio de pipes donde el padre le dice al hijo que imprima por pantalla lo que el usuario esta introduciendo por
teclado

*/

#include <unistd.h>
#include <signal.h>
#include <stdlib.h> /*para exit()*/
#include <string.h>

int main(){
	
	int fd_pipe[2];
	char mensaje[512];

	signal(SIGCLD,SIG_IGN); /*ignoro la señal SIGCLD para evitar hacer el wait() en el padre*/

	if(pipe(fd_pipe) < 0){
		perror("Fallo en pipe()");
		exit(1);
	}
	pid_t p = fork(); /*CLONACION*/
	if(p > 0){ /*este codigo lo va a ejecutar el padre*/
		close(fd_pipe[0]);
		do{
			int leidos = read(0,mensaje,511);
			write(fd_pipe[1],mensaje,leidos);
			mensaje[leidos]='\0'; /*para poder hacer el strcmp*/
		}while(strcmp(mensaje,"fin\n")!= 0);
		
		close(fd_pipe[1]);
	
	}else if (p == 0){ /*este codigo lo va a ejecutar el hijo*/
		close(fd_pipe[1]);
		while(1){
			int leidos = read(fd_pipe[0],mensaje,511);
			mensaje[leidos]='\0'; /*para poder hacer el strcmp*/
			if(strcmp(mensaje,"fin\n")== 0){
				break; /*sale del while*/
			}
			write(1,mensaje,leidos);
		}
		close(fd_pipe[0]);
	
	}else if(p == -1){
		perror("Ha fallado el fork");
		exit(1);
	}
	return 0;

}