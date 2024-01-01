#pragma once 

#include <stdio.h> 
#include "./Modèle/tetris.h"

//Fonctions d'aide pour les blocs 
char obtenir_cellule(JeuTetris *obj, int ligne, int colonne);
 void definir_cellule(JeuTetris *obj, int ligne, int colonne, char valeur); 
bool verifier_jeu(JeuTetris *obj, int ligne, int colonne); 
void placer_bloc(JeuTetris *obj, BlocTetris bloc); 
 void retirer_bloc(JeuTetris *obj, BlocTetris bloc); 
 bool peut_placer_bloc(JeuTetris *obj, BlocTetris bloc) ; 
 int tetrimino_aleatoire(void) ; 
 void nouveau_bloc_en_chute(JeuTetris *obj) ; 

//Foncntions d'aide pour les tours du jeu 
 void tic_gravite_jeu(JeuTetris *obj) ; 
 void tic_deplacer(JeuTetris *obj, int direction) ; 
 void tic_descendre(JeuTetris *obj) ; 
 void tic_rotation(JeuTetris *obj, int direction) ; 
 void tic_stockage(JeuTetris *obj) ; 
 void tic_traiter_deplacement(JeuTetris *obj, DeplacementTetris mouvement); 
 bool tic_ligne_pleine(JeuTetris *obj, int i) ;
 void tic_decaler_lignes(JeuTetris *obj, int r) ; 
 int tic_verifier_lignes(JeuTetris *obj) ; 
 void tic_ajuster_score(JeuTetris *obj, int lignes_effacees) ; 
 bool tic_jeu_termine(JeuTetris *obj) ; 