//Indique la broche à utiliser pour lire la tension d'entrée : broche 34 (ADC1_CH6)
#define brocheVin 34

//Constante pour indiquer la grosseur du tableau de lecture de la tension d'entrée
const int nbreDeLecturesVin = 16;

//Déclare un tableau et des variables pour faire la moyenne mobile de la lecture de la tension d'entrée
uint32_t lecturesVin[nbreDeLecturesVin];  //Lectures de l'entrée analogique
uint32_t totalVin = 0;                    //Somme totale
float moyenneVin = 0;                     //Moyenne
uint32_t indexLecture = 0;                //Index pour pointer dans le tableau de lecture

//Variable pour contenir la valeur de la tension d'entrée en "Volts"
//corrigée (avec coefficient) et Réel (avec facteur de correction).
float VinCoeff = 0;
float VinReel = 0;

//Constantes pour chacun des coefficients du polynome de degré 4 afin de calculer la tension de la tension d'entrée Vin
//Valeurs obtenues à partir d'un étalonnage (voir le programme ESP32_ADC_Read_Voltage_Accurate.ino")
const double coeff_A = -0.000000000000016;
const double coeff_B = 0.000000000118171;
const double coeff_C = -0.000000301211691;
const double coeff_D = 0.001109019271794;
const double coeff_E = 0.034143524634089;

//Facteur de conversion mesurée du diviseur de tension à l'entrée du convertisseur
//Vin = 5.19V ; Vconv = 2.595; facteur = Vin / Vconv
const float facteurConversion = 1;    //Aucun diviseur de tension utilisé avec la girouette

//Variables et constantes pour la minuterie d'affichage
unsigned long minuterieAffichage = millis();
int delaiAffichage = 1000;   //Délai d'une seconde

const int PIN_POW_EN = 13;

void setup() {
  //Initialise le port de communication série à 115200 bits/sec
  Serial.begin(115200);

  pinMode(PIN_POW_EN, OUTPUT);
  digitalWrite(PIN_POW_EN, HIGH);
  delay(500);

  //Initialise le tableau de mesure :
  for(int index = 0; index < nbreDeLecturesVin; index++) {
    lecturesVin[index] = 0;
  }
}

void loop() {
  //Acquisition de la tension d'entrée et calcul de la moyenne
  //Soustrait la dernière lecture de la somme totale du tableau :
  totalVin = totalVin - lecturesVin[indexLecture];
  //Lecture de la tension d'entrée :
  lecturesVin[indexLecture] = analogRead(brocheVin);
  //Ajoute la nouvelle valeur à la somme totale du tableau :
  totalVin = totalVin + lecturesVin[indexLecture];
  //Pointe vers la prochaine case du tableau :
  indexLecture = indexLecture + 1;

  //Regarde si nous pointons sur la dernière valeur du tableau ...
  if(indexLecture >= nbreDeLecturesVin){
    //Si oui, nous rrplaçons le pointeur au début du tableau :
    indexLecture = 0;
  }

  //Calcul la moyenne du tableau  :
  moyenneVin = (float)totalVin/nbreDeLecturesVin;

  //Applique un polynome de degré 4 pour corriger la "non-linéarité" du ADC
  if(moyenneVin < 1 || moyenneVin > 4095){
    VinCoeff = 0;
  }
  else{
    VinCoeff = coeff_A*pow(moyenneVin,4) + coeff_B*pow(moyenneVin,3) + coeff_C*pow(moyenneVin,2) + coeff_D*moyenneVin + coeff_E;  
  }

  //Conversion de la valeur du ADC en Volts
  VinReel = facteurConversion * VinCoeff;

  //Chaque "delai", nous affichons la tension d'entrée
  //dans le moniteur série
  if(millis() - minuterieAffichage > delaiAffichage){
    minuterieAffichage = millis();
    //Conversion de la valeur du ADC en Volts
    Serial.print("Tension Vin = " + String(VinReel) + " Volts \r\n\r\n");
  }

  //Délai d'une milliseconde entre chaque lecture dans le tableau
  delay(1);
}
