/*
    Title:    Programme pour la station météo poteau.
    Date:     
    Author:   Vincent Lavallée
    Version:  3.0.0
    Crédit:   Thomas Perras, Yannick Heynemand

    Description:
    - Programme de la Station Meteo Glacier conçu pour communiquer avec le LoRa pour envoyer des données à un gateway.

    Components:


    Sensors:
    - RM Young 05103L Wind Monitor


    Comments:
    - Sketch uses 98720 bytes (37%) of program storage space. Maximum is 262144 bytes.
    - Power consumption in deep sleep is ~625 uA at 12.5V

*/
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
    //*** v3.0.0 : Modification de la structure du code
*/

// ----------------------------------------------------------------------------
// Libraries
// ----------------------------------------------------------------------------
#include <Adafruit_Sensor.h>      // https://github.com/adafruit/Adafruit_Sensor
#include "Adafruit_BMP3XX.h"      // https://github.com/adafruit/Adafruit_BMP3XX
#include <Max44009.h>             // https://github.com/RobTillaart/Max44009
#include "DHT.h"                  // https://github.com/adafruit/DHT-sensor-library
#include "Adafruit_MAX31855.h"    // https://github.com/adafruit/Adafruit-MAX31855-library
#include "RTClib.h"               // https://github.com/adafruit/RTClib
#include "FS.h"                   // https://randomnerdtutorials.com/esp32-microsd-card-arduino/
#include "SD.h"
#include "MovingAverageFloat.h"   // https://github.com/pilotak/MovingAverageFloat
#include "SparkFun_VL53L1X.h"     // http://librarymanager/All#SparkFun_VL53L1X
#include "Adafruit_MAX31855.h"    // https://github.com/adafruit/Adafruit-MAX31855-library
#include <Statistic.h>            // https://github.com/RobTillaart/Statistic (v1.0.0)
#include <Wire.h>                 // Introduit la librarie I2C
#include <SPI.h>                  // Introduit la libraire pour le SPI
#include <LoRa.h>                 // https://github.com/sandeepmistry/arduino-LoRa
#include <math.h>

// ----------------------------------------------------------------------------
// Define identifiant unique
// ----------------------------------------------------------------------------
#define STATION_ID              "TGP"

// ----------------------------------------------------------------------------
// Define version du programme
// ----------------------------------------------------------------------------
#define VERSION                 "3.0.0"

#define SUPPORTED_FRAME_VERION  0x02

// ----------------------------------------------------------------------------
// Define dégogage
// ----------------------------------------------------------------------------
#define DEBUG                   true

// ----------------------------------------------------------------------------
// Define adresses utilisées
// ----------------------------------------------------------------------------
#define ADR_MAX44009            0x4A

// ----------------------------------------------------------------------------
// Define broches et type capteurs
// ----------------------------------------------------------------------------
#define SEALEVELPRESSURE_HPA (1013.25)

#define PIN_DHT22               25
#define DHTTYPE                 DHT22

#define PIN_BATTERY             39       //Broche pour permettre de lire la tension d'entrée Vin (5Volts) du ESP32
#define AVERAGE_BUFFER_SIZE     5

#define PIN_VL53L1X_SHUTDOWN    2
#define PIN_VL53L1X_INTERRUPT   3

//Broche pour la girouette
#define PIN_WIND_DIRECTION      34

//Broche pour l'anémomètre
#define PIN_WIND_SPEED          35 // The pin location of the anemometer sensor

#define PIN_MAXCS   14
#define PIN_SCLK   18
#define PIN_MISO   19

//Paramètre de communication ESP32 et module RFM95:
#define PIN_LORA_SS             16
#define PIN_LORA_RST            4 
#define PIN_LORA_DIO0           17
//#define ss                      16
//#define rst                     4 
//#define dio0                    17

//Broche d'activation i2c
#define PIN_POW_EN              13
#define PIN_GPS_EN              12 
//#define WindSensorPin 35 // The pin location of the anemometer sensor

// ----------------------------------------------------------------------------
// Déclaration des objets
// ----------------------------------------------------------------------------
Adafruit_BMP3XX       bmp;
Max44009              gy(ADR_MAX44009);
DHT                   dht(PIN_DHT22, DHTTYPE);
RTC_DS3231            rtc;
MovingAverageFloat    <AVERAGE_BUFFER_SIZE> movA_VinExt;
SFEVL53L1X            vl;
Adafruit_MAX31855     tc(PIN_MAXCS);

// ----------------------------------------------------------------------------
// Objets statisticques
// ----------------------------------------------------------------------------
typedef statistic::Statistic<float,uint32_t,false> StatisticCAL;
StatisticCAL bmpTempStats;          // Température du BMP388
StatisticCAL bmpPresStats;          // Pression du BMP388
StatisticCAL bmpAltStats;          // Altitude du BMP388
/*
StatisticCAL temperatureIntStats;  // Internal temperature
StatisticCAL humidityIntStats;     // Internal humidity
StatisticCAL pressureIntStats;     // Internal pressure
StatisticCAL temperatureExtStats;  // External temperature
StatisticCAL humidityExtStats;     // External humidity
StatisticCAL solarStats;           // Solar radiation
StatisticCAL windSpeedStats;       // Wind speed
StatisticCAL uStats;               // Wind east-west wind vector component (u)
StatisticCAL vStats;               // Wind north-south wind vector component (v)
*/
// ----------------------------------------------------------------------------
// Coefficients du polynômes de degré 4 pour corriger les non linéarités du convertisseur ADC
// ----------------------------------------------------------------------------
//Valeurs obtenues à partir d'un étalonnage
const double coeff_A      = -0.000000000000016;
const double coeff_B      = 0.000000000118171;
const double coeff_C      = -0.000000301211691;
const double coeff_D      = 0.001109019271794;
const double coeff_E      = 0.034143524634089;

// ----------------------------------------------------------------------------
// Variable pour la tension d'entré de la batterie
// ----------------------------------------------------------------------------
uint16_t RawData_VinExt             = 0;
float Vin_VinExt                    = 0;
float moyenneVin_VinExt             = 0;
const float FACTEUR_CONV_VIN_EXT    = 2;
float tensionReelle_VinExt          = 0;

// ----------------------------------------------------------------------------
// Variable de communication LoRa
// Référence: https://www.baranidesign.com/faq-articles/2019/4/23/lorawan-usa-frequencies-channels-and-sub-bands-for-iot-devices
// ----------------------------------------------------------------------------

//const byte currentSupportedFrameVersion = 0x02;

byte destination              = 0xF0;      // destination to send to (gateway)
byte localAddress             = 0x05;     // address of this device
const uint32_t BAND           = 902500000;   //902.3MHz, Channel 2
const uint8_t LoRasyncWord    = 0x33; //Équivant à une valeur de 51 en décimale
//The spreading factor (SF) impacts the communication performance of LoRa, which uses an SF between 7 and 12. A larger SF increases the time on air, which increases energy consumption, reduces the data rate, and improves communication range. For successful communication, as determined by the SF, the modulation method must correspond between a transmitter and a receiver for a given packet.
const uint8_t LoRaSF          = 10;
const uint32_t LoRaSB         = 125E3;
const uint8_t LoRaCR          = 5;

// ----------------------------------------------------------------------------
// Variable globales de TimeOnAir
// ----------------------------------------------------------------------------
bool transmissionTerminee               = false;
uint32_t ToAStart                       = 0;
RTC_DATA_ATTR uint32_t  LoRaTimeOnAir   = 0; //Variable qui doit persister
RTC_DATA_ATTR uint16_t iterationRTC;

// ----------------------------------------------------------------------------
// Variable globales de la nomenclature du fichier .TXT
// ----------------------------------------------------------------------------
String path         = "/data.txt"; //--> Chemin emprunter pour enregistrer les données sur la carte SD.
// À intégrer dans la structure des données
String labelData    = "Unixtime, Vin, bmpTemperature, bmpPression, bmpAltitude, dhtHumidite, dhtTemperature, tcTemperature, gyLuminosite, distanceVL, GirDirVent, AnemomVitVent\n"; //--> Première ligne enregistrer sur la carte SD, représente l'ordre des valeurs.

// ----------------------------------------------------------------------------
// Variable globales du deep sleep
// ----------------------------------------------------------------------------
unsigned long timeSleep   = 10000000;  //--> 60 secondes de deep sleep (microsec)
unsigned long startTime   = 0;   //--> Permet de mesurer le temps d'exécution du programme
unsigned long stopTime    = 0;   //--> Permet de mesurer le temps d'exécution du programme
unsigned long totSleep    = timeSleep - (stopTime - startTime); //--> Durée total du Deep Sleep ajustée (microsec)
unsigned long pre_millis  = 0; //--> Initialisation de la variable utiliser pour le delay sans arrêt <No_Stop_Delay (NSD)>
unsigned long trigger_NSD = 1000; //--> Trigger pour indiquer au delay sans arrêt qui est temps d'effectuer une action. <No_Stop_Delay (NSD)>

// ----------------------------------------------------------------------------
// Variable globales de la girouette
// ----------------------------------------------------------------------------
unsigned long lastSensorAcquisition = millis();
unsigned long lastSentSensorAcquisition = millis();
int delai_capteur_lecture = 250;
int delai = 0;
unsigned int adc_WindDirectionValue = 0;


// ----------------------------------------------------------------------------
// Variable globales de l'anémomètre
// ----------------------------------------------------------------------------
volatile unsigned long lastInterrupt;
volatile unsigned int deltaTime ;
unsigned long timerDisplay            = millis();
int delayDisplay                      = 500;
volatile bool flagISR                 = false;
float rps                             = 0;
float lastrps                         = 1; // Permet d'avoir un affichage initiale lors du démarage du code

// ----------------------------------------------------------------------------
// Déclaration de variables globales
// ----------------------------------------------------------------------------
float bmpTemperatureC         = 0.0;
float bmpPressionHPa          = 0.0;
float bmpAltitude             = 0.0;   
int windDirection = 0; 
int windSpeed = 0;

// ----------------------------------------------------------------------------
// Structures et unions
// ----------------------------------------------------------------------------
// Structure de donnée à transmettre -- ici un test, à modifier selon le besoin.
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
    uint32_t unix;         //                                (4 bytes)
    int16_t  bmpTemperatureC;  //                                (2 bytes)   * 100
    int16_t  dhtTemperatureC;  //                                (2 bytes)   * 100
    int16_t  tcTemperatureC;   //                                (2 bytes)   * 100
    uint16_t bmpPressionHPa;   //                                (2 bytes)
    uint8_t  dhtHumidite;      //                                (1 byte)
    uint16_t vlDistanceMM;     //                                (2 bytes)
    uint16_t gy49LuminositeLux;//                                (2 bytes)
    int16_t  bmpAltitude;      //                                (2 bytes)
    uint16_t windDirection;    //                                (1 bytes)
    uint16_t windSpeed;//                                (2 bytes)
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

struct struct_online
{
  bool bmp388   = false;
  bool gy49 = false;
  bool dht22    = false;
  bool tc       = false;
  bool ds3231   = false;
  bool vl   = false;
  bool gnss     = false;
  bool microSd  = false;
} online;

// Structure to store function timers
struct struct_timer
{
  unsigned long readBmp;
  unsigned long readDht;
  unsigned long readTC;
  unsigned long readGY49;
  unsigned long readVL;
  /*
  unsigned long readBattery;
  unsigned long configMicroSd;
  unsigned long writeMicroSd;
  unsigned long readGnss;
  unsigned long readBme280;
  unsigned long readVeml7700;
  unsigned long readLsm303;
  unsigned long readHmp60;
  unsigned long readSht31;
  unsigned long read5103L;
  unsigned long read7911;
  unsigned long readDFRWS;
  unsigned long readSp212;
  unsigned long iridium;
  */
} timer;
/*
//Simple fonction pour assigner du data dans la structure
void fillInData(void) {
  moSbdMessage.frameVersion = SUPPORTED_FRAME_VERION;
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
  moSbdMessage.windSpeed = anemometreVitesseVent() * 100;
  moSbdMessage.Vin = lecture_VinExt() * 100;
  moSbdMessage.latitudeGPS = 5;
  moSbdMessage.longitudeGPS = 22;
  moSbdMessage.altitudeGPS = 332;
  moSbdMessage.satellites = 56;
  moSbdMessage.hdop = 5;
  moSbdMessage.transmitStatus = 1;
}

//-----------------------------------------------------------------------
*/

/*


void setup() {
  startTime = micros();
  Serial.begin(115200);
  //while (!Serial);
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
  Serial.println("Vitesse de vent de l'anémomètre = " + String(moSbdMessage.windSpeed));
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
*/

void setup(){
  
  startTime = micros();
  
  // Initialisation du port série
  Serial.begin(115200);

  // Broches d'activation I2C et GPS (Voir schémas électroniques)
  pinMode(PIN_GPS_EN, OUTPUT);
  pinMode(PIN_POW_EN, OUTPUT);
  digitalWrite(PIN_GPS_EN, HIGH);
  digitalWrite(PIN_POW_EN, HIGH);

  //Initialisation de l'anémomètre
  pinMode(PIN_WIND_SPEED, INPUT);
  attachInterrupt(digitalPinToInterrupt(PIN_WIND_SPEED), isr_rotation, FALLING);
  lastInterrupt = millis();
  
  //Initialisation de la girouette
  pinMode(PIN_WIND_DIRECTION, OUTPUT);
  digitalWrite(PIN_WIND_DIRECTION, HIGH);
  
  myDelay(500);

  // Initialisation du I2C
  Wire.begin();
  Wire.setClock(400000); // Set I2C clock speed to 400 kHz

  init_SD(path, labelData);
  init_RTC();
  
  loraConfig();


}

void loop(){

  Serial.println(ESP.getFreeHeap());
  readBmp388();
  readDHT22();
  readTC();
  readGY49();
  readVL();
  readWindDirection();
  readWindSpeed();
  readVin();
  readRTC();

  sendData(str_donnees(), path);

  transmitData();

  Serial.println(ESP.getFreeHeap());
  
  printAcquisition();
  
  esp_deep_sleep(totSleep); //--> Entre en mode deep sleep pour une durée de x microsecondes
  
}
