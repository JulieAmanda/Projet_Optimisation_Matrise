//
//  heuristiqBSup.hpp
//  Projet_v2
//
//  Created by JULIE AMANDA TCHOUANDEM KEMOE on 2020-12-03.
//  Copyright © 2020 JULIE AMANDA TCHOUANDEM KEMOE. All rights reserved.
//

#ifndef heuristiqBSup_hpp
#define heuristiqBSup_hpp

#include <stdio.h>

//table dans laquelle on va sauvegarder l'historique des resultats au fil des itérations


//une fonction qui nous permettra de créer une distribution aléatoire une seule fois aucours de tout le programme ( on va la creer dans le main et l'utiliser partout où besoin) elle servira pour une verification des solutions. elle retourne un tableau de taille n*m
int * DistribAleatoire(int n, int m);


//dans cette fonction, on utilisera notre distribution aléatoire pour calculer une clé unique correspondante à la solution obtenue à une itération donnée ( la solution est identifiee par tSubgrad et tX). si la clé obtenue est identique à une clé présente dans l'historique, on suppose que cette solution a déja été traitée alors on va l'ignorer. aussi, on repertorie dans une table state la solution qui est évaluée.
bool heuristique( int * tabDistAleatr, int ** tabW, int ** tabX, int m, int n , int * historiqSol, int ** state );

//C'est une fonction qui nous permet de mettre à jour l'etat des variables au fur et à mesure des appels du slope scaling
//
void updateTabState( int ** tabSolScaling, int **  state, int m, int n);

#endif /* heuristiqBSup_hpp */


//int * tRandomVector (
//int * ModelBSup( int ** tMultiplicateurs, int ** tXij, )
