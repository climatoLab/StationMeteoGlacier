//Pluviomètre///////////////////////////////////////////////////////////////////////////////////////////////////////
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
  CompteurPluie = pcf_counter;
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


//Gyrouette/////////////////////////////////////////////////////////////////////////////////////////////////////
void girouette(){
  if (millis() - derniereMESURECAPTEURS > delai_capteur_lecture) {
    derniereMESURECAPTEURS = millis();

    valpot = analogRead(34);
    Serial.println("sortie analogique de la girouette = ");
    Pcardinaux = valpot;
    Serial.println(String(Pcardinaux) + "\n");

    roseDESvents(valpot);
  }  
}

void envoieGirouette(){
  if (millis() - derniereEnvoieMESURECAPTEURS > delai_capteur_lecture) {
    derniereEnvoieMESURECAPTEURS = millis();

    valpot = analogRead(34);
    Serial.print("sortie analogique de la girouette = ");
    Pcardinaux = valpot;
    Serial.println(Pcardinaux);

    roseDESvents(valpot);
    //tb.sendTelemetryFloat("Val potentiomètre", valpot);
    tb.sendTelemetryFloat("Direction", Pcardinaux);
  }  
}

void roseDESvents(int valpot) {

 static uint16_t angle = 0;
  if (valpot <= 715) angle = 113;       //EST SUD EST (ESE)
  else if (valpot <= 754) angle = 68;   //EST NORD EST (ENE)
  else if (valpot<= 814) angle = 90;   //EST (E)
  else if (valpot <= 950) angle = 158;  //SUD SUD EST (SSE) 
  else if (valpot <= 1123) angle = 135;  //SUD EST (SE) 
  else if (valpot <= 1271) angle = 203;  //SUD SUD OUEST (SSO) 
  else if (valpot<= 1522) angle = 180;  //SUD (S) 
  else if (valpot <= 1794) angle = 23;   //NORD NORD EST (NNE)
  else if (valpot<= 2106 ) angle = 45;   //NORD EST (NE) 
  else if (valpot <= 2384) angle = 248;  //OUEST SUD OUEST (OSO)
  else if (valpot <= 2563) angle = 225;  //SUD OUEST (SO)
  else if (valpot <= 2863) angle = 338;  //NORD NORD OUEST (NNO)
  else if (valpot <= 3094) angle = 0;    //NORD (N)
  else if (valpot <= 3331) angle = 293;  //OUEST NORD OUEST (ONO)
  else if (valpot <= 3664) angle = 315;  //NORD OUEST (NO)
  else if (valpot <= 3972) angle = 270;  //OUEST (0)
  else angle = 0;
//  ptr->angleVent = angle;

  String dirVent;
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

Serial.println(String(dirVent) + "\n");
/*
  
  if (valpot >= 1550 && valpot < 2550) {
    Serial.println("NE");
  }
  else if (valpot >= 2550 && valpot < 3050) {
    Serial.println("E");
  }
  else if (valpot >= 3050 && valpot < 3320) {
    Serial.println("SE");
  }
  else if (valpot >= 3320 && valpot < 3495) {
    Serial.println("S");
  }
  else if (valpot >= 3495 && valpot < 3605) {
    Serial.println("SO");
  }
  else if (valpot >= 3605 && valpot < 3700) {
    Serial.println("O");
  }
  else if (valpot >= 3700 && valpot < 3760) {
    Serial.println("NO");
  }
  else {
    Serial.println("N");
  }
*/
}


//Gyrouette/////////////////////////////////////////////////////////////////////////////////////////////////////
