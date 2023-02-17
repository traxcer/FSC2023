/* App de Conversión del Fichero de Asistencia de Google Meet a el formato de control de fomento.


Programador: Manuel Eloy Gutiérrez
Churriana 17 de Febrero del 2023
*/

#include <stdio.h>
#include <time.h>
#include <string.h>

int main (int argc, char *argv[]){
  
  //genera nombre de fichero segun fecha actual
  time_t t = time(NULL); //Obtenemos el tiempo como entero
  struct tm tiempoLocal = *localtime(&t); //obtenemois un struct
  // El lugar en donde se pondrá la fecha y hora formateadas
  char fecha[70];
  // El formato. Mira más en https://en.cppreference.com/w/c/chrono/strftime
  char *formato = "%d_%m_%y %H:%M:%S";
  // Intentar formatear
  int bytesEscritos = strftime(fecha, sizeof fecha, formato, &tiempoLocal);
  if (bytesEscritos != 0) {
    // Si no hay error, los bytesEscritos no son 0
    printf("Fecha y hora: %s\n", fecha);
  } else {
    printf("Error formateando fecha");
  }
  if (argc==1){
    char nombfich[40]= "Attendance Report-";
    memcpy(nombfich+18,fecha,8);
    printf("El nombre del fichero es: %s\n", nombfich); 
  }
    return 0;
}