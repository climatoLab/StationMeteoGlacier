/******************************************************************
   Nom: RTCLibrary.h
   Créer par: Vincent Lavallée, Annabelle Lambert, Raphael Richard
   Description: Librarie créer par les stagiaires de recherche de
                Station météo pour permettre une acquisition simple
                des données du DS3231.

   Type de données :
                    - Date
                    - Heure

   Liens :
          - https://github.com/adafruit/RTClib
          -

******************************************************************/
#ifdef DEBUG //-> Définition pour le DEBUG des modules individuels, NE PAS ACTIVER DANS LE CODE COMPLET DE LA STATION.
#include <Wire.h> //Introduit la librarie I2C
#include <SPI.h>  //Introduit la libraire pour le SPI
#endif

//--- LIBRAIRIE UTILISÉE POUR LE RTC ---//
#include "RTClib.h" // DE ADAFRUIT      //
//--------------------------------------//

RTC_DS3231 rtc; //Déclaration d'objet.

// Varaiables qui englobe les valeurs String (nom de chaque jour de la semaine).
//--> Valeurs allant de 0 à 6.
//---> ex. daysOfTheWeek(5) = "Vendredi";
char daysOfTheWeek[7][24] = {"Dimanche", "Lundi", "Mardi", "Mercredi", "Jeudi", "Vendredi", "Samedi"};

bool init_RTC() { // -> Fonction à introduire après l'initialisation du I2C. Permet l'initialisation de l'objet / module RS3231
  bool retCode = false; // -> valeur de retour de référence.

  if (! rtc.begin()) { //Démare l'initialisation et envoie une message d'erreur si la connection n'est pas réussis.
    Serial.println("Couldn't find RTC");
    Serial.flush();

    delay(10);
  } else { //Connecté? envoi un message de confirmation de connection.
    Serial.println("RTC : Initialisation complété.");
    retCode = true; // -> true : confirme le fonctionnement du DS3231
  }

  if (rtc.lostPower()) { //En cas que la batterie du RTC sois déchargé et quel à besoin d'enregistrer un nouveau temps.
    Serial.println("RTC lost power, let's set the time!");

    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // --> Temps du RTC par défaut.

    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
  return retCode; // -> retourne la valeur true si le capteur est bien initié, faux dans le cas contraire.
}

/*DateTime heureAvancee(bool _DEFAULT_) {
  int adj;
  DateTime now = rtc.now();
  if (_DEFAULT_ == true) {
    adj = 0;
  } else {
    adj = -1;
  }
  DateTime adjustTime(now + TimeSpan(0, adj, 0, 0));
  return adjustTime;
  }*/

bool setTimeRTC(uint16_t YYYY, uint8_t MM, uint8_t DD, uint8_t hh, uint8_t mm, uint8_t ss) { //Fonction qui permet une modification simple de la date et heure enregistré.
  rtc.adjust(DateTime(YYYY, MM, DD, hh, mm, ss)); //Ajuste le temps selon les préférences.
  return true; //Retourne la confirmation du changement d'heure.
}
String getDateRTC(){
  char date[11] =  "YYYY/MM/DD"; //Format
  rtc.now().toString(date); //Conversion de la valeur actuel en String.
  return date;
}
String getTimeRTC(bool heureAvance = false) { //Fonction qui retourne la valeur actuel du RTC

  /*int adj = 0;
    if (heureAvance == true){
    adj = 1;
    rtc.adjust(rtc.now()+TimeSpan(0,+adj,0,0));
    }else{
    rtc.adjust(rtc.now()+TimeSpan(0,+adj,0,0));
    }*/

  char rTime[9] =  "hh:mm:ss"; //Format
  rtc.now().toString(rTime); //Conversion de la valeur actuel en String.
  return rTime;
}
bool timeAdjust() {
  bool retCode = false;

}
//String MenuRTC(){
//  String txt =
//  "M ou m -> afficher le Menu" + "\n" +
//  "S ou s -> modifier le temps enregistrer dans le module RTC" + "\n";
//  return txt;
//}


#ifdef DEBUG // -> initialisation de l'I2C, EN MODE DEBUG SEULEMENT
void i2c_init(const int PIN_POW_EN = 13) {

  pinMode(PIN_POW_EN, OUTPUT);
  digitalWrite(PIN_POW_EN, HIGH);

  // wait for MAX chip to stabilize
  delay(500);
}
#endif
