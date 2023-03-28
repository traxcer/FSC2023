#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#define ESTADO_1 1
#define ESTADO_2 2
#define ESTADO_3 3

#define E_SIGUSR1 1
#define E_SIGUSR2 2
#define E_SIGINT  3

/**
 * Funciones auxiliares
 */
char * strEstado(int estado);
char * strEvento(int e);

/**
 * Variable global para notificar al programa principal
 * la llegada del evento
 */
int evento_recibido;


void m1(int signo) {
	evento_recibido = E_SIGUSR1;
	signal(SIGUSR1,m1);
}

void m2(int signo) {
	evento_recibido = E_SIGUSR2;
	signal(SIGUSR2,m2);
}

void m3(int signo) {
	evento_recibido = E_SIGINT;
}

int espera_evento() {
	pause();
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

	printf("pid = %d\n", getpid());

	int usr2;

	while (fin == 0) {
		printf("Estado: %s. Esperando evento...\n",strEstado(estado));
		evento = espera_evento();
		printf("\t-> evento recibido: %s\n",strEvento(evento));
		switch (estado) {
			case ESTADO_1:
				switch (evento) {
					case E_SIGUSR1: 
						break;
					case E_SIGUSR2:
						estado = ESTADO_2;
						printf("\t-> tránsito a %s\n", strEstado(estado));
						//reiniciamos el contador
						usr2 = 0;
						break;
					case E_SIGINT: 
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
					case E_SIGUSR1:
						estado = ESTADO_1;
						printf("\t-> tránsito a %s\n", strEstado(estado));
						break;
					case E_SIGUSR2:
						usr2 ++;
						printf("\t-> SIGUSR2 recibido %d veces\n", usr2);
						if (usr2 == 3) {
							estado = ESTADO_3;
							usr2 = 0;
							printf("\t-> tránsito a %s\n", strEstado(estado));
						}
						break;
					case E_SIGINT: 
						printf("Saliendo \n");
						fin = 1;
						break;
					default: 
						printf("Evento no esperado: %d\n", evento);
						exit(-1);
				}
 				break;

			case ESTADO_3: 
				switch (evento) {
					case E_SIGUSR1:
					case E_SIGUSR2:
						estado = ESTADO_1;
						printf("\t-> tránsito a %s\n", strEstado(estado));
						break;
					case E_SIGINT: 
						printf("Saliendo \n");
						fin = 1;
						break;
					default: 
						printf("Evento no esperado: %d\n", evento);
						exit(-1);
				}
 				break;
			
			default:
				printf("Estado no espera: %d\n", estado);
				exit(-1);
		}

	}

	//Se liberan recursos
	printf("Fin de la máquina\n");
	
	return 0;
}

char * strEstado(int estado) {
	switch(estado) {
		case ESTADO_1: 
			return "ESTADO_1";

		case ESTADO_2: 
			return "ESTADO_2";

		case ESTADO_3: 
			return "ESTADO_3";
		
		default:
			return "Estado no válido";
	}
}

char * strEvento(int e) {
	switch(e) {
		case E_SIGUSR1: 
			return "SIGUSR1";

		case E_SIGUSR2: 
			return "SIGUSR2";
		
		case E_SIGINT: 
			return "SIGINT";
		
		default:
			return "Evento no válido";
	}
}