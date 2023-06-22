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
#include<sys/select.h>>
#include<signal.h>>

//defino estados y eventos
#define IDLE 0
#define EJECUTANDO 1
#define ESPERANDO 2



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
FD_SET(0,&cjto);
FD_SET(fd_pipe[0],&cjto);




}

int main(){
    //Control de la señales
    if(signal(SIGSTOP,m_sigstop)==SIG_ERR) {
        perror("signal");
        exit(1);
        }
    if(signal(SIGCONT,m_sigcont)==SIG_ERR) {
        perror("signal");
        exit(1);
        }
    if(signal(SIGKILL,m_sigkill)==SIG_ERR) {
        perror("signal");
        exit(1);
        }
    fd_fifo=open("fsc_fifo", O_RDONLY);
    pipe(fd_pipe);
    
    int estado=IDLE;
    int evento;

    while (fin==0){
        evento=espera_evento;    
        
        switch (estado) {
            case IDLE:
            /* code */
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

