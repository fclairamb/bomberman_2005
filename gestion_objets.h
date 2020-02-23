#include "bib.h"

#ifndef _OBJETS_
#define _OBJETS_

/*******************************************************************************
***                              POUR LES OBJETS                             ***
*******************************************************************************/
// Ici ça serra de l'allocation dynamique

#define DIRECTION_ARRET  0
#define DIRECTION_DROITE 1
#define DIRECTION_BAS    2
#define DIRECTION_GAUCHE 3
#define DIRECTION_HAUT   4

// Le type animation_t est un tableau d'images (BITMAP *)
typedef struct st_animation {
    BITMAP **images; // Pointeur sur un tableau pointeur BITMAP (on est pas sorti de l'auberge)
    unsigned int nb_images;
    unsigned int image_actuelle;
    unsigned int nb_images_pause;
} animation_t;

// Seulement pour les infos complexes du bomberman
typedef struct st_infos {
    unsigned int no_joueur;
    unsigned int vies;
    unsigned int nb_bombes;
    unsigned int max_nb_bombes;
    unsigned char no_move;
    unsigned int puissance_bombe;
    unsigned int score;
    unsigned int vitesse;

} t_infos;

typedef struct st_objet {
    coord_p position;        // Position de l'objet par rapport à l'écran
    coord_c placement;       // Placement de l'objet sur les cases
    unsigned char type;      // Le type 100 est le type Bomberman, // Le type 101 est le type MECHANT
    unsigned long int typeb; // typeb, typec et typed sont des informations complémentaires
    unsigned long int typec;   
    unsigned long int typed;
    animation_t *animation;  // Image de l'objet
    unsigned char direction; // (ARRET, HAUT, BAS, GAUCHE, DROITE)
    int vitesse;
    unsigned long int heure_de_creation;
    t_infos *plus;
} objet;



/* Pour les bombes : 
typeb correspond a leur temps de vie restant
typec correspond à leur puissance (1 pour une case de déflagration)
*/

// Ce sont des definitions placées A LA PLACE DU POINTEUR MEMOIRE
#define OBJ_T_MUR                    1
#define OBJ_T_INCASSABLE             2

#define OBJ_T_BONUS_VIE              21
#define OBJ_T_BONUS_VITESSE          22
#define OBJ_T_BONUS_BOMBE_PUISSANCE  23
#define OBJ_T_BONUS_BOMBE_NOMBRE     24
#define OBJ_T_END_DOOR               60
#define OBJ_T_DIAMANT                70
#define OBJ_T_BIENTOT                80 // Pour un objet arrivant bientôt (spécifié par typeb)
// C'est nombres sont valables pour les fichiers de niveaux ET pour les pointeurs
// ils sont mis A LA PLACE des pointeurs (pour les objets qui n'ont pas besoin de structure)

// Ici sont les définition représentants les objets "complexes"
#define OBJ_T_BOMBERMAN              100
#define OBJ_T_BOMBERMAN_MORT         101
#define OBJ_T_MECHANT                110
#define OBJ_T_BOMBE                  140
#define OBJ_T_DEFLAGRATION           141
// Ils ne sont valables QUE DANS LES FICHIERS DE NIVEAUX, ils sont convertis au chargement
// du fichier de niveaux en objets (dont en pointeur sur objets) par la fonction creer_objet

// Les parametres qui NE SONT PAS DES OBJETS :
#define OBJ_T_TEMPS_PARCOURS         1000

/*******************************************************************************
***                              POUR LES CASES                              ***
*******************************************************************************/ 
// Ici, c'est du tout - statique


// Position en haut à gauche de l'ensemble des cases
#define POS_CASES_X 30
#define POS_CASES_Y 30

// Largeur de case
#define LAR_CASE_L 54

// Largeur des objets dans les cases
#define OBJ_LAR 25

// Nombre de cases
#define NB_CASES_X 11
#define NB_CASES_Y 10

// Nombre d'objets par case (en fait c'est un de moins)
#define NB_OBJ_CASE 10

// On defini le type carre ("case" est déja pris pour switch)
typedef struct {
    objet *objets[NB_OBJ_CASE]; // Entier correspondant à l'indice de l'objet sur lequel pointer
    
} carre;

/*******************************************************************************
***      INFORMATIONS RELATIVES A LA GESTION DES OBJETS                      ***
*******************************************************************************/

#define NB_JOUEURS_MAX 4

// Nombre de joueurs MAX
int nb_joueurs;

objet *joueurs[4];

unsigned int nb_diamants;

/*******************************************************************************
***            FONCTIONS DE GESTION D'OBJETS                                 ***
*******************************************************************************/



objet * creer_objet (int type, coord_c _case);

coord case_correspondante(coord position);

int yatil_obstacle_case(coord_c _case);

int yatil_obstacle_pos(coord_p pos);

int give_life (objet *objet);

coord_p position_from_placement (coord_c _case);

#endif
