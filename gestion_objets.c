#include "bib.h"

//...
// Declaration des variables nécessaires à la gestion de tous les objets
objet **objets;      // Tous les objets
int nb_obj = 0;     // Nombre d'objets
int nb_obj_a = 0;   // Nombre d'objets alloués

carre cases[NB_CASES_X][NB_CASES_Y];

objet *humains[4];
objet *ordis[4];

coord_p placement_porte_fin;

// Pour allouer un objet en mémoire
objet * creer_objet (int type, coord_c _case) {
    extern objet **objets;
    extern animation_t *anim_bonhomme, *anim_bombe, *anim_deflagration;
    extern int nb_obj, nb_obj_a;

    objet *nouv_objet, *pointeur;

    int i, j, k;

    DEBUG ("---=== creer_objet ===---");

    nouv_objet = malloc ( sizeof(objet) );

    if (type == 0) {
        ERREUR("Are you CRAZY ??? I'm gonna blow for that !");
        return NULL;
    }

    if (type != OBJ_T_DEFLAGRATION && yatil_obstacle_case(_case)) {
        DEBUG ("Ce n'est pas un type deflagration et il y a un obstacle");
        return NULL;
    
    }
    else if ( 
                type == OBJ_T_DEFLAGRATION &&
                ( 
                    _case.x < 0 || 
                    _case.y < 0 || 
                    _case.x >= NB_CASES_X || 
                    _case.y >= NB_CASES_Y || 
                    yatil_cet_objet_case(_case, (objet *) OBJ_T_INCASSABLE) || 
                    yatil_cet_objet_case(_case, (objet *) OBJ_T_END_DOOR)
                 )
             ) 
                {
                DEBUG ("C'est un type deflagration qui a rencontre un type incassable");
                return NULL;


    }
    if (nb_obj < nb_obj_a) { // Si on a de l'espace dans ce qui a déja été alloué
        DEBUG ("Il reste de l'espace pour l'objet");
       
        i = -1;
        do {
            i++;
            pointeur = objets[i];
        // Si on se plante, ça BUG
        } while ( pointeur != NULL);

        objets[i] = nouv_objet;
        // On recupere donc un pointeur vide
    }
    else { // Sinon, il faut allouer
        DEBUG ("Il faut allouer de l'espace");
        if (nb_obj_a == 0) {
            //printf ("Premiere allocation");
            objets = (objet **) malloc( sizeof(objet *) );
            nb_obj_a = 1;
			// On a UN objet alloué
        }
        else {
            //printf ("Reallocation necessaire");
            objets = (objet **) realloc(objets, sizeof(objet *) * (++nb_obj_a));
			// Ca fait un objet alloué de plus
        }
            
        objets[(nb_obj_a-1)] = nouv_objet;
        

    }       

    nb_obj++; // Ca fait un objet de plus



    DEBUG ("Les objets sont : %d / %d", nb_obj, nb_obj_a);

    // On s'occupe de rentrer les données dans l'objet
    nouv_objet->type = type;
    
    nouv_objet->position = position_from_placement( _case );
    DEBUG("L'objet cree a pour adresse : %08X", nouv_objet);
    DEBUG ("Les position de l'objet sont : %3d x %3d", nouv_objet->position.x, nouv_objet->position.y);
    

    nouv_objet->placement = _case;

    
    nouv_objet->animation = malloc( sizeof(animation_t) );

    // On inscrit bien l'objet dans une case
    if ( !inscrire_objet(_case.x, _case.y, nouv_objet) 
//          || (type == OBJ_T_BOMBE && case_nb_objet(_case) > 1)
        )
        {

        DEBUG ("L'objet n'a pas pu etre inscrit...");
        
        // N'IMPORTE QUOI : y'avait un simple free

        detruire_objet( nouv_objet );
        DEBUG ("L'objet a ete libere...");
        
        //nb_obj--; //<-- BORDEL : le nb_obj était déja inclus dans le detruire_objet();
       
    
        // ATTENTION :
       //objets[i] = NULL;
        
        return NULL;
    }
    

    // On s'occupe de rendre l'objet cohérent
    DEBUG ("Attribution de l'animation de l'objet...");
    
    
    if (type == OBJ_T_BOMBE) {
        *(nouv_objet->animation) = *anim_bombe;
        nouv_objet->typeb = (unsigned long int) nouv_objet->type;
        nouv_objet->type = OBJ_T_BIENTOT;
        
        // Puissance de déflagration par défaut
        nouv_objet->typec = 3;
        
        DEBUG ("Creer objet OBJ_T_BOMBE : %d, %d", (int) nouv_objet->type, (int) nouv_objet->typeb);
    }
    else if (type == OBJ_T_BOMBERMAN) {
        ERREUR ("Creer NE DOIT PLUS S'OCCUPER DES BOMBERMAN");
        
        
    }
    else if (type == OBJ_T_DEFLAGRATION) {
        DEBUG ("On copie l'animation de deflagration");
        *(nouv_objet->animation) = *anim_deflagration;
    }
    else if (type == OBJ_T_MECHANT) {
        nouv_objet->direction = DIRECTION_HAUT;
        //nouv_objet->animation->nb_images_pause = 20;

    }
    


    
    DEBUG ("OBJET FINI !!!");

    return nouv_objet;

}

// Pour faire disparaître un objet 
int detruire_objet (objet *futur_mort) {
    int i;
    DEBUG ("--> detruire_objet : %08X", futur_mort);
    for (i =0; i<nb_obj_a; i++) {
        DEBUG("* %08X     [%2d / %2d / %2d]", objets[i], i, nb_obj, nb_obj_a);
        if (objets[i] == futur_mort) break;
    }

    // On s'est peut-être planté, le pointeur n'existe pas
    if (objets[i] != futur_mort) {
        DEBUG ("On a pas trouve le pointeur correspondant...");
        return FALSE;
    }
    
    DEBUG ("On libere l'animation");
    free(futur_mort->animation);

    DEBUG ("On libere l'objet en lui même");
    
    // On libère l'objet
    free(futur_mort);

    // On nettoie son emplacement
    objets[i] = NULL;
    
    // Et ca fait un objet de moins (le nombre d'ob
    nb_obj--;

    DEBUG ("=== detruire_objet OK : %08X", futur_mort);

    DEBUG ("<-- detruire_objet");

    return TRUE;

    
}

// Pour inscrire un pointeur dans une case 
int inscrire_objet (coord_c _case, objet * pointeur) {
    

    extern carre cases[NB_CASES_X][NB_CASES_Y];   // carre[NB_CASES_X][NB_CASES_Y]
    extern unsigned int nb_diamants;
    extern unsigned char passer_au_niveau_suivant;
    carre *la_case;


    //objet *test;

    int i=0; //ok = FALSE;

    
    DEBUG ("inscrire objet : %08X", pointeur);

    la_case = &(cases[_case.x][_case.y]);

    if ( (int) pointeur >=100 ) { 
         if (pointeur->type == OBJ_T_BOMBE && case_nb_objet(_case) > 1)
            return FALSE;
         else if (pointeur->type == OBJ_T_BOMBERMAN) { 
            if ( yatil_cet_objet_case(_case, OBJ_T_DIAMANT) ) {
                pointeur->plus->score+= BONUS_DIAMANT;
                joue_son(SON_DIAMANT, _case);
                nb_diamants--;
                desinscrire_objet(_case, (objet *) OBJ_T_DIAMANT);
                if (nb_diamants <= 0) {
                    inscrire_objet(placement_porte_fin, (objet *) OBJ_T_END_DOOR);
                }
            }
            else if ( yatil_cet_objet_case(_case, OBJ_T_END_DOOR) ) {
                passer_au_niveau_suivant = TRUE;
                joue_son(SON_BONUS, _case);
                pointeur->plus->score+= BONUS_PORTE;
            }
            else if ( yatil_cet_objet_case(_case, OBJ_T_BONUS_BOMBE_PUISSANCE) ) {
                pointeur->plus->puissance_bombe++;
                joue_son(SON_BONUS, _case);
                desinscrire_objet(_case, (objet *) OBJ_T_BONUS_BOMBE_PUISSANCE);
            }
            else if ( yatil_cet_objet_case(_case, OBJ_T_BONUS_BOMBE_NOMBRE) ) {
                pointeur->plus->max_nb_bombes++;
                joue_son(SON_BONUS, _case);
                desinscrire_objet(_case, (objet *) OBJ_T_BONUS_BOMBE_NOMBRE);
            }
            else if ( yatil_cet_objet_case(_case, OBJ_T_BONUS_VIE) ) {
                pointeur->plus->vies++;
                joue_son(SON_BONUS, _case);
                desinscrire_objet(_case, (objet *) OBJ_T_BONUS_VIE);
            }
            else if (yatil_cet_objet_case(_case, OBJ_T_BONUS_VITESSE) ) {
                pointeur->plus->vitesse += 4;
                joue_son(SON_BONUS, _case);
                desinscrire_objet(_case, (objet *) OBJ_T_BONUS_VITESSE);

            }
         }
        
         
       
    }
       

    for (i=0; i<9; i++) {
        if (!la_case->objets[i]) {
            la_case->objets[(i)] = pointeur;
            la_case->objets[(i+1)] = NULL;
            //ok = TRUE;
            return TRUE;
        }
    }

    //DEBUG ("inscrire / objet : OK vaut : %d", ok);
    return FALSE;

}

// Pour desinscrire un pointeur d'une case
int desinscrire_objet (coord_c _case, objet * pointeur) {
    extern carre cases[NB_CASES_X][NB_CASES_Y];   // carre[NB_CASES_X][NB_CASES_Y]
    extern unsigned char mode_editeur;
    carre *la_case;

    //DEBUG ("---=== DESINSCRIRE OBJET ===---");
    DEBUG ("desinscrire_objet : %08X", pointeur);
    

    int i=0, j, k;

    la_case = &(cases[_case.x][_case.y]);
    
    /*if ( (int) pointeur >= 100 && pointeur->type == OBJ_T_BOMBERMAN) {
        pointeur->position.x = 0; pointeur->position.y = 0;
    }*/

    if (mode_editeur && (int) pointeur >= 100 && pointeur->type == OBJ_T_BOMBERMAN) pointeur->position.x = 0;

    // Tant qu'on a pas atteint l'objet, i s'incrémente
    for (i=0; i < NB_OBJ_CASE && la_case->objets[i]; i++) {
        DEBUG ("On attend l'objet : %08X, %i", la_case->objets[i], i);
        if (la_case->objets[i] == pointeur) break;

    }
    
    if (!la_case->objets[i]) {
        DEBUG ("On a pas trouve l'objet a desinscrire");
        return FALSE;
    }
    // On decale tout en écrasant l'objet en cours
    for (j=i; j < 9 ; j++) {
        DEBUG ("On colle le contenu de %2d dans %2d : %08X -> %08X", (j+1), j, la_case->objets[(j+1)], la_case->objets[j]);
        la_case->objets[j] = la_case->objets[(j+1)];

        // si on écrit le caractère de fin, on arrête la boucle
        if (la_case->objets[j] == NULL) {
            DEBUG ("On a ecrit le caractère de fin");
            return TRUE;
        }

    }
    
    // YAHOO! Vive le debug, j'ai retrouve une petite erreur qui faisait disparaitre
    // les bombermans sans raison.

    DEBUG ("/// desinscrire_objet");
    // Si on arrive jusqu'ici, c'est que ça a foiré, donc que l'objet n'est pas présent
    // dans cette case ou qu'il n'y a pas de pointeur NULL à la fin
    return FALSE;
}

// A prioris, on ne liberera jamais d'animation, donc on se prend pas la tête avec ça


// Pour bouger un objet
    // Cette fonction a pour principal but de desinscrire l'objet d'une case et 
    // le reinscrire dans une autre case

    // Elle retourne VRAI si ça marche
    // FALSE si ça veut pas (genre obstacle)
int bouge_objet(objet * pointeur, int direction, int avancement) {
    extern unsigned char passer_au_niveau_suivant;

    coord_p position_test;
    coord_p position_save;
    int retour;
    coord_c test;
    coord_c before;
    int t;
    //int case_x, case_y;

    DEBUG ("Etape A");

    before = pointeur->placement;

    //Copie des données initiales dans la structure servant pour le test de collision
    position_test = pointeur->position;

    
    
    if      (direction == DIRECTION_DROITE) position_test.x = pointeur->position.x + avancement;
    else if (direction == DIRECTION_GAUCHE) position_test.x = pointeur->position.x - avancement;
    else if (direction == DIRECTION_BAS)    position_test.y = pointeur->position.y + avancement;
    else if (direction == DIRECTION_HAUT)   position_test.y = pointeur->position.y - avancement;

    position_save = position_test;


    retour = 0;
    
    position_test.x-=OBJ_LAR/2; position_test.y-=OBJ_LAR/2; // Position haut gauche
    if ((direction == DIRECTION_HAUT || direction == DIRECTION_GAUCHE) && (t=yatil_obstacle_pos(position_test)) ) {
        retour++;
        //if (t == OBJ_T_END_DOOR) passer_au_niveau_suivant = TRUE;
    }

    position_test.x+=OBJ_LAR; // Position haut droite
    if ((direction == DIRECTION_HAUT || direction == DIRECTION_DROITE) && (t=yatil_obstacle_pos(position_test)) ) { 
        retour++;
        //if (t == OBJ_T_END_DOOR) passer_au_niveau_suivant = TRUE;
    }

    position_test.y+=OBJ_LAR; // Position bas droite
    if ((direction == DIRECTION_BAS || direction == DIRECTION_DROITE) && (t=yatil_obstacle_pos(position_test)) ) {
        retour++;
        //if (t == OBJ_T_END_DOOR) passer_au_niveau_suivant = TRUE;
    }

    position_test.x-=OBJ_LAR; // Position bas gauche
    if ((direction == DIRECTION_BAS || direction == DIRECTION_GAUCHE) && (t=yatil_obstacle_pos(position_test)) ) {
        retour++;
        //if (t == OBJ_T_END_DOOR) passer_au_niveau_suivant = TRUE;
    }
                    
                    


    // Si on a réussi tous les tests
    if (!retour) {
        pointeur->position = position_save;
                    
        // On regarde à quelle case correspond la nouvelle position
        test = case_correspondante(position_save);

        // Si la nouvelle case est différente de celle d'avant
        if ( (test.x != before.x) || (test.y != before.y) ) {
            DEBUG ("On doit desinscrire et reinscrire l'objet");
            desinscrire_objet(pointeur->placement, pointeur);
            inscrire_objet(test, pointeur);
            pointeur->placement = test;
        }
                    
    }
    if (retour) return FALSE;
    else return TRUE;
}

// Case correspondante à une position
coord_c case_correspondante(coord_p position) {
    static coord test;
    static coord retour;

    if (position.x < POS_CASES_X)
        retour.x = -1;
    else if (position.y < POS_CASES_Y)
        retour.y = -1;
    else {
        test.x = position.x - POS_CASES_X;
        test.y = position.y - POS_CASES_Y;

        retour.x = test.x / LAR_CASE_L;
        retour.y = test.y / LAR_CASE_L;
    }
    DEBUG ("case_correspondante : %d x %d --> %d x %d", position.x, position.y, retour.x, retour.y);

    // Puisque j'ai appris que les structures étaient des scalaires
    return retour;
}

int yatil_mort_ici(coord_c _case) {
    extern carre cases[NB_CASES_X][NB_CASES_Y];   // carre[NB_CASES_X][NB_CASES_Y]
    carre *la_case;
    int i=0, j, k;
    objet *pointeur;   

    la_case = &(cases[_case.x][_case.y]);

    for (i=0; i < 10; i++) {
        pointeur = la_case->objets[i];
        //j = (int) pointeur;
        //DEBUG ("Ca va bien...");
        if (!pointeur) return FALSE; // On arrive a la fin de la case, et pas de probleme
        else if ( (int) pointeur >= 100) {
            if (pointeur->type == OBJ_T_DEFLAGRATION) return TRUE;
            else if (pointeur->type == OBJ_T_MECHANT) return TRUE;
        }
    }

    // Si on arrive jusqu'ici c'est qu'il n'y a pas de pointeur NULL final
    ERREUR("ERREUR FATALE, en fait non, mais bon, comme ça devrait pas arriver on va dire que si !");
    return FALSE; // Ligne decoratrice
}

int yatil_cet_objet_case(coord_c _case, objet *objet) {
    extern carre cases[NB_CASES_X][NB_CASES_Y];   // carre[NB_CASES_X][NB_CASES_Y]
    carre *la_case;
    int i;
    DEBUG ("Appel de : yatil_cet_objet_case");
    
    
    la_case = &(cases[_case.x][_case.y]);

    for (i=0; i < NB_OBJ_CASE; i++) {
        if (la_case->objets[i] == objet) return TRUE;
        else if ( ! la_case->objets[i] ) break;
    }

    DEBUG ("fin de : yatil_cet_objet_case");
    return FALSE;

}   

int yatil_cet_objet_type(coord_c _case, int type) {
    extern carre cases[NB_CASES_X][NB_CASES_Y];   // carre[NB_CASES_X][NB_CASES_Y]
    carre *la_case;
    int i;
    DEBUG ("Appel de : yatil_cet_objet_case");
    
    
    la_case = &(cases[_case.x][_case.y]);

    for (i=0; i < NB_OBJ_CASE && la_case->objets[i]; i++) {
        if ( type >= 100 && (int) la_case->objets[i] >=  100 && la_case->objets[i]->type == type) return TRUE;
        else if ( (int) la_case->objets[i] == type) return TRUE;
    }

    DEBUG ("fin de : yatil_cet_objet_case");
    return FALSE;

}



int yatil_obstacle_case(coord_c _case) {
    extern carre cases[NB_CASES_X][NB_CASES_Y];   // carre[NB_CASES_X][NB_CASES_Y]
    carre *la_case;
    int i=0, j, k;
    objet *pointeur;

    if (_case.x < 0 || _case.y < 0 || _case.x >= NB_CASES_X || _case.y >= NB_CASES_Y )
        return TRUE;    

    la_case = &(cases[_case.x][_case.y]);

    DEBUG ("pointeur sur la_case defini");
    for (i=0; i < NB_OBJ_CASE; i++) {
        pointeur = la_case->objets[i];
        //j = (int) pointeur;
        //DEBUG ("Ca va bien...");
        if (!pointeur) return FALSE; // On arrive a la fin et pas d'obstacle
        else if ( (int) pointeur == OBJ_T_MUR 
               || (int) pointeur == OBJ_T_INCASSABLE
               || (int) pointeur == OBJ_T_BOMBERMAN
               || (int) pointeur == OBJ_T_BIENTOT
               //|| (int) pointeur == OBJ_T_END_DOOR
                ) return (int) pointeur; // il y'a un obstacle
        else if ( (int) pointeur >= 100) {
            if (pointeur->type == OBJ_T_BOMBE) return pointeur->type;
        }
    }

    // Si on arrive jusqu'ici c'est qu'il n'y a pas de pointeur NULL final
    ERREUR("ERREUR FATALE, en fait non, mais bon, comme ça devrait pas arriver on va dire que si !");
    return FALSE; // Ligne decoratrice
}

int yatil_obstacle_pos(coord_p pos) {
    coord_c _case;

    _case = case_correspondante(pos);

    return yatil_obstacle_case(_case);
}

int faire_disparaitre(objet *objet) {
    DEBUG ("-->faire_disparaitre : %08X", objet);
    if (!desinscrire_objet(objet->placement, objet)) {
        DEBUG ("ERREUR A LA DESINSCRIPTION DE L'OBJET");
        rest(5000);
    }
    if (!detruire_objet(objet)) {
        DEBUG ("ERREUR A LA DESTRUCTION DE L'OBJET");
        rest(5000);
    }

}

int give_life (objet *objet) {
    struct st_objet *p_objet;
    extern animation_t *anim_deflagration, *anim_bonhomme, *anim_bonhomme_mort;
    extern int nb_joueurs;
    extern struct st_objet *joueurs[NB_JOUEURS_MAX];
    unsigned long int tmp;
    int i;
    coord_c placement;
    DEBUG ("give_life est invoque...");
    if (objet->type == OBJ_T_BOMBE) {
        DEBUG ("Le typeb vaut : %d", objet->typeb);
        if (objet->typeb <= 0) {
            //faire_disparaitre(objet);

            // On transforme l'objet en deflagration 
            objet->type = OBJ_T_DEFLAGRATION;

            *(objet->animation) = *anim_deflagration;
            
            joue_son(SON_EXPLOSION, objet->placement);

            // On lui donne 40 tours de vie
            objet->typeb = 40;

            // Direction
            objet->direction = 0;
            

            // ATTENTION_V2 : L'objet n'est pas supprimé que je sache
            return TRUE;
        }
        else 
            objet->typeb--; // La pauvre petite vie de l'objet disparaît petit à petit
    }
    else if (objet->type == OBJ_T_DEFLAGRATION ) {
        DEBUG ("give_life : objet->type == OBJ_T_DEFLAGRATION");
        if (objet->typeb == 30 && objet->typec > 0) {
            DEBUG ("On cree la deflagration de droite");
            if (!objet->direction || objet->direction == DIRECTION_DROITE) {
                placement = objet->placement;
                placement.x++;
                p_objet = creer_objet(OBJ_T_DEFLAGRATION, placement);
                if (p_objet) {
                    p_objet->direction = DIRECTION_DROITE;
                    p_objet->typeb = 40;
                    p_objet->typec = objet->typec - 1;
                    p_objet->typed = objet->typed;
                }
                
            }
            if (!objet->direction || objet->direction == DIRECTION_GAUCHE) {
                DEBUG ("On cree la deflagration de gauche");
                placement = objet->placement;
                placement.x--;
                p_objet = creer_objet(OBJ_T_DEFLAGRATION, placement);
                if (p_objet) {
                    p_objet->direction = DIRECTION_GAUCHE;
                    p_objet->typeb = 40;
                    p_objet->typec = objet->typec - 1;
                    p_objet->typed = objet->typed;
                }
                
            }
            if (!objet->direction || objet->direction == DIRECTION_BAS) {
                DEBUG ("On cree la deflagration du bas");
                placement = objet->placement;
                placement.y++;
                p_objet = creer_objet(OBJ_T_DEFLAGRATION, placement);
                if (p_objet) {
                    p_objet->direction = DIRECTION_BAS;
                    p_objet->typeb = 40;
                    p_objet->typec = objet->typec - 1;
                    p_objet->typed = objet->typed;
                }
                
            }
            if (!objet->direction || objet->direction == DIRECTION_HAUT) {
                DEBUG ("On cree la deflagration du haut");
                placement = objet->placement;
                placement.y--;
                p_objet = creer_objet(OBJ_T_DEFLAGRATION, placement);
                if (p_objet) {
                    p_objet->direction = DIRECTION_HAUT;
                    p_objet->typeb = 40;
                    p_objet->typec = objet->typec - 1;
                    p_objet->typed = objet->typed;
                }
                
            }
            
        }
        else if (objet->typeb == 31) {
            DEBUG ("On VA DEMOLIR UN MUR");
            if (desinscrire_objet(objet->placement,  (void *) OBJ_T_MUR)) {
                objet->typec = 0;
                DEBUG ("Le mur a bien été démoli.");
                //if (yatil
            }
            else DEBUG ("Le mur n'a pas pu être démoli.");
        }
        else if (objet->typeb <= 0) {
            faire_disparaitre(objet);
            return FALSE; // "give_life m'a tuer"
            // Si on ne renvoie pas FALSE, affichage va croire que l'objet existe encore
            // et il va l'afficher => GROS BUG
        }
        objet->typeb--;

    } // fin de la gestion des OBJ_T_DEFLAGRATION

    else if (objet->type == OBJ_T_BOMBERMAN) {
        if (yatil_mort_ici(objet->placement) ) {
            *(objet->animation) = *anim_bonhomme_mort;
            objet->type = OBJ_T_BOMBERMAN_MORT;
            objet->plus->vies--;
            joue_son(SON_BOMBERMAN_MEURT, objet->placement );
            // 50 tours de vie avant de disparaitre
            objet->typeb = 50;    

            objet->direction=DIRECTION_ARRET;
            objet->plus->puissance_bombe=1;
            objet->plus->vitesse = 10;
            objet->plus->max_nb_bombes=1;
            objet->plus->nb_bombes=0;
        }

    }
    else if (objet->type == OBJ_T_BOMBERMAN_MORT) {
        if (objet->typeb <= 0) { // Si le bomberman mort doit changer d'état
            if (objet->plus->vies > 0) { // Si il lui reste encore des vies
                if (yatil_mort_ici(objet->placement)) {// Si il ne peut pas réaparaitre
                    DEBUG ("On retente plus tard");
                    objet->plus->no_move = TRUE;
                    objet->typeb=2; // Il retentera dans 2 tours
                }
                else { // Sinon,
                    DEBUG ("Le bomberman redevient lui même");
                    //*objet->animation = *anim_bonhomme;
                    objet->type       = OBJ_T_BOMBERMAN;
                }
            }
            else { // Sinon (si plus de vie), il disparait
                desinscrire_objet(objet->placement, objet);
                objet->typec = OBJ_T_BOMBERMAN_MORT;
                return FALSE;
            }
            
            
        }
        
        else objet->typeb--;

    }
    else if (objet->type == OBJ_T_BIENTOT) {
        DEBUG ("give_life . objet de type BIENTOT");
        DEBUG ("give_life voit %d qui veut devenir %d", objet->type, objet->typeb);
        if ( case_nb_objet (objet->placement) == 1 ) {
            DEBUG ("La case est vide");
            //tmp = objet->typeb;
            objet->type = (unsigned char) objet->typeb;

        }
        //else //DEBUG ("La case est pleine : %d", case_nb_objet (objet->placement));

        if (objet->type == OBJ_T_BOMBE) {
            objet->typeb = 40; // On redonne a la bombe la valeur qu'elle devrait avoir
        }
        //else objet->typeb = 40;
    }
    else if (objet->type == OBJ_T_MECHANT) {
            // S'il ne peut pas bouger
            if (! bouge_objet(objet, objet->direction, 2) ) {
                // il change de direction
                objet->direction = objet->direction%4 + 1;
                
            }
            if ( yatil_cet_objet_type(objet->placement, OBJ_T_DEFLAGRATION) ) {
                joue_son(SON_BOMBERMAN_MEURT, objet->placement );
                desinscrire_objet(objet->placement, objet);
                
                // ATTENTION_V1
                DEBUG ("On demolit le mechant");
                detruire_objet(objet);
                
                if (objet->typed < 10 && objet->typed < nb_joueurs) {
                    DEBUG ("Voyons voir si le mechant peut rapporter des points au joueur : %d", objet->typed);
                    joueurs[(objet->typed)]->plus->score+=50;
                    DEBUG ("mechant tue : %d points pour le joueur %d.", BONUS_MECHANT, objet->typed);
                }
                else DEBUG ("On ne comptabilise pas le mechant tue...");
                return FALSE;
            }
         
        
    }

    DEBUG ("<-- gife_life");
    // Give life a peut-être fait une modif mais l'objet est bien vivant
    return TRUE;

}


// Cette fonction libère les pointeurs de la matrice
// MAIS : Elle doit travailler sur une matrice propre
int vider_les_pointeurs () {
    unsigned int i, j, k;
    extern carre cases[NB_CASES_X][NB_CASES_Y];
    carre *case_actuelle;
    
    DEBUG ("--> vider_les_pointeurs()");

    for (i = 0; i < NB_CASES_X; i++)
        for (j = 0; j < NB_CASES_Y; j++) {
            case_actuelle = &(cases[i][j]);

            for (k = 0; k < NB_OBJ_CASE && (case_actuelle->objets)[k]; k++) {
                if ( (int) (case_actuelle->objets)[k] >= 100 && 
                     (int) (case_actuelle->objets)[k]->type != OBJ_T_BOMBERMAN && 
                     (int) (case_actuelle->objets)[k]->type != OBJ_T_BOMBERMAN_MORT
                   ) 
                    faire_disparaitre( (case_actuelle->objets)[k] );
            }
            
            
        }

    DEBUG ("<-- vider_les_pointeurs()");

}

int nettoyer_carte() {
    vider_les_pointeurs();
    unsigned int i, j;
    for (i = 0; i < NB_CASES_X; i++)
        for (j = 0; j < NB_CASES_Y; j++)
            cases[i][j].objets[0] = NULL;

}

int vider_la_case (coord_p _case) {
    int k;
    extern carre cases[NB_CASES_X][NB_CASES_Y];
    int nb_bombermans = 0;
    carre *case_actuelle;
    
    case_actuelle = &(cases[_case.x][_case.y]);

    for (k = 0; k < NB_OBJ_CASE && (case_actuelle->objets)[k]; k++) {
                if ( (int) (case_actuelle->objets)[k] >= 100) {
                    // les bombermans ne doivent jamais êtres détruits
                    if ((case_actuelle->objets)[k]->type == OBJ_T_BOMBERMAN) {
                            (case_actuelle->objets)[k]->position.x = 0;
                            (case_actuelle->objets)[k]->position.y = 0;
                            (case_actuelle->objets)[k] = NULL;
                            nb_bombermans++;
                    }
                    else faire_disparaitre( (case_actuelle->objets)[k] );
                }
                else (case_actuelle->objets)[k] = NULL;
    }

    return nb_bombermans;
}

int case_nb_objet(coord_p _case) {
    extern carre cases[NB_CASES_X][NB_CASES_Y];
    int i;
    carre *la_case;
    
    DEBUG ("Appel de case_nb_objet");

    la_case = &(cases[_case.x][_case.y]);

    for (i=0; i < NB_OBJ_CASE && la_case->objets[i]; i++) {
        if (la_case->objets[i] == NULL) break;
        else DEBUG ("Objet %d : %08X", i, la_case->objets[i]);

    }
    return i;

}

coord_p position_from_placement (coord_c _case) {
    coord_p position;
    position.x = POS_CASES_X + LAR_CASE_L * _case.x  + (LAR_CASE_L / 2);
    position.y = POS_CASES_Y + LAR_CASE_L * _case.y  + (LAR_CASE_L / 2);

    return position;
}

int place_un_bonus() {
    
  extern carre cases[NB_CASES_X][NB_CASES_Y];   // carre[NB_CASES_X][NB_CASES_Y]
  coord_p _case;
  carre *la_case;
  unsigned int type;
  unsigned char boucle = TRUE;

  while (boucle) 
  {
    _case.x = rand()%NB_CASES_X;
    _case.y = rand()%NB_CASES_Y;
    type = OBJ_T_BONUS_VIE + rand()%4;

    DEBUG ("place_un_bonus: %3d x %3d --> %3d", _case.x, _case.y, type);

    la_case = &(cases[_case.x][_case.y]);

    if (la_case->objets[0] == NULL)
        {
          inscrire_objet(_case, (objet *) type);
          joue_son(SON_APPARITION_BONUS, _case);
          boucle = FALSE;
        }
  }
    
  return TRUE;
}

