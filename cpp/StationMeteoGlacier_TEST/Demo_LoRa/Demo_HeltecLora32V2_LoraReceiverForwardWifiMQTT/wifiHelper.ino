bool connecterAuWifi() {
  bool retCode = false;
  String ssid = mesPreferences.getString("SSID", ""); 
  String pwd = mesPreferences.getString("PASSWORD", "");
  
  if (ssid == ""){  //Attention il est possible d'avoir un pwd vide 
    Serial.println("No values saved for ssid or password");
  } else {
    WiFi.disconnect(true);
	  delay(100);
	  WiFi.mode(WIFI_STA);
	  WiFi.setAutoConnect(true);
    u8g2.drawStr(0,20,"Wifi...");
    u8g2.sendBuffer();
    logOk("wifi", "Recherche  ");

    WiFi.begin(ssid.c_str(), pwd.c_str());
  
    int compteTour = 1;
    while (!wifiEstConnecte() && compteTour<30) {
      compteTour++;
      delay(500);
    }
    if (compteTour<30) {
      retCode = true;
      logOk("wifi", "Connecté.");
      u8g2.drawStr(44,20,"Ok!");
    } else {
      u8g2.drawStr(44,20,"Failed!");
    }
    u8g2.sendBuffer();
  }
  
  return retCode;
}

bool wifiEstConnecte() {
  return WiFi.status() == WL_CONNECTED;
}
