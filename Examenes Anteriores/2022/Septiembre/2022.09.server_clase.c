#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/select.h>
#include <signal.h>

#define PUERTO 4950
#define T 16

struct Solicitud {
	uint8_t tipo;
	uint16_t muestras;
};

struct Respuesta {
	uint8_t tipo;
	uint32_t payload[4];
};

ssize_t read_n(int fd, void * mensaje, size_t longitud_mensaje);
ssize_t write_n(int fd, void * mensaje, size_t longitud_mensaje);
int enviar_respuesta(int sd, struct Respuesta * r, int n_bytes);
int recibir_solicitud(int sd, struct Solicitud * s, int n_bytes);


int main(int argc, char * argv[]) {
	
	//1.- Declaramos variables
	int n_sd,sd; //descriptores de socket
	//direcciones de socket
	struct sockaddr_in serv_addr;
	struct sockaddr_in cli_addr;
	
	//2.- Creamos la dirección del servidor
	//2.1.- Inicializamos
	memset(&serv_addr, 0, sizeof(serv_addr));
	//2.2.- Rellenamos campos
	 //2.2.1.- Familia de direcciones
	 serv_addr.sin_family = AF_INET;
	 //2.2.2.- Puerto
	 serv_addr.sin_port = htons(PUERTO);
	 //2.2.3.- Dirección IP
	 serv_addr.sin_addr.s_addr = INADDR_ANY;
	 
	//3.- Creamos el socket
	if ((sd = socket(PF_INET, SOCK_STREAM, 0)) < 0 ) {
		perror("socket");
		exit(1);
	}
	
	//4.- Asociamos el socket al puerto: bind()
	if (bind(sd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0 ) {
		perror("bind");
		exit(1);
	}
	
	//5.- Ponemos el socket en escucha (modo pasivo): listen()
	if (listen(sd, 5) < 0)  {
		perror("listen");
		exit(1);
	}
	
	socklen_t addr_len = sizeof(cli_addr);
	while(1) {
		printf("Servidor esperando conexión ...\n");
		if ( (n_sd = accept(sd, (struct sockaddr *) &cli_addr, &addr_len )) < 0) {
			perror("accept");
			close(n_sd);
			close(sd);
			exit(1);
		}
		
		//si llegamos aquí, hay que aceptar al siguiente cliente
		//liberamos recursos
		if (close(n_sd) < 0) {
			perror("close");
			exit(sd);
			exit(1);
		}

		
	}
	
	return 0;
}

/**
 * Funciones auxiliares
 */
ssize_t read_n(int fd, void * mensaje, size_t longitud_mensaje) {
  ssize_t a_leer = longitud_mensaje;
  ssize_t total_leido = 0;
  ssize_t leido;
  
  do {
    errno = 0;
    leido = read(fd, mensaje + total_leido, a_leer);
    if (leido >= 0) {
      total_leido += leido;
      a_leer -= leido;
    }
  } while((
      (leido > 0) && (total_leido < longitud_mensaje)) ||
      (errno == EINTR));
  
  if (total_leido > 0) {
    return total_leido;
  } else {
    /* Para permitir que devuelva un posible error en la llamada a read() */
    return leido;
  }
}

ssize_t write_n(int fd, void * mensaje, size_t longitud_mensaje) {
  ssize_t a_escribir = longitud_mensaje;
  ssize_t total_escrito = 0;
  ssize_t escrito;
  
  do {
    errno = 0;
    escrito = write(fd, mensaje + total_escrito, a_escribir);
    if (escrito >= 0) {
      total_escrito += escrito ;
      a_escribir -= escrito ;
    }
  } while(
        ((escrito > 0) && (total_escrito < longitud_mensaje)) ||
        (errno == EINTR));
  
  if (total_escrito > 0) {
    return total_escrito;
  } else {
    /* Para permitir que devuelva un posible error de la llamada a write */
    return escrito;
  }
}


int enviar_respuesta(int sd, struct Respuesta * r, int n_bytes) {
	
	return n_bytes;
} 

int recibir_solicitud(int sd, struct Solicitud * s, int n_bytes) {

	return n_bytes;
}
