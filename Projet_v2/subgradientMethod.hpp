//
//  subgradientMethod.hpp
//  Projet_v2
//
//  Created by JULIE AMANDA TCHOUANDEM KEMOE on 2020-09-29.
//  Copyright © 2020 JULIE AMANDA TCHOUANDEM KEMOE. All rights reserved.
//

#ifndef subgradientMethod_hpp
#define subgradientMethod_hpp

#include "IterationDetails.hpp"

#include <stdio.h>
#include <iostream>


using namespace std; 
//paramètres à utiliser pour la méthode de sous-gradients

//ELEMENTS POUR l'heuristique de mise à jour de la borne sup

//int * tRandomVector (
//int * ModelBSup( int ** tMultiplicateurs, int ** tXij, )


const int omga_2= 70;  //si la valeur de l'objectif ne change pas après ce nombre d'iteration on met à jour la valeur de lambda ds la formule du pas
const int omga_3 = 100; // si la valeur de l'objectif ne change pas après ce nombre d'iteration on stop l'algo du sous -gradient
const int omga_4 = 1000; //nombre d'iteration maximal à ne pas dépasser
const int omga_1 = 2;   // les oméga st des paramètres utilisés aucours du contrôl des itérations

const int callHrstq = 20 ; // nombre d'itérations après lequel on fait appel à l'heuristiq



void affichTabSolutions ( int m, int n , int ** tab);
//pour afficher une table de données sur la ligne de commandes


// foction qui nous permettra de verifier le changement de la borne inférieure aucours de nb itérations successives.
bool lowerBndVariation(int nbIterationsMax, bool & updateStatus,  float & optimal_lwrBnd , int & count);
//nbItérations c'est le nombre d'itérations max pour lequel on surveille s'il y a un changement
// update statut nous permet de savoir s'il y a eu changement à date ou pas
// previouslowerBnd est la valeur de la borne inf à la précedente itération
//lowerBound c'est la borne inf du pb global à l'iteration t
// count permet de savoir à combien de valeurs consécutives égales l'on est rendu


IterationDetails Subgradient( int m, int n, int * tOffre_a, int * tDemand_b, int ** tCoutVar, int ** tCoutFix, int ** tCapacity,  int * tabDistAleatr);
//tabSgrad_t_1 : table of the subgradients at the previous iteration
//iteration : the current iteration



#endif /* subgradientMethod_hpp */
