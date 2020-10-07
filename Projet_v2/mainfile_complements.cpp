//
//  mainfile_complements.cpp
//  Projet_v2
//
//  Created by JULIE AMANDA TCHOUANDEM KEMOE on 2020-09-27.
//  Copyright © 2020 JULIE AMANDA TCHOUANDEM KEMOE. All rights reserved.
//

#include "mainfile_complements.hpp"
#include <iostream>

using namespace std;

void affich_subPbData (  int nodeValue, int * t1, int * t2, int *t3, int m, int n){
    
    
    cout << endl;
    cout << "offre/Demande : " <<nodeValue <<endl;
    
    cout <<"table des capacités : " <<endl;
    for (int i=0; i<m ; i++)
        cout << t1[i] << " ";
    
    cout << endl;
    cout <<"table des CoutVar : " <<endl;
    for (int i=0; i<m; i++)
        cout << t2[i] << " ";
    
    cout << endl;
    cout <<"table des coutFix : " <<endl;
    for (int i=0; i<m; i++)
        cout << t3[i] << " ";
    cout << endl;
    
      //cout << " ---- ----    résolution du sous problème  ---- ----" << endl;
    cout << endl;

    
}

void makeSpace(){
    cout << endl;
    cout << endl;
    cout << endl;
}
