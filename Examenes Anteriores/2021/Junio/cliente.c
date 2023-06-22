/* Servidor */

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define T 255


//Funciones auxiliares
uint32_t resumen(char *b, int longitud){
    uint32_t sum=0;
    for(int i=0; i<longitud;i++)
        sum+=b[i];
    return sum*longitud;
}

int main(int argc, char* argv[]){
    if (argc<3){
        printf("Usar: %s <IP> <PUERTO> <nombFichero\n", argv[0]);
        exit(1);
    }

    struct sockaddr_in d_serv;
    
    //1. Asignamos campos del servidor
    memset(&d_serv,0,sizeof(d_serv));
    d_serv.sin_family=AF_INET;
    d_serv.sin_port=htons(atoi(argv[2]));

    int direccion= inet_addr(argv[1]);
    if (direccion<0){
        printf("CLIENTE: direccion IP erronea\n");
        exit(1);
    }
    d_serv.sin_addr.s_addr=direccion; //admite cualquier dirección

    


    //2. Creo el socket
    int sd= socket(AF_INET,SOCK_STREAM,0);
    if (sd<0){
        perror("CLIENTE: socket");
        exit(1);
    }

    //3. Conecto
    socklen_t tam_d_serv = sizeof(d_serv);
    if((connect(sd,(struct sockaddr *)&d_serv,tam_d_serv))<0){
        perror("CLIENTE: connect");
        exit(1);
    }
   
    printf("Me conecté al servidor\n");
    uint8_t nivel_seguridad;
    read(sd,&nivel_seguridad,sizeof(uint8_t));
    printf("CLIENTE: Recibido el nivel de seguridad del servidor (%d)\n", nivel_seguridad);
    uint16_t tipo_msg, long_msg;
    tipo_msg=0; //nombre de fichero
    long_msg=strlen(argv[3]);
    printf("Enviado el tipo_msg(%d), long_msg(%d) y nombre del fichero (%s)\n", tipo_msg, long_msg,argv[3]);
    tipo_msg=htons(tipo_msg); //convierto a la red
    write (sd,&tipo_msg,sizeof(tipo_msg));
    long_msg=htons(long_msg); //convierto a la red
    write (sd,&long_msg,sizeof(long_msg));
    
    //Preparado para enviar el fichero
    int fd=open(argv[3],O_RDONLY);
    char b[T];
    int leidos;
    
    tipo_msg=1; //bloque de fichero
    tipo_msg=htons(tipo_msg); //convierto a la red
    uint32_t chkcli, chksrv;
    if (fd<0){
        perror("CLIENTE: open");
        exit(1);
    }
    write (sd,argv[3],strlen(argv[3])); //Todo Ok envio nombre del fichero
    while ((leidos= read(fd,b,T))>0){
        printf("CLIENTE: Leo (%d)\n", leidos);
        write(sd,&tipo_msg,sizeof(tipo_msg)); //Cabecera
        long_msg=htons(leidos); //convierto a la red
        write(sd,&long_msg,sizeof(long_msg)); //Cabecera
        write(sd,b,leidos); //escribe el payload
            if (nivel_seguridad==1){
                chkcli= resumen(b,leidos); //leidos local
                read(sd,&chksrv,sizeof(chksrv)); //lee del servidor
                chksrv=ntohl(chksrv); //convierte a local
                printf("CLIENTE: recibo chksrv (%u), calculo chkcli(%u)\n",chksrv,chkcli);
                if (chksrv!=chkcli){
                    printf("Valor incorrecto de la función resumen.\n");
                    printf("Posible interceptación de los datos\n");
                    close (fd);
                    close (sd);
                    exit(1);
                }
            }
    }
    
    close (sd);
    printf("Todo cerrado ordenadamente\n");

    return 0;
}