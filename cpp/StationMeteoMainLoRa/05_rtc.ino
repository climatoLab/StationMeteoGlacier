void init_RTC() { // -> Fonction à introduire après l'initialisation du I2C. Permet l'initialisation de l'objet / module RS3231
 
  if (! rtc.begin()) { //Démare l'initialisation et envoie une message d'erreur si la connection n'est pas réussis.
    Serial.println("Couldn't find RTC");
    Serial.flush();

    delay(10);
  } else { //Connecté? envoi un message de confirmation de connection.
    Serial.println("RTC : Initialisation complété.");
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
}


bool setTimeRTC(uint16_t YYYY, uint8_t MM, uint8_t DD, uint8_t hh, uint8_t mm, uint8_t ss) { //Fonction qui permet une modification simple de la date et heure enregistré.
  rtc.adjust(DateTime(YYYY, MM, DD, hh, mm, ss)); //Ajuste le temps selon les préférences.
  return true; //Retourne la confirmation du changement d'heure.
}
String getDateRTC(){
  char date[11] =  "YYYY-MM-DD"; //Format
  rtc.now().toString(date); //Conversion de la valeur actuel en String.
  return date;
}
String getTimeRTC(bool heureAvance = false) { //Fonction qui retourne la valeur actuel du RTC
  char rTime[9] =  "hh:mm:ss"; //Format
  rtc.now().toString(rTime); //Conversion de la valeur actuel en String.
  return rTime;
}
void readRTC(){
  moSbdMessage.unix = rtc.now().unixtime();
}

void updateIteration(){
  moSbdMessage.iterationCounter = iterationRTC;  //Met à jour le nombre d'itérations depuis le début du programme
  iterationRTC++;
}

void updateIterationLoRa(){
  moSbdMessage.iterationCounter = iterationRTC_LoRa;
  iterationRTC_LoRa++;
}
