#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <unistd.h>
#include <arpa/inet.h>

// Use for get ip of loopback
// char* get_ip() {
//     int n;
//     struct ifreq ifr;
//     char arr[] = "lo";

//     n = socket(AF_INET, SOCK_DGRAM, 0);
//     ifr.ifr_addr.sa_family =  AF_INET;
//     strncpy(ifr.ifr_name, arr, IFNAMSIZ - 1);
//     ioctl(n, SIOCGIFADDR, &ifr);
//     close(n);
//     char *ip = inet_ntoa(((struct sockaddr_in*)&ifr.ifr_addr)->sin_addr);
//     return ip;
// }

void get_ssh(char *port) {
    char *cmd = malloc(200);
    FILE *fp;

    sprintf(cmd, "ssh -y -f -N -i /mnt/flash/external/ssh/id_dropbear -R localhost:%s:127.0.0.1:22 switch-control@192.46.209.149 -p 2232", port);
    // printf("%s\n",cmd);

    fp = popen(cmd, "r");
    if (!fp) {
        return ;
    }

    pclose(fp);
}

int main(int argc, char *argv[]) {

    //char *ip = get_ip();
    get_ssh(argv[1]);
    printf("okay");
}
