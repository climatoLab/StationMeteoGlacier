//=====================================================================
/*
Nom : ICM_20948_acc_magn_gyros

Auteur : Par Thomas Perras 

Source des librairies :
-Adafruit_GPS (GPS) : https://github.com/adafruit/Adafruit_GPS
-IridiumSBD (ROCKBLOCK) : https://github.com/sparkfun/SparkFun_IridiumSBD_I2C_Arduino_Library
-ICM_20948 (Accel + Magneto + Gyroscope) : https://github.com/sparkfun/SparkFun_ICM-20948_ArduinoLibrary
-Decodeur : https://github.com/TechnoPhysCAL/TGP_Decodeur
*/
//=====================================================================

//===========================================================================================================================================
//                                                                   GPS
//===========================================================================================================================================

#include <Adafruit_GPS.h>
#define GPSSerial Serial1

#define BROCHE_RX_GPS 26
#define BROCHE_TX_GPS 27

// Connectez-vous au GPS sur le port matériel
Adafruit_GPS GPS(&GPSSerial);

// Définissez GPSECHO à 'false' pour désactiver l'écho des données GPS sur la console série.
// Définissez GPSECHO sur 'true' si vous souhaitez déboguer et écouter les phrases GPS brutes.
#define GPSECHO false

unsigned long GPS_timer = millis();

boolean flagGps = false;


//===========================================================================================================================================
//                                                           ROCKBLOCK
//===========================================================================================================================================

#include <IridiumSBD.h>

#define RXD1_ROCKBLOCK 16
#define TXD1_ROCKBLOCK 17

#define IridiumSerial Serial1
#define DIAGNOSTICS false // Change this to see diagnostics

IridiumSBD modem(IridiumSerial);

  int signalQuality = -1;
  int err;


//===========================================================================================================================================
//                                                     Sparkfun 9dof ICM_20948 (Accel + Magneto)
//===========================================================================================================================================
#include "ICM_20948.h"
ICM_20948_I2C myICM;

#define AD0_VAL 1


//===========================================================================================================================================
//                                                      Magnetomètre
//===========================================================================================================================================
#define OFFSET 0    // utiliser s'il y a beaucoup d'intèredérences dans les parages

float magx, magy, magz;
//valeurs raw avant la calibration

//valeur maison
/*
  float MagOffset[3] = { -15.721, -30.841, -25.927}; // Offsets pour les axes x, y et z
  float mCal[3][3] =
  {
  { +1.013, +0.021, -0.008},                       // valeurs pour calibrer
  { +0.021, +0.948, -0.028},
  { -0.008, -0.028, +1.005}
  };*/


//valeur Poste 7 planche (sur chaise)
/*float MagOffset[3] = {-18.187257, -32.086183, -23.333950}; // Offsets pour les axes x, y et z
  float mCal[3][3] =
  {
  { +1.061059, +0.029164, -0.076460},                       // valeurs pour calibrer
  { +0.029164, +1.004469, -0.003997},
  { -0.076460, -0.003997, +0.865420}
  };

  float MagOffset[3] = { -19.204531, -32.316344, 2.003259}; // Offsets pour les axes x, y et z
  float mCal[3][3] =
  {
  { +0.980003, +0.025002, -0.056128},                       // valeurs pour calibrer
  { +0.025002, +1.032849, +0.014710},
  { -0.056128, +0.014710, +1.398257}
  };*/
/*
  float MagOffset[3] = { -17.717899, -31.014872, -30.214448}; // Offsets pour les axes x, y et z
  float mCal[3][3] =
  {
  { +1.053349, -0.016248, -0.003388},                       // valeurs pour calibrer
  { -0.016248, +1.002788, +0.004029},
  { -0.003388, +0.004029, +1.006423}
  };*/


float MagOffset[3] = { -11.871960, -34.420585, 6.682056}; // Offsets pour les axes x, y et z
float mCal[3][3] =
{
  { +1.060185, -0.016016, +0.120590},                       // valeurs pour calibrer
  { -0.016016, +1.034507, -0.072198},
  { +0.120590, -0.072198, +1.364939}
};


float magxc, magyc, magzc;           //valeurs calibrées

const int numReadings  = 10;
float readings_Magneto [numReadings];
int readIndex_Magneto  = 0;
float total_Magneto  = 0;
float average_Magneto = 0;

unsigned long Magneto_timer = millis();

boolean flagMagneto = false;

//===========================================================================================================================================
//                                                      Accéléromètre
//===========================================================================================================================================

float accx, accy, accz;

int readIndex  = 0;

float readings_pitch [numReadings];
float total_pitch  = 0;
float average_pitch = 0;

float readings_roll [numReadings];
float total_roll  = 0;
float average_roll = 0;

unsigned long Accel_timer = millis();

boolean flagAccel = false;

//===========================================================================================================================================
//                                                      Gyroscope
//===========================================================================================================================================
float gyrox, gyroy, gyroz;
int compteurGyro;

boolean flagGyroscope = false;


//===========================================================================================================================================
//                                                      TGP décodeur
//===========================================================================================================================================

#include <Decodeur.h>

Decodeur monDecodeur(&Serial);



#ifndef _STATION_H
#define _STATION_H

#define GPS_PIN 12
#define COMPLET_PIN 13
#define SATELITE 4

#endif


const int PIN_GPS_EN = 12; //Pin pour le GPS
const int PIN_POW_EN = 13; //Pin pour le 3V3
//===========================================================================================================================================
//                                                    Setup et loop
//===========================================================================================================================================

void setup() {
  pinMode(PIN_GPS_EN, OUTPUT); //Pour déterminer le mode de la pin du GPS pour le mode deep sleep de l'esp32 firebeetle
  pinMode(PIN_POW_EN, OUTPUT); //Pour déterminer le mode de la pin 3V3 pour le mode deep sleep de l'esp32 firebeetle
  digitalWrite(PIN_GPS_EN, HIGH); //Pour activer la pin du GPS hors du mode deep sleep de l'esp32 firebeetle
  digitalWrite(PIN_POW_EN, HIGH); //Pour activer la pin 3V3 hors du mode deep sleep de l'esp32 firebeetle
  
  Allume(COMPLET_PIN);
  Allume(GPS_PIN);
  Allume(SATELITE);

  Serial.begin(115200);
  init_GPS();
  init_ICM();
  init_Magneto();
  init_Accel();
  init_Rock();
  Serial.println("//----------------------------//");
  Serial.println("Taper a pour obtenir des mesures continues de l'accéléromètre\nTaper A pour arrêter les mesures continues de l'accéléromètre\nTaper m pour obtenir des mesures continues du magnétomètre\nTaper M pour arrêter les mesures continues du magnétomètre\nTaper g pour les mesures du GPS\nTaper G pour arrêter les mesures du GPS\nTaper d pour obtenir des mesures continues du gyroscope\nTaper D pour arrêter les mesures continues du gyroscope\nTaper s ou S pour restarter l'ESP32");
  Serial.println("//----------------------------//");
}

void loop() {

if(flagAccel == true){
  AccelSensor(&myICM);
}
else if(flagMagneto == true){
  MagnetoSensor(&myICM);
}
else if(flagGps == true){
  lecture_GPS();
}
else if(flagGyroscope == true){
  gyroscope(&myICM);
}

  if (Serial.available()) {
    if (monDecodeur.available()) {
      char commande = monDecodeur.getCommand();

      switch (commande) {

        case'a':
          flagAccel = true;
          break;

        case'A':
          flagAccel = false;
          Serial.println("\n\n//----------------------------//");
          Serial.println("Taper a pour obtenir des mesures continues de l'accéléromètre\nTaper A pour arrêter les mesures continues de l'accéléromètre\nTaper m pour obtenir des mesures continues du magnétomètre\nTaper M pour arrêter les mesures continues du magnétomètre\nTaper g pour les mesures du GPS\nTaper G pour arrêter les mesures du GPS\nTaper d pour obtenir des mesures continues du gyroscope\nTaper D pour arrêter les mesures continues du gyroscope\nTaper s ou S pour restarter l'ESP32");
          Serial.println("//----------------------------//");
          break;
          
        case'm': 
          flagMagneto = true;
          break;
          
        case'M':
          flagMagneto = false;
          Serial.println("\n\n//----------------------------//");
          Serial.println("Taper a pour obtenir des mesures continues de l'accéléromètre\nTaper A pour arrêter les mesures continues de l'accéléromètre\nTaper m pour obtenir des mesures continues du magnétomètre\nTaper M pour arrêter les mesures continues du magnétomètre\nTaper g pour les mesures du GPS\nTaper G pour arrêter les mesures du GPS\nTaper d pour obtenir des mesures continues du gyroscope\nTaper D pour arrêter les mesures continues du gyroscope\nTaper s ou S pour restarter l'ESP32");
          Serial.println("//----------------------------//");          
          break;

        case'g': 
          flagGps = true;
          break;

        case'G':
          flagGps = false;
          Serial.println("\n\n//----------------------------//");
          Serial.println("Taper a pour obtenir des mesures continues de l'accéléromètre\nTaper A pour arrêter les mesures continues de l'accéléromètre\nTaper m pour obtenir des mesures continues du magnétomètre\nTaper M pour arrêter les mesures continues du magnétomètre\nTaper g pour les mesures du GPS\nTaper G pour arrêter les mesures du GPS\nTaper d pour obtenir des mesures continues du gyroscope\nTaper D pour arrêter les mesures continues du gyroscope\nTaper s ou S pour restarter l'ESP32");
          Serial.println("//----------------------------//");          
          break;

        case'd':
          flagGyroscope = true;
          break;

        case'D':
          flagGyroscope = false;
          Serial.println("\n\n//----------------------------//");
          Serial.println("Taper a pour obtenir des mesures continues de l'accéléromètre\nTaper A pour arrêter les mesures continues de l'accéléromètre\nTaper m pour obtenir des mesures continues du magnétomètre\nTaper M pour arrêter les mesures continues du magnétomètre\nTaper g pour les mesures du GPS\nTaper G pour arrêter les mesures du GPS\nTaper d pour obtenir des mesures continues du gyroscope\nTaper D pour arrêter les mesures continues du gyroscope\nTaper s ou S pour restarter l'ESP32");
          Serial.println("//----------------------------//");          
          break;

        case'r': case'R':
          ESP.restart();
          break;

        /*case's': case'S': Code pour tester le décodeur
          if (monDecodeur.getArg(0) == 0) {
            Serial.print("Hey");
          }

          else if (monDecodeur.getArg(0) == 1) {

          }
          else if (monDecodeur.getArg(0) == 2) {

          } 
          break;*/

        default:
          break;
      }
    }
  }
}




//Accéléromètre===================================================================================================================================================================================================
void init_Accel() {
  myICM.setSampleMode((ICM_20948_Internal_Acc), ICM_20948_Sample_Mode_Continuous);

  ICM_20948_fss_t myFSS;
  //Ceci utilise une structure "Full Scale Settings" qui peut contenir des valeurs pour tous les capteurs configurables.
  myFSS.a = gpm2; // (ICM_20948_ACCEL_CONFIG_FS_SEL_e)
  // gpm2
  // gpm4
  // gpm8
  // gpm16


  ICM_20948_dlpcfg_t myDLPcfg;

  myICM.setFullScale((ICM_20948_Internal_Acc), myFSS);
  // Similaire à FSS, ceci utilise une structure de configuration pour les capteurs souhaités.
  myDLPcfg.a = acc_d473bw_n499bw; // (ICM_20948_ACCEL_CONFIG_DLPCFG_e)

  myICM.setDLPFcfg((ICM_20948_Internal_Acc), myDLPcfg);

  ICM_20948_Status_e accDLPEnableStat = myICM.enableDLPF(ICM_20948_Internal_Acc, false);
  Serial.println();
  Serial.println(F("Configuration complete!"));
}

void AccelSensor(ICM_20948_I2C * sensor)
{
  if (myICM.dataReady())
  {

    for (uint8_t i = 0; i <= 10; i++) {

      myICM.getAGMT(); // Les valeurs ne sont mises à jour que lorsque vous appelez 'getAGMT'.
      delay(10);
      accx = sensor->accX();
      accy = sensor->accY();
      accz = sensor->accZ();


      float pitch = 180 * atan2(accx, sqrt(accy * accy + accz * accz)) / PI;
      float roll = 180 * atan2(accy, sqrt(accx * accx + accz * accz)) / PI;


      total_pitch = total_pitch - readings_pitch[readIndex];
      total_roll = total_roll - readings_roll[readIndex];

      readings_pitch[readIndex] = pitch;
      readings_roll[readIndex] = roll;


      total_pitch = total_pitch + readings_pitch[readIndex];
      total_roll = total_roll + readings_roll[readIndex];

      readIndex = readIndex + 1;


      if (readIndex >= numReadings) {
        readIndex = 0;
      }
    }

    average_pitch = total_pitch / numReadings;
    average_roll = total_roll / numReadings;

    Serial.print("pitch : " + String(average_pitch));
    Serial.print("      ");
    Serial.print("roll : " + String(average_roll));
    Serial.println("      ");

  }
}





void Allume(int valeurModule) {

  pinMode(valeurModule, OUTPUT);
  digitalWrite(valeurModule, HIGH);

}


//GPS============================================================================================================================================================================================================
void init_GPS() {
  Serial1.begin(9600, SERIAL_8N1, BROCHE_RX_GPS, BROCHE_TX_GPS);
  Serial.println("> Initialisation du module GPS...");
  //9600 NMEA est le baud rate par défaut pour Adafruit MTK GPS
  //GPS.begin(9600);                                        //Connecter la pin RX à la pin 16 et la pin TX à la pin 17 du Firebeetle
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);           //Décommentez cette ligne pour allumer RMC et GGA
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);        //Décommentez cette ligne pour allumer seulement le RMC
  //Ajuste l'update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);              // 1 Hz update rate
  GPS.sendCommand(PGCMD_ANTENNA);                         //Demande l'état de l'antène. Commentez pour garder silence.
  delay(1000);
  GPSSerial.println(PMTK_Q_RELEASE);                      //Demande la version du firmware
  Serial.println("> Module GPS initialisé correctement.");
}

void lecture_GPS() {
  GPS_timer = millis() ; // réinitialiser le timer
  //Lit les données venant du GPS
  char c = GPS.read();
  //Cette partie est pour le débuggage
  if (GPSECHO)
    if (c) Serial.print(c);
  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences!
    // so be very wary if using OUTPUT_ALLDATA and trying to print out data
    Serial.print(GPS.lastNMEA()); // this also sets the newNMEAreceived() flag to false
    if (!GPS.parse(GPS.lastNMEA())) // this also sets the newNMEAreceived() flag to false
      return; // we can fail to parse a sentence in which case we should just wait for another
  }
  Serial.print("\nTime: ");
  if (GPS.hour < 10) {
    Serial.print('0');
  }
  Serial.print(GPS.hour, DEC); Serial.print(':');
  if (GPS.minute < 10) {
    Serial.print('0');
  }
  Serial.print(GPS.minute, DEC); Serial.print(':');
  if (GPS.seconds < 10) {
    Serial.print('0');
  }
  Serial.print(GPS.seconds, DEC); Serial.print('.');
  if (GPS.milliseconds < 10) {
    Serial.print("00");
  } else if (GPS.milliseconds > 9 && GPS.milliseconds < 100) {
    Serial.print("0");
  }
  Serial.println(GPS.milliseconds);
  Serial.print("Date: ");
  Serial.print(GPS.day, DEC); Serial.print('/');
  Serial.print(GPS.month, DEC); Serial.print("/20");
  Serial.println(GPS.year, DEC);
  Serial.print("Fix: "); Serial.print((int)GPS.fix);
  Serial.print(" quality: "); Serial.println((int)GPS.fixquality);
  if (GPS.fix) {
    Serial.print("Location: ");
    Serial.print(GPS.latitude, 4); Serial.print(GPS.lat);
    Serial.print(", ");
    Serial.print(GPS.longitude, 4); Serial.println(GPS.lon);
    Serial.print("Speed (knots): "); Serial.println(GPS.speed);
    Serial.print("Angle: "); Serial.println(GPS.angle);
    Serial.print("Altitude: "); Serial.println(GPS.altitude);
    Serial.print("Satellites: "); Serial.println((int)GPS.satellites);
  }
}

void init_ICM() {
  Wire.begin();
  Wire.setClock(400000);

  bool initialized = false;
  while (!initialized)
  {

    myICM.begin(Wire, AD0_VAL);

    Serial.print(F("Initialization of the sensor returned: "));
    Serial.println(myICM.statusString());
    if (myICM.status != ICM_20948_Stat_Ok)
    {
      Serial.println("Trying again...");
      delay(500);
    }
    else
    {
      initialized = true;
    }
  }
  Serial.println("Device connected!");

  // Ici, nous faisons une réinitialisation du logiciel pour nous assurer que le dispositif démarre dans un état connu.
  myICM.swReset();
  if (myICM.status != ICM_20948_Stat_Ok)
  {
    Serial.print(F("Software Reset returned: "));
    Serial.println(myICM.statusString());
  }
  delay(250);

  myICM.sleep(false);
  myICM.lowPower(false);
}

//Magnétomètre===========================================================================================================================================================================================================
void init_Magneto() {

  myICM.startupMagnetometer();
  if (myICM.status != ICM_20948_Stat_Ok)
  {
    Serial.print(F("startupMagnetometer returned: "));
    Serial.println(myICM.statusString());
  }

}


void MagnetoSensor(ICM_20948_I2C * sensor)
{
  float Rotation;
  if (myICM.dataReady())
  {

    for (uint8_t i = 0; i <= 9; i++) {
      delay(10);
      myICM.getAGMT();
      magx = sensor->magX();
      magy = sensor->magY();
      magz = sensor->magZ();

      /*
            Serial.print(i);
            Serial.print("     ");
            Serial.print(magx);
            Serial.print("     ");
            Serial.print(magy);
            Serial.print("     ");
            Serial.print(magz);
            Serial.println("         ");*/

      magxc = mCal[0][0] * (magx - MagOffset[0]) + mCal[0][1] * (magy - MagOffset[1]) + mCal[0][2] * (magz - MagOffset[2]);
      magyc = mCal[1][0] * (magx - MagOffset[0]) + mCal[1][1] * (magy - MagOffset[1]) + mCal[1][2] * (magz - MagOffset[2]);
      magzc = mCal[2][0] * (magx - MagOffset[0]) + mCal[2][1] * (magy - MagOffset[1]) + mCal[2][2] * (magz - MagOffset[2]);

      Rotation = OFFSET + (atan2(magyc, magxc) * 180) / M_PI;


      total_Magneto = total_Magneto - readings_Magneto[readIndex_Magneto];

      readings_Magneto[readIndex_Magneto] = Rotation ;

      total_Magneto = total_Magneto + readings_Magneto[readIndex_Magneto];

      readIndex_Magneto = readIndex_Magneto + 1;


      if (readIndex_Magneto >= numReadings) {
        readIndex_Magneto = 0;
      }
    }
  }

  average_Magneto = total_Magneto / numReadings;    // moyenne mobile

  if(Rotation > 180){
    Rotation = Rotation - 360;
  }
  else if (Rotation < -180) {
    Rotation = Rotation + 360;
  }

  if (Rotation < 150 && Rotation > -150) {

    Serial.print("Rotation : " + String(average_Magneto) + " °");
    Serial.print("      ");



    if (average_Magneto > -30 && average_Magneto <= 30) {
      Serial.print("Nord");
    }

    else if (average_Magneto < 60 && average_Magneto >= 30) {
      Serial.print("Nord-West");
    }

    else if (average_Magneto < 120 && average_Magneto >= 60) {
      Serial.print("West");
    }

    else if (average_Magneto < 150 && average_Magneto >= 120) {
      Serial.print("Sud-West");
    }


    else if (average_Magneto > -60 && average_Magneto <= -30) {
      Serial.print("Nord-Est");
    }

    else if (average_Magneto > -120 && average_Magneto <= -60) {
      Serial.print("Est");
    }

    else if (average_Magneto > -150 && average_Magneto <= -120) {
      Serial.print("Sud-Est");
    }
  }

  else {
    Serial.print("Rotation : " + String(Rotation) + " °");
    Serial.print("      ");
    Serial.print("Sud");

  }


  Serial.println();

}

//Gyroscope==================================================================================================================================================================================================================
void gyroscope(ICM_20948_I2C * sensor){
  int temps = millis();
  if(temps-compteurGyro>=10){
     myICM.getAGMT();
     gyrox = sensor->gyrX();
     gyroy = sensor->gyrY();
     gyroz = sensor->gyrZ();
     Serial.println("gyrox = " + String(gyrox) + " DPS; gyroy = " + String(gyroy) + " DPS; gyroz = " + String(gyroz) + " DPS");  
     compteurGyro = temps;
  } 
}

//Communication cellulaire===================================================================================================================================================================================================

void init_Rock() {

  Serial1.begin(19200, SERIAL_8N1, RXD1_ROCKBLOCK, TXD1_ROCKBLOCK);
  delay(500);

  Serial.println("Starting modem...");
  err = modem.begin();
  if (err != ISBD_SUCCESS)
  {
    Serial.print("Begin failed: error ");
    Serial.println(err);
    if (err == ISBD_NO_MODEM_DETECTED)
      Serial.println("No modem detected: check wiring.");
    return;
  }

  char version[12];
  err = modem.getFirmwareVersion(version, sizeof(version));
  if (err != ISBD_SUCCESS)
  {
    Serial.print("FirmwareVersion failed: error ");
    Serial.println(err);
    return;
  }

  Serial.print("Firmware Version is ");
  Serial.print(version);
  Serial.println(".");

  err = modem.getSignalQuality(signalQuality);
  if (err != ISBD_SUCCESS)
  {
    Serial.print("SignalQuality failed: error ");
    Serial.println(err);
    return;
  }

  Serial.print("On a scale of 0 to 5, signal quality is currently ");
  Serial.print(signalQuality);
  Serial.println(".");
}


void Send_Rock_Mag() {

  Serial.print("Trying to send the message.  This might take several minutes.\r\n");
  err = modem.sendSBDText("HI");
  if (err != ISBD_SUCCESS)
  {
    Serial.print("sendSBDText failed: error ");
    Serial.println(err);
    if (err == ISBD_SENDRECEIVE_TIMEOUT)
      Serial.println("Try again with a better view of the sky.");
  }

  else
  {
    Serial.println("Hey, it worked!");
  }
}
