/*************************************************************************
                           ManagerSortie  -  description
                             -------------------
    début                : 19 févr. 2014
    copyright            : (C) 2014 par ymoreno
*************************************************************************/

//---------- Interface de la tâche <ManagerSortie> (fichier ManagerSortie.h) -------
#if ! defined ( MANAGERSORTIE_H_ )
#define MANAGERSORTIE_H_

//------------------------------------------------------------------------
// Rôle de la tâche <ManagerSortie>
// La tache ManagerSortie permet de gérer les demandes de sorties envoyées
// via le clavier, elle crée des tâches filles (videur) qui s'occupent de
// la sortie
//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////  INCLUDE
//--------------------------------------------------- Interfaces utilisées

//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques

void ManagerSortie();

#endif // MANAGERSORTIE_H_

