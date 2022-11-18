/******************************************************************
   Nom: Fichier Test pour la Librairie créer du VL53L1X
   Créer par: Vincent Lavallée
   Description: Ce programme sert de référence pour de
                futurs utilisation de la librairies créer par les
                stagiaires de recherche sur la station météo.

******************************************************************/


#define DEBUG //-> Definition exclusivement pour le débogage des modules individuels.

#define TIME_TO_SLEEP 4 //Temps pour le deep sleep (sec)
#define uS_TO_S_FACTOR 1000000  //Convertion de us a sec (microsec x 1M = sec)


#include "vl_Lib.h" //-> Librairie dévelppé pour le VL53L1X.

void setup() {
  Wire.begin();
  Serial.begin(115200);
  i2c_init_(); //-> Initiation du I2C // fonction seulement disponible en mode DEBUG
  init_VL(); // Initialisation du capteur

  Serial.println(String(distanceVL())); //Imprime le retour de la fonction.
  delay(100); //Delai pour permettre l'envoie au port série
}

void loop() {

  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR); // Définition du deep sleep
  esp_deep_sleep_start(); // Entre en deep sleep

  
}
