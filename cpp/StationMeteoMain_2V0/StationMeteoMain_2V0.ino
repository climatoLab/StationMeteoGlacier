/******************************************************************
   Nom: StationMeteoMain_2V0.ino
   Créer par: Vincent Lavallée
   Description: Programme de la Station Meteo Glacier en prévision
                pour la sortie au parc Yamaska.
                

******************************************************************/

#include "lib/StationMeteomain_2V0.h"//--> Librairie du fichier main.

String path = "/Yamaska_A2022.txt"; //--> Chemin emprunter pour enregistrer les données sur la carte SD.
String labelData = "Date, Time, Vin, bmpTemperature, bmpPression, bmpAltitude, dhtHumidite, dhtTemperature, tcTemperature, gyLuminosite\n"; //--> Première ligne enregistrer sur la carte SD, représente l'ordre des valeurs.

unsigned long timeSleep = 10; //--> Durée du Deep Sleep
unsigned long micInSec = 1000000; //--> Facteur de conversion de microsec en sec pour le Deep Sleep
unsigned long totSleep = timeSleep * micInSec; //--> Durée total du Deep Sleep en microsecondes.


unsigned long pre_millis = 0; //--> Initialisation de la variable utiliser pour le delay sans arrêt <No_Stop_Delay (NSD)>
unsigned long trigger_NSD = 10000; //--> Trigger pour indiquer au delay sans arrêt qui est temps d'effectuer une action. <No_Stop_Delay (NSD)>

void setup() {
  Serial.begin(115200);
  
  i2c_init_(); //--> initialisation des pins i2c.
  initLibs(path,labelData); //--> initialisation des objets.

  sendData(str_data(), path); //--> envoi des données vers la carte SD.
}

void loop() {
  unsigned long cur_millis = millis(); //--> indique la valeur actuel des millis() 
  if(cur_millis - pre_millis >= trigger_NSD){ //--> Condition que le trigger_NSD est respecter en rapport avec la différence des millis actuel et de l'ancienne boucle.
    pre_millis = cur_millis;  //--> ajustement inutile puisque le code se recompile (à voir)
    esp_deep_sleep(totSleep); //--> Entre en mode deep sleep pour une durée de x microsecondes
  }
}
