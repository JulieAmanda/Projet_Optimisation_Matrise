//
//  heuristiqBSup.cpp
//  Projet_v2
//
//  Created by JULIE AMANDA TCHOUANDEM KEMOE on 2020-12-03.
//  Copyright © 2020 JULIE AMANDA TCHOUANDEM KEMOE. All rights reserved.
//

#include "heuristiqBSup.hpp"
#include <random>
#include <chrono>
#include <iostream>

using namespace std;


int * DistribAleatoire(int n, int m)
{
    int tailleTab=n*m;
    int * tab =new int[tailleTab];
    
    
    std::mt19937 re(std::chrono::system_clock::now().time_since_epoch().count());
    //std::chrono::system_clock::now().time_since_epoch().count()
    //mt19937_64
    //default_random_engine
    std::uniform_int_distribution<int> distrib{ 1, 10000000};
    
    for (int i=0; i<tailleTab; i++)
        tab[i]= distrib(re);
    
    
    
    return tab;
}


/* -----*/

bool heuristique( int * tabDistAleatr, int ** tabW, int ** tabX, int m, int n , int * historiqSol, int ** state){
    
    int sum=0 ;
    int pos ;
    bool ignore =true ;
    
   // int ** state= new int * [2]; //on va stocker ici l'etat de la solution obtenue
    // 1 ligne pour la position à laquelle le sous-grad=0 et l'autre pour la valeur de yij à cette position
    
//    state[0][0]=0;// on va reserver cette case pour stocquer le nombre Yij qui ont déjà été retenus pour être fixés
//    state[1][0]=0 ;// on reserve la premiere case de la deuxieme ligne pour stocker la somme finale qui est la clé.
    
    
//    int posState=0; // on va commencer à modifier le tableau sate à partir de la 2e colonne car la colonne 1 a déjà été reservée
    
        
        for (int i=0; i<m ; i++){
            for (int j=0; j<n; j++){
                
               pos= i*m+j;
                state[0][pos]= pos;
               
                if (  tabW[i][j] - tabX[i][j]==0) {
                    
                    //state[0][posState]= pos;
                    
                   // state[1][posState]= 1;

                    
                    if  (tabX[i][j]>0 || tabW[i][j] >0){
                        //sum= sum + tabDistAleatr[pos] * 1 ... dans le cas ou tabXij=0 on va multiplier par 0 ( sum= sum + tabDistAleatr[pos] * 0) donc pas neccessaire de l'ajouter
                        sum= sum + tabDistAleatr[pos] ;
                        state[1][pos]= 1;
                    }
                    else{
                        state[1][pos]= 0;
                    }
                   
                }
                else{
                    // dans le cas ou le sous-gradient n'est pas nul on multiplie par -1
                    //sum= sum - tabDistAleatr[pos];
                     state[1][pos]= -1;
                }
                
//                state[0][0] +=1 ; //on signale que la taille du tableau à augmenter en mettant à jour la tete du tableau qui contient cette taile
//                posState += 1;
                
                
                

            }
        }
//        state[1][0]=sum; //on stocke la sum en 2e ligne 1ere colonne
        //je vais vérifier dans l'historique des solutions précédentes si la solution obtenue n'était pas déjà existante
        int taille= historiqSol[0];
        int i=1;
        while ( i<=taille && historiqSol[i]!=sum){
            i++;
        }
        
        //si cette solution n'avait pas encore traité alors je vais chercher la borne sup correspondante dans le cas contraire on ne fera rien on va l'ignorer
        if (i==taille+1){
            historiqSol[i]=sum;
            historiqSol[0] += 1; // on met à jour cette valeur qui represente la taille du tableau car on a ajouter une nouvelle clé
            ignore=false; //lorsque ignore est false on va devoir considérer cette solution et appliquer l'heuristique
            
            
        }
    
    cout<<"print state "<< endl;
    for (int i=0; i<n*m; i++)
        cout<<state[0][i]<< "   ";
    cout<<"print state 1 "<< endl;
    for (int i=0; i<n*m; i++)
        cout<<state[1][i]<< "   ";
    cout<<"print historiq 1 "<< endl;
    for (int i=0; i<=historiqSol[0]; i++)
        cout<<historiqSol[i]<< "   ";
        
        return ignore;
    
    
}


/* ----*/

// mettre à jour la table des state selon les valeurs des solutions de la procedure slotScaling
void updateTabState( int ** tabSolScaling, int **  state, int m, int n){
    
    cout << "lets'try" << endl;
    int pos;
    for (int i=0; i< m; i++){
        for (int j=0; j<n; j++){
            pos = i*m +j;
            if ((( state[1][pos]=0 ) && (tabSolScaling[i][j] !=0)) || (( state[1][pos]=1 )&& (tabSolScaling[i][j] !=1)))
                state[1][pos]=-1;
        }
    }
    
    
    cout<<"print state 1 "<< endl;
    for (int i=0; i<n*m; i++)
        cout<<state[1][i]<< "   ";
    
}

