#include "bib.h"

int lancer_menu_principal(void)
{
  //Variables diverses  
  int selection, quit; //selection: pour savoir sur quel menu on se trouve ; quit: mis à 1 pour quitter
  BITMAP *fond; //Fond du menu
  BITMAP *single, *single_hover, *deux, *deux_hover, *highscores, *highscores_hover,
         *editeur, *editeur_hover, *quitter, *quitter_hover; //Boutons du menu
  
  //Variables externes pour affichage du menu
  //Un seul buffer, on ne se sert pas du double_buffer
  extern BITMAP *sortie;  

  selection = -1;
  quit      = 0;

  show_mouse(screen);    
  
  
  //Chargement du bitmap de fond du menu
  fond = load_bmp("images/menu_principal.bmp", NULL);
   
  if(!fond)
    ERREUR("Erreur lors du chargement du fond du menu principal");
    
  //Chargement des boutons
  if(!(single           = load_bmp("images/un_joueur.bmp", NULL)))
    ERREUR("Erreur lors du chargement de : images/un_joueur.bmp");
  if(!(single_hover     = load_bmp("images/un_joueur_hover.bmp", NULL)))
    ERREUR("Erreur lors du chargement de : images/un_joueur_hover.bmp");
      
  if(!(deux             = load_bmp("images/deux_joueurs.bmp", NULL)))
    ERREUR("Erreur lors du chargement de : images/deux_joueurs.bmp");
  if(!(deux_hover       = load_bmp("images/deux_joueurs_hover.bmp", NULL)))
    ERREUR("Erreur lors du chargement de : images/deux_joueurs_hover.bmp");     
      
  if(!(highscores       = load_bmp("images/highscores.bmp", NULL)))
    ERREUR("Erreur lors du chargement de : images/highscores.bmp");
  if(!(highscores_hover = load_bmp("images/highscores_hover.bmp", NULL)))
    ERREUR("Erreur lors du chargement de : images/highscores_hover.bmp");
      
  if(!(editeur          = load_bmp("images/editeur.bmp", NULL)))
    ERREUR("Erreur lors du chargement de : images/editeur.bmp");
  if(!(editeur_hover    = load_bmp("images/editeur_hover.bmp", NULL)))
    ERREUR("Erreur lors du chargement de : images/editeur_hover.bmp");           
      
  if(!(quitter          = load_bmp("images/quitter.bmp", NULL)))
    ERREUR("Erruer lors du chargement de : images/quitter.bmp");
  if(!(quitter_hover    = load_bmp("images/quitter_hover.bmp", NULL)))
    ERREUR("Erruer lors du chargement de : images/quitter_hover.bmp");    
      
  while(!quit)
  {
    draw_sprite(sortie, fond, 0, 0);   
    
    selection = -1;

    //Dessin des boutons en fonction de la position de la souris
    //UN JOUEUR
    if((mouse_x > MENU_X) && (mouse_x < MENU_X + single->w) && (mouse_y > UN_JOUEUR_Y) && (mouse_y < UN_JOUEUR_Y + single->h))
    {
      draw_sprite(sortie, single_hover, MENU_X, UN_JOUEUR_Y);
      selection = MENU_UN_JOUEUR;
    }    
    else   
      draw_sprite(sortie, single      , MENU_X, UN_JOUEUR_Y);
       
    //DEUX JOUEURS
    if((mouse_x > MENU_X) && (mouse_x < MENU_X + deux->w) && (mouse_y > DEUX_JOUEURS_Y) && (mouse_y < DEUX_JOUEURS_Y + deux->h))
    {
      draw_sprite(sortie, deux_hover, MENU_X, DEUX_JOUEURS_Y);
      selection = MENU_DEUX_JOUEURS;
    }    
    else 
      draw_sprite(sortie, deux      , MENU_X, DEUX_JOUEURS_Y);       
      
    //HIGHSCORES  
    if((mouse_x > MENU_X) && (mouse_x < MENU_X + highscores->w) && (mouse_y > HIGHSCORES_Y) && (mouse_y < HIGHSCORES_Y + highscores->h))
    {
      draw_sprite(sortie, highscores_hover, MENU_X, HIGHSCORES_Y);
      selection = MENU_HIGHSCORES;
    }    
    else   
      draw_sprite(sortie, highscores      , MENU_X, HIGHSCORES_Y);
       
    //EDITEUR  
    if((mouse_x > MENU_X) && (mouse_x < MENU_X + editeur->w) && (mouse_y > EDITEUR_Y) && (mouse_y < EDITEUR_Y + editeur->h))
    {
      draw_sprite(sortie, editeur_hover, MENU_X, EDITEUR_Y);
      selection = MENU_EDITEUR;
    }    
    else 
      draw_sprite(sortie, editeur      , MENU_X, EDITEUR_Y);       
      
    //QUITTER  
    if((mouse_x > MENU_X) && (mouse_x < MENU_X + quitter->w) && (mouse_y > QUITTER_Y) && (mouse_y < QUITTER_Y + quitter->h))
    {
      draw_sprite(sortie, quitter_hover, MENU_X, QUITTER_Y);
      selection = MENU_QUITTER;
    }    
    else
      draw_sprite(sortie, quitter      , MENU_X, QUITTER_Y);           


    DEBUG ("Selection : %d", selection);
      
    //Si on clique, action selon le bouton sous lequel on se trouve
    if(mouse_b & 1 && selection > 0)
      quit = 1;
      
    //Swap du bitmap de dessin vers le buffer
    draw_sprite(screen, sortie, 0, 0);

    // Pour pas utiliser toutes les ressources du processeur inutilement
    pause_affichage(100);
  }    
    return selection;
}    
