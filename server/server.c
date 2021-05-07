#include "server.h"

#define BUFFER_SIZE 255
#define MAX_THREAD 4

typedef struct partie {
  char * mot_cherche;
  char * lettres_trouvees;
  int tentative_restantes;
  int statut_partie;
  int socket;
  struct sockaddr_in info_joueur;
}
Partie;

int sock_ecoute;
pthread_mutex_t verrou;

void do_or_exit(int result,
  const char * erreur) {
  if (result < 0) {
    perror(erreur);
    exit(EXIT_FAILURE);
  }
}
void init_sockaddr_in(struct sockaddr_in * serveur, unsigned short port_serveur) {
  serveur -> sin_family = AF_INET;
  serveur -> sin_port = htons(port_serveur);
  serveur -> sin_addr.s_addr = htonl(INADDR_ANY);
}

int initialiser_socket(struct sockaddr_in * serveur, short mon_port) {
  int sock;
  init_sockaddr_in(serveur, mon_port);
  do_or_exit(sock = socket(AF_INET, SOCK_STREAM, 0), "Erreur lors de la création de socket TCP");

  do_or_exit(bind(sock, (struct sockaddr * ) serveur, sizeof(struct sockaddr_in)),
    "Erreur lors de l'appel à la fonction bind");
  do_or_exit(listen(sock, 10), "Erreur lors de l'appel à la fonction listen");
  return sock;
}

void nouvelle_partie(Partie * p) {
  char * mot_trouve = malloc(strlen(p->mot_cherche)+1);
  strcpy(mot_trouve,p->mot_cherche);
  int i=0;
  printf("Le mot cherche est '%s' | %ld \n",p->mot_cherche,strlen(p->mot_cherche));
  while(p->mot_cherche[i]!='\0')
  {
    mot_trouve[i]='*';
    i++;
  }
  mot_trouve[strlen(p->mot_cherche)] = '\0';
  printf("mot trouve :%s\n",mot_trouve);
  send(p->socket,mot_trouve,strlen(mot_trouve)+1,0);

  while(p->statut_partie == 0)
  {
      int i = 0;
      break;
  }
}

void * creer_th(void * arg) {
  char buff[BUFFER_SIZE];
  Partie * p = (Partie * ) arg;
  int sock = p -> socket;
  recv(sock, buff, BUFFER_SIZE, 0);
  pthread_mutex_lock( & verrou);
  printf("Le client dit : %s \n", buff);
  send(sock, p -> mot_cherche, strlen(p -> mot_cherche) + 1, 0);
  sleep(5);
  pthread_mutex_unlock( & verrou);
  nouvelle_partie(p);
  close(sock);
}

Partie * init_partie(int sock) {
  Partie * p;
  p = malloc(sizeof(Partie));
  p -> lettres_trouvees = NULL;
  p -> statut_partie = 0;
  p -> tentative_restantes = 10;
  p -> socket = sock;
  // On intialise le mot a trouver pour cette partie
  srand(time(0));
  int num = (rand() % (30 - 1 + 1)) + 1;
  FILE * f = fopen("server/ressources/mots.txt", "r");
  if (f == NULL) {
    printf("Erreur ouverture fichier\n");
    exit(EXIT_FAILURE);
  }
  char ligne[256];
  int i = 0;
  while (fgets(ligne, sizeof(ligne), f)) {
    i++;
    if (i == num) 
    {
      printf("line : '%s'\n",ligne);
      p -> mot_cherche = malloc(strlen(ligne)+1);
      strcpy(p->mot_cherche, ligne);
      p->mot_cherche[strcspn(p->mot_cherche, "\n")] = 0; //On enleves le retour chariot
      break;
    }
  }
  fclose(f);
  return p;
}

int main(int argc, char ** argv) {
  struct sockaddr_in serveur;
  struct sockaddr_in client;
  char message[BUFFER_SIZE];
  int taille_serveur = sizeof(serveur);
  short port = 0;
  char buffer[255];
  sock_ecoute = 0;
  int i = 0;

  pthread_t ths[MAX_THREAD];
  pthread_mutex_init( & verrou, NULL);

  if (argc != 2) {
    fprintf(stderr, "usage: %s port\n", argv[0]);
    return EXIT_FAILURE;
  }
  port = atoi(argv[1]);
  sock_ecoute = initialiser_socket( & serveur, port);

  int sock_comm;
  //Le serveur ecoute et attend de nouvelles connexion
  do_or_exit(listen(sock_ecoute, 30), "Erreur listen");
  printf("En attente de connexion\n");

  while (1) {
    sock_comm = accept(sock_ecoute, (struct sockaddr * ) & client, (socklen_t * ) & taille_serveur);
    printf("Connexion acceptée: (%s,%4d)\n", inet_ntoa(serveur.sin_addr), ntohs(serveur.sin_port));
    Partie * p = init_partie(sock_comm);
    pthread_create(&ths[i++], NULL, creer_th, p);
  }

  pthread_mutex_destroy(&verrou);
  return EXIT_SUCCESS;
}