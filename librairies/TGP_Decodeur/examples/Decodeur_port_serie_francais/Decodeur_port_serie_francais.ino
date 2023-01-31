/*
  Librairie TPG DECODEUR - Decodeur_port_serie_francais.ino

  ###DESCRIPTION

  Note: ###NOTE

*/

#include <Decodeur.h> //Pour utiliser la librairie Decodeur

Decodeur monDecodeur(&Serial); //Le décodeur utilisera le texte provenant du port Serie

void setup()
{
  Serial.begin(115200); //Démarrage du port série afin d'envoyer des lignes de texte pour les fins de démonstrations.
}

void loop()
{

  if (monDecodeur.estDisponible()) // Si du texte a été reçu par le décodeur;
  {
    Serial.println("---------------------------------------------------");
    Serial.print("Commande           : ");
    Serial.println(monDecodeur.lireCommande());  //Envoyer sur le port série le nom de la commande décodée.
    
    Serial.print("Nombre d'arguments : ");
    Serial.println(monDecodeur.nombreArgument()); //Envoyer sur le port série le nombre d'arguments décodée.
    
    Serial.print("Argument           : ");
    for (int no = 0; no < monDecodeur.nombreArgument(); no++) //Pour chaque argument
    {
      Serial.print(monDecodeur.lireArgument(no)); //Envoyer sur le port série la valeur de l'argument (on assume qu'il est numérique).
      Serial.print(" , ");
    }
    Serial.println("");
  }
}
