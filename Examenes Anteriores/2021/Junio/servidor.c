/* Servidor */

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define PUERTO 4950
#define T 255

void salida(int s){
    //Ha llegado SIGINT finaliza de forma ordenada
    exit(0);
}

//Funciones auxiliares
uint32_t resumen(char *b, int longitud){
    uint32_t sum=0;
    for(int i=0; i<longitud;i++)
        sum+=b[i];
    return sum*longitud;
}

int main(int argc, char* argv[]){

    if (argc<2){
        printf("Usar: %s <0 o 1> (0.Transferencia normal/1.Protegida)\n",argv[0]);
        exit(1);
    }
    uint8_t nivel_seguridad=atoi(argv[1]);
    printf("Servidor corriendo en modo (%d)\n", nivel_seguridad);
    if (signal(SIGINT, salida)<0){
        perror("SERVER: signal");
        exit(1);
    }

    struct sockaddr_in d_serv, d_cli;

    //1. Asignamos campos al servidor
    memset(&d_serv,0,sizeof(d_serv));
    socklen_t tam_d_serv = sizeof(d_serv);
    socklen_t tam_d_cli = sizeof(d_cli);
    d_serv.sin_family=AF_INET;
    d_serv.sin_port=htons(PUERTO);
    d_serv.sin_addr.s_addr=INADDR_ANY; //admite cualquier dirección

    //2. Creo el socket
    int sd, n_sd;
    if ((sd=socket(PF_INET,SOCK_STREAM,0))<0){
        perror("SERVER: socket");
        exit(1);
    }
    //3. Vinculo el socket al puerto
    if((bind(sd,(struct sockaddr *)&d_serv,tam_d_serv))<0){
        perror("SERVER: bind");
        exit(1);
    }
    //4. Cola de escucha por el socket
    if((listen(sd,5))<0){
        perror("SERVER: listen");
        exit(1);
    }
    //5. Espera conexión
    memset(&d_cli,0,sizeof(d_cli));
    if((n_sd = accept(sd,(struct sockaddr *)&d_cli, &tam_d_cli))<0){
        perror("SERVER: accept");
        exit(1);
    }
    printf("Se conectó el cliente: %s\n",inet_ntoa(d_cli.sin_addr));
    printf("Envio el nivel de seguridad (%d) en el que esta trabajndo el SRV\n",nivel_seguridad);
    write(n_sd,&nivel_seguridad,sizeof(nivel_seguridad)); //sustituiré por escritura protegida
    uint16_t tipo_msg, long_msg;
    char payload[T];
    int leidos;
    leidos=read(n_sd,&tipo_msg,sizeof(uint16_t));
    tipo_msg=ntohs(tipo_msg); //ya tenemos en formato host
    leidos=read(n_sd,&long_msg,sizeof(uint16_t));
    long_msg=ntohs(long_msg); //ya tenemos en formato host
    leidos= read(n_sd,payload,long_msg);
    payload[leidos-2]='s';
    payload[leidos-1]='\0'; //convierto en cadena
    printf("SERVER: Recibido el tipo_msg (%d), long_msg(%d) y nombre del fichero (%s)\n",tipo_msg,long_msg,payload);
    
    int fd=open(payload,O_WRONLY | O_TRUNC | O_CREAT, 0644);
    
    if (fd<0){
        perror("SERVER: open");
        exit(1);
    }
    uint32_t chksrv;
    //Preparado para recibir los bloques del fichero
    do{
    leidos=read(n_sd,&tipo_msg,sizeof(uint16_t));
    printf("SERVER: Leidos (%d)\n", leidos);
    tipo_msg=ntohs(tipo_msg);
    leidos=read(n_sd,&long_msg,sizeof(uint16_t));
    long_msg=ntohs(long_msg);
    leidos=read(n_sd,payload,long_msg);
        if (nivel_seguridad==1){
            chksrv=resumen(payload,leidos);
            chksrv=htonl(chksrv);
            write(n_sd,&chksrv,sizeof(chksrv));
        }
    write(fd,payload,leidos);
    } while (leidos>0);
    close (sd);
    close (n_sd);
    printf("Todo cerrado ordenadamente\n");


    return 0;
}
