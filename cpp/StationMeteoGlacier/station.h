#ifndef _STATION_H
#define _STATION_H
#include <driver/rtc_io.h>

gpio_num_t BROCHE_DEL_BLEU = GPIO_NUM_2;
//================================================================================================================================
//                                              Version de la Station météo
//================================================================================================================================
const String VERSION = "0.1.0";

//================================================================================================================================
//                              Préférences pour accéder au credentials stocké dans la mémoire FLASH
//================================================================================================================================
#include <Preferences.h>      //https://RandomNerdTutorials.com/esp32-save-data-permanently-preferences/
Preferences preferences;
String WIFI_SSID;
String WIFI_PASSWD;
String PRIVATE_URL;

//================================================================================================================================
//                                              Bibliothèque générale
//================================================================================================================================

#include <Wire.h>
#include <SPI.h>
//================================================================================================================================
//                                              Moyenne mobile
//================================================================================================================================
#include "MovingAverageFloat.h"  // https://github.com/pilotak/MovingAverageFloat
const uint16_t AVERAGE_BUFFER_SIZE = 5;

//La profondeur de la moyenne mobile est de 16 points. La place en mémoire occupée est de : 16 * sizeof(float) = 64 octets de mémoire RAM
MovingAverageFloat <AVERAGE_BUFFER_SIZE> movA_accel_X;
MovingAverageFloat <AVERAGE_BUFFER_SIZE> movA_accel_Y;
MovingAverageFloat <AVERAGE_BUFFER_SIZE> movA_accel_Z;
MovingAverageFloat <AVERAGE_BUFFER_SIZE> movA_vitVent;
MovingAverageFloat <AVERAGE_BUFFER_SIZE> movA_Distance;
MovingAverageFloat <AVERAGE_BUFFER_SIZE> movA_Girouette;
MovingAverageFloat <AVERAGE_BUFFER_SIZE> movA_VinExt;
MovingAverageFloat <AVERAGE_BUFFER_SIZE> movA_Vin_sol;

//================================================================================================================================
//                                                   Décodeur
//================================================================================================================================
#include <Decodeur.h>           // Recherche TGP Decodeur dans le Gestionnaire des Bibliothèque
Decodeur monDecodeur(&Serial);
char commande = NULL ;
//================================================================================================================================
//                                             Définitions des broches
//================================================================================================================================
#define POWER_ON_CAPTEUR 13     //Broche pour activer la tension d'alimentation 3V3_SW
#define POWER_ON_GPS 12         //Broche pour activer la tension d'alimentation 3V3_GPS pour alimenter le GPS uniquement
#define BROCHE_ANEMOMETRE 35    //Broche pour mesurer la vitesse du vent (anémomètre)
#define BROCHE_DHT 25           //Broche pour le capteur DHT22 (température + humidité)
#define BROCHE_GIROUETTE 34     //Broche pour permettre de mesurer la direction du vent
#define BROCHE_SATELLITE 4      //Broche pour activer l'alimentation du module Satellite
#define BROCHE_SD 15            //Signal de chip select pour le lecteur de carte SD
#define BROCHE_THERMOCOUPLE 14  //Signal de chip select pour le module PMODE Thermocouple  
#define BROCHE_VIN_EXT 39       //Broche pour permettre de lire la tension d'entrée Vin (5Volts) du ESP32
#define BROCHE_VIN_SOL 36       //Broche pour permettre de lire la tension d'entrée Vin du panmneau solaire (6Volts)
#define BROCHE_SEL_MODE 5       //Broche pour le sélecteur de mode de fonctionnement de la Station météo
#define BROCHE_RX_GPS 27        //Broche de réception de données du module GPS
#define BROCHE_TX_GPS 26        //Broche de transmission de données du module GPS
//================================================================================================================================
//                                                  Deep sleep
//================================================================================================================================
#include "driver/rtc_io.h"
#define uS_TO_S_FACTOR 60000000       //Facteur de conversion de micro secondes à minutes (60 000 000 us) 
#define TIME_TO_SLEEP  30              //Temps pendant lequel l'ESP32 sera en Deep Sleep (en minutes)
RTC_DATA_ATTR int bootCount = 0;      //Compteur pour indiquer le nombre de fois que le ESP32 s'est réveillé.

/*
   Variable MODE pour connaître le mode de fonctionnement de la station météo
   False :
   - Mode avec alimentation murale (externe de 5V) et branchement à un ordinateur (USB)
   ou
   - Mode avec alimentation murale (externe de 5V) et sans branchement à un ordinateur (WiFi ou Bluetooth)

   True :
   - Mode sans alimentation murale, sans batterie et avec un branchement à un ordinateur (USB)
   ou
   - Mode sans alimentation murale, avec batterie et avec un branchement à un ordinateur (USB)
   ou
   - Mode sans alimentation murale, avec batterie et sans branchement à un ordinateur (WiFi ou Bluetoth)
   (Mode pour le Deep Sleep)
*/
RTC_DATA_ATTR bool etat_sel_mode = false;
//================================================================================================================================
//                                                  Minuterie
//================================================================================================================================
//Pour la lecture des capteurs sur une base régulière
unsigned long minuterie_Lecture_capteurs = millis();
const int DELAI_LECTURE_CAPTEURS = 1000;
bool modeContinu = false;

//================================================================================================================================
//                                                 Accéléromètre
//================================================================================================================================
#include <Adafruit_Sensor.h>                             // https://www.arduino.cc/reference/en/libraries/adafruit-unified-sensor/
#include "Adafruit_LSM303_Accel.h"                       // https://github.com/adafruit/Adafruit_LSM303_Accel                            

//Définition d'une onde sinusoidale qui représente la variation de l'accélération gravitionnelle
float xAmp = 9.806;       // Amplitude max de la composante X
float yAmp = 9.806;       // Amplitude max de la composante Y

//Initialise les variables d'angles
float xAng=0.0;
float yAng=0.0;

//Initialise les variables d'accelerations en X Y Z
float x, y, z;

//Initialise les variables de convertion en degres en X Y Z
float moy_accel_X, moy_accel_Y, moy_accel_Z;

/* Assigne un ID unique à ce capteur en même temps */
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(54321);
//================================================================================================================================
//                                                  Anémomètre
//================================================================================================================================
#include <math.h>
volatile unsigned long lastInterrupt;
volatile unsigned int deltaTime;
float rps = 0;
float WindSpeed; // speed miles per hour
//================================================================================================================================
//                Coefficients du polynômes de degré 4 pour corriger les non linéarités du convertisseur ADC
//================================================================================================================================
//Valeurs obtenues à partir d'un étalonnage
const double coeff_A = -0.000000000000016;
const double coeff_B = 0.000000000118171;
const double coeff_C = -0.000000301211691;
const double coeff_D = 0.001109019271794;
const double coeff_E = 0.034143524634089;

//================================================================================================================================
//                                                   Tension d'entrée Vin
//================================================================================================================================
uint16_t RawData_VinExt = 0;
float Vin_VinExt = 0;
float moyenneVin_VinExt = 0;
const float FACTEUR_CONV_VIN_EXT = 2;
float tensionReelle_VinExt = 0;

//================================================================================================================================
//                                                   Tension d'entrée du panneau solaire
//================================================================================================================================
uint16_t RawData_VinSol = 0;
float Vin_VinSol = 0;
float moyenneVin_VinSol = 0;
const float FACTEUR_CONV_VIN_SOL = 2;
float tensionReelle_VinSol = 0;

//================================================================================================================================
//                                               Capteur de distance
//================================================================================================================================
#include "SparkFun_VL53L1X.h"                   // https://github.com/adafruit/Adafruit_LSM303_Accel
SFEVL53L1X distanceSensor;
int moyenne_Distance = 0;                       //Moyenne
//================================================================================================================================
//                                                   Module GPS
//================================================================================================================================
#include <Adafruit_GPS.h>
#define GPSSerial Serial1
#define GPSECHO false                       //Utile pour le débuggage. Voir le fichier teste du GPS
Adafruit_GPS GPS(&GPSSerial);
float flat=0.0, flon=0.0, alt=0.0;
int nb_stats;
uint32_t timer = millis();
unsigned long age;
//================================================================================================================================
//                                                    Girouette
//================================================================================================================================
//float Direction[] = {2.60, 1.54, 1.68, 0.76, 0.78, 0.72, 1.00, 0.86, 1.25, 1.14, 2.14, 2.06, 3.08 , 2.73, 2.91, 2.36};
//char pole[17][4] = {"N", "NNE", "NE", "ENE", "E", "ESE", "SE", "SSE", "S", "SSO", "SO", "OSO", "O", "ONO", "NO", "NNO"};
float degree = 0;
uint16_t RawData_girouette = 0;
String orientation ;
//================================================================================================================================
//                                                Capteur d'humidité
//================================================================================================================================
#include "DHT.h"                               // https://github.com/adafruit/DHT-sensor-library
#define DHTTYPE DHT22
DHT dht(BROCHE_DHT, DHTTYPE);
float humidite = 0;
//================================================================================================================================
//                                              Capteur de luminosité
//================================================================================================================================
#include <Max44009.h>                          // https://www.arduino.cc/reference/en/libraries/max44009-library/
Max44009 light(0x4A);
float lux = 0;
//================================================================================================================================
//                                                   Pluviomètre
//================================================================================================================================
#include "PCF8583.h"                           // https://github.com/edebill/PCF8583
PCF8583 counter(0xA0);
RTC_DATA_ATTR volatile float pluie_24H = 0.0;
RTC_DATA_ATTR volatile float pluie_H = 0.0;
RTC_DATA_ATTR volatile int nb_Heure = 0;
RTC_DATA_ATTR int quart_dHeure = 1;
RTC_DATA_ATTR int boot = 0;

int nb_Basculement = 0;
float pluie = 0;
const float basculement_mm = 0.2794;
//================================================================================================================================
//                                               Capteur de pression
//================================================================================================================================
#include "Adafruit_BMP3XX.h"                  // https://github.com/adafruit/Adafruit_BMP3XX
#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BMP3XX bmp;
float pression = 0;
float Altitude = 0;                           //Cette variable commence avec une lettre majuscule puisque le version minuscule existe déjà
boolean etat_pression = false;
//================================================================================================================================
//                                                       RTC
//================================================================================================================================
#include "RTClib.h"                           // https://github.com/adafruit/RTClib
RTC_DS3231 myRTC;
int A, M, J, H, m, S;
uint16_t annee, mois, jour, heures, minutes, secondes;
String HEURES, MINUTES, SECONDES, JOURS, MOIS, ANNEE, DATE, TIME;
//================================================================================================================================
//                                                    Carte SD
//================================================================================================================================
#include "FS.h"
#include "SD.h"
String donnees_SD = "";
//String labelsDonnees_SD = "Date;Heure;Temperature;Humidite;Pression;luminosite;vitesse;direction;pluie_h;pluie_24h;distance;inclinaison_x;inclinaison_y;latitude;longitude;Altitude;Vin\r\n";
String labelsDonnees_SD = "Date;Heure;Humidite;luminosite;Pression;Temperature;VinExt;VinSol;vitesse;orientation\r\n";
//================================================================================================================================
//                                                Module Satellite (À compléter)
//================================================================================================================================
#include <IridiumSBD.h>                       // https://github.com/mikalhart/IridiumSBD
#define IridiumSerial Serial2
IridiumSBD modem(IridiumSerial, BROCHE_SATELLITE);
int err = 0;
int signalQuality = -1;
//================================================================================================================================
//                                                  Thermocouple
//================================================================================================================================
#include "Adafruit_MAX31855.h"                // https://github.com/adafruit/Adafruit-MAX31855-library
Adafruit_MAX31855 thermocouple(BROCHE_THERMOCOUPLE);
double temperature = 0;
//================================================================================================================================
//                                                  Module Wifi
//================================================================================================================================
#include <WiFi.h>
#include <HTTPClient.h>
const uint16_t WIFI_RETRY_CNTR = 20;  //Constante pour indiquer le nombre de tentatives permises au réseau WiFi
//================================================================================================================================
//                                                  JSON
//================================================================================================================================
#include <ArduinoJson.h>              // https://github.com/bblanchon/ArduinoJson
const size_t capacity = JSON_ARRAY_SIZE(7) + 7 * JSON_OBJECT_SIZE(2) + 128;
#endif
