/*****************************************************************
FICHERO: defs.h 
DESCRIPCION: fichero de cabecera para hacer uso de un servidor 
de fechas 
NOTA: Es comun para el servidor y el cliente.
*****************************************************************/
#ifndef __DEFS_H__
#define __DEFS_H__
const char direccion_ip[] = "127.0.0.1" ; /*localhost*/
const char puerto_tcp[] = "7010" ;
#define SOLICITUD_FECHA 1
#define RESPUESTA_FECHA 2
#define T 255
struct FechaYHora {
    short int dia ;
    short int mes ;
    short int anno ;
    short int hora ;
    short int minuto ;
    short int segundo ;
} ;
#endif