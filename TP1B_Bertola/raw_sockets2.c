#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
// agrego estas tres librerias
#include <netpacket/packet.h>
#include <net/ethernet.h>
#include <arpa/inet.h>

int main() {
    int sockfd;
    char buffer[65536];
    struct sockaddr saddr;
    socklen_t saddr_len = sizeof(saddr);

    // Crear raw socket
    sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL)); // ICMP --> IP todos los protocolos IP y AD_INET --> AF_PACKET
    if (sockfd < 0) {
        perror("Socket error");
        return 1;
    }

    // acá cambié el mensaje
    printf("Sniffer iniciado (capturando TODO el tráfico)... (Ctrl+C para detener)\n");

    while (1) {
        ssize_t packet_size = recvfrom(sockfd, buffer, sizeof(buffer), 0, &saddr, &saddr_len);
        if (packet_size < 0) {
            perror("recvfrom error");
            continue;
        }

        struct iphdr *ip = (struct iphdr*) buffer;
        printf("Paquete recibido. Protocolo: %d\n", ip->protocol); // para ir viendo los paquetes que llegan
        if (ip->protocol == IPPROTO_ICMP) {
            struct icmphdr *icmp = (struct icmphdr*)(buffer + ip->ihl * 4);
            printf("ICMP capturado: Tipo=%d Código=%d ID=%d Secuencia=%d\n",
                icmp->type,
                icmp->code,
                ntohs(icmp->un.echo.id),
                ntohs(icmp->un.echo.sequence)
            );

            // Guardar en archivo para analizar con Python
            FILE *f = fopen("icmp_packet.bin", "wb");
            if (f) {
                fwrite(buffer, 1, packet_size, f);
                fclose(f);
                printf("Paquete guardado en icmp_packet.bin\n");
                // break;
            }
        }
    }

    close(sockfd);
    return 0;
}