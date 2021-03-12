//
//  SlotScaling.cpp
//  Projet_v2
//
//  Created by JULIE AMANDA TCHOUANDEM KEMOE on 2020-12-05.
//  Copyright © 2020 JULIE AMANDA TCHOUANDEM KEMOE. All rights reserved.
//

#include "SlotScaling.hpp"
#include "projectParameters.h"
#include "heuristiqBSup.hpp"
//#include "samples.hpp"

#include <ilcplex/ilocplex.h>
#include <string.h>
ILOSTLBEGIN

//cette procédure est appelé lors de la résolution du pb de transport aucours des itérations du slot scaling pour définir les couts et construire l'objectif
//on suppose que lorsqu'on appelle cette fonction, la table State[1] contient les indications sur les arcs qui doivent etre fermés ou ouverts ou laisser libres ( apres l'itération 0, cette fonction est appelée après avoir fait un stateUpdate qui met à jour la table state selon les resultats obtenus à la précédente itération du slot scaling)

void costUpdate ( int m, int n, int iterationNb, int ** state, int **tabSol,  int ** tCoutVar, int ** tCoutFix, int ** tCapacity, int ** tabCost ){
    
//    int ** tabCost = new int * [m];
//    for (int i=0; i< m; i++)
//        tabCost[i]=new int [n];
    int pos;
    
    if( iterationNb==0){
        for(int i=0 ; i<m ; i++)
        {
            for(int j=0; j<n ; j++) {
                
                pos= i*m +j ;
                if (state[1][pos]==1 )
                    tabCost[i][j]= tCoutVar[i][j];
                else if (state[0][pos]==0 )
                    tabCost[i][j]= M*(tCoutVar[i][j] + (tCoutFix[i][j]/tCapacity[i][j])) ;
                else
                    tabCost[i][j] =tCoutVar[i][j] + (tCoutFix[i][j]/tCapacity[i][j]);
            }
            
        }
    }
    else{//je ne me préocupe pas du fait qu'on ai voulu fermé ou pas car à l'itération précedente il a déja choisi ce qu'il va fermer
        for(int i=0; i<m; i++){
            for(int j=0; j<n ; j++) {
                if (tabSol[i][j]>0 )
                    tabCost[i][j] =tCoutVar[i][j] + (tCoutFix[i][j]/tabSol[i][j]);
                
//                if (tabSol[i][j]==0 )
//                    tabCost[i][j] = M*(tCoutVar[i][j] + (tCoutFix[i][j]/tCapacity[i][j])) ;
//                else
//                    tabCost[i][j] =tCoutVar[i][j] + (tCoutFix[i][j]/tabSol[i][j]);
            }
        }
    }
    
   
}


//on va construire et resoudre un modèle de transport classique en linéarisant le modèle de base
//la fonction retourne la valeur de l'objectif à chaque appel

int flotProblem(int m, int n, int * tOffre_a, int * tDemand_b, int ** tCapacity,  int ** tSolution, int ** tabCost, int postOptim )
{
    IloEnv env;
    IloModel mod(env); //model
    IloCplex cplex(mod); // instance du solveur à utiliser.
    
    //limitons le nombre de processeurs
    cplex.setParam(IloCplex::Param::Threads, 1);
    
    //temps cpu max = 1h30 (en secondes)
    //cplex.setParam(IloCplex::TiLim, 9000);
    
    //preciser à cplex d'utiliser le network optimizer
    cplex.setParam(IloCplex::Param::RootAlgorithm , CPX_ALG_NET);

    
    //dire à cplex de s'arreter à la racine
   // cplex.setParam(IloCplex::Param::MIP::Limits::Nodes, 1);
    
    
    //set de l'algorithme à utiliser pour la resolution
    //if (useBenders==1)
    // cplex.setParam(IloCplex::Param::Benders::Strategy, 3);
    
   
 // costUpdate(m, n, iterationNb, state, tSolution, tCoutVar, tCoutFix, tCapacity, tabCost);
    
    
    // *** on recopie le tableau tCoutVar en standard iloconcert. on le recupere en transposee
    //la transposée facilitera les calculs entre tableau pour les produits scalaires.
    IloArray<IloIntArray> C_ij (env, m);
    for(int i=0 ; i<m ; i++){
        C_ij[i]= IloIntArray(env, n);
        for(int j=0; j<n ; j++) {
            C_ij[i][j]= tabCost[i][j];
        }

    }
    
        
    
    // *** definissons les variables
    
    IloArray<IloNumVarArray> x (env, m);//m ici est le nombre de sinks voir sample.h
    //chaque x[i] est un tableau de m variables reelles
    for(int i=0 ; i<m ; i++)
        x[i] = IloNumVarArray(env, n, 0.0, IloInfinity, ILOINT);
    
    
    // ***construisons l'expression de la fonction objetif
    
    IloExpr obj(env);
    for (int i= 0; i<m ; i++)
    {
        for(int j=0; j<n; j++)
            obj += x[i][j]* C_ij[i][j] ;
    }
    
    IloObjective objectif (env, obj, IloObjective::Minimize, "OBJ");
    mod.add(objectif);
    
    
    // *** definissons à présent les contraintes
    
    //si c'est une postOptimisation on va fermer certains arcs
    if (postOptim==1){
        for( int i=0; i<m; i++){
            for ( int j=0; j<n; j++){
                if (tSolution[i][j]==0){
                    mod.add(x[i][j]==0);
                }
            }
        }
    }
    
    //( sum(x_ij , i in M)= offre_i
    //pour chacun des m sous-tableaux/cases du tableau principal iloarray, on va prendre lélement j parmi n
    for (int j = 0; j< n; j++)
    {
        IloExpr ctr(env);
        for (int i=0; i<m ; i++)
            ctr = ctr + x[i][j];
        mod.add(ctr == tDemand_b[j] );
    }
    
    //sum(x_ij , j in N)=demand_j
    for (int i = 0; i< m; i++)
        mod.add(IloSum(x[i])==tOffre_a[i] );
    
    //x_ij < u_ij
    for(int i=0; i<m; i++)
    {
        for (int j =0; j<n ; j++)
            mod.add(x[i][j] <= tCapacity[i][j]);
    }
    cplex.solve();
//
//    cout << endl<< "tSolution= "<<endl;
//    for (int i=0; i< m; i++){
//        cout << endl;
//        for (int j=0; j<n; j++){
//            cout << tSolution[i][j] << "  " ;
//        }}
  
     
    cout << endl<< "x= "<<endl;
    for (int i=0; i< m; i++){
        cout << endl;
        for (int j=0; j<n; j++){
            cout << cplex.getValue(x[i][j]) << "  " ;
            tSolution[i][j]=int(cplex.getValue(x[i][j])) ;
             //cout << tSolution[i][j] << "  " ;
        }}
    

    cout << endl <<" valeur de l'objectif s_scaling = " << cplex.getObjValue() << endl;
    // cout << "algo utilisé"<< cplex.getAlgorithm() <<endl;
    
     int res =int(cplex.getObjValue());
    
    env.end();
//
//
   return (res);
//
    
}


//cette méthode est pour récupérer la meilleur solution obtenue par une itération de la procédure slopeScaling - la fixation sera ensuite faite sur la base de cette meilleure solution
//BORNSUP ICI EST LA MEILLEUREBORNE Sup courante
//bestsol est la table qui à chaque itération va garder les valeur de la meilleure slution du SS
//bestVal est la valeur de l'objectif obtenu en évaluant bestsol par rapprt au problème maitre, de be
float slopeRealValue(int m, int n, int ** tabSol, int ** tCoutFix, int ** tCoutVar){
    
    //
     float ValSol=0;
    
    int ** tY = new int * [m];
    for (int i=0; i< m; i ++){
        tY[i]= new int [n];
    }
    for (int i=0; i< m; i ++){
        for (int j=0; j<n; j++){
            if(tabSol[i][j]==0)
                tY[i][j] =0;
            else //if >0
                tY[i][j]=1;
        }
    }
    
    
    //on calcule la valeur de l'bjectif du problème maitre correspondant à la meilleure solution du slopeS
    for (int i=0; i<m; i++){
        for (int j=0; j<n; j++){
            ValSol += tabSol[i][j] * tCoutVar[i][j] + tY[i][j] * tCoutFix[i][j];
        }
    }
    
    
    //si on trouve que cette sol donne une meilleure borne inférieure on va mettre à jour la table contenant la meilleure borne inférieure
    //bestsol est la table contenant les valeurs des flots de la meilleure solution courante parmis les itérations du SS qui ont déjà été faites.
    //bestval est la meilleure soltions du slope scale parmi toutes les itérations.
    
    //pour le moment on va enlever ça mais le but est de faire cette coparaison quelque part pour recuperer la meilleur des solutions du slot scale
//    if (ValSol< bestVal){
//        bestVal=ValSol;
//        for (int i=0; i<m; i++){
//            for (int j=0; j<n; j++){
//                bestSol[i][j]= tabSol[i][j];
//            }
//        }
//    }
//
    
    return ValSol;
}



void costPostOptim(int m, int n, int **tabSol,  int ** tCoutVar, int ** tabCost){
    
    for (int i=0; i< m; i++){
        for (int j=0; j<n; j++){
            if (tabSol[i][j]==0)
                tabCost[i][j] = Mpost; //on fixe un cout tres eleve de sorte à empecher le flot à passer par cet arc
            else
                tabCost[i][j]=tCoutVar[i][j];
        }
    }
    
 
    
}


void slopeScaleMethod( int m, int n, int ** tabSolScaling, float & bornSup,  int ** tCoutFix, int ** tCoutVar, int ** tCapacity, int * tOffre, int * tDemand, int ** state ){
    
    float valSolSlope=1000 ;//valeur de la solution à l'itération courante
    int valPrec=5 ;
    int valCour= 10;//valeur de la solution à l'itération précédente.
    bool next = true ; // on va utiliser ca pour controller l'arrêt de la boucle des itérations de slope scaling lorsque l'on trouve deux solutions successives égales
    
    //pour la fonction flaotProblem
    int postOptim=0;
    
   //ici on va stocker l'information sur la meilleure solution trouver aucours d'une itération de slot scaling
    float bestVal=100000;
    int ** bestSol = new int * [m];
    for (int i=0; i<m;i++)
        bestSol[i]= new int [n];
    
    //on cree une table pour receuillir les couts linéaires au fil des iterations du slot scaling
    int ** tabCost = new int * [m];
    for (int i=0; i< m; i++){
        tabCost[i]=new int [n];
        for(int j=0; j<n ; j++)
            tabCost[i][j]= 0;
    }
    
    // on va recueillir ici les solutions de la procédure slope scaling
//    int ** tabSolScaling = new int * [m];
//    for (int i=0; i<m ;i++){
//        tabSolScaling[i]=new int [n];
//        for (int j=0; j<n; j++)
//            tabSolScaling[i][j]=0;
//    }
    
    int k=0;//pour compter les itérations au fil des résolutions du problè de flot
    
    // SS, SS1, SS2, SS3, SS4 et SS5 sont des scénarios différents d'implémentation du Slope Scaling
    //pour en savoir plus sur les détails d'un scénarios veuillez lire la description dans la partie résultats expérimentaux du mémoire.
    
    
    /*******    SS0  *******/
    
//    while (k<10) {
//
//        // faire le cost update ici
//        costUpdate(m, n, k, state, tabSolScaling, tCoutVar, tCoutFix, tCapacity, tabCost); //on fait un costupdate pour l'iteration k
//
//        valCour = flotProblem(m, n,  tOffre, tDemand, tCapacity, tabSolScaling, tabCost, postOptim);//iteration 0
//        valSolSlope= slopeRealValue(m, n, tabSolScaling, tCoutFix, tCoutVar);
//
//        k++;
//    }
//    bornSup=valSolSlope;
//    updateTabState(tabSolScaling, state, m, n);
    
    /******  fin SS ********/
    
    
    
    /*******    SS1  *******/
    //on arrete les itérations lorsqu'on trouve successivement deux solutions correspondant à la même solution réelle calculée qui est une borne sup. on travaille avec cette dernière solution
//
//     while (next) {
//
//     valPrec=valSolSlope;
//
//     // faire le cost update ici
//     costUpdate(m, n, k, state, tabSolScaling, tCoutVar, tCoutFix, tCapacity, tabCost); //on fait un costupdate pour l'iteration k
//
//
//     valCour = flotProblem(m, n,  tOffre, tDemand, tCapacity, tabSolScaling, tabCost, postOptim);//iteration 0
//         valSolSlope= slopeRealValue(m, n, tabSolScaling, tCoutFix, tCoutVar);
//     if (valSolSlope == valPrec)
//     next=false;
//
//     k++;
//     }
//    bornSup=valSolSlope;
//     updateTabState(tabSolScaling, state, m, n);

     /******  fin SS1 ********/
    
    
    
    /*******    SS2  *******/
    //on arrete les itérations lorsqu'on trouve deux meme valeurs de l' objectif successivement et on travaille avec la dernière solution

//    while (next) {
//
//        valPrec=valCour;
//
//       // faire le cost update ici
//        costUpdate(m, n, k, state, tabSolScaling, tCoutVar, tCoutFix, tCapacity, tabCost); //on fait un costupdate pour l'iteration k
//
//
//        valCour = flotProblem(m, n,  tOffre, tDemand, tCapacity, tabSolScaling, tabCost, postOptim);//iteration 0
//         valSolSlope= slopeRealValue(m, n, tabSolScaling, tCoutFix, tCoutVar);
//        if (valCour == valPrec)
//                 next=false;
//
//        k++;
//    }
//     bornSup=valSolSlope;
//    updateTabState(tabSolScaling, state, m, n);

  /******  fin SS2 ********/
    
    
    
    
   /* ******    SS4 (ou SS3 ) (voir le fichier subgradMethod.cpp pour la détermination de la methode T1 ou T2) juste après l'appel de la méthode slope scaling  ***** */
    //on fait comme en SS1 mais ensuite on travaille avec la solution permettant d'obtenir la meilleure solution
    
     //on va appliquer une procédure de slopescaling pour relier les sources aux destinations et s'assurer que le nombre d'arcs ouvert et fermés nous permet d'avoir une solution réalisable.
    while (next ){ //(valPrec!=valSolSlope)&&  stop == false



        valPrec=valSolSlope;

        //faire le cost update ici
        costUpdate(m, n, k, state, tabSolScaling, tCoutVar, tCoutFix, tCapacity, tabCost); //on fait un costupdate pour l'iteration k

       valCour = flotProblem(m, n,  tOffre, tDemand, tCapacity, tabSolScaling, tabCost, postOptim);//iteration 0

        valSolSlope= slopeRealValue(m, n, tabSolScaling, tCoutFix, tCoutVar);
        cout << "la valeur de la sol evaluée ds le prob global est : " << valSolSlope <<endl;



        if (valSolSlope == valPrec)
            next=false;


        //chercher la meilleure solution
        if (valSolSlope < bestVal){ //si la solution de l'itération courante est meilleure que celles des itérations precdt
            bestVal=valSolSlope; // on recueille cette valeur coe etant la meilleure soltn du slope Scal
            for (int i=0; i< m; i++){
                for(int j=0; j<n ; j++)
                    bestSol[i][j]= tabSolScaling[i][j]; //on receueille également ds une table les valeurs des variables
            }
        }
 cout << "la valeur reelle meilleure solution   est : " << bestVal <<endl;

        k +=1;
    }

    cout << "le nombre d'itération est " << k << endl;


    cout<<"borne sup évaluée avec la formule slopeRealValue "<< bestVal ;


//   // on va ensuite mettre à jour la fermeture et l'ouverture des arcs en modifiant l'etat des arcs dans le tableau state
    //cette partie n'est pas neccessaire si on ajoute la postOpt car ces lignes seront appelée à la fin de toute la méthode.
//        updateTabState(bestSol, state, m, n);
//        bornSup= bestVal; //on sauvegarde la meilleure borne sup retournée par la procédure



   /*   ****** fin SS4 (ou SS5)    *******    */



    /* *****     Post-opt pour SS5  ***** */
//
        //postoptimiser la meilleure solution de flot( bestsol) avec les couts reels
        costPostOptim(m, n, bestSol, tCoutVar, tabCost); //on va lui donner la table des meilleurs sol pour compute les couts qu'on va utiliser pour la postoptimisation
    
        postOptim=1;
    
       valCour = flotProblem(m, n, tOffre, tDemand, tCapacity, bestSol, tabCost, postOptim); //on execute à nouveau le pb de flot en postoptimisation avec les couts cij les resultats des variables on les garde dans tabsolscaling
        bestVal = slopeRealValue(m, n, bestSol, tCoutFix, tCoutVar);// c'est tabsolscaling qui contient deja les valeurs de la meilleur solution postoptimisée. donc on va calculer la meilleure valeur utilisant les solutions qui sont ds tabsolScaling
        cout<< "la valeur de la sol après post optim est :" << bestVal;


    //on va ensuite mettre à jour la fermeture et l'ouverture des arcs en modifiant l'etat des arcs dans le tableau state
    updateTabState(bestSol, state, m, n);
    bornSup= bestVal; //on sauvegarde la meilleure borne sup retournée par la procédure

    
   /* *******    fin postOpt   ***** **/


    
    for (int i=0; i<m; i++){
        delete [] tabCost[i];
        delete [] bestSol[i];
        
    }
    delete [] tabCost;
    delete [] bestSol;
    
    
    
    
    cout << endl;
    
    
}

//float lastOptim(int m, int n, int * tOffre_a, int * tDemand_b, int ** tCoutVar, int ** tCoutFix, int ** tCapacity, bool  ignore, int ** state, float bestBornSup, int ** tabBestSol, int ** historiqY_ij ){
//    
//    
//    
//}


