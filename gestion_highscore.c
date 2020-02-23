#include "bib.h"

// Les variables
t_hs_complet highscore;

unsigned long int timer;
unsigned long int pause_timer;


// Les fonctions de comptage du temps passe
void timer_start_count() {
    extern unsigned long int timer;
    timer = time(NULL);
}

unsigned long int timer_get_count() {
    extern unsigned long int timer;
    return (time(NULL) - timer);
}

unsigned long int timer_pause_count() {
    extern unsigned long int timer, pause_timer;
    pause_timer = time(NULL);
}

unsigned long int timer_resume_count() {
    extern unsigned long int timer, pause_timer;
    timer+= (time(NULL) - pause_timer);
}

char *graphique_get_str(int x, int y, char *message, int taille_max) {
    extern FONT *police;
    char touche;
    int pos = 0;
    static char entree[1024];

    textprintf_ex(screen, police, x, y, makecol(255, 255, 255), -1, "%s", message);
    clear_keybuf();
    while (touche != 13) {
        touche = (readkey() & 0xff);
        DEBUG ("touche vaut : %d", touche);
        if      (touche == 'q') touche = 'a';
        else if (touche == 'Q') touche = 'A';
        else if (touche == 'a') touche = 'q';
        else if (touche == 'A') touche = 'Q';
        else if (touche == 'z') touche = 'w';
        else if (touche == 'Z') touche = 'Z';
        else if (touche == 'w') touche = 'z';
        else if (touche == 'W') touche = 'Z';
        else if (touche == ';') touche = 'm';
        else if (touche == ':') touche = 'M';
        else if (touche == 'm') touche = ';';
        else if (touche == 'M') touche = ':';

        
        if (touche != 8) {
            if (touche != 13) {
                entree[pos]     = touche;
                entree[(pos+1)] = '\0';
            }
            pos++;
        }
        else if (pos > 0) {
            pos--;
            entree[pos]     = ' ';
            entree[(pos+1)] = '\0';   
            
        }

        if (pos > taille_max) pos = taille_max;
        rectfill(screen, x, y+15, x+((pos+1)*8), y+30, 0);
        textprintf_ex(screen, police, x, y+15, makecol(255, 255, 255), -1, "%s", entree);
    
         
    }
    
    return entree;
}

int high_scores(unsigned int score, unsigned int niveau, unsigned int nb_joueurs)
{
  // Buffer de sortie
  extern BITMAP *sortie;
  extern FONT *police;
  // Image à afficher
  BITMAP *fond;

  // Fichier de high_scores
  char fichier_hs[100];

  char message[1000];
  char nom[100];

  // On charge l'image
  if(!(fond = load_bitmap("images/fond_highscores.bmp", NULL)))
    ERREUR("Erreur lors du chargement de l'image de fond des highscores");

  
  sprintf(fichier_hs, "highscore-%d.hs", nb_joueurs);
  DEBUG ("Comparaison : %d - %d", KEY_A, 'a');
  DEBUG ("comparaison : %d - %d", KEY_D, 'd');
  DEBUG ("comparaison : %d - %d", KEY_E, 'e');
  if (! chargement_highscore(fichier_hs) ) {
        initialise_highscore();
        sauvegarde_highscore(fichier_hs);
        if (! chargement_highscore(fichier_hs) ) {
            ERREUR ("Il n'y avait pas de fichier de high_score et je n'ai pas pu en creer.");
            
        }

   }

   
  if (score && yatil_pour_toi(score, niveau)) {
    DEBUG ("Tu peux etre ajoute au HIGH'SCORES");
    
    sprintf(message, "Que le joueur qui a fait %d points dise son nom :", score);
    
    strcpy(nom, graphique_get_str(10, 10, message, 49) );
    DEBUG ("Le nom est %s", nom);

    
    ajout_highscore(score, niveau, nom);
  }


  while(!key[KEY_ESC] && !(mouse_b & 1) ) {
    // On colle le fond
    draw_sprite(sortie, fond, 0, 0);
    textprintf_ex(sortie, police, 220, 70, makecol(255, 255, 255), -1, "*** Les High'Scores pour les parties %d joueurs ***", nb_joueurs);
    genere_highscore();



    // On affiche le tout
    draw_sprite(screen, sortie, 0, 0);
    pause_affichage(100);
    

  }
  
  rest(500);
  sauvegarde_highscore(fichier_hs);

  destroy_bitmap(fond);

  return TRUE;
}

int genere_highscore() {
    extern BITMAP *sortie;
    extern FONT *police;
    extern t_hs_complet highscore;
    int i;
    for (i = 0; i < NB_HIGHSCORE && highscore.lignes[i].score; i++) {
        textprintf_ex(sortie, police, 220, 170+(15*i), makecol(255, 255, 255), -1, "%d ", 
            (i+1));
        
        textprintf_ex(sortie, police, 220+40, 170+(15*i), makecol(255, 255, 255), -1, ". %s",
            highscore.lignes[i].nom);

        textprintf_ex(sortie, police, 220+40+200, 170+(15*i), makecol(255, 255, 255), -1, "- %d",
            highscore.lignes[i].score);

        textprintf_ex(sortie, police, 220+40+200+80, 170+(15*i), makecol(255, 255, 255), -1, "(%2d)",
            highscore.lignes[i].niveau);

    }


}

int the_highscore_is() {



}

void game_over()
{
  //Image à afficher
  BITMAP *img;
  
  DEBUG ("*** GAME ' OVER ***");

  //on charge l'image
  if(!(img = load_bitmap("images/game_over.bmp", NULL)))
    ERREUR("Erreur lors du chargement de l'image Game Over !");
  
  //On la dessine
  draw_sprite(screen, img, SCREEN_W / 2 - img->w / 2, SCREEN_H / 2 - img->h / 2);
  
  //Et on attend que l'utilisateur appuie sur la touche Escape
  while(!key[KEY_ESC] ) rest(100);
  
  //Libération du bitmap
  destroy_bitmap(img);     

  //high_scores();
}



int chargement_highscore(char *nom_hs) {
    FILE *fp;
    extern t_hs_complet highscore;

    fp = fopen(nom_hs, "rb");
    
    if (!fp) return FALSE;

    fread(&highscore, 1, sizeof(highscore), fp);

    fclose(fp);

    return TRUE;
}

int sauvegarde_highscore(char *nom_hs) {
    FILE *fp;
    extern t_hs_complet highscore;

    fp = fopen(nom_hs, "wb");
    
    if (!fp) return FALSE;

    fwrite(&highscore, 1, sizeof(highscore), fp);

    fclose(fp);
    
    return TRUE;

}

int initialise_highscore() {
    extern t_hs_complet highscore;
    unsigned int i;
    // Seul le niveau a besoin d'être initialisé
    for (i = 0; i < NB_HIGHSCORE; i++)
        highscore.lignes[i].niveau = 0;

}

int yatil_pour_toi(unsigned int score, unsigned int niveau) {
    int i;
    extern t_hs_complet highscore;

    for (i = (NB_HIGHSCORE-1); i >= 0 && score >= highscore.lignes[i].score; i--) {
        DEBUG ("A / B : %3d / %3d", score, highscore.lignes[i].score);

    }

    if (i < NB_HIGHSCORE-1) return TRUE;
    else FALSE;
        
}

int ajout_highscore(unsigned int score, unsigned int niveau, char *nom) {
    extern t_hs_complet highscore;
    int position = 0;
    int i;

    t_hs_ligne temp;

    if (strlen(nom) > 49) {
        ERREUR ("Le nom ne devrait pas etre aussi long");
        return FALSE;
    }

    
    strcpy(temp.nom, nom);
    temp.niveau = niveau;
    temp.score  = score;
    
    DEBUG ("On va essayer d'y arriver");
    for (i = (NB_HIGHSCORE-1); i >= 0 && score >= highscore.lignes[i].score; i--) {
        DEBUG ("[%d] : My / HS : %3d / %3d", i, score, highscore.lignes[i].score);

    }
    i++;

    if (i >= NB_HIGHSCORE-1) return FALSE;
    
    DEBUG ("Tu seras %d e...", i);
    
    position = i;
    
    // On decale tous les highscore
    for (i = (NB_HIGHSCORE-1) ; i > position; i--) {
        DEBUG ("Le HS %d va en %d", (i-1), i );
        highscore.lignes[i] = highscore.lignes[(i-1)];
    }
    // on rentre le nouveau highscore
    highscore.lignes[position] = temp;

    
    
    
}

