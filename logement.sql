-- Destruction des tables créées
DROP TABLE IF EXISTS logement;
DROP TABLE IF EXISTS piece;
DROP TABLE IF EXISTS capteur_actionneur;
DROP TABLE IF EXISTS type_capteur_actionneur;
DROP TABLE IF EXISTS mesure;
DROP TABLE IF EXISTS facture;


-- Creation de la table logement et des champs
CREATE TABLE logement(
   id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
   adresse CHAR(100) NOT NULL,
   num_telephone CHAR(20) NOT NULL,
   ip CHAR(20) NOT NULL,
   date TIMESTAMP DEFAULT CURRENT_TIMESTAMP NOT NULL
);

-- Creation de la table piece et des champs
CREATE TABLE piece(
   id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
   id_logement INTEGER NOT NULL,
   nom CHAR(100) NOT NULL,
   x INTEGER,
   y INTEGER,
   z INTEGER,
   FOREIGN KEY(id_logement) REFERENCES logement(id)
);

-- Creation de la table capteur_actionneur et des champs
CREATE TABLE capteur_actionneur(
   id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
   id_piece INTEGER,
   id_type INTEGER,
   ref_commercial CHAR(100) NOT NULL,
   port INTEGER NOT NULL,
   date TIMESTAMP DEFAULT CURRENT_TIMESTAMP NOT NULL,
   FOREIGN KEY(id_piece) REFERENCES piece(id),
   FOREIGN KEY(id_type) REFERENCES type_capteur_actionneur(id)
);

-- Creation de la table type_capteur_actionneur et des champs
CREATE TABLE type_capteur_actionneur(
   id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
   unite CHAR(20) NOT NULL,
   precision REAL NOT NULL,
   nom CHAR(100) NOT NULL
);

-- Creation de la table mesure et des champs
CREATE TABLE mesure(
   id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
   id_capteur_actionneur INTEGER NOT NULL,
   valeur REAL NOT NULL,
   date TIMESTAMP DEFAULT CURRENT_TIMESTAMP NOT NULL,
   FOREIGN KEY(id_capteur_actionneur) REFERENCES capteur_actionneur(id)
);

-- Creation de la table facture et des champs
CREATE TABLE facture(
   id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
   id_logement INTEGER NOT NULL,
   type_fact CHAR NOT NULL,
   montant REAL,
   valeur REAL,
   date TIMESTAMP DEFAULT CURRENT_TIMESTAMP NOT NULL,
   FOREIGN KEY(id_logement) REFERENCES logement(id)
);


INSERT INTO logement(id, adresse, num_telephone, ip) VALUES(0, "1 place jussieu 75005", "06 12 34 56 78", "134.157.15.22");
INSERT INTO piece VALUES(0, 0, "salon", 0, 0, 1);
INSERT INTO piece VALUES(1, 0, "chambre", 1, 0, 1);
INSERT INTO piece VALUES(2, 0, "sdb", 0, 1, 1);
INSERT INTO piece VALUES(3, 0, "cuisine", 1, 1, 1);

-- Insertion de 4 types différent de capteur dans la table correspondante
INSERT INTO type_capteur_actionneur VALUES(0, "celsius", 0.1, "temperature");
INSERT INTO type_capteur_actionneur VALUES(1, "volt", 0.1, "tension");
INSERT INTO type_capteur_actionneur VALUES(2, "pourcent", 5, "humidité");
INSERT INTO type_capteur_actionneur VALUES(3, "bar", 1, "pression");

-- Insertion d'un capteur de temperature et un capteur d'humidité dans le salon
INSERT INTO capteur_actionneur(id, id_piece, id_type, ref_commercial, port) VALUES(0, 0, 0, "HEZBFLHBDSKU782364", 2012);
INSERT INTO capteur_actionneur(id, id_piece, id_type, ref_commercial, port) VALUES(1, 0, 2, "GEZB7656678U782364", 2013);
INSERT INTO capteur_actionneur(id, id_piece, id_type, ref_commercial, port) VALUES(2, 0, 1, "JIZB7656153U782364", 2014);

-- Insertion de quatre mesures pour les deux capteurs précédemment insérés
INSERT INTO mesure VALUES(0, 0, 21.2, "2018-10-04 12:12:12"); -- température
INSERT INTO mesure VALUES(1, 0, 23.3, "2018-10-04 13:12:13");
INSERT INTO mesure VALUES(2, 1, 30, "2018-10-04 12:12:12"); -- humidité
INSERT INTO mesure VALUES(3, 1, 21, "2018-10-04 13:12:13");

-- Insertion de quatre factures de : électricité, gaz, eau, internet
INSERT INTO facture(id_logement, type_fact, montant, valeur) VALUES(0, "e", 85.6, 121);
INSERT INTO facture(id_logement, type_fact, montant, valeur) VALUES(0, "d", 32.6, 121);
INSERT INTO facture(id_logement, type_fact, montant, valeur) VALUES(0, "g", 98.6, 132);
INSERT INTO facture(id_logement, type_fact, montant, valeur) VALUES(0, "i", 40.7, 222);
