# ProjetClima - Station Météo Automatique (Glacier)
![StationMeteo_Glacier_pic2](https://github.com/climatoLab/StationMeteoGlacier/assets/102256913/b81ce2fa-4a42-488c-8efd-8a72bb7f2622)

## Objectifs
Le présent projet de recherche vise le développement d’une station météo à faibles coûts et capable de produire des données utiles pour surveiller la fonte des glaciers afin d’en anticiper les effets et pallier les conséquences. 

## Présentation du projet 
L’objectif général de ce projet est la conception d'une station météorologique portative, autonome, fiable et à faible coût de manière à étudier la fonte des glaciers. Plus précisément, la station doit être capable de :
- Prendre des mesures météorologiques de base : température, humidité, pression, vitesse et direction du vent, précipitations, ensoleillement et altitude;
- Mesurer la variation d’épaisseur de la glace au sol;
- Mesurer son déplacement terrestre (pour mesurer le déplacement du glacier sur lequel elle se trouve);
- Transmettre ses données sur de très longues distances, à partir d’endroits isolés;
- Être autonome au niveau de son fonctionnement et de son alimentation énergétique;
- Résister et de fonctionner sous des conditions climatiques extrêmes de température, d’humidité et d’altitude.
- Offrir des caractéristiques mieux arrimées sur les besoins des glaciologues qu’une mini-station portative haut de gamme;

## Transmission et traitement des données
Les mesures des différents capteurs ainsi que la position GPS sont enregistrées toutes les heures et stockées en mémoire jusqu'à ce que l'intervalle de transmission souhaité soit atteinte. Les données sont compressées dans un message binaire pour minimiser le coût et le nombre total de transmissions requises. Les données sont transmises via le réseau satellite Iridium à des intervalles spécifiés par l'utilisateur. Par la suite les données sont reçus par une station Iridium au sol, traitées et envoyées à un serveur Thinsgboard.

## Contenu du référentiel (Repository )
La branche StationMeteoGlacier contient un programme Arduino qui fonctionne avec différents capteurs et un enregistreur de données qui a été développé en même temps que le projet. Le programme a été conçu pour fonctionner avec le ESP32 Firebeetle de DFRobot.

## Librairies utilisées

## Crédits
Projet de Cryologger : https://cryologger.org/

## License Information
Ce projet est distribué sous la licence publique générale GNU v3.0

