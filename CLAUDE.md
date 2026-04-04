# Bomberman 2005

A Bomberman clone written in C with Allegro 4.2, originally developed as a student project at ECE Paris in 2005.

## Build

- **Language:** C (ANSI C with Allegro)
- **Original IDE:** Dev-C++ (MinGW/GCC) - see `bomberman.dev`
- **Dependency:** [Allegro 4.2](https://liballeg.org/) for graphics, input, sound, and timing
- **Target platform:** Windows XP (800x600, fullscreen in release, windowed in debug)
- **Installer:** NSIS (`installer/`)

## Project structure

```
main.c                  - Entry point, game loop, menu integration
gestion_objets.c/h      - Object lifecycle, collision, movement, grid management (~880 lines, core of the engine)
affichage.c/h           - Rendering (double-buffered), sprite animation, HUD
chargement_niveaux.c/h  - Level loading/saving from .niv text files
chargement_graphismes.c/h - BMP sprite loading, animation setup
editeur.c/h             - In-game level editor (F4 to toggle)
sons.c/h                - Sound effects (WAV) and music (MIDI)
gestion_highscore.c/h   - High score persistence and display
menu_principal.c/menus.h - Main menu UI
debug.c/h               - Conditional debug macros (enabled via #define _DEBUG_)
types.h                 - Core types: coord, coord_c, coord_p
bib.h                   - Master include (all headers)
police.h                - Font-related constants
bomberman_private.h     - Version info (0.8.0.1)

niveaux/                - 25 level files (niveau1.niv - niveau25.niv)
images/                 - BMP sprites (players, enemies, UI, tiles)
sons/                   - WAV sound effects + MIDI music
installer/              - NSIS installer scripts and resources
```

## Architecture

### The pointer/integer trick

The most distinctive design choice: grid cells store `objet*` pointers, but values < 100 are **not pointers** -- they're integer type codes cast to pointer type. This avoids allocating full `objet` structs for static tiles (walls, bonuses, diamonds).

```c
// In rendering and collision code:
l = (int) objet_actuel;
if (l == OBJ_T_MUR)           // it's just an integer, not a real pointer
    draw_sprite(sortie, sp_mur, pos.x, pos.y);
else if (l >= 100)            // it's an actual pointer to an allocated objet
    // use objet_actuel->type, ->position, etc.
```

**Simple types (< 100):** OBJ_T_MUR (1), OBJ_T_INCASSABLE (2), bonuses (21-24), OBJ_T_END_DOOR (60), OBJ_T_DIAMANT (70), OBJ_T_BIENTOT (80)

**Complex types (>= 100):** OBJ_T_BOMBERMAN (100), OBJ_T_BOMBERMAN_MORT (101), OBJ_T_MECHANT (110), OBJ_T_BOMBE (140), OBJ_T_DEFLAGRATION (141)

### Grid system

- 11x10 cells (`NB_CASES_X` x `NB_CASES_Y`), each 54px wide (`LAR_CASE_L`)
- Grid starts at pixel (30, 30) (`POS_CASES_X`, `POS_CASES_Y`)
- Each cell holds up to 10 object pointers (`NB_OBJ_CASE`), NULL-terminated
- Objects are registered/unregistered from cells as they move

### Game loop

~20 FPS (50ms frame target via adaptive `pause_affichage()`):
1. Input scan (per-player keybindings)
2. Movement with fallback speeds (full -> 4px -> 1px) + corner-sliding
3. Bomb countdown, explosion propagation (recursive via `give_life()`)
4. Bonus collection on cell entry (in `inscrire_objet()`)
5. Double-buffered render to offscreen bitmap, then blit to screen
6. Level transition when all diamonds collected and exit door reached

### Level format (.niv)

Plain text, 3 lines per object: x, y, type. Special: type 1000 = level time limit (x=minutes, y=seconds).

### Key bindings

- Player 1: Arrow keys + END (bomb)
- Player 2: WASD + E (bomb)
- F4 (double-press): Toggle level editor
- ESC: Quit current game
- Command-line: `-j <players>`, `-n <level>`, `-e` (editor), `-m` (skip menu), `-t` (no timeout)

## Code conventions

- All comments and identifiers are in **French**
- snake_case for functions and variables
- `st_` prefix for struct tags, `_t` suffix for typedefs
- `OBJ_T_` prefix for object type constants
- `extern` declarations at function scope (not file scope) -- a quirk of the era
- Debug output controlled by `_DEBUG_` preprocessor flag
