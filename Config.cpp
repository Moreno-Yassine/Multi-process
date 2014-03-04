/*************************************************************************
                           Config  -  description
                             -------------------
    début                : 27 févr. 2014
    copyright            : (C) 2014 par ymoreno
*************************************************************************/

//---------- Réalisation du module <Config> (fichier Config.cpp.cpp) -----

/////////////////////////////////////////////////////////////////  INCLUDE
//-------------------------------------------------------- Include système

//------------------------------------------------------ Include personnel
#include "Config.h"

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
TypeBarriere IntToPorte (int porte) {
	TypeBarriere barriere = AUCUNE;
	switch(porte)
	{
	case(0):
			barriere = AUCUNE;
			break;
	case(1):
			barriere = PROF_BLAISE_PASCAL;
			break;
	case(2):
			barriere = AUTRE_BLAISE_PASCAL;
			break;
	case(3):
			barriere = ENTREE_GASTON_BERGER;
			break;
	case(4):
			barriere = SORTIE_GASTON_BERGER;
			break;
	}
	return barriere;
}

bool TrierRequetes ( Requete a, Requete b )
{
	if(a.usager==0)
		return false;
	if(b.usager==0)
		return true;

	if(a.usager==b.usager)
	{
		return (a.daterequete<b.daterequete);
	} else {
		return (a.usager==PROF);
	}
}
