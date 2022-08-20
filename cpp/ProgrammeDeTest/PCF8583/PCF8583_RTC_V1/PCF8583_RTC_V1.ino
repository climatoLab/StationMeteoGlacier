#include <Wire.h>
#include <Decodeur.h>

//Version du programme :
#define VERSION 1.0

//Adresse I2C du RTC PCF8583
const uint8_t PCF_I2C_ADDR = 0x50;

//Déclaration d'une variable "portSerie" du type SerieTPhys
Decodeur portSerie(&Serial); 

//Variables pour la réception de commandes et d'arguments
char commande = NULL;
uint16_t arguments = 0, nombreArguments = 0;

//Constantes pour le menu à afficher dans le moniteur série
const uint16_t NOMBRE_LIGNES_MENU = 9;
const char *MENU[NOMBRE_LIGNES_MENU] = {
"> ---------------------------------MENU---------------------------------------",
"> c : pour lire la valeur du compteur.",
"> m : pour afficher ce menu.",
"> r : pour \"resetter\" le compteur.",
"> v : pour lire le nom et la version du programme.",
"",
"> Chaque commande doit être terminé par un saut de ligne.",
"> ----------------------------------------------------------------------------", 
""};

//Définition des constantes pour les retours d'erreurs 
//pour la réception de commande et d'arguments :
const char ACK = '*';
const char NAK = '?';

//Broche pour activer la tension 3.3Volts permettant de fermer les interrupteurs du bus I2C
const int PIN_POW_EN = 13;

void setup(){
  //Initialisation du port série à 115200 bits/sec
  Serial.begin(115200);
  //Active la tension 3.3Volts "Switchée" pour permettre d'utiliser le bus I2C
  pinMode(PIN_POW_EN, OUTPUT);
  digitalWrite(PIN_POW_EN, HIGH);
  delay(500); //Petit délai de stabilisation
  
  //Affiche le message d'invite de la station météo ainsi que la version du programme :
  Serial.print("> Programme pour tester le compteur du RTC PCF8583. Version : ");
  Serial.println(VERSION);
  Serial.println("");
  //Configure le PCF8583 en mode compteur :
  PCF8583_config_cntr(PCF_I2C_ADDR);
  //Affiche le menu au démarrage du programme
  for(int index = 0; index < NOMBRE_LIGNES_MENU; index++)
    Serial.println(MENU[index]);
  //Efface le compteur au démarrage
  PCF8583_clr_cntr(PCF_I2C_ADDR);
}

void loop(){
  //*****************************************************************************
  //Regarde si nous avons recu une commande et des arguments du port série
  //*****************************************************************************
  if(portSerie.available()){
    //Lecture de la commande recue
    commande = portSerie.getCommand();
    switch(commande){
      //Commande pour afficher la valeur du compteur
      case 'c' :
        //Lecture de la valeur du compteur
        PCF8583_read_cntr(PCF_I2C_ADDR);
      break;

      //Commande pour afficher le menu dans le moniteur série
      case 'm' :
        for(int index = 0; index < NOMBRE_LIGNES_MENU; index++)
          Serial.println(MENU[index]);
      break;

      //Commande pour "resetter" le compteur du PCF8583
      case 'r' :
        //Appel de la fonction pour effacer le compteur
        PCF8583_clr_cntr(PCF_I2C_ADDR);
        //Transmet dans le moniteur série un Acknowledge pour confirmer la commande
        Serial.println(ACK);
      break;
      
      //Commande pour lire le nom et la version du programme
      case 'v' :
        Serial.print("> Programme pour tester le compteur du RTC PCF8583. Version : ");Serial.println(VERSION);
        Serial.println("");
      break;

      default :
        //Transmet un NAK pour indiquer que la commande est inconnue
        Serial.println(NAK);
      break;
    }//switch case
  }//est Disponible
}
