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

  *** Le code ci-présent est pour -- RECEIVER/RÉCEPTEUR -- Heltec LoRa 32 (V2) ***
  *** fait pour fonctionner avec le code test_ESP32-Lora-lib_Yh-jan2023.ino **

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
#include <U8g2lib.h>
#include <Wire.h>   //Pour le SSD1306
#include <WiFi.h>
#include <Preferences.h>
#include <nvs_flash.h>
#include <esp_sntp.h>

#define _VERSION "3.2.1"

//Paramètre de communication ESP32 et module RFM95:
// sur le Heltec LoRa 32 V2, il faut ss=18, rst=14 et dio0=26
#define ss 18   //Selon le schematic du Heltec LoRa 32 V2
#define rst 14
#define dio0 26  //Selon ma comprehension du schematic LoRa 32 V2

//Traitement des paramètres
#define STRING_LENGTH 48

//Paramètres de la communication LoRa:
//Consulter: https://www.baranidesign.com/faq-articles/2019/4/23/lorawan-usa-frequencies-channels-and-sub-bands-for-iot-devices
const uint32_t BAND = 902500000;   //902.3MHz, Channel 2
const uint8_t LoRasyncWord = 0x33;
const uint8_t LoRaSF = 10;
const uint32_t LoRaSB = 125E3;
const uint8_t LoRaCR = 5;

//Driver de l'afficheur SSD1306:
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ 16, /* clock=*/ 15, /* data=*/ 4);

Preferences mesPreferences;

const char* ntpServer = "0.ca.pool.ntp.org";


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

SBD_MO_MESSAGE loRaData;

struct pktData
{
  uint32_t lstTime;
  uint8_t length;
  int16_t rssi;
  float   snr;
}__attribute__((packed));

pktData lstPktData;

//Déf de qqe timers pour un fonctionnement non-bloquant
uint32_t rpktTimer = 0;
uint32_t rpktDelay = 5000;
uint32_t clockTimer = 0;
uint32_t clockDelay = 1000;
bool waitStatus = true;
bool receiveflag = false; // software flag for LoRa receiver, received data makes it true.


//Simple fonction pour afficher le data dans la structure (reçue)
void printReceivedstructData(SBD_MO_MESSAGE *msg) {
  Serial.println("Received data:");
  Serial.print("uxtime: ");
  Serial.println(msg->unixtime);
  Serial.print("tempint:");
  Serial.println(msg->temperatureInt);
  Serial.print("humint: ");
  Serial.println(msg->humidityInt);
  Serial.print("presint:");
  Serial.println(msg->pressureInt);
  Serial.print("pitch:  ");
  Serial.println(msg->pitch);
  Serial.print("roll:   ");
  Serial.println(msg->roll);
  Serial.print("heading:");
  Serial.println(msg->heading);
  Serial.print("lat:    ");
  Serial.println(msg->latitude);
  Serial.print("long:   ");
  Serial.println(msg->longitude);
  Serial.print("numsat: ");
  Serial.println(msg->satellites);
  Serial.print("hdop:   ");
  Serial.println(msg->hdop);
  Serial.print("voltage:");
  Serial.println(msg->voltage);
  Serial.print("txDur:  ");
  Serial.println(msg->transmitDuration);
  Serial.print("txStat: ");
  Serial.println(msg->transmitStatus);
  Serial.print("iterCnt:");
  Serial.println(msg->iterationCounter);
}

void onReceive(int packetSize)//LoRa receiver interrupt service
{
  digitalWrite(25,HIGH);
  // try to parse packet
  //int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packet
    //Serial.print("Received packet '");

    // read packet
  //  while (LoRa.available()) {  //Yh: je ne suis pas convaincu de l'utilité du while ici... mais pas du tout...
        lstPktData.length = LoRa.readBytes(loRaData.bytes,sizeof(loRaData));
        lstPktData.rssi = LoRa.packetRssi();
        lstPktData.snr = LoRa.packetSnr();
        lstPktData.lstTime = millis();
  //  }       
    receiveflag = true;
  }
}


void setup() {
  //initialize Serial Monitor
  Serial.begin(115200);
  while (!Serial);

  u8g2.begin();
  u8g2.clearBuffer();					// clear the internal memory
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.drawStr(0,10,"LoRa Receiver");	// write something to the internal memory
  u8g2.sendBuffer();

  //pinMode(26,INPUT);
  pinMode(25,OUTPUT);
  digitalWrite(25,LOW);
  
  Serial.println("LoRa Receiver");

  mesPreferences.begin("maConfig", false);

  if (validateParams()) {
    if (connecterAuWifi()) {
      u8g2.drawStr(0,30,"NTP config");
      u8g2.sendBuffer();
      configTime(0, 0, ntpServer);
      while (sntp_get_sync_status() != SNTP_SYNC_STATUS_COMPLETED) {1;}
      Serial.println("Done!");
      u8g2.drawStr(66,30,"Done");
      u8g2.sendBuffer();
      Serial.println("Configuring TZ");
      setenv("TZ", "EST5EDT,M3.2.0/2,M11.1.0", 1);
      tzset();
    } else {
      Serial.println("Ne connecte pas au Wifi!");
    }
  } else { 
    Serial.println("Un paramètre est invalide, corriger et #REBOOT");
    while (1) gererSerialEntrant();
  }

  //setup LoRa transceiver module
  LoRa.setPins(ss, rst, dio0);
  
  //replace the freq LoRa.begin(freq) argument with your location's frequency
  //Voir la note ci-haut à la def BAND
  while (!LoRa.begin(BAND)) {
    Serial.println(".");
    delay(500);
  }
  // Change sync word to match the receiver
  // The sync word assures you don't get LoRa messages from other LoRa transceivers
  // ranges from 0-0xFF
  LoRa.setSyncWord(LoRasyncWord);
  LoRa.enableCrc();
  LoRa.setSpreadingFactor(LoRaSF);
  LoRa.setSignalBandwidth(LoRaSB);
  LoRa.setCodingRate4(LoRaCR);

  //Configure la reception par interruption:
  LoRa.onReceive(onReceive);  //Yh au 30 janv23: marche, finalement... manquait l'instruction suivante (LoRa.receive())
  LoRa.receive();

  Serial.println("LoRa Initializing OK!");
  //Serial.println("Dumping RFM95 registers:");
  //LoRa.dumpRegisters(Serial);
  //Serial.println("\nEnd dumping RFM95 registers.");
  Serial.println("Current config:");
  Serial.print("SF=");
  Serial.println(LoRa.getSpreadingFactor());
  Serial.print("BW=");
  Serial.println(LoRa.getSignalBandwidth());

  //Fait le menage avant de quitter
  u8g2.clearBuffer();					
  u8g2.drawStr(0,10,"LoRa Receiver");
  u8g2.sendBuffer();

}

void loop() {

  //onReceive(0); //Poll if pkt received -- desactivé pcq reception par interruption

  //Lorsqu'un paquet a été reçu, mise a jour de l'écran:
  if (receiveflag) {
    Serial.print("Recv lenght: ");
    Serial.println(lstPktData.length);
    printReceivedstructData(&loRaData);
    // print RSSI of packet
    Serial.print("RSSI: ");
    Serial.println(lstPktData.rssi);
    Serial.print("SNR: ");
    Serial.println(lstPktData.snr);
    u8g2.setDrawColor(0);
    u8g2.drawBox(0, 10, u8g2.getDisplayWidth(), u8g2.getDisplayHeight()-21); //-20 pour 10 en haut et 10 en bas untouched
    u8g2.drawBox(83, 0, u8g2.getDisplayWidth()-83, 10);
    u8g2.setDrawColor(1);
    u8g2.drawStr(84, 10, "RcvPkt");
    u8g2.drawStr(0, 20, String("Iter:"+String(loRaData.iterationCounter)).c_str());
    u8g2.drawStr(0, 30, String("RSSI:"+String(lstPktData.rssi)+" SNR:"+String(lstPktData.snr)).c_str());
    u8g2.drawStr(0, 40, String("Dtsz:"+String(lstPktData.length)).c_str());
    u8g2.drawStr(0, 50, String("TxDr:"+String(loRaData.transmitDuration)).c_str());
    u8g2.sendBuffer();
    rpktTimer = millis();
    waitStatus = false;
    receiveflag = false;

    // Insérer ici le code qui prend le data de la structure, le formatte et l'envoie au point de service suivant
    // ceci peut être MQTT, ThingsBoard, ou encore ThingSpeak ou autre nuage!

    //Prépare pour la prochaine réception:
    LoRa.receive();
    digitalWrite(25,LOW);
  }

  //Apres un délais change le msg RcvPkt par Wait, pour signifer en attente de réception de msg
  if (millis()-rpktTimer > rpktDelay) {
    if (!waitStatus) {
      u8g2.setDrawColor(0);
      u8g2.drawBox(83, 0, u8g2.getDisplayWidth()-83, 10);
      u8g2.setDrawColor(1);
      u8g2.drawStr(84, 10, " Wait ");
      u8g2.sendBuffer();
      waitStatus = true;
    }
  }

  //Affiche l'heure au bas du OLED
  if (millis()-clockTimer > clockDelay) {
    clockTimer = millis();
    struct tm timeinfo;
    if(getLocalTime(&timeinfo)){
      //Serial.print(&timeinfo, "%A, %B %d %Y %H:%M:%S");
      char strftime_buf[64];
      strftime(strftime_buf, sizeof(strftime_buf), "%F %T", &timeinfo);
      u8g2.setDrawColor(0);
      u8g2.drawBox(0, 51, u8g2.getDisplayWidth(), 10);
      u8g2.setDrawColor(1);
      u8g2.drawStr(0, 60,strftime_buf);
      u8g2.sendBuffer();
    }
  }

  //Finalement pour permettre de procéder à la configuration:
  gererSerialEntrant();

}
