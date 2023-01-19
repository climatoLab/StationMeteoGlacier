#define DEBUG
#include "SDLibrary.h"

String path = "/donnees.txt";//Nom du fichier qui contiendras les fichiers.
String labelData = "Hello World!";//première ligne inscrite lors de la l'initialisation de la carte sd. 

void setup() {
Serial.begin(115200);
i2c_init_();
init_SD(path,labelData);
}

void loop() {
  String msg = String(random(100));
  Serial.println(msg);
  sendData(msg,path);
  delay(1000);

}
