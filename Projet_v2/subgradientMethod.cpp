//
//  subgradientMethod.cpp
//  Projet_v2
//
//  Created by JULIE AMANDA TCHOUANDEM KEMOE on 2020-09-29.
//  Copyright © 2020 JULIE AMANDA TCHOUANDEM KEMOE. All rights reserved.
//

#include<iostream>
#include <math.h>
#include "subgradientMethod.hpp"
#include "globalModel.hpp"
#include "subPb.hpp"
#include "IterationDetails.hpp"
#include "heuristiqBSup.hpp"
#include "SlotScaling.hpp"

using namespace std;


void affichTabSolutions ( int m, int n , int ** tab){
    
    for (int i=0; i<m; i++){
        for (int j=0; j<n; j++)
            cout<<tab[i][j] << " ";
        cout << endl ;
    }
}


// foction qui nous permettra de verifier le changement de la borne inférieure aucours de nb itérations successives.
bool lowerBndVariation(int nbIterationsMax, bool & valueUpdated, float & optimal_lwrBnd, float lowerBound, int & count){
    
    float stopVerify = false; // permet de savoir si on a atteind le nombre max d itérations ou pas
    
    if ( optimal_lwrBnd>=lowerBound){ // si il n'y a pas eu amélioration de la borne optimale
        count +=1;   //on compte une itération de plus sans amélioration
        }
    else {//si on obtient une meilleur borne inférieur
        optimal_lwrBnd=lowerBound;   //on met à jour la valeur de la meilleur borne inférieur
        valueUpdated=true;             //on enregistre qu'il y a eu amélioration
        count=0;                    // on reinitialise le décompte
        }
    
    if (count == nbIterationsMax) //si on a atteint le nombre max à vérifier on arrête
    { stopVerify=true;              //on arrete de surveiller l'amélioration
        count=0;                   //on recommence de surveiller l'amélioration à 0
    }
    
    // dans l'algo principal, si le valueUpdated sont true alors on va recommencer le count. si le stopVerfy est true faire les controles que cela implique pour la suite de l'algo de sous gradient
    return stopVerify;
    
}


//---------- implémentation de l'algorithme du sous-gradient --------//

IterationDetails Subgradient( int m, int n, int * tOffre, int * tDemand, int ** tCoutVar, int ** tCoutFix, int ** tCapacity, int * tabDistAleatr , int & nbItSS, int *** tabBestSol){
    
    
    // on crée deux var booléenes nous permettrons de verifier si la borne min change au fil de a ( repectivement b) itérations
    //on considère a = omga_2 et b=omga_3
    bool bMinUpdated_a_It= false;//on va mettre c'est deux en param ds la fctn lwrBndVariation
    bool bMinUpdated_b_It = false;
    
    // on crée 2 var bool pour controler si on est arrivé à un nombre max voulu d'itérations
    bool stopVerify_a_It = false; // nous permettra de savoir si on met à jour le parametre lambda ds le clcul du pas(apres a=omga_2 repetition)
    bool stopVerify_b_It = false ; //controle si on arrête l'algo ( apres b=omga_3 iterations)
    int count_a = 0;
    int count_b = 0; // ce seront les compteurs pour verifier le nb d'iterations successives
    
    float meilleur_bInf_a = 0;//meilleur b_Min enregistré sur un interval de a iterations
    float meilleur_bInf_b = 0;//meilleur b_Min enregistré sur un interval de b iterations

    
    int iteratnNumber =0;
    
    
    //on va fixer les multiplicateurs de lagrange à 0 pour la première iteration à 0 pour un début
   
    //initialisons nu (utilisée dans la formule de calcul des multiplcateurs de lagrange)
    float nu=0.5;
    
    //on initialise les données de ;l'algo de sous-gradient à 0 pour l'iteration 1 en créant une nouvelle instance d'itération
    IterationDetails I_prec(m, n);
    
    
/*---  initialisation des éléments pour l'heuristique d'intercalage de la borne sup*/
    bool ignore =true ;
    
    int sizeTab= n*m; // la taille max de state et historiqSol sera le nombre d'iterations max divisé par le nbre d'itérations apres quoi on appelle l'heuristiqer
    
    int nbCallHrstq = 0;//ON VA utiliser pour compter le nbre de fois que l'heuristique lagrangienne a déjà été appelé
    
     int lenghtTab= omga_4/callHrstq; //la taille max du tableau qui nous permettra de sauvegarder les bornes supérieures trouvées au cours de l'algo du sous-gradient chaque fois qu'on appelle l'heuristique lagrangienne.
    
    
    //nous construisons la table qui nous permettra de sauvegarder les valeurs x_ij de la meilleure solution obtenue tout au long de l'algorithme.
    int ** bestSol = new int * [m];
    for (int i=0; i<m;i++)
        bestSol[i]= new int [n];
    
    //on va à présent créer une table qui nous permettra d'enregistrer tout l'historique des valeurs des y_ij au cours des recherches de la meilleures borne supérieure. Pour chaque solution réalisable correspondant à une borneSup, on va enregistrer l'etat de chaque arc en sauvegardant les valeurs des y_ij. Cela nous permettra à la fin de constater les arcs qui sont toujours fermés ds ttes les solutions
    int ** historiqY_ij = new int * [lenghtTab];
    for (int i=0; i<lenghtTab;i++)
        historiqY_ij[i]= new int [sizeTab];
    
    
    int ** state= new int* [2]; //on va stocker ici l'etat de la solution obtenue
    // 1 ligne pour la position à laquelle le sous-grad=0 et l'autre pour la valeur de yij à cette position
    state[0]=new int[sizeTab];
    state[1]=new int[sizeTab];
    
    int * historiqSol =new int [sizeTab];  // permettra de sauvegarder au fur et à mesure des clés representant les solutions obtenues à différentes itérations.
    
    historiqSol[0] = 0 ; //on va reserver la première case pour stocker le nombre de clé qu'on a déjà obtenue et on va incrementer chaque fois qu'il y aura une nouvelle clé
    
    
    for (int i=0; i<sizeTab; i++){
        state[0][i]=0;
        state[1][i]=-1; // ON INITIALISE TOUT À -1 car au début l'état de l'arc est libre on a pas encore fait de fixation sur yij on a donc yij=-1
        //on aura state[i][j]=y_ij=1 si on fixe l'arc comme obligatoirement ouvert et ayant du flot et state[i][j]=y_ij=0 si on ferme l'arc pour l'obliger à ne pas faire passer du flot
        historiqSol[i]=0;
     
    }

    int countHrstq = 0; // va nous servir de compteur pour savoir à quel moment appeler l'heursitique lagrangienne pour la borne sup..il y a un appel toutes les 20 itération successives
    
    
    // on va recueillir ici les solutions de la procédure slope scaling
    int ** tabSolScaling = new int * [m];
    for (int i=0; i<m ;i++){
        tabSolScaling[i]=new int [n];
        for (int j=0; j<n; j++)
            tabSolScaling[i][j]=0;
    }

    
    
    
    int  * tabBornsup = new int [sizeTab];
    
   /* ----       -------*/
    
   //on initialise la borne sup par la solution du problème global de base à la racine de CPLEX
    float BestBornSup = 1000000;
  BestBornSup= ModelBase_Bsup( m , n, tOffre, tDemand, tCoutVar, tCoutFix, tCapacity, ignore, state,  BestBornSup, bestSol, historiqY_ij, nbCallHrstq);//cette méthode permet de calculer une nouvelle solution réalisable du problème (P) (en considérant les informations de state et ignore. Si elle trouve une borne sup meilleure que la courante, elle va mettre à jour bestsol et bestbornsup et à chaque appel il met à jour historiqY_ij.
    historiqSol[1]= BestBornSup ; // on va garder cette premiere borne sup dans l'historique
    historiqSol[0]=1; // on met à jour la taille du tableau
    
    
    
    //tant qu'on a pas atteint le nbre max d'iteration et que stopverify_b_it = false( la valeur optimale ne s'est pas encore repete b= omga_3 fois) on continue l'algo
    while ((I_prec.bMin != I_prec.bMax) && (iteratnNumber < omga_4) && (stopVerify_b_It == false)){
        
        //on va stocker ici les données de l'itération courante
        IterationDetails I_cour(m, n);
       
        /*  résolution des sous problèmes pour les noeuds sources */

        float valObj =0; //variable temporaire dans laquelle on va recupérer la solution des sous-problèmes au fil des itérations.
        
        for (int i=0; i<m; i++){
            
            //on va afficher les données utiles pour ce sous-Pb (pour améliorer la lecture des resultats)
            cout << endl;
            cout << "***  Noeud source : numéro " << i <<"   ***     "<<endl;
            
            //pour afficher les couts fixes/variables, capacité et demande lié au subpb
            //affich_subPbData(tOffre[i], tCapacity[i], tCoutVar[i], tCoutFix[i], m, n); //voir la fonction dans mainfile_compliments.hpp
            
            //o la tables des multiplicateurs de lagrange à 0 à la toute première itération
            float tAlpha_i [n];
            for (int j=0; j<n; j++)
                tAlpha_i[j]=I_prec.multiplyers[i][j];
            
            //on résoud le sous-problème pour le noeud i
            ssPbSrc_i( i, tAlpha_i, nu,  I_cour.tX_ij[i], tOffre[i],
                      tCapacity[i], tCoutVar[i], tCoutFix[i], n, valObj);
            I_cour.valObjSrc[i]=valObj;
            
            makeSpace(); // mettre de l'espace dans l'affichage (voir mainfile_complements.hpp
            }
        
        //résolution des sous_pb pour les noeuds destination  --------- */
        
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
            
            
            //pour afficher les couts fixes/variables, capacité et demande lié au subpb
            //affich_subPbData(tDemand[j], t1, t2, t3, m, n); //voir la fonction dans mainfile_compliments.hpp
            
            //table contenant les multiplicateurs de lagrange
            float tAlpha_j [m];
            for (int i=0; i<m; i++)
                tAlpha_j[i]=I_prec.multiplyers[i][j];
            //on prend les multiplicateurs de la destination j
            
            int * tab= new int[m]; //on va stocker ici nos valeurs des variables w_j[i] i de 1 à m de façon temporaire
            
            //à présent on fait appel au ss_pb coté Dest
            ssPbDest_j( j, tAlpha_j,  nu, tab, tDemand[j], t1 ,t2, t3,  m, valObj); // voir file subPb.cpp
            I_cour.valObjDest[j]= valObj;
            for (int i=0; i<m ;i++)
                I_cour.tW_ij[i][j]=tab[i];
            
            delete [] tab;
            
            makeSpace(); // on fait de l'espace dans l'affichage ( voir mainfile_compliments.hpp)
        }
        
        //calcul des sous-gradient et de la borneMin de l'objectif
        
        I_cour.calcul_subGradient(m, n);
        I_cour.calculBorneMin(m, n); // calcul borne Min -- voir subgradientMethod.cpp
        
        makeSpace(); // espace entre les paragraphes à l'affichage
        cout << " ***   borne min : " <<  I_cour.bMin<<"   ***  "<<endl;// val de borne minimale
        makeSpace(); // espace entre les paragraphes à l'affichage
        
        cout << "resolvons  le problème global à la racine pour obtenir une borne supérieure ..." "l'heuristique est inclue "<< endl;
        //  calcul de la borne sup -- voir globalModel.cpp
        
        countHrstq += 1 ;
        
        if (countHrstq == callHrstq){
            
        
            ignore= heuristique(tabDistAleatr, I_cour.tW_ij, I_cour.tX_ij,  m,  n, historiqSol, state); // cette heuristique va nous permettre de se faire une idée du type de la solution ( les arcs ouverts/fermés/libres. Aussi on va créer une clé unique correspondant à la solution. et nous renvoyer un booleen qui indique false si cette solution n'avait pas encore ete trouvée
            
           
            cout<<"go"<<endl;
            if (ignore==false){ //si la clée trouvée par l'heuristique n'avait pas encore été trouvée passée on va appliquer la procédure du slot scaling. Dans le cas contraire, il n'yaura pas de différence de résultat donc on va juste recommencer le décompte de 20 itérations pour essayer à nouveau de calculer une bSup
                

                float bornsupSS;
                
                //on execute la procédure de slope scaling intégrée à l'heuristique lagrangienne
                slopeScaleMethod(m, n, tabSolScaling, bornsupSS,  tCoutFix, tCoutVar, tCapacity, tOffre, tDemand, state, nbItSS, tabBestSol);
                cout<< "borne sup avec slope scale "<< bornsupSS;
                cout << endl;
              
                
                /**** calcul de la bsup correspondante après Slope scaling : stratégie T2  ****/
                
               
                //avec la solution issue du slope scaling, on va calculer une nvelle  la borne supérieure en resolvant le probleme global restreint par la fermeture des arcs dont state=0
                float bornSup = ModelBase_Bsup( m , n, tOffre, tDemand, tCoutVar, tCoutFix, tCapacity,ignore, state,BestBornSup, bestSol, historiqY_ij, nbCallHrstq);

                cout<<"borne sup évaluée d'après le modèle global est : "<< bornSup ;
                cout << endl;

                //on incrémente le décompte de l'appel de l'heuristique agrangienne
                nbCallHrstq +=1 ;
                
//on va repertotier l'ensemble des bornes sup obtenues tout au long
                            int a= historiqSol[0]-1;
                            tabBornsup[a]=bornSup;
//                             if (bornSup<BestBornSup)
//                             BestBornSup=bornSup;

              
                  /**** calcul de la bsup après Slope scaling : stratégie T1  ****/
//                int a= historiqSol[0]-1;
//                tabBornsup[a]=bornsupSS;
//                if (bornsupSS<BestBornSup)
//                    BestBornSup=bornsupSS;

              
                
            }
            countHrstq=0; //on reinitialise la borne sup
        }
        
        I_cour.bMax=BestBornSup;
        
        makeSpace(); // espace entre les paragraphes  à l'affichage
        cout << " ***    borne sup : " <<  I_cour.bMax <<"   ***  "<<endl;
        
        
        //on incremente le compteur d'iteration
        iteratnNumber = iteratnNumber + 1;
       
        
        
        //on va maintenant faire le test de variation de la borne inférieure du problème
        stopVerify_a_It=lowerBndVariation(omga_2, bMinUpdated_a_It, meilleur_bInf_a, I_cour.bMin, count_a);
        stopVerify_b_It=lowerBndVariation(omga_3, bMinUpdated_b_It, meilleur_bInf_b, I_cour.bMin, count_b);
        
        //testons s'il y a eu des variations pour effectuer les controls neccessaires
        //après omga_2=15 iterations sans changement de bMin on met à jour lamba
        if (stopVerify_a_It==true){
            I_cour.lambda = I_prec.lambda/omga_1;
            stopVerify_a_It=false;
        }
        else{
            I_cour.lambda = I_prec.lambda;
        }
        
        
        
        //on va calculer le pas et les multiplicateurs de lagrange etant donné qu'on a déjà tous les elements necessaires
    
        cout<< endl;
        cout << "iteration numero :" << iteratnNumber;
        cout << endl;
        
        I_cour.calculPAs(m, n, I_prec.bMin);
        I_cour.calculMultipliyers_t(m, n, I_prec.multiplyers);
        
        cout << endl;
        cout << "le pas est : " << I_cour.pas << endl;
        
        cout << endl;
        cout << "lambda est : " << I_cour.lambda << endl;
        
        cout << endl;
        cout << "tab multiplyers it prec" << endl;
        for (int i=0; i<m; i++){
            for (int j=0; j<n; j++)
                cout<<I_prec.multiplyers[i][j] << " ";
            cout << endl ;
        } //   affichons la table des sous-gradients
        
        
        cout << endl;
        cout << "tab multiplyers" << endl;
        for (int i=0; i<m; i++){
            for (int j=0; j<n; j++)
                cout<<I_cour.multiplyers[i][j] << " ";
            cout << endl ;
        } //   affichons la table des sous-gradients
        
        
        // --- affichage des solutions des ssPb et comparaison
        
        makeSpace();
        cout << "solutions et valeurs de l'instance précédente" << endl;

        cout << "tab x_ijˆs" << endl;
        affichTabSolutions(m, n, I_prec.tX_ij);//voir subpb.cpp
        
        cout << endl;
        cout << "tab x_ijˆd" << endl;
        affichTabSolutions(m,n, I_prec.tW_ij );  //affichons la transposée -- voir fonction dans subgradient.cpp
        
        cout << endl;
        cout << "tab alpha_i_j" << endl;
        affichTabSolutions(m,n,I_prec.tSubGrad ); //   affichons la table des sous-gradients
        
       
      
        makeSpace(); //espace à l'affichage (voir subpb.cpp)
        cout << "solutions et valeurs de l'instance courante" << endl;

        // --- affichage des solutions des ssPb et comparaison
        
        cout << "tab x_ijˆs" << endl;
        affichTabSolutions(m, n, I_cour.tX_ij);
        
        cout << endl;
        cout << "tab x_ijˆd" << endl;
        affichTabSolutions(m,n, I_cour.tW_ij );  //affichons la transposée -- voir fonction dans subgradient.cpp
        
        cout << endl;
        cout << "tab alpha_i_j" << endl;
        affichTabSolutions(m,n,I_cour.tSubGrad ); //   affichons la table des sous-gradients
        

        
        I_prec.updateInstance(m, n, I_cour);
        
        
    }
    
    cout << "le nombre d'iterations est :  " << iteratnNumber << endl;
    cout << "la meilleure borne inf de 15 est  :  " << meilleur_bInf_a << endl;
    cout << "la meilleure borne inf de 30 est  :  " << meilleur_bInf_b << endl;
    cout << "la meilleure borne supest  :  " << BestBornSup << endl;
    
    for (int i=0; i<historiqSol[0]; i++)
         cout<< tabBornsup[i] << "   " ;
    

    
    //  ***  on libere les pointeurs qui ont ete utiliser pour stocker les données
    
    for (int i=0; i<m; i++){
            delete [] tabSolScaling[i];
            delete [] bestSol[i];
    
    }
    for (int i=0; i<lenghtTab; i++){
        delete [] historiqY_ij[i];
    }
    
        delete [] tabSolScaling;
        delete [] tabBornsup;
        delete [] historiqY_ij;
        delete [] historiqSol;
        delete [] state[0];
        delete [] state[1];
        delete [] state;
    
   
    
    
  
    return I_prec;
    
}
