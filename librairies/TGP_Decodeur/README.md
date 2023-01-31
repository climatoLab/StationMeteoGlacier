# Librairie TGP Decodeur

Permet de décoder les messages selon le format 'C a1 a2 a3 ...' où C est une lettre représentant une commande donnée, suivis d'une liste d'arguments de taille et de type variable. La fin de la ligne doit se terminer par le caractère '\n' .

La librairie fonctionne avec tout objet qui implémente la classe [Stream](https://www.arduino.cc/reference/en/language/functions/communication/stream/); l'objet fournira le texte brut qui sera décodé.

## Notes de version

### 1.1.2 :
- Changé l'URL du projet pour pointer sur GitHub.

### 1.1.1 :
- Idem que précédent, avec un correcif pour le caractère '\n' et '\r'.

### 1.1.0 :
- Correction si le message reçu ne contient qu'un retour de chariot.

### 1.0.0 :
- Première version officielle, crée à partir de l'ancienne librairie SerieTPhys.




## Utilisation

```cpp
#include <Decodeur.h> 

Decodeur monDecodeur(&Serial); //fournir un objet qui implémente la classe Stream

void setup()
{
 
}

void loop()
{

  if (monDecodeur.available()) // Si du texte est disponible
  {

   char command = monDecodeur.getCommand();   //Obtenir le caractère de commande décodé.
   int argCount =  monDecodeur.getArgCount(); //Obtenir le nombre d'arguments décodé.
    
  for (int no = 0; no < argCount; no++) //Pour chaque argument
    {
      float arg = monDecodeur.getArg(no)); //Obtenir l'argument d'indice 'no'
    }
  }
}

```

## Constructeurs
```cpp
Decodeur(Stream *stream)
Decodeur(Stream *stream,char separateur, int format)
```
On spécifie l'objet qui implémente la classe [Stream](https://www.arduino.cc/reference/en/language/functions/communication/stream/) (dans Arduino on retrouve les objets suivants: Serial, Ethernet, Wire, SD ). On doit fournir l'addresse de l'objet, en ajoutant le caractère '&' devant celui-ci (voir section Utilisation).

On spécifie facultativement le caractère servant à séparer la commande et les arguments (par défaut, l'espace ' '), ainsi que le format attendu des arguments (mots définis ENTIER, HEXA ou FLOTTANT).

## Méthodes disponibles

```cpp
bool available()
```
Cette méthode indique si du texte a été reçu et décodé.

---
```cpp
char getCommand()
```
Permet de lire le caractère de commande décodé.

---
```cpp
int getArgCount()
```
Permet de lire le nombre d'arguments décodés.

---

```cpp
float getArg(int index)
```
Permet de lire l'argument d'index donné. Retourne 0 en cas d'indice non valide, ou si l'argument n'a pas été correctement décodé.
La variable retournée est de type float, on peut le réduire à un entier par 'casting explicite'.
