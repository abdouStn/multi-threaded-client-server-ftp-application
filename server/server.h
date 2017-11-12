#ifndef O_BINARY  
    #define O_BINARY 0 /* declarer la macro O_BINARY (si elle n'existe pas) pour lire/ecrire un fichier en binaire */
#endif
#ifndef ___SERVER_H___
#define ___SERVER_H___

#include <stdio.h>
#include <string.h>    
#include <stdlib.h>    
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <unistd.h>    
#include <pthread.h> 
#include <netinet/in.h>
#include <fcntl.h>   /* open(), creat() - and fcntl() */
#include <math.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>


/*thread*/
#define NB_MAX_CLIENT 5 /* Pour avoir le nombre max de client que le serveur peut accepter avec le listen */
#define NB_TOTAL_CLIENT_GERER 10
/**/


/*FTP*/
#define NB 0 /* Pour avoir le nombre de telechargement */
#define LS 1 /* Pour avoir la liste des fichier */
#define DL 2 /* Pour envoyer le fichier demmandé */
#define UL 3 /* Pour telecharger le fichier */
#define QT 4 /* Pour QuiTter */
#define ET 5 /* Pour envoyer une reponse Est Trouvé */
#define NT 6 /* Pour envoyer une reponse Non Trouvé */
#define ER 7 /* Pour envoyer une reponse ERreur */

//#define MAX_SIZE_NAME 260   /* Il servira a l'utilisation (pour avoir la taille max du nom du fichier). */
#define MAX_SIZE_SEND 45000//65000 /* Il serira a la taille du buffer pour stocker une partie du fichier. */
#define SIZE_PAQUET (MAX_SIZE_SEND+2+1+1) /* Il servira a la taille du paquet a envoyer. */

#ifndef O_BINARY  
    #define O_BINARY 0 /* declarer la macro O_BINARY (si elle n'existe pas) pour lire/ecrire un fichier en binaire */
#endif
/**/


typedef struct 
 {
     int *skt_fd_clt;
     size_t numero;
     size_t *nombredefichiertelecharge;
 }param_t ; 


typedef struct 
{
    uint8_t present;
    uint64_t taille;    
} fichier_st;



char* file_name(char buf[], size_t nb);

/**
    \fun void creerpaquet(uint8_t macro_fonctionement, size_t id, char str[]);
    \param id le numero du paquet
    \brief Cette fonction va nous permetre de creer le type de paquet a envoyer au client.
           Le paque se composera de :
                - 1 octet  : pour savoir si la copie du fichier a bien marché.
                - 2 octets : pour avoir le numero du paaquet.
                - MAX_SIZE_SEND octets : Pour avoir le contenu du text
*/
unsigned char *creerpaquet(uint8_t macro_fonctionement, size_t id, char str[]);

char * lister(const char *name);

void readfile(int skt_fd_clt, char name[]);

fichier_st est_present_fichier(const char *name);

uint64_t getTailleFile(char *name);
 
void *delegation(void *s);


#endif
