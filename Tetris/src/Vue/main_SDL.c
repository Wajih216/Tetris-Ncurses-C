#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>  // Ajout de la bibliothèque SDL_ttf pour la gestion du texte

#include "./Modèle/tetris.h"
#include "./Utilitaires/util.h"
#include "./Contrôleur/controlleur.h"

#define CELL_WIDTH 30
#define CELL_HEIGHT 30

/*
  2 colonnes par cellule rendent le jeu beaucoup plus agréable.
 */
#define COLONNES_PAR_CELLULE 2

void afficher_plateau(SDL_Surface *screen, JeuTetris *obj)
{
    SDL_Rect cellRect;
    cellRect.w = CELL_WIDTH;  // Largeur d'une cellule
    cellRect.h = CELL_HEIGHT; // Hauteur d'une cellule

    // Nettoyer l'écran
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));

    for (int i = 0; i < obj->lignes; i++)
    {
        for (int j = 0; j < obj->colonnes; j++)
        {
            cellRect.x = j * CELL_WIDTH;  // Position en x de la cellule
            cellRect.y = i * CELL_HEIGHT; // Position en y de la cellule

            // Récupérer la couleur de la cellule
            Uint32 couleur = obtenir_couleur(obj, i, j);

            // Dessiner la cellule sur la surface de l'écran
            SDL_FillRect(screen, &cellRect, couleur);
        }
    }

    // Mettre à jour l'écran
    SDL_Flip(screen);
}

void afficher_score(SDL_Surface *screen, JeuTetris *tg)
{
    // Créer une surface temporaire pour le rendu du texte
    SDL_Surface *textSurface = NULL;

    // Créer une police pour le texte (vous devez définir votre police TTF)
    TTF_Font *font = TTF_OpenFont("Police.ttf", 20);
    if (font == NULL)
    {
        fprintf(stderr, "Erreur lors du chargement de la police TTF : %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }

    // Couleur du texte
    SDL_Color textColor = {255, 255, 255}; // Blanc

    // Nettoyer l'écran
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));

    // Afficher le score
    afficher_texte("Score", font, textColor, screen, 10, 10);
    afficher_nombre(tg->points, font, textColor, screen, 10, 40);

    // Afficher le niveau
    afficher_texte("Niveau", font, textColor, screen, 10, 80);
    afficher_nombre(tg->niveau, font, textColor, screen, 10, 110);

    // Afficher les lignes restantes
    afficher_texte("Lignes", font, textColor, screen, 10, 150);
    afficher_nombre(tg->lignes_restantes, font, textColor, screen, 10, 180);

    // Mettre à jour l'écran
    SDL_Flip(screen);

    // Libérer la police TTF
    TTF_CloseFont(font);
}

void sauvegarder(JeuTetris *jeu, SDL_Surface *screen)
{
    SDL_Event event;
    bool sauvegarder = false;

    // Créer une police pour le texte (vous devez définir votre police TTF)
    TTF_Font *font = TTF_OpenFont("VotrePolice.ttf", 20);
    if (font == NULL)
    {
        fprintf(stderr, "Erreur lors du chargement de la police TTF : %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }

    // Couleur du texte
    SDL_Color textColor = {255, 255, 255}; // Blanc

    // Créer une surface pour afficher le message de sauvegarde
    SDL_Surface *messageSurface = NULL;
    messageSurface = TTF_RenderText_Solid(font, "Enregistrer et quitter ? [O/n]", textColor);

    // Mettre à jour l'écran
    SDL_BlitSurface(messageSurface, NULL, screen, NULL);
    SDL_Flip(screen);

    while (!sauvegarder)
    {
        SDL_WaitEvent(&event);

        switch (event.type)
        {
        case SDL_QUIT:
            // Gérer l'événement de fermeture de la fenêtre
            supprimer_jeu(jeu);
            TTF_CloseFont(font);
            SDL_FreeSurface(messageSurface);
            SDL_Quit();
            exit(EXIT_SUCCESS);
            break;

        case SDL_KEYDOWN:
            // Gérer les événements liés aux touches du clavier
            if (event.key.keysym.sym == SDLK_o)
            {
                sauvegarder = true;
            }
            else if (event.key.keysym.sym == SDLK_n)
            {
                // L'utilisateur a choisi de ne pas sauvegarder, terminer le jeu
                supprimer_jeu(jeu);
                TTF_CloseFont(font);
                SDL_FreeSurface(messageSurface);
                SDL_Quit();
                exit(EXIT_SUCCESS);
            }
            break;
        }
    }

    // Sauvegarder le jeu
    FILE *f = fopen("tetris.save", "w");
    sauvegarder_jeu(jeu, f);
    fclose(f);

    // Libérer les ressources
    TTF_CloseFont(font);
    SDL_FreeSurface(messageSurface);
}

void afficher_piece(SDL_Surface *screen, BlocTetris bloc)
{
    SDL_Rect cellRect;
    cellRect.w = CELL_WIDTH;  // Largeur d'une cellule
    cellRect.h = CELL_HEIGHT; // Hauteur d'une cellule

    // Nettoyer l'écran
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));

    if (bloc.type == -1)
    {
        // Pas de pièce à afficher
        SDL_Flip(screen);
        return;
    }

    for (int b = 0; b < TETRIS; b++)
    {
        EmplacementTetris c = TETRIMINOS[bloc.type][bloc.orientation][b];

        // Calculer la position en pixels de la cellule
        cellRect.x = (c.colonne * COLONNES_PAR_CELLULE) * CELL_WIDTH;
        cellRect.y = c.ligne * CELL_HEIGHT;

        // Récupérer la couleur de la cellule
        Uint32 couleur = TYPE_VERS_COULEUR(bloc.type);

        // Dessiner la cellule sur la surface de l'écran
        SDL_FillRect(screen, &cellRect, couleur);
    }

    // Mettre à jour l'écran
    SDL_Flip(screen);
}

void init_couleurs(void)
{
    // SDL init (vous devez déjà avoir initialisé SDL avant d'appeler cette fonction)
    SDL_Init(SDL_INIT_VIDEO);

    // Initialisation de SDL_ttf
    TTF_Init();

    // Couleurs Tetris
    SDL_Color couleurCyan = {0, 255, 255};       // Cyan
    SDL_Color couleurBleue = {0, 0, 255};         // Bleu
    SDL_Color couleurBlanc = {255, 255, 255};     // Blanc
    SDL_Color couleurJaune = {255, 255, 0};       // Jaune
    SDL_Color couleurVert = {0, 255, 0};          // Vert
    SDL_Color couleurMagenta = {255, 0, 255};     // Magenta
    SDL_Color couleurRouge = {255, 0, 0};         // Rouge

    // Mappez les couleurs Tetris à des valeurs spécifiques
    SDL_SetPaletteColors(SDL_GetVideoSurface()->format->palette, &couleurCyan, TC_CELLULE_I, 1);
    SDL_SetPaletteColors(SDL_GetVideoSurface()->format->palette, &couleurBleue, TC_CELLULE_J, 1);
    SDL_SetPaletteColors(SDL_GetVideoSurface()->format->palette, &couleurBlanc, TC_CELLULE_L, 1);
    SDL_SetPaletteColors(SDL_GetVideoSurface()->format->palette, &couleurJaune, TC_CELLULE_O, 1);
    SDL_SetPaletteColors(SDL_GetVideoSurface()->format->palette, &couleurVert, TC_CELLULE_S, 1);
    SDL_SetPaletteColors(SDL_GetVideoSurface()->format->palette, &couleurMagenta, TC_CELLULE_T, 1);
    SDL_SetPaletteColors(SDL_GetVideoSurface()->format->palette, &couleurRouge, TC_CELLULE_Z, 1);
}

SDL_Surface *load_image(const char *filename)
{
    SDL_Surface *loadedImage = NULL;
    SDL_Surface *optimizedImage = NULL;

    loadedImage = SDL_LoadBMP(filename);
    if (loadedImage != NULL)
    {
        optimizedImage = SDL_DisplayFormat(loadedImage);
        SDL_FreeSurface(loadedImage);
    }

    return optimizedImage;
}

void apply_surface(int x, int y, SDL_Surface *source, SDL_Surface *destination)
{
    SDL_Rect offset;
    offset.x = x;
    offset.y = y;
    SDL_BlitSurface(source, NULL, destination, &offset);
}

int main(int argc, char **argv)
{
    SDL_Surface *screen = NULL;
    bool en_cours = true;
    JeuTetris *jeu;
    DeplacementTetris mouvement = AUCUN_DEPLACEMENT;

    // Initialisation Tetris
    if (argc >= 2)
    {
        FILE *f = fopen(argv[1], "r");
        if (f == NULL)
        {
            perror("tetris");
            exit(EXIT_FAILURE);
        }
        jeu = charger_jeu(f);
        fclose(f);
    }
    else
    {
        jeu = creer_jeu(22, 10);
    }

    // Initialisation SDL
    init_couleurs();

    // Initialisation de la fenêtre SDL
    screen = SDL_SetVideoMode(jeu->colonnes * CELL_WIDTH, jeu->lignes * CELL_HEIGHT, 32, SDL_SWSURFACE);

    // Boucle de jeu
    SDL_Event event;
    while (en_cours)
    {
        // Gestion des événements SDL
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                en_cours = false;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                case SDLK_LEFT:
                    mouvement = DEPLACEMENT_GAUCHE;
                    break;
                case SDLK_RIGHT:
                    mouvement = DEPLACEMENT_DROITE;
                    break;
                case SDLK_UP:
                    mouvement = ROTATION_HORAIRE;
                    break;
                case SDLK_DOWN:
                    mouvement = CHUTE_RAPIDE;
                    break;
                case SDLK_SPACE:
                    mouvement = STOCKAGE;
                    break;
                case SDLK_q:
                    en_cours = false;
                    mouvement = AUCUN_DEPLACEMENT;
                    break;
                case SDLK_p:
                    // Mettez ici la logique pour mettre en pause le jeu
                    break;
                case SDLK_s:
                    // Mettez ici la logique pour sauvegarder le jeu
                    sauvegarder(jeu, screen);
                    mouvement = AUCUN_DEPLACEMENT;
                    break;
                }
                break;
            }
        }

        // Mettez ici votre logique de mise à jour du jeu
        en_cours = tic_jeu(jeu, mouvement);

        // Mettez ici votre logique pour afficher le plateau, la pièce suivante, le score, etc.
        afficher_plateau(screen, jeu);
        afficher_score(screen, jeu);

        // Ajoutez un délai pour contrôler la vitesse du jeu
        SDL_Delay(10);
    }

    // Libération des ressources
    SDL_Quit();
    supprimer_jeu(jeu);

    return 0;
}