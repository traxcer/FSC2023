/*** Función de escritura protegida contra señales. ***/

#include <errno.h>
#include <unistd.h>

int write_n(int fd,char *b, int escribir);

int write_n(int fd,char *b, int escribir){
    ssize_t porescribir=escribir;
    ssize_t totalescrito =0;
    ssize_t escritos;

    do{
        errno=0;
        if ((escritos=write(fd,b+totalescrito, porescribir) )>0){
            porescribir=-escritos;
            totalescrito=+escritos;
        }
    } while(((escritos>0)&&(totalescrito < escribir))||(errno=EINTR));
    if (totalescrito >0)
        return totalescrito;
    else 
        return escritos;
}