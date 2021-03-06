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
static long typeMessage;
static TypeZone zoneRequete;
//------------------------------------------------------ Fonctions privées
void destruction ();

void finEntree( int noSignal )
// finEntree élimine tous les voituriers en execution
{
	if ( noSignal == SIGUSR2 )
	{
		sigaction (SIGCHLD,NULL,NULL);	//On remasque sigchld pour éviter l'appel à MortVoiturier
		for ( map<pid_t,Voiture>::iterator itListePid = pidVoituriers.begin() ; itListePid != pidVoituriers.end(); itListePid++ )
		{
			kill(itListePid->first,SIGUSR2);
            waitpid(itListePid->first,NULL,0);
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
// initialisation des canaux de communication (en ecriture pour les MP)
// armement des signaux SIGCHLD pour la mort des voituriers et SIGUSR2
// pour la destruction de la tâche
{
    semId = semget(CLEFSEM,7,DROITS);
    requestAreaId = shmget(CLEFREQUEST,sizeof(Requete[3]),ECRITURE);
    parkingPlaceId = shmget(CLEFPLACES,sizeof(Voiture[8]),ECRITURE);
    ptPlaces = (Voiture*)shmat(parkingPlaceId,NULL,ECRITURE);
    ptRequetes = (Requete*)shmat(requestAreaId,NULL,ECRITURE);
    idMessagerie = msgget(CLEFMESSAGERIEENTREE,DROITS);

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

void moteur (TypeBarriere barriereCourante)
// Moteur récupère les voitures dans sa boîte aux lettres,
// crée immédatement la requète afin d'enregistrer l'heure
// charge la requète dans la MP, et attend une autorisation si le parking est plein
{
	MessagePorte messageEntrant;
	int msg=-1;

	msg = msgrcv (idMessagerie,&messageEntrant,TAILLE_MESSAGE_PORTE,typeMessage,typeMessage);
	if (msg !=-1)
	{
		//Création de la requète
		Requete requeteCourante;
		requeteCourante.barriere = barriereCourante;
		requeteCourante.usager = messageEntrant.vehiculeEntrant.conducteur;
        requeteCourante.daterequete = time(NULL);	//enregistre l'heure d'arrivée devant la porte

		DessinerVoitureBarriere(barriereCourante,requeteCourante.usager);

		//Chargement de la MP requete---
		sembuf bufferMutexRequest;
		bufferMutexRequest.sem_num = MUTEX_REQUETES;
		bufferMutexRequest.sem_op = -1;
		bufferMutexRequest.sem_flg = 0;

		ptRequetes[barriereCourante-INDICE_BARRIERE_1] = requeteCourante;

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
            semctl(semId,idElemSemSynchro,GETVAL,0);
		} else {
			//On enleve une place
			//semop(semId,&bufferPlaces,1);
			while(semop(semId,&bufferPlaces,1)!=0);
		}

		pid_t pidVoiturierCourant = GarerVoiture ( barriereCourante );
		if(pidVoiturierCourant!=-1)
		{
			pidVoituriers.insert(make_pair(pidVoiturierCourant, messageEntrant.vehiculeEntrant));
			//Vidage de la MP requete---
			bufferMutexRequest.sem_op = -1;
			semop(semId,&bufferMutexRequest,1);

			ptRequetes[barriereCourante-INDICE_BARRIERE_1] = REQUETE_NULLE;

			bufferMutexRequest.sem_op = 1;
			semop(semId,&bufferMutexRequest,1);
			//--fin chargement requete	
            Effacer(zoneRequete);
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
void Entree(TypeBarriere barriereCourante)
{
	typeMessage = (int)barriereCourante;
	switch(barriereCourante)
	{
	case PROF_BLAISE_PASCAL:
		idElemSemSynchro = SEM_AUTORISATION_PORTE_1;
        zoneRequete = REQUETE_R1;
		break;
	case AUTRE_BLAISE_PASCAL:
		idElemSemSynchro = SEM_AUTORISATION_PORTE_2;
        zoneRequete = REQUETE_R2;
		break;
	case ENTREE_GASTON_BERGER:
		idElemSemSynchro = SEM_AUTORISATION_PORTE_3;
        zoneRequete = REQUETE_R3;
		break;
    default:
        return;
        break;
	}

	initialisation();
	for(;;)
		moteur(barriereCourante);
}
