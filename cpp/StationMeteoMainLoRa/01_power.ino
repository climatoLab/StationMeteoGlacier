//Initialise le tableau pour la mesure de la tension d'entrée
void init_VinExt() {
  //Lecture de la tension d'entrée dans le domaine 0-2.5Volts
  RawData_VinExt = analogRead(PIN_BATTERY);

  //Applique un polynome de degré 4 pour corriger la "non-linéarité" du ADC
  if (RawData_VinExt < 1 || RawData_VinExt > 4095) {
    Vin_VinExt = 0;
  }
  else {
    Vin_VinExt = coeff_A * pow(RawData_VinExt, 4) + coeff_B * pow(RawData_VinExt, 3) + coeff_C * pow(RawData_VinExt, 2) + coeff_D * RawData_VinExt + coeff_E;
  }

  //Ramène la tension dans le domaine 0-5Volts
  tensionReelle_VinExt = FACTEUR_CONV_VIN_EXT * moyenneVin_VinExt;

  //Rempli le tableau de la moyenen mobile avec la même valeur.
  //La valeur correspond à celle mesurée une seule fois tout juste avant
  //l'appel de la fonction "fill"
  movA_VinExt.fill(tensionReelle_VinExt);
}

//Fonction qui lit et qui filtre la tension d'entrée Vin externe (5 Volts)
void readVin() {
  //Lecture de la tension d'entrée dans le domaine 0-2.5Volts
  RawData_VinExt = analogRead(PIN_BATTERY);

  //Applique un polynome de degré 4 pour corriger la "non-linéarité" du ADC
  if (RawData_VinExt < 1 || RawData_VinExt > 4095) {
    Vin_VinExt = 0;
  }
  else {
    Vin_VinExt = coeff_A * pow(RawData_VinExt, 4) + coeff_B * pow(RawData_VinExt, 3) + coeff_C * pow(RawData_VinExt, 2) + coeff_D * RawData_VinExt + coeff_E;
  }
  movA_VinExt.add(Vin_VinExt);
  moyenneVin_VinExt = movA_VinExt.get();

  //Ramène la tension dans le domaine 0-5Volts
  tensionReelle_VinExt = FACTEUR_CONV_VIN_EXT * moyenneVin_VinExt;
  Vin = tensionReelle_VinExt;
  voltageStats.add(Vin);
}

// Délai non-bloquant (milliseconds)
// https://arduino.stackexchange.com/questions/12587/how-can-i-handle-the-millis-rollover
void myDelay(unsigned long ms)
{
  unsigned long start = millis();        // Start: timestamp
  for (;;)
  {                            
    unsigned long now = millis();        // Now: timestamp
    unsigned long elapsed = now - start; // Elapsed: duration
    if (elapsed >= ms)                   // Comparing durations: OK
      return;
  }
}
