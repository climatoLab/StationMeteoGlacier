// ----------------------------------------------------------------------------
// Adafruit BMP388 Capteur Temperature Pression
// https://www.adafruit.com/product/3966
// ----------------------------------------------------------------------------
void configureBmp388()
{
  Serial.print("Info - Initializing BME388...");

  if (bmp.begin_I2C())
  {
    online.bmp388 = true;
    Serial.println("success!");
  }
  else
  {
    online.bmp388 = false;
    Serial.println("failed!");
  }
}

// Read BMP388
void readBmp388()
{
  // Start the loop timer
  unsigned long loopStartTime = millis();

  // Initialize sensor
  configureBmp388();

  // Check if sensor initialized successfully
  if (online.bmp388)
  {
    Serial.print("Info - Reading BMP388...");

    myDelay(250);

    bmpTemperatureC   = bmp.readTemperature();
    bmpPressionHPa    = bmp.readPressure() / 100;
    bmpAltitude       = bmp.readAltitude(SEALEVELPRESSURE_HPA);

    bmpTempStats.add(bmpTemperatureC);         
    bmpPresStats.add(bmpPressionHPa);          
    bmpAltStats.add(bmpAltitude);           
    
    Serial.println("done.");
  }
  else
  {
    Serial.println("Warning - BMP388 offline!");
  }
  // Stop the loop timer
  timer.readBmp = millis() - loopStartTime;
}

// ----------------------------------------------------------------------------
// DFRobot DHT22 Capteur Température Humidité
// https://www.dfrobot.com/product-1102.html
// ----------------------------------------------------------------------------
void configureDHT22()
{
  Serial.print("Info - Initializing DHT22...");
  
  dht.begin();
  
  float humInit = dht.readHumidity(); // Mesure prise pour la vérification de l'initialisation du capteur
  
  if (!isnan(humInit))
  {
    online.dht22 = true;
    Serial.println("success!");
  }
  else
  {
    online.dht22 = false;
    Serial.println("failed!");
  }
}

// Read BMP388
void readDHT22()
{
  // Start the loop timer
  unsigned long loopStartTime = millis();

  // Initialize sensor
  configureDHT22();

  // Check if sensor initialized successfully
  if (online.dht22)
  {

    for(int i = 0 ; i < 3 ; i++){
      myDelay(50);
    }
    Serial.print("Info - Reading DHT22...");

    myDelay(250);

    dhtTemperatureC   = dht.readTemperature();
    dhtHumidite       = dht.readHumidity();

    dhtTempStats.add(dhtTemperatureC);         
    dhtHumStats.add(dhtHumidite);

    Serial.println("done.");
  }
  else
  {
    Serial.println("Warning - DHT22 offline!");
  }
  // Stop the loop timer
  timer.readDht = millis() - loopStartTime;
}

// ----------------------------------------------------------------------------
// Digilent PMOD TC1 Thermocouple -- NOT WORKING
// https://digilent.com/shop/pmod-tc1-k-type-thermocouple-module-with-wire/
// ----------------------------------------------------------------------------
void configureTC()
{
  Serial.print("Info - Initializing Thermocouple...");
  
  if (tc.begin())
  {
    online.tc = true;
    Serial.println("success!");
  }
  else
  {
    online.tc = false;
    Serial.println("failed!");
  }
}

// Read BMP388
void readTC()
{
  // Start the loop timer
  unsigned long loopStartTime = millis();

  // Initialize sensor
  configureTC();

  // Check if sensor initialized successfully
  if (online.tc)
  {
    Serial.print("Info - Reading Thermocouple...");

    myDelay(250);

    tcTemperatureC = tc.readCelsius();

    tcTempStats.add(tcTemperatureC);
  
    Serial.println("done.");
  }
  else
  {
    Serial.println("Warning - Thermocouple offline!");
  }
  // Stop the loop timer
  timer.readTC = millis() - loopStartTime;
}

// ----------------------------------------------------------------------------
// Alibaba GY49 intensité de la lumière
// http://wiki.sunfounder.cc/index.php?title=Digital_Light_Intensity_Sensor_Module_(GY-49)
// ----------------------------------------------------------------------------
void configureGY49()
{
  Serial.print("Info - Initializing GY49...");
  
  if (gy.isConnected())
  {
    online.gy49 = true;
    Serial.println("success!");
  }
  else
  {
    online.gy49 = false;
    Serial.println("failed!");
  }
}

// Read GY49
void readGY49()
{
  // Start the loop timer
  unsigned long loopStartTime = millis();

  // Initialize sensor
  configureGY49();

  // Check if sensor initialized successfully
  if (online.gy49)
  {
    Serial.print("Info - Reading GY49...");

    myDelay(250);

    gy49LuminositeLux = gy.getLux();

    gyLuxStats.add(gy49LuminositeLux);
    
    Serial.println("done.");
  }
  else
  {
    Serial.println("Warning - GY49 offline!");
  }
  // Stop the loop timer
  timer.readGY49 = millis() - loopStartTime;
}

// ----------------------------------------------------------------------------
// Adafruit VL53L1X Distance ToF
// https://www.adafruit.com/product/3967
// ----------------------------------------------------------------------------
void configureVL()
{
  Serial.print("Info - Initializing VL53L1X...");
  
  if (vl.begin() == 0)
  {
    online.vl = true;
    Serial.println("success!");
  }
  else
  {
    online.vl = false;
    Serial.println("failed!");
  }
}

// Read GY49
void readVL()
{
  // Start the loop timer
  unsigned long loopStartTime = millis();

  // Initialize sensor
  configureVL();

  // Check if sensor initialized successfully
  if (online.vl)
  {
    Serial.print("Info - Reading VL53L1X...");
    
    vl.startRanging(); //Write configuration bytes to initiate measurement
    
    while (!vl.checkForDataReady()){
      delay(1);
    }
    vlDistanceMM = vl.getDistance(); //met en variable l'acquisition de la donnée

    vlDisStats.add(vlDistanceMM);
    
    vl.clearInterrupt();
    vl.stopRanging();
    
    myDelay(250);
  
    Serial.println("done.");
  }
  else
  {
    Serial.println("Warning - VL53L1X offline!");
  }
  // Stop the loop timer
  timer.readVL = millis() - loopStartTime;
}

// ----------------------------------------------------------------------------
// Girouette  Direction du vent
// https://www.sparkfun.com/products/15901
// ----------------------------------------------------------------------------
void readWindDirection(){
  if (millis() - lastSensorAcquisition > delai_capteur_lecture) {
    lastSensorAcquisition = millis();

    uint16_t adc_WindDirectionValue = analogRead(PIN_WIND_DIRECTION);
    
    //roseDESvents(moSbdMessage.GirValPot);
    windDirection = windCompass(adc_WindDirectionValue);

    windDirectionStats.add(windDirection);
  }  
}

uint8_t windCompass(int windDirection){

  /*--------- Tableau des valeurs ---------
   * 8BIT|| Point Cardinal  
   * --------------------------------------
   * 0    NORD            (N)     
   * 1    NORD NORD EST   (NNE)       
   * 2    NORD EST        (NE)
   * 3    EST NORD EST    (ENE)
   * 4    EST             (E)
   * 5    EST SUD EST     (ESE)
   * 6    SUD EST         (SE)
   * 7    SUD SUD EST     (SSE)
   * 8    SUD             (S)
   * 9    SUD SUD OUEST   (SSO)
   * 10   SUD OUEST       (SO)
   * 11   OUEST SUD OUEST (OSO)
   * 12   OUEST           (O)
   * 13   OUEST NORD OUEST(ONO)
   * 14   NORD OUEST      (NO)
   * 15   NORD NORD OUEST (NNO)
   ---------------------------------------*/

  uint8_t cardinalPoint = 0; // valeur par défaut
    if (windDirection <= 893) cardinalPoint               = 5;        //EST SUD EST (N) (0)
    else if (windDirection <= 932) cardinalPoint          = 3;        //EST NORD EST (ENE)
    else if (windDirection<= 993) cardinalPoint           = 4;        //EST (E)
    else if (windDirection <= 1126) cardinalPoint         = 7;        //SUD SUD EST (SSE) 
    else if (windDirection <= 1301) cardinalPoint         = 6;        //SUD EST (SE) 
    else if (windDirection <= 1455) cardinalPoint         = 9;        //SUD SUD OUEST (SSO) 
    else if (windDirection<= 1705) cardinalPoint          = 8;        //SUD (S) 
    else if (windDirection <= 1979) cardinalPoint         = 1;        //NORD NORD EST (NNE)
    else if (windDirection<= 2298 ) cardinalPoint         = 2;        //NORD EST (NE) 
    else if (windDirection <= 2580) cardinalPoint         = 11;       //OUEST SUD OUEST (OSO)
    else if (windDirection <= 2759) cardinalPoint         = 10;       //SUD OUEST (SO)
    else if (windDirection <= 3036) cardinalPoint         = 15;       //NORD NORD OUEST (NNO)
    else if (windDirection <= 3262) cardinalPoint         = 0;        //NORD (N)
    else if (windDirection <= 3450) cardinalPoint         = 13;       //OUEST NORD OUEST (ONO)
    else if (windDirection <= 3674) cardinalPoint         = 14;       //NORD OUEST (NO)
    else if (windDirection <= 4000) cardinalPoint         = 12;       //OUEST (0)
    else cardinalPoint                                    = 0;        //Défaut

    return cardinalPoint;
}
/*
void roseDESvents(int GirValPot) {

 static uint16_t angle = 0;
  if (GirValPot <= 715) angle = 113;       //EST SUD EST (ESE)
  else if (GirValPot <= 754) angle = 68;   //EST NORD EST (ENE)
  else if (GirValPot<= 814) angle = 90;   //EST (E)
  else if (GirValPot <= 950) angle = 158;  //SUD SUD EST (SSE) 
  else if (GirValPot <= 1123) angle = 135;  //SUD EST (SE) 
  else if (GirValPot <= 1271) angle = 203;  //SUD SUD OUEST (SSO) 
  else if (GirValPot<= 1522) angle = 180;  //SUD (S) 
  else if (GirValPot <= 1794) angle = 23;   //NORD NORD EST (NNE)
  else if (GirValPot<= 2106 ) angle = 45;   //NORD EST (NE) 
  else if (GirValPot <= 2384) angle = 248;  //OUEST SUD OUEST (OSO)
  else if (GirValPot <= 2563) angle = 225;  //SUD OUEST (SO)
  else if (GirValPot <= 2863) angle = 338;  //NORD NORD OUEST (NNO)
  else if (GirValPot <= 3094) angle = 0;    //NORD (N)
  else if (GirValPot <= 3331) angle = 293;  //OUEST NORD OUEST (ONO)
  else if (GirValPot <= 3664) angle = 315;  //NORD OUEST (NO)
  else if (GirValPot <= 3972) angle = 270;  //OUEST (0)
  else angle = 0;
//  ptr->angleVent = angle;
*/
/*  String dirVent;
  if(angle == 338 || angle == 0)  dirVent = "NORD";
  else if(angle == 23 || angle == 45)   dirVent = "NORD-EST";
  else if(angle == 68 || angle == 90)   dirVent = "EST";
  else if(angle == 113 || angle == 135) dirVent = "SUD-EST";
  else if(angle == 158 || angle == 180) dirVent = "SUD";
  else if(angle == 203 || angle == 225) dirVent = "SUD-OUEST";
  else if(angle == 248 || angle == 270) dirVent = "OUEST";
  else if(angle == 293 || angle == 315) dirVent = "NORD-OUEST";      
  else  dirVent = "NORD";

//  if(angle == 0)  dirVent = "NORD";
//  else if(angle == 23)  dirVent = "NNE";
//  else if(angle == 45)  dirVent = "NORD-EST";
//  else if(angle == 68)  dirVent = "ENE";
//  else if(angle == 90)  dirVent = "EST";
//  else if(angle == 113) dirVent = "ESE";
//  else if(angle == 135) dirVent = "SUD-EST";
//  else if(angle == 158) dirVent = "SSE";      
//  else if(angle == 180) dirVent = "SUD";
//  else if(angle == 203) dirVent = "SSO";  
//  else if(angle == 225) dirVent = "SUD-OUEST";
//  else if(angle == 248) dirVent = "OSO";  
//  else if(angle == 270) dirVent = "OUEST";
//  else if(angle == 293) dirVent = "ONO";
//  else if(angle == 315) dirVent = "NORD-OUEST";
//  else if(angle == 338) dirVent = "NNO";      
//  else  dirVent = "N";
*/
//Serial.println(String(dirVent) + "\n");
/*
  
  if (GirValPot >= 1550 && GirValPot < 2550) {
    Serial.println("NE");
  }
  else if (GirValPot >= 2550 && GirValPot < 3050) {
    Serial.println("E");
  }
  else if (GirValPot >= 3050 && GirValPot < 3320) {
    Serial.println("SE");
  }
  else if (GirValPot >= 3320 && GirValPot < 3495) {
    Serial.println("S");
  }
  else if (GirValPot >= 3495 && GirValPot < 3605) {
    Serial.println("SO");
  }
  else if (GirValPot >= 3605 && GirValPot < 3700) {
    Serial.println("O");
  }
  else if (GirValPot >= 3700 && GirValPot < 3760) {
    Serial.println("NO");
  }
  else {
    Serial.println("N");
  }
*/
//}

// ----------------------------------------------------------------------------
// Anémomètre Vitesse du vent
// https://www.sparkfun.com/products/15901
// ----------------------------------------------------------------------------
void readWindSpeed(){
  //Rotations = 0; // Set Rotations count to 0 ready for calculations

  // sei(); // Enables interrupts
  // delay (3000); // Wait 3 seconds to average
  // cli(); // Disable interrupts

  // convert to mp/h using the formula V=P(2.25/t)
  // V = P(2.25/3) = P * 0.75
  if (flagISR) {
    rps = 1000 / deltaTime;
    flagISR = false;
  }
  if (millis() - lastInterrupt > 5000)
  {
    rps = 0.00;
  }

  /*if (rps == inf) {
    rps = 0;
    }*/
  //moSbdMessage.windSpeed = rps * 3.66;

  windSpeed = rps * 2.4;
  //if (millis()-timerDisplay > delayDisplay) {
  if (rps != lastrps) {
    lastrps = rps;
    //Serial.println(moSbdMessage.windSpeed);
    //timerDisplay = millis();
  }

  windSpeedStats.add(windSpeed);
  //return moSbdMessage.windSpeed;
  //Serial.print(Rotations); Serial.print("\t\t");
  //Serial.println(moSbdMessage.windSpeed);  
}


// This is the function that the interrupt calls to increment the rotation count
void isr_rotation () {
  deltaTime = millis() - lastInterrupt;
  lastInterrupt = millis();
  flagISR = true;
  //Serial.println("Paquette");
}

// ----------------------------------------------------------------------------
// Pluiviomètre
// https://www.sparkfun.com/products/15901
// ----------------------------------------------------------------------------
void PCF8583_config_cntr(uint8_t i2c_addr){
  Serial.println("> Configuration du PCF8583 en mode Compteur");
  //Démarre le bus I2C
  Wire.begin();
  //Petit délai d'initialisation avant la première communication avec le PCF
  delay(100);

  //Configure le PCF en mode count pulses
  Wire.beginTransmission(i2c_addr);
  Wire.write(0x00);   //Sélectionne le Control and status register
  Wire.write(0x20);   //Configure les bits 4 et 5 en mode "counter" => Bit <5..4> = 0B10
  Wire.endTransmission(); //Communique avec le PCF à cet instant

  //Lecture du registre "Control and status register" pour confirmer l'écriture
  Wire.beginTransmission(i2c_addr);
  Wire.write(0x00);   //Sélectionnne le Control and Status register en lecture
  Wire.endTransmission();//Communique avec le PCF à cet instant

  //Lecture d'un octet du "Control and status register"
  Wire.requestFrom(i2c_addr,1); 
  uint8_t pcf_read_byte = Wire.read();  //Réception de la valeur du registre de contrôle
  if(pcf_read_byte = 0x20){
    Serial.println("> PCF8583 configuré correctement en mode Compteur \r\n");
  }  
  else{
    Serial.println("> Erreur de configuration ou de communication");
    Serial.println("> Veuillez vérifier le câblage ou la puce PCF8583");
    Serial.println("> Arrêt du programme.");
    while(1);
  }  
}

void PCF8583_read_cntr(uint8_t i2c_addr){
uint32_t pcf_counter = 0, val_tab[3];
  //Lecture des 6 registres de compteurs
  for(int index = 0; index < 3; index++){
    Wire.beginTransmission(i2c_addr);
    Wire.write(index+1); 
    Wire.endTransmission();
    Wire.requestFrom(i2c_addr,1); 
    val_tab[index] = Wire.read();
  }
  pcf_counter = ((val_tab[2])<<16) + ((val_tab[1])<<8) + val_tab[0];

  rainCount = pcf_counter;
  Serial.print("> Valeur du compteur = "); Serial.println(pcf_counter);
  Serial.println("");
}

void PCF8583_clr_cntr(uint8_t i2c_addr){
  for(uint8_t index = 0; index < 3; index++){
    Wire.beginTransmission(PCF_I2C_ADDR);
    Wire.write(index+1);   //Sélectionne le registre du compteur 0, 1 et 2 dans l'ordre
    Wire.write(0x00);   //Écrit la vaeur 0x00 dans chaque registre
    Wire.endTransmission(); //Communique avec le PCF à cet instant
  }
  Serial.println("> Compteur du RTC8583 effaçé correctement.");
  Serial.println("");
}

void configurePCF8583(){
  
  //Configure le PCF8583 en mode compteur :
  PCF8583_config_cntr(PCF_I2C_ADDR);
  
  //Efface le compteur au démarrage
  //PCF8583_clr_cntr(PCF_I2C_ADDR);


}

void readRain(){
  
  configurePCF8583();

  //Lecture de la valeur du compteur
  PCF8583_read_cntr(PCF_I2C_ADDR);

  
  
  moSbdMessage.rainHeight = rainCount * rain_CF;

  rainHeightStats.add(moSbdMessage.rainHeight);

  //hourRainCount();
  
  //Serial.println(rainCount);
  PCF8583_clr_cntr(PCF_I2C_ADDR);

  // Pour la pluie à l'heure.
  /*if(iterationRTC_rain == sec2hour/espSleepTime){
    iterationRTC_rain = 0;
    moSbdMessage.rainHeight = rainStats[].average();
  }else{
    iterationRTC_rain++;
  }*/

  Serial.println(iterationRTC_rain);
  
  
  
}
// NOT WORKING
void hourRainCount(){
  
  StatisticCAL rainStats[6]; //sec2hour/espSleepTime

  if(iterationRTC_rain <= 6){ //sec2hour/espSleepTime
    //rainStats[iterationRTC_rain].clear();
    rainStats[iterationRTC_rain].add(rainCount);
    
  }
  else{
    iterationRTC_rain = 0;
  }
  
  
  //rainHeightStats.add(rainCount);
  
  if(iterationRTC_rain >= 6){ //sec2hour/espSleepTime
    
    for (int i = 0; i <= 6; i++){ //sec2hour/espSleepTime
      
      rainHeightStats.add(rainStats[i].average());
      Serial.println(rainStats[i].average());
    }
    moSbdMessage.rainHeight = rainHeightStats.average();
    Serial.println("****");
    Serial.println(rainHeightStats.average());
    Serial.println("****");
    rainHeightStats.clear();
  }
  
  iterationRTC_rain++;
  
  /*if(iterationRTC_rain >= 6){ //sec2hour/espSleepTime
    iterationRTC_rain = 0;
    moSbdMessage.rainHeight = rainStats.average();
  }
  else{
    iterationRTC_rain++;
  }*/
  
}
