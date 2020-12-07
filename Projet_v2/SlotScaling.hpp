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


const int M=100 ;
const int nbIterations = 15;

//fonction qui implémente la procédure

int ** slotScale(int m, int n, int * tOffre_a, int * tDemand_b, int ** tCoutVar, int ** tCoutFix, int ** tCapacity, int ** state);

#endif /* SlotScaling_hpp */
