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

//int ** costUpdate ( int iterationNb, int ** state, int **tabSol){
//
//    int ** tabCost = new int * [taille_m];
//    for (int i=0; i< taille_m; i++)
//        tabCost[i]=new int [taille_n];
//
//    if( iterationNb==0){
//        for(int i=0 ; i<taille ; i++)
//        {
//            C_ij[i]= IloIntArray(env, n);
//            for(int j=0; j<n ; j++) {
//
//                pos= i*m +j +1;
//                if (state[1][pos]==1 )
//                    C_ij[i][j]= tCoutVar[i][j];
//                else if (state[0][pos]==0 )
//                    C_ij[i][j]= M*(tCoutVar[i][j] + (tCoutFix[i][j]/tCapacity[i][j])) ;
//                else
//                    C_ij[i][j] =tCoutVar[i][j] + (tCoutFix[i][j]/tCapacity[i][j]);
//            }
//
//        }
//    }
//
//}

//on va construire et resoudre un modèle de transport classique en linéarisant le modèle de base

int ** slotScale(int m, int n, int * tOffre_a, int * tDemand_b, int ** tCoutVar, int ** tCoutFix, int ** tCapacity, int ** state)
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
    cplex.setParam(IloCplex::Param::MIP::Limits::Nodes, 1);
    
    
    //set de l'algorithme à utiliser pour la resolution
    //if (useBenders==1)
    // cplex.setParam(IloCplex::Param::Benders::Strategy, 3);
    
    //
    
    int pos;
    // *** on recopie le tableau tCoutVar en standard iloconcert. on le recupere en transposee
    //la transposée facilitera les calculs entre tableau pour les produits scalaires.
    IloArray<IloIntArray> C_ij (env, n);
    for(int i=0 ; i<m ; i++)
    {
        C_ij[i]= IloIntArray(env, n);
        for(int j=0; j<n ; j++) {
            
            pos= i*m +j +1;
            if (state[1][pos]==1 )
                C_ij[i][j]= tCoutVar[i][j];
            else if (state[0][pos]==0 )
                C_ij[i][j]= M*(tCoutVar[i][j] + (tCoutFix[i][j]/tCapacity[i][j])) ;
            else
                C_ij[i][j] =tCoutVar[i][j] + (tCoutFix[i][j]/tCapacity[i][j]);
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
    
    
    
    int ** tSolution = new int * [m] ; //on va créer un nouveau tableau pour stocker les val des variables au fil des itérations. on va utiliser ca pour mettre à jour  les couts pour la prochaine linéarisation
    for (int i=0; i<m ; i++){
        tSolution[i]= new int [n];
    }
     
    cout << endl<< "x= "<<endl;
    for (int i=0; i< m; i++){
        cout << endl;
        for (int j=0; j<n; j++){
            //cout << cplex.getValue(x[i][j]) << "  " ;
            tSolution[i][j]=int(cplex.getValue(x[i][j])) ;
             //cout << tSolution[i][j] << "  " ;
        }}
    
    int a=int(cplex.getValue(x[0][3]));
    cout<<"a a pour jkak"<<a<< endl;
    
    cout<<"ici"<< endl;
    
    for (int k=0; k<nbIterations; k++){
    
        for(int i=0; i<m; i++){
            for(int j=0; j<n ; j++) {
              
                if (tSolution[i][j]==0 )
                    C_ij[i][j]= M*(tCoutVar[i][j] + (tCoutFix[i][j]/tCapacity[i][j])) ;
                else
                    C_ij[i][j] =tCoutVar[i][j] + (tCoutFix[i][j]/tSolution[i][j]);
            }
        }
        
        mod.remove(objectif);
         IloExpr obj2(env);
        for (int i= 0; i<m ; i++){
            for(int j=0; j<n; j++)
                obj += x[i][j]* C_ij[i][j] ;
        }
        
        IloObjective objectif2 (env, obj2, IloObjective::Minimize, "OBJ");
        mod.add(objectif2);
        
//        IloExpr obj(env);
//        for (int i= 0; i<m ; i++)
//        {
//            for(int j=0; j<n; j++)
//                obj += x[i][j]* C_ij[i][j] ;
//        }
//
//        IloObjective objectif (env, obj, IloObjective::Minimize, "OBJ");
//        mod.add(objectif);
        
        
        cout<<"bon"<< endl;
        cplex.solve();
        cout<<"nbloq"<< endl;
        cout << endl<< "x= "<<endl;
        for (int i=0; i< m; i++){
            cout << endl;
            for (int j=0; j<n; j++){
                cout << cplex.getValue(x[i][j]) << "  " ;
                tSolution[i][j]=cplex.getValue(x[i][j]) ;
                
            }}
        
        
    }
    
    
    // ------------------ AFFICHAGE ET OPTIMISATION ----------------
    
    // export du PL créé dans un fichier .lp
    //cplex.exportModel("/Users/julieamanda/Documents/WorkspaceMem/test.lp");
    // résolution
    //cplex.solve();
    // export de la solution dans un fichier texte
    //cplex.writeSolution("/Users/julieamanda/Documents/WorkspaceMem/sol.txt");
    // récupère la solution et l'affiche à l'écran
    cout << endl <<" valeur de l'objectif = " << cplex.getObjValue() << endl;
    // cout << "algo utilisé"<< cplex.getAlgorithm() <<endl;
    
    /*    cout << endl<< "x= "<<endl;
     for (int i=0; i< m; i++){
     cout << endl;
     for (int j=0; j<n; j++){
     cout << cplex.getValue(x[i][j]) << "  " ;
     
     }}
     
     cout << endl<< "y = "<<endl;
     for (int i=0; i< m; i++){
     cout << endl;
     for (int j=0; j<n; j++){
     cout << cplex.getValue(y[i][j]) << "  ";
     
     }}
     */
    
    
    
    env.end();
    
    
    return  tSolution ;
    
}





