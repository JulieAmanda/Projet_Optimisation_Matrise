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
float ModelBase_Bsup( int m , int n, int * tOffre_a, int * tDemand_b, int ** tCoutVar, int ** tCoutFix, int ** tCapacity, bool ignore , int ** state, float bestBornSup, int ** tabBestSol, int ** historiqY_ij, int nbCallHrstq);

//à la toute fin de notre processus de résolution, on va appliquer à nouveau une post optimisation, on va résoudre le problème global de base par cplex en lui précisant la meilleure solution existante ainsi que un ensemble d'arcs à fermer
float lastPostOptim( int m , int n, int * tOffre_a, int * tDemand_b, int ** tCoutVar, int ** tCoutFix, int ** tCapacity, float bestBornSup, int ** tabBestSol, int ** historiqY_ij,  int nbCallHrstq);


//cette fonction nous permettra d'extraire les arcs qui sont généralement fermés parmis toutes les solutions réslisable que nous avons trouvées avec l'heuristique lagrangienne, afin d'en extraire les arcs fermés à passer à cplex pour la dernière postoptimisation passer 
void currentClosedArcs (int ** historiqY_ij, int ** stateY_ij, int nbCallHrstq, int m, int n);

//modèle de base global qu'on va résoudre par cplex en laissant cplex utiliser ses propres méthodes, sans rien ajouter. Aucun paramètre n'est fixé sauf la limite de temps et la limite de processeur utilisé 1
float globalModel(int m, int n, int * tOffre_a, int * tDemand_b, int ** tCoutVar, int ** tCoutFix, int ** tCapacity, int timelimit);

#endif /* globalModel_hpp */
