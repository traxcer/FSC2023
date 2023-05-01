/*============================================================
TCP
Serv. concurrente con conectores AF_INET del tipo SOCK_STREAM 
Fichero: i-tcp-se.c
=============================================================*/
#include "scomun.h"
#include "i-addr.h"
#include "str-com.c"

int main (int argc, char *argv []){
    int sfd, nsfd, pid;
    struct sockaddr_in ser_addr, cli_addr;
    socklen_t cli_addr_len;
    nombre_prog = argv [0];
    /* Apertura de un conector orientado a conexión de la familia AF_INET. */
    if ((sfd = socket (AF_INET, SOCK_STREAM, 0)) == -1) {
        perror ("abrir socket");
        exit (-1);
    }
    /*Publicidad de la dirección del servidor. */
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_addr.s_addr=inet_addr(DIRECCION_NODO_SERVIDOR);
    ser_addr.sin_port = htons (PUERTO_SERVIDOR_TCP);
    if (bind (sfd,(struct sockaddr *)&ser_addr,sizeof(ser_addr)) == -1) {
        perror ("bind");
        exit(-1);
    }
    /*Declaración de una cola con 5 elementos para peticiones de conexión. */
    listen (sfd, 5);
    /*Bucle de lectura de peticiones de conexión.*/
    for (;;) {
        cli_addr_len = sizeof (cli_addr);
        if ((nsfd = accept (sfd, (struct sockaddr *) &cli_addr, &cli_addr_len)) == -1) {
            perror("accept");
            exit (-1);
        }
        /*Creación de un proceso hijo para atender al cliente. */
        if ((pid = fork ()) == -1)
            perror("fork");
        else if (pid == 0) {
                /*Código del proceso hijo. */
                close (sfd);
                recibir_mensaje_str (nsfd);
                close (nsfd);
                exit(0);
        }

        /*Código del proceso padre.*/
        close (nsfd);
    }
    return 0;
}