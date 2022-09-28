#ifdef DEBUG
#include <SPI.h>
#endif

#include "Adafruit_MAX31855.h"

#define MAXCS   14
Adafruit_MAX31855 thermocouple(MAXCS);


float tempInternalTC(){
  return thermocouple.readInternal();
}

float tempTC(){
  return thermocouple.readCelsius();
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
