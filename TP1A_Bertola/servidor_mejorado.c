#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PUERTO 8080
#define MAX 1024

int main() {
    int sockfd, cliente;
    struct sockaddr_in servidor, cliente_addr;
    socklen_t len = sizeof(cliente_addr);
    char buffer[MAX];

    // Crear socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Error al crear socket");
        exit(1);
    }

    // Configurar dirección
    servidor.sin_family = AF_INET;
    servidor.sin_port = htons(PUERTO);
    servidor.sin_addr.s_addr = INADDR_ANY;

    // Bind
    if (bind(sockfd, (struct sockaddr*)&servidor, sizeof(servidor)) != 0) {
        perror("Error en bind");
        exit(1);
    }

    // Listen
    if (listen(sockfd, 3) != 0) {
        perror("Error en listen");
        exit(1);
    }

    printf("Servidor esperando conexiones...\n");

    // Accept
    cliente = accept(sockfd, (struct sockaddr*)&cliente_addr, &len);
    if (cliente < 0) {
        perror("Error en accept");
        exit(1);
    }

    // Mostrar info del cliente
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &cliente_addr.sin_addr, ip, INET_ADDRSTRLEN);

    printf("Cliente conectado desde %s:%d\n",
           ip, ntohs(cliente_addr.sin_port));

    // Loop de comunicación
    while (1) {
        memset(buffer, 0, MAX);

        int bytes = recv(cliente, buffer, MAX, 0);
        if (bytes <= 0) {
            printf("Cliente desconectado\n");
            break;
        }

        // Si escribe FIN → termina
        if (strncmp(buffer, "FIN", 3) == 0) {
            printf("Cliente finalizó la conexión\n");
            break;
        }

        printf("Mensaje recibido: %s\n", buffer);

        // Eco (devuelve el mismo mensaje)
        send(cliente, buffer, strlen(buffer), 0);
    }

    close(cliente);
    close(sockfd);

    return 0;
}
