#include "bib.h"

// Les graphismes fixes
BITMAP *sp_mur,         *sp_mur2, *sp_end_door,      *sp_sol, 
       *sp_diamant,     *intfjeu, *sp_bonus_vitesse, *sp_bonus_bombe_puissance,
       *sp_bonus_vie,   *sp_bonus_bombe_nombre, *editmenu1, *editmenu2, *intfediteur;

// Les animations
animation_t *anim_bombe, *anim_deflagration, *anim_bonhomme_mort;

// Les animations 3D iso pour les joueurs
animation_t ***anim_joueurs;

// L'animation 3D isométrique
animation_t **anim_mechant;

//Police personnalisée
FONT *police;
DATAFILE *dat;

// Cette fonction ne s'utilise (pour l'instant) qu'une seule fois
// Si ce n'était pas le cas, il faudrait faire des initialisations de pointeurs
// dans le main.
int chargement_graphismes () {
    
    // Tous les graphismes sont des variables externes
    extern BITMAP *sp_mur, *sp_mur2, *sp_end_door, *sp_diamant, *intfjeu;
    extern BITMAP *sp_bonus_vitesse, *sp_bonus_bombe_puissance, *sp_bonus_bombe_nombre;
    extern BITMAP *editmenu1, *editmenu2, *intfediteur;
    extern animation_t *anim_bombe, *anim_deflagration, ***anim_joueurs, **anim_mechant;
    
    //police de caractères
    extern FONT *police;
    extern DATAFILE *dat;
    
    // Variables temporaires;
    int i, j;
    char ch_1[100], ch_2[100];

    //Chargement de la police : on commence par charger le fichier DAT
    dat = load_datafile("police.dat");
    if(!dat)
      ERREUR("Probleme au chargement du fichier de police");
    police = (FONT*) dat[DATA_police].dat;
      
    // CHARGEMENT DES ANIMATIONS
    //anim_bonhomme      = load_animation("bonhomme");
    anim_bombe         = load_animation("bombe");
    anim_bombe->nb_images_pause = 14;

    anim_deflagration  = load_animation("explosion");
    anim_bonhomme_mort = load_animation("mort");
    anim_bonhomme_mort->nb_images_pause = 10;

    // C'est de l'allocation pas vraiment dynamique...
    anim_joueurs = (animation_t ***) malloc( NB_JOUEURS_MAX * sizeof(animation_t **) );
    
    anim_mechant = (animation_t **) malloc( sizeof(animation_t *) * 5);
    
    // Pour faire les 4 joueurs
    for (i=0; i < NB_JOUEURS_MAX; i++) {
        int no_joueur;
        // Il y'a 5 directions, pour l'instant que 4 sont utilisées
        anim_joueurs[i] = (animation_t **) malloc( 5 * sizeof(animation_t *) );
        
        // Pour faire les 4 direction pour chaque joueurs
        
        for (j=0; j < 5; j++) {
            if (j == DIRECTION_GAUCHE)
                strcpy(ch_1, "gauche");
            else if (j == DIRECTION_DROITE) 
                strcpy(ch_1, "droite");
            else if (j == DIRECTION_BAS) 
                strcpy(ch_1, "bas");
            else if (j == DIRECTION_HAUT)
                strcpy(ch_1, "haut");
            // Pas d'image à l'arrêt
            else if (j == DIRECTION_ARRET)
                strcpy(ch_1, "droite");
            
            if (i <= 1) no_joueur = i+1;
            else no_joueur = 1;

            sprintf (ch_2, "player%d/%s", no_joueur, ch_1);
            
            anim_joueurs[i][j] = load_animation(ch_2);
            
            // il n'y a qu'un seul méchant
            if (i == 0) {
                sprintf (ch_2, "mechant/%s", ch_1);
                anim_mechant[j] = load_animation(ch_2);



            }
        }

        
    }
    


    // Chargement des images fixes
    if (! (sp_mur  = load_bmp("images/caisse.bmp", NULL)) )
        ERREUR("Probleme au chargement de images/mur.bmp");

    if (! (sp_mur2 = load_bmp("images/incassable.bmp", NULL)) )
        ERREUR("Probleme au chargement de images/mur2.bmp");

    if (! (sp_end_door = load_bmp("images/end_door.bmp", NULL)) )
        ERREUR("Probleme au chargement de images/end_door.bmp");
    
    if (! (sp_sol = load_bmp("images/sol.bmp", NULL)) )
        ERREUR("Probleme au chargement du sol");

    if (! (sp_diamant = load_bmp("images/diamant.bmp", NULL)) )
        ERREUR("Probleme au chargement des diamants");

    if (! (intfjeu = load_bmp("images/jeu.bmp", NULL)) )
        ERREUR("Probleme au chargement de l'interface du jeu");

    if (! (sp_bonus_vitesse = load_bmp("images/bonus_vitesse.bmp", NULL)) )
        ERREUR("Probleme au chargement du bonus de vitesse");

    if (! (sp_bonus_bombe_puissance = load_bmp("images/bonus_explosion.bmp", NULL)) )
        ERREUR("Probleme au chargement du bonus de puissance d'explosion");
    
    if (! (sp_bonus_bombe_nombre = load_bmp("images/bonus_bombes.bmp", NULL)) )
        ERREUR("Probleme au chargement du bonus du nombre de bombes");

    if (! (sp_bonus_vie = load_bmp("images/bonus_sante.bmp", NULL)) )
        ERREUR("Probleme au chargement du bonus de vie.");
    
    if (! (editmenu1 = load_bmp("images/editeur/menu1.bmp", NULL)) )
        ERREUR("Probleme au chargement du menu1.");

    if (! (editmenu2 = load_bmp("images/editeur/menu2.bmp", NULL)) )
        ERREUR("Probleme au chargement du menu2.");

    if (! (intfediteur = load_bmp("images/editeur/fond.bmp", NULL)) )
        ERREUR("Probleme au chargement du fond d'editeur.");
    
    
}

// Pour charger une animation (c'est à dire une suite d'images)
animation_t * load_animation(char *fichier) {
    char fichier_temp[100];
    BITMAP *bmp_temp;
    animation_t *animation; // L'animation que l'on va créer
    int i, j;
    ///extern unsigned char mode_editeur;
    // On alloue l'espace de l'animation
    animation = malloc(sizeof(animation_t)); 

    DEBUG ("L'animation cree a pour adresse : 0x%08X", animation);

    i = 0;
    //if (mode_editeur)
    //    sprintf(fichier_temp, "../images/%s_%d.bmp", fichier, i++);
    //else 
        sprintf(fichier_temp, "images/%s_%d.bmp", fichier, i++);
    DEBUG("On va charger le fichier %s.", fichier_temp);
    while ( bmp_temp = load_bmp(fichier_temp, NULL) ) {
        
        DEBUG ("L'adresse de bmp_temp est : %08X [%d].", bmp_temp,i);
        
        // On fait un malloc pour la première image
        if (i == 1) { 
            //DEBUG ("ET VOILA !!! ON BUG !");
            animation->images = (BITMAP **) malloc( sizeof(BITMAP *) );
            //DEBUG ("ON ARRIVE JAMAIS JUSQU'ICI !");
        }
        // On fait un realloc pour les autres
        else {        
            animation->images = (BITMAP **) realloc(animation->images, sizeof(BITMAP *) * i);
        }
        DEBUG ("On a passe l'allocation de memoire...");
        
        // On sauve cela dans le tableau (une '*') de pointeurs (une '*') sur image (BITMAP)
        // donc le BITMAP **images;
        animation->images[(i-1)] = bmp_temp;
        
        sprintf(fichier_temp, "images/%s_%d.bmp", fichier, i++);

    }
    //Si des images ont été chargées on initialise les paramètres  de l'animation
    if (i > 1) {
        animation->nb_images = (i-1);
        animation->nb_images_pause = 2; // (pause de 5 images à chaque fois)    
        animation->image_actuelle = 0;
    }
    //Sinon on la libère et on s'assure que NULL sera renvoyé par la fonction
    else {
        free(animation);
        animation = NULL;
        DEBUG ("Probleme au chargement de : %s", fichier);
        ERREUR ("PROBLEME DE CHARGEMENT DE FICHIER");
    }
    return animation;
}
