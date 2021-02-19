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
#include "heuristiqBSup.hpp"
//#include "samples.hpp"

#include <ilcplex/ilocplex.h>
#include <string.h>
ILOSTLBEGIN

//tabBestSol est la table dans laquelle on stoque la meilleure borne sup( solution réalisable obtenue tout le long de l'algorithme)
float ModelBase_Bsup(int m, int n, int * tOffre_a, int * tDemand_b, int ** tCoutVar, int ** tCoutFix, int ** tCapacity, bool  ignore, int ** state, float bestBornSup, int ** tabBestSol, int ** historiqY_ij, int nbCallHrstq)
{
    IloEnv env;
    IloModel mod(env); //model
    IloCplex cplex(mod); // instance du solveur à utiliser.
    
    //limitons le nombre de processeurs
    cplex.setParam(IloCplex::Param::Threads, 1);
    
    //temps cpu max = 1h30 (en secondes)
    //cplex.setParam(IloCplex::TiLim, 9000);
    
    //dire à cplex de s'arreter à la racine
    cplex.setParam(IloCplex::Param::MIP::Limits::Nodes, 1);
    
    
    //set de l'algorithme à utiliser pour la resolution
    //if (useBenders==1)
    // cplex.setParam(IloCplex::Param::Benders::Strategy, 3);
    
    
    
    
    // *** on recopie le tableau tCoutVar en standard iloconcert. on le recupere en transposee
    //la transposée facilitera les calculs entre tableau pour les produits scalaires.
    IloArray<IloIntArray> C_ij (env, m);
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
        x[i] = IloNumVarArray(env, n, 0.0, IloInfinity, ILOINT);
    
    
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
    
    //par défaut nous l'avons défini à 1.
    if (IV_suppl==1) // si on a choisi d'ajouter les inégalités valides supplémentaires au modèle
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
    
    /* on va faire la partie heuristique qui controle si on doit faire un calcul pour la mise à jour de la borne */
    
    // bool ignore= heuristique(tabDistAleatr, tSubGrad, tabX,  m,  n, historiqSol, state); si ignore est true cela signifie que la cle obtenue a deja eté donc on aura pas besoin de cette partie car elle intervient juste pour le calcul de la borne sup après le slope scaling
   
    if( ignore==false){
        int a, b;
        int nb= n*m;
        for (int i=0; i<nb ; i++) {
            //ajouter if ( c'est different de -1)
            
            if (state[1][i] != -1){
                a=state[0][i] / n;
                b=state[0][i] % n;
                mod.add(y[a][b] == state[1][i]);
                
            }
        }
    }
    
    
    
    // ------------------ AFFICHAGE ET OPTIMISATION ----------------
    
    // export du PL créé dans un fichier .lp
    
    cplex.solve();

    // récupère la solution et l'affiche à l'écran
    cout << endl <<" valeur de l'objectif = " << cplex.getObjValue() << endl;
    // cout << "algo utilisé"<< cplex.getAlgorithm() <<endl;
    

    
    //on ajoute les valeurs de y_ij suite à l'heuristiq lagrangienne dans l'historiqY_ij pour sauvegarder l'évolution de l'état des arcs
   
    if ( ignore==false){
        
        for (int i=0; i< m; i++){
            cout << endl;
            for (int j=0; j<n; j++){
                int pos=i*m+j;
                historiqY_ij[nbCallHrstq][pos]=int(cplex.getValue(y[i][j]));
                cout <<endl;
                cout<< "le tableau se print bien example" << historiqY_ij[nbCallHrstq][10]<< endl;
                cout<< endl;
                
            }
        }
    }

   
    float res = cplex.getObjValue();
    
    //on verifie s'il faut une mise à jour de la borne sup
    if( res>bestBornSup){
        bestBornSup=res;
        //on sauvegarde cette meilleure solution
        for (int i=0; i< m; i++){
            cout << endl;
            for (int j=0; j<n; j++){
                //cout << cplex.getValue(x[i][j]) << "  " ;
                tabBestSol[i][j]=int(cplex.getValue(x[i][j]));
            }
        }
    }
    env.end();
    return  res ;
}






void currentClosedArcs (int ** historiqY_ij, int ** stateY_ij, int nbCallHrstq, int m, int n){
    bool similar;
    
    int nb =m*n;
    int i=0;
    int k=1;
    int a, b;
    for ( int j=0; j<nb; j++){
        similar=true;
        a= j/n;
        b= j % n;
        if ( historiqY_ij[i][j]==0){
            while (( k<nbCallHrstq) && (similar==true )){
                if ( historiqY_ij[k][j]==0)
                    k +=1;
                else
                    similar=false;
            }
            if (similar==true){
                //cela marque que cet arc est toujours fermé
                stateY_ij[a][b]=0;
            }
        }
        else
            stateY_ij[i][j]= -1;//on ne va pas s'interresser à l'etat de cet arc
    }


}




float lastPostOptim( int m , int n, int * tOffre_a, int * tDemand_b, int ** tCoutVar, int ** tCoutFix, int ** tCapacity, float bestBornSup, int ** tabBestSol, int ** historiqY_ij, int nbCallHrstq){
    
    
    IloEnv env;
    IloModel mod(env); //model
    IloCplex cplex(mod); // instance du solveur à utiliser.
    
    //limitons le nombre de processeurs
    cplex.setParam(IloCplex::Param::Threads, 1);
    
    //on va préciser à cplex la valeur de la meilleure solution afin qu'il coupe ou ignore tous les noeuds ne proposant pas une meilleure valeur que cette solution
    cplex.setParam(IloCplex::CutUp, bestBornSup);
    
    //temps cpu max = 1h30 (en secondes)
    //cplex.setParam(IloCplex::TiLim, 9000);
    
    //dire à cplex de s'arreter à la racine
    //cplex.setParam(IloCplex::Param::MIP::Limits::Nodes, 1);
    
    
    //set de l'algorithme à utiliser pour la resolution
    //if (useBenders==1)
    // cplex.setParam(IloCplex::Param::Benders::Strategy, 3);
    
    //on cree une table qui servira à récupérer les arcs qui sont toujours fermés dans toutes les solutions parcourues
    int ** stateY_ij = new int * [m];
    for (int i=0; i<m;i++)
        stateY_ij[i]= new int [n];
    
    //on recupere les valeurs de tabBestsol pour convertir en type iloNum
    IloArray<IloNumArray> val_X (env, m);
    for(int i=0 ; i<m ; i++)
    {
        val_X[i]= IloNumArray(env, n);
        for(int j=0; j<n ; j++)
            val_X[i][j]= tabBestSol[i][j];
    }
    
    
    
    //nous allons dans ce tableau stocker les valeurs de y_ij pour la meilleur solution que nous allons passer en paramètres à CPLEX
      //ON VA RÉCUPÉRER les valeurs des variables y_ij correspondant à tabBestSol.
    IloArray<IloNumArray> val_Y (env, m);
    for(int i=0 ; i<m ; i++)
    {
        val_Y[i]= IloNumArray(env, n);
        for (int j=0; j<n ; j++){
            if ( tabBestSol[i][j] >0)
                val_Y[i][j] = 1;
            else
                val_Y[i][j]=0;
            
        }
    }
 
   
    
    // *** on recopie le tableau tCoutVar en standard iloconcert. on le recupere en transposee
    //la transposée facilitera les calculs entre tableau pour les produits scalaires.
    IloArray<IloIntArray> C_ij (env, m);
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
        x[i] = IloNumVarArray(env, n, 0.0, IloInfinity, ILOINT);
    
    
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
    
    //par défaut nous l'avons défini à 1.
    if (IV_suppl==1) // si on a choisi d'ajouter les inégalités valides supplémentaires au modèle
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
    
    //on remplie la table en indiquant les arcs qui étaient régulièrement fermés tout au long de l'heursitique.
    
    currentClosedArcs(historiqY_ij, stateY_ij, nbCallHrstq, m, n);
    
    
    
    //on va fermer les arcs pour lesquels stateY_ij=0: c'est l'ensemble des arcs qui étaient toujours fermés dans toutes les solutions parcourues pendant l'algorithm
    for (int i = 0; i< m; i++){
        for (int j=0; j<n ; j++){
            if ( stateY_ij ==0)
                 mod.add(y[i][j] == 0);
            
    }
}
    
    
    //on va ensuite passer en paramètre à cplex la meilleure solution obtenue : les valeurs des variables d'abord
    //la fction addMIPStart précise à Cplex qu'il doit commencer à chercher une meilleure solution par rapport à cette solution
    for (int i=0; i<m; i++){
        cplex.addMIPStart(x[i], val_X[i]);
        cplex.addMIPStart(y[i], val_Y[i]);
    }
    
    
    
    
    
    
    // ------------------ AFFICHAGE ET OPTIMISATION ----------------
    
    // export du PL créé dans un fichier .lp
    
    cplex.solve();
    
    // récupère la solution et l'affiche à l'écran
    cout << endl <<" valeur de l'objectif = " << cplex.getObjValue() << endl;
    // cout << "algo utilisé"<< cplex.getAlgorithm() <<endl;
    
    
    
 
    
    float res = cplex.getObjValue();
    
   
    env.end();
    return  res ;
    
    
}
