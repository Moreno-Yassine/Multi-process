/*************************************************************************
                           Parking  -  description
                             -------------------
    début                : 27 févr. 2014
    copyright            : (C) 2014 par ymoreno
*************************************************************************/

//---------- Réalisation de la tâche <Parking> (fichier Parking.cpp) ---

/////////////////////////////////////////////////////////////////  INCLUDE
//-------------------------------------------------------- Include système
//----------------------------------------------------- Include personnel
#include "Parking.h"
#include "Config.h"
#include "Entree.h"
#include "Simulation.h"
#include "Heure.h"
#include "ManagerSortie.h"
using namespace std;
///////////////////////////////////////////////////////////////////  PRIVE
//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types

//---------------------------------------------------- Variables statiques

//------------------------------------------------------ Fonctions privées
//static type nom ( liste de paramètres )
// Mode d'emploi :
//
// Contrat :
//
// Algorithme :
//
//{
//} //----- fin de nom

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
int main ()
{
	//Création de la mém.partagée pour la reception des requetes
	//int shmId = shmget(CLEFREQ,sizeof(Requete),0660);
	//Création de la mem.partagée pour le parking
	//int shmId = shmget(CLEFPARKING,sizeof(Parking),0660);
	//Création des BaL
    int msqIdEntree = msgget(CLEFMESSAGERIEENTREE,DROITS | IPC_CREAT);
    int msqIdSortie = msgget(CLEFMESSAGERIESORTIE,DROITS | IPC_CREAT);
	
	//Création des Semaphores
	int semId = semget(CLEFSEM,7,0660 | IPC_CREAT);	
	//Memoires partagées
    int requestAreaId = shmget(CLEFREQUEST,sizeof(Requete[3]),IPC_CREAT | DROITS);
    int parkingPlaceId = shmget(CLEFPLACES,sizeof(Voiture[8]),IPC_CREAT | DROITS);

	//Definition des Pid_t;
	pid_t simulationTache;
	pid_t entreeGastonBergerTache;
	pid_t entreeBlaisePascalAutreTache;
	pid_t entreeBlaisePascalProfTache;
	pid_t managerSortieTache;
	pid_t horlogeTache;
	//Masquage de SIGUSR2

	//Initialisation des semaphores
	semctl(semId,SEM_AUTORISATION_PORTE_1,SETVAL,0);
	semctl(semId,SEM_AUTORISATION_PORTE_2,SETVAL,0);
	semctl(semId,SEM_AUTORISATION_PORTE_3,SETVAL,0);
	semctl(semId,SEM_NB_PLACES_PK,SETVAL,NB_PLACES);
	semctl(semId,MUTEX_PLACES,SETVAL,1);
	semctl(semId,MUTEX_REQUETES,SETVAL,1);

	// Initialisation de l'application
	InitialiserApplication(TERMINAL);

	//Horloge
	horlogeTache = ActiverHeure();

	//Création des taches filles (Clavier + Entree + Sorties)
	if ( (simulationTache = fork()) == 0)
	{
		Simulation(msqIdEntree,msqIdSortie);
	} else if( (entreeGastonBergerTache = fork()) == 0 ) {
		Entree(ENTREE_GASTON_BERGER);
	} else if( (entreeBlaisePascalProfTache = fork()) == 0 ) {
		Entree(PROF_BLAISE_PASCAL);
	} else if( (entreeBlaisePascalAutreTache = fork()) ==0 ) {
		Entree(AUTRE_BLAISE_PASCAL);
	}else if( (managerSortieTache = fork()) ==0 ) {
		ManagerSortie();
	}
	else
	{
		waitpid(simulationTache,NULL,0);
		
		kill(horlogeTache,SIGUSR2);
		waitpid(horlogeTache,NULL,0);
		
		kill(entreeGastonBergerTache,SIGUSR2);
		waitpid(entreeGastonBergerTache,NULL,0);

		kill(entreeBlaisePascalProfTache,SIGUSR2);
		waitpid(entreeBlaisePascalProfTache,NULL,0);

		kill(entreeBlaisePascalAutreTache,SIGUSR2);
		waitpid(entreeBlaisePascalAutreTache,NULL,0);
		
		kill(managerSortieTache,SIGUSR2);
		waitpid(managerSortieTache,NULL,0);

		//Destruction des canaux de communication
		shmctl(requestAreaId,IPC_RMID,0);
		shmctl(parkingPlaceId,IPC_RMID,0);
		semctl(semId,0,IPC_RMID,0);
		msgctl(msqIdEntree,IPC_RMID,0);
		msgctl(msqIdSortie,IPC_RMID,0);
		TerminerApplication(true);
	}
	exit(0);
}

