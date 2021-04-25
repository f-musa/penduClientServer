#include "client.h"

void init_sockaddr_in(struct sockaddr_in * serveur, unsigned short port_serveur, char * ip_serveur){
    serveur->sin_family = AF_INET;
    serveur->sin_port = htons(port_serveur);
    inet_aton(ip_serveur,&(serveur->sin_addr));
}

int initialiser_socket(char * ip_serveur, unsigned short port_serveur, struct sockaddr_in * serveur){
   int sock;
   init_sockaddr_in(serveur, port_serveur, ip_serveur);
   if (sock = socket(AF_INET, SOCK_DGRAM, 0) < 0) {
      perror("Erreur lors de la création de socket UDP");
   }
   return sock;   
}

