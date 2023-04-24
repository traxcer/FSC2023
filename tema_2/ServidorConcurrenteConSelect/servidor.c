/*
* Programa: servidor.c
*
* Descripción: servidor concurrente de ECO sobre TCP
*
* Sintaxis: servidor
*
* Nota: Usando multiplexacion con select()
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

#define PROTOPORT 5200 /* puerto por defecto */
#define MAX_CLIENTES 4
#define TAM_BUFFER 1024



int get_max_fd(int actual, int nuevo){
	if(actual >= nuevo)
		return actual;
	else
		return nuevo;
}

int main(){
	int sd, nuevos_sd[MAX_CLIENTES];
	int n, max_descriptor;
    socklen_t clilen;
	int i,j;
	struct sockaddr_in cli_addr, serv_addr ; //declara direccion cliente y servidor
	char buffer[TAM_BUFFER] ;
	char nombre_host[200] ;
	fd_set cjto_descriptores, cjto_modificado ;

	gethostname(nombre_host, sizeof(nombre_host)) ; //función de libreria
	printf("Servidor TCP: el nombre del host actual es %s \n", nombre_host) ;

	for(i = 0; i < MAX_CLIENTES; i++) { //pone el array de clientes con todos a -1
		nuevos_sd[i] = -1 ;
	}

	sd = socket(PF_INET, SOCK_STREAM, 0) ; //crea el socket y obtiene sd
	if (sd < 0) {
		perror("Servidor TCP: creacion del socket fallida\n");
		exit(1) ;
	}

	memset((char *)&serv_addr, 0, sizeof(serv_addr)); //pone el socket a 0
	serv_addr.sin_family = AF_INET ; //asigna valores a la dirección del servidor
	serv_addr.sin_addr.s_addr = INADDR_ANY ;
	serv_addr.sin_port = htons(PROTOPORT) ;

	if (bind(sd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) { 
		perror("Servidor TCP: bind fallido\n") ; //vincula el sd a la dirección del servidor
		exit(1) ;
	}

	if (listen(sd, MAX_CLIENTES)< 0) { //Escucha hasta MAX_CLIENTES (4)
		perror("Servidor TCP: listen fallido\n") ;
		exit(1) ;
	}
	
	FD_ZERO(&cjto_descriptores) ; //limpia el conjunto de descriptores para select
	FD_SET(sd, &cjto_descriptores) ; //añade el socket al conjunto de descritores
	max_descriptor = sd;

	while (1){
		printf("Descriptores: ") ;
		for (i = 0; i < MAX_CLIENTES; i++) {
			printf("%d ", nuevos_sd[i]) ;
		}
		printf("\n") ;
        //copia el conjunto de descriptores
		memcpy(&cjto_modificado,&cjto_descriptores,sizeof(fd_set));

        //llama a select
		int resultado = select(max_descriptor+1, &cjto_modificado, NULL, NULL, NULL) ;
		if(resultado < 0){
			close(sd);
			perror("select");
			exit(1);
		}
		if(FD_ISSET(sd, &cjto_modificado)) { /* if X */
			for (i = 0; i < MAX_CLIENTES; i++) {
				if (nuevos_sd[i] == -1) {
					break ;
				}
			}
			if (i < MAX_CLIENTES) {
				printf("nueva conexion en la posicion %d\n",i);
				nuevos_sd[i] = accept(sd, (struct sockaddr *)&cli_addr, &clilen) ;
				if (nuevos_sd[i] < 0) {
					perror("Servidor TCP: accept fallido\n") ;
					exit(1) ;
				}
				printf("aceptada conexion: socket=%d (max_descriptor old=%d)\n",nuevos_sd[i],max_descriptor);
				max_descriptor = get_max_fd(max_descriptor,nuevos_sd[i]);
				printf("(max_descriptor nuevo=%d)\n",max_descriptor);

				FD_SET(nuevos_sd[i],&cjto_descriptores);
				printf("Servidor TCP: nuevo cliente incorporado al servicio de chat ==>\n") ;
				printf("Servidor TCP: conectado al cliente con direccion %s \n",
				inet_ntoa(cli_addr.sin_addr)) ;
			}
			else{
				printf("Servidor TCP: No hay sitio para el cliente\n") ;
			}
		}
		for (i = 0; i < MAX_CLIENTES; i++) {
			if((nuevos_sd[i] != -1) && FD_ISSET(nuevos_sd[i], &cjto_modificado) ) {
				n = read(nuevos_sd[i], buffer, TAM_BUFFER) ;
				if(n > 0){
					for (j = 0; j < MAX_CLIENTES; j++) {
						if((j!=i) && (nuevos_sd[j]!=-1))
							write(nuevos_sd[j], buffer, n) ;
					}
				}else { 
					close(nuevos_sd[i]) ;
					FD_CLR(nuevos_sd[i], &cjto_descriptores) ; 
					nuevos_sd[i] = -1 ; 
				}
			} /* if */
		} /* for */
		 /* fin if */
	} /* while */
} /* main */