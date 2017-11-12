/****************** *********** ****************/
/****************** CLIENT CODE ****************/
/****************** *********** ****************/

#include "client.h"


paquet_st bouclerecv(size_t skt_fd_clt, const size_t taille){
	static char bufferrecv_tmp[SIZE_PAQUET];
	//static uint8_t c=0;
	paquet_st p_st;
	char bufferrecv[SIZE_PAQUET];
	size_t taille_tmp=taille;
	size_t val_recv=0;
	memset(bufferrecv_tmp, 0, SIZE_PAQUET);
	size_t j=0;
	while(taille_tmp>0){
		memset(bufferrecv, 0, SIZE_PAQUET);
		if((val_recv=recv(skt_fd_clt, &bufferrecv, taille_tmp, 0)) == 0){
			perror("recv");
			close(skt_fd_clt);
			exit(-1);
		}
		printf("val recv est de %zu %zu\n", val_recv, taille_tmp);

		for (size_t i = 0; i < val_recv; ++i) 
			bufferrecv_tmp[j++]=bufferrecv[i];
		taille_tmp-=val_recv;
	}
	strcat(bufferrecv_tmp, "\0");
	p_st.paquet=(unsigned char*)bufferrecv_tmp; /* On recupere le paquet. */
	p_st.taille=taille-taille_tmp; /* On recupere la taille. */

	//sprintf(bufferrecv_tmp, "%c", p_st.paquet[0]+48);
	//if((atoi(bufferrecv_tmp)==0) && c==0)
	//	p_st=bouclerecv(skt_fd_clt, taille);
	//c++;
	return p_st;
}

size_t convertirid(unsigned char c_id1, unsigned char c_id2){
	size_t id=0x0000;
	uint8_t i = 1;
	for (; i <= 8; i++){
		if((c_id2 >> (i-1) & 1)) id |= (1 << (i-1));
		else                     id &= ~(1 << (i-1));
	}
	for (; i <= 16; i++){
		if((c_id1 >> (i-1-8) & 1)) id |= (1 << (i-1));
		else                       id &= ~(1 << (i-1));
	}
	return id;
}



char* recuperercontenu(char buf[], size_t nb){
    static char newbuf[MAX_SIZE_SEND];
    memset(newbuf, 0, MAX_SIZE_SEND);
    for(size_t i=nb, j=0; i<SIZE_PAQUET-1; i++, j++)
            newbuf[j]=buf[i];
    return newbuf;
}



void creerfichiertampon(size_t id, char str[], size_t taille){
    FILE* fichier = NULL;
    char name[MAX_SIZE_SEND];
    sprintf(name,"%d.tmp",(int)id);
 	
 	fichier = fopen(name, "w+b"); //ouverture du fichier en lecture et ecriture
    if (fichier == NULL)
    {
        fprintf(stderr, "erreur lors de la creation du fichier %s\n",name );
        perror("creation");
        exit(1);
    }
	for (size_t i = 0; i < taille; ++i){
	    fputc(str[i], fichier);
   	}
    fclose(fichier);
}




uint8_t reformerfichier(size_t nbfichier, char nom[]){
	//Ici on va lire tous les fichier tampon pour l'ecrire dans un autre fichier puis on supprimera le fichier.
	//FILE *fichier=NULL;
	FILE *fd_write=NULL;
	/* On ouvre le fichier final (source) */
	fd_write=fopen(nom, "wb");
	if(fd_write==NULL){
		fprintf(stderr, "Erreur lors de l'ouverture du fichier %s.\n", nom);
		return 0;
	}
	char bufnomfichier[MAX_SIZE_NAME];
	char bufferoffile [MAX_SIZE_SEND];
	size_t cptnbfichier=0; /* servra a savoir quel est le fichier tamporaire a lire */


	while(cptnbfichier<nbfichier){
	    ssize_t res_read=MAX_SIZE_SEND;
	    size_t i=0;
		memset(bufnomfichier, 0, MAX_SIZE_NAME);
		memset(bufferoffile, 0, MAX_SIZE_SEND);
		sprintf(bufnomfichier, "%zu.tmp", cptnbfichier);

		/* On ouvre le fichier a lire */
	    int fd_read = open (bufnomfichier, O_RDONLY | O_BINARY);

	    /* On verifi si le fichier est bien ouvert */
	    if(fd_read ==-1){
	        fprintf(stderr, "In function %s:\n", __FUNCTION__);
	        perror("open");
	        exit(-1);
	    }

        /* On recupere le nombre de caractere que l'on veut*/
        if((res_read=read (fd_read, bufferoffile, MAX_SIZE_SEND/*2*/)) == -1){
            fprintf(stderr, "IN FUNCTION \"%s\" - ", __FUNCTION__);
            perror("read");
            close(fd_read);
        }

        /* On ecrit dans le fichier source */
        while(i<(size_t)res_read)  fputc(bufferoffile[i++], fd_write);

        /* On ferme le fichier tampon */
   		close(fd_read);

        /* On supprime le fichier tampon */
	    if(remove(bufnomfichier)==-1){
	    	perror("remove");
	    	fclose(fd_write);
			return 0;
	    }

	    /* On incremente le numero du fichier tamporaire a lire */
	    cptnbfichier++;
	}
    fclose(fd_write);

	return 1;
}


void demandeDeTelechargement(int skt_fd_clt){

    /* On rentre dans la recuperation de fichier */
	char *tailledufichier_str; 
	char nomdufichier[MAX_SIZE_NAME];
	char buffersend[MAX_SIZE_SEND];
	char buffer_tmp[MAX_SIZE_SEND];
	memset(&buffersend, 0, MAX_SIZE_SEND); /* On met tout a zero */
	memset(&nomdufichier, 0, MAX_SIZE_NAME); /* On met tout a zero */
	uint64_t tailledufichier=0;
	int64_t cpttaillefichier=-1;
	size_t taillebuffer=0;
	size_t nbfichiertampo=0;

	/* Recuperation du nom du fichier a telecharger */
	printf("Vous avez demandé un telechargement.\n");
	printf("Entrez le nom du fichier : ");
	scanf("%250s", nomdufichier);
	FUNC_CLEAR_
	sprintf(buffersend, "%d%s", DL, nomdufichier);/* On cree un paquet qui va contenir la macro pour dire download ainsi que le nom du fichier */

	/* On envoi la macro et le nom du fichier */
	if(send(skt_fd_clt, buffersend, (MAX_SIZE_SEND), 0) < 0){
		perror("send");
		close(skt_fd_clt);
		exit(-1);
	}

	/* On recupere la reponse pour savoir si on peut telecharger le fichier, si oui, on recupere la taille du fichier */
	if(recv(skt_fd_clt, &buffersend, sizeof(buffersend), 0) < 0){
		perror("recv");
		close(skt_fd_clt);
		exit(-1);
	}

	/* On recupere la reponse qui est stocket dans le buffer buffersend */	
	memset(&buffer_tmp, 0, MAX_SIZE_SEND);
	sprintf(buffer_tmp, "%c", buffersend[0]);
	if(atoi(buffer_tmp) != ET){
		fprintf(stderr, "Le fichier demandé est introuvable %d\n", (atoi(buffer_tmp)));
	}
	else{
		/* On recupere la taille du fichier */
		tailledufichier_str = recuperercontenu(buffersend, 1);
		tailledufichier= atoi(tailledufichier_str);
		printf("La taille du fichier est de %llu octet.\n", tailledufichier);
		uint8_t continuer=1;

		/* On boucle sur tout le fichier a recevoir */
		while(cpttaillefichier<(int)tailledufichier && continuer){
			/* On recupere un paquet */
			if((tailledufichier-cpttaillefichier) > SIZE_PAQUET)
				taillebuffer = SIZE_PAQUET;
			else{
				taillebuffer = (tailledufichier-cpttaillefichier)+3/*+89*/;
				cpttaillefichier=tailledufichier;
			}

			/* On recupere un paquet envoyé par le server */
			paquet_st p_st=bouclerecv(skt_fd_clt, taillebuffer);

			/* On va determiner si un morceau du fichier a bien ete copié */
			sprintf(buffersend, "%c", p_st.paquet[0]+48);
			if(atoi(buffersend) != ET){
				fprintf(stderr, "Une erreur s'est produite coté serveur %d %c %d %s\n", atoi(buffersend), p_st.paquet[0], p_st.paquet[0], buffersend);
				for (int i = 0; i < 45004; ++i)
				{
					printf("%c", p_st.paquet[i]);
				}
				continuer=0;
			}
			else{


				/*printf("La valid=%zu  de   %c  -  %c\n", convertirid(p_st.paquet[2]-48, p_st.paquet[1]-48), p_st.paquet[1]-48, p_st.paquet[2]-48);
				*//* On ecrit maintenant dans un nouveau fichier une partie du fichier original */
				creerfichiertampon(convertirid(p_st.paquet[2], p_st.paquet[1]), recuperercontenu((char*)p_st.paquet, 3), p_st.taille-4);

				/* On se rapproche de la taille du fichier original */
				cpttaillefichier+=(p_st.taille-4);
//				printf("la taille est de %llu\n", cpttaillefichier);

			}
			nbfichiertampo++;
		}
		/* Une fois tous les fichier (tampon) recupere et cree */
		/* On reforme le fichier demandé a partir des fichier tampon */
		if(continuer){
			reformerfichier(nbfichiertampo, nomdufichier);
			printf("Telechargement terminé.\n");
		}
		else
			printf("Le telechargement n'a pas pu se faire.\n");
	}

}
