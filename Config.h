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
#include "Outils.h"
#include <errno.h>
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
const TypeTerminal TERMINAL = VT220;
const int NB_SEMAPHORES = 7;
const key_t CLEFMESSAGERIEENTREE = ftok("Parking",1);
const key_t CLEFMESSAGERIESORTIE = ftok("Parking",2);
const key_t CLEFSEM = ftok("Parking",3);
const key_t CLEFPLACES = ftok("Parking",4);
const key_t CLEFREQUEST = ftok("Parking",5);
const int TEMPO = 1;

// INDICE_BARRIERE_1 indique le numéro de la premiere porte 
// (ici le numéro associé à PROF_BLAISE_PASCAL est 1)
const int INDICE_BARRIERE_1 = 1;

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
{	NONE,
	SEM_AUTORISATION_PORTE_1,
	SEM_AUTORISATION_PORTE_2,
	SEM_AUTORISATION_PORTE_3,
	SEM_NB_PLACES_PK,
	MUTEX_REQUETES,
	MUTEX_PLACES
};

const Requete REQUETE_NULLE = {AUCUNE,AUCUN,100}; 
const int TAILLE_MESSAGE_PORTE = sizeof(MessagePorte);
//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
TypeBarriere IntToPorte (int porte);
// Mode d'emploi :
//	Renvoie la barriere correspondant au numero passé en paramètre
// Contrat :
//	Sans objet

bool TrierRequetes ( Requete a, Requete b );

#endif // CONFIG_H_

