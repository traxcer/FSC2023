#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUF_SIZE 1024
#define PORT 4950

int main(int argc, char *argv[]) {
    struct sockaddr_in servaddr;
    int sockfd, n;
   // socklen_t len;
    char buf[BUF_SIZE];

    if (argc != 3) {
        printf("Usar: %s <IP address> <mensaje>\n", argv[0]);
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

    sendto(sockfd, argv[2], strlen(argv[2]), 0, (struct sockaddr *) &servaddr, sizeof(servaddr));

    n = recvfrom(sockfd, buf, BUF_SIZE, 0, NULL, NULL);
    buf[n] = '\0';

    printf("Mensaje modificado: %s\n", buf);

    close(sockfd);

    return 0;
}
