#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h> 
#include <pthread.h>

/**
   Initialise la structure sockaddr_in pour que la socket qui y sera liée
   puisse communiquer avec le serveur d'adresse 'ip_serveur', sur le port 'port_serveur'
   @param serveur est un pointeur vers la structure que l'on souhaite initialiser
   @param port_serveur est le port sur serveur avec lequel on souhaite communiquer
   @param ip_serveur est l'adresse du serveur avec lequel on souhaite communiquer
   @require serveur != NULL && ip_serveur != NULL
*/
void init_sockaddr_in(struct sockaddr_in * serveur, unsigned short port_serveur, char * ip_serveur);
/**
   Etablit une connexion TCP avec le serveur d'adresse 'ip_serveur' sur le port 'port_serveur'.
   Initialise les structures `serveur` et `moi`,
   qui contienne des informations sur l'ip et le port utilisés par les deux points d'accès.
   @param ip_serveur est l'adresse du serveur auquel on se souhaite se connecter
   @param port_serveur est le port sur serveur auquel on se souhaite se connecter
   @param serveur est un pointeur vers la structure représentant les informations du serveur
   @param moi est un pointeur vers la structure représentant les informations du client
   @require serveur != NULL && client != NULL && ip_serveur != NULL
   @return la socket conectée au serveur, quitte le programme en cas d'échec.
*/
int initialiser_socket(char * ip_serveur, short port_serveur,
		       struct sockaddr_in * moi, struct sockaddr_in * serveur);
/** 
  Recupere l'etat du jeu a partir des donnees reçus par le serveur

*/
void recevoir_etat(int sock, struct sockaddr_in serveur);


/** 
Envoie les actions du joueur au serveur

 */
void envoyer_actions(int sock, struct sockaddr_in serveur);


/**
   Cette fonction permet de simplifier l'ecriture d'appels à des fonctions
   systèmes, qui renvoient une valeur négative en cas d'erreur.
   Ainsi, si la fonction échoue, on affiche un message d'erreur et on quitte le programme.
   @param result est le résultat de la fonctione appelée
   @param erreur est le message d'erreur à afficher si `result` est négative.
*/
void do_or_exit(int result, const char * erreur );


/**
   Cette fonction envoie le message 'Quelle heure est-il?' au serveur, via une socket TCP
   préalablement initalisée. Quitte le programme en cas d'échec de l'envoi.
   @param sock est la socket qui est utilisée pour communiquer.
   @param serveur permet de préciser le destinataire du message.
   @param moi sert à afficher les propriétés du client au moment de l'envoi du message (ip, port)
   @require sock, moi et serveur sont déjà initialisées.
*/
void echange_avec_serveur(struct sockaddr_in serveur, struct sockaddr_in moi, int sock);