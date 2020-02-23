#include "bib.h"


int debug_show_objets() {
    int i;
    extern objet **objets;
    extern unsigned int nb_obj_a;
    extern unsigned int nb_obj;
    DEBUG ("--> debug_show_objets()");
    DEBUG (" nb_obj / nb_obj_a ::: %3d / %3d ", nb_obj, nb_obj_a);
    for (i=0; i < nb_obj_a; i++) {
        DEBUG (" * %d : %08X", i, objets[i] ); 


    }


    DEBUG ("<-- debug_show_objets()");
}
