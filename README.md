# iot

1. créer la base de données avec la commande "sqlite3 mabdd.db".
2. lire le fichier logement.sql avec la commande ".read logement.sql".
3. compiler le server avec "gcc server.c -o server -lsqlite3".
4. compiler le client avec "gcc client.c -o client".
5. executer le serveur avec "./server".
6. executer un client avec "./client ip_address port type montant", ajoutant les champs dans la base de données.
   --> port ex: 2012, 2013, 2014.
   --> type ex: e, d, w, i, g.
   --> attention, le pie chart ne supporte pas des valeurs accumulées de montant pour un type supérieures à 100.
7. ecouter avec un navigateur web sur un port actif, pour afficher le pie chart des montants accumulés des factures pour chaque type dans la base de données.
