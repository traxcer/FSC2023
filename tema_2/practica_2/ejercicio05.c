#define _POSIX_SOURCE
/*
Ejercicio 5:  
 
Implementa un programa en C (y compílalo con gcc –ansi), donde se 
utilice la función read(), que es bloqueante,  para  recoger  desde  
teclado  (descriptor  número  0)  un  conjunto  de  caracteres  hacia  
un cadena  y  luego  imprimirlos  por  pantalla.  
Observa  el  efecto  de  la  llegada  de  una  señal  durante  esta 
operación  de  lectura  de  teclado,  por  ejemplo  una  alarma  
(la  función  read  devuelve  el  número  de caracteres leídos o un 
valor negativo en caso de error. ¿Es este error de tipo EINTR?). 
 
Vuelve ahora a repetir la compilación con gcc sin la opción –ansi 
(o con g++). ¿Qué sucede ahora? 
 
En  principio,  si  una  llamada  al  sistema  ha  sido  interrumpida
por  una  señal,  el  programador  podría decidir volver a realizar 
dicha llamada. En el ejemplo anterior, si se detecta que la llamada a
read() devuelve -1 y el tipo de error es EINTR se puede volver a llamar
a read(). Vuelve a implementar el programa del ejercicio para que 
realice este nuevo comportamiento. 
*/