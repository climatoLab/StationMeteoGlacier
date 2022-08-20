#include <Decodeur.h>
#include <Wire.h>
Decodeur monDecodeur(&Serial);


char ACK = '*';
char NAK = '?';

const int PIN_GPS_EN = 12;
const int PIN_POW_EN = 13;

const int PIN_I2C_SDA = 21;
const int PIN_I2C_SCL = 22;

void setup() {
  Serial.begin(115200);
  pinMode(PIN_GPS_EN, OUTPUT);
  pinMode(PIN_POW_EN, OUTPUT);
  digitalWrite(PIN_GPS_EN, LOW);
  digitalWrite(PIN_POW_EN, LOW);
  menu();

  Wire.begin();
  
//  pinMode(PIN_I2C_SDA, INPUT);
//  pinMode(PIN_I2C_SCL, INPUT);
//  pinMode(PIN_I2C_SDA, OUTPUT);
//  pinMode(PIN_I2C_SCL, OUTPUT);
//  digitalWrite(PIN_I2C_SDA, HIGH);
//  digitalWrite(PIN_I2C_SCL, HIGH);
}

void loop() {
  if (monDecodeur.available()) {
    char commande = monDecodeur.getCommand();
    int argument = 0;
    int nombreArg = 0;
    switch (commande) {
      //Commande pour activer (1)ou désactiver (0) la tension 3.3V du GPS
      case 'g' :
        nombreArg = monDecodeur.getArgCount();
        if (nombreArg >= 1) {
          argument = monDecodeur.getArg(0);
          if (argument == 0) {
            digitalWrite(PIN_GPS_EN, LOW);
          }
          else {
            digitalWrite(PIN_GPS_EN, HIGH);
          }
          Serial.println(ACK);
        }
        else {
          Serial.println(NAK);
        }
        break;

      //Commande pour activer (1) ou désactiver (0) la tension 3.3V des modules
      case 'p' :
        nombreArg = monDecodeur.getArgCount();
        if (nombreArg >= 1) {
          argument = monDecodeur.getArg(0);
          if (argument == 0) {
            digitalWrite(PIN_POW_EN, LOW);
          }
          else {
            digitalWrite(PIN_POW_EN, HIGH);
          }
          Serial.println(ACK);
        }
        else {
          Serial.println(NAK);
        }
        break;

      //Commande pour afficher le menu
      case 'm' :
        menu();
      break;

      default :
        Serial.println(NAK);
    }
    delay(10);
  }
}

void menu(void){
  Serial.println("***************************************************");
  Serial.println("g 0 Pour désactiver la tension 3.3V du GPS.");
  Serial.println("g 1 Pour activer la tension 3.3V du GPS.");
  Serial.println("p 0 Pour désactiver la tension 3.3V des modules en général.");
  Serial.println("p 1 Pour activer la tension 3.3V des modules en général.");
  Serial.println("m   Pour afficher ce menu.");
  Serial.println("***************************************************");
}
