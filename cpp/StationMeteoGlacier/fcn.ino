//*******************************************************************************************************************
//FONCTIONS POUR OBTENIR LES CREDENTIALS STOCKÉS DANS LA MÉMOIRE FLASH
//*******************************************************************************************************************
void init_Credentials(void) {
  preferences.begin("credentials", false);
  WIFI_SSID = preferences.getString("WIFI_SSID", "");
  WIFI_PASSWD = preferences.getString("WIFI_PASSWD", "");
  PRIVATE_URL = preferences.getString("PRIVATE_URL", "");
}

//*******************************************************************************************************************
//FONCTIONS POUR L'ACCELOROMETRE
//*******************************************************************************************************************
/*************************************************************************************************
   Programme permettant de connaître l'inclinaison de la station en X et Y.

   Nécessite les librairies Wire.h, Adafruit_Sensor.h et Adafruit_LSM303_Accel.h
   Accéléromètre utilisé: LSM303DHLC

*************************************************************************************************/
//==========================
//Initialise l'accéléromètre
//==========================
void init_Accelerometre() {
  //Initialise le capteur
  if (!accel.begin())       //initialise l'accel
  {
    //Il y a eu un problème de détection du ADXL345, vérifiez la connexion
    Serial.println("> Impossible de communiquer avec l'accéléromètre, verifiez le cablage !");
    while (1);
  }
  accel.setRange(LSM303_RANGE_2G);                    //2G signifie 2*9.8 m/s^2 = 19.8 m/s^2 => sensiblité max du capteur
  accel.setMode(LSM303_MODE_LOW_POWER);               //On met le capteur en mode faible consommation pour optimiser l'autonomie de la station
  Serial.println("> Accéléromètre initialisé correctement.");     //réponse au port série
}

//=========================================
//Mesure de l'accélération
//=========================================
void lecture_Accelerometre() {

  sensors_event_t event;
  accel.getEvent(&event);//On va chercher la mesure prise par le capteur en X,Y et Z

  // Mesure de l'accélération en X
  x = event.acceleration.x;
  //Serial.println(x);
  movA_accel_X.add(x);

  // Mesure de l'accélération en Y
  y = event.acceleration.y;
  //Serial.println(y);
  movA_accel_Y.add(y);

  // Mesure de l'accélération en Z
  z = event.acceleration.z;
  movA_accel_Z.add(z);

  //Convertit l'accélération en x et y en angles
  accel_to_deg();
}

//==================================
//Convertie l'accélération en degrée
//==================================
void accel_to_deg() {
  //Conversion de l'accélération en degrés à l'aide de formules trigonométriques

  moy_accel_X = movA_accel_X.get();
  moy_accel_Y = movA_accel_Y.get();

  //Ici, vérifie l'orientation donné en X et en Y en utilisant le rapport entre la valeur mesuré et
  // la valeur maximale de l'accélération gravitationnelle : https://www.theeminentcodfish.com/inclinometer/

  if (((moy_accel_X / xAmp) <= 1.0) && ((moy_accel_X / xAmp) >= -1.0)) {
    xAng = asin(moy_accel_X / xAmp) * 180 / PI; //Arduino donne les résulats en radians
    // On ajoute le facteur 180/PI pour avoir une mesure en degrés
  }
  else {
    xAng = 90.0;//L'axe X du capteur est perpendiculaire à la surface du sol
    //quand le capteur mesure l'accélération maximale
  }

  if ((( moy_accel_Y / yAmp) <= 1.0) && ((moy_accel_Y / yAmp) >= -1.0)) {
    yAng = asin(moy_accel_Y / yAmp) * 180 / PI; //Arduino donne les résulats en radians
    // On ajoute le facteur 180/PI pour avoir une mesure en degrés
  }
  else {
    yAng = 90.0;//L'axe Y du capteur est perpendiculaire à la surface du sol
    //quand le capteur mesure l'accélération maximale
  }

  if (xAng < 2 && xAng > -2) {
    xAng = 0;
  }
  if (yAng < 2 && yAng > -2) {
    yAng = 0;
  }
}

//Cette boucle permet d'avoir des messages d'avertissements en se basant sur une position
//indésirable du capteur
//On prend pour acquis que le capteur est parallèle à la surface du sol.
//void avertissement() {
//  if (moy_accel_Z <= 0.0) {
//    Serial.println(" La station météo est tombée ! Remettez-la à sa position initiale.");
//    delay(100);
//  }
//  else if ((moy_accel_X > 0.0 ) && (moy_accel_X < 9.806)) {
//    Serial.println(" La station météo est inclinée vers l'avant.");
//    delay(100);
//
//  }
//  else if ((moy_accel_X > -9.806) && (moy_accel_X < 0)) {
//    Serial.println(" La station météo est inclinée vers l'arrière.");
//    delay(100);
//  }
//  if (moy_accel_Y <= -9.806) {
//    Serial.println("La station météo est tombée ! Remettez-la à sa position initiale.");
//    delay(100);
//  }
//  else if ((moy_accel_Y < 0.0) && (moy_accel_Y > -9.806)) {
//    Serial.println(" La station météo est inclinée vers la gauche.");
//    delay(100);
//  }
//
//  else if ((moy_accel_Y > 0) && (moy_accel_Y < 9.806)) {
//    Serial.println("La station météo est inclinée vers la droite. ");
//    delay(100);
//  }
//  else {
//    Serial.println("");
//  }
//}

//*******************************************************************************************************************
//FONCTIONS POUR L'ANEMOMETRE
//*******************************************************************************************************************
/*************************************************************************************************
   Programme permettant d'utiliser l'anémomètre lors d'interruptions. L'anémomètre mesure la
   vitesse du vent en fermant un interrupteur chaque fois qu'un aimant passe devant le contact.
   Une vitesse de vent de 2.4 km/h (1.492 mph) est obtenue quand l'interrupteur ferme une fois
   par seconde.

   Anémomètre utilisé dans le kit: SEN-15901


   Connexions sur la Firebeetle:

   Module <-----------> Arduino
   VCC                  3V3
   GND                  GND
   Data                 IO35

*************************************************************************************************/

//=======================
//Initialise l'anénomètre
//=======================
void init_Anemometre(void) {
  pinMode(BROCHE_ANEMOMETRE, INPUT);    //set broche en imput
  //attachInterrupt(digitalPinToInterrupt(BROCHE_ANEMOMETRE), IRQ_anemometre, RISING);    //set l'interruption à la broche anemometre
  attachInterrupt(digitalPinToInterrupt(BROCHE_ANEMOMETRE), isr_rotation, FALLING);//Interruption de l'anémomètre
  Serial.println("> Anémomètre initialisé correctement.");      //reponse au port serie
}

//================================
//Fonction pour la lecture de vent
//================================
void lecture_Anemometre(void) {
  //  //Si l'anémomètre n'as pas tournée depuis 5 secondes, on assume qu'elle ne tourne plus :
  //  if (millis() - tempsDernierIRQ_Vent <= 5000) {
  //    //vitesse = vitesseVent;  //Ancienne ligne de code
  //    //Lit la valeur moyenne de la vitesse du vent à partir de la moyenne mobile
  //    vitesse = movA_vitVent.get();
  //  }
  //  else {
  //    //Dans le cas où elle ne tourne plus, on affiche une vitesse de 0 km/h
  //    vitesse = 0;
  //  }
  //  sei(); // Enables interrupts
  //  //delay (3000); // Wait 3 seconds to average
  //  cli(); // Disable interrupts
  //
  //  // convert to mp/h using the formula V=P(2.25/T)
  //  // V = P(2.25/3) = P * 0.75
  //
  //  WindSpeed = Rotations * 0.75;
  //
  //  //  Serial.print(Rotations); Serial.print("\t\t");
  //  //  Serial.println(WindSpeed);
  //
  //  Rotations = 0; // Set Rotations count to 0 ready for calculations

  if (millis() - lastInterrupt > 5000)
  {
    rps = 0;
  }
  else {
    rps = 1000. / deltaTime;
  }

  WindSpeed = rps * 3.66;
}

//*******************************************************************************************************************
//FONCTIONS POUR LA MESURE DE LA TENSION VIN
//*******************************************************************************************************************
/*************************************************************************************************
   Programme permettant de connaître la tension en entrée Vin (Vin externe de 5Volts). Ici, la broche 39 est
   utilisée pour connaître cette tension.
*************************************************************************************************/

//===========================================================
//Initialise le tableau pour la mesure de la tension d'entrée
//===========================================================
void init_VinExt() {
  //Lecture de la tension d'entrée dans le domaine 0-2.5Volts
  RawData_VinExt = analogRead(BROCHE_VIN_EXT);

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

//=================================================================
//Fonction qui lit et qui filtre la tension d'entrée Vin externe (5 Volts)
//=================================================================
void lecture_VinExt() {
  //Lecture de la tension d'entrée dans le domaine 0-2.5Volts
  RawData_VinExt = analogRead(BROCHE_VIN_EXT);

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
}


//*******************************************************************************************************************
//FONCTIONS POUR LA MESURE DE LA TENSION VIN DU PANNEAU SOLAIRE
//*******************************************************************************************************************
/*************************************************************************************************
   Programme permettant de connaître la tension en entrée du panneau solaire. Ici, la broche 36 est
   utilisée pour connaître cette tension.
*************************************************************************************************/

//===========================================================
//Initialise le tableau pour la mesure de la tension d'entrée
//===========================================================
void init_VinSol() {
  //Lecture de la tension d'entrée dans le domaine 0-2.5Volts
  RawData_VinSol = analogRead(BROCHE_VIN_SOL);

  //Applique un polynome de degré 4 pour corriger la "non-linéarité" du ADC
  if (RawData_VinSol < 1 || RawData_VinSol > 4095) {
    Vin_VinSol = 0;
  }
  else {
    Vin_VinSol = coeff_A * pow(RawData_VinSol, 4) + coeff_B * pow(RawData_VinSol, 3) + coeff_C * pow(RawData_VinSol, 2) + coeff_D * RawData_VinSol + coeff_E;
  }

  //Ramène la tension dans le domaine 0-5Volts
  tensionReelle_VinSol = FACTEUR_CONV_VIN_SOL * moyenneVin_VinSol;

  //Rempli le tableau de la moyenen mobile avec la même valeur.
  //La valeur correspond à celle mesurée une seule fois tout juste avant
  //l'appel de la fonction "fill"
  movA_Vin_sol.fill(tensionReelle_VinSol);
}

//=================================================================
//Fonction qui lit et qui filtre la tension d'entrée Vin solaire (5 Volts)
//=================================================================
void lecture_VinSol() {
  //Lecture de la tension d'entrée dans le domaine 0-2.5Volts
  RawData_VinSol = analogRead(BROCHE_VIN_SOL);

  //Applique un polynome de degré 4 pour corriger la "non-linéarité" du ADC
  if (RawData_VinSol < 1 || RawData_VinSol > 4095) {
    Vin_VinSol = 0;
  }
  else {
    Vin_VinSol = coeff_A * pow(RawData_VinSol, 4) + coeff_B * pow(RawData_VinSol, 3) + coeff_C * pow(RawData_VinSol, 2) + coeff_D * RawData_VinSol + coeff_E;
  }
  movA_Vin_sol.add(Vin_VinSol);
  moyenneVin_VinSol = movA_Vin_sol.get();

  //Ramène la tension dans le domaine 0-5Volts
  tensionReelle_VinSol = FACTEUR_CONV_VIN_SOL * moyenneVin_VinSol;
}

//*******************************************************************************************************************
//FONCTIONS POUR LA MESURE DE LA DISTANCE DE LA NEIGE
//*******************************************************************************************************************
/*************************************************************************************************
   Programme permettant de connaître la distance entre un objet et le capteur. Ce programme permet
   de prendre les mesures et de leurs appliquer un filtre.

   Nécessite la librairie SparkFun_VL53L1X.h
   Capteur de distance utilisé: VL53L1X


    Connexions sur la Firebeetle:

    Module <-----------> Arduino
    VCC                  3V3
    GND                  GND
    SCL                  IO22
    SDA                  IO21

*************************************************************************************************/

//===============================================
//Initialise le module de distance et son tableau
//===============================================
void init_Distance() {
  if (distanceSensor.begin() != 0)
    //Retourne 0 sur une initialisation réussi
  {
    Serial.println("> Impossible de communiquer avec le capteur de distance, verifiez le câblage !");
    while (1);
  }
  distanceSensor.setDistanceModeShort();
  Serial.println("> Capteur de distance (VL53L1X) initialisé correctement.");
}

//=================================
//Fonction pour mesurer la distance
//=================================
void lecture_Distance() {
  distanceSensor.startRanging();                //Écrit un bit de configuration pour initialiser les mesures
  //AJOUTER PROTECTION CI-DESSOUS POUR ÉVITER DE TOMBER DANS UN DEAD END S'IL Y A UN BUG !!!!!!!!!!
  while (!distanceSensor.checkForDataReady())
  {
    delay(1);
  }
  int distance = distanceSensor.getDistance();  //Obtient le résultat de la mesure du capteur
  distanceSensor.clearInterrupt();
  distanceSensor.stopRanging();
  movA_Distance.add(distance);
  moyenne_Distance = movA_Distance.get();
  //calcul_Distance(distance);                    //Appel de la fonction pour la moyenne
}

//*******************************************************************************************************************
//FONCTIONS POUR LE GPS
//*******************************************************************************************************************
/*************************************************************************************************
   Programme permettant de tester le GPS de la station météo.
   Communication Série (UART)

   Nécessite la librarie Adafruit_GPS.h
   Module GPS utilisé: Ultimate GPS Breakout V3 de Adafruit


    Connexions sur la Firebeetle:

    Module <-----------> Arduino
    Vin                  3V3
    GND                  GND
    RX                   IO17
    TX                   IO16

*************************************************************************************************/

//========================
//Initialise le module GPS
//========================
void init_GPS() {
  Serial1.begin(9600, SERIAL_8N1, BROCHE_RX_GPS, BROCHE_TX_GPS);
  Serial.println("> Initialisation du module GPS...");
  //9600 NMEA est le baud rate par défaut pour Adafruit MTK GPS
  //GPS.begin(9600);                                        //Connecter la pin RX à la pin 16 et la pin TX à la pin 17 du Firebeetle
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);           //Décommentez cette ligne pour allumer RMC et GGA
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);        //Décommentez cette ligne pour allumer seulement le RMC
  //Ajuste l'update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);              // 1 Hz update rate
  GPS.sendCommand(PGCMD_ANTENNA);                         //Demande l'état de l'antène. Commentez pour garder silence.
  delay(1000);
  GPSSerial.println(PMTK_Q_RELEASE);                      //Demande la version du firmware
  Serial.println("> Module GPS initialisé correctement.");
}

//=============================
//Lit la position de la station
//=============================
void lecture_GPS() {
  //Lit les données venant du GPS
  char c = GPS.read();
  //Cette partie est pour le débuggage
  if (GPSECHO)
    if (c) Serial.print(c);
  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences!
    // so be very wary if using OUTPUT_ALLDATA and trying to print out data
    Serial.print(GPS.lastNMEA()); // this also sets the newNMEAreceived() flag to false
    if (!GPS.parse(GPS.lastNMEA())) // this also sets the newNMEAreceived() flag to false
      return; // we can fail to parse a sentence in which case we should just wait for another
  }
  Serial.print("\nTime: ");
  if (GPS.hour < 10) {
    Serial.print('0');
  }
  Serial.print(GPS.hour, DEC); Serial.print(':');
  if (GPS.minute < 10) {
    Serial.print('0');
  }
  Serial.print(GPS.minute, DEC); Serial.print(':');
  if (GPS.seconds < 10) {
    Serial.print('0');
  }
  Serial.print(GPS.seconds, DEC); Serial.print('.');
  if (GPS.milliseconds < 10) {
    Serial.print("00");
  } else if (GPS.milliseconds > 9 && GPS.milliseconds < 100) {
    Serial.print("0");
  }
  Serial.println(GPS.milliseconds);
  Serial.print("Date: ");
  Serial.print(GPS.day, DEC); Serial.print('/');
  Serial.print(GPS.month, DEC); Serial.print("/20");
  Serial.println(GPS.year, DEC);
  Serial.print("Fix: "); Serial.print((int)GPS.fix);
  Serial.print(" quality: "); Serial.println((int)GPS.fixquality);
  if (GPS.fix) {
    Serial.print("Location: ");
    Serial.print(GPS.latitude, 4); Serial.print(GPS.lat);
    Serial.print(", ");
    Serial.print(GPS.longitude, 4); Serial.println(GPS.lon);
    Serial.print("Speed (knots): "); Serial.println(GPS.speed);
    Serial.print("Angle: "); Serial.println(GPS.angle);
    Serial.print("Altitude: "); Serial.println(GPS.altitude);
    Serial.print("Satellites: "); Serial.println((int)GPS.satellites);
  }
}

//*******************************************************************************************************************
//FONCTIONS POUR LA GIROUETTE
//*******************************************************************************************************************
/*************************************************************************************************
   Programme permettant de connaître la direction du vent selon la valeur de la résistance interne
   de la girouette. Chaque valeur de résistance représente un pôle sur la rose des vents. Une
   position entre deux pôles sera représenté comme les valeurs des résistances des pôles adjacents
   mis en parallèle. Voir datasheet.

   Girouette utilisée dans le kit: SEN-15901


   Connexions sur la Firebeetle:

   Module <-----------> Arduino
   VCC                  3V3
   GND                  GND
   Data                 IO34

*************************************************************************************************/

//=======================================
//Initialise le tableau pour la girouette
//=======================================
void init_Girouette() {
  //Met toutes les valeurs du tableau à 0 ... à vérifier !!!  VRAIMENT NÉCESSAIRE ?????
  //  for (int x = 0; x < 17; x++) {
  //    Degree[x] = x * 22.5;
  //  }
  RawData_girouette = analogRead(BROCHE_GIROUETTE);
  movA_Girouette.fill(RawData_girouette);
  Serial.println("> Girouette initialisé correctement.");
}

//=======================================================================================
//Girouette lecture du potentiomètre
//=======================================================================================
void lecture_Girouette(void) {
  RawData_girouette = analogRead(BROCHE_GIROUETTE);
  roseDESvents(RawData_girouette);
}

//========================================================================================
// Girouette Fonction rose des vents
//========================================================================================
// Cette fonction détermine les points cardinaux selon la valeur lu du potentiomètre
void roseDESvents(uint16_t lecturevaleur) {
  if (lecturevaleur >= 1550 && lecturevaleur < 2550) {
    //Serial.println("NE");
    orientation = "NE";
  }
  else if (lecturevaleur >= 2550 && lecturevaleur < 3050) {
    //Serial.println("E");
    orientation = "E";
  }
  else if (lecturevaleur >= 3050 && lecturevaleur < 3320) {
    //Serial.println("SE");
    orientation = "SE";
  }
  else if (lecturevaleur >= 3320 && lecturevaleur < 3495) {
    //Serial.println("S");
    orientation = "S";
  }
  else if (lecturevaleur >= 3495 && lecturevaleur < 3605) {
    //Serial.println("SO");
    orientation = "SO";
  }
  else if (lecturevaleur >= 3605 && lecturevaleur < 3700) {
    //Serial.println("O");
    orientation = "O";
  }
  else if (lecturevaleur >= 3700 && lecturevaleur < 3760) {
    //Serial.println("NO");
    orientation = "NO";
  }
  else {
    //Serial.println("N");
    orientation = "N";
  }
}

//*******************************************************************************************************************
//FONCTIONS POUR LA LECTURE DE L'HUMIDITÉ
//*******************************************************************************************************************
/**********************************************************************************************
    Programme permettant d'obtenir l'humidité relative et utilise une communication I2C.

    Nécessite la librarie DHT.h
    Capteur d'humidité utilisé: DHT22

    Connexions sur la Firebeetle:

    Module <-----------> Arduino
    VCC                  3V3
    GND                  GND
    SCL                  IO22
    SDA                  IO21

**********************************************************************************************/

//================================================
//Initialise la communication avec le module DHT22
//================================================
void init_Humidite() {
  dht.begin();
  humidite = dht.readHumidity();
  Serial.println("> Capteur d'humidite (DHT22) initialisé correctement.");
}

//=================================================
//Mesure et applique une correction au module DHT22
//=================================================
void lecture_Humidite() {
  humidite = dht.readHumidity();
}

//*******************************************************************************************************************
//FONCTIONS POUR LA LECTURE DE LA LUMIÈRE
//*******************************************************************************************************************
/**********************************************************************************************
    Programme permettant d'obtenir la luminosité et utilise une communication I2C.

    Nécessite les libraries Wire.h et MAX44009.h
    Capteur de luminosité utilisé: GY-49

    Connexions sur la Firebeetle:

    Module <-----------> Arduino
    VCC                  3V3
    GND                  GND
    SCL                  IO22
    SDA                  IO21

**********************************************************************************************/

//==========================
//Initialise le module GY-49
//==========================
void init_Luminosite() {
  lux = light.getLux();
  int err = light.getError();
  if (err != 0) {
    Serial.print("> Erreur de lecture du capteur de luminosité. Code d'erreur : ");
    Serial.println(err);
  }
  else {
    Serial.println("> Capteur de luminosité (GY-49) initialisé correctement.");
  }
}
//=================================================
//Mesure et applique une correction au module GY-49
//=================================================
void lecture_Luminosite() {
  float l = light.getLux();
  int err = light.getError();
  if (err == 0) {
    lux = l;
  }
}

//*******************************************************************************************************************
//FONCTIONS POUR LA LECTURE DE LA QUANTITÉ DE PLUIE TOMBÉE
//*******************************************************************************************************************
/*************************************************************************************************
   Programme permettant de connaître la quantité de pluie tombé durant la dernière heure et durant
   les 24 dernières heures

   Nécessite la bibliothèque PCF8583.h
   Pluviomètre utilisé dans le kit: SEN-15901, le pcf8583


   Connexions sur la Firebeetle:

   Module <-----------> Arduino
   VCC                  3V3
   GND                  GND
   Data                 IO35 ???????????????????? I2C !!!!

*************************************************************************************************/

//=========================
//Initialise le pluviomètre
//=========================
void init_Pluviometre() {
  counter.setMode(MODE_EVENT_COUNTER);
  Serial.println("> Pluviomètre initialisé correctement.");
}
//========================
//Mesure de la pluie tombé
//========================
void lecture_Pluie() {
  //  adjust_Heure();
  if (nb_Heure == 24) {                 //Remet l'heure à zéro lorsque 24h est passé
    nb_Heure = 0;
    pluie_24H = 0;
  }
  else if (quart_dHeure == 4) {         //Après 4 quart d'heure, "nb_heure" incrémente d'un
    pluie_H = 0;
    quart_dHeure = 0 ;
    nb_Heure++;
  }
  pluie = nb_Basculement * basculement_mm;
  counter.setCount(0);
  pluie_H = pluie + pluie_H;
  pluie_24H = pluie_24H + pluie;
}

//=====================================
//Ajuste l'heure pour le mode bluetooth
//=====================================
//void adjust_Heure() {
//  int Heures = 0;
//  if (heures == Heures) {
//    nb_Heure++ ;
//    pluie_H = 0;
//    Heures = heures + 1 ;
//  }
//}

//*******************************************************************************************************************
//FONCTIONS POUR LA LECTURE DE LA PRESSION ATMOSPHÉRIQUE
//*******************************************************************************************************************
/**********************************************************************************************
    Programme permettant d'obtenir la pression et utilise une commmunication I2C.

    Nécessite les libraries Wire.h, SPI.h, Adafruit_Sensor.h et Adafruit_BMP3XX.h
    Capteur de pression BMP388

    Connexions sur la Firebeetle:

    Module <-----------> Arduino
    VCC                  3V3
    GND                  GND
    SCL                  IO22
    SDA                  IO21

**********************************************************************************************/

//===========================
//Initialise le module BMP388
//===========================
boolean init_Pression(void) {
  if (!bmp.begin_I2C()) {
    Serial.println("> Impossible de communiquer avec le capteur de pression, verifiez le cablage !");
    //while (1);
    return false;
  }
  //Initialisation du filtre de l'échantillonnage
  bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
  bmp.setOutputDataRate(BMP3_ODR_50_HZ);
  pression = bmp.pressure / 1000.0;
  Serial.println("> Capteur de pression (BMP388) initialisé correctement.");
  return true;
}
//==================================================
//Mesure et applique une correction au module BMP388
//==================================================
void lecture_Pression(boolean etat) {
  if (etat == true) {
    if (! bmp.performReading()) {
      Serial.println("> N'a pas réussi à lire la valeur du capteur de pression !");
    }
    else {
      pression = bmp.pressure / 1000.0;
    }
  }
}

//*******************************************************************************************************************
//FONCTIONS POUR LA LECTURE DE L'HEURE
//*******************************************************************************************************************
/**********************************************************************************************
    Programme permettant de gérer l'heure et la date. Lorsque la batterie est retirée, il
    faut reprogrammer l'heure et la date en appuyant sur 'r' et entrer les informations dans
    le moniteur série.

    Nécessite la librarie RTClib.h
    Reel Time Clock utilisé: DS3231

    Connexions sur la Firebeetle:

    Module <-----------> Arduino
    VCC                  3V3
    GND                  GND
    SCL                  IO22
    SDA                  IO21

**********************************************************************************************/
//===========================
//Initialise le module DS3231
//===========================
void init_RTC(void) {
  myRTC.begin();
  Serial.println("> Module RTC (DS3231) initialisé correctement.");
  if (! myRTC.begin()) {
    Serial.println("> Impossible de communiquer avec le RTC, verifiez le cablage !");
    Serial.flush();
    abort();
  }
}
//=================================================
//Lit et ajuste les valeurs du RTC en stantard "00"
//=================================================
void lecture_RTC() {
  DateTime now = myRTC.now();
  ANNEE = now.year();
  mois = now.month();
  jour = now.day();
  heures = now.hour();
  minutes = now.minute();
  secondes = now.second();
  if (mois < 10) {
    MOIS = "0" + String(mois);
  } else  {
    MOIS = String(mois);
  }
  if (jour < 10) {
    JOURS = "0" + String(jour);
  } else {
    JOURS = String(jour);
  } if (heures < 10) {
    HEURES = "0" + String(heures);
  } else {
    HEURES = String(heures);
  } if (minutes < 10) {
    MINUTES = "0" + String(minutes);
  } else {
    MINUTES = String(minutes);
  }
  if (secondes < 10) {
    SECONDES = "0" + String(secondes);
  } else {
    SECONDES = String(secondes);
  }

  DATE = String(ANNEE + "/" + MOIS + "/" + JOURS);
  TIME = String(HEURES + ":" + MINUTES + ":" + SECONDES);
}

//*******************************************************************************************************************
//FONCTIONS POUR LA LECTURE ET L'ÉCRITURE SUR LA CARTE SD
//*******************************************************************************************************************
/**********************************************************************************************
    Programme permettant de gérer la carte SD

    Nécessite les libraries FS.h et SD.h
    Module SD utilisé:

    Connexions sur la Firebeetle:

    Module <-----------> Arduino
    VCC                  3V3
    GND                  GND
    SCL                  IO22
    SDA                  IO21

**********************************************************************************************/

//============================================
//Initialise la communication avec la carte SD
//============================================
void init_SD() {
  SD.begin(BROCHE_SD);
  if (!SD.begin(BROCHE_SD)) {
    Serial.println("> Échec du \"montage\" de la carte SD !");
    return;
  }
  //Lecture du type de carte SD
  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("> Incapable de détecter le type de carte !");
    return;
  }
  //Initialisation de la carte SD
  if (!SD.begin(BROCHE_SD)) {
    Serial.println("> Erreur - Incapable d'initialiser la carte SD !");
    return;  //Échec de l'initialisation
  }
  Serial.println("> Carte SD initialisée correctement.");
  //Ouverture du fichier donnee.txt
  File file = SD.open("/donnee.txt");
  //Si le fichier donnee.txt n'existe pas sur la carte SD
  if (!file) {
    Serial.println(">Le fichier donnee.txt n'existe pas !");
    Serial.println(">Écriture du nom des colonnes dans le fichier donnee.txt ...");
    //Écriture du nom de chaque colonne pour le "logging" des données
    writeFile(SD, "/donnee.txt", labelsDonnees_SD.c_str(), false);
  }
  else {
    //Serial.println(">Le fichier donnee.txt existe déjà !");
    //Ferme le fichier
    file.close();
  }
}
//=======================================
//vient ecrire les donnée sur la carte sd
//=======================================
void enregistreDonneeCarteSD() {
  //Formatte la chaîne de texte à écrire sur la carte SD
  //  donnees_SD = DATE + ";" + TIME + ";" + String(temperature, 2) + \
  //               ";" + String(humidite, 2) + ";" + String(pression, 2) + ";" + \
  //               String(lux, 2) + ";" + String(vitesse, 2) + ";" + String(degree, 1) + \
  //               ";" + String(pluie_H, 4) + ";" + String(pluie_24H, 4) + ";" + \
  //               String(moyenne_Distance) + ";" + String(xAng, 2) + ";" + String(yAng, 2) + \
  //               ";" + String(flat, 6) + ";" + String(flon, 6) + ";" + String(alt, 3) + \
  //               ";" + String(tensionReelle_VinExt , 2) + "\r\n";

  donnees_SD = DATE + ";" + TIME + ";" + String(humidite, 2) + ";" + \
               String(lux, 2) + ";" + String(pression, 2) + ";" + \
               String(temperature, 2) + ";" + String(tensionReelle_VinExt , 2) + \
               ";" + String(tensionReelle_VinSol , 2) + ";" + String(WindSpeed, 2) + \
               ";" + orientation + "\r\n";

  //Affiche dans le moniteur série ce qui va être écrit sur la carte SD :
  Serial.println("Chaîne de texte à écrire sur la carte SD : "); Serial.println(donnees_SD);
  //Écriture des données sur la carte SD :
  writeFile(SD, "/donnee.txt", donnees_SD.c_str(), true);
}
//==================================
//fonction qui ecrit sur la carte SD
//==================================
void writeFile(fs::FS & fs, const char * path, const char * message, boolean msg) {
  File file;
  if (msg) {
    //Ouverture du fichier en écriture en mode AJOUT ! :
    Serial.printf("Écriture des données des capteurs dans le fichier: %s\n", path);
    file = fs.open(path, FILE_APPEND);
    //En cas d'erreur :
    if (!file) {
      Serial.println("Incapable d'ouvrir le fichier pour ajouter les données des capteurs !");
      return;
    }
    //Lorsqu'il n'y a pas d'erreur :
    else {
      if (file.print(message))
        Serial.println("Les données des capteurs ont été ajoutées avec succès!");
      else
        Serial.println("Incapable d'ajouter les données des capteurs dans le fichier !");
    }
  }
  //Ouverture du fichier en écriture (depuis le début)uniquement
  else {
    Serial.printf("Écriture du nom de chaque colonne dans le fichier : %s\n", path);
    file = fs.open(path, FILE_WRITE);
    //En cas d'erreur :
    if (!file) {
      Serial.println("Incapable d'ouvrir le fichier pour ajouter le nom des colonne !");
      return;
    }
    //Lorsqu'il n'y a pas d'erreur :
    else {
      if (file.print(message))
        Serial.println("Écriture du nom des colonnes réussie avec succès !");
      else
        Serial.println("L'écriture du nom des colonnes dans le fichier a échouée !");
    }
  }
  //Fermeture du fichier
  file.close();
}

//*******************************************************************************************************************
//FONCTIONS POUR LE MODULE SATELLITE
//*******************************************************************************************************************
/*myRTC.begin void start_Sat() {
  while (!Serial);

  // Start the serial port connected to the satellite modem
  IridiumSerial.begin(19200);
  Serial.println("Starting modem...");
  err = modem.begin();

  err = modem.getSignalQuality(signalQuality);
  Serial.println("SIGNAL = " + String(signalQuality));
  }

  void sat_send(char message) {
  if (signalQuality >= 2) {
    // Send the message
    Serial.print("Trying to send a message.  This might take several minutes.\r\n");
    err = modem.sendSBDText(message);
    if (err != ISBD_SUCCESS)
    {
      Serial.print("sendSBDText failed: error ");
      Serial.println(err);
      if (err == ISBD_SENDRECEIVE_TIMEOUT)
        Serial.println("Try again with a better view of the sky.");
    }
    Serial.println("Hey, it worked!");
    Serial.println("Putting modem to sleep. message sent");
    err = modem.sleep();
  } else {
    // Put modem to sleep
    Serial.println("Putting modem to sleep. signal weak");
    err = modem.sleep();
    if (err != ISBD_SUCCESS)
    {
      Serial.print("sleep failed: error ");
      Serial.println(err);
    }
  }
  }
*/

//*******************************************************************************************************************
//FONCTIONS POUR LE THERMOCOUPLE
//*******************************************************************************************************************
/**********************************************************************************************
    Programme permettant de mesurer la température. Ici, en °C. Utilise la communication SPI

    Nécessite les librairies SPI.h et Adafruit_MAX31855.h
    Capteur de température utilisé: Type K, PmodTC1

    Connexions sur la Firebeetle:

    Module <-----------> Arduino
    VCC                  3V3
    GND                  GND
    CS                   IO14
    SCK                  IO18
    MISO                 IO19

   Un convertisseur de niveau logique doit être utilisé!

**********************************************************************************************/

//=================================================
//Mesure et applique une correction au thermocouple
//=================================================
void lecture_Temperature() {
  //double bidon = thermocouple.readCelsius(); //Patch pour corriger problème de première lecture erronnée
  //temperature = thermocouple.readCelsius();
  temperature = bmp.temperature;
  if (isnan(temperature)) {
    Serial.println("> Problème rencontré avec le thermocouple !");
  }
}

//void init_temperature() {
//  //double bidon = thermocouple.readCelsius(); //Patch pour corriger problème de première lecture erronnée
//  temperature = thermocouple.readCelsius();
//  if (isnan(bidon)) {
//    Serial.println("> Problème rencontré avec le thermocouple !");
//  }
//}

//*******************************************************************************************************************
//FONCTIONS POUR LA TRANSMISSION DE DONNÉES VIA LE WIFI
//*******************************************************************************************************************
/*************************************************************************************************
   Programme permettant d'envoyer les mesures prises vers un serveur WEB grâce à la carte ESP32
   Firebeetle. Ces données seront formattées en JSON.

   7 mesures sont envoyées à chaque fois: Température, Humidité, Vitesse, Direction, Pression,
   Pluie à toutes les heures et Pluie à toutes les 24 heures.

   Site utilisé: https://technophys.herokuapp.com/

   Nécessite les librairies ArduinoJson.h, WiFi.h, HTTPClient.h et credentials.h


   NOTE: Lorsque le message d'erreur est -11, ça semble venir du DHT22 (capteur d'humidité) qui
   ne fonctionne pas.
   Lorsqu'une valeur ne s'affiche pas sur herokuapp mais vous savez que vous l'avez envoyée,
   il faut envoyer une valeur à ce capteur par Postman. Ainsi, toutes les anciences mesures
   s'afficheront.

*************************************************************************************************/
//=============================
//Initialise la connection Wifi
//=============================
void init_Wifi() {
  int wifiRetryCntr = 0;  //Compteur du nombre d'essais de connection Wifi
  bool etatwifi = true;   //Variable pour savoir si on peut se connecter au réseau

  if (WIFI_SSID == "" || WIFI_PASSWD == "") {
    Serial.println("> Erreur, aucun SSID et mot de passe pour accéder au point d'accès WiFi !");
  }
  else {
    //Branchement au point d'accès WiFi
    WiFi.begin(WIFI_SSID.c_str(), WIFI_PASSWD.c_str());
    Serial.print("> Branchement en cours avec le réseau : "); Serial.println(WIFI_SSID);
    while (WiFi.status() != WL_CONNECTED) {
      Serial.print('.');
      delay(500);
      wifiRetryCntr++;
      // Après un certains nombres de tentatives, on quitte la boucle de connection Wifi
      if (wifiRetryCntr >= WIFI_RETRY_CNTR) {
        etatwifi = false;
        break;
      }
    }
    // si la connection à eu lieu on l'indique
    if (etatwifi == true) {
      Serial.println();
      Serial.println("> Branchement réussi avec le point d'accès WiFi.");
    }
    // si la connection n'a pas eu lieu on l'indique
    else {
      Serial.println();
      Serial.println("> Échec de la connexion avec le point d'accès WiFi.");
    }
  }
}

//=======================================
//Transmission des données au serveur TGP
//=======================================
void postServeur(void) {
  //============================
  //Création de la chaîne JSON :
  //============================
  DynamicJsonDocument doc(capacity);
  JsonObject doc_0 = doc.createNestedObject();
  doc_0["sensor"] = "Temperature";
  doc_0["value"] = String(temperature, 1);

  JsonObject doc_1 = doc.createNestedObject();
  doc_1["sensor"] = "Humidite";
  doc_1["value"] = String(humidite, 1);

  JsonObject doc_2 = doc.createNestedObject();
  doc_2["sensor"] = "WindSpeed";
  doc_2["value"] = String(WindSpeed, 1);

  JsonObject doc_3 = doc.createNestedObject();
  doc_3["sensor"] = "Direction";
  doc_3["value"] = String(degree, 1);

  JsonObject doc_4 = doc.createNestedObject();
  doc_4["sensor"] = "Pression";
  doc_4["value"] = String(pression, 1);

  JsonObject doc_5 = doc.createNestedObject();
  doc_5["sensor"] = "Pluie_H";
  doc_5["value"] = String(pluie_H, 1);

  JsonObject doc_6 = doc.createNestedObject();
  doc_6["sensor"] = "Pluie_24H";
  doc_6["value"] = String(pluie_24H, 1);

  //Transmet la chaîne JSON dans le moniteur Série
  serializeJson(doc, Serial);

  //===========================================
  //Transmisison au serveur de la chaîne JSON :
  //===========================================
  if (WIFI_SSID == "" || WIFI_PASSWD == "" || PRIVATE_URL == "") {
    Serial.println("> Erreur, aucun SSID, mot de passe ou URL privé pour accéder au serveur !");
  }
  else {
    //Si le branchement avec le point d'accès WiFi est valide
    if (WiFi.status() == WL_CONNECTED)
    {
      //Crée un objet http du type (classe) HTTPClient
      HTTPClient http;
      //==================================HEADER==================================
      //Indique la destination de la requête HTTP :
      //Spécifier l'URL du serveur ci-dessous :
      http.begin(PRIVATE_URL);
      //Spécifie l'agent ESP32 dans l'entête de la requête :
      http.addHeader("User-Agent", "ESP32HTTPClient");
      //Spécifie le contenu de l'entête de la requête (nous transmettons du JSON):
      http.addHeader("Content-Type", "application/json");
      //===========================================================================

      //========================BODY========================
      //Transmet la chaîne JSON dans le body de la requête :
      String requestBody;
      serializeJson(doc, requestBody);
      int httpResponseCode = http.POST(requestBody);
      //===================================================

      //Vérifie la réponse du serveur :
      if (httpResponseCode > 0) {
        //Affiche le code de retour :
        Serial.print("> Code reçu du serveur : "); Serial.println(httpResponseCode);
        //Affiche le body transmis par le serveur :
        String response = http.getString();
        Serial.print("> Body du serveur : "); Serial.println(response);
      }
      else {
        Serial.println("> Erreur !");
        Serial.print("> Code reçu du serveur : "); Serial.println(httpResponseCode);
      }
      //Libère les ressources HTTP du ESP32
      http.end();
    }
    //Si le branchement avec le point d'accès WiFi n'est pas valide
    else {
      Serial.println("> Erreur de connexion avec le point d'accès WiFi");
    }


  }
}

//========================================================
//Affiche la mesure de chacun de capteur sur le port serie
//========================================================
void valCapteursPortSerie() {
  Serial.println("> ====================================================================");
  Serial.println("> Date.............................................. " + String(DATE));
  Serial.println("> Heure............................................. " + String(TIME));
  Serial.println("> Température....................................... " + String(temperature, 2) + " °C");
  Serial.println("> Humidité.......................................... " + String(humidite, 2) + " %");
  Serial.println("> Pression.......................................... " + String(pression, 2) + " kPa");
  Serial.println("> Intensité lumineuse............................... " + String(lux, 2) + " lux");
  Serial.println("> Angle du vent..................................... " + String(degree, 1) + "°");
  Serial.println("> Direction du vent................................. " + String(orientation));
  Serial.println("> WindSpeed du vent................................... " + String(WindSpeed) + " Km/h");
  //Serial.println("> Quantité de pluie durant la dernière heure........ " + String(pluie_H, 4) + " mm");
  //Serial.println("> Quantité de pluie durant les 24 dernières heures.. " + String(pluie_24H, 4) + " mm");
  //Serial.println("> Altitude.......................................... " + String(alt, 2) + "m");
  //Serial.println("> Latitude.......................................... " + String(flat, 6) + "°");
  //Serial.println("> Longitude......................................... " + String(flon, 6) + "°");
  //Serial.println("> Distance du glacier............................... " + String(moyenne_Distance) + " mm");
  //Serial.println("> Angle d'inclinaison en X.......................... " + String(xAng, 2) + "°");
  //Serial.println("> Angle d'inclinaison en Y.......................... " + String(yAng, 2) + "°");
  Serial.println("> Tension d'entrée Vin externe...................... " + String(tensionReelle_VinExt , 2) + " Volts");
  Serial.println("> Tension d'entrée Vin solaire...................... " + String(tensionReelle_VinSol , 2) + " Volts");
  Serial.println("> =====================================================================");
}

//===================================================
//Décode le port Série lorsque la station est branché
//===================================================
void decodeurCMD() {
  if (monDecodeur.available()) {
    commande = monDecodeur.getCommand();
    switch (commande) {
      //Lecture de la tension d'entrée Vin externe de la station
      case'b':
        Serial.println("> =========================================");
        Serial.println("> Tension d'entrée Vin externe : " + String(tensionReelle_VinExt , 2) + " Volts");
        Serial.println("> =========================================");
        break;

      //Lecture de la tension d'entrée Vin solaire de la station
      case'B':
        Serial.println("> =========================================");
        Serial.println("> Tension d'entrée Vin solaire : " + String(tensionReelle_VinSol , 2) + " Volts");
        Serial.println("> =========================================");
        break;

      //Désactive [0] /active [1] l'affichage continu
      case'c':
        if (monDecodeur.lireArgument(0) == 1) {
          modeContinu = true;
          Serial.println("> Affichage continu Activé.");
        }
        else {
          modeContinu = false;
          Serial.println("> Affichage continu Désactivé.");
        }
        break;

      //Indique la distance entre la staton et et le glacier
      case'd':
        Serial.println("> =========================================");
        Serial.println("> Distance entre la station et le glacier : " + String(moyenne_Distance) + " mm");
        Serial.println("> =========================================");
        break;

      //Indique la position de la station (GPS)
      case'g' :
        Serial.println("> =========================================");
        Serial.println("> Altitude  : " + String(alt, 3) + " m.");
        Serial.println("> Latitude  : " + String(flat, 6) + "°;");
        Serial.println("> Longitude : " + String(flon, 6) + "°;");
        Serial.println("> =========================================");
        break;

      //Indique l'humidité relative en %H.R.
      case'h':
        Serial.println("> =========================================");
        Serial.println("> Humidité : " + String(humidite, 2) + " %");
        Serial.println("> =========================================");
        break;

      //Indique l'angle d'inclinaison de la station
      case'i':
        Serial.println("> =========================================");
        Serial.println("> Angle d'inclinaison en X : " + String(xAng, 2) + ";");
        Serial.println("> Angle d'inclinaison en Y : " + String(yAng, 2) + ";");
        Serial.println("> =========================================");
        break;

      //Indique l'intensité lumineuse ne LUX
      case'l':
        Serial.println("> =========================================");
        Serial.println("> Intensité lumineuse : " + String(lux, 2) + " lux");
        Serial.println("> =========================================");
        break;

      //Affiche ce menu
      case'm' :
        menu();
        break;

      //Indique la quantité de pluie tombée en mm
      case'p':
        Serial.println("> ========================================================");
        Serial.println("> Quantité de pluie durant la dernière heure       : " + String(pluie_H, 4) + " mm;");
        Serial.println("> Qauntité de pluie durant les 24 dernières heures : " + String(pluie_24H, 4) + " mm.");
        Serial.println("> ========================================================");
        break;

      //Indique la pression atmosphérique en kPa
      case'P':
        Serial.println("> =========================================");
        Serial.println("> Pression : " + String(pression, 2) + " kPa");
        Serial.println("> =========================================");
        break;

      //Affiche la valeur de chaque capteur de la station
      case 'q' :
        valCapteursPortSerie();
        break;

      //Affiche la date et l'heure
      case'r':
        Serial.println("> =========================================");
        Serial.println("> Date  : " + String(DATE) + ";");
        Serial.println("> Heure : " + String(TIME) + ";");
        Serial.println("> =========================================");
        break;

      //Ajuste la date et l'heure du RTC
      case'R':
        A = monDecodeur.lireArgument(0);
        M = monDecodeur.lireArgument(1);
        J = monDecodeur.lireArgument(2);
        H = monDecodeur.lireArgument(3);
        m = monDecodeur.lireArgument(4);
        S = monDecodeur.lireArgument(5);
        myRTC.adjust(DateTime(A, M, J, H, m, S));
        Serial.println("> Date et heure modifié avec succès!");
        break;

      //Enregistre les mesures sur la carte micro SD
      case's':
        enregistreDonneeCarteSD();
        break;

      //Affiche l'état (true/false) du sélecteur de mode
      case'S':
        Serial.print("> Selecteur de mode = "); Serial.println(etat_sel_mode);
        break;

      //Affiche la température en *C
      case't':
        Serial.println("> =========================================");
        Serial.println("> Température : " + String(temperature, 2) + " °C");
        Serial.println("> =========================================");
        break;

      //Affiche la vitesse, la direction et l'angle du vent
      case'v':
        Serial.println("> =========================================");
        Serial.println("> WindSpeed du vent   : " + String(WindSpeed) + " Km/h;");
        Serial.println("> Direction du vent : " + String(orientation)) + ";";
        Serial.println("> Angle du vent     : " + String(degree, 1) + "°.");
        Serial.println("> =========================================");
        break;

      //Transmet l'ensemble des mesures au serveur via Wifi
      case'w' :
        postServeur();
        break;

      //Pour modifier le mode de fonctionnement de la station
      case'z' :
        if (monDecodeur.lireArgument(0) == 1) {
          etat_sel_mode = true;
          Serial.println("> Mode deep sleep activé.");
        }
        else {
          etat_sel_mode = false;
          Serial.println("> Mode deep sleep désactivé.");
        }
        break;

      //Affiche l'état actuel mode Deep Sleep
      case'Z' :
        Serial.print("> Mode deep sleep = "); Serial.println(etat_sel_mode);
        break;

      //Quand la commande entrée est inconnue
      default:
        Serial.println("> Arguement non valide. Veuillez entrer une valeur dans les choix du menu.");
        break;
    }
  }
}
//============================================
//Menu des actions possibles sur le port serie
//============================================
void menu() {
  Serial.println();
  Serial.println("> ==========================================================================");
  Serial.println("> b.......................... Lecture de la tension d'entrée Vin externe de la station;");
  Serial.println("> B.......................... Lecture de la tension d'entrée Vin solaire de la station;");
  Serial.println("> c <0/1>.................... Désactive [0] /active [1] l'affichage continu;");
  Serial.println("> d.......................... Indique la distance entre la staton et et le glacier;");
  Serial.println("> g.......................... Indique la position de la station (GPS);");
  Serial.println("> h.......................... Indique l'humidité relative en %H.R.;");
  Serial.println("> i.......................... Indique l'angle d'inclinaison de la station;");
  Serial.println("> l.......................... Indique l'intensité lumineuse ne LUX;");
  Serial.println("> m.......................... Affiche ce menu;");
  Serial.println("> p.......................... Indique la quantité de pluie tombée en mm;");
  Serial.println("> P.......................... Indique la pression atmosphérique en kPa;");
  Serial.println("> q.......................... Affiche la valeur de chaque capteur de la station  ;");
  Serial.println("> r.......................... Affiche la date et l'heure  ;");
  Serial.println("> R <aa MM jj hh mm ss>...... Ajuste la date et l'heure du RTC;");
  Serial.println("> s.......................... Enregistre les mesures sur la carte micro SD;");
  Serial.println("> S.......................... Affiche l'état (true/false) du sélecteur de mode;");
  Serial.println("> t.......................... Affiche la température en *C;");
  Serial.println("> v.......................... Affiche la vitesse, la direction et l'angle du vent;");
  Serial.println("> w.......................... Transmet l'ensemble des mesures au serveur via Wifi.");
  Serial.println("> z <0/1 > .................. z = 0 (Deep sleep désactivé), z = 1 (Mode deep sleep activé)");
  Serial.println("> Z ......................... Affiche l'état du mode Deep Sleep");
  Serial.println("> ==========================================================================");
  Serial.println();
}

//============================================
//Fonction pour indiquer la source qui a permis
//au ESP32 de se réveiller du mode sommeil.
//============================================
void print_wakeup_reason() {
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch (wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("> Réveil causé par un signal externe utilisant RTC_IO."); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("> Réveil causé par un signal utilisant RTC_CNTL."); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("> Réveil causé par la minuterie."); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("> Réveil causé par le bouton sans contact."); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("> Réveil causé par le programme ULP."); break;
    default : Serial.printf("> La mise en route du ESP32 ne provient pas du réveil du sommeil profond. Code = %d\n", wakeup_reason); break;
  }
}

void _BlinkLed(int broche, int blink_nbr, int blink_time){
  for (int i = 0; i < blink_nbr; i++){
    digitalWrite(broche, HIGH);
    delay(blink_time);

    digitalWrite(broche, LOW);
    delay(blink_time);
  }
}

void init_rtc_gpio(gpio_num_t broche_rtc_gpio, bool direction_rtc_gpio){
  rtc_gpio_init(broche_rtc_gpio);
  if (direction_rtc_gpio == OUTPUT){
    rtc_gpio_set_direction(broche_rtc_gpio, RTC_GPIO_MODE_OUTPUT_ONLY);
    rtc_gpio_set_level(broche_rtc_gpio, LOW);
  }
  else{
     rtc_gpio_set_direction(broche_rtc_gpio, RTC_GPIO_MODE_INPUT_ONLY);
  }
}
