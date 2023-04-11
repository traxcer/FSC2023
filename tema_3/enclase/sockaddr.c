#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

int main(int argc, char *argv[]){
/*Programa que recibe una dirección IP-Puerto
como argumento de entrada y crea una dirección
de socket de internet
*/
    if (argc<3){
        printf("Uso: %s <ip> <puerto>\n", argv[0]);
        exit (-1);
    }
    struct sockaddr_in direccion;

    //1. Iniciamos la dirección
    memset(&direccion,0,sizeof(direccion));

    //2. Definimos la familia de la dirección
    direccion.sin_family=AF_INET;

    //3. Seteamos el puerto
    direccion.sin_port=htons(atoi(argv[2]));

    //4. Definimos la dirección IP
    u_int32_t dir_ip=inet_addr(argv[1]);
    if (dir_ip<0){
        printf("Dirección IP incorrecta: %s\n", argv[1]);
        exit (-1);
    }
    //5. copiamos la direccion IP a la structura
    direccion.sin_addr.s_addr=dir_ip;
    //  otra alternativa: memcpy(&direccion.sin_addr, &dir_ip,4);

return 0;
}