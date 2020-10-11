//
//  IterationDetails.cpp
//  Projet_v2
//
//  Created by JULIE AMANDA TCHOUANDEM KEMOE on 2020-10-01.
//  Copyright © 2020 JULIE AMANDA TCHOUANDEM KEMOE. All rights reserved.
//


// ceci est une classe dans laquelle on va stocquer tous les informations liées à une itération ... les solutions, les resultats de la méthode de sous-gradient ...
#include "IterationDetails.hpp"
#include <math.h>


IterationDetails::IterationDetails(int m, int n){
    
    this->bMin=0;
    this->bMax=100000;
    this->pas=0;
    this->lambda=1; // paramètre utilisé dans la formule de calcul du pas
    
    //on initialise les valeurs des objectifs à 0, les vals seront mises à jour au fil de l'exécution des ssPbs
   /* for (int i=0; i<m; i++)
        ValObjSrc[i]=0;
    for (int j=0; j<n; j++)
        ValObjDest[j]=0;*/
    
    this->valObjDest= new float[n];
    this->valObjSrc = new float[m];
    
    this->tX_ij = new int*[m];//ici on va stocker les valeurs des flots des noeuds sources
    for (int i = 0; i < m; ++i)
        this->tX_ij[i] = new int[n];

    this->tW_ij= new int* [n];  //ici on va stocker les valeurs des flots des noeuds destinations
    for (int j = 0; j < n; ++j)
        this->tW_ij[j] = new int[m];

    this->tSubGrad= new int* [n];  //table qui va contenir les sous-gradient après resolution des sspb
    for (int j = 0; j < n; ++j)
        this->tSubGrad[j] = new int[m];

    this->multiplyers= new float* [n];  //table qui va contenir les sous-gradient après resolution des sspb
    for (int j = 0; j < n; ++j)
    this->multiplyers[j] = new float[m];
    
    
    
    
    for (int i=0; i<m; i++){
        for (int j=0; j< n; j++){
            this->multiplyers[i][j]=0;
            this->tW_ij[i][j]=0;
            this->tX_ij[i][j]=0;
            this->tSubGrad[i][j]=0;
        }
    }
    
    for (int i=0; i<m; i++)
        this->valObjDest[i]=0;
    for (int j=0; j< n; j++)
        this->valObjSrc[j]=0;
    
    
    
}


void IterationDetails::calculBorneMin(int m, int n ){
    
    for (int i=0; i<m; i++){
        cout << this->valObjSrc[i] << " "; //affichage des solutions du coté source
        this->bMin += this->valObjSrc[i];}
    cout << endl ;
    
    for (int j=0; j<n; j++){
        this->bMin += this->valObjDest[j];
        cout << this->valObjDest[j] <<"  ";// affichage des solution coté destination
    }
  
}


//----------------

void IterationDetails::calcul_subGradient ( int m, int n){
    
    //    commencons par transposer la matrice des x_ijˆd afin de pouvoir comparer si les solutions sont égales pourdes i,j égaux des 2 cotés sources et destinations. la différence nous permettra d'obtenir le sous grandient plus tard
    
    // on va créer un nouveau tableau transposé pour y retranscrire les données.
    int ** tVal_NdDest2 = new int* [m];
    for (int i=0; i<m; i++)
        tVal_NdDest2[i]= new int [n];
    //construisons la transposée
    for (int i=0; i<m; i++){
        for (int j=0; j<n; j++)
            tVal_NdDest2[i][j] = this->tW_ij[i][j];}
    
    //calculons les ss-grad
    for (int i=0; i<m; i++){
        for (int j=0; j<n; j++)
            this->tSubGrad [i][j]= this->tW_ij[i][j] - this->tX_ij[i][j];
    }
    
    for (int i = 0; i < m; ++i)
        delete [] tVal_NdDest2[i];
    delete [] tVal_NdDest2;
    
}

//---------------

void IterationDetails::calculPAs( int m, int n, float bInf_ItPrec ){
    
    // Commençons par calculer la norme de la matrice sous-gragient
    float norm = 0;
    
    for (int i=0; i<m; i++){
        for (int j=0; j< n; j++)
            norm += pow(this->tSubGrad[i][j],2);
    }
   
    this->pas= (this->lambda *( this->bMax - bInf_ItPrec))/norm;
    cout<<endl;
    cout<< "la borne max est"<< this->bMax<< "la borne min est"<<this->bMin<< "la norm est"<< norm << endl;
    
}




// --------

void IterationDetails::calculMultipliyers_t(int m, int n,  float ** tMultiplyers_prec  ){
    
   
    int j=0;
    int go=n-1;
    while(j<n-1 ) {
        for (int i=0; i<go ; i++)
            this->multiplyers[i][j]=tMultiplyers_prec[i][j] + (this->pas * (this->tSubGrad[i][j]));
        go --;
        j ++;
    }
    int count=0;
    for(int i=m-1; i==0; i--){
        for (int j=count; j<n; j++){
            this->multiplyers[i][j]=tMultiplyers_prec[i][j] - (this->pas * (this->tSubGrad[i][j]));
        }
        count +=1;
    }
    
}

// ----------


/*
 
 for (int i=0; i<m; i++){
 for (int j=0; j< n; j++)
 this->multiplyers[i][j]=tMultiplyers_prec[i][j] - (this->pas * (this->tSubGrad[i][j]));
 }
 
 
 */


//on va mettre à jour les valeurs de l'iteratin precédente par les solutions en cours pour passer à une autre iteration
void IterationDetails::updateInstance(int m, int n,  IterationDetails Instance){
    
    
    this->bMax=Instance.bMax;
    this->bMin= Instance.bMin;
    this->pas=Instance.pas;
    this->lambda=Instance.lambda;
    
    for (int i=0; i<m; i++){
        for (int j=0; j< n; j++){
            this->multiplyers[i][j]=Instance.multiplyers[i][j];
            this->tW_ij[i][j]=Instance.tW_ij[i][j];
            this->tX_ij[i][j]=Instance.tX_ij[i][j];
            this->tSubGrad[i][j]=Instance.tSubGrad[i][j];
        }
    }
    
    for (int i=0; i<m; i++)
        this->valObjDest=Instance.valObjDest;
    for (int j=0; j< n; j++)
        this->valObjSrc=Instance.valObjSrc;
        
    
    
}

void IterationDetails::freeIterationDetails(int m, int n){
    
    for (int i = 0; i < m; ++i)
        delete [] this->tSubGrad[i];
    delete [] this->tSubGrad;
    
    for (int j = 0; j < n; ++j)
        delete [] this->tX_ij[j];
    delete [] this->tX_ij;
    
    for (int j = 0; j < n; ++j)
        delete [] this->tW_ij[j];
    delete [] this->tW_ij;
    
    for (int j = 0; j < n; ++j)
        delete [] this->multiplyers[j];
    delete [] this->multiplyers;
    
    delete [] this->valObjDest;
    delete [] this->valObjSrc;

    

    
}
