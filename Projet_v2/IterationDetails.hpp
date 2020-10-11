//
//  IterationDetails.hpp
//  Projet_v2
//
//  Created by JULIE AMANDA TCHOUANDEM KEMOE on 2020-10-01.
//  Copyright © 2020 JULIE AMANDA TCHOUANDEM KEMOE. All rights reserved.
//

// ceci est une classe dans laquelle on va stocquer tous les informations liées à une itération ... les solutions, la valeur de l'objectif, les resultats de la méthode de sous-gradient ...


#ifndef IterationDetails_hpp
#define IterationDetails_hpp

#include <iostream>

#include <stdio.h>

using namespace std;

class  IterationDetails {
    public :
    //IterationDetails();
    
    // constructeur qui nous permettra d'instancier une classe
    IterationDetails(int m, int n);
    
    //methode pour liberer la mémoire dynamique
    void freeIterationDetails(int m, int n);
    
    //methode qui nous permettra de faire la mise à jour quand on passera d'une iteration à une autre
    void updateInstance(int m, int n, IterationDetails Instance);
    //on va l'utiliser pour mettre à jour les donnés de l'instance précedente entre 2 iteratons
    
    void calculBorneMin(int m, int n );
    // tValsrc et tValDest sont les valeurs de l'objectif du coté source et du coté destination respectivement.
    void calcul_subGradient ( int m, int n);
    void calculPAs( int m, int n, float bInf_ItPrec );
    void calculMultipliyers_t(int m, int n,  float ** tMultiplyers_prec  );
    // calcul des multiplicateurs de lagrange à l'itération t
    // on lui donne la table des multiplicateurs à l'étape précédente Multiplyers_t_1
    //tabSugradient : tableau des sous-gradients à l'itération courante
    // T_alpha est la table des multiplicateurs de lagrange

    
    
    float bMin ;
    float bMax ;
    float * valObjSrc;
    float * valObjDest;
    int ** tX_ij;
    int ** tW_ij;
    int ** tSubGrad;
    float ** multiplyers;
    float pas;
    float lambda; // facteur utilisé dans la formule du calcul du pas 
    
    
};

#endif /* IterationDetails_hpp */

