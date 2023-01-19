#define DEBUG
#include "pmodTC.h"



void setup() {
  Serial.begin(115200);
  i2c_init();

}

void loop() {
  Serial.println(String(tempTC()));
  delay(1000);
}
