/*************************************************************************
                           ManagerSortie  -  description
                             -------------------
    début                : 19 févr. 2014
    copyright            : (C) 2014 par ymoreno
*************************************************************************/

//---------- Réalisation de la tâche <ManagerSortie> (fichier ManagerSortie.cpp) ---

/////////////////////////////////////////////////////////////////  INCLUDE
//-------------------------------------------------------- Include système

//------------------------------------------------------ Include personnel
#include "ManagerSortie.h"
#include "Config.h"
#include <algorithm>
#include <vector>
#include <map>
using namespace std;
///////////////////////////////////////////////////////////////////  PRIVE
//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types

//---------------------------------------------------- Variables statiques
static int idMessagerie;
static int semId;
static int requestAreaId;
static int parkingPlaceId;
static map<pid_t,int> pidVideurs;
static Voiture * ptPlaces;
static Requete * ptRequetes;
//------------------------------------------------------ Fonctions privées
static void destruction();

static void finSortie ( int noSignal )
{
	if (noSignal==SIGUSR2)
	{
		sigaction (SIGCHLD,NULL,NULL);
		for ( map<pid_t,int>::iterator itListePid = pidVideurs.begin() ; itListePid != pidVideurs.end(); itListePid++ )
		{
			kill(itListePid->first,SIGUSR2);
			while(waitpid(itListePid->first,NULL,0)!=0);
		}
		destruction();
		
	}
}

static void mortVideur( int noSignal )
{
	if (noSignal == SIGCHLD)
	{
		int crdu;
		pid_t noVoiturierDecede = waitpid(-1,&crdu,0);
		if ( WIFEXITED( crdu ) )
		{
			int noPlace = WEXITSTATUS( crdu );
			//Extraction place ------
			sembuf bufferMutexPlaces;
			bufferMutexPlaces.sem_num = MUTEX_PLACES;
			bufferMutexPlaces.sem_op = 1;
			bufferMutexPlaces.sem_flg = 0;
			semop(semId,&bufferMutexPlaces,1);
			Voiture voitureCourante = ptPlaces[noPlace]; //On commence à 0 ou à 1?????
			bufferMutexPlaces.sem_op = -1;
			semop(semId,&bufferMutexPlaces,1);
			//----Fin extraction

			AfficherSortie(voitureCourante.conducteur, voitureCourante.numVoiture, voitureCourante.arrivee, time(NULL) );
			pidVideurs.erase(pidVideurs.find(noVoiturierDecede));
			sembuf bufferPlaces;
			bufferPlaces.sem_num = SEM_NB_PLACES_PK;
			bufferPlaces.sem_op = 1;
			bufferPlaces.sem_flg = 0;
			if(semctl(semId,SEM_NB_PLACES_PK,GETVAL,0)==0)
			//Si le parking est plein, on va donner l'autorisation au bon usager
			{
				sembuf mutexPlacesVerif;
				mutexPlacesVerif.sem_num = MUTEX_PLACES;
				mutexPlacesVerif.sem_op = -1;
				mutexPlacesVerif.sem_flg = 0;
				int verif = semop(semId,&mutexPlacesVerif,1);


				vector<Requete> listeRq;
				listeRq.push_back(ptRequetes[0]);
				listeRq.push_back(ptRequetes[1]);
				listeRq.push_back(ptRequetes[2]);				

				mutexPlacesVerif.sem_op =1;
				semop(semId,&mutexPlacesVerif,1);


				sort(listeRq.begin(),listeRq.end(),TrierRequetes);
				if(listeRq[0].usager != AUCUN)
				{
					sembuf bufferSynchro;
					bufferSynchro.sem_num = listeRq[0].barriere;
					bufferSynchro.sem_op = 1;
					bufferSynchro.sem_flg = 0;
					semop(semId,&bufferSynchro,1);
				} else {
					semop(semId,&bufferPlaces,1);				
				}
			} else {
				semop(semId,&bufferPlaces,1);

			}
		}
	}
}

static void initialisation()
{
	//Recuperation COMM
	semId = semget(CLEFSEM,7,0);
	requestAreaId = shmget(CLEFREQUEST,sizeof(Requete[3]),0);
	parkingPlaceId = shmget(CLEFPLACES,sizeof(Voiture[8]),0);
	ptPlaces = (Voiture*)shmat(parkingPlaceId,NULL,0);
	ptRequetes = (Requete*)shmat(requestAreaId,NULL,0);
	idMessagerie = msgget(CLEFMESSAGERIESORTIE,0);

	struct sigaction actionFinManager; //HANDLER de SIGUSR2
	struct sigaction actionMortVideur; //HANDLER de SIGCHLD
	actionFinManager.sa_handler = finSortie;
	actionMortVideur.sa_handler = mortVideur;
	sigemptyset(&actionFinManager.sa_mask);
	sigemptyset(&actionMortVideur.sa_mask);
	actionFinManager.sa_flags = 0;
	actionMortVideur.sa_flags = 0;
	sigaction (SIGUSR2,&actionFinManager,NULL);
	sigaction (SIGCHLD,&actionMortVideur,NULL);

}
static void moteur ()
{
	long type = 0;
	unsigned int taille = sizeof(int);
	unsigned int place;
	int msg = msgrcv (idMessagerie,&place,taille,type,0);
	pid_t videur;
	if (msg!= -1)
	{
		// GESTION DE SEMAPHORES
		videur = SortirVoiture (place);
		if(videur!=-1)
			pidVideurs.insert(make_pair(videur,place));
	}
	//waitpid (videur, NULL , 0);
	//Effacer ( TypeZone zone );

	//sleep (TEMPO);		????? c'est ici qu'elle va la tempo?
}
static void destruction ()
{
	shmdt(ptRequetes);
	shmdt(ptPlaces);
	exit(0);
}
//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
void ManagerSortie ()
{
	initialisation();
	for(;;)
	moteur();
}
