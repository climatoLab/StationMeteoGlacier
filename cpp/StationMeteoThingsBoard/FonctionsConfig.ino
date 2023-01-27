//================================================================================================================================
//                Prise en charge de la configuration du Thingsboard via port Série. Mode "S" du menu.
//================================================================================================================================
bool gererSaisieCfgTB() {
  bool configTerminee = false;
  if (Serial.available()) {                   //Verifie si du texte a été envoyé via le moniteur série
    String message = Serial.readStringUntil('\n');     //Mets en mémoire tout le texte envoyé par le port Série
    if (message.charAt(0) == '#') {
      if (message.startsWith("#TBHOST")) {
        if (readWrite("TBHOST", message)) {
          Serial.println("Param Thingsboard HOST sauvegardé!");
        } else {
          Serial.println("Erreur de param TB Host!");
        }
      }
      if (message.startsWith("#TBTOKEN")) {
        if (readWrite("TBTOKEN", message)) {
          Serial.println("Param Thingsboard Token sauvegardé!");
        } else {
          Serial.println("Erreur de param TB TOKEN!");
        }
      }
      if (message.startsWith("#STID")) {
        if (readWrite("STID", message)) {
          Serial.println("Param stationID sauvegardé!");
        } else {
          Serial.println("Erreur de param stationID!");
        }
      }
      if (message.startsWith("#STATUS")) {
        int indexValue = message.indexOf(':') + 1;
        if (indexValue > 0) {
          char etat = toupper(message.charAt(indexValue));
          if ( etat == 'A' )
            mesPreferences.putBool("tbStatus",true);
          else
            mesPreferences.putBool("tbStatus",false);
       
          if (mesPreferences.getBool("tbStatus"))
            Serial.println("Param status sauvegardé: Actif");
          else
            Serial.println("Param status sauvegardé: Inactif");
        }
      }
      if (message.startsWith("#OUBLIE")) {
        mesPreferences.putString("TBHOST", "");
        mesPreferences.putString("TBTOKEN", "");
        mesPreferences.putBool("tbStatus",false);
        Serial.println("Param TB effacés!");
      }
      if (message.startsWith("#END")) {
        configTerminee = true;
      }
    }
  }
  return configTerminee;
}


//================================================================================================================================
//                Prise en charge de la configuration du Wifi via port Série. Mode "W" du menu.
//================================================================================================================================
bool gererSaisieCfgWifi() {
  bool configTerminee = false;
  if (Serial.available()) {                   //Verifie si du texte a été envoyé via le moniteur série
    String message = Serial.readStringUntil('\n');     //Mets en mémoire tout le texte envoyé par le port Série
    if (message.charAt(0) == '#') {
      if (message.startsWith("#SSID")) {
        if (readWrite("SSID", message)) {
          Serial.println("Param Wifi SSID sauvegardé!");
        } else {
          Serial.println("Erreur de param Wifi SSID!");
        }
      }
      if (message.startsWith("#PWD")) {
        if (readWrite("PASSWORD", message)) {
          Serial.println("Param Wifi PWD sauvegardé!");
        } else {
          Serial.println("Erreur de param Wifi PWD!");
        }
      }
      if (message.startsWith("#OUBLIE")) {
        mesPreferences.putString("SSID", "");
        mesPreferences.putString("PASSWORD", "");
        Serial.println("Param Wifi effacés!");
      }
      if (message.startsWith("#END")) {
        configTerminee = true;
      }
    }    
  }
  return configTerminee;
}


//================================================================================================================================
//                Décodage
//================================================================================================================================
bool readWrite(String nom, String msg) {
  bool retCode = false;
  int indexValue = msg.indexOf(':') + 1;
  if (indexValue > 0) {
    char dataPlaceHolder[STRING_LENGTH];
    copy(indexValue, msg, dataPlaceHolder);
    if (testerStr(nom, String(dataPlaceHolder))) {
       mesPreferences.putString(nom.c_str(), String(dataPlaceHolder));
      retCode = true;
    }    
  }
  return retCode;
}

bool testerStr(String nom, String param) {
  bool retCode = false;
  if (!testChar(param)) {
     Serial.println(nom+": Parametre invalide");
  }
  else {
    Serial.println(nom+": Parametre Ok");
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


//================================================================================================================================
//                WiFi
//================================================================================================================================


bool Wifi_begin() {
  bool retCode = false;

  String ssid = mesPreferences.getString("SSID", ""); 
  String pwd = mesPreferences.getString("PASSWORD", "");

  Serial.println("SSID : "String(ssid) + " et PWD : " + String(pwd));
  Serial.println("> ESP32 Client WiFi. Version du code = "); Serial.println(Version);
 
  if (ssid == "") {  //Il est possible d'avoir un PWD "vide"
    Serial.println("Wifi non configuré");
  } else {
    //Branchement au point d'accès WiFi
    Serial.print("> Branchement en cours avec le réseau : "); Serial.println(ssid);

    enableWiFi();

    WiFi.begin(ssid.c_str(), pwd.c_str());
  
    int compteTour = 1;
    while (!wifiEstConnecte() && compteTour < 20) {
      Serial.print('.');
      compteTour++;
      if (compteTour%25 == 0) Serial.print("\n");
      gererSaisieCfgWifi();
      etat_connection = false;
      delay(500);
    }
    if (wifiEstConnecte()) { 
      retCode = true; etat_connection = true;
      Serial.println("Connexion établie");
      Serial.print("\r\n> Adresse MAC : "); Serial.println(WiFi.macAddress());
      Serial.print("> Adresse IP : "); Serial.println(WiFi.localIP());
    } else {
      disableWiFi();
      Serial.println("Echec de connexion");
    }

  }
  return retCode;
}


void disableWiFi(){
    WiFi.disconnect(true);  // Disconnect from the network
    WiFi.mode(WIFI_OFF);    // Switch WiFi off
}


void enableWiFi(){
    WiFi.disconnect(false);  // Reconnect the network
    WiFi.mode(WIFI_STA);    // Switch WiFi off
}


bool wifiEstConnecte() {
  return WiFi.status() == WL_CONNECTED;
}


//================================================================================================================================
//                ThingsBoard
//================================================================================================================================
bool connecteTB(void) {
  // Reconnect to ThingsBoard, if needed
  bool retCode=false;
  if (etat_connection) {
    if (!tb.connected()) { 
      isSubcribed = false;
      //Récupération des valeurs:
      String tbhost = mesPreferences.getString("TBHOST","");
      String tbtoken = mesPreferences.getString("TBTOKEN","");
      if (tbhost == "" || tbtoken == ""){
         Serial.println("Thngsboard non configuré");
      } else {
        // Connect to the ThingsBoard
        Serial.print("Connecting to: ");
        Serial.print(tbhost);
        Serial.print(" with token ");
        Serial.println(tbtoken);
        if (tb.connect(tbhost.c_str(), tbtoken.c_str())) {
          retCode=true;
          isSubcribed=true;
          Serial.println("Connecté au serveur ThingsBoard");
        } else {
          Serial.println("Échec de connexion au serveur ThingsBoard");
        }
      } 
    } else retCode=true;    
  }
  return retCode;
}
