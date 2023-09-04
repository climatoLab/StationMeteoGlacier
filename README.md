# ProjetClima - Station Météo Automatique (Glacier)
![StationMeteo_Glacier_pic2](https://github.com/climatoLab/StationMeteoGlacier/assets/102256913/95f78aa6-38ef-4310-b76b-416026506cf9)
Photo de la station météo déployée au kilomètre 10,5 à Kuujjuarapik

## Objectifs
L’objectif général de ce projet est la conception d'une station météorologique portative, autonome, fiable et à faible coût et capable de produire des données utiles pour surveiller la fonte des glaciers afin d’en anticiper les effets et pallier les conséquences

## Caractéristiques de la station 
La station météo est capable de :
- Prendre des mesures météorologiques de base : température, humidité, pression, vitesse et direction du vent, précipitations, ensoleillement et altitude;
- Mesurer la variation d’épaisseur de la glace au sol;
- Mesurer son déplacement terrestre (pour mesurer le déplacement du glacier sur lequel elle se trouve);
- Transmettre ses données sur de très longues distances, à partir d’endroits isolés;
- Autonome au niveau de son fonctionnement et de son alimentation énergétique;
- Résiste et fonctionne sous des conditions climatiques extrêmes de température, d’humidité et d’altitude.
- Offre des caractéristiques mieux arrimées sur les besoins des glaciologues qu’une mini-station portative haut de gamme;

## Transmission et traitement des données
Les mesures des différents capteurs ainsi que la position GPS sont enregistrées toutes les heures et stockées en mémoire jusqu'à ce que l'intervalle de transmission souhaité soit atteinte. Les données sont compressées dans un message binaire pour minimiser le coût et le nombre total de transmissions requises. Les données sont transmises via le réseau satellite Iridium à des intervalles spécifiés par l'utilisateur. Par la suite les données sont reçus par une station Iridium au sol, traitées et envoyées à un serveur Thinsgboard.

Tableau de bord de la station météo installée à Kuujjuarapik en mai 2023 :

https://thingsboard.cloud/dashboard/17a06570-f024-11ed-97ca-e57c88951dd5?publicId=cb8d6950-f06c-11ed-97ca-e57c88951dd5

## Contenu du référentiel (Repository )
La branche StationMeteoGlacier contient un programme Arduino qui fonctionne avec différents capteurs et un enregistreur de données qui a été développé en même temps que le projet. Le programme a été conçu pour fonctionner avec le ESP32 Firebeetle de DFRobot.

## Librairies utilisées
- "MovingAverageFloat" by Pavel Slama
- "tgp Decodeur" by Technologie du génie physique
- "Adafruit LSM303 Accel" by Adafruit  
	Installer les dépendances :
	- "Adafruit Unified Sensor"
	- "Adafruit Bus IO"
- "SparkFun VL53L1X 4m Laser Distance Sensor"
- "Adafruit GPS Library" by Adafruit
- "DHT sensor Library" by Adafruit
- "MAX44009 Library" by Dan Tudose
- "PCF8583" by Xose Pérez
- "Adafruit BMP3XX Library" by Adafruit
- "RTClib" by Adafruit	
- "IridiumSBDi2c" by Sparkfun Electronics
- "Adafruit MAX31855 library" by Adafruit
- "ArduinoJson" by Benoit Blanchon

## Crédits
Projet de Cryologger : https://cryologger.org/

## License Information
Ce projet est distribué sous la licence publique générale GNU v3.0
