#include <unistd.h>
#include <stdio.h>
#include <string.h>

/*estados*/
#define OCIOSO 0
#define FILTRANDO 1
#define APAGON 2

/*eventos*/
#define OFF 0
#define ON 1
#define MENSAJE 2
#define FIN 3

/*cadena global, compartida por espera_evento y la maquina de estados*/
char mensaje[100];

int espera_evento(){
	int leidos = read(0,mensaje,99);
	mensaje[leidos-1] = '\0'; //convierte en cadena y quita el '\n' del buffer de teclado
	if(strcmp("/on",mensaje)==0){
		return ON;
	}else if(strcmp("/off",mensaje)==0){
		return OFF;
	}else if(strcmp("/fin",mensaje)==0){
		return FIN;
	}else{
		return MENSAJE;
	}
}

int main(){
	int estado = OCIOSO;
	int evento;
	int tacos = 0;
	int fin = 0;
	char *posicion;
	while(!fin){
	
		printf("(ESTADO %i) Introduzca comando [/on, /off, /fin] o mensaje:\n",estado);
		evento = espera_evento();
		switch(estado){
			case OCIOSO: 	switch(evento){
								case ON: 	estado = FILTRANDO;
											break;
								case MENSAJE: 	printf("Mensaje: %s\n",mensaje);
												break;
								case FIN:	fin = 1;
											break;
								default:	printf("Evento %d no esperado en OCIOSO\n",evento);				
											break;
							}
							break;
							
			case FILTRANDO: switch(evento){
								case MENSAJE: 	posicion = strstr(mensaje, "taco");
												while(posicion!=NULL){
													int i;
													for(i=0;i<4;i++){
														*(posicion+i) = 'x';
													} /* equivale a: strncpy(posicion,"xxxx",4); */ 
													tacos++;
													posicion = strstr(posicion, "taco");
												}
												printf("Mensaje: %s\n",mensaje);
												if(tacos >= 3){
													tacos = 0;
													estado = APAGON;
												}
												break;
								case OFF:	estado = OCIOSO;
											tacos = 0;
											break;
								case FIN:	fin = 1;
											break;
								default:	printf("Evento %d no esperado en FILTRANDO\n",evento);				
											break;
							}
							break;
							
			case APAGON: 	switch(evento){
								case OFF: 	estado = FILTRANDO;
											break;
								case MENSAJE: 	printf("<usted no vera nada!>\n");
												break;
								case FIN:	fin = 1;
											break;
								default:	printf("Evento %d no esperado en APAGON\n",evento);	
											break;			
							}
							break;
			default:	printf("Estado %d no esperado!!\n",estado);
						break;
						
		}
	}//end del while
	return 0;
}