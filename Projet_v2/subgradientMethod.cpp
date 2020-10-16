//
//  subgradientMethod.cpp
//  Projet_v2
//
//  Created by JULIE AMANDA TCHOUANDEM KEMOE on 2020-09-29.
//  Copyright © 2020 JULIE AMANDA TCHOUANDEM KEMOE. All rights reserved.
//

#include<iostream>
#include <math.h>
#include "subgradientMethod.hpp"
#include "globalModel.hpp"
#include "subPb.hpp"
#include "IterationDetails.hpp"

using namespace std;


void affichTabSolutions ( int m, int n , int ** tab){
    
    for (int i=0; i<m; i++){
        for (int j=0; j<n; j++)
            cout<<tab[i][j] << " ";
        cout << endl ;
    }
}


// foction qui nous permettra de verifier le changement de la borne inférieure aucours de nb itérations successives.
bool lowerBndVariation(int nbIterationsMax, bool & valueUpdated, float & optimal_lwrBnd, float lowerBound, int & count){
    
    float stopVerify = false; // permet de savoir si on a atteind le nombre max d itérations ou pas
    
    if ( optimal_lwrBnd>=lowerBound){ // si il n'y a pas eu amélioration de la borne optimale
        count +=1;   //on compte une itération de plus sans amélioration
        }
    else {//si on obtient une meilleur borne inférieur
        optimal_lwrBnd=lowerBound;   //on met à jour la valeur de la meilleur borne inférieur
        valueUpdated=true;             //on enregistre qu'il y a eu amélioration
        count=0;                    // on reinitialise le décompte
        }
    
    if (count == nbIterationsMax) //si on a atteint le nombre max à vérifier on arrête
    { stopVerify=true;              //on arrete de surveiller l'amélioration
        count=0;                   //on recommence de surveiller l'amélioration à 0
    }
    
    // dans l'algo principal, si le valueUpdated sont true alors on va recommencer le count. si le stopVerfy est true faire les controles que cela implique pour la suite de l'algo de sous gradient
    return stopVerify;
    
}


//---------- implémentation de l'algorithme du sous-gradient --------//

IterationDetails Subgradient( int m, int n, int * tOffre, int * tDemand, int ** tCoutVar, int ** tCoutFix, int ** tCapacity){
    
    
    // on crée deux var booléenes nous permettrons de verifier si la borne min change au fil de a ( repectivement b) itérations
    //on considère a = omga_2 et b=omga_3
    bool bMinUpdated_a_It= false;//on va mettre c'est deux en param ds la fctn lwrBndVariation
    bool bMinUpdated_b_It = false;
    
    // on crée 2 var bool pour controler si on est arrivé à un nombre max voulu d'itérations
    bool stopVerify_a_It = false; // nous permettra de savoir si on met à jour le parametre lambda ds le clcul du pas(apres a=omga_2 repetition)
    bool stopVerify_b_It = false ; //controle si on arrête l'algo ( apres b=omga_3 iterations)
    int count_a = 0;
    int count_b = 0; // ce seront les compteurs pour verifier le nb d'iterations successives
    
    float meilleur_bInf_a = 0;//meilleur b_Min enregistré sur un interval de a iterations
    float meilleur_bInf_b = 0;//meilleur b_Min enregistré sur un interval de b iterations

    
    int iteratnNumber =0;
    
    
    //on va fixer les multiplicateurs de lagrange à 0 pour la première iteration à 0 pour un début
   
    //fixons nu
    float nu=0.5;
    
    //on initialise les données de ;'algo à 0 pour l'iteration 1
    IterationDetails I_prec(m, n);
   
    
    
    //tant qu'on a pas atteint le nbre max d'iteration et que stopverify_b_it = false( la valeur optimale ne s'est pas encore repete b= omga_3 fois) on continue l'algo
    while ((I_prec.bMin != I_prec.bMax) && (iteratnNumber < omga_4) && (stopVerify_b_It == false)){
        
        //on va stocker ici les données de l'itération courante
        IterationDetails I_cour(m, n);
       
        /*  résolution des sous problèmes pour les noeuds sources */

        float valObj =0;
        
        for (int i=0; i<m; i++){
            
            //on va afficher les données utiles pour ce sous-Pb
            cout << endl;
            cout << "***  Noeud source : numéro " << i <<"   ***     "<<endl;
            
            //pour afficher les couts fixes/variables, capacité et demande lié au subpb
            //affich_subPbData(tOffre[i], tCapacity[i], tCoutVar[i], tCoutFix[i], m, n); //voir la fonction dans mainfile_compliments.hpp
            
            //on va fixer la tables des multiplicateurs de lagrange à 0 pour un début
            float tAlpha_i [n];
            for (int j=0; j<n; j++)
                tAlpha_i[j]=I_prec.multiplyers[i][j];
            
            //on résoud le sous-problème pour le noeud i
            ssPbSrc_i( i, tAlpha_i, nu,  I_cour.tX_ij[i], tOffre[i],
                      tCapacity[i], tCoutVar[i], tCoutFix[i], n, valObj);
            I_cour.valObjSrc[i]=valObj;
            
            makeSpace(); // mettre de l'espace dans l'affichage (voir mainfile_complements.hpp
            }
        
        //résolution des sous_pb pour les noeuds destination  --------- */
        
        makeSpace();
        cout<< "partie Destination "<< endl;
        
        //construisons les tables à passer en paramètres au sous_prob du coté destination
        int t1 [m]; //on va prendre ici la colonne j du tableau des capacités u_ij
        int t2 [m];
        int t3 [m];
        
        
        
        for (int j=0; j<n; j++){
            
            // etant donné que pour le souspb du coté destination à un noeud j on a besoin de tableau contenant les capacité, couts fixes et variables à partir du noeud j, on va construire les tableaux contenant ces données à partir des matrices tcapacity, tcoutFix et tCoutVar
            for (int i=0;i<m;i++) {
                t1[i]= tCapacity[i][j];
                t2[i]=tCoutVar[i][j];
                t3[i]=tCoutFix[i][j]; }
            
            // affichons les données pour vérification
            cout << endl;
            cout << "***  Noeud Destination : numéro " << j <<"  ***     "<<endl;
            
            
            //pour afficher les couts fixes/variables, capacité et demande lié au subpb
            //affich_subPbData(tDemand[j], t1, t2, t3, m, n); //voir la fonction dans mainfile_compliments.hpp
            
            //table contenant les multiplicateurs de lagrange
            float tAlpha_j [m];
            for (int i=0; i<m; i++)
                tAlpha_j[i]=I_prec.multiplyers[i][j];
            //on prend les multiplicateurs de la destination j
            
            int * tab= new int[m]; //on va stocker ici nos valeurs des variables w_j[i] i de 1 à m de façon temporaire
            
            //à présent on fait appel au ss_pb coté Dest
            ssPbDest_j( j, tAlpha_j,  nu, tab, tDemand[j], t1 ,t2, t3,  m, valObj); // voir file subPb.cpp
            I_cour.valObjDest[j]= valObj;
            for (int i=0; i<m ;i++)
                I_cour.tW_ij[i][j]=tab[i];
            
            delete [] tab;
            
            makeSpace(); // on fait de l'espace dans l'affichage ( voir mainfile_compliments.hpp)
        }
        
        //calcul des sous-gradient et de la borneMin de l'objectif
        
        I_cour.calcul_subGradient(m, n);
        I_cour.calculBorneMin(m, n); // calcul borne Min -- voir subgradientMethod.cpp
        
        makeSpace(); // espace entre les paragraphes à l'affichage
        cout << " ***   borne min : " <<  I_cour.bMin<<"   ***  "<<endl;// val de borne minimale
        makeSpace(); // espace entre les paragraphes à l'affichage
        
        cout << "resolvons  le problème global à la racine pour obtenir une borne supérieure ..." << endl;
        //  calcul de la borne sup -- voir globalModel.cpp
        float bornSup = ModelBase_Bsup( m , n, tOffre, tDemand, tCoutVar, tCoutFix, tCapacity);
        I_cour.bMax=bornSup;
        
        makeSpace(); // espace entre les paragraphes  à l'affichage
        cout << " ***    borne sup : " <<  I_cour.bMax <<"   ***  "<<endl;
        
        
        //on incremente le compteur d'iteration
        iteratnNumber = iteratnNumber + 1;
       
        
        
        //on va maintenant faire le test de variation de la borne inférieure du problème
        stopVerify_a_It=lowerBndVariation(omga_2, bMinUpdated_a_It, meilleur_bInf_a, I_cour.bMin, count_a);
        stopVerify_b_It=lowerBndVariation(omga_3, bMinUpdated_b_It, meilleur_bInf_b, I_cour.bMin, count_b);
        
        //testons s'il y a eu des variations pour effectuer les controls neccessaires
        //après omga_2=15 iterations sans changement de bMin on met à jour lamba
        if (stopVerify_a_It==true){
            I_cour.lambda = I_prec.lambda/omga_1;
            stopVerify_a_It=false;
        }
        else{
            I_cour.lambda = I_prec.lambda;
        }
        
        
        
        //on va calculer le pas et les multiplicateurs de lagrange etant donné qu'on a déjà tous les elements necessaires
    
        cout<< endl;
        cout << "iteration numero :" << iteratnNumber;
        cout << endl;
        
        I_cour.calculPAs(m, n, I_prec.bMin);
        I_cour.calculMultipliyers_t(m, n, I_prec.multiplyers);
        
        cout << endl;
        cout << "le pas est : " << I_cour.pas << endl;
        
        cout << endl;
        cout << "lambda est : " << I_cour.lambda << endl;
        
        cout << endl;
        cout << "tab multiplyers it prec" << endl;
        for (int i=0; i<m; i++){
            for (int j=0; j<n; j++)
                cout<<I_prec.multiplyers[i][j] << " ";
            cout << endl ;
        } //   affichons la table des sous-gradients
        
        
        cout << endl;
        cout << "tab multiplyers" << endl;
        for (int i=0; i<m; i++){
            for (int j=0; j<n; j++)
                cout<<I_cour.multiplyers[i][j] << " ";
            cout << endl ;
        } //   affichons la table des sous-gradients
        
        
        // --- affichage des solutions des ssPb et comparaison
        
        makeSpace();
        cout << "solutions et valeurs de l'instance précédente" << endl;

        cout << "tab x_ijˆs" << endl;
        affichTabSolutions(m, n, I_prec.tX_ij);//voir subpb.cpp
        
        cout << endl;
        cout << "tab x_ijˆd" << endl;
        affichTabSolutions(m,n, I_prec.tW_ij );  //affichons la transposée -- voir fonction dans subgradient.cpp
        
        cout << endl;
        cout << "tab alpha_i_j" << endl;
        affichTabSolutions(m,n,I_prec.tSubGrad ); //   affichons la table des sous-gradients
        
       
      
        makeSpace(); //espace à l'affichage (voir subpb.cpp)
        cout << "solutions et valeurs de l'instance courante" << endl;

        // --- affichage des solutions des ssPb et comparaison
        
        cout << "tab x_ijˆs" << endl;
        affichTabSolutions(m, n, I_cour.tX_ij);
        
        cout << endl;
        cout << "tab x_ijˆd" << endl;
        affichTabSolutions(m,n, I_cour.tW_ij );  //affichons la transposée -- voir fonction dans subgradient.cpp
        
        cout << endl;
        cout << "tab alpha_i_j" << endl;
        affichTabSolutions(m,n,I_cour.tSubGrad ); //   affichons la table des sous-gradients
        

        
        I_prec.updateInstance(m, n, I_cour);
        
        
    }
    
    cout << "le nombre d'iterations est :  " << iteratnNumber << endl;
    cout << "la meilleure borne inf de 15 est  :  " << meilleur_bInf_a << endl;
    cout << "la meilleure borne inf de 30 est  :  " << meilleur_bInf_b << endl;
    return I_prec;
    
}
