/****************************************************************
// Librairie Decodeur
//Auteurs : Claude Bouchard / Jude Levasseur
//Date : nov 2021
//version : 1.1.1
//Description: Permet de lire et décoder des commandes et des
//arguments dans un format simplifié
//***************************************************************/
#include "Stream.h"
#include "Decodeur.h"

/***************************************************************
// Déclaration des variables et constantes privées
//***************************************************************/
char _IncomingByte;						 //Variable pour la lecture du buffer du UART
char _InComBuffer[_maxComLine + 1] = {}; //Buffer pour emmagasiner les entrées du UART
char _Commande;							 //Variable pour conserver la commande, seulement le premier caractère n'est considéré
int _InComIndex;						 //Index pour ajouter les nouveaux caractères entrants au Buffer
float _Arguments[_maxArg];				 //Vecteur pour les arguments suivants la commande
int _NbArg;								 //Variable pour le nombre d'argument
char _separateur[2];					 //Séparateur par défaut est une espace
int _base;								 //Variable pour contenir la base

char _CommandeStr[_maxCommandeStr + 1] = {}; //Variable string pour commande de type string
Stream *_MyStream;							 //Pointeur pour permettre de sélectionner le type de communication

/***************************************************************
// Constructeur
//***************************************************************/
Decodeur::Decodeur(Stream *stream)
{
	_IncomingByte = 0;
	_MyStream = stream;
	_InComIndex = 0;
	_NbArg = 0;
	_separateur[0] = ' ';
	_base = ENTIER;
}
Decodeur::Decodeur(Stream *stream, char separateur, int base)
{
	_IncomingByte = 0;
	_MyStream = stream;
	_InComIndex = 0;
	_NbArg = 0;
	_separateur[0] = separateur;
	_base = base;
}
/***************************************************************************************
//Méthodes publiques
//***************************************************************************************/
bool Decodeur::estDisponible() { return available(); }			  //Temporaire pour transition.
int Decodeur::nombreArgument() { return getArgCount(); }		  //Temporaire pour transition.
char Decodeur::lireCommande() { return getCommand(); }			  //Temporaire pour transition.
float Decodeur::lireArgument(int noArg) { return getArg(noArg); } //Temporaire pour transition.

/****************************************************************
Fonction pour initialiser le décodage du Stream de données.
Permet de configurer aussi le type de séparateur entre chaque
arguments ainsi que la base. La base doit être:

Même fonction que ci-dessus, mais qui fonctionne avec les cartes Arduino qui possède
uniquement le port série Serial0. Donc, il n'y a pas de paramètre selection.
****************************************************************/

/****************************************************************
Fonction pour indiquer si une commande à été reçue.
****************************************************************/
bool Decodeur::available()
{
	if (lireBuffer())
	{
		decoderCommande();
		return true;
	}
	else
	{
		return false;
	}
}

/****************************************************************
Fonction pour indiquer le nombre d'arguments reçues.
****************************************************************/
int Decodeur::getArgCount()
{
	return _NbArg;
}

/****************************************************************
Fonction pour retourner la commande reçue.
****************************************************************/
char Decodeur::getCommand()
{
	return _Commande;
}

/****************************************************************
Fonction pour retourner l'argument sélectionnée
****************************************************************/
float Decodeur::getArg(int noArg)
{
	if ((noArg < 0) || (noArg) > _maxArg)
	{
		return 0;
	}
	else
	{
		return _Arguments[noArg];
	}
}

/***************************************************************************************
//Méthodes privées
//**************************************************************************************/

/*********************************************************
//Routine pour décoder les tokens de la commande
//********************************************************/

bool Decodeur::lireBuffer()
{
	_IncomingByte = -1;
	while ((_MyStream->available() > 0) && (_IncomingByte != '\n'))
	{

		if (_InComIndex >= _maxComLine)
		{					 //Pour éviter le débordement,
			_InComIndex = 0; //on flush le buffer si trop de caractères reçus
			_InComBuffer[_InComIndex] = '\0';
		}
		// Lecture du byte reçu et ajout au Buffer de reception
		_IncomingByte = _MyStream->read();
		_InComBuffer[_InComIndex] = _IncomingByte; //Ajoute le byte reçu dans dans le buffer
		++_InComIndex;
		_InComBuffer[_InComIndex] = '\0'; //Déplace le caractère de fin de chaine
	}
	if (_IncomingByte == '\n' || _IncomingByte == '\r')
	{
		if (_InComIndex < 2)
		{					 //Correction pour une réception avec \n seulement
			_InComIndex = 0; //on flush le buffer si seulement \n
			_InComBuffer[_InComIndex] = '\0';
			return false;
		}
		else
		{
			return true;
		}
	}
	else
	{
		return false;
	}
}

bool Decodeur::decoderCommande()
{
	SplitToken(_InComBuffer, &_Commande, _Arguments, _base); //on appelle la fonction pour splitter les tokens (entier)

	_InComIndex = 0;				  //On flush le buffer,
	_InComBuffer[_InComIndex] = '\0'; //en inscrivant le caratère de fin de chaine à son début.

	return true; //On retrourne "vrai" si une commande complete reçue
}

/*********************************************************
// Fonction pour séparer les tokens
//********************************************************/
void Decodeur::SplitToken(char Buf[], char *Comm, float Arg[], int base)
{
	_NbArg = 0;
	char *p; // Pointeur pour la valeur de retour de la fonction "strtok"
	int i;
	//char separators[] = " \t.,!?\n\r";  		//Liste des séparateurs valides

	//Correction pour séparateurs; effectuée à la version 1.4

	//char separators[2];
	//strcpy(separators, _separateur);

	char separators[4];
	strcpy(separators, _separateur);
	strcat(separators, "\n\r"); //Ajout de CR et LF comme délimiteurs par défaut. Permet de régler le nombre d'argument = 1 lorsque la commande est seule.
								//FIN Correction pour séparateurs; effectuée à la version 1.4

	p = strtok(Buf, separators); //Premier appel de strtok() pour isoler le 1er token, soit la commande
	*Comm = p[0];				 //Enregistre la commande d'un seul charactère (pour conserver la compatibilité)

	//Ajout commande chaine pour version 1.4
	strncpy(_CommandeStr, p, _maxCommandeStr); //Enregistre la commande sous frome de chaine; limité à la longueur maximale "_maxCommandeStr"
	//FIN Ajout commande chaine pour version 1.4

	i = 0;
	while (p != NULL && i < _maxArg) //Boucle pour isoler les tokens arguments suivants la commande
	{
		p = strtok(NULL, separators); //Appels successifs de strtok isolant les tokens
		if (p != NULL)
		{
			Arg[i++] = convertirArg(p, base);
		}
	}
	_NbArg = i; //Enregistre le nombre de tokens arguments
}

/*********************************************************
// Fonction pour convertir l'argument textuel en type float
//********************************************************/

float Decodeur::convertirArg(char *p, int base)
{
	switch (base)
	{
	case ENTIER:
		return atoi(p);
		break; //Conversion token en valeur integer et sauvegarde
	case HEXA:
		return HexaToDecimal(p); //Conversion token en valeur hexadecimale et sauvegarde
		break;
	case FLOTTANT:
		return atof(p); //Conversion token en valeur hexadecimale et sauvegarde
		break;
	default:
		return 0;
	}
}

/*********************************************************
// Fonction pour convertir une chaîne de caractères
// exprimée en valeur hexadécimale en sa valeur décimale
// correspondante.
//********************************************************/
int Decodeur::HexaToDecimal(const char *Hexa)
{
	int ret = 0, t = 0, n = 0;
	const char *c = Hexa;
	while (*c && (n < 16))
	{
		if ((*c >= '0') && (*c <= '9'))
			t = (*c - '0');
		else if ((*c >= 'A') && (*c <= 'F'))
			t = (*c - 'A' + 10);
		else if ((*c >= 'a') && (*c <= 'f'))
			t = (*c - 'a' + 10);
		else
			break;
		n++;
		ret *= 16;
		ret += t;
		c++;
		if (n >= 8)
			break;
	}
	return ret;
}
