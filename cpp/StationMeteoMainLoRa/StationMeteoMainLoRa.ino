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
    //*** v3.1.0 : Ajout des Statistiques
    //*** v3.2.0 : Ajout capteur de pluie
    //*** v3.3.0 : Ajout du GPS
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
#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>
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
#define VERSION                 "3.3.0"

#define SUPPORTED_FRAME_VERION  0x02

// ----------------------------------------------------------------------------
// Define dégogage
// ----------------------------------------------------------------------------
#define DEBUG                   true

// ----------------------------------------------------------------------------
// Define adresses utilisées
// ----------------------------------------------------------------------------
#define ADR_MAX44009            0x4A
#define PCF_I2C_ADDR            0x50

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

//Broches pour la communication GPS
#define PIN_GPS_TX              27
#define PIN_GPS_RX              26

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

SoftwareSerial mySerial(PIN_GPS_TX, PIN_GPS_RX);
Adafruit_GPS GPS(&mySerial);

// ----------------------------------------------------------------------------
// Objets statistiques
// ----------------------------------------------------------------------------
typedef statistic::Statistic<float,uint32_t,false> StatisticCAL;
StatisticCAL bmpTempStats;          // Température du BMP388
StatisticCAL bmpPresStats;          // Pression du BMP388
StatisticCAL bmpAltStats;           // Altitude du BMP388
StatisticCAL dhtTempStats;          // Température du DHT22
StatisticCAL dhtHumStats;           // Humidité du DHT22
StatisticCAL tcTempStats;           // Température du Thermocouple
StatisticCAL gyLuxStats;            // Luminosité du GY49
StatisticCAL vlDisStats;            // Distance du VL
StatisticCAL windDirectionStats;    // Direction de la girouette
StatisticCAL windSpeedStats;        // Vitesse de l'anémomètre
StatisticCAL voltageStats;          // Altitude du BMP388
StatisticCAL rainCountStats;          // Altitude du BMP388

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
// Variables de contrôles
// ----------------------------------------------------------------------------
  uint16_t espSleepTime           = 10; // Temps du deep sleep en secondes
  unsigned int  averageInterval   = 6 ; // Nombres de données à réaliser la moyenne.
  uint16_t timeout_LoRa           = 20; // Timeout du LoRa en secondes (Défaut : 20)
  uint16_t timeout_GPS            = 20; // Timeout du LoRa en secondes (Défaut : 20)
  
// ----------------------------------------------------------------------------
// Variable globales du deep sleep
// ----------------------------------------------------------------------------
unsigned long micro2sec   = 1000000;  //--> Variable de convertion de micro sec à sec.
unsigned long millis2sec  = 1000;     //--> Variable de convertion de millis sec à sec.
unsigned long sec2hour    = 3600;
unsigned long startTime   = 0;   //--> Permet de mesurer le temps d'exécution du programme
unsigned long stopTime    = 0;   //--> Permet de mesurer le temps d'exécution du programme
unsigned long totSleep    = (espSleepTime * micro2sec) - (stopTime - startTime); //--> Durée total du Deep Sleep ajustée (microsec)
unsigned long pre_millis  = 0; //--> Initialisation de la variable utiliser pour le delay sans arrêt <No_Stop_Delay (NSD)>
unsigned long trigger_NSD = 1000; //--> Trigger pour indiquer au delay sans arrêt qui est temps d'effectuer une action. <No_Stop_Delay (NSD)>
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

// Pour le NBD <Non-Blocking Delay> du LoRa timeout.
unsigned long previousMillis_LoRa = 0;
unsigned long interval_LoRaTimeout = timeout_LoRa * millis2sec;

// ----------------------------------------------------------------------------
// Variable globales du GPS
// ----------------------------------------------------------------------------
// Pour le NBD <Non-Blocking Delay> du GPS timeout.
unsigned long previousMillis_GPS = 0;
unsigned long interval_GPSTimeout = timeout_GPS * millis2sec;

// ----------------------------------------------------------------------------
// Variable globales de TimeOnAir
// ----------------------------------------------------------------------------
bool transmissionTerminee               = false;
uint32_t ToAStart                       = 0;
RTC_DATA_ATTR uint32_t  LoRaTimeOnAir   = 0; //Variable qui doit persister
RTC_DATA_ATTR uint16_t iterationRTC;
RTC_DATA_ATTR uint16_t iterationRTC_LoRa;
RTC_DATA_ATTR uint16_t sampleCounter;

// ----------------------------------------------------------------------------
// Variable globales de la nomenclature du fichier .TXT
// ----------------------------------------------------------------------------
const char* path         = "/data.txt"; //--> Chemin emprunter pour enregistrer les données sur la carte SD.
// À intégrer dans la structure des données
String labelData    = "Unixtime, Vin, bmpTemperature, bmpPression, bmpAltitude, dhtHumidite, dhtTemperature, tcTemperature, gyLuminosite, distanceVL, GirDirVent, AnemomVitVent\n"; //--> Première ligne enregistrer sur la carte SD, représente l'ordre des valeurs.

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
// Variable globales du Rain Gauge
// ----------------------------------------------------------------------------
const float rain_CF                   = 0.2794; // [mm/cmpt] // facteur de conversion pour tranformer le nombre de comptes en mm.
float rainCount                    = 0;  
uint16_t rainIndex                    = 6;

RTC_DATA_ATTR uint16_t iterationRTC_rain;
uint16_t iterationPerHour_rain = espSleepTime / sec2hour;
// ----------------------------------------------------------------------------
// Déclaration de variables globales
// ----------------------------------------------------------------------------
float bmpTemperatureC           = 0.0;
float bmpPressionHPa            = 0.0;
float bmpAltitude               = 0.0;  
float dhtTemperatureC           = 0.0;
int dhtHumidite                 = 0;
float tcTemperatureC            = 0.0;
unsigned long gy49LuminositeLux = 0;
unsigned int vlDistanceMM       = 0;
float Vin                       = 0.0; 
int windDirection               = 0; 
int windSpeed                   = 0;
float latitudeGPS = 0.0;
float longitudeGPS =0.0;
float altitudeGPS =0.0;
uint8_t satellites =  0;
   

// ----------------------------------------------------------------------------
// Drapeau booléen
// ----------------------------------------------------------------------------
bool flag_initSD = false;
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
    uint16_t windSpeed;//                                        (2 bytes)
    uint16_t rainHeight;       //                                (2 bytes)
    uint16_t Vin;              //                                (2 byte) * 100
    int32_t  latitudeGPS;      //                                (4 byte)
    int32_t  longitudeGPS;     //                                (4 byte)
    int32_t  altitudeGPS;      //                                (4 byte)
    uint8_t  satellites;       // # of satellites                (1 byte)
    uint16_t hdop;             // HDOP                           (2 bytes)
    uint16_t transmitDuration; // Previous transmission duration (2 bytes)
    uint8_t  transmitStatus;   // Iridium return code            (1 byte)
    uint16_t iterationCounter; // Message counter                (2 bytes)
  } __attribute__((packed));                           // Total: (50 bytes)
  uint8_t bytes[50];
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
  bool lora = false;
  bool gps = false;
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

  if (flag_initSD){
    
    Serial.println(ESP.getFreeHeap());
    updateIteration();
    readBmp388();
    readDHT22();
    readTC();
    readGY49();
    readVL();
    readWindDirection();
    readWindSpeed();
    readRain();
    readVin();
    readRTC();
    readGPS();
    Serial.print(String(online.gps));
    
    Serial.println(sampleCounter);
  
    sendData(str_donnees(), path);
    printAcquisition();
  
    // Check if number of samples collected has been reached and calculate statistics (if enabled)
    if ((sampleCounter == averageInterval))
    {
      updateIterationLoRa();
      calculateStats(); // Calcul des statistiques
      transmitData(); // Transmet via LoRa
      printStats();
      sampleCounter = 0; // Reset sample counter
    }

    
    
    lastLine(SD, path);
  
    printSD_Info();

    Serial.println(ESP.getFreeHeap());
  
    sampleCounter++;
  }

    
  esp_deep_sleep(totSleep); //--> Entre en mode deep sleep pour une durée de x microsecondes
  
}
