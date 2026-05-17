#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PUERTO 8080

int main() {
    int sockfd;
    char buffer[1024];
    struct sockaddr_in servidor;
    socklen_t len = sizeof(servidor);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    servidor.sin_family = AF_INET;
    servidor.sin_port = htons(PUERTO);
    inet_pton(AF_INET, "127.0.0.1", &servidor.sin_addr);

    char *mensaje = "Hola desde cliente UDP";

    sendto(sockfd, mensaje, strlen(mensaje), 0,
           (struct sockaddr*)&servidor, len);

    recvfrom(sockfd, buffer, sizeof(buffer), 0,
             NULL, NULL);

    printf("Respuesta del servidor: %s\n", buffer);

    close(sockfd);
    return 0;
}