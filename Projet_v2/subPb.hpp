//
//  subPb.hpp
//  Projet_v2
//
//  Created by JULIE AMANDA TCHOUANDEM KEMOE on 2020-09-16.
//  Copyright © 2020 JULIE AMANDA TCHOUANDEM KEMOE. All rights reserved.
//

#ifndef subPb_hpp
#define subPb_hpp

#include <stdio.h>

//ici on va noter x_ijˆs = x_ij
//et x_ijˆd= w_ij

void ssPbSrc_i( int numSrc, float * tAlpha, float nu, int * ValX_ij, int Offre_i,
               int * val_U_ij, int * val_C_ij, int * val_F_ij, int n, float  & valObj);

/* numsrc : indice du noeud qu'on est entrain de traiter
 alpha, beta, nu : ce sont des valeurs qu'on va utiliser comme paramètre plus tard donc on va les garder
 alpha : multiplicateurs de lagrane
 beta : valeurs des pertubations 
 valX_ij : tableaux de n valeurs correspondant aux flots allant du noeuds i vers chaque noeud j
 Offre_i : c'est l'offre du noeud i
 val_u_ij : valeur des capacités sur les routes allant de i vers chacun des noeuds j
 val_C_ij : valeur des couts fixes pour acheminer du noeud i vers les différents noeud j
 val_F_if valeur des couts variables pour acheminer du noeud i vers les differents noeuds j
 */

void ssPbDest_j(int numDest, float * tAlpha,  float nu, int * ValW_ij, int Demand_j,
                int * val_U_ij, int * val_C_ij, int * val_F_ij, int m ,float & valObj);

#endif /* subPb_hpp */
