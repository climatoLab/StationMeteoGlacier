#define DEBUG
#include "librairiesDHT.h"

void setup() {
 Serial.begin(115200); // put your setup code here, to run once:
 i2c_init_();
init_Humidite_DHT();
}

void loop() {
  // put your main code here, to run repeatedly:
Serial.println(String(lecture_Humidite_DHT()));
Serial.println(String(lecture_Temperature_DHT()));
delay(1000);
}
