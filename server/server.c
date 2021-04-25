#include "server.h"

/**
   Initialise la structure sockaddr_in pour que la socket qui y sera liée
   puisse communiquer avec toutes les adresses ips possibles, sur le port 'port_serveur'
   @param serveur est un pointeur vers la structure que l'on souhaite initialiser
   @param port_serveur est le port auquel la socket sera liée.
   @require serveur != NULL
*/
void init_sockaddr_in(struct sockaddr_in * serveur, unsigned short port_serveur){
  serveur->sin_family = AF_INET;
  serveur->sin_port = htons(port_serveur);
  serveur->sin_addr.s_addr = htonl(INADDR_ANY);
}
/**
   Initialise une socket permettant d'accepter des communications en UDP depuis n'importe
   qu'elle adresse IP.   
   @param port_serveur est le port auquel la socket sera liée.
   @return une socket prête à l'emploi si aucune erreur de survient, quitte le programme sinon.
*/
int initialiser_socket(unsigned short port_serveur){
    struct sockaddr_in serveur;
    int sock;
    init_sockaddr_in(&serveur, port_serveur);
    if (sock = socket(AF_INET, SOCK_DGRAM, 0) < 0) {
        perror("Erreur lors de la création de socket UDP");
    }
    if (bind(sock, (struct sockaddr *) &serveur, sizeof(struct sockaddr_in)) < 0) {
        perror("Erreur lors de l'appel à la fonction bind");
    }
  return sock;
}
