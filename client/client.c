#include "client.h"

void do_or_exit(int result, const char * erreur){
  if(result < 0){
    perror(erreur);
    exit(EXIT_FAILURE);
  }
}

void init_sockaddr_in(struct sockaddr_in * serveur, unsigned short port_serveur, char * ip_serveur){
    serveur->sin_family = AF_INET;
    serveur->sin_port = htons(port_serveur);
    inet_aton(ip_serveur,&(serveur->sin_addr));
}

int initialiser_socket(char * ip_serveur, short port_serveur,
		       struct sockaddr_in * moi, struct sockaddr_in * serveur){  
  int sock;
  do_or_exit(sock = socket(AF_INET, SOCK_STREAM, 0), "Erreur lors de la création de socket TCP");
  init_sockaddr_in(serveur, port_serveur, ip_serveur);
  
  //On établit la connexion avec le serveur
  do_or_exit(connect(sock, (struct sockaddr *)serveur, sizeof(struct sockaddr_in)),
	     "Erreur lors de la connexion au serveur");
  
  int len = sizeof(struct sockaddr_in);
  // Permet de connaître le port attribué par le système
  getsockname(sock, (struct sockaddr *)moi, (socklen_t *) &len); 
  
  moi->sin_addr = *((struct in_addr*) gethostbyname("localhost")->h_addr_list[0]) ;

  return sock;
}

void echange_avec_serveur(struct sockaddr_in serveur, struct sockaddr_in moi, int sock){
  char message[1024] = "Wesh mon gars le serveur, ça dit quoi ?";
  char reponse[30];
    
  write(sock, message, strlen(message)+1),
    
  read(sock, reponse, 30);
    printf("client (%s,%4d) recoit de ", inet_ntoa(moi.sin_addr),ntohs(moi.sin_port));
  printf("(%s,%4d) : \"%s\" \n", inet_ntoa(serveur.sin_addr), ntohs(serveur.sin_port), reponse);
}

int main(int argc , char ** argv)
{
  struct sockaddr_in moi; 
  struct sockaddr_in serveur; 
  short port_serveur = 0;
  char * ip_serveur;
  int sock = 0; /* socket de communication */

    if (argc != 3) {
      fprintf(stderr,"usage: %s host port_serveur\n", argv[0]);
      return EXIT_FAILURE;
    }
    port_serveur = atoi(argv[2]);
    ip_serveur = argv[1];

    
    printf("Connexion au serveur: %d\n", port_serveur);
    sock = initialiser_socket(ip_serveur, port_serveur, &moi, &serveur);
    while(1){
          echange_avec_serveur(serveur, moi, sock);
           sleep(2);
    }
    close(sock);
     
  return EXIT_SUCCESS;
}