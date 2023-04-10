#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>

#define ESTADO_1 0
#define ESTADO_2 1

#define EVENTO_1 1
#define EVENTO_2 2
#define EVENTO_SALIR 3

/**
 * Funciones auxiliares
 */
char * strEstado(int estado);
char * strEvento(int e);
ssize_t read_n(int fd, void * mensaje, size_t longitud_mensaje);
ssize_t write_n(int fd, void * mensaje, size_t longitud_mensaje);

/**
 * Variable global 
 */
int p[2];

void m1(int signo) {
	int e = EVENTO_1;
	write(1,"Llega SIGUSR1\n",14);
	if (write_n(p[1], &e, sizeof(int)) != sizeof(int)) {
		perror("write_n");
		exit(-1);
	}
	signal(SIGUSR1,m1);
}

void m2(int signo) {
	write(1,"Llega SIGUSR2\n",14);
	int e = EVENTO_2;
	if (write_n(p[1], &e, sizeof(int)) != sizeof(int)) {
		perror("write_n");
		exit(-1);
	}
	signal(SIGUSR2,m2);
}

void m3(int signo) {
	write(1,"Llega SIGINT\n",14);
	int e = EVENTO_SALIR;
	if (write_n(p[1], &e, sizeof(int)) != sizeof(int)) {
		perror("write_n");
		exit(-1);
	}
	signal(SIGINT,m3);
}

int espera_evento() {
	int evento_recibido;
	if (read_n(p[0], &evento_recibido, sizeof(int)) != sizeof(int)) {
		return -1;
	}
	return evento_recibido;
}

int main(int argc, char * argv[]) {
	
	//1.- registro de señales
	if (signal(SIGUSR1,m1) == SIG_ERR) {
		perror("signal");
		exit(-1);
	}

	if (signal(SIGUSR2,m2) == SIG_ERR) {
		perror("signal");
		exit(-1);
	}
	
	if (signal(SIGINT,m3) == SIG_ERR) {
		perror("signal");
		exit(-1);
	}

	int estado = ESTADO_1;
	int evento;
	int fin = 0;

	if (pipe(p) < 0) {
		perror("pipe");
		exit(-1);
	}

	printf("pid = %d\n", getpid());

	while (fin == 0) {
		printf("Estado: %s. Esperando evento...\n",strEstado(estado));
		evento = espera_evento();
		printf("\t-> evento recibido: %s\n",strEvento(evento));
		switch (estado) {
			case ESTADO_1:
				switch (evento) {
					case EVENTO_1: 
						break;
					case EVENTO_2:
						estado = ESTADO_2;
						printf("\t-> tránsito a %s\n", strEstado(estado));
						break;
					case EVENTO_SALIR: 
						printf("Saliendo \n");
						fin = 1;
						break;
					default: 
						printf("Evento no esperado: %d\n", evento);
						exit(-1);
				}
 				break;
			
			case ESTADO_2: 
				switch (evento) {
					case EVENTO_1:
						estado = ESTADO_1;
						printf("\t-> tránsito a %s\n", strEstado(estado));
						break;
					case EVENTO_2:
						break;
					case EVENTO_SALIR: 
						printf("Saliendo \n");
						fin = 1;
						break;
					default: 
						printf("Evento no esperado: %d\n", evento);
						exit(-1);
				}
 				break;
			
			default:
				printf("Estado no esperado: %d\n", estado);
				exit(-1);
		}

	}

	//Se liberan recursos
	printf("Fin de la máquina\n");
	
	return 0;
}


void procesamiento(int s) {
	struct timeval ti, tf;
	int fin = 0;

	gettimeofday(&ti, NULL);
	while (fin == 0) {
		gettimeofday(&tf, NULL);
		double tiempo = (tf.tv_sec - ti.tv_sec)*1000 + (tf.tv_usec - ti.tv_usec)/1000.0;
		if (tiempo >= s*1000) {
			fin = 1;
		}
	}
}

char * strEstado(int estado) {
	switch(estado) {
		case ESTADO_1: 
			return "ESTADO_1";

		case ESTADO_2: 
			return "ESTADO_2";
		
		default:
			return "Estado no válido";
	}
}

char * strEvento(int e) {
	switch(e) {
		case EVENTO_1: 
			return "SIGUSR1";

		case EVENTO_2: 
			return "SIGUSR2";
		
		case EVENTO_SALIR: 
			return "SIGINT";
		
		default:
			return "Evento no válido";
	}
}

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
