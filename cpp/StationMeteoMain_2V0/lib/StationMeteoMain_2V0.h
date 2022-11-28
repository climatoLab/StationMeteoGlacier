/******************************************************************
   Nom: StationMeteoMain_2V0.h
   Créer par: Vincent Lavallée, Annabelle Lambert, Raphael Richard
   Description: Librarie créer par les stagiaires de recherche de
                Station météo pour permettre l'utilisation de la
                Station Météo Glacier sur le terrain et pour des essaie

   Type de données :
                    - Date et heures (Séparé) --> Station rouge avec temps réel, pas la blanche
                    - Tension de la batterie / Vin (V)
                    - Température (C)  // Non-Calibré
                    - Pression (Pa)    // Non-Calibré
                    - Altitude (m)     // Non-Calibré
                    - Humidité (%)     // Non-Calibré
                    - Luminosité (LUX) // Non-Calibré

   Liens :
          -
          -

******************************************************************/
//Libraries créée par les stagiaires.
#include "FonctionsGY49.h"
#include "RTCLibrary.h"
#include "SDLibrary.h"
#include "VinLibrary.h"
#include "bmpLib.h"
#include "librairiesDHT.h"
#include "pmodTC.h"
//------------------------------------
String vir = ","; //--> simplification

void initLibs(String path, String labelData){ //--> initiation de tous les objets
  init_RTC();
  init_SD(path, labelData);
  init_VinExt();
  BMP3XX_init();
  init_Humidite_DHT();
}

String str_data(){ //--> Met dans une variable String la structure de nos données
  String s_msg = getDateRTC() + vir
               + getTimeRTC() + vir
               + String(lecture_VinExt()) + vir
               + String(bmpTemp()) + vir
               + String(bmpPression()) + vir
               + String(bmpAltitude()) + vir
               + String(dhtHumi()) + vir
               + String(dhtTemp()) + vir
               + String(tempTC()) + vir
               + String(gyLux()) + "\n";
return s_msg;              
}

void i2c_init_(const int PIN_POW_EN = 13) { //--> initialisation de l'i2c

  pinMode(PIN_POW_EN, OUTPUT);
  digitalWrite(PIN_POW_EN, HIGH);
  delay (500);
}
