/****************************************************************/
/* FICHERO: servidor.c */
/* DESCRIPCION: codigo de un servidor de fechas */
/****************************************************************/
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "defs.h"

int main() {
    // Variables
    struct sockaddr_in direccionServidor, direccionCliente ;
    int descSocket, nuevoDescSocket ;
    socklen_t longDirCliente;//longitud de la direccion del cliente
    uint16_t codigoOperacion ;
    int numbytes;
    char buffer[T];

    /* creacion del socket TCP */
    descSocket = socket(PF_INET, SOCK_STREAM, 0) ;
        if (descSocket < 0) {
            perror("no se ha podido abrir el socket") ;
            exit(1) ;
        }
    
    /* se pone a cero la direccion del servidor */
    memset(&direccionServidor, 0, sizeof(direccionServidor));
    /* Asigna velores a la dirección del servidor */
    direccionServidor.sin_family = AF_INET;
    direccionServidor.sin_addr.s_addr = INADDR_ANY; //desde cualquier dirección
    direccionServidor.sin_port = htons(atoi(puerto_tcp)); 
    
    /*vincula el proceso con el puerto de escucha indicado en la var. direccionServidor */
    if (bind(descSocket, (struct sockaddr *) &direccionServidor, sizeof(direccionServidor))< 0) {
        perror("error al vincular la direccion local") ;
        exit(1) ;
    }
    
    /* pone el socket en modo pasivo (ahora puede aceptar conexiones) */
    listen(descSocket, 5); /*cinco clientes pueden esperar su turno en cola*/
    
    //Bucle del Servidor
    while(1){
        printf("Servidor esperando conexion ...\n");
        /* espera una conexion del cliente, devuelve dicha conexion en nuevoDescSocket */
        longDirCliente=sizeof(direccionCliente);/* longitud de la direccion del cliente */
        nuevoDescSocket = accept(descSocket, (struct sockaddr *) &direccionCliente , &longDirCliente);
        if(nuevoDescSocket < 0) {
            fprintf(stderr, "SERVIDOR: error al aceptar nueva conexion \n") ;
            exit(1) ;
        }
        //Aqui ya se ha conectado
        printf("Conexión aceptada...\n");
        numbytes=recv(nuevoDescSocket, &codigoOperacion, sizeof(codigoOperacion),0) ;
        if (numbytes==0){
            if (close(nuevoDescSocket)<0){
                perror("close");
                exit(-1);
            }
            printf("Conexión cerrada, esperando nueva conexión...\n");
            continue;
        } else if(numbytes!=sizeof(codigoOperacion)){
            perror("revc");
            exit(-1);
        }
        printf("Numero de bytes recibidos del cliente:%d con la Operación: %d\n", numbytes,codigoOperacion);

        //Si llegamos aqui recibimos un buen codigo de operación
        //Recibimos el código de operación
        switch(codigoOperacion) {
            case SOLICITUD_FECHA : {
                time_t fechaActual = time(0) ; /*obtiene el tiempo*/
                struct tm * fechaPtr = gmtime(&fechaActual); /*se convierte a una estructura tm*/
                struct FechaYHora fecha ;
                    fecha.dia = fechaPtr->tm_mday ;
                    fecha.mes = fechaPtr->tm_mon + 1 ; /* porque devuelve de 0 a 11 */
                    fecha.anno = fechaPtr->tm_year + 1900;/*devuelve el transcurrido desde 1900 */
                    fecha.hora = fechaPtr->tm_hour ;
                    fecha.minuto = fechaPtr->tm_min ;
                    fecha.segundo = fechaPtr->tm_sec ;
                codigoOperacion = RESPUESTA_FECHA ;
                printf("Enviando fecha...\n");
                send(nuevoDescSocket, &codigoOperacion, sizeof(codigoOperacion),0) ;
                // send(nuevoDescSocket, &fecha , sizeof(fecha),0) ; Hay que enviar campo a campo
                //empaqueto y envio
                printf("Fecha obtenida: %hd/%hd/%hd\n",fecha.dia,fecha.mes,fecha.anno);
                char *aux;
                aux=buffer;
                fecha.dia=htons(fecha.dia);
                memcpy(aux,&fecha.dia, sizeof(fecha.dia));
                aux+=sizeof(fecha.dia);
                fecha.mes=htons(fecha.mes);
                memcpy(aux,&fecha.mes, sizeof(fecha.mes));
                aux+=sizeof(fecha.mes);
                fecha.anno=htons(fecha.anno);
                memcpy(aux,&fecha.anno, sizeof(fecha.anno));
                aux+=sizeof(fecha.anno);
                fecha.hora=htons(fecha.hora);
                memcpy(aux,&fecha.hora, sizeof(fecha.hora));
                aux+=sizeof(fecha.hora);
                fecha.minuto=htons(fecha.minuto);
                memcpy(aux,&fecha.minuto, sizeof(fecha.minuto));
                aux+=sizeof(fecha.minuto);
                fecha.segundo=htons(fecha.segundo);
                memcpy(aux,&fecha.segundo, sizeof(fecha.segundo));               
                aux+=sizeof(fecha.segundo);
                printf("Fecha enviada: %hd/%hd/%hd\n",fecha.dia,fecha.mes,fecha.anno);
                send(nuevoDescSocket,buffer,aux-buffer,0);
                break ;
                }
            default: 
                fprintf(stderr, "SERVIDOR: mensaje no valido: %d\n", codigoOperacion);
                exit(1) ;    
            } /* fin del switch */
    } /* fin del while */
} /* fin del main */
