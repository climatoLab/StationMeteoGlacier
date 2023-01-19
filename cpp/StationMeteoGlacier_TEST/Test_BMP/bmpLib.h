/******************************************************************
   Nom: bmpLib.h
   Créer par: Vincent Lavallée, Annabelle Lambert, Raphael Richard
   Description: Librarie créer par les stagiaires de recherche de
                Station météo pour permettre une acquisition simple
                des données du BMP388.

   Type de données :
                    - Température (C) // Non-Calibré
                    - Pression (Pa)   // Non-Calibré
                    - Altitude (m)    // Non-Calibré

   Liens :
          -
          -

******************************************************************/

#ifdef DEBUG //-> Définition pour le DEBUG des modules individuels, NE PAS ACTIVER DANS LE CODE COMPLET DE LA STATION.
#include <Wire.h> //Introduit la librarie I2C
#endif

//--- LIBRARIES UTILISÉES POUR LE BMP388 ---//
#include <Adafruit_Sensor.h> // DE ADAFRUIT //
#include "Adafruit_BMP3XX.h" // DE ADAFRUIT //
//------------------------------------------//
#define SEALEVELPRESSURE_HPA (1013.25) //Définition de la pression au niveau de la mer.

Adafruit_BMP3XX bmp; //Déclaration d'objet.

bool BMP3XX_init() { // -> Fonction à introduire après l'initialisation du I2C. Permet l'initialisation de l'objet / module BMP388
  bool retCode = false; // -> valeur de retour de référence.

  while (!Serial);
  Serial.println("Adafruit BMP388 / BMP390 test");

  if (!bmp.begin_I2C()) {   // hardware I2C mode, can pass in address & alt Wire
    //if (! bmp.begin_SPI(BMP_CS)) {  // hardware SPI mode
    //if (! bmp.begin_SPI(BMP_CS, BMP_SCK, BMP_MISO, BMP_MOSI)) {  // software SPI mode
    Serial.println("Could not find a valid BMP3 sensor, check wiring!");
  } else {
    // Set up oversampling and filter initialization
    bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
    bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
    bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
    bmp.setOutputDataRate(BMP3_ODR_50_HZ);

    retCode = true; // -> true : confirme le fonctionnement du BMP388
  }
  //------------------------------------------------//
  // Prise de mesure initiale pour éviter la prise  //
  // de données erroné.                             //
  //                                                //
  // Remarque : Sans cette prise de donnée initiale //
  //            le capteur ne fait que répéter les  //
  //            mêmes données.                      //
  //------------------------------------------------//
  int temp = bmp.readTemperature();                 //
  int alt = bmp.readAltitude(SEALEVELPRESSURE_HPA); //
  int pressure = bmp.readPressure();                //
  //------------------------------------------------//

  return retCode; // -> retourne la valeur true si le capteur est bien initié, faux dans le cas contraire.
}

float bmpTemp() { // Retourne la température capté
  return bmp.readTemperature();
}

float bmpPression(int echelle = 100) { // Retourne la pression capté
  return bmp.readPressure() / echelle;
}

float bmpAltitude() { // Retourne l'alitude capté
  return bmp.readAltitude(SEALEVELPRESSURE_HPA);
}

String Err_bmp3xx() { // Message d'erreur.
  if (! bmp.performReading()) {
    return "Failed to perform reading :(";
  }
  else {
    return "BMP3XX Connected!";
  }
}

uint8_t BMP3XX_ChipID() { // Informations sur le BMP280;
  return bmp.chipID();
}

#ifdef DEBUG // -> initialisation de l'I2C, EN MODE DEBUG SEULEMENT
void i2c_init_(const int PIN_GPS_EN = 12, const int PIN_POW_EN = 13) {

  pinMode(PIN_GPS_EN, OUTPUT);
  pinMode(PIN_POW_EN, OUTPUT);
  digitalWrite(PIN_GPS_EN, HIGH);
  digitalWrite(PIN_POW_EN, HIGH);

  delay(500);
}

#endif
