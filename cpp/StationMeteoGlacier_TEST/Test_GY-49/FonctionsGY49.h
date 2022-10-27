#ifdef DEBUG
#include <Wire.h> //I2C
#include <SPI.h> //SPI
#endif

#include <Max44009.h> //Rob

Max44009 light(0x4A);


//----------------------------------------------------Initialisation du module----------------------------------------------------
bool gy_init() {
  bool retCode = false;
  Serial.println("GY49 test...");

  if (!light.isConnected()) {
    Serial.println("Could not find a valid GY49 sensor, check wiring!");
  }
  else {
    retCode = true;
  }
  return retCode;
}


//-------------------------------------------------Mesure et correction au module-------------------------------------------------
float gyLux() {
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


//-------------------------------------------Affichage de l'intensité sur le port série-------------------------------------------
#ifdef DEBUG
void i2c_init(const int PIN_POW_EN = 13) {
  //Active la tension d'alimentation 3V3_SW
  pinMode(PIN_POW_EN, OUTPUT);
  digitalWrite(PIN_POW_EN, HIGH);
  //Délai pour laisser le temps au modules d'être alimenté correctement
  delay(500);
}
#endif
