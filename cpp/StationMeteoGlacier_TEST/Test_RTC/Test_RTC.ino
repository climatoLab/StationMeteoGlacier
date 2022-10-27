#define DEBUG
#include "RTCLibrary.h"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
i2c_init();
init_RTC();
//setTimeRTC(2022,10,27,17,54,0); -> Correspond à la date de la dernière modification du temps pour la station rouge.
}

void loop() {
  // put your main code here, to run repeatedly:
Serial.println(String(getTimeRTC(true)));
delay(1000);
}
