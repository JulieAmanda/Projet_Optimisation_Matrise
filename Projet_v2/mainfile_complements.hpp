//
//  mainfile_complements.hpp
//  Projet_v2
//
//  Created by JULIE AMANDA TCHOUANDEM KEMOE on 2020-09-27.
//  Copyright © 2020 JULIE AMANDA TCHOUANDEM KEMOE. All rights reserved.
//

#ifndef mainfile_complements_hpp
#define mainfile_complements_hpp

#include <stdio.h>


//on va définir une fonction qui va nous permettre d'améliorer l'affichage au niveau de la solution des sous pbs
// pour chaque sous problème correspondant à un noeud, on va afficher toutes les data relatives
void affich_subPbData (int nodeValue, int * t1, int * t2, int *t3, int m, int n);

//nodeValue: valeur de l'offre ou demande associé à ce noeud précis
// t1 ; tab des capacités maximales des flots quittant/arrivant au noeud
//t2 : tab des coutVar associés aux différents flots à partor de ce noeud
//t3 : tab des couts fixes ...
// m , n : taille associé à un tableau/ nombre de sources(ou destinations)


#endif /* mainfile_complements_hpp */

//on fait une fonction pour separer les paragraphes du resultat et améliorer la lisibilité de l'affichage
void makeSpace();
