#include "bib.h"

// Bitmap de sortie
BITMAP *sortie;


int affichage() {
    int i, j, k, l;
    static unsigned char nb_buffer = 0; // (c'est 0 ou 1)
    // Declaration des variables externes
    extern carre cases[NB_CASES_X][NB_CASES_Y];   // carre[NB_CASES_X][NB_CASES_Y]
    //extern objet **objets;  // objet *
    extern BITMAP *sortie;
    extern unsigned char mode_editeur;
    extern unsigned int niveau_actuel;
    extern int nb_joueurs;
    extern BITMAP *sp_mur,                *sp_mur2,                  *sp_end_door,
                  *sp_diamant,            *sp_sol,                   *intfjeu, 
                  *sp_bonus_vitesse,      *sp_bonus_bombe_puissance, *sp_bonus_vie, 
                  *sp_bonus_bombe_nombre;

    extern animation_t ***anim_joueurs, **anim_mechant;
    extern FONT *police;
    
    extern unsigned int nb_diamants;
    
    unsigned char yatil_deflagration = FALSE;
    unsigned char yatil_mechant      = FALSE;
    static unsigned char last_yatil_deflagration = FALSE;
    extern unsigned long int temps_a_pas_depasser;
    unsigned long int temps_restant;
    carre *case_actuelle;
    coord_p pos;
    //char no_move;

    objet *objet_actuel;

    DEBUG ("-- AFFICHAGE --");

    //On dessine l'interface et le fond du jeu
    draw_sprite(sortie, intfjeu, 0, 0);
    
    // On dessine tout
    //rectfill(sortie, makecol(255, 0, 200), 0, 0, 800, 600);
    
    /***************************************************************************
    ***         AFFICHAGES DES OBJETS DE CHAQUE CASE                         ***
    ***************************************************************************/
    for (j = 0; j < NB_CASES_Y; j++)
        for (i = 0; i < NB_CASES_X; i++)
         {
            case_actuelle = &(cases[i][j]);
            k = 0;
            
            while ( objet_actuel = (case_actuelle->objets)[(k++)]) {
                // Tant qu'on a pas de pointeur nul
                
                l = (int) objet_actuel;                
                
                pos.x = POS_CASES_X + ( LAR_CASE_L * i );
                pos.y = POS_CASES_Y + ( LAR_CASE_L * j );
                if (l == OBJ_T_MUR) 
                    draw_sprite(sortie, sp_mur, pos.x, pos.y);
                else if (l == OBJ_T_INCASSABLE) 
                    draw_sprite(sortie, sp_mur2, pos.x, pos.y);
                else if (l == OBJ_T_END_DOOR)
                    draw_sprite(sortie, sp_end_door, pos.x, pos.y);
                else if (l == OBJ_T_DIAMANT)
                    draw_sprite(sortie, sp_diamant, pos.x, pos.y);
                else if (l == OBJ_T_BONUS_VITESSE)
                    draw_sprite(sortie, sp_bonus_vitesse, pos.x, pos.y);
                else if (l == OBJ_T_BONUS_BOMBE_PUISSANCE)
                    draw_sprite(sortie, sp_bonus_bombe_puissance, pos.x, pos.y);
                else if (l == OBJ_T_BONUS_BOMBE_NOMBRE)
                    draw_sprite(sortie, sp_bonus_bombe_nombre, pos.x, pos.y);
                else if (l == OBJ_T_BONUS_VIE)
                    draw_sprite(sortie, sp_bonus_vie, pos.x, pos.y);

                else if (l >= 100) { // C'est une adresse et donc un objet
                    
                    DEBUG("case : %2d x %2d, pos : %3d x %3d, adresse : %08X / %3d - %3d - %3d", i, j, pos.x, pos.y, l, objet_actuel->type, objet_actuel->typeb, objet_actuel->typec);
                    // L'objet peut - il bouger ??                
                    // no_move = 1 (si ce n'est pas le cas)
                    
                    // Si give_life retourne FALSE, c'est que give_live vient de reprendre
                    // la vie qu'il animait jusque la. Comme c'est beau...
                    
                        if ( mode_editeur || give_life(objet_actuel)) {
                            if (objet_actuel->type == OBJ_T_MECHANT) {
                                    DEBUG ("On va afficher le mechant");
                                    draw_animation(anim_mechant[(objet_actuel->direction)], objet_actuel->position.x, objet_actuel->position.y, 0, TRUE);
                                    DEBUG ("Mechant affiche");
                                    yatil_mechant = TRUE;
                            }
                            else if (objet_actuel->type == OBJ_T_BOMBERMAN) {

                                draw_animation(anim_joueurs[(objet_actuel->plus->no_joueur)][(objet_actuel->direction)], objet_actuel->position.x, objet_actuel->position.y, 0, objet_actuel->plus->no_move);

                            }
                            else {
                                if (objet_actuel->type == OBJ_T_BOMBERMAN_MORT) 
                                    objet_actuel->direction = DIRECTION_HAUT;
                                draw_animation(objet_actuel->animation, objet_actuel->position.x, objet_actuel->position.y, (objet_actuel->direction*64), FALSE);
                            }
                            
                        }
                        else DEBUG ("give_life retourne FALSE ou mode_editeur");

                        if (objet_actuel->type == OBJ_T_DEFLAGRATION)
                            yatil_deflagration = TRUE;
                    
                    //else draw_animation(anim_joueurs[(objet_actuel->plus->no_joueur)][(objet_actuel->direction)], objet_actuel->position.x, objet_actuel->position.y, (objet_actuel->direction*64), FALSE);
                }
                //if (k == 1) 
                    //draw_sprite(sortie, sp_sol, pos.x, pos.y);
            }
        }

    
    

    /***************************************************************************
    ***                   AFFICHAGE DES INFOS DES JOUEURS                    ***
    ***************************************************************************/
    if (!mode_editeur) {

        if(nb_joueurs > 0)
        {
          //Nombre de vies restantes  
          textprintf_ex(sortie, police, VIE_X, VIE_1_Y, makecol(255, 255, 255), -1, "%2d", joueurs[0]->plus->vies);   
          //Score du joueur
          textprintf_ex(sortie, police, SCORE_X, VIE_1_Y, makecol(255, 255, 255), -1, "%8d", joueurs[0]->plus->score);             
        }    
        if(nb_joueurs > 1)
        {
          //Nombre de vies restantes  
          textprintf_ex(sortie, police, VIE_X, VIE_2_Y, makecol(255, 255, 255), -1, "%2d", joueurs[1]->plus->vies);   
          //Score du joueur
          textprintf_ex(sortie, police, SCORE_X, VIE_2_Y, makecol(255, 255, 255), -1, "%8d", joueurs[1]->plus->score);             
        }
        if(nb_joueurs > 2)
        {
          //Nombre de vies restantes  
          textprintf_ex(sortie, police, VIE_X, VIE_3_Y, makecol(255, 255, 255), -1, "%2d", joueurs[2]->plus->vies);   
          //Score du joueur
          textprintf_ex(sortie, police, SCORE_X, VIE_3_Y, makecol(255, 255, 255), -1, "%8d", joueurs[2]->plus->score);             
        }
        if(nb_joueurs > 3)
        {
          //Nombre de vies restantes  
          textprintf_ex(sortie, police, VIE_X, VIE_4_Y, makecol(255, 255, 255), -1, "%2d", joueurs[3]->plus->vies);   
          //Score du joueur
          textprintf_ex(sortie, police, SCORE_X, VIE_4_Y, makecol(255, 255, 255), -1, "%8d", joueurs[3]->plus->score);             
        }
        
        // Nombre de diamants restants
        textprintf_ex(sortie, police, DIAMANTS_X, DIAMANTS_Y, makecol(255, 255, 255), -1, "%d", nb_diamants); 
        

        temps_restant = temps_a_pas_depasser - time(NULL);
        // temps restant
        textprintf_ex(sortie, police, DIAMANTS_X-50, DIAMANTS_Y+200, makecol(255, 255, 255), -1, "%d : %d", (int) temps_restant/60, temps_restant%60); 
    }

    textprintf_ex(sortie, police, 300, 5, makecol(255, 255, 255), -1, "Niveau %d", niveau_actuel); 
    


    draw_sprite(screen, sortie, 0, 0);

// Ca c'est pour quand on aura enfin un son de déflagration
    
    if (yatil_deflagration != last_yatil_deflagration) {
        last_yatil_deflagration = yatil_deflagration;

        pos.x = 128;
        if (yatil_deflagration) joue_son_continue(SON_DEFLAGRATION, pos);
        else stop_son_continue(SON_DEFLAGRATION);
        
    }

    if (yatil_mechant) {
        for (i=1; i <= 4; i++) {
            anim_mechant[i]->image_actuelle++;

            if (anim_mechant[i]->image_actuelle >= anim_mechant[i]->nb_images * anim_mechant[i]->nb_images_pause) 
                anim_mechant[i]->image_actuelle = 0;
        }
    }

    DEBUG ("Fin de l'affichage");

    #ifdef _DEBUG_
    debug_show_objets();
    #endif

    return FALSE;
    
}

// Pour l'affichage d'animation
int draw_animation (animation_t *animation, int x, int y, int angle, char no_move) {
    static BITMAP *image_en_cours; // pas de liberation création (faut que ça aille vite)
    extern BITMAP *sortie;

    DEBUG ("--> draw_animation : %08X", animation);

    if (!animation)
        ERREUR("L'animation a afficher n'existe pas...");

    //DEBUG ("L'animation passee a pour adresse : 0x%08X", animation);
    

    //DEBUG ("Nb : %d",(animation->image_actuelle/animation->nb_images_pause));

    image_en_cours = animation->images[(animation->image_actuelle/animation->nb_images_pause)];
    pivot_sprite(sortie, image_en_cours, x, y, image_en_cours->w/2, image_en_cours->h/2, itofix(angle));
    
    // Si on spécifie no_move
    if (! no_move) animation->image_actuelle++;

    if (animation->image_actuelle >= animation->nb_images * animation->nb_images_pause) 
        animation->image_actuelle = 0;

    DEBUG ("<-- draw_animation");

    return TRUE;
}

// Pour que la vitesse d'affichage soit la même sur tous les ordinateurs
// pause_affichage adapte la pause automatiquement
void pause_affichage(int pause_voulue) {
    static unsigned long int dernier_temps, temps;
    static unsigned int pause = 50;
    
    temps = clock();

    if ( (temps-dernier_temps) < pause_voulue) 
        pause++;
    
    else if ( (temps-dernier_temps) > pause_voulue) {
        if (pause > 0) pause--;
    }

    if (pause) rest(pause);

    //DEBUG ("Pause de %d", pause);

    dernier_temps = temps;
}

