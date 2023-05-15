/*servidor de chat con maquina de estados (version 1)*/
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>

ssize_t read_n(int fd, char *buffer, size_t n){
	size_t  nleft;
	ssize_t nread;
	char   *ptr;

	ptr = buffer;
	nleft = n;
	while (nleft > 0) {
		if ( (nread = read(fd, ptr, nleft)) < 0) {
             if (errno == EINTR)
                 nread = 0;      /* es un error por una senal, vuelve a leer */
             else
                 return (-1);
        } else if (nread == 0)
             break;              /* fin de fichero o de la comunicacion */

		 nleft -= nread;
         ptr += nread;
    }
    return (n - nleft);         /* devuelve >= 0 */
}


ssize_t write_n(int fd, const char *buffer, size_t n){
    size_t nleft;
    ssize_t nwritten;
    const char *ptr;

    ptr = buffer;
    nleft = n;
    while (nleft > 0) {
        if ( (nwritten = write(fd, ptr, nleft)) <= 0) {
            if (nwritten < 0 && errno == EINTR)
                nwritten = 0;   /* es un error por una senal, vuelve a leer */
            else
                return (-1);    /* error */
        }

        nleft -= nwritten;
        ptr += nwritten;
    }
    return (n);
}

const uint16_t PORT = 5000;
const int MAX_CLIENTES = 40;

#define estadoEsperaUSR 0
#define estadoEsperaPASS 1
/*funcion maquina de estados, input: socket,
 devuelve: 0 si el cliente ha entrado ok y -1 si ha fallado*/
int maquina_estados_login(int sockc){
  /*asumo mensajes de tamaño fijo para simplificar la recepcion*/
  /*USR alumno */
  /*PASS 12345 */  /*los dos mensajes de 10 caracteres!!*/
  char cadena[11];
  int estado = estadoEsperaUSR;
  int fin = 0;
  while(!fin){
    int leidos = read_n(sockc,cadena,10);
      if(leidos < 10){
	  return -1;
      }
      cadena[leidos] = '\0';
      printf("Recibido: %s\n",cadena);
    
    switch(estado){
      case estadoEsperaUSR:
	  if((strncmp("USR",cadena,3)==0)&&(strncmp("alumno",cadena+4,6)==0)){
	    write_n(sockc,"200 User ok\n",12);
	    estado = estadoEsperaPASS;
	  }else
	    write_n(sockc,"500 User unknown\n",17);
	  break;
      case estadoEsperaPASS:
	  if((strncmp("PASS",cadena,4)==0)&&(strncmp("12345",cadena+5,5)==0)){
	    write_n(sockc,"210 logged\n",11);
	    fin = 1;
	  }else
	    write_n(sockc,"510 Invalid password\n",21);
	  break;
    }
  }
  return 0;
}

int main(){
	int sd_clientes[MAX_CLIENTES];  
	int i;
	for(i=0;i<MAX_CLIENTES;i++)
		sd_clientes[i]= -1; /*hueco libre si vale -1*/
	int sd = socket(PF_INET, SOCK_STREAM,0);
	if(sd<0){ perror("socket"); exit(1); }
	
	struct sockaddr_in dirS;
	memset(&dirS,0,sizeof(dirS)); /*relleno la estructura, primero pongo a cero*/
	dirS.sin_family = AF_INET; /*man 7 ip para saber los campos*/
	dirS.sin_port = htons(PORT); /*ojo, siempre en big endian*/
	dirS.sin_addr.s_addr = INADDR_ANY;
	/*vincula el socket a un puerto para que transporte le haga llegar segmentos*/
	if(bind(sd,(struct sockaddr *)&dirS,sizeof(dirS)) < 0){
		perror("bind");
		exit(1);
	}
	
	if(listen(sd,5)<0){	/*pone el socket en modo pasivo para ser servidor*/
		perror("listen");
		exit(1);
	}
	
	fd_set desc, copia_desc;
	FD_ZERO(&desc);
	FD_SET(sd,&desc);
	/* calculamos el tamaño de la tabla de descriptores del proceso*/
	int dsize = getdtablesize(); /*es mas ineficiente, pero me olvido de estar calculando el maximo cada vez que llamo a select*/
	while(1){ /*comienza el bucle de servicio a clientes*/
	  memcpy(&copia_desc,&desc,sizeof(copia_desc));
	  int r = select(dsize, &copia_desc, NULL, NULL, NULL);
	  if(r < 0){
		perror("select");
		exit(1);
	  }	
	  if(FD_ISSET(sd,&copia_desc)){ /*solicitan una nueva conexion*/
		int indice = -1;
		for(i=0; i< MAX_CLIENTES;i++){
			if(sd_clientes[i]==-1){
				indice = i;
				break; /*sale del bucle*/
			}
		}
		if(indice!=-1){ /*si hay algun hueco libre acepta la conexion*/
			struct sockaddr_in dirCliente;
			socklen_t longdir = sizeof(dirCliente);
			sd_clientes[indice] = accept(sd,(struct sockaddr *)&dirCliente,&longdir);
			/*hace login*/
			if(maquina_estados_login(sd_clientes[indice]) < 0)
			  close(sd_clientes[indice]);
			else
			  FD_SET(sd_clientes[indice],&desc); /*nuevo socket para el select!!*/
			printf("login con exito!\n");
		}
	  }
	  /*chequeo de sockets conectados con clientes*/
	  for(i=0; i< MAX_CLIENTES;i++){
		if((sd_clientes[i]!=-1) &&(FD_ISSET(sd_clientes[i],&copia_desc))){ /*da el servicio de chat*/
			char mensaje[512];
			int leidos = read(sd_clientes[i],mensaje,512);
			if(leidos > 0){ /*retransmite al resto*/
				int j;
				for(j=0; j< MAX_CLIENTES; j++){
					if(sd_clientes[j]!=-1){
						int escritos = write_n(sd_clientes[j],mensaje,leidos);
						if(escritos < 0){ perror("write_n chat"); exit(1); }
					}
				}
				
			}else if(leidos == 0){ /*el cliente se ha desconectado*/
				sd_clientes[i] = -1; /*dejo el hueco libre*/
				FD_CLR(sd_clientes[i],&desc); /*ya no me interesa para el select*/
			}
		}
	  }
	}/*while*/
	return 0;  
}
