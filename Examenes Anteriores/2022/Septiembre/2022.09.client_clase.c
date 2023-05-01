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
#include <fcntl.h>
#include <netdb.h>
#include <sys/select.h>

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
void copia_fdset(fd_set * dst, fd_set * origen, int m_m_1);
int enviar_solicitud(int sd, struct Solicitud * s, int n_bytes);
int recibir_respuesta(int sd, struct Respuesta * r, int n_bytes);

int main(int argc, char * argv[]) {
	//1.- Procesamos los argumentos
	if (argc < 4) {
		printf("Uso: %s <IP> <PUERTO> <muestras>\n",argv[0]);
		exit(1);
	}
	
	//2.- Declaración de variables
	//descriptor de socket
	int sd;
	//dirección del servidor
	struct sockaddr_in serv_addr;
	//longitudes
	
	//3.- Montamos la dirección del servidor
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port   = htons(atoi(argv[2]));
	uint32_t ip_servidor;
	if ((ip_servidor = inet_addr(argv[1])) < 0) {
		printf("Error al convertir la IP %s\n", argv[1]);
		exit(1);
	}
	memcpy(&serv_addr.sin_addr, &ip_servidor, sizeof(ip_servidor));
	//serv_addr.sin_addr.s_addr = ip_servidor;
	
	//4.- Creamos el socket
	if ((sd = socket(PF_INET, SOCK_STREAM, 0)) < 0 ) {
		perror("socket");
		exit(1);
	}	
	
	//5.- Abrimos la conexión con el servidor
	if (connect(sd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0 ) {
		perror("connect");
		exit(1);
	}

	//========> 1. Enviamos la solicitud al servidor
	struct Solicitud s;
	s.tipo = 1;
	s.muestras = atoi(argv[3]);
	int tam_s = sizeof(s.tipo)+sizeof(s.muestras);
	if (enviar_solicitud(sd, &s,tam_s)!= tam_s){
		close(sd);
		exit(1);
	}

	//========> Iterar esperando respuestas del servidor y/o lectura de estado del cliente
	fd_set original, copia;
	FD_ZERO(&original);
	FD_SET(sd, &original);
	FD_SET(0,&original);

	//variables del bucle
	char b[T];
	char show= 'a';

	while(1){
		//1. Hacemos copia del conjunto de descriptores
		copia_fdset (&copia, &original, sd+1); //nuestro máximo es sd

		//2. Llamamos a select()
		if (select(sd+1, &copia, NULL, NULL, NULL)<0){
			perror("select");
			close (sd);
			exit(1);
		}
		// Si desbloqueo el select puede ser por:
		//3.1 El usuario escribe por el teclado
		if (FD_ISSET(0, &copia)){
			int leidos = read(0,b,T-1);
			b[leidos-1]='\0';
			if (strncmp(b,"/show",5)==0){ //comparo 5 caracteres (bytes)
				show = b[6];
			} else {
			printf("Comando Incorrecto: %s\n",b);
			close(sd);
			exit(1);
			}
		}
		//3.2 El servidor ha enviado datos por el socket
		if (FD_ISSET(sd, &copia)){
			struct  Respuesta r;
			int r_size = sizeof(r.tipo)+ 4*sizeof(uint32_t);
			int leidos = recibir_respuesta(sd, &r, r_size);
			if (leidos <0){
				close(sd);
				exit(1);
			} else if (leidos ==0){
				//soket desconectado
				printf("Fin de la monitorización\n");
				break;
			} else if (leidos != r_size){ //es igual que la condicion (leidos < 0)
				close (sd);
				exit(1);
			}

			//aqui ya tenemos todos los datos que esperabamos leer
			if (r.tipo == 1){ 
				//respuesta correcta
				uint32_t total = r.payload[0];
				uint32_t free = r.payload[1];
				uint32_t shared = r.payload[2];
				uint32_t buffered = r.payload[3];

				switch (show){
				case 'a':
					printf("Memory: %d %d %d %d\n",total, free, shared,buffered);
					break;
				case 't':
					printf("Total memory: %d\n",total);
					break;
				case 'f':
					printf("Free memory: %d\n",free);
					break;
					
				case 's':
					printf("Shared memory: %d\n",shared);
					break;
				case 'b':
					printf("Buffered memory: %d\n",buffered);
					break;
				default:
					printf("Código de visualización incorrecto: %c\n", show);
					break;
				}

			} else {
				printf("Respuesta incorrecta recibida del servidor\n");
			}
		}
		printf("Introduzca el modo de visualización: \\show [t|f|s|b|a]:\n");
	}


	//7.- Cerramos
	if (close(sd) < 0) {
		perror("close");
		exit(1);
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

void copia_fdset(fd_set * dst, fd_set * origen, int m_m_1) {
	FD_ZERO(dst);
	for (int i = 0; i < m_m_1; i++) 
		if (FD_ISSET(i,origen))
			FD_SET(i,dst);
}

int enviar_solicitud(int sd, struct Solicitud * s, int n_bytes) {
//===========> Implemento
	char b[n_bytes];
	char *aux = b;
	memcpy(aux, &s->tipo, sizeof(s->tipo));
	aux += sizeof(s->tipo);
	s->muestras = htons(s->muestras); //Ojo es un uint_16
	memcpy(aux, &s->muestras, sizeof(s->muestras));
	//enviamos b empaquetado
	int escritos = write_n(sd,b,n_bytes);
	if (escritos !=n_bytes)	{
		perror("enviar solicitud");
		return escritos;
	}
	return n_bytes;
} 

int recibir_respuesta(int sd, struct Respuesta * r, int n_bytes) {
//===========> Implemento
	char b[n_bytes];
	char *aux = b;
	int leidos = read_n(sd,b,n_bytes);
	if (leidos==0){
		//socket desconectado
		return 0;
	}
	if (leidos!= n_bytes){
		perror("read_n recibir respuestas");
		return leidos;
	}
	//desempaquetamos la info
	memcpy(&r->tipo, aux, sizeof(r->tipo));
	aux+= sizeof(r->tipo);
	memcpy(r->payload, aux, 4*sizeof(uint32_t));
	//convertimos desde formato de red a formato de host
	for( int i=0; i<4;i++)
		r->payload[i]= ntohs(r->payload[i]);
	return n_bytes;
}