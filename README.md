# Bomberman 2005

A Bomberman clone written in C with [Allegro 4.2](https://liballeg.org/), built as a student project during my first year at [ECE Paris](https://www.ece.fr/) in 2005.

This game is quite important to me because both my girlfriend and my little sister played it intensively for years and even added their own levels (sadly lost now).

This was a two-person project: I wrote most of the code (everything except the main menu assets), the NSIS installer, and none of the assets (icons, images, sounds).

## Play it

**[Play in your browser](https://fclairamb.github.io/bomberman_2005/)** — a Phaser.js port of the original, deployed via GitHub Pages.

## Features

- **Single-player and 2-player** local modes (same keyboard)
- **25 hand-crafted levels** with diamonds to collect, enemies to avoid, and walls to blast
- **Power-ups**: extra life, speed boost, bomb power, more bombs
- **Built-in level editor** (press F4 twice during gameplay)
- **High score tracking** (top scores per game mode)
- **MIDI soundtrack** and sound effects

## Screenshots

The game runs at 800x600 on a grid of 11x10 cells. Players navigate through destructible and indestructible walls, collect diamonds to unlock the exit door, and use bombs to clear paths and defeat enemies.

## How it works

### The pointer trick

The most interesting (and admittedly crazy) technical choice: the game grid stores `objet*` pointers in each cell, but values below 100 aren't actually pointers -- they're just integers cast to pointer type, describing simple static elements (walls, bonuses, diamonds). Only values >= 100 are real pointers to allocated game objects (players, bombs, enemies).

This avoids allocating full structures for the majority of grid contents while keeping a single unified array per cell.

### Architecture

```
main.c                  Game loop, input handling, menu flow
gestion_objets.c        Object creation, destruction, collision, movement (~880 lines)
affichage.c             Double-buffered rendering, sprite animation, HUD
chargement_niveaux.c    Level loading/saving (plain text .niv format)
chargement_graphismes.c BMP sprite and font loading
editeur.c               In-game level editor
sons.c                  WAV/MIDI audio playback
gestion_highscore.c     Score persistence
menu_principal.c        Main menu
```

### Level format

Levels are plain text files (`niveaux/niveau1.niv` through `niveau25.niv`). Each object is 3 lines: x coordinate, y coordinate, type code. A special type (1000) encodes the level time limit.

### Game loop

Runs at ~20 FPS with adaptive frame pacing. Each frame: scan input, attempt movement with fallback speeds (full speed, then 4px, then 1px) plus corner-sliding for smoother navigation, tick bombs and explosions, render to offscreen buffer, blit to screen.

## Building

Originally built with **Dev-C++** (MinGW/GCC) on Windows XP. The project file is `bomberman.dev`. Requires Allegro 4.2 headers and `alleg42.dll` at runtime.

A pre-built `bomberman.exe` is included in the repository.

### Command-line options

```
-j <number>    Number of players
-n <level>     Starting level
-m             Skip the menu
-e             Launch the level editor directly
-t             Double the time limit
-h / --help    Show help
```

## Controls

| Action | Player 1 | Player 2 |
|--------|----------|----------|
| Move   | Arrow keys | WASD |
| Bomb   | END | E |

**Editor**: F4 (double-press) to enter/exit. F1 to load, F2 to save, mouse to place objects.

## License

This project is archived as-is. It was never cleaned up after the original 2005 development and is preserved here for sentimental and historical value.
