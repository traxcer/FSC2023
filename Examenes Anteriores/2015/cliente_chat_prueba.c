#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 

const char ip[] = "127.0.0.1" ; /* ip del servidor */
const uint16_t PORT = 5000; /* puerto de escucha del servidor */
#define LONGBUFFER 1024 /* longitud del buffer */

int main() {
	struct sockaddr_in sad ; /* estructura para guardar una direccion IP */
	int port ; /* numero de puerto */
	int sd ; /* descriptor de socket */
	int n ; /* numero de caracteres leidos */
	char buffer[LONGBUFFER] ; /* buffer para la lectura de datos desde el socket */
	/* Creacion del socket */
	sd = socket(PF_INET, SOCK_STREAM, 0) ;
	if (sd < 0) {
		perror("Cliente TCP: creacion del socket fallida \n");
		exit(1);
	}
	memset((char *)&sad,0,sizeof(sad)); /* limpia la estructura sockaddr */
	sad.sin_family = AF_INET; /* familia de Internet */
	sad.sin_port = htons(PORT);
	uint32_t ipservidor = inet_addr(ip);
	memcpy(&sad.sin_addr, &ipservidor, 4);
	/* Conecta el socket al servidor especificado */
	if (connect(sd, (struct sockaddr *)&sad, sizeof(sad)) < 0) {
		perror("Cliente TCP: conexion fallida \n") ;
		exit(1) ;
	}
	/* Le pedimos al usuario que introduzca una cadena por teclado */
	printf("Cliente TCP conectado y haciendo login\n\n") ;
	/*login con usuario y password*/

	char msg_usr[] = "USR alumno";
	char msg_pass[] = "PASS 12345";
	
	send(sd, msg_usr, strlen(msg_usr), 0) ;
	n = recv(sd, buffer, LONGBUFFER, 0) ;
	buffer[n] = '\0' ;
	printf("%s\n", buffer) ;
	
	send(sd, msg_pass, strlen(msg_pass), 0) ;
	n = recv(sd, buffer, LONGBUFFER, 0) ;
	buffer[n] = '\0' ;
	printf("%s\n", buffer) ;
	
	fd_set desc,copia_desc;
	FD_ZERO(&desc);
	FD_SET(0,&desc);
	FD_SET(sd,&desc);
	while (1) {
		memcpy(&copia_desc,&desc,sizeof(copia_desc));
		int r = select(sd+1,&copia_desc,NULL,NULL,NULL);
		if(r < 0){
		  perror("select");
		  exit(1);
		}
		if(FD_ISSET(sd,&copia_desc)){
		  n = recv(sd, buffer, LONGBUFFER, 0) ;
		  if(n==0)
		    break; //sale del bucle por desconexion del otro extremo
		  buffer[n] = '\0' ;
		  printf("Recibido: %s\n", buffer) ;
		}
		if(FD_ISSET(0,&copia_desc)){
		  gets(buffer);
		/* Envia la cadena al servidor... */
		  send(sd, buffer, strlen(buffer), 0); //falta comprobar que se han mandado todos los bytes
		}
	}
	/* Cierre del socket */
	close(sd) ;
	/* Termina el programa cliente satisfactoriamente */
	return 0;
} /* fin main */ 