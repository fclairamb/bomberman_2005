#ifndef _AFFICHAGE_

#define _AFFICHAGE_

// Temps que doit mettre l'ordinateur pour effectuer une boucle
#define TEMPS_DE_BOUCLE 50

//Positions des différentes infos des joueurs
#define VIE_X 686
#define SCORE_X 717
#define VIE_1_Y 33
#define VIE_2_Y 103
#define VIE_3_Y 174
#define VIE_4_Y 244
#define DIAMANTS_X 765
#define DIAMANTS_Y 314


int affichage();
int draw_animation (animation_t *animation, int x, int y, int angle, char no_move);

#endif
