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
   puisse communiquer avec le serveur d'adresse 'ip_serveur', sur le port 'port_serveur'
   @param serveur est un pointeur vers la structure que l'on souhaite initialiser
   @param port_serveur est le port sur serveur avec lequel on souhaite communiquer
   @param ip_serveur est l'adresse du serveur avec lequel on souhaite communiquer
   @require serveur != NULL && ip_serveur != NULL
*/
void init_sockaddr_in(struct sockaddr_in * , unsigned short , char * );

/**
   Initialise une socket UDP, et les structures `serveur` et `moi`,
   qui contienne des informations sur l'ip et le port utilisés par les deux points d'accès.
   @param ip_serveur est l'adresse du serveur avec lequel on souhaite communiquer
   @param port_serveur est le port sur serveur avec lequel on souhaite communiquer
   @param serveur est un pointeur vers la structure représentant les informations du serveur
   @require serveur != NULL && ip_serveur != NULL
   @return la socket initialisée, quitte le programme en cas d'échec.
*/
int initialiser_socket(char * , unsigned short, struct sockaddr_in *);
