//Girouette/////////////////////////////////////////////////////////////////////////////////////////////////////
uint16_t girouette(){
  if (millis() - derniereMESURECAPTEURS > delai_capteur_lecture) {
    derniereMESURECAPTEURS = millis();

    moSbdMessage.GirValPot = analogRead(34);
    roseDESvents(moSbdMessage.GirValPot);
  }  
  return moSbdMessage.GirValPot;
}


void roseDESvents(int GirValPot) {

 static uint16_t angle = 0;
  if (GirValPot <= 715) angle = 113;       //EST SUD EST (ESE)
  else if (GirValPot <= 754) angle = 68;   //EST NORD EST (ENE)
  else if (GirValPot<= 814) angle = 90;   //EST (E)
  else if (GirValPot <= 950) angle = 158;  //SUD SUD EST (SSE) 
  else if (GirValPot <= 1123) angle = 135;  //SUD EST (SE) 
  else if (GirValPot <= 1271) angle = 203;  //SUD SUD OUEST (SSO) 
  else if (GirValPot<= 1522) angle = 180;  //SUD (S) 
  else if (GirValPot <= 1794) angle = 23;   //NORD NORD EST (NNE)
  else if (GirValPot<= 2106 ) angle = 45;   //NORD EST (NE) 
  else if (GirValPot <= 2384) angle = 248;  //OUEST SUD OUEST (OSO)
  else if (GirValPot <= 2563) angle = 225;  //SUD OUEST (SO)
  else if (GirValPot <= 2863) angle = 338;  //NORD NORD OUEST (NNO)
  else if (GirValPot <= 3094) angle = 0;    //NORD (N)
  else if (GirValPot <= 3331) angle = 293;  //OUEST NORD OUEST (ONO)
  else if (GirValPot <= 3664) angle = 315;  //NORD OUEST (NO)
  else if (GirValPot <= 3972) angle = 270;  //OUEST (0)
  else angle = 0;
//  ptr->angleVent = angle;

/*  String dirVent;
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
*/
//Serial.println(String(dirVent) + "\n");
/*
  
  if (GirValPot >= 1550 && GirValPot < 2550) {
    Serial.println("NE");
  }
  else if (GirValPot >= 2550 && GirValPot < 3050) {
    Serial.println("E");
  }
  else if (GirValPot >= 3050 && GirValPot < 3320) {
    Serial.println("SE");
  }
  else if (GirValPot >= 3320 && GirValPot < 3495) {
    Serial.println("S");
  }
  else if (GirValPot >= 3495 && GirValPot < 3605) {
    Serial.println("SO");
  }
  else if (GirValPot >= 3605 && GirValPot < 3700) {
    Serial.println("O");
  }
  else if (GirValPot >= 3700 && GirValPot < 3760) {
    Serial.println("NO");
  }
  else {
    Serial.println("N");
  }
*/
}


//Anémomètre/////////////////////////////////////////////////////////////////////////////////////////////////////

uint16_t anemometre(){
  //Rotations = 0; // Set Rotations count to 0 ready for calculations

  // sei(); // Enables interrupts
  // delay (3000); // Wait 3 seconds to average
  // cli(); // Disable interrupts

  // convert to mp/h using the formula V=P(2.25/t)
  // V = P(2.25/3) = P * 0.75
  if (flagISR) {
    rps = 1000. / deltaTime;
    flagISR = false;
  }
  if (millis() - lastInterrupt > 5000)
  {
    rps = 0.00;
  }

  /*if (rps == inf) {
    rps = 0;
    }*/
  //moSbdMessage.AnemomVitesseVent = rps * 3.66;

  moSbdMessage.AnemomVitesseVent = rps * 2.4;
  //if (millis()-timerDisplay > delayDisplay) {
  if (rps != lastrps) {
    lastrps = rps;
    //Serial.println(moSbdMessage.AnemomVitesseVent);
    //timerDisplay = millis();
  }
  return moSbdMessage.AnemomVitesseVent;
  //Serial.print(Rotations); Serial.print("\t\t");
  //Serial.println(moSbdMessage.AnemomVitesseVent);  
}


// This is the function that the interrupt calls to increment the rotation count
void isr_rotation () {
  deltaTime = millis() - lastInterrupt;
  lastInterrupt = millis();
  flagISR = true;
  //Serial.println("Paquette");
}
