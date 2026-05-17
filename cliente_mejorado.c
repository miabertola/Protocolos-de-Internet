#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PUERTO 8080
#define MAX 1024

int main() {
    int sock;
    struct sockaddr_in servidor;
    char buffer[MAX];

    // Crear socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("Error al crear socket");
        exit(1);
    }

    // Configurar servidor
    servidor.sin_family = AF_INET;
    servidor.sin_port = htons(PUERTO);
    inet_pton(AF_INET, "127.0.0.1", &servidor.sin_addr);

    // Conectar
    if (connect(sock, (struct sockaddr*)&servidor, sizeof(servidor)) != 0) {
        perror("Error en connect");
        exit(1);
    }

    printf("Conectado al servidor. Escribí mensajes (FIN para salir):\n");

    // Loop de envío
    while (1) {
        memset(buffer, 0, MAX);

        // Leer desde teclado
        fgets(buffer, MAX, stdin);

        // Enviar al servidor
        send(sock, buffer, strlen(buffer), 0);

        // Si escribe FIN → salir
        if (strncmp(buffer, "FIN", 3) == 0) {
            break;
        }

        // Recibir respuesta
        memset(buffer, 0, MAX);
        recv(sock, buffer, MAX, 0);

        printf("Servidor: %s\n", buffer);
    }

    close(sock);
    return 0;
}