/******************************************************************
 * Nom: FonctionsGY49.h
 * Créer par: Vincent Lavallée, Annabelle Lambert, Raphael Richard
 * Description: Librarie créer par les stagiaires de recherche de
 *              Station météo pour permettre une acquisition simple
 *              des données du GY-49.
 *              
 * Type de données :
 *                  - Luminosité (Lux) // Non-Calibré
 *         
 * Liens : 
 *        - https://github.com/RobTillaart/Max44009
 *        -
 * 
******************************************************************/

#ifdef DEBUG //-> Définition pour le DEBUG des modules individuels, NE PAS ACTIVER DANS LE CODE COMPLET DE LA STATION.
#include <Wire.h> //Introduit la librarie I2C
#endif

//--- LIBRARIES UTILISÉES POUR LE GY-49 ---//
#include <Max44009.h> // DE ROB TILLAART   //
//-----------------------------------------//

Max44009 light(0x4A);  //Déclaration d'objet + sont adresse I2C.


bool gy_init() { // -> Fonction à introduire après l'initialisation du I2C. Permet l'initialisation de l'objet / module GY-49
  bool retCode = false; // -> valeur de retour de référence.
  Serial.println("GY49 test...");

  if (!light.isConnected()) { // Détermine si le cateur est bien connecté
    Serial.println("Could not find a valid GY49 sensor, check wiring!");
  }
  else {
    retCode = true; // -> true : confirme le fonctionnement du GY-49
  }
  return retCode; // -> retourne la valeur true si le capteur est bien initié, faux dans le cas contraire.
}

float gyLux() { // Retourne la Lumininosité capté
  return light.getLux();
}

/*String err_GY49() {
  int err = light.getError();
  String msgErr;
  if (err != 1) {
    msgErr = "Error gy";
  }
  else {
    msgErr = "GY49: ok";
  }
  return msgErr;
}*/

#ifdef DEBUG // -> initialisation de l'I2C, EN MODE DEBUG SEULEMENT
void i2c_init(const int PIN_POW_EN = 13) {
  //Active la tension d'alimentation 3V3_SW
  pinMode(PIN_POW_EN, OUTPUT);
  digitalWrite(PIN_POW_EN, HIGH);
  //Délai pour laisser le temps au modules d'être alimenté correctement
  delay(500);
}
#endif
