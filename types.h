#ifndef __BOMB_TYPES__
#define __BOMB_TYPES__

typedef struct st_coord {
    int x;
    int y;

} coord;

// Juste pour que ce soit plus clair pour nous :
// =============================================
// * Coordonnee de case
#define coord_c coord
//typedef coord coord_c; <-- Ca, ca serra quand tout sera bien differencie

// * Coordonnee de position
#define coord_p coord
//typedef coord coord_p; <-- pareil (voir plus haut)

#define TOUCHE_GAUCHE 0
#define TOUCHE_DROITE 1
#define TOUCHE_HAUT   2
#define TOUCHE_BAS    3
#define TOUCHE_BOMBE  4

#endif

