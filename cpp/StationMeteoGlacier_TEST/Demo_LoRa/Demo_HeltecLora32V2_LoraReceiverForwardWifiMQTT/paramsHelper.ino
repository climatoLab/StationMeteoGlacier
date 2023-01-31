void logOk(String subject, String msg) {
  Serial.println(String("#ok:") + subject + ' ' + msg);
}
void logError(String subject, String msg) {
  Serial.println(String("#ERR:") + subject + ' ' + msg);
}

void gererSerialEntrant() {
  if (Serial.available()) {                   //Verifie si du texte a été envoyé via le moniteur série
    String message = Serial.readStringUntil('\n');     //Mets en mémoire tout le texte envoyé par le port Série
    if (message.charAt(0) == '#') {
      gererNouveauxParams(message);
    }
  }
}

bool validateParams() {
  bool retCode = false;
  if (testerStr("SSID", mesPreferences.getString("SSID",""),false))
    //if (testerStr("PASSWORD", mesPreferences.getString("PASSWORD",""),true))
    //  if (testerStr("WWWU", mesPreferences.getString("WWWU",""),false))
    //    if (testerStr("WWWP", mesPreferences.getString("WWWP",""),false))
    //      if (testerStr("NOM", mesPreferences.getString("NOM",""),false))
            retCode = true;
 return retCode;
}

bool testerUShort(String nom, uint16_t valeur) {
  bool retCode = false;
  if (valeur == 0) {
    logError(nom, "invalide");
  } else {
    logOk(nom, String(valeur));
    retCode = true;
  }
  return retCode;
}

bool testerStr(String nom, String param, bool sensible) {
  bool retCode = false;
  if (!testChar(param)) {
    logError(nom, "invalide");
  }
  else {
    if (sensible)
      logOk(nom,"**********");
    else
      logOk(nom, param);
    retCode = true;
  }
  return retCode;
}

bool testChar(String str) {
  bool retCode = false;
  if (str.length() > 0)
    for (int i = 0; i < STRING_LENGTH; i++) {
      if (str.charAt(i) == '\0') {
        retCode = true;
      }
    }
  return retCode;
}


void gererNouveauxParams(String msg) {
  if (msg.startsWith("#SSID")) {
    readWrite("SSID", msg);
  }
  else if (msg.startsWith("#PASSWORD")) {
    readWrite("PASSWORD", msg);
  }
  else if (msg.startsWith("#WWWU")) {
    readWrite("WWWU", msg);
  }
  else if (msg.startsWith("#WWWP")) {
    readWrite("WWWP", msg);
  }
  else if (msg.startsWith("#NOM")) {
    readWrite("NOM", msg);
  }  
  else if (msg.startsWith("#MAC")) {
    byte mac[6];
    WiFi.macAddress(mac);
    printMAC(mac);
  }
  else if (msg.startsWith("#IP")) {
    IPAddress ip = WiFi.localIP();
    Serial.print("#ok:IP:");
    Serial.println(ip);
  }
  else if (msg.startsWith("#OUBLIE")) {
    mesPreferences.putString("SSID", "");
    mesPreferences.putString("PASSWORD", "");
    Serial.print("#ok:ParamWifiOublies");
  }
  else if (msg.startsWith("#ALLCFG")) {
    //Suggestion: pour le robot, entre Mega et ESP, ajouter un delay(200) entre les print des params
    Serial.println("Tous les params de config:");
    byte mac[6] = {0,0,0,0,0,0};
    WiFi.macAddress(mac);
    IPAddress ip = WiFi.localIP();
    Serial.println("VERSION: "+String(_VERSION));
    Serial.print("NOM: "); Serial.println(mesPreferences.getString("NOM",""));
    Serial.print("MAC: "); printMAC(mac);
    Serial.print("IP: "); Serial.println(ip);
    Serial.print("SSID: "); Serial.println(mesPreferences.getString("SSID",""));
    Serial.print("PWD:  "); for (uint8_t i=0; i<mesPreferences.getString("PASSWORD","").length();i++) Serial.print("*"); Serial.print("\n");
    Serial.print("WWWU: "); Serial.println(mesPreferences.getString("WWWU",""));
    //Serial.println("PORT: "+String(mesPreferences.getUShort("PORT",0)));
    Serial.print("WWWP: "); Serial.println(mesPreferences.getString("WWWP",""));
    Serial.print("État de connexion wifi: "); Serial.println(wifiEstConnecte()?"OUI":"NON");
//    Serial.print("État du websocket: "); Serial.println(client.available()?"OK":"NOTOK");
  }
  else if (msg.startsWith("#CLEARALL")) {
    //Pas super-clean, faudrait effacer toute la chaine pour etre securitaire, mais ca suffira pour l'instant...
    Serial.println("REMIZE A ZERO DES PARAMETRES DANS 5 SECONDES!!!");
    delay(5000);
    nvs_flash_erase(); // erase the NVS partition and...
    nvs_flash_init();
    Serial.println("REDEMARRAGE DANS 5 SECONDES!!!");
    delay(5000);    
    ESP.restart();
  } else {
      logError("INCONNU", "Parametre non reconnu: "+msg);
  }

  if (msg.startsWith("#REBOOT")) {
    ESP.restart();
  }
}

void printMAC(byte mac[6]) {
  Serial.print("#ok:MAC:");
  printHex(mac[0]);
  Serial.print(":");
  printHex(mac[1]);
  Serial.print(":");
  printHex(mac[2]);
  Serial.print(":");
  printHex(mac[3]);
  Serial.print(":");
  printHex(mac[4]);
  Serial.print(":");
  printHex(mac[5]);
  Serial.print('\n');
}

void printHex(byte value) {
  if(value < 0x10) { Serial.print('0');}
  Serial.print(value, HEX);
}

bool readWriteUint(String dest, String msg) {
  bool retCode = false;
  int indexValue = msg.indexOf(':') + 1;
  if (indexValue > 0) {
    uint16_t valeur = msg.substring(indexValue).toInt();
    if (testerUShort(dest,valeur)) {
      mesPreferences.putUShort(dest.c_str(), valeur);
      retCode = true;
    }
  }
  return retCode;
}

bool readWrite(String nom, String msg) {
  bool retCode = false;
  int indexValue = msg.indexOf(':') + 1;
  if (indexValue > 0) {
    char dataPlaceHolder[STRING_LENGTH];
    copy(indexValue, msg, dataPlaceHolder);
    if (testerStr(nom, String(dataPlaceHolder),false)) {
       mesPreferences.putString(nom.c_str(), String(dataPlaceHolder));
      retCode = true;
    }    
  }
  return retCode;
}
void copy(int startIndex, String msg, char* dest) {
  int index = 0;
  while (index < (STRING_LENGTH-1) && msg.charAt(startIndex + index) != '\r' && msg.charAt(startIndex + index) != '\n' && (startIndex + index) < msg.length()) {
    dest[index] = msg.charAt(startIndex + index);
    index++;
  }
  while (index < STRING_LENGTH) {
    dest[index] = '\0';
    index++;
  }
}
