
tests/button.c :
    - On a vérifié qu'on appelle le callback et donc que cela affiche 'click'.
    - On a testé un click normal, un click avec appui dans la zone et relâchement en dehors (ce qui ne compte pas pour
      un click donc), et un click avec appui dans la zone puis sortie de la zone puis entrée dans la zone et relâchement
      dans la zone (ce qui ne compte pas non plus pour un click).

tests/frame.c :
    - On teste le fonctionnement du clipper avec une frame en bas à droite.
    - On teste l'affichage du texte et des images sur des frames (en prenant en compte clipper et anchor).

tests/hello_world :
    - On teste l'affichage de la toplevel. Ce test permet aussi de tester que la toplevel est bien affichée, même lorque
      elle dépasse les bords de son parent.

tests/minesweeper :
    - On vérifie qu'on peut jouer correctement au démineur (poser un drapeau, compteur, restart, ...).

tests/puzzle :
    - On vérifie qu'on peut jouer au puzzle.
    - On peut déplacer la fenêtre, la fermer.

tests/test_button :
    - On teste l'affichage du bouton avec ses parties hautes, basses et la partie complète.
    - On teste le bon affichagedu texte avec draw_text.
    - On teste aussi la fonction copy_surface.

tests/test_polygon :
    - On teste notre fonction polygone et arcs avec plusieurs tracés de rectangles, cercles et étoiles. On affiche à l'écran
      les drapeaux du Sénégal et de la Tunisie.

tests/polyline :
    - On test notre fonction polyline avec un tracé d'une forme un peu plus complexe (étoile) et on affiche à l'écran
      le drapeau du Maroc.

tests/top_in_top :
    - On vérifie que mettre une toplevel dans une toplevel est bien géré, avec les déplacements et redimensionnement (
      redimensionnement selon x ou selon les deux directions.).
    - Un "fake" bouton est présent dans la deuxième toplevel, qui nous fait quitter l'application en appuyant
      dessus (On vérifie le callback grâce à cela.).

tests/two048 :
    - On vérifie qu'on peut jouer au jeu dans une fenêtre.
    - On peut aussi ouvrir de nouvelles fenêtres (ctrl+N) et jouer dans celles-ci.
    - On peut fermer des fenêtres de jeu (bouton closable ou ctrl+W).
    - On peut déplacer les fenêtres sans impact sur le jeu.

