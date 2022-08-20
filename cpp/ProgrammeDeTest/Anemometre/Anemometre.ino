//Indique le numéro de broche à utiliser pour activer la tension de 3.3V pour les dispositifs I2C
#define BROCHE_3V3_SW 13

//Indique le numéro de la broche pour le pluviomètre
#define INTERRUPT_PIN 35       //Pour tester le Wind speed

volatile int windTicks = 0;

void setup(){
  //Initialise le port de communication série à 115200 bps
  Serial.begin(115200);

  //Active la tension sur le rail d'alimentation 3V3
  pinMode(BROCHE_3V3_SW, OUTPUT);
  digitalWrite(BROCHE_3V3_SW, HIGH);

  delay(10);

  pinMode(INTERRUPT_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), windTick, RISING);
  windTicks = 0;
}    

void loop(){
  delay(10);
}

//Capture l'instant où l'interrupteur de l'anémomètre à basculé.
void windTick(void){
  windTicks++;
  Serial.println(windTicks);
  
}
