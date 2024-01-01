#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#include "./Modèle/tetris.h"
#include "./Contrôleur/controleur.h"

#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))
#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))

/*******************************************************************************

                               Définitions de Tableaux

*******************************************************************************/

EmplacementTetris TETRIMINOS[NB_TETRIMINOS][NB_ORIENTATIONS][TETRIS] = {
  // I
  {{{1, 0}, {1, 1}, {1, 2}, {1, 3}},
   {{0, 2}, {1, 2}, {2, 2}, {3, 2}},
   {{3, 0}, {3, 1}, {3, 2}, {3, 3}},
   {{0, 1}, {1, 1}, {2, 1}, {3, 1}}},
  // J
  {{{0, 0}, {1, 0}, {1, 1}, {1, 2}},
   {{0, 1}, {0, 2}, {1, 1}, {2, 1}},
   {{1, 0}, {1, 1}, {1, 2}, {2, 2}},
   {{0, 1}, {1, 1}, {2, 0}, {2, 1}}},
  // L
  {{{0, 2}, {1, 0}, {1, 1}, {1, 2}},
   {{0, 1}, {1, 1}, {2, 1}, {2, 2}},
   {{1, 0}, {1, 1}, {1, 2}, {2, 0}},
   {{0, 0}, {0, 1}, {1, 1}, {2, 1}}},
  // O
  {{{0, 1}, {0, 2}, {1, 1}, {1, 2}},
   {{0, 1}, {0, 2}, {1, 1}, {1, 2}},
   {{0, 1}, {0, 2}, {1, 1}, {1, 2}},
   {{0, 1}, {0, 2}, {1, 1}, {1, 2}}},
  // S
  {{{0, 1}, {0, 2}, {1, 0}, {1, 1}},
   {{0, 1}, {1, 1}, {1, 2}, {2, 2}},
   {{1, 1}, {1, 2}, {2, 0}, {2, 1}},
   {{0, 0}, {1, 0}, {1, 1}, {2, 1}}},
  // T
  {{{0, 1}, {1, 0}, {1, 1}, {1, 2}},
   {{0, 1}, {1, 1}, {1, 2}, {2, 1}},
   {{1, 0}, {1, 1}, {1, 2}, {2, 1}},
   {{0, 1}, {1, 0}, {1, 1}, {2, 1}}},
  // Z
  {{{0, 0}, {0, 1}, {1, 1}, {1, 2}},
   {{0, 2}, {1, 1}, {1, 2}, {2, 1}},
   {{1, 0}, {1, 1}, {2, 1}, {2, 2}},
   {{0, 1}, {1, 0}, {1, 1}, {2, 0}}},
};

int NiveauGravite[NIVEAU_MAX+1] = {
// 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
  50, 48, 46, 44, 42, 40, 38, 36, 34, 32,
//10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
  30, 28, 26, 24, 22, 20, 16, 12,  8,  4
};


/*******************************************************************************

                               Fonctions Principales du Jeu

*******************************************************************************/

/*
  Effectue une tick du jeu : traite la gravité, l'entrée de l'utilisateur et le score.
  Renvoie vrai si le jeu est toujours en cours, faux s'il est terminé.
 */
bool tic_jeu(JeuTetris *obj, DeplacementTetris deplacement)
{
  int lignes_effacees;
  // Gérer la gravité.
  tic_gravite_jeu(obj);

  // Gérer l'entrée utilisateur.
  tic_traiter_deplacement(obj, deplacement);

  // Vérifier les lignes effacées.
  lignes_effacees = tic_verifier_lignes(obj);

  // Ajuster le score.
  tic_ajuster_score(obj, lignes_effacees);

  // Retourner si le jeu va continuer (PAS s'il est terminé).
  return !tic_jeu_termine(obj);
}

/*
  Initialise la structure de données du jeu.
 */
void initialiser_jeu(JeuTetris *obj, int lignes, int colonnes)
{
  // initialisation
  obj->lignes = lignes;
  obj->colonnes = colonnes;
  obj->tableau = malloc(lignes * colonnes);
  memset(obj->tableau, TC_VIDE, lignes * colonnes);
  obj->points = 0;
  obj->niveau = 0;
  obj->ticks_jusqua_gravite = NiveauGravite[obj->niveau];
  obj->lignes_restantes = LIGNES_PAR_NIVEAU;
  srand(time(NULL));
  nouveau_bloc_en_chute(obj);
  nouveau_bloc_en_chute(obj);
  obj->stocke.type = -1;
  obj->stocke.orientation = 0;
  obj->stocke.emplacement.ligne = 0;
  obj->suivant.emplacement.colonne = obj->colonnes/2 - 2;
}

/*
  Crée un nouvel objet du jeu.
 */
JeuTetris *creer_jeu(int lignes, int colonnes)
{
  JeuTetris *obj = malloc(sizeof(JeuTetris));
  initialiser_jeu(obj, lignes, colonnes);
  return obj;
}

/*
  Détruit la structure de données du jeu.
 */
void detruire_jeu(JeuTetris *obj)
{
  // nettoyage
  free(obj->tableau);
}

/*
  Supprime complètement l'objet du jeu.
 */
void supprimer_jeu(JeuTetris *obj)
{
  detruire_jeu(obj);
  free(obj);
}

/*
  Charge un jeu à partir d'un fichier.
 */
JeuTetris *charger_jeu(FILE *f)
{
  JeuTetris *obj = malloc(sizeof(JeuTetris));
  fread(obj, sizeof(JeuTetris), 1, f);
  obj->tableau = malloc(obj->lignes * obj->colonnes);
  fread(obj->tableau, sizeof(char), obj->lignes * obj->colonnes, f);
  return obj;
}

/*
  Enregistre un jeu dans un fichier.
 */
void sauvegarder_jeu(JeuTetris *obj, FILE *f)
{
  fwrite(obj, sizeof(JeuTetris), 1, f);
  fwrite(obj->tableau, sizeof(char), obj->lignes * obj->colonnes, f);
}

/*
  Imprime un tableau de jeu dans un fichier.
 */
void imprimer_jeu(JeuTetris *obj, FILE *f) {
  int i, j;
  for (i = 0; i < obj->lignes; i++) {
    for (j = 0; j < obj->colonnes; j++) {
      if (EST_VIDE(obtenir_cellule(obj, i, j))) {
        fputs(CHAINE_VIDE, f);
      } else {
        fputs(CHAINE_BLOC, f);
      }
    }
    fputc('\n', f);
  }
}
