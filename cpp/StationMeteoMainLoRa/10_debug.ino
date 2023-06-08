void printLine()
{
  for (byte i = 0; i < 80; i++)
  {
    Serial.print("-");
  }
  Serial.println();
}

void printTab(byte _times)
{
  for (byte i = 0; i < _times; i++)
  {
    Serial.print("\t");
  }
}

void printSettings(){
  
  printLine();
  Serial.println("");
  printLine();
  Serial.print("");   printTab(1);    Serial.println();
  Serial.print("");   printTab(1);    Serial.println();
  Serial.print("");   printTab(1);    Serial.println();
  Serial.print("");   printTab(1);    Serial.println();
  Serial.print("");   printTab(1);    Serial.println();
  Serial.print("");   printTab(1);    Serial.println();
  Serial.print("");   printTab(1);    Serial.println();
  Serial.print("");   printTab(1);    Serial.println();
  Serial.print("");   printTab(1);    Serial.println();
  Serial.print("");   printTab(1);    Serial.println();
  
  printLine();
}

void printAcquisition(){

  printLine();
  Serial.println("Données enregistrées");
  printLine();
  
  Serial.println("Température du BMP388 (°C)          = " + String(bmpTemperatureC));
  Serial.println("Pression du BMP388 (hPa)            = " + String(bmpPressionHPa));
  Serial.println("Altitude du BMP388 (m)              = " + String(bmpAltitude));
  Serial.println("Température du DHT22 (°C)           = " + String(dhtTemperatureC));
  Serial.println("Humidité du DHT22 (%)               = " + String(dhtHumidite));
  Serial.println("Température du thermocouple (°C)    = " + String(tcTemperatureC));
  Serial.println("Luminosité (lux)                    = " + String(gy49LuminositeLux));
  Serial.println("Distance du VL53L1X (mm)            = " + String(vlDistanceMM));
  Serial.println("Point cardinal de la girouette      = " + String(windDirection));
  Serial.println("Vitesse de vent de l'anémomètre     = " + String(windSpeed));
  Serial.println("Tension du Vin (V)                  = " + String(Vin));
  Serial.println("IterationCounter                    = " + String(moSbdMessage.iterationCounter));

  printLine();
  Serial.print("Chaine de données envoyée sur la carte uSD\n"+str_donnees());
  printLine();
}

void printStats(){

  printLine();
  Serial.println("Statistiques");
  printLine();
  
  Serial.println("Température du BMP388 (°C)          = " + String(moSbdMessage.bmpTemperatureC));
  Serial.println("Pression du BMP388 (hPa)            = " + String(moSbdMessage.bmpPressionHPa));
  Serial.println("Altitude du BMP388 (m)              = " + String(moSbdMessage.bmpAltitude));
  Serial.println("Température du DHT22 (°C)           = " + String(moSbdMessage.dhtTemperatureC));
  Serial.println("Humidité du DHT22 (%)               = " + String(moSbdMessage.dhtHumidite));
  Serial.println("Température du thermocouple (°C)    = " + String(moSbdMessage.tcTemperatureC));
  Serial.println("Luminosité (lux)                    = " + String(moSbdMessage.gy49LuminositeLux));
  Serial.println("Distance du VL53L1X (mm)            = " + String(moSbdMessage.vlDistanceMM));
  Serial.println("Point cardinal de la girouette      = " + String(moSbdMessage.windDirection));
  Serial.println("Vitesse de vent de l'anémomètre     = " + String(moSbdMessage.windSpeed));
  Serial.println("Tension du Vin (V)                  = " + String(moSbdMessage.Vin));
  Serial.println("IterationCounter                    = " + String(moSbdMessage.iterationCounter));

  printLine();

}

void printSD_Info(){
  
  printLine();
  Serial.println("Information carte microSD");
  printLine();

  uint8_t cardType = SD.cardType();
  Serial.print("Type de carte micro SD:");   printTab(1);
  if(cardType == CARD_MMC){
    Serial.println("MMC");
  } else if(cardType == CARD_SD){
    Serial.println("SDSC");
  } else if(cardType == CARD_SDHC){
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }
    
  Serial.print("Taille de la carte micro SD:");   printTab(1);    Serial.println(String(SD.cardSize()));
  Serial.print("Espace total : ");                printTab(1);    Serial.println(String(SD.totalBytes()));
  Serial.print("Espace restant : ");              printTab(1);    Serial.println(String(SD.usedBytes()));
  
    
  printLine();
  
}
