//--- Routines specifiques ----------------------------------------------

String str_data(){ //--> Met dans une variable String la structure de nos données
  String s_msg = getDateRTC() + vir
               + getTimeRTC() + vir
               + String(lecture_VinExt()) + vir
               + String(bmpTemp()) + vir
               + String(bmpPression()) + vir
               + String(bmpAltitude()) + vir
               + String(dhtHumi()) + vir
               + String(dhtTemp()) + vir
               + String(tempTC()) + vir
               + String(gyLux()) + "\n";
return s_msg;
}

void menu() {
  Serial.println("---------------------------menu---------------------------");
  Serial.println("C pour afficher les données de température du BMP388, du thermocouple, du DHT2 ainsi que c pour arrêter");
  Serial.println("P pour afficher les données de pression et d'altitude du BMP388 et p pour arrêter");
  Serial.println("L pour afficher les mesures du GY-49 et l pour arrêter");
  Serial.println("A pour afficher l'humidité captée par le DHT22 et a pour arrêter");
  Serial.println("Y pour afficher la valeur du compteur du pluviomètre et y pour effacer sa valeur");
  Serial.println("F pour afficher toutes les mesures de la station météo et f pour arrêter");
  Serial.println("T pour afficher la tension d'entrée et t pour arrêter");
  Serial.println("H pour afficher l'heure actuelle du rtc et h pour arrêter");
  Serial.println("D ou d pour afficher la date du rtc");
  Serial.println("K ou k pour enregistrer des données sur la carte SD toutes les 10 secondes");
  Serial.println("W ou w pour configuration du Wifi");
  Serial.println("I pour afficher l'intensité du signal du réseau WiFi (RSSI)");
  Serial.println("S ou s pour la configuration Thingsboard");
  Serial.println("E pour envoyer les mesures de la station météo vers ThingsBoard et e pour arrêter");
  Serial.println("U pour activer le mode automatique et u pour arrêter");
  Serial.println("V ou v pour obtenir la Version du programme");
  Serial.println("M ou m pour afficher ce menu");  
  Serial.println("R ou r pou restarter l'ESP32");    
  Serial.println("----------------------------------------------------------\r\n");
}


void decodage() {

  if (monDecodeur.available()) {
    //commande = monDecodeur.getCommand();
    switch (monDecodeur.getCommand()) {

      //++++++++++++++++++++++++++++++++++++++++++++++
      //case qui lit les températures du BMP388, du thermocouple et du DHT22
      //++++++++++++++++++++++++++++++++++++++++++++++
      case'C' :
        Serial.println(ACK);
        Serial.println("Températures du BMP388, du thermocouple et du DHT22 :\n");      
        flagCtemperatures = true;
      break;

      case 'c':
        Serial.println(ACK);
        Serial.println("Interruption de la commande\n");    
        flagCtemperatures = false;
        menu();
      break;
      //++++++++++++++++++++++++++++++++++++++++++++++
      //case qui lit la pression mesuré par le BMP388
      //++++++++++++++++++++++++++++++++++++++++++++++
      case'P' :
        Serial.println(ACK);
        Serial.println("Pression mesurée par le BMP388 :\n");      
        flagHPaBmp = true;
      break;  

      case'p' :
        Serial.println(ACK);
        Serial.println("Interruption de la commande\n");       
        flagHPaBmp = false;
        menu();
      break;  
      //++++++++++++++++++++++++++++++++++++++++++++++
      //case qui lit l'humidité mesurée par le DHT22
      //++++++++++++++++++++++++++++++++++++++++++++++
      case'A' :
        Serial.println(ACK);
        Serial.println("Humidité mesurée par le DHT22 :\n");      
        flagHdht = true;
      break;  

      case'a' :
        Serial.println(ACK);
        Serial.println("Interruption de la commande\n");       
        flagHdht = false;
        menu();
      break;                  
      //++++++++++++++++++++++++++++++++++++++++++++++
      //case qui ajuste la date et l'heure
      //++++++++++++++++++++++++++++++++++++++++++++++


      //++++++++++++++++++++++++++++++++++++++++++++++
      //case qui donne la date
      //++++++++++++++++++++++++++++++++++++++++++++++
      case'D' : case'd' :
        Serial.println(ACK);
        Serial.println("Affichage de la date d'aujourd'hui\n"); 
        rtcMontreDate();
      break;

      //++++++++++++++++++++++++++++++++++++++++++++++
      //case qui donne l'heure
      //++++++++++++++++++++++++++++++++++++++++++++++
      case'H' :
        Serial.println(ACK);
        Serial.println("Affichage de l'heure actuelle\n"); 
        flagMontreheureRtc = true;
      break;

      case 'h':
        Serial.println(ACK);
        Serial.println("Interruption de la commande\n");  
        flagMontreheureRtc = false;
        menu();
      break;
      //++++++++++++++++++++++++++++++++++++++++++++++
      //case pour traiter le mode dump du data de la SD
      //++++++++++++++++++++++++++++++++++++++++++++++
      case'K' : 
        Serial.println(ACK);
        Serial.println("ÉCriture des données mesurées sur la carte micro SD\n"); 
        flagEcritSd = true;      
      break;
      
      case 'k':  
        Serial.println(ACK);
        Serial.println("Interruption de la commande\n");  
        flagEcritSd = false;
        menu();
      break;  
      //++++++++++++++++++++++++++++++++++++++++++++
      //case qui écrit et lit les parametres du GY-49
      //++++++++++++++++++++++++++++++++++++++++++++
      case 'L' :
        Serial.println(ACK);
        Serial.println("Affichage de la lumière ambiante captée par le gy-49\n");  
        flagGy49 = true;
      break;

      case 'l':
        Serial.println(ACK);
        Serial.println("Interruption de la commande\n");  
        flagGy49 = false;
        menu();
      break;
      //++++++++++++++++++++++++++++++++++
      //case qui écrit la tension d'entrée
      //++++++++++++++++++++++++++++++++++
      case 'T' : 
        Serial.println(ACK);
        Serial.println("Affichage de la tension d'entrée\n");  
        flagVin = true;
      break;

      case 't' :
        Serial.println(ACK);
        Serial.println("Interruption de la commande\n");  
        flagVin = false;
        menu();
      break;      
      //++++++++++++++++++++++++++++++
      //case qui écrit le menu
      //++++++++++++++++++++++++++++++
      case 'M' : case 'm' :
        Serial.println(ACK);
        Serial.println(commande);
        menu();
      break;
      
      //++++++++++++++++++++++++++++++
      //case pour envoyer des données sur ThingsBoard
      //++++++++++++++++++++++++++++++
      case 'E' :
        Serial.println(ACK);
        Serial.println("Envoie des données");
        flagEnvoie = true;               
      break;

      case 'e' : 
        Serial.println(ACK);
        Serial.println("Arrêt de l'envoie des données");
        flagEnvoie = false;     
        menu();                 
      break;

            
      //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      //case qui affiche les données de mesures dans le moniteur série
      //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      case 'F' :
        Serial.println(ACK);   
        Serial.println("Affichage de toutes les mesures de la station météo\n");     
        flagMesures = true;
      break;

      case 'f':
        Serial.println(ACK);  
        Serial.println("Interruption de la commande\n");      
        flagMesures = false;
        menu();
      break;        
      //++++++++++++++++++++++++++++++++++++++++++++++
      //case pour entre en mode configuration du TB
      //++++++++++++++++++++++++++++++++++++++++++++++
      case 'S' : case 's':
        Serial.println("Mode de configuration di service Thingsboard\nLes commandes suivantes sont disponibles:");
        Serial.println("#TBHOST:[nom_ou_IP_serveurTB]\n#TBTOKEN:[token_client]\n#STID:[num_indicateur]\n#STATUS:[(A)ctif|(I)nactif]\n#OUBLIE pour effacer les parametres\n#END pour terminer la saise et sauvegarder.");
        while (!gererSaisieCfgTB()) {}
        Serial.println("Initializing TB connection");
        connecteTB();
      break;
            
      //++++++++++++++++++++++++++++++++++++++++++++++
      //case pour entre en mode configuration du Wifi
      //++++++++++++++++++++++++++++++++++++++++++++++
      case 'W' : case 'w':
        Serial.println("Mode de configuration du Wifi\nLes commandes suivantes sont disponibles:");
        Serial.println("#SSID:[nom_SSID]\n#PWD:[mdp_wifi]\n#OUBLIE pour effacer les parametres\n#END pour terminer la saise et sauvegarder.");
        while (!gererSaisieCfgWifi()) {}
        Wifi_begin();
        //if (etat_connection) iniRTC();
      break;

      //++++++++++++++++++++++++++++++++++++++++++++++
      //case pour afficher le nom du programme et sa Version
      //++++++++++++++++++++++++++++++++++++++++++++++
      case 'V' : case 'v': 
        Serial.println("Station Météo - Version " + String(Version));
      break;
      
      //++++++++++++++++++++++++++++++++++++++++++++++
      //case pour afficher le compteur du pluviomètre
      //++++++++++++++++++++++++++++++++++++++++++++++
      case 'Y' :
        PCF8583_read_cntr(PCF_I2C_ADDR); //Pour afficher les valeurs du compteur
      break;

      case 'y' :
        PCF8583_clr_cntr(PCF_I2C_ADDR); //Pour effacer les valeurs du compteur
      break;
  
      //++++++++++++++++++++++++++++++++++++++++++++++
      //case pour afficher le compteur du pluviomètre
      //++++++++++++++++++++++++++++++++++++++++++++++   
      case 'G' :
        flagGirouette = true;
      break;

      case 'g' :
        flagGirouette = false;
      break;

      //++++++++++++++++++++++++++++++++++++++++++++++
      //case pour afficher le SSID du réseau WiFi
      //++++++++++++++++++++++++++++++++++++++++++++++   
      case 'I' :
        Serial.println(WiFi.RSSI());
      break;   

      //++++++++++++++++++++++++++++++++++++++++++++++
      //case pour restarter l'ESP32
      //++++++++++++++++++++++++++++++++++++++++++++++   
      case 'R' : case 'r' :
        ESP.restart();
      break;  

      //++++++++++++++++++++++++++++++++++++++++++++++
      //case pour activer le mode automatique
      //++++++++++++++++++++++++++++++++++++++++++++++   
      case 'U' :
        flagAutomatique = true;
      break;

      case 'u' :
        flagAutomatique = false;
      break;      
      
      default:
        Serial.println(NAK);
    }
  }
}


void rtcMontreDate(){
  Serial.println("Date : " + String(getDateRTC()) + "\n");
}


void rtcMontreheure(){
  Serial.println("Temps : " + String(getTimeRTC())+ "\n");  
}


void sdEcritDonnees(){
  unsigned long temps = millis();
  //if(temps - sdCompteur >= 10000){
    NbrEcritures++;
    Serial.println("Envoie de données sur la carte toutes les 10 secondes");
    Serial.println("Nombres d'écritures = " + String(NbrEcritures) + "\n");
    sendData(str_data(), path); //--> envoi des données vers la carte SD.    
    //sdCompteur = temps;
    delay(5000); 
  //}
}

void MesuresTotales(){
  Serial.println("Température du BMP388 (°C) = " + String(bmpTemperatureC));
  Serial.println("Pression du BMP388 (hPa) = " + String(bmpPressionHPa));
  Serial.println("Température du thermocouple (°C) = " + String(tcTemperatureC));
  Serial.println("Humidité du DHT22 (%) = " + String(dhtHumidite));
  Serial.println("Température du DHT22 (%) = " + String(dhtTemperatureC));
  Serial.println("Distance captée (mm) = " + String(vlDistanceMM));
  Serial.println("Date : " + String(getDateRTC())); 
  Serial.println("Temps : " + String(getTimeRTC()));
  Serial.println("Tension : " + String(Vin)); 
  delay(5000);      
}

void TemperaturesC() {
  unsigned long temps = millis();
  //if(temps - CompteurC >= 5000){
    if (bmpTemperatureC >= -40 && bmpTemperatureC <= 85) { 
      Serial.println(String ("Température du BMP (°C) = ") + String(bmpTemperatureC) + String(" °C"));
     }
    if (dhtTemperatureC >= -40 && dhtTemperatureC <= 80) {   
      Serial.println(String ("Température du DHT (°C) = ") + String(dhtTemperatureC) + String(" °C"));    }
    if (tcTemperatureC >= -73 && tcTemperatureC <= 482){ 
      Serial.println(String ("Température du TC (°C) = ") + String(tcTemperatureC) + String(" °C\n"));   
    }   
    //CompteurC = temps; 
    delay(5000); 
  //}

}


void PressionKPa() {
  unsigned long temps = millis();
  //if(temps - bmpCompteurHPa >= 5000){
    if (bmpPressionHPa >= 300 && bmpPressionHPa <= 1250) {
      Serial.println(String("Pression (hPa) = ") + String(bmpPressionHPa) + String(" KPa"));
    }
  
    else if (bmpPressionHPa < 300) {
      Serial.println(String("Erreur ! ") + String(bmpPressionHPa) + String(" hPa est inférieur à la pression minimale que le BMP388 peut capter de 300 hPa"));
    }
    else if (bmpPressionHPa > 1250) {
      Serial.println(String("Erreur ! ") + String(bmpPressionHPa) + String(" hPa est supérieur à la température maximale que le BMP388 peut capter de 1250 hPa"));
    }
    else if (isnan(bmpPressionHPa)) {
      Serial.println("Erreur ! NaN");
    }
    Serial.println("Altitude = " + String(bmpAltitude()) + " m\n");
    //bmpCompteurHPa = temps;
    delay(5000); 
  //}
}


void humditePourcentage() {
  unsigned long temps = millis();
  //if(temps - dhtCompteurHum >= 5000){
    if (dhtHumidite >= 0 && dhtHumidite <= 100) {
      Serial.println(String ("Humidité (%) = ") + String(dhtHumidite) + String(" %\n"));
    }
    else if (dhtHumidite < 0) {
      Serial.println(String("Erreur ! ") + String(dhtHumidite) + String("% est plus petit que 0%\n"));
    }
    else if (dhtHumidite > 100) {
      Serial.println(String("Erreur ! ") + String(dhtHumidite) + String("% est plus grand que 100%\n"));
    }
    else if (isnan(dhtHumidite)) {
      Serial.println("Erreur ! NaN ");
    }
    //dhtCompteurHum = temps;
    delay(5000); 
  //}
}


void luminosite() {
  unsigned long temps = millis();
  //if(temps - gy49CompteurLux >= 5000){  
    if (gy49LuminositeLux >= 0.045 && gy49LuminositeLux <= 188000) {
      Serial.println(String("Luminosité (lux) = ") + String(gy49LuminositeLux) + String(" lux\n"));
    }
  
    else if (gy49LuminositeLux < 0,045) {
      Serial.println(String("Erreur ! ") + String(gy49LuminositeLux) + String(" lux est inférieur à la luminosité minimale que le gy-49 peut capter de 0.045 lux\n"));
    }
    else if (gy49LuminositeLux > 188000) {
      Serial.println(String("Erreur ! ") + String(gy49LuminositeLux) + String(" lux est supérieur à la luminosité maximale que le gy-49 peut capter de 188000 lux\n"));
    }
   //gy49CompteurLux = temps; 
   delay(5000); 
  //}
}


void VIN() {
  unsigned long temps = millis();
  //if(temps - VinCompteur >= 5000){  
    Serial.println(String ("Vin = ") + String(Vin) + String(" V\n"));
    //VinCompteur = temps; 
    delay(5000); 
  //}  
}


void distanceMM() {
  unsigned long temps = millis();
  //if(temps - vlComptpeurMM >= 5000){
    Serial.println(String ("Distance (mm) = ") + String(vlDistanceMM) + String(" mm\n"));    
    //vlComptpeurMM = temps;
    delay(5000); 
  //}
}


void GPS() {
  // This sketch displays information every time a new sentence is correctly encoded.
  //unsigned long temps = millis();
  //if(temps - gpsCompteur > msgInterval){
    while (Serial1.available() > 0)
      if (gps.encode(Serial1.read()))
        Serial.println(String ("Latitude = ") + String(latitudeGPS) + String(""));
        Serial.println(String ("Longitude = ") + String(longitudeGPS) + String(""));
        Serial.println(String ("Altitude = ") + String(altitudeGPS) + String("\n")); 
  
    // If 5000 milliseconds pass and there are no characters coming in
    // over the software serial port, show a "No GPS detected" error
    if (millis() > 5000 && gps.charsProcessed() < 10)
    {
      Serial.println("No GPS detected\n");
      while(true);
    }  
    //gpsCompteur = temps;    
  //}
}

//ThingsBoard///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void envoieMesures(){
  unsigned long temps = millis();
 // if(temps - compteurTB >= 5000){
    //envoieGPS();
    envoieTemperaturesC();
    envoiePressionKPa(); 
    envoiehumditePourcentage(); 
    envoieLuminosite();  
    envoieVin(); 
    envoieDistance();
    envoiePluviometre();
    envoieRSSI();
    envoieGirouette();
    NbrMessagesEnvoyes++;
    Serial.println("Nbr messages envoyés = " + String(NbrMessagesEnvoyes)+ "\n\n");
    //compteurTB = temps;  
    delay(5000);    
  //}  
}

void envoieRSSI(){
  int signalIntensite = WiFi.RSSI();
  tb.sendTelemetryFloat("Intensité WiFi", signalIntensite);
  Serial.println("Intensité du signal = " + String(signalIntensite) + "\n");
}


void envoiePluviometre() {
    PCF8583_read_cntr(PCF_I2C_ADDR);
    tb.sendTelemetryFloat("Compteur pluviomètre", CompteurPluie);
    //Serial.println(String ("Compteur du pluviomètre = ") + String(CompteurPluie) + String("\n\n"));
}


void envoieTemperaturesC() {
  
  if (bmpTemperatureC >= -40 && bmpTemperatureC <= 85) { 
    tb.sendTelemetryFloat("\n\nTempérature BMP", bmpTemperatureC);  
    Serial.println(String ("Température du BMP (°C) = ") + String(bmpTemperatureC) + String(" °C"));
    //NbrMessagesEnvoyes++;
   }
  if (dhtTemperatureC >= -40 && dhtTemperatureC <= 80) {  
    tb.sendTelemetryFloat("Température DHT", dhtTemperatureC);   
    Serial.println(String ("Température du DHT (°C) = ") + String(dhtTemperatureC) + String(" °C"));    }
  if (tcTemperatureC >= -73 && tcTemperatureC <= 482){ 
    tb.sendTelemetryFloat("Température TC", tcTemperatureC);
    Serial.println(String ("Température du TC (°C) = ") + String(tcTemperatureC) + String(" °C\n"));   
  }
}


void envoiePressionKPa() {
  if (bmpPressionHPa >= 300 && bmpPressionHPa <= 1250) {
    tb.sendTelemetryFloat("Pression BMP", bmpPressionHPa);
    Serial.println(String("Pression (hPa) = ") + String(bmpPressionHPa) + String(" KPa\n"));
    //NbrMessagesEnvoyes++;
  }

  else if (bmpPressionHPa < 300) {
    Serial.println(String("Erreur ! ") + String(bmpPressionHPa) + String(" hPa est inférieur à la pression minimale que le BMP388 peut capter de 300 hPa\n"));
  }
  else if (bmpPressionHPa > 1250) {
    Serial.println(String("Erreur ! ") + String(bmpPressionHPa) + String(" hPa est supérieur à la température maximale que le BMP388 peut capter de 1250 hPa\n"));
  }
  else if (isnan(bmpPressionHPa)) {
    Serial.println("Erreur ! NaN\n");
  }
  Serial.println("Altitude = " + String(bmpAltitude()) + " m\n");
  tb.sendTelemetryFloat("Altitude BMP", bmpAltitude());
}


void envoiehumditePourcentage() {
  if (dhtHumidite >= 0 && dhtHumidite <= 100) {
    tb.sendTelemetryFloat("Humidité DHT", dhtHumidite);
    Serial.println(String ("Humidité (%) = ") + String(dhtHumidite) + String(" %\n"));
    //NbrMessagesEnvoyes++;
  }
  else if (dhtHumidite < 0) {
    Serial.println(String("Erreur ! ") + String(dhtHumidite) + String("% est plus petit que 0%\n"));
  }
  else if (dhtHumidite > 100) {
    Serial.println(String("Erreur ! ") + String(dhtHumidite) + String("% est plus grand que 100%\n"));
  }
  else if (isnan(dhtHumidite)) {
    Serial.println("Erreur ! NaN ");
  }
}


void envoieLuminosite() {
  if (gy49LuminositeLux >= 0.045 && gy49LuminositeLux <= 188000) {
    tb.sendTelemetryFloat("Luminosité GY49", gy49LuminositeLux);
    Serial.println(String("Luminosité (lux) = ") + String(gy49LuminositeLux) + String(" lux\n"));
    //NbrMessagesEnvoyes++;
  }

  else if (gy49LuminositeLux < 0,045) {
    Serial.println(String("Erreur ! ") + String(gy49LuminositeLux) + String(" lux est inférieur à la luminosité minimale que le gy-49 peut capter de 0.045 lux\n"));
  }
  else if (gy49LuminositeLux > 188000) {
    Serial.println(String("Erreur ! ") + String(gy49LuminositeLux) + String(" lux est supérieur à la luminosité maximale que le gy-49 peut capter de 188000 lux\n"));
  }
}


void envoieVin() {
  tb.sendTelemetryFloat("Vin", Vin);  
  Serial.println(String ("Vin = ") + String(Vin) + String(" V\n"));
}


void envoieDistance() {
  tb.sendTelemetryFloat("Distance VL53L1X", vlDistanceMM);  
  Serial.println(String ("Distance (mm) = ") + String(vlDistanceMM) + String(" mm\n"));
}


void envoieGPS() {
  // This sketch displays information every time a new sentence is correctly encoded.
  while (Serial1.available() > 0)
    if (gps.encode(Serial1.read()))
      tb.sendTelemetryFloat("Latitude GPS", latitudeGPS);  
      tb.sendTelemetryFloat("Longitude GPS", longitudeGPS);  
      tb.sendTelemetryFloat("Altitude GPS", altitudeGPS); 
      Serial.println(String ("Latitude = ") + String(latitudeGPS) + String(""));
      Serial.println(String ("Longitude = ") + String(longitudeGPS) + String(""));
      Serial.println(String ("Altitude = ") + String(altitudeGPS) + String("\n")); 

  // If 5000 milliseconds pass and there are no characters coming in
  // over the software serial port, show a "No GPS detected" error
  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println("No GPS detected\n");
    while(true);
  }  
}
