/*************************************************************************
                           Entree  -  description
                             -------------------
    début                : 27 févr. 2014
    copyright            : (C) 2014 par ymoreno
*************************************************************************/

//---------- Réalisation de la tâche <Entree> (fichier Entree.cpp) ---

/////////////////////////////////////////////////////////////////  INCLUDE
//-------------------------------------------------------- Include système
#include <map>
//------------------------------------------------------ Include personnel
#include "Entree.h"
#include "Config.h"
#include <errno.h>
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
static int idElemSemSynchro;
static int idMessagerie;
static TypeBarriere barriereCourante;
//------------------------------------------------------ Fonctions privées
void destruction ();

void finEntree( int noSignal )
{
	if ( noSignal == SIGUSR2 )
	{
		sigaction (SIGCHLD,NULL,NULL);	//On remasque sigchld pour éviter l'appel à MortVoiturier
		for ( map<pid_t,Voiture>::iterator itListePid = pidVoituriers.begin() ; itListePid != pidVoituriers.end(); itListePid++ )
		{
			kill(itListePid->first,SIGUSR2);
			while(waitpid(itListePid->first,NULL,0)!=-1);
		}
		destruction();
	}
}

void mortVoiturier(int noSignal)
{
	if (noSignal == SIGCHLD)
	{
		int crdu;
		pid_t noVoiturierDecede = waitpid(-1,&crdu,0);
		if ( WIFEXITED( crdu ) )
		{
			int noPlace = WEXITSTATUS( crdu );
			map<pid_t,Voiture>::iterator itVoiture = pidVoituriers.find(noVoiturierDecede);
			if(itVoiture!=pidVoituriers.end())
			{
				Voiture voitureCourante = itVoiture->second;
				
				//Chargement MP(Places)-----
				sembuf bufferMP; //Abaissement MUTEX
				bufferMP.sem_num = MUTEX_PLACES;
				bufferMP.sem_op = -1;
				bufferMP.sem_flg = 0;
				semop(semId,&bufferMP,1);

				ptPlaces[noPlace] = voitureCourante; //Remplissage place dans la MP

				bufferMP.sem_op = 1; //Relèvement MUTEX
				semop(semId,&bufferMP,1);
				//------fin chargement

				AfficherPlace( noPlace, voitureCourante.conducteur , voitureCourante.numVoiture, voitureCourante.arrivee );
				pidVoituriers.erase(itVoiture);
			}


		}
	}
	//Afficher dans la zone correspondante

}

void initialisation()
{
	semId = semget(CLEFSEM,7,0660);
	Afficher (MESSAGE,errno);
	sleep(3);
	requestAreaId = shmget(CLEFREQUEST,sizeof(Requete[3]),0);
	parkingPlaceId = shmget(CLEFPLACES,sizeof(Voiture[8]),0);
	ptPlaces = (Voiture*)shmat(parkingPlaceId,NULL,0);
	ptRequetes = (Requete*)shmat(requestAreaId,NULL,0);
	idMessagerie = msgget(CLEFMESSAGERIEENTREE,0); //0660 | IPC_CREAT

	struct sigaction actionFinEntree; //HANDLER de SIGUSR2
	struct sigaction actionMortVoiturier; // HANDLER DE SIGCHLD
	actionFinEntree.sa_handler = finEntree;
	actionMortVoiturier.sa_handler = mortVoiturier;
	sigemptyset(&actionFinEntree.sa_mask);
	sigemptyset(&actionMortVoiturier.sa_mask);
	actionFinEntree.sa_flags = 0;
	actionMortVoiturier.sa_flags = 0;
	//ARMEMENT
	sigaction (SIGUSR2,&actionFinEntree,NULL);
	sigaction (SIGCHLD,&actionMortVoiturier,NULL);
}

void moteur (int numPorte)
{
	long type = numPorte;// a définir dans la config
	int taille = sizeof(MessagePorte);// a definir dans la config
	MessagePorte messageEntrant;
	int msg=-1;

	msg = msgrcv (idMessagerie,&messageEntrant,taille,type,type);
	Afficher (MESSAGE,errno);
	if (msg !=-1)
	{
		DessinerVoitureBarriere(barriereCourante,AUTRE);
		
		//Création de la requète
		Requete requeteCourante;
		requeteCourante.barriere = IntToPorte(numPorte);
		requeteCourante.usager = messageEntrant.vehiculeEntrant.conducteur;
		requeteCourante.daterequete = time(NULL);	//On enregistre l'heure d'arrivée devant la porte


		//Chargement de la MP requete---
		sembuf bufferMutexRequest;
		bufferMutexRequest.sem_num = MUTEX_REQUETES;
		bufferMutexRequest.sem_op = -1;
		bufferMutexRequest.sem_flg = 0;

		ptRequetes[numPorte-1] = requeteCourante;

		bufferMutexRequest.sem_op = 1;
		semop(semId,&bufferMutexRequest,1);
		//--fin chargement requete

		AfficherRequete(barriereCourante,
						messageEntrant.vehiculeEntrant.conducteur,
						requeteCourante.daterequete);

		//Vérification remplissage parking
		sembuf bufferPlaces;
		bufferPlaces.sem_num = SEM_NB_PLACES_PK;
		bufferPlaces.sem_op = -1;
		bufferPlaces.sem_flg = 0;
		if(semctl(semId,SEM_NB_PLACES_PK,GETVAL,0)==0)
		//Le PK est plein
		{
			sembuf bufferSynchro;
			bufferSynchro.sem_num = idElemSemSynchro;
			bufferSynchro.sem_op = -1;
			bufferSynchro.sem_flg = 0;
			//En attente de la sortie pour autorisation de passage
			while(semop(semId,&bufferSynchro,1)!=0);
			int i = semctl(semId,idElemSemSynchro,GETVAL,0);
			Afficher(MESSAGE,i);
			sleep(1);
		} else {
			//On enleve une place
			//semop(semId,&bufferPlaces,1);
			while(semop(semId,&bufferPlaces,1)!=0);
		}

		TypeBarriere barriere = IntToPorte(numPorte);
		pid_t pidVoiturierCourant = GarerVoiture ( barriere );
		if(pidVoiturierCourant!=-1)
		{
			pidVoituriers.insert(make_pair(pidVoiturierCourant, messageEntrant.vehiculeEntrant));
			//Vidage de la MP requete---
			bufferMutexRequest.sem_op = -1;
			semop(semId,&bufferMutexRequest,1);

			ptRequetes[numPorte-1] = REQUETE_NULLE;

			bufferMutexRequest.sem_op = 1;
			semop(semId,&bufferMutexRequest,1);
			//--fin chargement requete	
			sleep(TEMPO);
		}
		
	}
}

void destruction ()
{
	shmdt(ptRequetes);
	shmdt(ptPlaces);

	exit(0);
}

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
void Entree(int numPorte)
{
	switch(numPorte)
	{
	case 1:
		barriereCourante = PROF_BLAISE_PASCAL;
		idElemSemSynchro = SEM_AUTORISATION_PORTE_1;
		break;
	case 2:
		barriereCourante = AUTRE_BLAISE_PASCAL;
		idElemSemSynchro = SEM_AUTORISATION_PORTE_2;
		break;
	case 3:
		barriereCourante = ENTREE_GASTON_BERGER;
		idElemSemSynchro = SEM_AUTORISATION_PORTE_3;
		break;
	}

	initialisation();
	for(;;)
		moteur(numPorte);
}
