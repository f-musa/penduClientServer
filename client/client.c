#include "client.h"
#define BUFFER_SIZE 100

int commencer_sauvegarde;

void do_or_exit(int result, const char *erreur)
{
	if (result < 0)
	{
		perror(erreur);
		exit(EXIT_FAILURE);
	}
}

void init_sockaddr_in(struct sockaddr_in *serveur, unsigned short port_serveur, char *ip_serveur)
{
	serveur->sin_family = AF_INET;
	serveur->sin_port = htons(port_serveur);
	inet_aton(ip_serveur, &(serveur->sin_addr));
}

int initialiser_socket(char *ip_serveur, short port_serveur,
	struct sockaddr_in *moi, struct sockaddr_in *serveur)
{
	int sock;
	do_or_exit(sock = socket(AF_INET, SOCK_STREAM, 0), "Erreur lors de la création de socket TCP");
	init_sockaddr_in(serveur, port_serveur, ip_serveur);

	//On établit la connexion avec le serveur
	do_or_exit(connect(sock, (struct sockaddr *) serveur, sizeof(struct sockaddr_in)),
		"Erreur lors de la connexion au serveur");

	int len = sizeof(struct sockaddr_in);
	// Permet de connaître le port attribué par le système
	getsockname(sock, (struct sockaddr *) moi, (socklen_t*) &len);

	moi->sin_addr = *((struct in_addr *) gethostbyname("localhost")->h_addr_list[0]);

	return sock;
}
char tour_partie(char *mot_trouve, int tentatives_restantes)
{
	char c;
	printf("****************************Jeu du Pendu****************************\n");
	printf("    --Taper '*' pour quitter la partie--\n\n");
	printf("Mot:\t%s\n", mot_trouve);
	printf("Tentatives restantes:\t%d\n", tentatives_restantes);
	printf("Proposer une lettre : ");
	scanf(" %c", &c);
	getchar();	//On elimine le retour chariot
	printf("\n");
	return c;
}

void sauvegarde(char *mot_trouve, char *mot_cherche, char *tentatives_restantes)
{
	FILE *log = fopen(".save.log", "w");
	fprintf(log, "%s\n", mot_trouve);
	fprintf(log, "%s\n", mot_cherche);
	fprintf(log, "%s\n", tentatives_restantes);
	fclose(log);
}
void chargement(int sock_serveur)
{
	FILE *log = fopen(".save.log", "r");
	char c;
	if (log != NULL)
	{
		commencer_sauvegarde = 1;
		//On signale au serveur qu'une sauvegarde existe
		c = 'v';
		send(sock_serveur, &c, sizeof(char), 0);
		char *buffer;
		size_t s = 0;
		//Lecture et envoie du mot trouve
		getline(&buffer, &s, log);
		buffer[strcspn(buffer, "\n")] = 0;
		int taille = strlen(buffer);
		do_or_exit(send(sock_serveur, &taille, sizeof(taille), 0), "Erreur send");
		do_or_exit(send(sock_serveur, buffer, taille, 0), "Erreur send");

		//Lecture et envoie du mot cherche
		getline(&buffer, &s, log);
		buffer[strcspn(buffer, "\n")] = 0;
		taille = strlen(buffer);
		do_or_exit(send(sock_serveur, &taille, sizeof(taille), 0), "Erreur send");
		do_or_exit(send(sock_serveur, buffer, taille, 0), "Erreur send");

		//Lecture et envoie du nombre de tentatives restantes
		getline(&buffer, &s, log);
		buffer[strcspn(buffer, "\n")] = 0;
		taille = strlen(buffer);
		do_or_exit(send(sock_serveur, &taille, sizeof(taille), 0), "Erreur send");
		do_or_exit(send(sock_serveur, buffer, taille, 0), "Erreur send");

		fclose(log);
	}
	else
	{
		//On signale au serveur qu'il n'y a aucune sauvegarde
		c = 'f';
		send(sock_serveur, &c, sizeof(char), 0);
	}
}

int main(int argc, char **argv)
{
	struct sockaddr_in moi;
	struct sockaddr_in serveur;
	short port_serveur = 0;
	char *ip_serveur;
	int sock = 0; /*socket de communication */
	commencer_sauvegarde = 0;
	if (argc != 3)
	{
		fprintf(stderr, "usage: %s host port_serveur\n", argv[0]);
		return EXIT_FAILURE;
	}
	port_serveur = atoi(argv[2]);
	ip_serveur = argv[1];

	printf("Connexion au serveur: %d\n", port_serveur);
	sock = initialiser_socket(ip_serveur, port_serveur, &moi, &serveur);
	char statut_partie[3];
	*statut_partie = 0;
	char mot_trouve[BUFFER_SIZE];
	*mot_trouve = 0;
	char tentatives_restantes[3];
	*tentatives_restantes = 0;

	//S'il y a une partie existante on la charge
	chargement(sock);

	do {
		int taille = 0;
		//On recupere l'etat du jeu avant de l'afficher
		do_or_exit(recv(sock, &taille, sizeof(taille), 0), "Erreur Recv");
		do_or_exit(recv(sock, mot_trouve, taille, 0), "Erreur Recv");
		do_or_exit(recv(sock, &taille, sizeof(taille), 0), "Erreur Recv");
		do_or_exit(recv(sock, tentatives_restantes, taille, 0), "Erreur recv");
		char c;
		c = tour_partie(mot_trouve, atoi(tentatives_restantes));
		// On envoie au serveur le charactere propose

		send(sock, &c, sizeof(char), 0);

		//On recupere l'etat de la partie
		do_or_exit(recv(sock, &taille, sizeof(taille), 0), "Erreur Recv");
		do_or_exit(recv(sock, &statut_partie, taille, 0), "Erreur Recv");
		system("clear");
	} while (strcmp(statut_partie, "0") == 0);

	// On nettoie l'ecran et on recupere la solution du jeu
	system("clear");
	int taille = 0;
	char mot_cherche[BUFFER_SIZE];
	do_or_exit(recv(sock, &taille, sizeof(taille), 0), "Erreur Recv");
	do_or_exit(recv(sock, &mot_cherche, taille, 0), "Erreur Recv");

	//Si la partie s'est termine de façon normale on affiche le resultat du jeu
	if (strcmp(statut_partie, "1") == 0 || strcmp(statut_partie, "2") == 0)
	{
		strcmp(statut_partie, "1") == 0 ? printf("Le mot etait : %s \nVous avez gagne !\n", mot_cherche) :
			printf("Le mot etait : %s \nVous avez perdu !\n", mot_cherche);
		// On supprime la derniere sauvegarde si on a commencer la partie a partir d'une sauvegarde
		int k;
		if (commencer_sauvegarde == 1)
			if ((k = unlink(".save.log")) != 0)
				perror("Erreur lors de la suppression du fichier log");
	}
	//Si la partie a ete termine par le serveur ou par le client alors on la sauvegarde
	else
	{
		(strcmp(statut_partie, "3") == 0) ? printf("Le serveur a termine la partie\nSauvegarde de la partie en cours..\n") :
      printf("Arret de la partie \nSauvegarde de la partie en cours..\n");
		sauvegarde(mot_trouve, mot_cherche, tentatives_restantes);
	}
	close(sock);

	return EXIT_SUCCESS;
}