// Date and time functions using a DS3231 RTC connected via I2C and Wire lib
#ifdef DEBUG
#include <Wire.h>
#include <SPI.h>
#endif

#include "RTClib.h"

RTC_DS3231 rtc;

char daysOfTheWeek[7][24] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};


bool init_RTC(){
  bool retCode = false;
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    
    delay(10);
  } else {
    Serial.println("RTC : ok");
    retCode = true;
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
  return retCode;
}
DateTime heureAvancee(bool _DEFAULT_){
  int adj;
  DateTime now = rtc.now();
  if (_DEFAULT_ == true){
    adj = 0;
  } else{
    adj = -1;
  }
  DateTime adjustTime(now + TimeSpan(0,adj,0,0));
  return adjustTime;
}

bool setTimeRTC(uint16_t YYYY, uint8_t MM,uint8_t DD, uint8_t hh, uint8_t mm, uint8_t ss){
  rtc.adjust(DateTime(YYYY,MM,DD,hh,mm,ss));
  return true;
}
String getTimeRTC(bool heureAvance = false){
  /*int adj = 0;
  if (heureAvance == true){
    adj = 1;
    rtc.adjust(rtc.now()+TimeSpan(0,+adj,0,0));
  }else{
    rtc.adjust(rtc.now()+TimeSpan(0,+adj,0,0));
  }*/
  char date[23] =  "YYYY/MM/DD hh:mm:ss";
  rtc.now().toString(date);
  return date;
}
bool timeAdjust(){
  bool retCode = false;
   
}
//String MenuRTC(){
//  String txt = 
//  "M ou m -> afficher le Menu" + "\n" +
//  "S ou s -> modifier le temps enregistrer dans le module RTC" + "\n"; 
//  return txt;
//}


#ifdef DEBUG
void i2c_init(const int PIN_POW_EN = 13){

  pinMode(PIN_POW_EN, OUTPUT);
  digitalWrite(PIN_POW_EN, HIGH);
  
  // wait for MAX chip to stabilize
  delay(500);
}
#endif

   
  
