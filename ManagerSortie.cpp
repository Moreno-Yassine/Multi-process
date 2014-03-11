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
// Destruction successive de l'ensemble des videurs actuellement en execution
{
	if (noSignal==SIGUSR2)
	{
		for ( map<pid_t,int>::iterator itListePid = pidVideurs.begin() ; itListePid != pidVideurs.end(); itListePid++ )
		{
            kill(itListePid->first,SIGUSR2);
            Afficher(MESSAGE,itListePid->first);
            waitpid(itListePid->first,NULL,0);
		}
        destruction();
	}
}

static void mortVideur( int noSignal )
// Mort videur récupère d'abord la place correspondant au videur,
// puis affiche la sortie, et traite les priorité en cas de parking plein
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

            Voiture voitureCourante = ptPlaces[noPlace];

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
                semop(semId,&mutexPlacesVerif,1);


				vector<Requete> listeRq;
				listeRq.push_back(ptRequetes[0]);
				listeRq.push_back(ptRequetes[1]);
				listeRq.push_back(ptRequetes[2]);				

				mutexPlacesVerif.sem_op =1;
				semop(semId,&mutexPlacesVerif,1);

				//Classement des usagers par priorité
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
// l'initialisation récupère les canaux de communication (en lecture)
// et arme les signaux
// SIGUSR2 pour la destruction, et SIGCHLD pour la mort d'un videur
{
	//Recuperation COMM
    semId = semget(CLEFSEM,7,DROITS);
    requestAreaId = shmget(CLEFREQUEST,sizeof(Requete[3]),LECTURE);
    parkingPlaceId = shmget(CLEFPLACES,sizeof(Voiture[8]),LECTURE);
    ptPlaces = (Voiture*)shmat(parkingPlaceId,NULL,LECTURE);
    ptRequetes = (Requete*)shmat(requestAreaId,NULL,LECTURE);
    idMessagerie = msgget(CLEFMESSAGERIESORTIE,DROITS);

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
// Le moteur récupère la voiture à sortir dans la boîte aux lettres,
// on vérifie si la place est occupée, si la place l'est bien, on appelle
// un videur, on efface la place sur l'ecran, puis on attend
{
	long type = 0;
	unsigned int taille = sizeof(int);
	unsigned int place;
    bool placeVide = false;

	int msg = msgrcv (idMessagerie,&place,taille,type,0);

	pid_t videur;
    if (msg!= -1)
	{
        //Extraction place ------
        sembuf bufferMutexPlaces;
        bufferMutexPlaces.sem_num = MUTEX_PLACES;
        bufferMutexPlaces.sem_op = 1;
        bufferMutexPlaces.sem_flg = 0;
        semop(semId,&bufferMutexPlaces,1);

        placeVide = (ptPlaces[place].conducteur==AUCUN);

        bufferMutexPlaces.sem_op = -1;
        semop(semId,&bufferMutexPlaces,1);
        //----Fin extraction

        if(!placeVide)
        {
            videur = SortirVoiture (place);


            if(videur!=-1)
            {
                Effacer((TypeZone)place);
                pidVideurs.insert(make_pair(videur,place));
            }
        } else {
            Afficher(MESSAGE,"Vous avez du vous tromper de place...");
        }
    }
}
static void destruction ()
// la destruction relâche les mémoires partagées
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
