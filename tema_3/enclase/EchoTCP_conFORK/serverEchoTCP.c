/*------------------------------------------------------------------------
* Programa: servidorEchoTCP.c
*
* Descripción: servidor concurrente de ECO sobre TCP
*
* Sintaxis: servidor
*
* Nota: Usando multiplexacion con fork()
*------------------------------------------------------------------------
*/
#include <unistd.h> 
#include <string.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <netdb.h> 
#include <errno.h>
#include <signal.h>

#define PUERTO 4994 /* puerto por defecto */
#define MAX_CLIENTES 5
#define TAM_BUFFER 1024

//Prototipo de funciones auxiliares
void espera_hijo(int signo);
ssize_t read_n(int fd, char *b, int n);
ssize_t write_n(int fd, char *b, int n);

void espera_hijo(int signo){
	printf("Hijo terminado\n");
	wait(0);
	signal(SIGCHLD, espera_hijo);
}

int main(){
	if(signal(SIGCHLD,espera_hijo)<0){
		perror("SIGCHLD");
		exit(1);
	}
	int sd_datos;
	
	char b[TAM_BUFFER] ;
	char nombre_host[200] ;
	int longBE, longitud;
	ssize_t leidos;

	gethostname(nombre_host, sizeof(nombre_host)) ;
	printf("Servidor TCP: el nombre del host actual es %s \n", nombre_host) ;
	
	//1.- Dirección del socket donde escucha el servidor
	struct sockaddr_in serv_addr;
	memset((char *)&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET ;
	serv_addr.sin_addr.s_addr = INADDR_ANY ;
	serv_addr.sin_port = htons(PUERTO) ;
	
	//2.- Creamos el socket de conexiones
	int sd_conexiones = socket(PF_INET, SOCK_STREAM, 0) ;
	if (sd_conexiones < 0) {
		perror("Servidor TCP: creacion del socket fallida\n");
		exit(1) ;
	}

	//3.- Vinculamos el socket a las direccion donde se escucha
	if (bind(sd_conexiones, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		perror("Servidor TCP: bind fallido\n") ;
		exit(1) ;
	}

	//4.- Ponemos el servidor en modo escucha a clientes
	if (listen(sd_conexiones, MAX_CLIENTES)< 0) {
		perror("Servidor TCP: listen fallido\n") ;
		exit(1) ;
	}
	
	//5.- Rutina del servideor
	struct sockaddr_in cli_addr;
	socklen_t long_cli_addr= sizeof(cli_addr);
	pid_t pid;
	while (1){
	    printf("Servidor esperando conexión...\n");
		sd_datos= accept(sd_conexiones, (struct sockaddr *)&cli_addr,&long_cli_addr);
		if (sd_datos<0){
			if (errno==EINTR){
				errno=0;
				continue;
			} else {
				perror("accept");
				close (sd_datos);
				close (sd_conexiones);
				exit(1);
			}
		}
		write(1,"Ya tengo un cliente conectado\n",31);
		//Al llegar aqui es que ya tengo un cliente conectado aqui creo el fork
			pid=fork();
			//¿Que puede pasar con ese pid?
			if (pid<0){
				perror("fork");
				//para no tener problemas con las conexiones debo cerrarlas antes
				close(sd_datos);
				close(sd_conexiones);
				exit(1);
			}
			if (pid>0){ //estoy en el padre
				//el padre debe cerrar sd_datos, para no acumular descriptores
				if (close(sd_datos)<0){
					perror("close sd_datos");
					exit(1);
				//wait(0); si el padre espera al hijo se bloquea y no admitirá mas conexiones
			} else 
				{ //estoy en el hijo
				/*leemos cabecera + mensaje hasta que el cliente se desconecte
					lectura adelantada + while;
					recibo la longitud en big endian (red)
				*/
				leidos=read_n(sd_datos,(char *)&longBE, sizeof(longBE));
				while (leidos>0){
					//proceso
					if (leidos !=sizeof(longBE)){
						perror("read_n lonBE");
						close(sd_datos);
						close(sd_conexiones);
						exit(1);
					}
					//ya tenemos la longitud en big endian
					//la convierto a formato de host
					longitud=ntohl(longBE);
					printf("longitud del mensaje = %d\n", longitud);
					//recibo el mensaje
					if(read_n(sd_datos,b,longitud) != longitud){
						perror("read_n mensaje");
						close(sd_datos);
						close(sd_conexiones);
						exit(1);
					}
					//devolvemos el mensaje al cliente (ECO)
					//prinmero la cabecera
					longBE= htonl(longitud);
					if(write_n(sd_datos,(char *)&longBE,sizeof(longBE))!= sizeof(longBE)){
						perror("write_n longBE");
						close(sd_datos);
						close(sd_conexiones);
						exit(1);
					}
					//enviamos los datos
					if(write_n(sd_datos,b,longitud)!=longitud){
						perror("write_n mensaje");
						close(sd_datos);
						close(sd_conexiones);
						exit(1);
					}

					if (leidos ==0 ){
						if(close(sd_datos)<0){
							perror("close sd_datos");
							exit(1);
						} 
					} else { //cubre leidos <0 y leidos!=sizeof(longBE)
						perror("read_n longBE");
						exit(1);
					}
					

				} //fin while leidos
				//salida ordenada del hijo
				exit(0);
				} //else del fork (hijo)
			} /* Fin del Padre*/
	} /* while */
} /* main */


ssize_t read_n(int fd, char *b, int n){
	int porleer=n;
	int total_leidos=0;
	int leidos;
	do{
	errno=0;
		if ((leidos=read(fd, b+total_leidos, porleer))>0){
			porleer=-leidos;
			total_leidos=+leidos;
		}
	} while (((leidos>0)&&(total_leidos<n))||(errno==EINTR));
	if (total_leidos>0)
		return total_leidos;
	else
		return leidos;	
}
ssize_t write_n(int fd, char *b, int n){
	int porescribir=n;
	int total_escritos=0;
	int escritos;
	do{
	errno=0;
		if ((escritos=read(fd, b+total_escritos, porescribir))>0){
			porescribir=-escritos;
			total_escritos=+escritos;
		}
	} while (((escritos>0)&&(total_escritos<n))||(errno==EINTR));
	if (total_escritos>0)
		return total_escritos;
	else
		return escritos;	
}