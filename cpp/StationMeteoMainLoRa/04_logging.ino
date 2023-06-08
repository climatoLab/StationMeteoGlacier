// ----------------------------------------------------------------------------
// Enregistre toutes les données sous une chaine de caractères
// ----------------------------------------------------------------------------
String str_donnees() { //--> Met dans une variable String la structure de nos données
  char vir = ',';
  String s_msg = 
                   String(moSbdMessage.iterationCounter) + vir  
                 + String(moSbdMessage.unix) + vir
                 + String(Vin) + vir
                 + String(bmpTemperatureC) + vir
                 + String(bmpPressionHPa) + vir
                 + String(bmpAltitude) + vir
                 + String(dhtHumidite) + vir
                 + String(dhtTemperatureC) + vir
                 + String(tcTemperatureC) + vir
                 + String(gy49LuminositeLux) + vir
                 + String(vlDistanceMM) + vir
                 + String(windDirection) + vir
                 + String(windSpeed) + "\n";
  return s_msg;
}

// ----------------------------------------------------------------------------
// Fonctions créer pour l'enregistrement des données et l'initialisation
// ----------------------------------------------------------------------------
void init_SD(const char* path, String labelData) {
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
      Serial.println("Le fichier " + String(path) + " n'existe pas !");
      Serial.println("Écriture du nom des colonnes dans le fichier "+ String(path) +" ...");
      //Écriture du nom de chaque colonne pour le "logging" des données

      writeFile(SD, path, labelData.c_str());
      i = true;
      break;
    }
    else {
      Serial.println("Le fichier " + String(path) + " existe déjà !");
      //Ferme le fichier
      i = true;
      file.close();
    }
  }
  flag_initSD = i;
}

bool sendData(String msg, const char* path){
  appendFile(SD,path,msg.c_str());
  return true;
}

// ----------------------------------------------------------------------------
// Fonctions pour l'utilisation de la carte SD
// https://randomnerdtutorials.com/esp32-microsd-card-arduino/
// ----------------------------------------------------------------------------
void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("Failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.name(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

void createDir(fs::FS &fs, const char * path){
    Serial.printf("Creating Dir: %s\n", path);
    if(fs.mkdir(path)){
        Serial.println("Dir created");
    } else {
        Serial.println("mkdir failed");
    }
}

void removeDir(fs::FS &fs, const char * path){
    Serial.printf("Removing Dir: %s\n", path);
    if(fs.rmdir(path)){
        Serial.println("Dir removed");
    } else {
        Serial.println("rmdir failed");
    }
}

void readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if(!file){
        Serial.println("Failed to open file for reading");
        return;
    }

    Serial.print("Read from file: ");
    while(file.available()){
        Serial.write(file.read());
    }
    file.close();
}

void writeFile(fs::FS &fs, const char * path, String message){
    Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("File written");
    } else {
        Serial.println("Write failed");
    }
    file.close();
}

void appendFile(fs::FS &fs, const char * path, String message){
    Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("Failed to open file for appending");
        return;
    }
    if(file.print(message)){
        Serial.println("Message appended");
    } else {
        Serial.println("Append failed");
    }
    file.close();
}

void renameFile(fs::FS &fs, const char * path1, const char * path2){
    Serial.printf("Renaming file %s to %s\n", path1, path2);
    if (fs.rename(path1, path2)) {
        Serial.println("File renamed");
    } else {
        Serial.println("Rename failed");
    }
}

void deleteFile(fs::FS &fs, const char * path){
    Serial.printf("Deleting file: %s\n", path);
    if(fs.remove(path)){
        Serial.println("File deleted");
    } else {
        Serial.println("Delete failed");
    }
}

void testFileIO(fs::FS &fs, const char * path){
    File file = fs.open(path);
    static uint8_t buf[512];
    size_t len = 0;
    uint32_t start = millis();
    uint32_t end = start;
    if(file){
        len = file.size();
        size_t flen = len;
        start = millis();
        while(len){
            size_t toRead = len;
            if(toRead > 512){
                toRead = 512;
            }
            file.read(buf, toRead);
            len -= toRead;
        }
        end = millis() - start;
        Serial.printf("%u bytes read for %u ms\n", flen, end);
        file.close();
    } else {
        Serial.println("Failed to open file for reading");
    }


    file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }

    size_t i;
    start = millis();
    for(i=0; i<2048; i++){
        file.write(buf, 512);
    }
    end = millis() - start;
    Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
    file.close();
}
void lastLine(fs::FS &fs, const char * path){
  int msg;
    File file = SD.open(path);
  if (file) {
    uint32_t lineStart = 0;
    while (file.available()) {
      lineStart = file.position();
      if (!file.find((char*) "\n"))
        break;
    }
    file.seek(lineStart);
    while (file.available()) {
        msg = file.read();
        Serial.write(msg);
    }
    file.close();
  } else {
    Serial.println("error opening datalog.txt");
  }
}
