//#include <MovingAverage.h>

unsigned long derniereMESURECAPTEURS = millis();
int delai_capteur_lecture = 250;
int delai = 0;

//MovingAverage<unsigned> MM(16, 0);

int valpot;// valeur analogique du potentiomètre
int Pcardinaux;

void setup() {
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);

  //LastValue = 1;
  Serial.begin(115200);
  pinMode(34, OUTPUT);
  digitalWrite(34, HIGH);
}

void loop() {

  if (millis() - derniereMESURECAPTEURS > delai_capteur_lecture) {
    derniereMESURECAPTEURS = millis();

    valpot = analogRead(34);
    Serial.println("sortie analogique de la girouette = ");
    Pcardinaux = valpot;
    Serial.println(Pcardinaux);

    roseDESvents(valpot);
  }
}

void roseDESvents(int valpot) {
  if (valpot >= 1550 && valpot < 2550) {
    Serial.println("NE");
  }
  else if (valpot >= 2550 && valpot < 3050) {
    Serial.println("E");
  }
  else if (valpot >= 3050 && valpot < 3320) {
    Serial.println("SE");
  }
  else if (valpot >= 3320 && valpot < 3495) {
    Serial.println("S");
  }
  else if (valpot >= 3495 && valpot < 3605) {
    Serial.println("SO");
  }
  else if (valpot >= 3605 && valpot < 3700) {
    Serial.println("O");
  }
  else if (valpot >= 3700 && valpot < 3760) {
    Serial.println("NO");
  }
  else {
    Serial.println("N");
  }
}
