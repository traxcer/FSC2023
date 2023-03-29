#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>

#define OCIOSO 0
#define FILTRANDO 1

/*eventos*/
#define OFF 0
#define ON 1
#define MENSAJE 2
#define FIN 3

#define T 32

int espera_evento(int fd)
{
    int b[T+1];
    int leidos= read(fd,b,T);
    if (leidos<0){
        perror("read");
        exit(1);
    }
    b[leidos-1]='\0';
    if ((strcmp(b,"/on"))==0){
        return ON;
    }else if (strcmp(b,"/off")==0){
        return OFF;
    }else if (strcmp(b,"/fin")==0){
        return FIN;
	return MENSAJE;
    }
}
int main(int argc, char *argv[]){
    if (argc<2){
        printf("Uso: %s <tuberia>\n", argv[0]);
        exit (1);
    }
    //abrimos la fifo
    int fd;
    if((fd=open(argv[1],O_RDONLY))<0){
        perror("open");
        exit(1);
    }

	int estado = OCIOSO;
	int evento;
    int fin=0;

	while (!fin)
	{
        printf("Estados %d: introduzca comando (/on, /off, /fin)");
		evento = espera_evento(fd);
		switch (estado)
		{
		case OCIOSO:
			switch (evento)
			{
			case ON:
                printf("Transio a FILTRANDO\n");
                estado = FILTRANDO;
				break;
			case MENSAJE:
            printf("Mesaje: %s\n", b)
				break;
			case FIN:
				break;
			default:
				printf("Evento %d no esperado en OCIOSO\n", evento);
				break;
			}
			break;

		case FILTRANDO:
			switch (evento)
			{
			case MENSAJE:
                //Puede aparecer varias veces "taco", usar strncpy()
				break;
			case OFF:
				break;
			case FIN:
				break;
			default:
				printf("Evento %d no esperado en FILTRANDO\n", evento);
				break;
			}
			break;
		default:
			printf("Estado %d no esperado!!\n", estado);
			break;
		}
	} //end del while
	return 0;
}