#include "server.h"
#define BUFFER_SIZE 100
#define MAX_THREAD 4

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

int NB_PARTIE;

int sock_ecoute;

pthread_mutex_t verrou;

int continuer_serveur;

void do_or_exit(int result, const char *erreur)
{
	if (result < 0)
	{
		perror(erreur);
		exit(EXIT_FAILURE);
	}
}
void init_sockaddr_in(struct sockaddr_in *serveur, unsigned short port_serveur)
{
	serveur->sin_family = AF_INET;
	serveur->sin_port = htons(port_serveur);
	serveur->sin_addr.s_addr = htonl(INADDR_ANY);
}

int initialiser_socket(struct sockaddr_in *serveur, short mon_port)
{
	int sock;
	init_sockaddr_in(serveur, mon_port);
	do_or_exit(sock = socket(AF_INET, SOCK_STREAM, 0), "Erreur lors de la création de socket TCP");

	do_or_exit(bind(sock, (struct sockaddr *) serveur, sizeof(struct sockaddr_in)),
		"Erreur lors de l'appel à la fonction bind");
	do_or_exit(listen(sock, 10), "Erreur lors de l'appel à la fonction listen");
	return sock;
}

void envoyer_etat_jeu(Partie *p, char *mot_trouve)
{
	//D'abord le mot avec les parties qu'il a reussi a dechifrer
	int taille = strlen(mot_trouve);
	do_or_exit(send(p->socket, &taille, sizeof(taille), 0), "Erreur send");
	do_or_exit(send(p->socket, mot_trouve, taille, 0), "Erreur send");
	//Ensuite le nombres de tentatives restantes
	char converti[2];
	sprintf(converti, "%d", p->tentative_restantes);
	taille = strlen(converti);
	converti[1] = '\0';
	do_or_exit(send(p->socket, &taille, sizeof(taille), 0), "Erreur send");
	do_or_exit(send(p->socket, converti, taille, 0), "Erreur send");
}

char *charger_partie(Partie *p, int *tentatives_restantes)
{
	char c;
	char *mot_trouve;
	do_or_exit(recv(p->socket, &c, sizeof(char), 0), "Erreur Recv");
	if (c == 'v')
	{
		char buff[BUFFER_SIZE];
		int taille;
		//On recupere le mot trouve
		do_or_exit(recv(p->socket, &taille, sizeof(taille), 0), "Erreur Recv");
		do_or_exit(recv(p->socket, buff, taille, 0), "Erreur Recv");
		mot_trouve = malloc(strlen(buff) + 1);
		strcpy(mot_trouve, buff);
		mot_trouve[strlen(buff)] = '\0';
		//On recupere le mot cherche
		do_or_exit(recv(p->socket, &taille, sizeof(taille), 0), "Erreur Recv");
		do_or_exit(recv(p->socket, buff, taille, 0), "Erreur Recv");
		p->mot_cherche = malloc(strlen(buff) + 1);
		strcpy(p->mot_cherche, buff);
		//On recupere le nombre de tentatives restantes
		do_or_exit(recv(p->socket, &taille, sizeof(taille), 0), "Erreur Recv");
		do_or_exit(recv(p->socket, buff, taille, 0), "Erreur Recv");
		*tentatives_restantes = atoi(buff);
		return mot_trouve;
	}
	return NULL;
}

void nouvelle_partie(Partie *p)
{
	char *mot_trouve = NULL;
	//S'il n'y a aucune sauvegarde on demarre la partie avec la config intiale
	if ((mot_trouve = charger_partie(p, &p->tentative_restantes)) == NULL)
	{
		mot_trouve = malloc(strlen(p->mot_cherche) + 1);
		strcpy(mot_trouve, p->mot_cherche);
		int i = 0;
		while (p->mot_cherche[i] != '\0')
		{
			mot_trouve[i] = '*';
			i++;
		}
		mot_trouve[strlen(p->mot_cherche)] = '\0';
	}
	else
	// Si la partie demarre a partie d'une sauvegarde alors on met a jour la variable letrres trouvees 
	{
		int i = 0;
		while (mot_trouve[i] != '\0')
		{
			if (strchr(p->lettres_trouvees, mot_trouve[i]) == NULL && mot_trouve[i] != '*')
			{
				char concat[2];
				concat[0] = mot_trouve[i];
				concat[1] = '\0';
				strcat(p->lettres_trouvees, concat);
			}
			i++;
		}
	}
	printf("Partie en cours avec le client : %s le mot cherche est %s\n", inet_ntoa(p->info_joueur.sin_addr), p->mot_cherche);

	while (p->statut_partie == 0 && continuer_serveur)
	{
		char c;
		//on envoie l'etat du jeu au client
		envoyer_etat_jeu(p, mot_trouve);
		// On recupere le charactere propose par le client
		do_or_exit(recv(p->socket, &c, sizeof(char), 0), "Erreur Recv");

		// Si la lettre figure dans le mot et n'a pas precedemment ete propose on l'ajoute aux lettre trouvees
		if (strchr(p->mot_cherche, c) != NULL && strchr(p->lettres_trouvees, c) == NULL)
		{
			char concat[2];
			concat[0] = c;
			concat[1] = '\0';
			strcat(p->lettres_trouvees, concat);
		}
		else
			p->tentative_restantes--;

		//On met a jour la variable mot trouve
		int k = 0;
		while (p->mot_cherche[k] != '\0')
		{
			if (strchr(p->lettres_trouvees, p->mot_cherche[k]) != NULL)
				mot_trouve[k] = p->mot_cherche[k];
			k++;
		}

		// Si le mot est trouve alors la partie prend fin
		if (strcmp(p->mot_cherche, mot_trouve) == 0)
			p->statut_partie = 1;
		else if (p->tentative_restantes == 0)
			p->statut_partie = 2;
      
		// sinon avant de continuer la partie on verifie que l'arret du serveur n'a pas ete demande
		
    // dans ce cas on signale au client de sauvegarder la partie (statut 3)
		else if (continuer_serveur == 0)
			p->statut_partie = 3;
    
    // Cas ou le client demande l'arret de la partie
    else if(c == '*')
    {
      p->statut_partie = 4;
      p->tentative_restantes++;
    }
      

		//Envoie du statut de la partie au client
		char converti[3];
		sprintf(converti, "%d", p->statut_partie);
		int taille = strlen(converti);
		do_or_exit(send(p->socket, &taille, sizeof(taille), 0), "Erreur send");
		do_or_exit(send(p->socket, converti, taille, 0), "Erreur send");
	}
	// Si la partie est termine ou si l'aret du serveur est demande on envoie la solution
	if (p->statut_partie != 0)
	{
		int taille = strlen(p->mot_cherche);
		do_or_exit(send(p->socket, &taille, sizeof(taille), 0), "Erreur send");
		do_or_exit(send(p->socket, p->mot_cherche, taille, 0), "Erreur send");
	}
}

void *creer_th(void *arg)
{
	char buff[BUFFER_SIZE];
	Partie *p = (Partie*) arg;
	int sock = p->socket;
	nouvelle_partie(p);
	close(sock);
	printf("Partie terminee avec le client : %s \n", inet_ntoa(p->info_joueur.sin_addr));
	pthread_mutex_lock(&verrou);
	NB_PARTIE--;
	pthread_mutex_unlock(&verrou);
	free(p->mot_cherche);
	free(arg);
}

Partie* init_partie(int sock)
{
	Partie * p;
	p = malloc(sizeof(Partie));
	p->lettres_trouvees[0] = '\0';
	p->statut_partie = 0;
	p->tentative_restantes = 8;
	p->socket = sock;
	// On intialise le mot a trouver pour cette partie
	srand(time(0));
	int num = (rand() % (30 - 1 + 1)) + 1;
	FILE *f = fopen("server/ressources/mots.txt", "r");
	if (f == NULL)
	{
		printf("Erreur ouverture fichier\n");
		exit(EXIT_FAILURE);
	}
	char ligne[256];
	int i = 0;
	while (fgets(ligne, sizeof(ligne), f))
	{
		i++;
		if (i == num)
		{
			p->mot_cherche = malloc(strlen(ligne) + 1);
			strcpy(p->mot_cherche, ligne);
			p->mot_cherche[strcspn(p->mot_cherche, "\n")] = 0;	//On enleves le retour chariot
			break;
		}
	}
	fclose(f);
	return p;
}

void set_continuer(int signal)
{
	continuer_serveur = 0;
}

int main(int argc, char **argv)
{
	struct sockaddr_in serveur;
	struct sockaddr_in client;
	char message[BUFFER_SIZE];
	int taille_serveur = sizeof(serveur);
	short port = 0;
	char buffer[255];
	sock_ecoute = 0;
	continuer_serveur = 1;

	struct sigaction s;
	s.sa_handler = set_continuer;
	sigaction(SIGINT, &s, 0);

	pthread_t ths[MAX_THREAD];
	pthread_mutex_init(&verrou, NULL);
	NB_PARTIE = 0;
	if (argc != 2)
	{
		fprintf(stderr, "usage: %s port\n", argv[0]);
		return EXIT_FAILURE;
	}
	port = atoi(argv[1]);
	sock_ecoute = initialiser_socket(&serveur, port);

	int sock_comm;
	//Le serveur ecoute et attend de nouvelles connexion
	do_or_exit(listen(sock_ecoute, 30), "Erreur listen");
	printf("En attente de connexion\n");

	while (continuer_serveur)
	{
		if (NB_PARTIE < MAX_THREAD)
		{
			sock_comm = accept(sock_ecoute, (struct sockaddr *) &client, (socklen_t*) &taille_serveur);
			if (sock_comm != -1)
			{
				printf("Connexion acceptée: (%s,%4d)\n", inet_ntoa(serveur.sin_addr), ntohs(serveur.sin_port));
				Partie *p = init_partie(sock_comm);
				pthread_create(&ths[NB_PARTIE], NULL, creer_th, p);
				NB_PARTIE++;
				sleep(5);
			}
		}
		else
		{
			printf("Serveur occupee, %d partie deja en cours ..\n", NB_PARTIE);
			sleep(5);
		}
	}
	close(sock_ecoute);
	printf("Sauvegrade des parties en cours et arret du serveur ..\n");
	int i;
	for (i = 0; i < NB_PARTIE; ++i)
		pthread_join(ths[i], NULL);

	pthread_mutex_destroy(&verrou);
	return EXIT_SUCCESS;
}