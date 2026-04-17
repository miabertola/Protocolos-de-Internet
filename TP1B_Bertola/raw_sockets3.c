#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netpacket/packet.h>
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>

int main() {
    int sockfd;
    char buffer[65536];

    // Crear raw socket (captura todo)
    sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sockfd < 0) {
        perror("Socket error");
        return 1;
    }

    printf("Sniffer iniciado...\n");

    while (1) {
        int packet_size = recvfrom(sockfd, buffer, sizeof(buffer), 0, NULL, NULL);
        if (packet_size < 0) {
            perror("recvfrom error");
            continue;
        }

        // Leer cabecera Ethernet
        struct ethhdr *eth = (struct ethhdr *)buffer;

        // Filtrar solo IPv4
        if (ntohs(eth->h_proto) != 0x0800)
            continue;

        // Leer cabecera IP
        struct iphdr *ip = (struct iphdr *)(buffer + sizeof(struct ethhdr));

        // Mostrar protocolo
        if (ip->protocol == 1)
            printf("ICMP detectado\n");
        else if (ip->protocol == 6)
            printf(">>> TCP detectado <<<\n");
        else if (ip->protocol == 17)
            printf("UDP detectado\n");
        else
            printf("Otro protocolo: %d\n", ip->protocol);

        // Mostrar info ICMP
        if (ip->protocol == 1) {
            struct icmphdr *icmp = (struct icmphdr *)(buffer + sizeof(struct ethhdr) + ip->ihl * 4);

            printf("ICMP -> Tipo=%d Codigo=%d\n",
                   icmp->type,
                   icmp->code);
        }
    }

    close(sockfd);
    return 0;
}