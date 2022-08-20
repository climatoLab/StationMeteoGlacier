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
