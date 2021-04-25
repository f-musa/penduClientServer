#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h> 

/**
   Initialise la structure sockaddr_in pour que la socket qui y sera liée
   puisse communiquer avec toutes les adresses ips possibles, sur le port 'port_serveur'
   @param serveur est un pointeur vers la structure que l'on souhaite initialiser
   @param port_serveur est le port auquel la socket sera liée.
   @require serveur != NULL
*/
void init_sockaddr_in(struct sockaddr_in * serveur, unsigned short port_serveur);


/**
   Initialise une socket permettant d'accepter des communications en UDP depuis n'importe
   qu'elle adresse IP.   
   @param port_serveur est le port auquel la socket sera liée.
   @return une socket prête à l'emploi si aucune erreur de survient, quitte le programme sinon.
*/
int initialiser_socket(unsigned short port_serveur);