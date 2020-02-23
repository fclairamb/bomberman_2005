#include "bib.h"

#ifndef _CHARGEMENT_NIVEAUX_
#define _CHARGEMENT_NIVEAUX_
//unsigned char mode_editeur;

int chargement_niveau(int niveau_nb) {
    char fichier_nom[200];
    extern unsigned int niveau_actuel;
    sprintf(fichier_nom, "niveaux\\niveau%d.niv", niveau_nb);

    if (niveau_nb != niveau_actuel) niveau_actuel = niveau_nb;

    chargement_fichier(fichier_nom);

}

unsigned int temps_parcours_niveau;
unsigned long int temps_a_pas_depasser;

// Le chargement de fichier permettra de sauvegarder les niveaux, les cartes persos,
// les sauvegarde de parties en cours. Bref... TOUT
int chargement_fichier(char *fichier_nom) {
    
    char ligne[21];
    FILE *fichier;
    coord_c _case;      // pos ne représente pas une position en pixel mais en case (x/y)
    int type;
    int i, j, k;
    int joueur_actuel = 0;
    carre *case_actuelle;
	//static premier_chargement = TRUE;
    //extern unsigned char mode_editeur;


    // Declaration des variables externes
    extern carre cases[NB_CASES_X][NB_CASES_Y];   // carre[NB_CASES_X][NB_CASES_Y]
    extern objet **objets;  // objet *
    extern objet *joueurs[4];
    extern coord_p placement_porte_fin;
    extern unsigned int nb_diamants;
    extern unsigned char mode_editeur;
    objet *p_objet;
    
    extern animation_t *anim_bonhomme, *anim_bombe;

    extern unsigned int temps_parcours_niveau;

    vider_les_pointeurs();

    nb_diamants = 0;

    if (! (fichier = fopen(fichier_nom, "r")) ) {
        DEBUG("Probleme a l'ouverture du fichier de niveau");
        return FALSE;
    }
    // Initialisation de toutes les cases
    for (i = 0; i < NB_CASES_X; i++)
        for (j = 0; j < NB_CASES_Y; j++) {
            case_actuelle = &(cases[i][j]);
            (case_actuelle->objets)[0] = (objet*) NULL;
        }

    
    //Lecture jusqu'à arriver à la fin du fichier
    while (!feof(fichier)) {
        // Ca marche de 3 lignes en 3 lignes
        // case_x, case_y, type : sur chaque ligne
        for (i=0; i < 3 && !feof(fichier); i++) {
            fgets(ligne, 20, fichier);
            if (i == 0)         _case.x = atoi(ligne);
            else if (i == 1)    _case.y = atoi(ligne);
            else if (i == 2)    {
                type = atoi(ligne);
                case_actuelle = &cases[(_case.x)][(_case.y)];
                if (type < 100) { // Tous les types inférieurs à 50 sont des types simples
                    // Les types simples sont tous les objets qui ne peuvent rien faire
                    // genre : mur, bonus, etc.
                    if (type == OBJ_T_DIAMANT) 
                        nb_diamants++;
                    
                    if (type == OBJ_T_END_DOOR && !mode_editeur) {
                        placement_porte_fin = _case;
                    }
                    else {
                        inscrire_objet(_case, type);
                    }
                }
                else if (type >= 1000) {
                    if (type == OBJ_T_TEMPS_PARCOURS) {
                        temps_parcours_niveau = _case.x * 60 + _case.y;
                        temps_a_pas_depasser = time(NULL) + temps_parcours_niveau;
                        DEBUG ("temps_parcours_niveau : %d", temps_parcours_niveau);
                        DEBUG ("Temps sous : %d", temps_a_pas_depasser-time(NULL) );

                    }
                }
                else {
                    /* A noter que l'inscription de l'objet dans la case est gérée
                       par creer_objet. C'est pour cela qu'on a pas a reproduire 
                       les quelques lignes plus haut.
                    */

                    if (type != OBJ_T_BOMBERMAN) {
                        p_objet = creer_objet(type, _case);
                        if (p_objet) DEBUG("OBJ_T_XXXX [ 0x%08X ] / %2d -> %2d x %2d / %d", p_objet, p_objet->type, p_objet->position.x, p_objet->position.y, p_objet->type);
                    
                    }
                    else if (joueur_actuel < nb_joueurs) {
                        joueurs[joueur_actuel]->placement  = _case;
                        joueurs[joueur_actuel]->position   = position_from_placement( _case );
                        joueurs[joueur_actuel]->type       = OBJ_T_BOMBERMAN;

                        //joueurs[joueur_actuel]->plus->vies      = 3;

                        //joueurs[joueur_actuel]->plus->vies = 3;
                        
                        //*(joueurs[joueur_actuel]->animation)  =  *anim_bonhomme;
                        inscrire_objet(_case, joueurs[joueur_actuel]);
                        
                        DEBUG("OBJ_T_BOMB%2d [ 0x%08X ] / %2d -> %2d x %2d", joueur_actuel, joueurs[joueur_actuel], joueurs[joueur_actuel], joueurs[joueur_actuel]->position.x, joueurs[joueur_actuel]->position.y);
                        joueur_actuel++;
                    }
                    
                    
					// Note : inscrire_objet est compris dans creer_objet.

                    if (type == OBJ_T_BOMBE) {
                            p_objet->typec = 1;
                            p_objet->typed = 10;

                    }
                    
                        
                    


                }
                
                
            }
        }
        
        

    }

    // On ferme le fichier de niveau
    DEBUG ("On va fermer le fichier de niveau...");
    if (fichier) fclose(fichier);
    DEBUG ("Le fichier de niveau est ferme...");

    if (!nb_diamants) inscrire_objet(placement_porte_fin, OBJ_T_END_DOOR);

    if (joueur_actuel < nb_joueurs ) {
            for (i=joueur_actuel; i < nb_joueurs; i++) {
                joueurs[i]->placement  = joueurs[0]->placement;
                joueurs[i]->position   = joueurs[0]->position;
                joueurs[i]->type       = OBJ_T_BOMBERMAN;


            }


    }

    return TRUE;
}

int sauvegarde_niveau(int niveau_nb) {
    char fichier_nom[200];
    sprintf(fichier_nom, "niveaux\\niveau%d.niv", niveau_nb);

    sauvegarde_fichier(fichier_nom);

}

int sauvegarde_fichier(char *fichier_nom) {
    
    char ligne[21];
    FILE *fichier;
    coord_c _case;      // pos ne représente pas une position en pixel mais en case (x/y)
    int type;
    int i, j, k;
    int joueur_actuel = 0;
    carre *case_actuelle;
	//static premier_chargement = TRUE;
    //extern unsigned char mode_editeur;


    // Declaration des variables externes
    extern carre cases[NB_CASES_X][NB_CASES_Y];   // carre[NB_CASES_X][NB_CASES_Y]
    extern objet **objets;  // objet *
    extern objet *joueurs[4];
    objet *p_objet;
    
    //extern animation_t *anim_bonhomme, *anim_bombe;


    DEBUG ("*** SAUVEGARDE DU FICHIER %s ***", fichier_nom);



    

    if (! (fichier = fopen(fichier_nom, "w")) ) {
        DEBUG("Probleme a l'ouverture du fichier de niveau");
        return FALSE;
    }
    for (i = 0; i < NB_CASES_X; i++)
        for (j = 0; j < NB_CASES_Y; j++) {
            case_actuelle = &(cases[i][j]);

            for (k = 0; k < NB_OBJ_CASE && (case_actuelle->objets)[k]; k++) {
                if ( (int) (case_actuelle->objets)[k] < 100) 
                    fprintf(fichier, "%d\n%d\n%d\n", i, j, (int) (case_actuelle->objets)[k]);
                else  {
                    if ((case_actuelle->objets)[k]->type == OBJ_T_BIENTOT)  
                        fprintf(fichier, "%d\n%d\n%d\n", i, j, (case_actuelle->objets)[k]->typeb);
                    else
                        fprintf(fichier, "%d\n%d\n%d\n", i, j, (case_actuelle->objets)[k]->type);
                }
            }
            
            
        }

    for (i=0; i < nb_joueurs; i++) 
        fprintf(fichier, "%d\n%d\n%d\n", joueurs[i]->placement.x, joueurs[i]->placement.y, OBJ_T_BOMBERMAN);

    fprintf (fichier, "%d\n%d\n%d\n", temps_parcours_niveau/60, temps_parcours_niveau%60, OBJ_T_TEMPS_PARCOURS);

    // On ferme le fichier de niveau
    DEBUG ("On va fermer le fichier de niveau...");
    if (fichier) fclose(fichier);
    DEBUG ("Le fichier de niveau est ferme...");

    return TRUE;
}

#endif
