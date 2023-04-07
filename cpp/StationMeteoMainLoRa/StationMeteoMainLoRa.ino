/******************************************************************
   Nom: StationMeteoMain_2V0.ino
   Créer par: Thomas Perras
   Description: Programme de la Station Meteo Glacier conçu pour communiquer avec le LoRa pour envoyer des données à un gateway.

******************************************************************/

/* --- HISTORIQUE de développement --------------------------------------
    //*** v1.0.0 : Programmation pour la sortie à Yamaska
    //*** v2.0.0 : Programmation pour la communication avec le LoRa
    //*** v2.1.0 : Inclusion de la fonction pour la girouette
    //*** v2.2.0 : Inclusion de la fonction pour l'anémomètre
    //*** v2.2.1 : Changement de nom pour les variables de la girouette et de l'anémomètre
    //*** v2.3.0 : Ajout des données de la girouette et de l'anémomètre dans la carte micro SD ainsi que des modifications sur les noms de leur fonction
    //*** v2.4.0 : Commentaire sur la version du code et ajustement du temps du deep sleep pour la station météo
    //*** v2.5.0 : Prolongement de la mesure des données par la station météo
    //*** v2.5.1 : Réorganisation du code pour mieux capter et mieux afficher les données
    //*** v2.6.0 : Ajout de commentaires pour une meilleure compréhension du code
    //*** v2.7.0 : Ajout d'une adresse destination et source et d'une version de la trame pour communication LoRa
    //*** v2.8.0 : Amélioration du calcul Time on air
*/
//-----------------------------------------------------------------------


//--- Déclaration des librairies (en ordre alpha) -----------------------
#include "lib\StationMeteoMain_2V0.h"//--> Librairie du fichier main.
#include "lib\VLlibrary.h"
#include <LoRa.h> //https://github.com/sandeepmistry/arduino-LoRa
#include <math.h>
//-----------------------------------------------------------------------


//--- Definitions -------------------------------------------------------
#define Version "2.8.1"
//Paramètre de communication ESP32 et module RFM95:
#define ss 16
// Note pour ces broches:
// sur ESP32-WROOM (DOIT) et le module PMODRFM95 avec R2 et R4 normales, il faut rst=12 et dio0=13
// sur ESP32-WROOM (DOIT) et le module PMODRFM95 avec modif (jumper) entre R2 et R4, il faut rst=13 et dio0=12
#define rst 4 // Original: 12
#define dio0 17 // Original: 13
//Broche pour l'anémomètre
#define WindSensorPin 35 // The pin location of the anemometer sensor
//-----------------------------------------------------------------------


//--- Declaration des objets --------------------------------------------

//-----------------------------------------------------------------------


//--- Constantes et variables --------------------------------------------------------
//Paramètres de la communication LoRa:
const byte currentSupportedFrameVersion = 0x02;
byte destination = 0xF0;      // destination to send to (gateway)
byte localAddress = 0x01;     // address of this device
//Consulter: https://www.baranidesign.com/faq-articles/2019/4/23/lorawan-usa-frequencies-channels-and-sub-bands-for-iot-devices
const uint32_t BAND = 902500000;   //902.3MHz, Channel 2
const uint8_t LoRasyncWord = 0x33; //Équivant à une valeur de 51 en décimale
//The spreading factor (SF) impacts the communication performance of LoRa, which uses an SF between 7 and 12. A larger SF increases the time on air, which increases energy consumption, reduces the data rate, and improves communication range. For successful communication, as determined by the SF, the modulation method must correspond between a transmitter and a receiver for a given packet.
const uint8_t LoRaSF = 10;
const uint32_t LoRaSB = 125E3;
const uint8_t LoRaCR = 5;
//Variable globales de TimeOnAir
bool transmissionTerminee = false;
uint32_t ToAStart = 0;
RTC_DATA_ATTR uint32_t  LoRaTimeOnAir = 0; //Variable qui doit persister
RTC_DATA_ATTR uint16_t iterationRTC;

String path = "/Yamaska_A2022.txt"; //--> Chemin emprunter pour enregistrer les données sur la carte SD.
String labelData = "Date, Time, Vin, bmpTemperature, bmpPression, bmpAltitude, dhtHumidite, dhtTemperature, tcTemperature, gyLuminosite, distanceVL, GirDirVent, AnemomVitVent\n"; //--> Première ligne enregistrer sur la carte SD, représente l'ordre des valeurs.

//Mode deep sleep
unsigned long timeSleep = 60000000;  //--> 60 secondes de deep sleep (microsec)
unsigned long startTime = 0;   //--> Permet de mesurer le temps d'exécution du programme
unsigned long stopTime = 0;   //--> Permet de mesurer le temps d'exécution du programme
unsigned long totSleep = timeSleep - (stopTime - startTime); //--> Durée total du Deep Sleep ajustée (microsec)
unsigned long pre_millis = 0; //--> Initialisation de la variable utiliser pour le delay sans arrêt <No_Stop_Delay (NSD)>
unsigned long trigger_NSD = 1000; //--> Trigger pour indiquer au delay sans arrêt qui est temps d'effectuer une action. <No_Stop_Delay (NSD)>

//Valeurs pour la girouette
unsigned long derniereMESURECAPTEURS = millis();
unsigned long derniereEnvoieMESURECAPTEURS = millis();
int delai_capteur_lecture = 250;
int delai = 0;

//Valeurs pour l'anémomètre
volatile unsigned long lastInterrupt;
volatile unsigned int deltaTime ;
unsigned long timerDisplay = millis();
int delayDisplay = 500;
volatile bool flagISR = false;
float rps = 0;
float lastrps = 1; // Permet d'avoir un affichage initiale lors du démarage du code

//Structure de donnée à transmettre -- ici un test, à modifier selon le besoin.
// Le récepteur doit avoir exactement la même définition de son côté (code)
typedef union
{
  struct
  {
    // Ajouter destinataire et source: uint8_t chacun
    // Ajouter la version de la source: uint8_t
    uint8_t  frameVersion;     //                                (1 byte)
    uint8_t  recipient;        //                                (1 byte)
    uint8_t  sender;           //                                (1 byte)
    uint32_t unixtime;         //                                (4 bytes)
    int16_t  bmpTemperatureC;  //                                (2 bytes)   * 100
    int16_t  dhtTemperatureC;  //                                (2 bytes)   * 100
    int16_t  tcTemperatureC;   //                                (2 bytes)   * 100
    uint16_t bmpPressionHPa;   //                                (2 bytes)
    uint8_t  dhtHumidite;      //                                (1 byte)
    uint16_t vlDistanceMM;     //                                (2 bytes)
    uint16_t gy49LuminositeLux;//                                (2 bytes)
    int16_t  bmpAltitude;      //                                (2 bytes)
    uint16_t GirValPot;        //                                (2 bytes)
    uint16_t AnemomVitesseVent;//                                (2 bytes)
    uint16_t Vin;              //                                (2 byte) * 100
    int32_t  latitudeGPS;      //                                (4 byte)
    int32_t  longitudeGPS;     //                                (4 byte)
    int32_t  altitudeGPS;      //                                (4 byte)
    uint8_t  satellites;       // # of satellites                (1 byte)
    uint16_t hdop;             // HDOP                           (2 bytes)
    uint16_t transmitDuration; // Previous transmission duration (2 bytes)
    uint8_t  transmitStatus;   // Iridium return code            (1 byte)
    uint16_t iterationCounter; // Message counter                (2 bytes)
  } __attribute__((packed));                           // Total: (48 bytes)
  uint8_t bytes[48];
} SBD_MO_MESSAGE;

SBD_MO_MESSAGE moSbdMessage;


//Simple fonction pour assigner du data dans la structure
void fillInData(void) {
  moSbdMessage.frameVersion = currentSupportedFrameVersion;
  moSbdMessage.recipient = destination;
  moSbdMessage.sender = localAddress;

  moSbdMessage.unixtime = rtc.now().unixtime();
  moSbdMessage.bmpTemperatureC = bmpTemp() * 100;
  moSbdMessage.dhtTemperatureC = dhtTemp() * 100;
  moSbdMessage.tcTemperatureC = tempTC() * 100;
  moSbdMessage.bmpPressionHPa = bmpPression();
  moSbdMessage.dhtHumidite = dhtHumi();
  moSbdMessage.vlDistanceMM = distanceVL();
  moSbdMessage.gy49LuminositeLux = gyLux();
  moSbdMessage.bmpAltitude = bmpAltitude();
  moSbdMessage.GirValPot = girouetteDirectionVent();
  moSbdMessage.AnemomVitesseVent = anemometreVitesseVent() * 100;
  moSbdMessage.Vin = lecture_VinExt() * 100;
  moSbdMessage.latitudeGPS = 5;
  moSbdMessage.longitudeGPS = 22;
  moSbdMessage.altitudeGPS = 332;
  moSbdMessage.satellites = 56;
  moSbdMessage.hdop = 5;
  moSbdMessage.transmitStatus = 1;
}

//-----------------------------------------------------------------------

String str_donnees() { //--> Met dans une variable String la structure de nos données
  String s_msg = getDateRTC() + vir
                 + getTimeRTC() + vir
                 + String(lecture_VinExt()) + vir
                 + String(bmpTemp()) + vir
                 + String(bmpPression()) + vir
                 + String(bmpAltitude()) + vir
                 + String(dhtHumi()) + vir
                 + String(dhtTemp()) + vir
                 + String(tempTC()) + vir
                 + String(gyLux()) + vir
                 + String(distanceVL()) + vir
                 + String(girouetteDirectionVent()) + vir
                 + String(anemometreVitesseVent()) + "\n";
  return s_msg;
}

void onTxDone() {
  transmissionTerminee = true;
  uint32_t ToAStop = micros();
  if (ToAStop > ToAStart)
    LoRaTimeOnAir = ToAStop - ToAStart;
  else LoRaTimeOnAir = 0;

  //TBD: variable (globale) indiquant la complétion de la transmission pour permettre d'éteindre les modules et entrer en dormance
}

void setup() {
  startTime = micros();
  Serial.begin(115200);
  while (!Serial);
  i2c_init_(); //--> initialisation des pins i2c.
  initLibs(path, labelData); //--> initialisation des objets.
  //Initialise le VL53L1X
  init_VL();

  //Initialise l'anémomètre
  pinMode(WindSensorPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(WindSensorPin), isr_rotation, FALLING);
  lastInterrupt = millis();
  //Initialise la girouette
  pinMode(34, OUTPUT);
  digitalWrite(34, HIGH);

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

void loop() {
  fillInData(); //Met à jour les données
  sendData(str_donnees(), path); //--> envoi des données vers la carte SD.
  Serial.print("Sending packet: ");
  Serial.print(moSbdMessage.iterationCounter);
  Serial.print("  of len=");
  Serial.println(sizeof(moSbdMessage));

  moSbdMessage.transmitDuration = (LoRaTimeOnAir/1000UL);   //Met à jour la durée de la transmission du paquet
  moSbdMessage.iterationCounter = iterationRTC;  //Met à jour le nombre d'itérations depuis le début du programme
  //Send LoRa packet to receiver
  LoRa.beginPacket();
  LoRa.write(moSbdMessage.bytes, sizeof(moSbdMessage));  //Clef pour la transmission "binaire"
  ToAStart = micros();
  LoRa.endPacket(true);

  Serial.println("\nTempérature du BMP388 (°C) = " + String(moSbdMessage.bmpTemperatureC));
  Serial.println("Température du DHT22 (°C) = " + String(moSbdMessage.dhtTemperatureC));
  Serial.println("Température du thermocouple (°C) = " + String(moSbdMessage.tcTemperatureC));
  Serial.println("Pression (hPa) = " + String(moSbdMessage.bmpPressionHPa));
  Serial.println("Humidité du DHT22 (%) = " + String( moSbdMessage.dhtHumidite));
  Serial.println("Distance du VL53L1X (mm) : " + String(moSbdMessage.vlDistanceMM));
  Serial.println("Luminosité (lux) : " + String(moSbdMessage.gy49LuminositeLux));
  Serial.println("Altitude : " + String(moSbdMessage.bmpAltitude));
  Serial.println("Sortie analogique de la girouette = " + String(moSbdMessage.GirValPot));
  Serial.println("Vitesse de vent de l'anémomètre = " + String(moSbdMessage.AnemomVitesseVent));
  Serial.println("Tension du Vin (V) : " + String(moSbdMessage.Vin));
  Serial.println("TransmitDuration = " + String(moSbdMessage.transmitDuration));
  Serial.println("TransmitStatus = " + String(moSbdMessage.transmitStatus));
  Serial.println("IterationCounter = " + String(moSbdMessage.iterationCounter) + "\n");
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
  //if(cur_millis - pre_millis >= trigger_NSD){ //--> Condition que le trigger_NSD est respecter en rapport avec la différence des millis actuel et de l'ancienne boucle.
  //pre_millis = cur_millis;  //--> ajustement inutile puisque le code se recompile (à voir)
  esp_deep_sleep(totSleep); //--> Entre en mode deep sleep pour une durée de x microsecondes
  //}
}
