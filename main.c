#include "bib.h"


// N'importe quelle fonction peut demander à passer au niveau suivant
unsigned char passer_au_niveau_suivant;

// Le niveau actuel
unsigned int niveau_actuel;

/*******************************************************************************
*** JE CROIS QUE LA ON PEUT PARLER DE NIGHTLY BUILD                          ***
*******************************************************************************/


int main(int argc, char *argv[]) {

    /***************************************************************************
    ***           V A R I A B L E S    E X T E R N E S                       ***
    ***************************************************************************/

    // La matrice contenant la structure "carre"
    extern carre cases[NB_CASES_X][NB_CASES_Y];   // carre[NB_CASES_X][NB_CASES_Y]
    
    // Le tableau d'objets
    extern objet **objets;  // objet *
    
    // La bitmap de sortie pour l'affichage
    extern BITMAP  *sortie;  // BITMAP *

    // Le tableau des joueurs (qui sont 4 au maximum)
    extern objet *joueurs[NB_JOUEURS_MAX];

    // Le nombre de joueurs
    extern int nb_joueurs ;
    
    // Le nombre d'objets alloués
    // CAD : Au moins autant que le nombre de joueurs utilisés
    extern nb_obj_a;

    // Si n'importe quelle fonction demande à passer au niveau suivant au prochain tour
    extern unsigned char passer_au_niveau_suivant;

    // Le nombre de joueurs 
    extern int nb_joueurs;

    // Les animations à stocker
    extern animation_t *anim_bonhomme;
    
    //fichier dat pour la police de caractères
    extern DATAFILE *dat;
    
    //musique du jeu
    extern MIDI *mid_musique;

    //temps alloué pour parcourir le niveau, récupéré au chargement du niveau
    extern unsigned int temps_parcours_niveau;

    
    //temps recuperé au chargement de niveau , dépend de temps_parcours_niveau
    extern unsigned long int temps_a_pas_depasser;

    /***************************************************************************
    ***    L E S    V A R I A B L E S    D U    M A I N                      ***
    ***************************************************************************/
    
    // Le niveau actuel
    extern unsigned int niveau_actuel;
    
    // Les tours qui défiles
    unsigned int tours = 1; //(pour que les modulos ne soit pas egaux à 0)

    // Le nombre de joueurs qui sont morts
    unsigned int nb_joueurs_morts = 0;

    // La variable de fermeture de boucle
    unsigned char boucle = TRUE;

    // La variable pour pas qu'il y ai de limite de temps
    unsigned char pas_de_timeout = FALSE;
    
    // Des coordonnées de case temporaires
    coord_c coor_tmp;

    // Des coordonnées de position temporaires
    coord_p tmp_pos;

    // Des variables temporaires
    int i, j, k;

    // La variable récupérant la sélection du menu
    int selection;

    // Un pointeur sur case (pour simplifier le code)
    carre *case_actuelle;

    // Pour savoir si une touche est appuyée ou pas
    int appuie[NB_JOUEURS_MAX];

    // Pour les touches
    int touches[NB_JOUEURS_MAX][10]; // Pour l'instant, on utilise que 5 touches

    // La variable qui récupère le choix du menu de départ
    int choix_menu;

    // Un pointeur sur objet (pour simplifier le code)
	objet *p_objet;


    // Pour la gestion des arguments
    char *p_ch; int nb = 1;

    // Variables spécialement héritées de la gestion des arguments
    char lance_editeur = FALSE, pas_de_menu = FALSE;

    // on définit le niveau de départ
    niveau_actuel = 1;
    
    // Ceci n'est activé que si le debuggage est activé...
    #ifdef _DEBUG_

    // Si on est en mode DEBUG, on ouvre le fichier
    fi_debug = fopen("debug_auto.txt", "w");
    #endif

    // Initialisation de la génération des nombres aléatoires
    srand(time(NULL) + getpid());

    /***************************************************************************
    ***      G E S T I O N    D E S    A R G U M E N T S                     ***
    ***************************************************************************/
    DEBUG ("Lecture des arguments de la ligne de commande");
    // Tant qu'on a pas parcouru tous les arguments de la ligne de commande
    while (nb < argc) {
       p_ch = argv[nb++];
       // Pour spécifier le nombre de joueurs
       if (strcmp(p_ch, "-j") == 0) {
           nb_joueurs = atoi( argv[nb++] );
           DEBUG ("LC : Nombre de joueurs : %d", nb_joueurs);				
       }
       // Pour spécifier le niveau
       else if (strcmp(p_ch, "-n") == 0) {
           niveau_actuel = atoi( argv[nb++] );
           DEBUG ("LC : Niveau actuel : %d", niveau_actuel);
       }
       // Pour ne pas utiliser le menu
       else if (strcmp(p_ch, "-m") == 0) {
           pas_de_menu = TRUE;
           DEBUG ("LC : Pas de menu");
       }
       // Pour lancer l'editeur
       else if (strcmp(p_ch, "-e") == 0) {
		     DEBUG ("LC : Editeur active");
		     lance_editeur = TRUE;
             pas_de_menu = TRUE;
       }
       // Pour lancer l'editeur
       else if (strcmp(p_ch, "-t") == 0) {
		     DEBUG ("LC : Pas de timeout");
		     pas_de_timeout = TRUE;
       }
       else if (strcmp(p_ch, "/?") == 0 || strcmp(p_ch, "-h") == 0 || strcmp(p_ch, "--help") == 0) {
            puts("\n"
                 "\tLes options de la ligne de commandes :\n"
                 "\t======================================\n"
                 "\t -j <nombre>          Pour choisir le nombre de joueurs\n"
                 "\t -n <niveau>          Pour choisir le niveau a charger\n"
                 "\t -m                   Pour ne pas afficher le menu\n"
                 "\t -e                   Pour lancer l'editeur de niveau\n");
            exit(0);
       }
       // Sinon : Il y a une erreur
       else {
            printf ("Option : '%s' non reconnue...\n"
                    "Tapez --help pour avoir l'aide du programme\n", p_ch);
            exit(0);
       }
    }
    /***************************************************************************
    ***    I N I T I A L I S A T I O N    D ' A L L E G R O                  ***
    ***************************************************************************/
  


    // Initialisation primaire d'Allegro
    allegro_init();

    // Initialisation du clavier
    install_keyboard();

    // Initialisation de la souris
    install_mouse();

    // Initialisation du son
    install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, 0);

    // Initialisation du mode graphique

    // On essaye en 800x600x32bpp
    set_color_depth(32);
    if (set_gfx_mode(MODE_GRAPHIQUE, 800, 600, 0, 0)) {
        
        // Puis en 800x600x24bpp
        set_color_depth(24);
        if (set_gfx_mode(MODE_GRAPHIQUE, 800, 600, 0, 0)) {
            
            // Puis en 800x600x16bpp
            set_color_depth(16);
            if (set_gfx_mode(MODE_GRAPHIQUE, 800, 600, 0, 0)) {
                
                // Puis en 800x600x8bpp
                set_color_depth(8);
                if (set_gfx_mode(MODE_GRAPHIQUE, 800, 600, 0, 0)) {
                    ERREUR("Erreur: lors de set_gfx_mode()");
                }
            }
        }
    }
        
    /***************************************************************************
    ***    I N I T I A L I S A T I O N    D U    J E U                       ***
    ****************************************************************************

    AVEC :
    ======
    * Initialisation de la matrice
    * Allocation de la bitmap d'affichage
    * Chargement des graphismes

    */

    // La configuration actuelle des touches
    // JOUEUR 1
    touches[0][TOUCHE_HAUT]   = KEY_UP;
    touches[0][TOUCHE_BAS]    = KEY_DOWN;
    touches[0][TOUCHE_DROITE] = KEY_RIGHT;
    touches[0][TOUCHE_GAUCHE] = KEY_LEFT;
    touches[0][TOUCHE_BOMBE]  = KEY_END;
    
    // JOUEUR 2
    touches[1][TOUCHE_HAUT]   = KEY_W;
    touches[1][TOUCHE_BAS]    = KEY_S;
    touches[1][TOUCHE_GAUCHE] = KEY_A;
    touches[1][TOUCHE_DROITE] = KEY_D;
    touches[1][TOUCHE_BOMBE]  = KEY_E;

    // JOUEUR 3
    touches[2][TOUCHE_HAUT]   = KEY_W;
    touches[2][TOUCHE_BAS]    = KEY_S;
    touches[2][TOUCHE_GAUCHE] = KEY_A;
    touches[2][TOUCHE_DROITE] = KEY_D;
    touches[2][TOUCHE_BOMBE]  = KEY_E;

    // On charge les graphismes
    chargement_graphismes();

    // On charge les sons
    chargement_sons();

    // Allocation du double buffer
    sortie   = create_bitmap(800, 600);       // Ca c'est pour mettre toute l'image en transparence

    /***************************************************************************
    ***        ALLOCATION ET INITIALISATION DES JOUEURS                      ***
    ***************************************************************************/
    
    for (i=0; i < NB_JOUEURS_MAX; i++) {

            // Allocation d'un objet
            joueurs[i] = malloc ( sizeof(objet) );
            
            // Allocation des infos spécifique à chaque bomberman
            joueurs[i]->plus = malloc( sizeof(t_infos) );

            // Allocation de l'animation du bomberman (bientôt à supprimer)
            joueurs[i]->animation = malloc( sizeof(animation_t) );

            // On fixe le numéro de joueur dans chaque champs "plus"
            // Car dans le tableau d'affichage, on ne sait pas qui est qui
            joueurs[i]->plus->no_joueur = i;
    }
    

    //Lancement de la musique
    play_midi(mid_musique, 1);
    
    //Réglage du volume
    set_volume(-1, 90);

    // Si le menu n'est pas désactivé
    if (!pas_de_menu) {
        do {
            // On lance le menu principal
            selection = lancer_menu_principal();
        
            if      (selection == MENU_UN_JOUEUR)      nb_joueurs = 1;
            else if (selection == MENU_DEUX_JOUEURS)   nb_joueurs = 2;
            else if (selection == MENU_EDITEUR)        lance_editeur = TRUE;
            else if (selection == MENU_HIGHSCORES)      {
                high_scores(0,2,1);
                high_scores(0,2,2);
                selection = -1;
            }
        }
        while (selection == -1);
    }

    // Tant que l'utilisateur n'a pas cliqué sur le menu QUITTER
    while (selection != MENU_QUITTER) {
       
       
        // A chaque nouvelle partie, on initialise les variables des joueurs
       for (i=0; i < NB_JOUEURS_MAX; i++) {     
            // Les bombes doivent avoir une puissance de 1
            joueurs[i]->plus->puissance_bombe = 1;

            // Les scores repartent à zéro
            joueurs[i]->plus->score = 0;

            

            // On fixe le nombre de vies
            
            if (nb_joueurs >= 2)
                joueurs[i]->plus->vies = 10;
            else
                joueurs[i]->plus->vies = 3;

            // La vitesse par défaut du bomberman
            joueurs[i]->plus->vitesse = 8;

            // Le nombre maximum de bombe que peut avoir le bomberman
            joueurs[i]->plus->max_nb_bombes = 1;

            // On commence sans bombe
            joueurs[i]->plus->nb_bombes = 0;
    
            // On fixe la position de départ à l'arrêt
            joueurs[i]->direction=DIRECTION_ARRET;
            
            // Pour confirmer que le bomberman est toujours bien vivant
            joueurs[i]->typec = OBJ_T_BOMBERMAN;
        
        }
    
    
        // Si il n'est pas défini, le nb de joueurs est fixé à 2
        if (!nb_joueurs) nb_joueurs = 2;
    
        // S'il n'est pas défini, le niveau actuel est le niveau 1
        if (!niveau_actuel) niveau_actuel = 1;
    
        
        // On initilialise les cases : 
        // Toutes les cases ont leur premier pointeur nul, donc elles sont vides
        for (i = 0; i < NB_CASES_X; i++)
            for (j = 0; j < NB_CASES_Y; j++) {
                case_actuelle = &(cases[i][j]);
                (case_actuelle->objets)[0] = (objet*) NULL;
    
    
            }
        
        
        // On ne souhaite pas passer au niveau suivant
        passer_au_niveau_suivant = FALSE;
    
    
    
        
    
        
        // On charge le premier niveau
        chargement_niveau(niveau_actuel);
       
        // HOP!!! Le temps TOTAL (jusqu'à la mort du bomberman) commence...
        timer_start_count();
        
        // C'est le time() qui ne devra pas etre depasse avant la fin de ce niveau
        if (pas_de_timeout) temps_parcours_niveau*= 2;

        temps_a_pas_depasser = time(NULL) + temps_parcours_niveau;
    
        /***************************************************************************
        ***    L A    B O U C L E    C E N T R A L E    D U    J E U             ***
        ***************************************************************************/
       
        while (boucle ) {
            // Pour chaque joueur 
            for (k=0; k < nb_joueurs; k++) {
                // Le joueur bouge
                (joueurs[k])->plus->no_move = FALSE;
                
                // Le joueur appuie sur une touche
                appuie[k] = TRUE;
            }
        
            // Tous les 800 affichages, on place un bonus
            if (tours%800 == 0) {
                DEBUG ("On va placer un bonus");
                place_un_bonus();

            }
            
            // Si le mode_editeur est activé ou si la touche F4 est pressée 2 fois
            // l'éditeur se lance.
            if (selection != MENU_HIGHSCORES)
            if ( (key[KEY_F4] && confirm_key(KEY_F4)) || lance_editeur) {
                editeur(niveau_actuel);
                
                lance_editeur = FALSE;
                if (selection == MENU_EDITEUR) boucle = FALSE;

                temps_a_pas_depasser = time(NULL) + temps_parcours_niveau;
            }
        

             
            if (time(NULL) > temps_a_pas_depasser ) {
                    DEBUG ("On a depasse le temps imparti...");
                    game_over();
                    boucle = FALSE; 
            }
            
            for (k=0; k < nb_joueurs; k++) 
            if (joueurs[k]->type == OBJ_T_BOMBERMAN) {
                    
                
                    if (tours%100 == 0 && joueurs[k]->plus->nb_bombes < joueurs[k]->plus->max_nb_bombes) {
                        joueurs[k]->plus->nb_bombes++;
                    
                    }
                                    
                if (key[(touches[k][TOUCHE_HAUT])]) 
                    joueurs[k]->direction=DIRECTION_HAUT;
                    
                else if (key[(touches[k][TOUCHE_BAS])])
                    joueurs[k]->direction=DIRECTION_BAS;
                    
                else if(key[(touches[k][TOUCHE_DROITE])])
                    joueurs[k]->direction=DIRECTION_DROITE;
                    
                else if (key[(touches[k][TOUCHE_GAUCHE])])
                    joueurs[k]->direction=DIRECTION_GAUCHE;
                    
                else {
                    appuie[k] = FALSE; // == 0
                    joueurs[k]->plus->no_move = TRUE;
                }
                if ( key[(touches[k][TOUCHE_BOMBE])] ) {
                    DEBUG ("touche de bombe");
                    if (joueurs[k]->plus->nb_bombes > 0) {
                        DEBUG ("Creation d'une bombe");
    
                        p_objet = creer_objet(OBJ_T_BOMBE, joueurs[k]->placement);
                        if (p_objet) {
                            joueurs[k]->plus->nb_bombes--;
                            p_objet->typec = joueurs[k]->plus->puissance_bombe; 
                            p_objet->typed = k;
                            DEBUG ("Nombre de bombe pour %d : %d", k, joueurs[k]->plus->nb_bombes);
                            
                            //DEBUG ("Puissance de bombe : %2d", p_objet->typec)
                        }

                    }
                    else DEBUG ("Pas de bombe dispo");
                }
            }    
            
            if (key[KEY_ESC]) boucle = FALSE;
    
            nb_joueurs_morts = 0;
            for (k=0; k < nb_joueurs; k++) 
            if (joueurs[k]->type == OBJ_T_BOMBERMAN && joueurs[k]->direction && appuie[k]) {
                    if ( !bouge_objet(joueurs[k], joueurs[k]->direction, joueurs[k]->plus->vitesse) ) // Si on a pas pu avancer de 10
                    if ( !bouge_objet(joueurs[k], joueurs[k]->direction, 4)  )  // Si on a pas pu avancer de 4
                    if ( !bouge_objet(joueurs[k], joueurs[k]->direction, 1)  )  {// Si on a pas pu avancer de 1
                        
                          
                        for (i=0; i < 2; i++) {
                            tmp_pos = joueurs[k]->position;
                            
                            if (joueurs[k]->direction == DIRECTION_HAUT) 
                                tmp_pos.y -= LAR_CASE_L;
                            else if (joueurs[k]->direction == DIRECTION_BAS)
                                tmp_pos.y += LAR_CASE_L;
                            else if (joueurs[k]->direction == DIRECTION_DROITE)
                                tmp_pos.x += LAR_CASE_L;
                            else if (joueurs[k]->direction == DIRECTION_GAUCHE)
                                tmp_pos.x -= LAR_CASE_L;
                        
                            
                            if (joueurs[k]->direction == DIRECTION_HAUT || joueurs[k]->direction == DIRECTION_BAS) {
                                if (i) {
                                    tmp_pos.x += LAR_CASE_L/2;
                                    j = DIRECTION_DROITE;
                                }
                                else   {
                                    tmp_pos.x -= LAR_CASE_L/2;
                                    j = DIRECTION_GAUCHE;
                                }
                            }
                            else {
                                if (i) {
                                    tmp_pos.y += LAR_CASE_L/2;
                                    j = DIRECTION_BAS;
                                }
                                else   {
                                    tmp_pos.y -= LAR_CASE_L/2;
                                    j = DIRECTION_HAUT;
                                }
                            }
                            
                            if ( !yatil_obstacle_pos(tmp_pos) ) {
                                bouge_objet(joueurs[k], j, 1);
                                if (i == 0) i = 1;
                            }   
                        }
                    }
            }
            else if (joueurs[k]->typec == OBJ_T_BOMBERMAN_MORT) 
                nb_joueurs_morts++;
    
            
            if (nb_joueurs == nb_joueurs_morts) {
                DEBUG ("Tous les joueurs sont morts.");
                game_over();
                boucle = FALSE;
            }

            pause_affichage(50);
            //DEBUG ("Le compte est de : %d", timer_get_count() );
            //DEBUG ("Attention AFFICHAGE");
            affichage();
            tours++;

            // Si passer au niveau suivant est invoqué
            if (passer_au_niveau_suivant) {
            
                // on fait une petite pause de 500ms pour pouvoir reprendre nos esprits
                // et surtout éviter de se déplacer dans la continuité du monde précédente
                rest(500);
                passer_au_niveau_suivant = FALSE;
                
                // On souhaite passer au niveau suivant
                niveau_actuel++;
    
    
                // On charge le niveau actuel
                if (chargement_niveau(niveau_actuel)) {
                    DEBUG ("Le niveau est bien bien charge...");            
                }
                else {
                    // Si il n'y a plus de niveau, on marque : GAME OVER
                    // On va créer une interface de félicitation à la place...
                    DEBUG ("On a pas pu charger le niveau...");
                    game_over();

                }
                // A faire avant chaque chargement de niveau 
                
                
    
                DEBUG ("Les parametres des %d joueurs sont initialises...", nb_joueurs);
                
                // Initialisation des paramêtres de joueurs
                if (nb_joueurs > 1) // Si on est en solo, on garde ses bonus, sinon nan 
                    for (i=0; i<nb_joueurs; i++) {
                        joueurs[i]->direction=DIRECTION_ARRET;
                        joueurs[i]->plus->puissance_bombe=1;
                        joueurs[i]->plus->vitesse = 10;
                        joueurs[i]->plus->max_nb_bombes=1;
                        joueurs[i]->plus->nb_bombes=0;
                    }

                // C'est la définition du temps à ne pas dépasser
                if (pas_de_timeout) temps_parcours_niveau*= 2;
                temps_a_pas_depasser = time(NULL) + temps_parcours_niveau;
        
            }
    
            
        }
        // au prochain coup, la boucle sera refermée
        boucle = TRUE;
       
        // A la fin de la boucle, on stocke les highscores de chaque joueur (pour vu qu'ils ne soient pas nuls)
        if (selection != MENU_EDITEUR) 
        for (k=0; k < nb_joueurs; k++) 
            high_scores(joueurs[k]->plus->score, niveau_actuel, nb_joueurs);

         // Le menu du début doit être aussi inclut dans la boucle
         if (!pas_de_menu) {
            selection = lancer_menu_principal();
            if      (selection == MENU_UN_JOUEUR)       nb_joueurs = 1;
            else if (selection == MENU_DEUX_JOUEURS)    nb_joueurs = 2;
            else if (selection == MENU_EDITEUR)         lance_editeur = TRUE;
            else if (selection == MENU_HIGHSCORES)      {
                // On affiche les highscores pour 1 joueurs
                high_scores(0,0,1);

                // On affiche les highscores pour 2 joueurs
                high_scores(0,0,2);
            }
            niveau_actuel = 1;
            
        }
        else (selection = MENU_QUITTER);

    }

  // On décharge le fichier de données qui contient nottament la police
  unload_datafile(dat);  

} END_OF_MAIN();
