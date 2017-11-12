#include "server.h"


char* file_name(char buf[], size_t nb){
    static char newbuf[MAX_SIZE_SEND];
    memset(newbuf, 0, MAX_SIZE_SEND);
    for(size_t i=nb, j=0; i<SIZE_PAQUET-1; i++, j++)

            newbuf[j]=buf[i];

    return newbuf;
}


/**
    \fun void creerpaquet(uint8_t macro_fonctionement, size_t id, char str[]);
    \param id le numero du paquet
    \brief Cette fonction va nous permetre de creer le type de paquet a envoyer au client.
           Le paque se composera de :
                - 1 octet  : pour savoir si la copie du fichier a bien marché.
                - 2 octets : pour avoir le numero du paaquet.
                - MAX_SIZE_SEND octets : Pour avoir le contenu du text
*/
unsigned char *creerpaquet(uint8_t macro_fonctionement, size_t id, char str[]){
    static char buff[SIZE_PAQUET]; //On a donc la taille du paquet+la taille de l'id+la taille de la macro+ 1 octet pour le caractere '\O'
    unsigned long long int n=0;
    unsigned char c_id1 = 0x0000;
    unsigned char c_id2 = 0x0000;

    /* On cree l'identifiant du paquet avec 2 caractere */
    for(n=0;n<=7;n++){
        if(id%2) c_id1 |= (1 << n);
        else    c_id1 &= ~(1 << n);
        id/=2;
    }
    for(n=0;n<=7;n++){
        if(id%2) c_id2 |= (1 << n);
        else    c_id2 &= ~(1 << n);
        id/=2;
    }

    /* On initegre la macro et l'identifiant */
    memset(buff, 0, SIZE_PAQUET);   
    sprintf(buff, "%c%c%c", macro_fonctionement, c_id1, c_id2); /*on integre la demande*/

    /* On integre la chaine de caractere */
    for (size_t i = 3; i < MAX_SIZE_SEND+4; ++i)
        buff[i]=str[i-3];
    strcat(buff, "\0");

    /*on retourne le paquet*/
   return (unsigned char*)buff;
}



char * lister(const char *name)
{
    static char buf[MAX_SIZE_SEND];
    memset(buf, 0, MAX_SIZE_SEND);
    DIR* repertoire = opendir(name); // repertoire à lister
    if (repertoire==NULL)
    {
        fprintf(stderr, "Impossible d'ouvrir ce repertoire %s \n", name );
        exit(0);
    }
    else
    {
        struct dirent *entree;
        //char chemin[512];
        //struct stat etat;
        while((entree=readdir(repertoire)) != NULL)
        {
            
            if (strcmp(entree->d_name,".") && strcmp(entree->d_name,".."))
            {
                //printf("%s\n",entree->d_name);
                strcat(strcat(buf,entree->d_name),"\n");
            }
        }
        strcat(buf,"\0");
        closedir(repertoire);
    }
    return buf;
}



void readfile(int skt_fd_clt, char name[]){
    int fd_read = open (name, O_RDONLY | O_BINARY);

    if(fd_read ==-1){
        fprintf(stderr, "In function %s:\n", __FUNCTION__);
        perror("open");
        exit(-1);
    }

    /* On cree un le buffer pour la lecture */
    char bufferoffile[MAX_SIZE_SEND];

    /*On cree une variable pour connaitre le resultat de la lecture, 
      puis on boucle tant qu'on peut lire le fichier */
    ssize_t res_read=MAX_SIZE_SEND;
    size_t i=0;
    uint8_t continuer=1;
    while(continuer)
    {
        memset(bufferoffile, 0, MAX_SIZE_SEND);
        /* On recupere le nombre de caractere que l'on veut*/
        if((res_read=read (fd_read, bufferoffile, MAX_SIZE_SEND/*2*/)) == -1){
            fprintf(stderr, "IN FUNCTION \"%s\" - ", __FUNCTION__);
            perror("read");
            close(fd_read);
            continuer=0;
        }
        /* Si on est a la fin du fichier on s'arrete*/
        else if(res_read == 0) break;

        /*Si on est pas a la fin du fichier alors on cree le paquet a envoyer*/
        uint8_t macro_fonctionement=0;
        if(!continuer) macro_fonctionement = ER;
        else           macro_fonctionement = ET;
        ssize_t nbcara=0;

        /* On cree un le paquet avec les info dont on a besoin */
        unsigned char *paquet = creerpaquet(macro_fonctionement, i, bufferoffile);
  
        /* Ici on envoira le contenu du paquet au client via la fonction send */
        if((nbcara=send(skt_fd_clt, (char*)paquet, res_read+1+2+1, 0)) < 0){
            perror("send");
            close(skt_fd_clt);
            close(fd_read);
            exit(-1);
        }

        printf("le nombre de cara envoyé est de : %lu.\n", nbcara);
        /* On incremente l'id du paquet */
        i++;
    }

    close(fd_read);

    return ;
}




fichier_st est_present_fichier(const char *name)
{
    struct stat etat;
    fichier_st infos;

    if ((lstat(name, &etat)< 0))
    {
        //fprintf(stderr, "Fichier introuvable %s\n", name);
        infos.present = NT;
        infos.taille = 0;
    }
    else if (S_ISREG(etat.st_mode)) //si fichier regulier
    {
        infos.present = ET;
        infos.taille = (uint64_t)etat.st_size;
        //printf("taille : %d\n", (int)etat.st_size);
    }
    return infos;
}




uint64_t getTailleFile(char *name){
    struct stat etat;
    if(stat(name,&etat) >= 0) return (uint64_t)etat.st_size;
    return 0;
}





 
//the thread function
void *delegation(void *s){
    param_t *p = (param_t*)s;
         
    char buf[MAX_SIZE_SEND];
    while(1)
    {
        printf("Attente d'une action, thread %zu : \n", p->numero);
        /*Recuperation du message*/
        
        memset(buf, 0, MAX_SIZE_SEND);
        strcat(buf, "\0");
        while(strcmp(buf, "\0") == 0)
        if(recv(*(p->skt_fd_clt), &buf, sizeof(buf), 0) < 0){
            perror("recv");
            //close(skt_fd_srv);
            close(*(p->skt_fd_clt));
            exit(-1);
        }

        /*--------Affichage du message*/
        //printf("reception du serveur: %s \n", buf);

        /* preparation de notre switch */
        char* bufdemande=(char*)malloc(sizeof(char)); /*on alloue la place qu'il faut pour recuperer la demande*/
        sprintf(bufdemande, "%c", buf[0]); /*on recupere la demande*/
        int demande=atoi(bufdemande); /* On converti la demamande en entier*/
        int fermer =0; /* variable qui va nous servir a fermer la socket du client*/
        char bufferrecv[MAX_SIZE_SEND];
        char buffersend[MAX_SIZE_SEND];
        //uint64_t tailledufichier=0;
        memset(bufferrecv, 0, MAX_SIZE_SEND);


        /*Switch sur plusieurs demande possible*/
        switch(demande){
            case 0:
                printf("Le nombre de fichier telechargé est de %zu\n", *(p->nombredefichiertelecharge));
                //SEND
                size_t nbDDL= *p->nombredefichiertelecharge;
                if(send(*(p->skt_fd_clt), &nbDDL, sizeof(size_t), 0) < 0){
                    perror("send");
                    close(*(p->skt_fd_clt));
                    exit(-1);
                }
                break;
            case 1:
                printf("Demande un ls\n");
                //memset(bufferrecv, 0, MAX_SIZE_SEND);
                sprintf(bufferrecv, "%s", lister(".")); /*on recupere la demande*/
                //SEND
                if(send(*(p->skt_fd_clt), &bufferrecv, MAX_SIZE_SEND, 0) < 0){
                    perror("send");
                    close(*(p->skt_fd_clt));
                    exit(-1);
                }
                break;
            case 2:
                /***********************************************/
                       /********* Telechargement *********/
                /***********************************************/
                printf("Demande un Download\n");
                char *filename = file_name(buf, 1);
                fichier_st f_st;

                //s'il est present, envoyer un paquet disant que le fichier est present ainsi que sa taille.
                f_st = est_present_fichier(filename);
                f_st.taille=getTailleFile(filename);
                if(f_st.present == NT)
                    sprintf(buffersend, "%d%llu", NT, f_st.taille); /*on integre la demande*/
                else 
                    sprintf(buffersend, "%d%llu", ET, f_st.taille); /*on integre la demande*/
               
                printf("La taille est de %llu octets\n", f_st.taille);

                //printf("tu vas envoyer: %s.\n", buffersend);
                if(send(*(p->skt_fd_clt), buffersend, MAX_SIZE_SEND, 0) < 0){
                    perror("send");
                    close(*(p->skt_fd_clt));
                    exit(-1);
                }
                if(f_st.present == ET){
                    /* on envoi le fichier ici */
                    readfile(*(p->skt_fd_clt), filename);
                    printf("fichier envoyé avec  succès\n");
                    *(p->nombredefichiertelecharge) = *(p->nombredefichiertelecharge)+1;
                }
                break;
                /***********************************************/
                       /********* -------------- *********/
                /***********************************************/
            case 3:
                printf("Demande un Upload\n");
                break;
            case 4:
                printf("Au revoir\n");
                close(*(p->skt_fd_clt));
                fermer=1;
                break;
            default:
                demande=ER;/* On va dire au client que la demande est incorrect*/
                if(send(*(p->skt_fd_clt), &demande, sizeof(demande), 0) < 0){
                    perror("send");
                    close(*(p->skt_fd_clt));
                    exit(-1);
                }
        };

        /*On ferme le prog a la demande du client*/
        if(fermer)
            break;

        memset(buf, 0, MAX_SIZE_SEND);
        free(bufdemande);
    }
    return 0;
} 
