#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h> 
#include <pthread.h>
#include <signal.h>

/**
   Initialise la structure sockaddr_in pour que la socket qui y sera liée
   puisse communiquer avec toutes les adresses ips possibles, sur le port 'port_serveur'
   @param serveur est un pointeur vers la structure que l'on souhaite initialiser
   @param port_serveur est le port auquel la socket sera liée.
   @require serveur != NULL
*/
void init_sockaddr_in(struct sockaddr_in * serveur, unsigned short port_serveur);


/**
   Initialise une socket permettant d'accepter des connexions en TCP depuis n'importe
   qu'elle adresse IP.
   @param serveur est un pointeur vers la structure que l'on souhaite initialiser
   @param port_serveur est le port auquel la socket sera liée.
   @require serveur != NULL
   @return une socket pouvant accepter des connexions si aucune erreur de survient,
           quitte le programme sinon.
*/
int initialiser_socket(struct sockaddr_in * serveur, short mon_port);
 

/**
   Cette fonction permet de simplifier l'ecriture d'appels à des fonctions
   systèmes, qui renvoient une valeur négative en cas d'erreur.
   Ainsi, si la fonction échoue, on affiche un message d'erreur et on quitte le programme.
   @param result est le résultat de la fonctione appelée
   @param erreur est le message d'erreur à afficher si `result` est négative.
*/
void do_or_exit(int, const char * );
