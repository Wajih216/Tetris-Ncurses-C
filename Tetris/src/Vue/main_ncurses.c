#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <ncurses.h>
#include <string.h>

#include "./Modèle/tetris.h"
#include "./Utilitaires/util.h"
#include "./Contrôleur/controleur.h"

/*
  2 colonnes par cellule rendent le jeu beaucoup plus agréable.
 */
#define COLONNES_PAR_CELLULE 2
/*
  Macro pour afficher une cellule d'un type spécifique dans une fenêtre.
 */
#define AJOUTER_BLOC(w, x) waddch((w), ' '|A_REVERSE|COLOR_PAIR(x));     \
                       waddch((w), ' '|A_REVERSE|COLOR_PAIR(x))
#define AJOUTER_VIDE(w) waddch((w), ' '); waddch((w), ' ')

/*
  Affiche le plateau de Tetris dans la fenêtre ncurses.
 */
void afficher_plateau(WINDOW *w, JeuTetris *obj)
{
  int i, j;
  box(w, 0, 0);
  for (i = 0; i < obj->lignes; i++) {
    wmove(w, 1 + i, 1);
    for (j = 0; j < obj->colonnes; j++) {
      if (EST_PLEIN(obtenir_cellule(obj, i, j))) {
        AJOUTER_BLOC(w, obtenir_cellule(obj, i, j));
      } else {
        AJOUTER_VIDE(w);
      }
    }
  }
  wnoutrefresh(w);
}

/*
  Affiche une pièce Tetris dans une fenêtre dédiée.
*/
void afficher_piece(WINDOW *w, BlocTetris bloc)
{
  int b;
  EmplacementTetris c;
  wclear(w);
  box(w, 0, 0);
  if (bloc.type == -1) {
    wnoutrefresh(w);
    return;
  }
  for (b = 0; b < TETRIS; b++) {
    c = TETRIMINOS[bloc.type][bloc.orientation][b];
    wmove(w, c.ligne + 1, c.colonne * COLONNES_PAR_CELLULE + 1);
    AJOUTER_BLOC(w, TYPE_VERS_CELLULE(bloc.type));
  }
  wnoutrefresh(w);
}

/*
  Affiche les informations de score dans une fenêtre dédiée.
 */
void afficher_score(WINDOW *w, JeuTetris *tg)
{
  wclear(w);
  box(w, 0, 0);
  wprintw(w, "Score\n%d\n", tg->points);
  wprintw(w, "Niveau\n%d\n", tg->niveau);
  wprintw(w, "Lignes\n%d\n", tg->lignes_restantes);
  wnoutrefresh(w);
}


/*
  Enregistre et quitte le jeu.
 */
void sauvegarder(JeuTetris *jeu, WINDOW *w)
{
  FILE *f;

  wclear(w);
  box(w, 0, 0); // remettre la bordure
  wmove(w, 1, 1);
  wprintw(w, "Enregistrer et quitter ? [O/n] ");
  wrefresh(w);
  timeout(-1);
  if (getch() == 'n') {
    timeout(0);
    return;
  }
  f = fopen("tetris.save", "w");
  sauvegarder_jeu(jeu, f);
  fclose(f);
  supprimer_jeu(jeu);
  endwin();
  printf("Jeu enregistré dans \"tetris.save\".\n");
  printf("Reprenez en passant le nom de fichier en argument de ce programme.\n");
  exit(EXIT_SUCCESS);
}

/*
  Effectue les étapes d'initialisation NCURSES pour les blocs de couleur.
 */
void init_couleurs(void)
{
  start_color();
  init_color(COLOR_BLACK, 0, 0, 0);
  init_pair(TC_CELLULE_I, COLOR_CYAN, COLOR_BLACK);
  init_pair(TC_CELLULE_J, COLOR_BLUE, COLOR_BLACK);
  init_pair(TC_CELLULE_L, COLOR_WHITE, COLOR_BLACK);
  init_pair(TC_CELLULE_O, COLOR_YELLOW, COLOR_BLACK);
  init_pair(TC_CELLULE_S, COLOR_GREEN, COLOR_BLACK);
  init_pair(TC_CELLULE_T, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(TC_CELLULE_Z, COLOR_RED, COLOR_BLACK);
}

/*
  Jeu Tetris principal !
 */
int main(int argc, char **argv)
{
  JeuTetris *jeu;
  DeplacementTetris mouvement = AUCUN_DEPLACEMENT;
  bool en_cours = true;
  WINDOW *plateau, *suivant, *attente, *score;

  // Charger le fichier si un nom de fichier est fourni.
  if (argc >= 2) {
    FILE *f = fopen(argv[1], "r");
    if (f == NULL) {
      perror("tetris");
      exit(EXIT_FAILURE);
    }
    jeu = charger_jeu(f);
    fclose(f);
  } else {
    // Sinon, créer un nouveau jeu.
    jeu = creer_jeu(22, 10);
  }

  // Initialisation NCURSES :
  initscr();             // initialiser curses
  cbreak();              // transmettre les pressions de touche au programme, mais pas les signaux
  noecho();              // ne pas écho des touches à l'écran
  keypad(stdscr, TRUE);  // permettre les touches de flèche
  timeout(0);            // pas de blocage sur getch()
  curs_set(0);           // rendre le curseur invisible
  init_couleurs();       // configurer les couleurs Tetris

  // Créer des fenêtres pour chaque section de l'interface.
  plateau = newwin(jeu->lignes + 2, 2 * jeu->colonnes + 2, 0, 0);
  suivant  = newwin(6, 10, 0, 2 * (jeu->colonnes + 1) + 1);
  attente  = newwin(6, 10, 7, 2 * (jeu->colonnes + 1) + 1);
  score = newwin(6, 10, 14, 2 * (jeu->colonnes + 1 ) + 1);

  // Boucle de jeu
  while (en_cours) {
    en_cours = tic_jeu(jeu, mouvement);
    afficher_plateau(plateau, jeu);
    afficher_piece(suivant, jeu->suivant);
    afficher_piece(attente, jeu->stocke);
    afficher_score(score, jeu);
    doupdate();
    sleep_milli(10);

    switch (getch()) {
    case KEY_LEFT:
      mouvement = DEPLACEMENT_GAUCHE;
      break;
    case KEY_RIGHT:
      mouvement = DEPLACEMENT_DROITE;
      break;
    case KEY_UP:
      mouvement = ROTATION_HORAIRE;
      break;
    case KEY_DOWN:
      mouvement = CHUTE_RAPIDE;
      break;
    case 'q':
      en_cours = false;
      mouvement = AUCUN_DEPLACEMENT;
      break;
    case 'p':
      wclear(plateau);
      box(plateau, 0, 0);
      wmove(plateau, jeu->lignes/2, (jeu->colonnes*COLONNES_PAR_CELLULE-6)/2);
      wprintw(plateau, "EN PAUSE");
      wrefresh(plateau);
      timeout(-1);
      getch();
      timeout(0);
      mouvement = AUCUN_DEPLACEMENT;
      break;
    case 's':
      sauvegarder(jeu, plateau);
      mouvement = AUCUN_DEPLACEMENT;
      break;
    case ' ':
      mouvement = STOCKAGE;
      break;
    default:
      mouvement = AUCUN_DEPLACEMENT;
    }
  }

  // Déinitialiser NCurses
  wclear(stdscr);
  endwin();

  // Message de fin.
  printf("Partie terminée !\n");
  printf("Vous avez terminé avec %d points au niveau %d.\n", jeu->points, jeu->niveau);

  // Désinitialiser Tetris
  supprimer_jeu(jeu);
  return 0;
}