//
//  globalModel.hpp
//  Projet_v2
//
//  Created by JULIE AMANDA TCHOUANDEM KEMOE on 2020-09-28.
//  Copyright © 2020 JULIE AMANDA TCHOUANDEM KEMOE. All rights reserved.
//

#ifndef globalModel_hpp
#define globalModel_hpp

#include <stdio.h>

#include "samples.hpp"

#include <stdio.h>

//ici, on a défini le modèle dans sa forme originale, on l'utilisera pour calculer une borne supérieure au problème. en précisant à cplex de s'arreter juste à trouver une solution réalisable à la racine


float ModelBase_Bsup( int m , int n, int * tOffre_a, int * tDemand_b, int ** tCoutVar, int ** tCoutFix, int ** tCapacity, bool ignore , int ** state);


#endif /* globalModel_hpp */
