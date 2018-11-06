#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <sys/wait.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <curl/curl.h>

int nb_port= 0;
uint32_t port_tab[100]= {0};


char html[2500];
char html_raw[] ="<html>\n\
  <head>\n\
    <script type=\"text/javascript\" src=\"https://www.gstatic.com/charts/loader.js\"></script>\n\
    <script type=\"text/javascript\">\n\
      google.charts.load(\"current\", {packages:[\"corechart\"]});\n\
      google.charts.setOnLoadCallback(drawChart);\n\
      function drawChart() {\n\
        var data = google.visualization.arrayToDataTable([\n\
          [\'Type\', \'Montant\'],\n\
          [\'Electricite\',     %.2lf],\n\
          [\'Eau\',      %.2lf],\n\
          [\'Gaz\',  %.2f],\n\
          [\'Internet\', %.2lf],\n\
          [\'Dechets\',    %.2lf]\n\
        ]);\n\
\n\
        var options = {\n\
          title: \'Factures\',\n\
          is3D: true,\n\
        };\n\
\n\
        var chart = new google.visualization.PieChart(document.getElementById(\'piechart_3d\'));\n\
        chart.draw(data, options);\n\
      }\n\
    </script>\n\
  </head>\n\
  <body>\n\
    <div id=\"piechart_3d\" style=\"width: 900px; height: 500px;\"></div>\n\
  </body>\n\
</html>";



char reply[] =
"HTTP/1.1 200 OK\r\n\
Connection: keep-alive\r\n\
Content-Type: text/html\r\n\
Content-Length: %ld\r\n\
\r\n";



double montant_tot_e = 0, montant_tot_w = 0, montant_tot_d = 0, montant_tot_i = 0, montant_tot_g = 0;


int main(int argc, char *argv[])
{
  sqlite3 *db;
  char *err_msg;
  sqlite3_stmt *stmt;

  char req[255];
  int rc, s;

  // initialisation de la graine pour les nbs aléatoires
  srand((unsigned int)time(NULL));

  // ouverture de la base de données
  rc=sqlite3_open("mabdd.db", &db);



/////////////////////////Recuperation de la liste des ports//////////////////////


  // lecture dans la base
  // requête SQL select
  sprintf(req,"SELECT port FROM capteur_actionneur;");
  printf("sql=|%s|\n",req);
  // préparation de la requête
  rc=sqlite3_prepare_v2(db, req, -1, &stmt, 0);
  // parcours des enregistrements
  while(1){
    // lecture de l'enregistrement suivant
    s=sqlite3_step(stmt);
    // enregistrement existant
    if (s==SQLITE_ROW){

      const unsigned char *val=sqlite3_column_text(stmt, 0);
      // affichage de la valeur
      printf("port: %s ",val); //col

      port_tab[nb_port++]= atoi(val);

	  putchar('\n');
   }
   // parcours terminé
   else if (s==SQLITE_DONE)
      break;
  }


///////////////////////////////////////////////////////////////////////////////

  pid_t pid = 0; // for fork

  uint32_t portno= 0;

  //initiation des serveurs pour chaque port de la bdd
  for(int i= 0; i < nb_port; i++)
  {
  	portno= port_tab[i];

  	pid= fork();

  	if(pid == 0)
  		break;
  }

  if(pid != 0)
  	while(1);
  //si c'est un fils alors on est un serveur
  else
  {
		char buffer[256], miniBuffer[10];  //data recue par le client
		char type; // type eau electricite dechet
		float montant;

		int sockfd, client_socket_fd, tmp;
		socklen_t clilen;
		struct sockaddr_in serv_addr, cli_addr;
		int n;

		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd < 0) 
			puts("ERROR opening socket");

		bzero((char *) &serv_addr, sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_addr.s_addr = INADDR_ANY;
		serv_addr.sin_port = htons(portno);
		if (bind(sockfd, (struct sockaddr *) &serv_addr,
		sizeof(serv_addr)) < 0) 
			puts("ERROR on binding");
		listen(sockfd,5);
		clilen = sizeof(cli_addr);
		
		while(1)
		{
			printf("START LISTENNING PORTNO %d\n", portno);
			client_socket_fd = accept (sockfd, (struct sockaddr *)&cli_addr, &clilen);
			if(client_socket_fd < 0)
				puts("ERROR on accept");

			pid = fork();
			if(pid == 0)
			{
				// code execute par le fils
				bzero(buffer,256);
				tmp = read(client_socket_fd,buffer,255);

				printf("message : %s\n", buffer);

				strncpy(miniBuffer, buffer, 3);

				if(strcmp(miniBuffer, "GET") == 0)
				{
					sprintf(req,"SELECT type_fact, montant FROM facture;");
					printf("sql=|%s|\n",req);
					// préparation de la requête
					rc=sqlite3_prepare_v2(db, req, -1, &stmt, 0);

					montant_tot_e = montant_tot_d = montant_tot_w = montant_tot_i = montant_tot_g = 0;
					while(1)
					{
					    // lecture de l'enregistrement suivant
					    s=sqlite3_step(stmt);
					    // enregistrement existant
					    if (s==SQLITE_ROW){
						
					      	const char type_val=sqlite3_column_text(stmt, 0)[0];
					      	const float montant_val=sqlite3_column_double(stmt, 1);
					     	printf("type_val = %c\n", type_val);
					       	printf("montant_val = %f\n", montant_val);

							switch(type_val)
							{
							case 'e':
								montant_tot_e += montant_val;
								break;
							case 'd':
								montant_tot_d += montant_val;
								break;
							case 'w':
								montant_tot_w += montant_val;
								break;
							case 'i':
								montant_tot_i += montant_val;
								break;
							case 'g':
								montant_tot_g += montant_val;
								break;
							default:
								break;
							}

					   	}
					   // parcours terminé
					   	else if (s==SQLITE_DONE)
					      break;

				  	}
					
					printf("montant_tot_e = %.2lf\n", montant_tot_e);
					printf("montant_tot_d = %.2lf\n", montant_tot_d);
					printf("montant_tot_w = %.2lf\n", montant_tot_w);
					printf("montant_tot_i = %.2lf\n", montant_tot_i);
					printf("montant_tot_g = %.2lf\n", montant_tot_g);

					printf("[HTML-R]\n%s\n[HTML-R]\n", html_raw);
				  	sprintf(html, html_raw, montant_tot_e, montant_tot_w, montant_tot_g, montant_tot_i, montant_tot_d);

					printf("[HTML]\n%s\n[HTML]\n", html);

					// ecriture de index.html
				  	FILE *f= fopen("index.html", "w");
				  	fputs(html, f);
				  	fclose(f);

					// lecture de index.html
				  	long fsize;
					FILE *fp = fopen("index.html", "r");
					fseek(fp, 0, SEEK_END);
					fsize = ftell(fp);
					printf("fsize = %ld\n", fsize);
					rewind(fp);
					char *msg = (char*)malloc((fsize+1)*sizeof(char));
					fread(msg, 1, fsize+1, fp);
					fclose(fp);	

					putchar('\n');
					for(int i=0; i<fsize+1; i++)
						putchar(msg[i]);
					putchar('\n');

					sprintf(reply, reply, fsize+1);

					putchar('\n');
					for(int i=0; i<strlen(reply); i++)
						putchar(reply[i]);
					putchar('\n');


					write(client_socket_fd, reply, strlen(reply));

					send(client_socket_fd, msg, fsize+1, 0);

				}
				else if(strlen(buffer) != 0)
				{

					printf("Received packet from %s:%d\nData: %s\n\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port), buffer);
					sscanf(buffer, "%c %f", &type, &montant);
					printf("Writing : \"type: %c, montant: %.2f\"\n", type, montant);

		  			sprintf(req,"INSERT INTO facture(id_logement, type_fact, montant, valeur) VALUES(0, '%c', %.2f, 0);", type, montant);
		  			printf("req = %s\n", req);

		  			rc = sqlite3_exec(db, req, 0, 0, &err_msg);
				}
				close(client_socket_fd);
				puts("socket client closed\n");

				sleep(1);
				exit(0);
			}
			else // pere
			{
				waitpid(-1, NULL, WNOHANG);
			}
  		}
	close(sockfd);
  }
  return 0;
}
