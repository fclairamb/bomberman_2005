#include "bib.h"

SAMPLE *sm_explosion,    *sm_meurt, *sm_diamant, *sm_bonus,
       *sm_deflagration, *sm_apparition_bonus;
MIDI   *mid_musique;

// Donne le pointeur de sample selon le code (défini dans sons.h)
SAMPLE * sample_from_code (unsigned int code) {
    extern SAMPLE *sm_deflagration, *sm_meurt, *sm_diamant, *sm_bonus;
    if (code == SON_BOMBERMAN_MEURT)
        return sm_meurt;
    else if (code == SON_EXPLOSION)
        return sm_explosion;
    else if (code == SON_DIAMANT)
        return sm_diamant;
    else if (code == SON_APPARITION_BONUS)
        return sm_apparition_bonus;
    else if (code == SON_BONUS)
        return sm_bonus;
    else if (code == SON_DEFLAGRATION)
        return sm_deflagration;
}


// Donne le valeur du pan selon le cote (défini dans sons.h)
int pan_from_case(coord_p _case) {
    
     return ( (_case.x * 256) / NB_CASES_X );
}

// charge tous les sons
int chargement_sons() {
    extern SAMPLE *sm_deflagration, *sm_meurt, *sm_diamant, *sm_bonus, *sm_explosion;
    extern MIDI   *mid_musique;

    //Chargement des samples
    if (! (sm_meurt = load_wav("sons/meurt.wav")) )
        ERREUR ("Pas de chargement du son de la mort");

    if (! (sm_deflagration = load_wav("sons/deflagration.wav")) )
        ERREUR ("Pas de chargement du son de l'explosion");

    if (! (sm_explosion = load_wav("sons/explode.wav")) )
        ERREUR ("Pas de chargement du son de l'explosion");

    if (! (sm_diamant = load_wav("sons/diamant.wav")) )
        ERREUR ("Pas de chargement du son de diamant");
            
    if (! (sm_bonus = load_wav("sons/bonus.wav")) )
        ERREUR ("Pas de chargement du son des bonus");   
        
    if (! (sm_apparition_bonus = load_wav("sons/apparition_bonus.wav")) )
        ERREUR ("Pas de chargement du son des bonus"); 
        
    //Chargement de la musique
    if (!(mid_musique = load_midi("sons/musique.mid")))
        ERREUR("Problème de chargement de la musique !");
    
}

// Joue un son
int joue_son (unsigned int code, coord_p _case) {
    DEBUG ("--> joue_son : %d", code);
    play_sample( sample_from_code(code), 255, pan_from_case(_case), 1000, 0);
    DEBUG ("<-- joue_son");
}


// Joue un son en continu
int joue_son_continue (unsigned int code, coord_p _case) {
    
    play_sample ( sample_from_code(code), 255, pan_from_case(_case), 1000, 1);

}

// Arrête la lecture du son joué en continu
int stop_son_continue (unsigned int code) {
    
    stop_sample ( sample_from_code( code ) );


}
