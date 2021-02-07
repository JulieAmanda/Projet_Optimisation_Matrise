//
//  main.cpp
//  Projet_v2
//
//  Created by JULIE AMANDA TCHOUANDEM KEMOE on 2020-09-16.
//  Copyright © 2020 JULIE AMANDA TCHOUANDEM KEMOE. All rights reserved.
//

#include <iostream>
#include "IterationDetails.hpp"
#include "samples.hpp"
#include "projectParameters.h"
#include "subPb.hpp"
#include "globalModel.hpp"
#include "subgradientMethod.hpp"
#include "heuristiqBSup.hpp"
#include "SlotScaling.hpp"
#include "globalVars.h"



using namespace std;




int main(int argc, const char * argv[]) {
    
    //***   commençons par construire les structures de données à utiliser à partir des données prises dans les fichiers echantillons  ***//

   //on a déclaré m et n comme variables globales dans globalVars du projet
    
    int m =  0;
    int  n = 0 ;
    get_M_N(m, n); //on va recuperer les valeurs de m et n contenues dans le fichier(voir le fichier samples.cpp)
    
    cout<< m<< endl;
    cout <<n << endl;
   
    //on construit les structures de données dont on aura pour stocker les données
    int * tOffre = new int[m];
    int * tDemand = new int[n]; //on choisit de reserver 100
    
    int** tCoutFix = new int*[m];
    for (int i = 0; i < m; ++i)
        tCoutFix[i] = new int[n];
    
    int** tCoutVar = new int*[m];
    for (int i = 0; i < m; ++i)
        tCoutVar[i] = new int[n];
    
    int** tCapacity = new int*[m];// va contenir la capacité des différentes routes
    for (int i = 0; i < m; ++i)
        tCapacity[i] = new int[n];
    
    // on va lire toutes données dans le fichier selon le groupe auquel appartient le file
    // il existe 2 dossiers. et selon le dossier, la disposition des fichiers est differente
    if (datasetGroup==2)
        readInput_Dataset2( m, n, tOffre, tDemand,tCoutVar, tCoutFix);// voir samples.cpp
    else if (datasetGroup==3)
        readInput_Dataset3(m, n, tOffre, tDemand,tCoutVar, tCoutFix);
    else
        cout << "ce dataset Group est inexistant";
    
    //on va construire le tableau des capacités
    BuiltCapacity(m, n, tOffre, tDemand, tCapacity);//voir samples.cpp
    
    //construction d'un tableau de distribution aléatoire qui nous servira pour notre heuristique de borne sup dans l'algo de sous-gradient
   // int a=n*m;
    int * tRandom  = DistribAleatoire(n, m);
    
    
    //paramètre pour le slope scaling
    const int countSlope = omga_4/callHrstq; //nombre max d'appel de slope Scaling possible
    int * ** tabBestSol= new int ** [countSlope]; // tableau dans lequel on va classer les meilleures solutions de tous les appels de slope scaling
    int  nbItSS=0; // pour compter le nombre d'appel de slope scaling(SS) qui a été fait
    

    //application de l'algorithme du sous gradient pour determiner la solution (voir le fichier subgradientMethod.cpp
   Subgradient(m, n, tOffre, tDemand, tCoutVar, tCoutFix, tCapacity, tRandom,  nbItSS, tabBestSol);
    

    
   
//    cout<<endl;
//    for (int i=0; i<a; i++)
//        cout<< tRandom[i]<< "  ";
//
    
    
    //  ***  on libere les pointeurs qui ont ete utiliser pour stocker les données
        for (int i=0; i<m; i++)
    {
        delete [] tCoutFix[i];
        delete [] tCoutVar[i];
        delete [] tCapacity[i];
    }
    
    delete [] tCoutFix;
    delete [] tCoutVar;
    delete [] tCapacity;
    
    delete [] tOffre;
    delete [] tDemand;
 
    
    return 0;
}
