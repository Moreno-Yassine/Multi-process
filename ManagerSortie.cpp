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
using namespace std;
///////////////////////////////////////////////////////////////////  PRIVE
//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types

//---------------------------------------------------- Variables statiques
static map<pid_t,Voiture> pidVoituriers;
static int semId;
static int requestAreaId;
static int parkingPlaceId;
static Voiture * ptPlaces;
static Requete * ptRequetes;
//------------------------------------------------------ Fonctions privées

/*void finSortie ( int noSignal )
{

}

void mortVideur( int noSignal )
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

			sembuf bufferPlaces;
			bufferPlaces.sem_num = SEM_NB_PLACES_PK;
			bufferPlaces.sem_op = 1;
			bufferPlaces.sem_flg = 0;
			if(semctl(semId,SEM_NB_PLACES_PK,GETVAL,0)==0)
			//Si le parking est plein, on va donner l'autorisation au bon usager
			{
				if(semctl(semId,MUTEX_REQUETE,GETVAL,0)!=0)
				{
					vector<Requete> listeRq;
					listeRq.push_back(ptRequetes[0]);
					listeRq.push_back(ptRequetes[1]);
					listeRq.push_back(ptRequetes[2]);
					sort(listeRq.begin(),listeRq.end(),TrierRequetes);
					if(listeRq[0]!=NULL)
					{
						sembuf bufferSynchro;
						bufferSynchro.sem_num = listeRq[0].barriere; //ARCHI FAUX!!, mais ça doit marcher (def de l'enum des sem)
						bufferSynchro.sem_op = 1;
						bufferSynchro.sem_flg = 0;
						semop(semId,&bufferSynchro,1);
					}
				}
			} else {
				semop(semId,&bufferPlaces,1);
			}
		}
	}
}

void initialisation()
{
	//Recuperation COMM
	semId = semget(CLEFSEM,7,0);
	requestAreaId = shmget(CLEFREQUEST,sizeof(Requete[3]),0);
	parkingPlaceId = shmget(CLEFPLACES,sizeof(Voiture[8]),0);
	ptPlaces = shmat(requestPlaceId,NULL,0);
	ptRequetes = shmat(requestAreaId,NULL,0);


	struct sigaction actionFinManager; //HANDLER de SIGUSR2
	struct sigaction actionMortVideur; //HANDLER de SIGCHLD
	actionFinManager.sa_handler = finSortie;
	actionMortVideur.sa_handler = mortVideur;
	sigemptyset(&actionFinManager.sa_mask);
	sigemptyset(&actionMortVideur.sa_mask);
	actionFinManager.sa_flags = 0;
	actionMortVideur.sa_flags = 0;
	sigaction (SIGUSR2,&actionFinManager,NULL);
	sigaction (SIGUSR2,&actionMortVideur,NULL);

}
void moteur (int msqId)
{
	long type = 0;
	unsigned int taille = sizeof(int);
	unsigned int place;
	//int msg = msgrcv (msqId,&place,taille,type,0);
	pid_t videur = SortirVoiture (place);
	waitpid (videur, NULL , 0);
	//Effacer ( TypeZone zone );


}
void destruction ()
{
	shmdt(ptRequetes);
	shmdt(ptPlaces);
}
//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
void ManagerSortie (int msqId)
{
	initialisation();
	moteur(msqId);
}*/
