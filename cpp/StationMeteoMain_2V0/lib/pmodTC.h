/******************************************************************
   Nom: bmpLib.h
   Créer par: Vincent Lavallée, Annabelle Lambert, Raphael Richard
   Description: Librarie créer par les stagiaires de recherche de
                Station météo pour permettre une acquisition simple
                des données du Thermocouple.

   Type de données :
                    - Température (C) // Non-Calibré

   Liens :
          -
          -

******************************************************************/

#ifdef DEBUG
#include <SPI.h>
#endif

#include "Adafruit_MAX31855.h"

#define MAXCS   14
Adafruit_MAX31855 tc(MAXCS);


float tempInternalTC(){
  return tc.readInternal();
}

float tempTC(){
  return tc.readCelsius();
}

String err_TC(float tempTC){
  if (isnan(tempTC)) {
    return "Temperature is not a valid number, verify thermocouple.";
  } else{
    return "Thermocouple: ok";
  }
}

#ifdef DEBUG
void i2c_init(const int PIN_POW_EN = 13){

  pinMode(PIN_POW_EN, OUTPUT);
  digitalWrite(PIN_POW_EN, HIGH);
  
  // wait for MAX chip to stabilize
  delay(500);
}
#endif
