

#include "DHT.h"




#define BROCHE_DHT 25           //Broche pour le capteur DHT22 (température + humidité)

//================================================================================================================================
//                                                Capteur d'humidité
//================================================================================================================================
// https://github.com/adafruit/DHT-sensor-library
#define DHTTYPE DHT22
DHT dht(BROCHE_DHT, DHTTYPE);
float humidite = 0;


//================================================
//Initialise la communication avec le module DHT22
//================================================
bool init_Humidite_DHT() {
  bool retCode = false;
  dht.begin();
  float x = dht.readHumidity();
  if (isnan(x)) {
    Serial.println("don't work");
  } else {
    Serial.println("> Capteur d'humidite (DHT22) initialisé correctement.");
    retCode = true;
  }

  return retCode;
}

//=================================================
//Mesure et applique une correction au module DHT22
//=================================================
float lecture_Humidite_DHT() {
  return dht.readHumidity();
}


float lecture_Temperature_DHT() {
  return dht.readTemperature();
}
#ifdef DEBUG

void i2c_init_(const int PIN_POW_EN = 13){
  pinMode(PIN_POW_EN, OUTPUT);
  digitalWrite(PIN_POW_EN, HIGH);
  delay (500);
}
#endif
