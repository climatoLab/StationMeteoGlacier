/* 
  Ce sketch est une de démonstration d'une communication point-à-point entre deux noeuds
  à l'aide de la modulation LoRa.
  Un premier noeud est l'ÉMETTEUR (SENDER) et l'autre est le RÉCEPTEUR (RECEIVER).
  La communication ne se réalise que dans 1 sens, il n'y a pas de confirmation de livraison.
  Ce sketch sert aussi à valider le bon fonctionnement et la flexibilité de la librairie Arduino-LoRa.
  Ce sketch sert aussi à démontrer le bon fonctionnement du matériel en main, soient
    - ÉMETTEUR: module PMOD RFM95, dont le breakout est de Richer Milette.
    - RÉCEPTEUR: un module Heltec LoRa 32 (V2): https://heltec.org/project/wifi-lora-32/
  
  Librairie: Librairies Arduino-Lora: https://github.com/sandeepmistry/arduino-LoRa
  *** Le code ci-présent est pour -- SENDER/TRANSMETTEUR -- ESP32-WROOM (DOIT) ***
  *** fait pour fonctionner avec le code Demo_HeltecLora32V2_Lora-Receiver.ino **
  Développements à poursuivre:
    - Impliquer une retransmission Wifi qui reprend et valide la trame et l'envoie à un service comme Thingsboard
    - Implémenter une transmission point-à-multipoint, à l'aide de l'information dans la trame. Consulter le code exemple https://github.com/sandeepmistry/arduino-LoRa/blob/master/examples/LoRaDuplex/LoRaDuplex.ino
    - Faire des essais du TxPower (via la méthode setTxPower)
    - Comprendre l'effet des paramètres: enableInvertIQ, setGain, setPreambleLength  (et voir si applicables/avantageux)
    - Comprendre et implémenter l'utilisation de idle() et sleep() dans un contexte de deep_sleep du ESP32
    - Comprendre et expérimenter l'interruption utilisant onReceive() et de la broche dio0
    - Développer des essais/familiarisation d'un mode meshed (consulter: https://www.hackster.io/davidefa/esp32-lora-mesh-1-the-basics-3a0920)
 * Crédits d'inspiration:
     https://randomnerdtutorials.com/esp32-lora-rfm95-transceiver-arduino-ide/
     
*/

#include <SPI.h>
#include <LoRa.h>

#define _VERSION "2.1.7"

//Paramètre de communication ESP32 et module RFM95:
#define ss 16
// Note pour ces broches:
// sur ESP32-WROOM (DOIT) et le module PMODRFM95 avec R2 et R4 normales, il faut rst=12 et dio0=13
// sur ESP32-WROOM (DOIT) et le module PMODRFM95 avec modif (jumper) entre R2 et R4, il faut rst=13 et dio0=12
#define rst 4 // Original: 12
#define dio0 17 // Original: 13


//Paramètres de la communication LoRa:
//Consulter: https://www.baranidesign.com/faq-articles/2019/4/23/lorawan-usa-frequencies-channels-and-sub-bands-for-iot-devices
const uint32_t BAND = 902500000;   //902.3MHz, Channel 2
const uint8_t LoRasyncWord = 0x33;
//The spreading factor (SF) impacts the communication performance of LoRa, which uses an SF between 7 and 12. A larger SF increases the time on air, which increases energy consumption, reduces the data rate, and improves communication range. For successful communication, as determined by the SF, the modulation method must correspond between a transmitter and a receiver for a given packet.
const uint8_t LoRaSF = 10;
const uint32_t LoRaSB = 125E3;
const uint8_t LoRaCR = 5;

const int PIN_POW_EN = 13; //Pin pour le 3V3

//Structure de donnée à transmettre -- ici un test, à modifier selon le besoin.
// Le récepteur doit avoir exactement la même définition de son côté (code)
typedef union
{
  struct
  {
    // Ajouter destinataire et source: uint8_t chacun
    // Ajouter la version de la source: uint8_t
    uint32_t  unixtime;         // UNIX Epoch time                (4 bytes)
    int16_t   temperatureInt;   // Internal temperature (°C)      (2 bytes)   * 100
    uint16_t  humidityInt;      // Internal humidity (%)          (2 bytes)   * 100
    uint16_t  pressureInt;      // Internal pressure (hPa)        (2 bytes)   - 850 * 100
    int16_t   pitch;            // Pitch (°)                      (2 bytes)   * 100
    int16_t   roll;             // Roll (°)                       (2 bytes)   * 100
    uint16_t  heading;          // Heading (°)                    (2 bytes)
    int32_t   latitude;         // Latitude (DD)                  (4 bytes)   * 1000000
    int32_t   longitude;        // Longitude (DD)                 (4 bytes)   * 1000000
    uint8_t   satellites;       // # of satellites                (1 byte)
    uint16_t  hdop;             // HDOP                           (2 bytes)
    uint16_t  voltage;          // Battery voltage (V)            (2 bytes)   * 100
    uint16_t  transmitDuration; // Previous transmission duration (2 bytes)
    uint8_t   transmitStatus;   // Iridium return code            (1 byte)
    uint16_t  iterationCounter; // Message counter                (2 bytes)
  } __attribute__((packed));                            // Total: (34 bytes)
  uint8_t bytes[34];
} SBD_MO_MESSAGE;

SBD_MO_MESSAGE moSbdMessage;

//Simple fonction pour assigner du data dans la structure
void fillInDummyData(void) {
  moSbdMessage.unixtime = 123456789;
  moSbdMessage.temperatureInt = -1020;
  moSbdMessage.humidityInt = 2000;
  moSbdMessage.pressureInt = 15000;
  moSbdMessage.pitch = -200;
  moSbdMessage.roll = 150;
  moSbdMessage.heading = 32;
  moSbdMessage.latitude = 4298745;
  moSbdMessage.longitude = -7312356;
  moSbdMessage.satellites = 5;
  moSbdMessage.hdop = 22;
  moSbdMessage.voltage = 332;
  moSbdMessage.transmitDuration = millis();
  moSbdMessage.transmitStatus = 5;
  moSbdMessage.iterationCounter = 0;
}

void setup() {
  //initialize Serial Monitor
  Serial.begin(115200);
  while (!Serial);
  pinMode(PIN_POW_EN, OUTPUT);
  digitalWrite(PIN_POW_EN, HIGH);
  delay (500);
  Serial.println("LoRa Sender");

  //setup LoRa transceiver module
  LoRa.setPins(ss, rst, dio0);
  
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
  

  Serial.println("LoRa Initializing OK!");
  //Serial.println("Dumping RFM95 registers:");
  //LoRa.dumpRegisters(Serial);
  //Serial.println("\nEnd dumping RFM95 registers.");
  Serial.println("Current config:");
  Serial.print("SF=0x");
  Serial.println(LoRa.getSpreadingFactor(),HEX);
  Serial.print("BW=");
  Serial.println(LoRa.getSignalBandwidth());

  fillInDummyData();
}

void loop() {
  Serial.print("Sending packet: ");
  Serial.print(moSbdMessage.iterationCounter);
  Serial.print("  of len=");
  Serial.println(sizeof(moSbdMessage));

  //Send LoRa packet to receiver
  uint32_t startTime = micros();  //Pour mesurer approx le temps de transmission
  LoRa.beginPacket();
  LoRa.write(moSbdMessage.bytes, sizeof(moSbdMessage));  //Clef pour la transmission "binaire"
  LoRa.endPacket();
  uint32_t endTime = micros();

  moSbdMessage.unixtime = millis();
  moSbdMessage.iterationCounter++;
  moSbdMessage.transmitDuration = endTime-startTime;

  delay(1000);
}
