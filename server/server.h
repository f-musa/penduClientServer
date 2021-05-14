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

typedef struct partie
{
	char *mot_cherche;
	char lettres_trouvees[27];
	int tentative_restantes;
	int statut_partie;
	int socket;
	struct sockaddr_in info_joueur;
}
Partie;


/**
 *Initialise la structure sockaddr_in pour que la socket qui y sera liée
 *puisse communiquer avec toutes les adresses ips possibles, sur le port 'port_serveur'
 *@param serveur est un pointeur vers la structure que l'on souhaite initialiser
 *@param port_serveur est le port auquel la socket sera liée.
 *@require serveur != NULL
 *@author Julien David
 */
void init_sockaddr_in(struct sockaddr_in *serveur, unsigned short port_serveur);

/**
   *Initialise une socket permettant d'accepter des connexions en TCP depuis n'importe
   *qu'elle adresse IP.
   *@param serveur est un pointeur vers la structure que l'on souhaite initialiser
   *@param port_serveur est le port auquel la socket sera liée.
   *@require serveur != NULL
   *@return une socket pouvant accepter des connexions si aucune erreur de survient,
           quitte le programme sinon.
   *@author Julien David
*/
int initialiser_socket(struct sockaddr_in *serveur, short mon_port);

/**
 *Cette fonction permet de simplifier l'ecriture d'appels à des fonctions
 *systèmes, qui renvoient une valeur négative en cas d'erreur.
 *Ainsi, si la fonction échoue, on affiche un message d'erreur et on quitte le programme.
 *@param result est le résultat de la fonctione appelée
 *@param erreur est le message d'erreur à afficher si `result` est négative.
 *@author Julien David
 */
void do_or_exit(int, const char *);

/** 
 *Cette fonction envoie l'etat du jeu au joueur.
 *C'est a dire le nombre de tentatives restantes et le mot deviné jusque là par le joueur. 
 *@param p est la structure contenant les données concernant la partie (socket et infos de la partie en cours).  
 *@param mot_trouve est le mot jusque là deviné par le joueur.
 *@require p!=NULL && mot_trouve!=NULL
 *@author Moussa Fall
 */
void envoyer_etat_jeu(Partie *p, char *mot_trouve);

/** 
 *Cette fonction permet de charger une sauvegarde si le client en posséde une, les donnees sont recuperées
 *du client et chargées dans la strcuture partie correspondant à la partie en cours. 
 *@param p est la structure contenant les données concernant la partie (sera mise a jour à l'issu du chargement).
 *@param tentatives_restantes est le nombre de tentatives restantes au joueur pour cette partie (sera mise a jour à l'issu du chargement).
 *@return le mot deviné par le joueur (correspond à celui de la sauvegarde).
 *@require p!=NULL
 *@author Moussa Fall
 */
char *charger_partie(Partie *p, int *tentatives_restantes);

/**
 *Cette fonction commence une nouvelle partie avec le client connecte au socket de la strucutre p.
 *@param p p est la structure contenant les données concernant la partie.
 *@require p!=NULL
 *@author Moussa Fall 
 */
void nouvelle_partie(Partie *p);

/**  
 *Cette fonction permet de lancer un nouveau thread, en lui affectant un client.
 *@param arg correspond à la structure contenant les données de la nouvelle partie devant etre géré par le thread.
 *@require arg!=NULL
 *@author Moussa Fall
 */
void *creer_th(void *arg);

/** 
 *Cette fonction permet d'initialiser une structure partie.
 *Un mot chosit sera choisi aléatoirement et representera le mot que le joueur devra deviner 
 *pour cette partie, un socket pour la communication entre le joueur et le serveur sera aussi intialisé. 
 *@param sock est le socket qui lie le joueur et le serveur.
 *@return une structure partie intialisée avec un mot aleatoire et un socket lié au joueur de la partie.
 *@require sock > 0
 *@author  Moussa Fall
 */
Partie* init_partie(int sock);

/** 
 *Cette fonction permet de mettre à jour la variable globale qui permet l'arret du serveur
 *en cas de reception du signal SIGINT.
 *@param signal est le signal qui enclenche l'arret du serveur.
 *@author Moussa Fall
 */
void set_continuer(int signal);