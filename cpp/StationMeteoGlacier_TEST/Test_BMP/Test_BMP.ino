/******************************************************************
 * Nom: Fichier Test pour la Librairie créer du BMP388
 * Créer par: Vincent Lavallée
 * Description: Ce programme assure et sert de référence pour de
 *              futurs utilisation de la librairies créer par les
 *              stagiaires de recherche sur la station météo.
 * 
******************************************************************/

#define DEBUG //-> Definition exclusivement pour le débogage des modules individuels.

#define TIME_TO_SLEEP 4 //Temps pour le deep sleep (sec)
#define uS_TO_S_FACTOR 1000000  //Convertion de us a sec (microsec x 1M = sec)

#include "bmpLib.h" //Librarie développer pour le BMP388
String DATA; //Encapsule toutes les données prises par le BMP388 sous format -String-

void setup() {
  Serial.begin(115200);
  i2c_init_(); //-> Initiation du I2C // fonction seulement disponible en mode DEBUG
  BMP3XX_init(); // Initialisation du capteur

// Encapsulation des données:
// Format : "Nom : Valeur"
  DATA =
  "Température(C): " + String(bmpTemp()) + "\n" +
  "Pression(hPa): " + String(bmpPression()) + "\n" +
  "Altitude(m): " + String(bmpAltitude()) + "\n";

  Serial.println(DATA); //Envoie sur le port série
  
  delay(5000); //Delai de 5 sec.

  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR); // Définition du deep sleep
  esp_deep_sleep_start(); // Entre en deep sleep
  
}

void loop() {
//  boucle loop() n'à aucune utilitée en mode deep sleep.
}
