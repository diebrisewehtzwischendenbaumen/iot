#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>


//134.157.104.2
//2012


int main(int argc, char *argv[]){

	struct sockaddr_in server_ad, client_ad;
	struct hostent *ip_ad;

	//Récuperation du numéro de port dans les arguments
	int portnum = atoi(argv[2]);


	//Configuration du serveur
	server_ad.sin_family= AF_INET;
	server_ad.sin_port= htons(portnum);

	//Récuperation de l'adresse ip du serveur
	ip_ad= gethostbyname(argv[1]);
	bcopy((char*)ip_ad->h_addr, (char*)&server_ad.sin_addr.s_addr, ip_ad->h_length);

	//initialisation socket
	int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

	//Connexion au serveur
	if( connect( socket_fd, (struct sockaddr *) &server_ad, sizeof(server_ad) ) <  0)
		puts("ERROR connecting");
	else
		puts("SUCCES connecting");


	//Envoi de la chaine type et montant
	char buffer[256]= {0};
	sprintf(buffer,"%s %s\n", argv[3], argv[4]);
    	write(socket_fd,buffer,strlen(buffer));



    //Fermeture du socket
    close(socket_fd);

    return 0;
}
