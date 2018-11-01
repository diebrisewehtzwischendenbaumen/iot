

#include <sqlite3.h>


/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#include <netdb.h>
#include <arpa/inet.h>
/*
struct sockaddr
{
  short sin_family;
  u_short sin_port;
  struct in_addr sin_addr;
  char sin_zero[8];
};
*/

int nb_port= 0;
uint32_t port_tab[100]= {0};

char html4[] ="<html>\
  <head>\
    <script type=\"text/javascript\" src=\"https://www.gstatic.com/charts/loader.js\"></script>\
    <script type=\"text/javascript\">\
      google.charts.load(\"current\", {packages:[\"corechart\"]});\
      google.charts.setOnLoadCallback(drawChart);\
      function drawChart() {\
        var data = google.visualization.arrayToDataTable([\
          [\'Type\', \'Montant\'],\
          [\"Electricite\",     %.2f],\
          [\'Eau\',      %.2f],\
          [\'Gaz\',  %.2f],\
          [\'Internet\', %.2f],\
          [\'Dechets\',    %.2f]\
        ]);\
\
        var options = {\
          title: \'Factures\',\
          is3D: true,\
        };\
\
        var chart = new google.visualization.PieChart(document.getElementById(\'piechart_3d\'));\
        chart.draw(data, options);\
      }\
    </script>\
  </head>\
  <body>\
    <div id=\"piechart_3d\" style=\"width: 900px; height: 500px;\"></div>\
    <h1>TITRE TEST</h1>\
  </body>\
</html>";



char html3[] ="<html>\
  <head>\
  </head>\
  <body>\
    <div id=\"piechart_3d\" style=\"width: 900px; height: 500px;\">DIV</div>\
    <h1>TITRE TEST</h1>\
  </body>\
</html>";






char html[] = "<html>\n\
  <head>\n\
    <script type=\"text/javascript\" src=\"https://www.gstatic.com/charts/loader.js\"></script>\n\
    <script type=\"text/javascript\">\n\
      google.charts.load(\"current\", {packages:[\"corechart\"]});\n\
      google.charts.setOnLoadCallback(drawChart);\n\
      function drawChart() {\n\
        var data = google.visualization.arrayToDataTable([\n\
          ['Type', 'Montant'],\n\
          ['Electricite',     15],\n\
          ['Eau',      10],\n\
          ['Gaz',  17],\n\
          ['Internet', 5],\n\
          ['Dechets',    4]\n\
        ]);\n\
\n\
        var options = {\n\
          title: 'Factures',\n\
          is3D: true,\n\
        };\n\
\n\
        var chart = new google.visualization.PieChart(document.getElementById('piechart_3d'));\n\
        chart.draw(data, options);\n\
      }\n\
    </script>\n\
  </head>\n\
  <body>\n\
    <div id=\"piechart_3d\" style=\"width: 900px; height: 500px;\"></div>\n\
    <div>TEST</div>\n\
  </body>\n\
</html>";




char html2[] = "<html>\n\
					<body>\n\
						<h1 style=\"width: 900px; height: 500px; color: red;\">\"HELLO %.2f\"</h1>\n\
					</body>\n\
				</html>";


char reply[] =
"HTTP/1.1 200 OK\r\n\
Connection: keep-alive\r\n\
Content-Type: text/html\r\n\
Content-Length: %d\r\n\
\r\n";



double montant_tot_e = 0, montant_tot_w = 0, montant_tot_d = 0, montant_tot_i = 0, montant_tot_g = 0;


int main(int argc, char *argv[])
{
  /*if (argc < 2) {
      printf("ERROR, no port provided\n");
      exit(1);
  }*/

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
		
		/*int client_socket_fd, tmp, socket_fd;
		socklen_t clilen;

		socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) 
      puts("ERROR opening socket\n");

		struct sockaddr_in server_ad, client_ad;


		bzero((char *) &server_ad, sizeof(server_ad));


  	server_ad.sin_family = AF_INET;
  	server_ad.sin_addr.s_addr = INADDR_ANY;
  	server_ad.sin_port = htons(portno);


		if( bind(socket_fd, (struct sockaddr *)&server_ad, sizeof(server_ad)) < 0)
			puts("ERROR on binding");
		listen(socket_fd, 5);

		clilen = sizeof(client_ad);*/


		//int loop= 1;
		//char c;

		while(1)
		{
			printf("START LISTENNING PORTNO %d\n", portno);
			client_socket_fd = accept (sockfd, (struct sockaddr *)&cli_addr, &clilen);
			if(client_socket_fd < 0)
				puts("ERROR on accept");
			puts("un client est entréééé\n");


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



					sprintf(req,"SELECT type, montant FROM facture;");
					printf("sql=|%s|\n",req);
					// préparation de la requête
					rc=sqlite3_prepare_v2(db, req, -1, &stmt, 0);


					montant_tot_e = montant_tot_d = montant_tot_w = montant_tot_i = montant_tot_g = 0;

					while(1){
					    // lecture de l'enregistrement suivant
					    s=sqlite3_step(stmt);
					    // enregistrement existant
					    if (s==SQLITE_ROW){

					      	const char type_val=sqlite3_column_text(stmt, 0);
					      	const float montant_val=sqlite3_column_double(stmt, 1);
					      
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



				  	//sprintf(html, html, montant_tot_e, montant_tot_w, montant_tot_g, montant_tot_i, montant_tot_d);



				  	//FILE *f= fopen("index.html", "w");
				  	//fputs(html2, f);
				  	//fclose(f);

				  	char html5[2000];

				  	FILE *f= fopen("index.html", "r");
				  	fgets(html5, 2000, f);
				  	fclose(f);



				  	sprintf(html2, html2, 0.12);

				  	printf("send reply\n");

				  	sprintf(reply, reply, strlen(html5));
					write(client_socket_fd, reply, strlen(reply));

					printf("send html\n");
					puts(html5);
					//write(client_socket_fd, "coucou", 6);
					write(client_socket_fd, html5, strlen(html5));

					

					//strcpy(buffer,"coucou");
					//write(client_socket_fd, buffer, strlen(buffer));
				}
				else if(strlen(buffer) != 0)
				{

				//for(int m= 0; m < strlen(buffer); m++)
				//	printf("-%c-\n", buffer[m]);


					printf("Received packet from %s:%d\nData: %s\n\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port), buffer);
					sscanf(buffer, "%c %f", &type, &montant);
					printf("Writing : \"type: %c, montant: %.2f\"\n", type, montant);

					// ouverture de fichier
					/*FILE *file = fopen("factures.txt", "a+");
					if (file == NULL)
					{
						printf("ERROR, can't open file\n");
						exit(1);
					}
					fprintf(file, "%s %.2f\n", type, montant);
					fclose(file);*/



		  			sprintf(req,"INSERT INTO facture(id_logement, type, montant, valeur) VALUES(0, '%c', %.2f, 0);", type, montant);
		  			printf("req = %s\n", req);

		  			rc = sqlite3_exec(db, req, 0, 0, &err_msg);


					//write(client_socket_fd, html, strlen(html));
					//strcpy(buffer,"coucou");
					//write(client_socket_fd, buffer, strlen(buffer));

					
				}

				close(client_socket_fd);
				puts("socket client closed\n");

				sleep(1);
				exit(0);
			}
			else // pere
			{
				waitpid(-1, NULL, WNOHANG);
				/*char * line = NULL;
				size_t len = 0;
				ssize_t read;

				char temp_type[1];
				//double temp_e, temp_d, temp_w, temp_i, temp_g;
				double temp_montant;
				montant_tot_e = 0, montant_tot_d = 0, montant_tot_w = 0, montant_tot_i = 0, montant_tot_g = 0;

				FILE *file2 = fopen("factures.txt", "r");
				if (file2 == NULL)
				{
					printf("ERROR, can't open file\n");
					exit(1);
				}
				while ((read = getline(&line, &len, file2)) != -1) {
					sscanf(line, "%s %f", temp_type, temp_montant);
					switch(temp_type[0])
					{
						case 'e':
							montant_tot_e += temp_montant;
							break;
						case 'd':
							montant_tot_d += temp_montant;
							break;
						case 'w':
							montant_tot_w += temp_montant;
							break;
						case 'i':
							montant_tot_i += temp_montant;
							break;
						case 'g':
							montant_tot_g += temp_montant;
							break;
						default:
							break;
					}
				}
				fclose(file2);
			}*/




			//if(c = getchar() == '\n')
			//  loop= 0;

			}


		
  		}

	close(sockfd);

  }

  return 0;
}
