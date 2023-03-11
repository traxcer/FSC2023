# PIPES y FIFOS


## En general

### pipe (tuberia)
Usadas para pasar datos entre procesos **relacionados**

### FIFO (tuberia con nombre)
Usadas para la comunicación entre procesos cualquiera.

***

En el shell podemos usar tuberias de la siguiente manera:

    $ ls | wc -l

    ls escribe (filedes 1) en la tuberia
    wc lee (filedes 2) de la tuberia.

Una tuberia (pipe) es un flujo de bytes no estructurados. Los bytes son leidos en el mismo orden en el que fueron escritos. *First in Input, First in Output ***(Primero en escribir, primero en leer)****

#### Lectura de la tuberia (pipe): 
Intentar leer de una tuberia vacia, bloquea hasta que se haya escrito 1 byte en ella. Si el extremo de escritura se cierra el proceso de lectura lo toma como el final del fichero **(EOF)**
Las tuberias son **unidireccionales**.

#### Se garantiza que las escrituras de hasta PIPE_BUF bytes sean atómicas

Si varios procesos están escribiendo en una sola canalización, se garantiza que sus los datos no se entremezclarán si no escriben más de PIPE_BUF bytes a la vez.

Al escribir bloques de datos de más de PIPE_BUF bytes en una tubería, el núcleo puede transfiera los datos en múltiples piezas más pequeñas, agregando más datos como lector elimina bytes de la tubería. (La llamada write() se bloquea hasta que todos los datos han sido
escrito en la tubería.) Cuando solo hay un proceso que escribe en una tubería (lo habitual caso), esto no importa. Sin embargo, si hay varios procesos de escritura, entonces las escrituras de bloques grandes pueden dividirse en segmentos de tamaño arbitrario (que pueden ser menor que PIPE_BUF bytes) e intercalado con escrituras de otros procesos.


## Creando y usando pipes

La llamada al sistema *pipe()* creará una pipe.

    #include <unistd.h>
    
    int pipe (int filedes[2]);

                            Devuelve 0 si Ok, y -1 en caso de error.



Una llamada exitosa a pipe() devuelve dos descriptores de archivos abiertos en la matriz de archivos: uno para el *extremo de lectura* de la canalización (**filedes[0]**) y uno para el *extremo de escritura* (**filedes[1]**).

Al igual que con cualquier descriptor de archivo, podemos usar las llamadas al sistema read() y write() para realizar formar E/S en la tubería. 

Una vez escritos en el extremo de escritura de una canalización, los datos se disponible para ser leído desde el extremo de lectura. Una lectura () de una tubería obtiene el menor de los número de bytes solicitados y el número de bytes disponibles actualmente en la canalización *(pero se bloquea si la tubería está vacía)*.

También podemos usar las funciones stdio (printf(), scanf(), etc.) con tuberías por primero usando fdopen() para obtener un flujo de archivo correspondiente a uno de los descriptores en archivos. Sin embargo, al hacer esto, debemos estar atentos a los problemas de *stdio* de almacenamiento en el búfer.

