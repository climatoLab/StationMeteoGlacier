/******************************************************************
   Nom: Fichier Test pour la Librairie créer du module RTC
   Créer par: Vincent Lavallée
   Description: Ce programme sert de référence pour de
                futurs utilisation de la librairies créer par les
                stagiaires de recherche sur la station météo.

******************************************************************/
#define DEBUG //-> Definition exclusivement pour le débogage des modules individuels.
#include "RTCLibrary.h" //Librarie développé pour le module RTC

void setup() {

  Serial.begin(115200);
  i2c_init(); //-> Initiation du I2C // fonction seulement disponible en mode DEBUG
  init_RTC(); // Initialisation du capteur
  
  // (AAAA, MM, JJ, hh, mm, ss)
  //setTimeRTC(2022,12,07,13,9,0);// -> Correspond à la date de la dernière modification du temps pour la station rouge.
  //setTimeRTC(2022,12,07,13,14,0);// -> Correspond à la date de la dernière modification du temps pour la station blanche.
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(String(getDateRTC()) + " " + String(getTimeRTC(true))); //Imprime la date et l'heure enregistré par le RTC. 
  delay(1000);
}
