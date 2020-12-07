//
//  samples.hpp
//  Projet_v2
//
//  Created by JULIE AMANDA TCHOUANDEM KEMOE on 2020-09-28.
//  Copyright © 2020 JULIE AMANDA TCHOUANDEM KEMOE. All rights reserved.
//

// nous definissons ici les methodes neccessaires pour lire les données à partir des fichiers et former le modèle.



#ifndef samples_hpp
#define samples_hpp

#include <stdio.h>


#include <stdio.h>
#include <string>

using namespace std;

//on va declarer les données et structures dans lesquelles on va stocker les données extraites du fichier


int uploadData( string line, int pos, int &posSuiv);
// on va lire les données de la ligne line à partir de la position pos

void get_M_N(int &m, int &n); // on va extraire les valeurs de m et n des fichiers

void readInput_Dataset2(  int * tOffre_a, int * tDemand_b, int ** tCoutVar, int ** tCoutFix );
//cette fonction permet de recuperer les donnees d'un fichier appartenant au dataset2
//et de les stocker dans des structures de données pour les utiliser

void readInput_Dataset3( int * tOffre_a, int * tDemand_b, int ** tCoutVar, int ** tCoutFix );
//cette fonction permet de recuperer les donnees d'un fichier appartenant au dataset #
//et de les stocker dans des structures de données pour les utiliser

// on va faire une fonction qui construit la table des capacités pour chaque arc
void BuiltCapacity(int m, int n , int tOffre[], int tDemand[], int ** tCapacity);




#endif /* samples_hpp */
