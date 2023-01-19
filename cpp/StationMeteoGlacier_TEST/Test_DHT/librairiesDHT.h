/******************************************************************
 * Nom: librariesDHT.h
 * Créer par: Vincent Lavallée, Annabelle Lambert, Raphael Richard
 * Description: Librarie créer par les stagiaires de recherche de
 *              Station météo pour permettre une acquisition simple
 *              des données du DHT22.
 *              
 * Type de données :
 *                  - Température (C) // Non-Calibré
 *                  - Humidité (%)    // Non-Calibré
 *         
 * Liens : 
 *        - DHT.h : https://github.com/adafruit/DHT-sensor-library
 * 
******************************************************************/
//--- LIBRARIES UTILISÉES POUR LE DHT22 ---//
#include "DHT.h" // DE ADAFRUIT            //
//-----------------------------------------//
#define BROCHE_DHT 25 // Broche utilisée pour le DHT22 sur la Station Météo Glacier

#define DHTTYPE DHT22 //Définition du type de DHT utilisé.

DHT dht(BROCHE_DHT, DHTTYPE); //// Déclaration d'objet.

bool init_Humidite_DHT() { // -> Fonction à introduire après l'initialisation du I2C. Permet l'initialisation de l'objet / module DHT22
  bool retCode = false; // -> valeur de retour de référence.
  
  dht.begin(); // Initialisation du capteur
  
  float x = dht.readHumidity(); // Mesure prise pour la vérification de l'initialisation du capteur
  
  if (isnan(x)) { // Cette boucle affiche l'état du capteur, s'il fonctionne ou non.
    Serial.println("Erreur d'initialisation du capteur"); // 
  } else {
    Serial.println("> Capteur d'humidite (DHT22) initialisé correctement.");
    retCode = true; // -> true : confirme le fonctionnement du DHT22
  }

  return retCode; // -> retourne la valeur true si le capteur est bien initié, faux dans le cas contraire.
}

float dhtHumi() { // Retourne la humidité capté
  return dht.readHumidity();
}


float dhtTemp() { // Retourne la température capté
  return dht.readTemperature();
}
#ifdef DEBUG // -> initialisation de l'I2C, EN MODE DEBUG SEULEMENT
void i2c_init_(const int PIN_POW_EN = 13){
  pinMode(PIN_POW_EN, OUTPUT);
  digitalWrite(PIN_POW_EN, HIGH);
  delay (500);
}
#endif
