#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char *argv[]){
  
  //Nos aseguramos que introducimos correctamente los argumentos por la consola. En realidad nunca va a tirar por esta bifurcacion, porque el proceso muta de "ejercicio3.cpp"
  if(argc != 2){
    perror("Error, se debe introducir unicamente 2 argumentos por la consola");
    exit(1);
  }
  
  //Zona de declaracion de variables
  int fd1,fd2,fd3,fd4;	//Para guardar lo que nos devuelve las aperturas de los ficheros pipes con nombre
  int r1=1,r2,r3;	//Para guardar lo que nos devuelve "read"
  int algo;	//Es la señal que nos da driver_impresion que nos permite continuar con el proceso
  char lecturaFichero[1];	//Array de una posicion, que va leyendo caracter a caracter el fichero entero
  int contador = 0;	//Va contando el numero de caracteres que tiene el fichero
  char pid_cadena[512];
  char buffer[1024];
  int w1,w2,w3;
  //int c;

  int pid_solicitante;
  int size_file;
  
  //1. Obtengo mi pid
  pid_solicitante = getpid();
  
  //2. Abrimos la pipe "fifo_driver_control" para escritura
  fd1 = open("/tmp/fifo_driver_control", O_WRONLY | O_APPEND);
  if(fd1 == -1){
    perror("Error 3");
    exit(1);
  }

	printf("El cliente ha abierto la fifo de control\n");

  //3. Abrimos la pipe fifo_driver_datos para escritura
  fd3 = open("/tmp/fifo_driver_datos", O_WRONLY | O_APPEND);
  if(fd3 == -1){
    perror("Error 10");
    exit(1);
  }
  
	printf("El cliente ha abierto la fifo de datos\n");
  
  //4. Transforma el pid del proceso en una cadena de caracteres para poder crear la pipe
  sprintf(pid_cadena, "/tmp/%d", pid_solicitante);
  
  //5. Creamos nuestra pipe con nombre pid del proceso
  mknod(pid_cadena, S_IFIFO|0666, 0);
  
  printf("El cliente ha creado la fifo con nombre su pid\n");

  //6. Pasamos por la pipe "fifo_driver_control" nuestro pid
  w1 = write(fd1, &pid_solicitante, sizeof(int));
  if(w1 == -1){
    perror("Error 5");
    exit(1);
  }
  
	printf("El cliente ha enviado su pid por control\n");
	
  //7. Determinar tamanyo fichero
	contador = 0;
  fd4 = open(argv[1], O_RDONLY);	
  if(fd4 == -1){
    perror("Error 6");
    exit(1);
  }
	//Ir leyendo el fichero caracter a caracter
	r1 = read(fd4, lecturaFichero, 1);	
	while(r1 != 0){
    if(r1 == -1){
      perror("Error 7");
      exit(1);
    }
    //Se incrementa por cada caracter contado (en este caso da igual si hiciéramos contador++ porque sólo leemos un caracter)
    contador += r1;	
    r1 = read(fd4, lecturaFichero, 1);
  }
  size_file = contador;	
 
 	printf("El cliente ha contado el tamanyo del fichero a enviar: %d\n", size_file);

  //9. Pasamos por pipe "fifo_driver_control" el tamaño del fichero
  w2 = write(fd1, &size_file, sizeof(int));
  if(w2 == -1){
    perror("Error 8");
    exit(1);
  }
  
	printf("El cliente ha pasado el tamanyo por el control\n");

  //10. Abrimos la pipe creada. La abrimos como lectura
  fd2 = open(pid_cadena, O_RDONLY);
  if(fd2 == -1){
    perror("Error 4");
    exit(1);
  }

	printf("El cliente abre la fifo con su pid\n");

  //11. Esperamos lectura por fifo pid, que nos da la señal para poder enviar datos
  r3 = read(fd2, &algo, sizeof(int));
  if(r3 == -1){
    perror("Error 9");
    exit(1);
  }
  
	printf("El cliente ha recibido algo del servidor para sincronizarse\n");

  
  //12. Extraemos el contenido del fichero a imprimir, y lo escupimos en la variable "buffer"
	//(Primero tenemos que restablecer el offset del descriptor de fichero al principio)
	int res;
	if ((res = lseek(fd4, 0, SEEK_SET)) == -1)
	{
		perror("Problema con lseek\n");
		return -1;
	}
	r2 = read(fd4, buffer, size_file);
  if(r2 == -1){
    perror("Error 12");
    exit(1);
  }
 
 	printf("El cliente ha leido el fichero\n");

  //13. Inyectamos el contenido de "buffer" en fifo_driver_datos
  w3 = write(fd3, buffer, r2);
  if(w3 == -1){
    perror("Error 13");
    exit(1);
  }
  
	printf("El cliente ha mandado los datos del fichero\n");

  return 0;
}
  
  
