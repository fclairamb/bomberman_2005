#include "bib.h"

#ifndef _HIGHSCORE_
#define _HIGHSCORE_

#define NB_HIGHSCORE    20

#define BONUS_DIAMANT   100
#define BONUS_PORTE     200
#define BONUS_MECHANT   50


typedef struct st_hs_ligne {
    unsigned int niveau;
    unsigned int score;
    char nom[50];
} t_hs_ligne;

typedef struct st_hs_complet {
    t_hs_ligne lignes[NB_HIGHSCORE];

} t_hs_complet;

char * graphique_get_str(int, int, char *, int);











#endif
