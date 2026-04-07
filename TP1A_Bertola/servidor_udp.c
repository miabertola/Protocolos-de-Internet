#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PUERTO 8080

int main() {
    int sockfd;
    char buffer[1024];
    struct sockaddr_in servidor, cliente;
    socklen_t len = sizeof(cliente);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    servidor.sin_family = AF_INET;
    servidor.sin_port = htons(PUERTO);
    servidor.sin_addr.s_addr = INADDR_ANY;

    bind(sockfd, (struct sockaddr*)&servidor, sizeof(servidor));

    printf("Servidor UDP esperando mensajes...\n");

    recvfrom(sockfd, buffer, sizeof(buffer), 0,
             (struct sockaddr*)&cliente, &len);

    printf("Mensaje recibido: %s\n", buffer);

    char *respuesta = "Mensaje recibido (UDP)";
    sendto(sockfd, respuesta, strlen(respuesta), 0,
           (struct sockaddr*)&cliente, len);

    close(sockfd);
    return 0;
}
