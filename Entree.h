/*************************************************************************
                           Entree  -  description
                             -------------------
    début                : 27 févr. 2014
    copyright            : (C) 2014 par ymoreno
*************************************************************************/

//---------- Interface de la tâche <Entree> (fichier Entree.h) -------
#if ! defined ( ENTREE_H_ )
#define ENTREE_H_

//------------------------------------------------------------------------
// Rôle de la tâche <Entree>
//	La tâche entrée gère une des porte du parking, elle prend en charge
// un client depuis son arrivée devant la barrière, jusqu'à la fin du
// stationnement de la voiture
//
//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////  INCLUDE
//--------------------------------------------------- Interfaces utilisées

//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
void Entree(int numPorte);
// Mode d'emploi :
//	Procédure principale
// Contrat :
//	NumPorte doit être compris entre 1 et 3

#endif // ENTREE_H_

