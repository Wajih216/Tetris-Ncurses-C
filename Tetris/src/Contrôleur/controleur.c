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

                          Fonctions d'Aide pour les Blocs

*******************************************************************************/

/*
   Retourne le bloc à la ligne et à la colonne donnée.
 */
char obtenir_cellule(JeuTetris *obj, int ligne, int colonne)
{
  return obj->tableau[obj->colonnes * ligne + colonne];
}

/*
  Place le bloc à la ligne et à la colonne donnée.
 */
void definir_cellule(JeuTetris *obj, int ligne, int colonne, char valeur)
{
  obj->tableau[obj->colonnes * ligne + colonne] = valeur;
}

/*
  Vérifie si une ligne et une colonne sont dans les limites.
 */
bool verifier_jeu(JeuTetris *obj, int ligne, int colonne)
{
  return 0 <= ligne && ligne < obj->lignes && 0 <= colonne && colonne < obj->colonnes;
}

/*
  Place un bloc sur le tableau.
 */
void placer_bloc(JeuTetris *obj, BlocTetris bloc)
{
  int i;
  for (i = 0; i < TETRIS; i++) {
    EmplacementTetris cellule = TETRIMINOS[bloc.type][bloc.orientation][i];
    definir_cellule(obj, bloc.emplacement.ligne + cellule.ligne, bloc.emplacement.colonne + cellule.colonne,
           TYPE_VERS_CELLULE(bloc.type));
  }
}

/*
  Retire un bloc du tableau.
 */
void retirer_bloc(JeuTetris *obj, BlocTetris bloc)
{
  int i;
  for (i = 0; i < TETRIS; i++) {
    EmplacementTetris cellule = TETRIMINOS[bloc.type][bloc.orientation][i];
    definir_cellule(obj, bloc.emplacement.ligne + cellule.ligne, bloc.emplacement.colonne + cellule.colonne, TC_VIDE);
  }
}

/*
  Vérifie si un bloc peut être placé sur le tableau.
 */
bool peut_placer_bloc(JeuTetris *obj, BlocTetris bloc)
{
  int i, ligne, colonne;
  for (i = 0; i < TETRIS; i++) {
    EmplacementTetris cellule = TETRIMINOS[bloc.type][bloc.orientation][i];
    ligne = bloc.emplacement.ligne + cellule.ligne;
    colonne = bloc.emplacement.colonne + cellule.colonne;
    if (!verifier_jeu(obj, ligne, colonne) || EST_PLEIN(obtenir_cellule(obj, ligne, colonne))) {
      return false;
    }
  }
  return true;
}

/*
  Retourne un type de tétrimino aléatoire.
 */
int tetrimino_aleatoire(void) {
  return rand() % NB_TETRIMINOS;
}

/*
  Crée un nouveau bloc en chute et remplit le bloc suivant avec un tétrimino aléatoire.
 */
void nouveau_bloc_en_chute(JeuTetris *obj)
{
  // Placez un nouveau tétrimino en chute.
  obj->en_chute = obj->suivant;
  obj->suivant.type = tetrimino_aleatoire();
  obj->suivant.orientation = 0;
  obj->suivant.emplacement.ligne = 0;
  obj->suivant.emplacement.colonne = obj->colonnes/2 - 2;
}

/*******************************************************************************

                       Fonctions d'Aide pour les Tours de Jeu

*******************************************************************************/

/*
  Effectue une tick de la gravité et déplace le bloc vers le bas si la gravité doit agir.
 */
void tic_gravite_jeu(JeuTetris *obj)
{
  obj->ticks_jusqua_gravite--;
  if (obj->ticks_jusqua_gravite <= 0) {
    retirer_bloc(obj, obj->en_chute);
    obj->en_chute.emplacement.ligne++;
    if (peut_placer_bloc(obj, obj->en_chute)) {
      obj->ticks_jusqua_gravite = NiveauGravite[obj->niveau];
    } else {
      obj->en_chute.emplacement.ligne--;
      placer_bloc(obj, obj->en_chute);

      nouveau_bloc_en_chute(obj);
    }
    placer_bloc(obj, obj->en_chute);
  }
}

/*
  Déplace le bloc en chute vers la gauche (-1) ou la droite (+1).
 */
void tic_deplacer(JeuTetris *obj, int direction)
{
  retirer_bloc(obj, obj->en_chute);
  obj->en_chute.emplacement.colonne += direction;
  if (!peut_placer_bloc(obj, obj->en_chute)) {
    obj->en_chute.emplacement.colonne -= direction;
  }
  placer_bloc(obj, obj->en_chute);
}

/*
  Envoie le bloc en chute en bas.
 */
void tic_descendre(JeuTetris *obj)
{
  retirer_bloc(obj, obj->en_chute);
  while (peut_placer_bloc(obj, obj->en_chute)) {
    obj->en_chute.emplacement.ligne++;
  }
  obj->en_chute.emplacement.ligne--;
  placer_bloc(obj, obj->en_chute);
  nouveau_bloc_en_chute(obj);
}

/*
  Fait tourner le bloc en chute dans l'une ou l'autre direction (+/-1).
 */
void tic_rotation(JeuTetris *obj, int direction)
{
  retirer_bloc(obj, obj->en_chute);

  while (true) {
    obj->en_chute.orientation = (obj->en_chute.orientation + direction) % NB_ORIENTATIONS;

    // Si la nouvelle orientation convient, nous avons terminé.
    if (peut_placer_bloc(obj, obj->en_chute))
      break;

    // Sinon, essayez de déplacer vers la gauche pour le faire correspondre.
    obj->en_chute.emplacement.colonne--;
    if (peut_placer_bloc(obj, obj->en_chute))
      break;

    // Enfin, essayez de déplacer vers la droite pour le faire correspondre.
    obj->en_chute.emplacement.colonne += 2;
    if (peut_placer_bloc(obj, obj->en_chute))
      break;

    // Replacez-le à son emplacement d'origine et essayez la prochaine orientation.
    obj->en_chute.emplacement.colonne--;
    /* Dans le pire des cas, nous revenons à l'orientation d'origine et elle correspond, donc cette
     boucle se terminera.*/
  }

  placer_bloc(obj, obj->en_chute);
}

/*
  Échange le bloc en chute avec le bloc dans le tampon de stockage.
 */
void tic_stockage(JeuTetris *obj)
{
  retirer_bloc(obj, obj->en_chute);
  if (obj->stocke.type == -1) {
    obj->stocke = obj->en_chute;
    nouveau_bloc_en_chute(obj);
  } else {
    int type = obj->en_chute.type, orientation = obj->en_chute.orientation;
    obj->en_chute.type = obj->stocke.type;
    obj->en_chute.orientation = obj->stocke.orientation;
    obj->stocke.type = type;
    obj->stocke.orientation = orientation;
    while (!peut_placer_bloc(obj, obj->en_chute)) {
      obj->en_chute.emplacement.ligne--;
    }
  }
  placer_bloc(obj, obj->en_chute);
}

/*
  Effectue l'action spécifiée par le mouvement.
 */
void tic_traiter_deplacement(JeuTetris *obj, DeplacementTetris mouvement)
{
  switch (mouvement) {
    case DEPLACEMENT_GAUCHE:
      tic_deplacer(obj, -1);
      break;
    case DEPLACEMENT_DROITE:
      tic_deplacer(obj, 1);
      break;
    case CHUTE_RAPIDE:
      tic_descendre(obj);
      break;
    case ROTATION_HORAIRE:
      tic_rotation(obj, 1);
      break;
    case ROTATION_ANTIHORAIRE:
      tic_rotation(obj, -1);
      break;
    case STOCKAGE:
      tic_stockage(obj);
      break;
    default:
      // Ignorer
      break;
  }
}

/*
  Renvoie vrai si la ligne i est pleine.
 */
bool tic_ligne_pleine(JeuTetris *obj, int i)
{
  int j;
  for (j = 0; j < obj->colonnes; j++) {
    if (EST_VIDE(obtenir_cellule(obj, i, j)))
      return false;
  }
  return true;
}

/*
  Décale chaque ligne au-dessus de r vers le bas d'un.
 */
void tic_decaler_lignes(JeuTetris *obj, int r)
{
  int i, j;
  for (i = r-1; i >= 0; i--) {
    for (j = 0; j < obj->colonnes; j++) {
      definir_cellule(obj, i+1, j, obtenir_cellule(obj, i, j));
      definir_cellule(obj, i, j, TC_VIDE);
    }
  }
}

/*
  Trouve les lignes remplies, les supprime, les décale et renvoie le nombre de
  lignes effacées.
 */
int tic_verifier_lignes(JeuTetris *obj)
{
  int i, lignes_effacees = 0;
  retirer_bloc(obj, obj->en_chute); // ne veut pas perturber le bloc en chute

  for (i = obj->lignes-1; i >= 0; i--) {
    if (tic_ligne_pleine(obj, i)) {
      tic_decaler_lignes(obj, i);
      i++; // refaire cette ligne car elles sont décalées
      lignes_effacees++;
    }
  }

  placer_bloc(obj, obj->en_chute); // remettre en place
  return lignes_effacees;
}

/*
  Ajuste le score du jeu, en fonction du nombre de lignes qui viennent d'être effacées.
 */
void tic_ajuster_score(JeuTetris *obj, int lignes_effacees)
{
   int multiplicateur_ligne[] = {0, 40, 100, 300, 1200};
  obj->points += multiplicateur_ligne[lignes_effacees] * (obj->niveau + 1);
  if (lignes_effacees >= obj->lignes_restantes) {
    obj->niveau = MIN(NIVEAU_MAX, obj->niveau + 1);
    lignes_effacees -= obj->lignes_restantes;
    obj->lignes_restantes = LIGNES_PAR_NIVEAU - lignes_effacees;
  } else {
    obj->lignes_restantes -= lignes_effacees;
  }
}

/*
  Renvoie vrai si le jeu est terminé.
 */
bool tic_jeu_termine(JeuTetris *obj)
{
  int i, j;
  bool jeu_termine = false;
  retirer_bloc(obj, obj->en_chute);
  for (i = 0; i < 2; i++) {
    for (j = 0; j < obj->colonnes; j++) {
      if (EST_PLEIN(obtenir_cellule(obj, i, j))) {
        jeu_termine = true;
      }
    }
  }
  placer_bloc(obj, obj->en_chute);
  return jeu_termine;
}
