/******************************************************************************/
/* FICHERO: receptorUDP.c */
/* DESCRIPCION: espera algún paquete en el puerto 4950 y muestra su contenido */
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

#define PUERTO 4950
#define T 100

int main()
{
    struct sockaddr_in dir_ser, dir_cli;
    int sd, bytesrecibidos;
    char buffer[T];

    // 1. Relleno campos
    memset(&dir_ser, 0, sizeof(dir_ser));
    dir_ser.sin_family = AF_INET;
    dir_ser.sin_port = htons(PUERTO);
    dir_ser.sin_addr.s_addr = INADDR_ANY;
    // 2. Creo el socket UDP
    if ((sd = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket");
        exit(-1);
    }
    // 3. Vinculo la dirección al socket
    socklen_t long_dir_serv = sizeof(dir_ser);
    if ((bind(sd, (struct sockaddr *)&dir_ser, long_dir_serv)) < 0)
    {
        perror("bind");
        exit(-1);
    }
    // 4. Longitud de la direccion del cliente
    socklen_t long_dir_cli = sizeof(dir_cli);
    // 5. espero a recibir
    printf("Servidor en espera de recibir un datagrama...\n");
    if ((bytesrecibidos = recvfrom(sd, &buffer, T, 0, (struct sockaddr *)&dir_cli, &long_dir_cli)) < 0)
    {
        perror("recvfrom");
        exit(-1);
    }
    // 6. Muestro lo recibido
    buffer[bytesrecibidos] = '\0';
    printf("El paquete recibido de: %s\n", inet_ntoa(dir_cli.sin_addr));
    printf("El paquete tiene una longitud de %d bytes\n", bytesrecibidos);
    printf("El paquete contiene ésta cadena: %s\n", buffer);

    // 7. Cierro el socket
    if (close(sd) < 0)
    {
        perror("close");
        exit(-1);
    }
    printf("El servidor ha terminado su trabajo.\n");
    return 0;
}