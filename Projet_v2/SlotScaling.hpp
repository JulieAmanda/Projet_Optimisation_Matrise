//
//  SlotScaling.hpp
//  Projet_v2
//
//  Created by JULIE AMANDA TCHOUANDEM KEMOE on 2020-12-05.
//  Copyright © 2020 JULIE AMANDA TCHOUANDEM KEMOE. All rights reserved.
//

#ifndef SlotScaling_hpp
#define SlotScaling_hpp


#include <stdio.h>


const int M=50 ;
const int nbScalingIterat = 10;
const int Mpost=10000000; // cout des arcs à fermer pour la postoptimisation après le slot scaling



//fonction qui implémente la procédure

int flotProblem(int m, int n, int * tOffre_a, int * tDemand_b, int ** tCapacity,  int ** tSolution, int ** tabCost, int postOptim);

void costUpdate ( int m, int n, int iterationNb, int ** state, int **tabSol,  int ** tCoutVar, int ** tCoutFix, int ** tCapacity,  int ** tabCost );

//void slopeSolution(int ** tabSol, int ** tCoutFix, int ** tCoutVar, int m, int n, int Bornsup, int ** bestSol, int & bestVal);
float slopeRealValue(int m, int n, int ** tabSol, int ** tCoutFix, int ** tCoutVar);

void costPostOptim(int m, int n, int **tabSol,  int ** tCoutVar, int ** tabCost);

void slopeScaleMethod( int m, int n, int ** tabSolScaling, float & bornSup,  int ** tCoutFix, int ** tCoutVar, int ** tCapacity, int * tOffre, int * tDemand, int ** state );

#endif /* SlotScaling_hpp */
