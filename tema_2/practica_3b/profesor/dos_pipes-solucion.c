#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[]){

	if (argc < 2){
		printf("Error! uso del programa: %s limite_bytes\n",argv[0]);
		exit(1);
	}
	
	int fd_pipe_p_a_h[2]; /*Pipe en sentido de padre a hijo*/
	int fd_pipe_h_a_p[2]; /*Pipe en sentido de hijo a padre*/
	int limite_bytes = atoi(argv[1]); /* es el limite de bytes a imprimir por el hijo en pantalla*/
	char mensaje[512];
	char c;

	/*1. ignora la señal SIGCLD para no hacer wait en el codigo*/
	signal(SIGCLD,SIG_IGN);
	
	/*2. crea una pipe y almacena los descriptores de lectura y escritura en fd_pipe*/
	int r1 = pipe(fd_pipe_p_a_h);
	int r2 = pipe(fd_pipe_h_a_p);	
	
	if((r1 < 0) || (r2 < 0)){
		perror("Fallo en creacion de pipes");
		exit(1);
	}
	/*3. Clona el proceso*/
	pid_t p = fork(); 
	if(p > 0){ /*quien va a ejecutar este codigo? PADRE*/
		/*4. Cierra los descriptores de pipe que no se van a usar*/
		close(fd_pipe_p_a_h[0]); /*Cierro el de lectura ya que el padre usa el de escritura para la pipe*/
		close(fd_pipe_h_a_p[1]); /*Cierro el descriptor de escritura porque aqui solo voy a leer*/
		do{
			/*5. lee de teclado un mensaje de usuario (chequea el valor de retorno de read!)*/
			int leidos = read(0,mensaje,512);
			if (leidos == -1){
				perror("Error en el read de teclado");
				exit(1);
			}
			/*6. escribe en la tuberia el mensaje (chequea el valor de retorno de write!)*/
			int wr = write(fd_pipe_p_a_h[1],mensaje,leidos);
			if (wr == -1){
				perror("Error al escribir en la pipe");
				exit(1);
			}
			
			/*lectura del caracter para continuar o no, que manda el hijo por la otra pipe*/
			int r = read(fd_pipe_h_a_p[0],&c,1);
			printf("chequeo padre: leido en pipe de respuesta: %c\n",c);
			if (r == -1){
				perror("Error en el read de la pipe del hijo");
				exit(1);
			}

		}while(c != 'n');

		/*wait(0);*/ /*aqui seria el momento de esperar el fin del hijo, pero como hemos ignorado la senal no hace
		falta*/
		
		/*7. cierra los descriptores de pipes que faltan*/
		close(fd_pipe_p_a_h[1]);
		close(fd_pipe_h_a_p[0]);
	}else if (p == 0){ /*quien va a ejecutar este codigo? HIJO*/
		/*8. Cierra los descriptores de pipe que no se van a usar*/
		close(fd_pipe_p_a_h[1]);
		close(fd_pipe_h_a_p[0]);
		
		int total=0; /*contador de bytes leidos*/
		while(1){
			/*9. lee por la pipe el mensaje (chequea el valor de retorno de read!)*/	
			int leidos = read(fd_pipe_p_a_h[0],mensaje,512);
			if (leidos == -1){
				perror("Error en el read de la pipe");
				exit(1);
			}
			/*10. escribe en pantalla el mensaje (chequea el valor de retorno de write!)*/
			int wr = write(1,mensaje,leidos);
			if (wr == -1){
				perror("Error al escribir en pantalla");
				exit(1);
			}
			total+=leidos;
			printf("chequeo hijo: total de bytes leidos= %d\n", total);
			if(total >= limite_bytes){
				c = 'n';
				int w = write(fd_pipe_h_a_p[1],&c,1);
				if (w == -1){
					perror("Error al escribir en la pipe del hijo al padre");
					exit(1);
				}
				//close(fd_pipe_h_a_p[1]);
				break;
			}else{
				c = 'y';
				int w = write(fd_pipe_h_a_p[1],&c,1);
				if (w == -1){
					perror("Error al escribir en la pipe del hijo al padre");
					exit(1);
				}
				
			}
		}
		/*11. cierra los descriptores de las pipes que faltan*/
		close(fd_pipe_p_a_h[0]);
		close(fd_pipe_h_a_p[1]);

		exit(0);
		
	}else if(p == -1){
		perror("Ha fallado el fork");
		exit(1);
	}
	return 0;

}