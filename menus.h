#ifndef _MENUS_

#define _MENUS_

#define MENU_UN_JOUEUR    1
#define MENU_DEUX_JOUEURS 2
#define MENU_HIGHSCORES   3
#define MENU_EDITEUR      4
#define MENU_QUITTER      5

//Constantes pour les coordonnées des boutons
#define MENU_X         217
#define UN_JOUEUR_Y    163
#define DEUX_JOUEURS_Y 223
#define HIGHSCORES_Y   283
#define EDITEUR_Y      343
#define QUITTER_Y      403 

/* 
  Pour afficher le menu principal.
*/
int lancer_menu_principal(void);

#endif
