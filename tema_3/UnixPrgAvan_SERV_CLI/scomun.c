#include "scomun.h"
char *nombre_prog;
/***
leer_estad: lee datos referentes al proceso servidor para enviárselos al cliente.
***/
struct estad leer_estad() {
    struct estad est;
    struct utsname sistema;
    time_t fecha;
    uname (&sistema);
    strncpy (est.nodo, sistema.nodename, 9);
    strncpy (est.sistema, sistema.sysname, 9);
    strncpy (est.version, sistema.release, 9);
    est.pid = getpid ();
    time (&fecha);
    strncpy (est.fecha, asctime(localtime(&fecha)), 24);
    est.fecha[24] = 0;
return (est);
};
/***
error: imprime mensajes de error.
***/
void error (char *fichero, int linea, char *mensaje) {
    extern int errno;
    fprintf (stderr, "%s:(%s - %d). %s - %s\n", nombre_prog, fichero,linea, mensaje, sys_errlist[errno]);
}