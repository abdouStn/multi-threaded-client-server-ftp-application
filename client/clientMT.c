/****************** *********** ****************/
/****************** CLIENT CODE ****************/
/****************** *********** ****************/

#include "client.h"

int main(int argc, char** argv){
    int port=0;
    if(argc< 2){
        fprintf(stderr, "Error: pas assez de parametre\n");
        exit(1);
    }
    else
        port=atoi(argv[1]);

	/*Socket*/
    int skt_fd_clt=socket(PF_INET, SOCK_STREAM, 0);
    if(skt_fd_clt==-1){
    	perror("skt_fd_srv");
    	exit(-1);
    }
    fprintf(stdout, "socket.......OK\n");

    /*sockaddr_in*/
	struct sockaddr_in dest; 
	memset(&dest, 0, sizeof(dest));
	dest.sin_family = AF_INET;
	dest.sin_addr.s_addr = htonl(INADDR_LOOPBACK); /* set destination IP number - localhost, 127.0.0.1*/ 
	dest.sin_port = htons(port);                /* set destination port number */
 
	/*Connect*/
	connect(skt_fd_clt, (struct sockaddr *)&dest, sizeof(struct sockaddr));
    fprintf(stdout, "connect......OK\n");
	//char mstring [SIZE_STR];
	uint8_t continuer=1;
	while(continuer){
		// On cree le buffer qui va contenir le nom du fichier.
		char nomdufichier[MAX_SIZE_SEND];
		char buffersend[MAX_SIZE_SEND];
		char bufferrecv[SIZE_PAQUET];
		//char bufferrecv_complet[SIZE_PAQUET];
		//char *tailledufichier_str; 
		char buff[4];
		//int cpt_recv=0;
		//size_t id=0;
		size_t nbDDL=0;//Si tout sest bien passé, afficher le resultat, sinon afficher une erreur
		//size_t nbfichiertampo=0;
		//uint64_t tailledufichier=0; 
		//uint64_t cpt=0;
		//size_t taille=SIZE_PAQUET;
		//uint8_t bienpasse=1;
		memset(&nomdufichier, 0, sizeof(nomdufichier));
		memset(&buffersend, 0, sizeof(buffersend));
		memset(&bufferrecv, 0, sizeof(bufferrecv));
	   	printf("-------------------------------------------------------------------\n");
		printf("| Nb de DDL(0) | LS(1) | DDL(2) | UP(3) | fermer la connexion (4) |\n");
		printf("-------------------------------------------------------------------\n");
		printf("Que voulez-vous faire? ");
		uint8_t choix_caractere = fgetc(stdin)-48;
		fflush(stdin);
	    FUNC_CLEAR_
		switch(choix_caractere){
			case 0:
				printf("Vous avez demandé le nombre de telechargement effectué.\n");
				//SEND
				memset(&bufferrecv, 0, sizeof(buff));
				memset(&buff, 0, sizeof(buff));
				sprintf(buff, "%d", NB);
				if(send(skt_fd_clt, buff, 1, 0) < 0){
					perror("send");
					close(skt_fd_clt);
					exit(-1);
				}
				//RECV
				if(recv(skt_fd_clt, &nbDDL, sizeof(nbDDL), 0) < 0){
					perror("recv");
					close(skt_fd_clt);
					exit(-1);
				}
				printf("Le nombre de telechargement effectué est de %zu.\n", nbDDL);
				break;
			case 1:
				printf("Vous avez demandé la liste des fichier present dans le serveur.\n");
				memset(&buff, 0, sizeof(buff));
				sprintf(buff, "%d", LS);
				if(send(skt_fd_clt, buff, 1, 0) < 0){
					perror("send");
					close(skt_fd_clt);
					exit(-1);
				}
				//RECV
				if(recv(skt_fd_clt, &bufferrecv, MAX_SIZE_SEND, 0) < 0){
					perror("recv");
					close(skt_fd_clt);
					exit(-1);
				}
				printf("La liste des fichier sont: %s.\n", bufferrecv);
				break;
			case 2:
				demandeDeTelechargement(skt_fd_clt);
				break;
			case 3:
				printf("Vous avez demandé un upload.\n");
				printf("Entrez le nom du fichier : ");
				// On attend que l'utilisateur ai tapé le nom du fichier avec comme securité de 250 caractere maximum?
				scanf("%250s", nomdufichier);
				// On vide les autres caractere tapés.
				FUNC_CLEAR_
				printf("le fichier a demandé est %s.\n", nomdufichier);
				memset(&buff, 0, sizeof(buff));                /* zero the struct */
				sprintf(buff, "%d%s", DL, nomdufichier);
				if(send(skt_fd_clt, buff, 1, 0) < 0){
					perror("send");
					close(skt_fd_clt);
					exit(-1);
				}
				//RECV
				if(recv(skt_fd_clt, &nbDDL, sizeof(nbDDL), 0) < 0){
					perror("recv");
					close(skt_fd_clt);
					exit(-1);
				}
				break;
			case 4:
				sprintf(buff, "%d", BY);
				if(send(skt_fd_clt, buff, 1, 0) < 0){
					perror("send");
					close(skt_fd_clt);
					exit(-1);
				}
				printf("Connexion fermé.");
				continuer=0;
				break;
			default:
				fprintf(stderr, "Votre choix ne fait pas partie de la liste.\n");
				break;
		};
		buff[0]='\0';
	}



	/*Fermeture de la socket*/
    close(skt_fd_clt);

	return 0;
}

