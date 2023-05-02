/*===========================================================================
Fichero de cabecera para los ejemplos de servidores y clientes que emplean la
familia de conectores AF_INET.
=============================================================================*/
#ifndef _INTERNET_ADDR_
#define _INTERNET_ADDR_
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define PUERTO_SERVIDOR_TCP 7000
#define PUERTO_SERVIDOR_UDP 7000
#define DIRECCION_NODO_SERVIDOR "127.0.0.1"
#endif