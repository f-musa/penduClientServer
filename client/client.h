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
void init_sockaddr_in(struct sockaddr_in *serveur, unsigned short port_serveur, char *ip_serveur);
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
   @author Julien David
*/
int initialiser_socket(char *ip_serveur, short port_serveur,
	struct sockaddr_in *moi, struct sockaddr_in *serveur);

/**
   Cette fonction permet de simplifier l'ecriture d'appels à des fonctions
   systèmes, qui renvoient une valeur négative en cas d'erreur.
   Ainsi, si la fonction échoue, on affiche un message d'erreur et on quitte le programme.
   @param result est le résultat de la fonctione appelée
   @param erreur est le message d'erreur à afficher si `result` est négative.
   @author Julien David
*/
void do_or_exit(int result, const char *erreur);

/**
 *Cette fonction affiche au client l'etat de la partie en cours et lui permet de proposer
 *une lettre. 
 * 
 *@param mot_trouve est le mot jusque là deviné  par le joueur. 
 *@param tentatives_restantes est le nombre de tentatives restantes au joueur pour trouver le mot.
 *@return la lettre proposée par le joueur.
 *@require mot_trouve!=NULL && tentatives_restantes!=NULL
 *@author Serigne Abdou Lat Sarr 
 */

char tour_partie(char *mot_trouve, int tentatives_restantes);

/** 
 *Cette fonction permet au client de sauvegarder l'etat de la partie en cours dans un fichier.
 *@param mot_trouve est le mot jusque jusque là deviné par le joueur.
 *@param mot_cherche est le mot à trouver pour cette partie.
 *@param tentatives_restantes est le le nombre de tentatives restantes au joueur pour trouver le mot.
 *@require mot_trouve!=NUL && mot_cherche!=NULL && tentatives_restantes!=NULL
 *@author Serigne Abdou Lat Sarr 
 */
void sauvegarde(char *mot_trouve, char *mot_cherche, char *tentatives_restantes);

/**
 *Cette fonction verifie si le client posséde une sauvegarde, et si c'est le cas envoie les donnees de la 
 *sauvegarde au serveur. 
 *@param sock_serveur socket permettant la communication avec le serveur.
 *@require sock_serveur>0
 *@author Sergine Abdou Lat Sarr
 */
void chargement(int sock_serveur);