#include "bib.h"

/* ======================= EDITEUR DE BOMBERMAN ================================
   =======================______________________================================

A noter que l'éditeur se sert en grande partie des fonctions de la librairie du
jeu qui ont été faite pour être, comme ici, réutilisables à volonté.

L'éditeur peut éditer un niveau à n'importe quel moment du jeu.

*/





unsigned char mode_editeur;

int confirm_key(int touche) {
    //beep();
    DEBUG ("En attente de confirmation de touche..");
    textprintf_ex(screen, font, 10, 10, makecol(255,255,255), makecol(0,255,255), "CONFIRMEZ LA TOUCHE ou ESCAPE");
    while(key[touche]) ; // On attend que la touche soit fini d'être tapée
    rest(200);           // On fait une pause de 300 ms
    while (!key[touche]) {
        rest(100);
        if (key[KEY_ESC]) // Si c'est ESC qu'a été frappé, c'est annulé
            return FALSE;
    }
    
    DEBUG ("OK");

    textprintf_ex(screen, font, 10, 10, makecol(255,255,255), makecol(0,0,0), "                             ");
    return TRUE; // Si la touche a bien été frappée, c'est bon...
}

int quel_type() {
    
    extern BITMAP *editmenu2;
        
    /*
    textprintf_ex(screen, font, 10, 550, makecol(255,255,255), 0, "Appuyez sur :                                                  ");
    textprintf_ex(screen, font, 10, 558, makecol(255,255,255), 0, "* 'a' pour BOMBERMAN          |    * 'b' pour une bombe        ");
    textprintf_ex(screen, font, 10, 566, makecol(255,255,255), 0, "* 'w' pour un MUR CASSABLE    |    * 'x' pour un MUR INCASSABLE");
    textprintf_ex(screen, font, 10, 574, makecol(255,255,255), 0, "* 'i' pour un MECHANT         |    * 'p' pour la porte de fin  ");
    textprintf_ex(screen, font, 10, 582, makecol(255,255,255), 0, "* 'v' pour un bonus VIE       |    * 'c' pour une bonus puissance de bombe");
    textprintf_ex(screen, font, 10, 590, makecol(255,255,255), 0, "ESC pour annuler              |    * 'd' pour un diamant");
    */
    
    while (! key[KEY_ESC]) {
        draw_sprite(screen, editmenu2, 636, 79);
        
        if (key[KEY_Q]) return OBJ_T_BOMBERMAN;
        else if (key[KEY_I]) return OBJ_T_MECHANT;
        else if (key[KEY_B]) return OBJ_T_BOMBE;
        else if (key[KEY_Z]) return OBJ_T_MUR;
        else if (key[KEY_X]) return OBJ_T_INCASSABLE;
        else if (key[KEY_P]) return OBJ_T_END_DOOR;
        else if (key[KEY_D]) return OBJ_T_DIAMANT;
        else if (key[KEY_V]) return OBJ_T_BONUS_VIE;
        else if (key[KEY_C]) return OBJ_T_BONUS_BOMBE_PUISSANCE;
        


        rest(50); // Evitons de faire tourner le processeur à fond

    }   

    return 0;
}

// L'éditeur est tout un programme en lui même
int editeur(int niveau_voulu) {
  unsigned char niveau_charge = TRUE;
  unsigned char perte_autorise = TRUE;
  static unsigned int niveau_nb = 0;
  unsigned int joueurs_actuel = 0;
  unsigned int joueur_actuel = 0;
  unsigned char boucle = TRUE;
  coord_p pos; coord_c _case; //l_case;
  int type = 0;
  extern unsigned char mode_editeur;
  extern int nb_joueurs ;
  extern unsigned int temps_parcours_niveau;

  // L'editeur n'utilise ces deux variables que pour faire disparaitre la porte de sortie
  // il ne travaille, sinon, jamais directement sur la matrice.
  extern coord_p placement_porte_fin;
  extern carre cases[NB_CASES_X][NB_CASES_Y]; 

  extern BITMAP *intfjeu, *intfediteur, *editmenu1;
  BITMAP *tmp;

  int i, j, k;

  mode_editeur = TRUE;
  DEBUG ("**** MODE EDITEUR ****");

  timer_pause_count();

  show_mouse(screen);
  textprintf_ex(screen, font, 750, 10, makecol(255,255,255), 0, "!!! EDITION !!!");
  rest(300);  
  
  joueurs_actuel = nb_joueurs;

  if (niveau_voulu && !niveau_nb) niveau_nb = niveau_voulu;
  else if (!niveau_nb) niveau_nb = 1;
  //Sauvegarde du fond actuel
  tmp = intfjeu;
  //On change le fond du jeu
  intfjeu = intfediteur;

  while( boucle ) {
    
    if (niveau_charge) affichage();
    else {
        textprintf_ex(screen, font, 10, 10, makecol(255,255,255), 0, "EDITION : Aucun niveau charge           ");
    }   


    if (key[KEY_F1] ) {
      DEBUG ("Demande de chargement de niveau : %d", niveau_nb);
      if (chargement_niveau(niveau_nb)) {
             niveau_charge = TRUE;
      }

    }
    else if ( key[KEY_F4] && confirm_key(KEY_F4) )
        boucle = FALSE;
    else if ( key[KEY_F2] && confirm_key(KEY_F2) ) {
        sauvegarde_niveau(niveau_nb);

    }   
    else if ( key[KEY_F5] ) {
        temps_parcours_niveau = atoi( graphique_get_str(20, 20, "Quel temps de parcours maximum (en secondes) ?", 4) ); 
    
    }
    
    else if ( 
        // Il faut simplifier cette "table de vérité" (c'est pas ma priorité la)
        ((mouse_b & 1) && _case.x >= 0 && _case.y >= 0 && _case.x < NB_CASES_X && _case.y < NB_CASES_Y && (type = quel_type()) )
        ||
        ((mouse_b & 4) && _case.x >= 0 && _case.y >= 0 && _case.x < NB_CASES_X && _case.y < NB_CASES_Y && type) 
        ) {
        if (type < 100) inscrire_objet(_case, (objet *) type);
        else {
            if (type != OBJ_T_BOMBERMAN) {
                creer_objet(type, _case);
            }
            else if (joueurs_actuel < NB_JOUEURS_MAX) {
                for (i=0; i < NB_JOUEURS_MAX; i++) 
                    if (joueurs[i]->position.x == 0) {
                        joueur_actuel = i;            
                        i = NB_JOUEURS_MAX;
                    }                
                        
                joueurs[joueur_actuel]->placement  = _case;
                joueurs[joueur_actuel]->position   = position_from_placement( _case );
                joueurs[joueur_actuel]->type       = OBJ_T_BOMBERMAN;
                inscrire_objet(joueurs[joueur_actuel]->placement, joueurs[joueur_actuel]);
                //joueurs[joueur_actuel]->typec      = 3;
                //*(joueurs[joueur_actuel]->animation)  =  *anim_bonhomme;
                joueurs_actuel++;
            }
        }
    }
    else if ( (mouse_b & 2) && _case.x >= 0 && _case.y >= 0 && _case.x < NB_CASES_X && _case.y < NB_CASES_Y ) {
        joueurs_actuel -= vider_la_case(_case);
        DEBUG ("Nombre de joueurs actuel : %d", joueurs_actuel);
    } 
    else if ( key[KEY_PGUP] ) {
        niveau_nb++;
        
    }
    else if ( key[KEY_PGDN] & niveau_nb > 0 ) {
        niveau_nb--;
        //textprintf_ex(screen, font, 10, 2, makecol(255,255,255), 0, "Niveau : %d                ", niveau_nb);
    }
    else if ( key[KEY_N]) {
        nettoyer_carte();
        joueurs_actuel = 0;
        for (i=0; i < NB_JOUEURS_MAX; i++) 
            joueurs[i]->position.x = 0;
    }
    textprintf_ex(screen, font, 670, 549, makecol(255,255,255), -1, "Niveau : %d", niveau_nb);

    DEBUG ("Nombre de joueurs actuel : %d", joueurs_actuel);

    
    pos.x = mouse_x; pos.y = mouse_y;
    _case = case_correspondante(pos);
    
    
    textprintf_ex(screen, font, 670, 563, makecol(255,255,255), -1, "Cases  : %d x %d", _case.x, _case.y);
        
    draw_sprite(screen, editmenu1, 636, 79);
    
    /*    //l_case = _case;
    textprintf_ex(screen, font, 10, 550, makecol(255,255,255), 0, "Appuyez sur :");
    textprintf_ex(screen, font, 10, 558, makecol(255,255,255), 0, "* F1 pour charger le niveau en cours                           ");
    textprintf_ex(screen, font, 10, 566, makecol(255,255,255), 0, "* F2 pour sauver le niveau en cours                            ");
    textprintf_ex(screen, font, 10, 574, makecol(255,255,255), 0, "* F3 pour abandonner le niveau en cours                        ");
    textprintf_ex(screen, font, 10, 582, makecol(255,255,255), 0, "* F4 pour quitter le mode edition                              "); 
    */ 
    pause_affichage(100);
    

  }
  
  DEBUG ("**** FIN DU MODE EDITEUR ****");

  //on remet le fond du jeu
  intfjeu = tmp;

  placement_porte_fin = _case;

  for (i = 0; i < NB_CASES_X; i++)
  for (j = 0; j < NB_CASES_Y; j++)
  for (k = 0; k < NB_OBJ_CASE && cases[i][j].objets[k]; k++)
    if ( (int) cases[i][j].objets[k] == OBJ_T_END_DOOR) { 
        _case.x = i;
        _case.y = j;
        placement_porte_fin = _case;
        desinscrire_objet(_case, OBJ_T_END_DOOR);

    }

  mode_editeur = FALSE;

  timer_resume_count();
  
  return TRUE;




}
