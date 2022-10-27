#define DEBUG
#include "FonctionsGY49.h"


void setup() {
Serial.begin(115200);
i2c_init();
gy_init();
}

void loop() {
Serial.println("Luminosité: " + String(gyLux()));
delay(1000);
}
