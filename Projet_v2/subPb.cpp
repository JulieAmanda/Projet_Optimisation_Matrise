//
//  subPb.cpp
//  Projet_v2
//
//  Created by JULIE AMANDA TCHOUANDEM KEMOE on 2020-09-16.
//  Copyright © 2020 JULIE AMANDA TCHOUANDEM KEMOE. All rights reserved.
//
#include "subPb.hpp"
#include "projectParameters.h"
#include <iostream>

#include <ilcplex/ilocplex.h>
#include <string.h>
ILOSTLBEGIN


//temps cpu max = 1h30 (en secondes)
//cplex.setParam(IloCplex::TiLim, 9000);

//set de l'algorithme à utiliser pour la resolution
//if (useBenders==1)
//cplex.setParam(IloCplex::Param::Benders::Strategy, 3);

void ssPbSrc_i( int numSrc, float * tAlpha, float nu, int * ValX_ij, int Offre_i,
               int * val_U_ij, int * val_C_ij, int * val_F_ij, int n, float & valObj){
    

IloEnv env;
IloModel mod(env); //model
IloCplex cplex(mod); // instance du solveur à utiliser.

//limitons le nombre de processeurs
cplex.setParam(IloCplex::Param::Threads, 1);

  //** on convertit les tableaux Val_(C|U|F)_ij du type entier ver le type entier de iloconcert
    
   /* cout << endl;
    for (int j=0; j<n; j++){
        cout << val_F_ij[j] << "  " ;
        }*/
   
    IloIntArray  U_ij (env, n);
    for(int j=0; j<n ; j++)
        U_ij[j]= val_U_ij[j];
   
    
    IloIntArray  C_ij (env, n);
    for(int j=0; j<n ; j++)
        C_ij[j]= val_C_ij[j];
    
    
    IloIntArray  F_ij (env, n);
    for(int j=0; j<n ; j++)
        F_ij[j]= val_F_ij[j];
    
    IloIntArray  alpha (env, n);
    for(int j=0; j<n ; j++)
        alpha[j]= tAlpha[j];
    
    

    
    // *** definissons les variables
    IloNumVarArray  x_i (env, n, 0.0, IloInfinity, ILOINT);
    
    IloNumVarArray y_i (env, n, 0, 1, ILOINT);
    
    
    
    // ***construisons l'expression de la fonction objetif
    
    IloExpr obj(env);
    obj=nu*(IloScalProd(C_ij, x_i) + IloScalProd(F_ij, y_i))- IloScalProd(alpha, x_i);
   
    //cout << IloScalProd(C_ij, x_i) + IloScalProd(F_ij, y_i)<<endl;
    
    IloObjective objectif (env, obj, IloObjective::Minimize, "OBJ");
    mod.add(objectif);
    
    
    // *** definissons à présent les contraintes
    
    //( sum(x_ij , j in N)= offre_i
    mod.add(IloSum(x_i)==Offre_i);
    
    //x_ij < u_ij*y_ij
    for (int j =0; j<n ; j++)
        mod.add(x_i[j] <= U_ij[j] *y_i[j]);
    
    
    
    //*** on va ajouter ici l'Inégalité valide qu'on facultative qui correspond au coté offre
    
    if (IV_suppl==1) // si on a choisi d'ajouter les contraintes suppl au modèle
    {
        //( sum(y_ij*u_ij , i in M) >= offfre_i
        mod.add(IloScalProd(U_ij, y_i)>= Offre_i);
       
    }
    
    
    
    // ------------------ AFFICHAGE ET OPTIMISATION ----------------
    
    // export du PL créé dans un fichier .lp
    cplex.exportModel("/Users/julieamanda/Documents/WorkspaceMem/test.lp");
    // résolution
    cplex.solve();
    // export de la solution dans un fichier texte
    cplex.writeSolution("/Users/julieamanda/Documents/WorkspaceMem/sol.txt");
    // récupère la solution et l'affiche à l'écran
    //cout << endl <<" valeur de l'objectif = " << cplex.getObjValue() << endl;
    // cout << "algo utilisé"<< cplex.getAlgorithm() <<endl;
    
    valObj= cplex.getObjValue();
    cout << endl <<" valeur de l'objectif = " << valObj << endl;
    
    
        for (int j=0; j<n; j++){
            ValX_ij[j] = cplex.getValue(x_i[j]);
             cout << cplex.getValue(x_i[j]) << "  " ;
        }
    
    
    cout <<endl;
    for (int j=0; j<n; j++){
       // ValX_ij[j] = cplex.getValue(x_i[j]);
        cout << cplex.getValue(y_i[j]) << "  " ;
    }

        
    
    
}


//la fonction du sous pb du coté destination


void ssPbDest_j( int numDest, float * tAlpha, float nu, int * ValW_ij, int Demand_j,
                int * val_U_ij, int * val_C_ij, int * val_F_ij, int m, float & valObj)
    {
        
        
        IloEnv env;
        IloModel mod(env); //model
        IloCplex cplex(mod); // instance du solveur à utiliser.
        
        //limitons le nombre de processeurs
        cplex.setParam(IloCplex::Param::Threads, 1);
        
        //** on convertit les tableaux Val_(C|U|F)_ij du type entier ver le type entier de iloconcert
        
       /* cout << endl;
        for (int j=0; j<m; j++){
            cout << val_F_ij[j] << "  " ;
        }*/
        
        IloIntArray  U_ij (env, m);
        for(int j=0; j<m ; j++)
            U_ij[j]= val_U_ij[j];
        
        
        IloIntArray  C_ij (env, m);
        for(int j=0; j<m ; j++)
            C_ij[j]= val_C_ij[j];
        
        
        IloIntArray  F_ij (env, m);
        for(int j=0; j<m ; j++)
            F_ij[j]= val_F_ij[j];
        
        IloIntArray  alpha (env, m);
        for(int j=0; j<m ; j++)
            alpha[j]= tAlpha[j];
        
        
        
        
        // *** definissons les variables
        IloNumVarArray  w_i (env, m, 0.0, IloInfinity, ILOINT); // du coté destination x_ijˆd=w_ij
        
        
        IloNumVarArray z_i (env, m, 0, 1, ILOINT); //du coté destination y_ijˆd=z_ij
        
        
        
        // ***construisons l'expression de la fonction objetif
        
        
        IloExpr obj(env);
        obj=(1-nu) * (IloScalProd(C_ij, w_i) + IloScalProd(F_ij, z_i)) + IloScalProd(alpha, w_i);
        
       // cout << IloScalProd(C_ij, w_i) + IloScalProd(F_ij, z_i) << endl;
        IloObjective objectif (env, obj, IloObjective::Minimize, "OBJ");
        mod.add(objectif);
        
        
        // *** definissons à présent les contraintes
        
        //( sum(x_ij , j in N)= demand_j
        mod.add(IloSum(w_i)==Demand_j);
        
        //x_ij < u_ij*y_ij
        for (int j =0; j<m ; j++)
            mod.add(w_i[j] <= U_ij[j] *z_i[j]);
        
        
        
        //*** on va ajouter ici l'Inégalité valide qu'on facultative qui correspond au coté offre
        
        if (IV_suppl==1) // si on a choisi d'ajouter les contraintes suppl au modèle
        {
            //( sum(y_ij*u_ij , i in M) >= offfre_i
            mod.add(IloScalProd(U_ij, z_i)>= Demand_j);
            
        }
        
        
        
        // ------------------ AFFICHAGE ET OPTIMISATION ----------------
        
        // export du PL créé dans un fichier .lp
        cplex.exportModel("/Users/julieamanda/Documents/WorkspaceMem/test.lp");
        // résolution
        cplex.solve();
        // export de la solution dans un fichier texte
        cplex.writeSolution("/Users/julieamanda/Documents/WorkspaceMem/sol.txt");
        // récupère la solution et l'affiche à l'écran
       
        // cout << "algo utilisé"<< cplex.getAlgorithm() <<endl;
        
        valObj= cplex.getObjValue();
         cout << endl <<" valeur de l'objectif = " << valObj << endl;
        for (int j=0; j<m; j++){
            ValW_ij[j] = cplex.getValue(w_i[j]);
            cout << cplex.getValue(w_i[j]) << "  " ;
        }
        
        cout << endl;
        for (int j=0; j<m; j++){
            //ValW_ij[j] = cplex.getValue(w_i[j]);
            cout << cplex.getValue(z_i[j]) << "  " ;
        }
        
    
    
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


}
