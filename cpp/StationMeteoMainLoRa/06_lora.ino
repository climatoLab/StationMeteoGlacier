void onTxDone() {
  transmissionTerminee = true;
  uint32_t ToAStop = micros();
  if (ToAStop > ToAStart)
    LoRaTimeOnAir = ToAStop - ToAStart;
  else LoRaTimeOnAir = 0;

  //TBD: variable (globale) indiquant la complétion de la transmission pour permettre d'éteindre les modules et entrer en dormance
}

void loraConfig(){
  Serial.println("LoRa Sender");

  //setup LoRa transceiver module
  LoRa.setPins(PIN_LORA_SS, PIN_LORA_RST, PIN_LORA_DIO0);

  //replace the freq LoRa.begin(freq) argument with your location's frequency
  //Voir la note ci-haut à la def BAND
  Serial.println("Initializing");

  while (!LoRa.begin(BAND)) {
    Serial.print(".");
    delay(500);
  }
  
  Serial.println(".");
  // Change sync word to match the receiver
  // The sync word assures you don't get LoRa messages from other LoRa transceivers
  // ranges from 0-0xFF
  LoRa.setSyncWord(LoRasyncWord);
  LoRa.enableCrc();
  //The spreading factor (SF) impacts the communication performance of LoRa, which uses an SF between 7 and 12. A larger SF increases the time on air, which increases energy consumption, reduces the data rate, and improves communication range. For successful communication, as determined by the SF, the modulation method must correspond between a transmitter and a receiver for a given packet.
  LoRa.setSpreadingFactor(LoRaSF);
  LoRa.setSignalBandwidth(LoRaSB);
  LoRa.setCodingRate4(LoRaCR);

  LoRa.onTxDone(onTxDone);


  Serial.println("LoRa Initializing OK!");
  //Serial.println("Dumping RFM95 registers:");
  //LoRa.dumpRegisters(Serial);
  //Serial.println("\nEnd dumping RFM95 registers.");
  Serial.println("Current config:");
  Serial.print("SF=0x");
  //Serial.println(LoRa.getSpreadingFactor(),HEX);
  Serial.print("BW=");
  //Serial.println(LoRa.getSignalBandwidth());
}

void transmitData(){
  Serial.print("Sending packet: ");
  Serial.print(String(iterationRTC));
  Serial.print(" of len = ");
  Serial.println(sizeof(moSbdMessage));

  moSbdMessage.frameVersion = SUPPORTED_FRAME_VERION;
  moSbdMessage.recipient = destination;
  moSbdMessage.sender = localAddress;

  moSbdMessage.transmitDuration = (LoRaTimeOnAir/1000UL);   //Met à jour la durée de la transmission du paquet
  moSbdMessage.iterationCounter = iterationRTC;  //Met à jour le nombre d'itérations depuis le début du programme
  //Send LoRa packet to receiver
  LoRa.beginPacket();
  LoRa.write(moSbdMessage.bytes, sizeof(moSbdMessage));  //Clef pour la transmission "binaire"
  ToAStart = micros();
  LoRa.endPacket(true);

  unsigned long cur_millis = millis(); //--> indique la valeur actuel des millis()

  int compteur = 50;
  while (!transmissionTerminee && compteur > 0) {
    compteur--;
    delay(100);
  }

  if (compteur <= 0) {
    Serial.println("Le flag de transmission a échoué");
  }
  stopTime = micros();
  iterationRTC++;
}
