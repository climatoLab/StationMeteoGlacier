////=======================================================
////Fonction d'interruption pour déterminer la vitesse du vent
////=======================================================
//void IRQ_anemometre(void) {
//  //Obtient la différence de temps entre le temps initiale et le temps lors d'une interruption.
//  tempsDepuisDernierIRQ_Vent = millis() - tempsDernierIRQ_Vent;
//  tempsDernierIRQ_Vent = millis();
//  //Limite le calcul de la vitesse du vent à des vitesses inférieure à 240kmh
//  if (tempsDepuisDernierIRQ_Vent >= 10) {
//    vitesseVent = (1000. / tempsDepuisDernierIRQ_Vent) * 2.4;
//    //Ajoute la nouvelle valeur dans la moyenne mobile
//    movA_vitVent.add(vitesseVent);
//  }
//}

//=============================================
// Fonction d'interruption de l'anémomèetre
//=============================================
void isr_rotation () {
  deltaTime = millis() - lastInterrupt;
  lastInterrupt = millis();
}
