//Servidor
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>  
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>


#define PUERTO 4950
#define T 255




int main(){
    printf("Arrancando servidor y esperando un cliente...(PID: %d)\n",getpid());
    struct sockaddr_in dir_server, dir_client;
    socklen_t tam_dir_server, tam_dir_client;

    //.1 Asignamos campos
    tam_dir_server=sizeof(dir_server);
    memset(&dir_server,0,tam_dir_server);
    dir_server.sin_family=AF_INET;
    dir_server.sin_port=htons(PUERTO);
    dir_server.sin_addr.s_addr=INADDR_ANY; //escucha desde cualquier IP
    //2. Creamos el socket
    int sd, n_sd;
    if ((sd=socket(AF_INET,SOCK_STREAM,0))<0){
        perror("SERVER: socket");
        exit(1);
    }
    //3. Vinculamos el socket al puerto
    if((bind(sd,(struct sockaddr *)&dir_server,tam_dir_server))<0){
        perror("SERVER: bind");
        exit(1);
    }
    //4. Ponemos el servidor en modo escucha
    if((listen(sd,5))<0){
        perror("SERVER: listen");
        exit(1);
    }
    //5. Esperando conexión
    tam_dir_client=sizeof(dir_client);
    if ((n_sd=accept(sd,(struct sockaddr*)&dir_client,&tam_dir_client))<0){
        perror("SERVER: accept");
        exit(1);
    }
    printf("Se ha conectado un cliente (IP:%s)\n",inet_ntoa(dir_client.sin_addr));
    //Proceso Iterativo
    uint8_t tipo_de_operacion, numero_de_operandos;
    uint16_t operandos[T];
    char b[T];
    char *aux;
    struct respuestas{
        uint8_t tipo_de_respuesta;
        int32_t resultado;
    }respuesta;
    
    int leidos;
    while(1){
        printf("Esperando nueva operación...\n");
        //lee operación codificada con el formato OPERACION
        uint16_t tam_pack;
        if((leidos=read(n_sd,&tam_pack,sizeof(u_int16_t)))<0){
            perror("SERVER: read tam_pack");
            exit(1);
        }
        tam_pack=ntohs(tam_pack);
        printf("Leido (%d bytes de tam_pack)\n",tam_pack);
        //leo la operacion empaquetada en b
        if((leidos=read(n_sd,b,tam_pack))<0){
            perror("SERVER: read");
            exit(1);
        }
        if (leidos==0){
            printf("Se ha cerrado el cliente\n");
            exit(1);
        }
        //desempaqueto
        aux=b;
        memcpy(&tipo_de_operacion,aux,sizeof(uint8_t));
        aux+=sizeof(tipo_de_operacion);
        printf("Tipo de operación: %d\n",tipo_de_operacion);
        memcpy(&numero_de_operandos,aux,1);
        aux+=sizeof(numero_de_operandos);
        printf("Número de operandos: %d\n",numero_de_operandos);
        for (int i=0;i<numero_de_operandos;i++){ //cargamos operandos
            memcpy(operandos+i,aux,sizeof(uint16_t));
            aux+=sizeof(uint16_t);
        }
    
        //hacemos la operación
        if (tipo_de_operacion=='s'){
            //se trata de una suma
            printf("He recibido una suma\n");
            respuesta.tipo_de_respuesta=1;
            for (int i=0;i<numero_de_operandos;i++)
                respuesta.resultado+=operandos[i];
        }
        if (tipo_de_operacion=='m'){
                //se trata de una suma
                printf("He recibido una multiplicación\n");
                respuesta.tipo_de_respuesta=1;
                for (int i=0;i<numero_de_operandos;i++)
                    respuesta.resultado*=operandos[i];
        }
        if (tipo_de_operacion!='s' && tipo_de_operacion!='m') {
            printf("He recibido una operación no contemplada\n");
            respuesta.tipo_de_respuesta=0;
            respuesta.resultado=0;
        }
        write(n_sd,&respuesta.tipo_de_respuesta,sizeof(respuesta.tipo_de_respuesta));
        respuesta.resultado=htonl(respuesta.resultado);
        write(n_sd,&respuesta.resultado,sizeof(respuesta.resultado));
    } //fin while
    close(sd);
    close(n_sd);
return 0;    

}