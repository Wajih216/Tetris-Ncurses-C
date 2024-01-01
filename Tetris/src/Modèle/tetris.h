#pragma once 

#include <stdio.h> // pour FILE
#include <stdbool.h> // pour bool

/*
  Convertir un type de tétrimino en sa cellule correspondante.
 */
#define TYPE_VERS_CELLULE(x) ((x)+1)

/*
  Chaînes pour imprimer un tableau Tetris.
 */
#define CHAINE_VIDE " "
#define CHAINE_BLOC "\u2588"

/*
  Questions sur une cellule Tetris.
 */
#define EST_VIDE(x) ((x) == TC_VIDE)
#define EST_PLEIN(x) (!EST_VIDE(x))

/*
  Nombre de cellules dans un tétrimino 
 */
#define TETRIS 4
/*
  Nombre de tétriminos 
 */
#define NB_TETRIMINOS 7
/*
  Nombre d'orientations d'un tétrimino 
 */
#define NB_ORIENTATIONS 4

/*
  Constantes de niveau.
 */
#define NIVEAU_MAX 19
#define LIGNES_PAR_NIVEAU 10

/*
  Une "cellule" est un bloc 1x1 dans un tableau Tetris.
 */
typedef enum {
  TC_VIDE, TC_CELLULE_I, TC_CELLULE_J, TC_CELLULE_L, TC_CELLULE_O, TC_CELLULE_S, TC_CELLULE_T, TC_CELLULE_Z
} CelluleTetris;

/*
  Un "type" est un type/forme d'un tétrimino.  Non inclus l'orientation.
 */
typedef enum {
  TETRIMINO_I, TETRIMINO_J, TETRIMINO_L, TETRIMINO_O, TETRIMINO_S, TETRIMINO_T, TETRIMINO_Z
} TypeTetrimino;

/*
  Une paire de ligne, colonne.  Des nombres négatifs sont autorisés, car nous en avons besoin pour
  les décalages.
 */
typedef struct {
  int ligne;
  int colonne;
} EmplacementTetris;

/*
  Un "bloc" est une structure qui contient des informations sur un tétrimino.
  Spécifiquement, quel type il est, quelle orientation il a et où il est.
 */
typedef struct {
  int type;
  int orientation;
  EmplacementTetris emplacement;
} BlocTetris;

/*
  Tous les mouvements possibles à donner en entrée au jeu.
 */
typedef enum {
  DEPLACEMENT_GAUCHE, DEPLACEMENT_DROITE, ROTATION_HORAIRE, ROTATION_ANTIHORAIRE, CHUTE_RAPIDE, STOCKAGE, AUCUN_DEPLACEMENT
} DeplacementTetris;

/*
  Un objet de jeu !
 */
typedef struct {
  /*
    Informations sur le tableau de jeu :
   */
  int lignes;
  int colonnes;
  char *tableau;
  /*
    Informations de score :
   */
  int points;
  int niveau;
  /*
    Le bloc en chute est celui qui descend actuellement. Le bloc suivant est celui qui
    descendra après celui-ci. Stocké est le bloc que vous pouvez échanger.
   */
  BlocTetris en_chute;
  BlocTetris suivant;
  BlocTetris stocke;
  /*
    Nombre de ticks jusqu'à la gravité pour que le bloc descende.
   */
  int ticks_jusqua_gravite;
  /*
    Nombre de lignes jusqu'à ce que vous passiez au niveau suivant.
   */
  int lignes_restantes;
} JeuTetris;

/*
  Cet tableau stocke toutes les informations nécessaires sur les cellules remplies par
  chaque tétrimino. Le premier indice est le type du tétrimino (c'est-à-dire la forme,
  par exemple I, J, Z, etc.). L'indice suivant est l'orientation (0-3). Le tableau final
  contient 4 objets EmplacementTetris, chacun se rapportant à un décalage par rapport à un
  point en haut à gauche qui est l'"origine" du tétrimino.
 */
extern EmplacementTetris TETRIMINOS[NB_TETRIMINOS][NB_ORIENTATIONS][TETRIS];

/*
  Cet tableau vous indique combien de ticks par gravité par niveau. Diminue à mesure que le
  niveau augmente, pour ajouter de la difficulté.
 */
extern int NiveauGravite[NIVEAU_MAX+1];

// Manipulation de la structure de données.
void initialiser_jeu(JeuTetris *obj, int lignes, int colonnes);
JeuTetris *creer_jeu(int lignes, int colonnes);
void detruire_jeu(JeuTetris *obj);
void supprimer_jeu(JeuTetris *obj);
JeuTetris *charger_jeu(FILE *f);
void sauvegarder_jeu(JeuTetris *obj, FILE *f);

// Méthodes publiques non liées à la mémoire :
char obtenir_cellule(JeuTetris *obj, int ligne, int colonne);
bool verifier_jeu(JeuTetris *obj, int ligne, int colonne);
bool tic_jeu(JeuTetris *obj, DeplacementTetris deplacement);
void imprimer_jeu(JeuTetris *obj, FILE *f);

