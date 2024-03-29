#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 4994
#define BUF_SIZE 1024


int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usar: %s <IP address> <mensaje>\n", argv[0]);
        exit(1);
    }
    struct sockaddr_in servaddr;
    int sockfd, n;
    char buffer[BUF_SIZE];
    
    //1. Creo el socket de conexiones TCP
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    //2. Cargo campos para conectar
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr=inet_addr(argv[1]);
    socklen_t tam_servaddr=sizeof(servaddr);
    //inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
    
    //3. Conecto al servidor
    if (connect (sockfd, (struct sockaddr *)&servaddr,tam_servaddr)<0){
        perror("connect");
        close(sockfd);
        exit(-1);
    }
    printf("Conectado al servidor...\n");
    uint16_t tam_msg=strlen(argv[2]);
    uint tam=htons(tam_msg);
    send(sockfd, tam, sizeof(tam_msg), 0);
    send(sockfd, argv[2], tam_msg, 0);
    n = recv(sockfd, buffer, BUF_SIZE, 0);
    buffer[n] = '\0';

    printf("Mensaje de echo: %s\n", buffer);

    close(sockfd);

    return 0;
}
