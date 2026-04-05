#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PUERTO 8080

int main() {
    int sockfd, cliente;
    struct sockaddr_in servidor;
    char buffer[1024] = {0};

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    servidor.sin_family = AF_INET;
    servidor.sin_port = htons(PUERTO);
    servidor.sin_addr.s_addr = INADDR_ANY;

    bind(sockfd, (struct sockaddr*)&servidor, sizeof(servidor));
    listen(sockfd, 3);

    printf("Servidor esperando conexiones...\n");

    cliente = accept(sockfd, NULL, NULL);

    read(cliente, buffer, sizeof(buffer));
    printf("Mensaje recibido: %s\n", buffer);

    char *respuesta = "Mensaje recibido correctamente";
    write(cliente, respuesta, strlen(respuesta));

    close(cliente);
    close(sockfd);

    return 0;
}
