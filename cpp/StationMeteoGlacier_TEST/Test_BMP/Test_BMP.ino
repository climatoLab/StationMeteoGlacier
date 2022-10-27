#define DEBUG
#define TIME_TO_SLEEP 4
#define uS_TO_S_FACTOR 1000000  

#include "bmpLib.h"

int x = 0;
String DATA;

void setup() {
  Serial.begin(115200);
  i2c_init_();
  BMP3XX_init();  
    
  DATA =
  "Température(C): " + String(bmpTemp()) + "\n" +
  "Pression(hPa): " + String(bmpPression()) + "\n" +
  "Altitude(m): " + String(bmpAltitude()) + "\n";

  Serial.println(DATA);
  
  delay(5000);

  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  esp_deep_sleep_start();
  
}

void loop() {

}
