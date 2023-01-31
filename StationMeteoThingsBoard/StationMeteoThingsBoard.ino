//--- Déclaration des librairies (en ordre alpha) -----------------------

#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <WiFi.h>
#include "ThingsBoard.h"
#include <math.h>
#include "pmodTC.h"
#include "bmpLib.h"
#include "VLlibrary.h"
#include "FonctionsGY49.h"
#include "librairiesDHT.h"
#include <TinyGPS++.h>
#include <Decodeur.h>
#include <Preferences.h>
#include <nvs_flash.h>
#include <ArduinoJson.h>
#include "SDLibrary.h"
#include "RTCLibrary.h"
#include "VinLibrary.h"
//Valeurs pour sauvegarder dans la mémoire flash de l'esp32 
Preferences mesPreferences;
//-----------------------------------------------------------------------

//--- Definitions -------------------------------------------------------
#define USE_WIFI_NINA false
#define USE_WIFI_NINA false
//#define TOKEN               "LczZzkdYmVjCQeQlMSp9" //dispositif : test "PlZKzu6z8lFPTYLgphTZ" //dispositif : Planche de test de Thomas Perras
//#define THINGSBOARD_SERVER  "192.168.1.10"
#define PIN_MQ135 A0
#define BROCHE_RX_GPS 27
#define BROCHE_TX_GPS 26
#define Version ""
#define modeAutomatique //Commenter pour utiliser le mode manuelle et enlever le commentaire pour utiliser le mode automatique
//-----------------------------------------------------------------------

//--- Declaration des objets, constantes, et variables globales----------

//Valeurs pour la connexion au réseau WiFi
//const char* ssid = "Dista";  // Enter SSID here CAL-Techno
//const char* pwd = "dista-wifi";  //Enter Password here technophys123
int timeout = 0;
boolean etat_connection = false;
boolean isSubcribed = false;  //Utilisé pour connaitre état de la connexion avec Thingsboard
WiFiClient espClient;
String wifiTest = mesPreferences.getString("SSID", "");
//WiFiSignal sig = WiFi.RSSI();
//float strength = sig.getStrength();

//Valeurs pour utiliser le menu géré par le décodeur
Decodeur monDecodeur(&Serial);
long previousMillis = 0; 
long interval = 60000;
char commande = NULL ;
const uint8_t STRING_LENGTH = 48;
const char ACK = '*';
const char NAK = '?';
uint32_t  Lecture, DELAI_LECTURE_CAPTEURS = 100; // Variables de la minuterie


//Valeurs pour la carte SD
String path = "/Yamaska_A2022.txt"; //--> Chemin emprunter pour enregistrer les données sur la carte SD.
String labelData = "Date, Time, Vin, bmpTemperature, bmpPression, bmpAltitude, dhtHumidite, dhtTemperature, tcTemperature, gyLuminosite\n"; //--> Première ligne enregistrer sur la carte SD, représente l'ordre des valeurs.


//Valeurs captées
float bmpTemperatureC, dhtTemperatureC, tcTemperatureC, bmpPressionHPa, gy49LuminositeLux, Vin, latitudeGPS, longitudeGPS, altitudeGPS;
uint16_t bmpTemperatureIntC, dhtTemperatureIntC, tcTemperatureintC;
uint8_t dhtHumidite;
uint16_t vlDistanceMM;


//Valeurs pour le GY-49
Max44009 myLux(0x4A);


//Valeurs pour le protocole MQTT
ThingsBoard tb(espClient);
uint16_t NbrMessagesEnvoyes;


//Valeurs pour le compteur
unsigned long lastMillis;
const uint16_t msgInterval = 5000; //Mettre à 60000 pour une minute


//Valeurs pour le format des données en Json
String message;
uint16_t dataSize;
String vir = ",";


//Valeurs pour activer la broche 3V3 et celle du GPS
const int PIN_GPS_EN = 12; //Pin pour le GPS
const int PIN_POW_EN = 13; //Pin pour le 3V3


//Valeurs pour le GPS
uint16_t GPSBaud = 9600;
TinyGPSPlus gps;

//Valeurs pour ThingsBoard
bool flagCtemperatures, flagHPaBmp, flagEnvoie, flagEnvoieAuto, flagHdht, flagMontreheureRtc, flagEcritSd, flagGy49, flagVin, flagMesures, flagGirouette;

//Délais
uint16_t compteurTB;
uint16_t CompteurC, bmpCompteurHPa, gy49CompteurLux, VinCompteur, gpsCompteur, dhtCompteurHum, vlComptpeurMM, sdCompteur;

//Valeurs pour la carte micro SD
uint16_t NbrEcritures;

//Valeurs pour le pluviomètre
const uint8_t PCF_I2C_ADDR = 0x50; //Adresse I2C du RTC PCF8583
uint16_t CompteurPluie;

//Gyrouette
unsigned long derniereMESURECAPTEURS = millis();
unsigned long derniereEnvoieMESURECAPTEURS = millis();
int delai_capteur_lecture = 250;
int delai = 0;
int valpot;// valeur analogique du potentiomètre
int Pcardinaux;


//Mode deep sleep
unsigned long timeSleep = 60; //--> Durée du Deep Sleep (sec)
unsigned long micInSec = 1000000; //--> Facteur de conversion de microsec en sec pour le Deep Sleep
unsigned long totSleep = timeSleep * micInSec; //--> Durée total du Deep Sleep en microsecondes.
unsigned long pre_millis = 0; //--> Initialisation de la variable utiliser pour le delay sans arrêt <No_Stop_Delay (NSD)>
unsigned long trigger_NSD = 5000; //--> Trigger pour indiquer au delay sans arrêt qui est temps d'effectuer une action. <No_Stop_Delay (NSD)>
unsigned long compteurSleep;
unsigned long stationSleep = 10000;


//Initialiser la communication i2c
void i2c_init_(const int PIN_POW_EN = 13) { //--> initialisation de l'i2c
  pinMode(PIN_POW_EN, OUTPUT);
  digitalWrite(PIN_POW_EN, HIGH);
  delay (500);
}


//--- Setup et Loop -----------------------------------------------------
void setup() {
  
  Serial.begin(115200);
  //Serial1.begin(GPSBaud, SERIAL_8N1, BROCHE_RX_GPS, BROCHE_TX_GPS);
  mesPreferences.begin("maConfig", false);
  //pinMode(PIN_GPS_EN, OUTPUT);
  //digitalWrite(PIN_GPS_EN, HIGH);
  i2c_init_();
  init_RTC();  
  BMP3XX_init();
  init_Humidite_DHT();
  init_VL();
  init_VinExt();
  init_SD(path, labelData);
  //Configure le PCF8583 en mode compteur :
  PCF8583_config_cntr(PCF_I2C_ADDR);
  //Efface le compteur au démarrage
  PCF8583_clr_cntr(PCF_I2C_ADDR);
  //Pour activer la gyrouette  
  pinMode(34, OUTPUT);
  digitalWrite(34, HIGH);
  menu();    
  //flagEnvoie = true; //Pour envoyer des données au serveur de ThingsBoard et les afficher sur le moniteur série
  #ifdef modeAutomatique
  Wifi_begin(); //Se connecte au réseau WiFi préconfiguré par ses paramètres sauvegardés dans la mémoire Flash de l'ESP32
  Serial.println("Initializing TB connection");
  delay(1000);
  connecteTB(); //Se connecte au serveur ThingsBoard préconfiguré par ses paramètres sauvegardés dans la mémoire Flash de l'ESP32
  delay(1000);
  flagEnvoieAuto = true;
  flagEnvoie = false; //Envoie les données vers ThingsBoard
  sendData(str_data(), path); 
  #endif
  
}//Fin de setup()

void loop() {
  tb.loop();
  decodage();

  bmpTemperatureC = bmpTemp()*10;
  bmpPressionHPa = bmpPression();// / 100.0F / 10);
  dhtHumidite = dhtHumi();
  dhtTemperatureC = dhtTemp()*10;
  gy49LuminositeLux = gyLux(); 
  tcTemperatureC = tempTC()*10;
  vlDistanceMM = distanceVL();
  Vin = lecture_VinExt();
  //latitudeGPS = gps.location.lat();
  //longitudeGPS = gps.location.lng();
  //altitudeGPS = gps.altitude.meters();

  bmpTemperatureC = (round(bmpTemperatureC))/10;
  dhtTemperatureC = (round(dhtTemperatureC))/10;
  tcTemperatureC = (round(tcTemperatureC))/10;
  
  if(flagCtemperatures == true){
    TemperaturesC();  
  }
  else if(flagEnvoie == true){
    envoieMesures();   
  }
  else if(flagHPaBmp == true){
    PressionKPa();   
  }  
  else if(flagHdht == true){
    humditePourcentage();   
  }  
  else if(flagMontreheureRtc == true){
    rtcMontreheure(); 
  }    
  else if(flagEcritSd == true){
    sdEcritDonnees();   
  }  
  else if(flagGy49 == true){
    luminosite();   
  }  
  else if(flagVin == true){
    VIN();   
  } 
  else if(flagMesures == true){
    MesuresTotales();   
  }
  else if(flagGirouette == true){
    girouette(); 
  }

  #ifdef modeAutomatique 
  unsigned long cur_millis = millis(); //--> indique la valeur actuel des millis() 
  if(cur_millis - pre_millis >= trigger_NSD){
    envoieTemperaturesC();
    envoiePressionKPa(); 
    envoiehumditePourcentage(); 
    envoieLuminosite();  
    envoieVin(); 
    envoieDistance();
    envoiePluviometre();
    envoieRSSI();
    envoieGirouette();
    NbrMessagesEnvoyes++;
    Serial.println("Nombre de messages envoyés = " + String(NbrMessagesEnvoyes)+ "\n");
    pre_millis = cur_millis;  //--> ajustement inutile puisque le code se recompile (à voir)
    if(cur_millis - compteurSleep >= stationSleep){ //--> Condition que le trigger_NSD est respecter en rapport avec la différence des millis actuel et de l'ancienne boucle.
      esp_deep_sleep(totSleep); //--> Entre en mode deep sleep pour une durée de x microsecondes
      compteurSleep = cur_millis;
    }   
   }
  #endif
}

//Fin de loop()
//-----------------------------------------------------------------------
