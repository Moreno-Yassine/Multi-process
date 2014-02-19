/*************************************************************************
                           ManagerSortie  -  description
                             -------------------
    début                : 19 févr. 2014
    copyright            : (C) 2014 par ymoreno
*************************************************************************/

//---------- Réalisation de la tâche <ManagerSortie> (fichier ManagerSortie.cpp) ---

/////////////////////////////////////////////////////////////////  INCLUDE
//-------------------------------------------------------- Include système
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <cstdlib>
#include <signal.h>
#include <wait.h>
//------------------------------------------------------ Include personnel
#include "ManagerSortie.h"
#include "Outils.h"
using namespace std;
///////////////////////////////////////////////////////////////////  PRIVE
//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types

//---------------------------------------------------- Variables statiques

//------------------------------------------------------ Fonctions privées
static void stationnement (int msqId)
{

}
static void finSortie ()
{

}

// les PHASES
static void initialisation()
{
	struct sigaction actionStation;
	actionStation.sa_handler = finSortie;
	sigemptyset(&actionStation.sa_mask);
	actionStation.sa_flags = 0;
	sigaction (SIGUSR2,&actionStation,NULL);

}
static void moteur (int msqId)
{
	long type;// a définir dans la config
	int taille;// a definir dans la config
	unsigned int *pointeur; // struct voiture ???? (pour afficher sortie)
	int msg = msgrcv (msqId,pointeur,taille,type,0);
	pid_t videur = SortirVoiture (*pointeur);
	waitpid (videur, NULL , 0);
	Effacer(Zone du véhicule);
	AfficherSortie(caracteristiques);

}
static void destruction ()
{

}
//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
void ManagerSortie ()
{


}
