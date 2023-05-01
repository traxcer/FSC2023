/*=========================================================================
Declaración de tipos y funciones utilizados por los programas de ejemplo de
manejo de conectores.
===========================================================================*/
#ifndef _SCOMUN_
#define _SCOMUN_
#include <stdio.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <unistd.h>

#define CR 10 /*Carácter retorno de carro. */

extern char *nombre_prog;

/*Estructura que almacena información sobre el sistema.*/
struct estad {
    char nodo [9];
    char sistema[9];
    char version [9];
    int pid;
    char fecha [25];
};

/*Declaración de funciones.*/
struct estad leer_estad ();

/*Constantes de ayuda a la depuracion.*/

#define DEP __FILE__, __LINE__
#endif