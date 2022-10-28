/******************************************************************
 * Nom: Fichier Test pour la Librairie créer du DHT22
 * Créer par: Vincent Lavallée
 * Description: Ce programme sert de référence pour de
 *              futurs utilisation de la librairies créer par les
 *              stagiaires de recherche sur la station météo.
 * 
******************************************************************/

#define DEBUG //-> Definition exclusivement pour le débogage des modules individuels.

#define TIME_TO_SLEEP 4 //Temps pour le deep sleep (sec)
#define uS_TO_S_FACTOR 1000000  //Convertion de us a sec (microsec x 1M = sec)

#include "librairiesDHT.h"

String DATA; //Varaible qui encapsule toutes les données prises par le BMP388 sous format -String-

void setup() {
  Serial.begin(115200); // put your setup code here, to run once:
  
  i2c_init_(); //-> Initiation du I2C // fonction seulement disponible en mode DEBUG
  init_Humidite_DHT(); // Initialisation du capteur
  
// Encapsulation des données:
// Format : "Nom : Valeur"
  DATA =
  "Température(C): " + String(lecture_Temperature_DHT()) + "\n" +
  "Humidité (%): " + String(lecture_Humidite_DHT()) + "\n";

  Serial.println(DATA); //Envoie sur le port série
  delay(1000); //Delai pour permettre l'envoie au port série
  
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR); // Définition du deep sleep
  esp_deep_sleep_start(); // Entre en deep sleep
}

void loop() {
//  boucle loop() n'à aucune utilitée en mode deep sleep.

}
