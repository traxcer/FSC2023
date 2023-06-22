#include<sys/socket.h>
#include<sys/types.h>
#include<sys/time.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>
#include<sys/select.h>
#include<signal.h>

//defino estados y eventos
#define IDLE 0
#define EJECUTANDO 1
#define ESPERANDO 2

#define CMD 0
#define STOP 1
#define CONT 2
#define FIN 3

#define T 255

//funciones auxilares

int read_n(int fd, char* b,int n){
    int leidos;
    int a_leer=n;
    int t_leido=0;
    do {
        errno=0;
        if((leidos=read(fd,b+t_leido,a_leer))>=0){
            t_leido+=leidos;
            a_leer-=leidos;
        }
    } while (((leidos>0)||(t_leido<n))&&(errno==EINTR));
    if (t_leido>0)
        return t_leido;
    else
        return leidos;
}

int fd_fifo;
int fin=0;
int fd_pipe[2];
char comando[T];

void m_sigstop(int s){
    write(fd_pipe[1],"/stop",5);
    signal(s,m_sigstop);
}
void m_sigcont(int s){
    write(fd_pipe[1],"/cont",5);
    signal(s,m_sigcont);
}
void m_sigkill(int s){
    write(fd_pipe[1],"/fin",5);
    signal(s,m_sigkill);
}

int espera_evento(){
fd_set cjto,cjtotrab;
FD_ZERO (&cjto);
FD_SET(fd_fifo,&cjto);
FD_SET(fd_pipe[0],&cjto);
int max_fd=fd_fifo>fd_pipe[0] ? fd_fifo:fd_pipe[0];

memcpy(&cjtotrab,&cjto,sizeof(cjto));
int r= select(max_fd+1,&cjtotrab,NULL, NULL, NULL);
if(r<1){
    perror("select");
    exit(1);
}
    if (FD_ISSET(fd_fifo,&cjtotrab)){ //ha llegado un comando por teclado
        char buffer[T];
        int leidos=read(0,buffer,T-1);
        buffer[leidos-1]='\0';
        int tam = strlen(buffer);
        if (strncmp("/cmd",buffer,4)){
            //extraigo comando
            memcpy(&comando,&buffer+5,tam-4);
            comando[tam-3]='\0';
            printf("comando:%s",comando);
            return CMD;
        }
        if (strncmp("/stop",comando,5)){
            return STOP;
        }
        if (strncmp("/cont",comando,5)){
            return CONT;
        }
        if (strncmp("/fin",comando,5)){
            return FIN;
        }
    }
    if (FD_ISSET(fd_pipe[0],&cjtotrab)){
        //

    }
return -1;
}

int main(){
    //Control de la señales
        signal(SIGSTOP,m_sigstop);
        signal(SIGCONT,m_sigcont);
        signal(SIGKILL,m_sigkill);
        
    fd_fifo=open("fsc_fifo", O_RDONLY);
    pipe(fd_pipe);
    
    int estado=IDLE;
    int evento;
    int pid;

    while (fin==0){
        evento=espera_evento();    
        
        switch (estado) {
            case IDLE:
            /* code */
            switch (evento) {
                case CMD:
                    //Ejecuta en un hijo un EXEC
                    pid=fork();
                        if (pid==0){

                        }

                break;
            default:
                fin=1;
                break;
            }
            break;
            case EJECUTANDO:
            /* code */
            break;
            case ESPERANDO:
            /* code */
            break;
        
        default:
            break;
        }
        
    } //fin while buble principal



    return 0;
} //fin del main

