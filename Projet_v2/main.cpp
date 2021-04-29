//
//  main.cpp
//  Projet_v2
//
//  Created by JULIE AMANDA TCHOUANDEM KEMOE on 2020-09-16.
//  Copyright © 2020 JULIE AMANDA TCHOUANDEM KEMOE. All rights reserved.
//

#include <iostream>
#include <ctime>
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
    
    //on cree une variable pour calculer le temps d'execution total
    float temp =0;
    time_t before = time(0); //le temps systeme actuel en seconde
    
    /* cas 1 : l'on veut laisser le modèle globale à cplex pour qu'il applique ses propres algo. le code suivant doit être laissé en commentaire si vous ne souhaitez pas tester ce cas. Si vous testez ce cas mettez le code entre le debut et la fin du cas 2 en commentaire */

//    int timelimit = 89;
//    float resCplex= globalModel(m, n, tOffre, tDemand, tCoutVar, tCoutFix, tCapacity, timelimit);
//
//    cout << " le resultat trouvé par cplex dans la période de temps donnée est " << resCplex << endl;

    
    /* fin de ce cas si vous voulez tester ce cas enlever le en commentaire si non laisser le code entre ces deux commentaires sous forme de commentaire et tester le cas suivant. */
    
    
    
    
    
    /* cas 2 : nous implémentons notre méthode de sous gradient avec heuristique lagrangienne, slope scaling et postOptimisation */
    


    //construction d'un tableau de distribution aléatoire qui nous servira pour notre heuristique de borne sup dans l'algo de sous-gradient
   // int a=n*m;
    int * tRandom  = DistribAleatoire(n, m);


    //paramètre pour le slope scaling
    const int countSlope = omga_4/callHrstq; //nombre max d'appel de slope Scaling(ou d'heuristique lagrangienne) possible

    //on va ensuite utiliser cette variable pour compter le nombre réel de fois que l'heuristique lagrangienne a déjà été appelé.
    int nbCallHrstq = 0;

    //nous construisons la table qui nous permettra de sauvegarder les valeurs x_ij et y_ij de la meilleure solution obtenue tout au long de l'algorithme.

    int ** tabBestSol = new int*[m];
    for (int i=0; i<m; ++i)
        tabBestSol[i]= new int[n];
    int ** tabBestSol_Y = new int*[m];
    for (int i=0; i<m; ++i)
        tabBestSol_Y[i]= new int[n];

    //on va stocquer ici la valeur de la meilleure solution obtenue tout au long de l'algo de sous-gradient
    float bestSolValue =0 ;//on l'initialise par une valeur infinie


    //on va à présent créer une table qui nous permettra d'enregistrer tout l'historique des valeurs des y_ij au cours des recherches de la meilleures borne supérieure. Pour chaque solution réalisable correspondant à une borneSup, on va enregistrer l'etat de chaque arc en sauvegardant les valeurs des y_ij. Cela nous permettra à la fin de constater les arcs qui sont toujours fermés ds ttes les solutions
    int nb=n*m;
    int ** historiqY_ij = new int * [countSlope];
    for (int i=0; i<countSlope; ++i)
        historiqY_ij[i]= new int [nb];


    //application de l'algorithme du sous gradient pour determiner la solution (voir le fichier subgradientMethod.cpp
   Subgradient(m, n, tOffre, tDemand, tCoutVar, tCoutFix, tCapacity, tRandom,  tabBestSol, tabBestSol_Y, bestSolValue, historiqY_ij, nbCallHrstq);

    cout << "le nombre de solution calculée ( nbre de fois que l'heuristique a été appelée) est : " << nbCallHrstq << endl;
    float valsol = 0;

    for (int i=0; i<m ;++i){
        for (int j=0; j<n; j++)
            valsol += tCoutFix[i][j]*tabBestSol_Y[i][j] + tCoutVar[i][j]*tabBestSol[i][j];
            }

   cout << "la valeur de la meilleure solution évaluée dans la fonction objectif est : " << valsol << endl ;


    float finalSol = lastPostOptim(m, n,  tOffre, tDemand, tCoutVar, tCoutFix, tCapacity, bestSolValue, tabBestSol, tabBestSol_Y, historiqY_ij, nbCallHrstq);



    cout<<endl;
    cout << "la solution après amélioration et la meilleure est :  "<< finalSol << endl;
//

    for (int i=0; i<m; i++)
    {
        delete [] tabBestSol[i];
    }



    for (int i=0; i<countSlope; i++){
        delete [] historiqY_ij[i];
    }
    delete [] historiqY_ij;
    delete [] tabBestSol;


    
    /* fin du code pour le cas 2*/
    
    
    
   
    time_t after = time(0);
    temp=after-before;
    
    
     cout << "Le temps total d'execution en secondes est " << temp << endl;
    cout << " Temps d'execution en minutes : " << temp/60 << " et en heures : " << temp /3600 << endl;
    
    //  ***  on libere les pointeurs qui ont ete utiliser pour stocker les données
        for (int i=0; i<m; i++)
    {
        delete [] tCoutFix[i];
        delete [] tCoutVar[i];
        delete [] tCapacity[i];
      
    }
    
    delete [] tOffre;
    delete [] tDemand;
    delete [] tCoutFix;
    delete [] tCoutVar;
    delete [] tCapacity;
    
   
    return 0;
}
