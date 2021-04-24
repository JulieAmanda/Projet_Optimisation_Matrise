//
//  projectParameters.h
//  Projet_v2
//
//  Created by JULIE AMANDA TCHOUANDEM KEMOE on 2020-09-16.
//  Copyright © 2020 JULIE AMANDA TCHOUANDEM KEMOE. All rights reserved.
//


//dans ce fichier, on définit tous les paramètres neccessaires pour extraire le modèle et les données( nom du fichier ayant les data, numro du groupe auquel appartient le fichier,  et les passer au solveur cplex

#ifndef projectParameters_h
#define projectParameters_h
#include <string>

using namespace std ;


//on déclare le fichier à partir du quel on va extraire les echantillons
//const string monFichier = "/Users/julieamanda/Documents/WorkspaceMem/Data Set 3/Table 5 - B20 Theta0.0/50x50/1.dat" ; //E6
//const string monFichier = "/Users/julieamanda/Documents/WorkspaceMem/Data Set 3/Table 5 - B20 Theta0.0/30x30/2.dat" ; //E7


const string monFichier = "/Users/julieamanda/Documents/WorkspaceMem/Data Set 3/Table 9 - B50 Theta0.2/30x30/1.dat" ; //E8


//const string monFichier ="/Users/julieamanda/Documents/WorkspaceMem/Data Set 2/Table 2 - Theta0.0/1.txt";

//THETA = 0.2  B=50
//const string monFichier = "/Users/julieamanda/Documents/WorkspaceMem/Data Set 3/Table 9 - B50 Theta0.2/30x30/2.dat" ; //don't use --12534.5

//const string monFichier ="/Users/julieamanda/Documents/WorkspaceMem/Data Set 2/toy1.txt";
//const string monFichier = "/Users/julieamanda/Documents/WorkspaceMem/Data Set 2/Table 3 - Theta0.2/2.txt";
//const string monFichier ="/Users/julieamanda/Documents/WorkspaceMem/Data Set 2/Table 4 - Theta0.5/3.txt";



// on a deux groupes(dossiers) d echantillons et le formatage des données dans les fichiers est different dans chacun des deux dossiers, il est donc important de preciser le numero du dossier ( 2<-> data set 2 ou 3 <-> data set 3) pour que le programme appelle la bonne methode de lecture de données

// on veut preciser le groupe du dossier ( 2 ou 3) pour mieux gerer la lecture des données dans les fichiers
const int datasetGroup = 3;

//le param nu pour la methode de sous-gradient
const float nu= 0.5;

//on va utiliser une constante qui va definir si on ajoute(1) ou pas (0 <=> on va travailler juste sur le modèle de base) des IV au modèle
const int IV_suppl = 1 ;

//on va determiner si on utilise benders ou pas coe methode de resolution // 0 si on utilise pas Benders 1 si Oui
const int useBenders = 0;




#endif /* projectParameters_h */

