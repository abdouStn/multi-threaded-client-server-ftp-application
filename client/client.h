#ifndef O_BINARY  
    #define O_BINARY 0 /* declarer la macro O_BINARY (si elle n'existe pas) pour lire/ecrire un fichier en binaire */
#endif

#ifndef ___FTP_CLIENT_H___
#define ___FTP_CLIENT_H___

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



/*FTP*/
#define NB 0 /* Pour avoir le nombre de telechargement */
#define LS 1 /* Pour avoir la liste des fichier */
#define DL 2 /* Pour envoyer le fichier demmandé */
#define UL 3 /* Pour telecharger le fichier */
#define QT 4 /* Pour QuiTter */
#define ET 5 /* Pour envoyer une reponse Est Trouvé */
#define NT 6 /* Pour envoyer une reponse Non Trouvé */
#define ER 7 /* Pour envoyer une reponse ERreur */
#define BY 8 /* Pour envoyer une reponse bye pour fermer la connexion du client coté serveur */

#define MAX_SIZE_NAME 260   /* Il servira a l'utilisation (pour avoir la taille max du nom du fichier). */
#define MAX_SIZE_SEND 45000//65000 /* Il serira a la taille du buffer pour stocker une partie du fichier. */
#define SIZE_PAQUET (MAX_SIZE_SEND+2+1+1) /* Il servira a la taille du paquet a envoyer. */

#define FUNC_CLEAR_ {int c; do { c = getchar(); } while (c != '\n' && c != EOF); }
/**/

typedef struct {
    uint8_t present;
    uint64_t taille;    
} fichier_st;


typedef struct paquet_st{
    unsigned char *paquet;
    size_t taille;
}paquet_st;


paquet_st bouclerecv(size_t skt_fd_clt, const size_t taille);

size_t convertirid(unsigned char c_id1, unsigned char c_id2);

char* recuperercontenu(char buf[], size_t nb);

void creerfichiertampon(size_t id, char str[], size_t taille);

uint8_t reformerfichier(size_t nbfichier, char nom[]);

void demandeDeTelechargement(int skt_fd_clt);

#endif
