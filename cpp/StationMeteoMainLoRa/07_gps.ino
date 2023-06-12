void configureGPS(){
   // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  GPS.begin(9600);

  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
  // the parser doesn't care about other sentences at this time

  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate
  // For the parsing code to work nicely and have time to sort thru the data, and
  // print it out we don't suggest using anything higher than 1 Hz

  // Request updates on antenna status, comment out to keep quiet
  GPS.sendCommand(PGCMD_ANTENNA);

  
}
// Lecture du GPS
void readGPS(){
  //Configuration du GPS 
  configureGPS();

  Serial.print("En attente du traffic GPS...");

  // Mise en variable de l'itération de la boucle while qui suit.
  int i = 0;

  // Boucle while si le GPS n'est pas connecté.
  while (!online.gps){

    // Début d'acquisition jusqu'a ce que le timeout soit dépassé.
    if(millis() - previousMillis_GPS <= interval_GPSTimeout ){
      
      //Voit si le fix sattelite à été réalisé
      if (GPS.fix) {

        // mise en variable des données
        latitudeGPS = GPS.lat;
        longitudeGPS = GPS.lon;
        altitudeGPS = GPS.altitude;
        satellites = (int)GPS.satellites;

        moSbdMessage.latitudeGPS = latitudeGPS * 10000;
        moSbdMessage.longitudeGPS = longitudeGPS * 10000;
        moSbdMessage.altitudeGPS = altitudeGPS * 100;
        moSbdMessage.satellites = satellites;

        // Variable booléen pour confirmer le fonctionnement.
        online.gps = true;
      }
      
      //Incrémente la variable d'itération
      i++;

      // Délai
      if(i<10){
        Serial.print(".");
        myDelay(250);
      }
      else {
        myDelay(250);
      }
  
    }
    // SI le fix n'est toujours pas disponible après le timeout, sort de la boucle while.
    else{
      break;
    }
  }
  // Message d'erreur
  if(online.gps){
    Serial.println("Réussi!");
  }
  else{
    Serial.println("Échec!");
  }
  
}
