/*************************************************************************
                           Simulation  -  description
                             -------------------
    début                : 27 févr. 2014
    copyright            : (C) 2014 par ymoreno
*************************************************************************/

//---------- Réalisation de la tâche <Simulation> (fichier Simulation.cpp) ---

/////////////////////////////////////////////////////////////////  INCLUDE
//-------------------------------------------------------- Include système
#include <stdlib.h>
#include <time.h>
//------------------------------------------------------ Include personnel
#include "Simulation.h"
#include "Menu.h"
#include "Config.h"
#include <errno.h>
///////////////////////////////////////////////////////////////////  PRIVE
//------------------------------------------------------------- Constantes
const char CODE_Q = 'Q';
const char CODE_A = 'A';
const char CODE_P = 'P';
const char CODE_S = 'S';
const char* MSG_SORTIE = "Sortie entamée";
//------------------------------------------------------------------ Types

//---------------------------------------------------- Variables statiques
static int msqEntree;
static int msqSortie;
//------------------------------------------------------ Fonctions privées

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques

void Simulation (int msqIdEntree,int msqIdSortie)
{
	msqEntree = msqIdEntree;
	msqSortie = msqIdSortie;
	for(;;)
	Menu();
}

void Commande ( char code, unsigned int valeur )
{
	TypeBarriere barriere;
	unsigned int placeSortie;
	static int compteur = 1; // num des voiture
	switch (code)
	{
	case (CODE_Q) :
					exit(0);
					break;
	case (CODE_A) :
					//créer voiture AUTRE,Compteur,HeureArrivee
							if (valeur == 1)
							{
								barriere = AUTRE_BLAISE_PASCAL;
							}
							if (valeur == 2)
							{
								barriere = ENTREE_GASTON_BERGER;
							}
					Voiture v;
					v.conducteur = AUTRE;
					v.numVoiture = compteur++;
					v.arrivee = time(NULL);
					MessagePorte carsent;
					carsent.type = barriere;
					carsent.vehiculeEntrant = v;
                    msgsnd(msqEntree,&carsent,sizeof(MessagePorte),barriere);// DEPOT BLOQUANT SI SATURATION
					break;
	case (CODE_P) :
					//créer voiture AUTRE,Compteur,HeureArrivee
							if (valeur == 1)
							{
								barriere = PROF_BLAISE_PASCAL;
							}
							if (valeur == 2)
							{
								barriere = ENTREE_GASTON_BERGER;
                            }
					Voiture vProf;
					vProf.conducteur = PROF;
					vProf.numVoiture=compteur++;
					vProf.arrivee = time(NULL);
					MessagePorte carsentProf;
					carsentProf.type = barriere;
					carsentProf.vehiculeEntrant=vProf;
                    msgsnd(msqEntree,&carsentProf,sizeof(MessagePorte),barriere);
					//Voiture v (PROF,compteur++,time(NULL))
					// DEPOT BLOQUANT SI SATURATION
					break;
	case (CODE_S) :
					//sortie voiture ...
					placeSortie = valeur;
                    msgsnd (msqSortie,&placeSortie,sizeof(unsigned int),0);
					break;
	default :
		break;

	}
	if (compteur>999)
	{
		compteur = 0;
	}
}
