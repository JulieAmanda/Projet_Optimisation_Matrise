//
//  projectParameters.h
//  Projet_v2
//
//  Created by JULIE AMANDA TCHOUANDEM KEMOE on 2020-09-16.
//  Copyright © 2020 JULIE AMANDA TCHOUANDEM KEMOE. All rights reserved.
//

#ifndef projectParameters_h
#define projectParameters_h
#include <string>

using namespace std;


//on déclare le fichier à partir du quel on va extraire les echantillons
//const string monFichier = "/Users/julieamanda/Documents/WorkspaceMem/Data Set 3/Table 5 - B20 Theta0.0/50x50/1.dat" ;

//const string monFichier ="/Users/julieamanda/Documents/WorkspaceMem/Data Set 2/Table 2 - Theta0.0/1.txt";

//THETA = 0.2  B=50
const string monFichier = "/Users/julieamanda/Documents/WorkspaceMem/Data Set 3/Table 9 - B50 Theta0.2/30x30/2.dat" ;

//const string monFichier ="/Users/julieamanda/Documents/WorkspaceMem/Data Set 2/Table 2 - Theta0.0/2.txt";


// on a deux graoupes d echantillons et la disposition des fichiers est differente dans chacun
// on veut preciser le groupe pour mieux gerer la lecture des données dans les fichiers
const int datasetGroup = 3;

//le param nu pour la décomposition lagrangienne
const float nu= 0.5;

//on va utiliser une constante qui va definir si on ajoute(1) ou pas (0) des IV au modèle
const int IV_suppl = 1;

//on va determiner si on utilise benders ou pas // 0 si on utilise pas Benders 1 si Oui
//const int useBenders = 1;

//#include <stdio.h>

#endif /* projectParameters_h */

