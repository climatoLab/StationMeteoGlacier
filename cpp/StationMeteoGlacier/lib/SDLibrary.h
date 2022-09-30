#ifdef DEBUG
#include "SPI.h"
#endif

#include "FS.h"
#include "SD.h"
#include "fonctionsSD.h"

String donneesSD = "";

void init_SD(String path, String labelData) {
  if (!SD.begin(15)) {
    Serial.println("Échec de montage de la carte SD !");    //montage?
    return;
  }

  //Lecture du type de carte SD
  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("Incapable de détecter le type de carte !");
    return;
  }
  Serial.print("Type de carte SD : "); Serial.println(cardType);
  //Initialisation de la carte SD
  Serial.println("Initialisation de la carte SD ...");


//  if (!SD.begin()) {
//    Serial.println("Erreur - Incapable d'initialiser la carte SD !");
//    return;  //Échec de l'initialisation
//  }
  Serial.println("Carte SD initialisée correctement.");
  //Ouverture du fichier donnee.txt


  //Création du fichier de donnée si inexistant
  bool i = false;
  while ( i == false) {

    File file = SD.open(path);
    //Si le fichier donnee.txt n'existe pas sur la carte SD
    if (!file) {
      Serial.println("Le fichier donnee.txt n'existe pas !");
      Serial.println("Écriture du nom des colonnes dans le fichier donnee.txt ...");
      //Écriture du nom de chaque colonne pour le "logging" des données

      writeFile(SD, path, labelData.c_str());
      i = true;
      break;
    }
    else {
      Serial.println("Le fichier donnee.txt existe déjà !");
      //Ferme le fichier
      i = true;
      file.close();
    }
  }
}

bool sendData(String msg, String path){
  appendFile(SD,path,msg.c_str());
  return true;
}



#ifdef DEBUG
void i2c_init_(const int PIN_POW_EN = 13) {

  pinMode(PIN_POW_EN, OUTPUT);
  digitalWrite(PIN_POW_EN, HIGH);
  delay (500);
}

#endif
