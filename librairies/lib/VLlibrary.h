/******************************************************************
   Nom: vl_Lib.h
   Créer par: Vincent Lavallée, Annabelle Lambert, Raphael Richard
   Description: Librarie créer par les stagiaires de recherche de
                Station météo pour permettre une acquisition simple
                des données du VL53L1X.
   Type de données :
                    - Distance (mm) // Non-Calibré
   Liens :
          -  http://librarymanager/All#SparkFun_VL53L1X
          -
******************************************************************/

#ifdef DEBUG //-> Définition pour le DEBUG des modules individuels, NE PAS ACTIVER DANS LE CODE COMPLET DE LA STATION.
#include <Wire.h> //Introduit la librarie I2C
#endif

//---- LIBRARIE UTILISÉE POUR LE VL53L1X ----//
#include "SparkFun_VL53L1X.h" // DE SPARKFUN //
//-------------------------------------------//

//Optional interrupt and shutdown pins.
#define SHUTDOWN_PIN 2
#define INTERRUPT_PIN 3

SFEVL53L1X distanceSensor; //Déclaration d'objet.
//Uncomment the following line to use the optional shutdown and interrupt pins.
//SFEVL53L1X distanceSensor(Wire, SHUTDOWN_PIN, INTERRUPT_PIN);

bool init_VL(){ // -> Fonction à introduire après l'initialisation du I2C. Permet de confirmer l'initialisation de l'objet / module VL53L1X.
  bool retCode = false; //-> valeur de retour de référence.
  if (distanceSensor.begin() != 0){ //Begin returns 0 on a good init
    Serial.println("Sensor failed to begin. Please check wiring. Freezing...");
  }else{
    retCode = true;//-> Si tout fonctionne
  }
  return retCode; // Retourne la confirmation.
}

int distanceVL(){ //Fonction retournant la distance capté par le capteur ToF
  distanceSensor.startRanging(); //Write configuration bytes to initiate measurement
  while (!distanceSensor.checkForDataReady()){
    delay(1);
  }
  int distance = distanceSensor.getDistance(); //met en variable l'acquisition de la donnée
  distanceSensor.clearInterrupt();
  distanceSensor.stopRanging();
  return distance;
}
#ifdef DEBUG // -> initialisation de l'I2C, EN MODE DEBUG SEULEMENT
void i2c_init_(const int PIN_POW_EN = 13){
  pinMode(PIN_POW_EN, OUTPUT);
  digitalWrite(PIN_POW_EN, HIGH);
  delay (500);
}
#endif