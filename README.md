# Tetris-Ncurses-C
Tetris-NCurses est une implémentation du célèbre jeu Tetris en C, utilisant la librairie ncurses pour l'interface utilisateur. Ce projet a été développé dans le cadre de mon cours de programmation avancée en Licence 3 Informatique. 

# I. Tetris Game

## 1. Auteur

- TEBNI WAJIH , Licence 3 informatique à la faculté des sciences Jean Perrin à Lens. 
  
## 2. Compilation

Pour compiler le jeu Tetris, suivez les étapes ci-dessous :

1. Ouvrez un terminal.

2. Naviguez jusqu'au répertoire contenant le fichier Makefile.

3. Tapez la commande suivante pour lancer le processus de compilation :

    $ make 

## 3. Exécution

4. Une fois la compilation terminée avec succès, vous pouvez exécuter le jeu en utilisant la commande suivante :

    $ bin/release/main_ncurses 

# II. Instructions pour sauvegarder et reprendre une partie

## 1. Sauvegarde du jeu

- Pendant une partie de Tetris, vous pouvez sauvegarder votre progression en appuyant sur la touche **s**. 

- Un message s'affichera pour confirmer que vous souhaitez sauvegarder et quitter le jeu. 

- Appuyez ensuite sur une touche autre que **n** pour confirmer la sauvegarde. 

- La partie sera sauvegardée dans le fichier `tetris.save`.

## 2. Reprise d'une partie sauvegardée

Pour reprendre une partie sauvegardée, exécutez la commande suivante dans le terminal :

    $ bin/release/main tetris.save

## Remarques : 

- Remarque : Assurez-vous d'avoir les outils de compilation nécessaires installés sur votre système, et vérifiez le fichier Makefile pour toute configuration spécifique au projet.
