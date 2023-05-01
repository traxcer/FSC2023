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
#include <ctype.h>

#define PUERTO 4950
#define T 512

ssize_t read_n(int fd, void *mensaje, size_t longitud_mensaje);
ssize_t write_n(int fd, void *mensaje, size_t longitud_mensaje);

struct PDU {
	int size;
	char payload[T];
};

int recibirPDU(int sd, struct PDU * pdu);
int enviarPDU(int sd, struct PDU * pdu);
void imprimirPDU(struct PDU * pdu);


int main(int argc, char *argv[])
{
	//1.- Dirección de socket donde escucha el servidor
	struct sockaddr_in dir_servidor;
	memset(&dir_servidor, 0, sizeof(dir_servidor));
	dir_servidor.sin_family = AF_INET;
	dir_servidor.sin_port = htons(PUERTO);
	dir_servidor.sin_addr.s_addr = INADDR_ANY;

	//2.- Creamos el socket de conexiones
	int sd_conexiones = socket(PF_INET, SOCK_STREAM, 0);
	if (sd_conexiones < 0) {
		perror("socket");
		exit(1);
	}

	//3.- Vinculamos el socket a la dirección de socket donde 
	//    escucha a los clientes
	if (bind(sd_conexiones, (struct sockaddr *) &dir_servidor, sizeof(dir_servidor)) < 0) {
		perror("bind");
		exit(1);
	}

	//4.- Ponemos el servidor en modo escucha para que atienda 
	//    a clientes
	if (listen(sd_conexiones, 5) < 0) {
		perror("listen");
		exit(1);
	}

	//5.- Rutina del servidor
	struct sockaddr_in dir_cliente;
	socklen_t long_dir_cliente = sizeof(dir_cliente);
	while(1) {
		printf("Servidor esperando conexión...\n");
		int sd_datos = accept(
			sd_conexiones,
			(struct sockaddr *) &dir_cliente,
			&long_dir_cliente
		);

		if (sd_datos < 0) {
			perror("accept");
			close(sd_conexiones);
			exit(1);
		}

		//Rutina de atención al cliente
		//-> recibe nombre y apellido hasta que el cliente se 
		//desconecte
		struct PDU pdu;
		int leidos = recibirPDU(sd_datos, &pdu);
		while (leidos > 0) {
			//imprimo la pdu
			imprimirPDU(&pdu);

			//proceso la primera pdu -> nombre
			//cambio la primer letra en mayúsculas
			pdu.payload[0] = toupper(pdu.payload[0]);

			//envío el nombre al cliente
			if (enviarPDU(sd_datos, &pdu) < 0) {
				close(sd_conexiones);
				close(sd_datos);
				exit(1);
			}

			//leo la pdu con el apellido
			if (recibirPDU(sd_datos, &pdu) < 0) {
				close(sd_conexiones);
				close(sd_datos);
				exit(1);
			}

			//la imprimo
			imprimirPDU(&pdu);

			//proceso la pdu con el apellido -> todo a mayúsculas
			for (int i = 0; i < pdu.size; i++)
				pdu.payload[i] = toupper(pdu.payload[i]);

			//envío la pdu al cliente
			if (enviarPDU(sd_datos, &pdu) < 0) {
				close(sd_conexiones);
				close(sd_datos);
				exit(1);
			}

			//vuelvo a leer
			leidos = recibirPDU(sd_datos, &pdu);
		} //while leidos > 0

		if (leidos < 0) {
			perror("read_n");
			close(sd_datos);
			close(sd_conexiones);
			exit(1);
		}

		//leidos = 0 
		if (close(sd_datos) < 0) {
			perror("close sd_datos");
			close(sd_conexiones);
			exit(1);
		}


	}

	//6.- Chapamos
	if (close(sd_conexiones) < 0) {
		perror("close");
		exit(1);
	}

	return 0;
}

/**
 * Funciones auxiliares
 */
ssize_t read_n(int fd, void *mensaje, size_t longitud_mensaje)
{
	ssize_t a_leer = longitud_mensaje;
	ssize_t total_leido = 0;
	ssize_t leido;

	do
	{
		errno = 0;
		leido = read(fd, mensaje + total_leido, a_leer);
		if (leido >= 0)
		{
			total_leido += leido;
			a_leer -= leido;
		}
	} while ((
				 (leido > 0) && (total_leido < longitud_mensaje)) ||
			 (errno == EINTR));

	if (total_leido > 0)
	{
		return total_leido;
	}
	else
	{
		/* Para permitir que devuelva un posible error en la llamada a read() */
		return leido;
	}
}

ssize_t write_n(int fd, void *mensaje, size_t longitud_mensaje)
{
	ssize_t a_escribir = longitud_mensaje;
	ssize_t total_escrito = 0;
	ssize_t escrito;

	do
	{
		errno = 0;
		escrito = write(fd, mensaje + total_escrito, a_escribir);
		if (escrito >= 0)
		{
			total_escrito += escrito;
			a_escribir -= escrito;
		}
	} while (
		((escrito > 0) && (total_escrito < longitud_mensaje)) ||
		(errno == EINTR));

	if (total_escrito > 0)
	{
		return total_escrito;
	}
	else
	{
		/* Para permitir que devuelva un posible error de la llamada a write */
		return escrito;
	}
}

/**
 * Función que recibe una PDU y devuelve:
 * 	-1: si ha habido algún error
 * 	 0: si el socket está cerrado
 *   n: el tamaño de la PDU en otro caso
 */
int recibirPDU(int sd, struct PDU * pdu) {
	//leer el tamaño del payload
	uint32_t size_big_endian;
	int leidos = read_n(sd, &size_big_endian, sizeof(size_big_endian));

	//Actuamos según el valor devuelto por read_n
	if (leidos < 0) {
		perror("read_n pdu->size");
		return -1;
	}

	if (leidos == 0) {
		return 0;
	}

	if (leidos != sizeof(size_big_endian)) {
		perror("read_n pdu->size");
		return -1;
	}

	//convierto el entero desde formato de red a formato de host
	pdu->size = ntohl(size_big_endian);

	//leer el payload
	if (read_n(sd, pdu->payload, pdu->size) != pdu->size ) {
		perror("read_n payload");
		return -1;
	}

	return sizeof(pdu->size) + pdu->size;
}

void pack_pdu(char * b, struct PDU * pdu) {
	uint32_t size_big_endian = htonl(pdu->size);
	memcpy(b, &size_big_endian, sizeof(size_big_endian));
	memcpy(b+sizeof(size_big_endian), pdu->payload, pdu->size);
}

/**
 * Función que envía una PDU por el socket 
 * en una única llamada a write_n().
 * La función devuelve:
 * 	-1: en caso de error
 *   n: el nº de bytes escritos en otro caso
 */
int enviarPDU(int sd, struct PDU * pdu) {
	int buffer_size = sizeof(int) + pdu->size;
	char b[buffer_size];
	
	//empaqueto en el buffer
	pack_pdu(b, pdu);

	//envío el buffer
	if (write_n(sd, b, buffer_size) != buffer_size){
		perror("write_n buffer");
		return -1;
	}

 	return buffer_size;
}


void imprimirPDU(struct PDU * pdu) {
	pdu->payload[pdu->size] = '\0';
	printf("PDU = %s (%d)\n",pdu->payload, pdu->size);
}
