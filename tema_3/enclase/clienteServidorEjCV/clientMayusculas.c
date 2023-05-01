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

/**
 * Cliente TCP que envía peticiones a un servidor cuya IP se introduce
 * como argumento del main y que escucha en el puerto 4950. 
 * Para realizar dichas peticiones, el cliente 
 * solicita al usuario nombres y apellidos por teclado y los 
 * envía hasta que éste introduce la cadena "fin" en el nombre. 
 * La cadena "fin" no se envía al servidor.
 * Entonces se desconecta y finaliza la conexión.
 */
int main(int argc, char *argv[])
{
	if (argc < 2) {
		printf("Uso: %s <ip>\n", argv[0]);
		exit(1);
	}

	//1.- Montamos la dirección de socket para conectar con 
	//    el servidor
	struct sockaddr_in dir_servidor;
	memset(&dir_servidor, 0, sizeof(dir_servidor));
	dir_servidor.sin_family = AF_INET;
	dir_servidor.sin_port = htons(PUERTO);
	uint32_t ip = inet_addr(argv[1]);
	if (ip < 0) {
		printf("IP incorrecta: %s\n", argv[1]);
		exit(1);
	}
	dir_servidor.sin_addr.s_addr = ip;

	//Creamos el socket TCP
	int sd = socket(PF_INET, SOCK_STREAM, 0);
	if (sd < 0) {
		perror("socket");
		exit(1);
	}

	//2.- Establecemos la conexión con el servidor
	if ((connect(sd, (struct sockaddr *) &dir_servidor, sizeof(dir_servidor)) ) < 0) {
		perror("connect");
		exit(1);
	}

	//3.- Rutina del servidor
	struct PDU pdu;
	printf("Introduzca su nombre:\n");
	int leidos = read(0,pdu.payload,T-1);
	pdu.payload[leidos - 1] = '\0';
	pdu.size = leidos - 1;

	while(strcmp(pdu.payload, "fin") != 0) {
		//enviar el nombre
		if (enviarPDU(sd, &pdu) < 0) {
			close(sd);
			exit(1);
		}

		//leo y envío el apellido
		printf("Introduzca su apellido:\n");
		leidos = read(0, pdu.payload, T-1);
		pdu.payload[leidos - 1] = '\0';
		pdu.size = leidos -1;

		if (enviarPDU(sd, &pdu) < 0) {
			close(sd);
			exit(1);
		}

		//esperamos respuesta del servidor
		//primero el nombre
		if (recibirPDU(sd, &pdu) < 0) {
			close(sd);
			exit(1);
		}
		imprimirPDU(&pdu);

		//ahora el apellido
		if (recibirPDU(sd, &pdu) < 0) {
			close(sd);
			exit(1);
		}
		imprimirPDU(&pdu);



		//volvemos a leer
		printf("Introduzca su nombre:\n");
		leidos = read(0,pdu.payload,T-1);
		pdu.payload[leidos - 1] = '\0';
		pdu.size = leidos - 1;
	}



	//4.- chapar
	if (close(sd) < 0) {
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
