//
//  globalModel.cpp
//  Projet_v2
//
//  Created by JULIE AMANDA TCHOUANDEM KEMOE on 2020-09-28.
//  Copyright © 2020 JULIE AMANDA TCHOUANDEM KEMOE. All rights reserved.
//

//on va construire le modèle global et imposer comme paramètre au solveur d'arrêter l'optimisation au noeud racine


#include "globalModel.hpp"
#include "projectParameters.h"
//#include "samples.hpp"

#include <ilcplex/ilocplex.h>
#include <string.h>
ILOSTLBEGIN

float ModelBase_Bsup(int m, int n, int * tOffre_a, int * tDemand_b, int ** tCoutVar, int ** tCoutFix, int ** tCapacity)
{
    IloEnv env;
    IloModel mod(env); //model
    IloCplex cplex(mod); // instance du solveur à utiliser.
    
    //limitons le nombre de processeurs utilisés à 1
    cplex.setParam(IloCplex::Param::Threads, 1);
    
    //BLOCKER L'AFFICHAGE DES DÉTails d'optimisation
    cplex.setParam(IloCplex::Param::MIP::Display, 0);
    
    //dire à cplex de s'arreter à la racine
    cplex.setParam(IloCplex::Param::MIP::Limits::Nodes, 0);
    
    
    //temps cpu max = 2h30 (en secondes)
   // cplex.setParam(IloCplex::TiLim, 9000);
    //set de l'algorithme à utiliser pour la resolution
    //if (useBenders==1)
       // cplex.setParam(IloCplex::Param::Benders::Strategy, 3);
    
   
    
    // *** on recopie le tableau tCoutVar en standard iloconcert. on le recupere en transposee
    //la transposée facilitera les calculs entre tableau pour les produits scalaires.
    IloArray<IloIntArray> C_ij (env, n);
    for(int i=0 ; i<m ; i++)
    {
        C_ij[i]= IloIntArray(env, n);
        for(int j=0; j<n ; j++)
            C_ij[i][j]= tCoutVar[i][j];
    }
    
    
    // *** on recopie le tableau tCouFix en standard iloconcert. on le recupere en transposee
    //la transposée facilitera les calculs entre tableau pour les produits scalaires.
    IloArray<IloIntArray> F_ij (env, m);
    for(int i=0 ; i<m ; i++)
    {
        F_ij[i]= IloIntArray(env, n);
        for(int j=0; j<n; j++)
            F_ij[i][j]= tCoutFix[i][j];
    }
    
    
    // *** definissons les variables
    
    IloArray<IloNumVarArray> x (env, m);//m ici est le nombre de sinks voir sample.h
    //chaque x[i] est un tableau de m variables reelles
    for(int i=0 ; i<m ; i++)
        x[i] = IloNumVarArray(env, n, 0.0, IloInfinity, ILOFLOAT);
    
    
    IloArray<IloNumVarArray> y (env, m);//m ici est le nombre de sinks voir sample.h
    //chaque x[i] est un tableau de m variables binaires
    for(int i=0 ; i<m ; i++)
        y[i] = IloNumVarArray (env, n, 0, 1, ILOINT);
    
    // ***construisons l'expression de la fonction objetif
    
    IloExpr obj(env);
    for (int i= 0; i<m ; i++)
    {
        for(int j=0; j<n; j++)
            obj += x[i][j]* C_ij[i][j] + y[i][j] * F_ij[i][j];
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
    
    //x_ij < u_ij*y_ij
    for(int i=0; i<m; i++)
    {
        for (int j =0; j<n ; j++)
            mod.add(x[i][j] <= tCapacity[i][j] *y[i][j]);
    }
    
    //*** on va determiner ici les 2 Inégalités valides qu'on pourrait ajouter ou pas au modèle
    
    if (IV_suppl==1) // si on a choisi d'ajouter les contraintes suppl au modèle
    {
        //( sum(y_ij*u_ij , j in N) >= demand_j
        for (int j = 0; j< n; j++)
        {
            IloExpr ctr(env);
            for (int i=0; i<m ; i++)
                ctr = ctr + y[i][j]* tCapacity[i][j];
            mod.add(ctr >= tDemand_b[j] );
        }
        
        //( sum(y_ij*u_ij , i in M) >= offfre_i
        for (int i = 0; i< m; i++)
        {
            IloExpr ctr(env);
            for (int j=0; j<n ; j++)
                ctr = ctr +y[i][j]* tCapacity[i][j];
            mod.add(ctr >= tOffre_a[i] );
        }
    }
    
    
    
    // ------------------ AFFICHAGE ET OPTIMISATION ----------------
    
    // résolution
    cplex.solve();
    
    // récupère la solution et l'affiche à l'écran
    cout << endl <<" valeur de l'objectif = " << cplex.getObjValue() << endl;
    // cout << "algo utilisé"<< cplex.getAlgorithm() <<endl;
    
    
    // export du PL créé dans un fichier .lp
    //cplex.exportModel("/Users/julieamanda/Documents/WorkspaceMem/test.lp");
    
    // export de la solution dans un fichier texte
   // cplex.writeSolution("/Users/julieamanda/Documents/WorkspaceMem/sol.txt");
    
    
   
    float res = cplex.getObjValue();
    env.end();
  
    
    return  res ;
   
}





