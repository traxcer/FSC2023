#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(){
  
  //Zona de declaracion de variables
  int fd1, fd2, fd3;	//Para guardar lo que devuelven las aperturas de los ficheros pipes con nombre
  int pid_proceso = 0;
  char buffer[1024];
  int tam_fichero;
  char pid_cadena[512];
  int algo = 5;
  int r1,r2,r3;
  int w1;
  
  
  //1. Creamos las pipes con nombre "fifo_driver_control" y "fifo_driver_datos"
  umask(0); /*permite crear ficheros con cualquier tipo de permiso, reseteando los valores prohibidos por defecto*/
  int retorno = mknod("/tmp/fifo_driver_control", S_IFIFO | 0666, 0);
  if((retorno == -1) && (errno != EEXIST)){ /*si errno vale EEXIST no la ha creado porque existe*/
	perror("error mk_nod de fifo_driver_control");
	exit(1);
  }
  retorno = mknod("/tmp/fifo_driver_datos", S_IFIFO | 0666, 0);
  if((retorno == -1) && (errno != EEXIST)){ /*si errno vale EEXIST no la ha creado porque existe*/
	perror("error mk_nod de fifo_driver_datos");
	exit(1);
  }
  
  //2. Abrimos las pipes con nombre para lectura solamente
  fd1 = open("/tmp/fifo_driver_control", O_RDONLY);
  if(fd1 == -1){
    perror("Error 14");
    exit(1);
  }
  fd2 = open("/tmp/fifo_driver_datos", O_RDONLY);
  if(fd2 == -1){
    perror("Error 15");
    exit(1);
  }
  
  printf("FIFOS creadas y abiertas\n");
  
  //3. Lee el pid del proceso cliente_impresion de la pipe fifo_driver_control
  r1 = read(fd1, &pid_proceso, sizeof(int));
  if(r1 == -1){
    perror("Error 16");
    exit(1);
  }
  
  printf("pid de proceso %d recibido\n", (int)pid_proceso);

  //4. Lee el tamaño de fichero que esta en fifo_driver_control
  r2 = read(fd1, &tam_fichero, sizeof(int));
  if(r2 == -1){
    perror("Error 17");
    exit(1);
  }
  
  printf("El tamaño de fichero que el cliente quiere imprimir es de %d bytes\n", (int)tam_fichero);

  //Transformamos el pid del proceso a cadena, para poder abrir la fifo con nombre pid
  sprintf(pid_cadena, "/tmp/%d", pid_proceso);
  
  //5. Abrimos la fifo "su_pid" para escribir en ella y mandar una señal a cliente_impresion para desbloquearlo
  fd3 = open(pid_cadena, O_WRONLY | O_APPEND);
  if(fd3 == -1){
    perror("Error 18");
    exit(1);
  }
  
  printf("Abrimos fifo pid\n");

  //6. Escribimos "algo" en pipe "su_pid" para desbloquear a cliente_impresion para que continue su trabajo
  w1 = write(fd3, &algo, sizeof(int));
  if(w1 == -1){
    perror("Error 19");
    exit(1);
  }

  printf("Mandamos algo al cliente para sincronizar\n");
  
  //7. Leemos datos por la pipe fifo_driver_datos
  r3 = read(fd2, buffer, tam_fichero);
  if(r3 == -1){
    perror("Error 20");
    exit(1);
  }
  
  
  //8.Imprimimos por pantalla el contenido de "buffer"
  for(int i=0; i<r3; i++){
    printf("%c",buffer[i]);
  }
  
  return 0;
}
  
  
  
