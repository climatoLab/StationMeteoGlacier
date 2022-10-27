#ifdef DEBUG
#include <Wire.h>
#include <SPI.h>
#endif


#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BMP3XX bmp;

bool BMP3XX_init(){
  bool retCode = false;
  
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
  
    retCode = true;
  }
  
  return retCode;
}

float bmpTemp(){
  return bmp.readTemperature();
}

float bmpPression(int echelle = 100){
  return bmp.readPressure()/echelle;
}

float bmpAltitude(){
  return bmp.readAltitude(SEALEVELPRESSURE_HPA);
}

String Err_bmp3xx(){
  if (! bmp.performReading()) {
    return "Failed to perform reading :(";
  }
  else{
    return "BMP3XX Connected!";
  }
}

#ifdef DEBUG
void i2c_init_(const int PIN_GPS_EN = 12,const int PIN_POW_EN = 13){

  pinMode(PIN_GPS_EN, OUTPUT);
  pinMode(PIN_POW_EN, OUTPUT);
  digitalWrite(PIN_GPS_EN, HIGH);
  digitalWrite(PIN_POW_EN, HIGH);

  delay(500);
}
#endif
