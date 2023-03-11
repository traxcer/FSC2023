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

Si bien es posible que el padre y el niño lean y escriban en el pipe, esto no es habitual. Por lo tanto, inmediatamente después del fork(), un proceso cierra su
descriptor para el extremo de escritura de la tubería, y el otro cierra su descriptor para el extremo de lectura. 

Por ejemplo, si el padre debe enviar datos al hijo, entonces cerraría su descriptor de lectura para la tubería, filedes[0], mientras que el hijo cerraría su descriptor de escritura de la tubería, filedes[1]. 

Una de las razones por las que no es habitual que tanto el padre como el hijo lean de un tubería única es que si dos procesos intentan leer simultáneamente de una tubería, no podemos asegúrar qué proceso será el primero en tener éxito: los dos procesos compiten por los datos.

Prevenir tales carreras requeriría el uso de algún mecanismo de sincronización. Sin embargo, si requerimos una comunicación bidireccional, existe una forma más sencilla: simplemente cree dos pipes, uno para enviar datos en cada dirección entre los dos procesos. (Si emplea esta técnica, entonces debemos tener cuidado con los interbloqueos que pueden ocurrir si ambos procesos se bloquean al intentar leer de tuberías vacías o al intentar escribir en tuberías que ya están llenas).

Si bien es posible tener múltiples procesos escribiendo en una tubería, es típico tener un solo escritor. Por el contrario, hay situaciones en las que puede ser útil tener múltiples escritores en un FIFO.

### Las tuberías permiten la comunicación entre procesos relacionados.

Hasta ahora, hemos hablado sobre el uso de tuberías para la comunicación entre un proceso padre y uno hijo. Sin embargo, las tuberías se pueden utilizar para la comunicación entre dos (o más) procesos relacionados, siempre que la tubería se haya creado por un ancestro común antes de la serie de llamadas fork() que llevaron a la existencia de los procesos. Por ejemplo, una tubería podría usarse para la comunicación entre un proceso y su nieto. El primer proceso crea la tubería, y luego
bifurca a un hijo que a su vez bifurca para dar al nieto. Un escenario común es que la tubería se use para la comunicación entre dos hermanos: su padre crea la tubería,
y luego crea a los dos hijos. Esto es lo que hace el shell cuando construye una canalización.

Hay una excepción a la afirmación de que las tuberías se pueden usar para comunicar
sólo entre procesos relacionados. Pasar un descriptor de archivo sobre un dominio UNIX socket hace posible para pasar un descriptor de archivo para una canalización a un proceso no relacionado.

### Cierre de descriptores de archivos de tubería no utilizados

Cerrar los descriptores de archivos de tubería no utilizados es más que una cuestión para garantizar que un proceso no agota su conjunto limitado de descriptores de archivos, es esencial para el correcto uso de tuberías. 

Ahora consideramos por qué los descriptores de archivo no utilizados para los extremos de lectura y escritura de la tubería deben estar cerrados. El proceso que lee de la tubería cierra su descriptor de escritura para la tubería, por lo que que, cuando el otro proceso completa su salida y cierra su descriptor de escritura, el lector ve el final del archivo (una vez que ha leído los datos pendientes en la canalización).

Si el proceso de lectura no cierra el extremo de escritura de la canalización, entonces, después de la otro proceso cierra su descriptor de escritura, el lector no verá el final del archivo, incluso después de leer todos los datos de la tubería. 

En cambio, un read() bloquearía la espera de datos, porque el kernel sabe que todavía hay al menos un descriptor de escritura abierto para la pipe. Que este descriptor se mantenga abierto por el propio proceso de lectura es irrelevante; en teoría, ese proceso aún podría escribir en la tubería, incluso si está bloqueada al intentar leer.

Por ejemplo, el read() puede ser interrumpido por un controlador de señal que escriba datos en la pipe. (Este es un escenario realista). El proceso de escritura cierra su descriptor de lectura para la tubería por una razón diferente. Cuando un proceso intenta escribir en una canalización para la que ningún proceso tiene una lectura abierta descriptor, el núcleo envía la señal SIGPIPE al proceso de escritura. Por defecto, esta señal mata un proceso. En cambio, un proceso puede organizarse para capturar o ignorar esta señal, en cuyo caso, el write() en la tubería falla con el error EPIPE (tubería rota). Recepcionar la señal SIGPIPE u obtener el error EPIPE es una indicación útil sobre el estado de la tubería, y es por eso que los descriptores de lectura no utilizados para la tubería deben cerrarse.

    Tenga en cuenta que el tratamiento de un write() que es interrumpido por un controlador SIGPIPE es especial. Normalmente, cuando un write() (u otra llamada al sistema "lenta") es interrumpida por un controlador de señal, la llamada se reinicia automáticamente o falla con el error EINTR, dependiendo de si el controlador se instaló con sigaction() 
    Indicador SA_RESTART (Sección 21.5). El comportamiento en el caso de SIGPIPE es diferente porque no tiene sentido reiniciar automáticamente el write() o simplemente indicar que write() fue interrumpida por un controlador (lo que implica que el write() podría reiniciarse manualmente de manera útil). En ninguno de los dos casos puede El intento de write() tiene éxito, porque la tubería aún estará rota.

Si el proceso de escritura no cierra el extremo de lectura de la canalización, incluso después de la otro proceso cierra el extremo de lectura de la tubería, el proceso de escritura aún podrá escribir en la tubería. Eventualmente, el proceso de escritura llenará la tubería, y otro intento de escribir se bloqueará indefinidamente. Una razón final para cerrar los descriptores de archivos no utilizados es que solo después de que todos los archivos los descriptores en todos los procesos que se refieren a una tubería están cerrados que la tubería se destruye y sus recursos liberados para su reutilización por otros procesos. En este punto, cualquier no leído los datos en la tubería se pierden.

