//
//  subPb.hpp
//  Projet_v2
//
//  Created by JULIE AMANDA TCHOUANDEM KEMOE on 2020-09-16.
//  Copyright © 2020 JULIE AMANDA TCHOUANDEM KEMOE. All rights reserved.
//

// on va définir ici l'implémentation des sous pbs qui seront utilisés dans l'algorithme du sous -gradient


#ifndef subPb_hpp
#define subPb_hpp


#include <stdio.h>

//ici on va noter x_ijˆs = x_ij
//et x_ijˆd= w_ij

void ssPbSrc_i( int numSrc, float * LgrgeMultpipl_i, float nu, int * ValX_ij, int Offre_i,
               int * val_U_ij, int * val_C_ij, int * val_F_ij, int n, float  & valObj);

/* numsrc : indice du noeud qu'on est entrain de traiter
 alpha, beta, nu : ce sont des valeurs qu'on va utiliser comme paramètre plus tard donc on va les garder
 alpha : multiplicateurs de lagrane
 beta : valeurs des pertubations 
 valX_ij : tableaux de n valeurs correspondant aux flots allant du noeuds i vers chaque noeud j
 Offre_i : c'est l'offre du noeud i
 val_u_ij : valeur des capacités sur les routes allant de i vers chacun des noeuds j
 val_C_ij : valeur des couts fixes pour acheminer du noeud i vers les différents noeud j
 val_F_if valeur des couts variables pour acheminer du noeud i vers les differents noeuds j
 valObj : est la valeur de l'objectif pour le sous problème correspondant à ce noeud là
 */

void ssPbDest_j(int numDest, float * LgrgeMultpipl_j,  float nu, int * ValW_ij, int Demand_j,
                int * val_U_ij, int * val_C_ij, int * val_F_ij, int m ,float & valObj);


//on va définir une fonction qui va nous permettre d'améliorer l'affichage au niveau de la solution des sous pbs
// pour chaque sous problème correspondant à un noeud, on va afficher toutes les data relatives
void affich_subPbData (int nodeValue, int * t1, int * t2, int *t3, int m, int n);

//nodeValue: valeur de l'offre ou demande associé à ce noeud précis
// t1 ; tab des capacités maximales des flots quittant/arrivant au noeud
//t2 : tab des coutVar associés aux différents flots à partor de ce noeud
//t3 : tab des couts fixes ...
// m , n : taille associé à un tableau/ nombre de sources(ou destinations)


//on fait une fonction pour separer les paragraphes du resultat et améliorer la lisibilité de l'affichage
void makeSpace();


#endif /* subPb_hpp */
