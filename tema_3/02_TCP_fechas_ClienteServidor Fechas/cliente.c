/****************************************************************/
/* FICHERO: cliente.c */
/* DESCRIPCION: cliente hace uso del servidor de fechas */
/****************************************************************/
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "defs.h"
int main() {
    //variables
    struct sockaddr_in direccionServidor ;
    int descSocket; /* descriptor del socket */
    uint16_t codigoOperacion ;
    struct FechaYHora fecha;
    int numbytes;
    char buffer[T];
    
    //cargamos campos del socket
    memset(&direccionServidor, 0, sizeof(direccionServidor)); /*la pone a cero*/
    direccionServidor.sin_family = AF_INET;
    direccionServidor.sin_addr.s_addr = inet_addr(direccion_ip);
    direccionServidor.sin_port = htons(atoi(puerto_tcp));
    
    /*creacion del socket TCP*/
    if ((descSocket = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Cliente: no se ha podido abrir el socket") ;
        exit(1);
    } 
    
    /*conexion al servidor*/
    if (connect(descSocket,(struct sockaddr *) &direccionServidor, sizeof(direccionServidor)) < 0) {
        perror("no se ha podido conectar con servidor") ;
        exit(1) ;
    }
    printf("Cliente: conexion establecida\n") ;
    codigoOperacion = SOLICITUD_FECHA ;
    /*solicita la fecha al servidor*/
    numbytes=send(descSocket, &codigoOperacion, sizeof(codigoOperacion),0) ;
    if (numbytes<0){
        perror("send");
        exit(-1);
    } else if (numbytes!=sizeof(codigoOperacion)){
        perror("send codigoOperacion");
        exit(-1);
    }
    printf("Numero de bytes enviados al servidor:%d con la Operación: %d\n", numbytes,codigoOperacion);
    /*espera respuesta*/
    numbytes=recv(descSocket, &codigoOperacion, sizeof(codigoOperacion),0) ;
    if (numbytes<0){
        perror("recv codigoOperacion");
        exit(-1);
    } else if (numbytes!=sizeof(codigoOperacion)){
        perror("rec codigoOperacion");
        exit(-1);
    }
    printf("Numero de bytes recibidos del servidor:%d con la Operacion: %d\n", numbytes,codigoOperacion);
    if (codigoOperacion == RESPUESTA_FECHA) {

    /*recibe la fecha*/
    numbytes=recv(descSocket, &buffer, sizeof(short),0) ;
    if (numbytes<0){
        perror("recv fecha");
        exit(-1);
    } else if (numbytes==0){
        perror("Cerrada Conexion");
        close(descSocket);
        exit(-1);
    }
    printf("Numero de bytes recibidos del servidor:%d\n", numbytes);
    fecha.dia=ntohs(buffer);
    fecha.mes=ntohs(buffer[2*sizeof(short)]);
    fecha.anno=ntohs(buffer[3*sizeof(short)]);
    printf("Fecha: %d-%d-%d ", fecha.dia , fecha.mes , fecha.anno ) ;
    printf("Hora:%d-%d-%d\n", fecha.hora, fecha.minuto, fecha.segundo) ;
    }
    else {
    fprintf(stderr, "CLIENTE: Error al recibir->") ;
    fprintf(stderr, "mensaje desconocido: %d\n", codigoOperacion) ;
    exit(1) ;
    }
return 0;
} /* main */