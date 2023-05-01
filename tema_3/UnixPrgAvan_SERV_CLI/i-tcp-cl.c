/*============================================================
TCP
Cliente con conectores AF_INET del tipo SOCK_STREAM
Fichero: i-tcp-cl.c
=============================================================*/

#include "scomun.h"
#include "i-addr.h"
main (int argc, char *argv [])
{
int sfd;
struct sockaddr_in ser_addr;
nombre_prog = argv[0];
/*Apertura de un conector orientado a conexión de la familia AF_INET.*/
if ((sfd = socket (AF_INET, SOCK_STREAM, 0)) == -1) {
error (DEP, "apertura del conector");
exit (-1);
}
/*Petición de conexión con el servidor.*/
ser_addr.sin_family = AF_INET;
ser_addr.sin_addr.s_addr = inet_addr (DIRECCION_NODO_SERVIDOR);
ser_addr.sin_port = htons (PUERTO_SERVIDOR_TCP);
if (connect(sfd,(struct sockaddr *)&ser_addr,sizeof(ser_addr))==-1) {
error (DEP, "conexión");
exit (-1);
}
/*Comunicación con el servidor.*/
enviar_mensajes_str (sfd);
close (sfd);
exit (0);
}