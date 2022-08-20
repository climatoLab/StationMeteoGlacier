<div id="top"></div>
<!--
Le document est une copie du ReadMe de la Station météo version école alors beaucoup de chose est à changer.








[![Contributors][contributors-shield]][contributors-url]
[![Forks][forks-shield]][forks-url]
[![Stargazers][stars-shield]][stars-url]
[![Issues][issues-shield]][issues-url]
[![MIT License][license-shield]][license-url]
[![LinkedIn][linkedin-shield]][linkedin-url]
-->

<!-- PROJECT LOGO -->

<br />
<div align="center">
  <a href="">
    <img src="Images/logo.png" alt="Logo" width="80" height="80">
  </a>

  <h3 align="center">Station météo glacier</h3>

  <p align="center">
    Une station météo !
    <br />
    <a href="https://github.com/distaCAL/StationMeteo/issues">Signaler un Problème</a>
    ·
    <a href="https://github.com/distaCAL/StationMeteo/issues">Demander une fonctionnalité</a>
  </p>
</div>
 


<!-- Table des matières--------------------------------------------------------------------------------------- -->
<details>
  <summary>Table des Matières</summary>
  <ol>
    <li>
      <a href="#À-propos-du projet">À propos du projet</a>
      <ul>
        <li><a href="#Créé-avec">Créé avec</a></li>
      </ul>
    </li>
    <li>
      <a href="#Pour Commencer">Pour Commencer</a>
      <ul>
        <li><a href="#Prérequis">Prérequis</a></li>
        <li><a href="#Installation">Installation</a>
         <ul>
           <li><a href="#Choix de Carte">Choix de Carte</a></li>
           <li><a href="#Choix du Port">Comment choisir le bon port</a></li>
           <li><a href="#Installation du Programme">Installater le Programme</a></li>
         </ul>
      </ul>
    </li>
    <li><a href="#Utilités">Utilités</a></li>
    <li><a href="#Feuille de route">Feuille de route</a></li>
    <li><a href="#Contribution">Contribution</a></li>
    <li><a href="#Licenses">Licenses</a></li>
    <li><a href="#Contacts">Contacts</a></li>
    <li><a href="#Reconnaissances">Reconnaissances</a></li>
  </ol>
</details>

<!-- à propos du projet--------------------------------------------------------------------------------------- -->
<h2 id="À-propos-du projet">À propos du projet</h2>

<!-- Image du projet -->
<div align="center">
  <a href="">
   <!-- <img src="Images/StationAngle.jpg" alt="Station Météo" width="750" height="750"> -->
  </a>
  
 <div align="left">

 <!--Décrit qu'est-ce qu'est le projet à quoi il sert et le pourquoi, comment, quand? -->
  <!-- Modifier le texte pour rajouter les détails sur les capteurs -->
La branche glacier de la station météo vise à donner un porgramme Arduino qui fonctionne avec les appareils qui ont été développé en même temps que le projet. L'appareil développé permet de mesurer plusieurs données de l'environnement autour en utilisant des capteurs attachés avec la station qui va être déployé pour de longues périodes. Ces capteurs sont :

* Acceleromètre
* Anemomètre
* Capteur de distance
* Girouette
* GPS
* Humidité
* Luminosité
* Pluviomètre
* Pression
* RTC
* Satellitte
* Thermocouple


<p align="right">(<a href="#top">retour au début</a>)</p>


<!-- créé avec--------------------------------------------------------------------------------------- -->
<h3 id="Créé-avec">Créé avec</h3>

<!-- 
Cette section devrait contenir une listes de bibliothèque et peut-être de hardware. 
Laisse les add-ons/ pluggin pour la section reconnaissance.
-->
Pour faire fonctionner la Esp32 avec les capteurs et les interfaces utilisées sur l'appareil, plusieurs bibliothèque sont intégrées.

Bibliothèques utilisées :


<p align="right">(<a href="#top">retour au début</a>)</p>



<!-- pour commencer ---------------------------------------------------------------------------------- -->
<h2 id="Pour Commencer">Pour Commencer</h2>

<!-- montre comment installer le projet localement -->

L'installation est plutot simple. Il suffit de charger le programme dans la Firebeetle de la station météo et l'appareil fonctionne.

<!-- prérequis ---------------------------------------------------------------------------------- -->
<h3 id="Prérequis">Prérequis</h3>

<!-- ceci montre comment installer des prérequis -->
Pour installer les bibliothèques, les étapes sont très simples :

  ```sh
  1- Ouvrir Arduino
  2- Aller dans la section "Outils"
  3- Aller dans "Gérer les bibliothèques"
  ```
  
  <img src="Images/GererBibliotheque.png" alt="outis>Gérér les bibliothèques">
  
  ```
  4- Il suffit de taper le nom de la bibliothèque dans la bars de recherche
  ```
  
 <img src="Images/BarreRecherche.png" alt="La barre de recherche pour gérer les bibliothèques">
 
<!-- Installation---------------------------------------------------------------------------------- -->
<h3 id="Installation">Installation</h2>

<!-- En dessous est une explication sur comment installer le projet -->
   Pour installer le programme il faut d'abord faire choisir la ESP32 dans les carte puis il faut choisir le bon port
   <h4 id="Choix de Carte">Choix de la carte</h2> 
   
   ```
   1- Aller dans l'onglet "Outils"
   2- Aller dans "Type de carte:"
   3- Aller dans "ESP32 Arduino"
   4- Choisir la FireBeetle-ESP32
   ```
   
   <img src="Images/ChoixCarte.png" alt="Où trouver la Firebeetle">
   
   <i> Si les cartes ESP32 ne sont pas disponible, cliquer </i> [ICI](https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/) <i> pour savoir comment les installer </i>
   
   <h4 id="Choix du Port">Comment choisir le bon port</h2>
   
   Pour choisir le bons port il faut faire quelques étapes :
   ```
   1- Aller dans l'onglet "Outils"
   2- Aller dans la section "Port:"
   3- Prendre note des Ports disponible
   4- Sortir de l'onglet "Outils"
   5- Connecter la station météo à l'ordinateur via un cable USB
   6- Retourner dans la section "Port:" en suivant les étapes 1 & 2
   7- Sélectionner le nouveau port disponible
   ```
   
   <img src="Images/ChoixPort.png" alt="OÈu faire le choix de port">
   
   <h4 id="Installation du Programme">Intstaller le programme</h2>
   
   ```
   L'installation du programme se fait avec le bouton Téléverser en haut à droite
   lorsque la carte et le port est bien choisi
   ```
   
 <img src="Images/Televerser.png" alt="Bouton pour Téléverser">



<p align="right">(<a href="#top">retour au début</a>)</p>



<!-- exemple d'utilité ---------------------------------------------------------- -->
<h2 id="Utilités">Utilités</h2>

<!-- Montre des exemples utiles du projet, peut rajouter des screenshots, demo, code, etc. -->
La station météo offre plusieurs fonctionnalités avec tous ses capateurs et aussi plusieurs utilisation

   Les fonctionnalités comprend une communication avec le port série à un Baud rate de 115200.
   Ce port série permet de pouvoir rentrer des commandes pour interagir avec l'appareil.
   <br>
   <br>
   _Les commandes qui peuvent être envoyer comprend :_
   ```
   "b" Pour vérifier la tension d'entré
   "c <0/1>" pour désactiver/activer l'affichage continue
   "d" pour afficher la distance du glacier
   "g" pour indiquer la position de la station
   "h" pour indiquer l'humidité relative
   "i" pour indiquer l'inclinaison de la station
   "l" pour indiquer l'intensité lumineuse
   "m" pour afficher le menu
   "p" pour afficher la quantité de pluie tombée
   "P" pour indiquer la pression
   "q" pour faire la lecture des capteurs
   "r" pour afficher l'heure
   "R <aa MM jj hh mm ss>" pour ajuster l'année, le mois, le jour, l'heure, la minute et les secondes
   "s" pour enregistrer lees mesures sur la carte SD
   "t" pour afficher la température
   "v" pour afficher les caractéristiques du vent
   "w" pour transmettre les mesures au serveur par Wifi
   ```
   <br>
   
  <br> 
<!-- <i>peut créer une référence </i> [Documentation](https://example.com) -->
<!--  Rajouter les utilisations possibles et des photos pour la présentation    -->
   
   
<p align="right">(<a href="#top">retour au début</a>)</p>



<!-- Feuille de route----------------------------------------------------- -->
<h2 id="Feuille de route">Feuille de route</h2>


<!-- montre la feuille de route -->


- [ ] Rajouter un Changelog
- [x] Rajouter back to top links (francais)
- [ ] Rajouter textes pour readme
    - [ ] Utilités 
    - [ ] Feuille de route
    - [ ] Contribution
    - [ ] License
    - [ ] Contacts
    - [ ] Reconnaissance

Voir les [Questions ouvertes](https://github.com/distaCAL/StationMeteo/issues) pour une listes complêtes de fonctionnalités demander et de problèmes connus.
<p align="right">(<a href="#top">retour au début</a>)</p>



<!-- Contribution ---------------------------------------------------------- -->
<h2 id="Contribution">Contribution</h2>


<!-- listes de personnes qui ont aidé et leur contribution. -->
Les contribution sont qu'est-ce qui fait que les communauté Èa source ouvert sont de merveilleuse places. Toutes contributions sont **grandement apprécié**.

Si vous avez une suggestion qui pourrait améliorer le projet, s'il-vous-plait créez une requête pour pull. Vous pouvez aussi ouvrir une question avec le tag "amélioration"

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

<p align="right">(<a href="#top">retour au début</a>)</p>



<!-- LICENSE---------------------------------------------------------------- -->
<h2 id="Licenses">Licenses</h2>
<!-- Donne la/les license(s) -->
Distribué sous la license '' Voir `` pour plus d'information.

<p align="right">(<a href="#top">retour au début</a>)</p>



<!-- Contacts ------------------------------------------------------------------- -->
<h2 id="Contacts">Contacts</h2>

<!-- rajouter les contacts -->

Your Name - [@your_twitter](https://twitter.com/your_username) - email@example.com

Project Link: [https://github.com/your_username/repo_name](https://github.com/your_username/repo_name)

<p align="right">(<a href="#top">retour au début</a>)</p>



<!-- Reconnaissances ---------------------------------------------------- -->
<h2 id="Reconnaissances">Reconnaissances</h2>

<!-- inclure une liste de ressouces qui ont aidé et qui devrait être crédité voici des exemples -->



* [Choose an Open Source License](https://choosealicense.com)
* [GitHub Emoji Cheat Sheet](https://www.webpagefx.com/tools/emoji-cheat-sheet)
* [Malven's Flexbox Cheatsheet](https://flexbox.malven.co/)
* [Malven's Grid Cheatsheet](https://grid.malven.co/)
* [Img Shields](https://shields.io)
* [GitHub Pages](https://pages.github.com)
* [Font Awesome](https://fontawesome.com)
* [React Icons](https://react-icons.github.io/react-icons/search)

<p align="right">(<a href="#top">retour au début</a>)</p>



<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[contributors-shield]: https://img.shields.io/github/contributors/othneildrew/Best-README-Template.svg?style=for-the-badge
[contributors-url]: https://github.com/othneildrew/Best-README-Template/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/othneildrew/Best-README-Template.svg?style=for-the-badge
[forks-url]: https://github.com/othneildrew/Best-README-Template/network/members
[stars-shield]: https://img.shields.io/github/stars/othneildrew/Best-README-Template.svg?style=for-the-badge
[stars-url]: https://github.com/othneildrew/Best-README-Template/stargazers
[issues-shield]: https://img.shields.io/github/issues/othneildrew/Best-README-Template.svg?style=for-the-badge
[issues-url]: https://github.com/othneildrew/Best-README-Template/issues
[license-shield]: https://img.shields.io/github/license/othneildrew/Best-README-Template.svg?style=for-the-badge
[license-url]: https://github.com/othneildrew/Best-README-Template/blob/master/LICENSE.txt
[linkedin-shield]: https://img.shields.io/badge/-LinkedIn-black.svg?style=for-the-badge&logo=linkedin&colorB=555
[linkedin-url]: https://linkedin.com/in/othneildrew
[product-screenshot]: images/screenshot.png
