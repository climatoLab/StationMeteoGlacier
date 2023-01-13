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

/*
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
  */


void roseDESvents(int valpot) {


 static uint16_t angle = 0;
  if (valpot <= 164) angle = 113;       //EST SUD EST (ESE)
  else if (valpot <= 220) angle = 68;   //EST NORD EST (ENE)
  else if (valpot<= 305) angle = 90;   //EST (E)
  else if (valpot <= 490) angle = 158;  //SUD SUD EST (SSE) 
  else if (valpot <= 725) angle = 135;  //SUD EST (SE) 
  else if (valpot <= 925) angle = 203;  //SUD SUD OUEST (SSO) 
  else if (valpot<= 1254) angle = 180;  //SUD (S) 
  else if (valpot <= 1612) angle = 23;   //NORD NORD EST (NNE)
  else if (valpot<= 2005 ) angle = 45;   //NORD EST (NE) 
  else if (valpot <= 2345) angle = 248;  //OUEST SUD OUEST (OSO)
  else if (valpot <= 2555) angle = 225;  //SUD OUEST (SO)
  else if (valpot <= 2878) angle = 338;  //NORD NORD OUEST (NNO)
  else if (valpot <= 3145) angle = 0;    //NORD (N)
  else if (valpot <= 3390) angle = 293;  //OUEST NORD OUEST (ONO)
  else if (valpot <= 3728) angle = 315;  //NORD OUEST (NO)
  else if (valpot <= 3931) angle = 270;  //OUEST (0)
  else angle = 0;
//  ptr->angleVent = angle;


  String dirVent;
  if(angle == 338 || angle == 0)  dirVent = "NORD";
  else if(angle == 23 || angle == 45)   dirVent = "NORD-EST";
  else if(angle == 68 || angle == 90)   dirVent = "EST";
  else if(angle == 113 || angle == 135) dirVent = "SUD-EST";
  else if(angle == 158 || angle == 180) dirVent = "SUD";
  else if(angle == 203 || angle == 225) dirVent = "SUD-OUEST";
  else if(angle == 248 || angle == 270) dirVent = "OUEST";
  else if(angle == 293 || angle == 315) dirVent = "NORD-OUEST";      
  else  dirVent = "NORD";

//  if(angle == 0)  dirVent = "NORD";
//  else if(angle == 23)  dirVent = "NNE";
//  else if(angle == 45)  dirVent = "NORD-EST";
//  else if(angle == 68)  dirVent = "ENE";
//  else if(angle == 90)  dirVent = "EST";
//  else if(angle == 113) dirVent = "ESE";
//  else if(angle == 135) dirVent = "SUD-EST";
//  else if(angle == 158) dirVent = "SSE";      
//  else if(angle == 180) dirVent = "SUD";
//  else if(angle == 203) dirVent = "SSO";  
//  else if(angle == 225) dirVent = "SUD-OUEST";
//  else if(angle == 248) dirVent = "OSO";  
//  else if(angle == 270) dirVent = "OUEST";
//  else if(angle == 293) dirVent = "ONO";
//  else if(angle == 315) dirVent = "NORD-OUEST";
//  else if(angle == 338) dirVent = "NNO";      
//  else  dirVent = "N";

Serial.println(dirVent);
/*
  
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
*/
}
