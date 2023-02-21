
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#define T 255

int main(int argc, char *argv[]){

    if (argc<2){
        printf("Uso: %s <nombfich>\n", argv[0]);
        exit (1);
    }

    int fd, leido;
    char b[T];

    fd = open(argv[1],O_RDONLY);
    if (fd<0){
        perror("open");
        exit (1);
    }
    leido=read(fd,b,T-1);
    while (leido>0){
        b[leido]='\0';
        printf("%s",b);
        leido=read(fd,b,T-1);
    }
    if (leido<0){
        perror("read");
        exit (1);
    }

    if ((close(fd))<0){
        perror("close");
        exit (1);
    };

    return 0;
}