/*
Programa sobre la sessión 11 pipes
*/

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <ctype.h>
#include <errno.h>

#define T 8

int read_n(int fd,void *b,int n);
int write_n(int fd,void *b,int n);
void sustituir(char * input, char ** output);
int damemoria(char *c);
void sust(char **p, char c);
int tamanioCadena(char c[]);
void concatena(char c1[], char c2[]); 


int main(int argc, char * argv[]){
if (argc<2){
    printf("Usar%s: <ficheros> <fichero> ...\n",argv[0]);
    exit(1);
}

int p[2]; /* manejadores pipe */

if (pipe(p)<0){ /*Abre la pipe */
    perror("pipe");
    exit(1);
}

pid_t pid;
pid=fork(); /* Bifurcamos en con un hijo */

if(pid<0){ /* Error de LLamada al sistema fork */
    perror("fork");
    exit(1);
    } 

else if (pid==0){ /* Zona del hijo */
    if(close(p[1])<0){ /* Cerramos el extremo de escritura del hijo */
        perror ("close escritura hijo");
        exit(1);
    }
    char filename_1[35],cadena[35],*aux;
    int longitud,fd3;
    for(int i = 1; i < argc; i++){ /* Bucle ficheros */
        strcpy(filename_1,argv[i]);
        strcat(filename_1,".f2");
        fd3 = open(filename_1,O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if(fd3 < 0){
            perror("open f2");
            exit(1);
        }
        int leo_long = read(p[0],&longitud,sizeof(int));
        if(leo_long < 0){
            perror("longitud");
            exit(1);
        }
        int leo_cad = read_n(p[0],cadena,longitud);
        if(leo_cad != longitud){
            perror("read longitud");
            exit(1);
        }
        cadena[longitud] = '\0';
        while(longitud != 0){
            int capacidad = tamanioCadena(cadena);
            sustituir(cadena,&aux);
            int escritos = write_n(fd3,aux,capacidad);
            if(escritos != capacidad){
                perror("write_n");
                exit(1);
            }
        leo_long = read(p[0],&longitud,sizeof(int));
        leo_cad = read_n(p[0],cadena,longitud);
        if(leo_cad != longitud){
            perror("read longitud");
            exit(1);
        }
        cadena[longitud] = '\0';
    }

    if(close(fd3) < 0){
        perror("close fd3");
        exit(1);
    }
}
    if(close(p[0]) < 0){
        perror("tuveria lectura hijo");
        exit(1);
    }    
    
    exit(1);/* El hijo termina su trabajo */

} else { /* Zona del Padre */
    if(close(p[0])<0){ /* Cerramos el extremo de lectura del padre */
        perror ("close lectura padre");
        exit(1);
    }
    ssize_t fds, fdd;
    int leidos, escritos;
    char destino[30];
    char buffer[T];

    for(int i=1;i<argc;i++){ /* BUCLE PRINCIPAL lista de ficheros */
        if ((fds=open(argv[i],O_RDONLY)) < 0){
            perror("open"); /* Apertura del fichero de la lista original */
            exit(1);
        }
        printf("Nombre origen: %s\n",argv[i]);
        sprintf(destino,"%s.f1",argv[i]);
        /* Apertura del fichero de destino */
        if ((fdd=open(destino,O_CREAT | O_WRONLY | O_TRUNC, 0644)) < 0){
        perror("open");
        exit(1);
        }
        printf("Nombre destino: %s\n",destino);

        do{ /* Bucle de lectura de registros */
            if ((leidos= read(fds,buffer,T))<0){ /* No se cuantos va a leer unos read */
                perror("read");
                exit(1);
            }
            if (leidos>0){ /* Hay datos leidos, filtro y escribo*/
                escritos=write_n(p[1],&leidos,sizeof(leidos));
                escritos=write_n(p[1],buffer,leidos); /*Escribe en la pipe*/
                buffer[leidos]='\0';
                //printf("Escrito en la tuberia por el padre: tamaño:%d - Payload:%s\n",leidos,buffer);
                for (int r=0;r<leidos;r++) /* recorro el buffer leido*/
                    buffer[r]=tolower(buffer[r]); /* Filtro a minusculas */
                if( (escritos=write(fdd,buffer,leidos)) != leidos ){ /*Escribo fichero*/
                    perror("write");
                exit(1);
            }
        }
        } while (leidos>0);
    /* Ya he leido el fichero entero */
    if (close(fds)<0){
        perror("close origen");
        exit(1);
    }
    if (close(fdd)<0){
        perror("close destino");
        exit(1);
    }
    } //Fin del Bucle de ficheros
    write(1,"El buen padre está esperando a su hijo\n",40);
    if(close(p[1])<0){ /* Cerramos el extremo de escritura del padre */
        perror ("close escritura padre");
        exit(1);
    }
    wait(NULL); /* Espera al hijo */
} //Fin de la zona del padre

return 0;
} //Fin del main

/* Funciones auxiliares */

/* Lecturas protegidas */
int read_n(int fd,void *b, int n){
    int a_leer=n;
    int t_leido=0;
    int leido;

    do
    {
        errno=0;
        leido=read(fd,b+t_leido,a_leer);
        if (leido>0){
            t_leido+=leido;
            a_leer-=leido;
        }
    } while ( ((leido>0)&&(t_leido<n))||(errno==EINTR));
    if (t_leido>0){
        return t_leido;
    } else
        return leido;

}

/* Escrituras protegidas */

int write_n(int fd,void *b,int n){
    int a_escribir=n;
    int t_escrito=0;
    int escrito;

    do {
        errno=0;
        
        if ((escrito=write(fd,b+t_escrito,a_escribir))<0){
            perror("write_n");
            exit(1);
        }
        if (escrito>0){
            t_escrito +=escrito;
            a_escribir -=escrito;
        }
    } while (((t_escrito<n)&&(escrito>0))||(errno==EINTR));
    if (t_escrito>0){
        return t_escrito;
    } else
        return escrito;
}


void sustituir(char * input, char ** output){
    int longitud = tamanioCadena(input);
    int n = damemoria(input);
    *output =(char*) calloc(n,sizeof(char));
    for(int i=0;i<longitud;i++){
        sust(output,input[i]);
    }
}
int damemoria(char *c){
    int mem = 0,cnt=0;
    char *p;
    p=c;
    while(p[cnt] != '\0'){
        if(p[cnt] == '0' || p[cnt] == '1' || p[cnt] == '2' || p[cnt] == '3' || p[cnt] == '4' || p[cnt] == '5' || p[cnt] == '6' || p[cnt] == '7' || p[cnt] == '8' || p[cnt] == '9'){
            mem = mem + atoi(&p[cnt]) + 2;
        }else{
            mem++;
        }
        cnt++;
    }
    return mem+1;
}

void sust(char **p, char c){
    int n;
    char cc[]="a";
    if(c == '0' || c == '1' || c == '2' || c == '3' || c == '4' || c == '5' || c == '6' || c == '7' || c == '8' || c == '9'){
        n = atoi(&c);
        concatena(*p, "|");
        for(int i=0;i<n;i++){
            concatena(*p, "*");
        }
        concatena(*p, "|");
    }else{
        cc[0] = c;
        concatena(*p, cc);
        
    }
}
int tamanioCadena(char c[]){
    char *p;
    int res = 0;
    p = c;
    while(p[res] != '\0'){
        res++;
    }
    return res;
}
void concatena(char c1[], char c2[]){
    int l = tamanioCadena(c1);
    int cnt = 0;
    char *p1,*p2;
    p1 = c1;
    p2 = c2;
    while(p2[cnt] != '\0'){
        p1[l] = p2[cnt];
        cnt++;
        l++;
    }
}