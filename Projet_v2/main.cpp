

#include <iostream>
#include "sample.hpp"
#include "projectParameters.h"
#include "subPb.hpp"
#include "mainfile_complements.hpp"

using namespace std;




int main(int argc, const char * argv[]) {
    
    //***   commençons par construire les structures de données à utiliser à partir des données prises dans les fichiers echantillons  ***//

    int m =  0;
    int  n = 0 ;
    get_M_N(m, n); //on va recuperer les valeurs de m et n contenues dans le fichier
    
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
        readInput_Dataset2(m, n, tOffre, tDemand,tCoutVar, tCoutFix);
    else if (datasetGroup==3)
        readInput_Dataset3(m, n, tOffre, tDemand,tCoutVar, tCoutFix);
    else
        cout << "ce dataset Group est inexistant";
    
    //on va construire le tableau des capacités
    BuiltCapacity(m, n, tOffre, tDemand, tCapacity);
    
    

    //***  On va maintenant construire le problème par décompositiion lagrangienne en construisant des sous problèmes par noeud et recuperant les valeurs de chaque variables par noeud ***//
    
    int ** tVal_NdSrc = new int*[m];//ici on va stocker les valeurs des flots des noeuds sources
    for (int i = 0; i < m; ++i)
        tVal_NdSrc[i] = new int[n];
    
    
    int ** tVal_NdDest= new int* [n];  //ici on va stocker les valeurs des flots des noeuds destinations
    for (int j = 0; j < n; ++j)
        tVal_NdDest[j] = new int[m];
    
    int ** tabSubgradient= new int* [n];  //table qui va contenir les sous-gradient après resolution des sspb
    for (int j = 0; j < n; ++j)
        tabSubgradient[j] = new int[m];
    
    //construisons les tables qui contiendra les valeurs des objectifs pour chaque noeud pr
    
    float * ValObjSrc = new float[m];
    float * ValObjDest= new float[n];
    float valObj = 0; //ici on va recuperer le resultat à partir des sous-pb pour le garder ds le tableau
    
    cout<< m<< endl;
    cout <<n << endl;
   
    
    
    
/*  résolution des sous problèmes pour les noeuds sources */
    
    //on va fixer les alpha à 0 pour un début
    float tAlpha_i [n];
    for (int i=0; i<n; i++)
        tAlpha_i[i]=0;
    float nu=0.5;
    
    for (int i=0; i<m; i++){
        
        //on va afficher les données utiles pour ce sous-Pb
        cout << endl;
        cout << "***  Noeud source : numéro " << i <<"   ***     "<<endl;
        affich_subPbData(tOffre[i], tCapacity[i], tCoutVar[i], tCoutFix[i], m, n); //voir la fonction dans mainfile_compliments.hpp

        
        //on résoud le sous-problème pour le noeud i
        ssPbSrc_i( i, tAlpha_i, nu,  tVal_NdSrc[i], tOffre[i],
              tCapacity[i], tCoutVar[i], tCoutFix[i], n, valObj);
        
        ValObjSrc[i]=valObj;
        
        makeSpace(); // mettre de l'espace dans l'affichage (voir mainfile_complements.hpp
    }
    
/* _____________   */
    
    
    
    

/* ***** résolution des sous_pb pour les noeuds destination  --------- */
    
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

         affich_subPbData(tDemand[j], t1, t2, t3, m, n); //voir la fonction dans mainfile_compliments.hpp
        
        //table contenant les multiplicateurs de lagrange
        float tAlpha_j [m];
        for (int i=0; i<m; i++)
            tAlpha_j[i]=0;

        //à présent on fait appel au ss_pb coté Dest
         ssPbDest_j( j, tAlpha_j,  nu, tVal_NdDest[j], tDemand[j], t1 ,t2, t3,  m, valObj); // voir file subPb.cpp
     
        ValObjDest[j]= valObj;
        
        makeSpace(); // on fait de l'espace dans l'affichage ( voir mainfile_compliments.hpp)
    }
    
/* ---------- fin subPBDest ---------- */
    
    
    
    
/*   **** calcul d une borne min de la solution à partir des resultats des sous problèmes.   */
    
    float borneMin =0.0;
    
    for (int i=0; i<m; i++){
        cout << ValObjSrc[i]<< " ";
        borneMin+= ValObjSrc[i];}
    cout << endl ;

    for (int j=0; j<n; j++){
        borneMin+= ValObjDest[j];
        cout << ValObjDest[j]<<"  ";
}
    
    cout <<endl;
    cout << "borne min : " <<  borneMin<<endl;
    
/* -----      --*/

 
 
    
 /* --- affichage des solutions des ssPb et comparaison  --- */
    
    cout << "tab x_ijˆs" << endl;
    for (int i=0; i<m; i++){
        for (int j=0; j<n; j++)
            cout<<tVal_NdSrc[i][j] << " ";
        cout << endl;
    }
    
    //    commencons par transposé la matrice des x_ijˆd afin de pouvoir comparer si les solutions sont égales pour       des i,j égaux des 2 cotés sources et destinations. la différence nous permettra d'obtenir le sous grandient
    
    // on va créer un nouveau tableau transposé pour y retranscrire les données.
    int ** tVal_NdDest2 = new int* [m];
    for (int i=0; i<m; i++)
        tVal_NdDest2[i]= new int [n];
    
    //construisons la transposée
    cout << endl;
    cout << "tab x_ijˆd" << endl;
    for (int i=0; i<m; i++){
        for (int j=0; j<n; j++)
            tVal_NdDest2[i][j]= tVal_NdDest[i][j];
    }
    
    //affichons la transposée
    for (int i=0; i<m; i++){
        for (int j=0; j<n; j++)
            cout<<tVal_NdDest2[i][j] << " ";
        cout << endl;
        
    }
    
    
    
    //    construisons la table des sous-gradient
    cout << endl;
    cout << "tab alpha_i_j" << endl;
    for (int i=0; i<m; i++){
        for (int j=0; j<n; j++)
            tabSubgradient[i][j]= tVal_NdDest2[i][j] - tVal_NdSrc[i][j];
    }
    
    //   affichons la table des sous-gradients
    for (int i=0; i<m; i++){
        for (int j=0; j<n; j++)
            cout<<tabSubgradient[i][j] << " ";
        cout << endl;
        
    }
    
    /*  -----   ----------  */
    
    
    
    
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
    
    
    
    for (int i = 0; i < m; ++i)
        delete [] tVal_NdSrc[i];
    delete [] tVal_NdSrc;
    
    for (int j = 0; j < n; ++j)
        delete [] tVal_NdDest[j];
    delete [] tVal_NdDest;
        
    for (int i = 0; i < m; ++i)
        delete [] tVal_NdDest2[i];
    delete [] tVal_NdDest2;
    
    for (int i = 0; i < m; ++i)
        delete [] tabSubgradient[i];
    delete [] tabSubgradient;
    
    delete [] ValObjSrc;
    delete [] ValObjDest;
    
    
    
    
    return 0;
}
