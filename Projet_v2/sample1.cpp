//
//  sample.cpp
//  ESSAIE1
//
//  Created by JULIE AMANDA TCHOUANDEM KEMOE on 2020-07-18.
//
#include "sample.hpp"
#include "projectParameters.h"
#include <iostream>
#include <string.h>
#include <fstream>


using namespace std ;



// recuperer la donnŽe ( data) qui se trouve   ˆ la position (pos) d'une ligne (line ) du fichier en lecture
int uploadData( string line, int pos,  int &posSuiv)
//apres il faudra se rassurer du type de line lors de l appel de la fonction
{
    int data;
    int i= pos ;
   // int tab[10] = { 0,1 , 2, 3,4, 5, 6, 7, 8, 9 };
    // int n = strlen(line);
    int n = line.length();
    
    char  s2= ' ' ;
    
    while( (i<n+1)  &&  ( line[i] == s2) )
        i+=1;
    
  //  char * temp = new char[4] ;// on reserve de l'espace mémoire pour recuperer les doées
    string a = "" ;//on va stocker ici la donnée ˆ recupérer
    
    while ( (i<n+1) && ( line[i] != s2)  )
    {
        a=a+line[i];
        i+=1;
    }
    posSuiv = i ;  // on garde la position suivante en memoire pour le prochain parcours
    data= stoi(a);  // on cast le caractère en int
    
    return data;
}




void get_M_N( int & m, int & n )
{
    int posSuiv =0  ;// on va s enservir pour suivre la lecture sur la ligne en lcture par possition
    //ouverture en lecture de monFichier( fichier encours de traitement declare ds sample.h
    ifstream file( monFichier, ios::in);
    if (file)
    {
        
        if(datasetGroup==2)
        {
            string line1;
            getline(file, line1);
            m = uploadData(line1, 0,  posSuiv);
            n = uploadData(line1, posSuiv, posSuiv);
        }
        else if (datasetGroup==3)
        {
            string line1;
            string line2;
            getline(file, line1);
            m = uploadData(line1, 0,  posSuiv);
            getline(file, line2);
            n = uploadData(line2, 0,  posSuiv);
        }
        file.close();
    }
    else
        cout << "imposible de lire le fichier ! "<< endl;
}

//cette fonction permet de recuperer les donnees d'un fichier appartenant au dataset2
//et de les stocker dans des structures de données pour les utiliser


void readInput_Dataset2( int &m , int &n, int * tOffre_a, int * tDemand_b, int ** tCoutVar, int ** tCoutFix )
{
    int pos= 0;
    //on complete la construction des pointeurs déclarés dans le fichier d'entete
  /*  for (int i = 0; i < m; ++i)
        tCoutFix[i] = new int[n];
    
    for (int i = 0; i < m; ++i)
        tCoutVar[i] = new int[n];*/
    
    //ouverture en lecture de monFichier( fichier encours de traitement declare ds sample.h
    ifstream file( monFichier, ios::in);
    if (file)
    {
        
        string l_offre ; //on va stocker ici la ligne ayant la quantité d'offre de chaque sommet destination
        string  l_demand ; //on va stocker ici la ligne ayant la quantité de demande de chaque sommet destination
        string l_trash; // on va utiliser ceci pour ecraser les lignes vides
        
        
        getline(file, l_trash);// on va ecraser la 1ere ligne car on a deja recuperer m et n

        // recuperons chaque offre et stockons dans le tableau d'offre pour chaque sommet
        getline(file, l_offre);
        for(int i=0; i<n ; i++)
        {
            // pos = 0;//indique le premier caractère de la ligne

            *(tOffre_a +i) = uploadData(l_offre, pos,  pos);
        }
        
        // recuperons chaque demande et stockons dans le tableau de demande pour chaque sommet
        getline(file, l_demand);
        pos =0;// on reinitialise la position initiale à 0
        for(int i=0; i<n ; i++)
        {
             //pos=0;
            *( tDemand_b + i) = uploadData(l_demand, pos,  pos);
        }
        
        //on va maintenant lire les coûtss fixes et stocker dans un tableau
        for (int i=0; i<m ; i++ )
        {
             pos=0;
            string lineCF; //lire successivement les lignes contenant les couts fixes, recuperer les doées et stocker
            getline(file, lineCF);
            /// *(*(tCoutFix + i))
          tCoutFix[i][0] = uploadData(lineCF, pos, pos);
            for (int j=1; j<n; j++)
                // *(*(tCoutFix + i) +j )
                tCoutFix[i][j] = uploadData(lineCF, pos,  pos);
        }
        
        getline(file, l_trash);
        
        
        
        for (int i=0; i<m ; i++ )
        {
             pos=0;
            string lineCV ;
            getline(file, lineCV);
            tCoutVar[i][0]  = uploadData(lineCV, pos, pos);
            for (int j=1; j<n; j++)
                tCoutVar[i][j]  = uploadData(lineCV, pos,  pos);
        }
        
        file.close();
        
    }
    else
        cout<<"impossible de lire le fichier ! "<< endl;
    
    
}
//{}

void readInput_Dataset3( int &m , int &n, int * tOffre_a, int * tDemand_b, int ** tCoutVar, int ** tCoutFix )
{
    
    int pos=0;
    //on complete la construction des pointeurs déclarés dans le fichier d'entete
    /*for (int i = 0; i < m; ++i)
        tCoutFix[i] = new int[n];
    
    for (int i = 0; i < m; ++i)
        tCoutVar[i] = new int[n];*/
    
    
    //ouverture en lecture de monFichier( fichier encours de traitement declare ds sample.h
    ifstream file( monFichier, ios::in);
    if (file)
    {
        
        string l_offre ; //on va stocker ici la ligne ayant la quantité d'offre de chaque sommet destination
        string  l_demand ; //on va stocker ici la ligne ayant la quantité de demande de chaque sommet destination
        string l_trash; // on va utiliser ceci pour ecraser les lignes vides
        
        
        getline(file, l_trash);// on va ecraser la 1ere ligne car on a deja recuperer m
        getline(file, l_trash);// on va ecraser la 2 eme ligne car on a deja recuperer n
        getline(file, l_trash);// on va ecraser la 3eme ligne qui est vide


        
        // recuperons chaque offre et stockons dans le tableau d'offre pour chaque sommet
        
        for(int i=0; i<n ; i++)
        {
           
            getline(file, l_offre);
            *(tOffre_a +i) = uploadData(l_offre, pos,  pos);
             pos= 0;
        }
        
        getline(file, l_trash);// suppression d'une ligne vide

        
        // recuperons chaque demande et stockons dans le tableau de demande pour chaque sommet
        for(int i=0; i<n ; i++)
        {
           
            getline(file, l_demand);
            *( tDemand_b + i) = uploadData(l_demand, pos,  pos);
             pos= 0;
        }
        
        getline(file, l_trash);// suppression d'une ligne vide

         int a, b;
        //on va maintenant lire les coûtss fixes et stocker dans un tableau
        for (int i=0; i<m*n ; i++ )
        {
           int pos =0;
            string lineC; //lire successivement les lignes contenant les couts fixes et variables, recuperer les doées et stocker
            getline(file, lineC);
            a = uploadData(lineC, 0, pos);
            b = uploadData(lineC, pos, pos);
            *(*(tCoutVar + a) +b)= uploadData(lineC, pos, pos);
            *(*(tCoutFix + a) +b ) = uploadData(lineC, pos,  pos);
        }
        
        getline(file, l_trash);
        
        file.close();
        
    }
    else
        cout<<"impossible de lire le fichier ! "<< endl;
    
}


void BuiltCapacity(int m ,int n, int tOffre[], int tDemand[], int ** tCapacity)
{
    //onn termine la construction du pointeur déclaré dans sample.h
  /*  for (int i = 0; i < m; ++i)
        tCapacityFix[i] = new int[n];*/
    
    for (int i=0 ; i<m; i++)
    {
        for(int j=0; j<n; j++)
        { if (tOffre[i] <= tDemand[j])
                tCapacity[i][j]=tOffre[i];
            else
                  tCapacity[i][j]=tDemand[j];
    }
    }
    
    
    
}

