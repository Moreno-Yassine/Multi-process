/*************************************************************************
                           Config  -  description
                             -------------------
    début                : 27 févr. 2014
    copyright            : (C) 2014 par ymoreno
*************************************************************************/

//---------- Interface du module <Config> (fichier Config.h) ---------
#if ! defined ( CONFIG_H_ )
#define CONFIG_H_

//------------------------------------------------------------------------
// Rôle du module <Config>
//
//
//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////  INCLUDE
//--------------------------------------------------- Interfaces utilisées
#include "/public/tp/tp-multitache/Outils.h"
#include <sys/msg.h>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <cstdlib>
#include <signal.h>
#include <wait.h>
#include <algorithm>
    
//------------------------------------------------------------- Constantes
const TypeTerminal TERMINAL = XTERM;
const int NB_SEMAPHORES = 7;
const key_t CLEFMESSAGERIEENTREE = ftok("Parking",1);
const key_t CLEFMESSAGERIESORTIE = ftok("Parking",2);
const key_t CLEFPARKING = ftok("Parking",3);
const key_t CLEFREQ = ftok("Parking",4);
const key_t CLEFSEM = ftok("Parking",5);
const key_t CLEFPLACES = ftok("Parking",6);
const key_t CLEFREQUEST = ftok("Parking",7);
//Id du bloc de semaphore, partagé avec tous les modules
const int TEMPO = 1;
//------------------------------------------------------------------ Types
struct Voiture
{
	TypeUsager conducteur;
	unsigned int numVoiture;
	time_t arrivee;
};

struct Parking
{
	Voiture places [NB_PLACES];
};

struct Requete
{
	TypeBarriere barriere;
	TypeUsager usager;
	time_t daterequete;
};
struct MessagePorte
{
	long type; // type variable suivant la porte
	Voiture vehiculeEntrant;
};

enum Semaphores
{	SEM_NB_PLACES_PK,
	SEM_AUTORISATION_PORTE_1,
	SEM_AUTORISATION_PORTE_2,
	SEM_AUTORISATION_PORTE_3,
	MUTEX_REQUETES,
	MUTEX_PLACES
};
//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
TypeBarriere IntToPorte (int porte);
// Mode d'emploi :
//	Renvoie la barriere correspondant au numero passé en paramètre
// Contrat :
//	Sans objet

bool TrierRequetes ( Requete a, Requete b );

#endif // CONFIG_H_

