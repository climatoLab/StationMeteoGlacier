/*  --- Entête principale -- information sur le sketche

    Programme:        Climato.ino
    Date:             29 mars 2022
    Auteur:           ClimatoLab
    Plateforme visée: DFrobot ESP32 FireBeetle V4.0
    Description:      (brève description de ce qu'accompli ce programme/sketche (+lignes si requis)
    Fonctionnalités:  (fonctionnalités principales)
    Plateforme logicielle : Arduino 1.8.15
    Notes:

    -- Inspirations et credits: --
     (faire la liste des codes d'inspiration et ou des crédits de code)

   Format des versions et historique de développement :
   format x.y.z
   x => Niveau principal (0=preuve de concept et essais, 1=1ere prod, ...=subsequente)
   y => Sous-version.
   z => Incrément de la sous version.

   HISTORIQUE:
   v0.1.0: version initiale de test
      - détails sur ce qui a été fait
      - détails sur ce qui a été expérimenté
      - détails...
   v1.0.x: version 1 de niveau production (détails)
   v2.x: versions futures et développements

*/
#include "station.h"



void setup() {
  //Initialise le port de communication série
  Serial.begin(115200);
  //Laisse le temps au port série d'être prêt
  
  //Affiche dans le moniteur série la version de la Station Météo
  Serial.println("> Station Météo version " + VERSION);

  //Incrémente le compteur du nombre de démaragge chaque fois que le ESP32 démarre
  ++bootCount;
  Serial.println("> Nombre de réveil = " + String(bootCount));

  //Affiche la source qui a permis au ESp32 de se réveiller
  print_wakeup_reason();

  //Configure la source à partir de laquelle le ESP32 va se réveiller
  //Le ESP32 est configuré pour se réveiller à partir d'une minuterie
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("> ESP32 est configuré pour se réveiller à toute les " + String(TIME_TO_SLEEP) + " minutes.");

  //Configure la broche pour lire le mode de fonctionnement de la Station météo
  pinMode (BROCHE_SEL_MODE, INPUT);
  //etat_sel_mode = digitalRead(BROCHE_SEL_MODE);
  Serial.print("> Selecteur de mode = "); Serial.println(etat_sel_mode);

  //Active la tension d'alimentation 3V3_SW
  pinMode(POWER_ON_CAPTEUR, OUTPUT);
  digitalWrite(POWER_ON_CAPTEUR, HIGH);
  //Active la tension d'Alimentation du module GPS uniquement
  pinMode(POWER_ON_GPS, OUTPUT);
  //digitalWrite(POWER_ON_GPS, HIGH); pas utilisé
  digitalWrite(POWER_ON_GPS, LOW);
  //Délai pour laisser le temps au modules d'être alimenté correctement
  delay(1000);

  //Initialise le port I2C pour permettre d'utiliser tous les modules I2C de la Station
  Wire.begin();

  //Obtention des mots de passes stockés dans la mémoire Flash du ESP32
  //init_Credentials();

  //Initialisation des différents capteurs de la Station météo
  //init_Accelerometre();     //Voir code de Gregory ...
  init_Anemometre();
  init_Humidite();
  init_Luminosite();
  etat_pression = init_Pression();
  //init_Distance();          //Voir code de Salah
  init_Girouette();
  //init_Pluviometre();       //A implanter/tester dans le future. Pas supporter pour l'instant !
  init_RTC();
  init_VinExt();
  init_VinSol();
  //init_GPS();               //Voir code de Gregory ...

  init_SD();
  //init_Wifi();

  //Affiche le menu utilisateur dans le moniteur série
  menu();

  pinMode(LED_BUILTIN, OUTPUT);
//  digitalWrite(LED_BUILTIN, LOW);
//  delay(250);
//  digitalWrite(LED_BUILTIN, HIGH);
//  delay(250);
//  digitalWrite(LED_BUILTIN, LOW);
//  delay(250);
//  digitalWrite(LED_BUILTIN, HIGH);
//  delay(250);
//  digitalWrite(LED_BUILTIN, LOW);
  
   _BlinkLed(LED_BUILTIN,4, 250);

}

void loop()
{
  for (int i = 0 ; i < 5 ; i++)
  {
    delay(1000);
    lecture_RTC();
    lecture_Anemometre();
    lecture_Humidite();
    lecture_Luminosite();
    lecture_Pression(etat_pression);
    lecture_Temperature();
    lecture_Girouette();
    lecture_VinExt();
    lecture_VinSol();
  }

  enregistreDonneeCarteSD();

//  digitalWrite(LED_BUILTIN, LOW);
//  delay(500);
//  digitalWrite(LED_BUILTIN, HIGH);
//  delay(500);
//  digitalWrite(LED_BUILTIN, LOW);
//  delay(500);
//  digitalWrite(LED_BUILTIN, HIGH);
//  delay(500);
//  digitalWrite(LED_BUILTIN, LOW);

  _BlinkLed(LED_BUILTIN,3, 500);

  
  //Fermer la tension 3.3V avant de s'endormir
  digitalWrite(POWER_ON_CAPTEUR, LOW);
  Serial.println("> Passe en mode sommeil profond dans une seconde !");
  delay(1000);
  //Vide la mémoire tampon du port série (TX Serial Buffer uniquement !)
  Serial.flush();
  //Passe en mode sommeil profond immédiatement :
  esp_deep_sleep_start();

  
}

//void loop() {
//  //***********************************************************************************************************
//  //Lit le mode de fonctionnement de la Station Météo
//  //***********************************************************************************************************
//  //etat_sel_mode = digitalRead(BROCHE_SEL_MODE);
//
//  //***********************************************************************************************************
//  //Réception des commandes et arguments via le port série pour permettre
//  //de contrôler la station météo
//  //***********************************************************************************************************
//  decodeurCMD();
//
//  //*************************************************************
//  //Lecture de chaque capteur sur une base régulière.
//  //Certains capteurs utilisent une moyenne mobile pour obtenir des données plus stables.
//  //Peu importe le mode de la station météo, il est important de bien remplir le tableau
//  //de la moyenne mobile lors de la mise en route de la station (losr de la première passe !)
//  //Les fonctions
//  //*************************************************************
//  static uint16_t averageCounter = 0;
//  if (averageCounter < AVERAGE_BUFFER_SIZE) {
//    unsigned long millisReading = millis();
//    if (millisReading - minuterie_Lecture_capteurs >= DELAI_LECTURE_CAPTEURS) {
//      minuterie_Lecture_capteurs = millisReading;
//      //Affiche les données dans le moniteur série quand le mode continu est activé
//      if (modeContinu == true) {
//        //Serial.print("Compteur de moyenne = "); Serial.println(averageCounter); //Pour "debuggage" uniquement !
//        valCapteursPortSerie();
//      }
//      averageCounter++;
//      lecture_RTC();
//      //lecture_Accelerometre();  //Voir code de Gregory ...
//      lecture_Anemometre();
//      lecture_Humidite();
//      lecture_Luminosite();
//      lecture_Pression(etat_pression);
//      lecture_Temperature();
//      //lecture_Distance();       //Voir code de Salah
//      lecture_Girouette();
//      //lecture_Pluie();          //A implanter/tester dans le future. Pas supporter pour l'instant !
//      lecture_VinExt();
//      lecture_VinSol();
//      //lecture_GPS();            //Voir code de Gregory ...
//    }
//  }
//  //*************************************************************
//  //Mode avec alimentation murale (externe de 5V) et branchement à un ordinateur (USB)
//  //ou
//  //Mode avec alimentation murale (externe de 5V) et sans branchement à un ordinateur (WiFi ou Bluetooth)
//  //*************************************************************
//  else if (etat_sel_mode == false) {
//    //Réinitialise le compteur de moyennage quand la station ne passe pas en mode sommeil profond
//    averageCounter = 0;
//    enregistreDonneeCarteSD();
//  }
//  //*************************************************************
//  //Mode sans alimentation murale, sans batterie et avec un branchement à un ordinateur (USB)
//  //ou
//  //Mode sans alimentation murale, avec batterie et avec un branchement à un ordinateur (USB)
//  //ou
//  //Mode sans alimentation murale, avec batterie et sans branchement à un ordinateur (WiFi ou Bluetoth)
//  //(Mode pour le Deep Sleep) ===> etat_sel_mode == true
//  //*************************************************************
//  else {
//    enregistreDonneeCarteSD();
//    //Fermer la tension 3.3V avant de s'endormir
//    digitalWrite(POWER_ON_CAPTEUR, LOW);
//    Serial.println("> Passe en mode sommeil profond dans une seconde !");
//    delay(1000);
//    //Vide la mémoire tampon du port série (TX Serial Buffer uniquement !)
//    Serial.flush();
//    //Passe en mode sommeil profond immédiatement :
//    esp_deep_sleep_start();
//  }
//
//  //Petit délai de boucle pour le décodeur de commandes afin de laisser le temps au commande d'arriver dans le buffer
//  delay(10);
//}
