/******************************************************************************/
/* FICHERO: emisorUDP.c */
/* DESCRIPCION: envia un datagrama al puerto 4950 */
/******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PUERTO 4950
#define T 100

int main(int argc,char *argv[]){
    if (argc<3){
        printf("uso: %s <IP> mensaje\n", argv[0]);
        exit(1);
    }

struct sockaddr_in dir_servidor;
int sd;
ssize_t enviobytes;
socklen_t lon_dir_servidor=sizeof(dir_servidor);

//1. relleno campos
memset(&dir_servidor,0,sizeof(dir_servidor)); //limpio
dir_servidor.sin_family=AF_INET;
dir_servidor.sin_port=htons(PUERTO);
uint32_t ip_servidor=inet_addr(argv[1]);
if (ip_servidor<0){
    perror("int_addr");
    exit(-1);
}
//dir_servidor.sin_addr.s_addr=inet_addr(htonl(atoi(argv[1]))); otra opcion que pongo yo
dir_servidor.sin_addr.s_addr=ip_servidor;

//2. creo el socket UDP
if ((sd=socket(PF_INET, SOCK_DGRAM,0))<0){
    perror("socket");
    exit(-1);
}

//3. Envio el mensaje
if ((enviobytes=sendto(sd,argv[2],strlen(argv[2]),0,(struct sockaddr *)&dir_servidor,lon_dir_servidor))<0){
    perror("sendto");
    exit(-1);
}
//4. Muestro en pantalla
printf("Enviados %zd bytes a %s\n", enviobytes, inet_ntoa(dir_servidor.sin_addr));

//5. Cierro el socket
if (close (sd)<0){
    perror("close");
    exit(-1);
}


    return 0;
}