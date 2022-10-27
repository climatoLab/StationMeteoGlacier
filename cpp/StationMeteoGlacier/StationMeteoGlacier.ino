#define setHeure true //true = heure avancé - false = heure reculé
//#define DEBUG

#include "lib/megantic_main.h"

String pathSD = "/donnee.txt";

// Détermine le nom des valeurs dans le tableau
String labelsDonneesSD = String("TimeStamp; Vin; Humidité DHT; Température DHT; Température TC; Pression BMP ; Température BMP; Altitude bmp; \r\n");

unsigned long timeSleep = 10; //->sec
unsigned long micInSec = 1000000;

unsigned long totSleep = timeSleep * micInSec - 500000;

void setup() {
  Serial.begin(115200);
  i2c_init_();
  init_Humidite_DHT();
  BMP3XX_init();
  init_SD(pathSD, labelsDonneesSD);
  init_VinExt();
  init_RTC();
  allData = timeStamp + vir + String(lecture_VinExt()) + vir + String(gyLux()) + vir + String(lecture_Humidite_DHT()) + vir + String(lecture_Temperature_DHT()) + vir + String(tempTC()) + vir + String(bmpPression()) + vir + String(bmpTemp()) + vir + String(bmpAltitude()) + "\n";
  //delay(500);
}

void loop() {

  date = String(heureAvancee(setHeure).year()) + "/" + String(heureAvancee(setHeure).month()) + "/" + String(heureAvancee(setHeure).day());
  temps = String(heureAvancee(setHeure).hour()) + "/" + String(heureAvancee(setHeure).minute()) + "/" + String(heureAvancee(setHeure).second());
  timeStamp = date + "/" + temps;
  allData = timeStamp + vir + String(lecture_VinExt()) + vir + String(gyLux()) + vir + String(lecture_Humidite_DHT()) + vir + String(lecture_Temperature_DHT()) + vir + String(tempTC()) + vir + String(bmpPression()) + vir + String(bmpTemp()) + vir + String(bmpAltitude()) + "\n";
  Serial.print(allData);
  sendData(allData, pathSD);
  debugData();



  //Sleep mode

  //création d'une valeur de delay
  unsigned long delayTime;

  // on initialise un light sleep afin d'économiser de la batterie
  delay(delayTime);
  esp_deep_sleep(totSleep); 
  Serial.print("light_sleep:");


}

void debugData(){
  Serial.println("Date: " + String(heureAvancee(setHeure).year()) + "/" + String(heureAvancee(setHeure).month()) + "/" + String(heureAvancee(setHeure).day()));
  Serial.println("Time: " + String(heureAvancee(setHeure).hour()) + "/" + String(heureAvancee(setHeure).minute()) + "/" + String(heureAvancee(setHeure).second()));
  Serial.println("Vin: " + String(lecture_VinExt()));
  Serial.println("LUX: " + String(gyLux()));
  Serial.println("DHT Hum: " + String(lecture_Humidite_DHT()));
  Serial.println("DHT Temp: " + String(lecture_Temperature_DHT()));
  Serial.println("TC Temp: " + String(tempTC()));
  Serial.println("BMP Pres: " + String(bmpPression()));
  Serial.println("BMP Temp: " + String(bmpTemp()));
  Serial.println("BMP Alti: " + String(bmpAltitude()));
}
