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
    Serial.println("\t");
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
  
  Serial.println("Température du BMP388 (°C)          = " + String(moSbdMessage.bmpTemperatureC));
  Serial.println("Pression du BMP388 (hPa)            = " + String(moSbdMessage.bmpPressionHPa));
  Serial.println("Altitude du BMP388 (m)              = " + String(moSbdMessage.bmpAltitude));
  Serial.println("Température du DHT22 (°C)           = " + String(moSbdMessage.dhtTemperatureC));
  Serial.println("Humidité du DHT22 (%)               = " + String( moSbdMessage.dhtHumidite));
  Serial.println("Température du thermocouple (°C)    = " + String(moSbdMessage.tcTemperatureC));
  Serial.println("Luminosité (lux)                    = " + String(moSbdMessage.gy49LuminositeLux));
  Serial.println("Distance du VL53L1X (mm)            = " + String(moSbdMessage.vlDistanceMM));
  Serial.println("Point cardinal de la girouette      = " + String(moSbdMessage.windDirection));
  Serial.println("Vitesse de vent de l'anémomètre     = " + String(moSbdMessage.windSpeed));
  Serial.println("Tension du Vin (V)                  = " + String(moSbdMessage.Vin));
  Serial.println("IterationCounter                    = " + String(moSbdMessage.iterationCounter));

  printLine();
  Serial.print("Chaine de données envoyée sur la carte uSD\n"+str_donnees());
  printLine();
}
