#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>


int main() {
    int sockfd;
    char buffer[65536];
    struct sockaddr saddr;
    socklen_t saddr_len = sizeof(saddr);
    printf("Paso 1: Creo el raw socket para capturar trafico ICMP\n");
    
    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        perror("\nError al crear el socket");
        return 1;
    }


    printf("Paso 2: Socket creado correctamente.\n");
    
    printf("\n\nSniffer ICMP iniciado... (Ctrl+C para detener)\n\n");

    printf("Paso 3: El sniffer queda esperando la llegada de paquetes...\n");
    while (1) {
        printf("\nPaso 4: Esperando un paquete con recvfrom() \n");
        ssize_t packet_size = recvfrom(sockfd, buffer, sizeof(buffer), 0, &saddr, &saddr_len);
        if (packet_size < 0) {
            perror("recvfrom error");
            continue;
        }
        
        printf("Paso 5: Paquete recibido correctamente.\n");
        printf("Paso 6: Tamaño del paquete recibido: %zd bytes.\n", packet_size);
        printf("Paso 7: Interpretando el comienzo del buffer como cabecera IP\n");
        
        struct iphdr *ip = (struct iphdr *)buffer;
        printf("Paso 8: Leyendo el campo protocolo de la cabecera IP\n");
        printf("\nProtocolo detectado en IP: %d\n\n", ip->protocol);
        
        if (ip->protocol == IPPROTO_ICMP) {
            printf("Paso 9: el protocolo detectado es ICMP.\n");
            printf("Paso 10: Calculando donde comienza la cabecera ICMP\n");

            struct icmphdr *icmp = (struct icmphdr*)(buffer + ip->ihl * 4);
            printf("ICMP capturado: Tipo=%d Código=%d ID=%d Secuencia=%d\n",
                icmp->type,
                icmp->code,
                ntohs(icmp->un.echo.id),
                ntohs(icmp->un.echo.sequence)
            );

            printf("Paso 12: guardando el paquete en el archivo icmp_packet.bin\n");
            FILE *f = fopen("icmp_packet.bin", "wb");
            if (f) {
                fwrite(buffer, 1, packet_size, f);
                fclose(f);
                printf("Paso 13: paquete guardado en icmp_packet.bin\n");
                printf("\n\nPaquete guardado en icmp_packet.bin\n\n");  
                break; // solo capturamos uno para el ejemplo
            }

            printf("Paso 14: Finalizando.\n");
            
        }
    }
    printf("Paso 15: Cerrando el socket\n");
    close(sockfd);
    printf("Paso 16: Programa finalizado correctamente.\n");
    return 0;
}