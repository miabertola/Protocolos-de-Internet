#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netpacket/packet.h>
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <arpa/inet.h>

int main() {
    int sockfd;
    char buffer[65536];

    sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sockfd < 0) {
        perror("Socket error");
        return 1;
    }

    printf("Sniffer IP/TCP/UDP iniciado...\n");

    while (1) {
        int packet_size = recvfrom(sockfd, buffer, sizeof(buffer), 0, NULL, NULL);
        if (packet_size < 0) {
            perror("recvfrom error");
            continue;
        }

        struct ethhdr *eth = (struct ethhdr *)buffer;

        // Solo IPv4
        if (ntohs(eth->h_proto) != 0x0800)
            continue;

        struct iphdr *ip = (struct iphdr *)(buffer + sizeof(struct ethhdr));

        struct sockaddr_in src, dst;
        src.sin_addr.s_addr = ip->saddr;
        dst.sin_addr.s_addr = ip->daddr;

        printf("\n[IP] %s → %s\n",
               inet_ntoa(src.sin_addr),
               inet_ntoa(dst.sin_addr));

        // TCP
        if (ip->protocol == 6) {
            struct tcphdr *tcp = (struct tcphdr *)(buffer + sizeof(struct ethhdr) + ip->ihl * 4);

            printf("[TCP] detectado\n");
        }

        // UDP
        else if (ip->protocol == 17) {
            struct udphdr *udp = (struct udphdr *)(buffer + sizeof(struct ethhdr) + ip->ihl * 4);

            printf("[UDP] Puerto origen: %d | Puerto destino: %d\n",
                   ntohs(udp->source),
                   ntohs(udp->dest));
        }

        // ICMP (opcional mostrar también)
        else if (ip->protocol == 1) {
            printf("[ICMP] paquete detectado\n");
        }
    }

    close(sockfd);
    return 0;
}