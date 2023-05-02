/*===========================================================================
Funciones para comunicarse con conectores del tipo STREAM (str-com.c)
=============================================================================*/
#include "scomun.h"
#include "scomun.c"
#define MAXLINEA 256

void recibir_mensaje_str (int sfd);
void enviar_mensajes_str (int sfd);
/*=============================================================================
escribirn: función utilizada para escribir en un fichero referenciado por el
descriptor fd. Esta función es una interfaz con write y es necesaria porque la
escritura de bloques de datos muy grandes en un conector no se puede realizar
mediante una sola llamada a write.
=============================================================================*/
int escribirn (int fd, char *buf, int nbytes){
    int escritos, resto = nbytes;
    while (resto > 0) {
        escritos = write (fd, buf, resto);
        if (escritos == -1)
        return (-1);
        resto -= escritos;
        buf += escritos;
    }
    return (nbytes - resto);
}
/*=============================================================================
leer_linea: función para leer líneas de un fichero de entrada.
=============================================================================*/
int leer_linea (int fd, char *buf, int nbytes){
    int n, nb;
    char b;
    for (n = 1; n < nbytes; n++) {
        if ((nb = read (fd, &b, 1)) == -1){
            return (-1); /*Error de lectura. */
            if (b == '\n'){ /*Un mensaje termina con retorno de carro.*/
                break;
            }
        if (nb == 0){
            if (n == 1){
                return (0); /*Caso de encontrar EOF.*/
            } else {
                break;
            }
        }
        *buf++ = b;
    }
    *buf = 0;
    }
return (n);
}

/*=============================================================================
recibir_mensaje_str: esta función la ejecutan los servidores que trabajen con
conectores del tipo STREAM.
=============================================================================*/
void recibir_mensaje_str (int sfd){
    int nbytes;
    char linea [MAXLINEA], mensaje [MAXLINEA];
    struct estad est;
    for (;;) {
    /*Lectura de los mensajes procedentes del cliente. */
    nbytes = leer_linea (sfd, linea, MAXLINEA);
    if (nbytes == 0)
        return; /*Fin de servicio.*/
    else if (nbytes == -1) {
        perror("leer_linea");
        exit (1);
    }
    printf ("Mensaje recibido: %s\n", linea);
    /*Formación del mensaje para el cliente.*/
    est = leer_estad ();
    sprintf (mensaje, "%s(%s, %s), PID(%d), %24s, %s\n", 
                est.nodo, est.sistema, est.version, est.pid, est.fecha, linea);
    /*Envío del mensaje al cliente.*/
    nbytes = strlen (mensaje);
    if (escribirn (sfd, mensaje, nbytes) != nbytes) {
        perror ("escribirn");
        exit (1);
    }
    printf ("Mensaje devuelto: %s\n", mensaje);
    }
}

/*=============================================================================
enviar_mensajes_str: esta función la ejecutan los clientes que trabajen con
conectores del tipo STREAM.
=============================================================================*/
void enviar_mensajes_str (int sfd){
    int nbytes;
    char linea [MAXLINEA];
    /*Leer líneas del flujo estándar de entrada.*/
    printf ("<== "); fflush (stdout);
    while (fgets (linea, sizeof (linea), stdin) != NULL) {
        strcat (linea, "\n");
        /*Escribir cada línea en el conector para enviárselo al servidor.*/
        if (escribirn (sfd, linea, strlen (linea)) == -1) {
            perror ("escribirn");
            exit (-1);
        }
    /*Leer la respuesta procedente del servidor.*/
    nbytes = leer_linea (sfd, linea, MAXLINEA);
    if (nbytes == -1) {
        perror ("leer_linea");
        exit (-1);
    }
    /*Sacar el mensaje por el flujo estándar de salida.*/
    printf ("==> %s\n", linea); fflush (stdout);
    printf ("<== "); fflush (stdout);
    }
}