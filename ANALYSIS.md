# Code Analysis

An honest assessment of the Bomberman 2005 codebase, written by Claude in April 2026.

## Overall verdict

For a first-year student project in 2005, this is genuinely impressive. The architecture shows real understanding of game programming fundamentals: game loops, grid-based collision, sprite animation, state machines, and resource management. The fact that it shipped as a complete, playable game with 25 levels, an editor, sound, high scores, and multiplayer is remarkable for a first-year project.

The code reads like someone who understood the *concepts* but was still developing taste for *structure*. That's exactly where a first-year student should be.

**Grade: solid B+**

## What's good

### It works, and it's fun
The game was played intensively by real people for years. That matters more than any code quality metric.

### Clear modular structure
The separation into rendering (`affichage.c`), objects (`gestion_objets.c`), levels (`chargement_niveaux.c`), audio (`sons.c`), editor (`editeur.c`), and menus (`menu_principal.c`) is solid. Each module has a well-defined responsibility. Many student projects are a single 2000-line `main.c` -- this one has 23 source files with clear boundaries.

### The pointer/integer trick is wild
Casting integers < 100 as fake pointers to avoid allocating structs for static tiles is technically undefined behavior and would make any C standards committee member faint -- but it's also genuinely clever resource optimization for a 2005 student who understood memory layout. It works because on 32-bit Windows, small integers are never valid heap addresses. It's the kind of hack that shows someone thinking about the problem rather than following a textbook.

### The level editor is a real feature
Having a working in-game editor with load/save, mouse input, and keyboard shortcuts at this level of experience shows ambition. Many professional games ship without mod tools; this student project has one.

### `give_life()` is a complete state machine
It handles bomb timers, explosion propagation in 4 directions, wall destruction, player death and respawn with invincibility, enemy AI (simple direction cycling), and the "bientot" (upcoming) object transition -- all working correctly together. The logic is complete.

### Adaptive frame pacing
`pause_affichage()` dynamically adjusts sleep time based on actual frame duration to maintain consistent speed across different hardware. A smart touch that many student games skip entirely.

### Graceful graphics fallback
The initialization code tries 32bpp, then 24, 16, and 8-bit color depths sequentially. Defensive and practical for the era of wildly varying graphics hardware.

### Spatial audio
`pan_from_case()` pans sound effects left/right based on the object's grid position. A small detail that shows care for the player experience.

## What's rough

### Global variables everywhere
30+ `extern` declarations in `main()` alone. Every module reaches into shared mutable state. The grid (`cases`), players (`joueurs`), object list (`objets`), display buffer (`sortie`), sprites, animations, sounds -- all globals. This makes the code hard to reason about: any function can modify anything at any time.

### `give_life()` is a god function
At ~200 lines, it handles bombs, explosions, player death, respawn, enemy AI, and object type transitions. It should be 5-6 separate functions (`tick_bomb()`, `tick_explosion()`, `tick_player()`, `tick_enemy()`, etc.), but it works as-is.

### The `typeb`/`typec`/`typed` fields are opaque
These fields on `objet` mean completely different things depending on object type:
- For bombs: `typeb` = time to detonation, `typec` = explosion radius, `typed` = owner player
- For explosions: `typeb` = remaining lifetime, `typec` = remaining propagation distance, `typed` = owner player
- For dead bomberman: `typeb` = respawn timer

A union or separate structs per object type would be far clearer. As-is, you need to mentally track what each field means in each context.

### `inscrire_objet()` has hidden side effects
What sounds like "register object in cell" also handles: diamond collection and scoring, door unlocking, all four bonus types, and triggering level transitions. This violates the principle of least surprise -- the function name promises one thing and does six.

### No `free()` discipline
`creer_objet()` allocates with `malloc()` but cleanup paths are incomplete. `vider_les_pointeurs()` deliberately skips bomberman objects. There's no cleanup on game exit (the OS reclaims everything, but it's still sloppy). The `detruire_objet()` function uses the freed pointer address in a DEBUG statement after `free()` -- harmless but technically UB.

### The realloc-by-one pattern
`creer_objet()` grows the `objets` array one element at a time via `realloc()`. This is O(n^2) over n object creations. A doubling strategy would be better, though with the small number of objects in a Bomberman game (~50-100 max), it doesn't matter in practice.

### Inconsistent indentation and bracing
Tabs and spaces are mixed throughout. Brace placement varies between same-line and next-line. The `main.c` game loop has deeply nested `if`/`for`/`while` blocks that are hard to follow without consistent formatting. Some blocks lack braces entirely (e.g., `for` loops controlling `if` statements).

### Variables declared far from use
Following C89 conventions (or habit), variables are declared in large blocks at function tops, often dozens of lines before first use. This makes it harder to track what's in scope.

### Hardcoded Windows paths
Level files use backslash paths (`niveaux\\niveau%d.niv`), making the code non-portable. A minor point given the Windows-only Allegro 4.2 dependency.

### Player 3 keybindings are a copy of Player 2
`touches[2]` is identical to `touches[1]` (both WASD+E), so 3+ player mode wouldn't actually work.

### Missing return values
Several functions declared as returning `int` don't always return a value (`chargement_niveau()`, `chargement_sons()`, `nettoyer_carte()`, `faire_disparaitre()`). Works on most compilers but is technically undefined behavior.

## Notable code moments

### The "nightly build" comment
```c
/*******************************************************************************
*** JE CROIS QUE LA ON PEUT PARLER DE NIGHTLY BUILD                          ***
*******************************************************************************/
```
Translation: "I think at this point we can call this a nightly build." Peak student energy.

### The existential error message
```c
ERREUR("ERREUR FATALE, en fait non, mais bon, comme ca devrait pas arriver on va dire que si !");
```
Translation: "FATAL ERROR, well actually no, but since this shouldn't happen let's say it is!"

### The philosophical debug comment
```c
// Si give_life retourne FALSE, c'est que give_live vient de reprendre
// la vie qu'il animait jusque la. Comme c'est beau...
```
Translation: "If give_life returns FALSE, it means give_life just took back the life it was animating until now. How beautiful..."

### The self-aware anger
```c
// N'IMPORTE QUOI : y'avait un simple free
```
Translation: "NONSENSE: there was just a simple free"

```c
//nb_obj--; //<-- BORDEL : le nb_obj etait deja inclus dans le detruire_objet();
```
Translation: "DAMN: the nb_obj was already included in detruire_objet()"

These comments are the most authentic documentation in the entire project. You can feel the debugging sessions.

## Stats

| Metric | Value |
|--------|-------|
| Total C code | ~3,300 lines across 23 files |
| Largest file | `gestion_objets.c` (~880 lines) |
| Levels | 25 |
| Sprites | 37 BMP files |
| Sound effects | 13 WAV + 1 MIDI |
| Object types | 13 (5 simple, 5 complex, 3 bonus-related) |
| Max players | 4 (2 tested, 3-4 have duplicate keybindings) |
| Frame rate | ~20 FPS |
| Resolution | 800x600 |
| Original platform | Windows XP |
| Original IDE | Dev-C++ (MinGW/GCC) |
