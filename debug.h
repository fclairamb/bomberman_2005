#ifndef _DEBUG_H_
#define _DEBUG_H_

// LA VARIABLE DE DEBUG :
//#define _DEBUG_

#define ERREUR(msg) {\
   set_gfx_mode(GFX_TEXT,0,0,0,0);\
   allegro_message("ERREUR: fichier %s, ligne %4d : %s\n",__FILE__, __LINE__, msg);\
   allegro_exit();\
   exit(EXIT_FAILURE);\
}

#ifdef _DEBUG_

FILE *fi_debug;

#define DEBUG(format...)    { \
    printf ("(debug) %s - %4d : ", __FILE__, __LINE__);\
    printf(format); \
    printf ("\n");\
    fprintf (fi_debug, "(debug) %s - %4d : ", __FILE__, __LINE__);\
    fprintf (fi_debug, format); \
    fprintf (fi_debug, "\n");\
    fflush(fi_debug);\
}


#define MODE_GRAPHIQUE GFX_AUTODETECT_WINDOWED

#else

#define DEBUG(format...) {}
#define MODE_GRAPHIQUE GFX_AUTODETECT_FULLSCREEN

#endif

#endif
