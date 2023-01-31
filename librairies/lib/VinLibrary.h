
#ifdef DEBUG //-> Définition pour le DEBUG des modules individuels, NE PAS ACTIVER DANS LE CODE COMPLET DE LA STATION.
#include <Wire.h> //Introduit la librarie I2C
#endif

#include "MovingAverageFloat.h"  // https://github.com/pilotak/MovingAverageFloat

#define BROCHE_VIN_EXT 39       //Broche pour permettre de lire la tension d'entrée Vin (5Volts) du ESP32

//================================================================================================================================
//                                              Moyenne mobile
//================================================================================================================================

const uint16_t AVERAGE_BUFFER_SIZE = 5;
MovingAverageFloat <AVERAGE_BUFFER_SIZE> movA_VinExt;

//================================================================================================================================
//                Coefficients du polynômes de degré 4 pour corriger les non linéarités du convertisseur ADC
//================================================================================================================================
//Valeurs obtenues à partir d'un étalonnage
const double coeff_A = -0.000000000000016;
const double coeff_B = 0.000000000118171;
const double coeff_C = -0.000000301211691;
const double coeff_D = 0.001109019271794;
const double coeff_E = 0.034143524634089;
//================================================================================================================================
//                                                   Tension d'entrée Vin
//================================================================================================================================
uint16_t RawData_VinExt = 0;
float Vin_VinExt = 0;
float moyenneVin_VinExt = 0;
const float FACTEUR_CONV_VIN_EXT = 2;
float tensionReelle_VinExt = 0;


//*******************************************************************************************************************
//FONCTIONS POUR LA MESURE DE LA TENSION VIN
//*******************************************************************************************************************
/*************************************************************************************************
   Programme permettant de connaître la tension en entrée Vin (Vin externe de 5Volts). Ici, la broche 39 est
   utilisée pour connaître cette tension.
*************************************************************************************************/

//===========================================================
//Initialise le tableau pour la mesure de la tension d'entrée
//===========================================================
void init_VinExt() {
  //Lecture de la tension d'entrée dans le domaine 0-2.5Volts
  RawData_VinExt = analogRead(BROCHE_VIN_EXT);

  //Applique un polynome de degré 4 pour corriger la "non-linéarité" du ADC
  if (RawData_VinExt < 1 || RawData_VinExt > 4095) {
    Vin_VinExt = 0;
  }
  else {
    Vin_VinExt = coeff_A * pow(RawData_VinExt, 4) + coeff_B * pow(RawData_VinExt, 3) + coeff_C * pow(RawData_VinExt, 2) + coeff_D * RawData_VinExt + coeff_E;
  }

  //Ramène la tension dans le domaine 0-5Volts
  tensionReelle_VinExt = FACTEUR_CONV_VIN_EXT * moyenneVin_VinExt;

  //Rempli le tableau de la moyenen mobile avec la même valeur.
  //La valeur correspond à celle mesurée une seule fois tout juste avant
  //l'appel de la fonction "fill"
  movA_VinExt.fill(tensionReelle_VinExt);
}

//=================================================================
//Fonction qui lit et qui filtre la tension d'entrée Vin externe (5 Volts)
//=================================================================
float lecture_VinExt() {
  //Lecture de la tension d'entrée dans le domaine 0-2.5Volts
  RawData_VinExt = analogRead(BROCHE_VIN_EXT);

  //Applique un polynome de degré 4 pour corriger la "non-linéarité" du ADC
  if (RawData_VinExt < 1 || RawData_VinExt > 4095) {
    Vin_VinExt = 0;
  }
  else {
    Vin_VinExt = coeff_A * pow(RawData_VinExt, 4) + coeff_B * pow(RawData_VinExt, 3) + coeff_C * pow(RawData_VinExt, 2) + coeff_D * RawData_VinExt + coeff_E;
  }
  movA_VinExt.add(Vin_VinExt);
  moyenneVin_VinExt = movA_VinExt.get();

  //Ramène la tension dans le domaine 0-5Volts
  tensionReelle_VinExt = FACTEUR_CONV_VIN_EXT * moyenneVin_VinExt;
  return tensionReelle_VinExt;
}




#ifdef DEBUG
void i2c_init_(const int PIN_POW_EN = 13) {

  pinMode(PIN_POW_EN, OUTPUT);
  digitalWrite(PIN_POW_EN, HIGH);
  delay (500);
}

#endif
