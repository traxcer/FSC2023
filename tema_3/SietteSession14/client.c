#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUF_SIZE 1024
#define PORT 5200

int main(int argc, char *argv[]) {
    struct sockaddr_in servaddr;
    int sockfd, n;
   // socklen_t len;
    char buf[BUF_SIZE];

    if (argc != 3) {
        printf("Usar: %s <IP address> <mensaje>\n", argv[0]);
        exit(1);
    }

    sockfd = socket(PF_INET, SOCK_STREAM, 0);

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    uint32_t ip = inet_addr(argv[1]);
    servaddr.sin_addr.s_addr=ip;
    //2.- Establecemos la conexión con el servidor
	if ((connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) ) < 0) {
		perror("connect");
		exit(1);
	}
    printf("Conectado...\");
    //3.- Rutina del servidor
    if (write_n(sockfd, argv[2], sizeof(argv[2])) != sizeof(argv[2])){
		perror("write_n buffer");
		return -1;
	}
    
    close(sockfd);

    return 0;
}
