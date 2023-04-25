#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUF_SIZE 1024
#define PORT 4950

/*Prototipos*/
void sustituir(char * input, char ** output);
int damemoria(char *c);
void sust(char **p, char c);
int tamanioCadena(char c[]);
void concatena(char c1[], char c2[]); 


int main(void) {
    struct sockaddr_in servaddr, cliaddr;
    int sockfd, n;
    socklen_t len;
    char buf[BUF_SIZE];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

    while (1) {
        len = sizeof(cliaddr);
        n = recvfrom(sockfd, buf, BUF_SIZE, 0, (struct sockaddr *) &cliaddr, &len);
        buf[n] = '\0';
        char *c;
        sustituir(buf,&c);
        sendto(sockfd, c, strlen(c), 0, (struct sockaddr *) &cliaddr, sizeof(cliaddr));
    }

    return 0;
}
/*-------------------
Funciones Auxiliares
------------------*/
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