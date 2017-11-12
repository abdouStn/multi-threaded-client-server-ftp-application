

#include "server.h"


int main(int argc, char** argv){
    int port=0;
    if(argc< 2){
        fprintf(stderr, "Error: pas assez de parametre\n");
        exit(1);
    }
    else
        port=atoi(argv[1]);

    /*Socket*/
    int skt_fd_srv=socket(PF_INET, SOCK_STREAM, 0);
    if(skt_fd_srv==-1){
        perror("skt_fd_srv");
        exit(-1);
    }
    fprintf(stdout, "socket.......OK\n");

    /*sockaddr_in*/
    struct sockaddr_in ad;
    ad.sin_family= AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    ad.sin_port= htons(port);
    ad.sin_addr.s_addr=htonl(INADDR_ANY); //inet_addr("127.0.0.1");

    /*Bind*/
    if(bind(skt_fd_srv, (struct sockaddr*)& ad, sizeof(ad)) < 0){
        perror("bind");
        close(skt_fd_srv);
        exit(-1);
    }
    fprintf(stdout, "bind........OK\n");

    /*Listen*/
    if(listen(skt_fd_srv, NB_MAX_CLIENT) < 0){
        perror("listen");
        close(skt_fd_srv);
        exit(-1);
    }
    fprintf(stdout, "listen.......OK\n");


    /*Accept*/
    struct sockaddr_in brCv;
    socklen_t lgbrCv = sizeof(brCv);
    
    param_t param[NB_TOTAL_CLIENT_GERER];
    pthread_t thread_id[NB_TOTAL_CLIENT_GERER];

    int skt_fd_clt, nb_client_acceptes = 0 ;
    size_t nombredefichiertelecharge=0;
	
    while( (skt_fd_clt = accept(skt_fd_srv, (struct sockaddr*)&brCv, &lgbrCv)) )
    {
        if (nb_client_acceptes < NB_TOTAL_CLIENT_GERER)
        {
            fprintf(stdout, "accept.......OK\n");
            param[nb_client_acceptes].numero = nb_client_acceptes+1;
            param[nb_client_acceptes].skt_fd_clt = &skt_fd_clt;
            param[nb_client_acceptes].nombredefichiertelecharge = &nombredefichiertelecharge;
            
            if( pthread_create(thread_id+nb_client_acceptes, NULL ,delegation , param+nb_client_acceptes) < 0)
            {
                perror("could not create thread");
                return 1;
            }
         
            //Now join the thread , so that we dont terminate before the thread
            //pthread_join( thread_id , NULL);
            puts("Delegation a un thread");
            nb_client_acceptes++;
        }

    } 
    if (skt_fd_clt < 0)
    {
        perror("accept failed");
        //return 1;
    }
    
    for (int i = 0; i <= nb_client_acceptes; ++i)
    {
        pthread_join(thread_id[i],NULL);
    }
    puts("fermeture du serveur");
    close(skt_fd_srv); 
    return 0;
}
