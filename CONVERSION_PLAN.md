# Bomberman 2005 → Phaser.js Conversion Plan

Target: **Phaser v3.90.0** (latest stable as of 2026). Phaser 4 is in RC7 but not yet stable — we can upgrade later if needed.

Stack: **TypeScript + Vite + Phaser 3.90**

---

## Phase 0: Project Scaffolding

### 0.1 Initialize the web project

```
web/
├── index.html
├── package.json
├── tsconfig.json
├── vite.config.ts
├── src/
│   ├── main.ts              # Phaser.Game bootstrap
│   ├── config.ts             # Game constants (grid size, speeds, types)
│   ├── scenes/
│   │   ├── BootScene.ts      # Asset preloading
│   │   ├── MenuScene.ts      # Main menu
│   │   ├── GameScene.ts      # Core gameplay
│   │   ├── EditorScene.ts    # Level editor
│   │   ├── HighScoreScene.ts # High score display
│   │   ├── GameOverScene.ts  # Game over screen
│   │   └── HUDScene.ts       # Overlay HUD (parallel scene)
│   ├── objects/
│   │   ├── Player.ts         # Bomberman player (extends Phaser.GameObjects.Sprite)
│   │   ├── Enemy.ts          # Mechant enemy
│   │   ├── Bomb.ts           # Bomb with countdown
│   │   ├── Explosion.ts      # Deflagration propagation
│   │   └── Bonus.ts          # Pickup items
│   ├── systems/
│   │   ├── Grid.ts           # 11x10 cell grid management
│   │   ├── CollisionSystem.ts # 4-corner collision + fallback speeds
│   │   ├── LevelLoader.ts    # .niv parser + JSON level support
│   │   └── ScoringSystem.ts  # Score tracking, diamond counting
│   ├── data/
│   │   └── levels/           # Converted level JSON files
│   └── types.ts              # Shared TypeScript interfaces
├── public/
│   ├── assets/
│   │   ├── sprites/          # Converted PNG spritesheets
│   │   ├── ui/               # Menu backgrounds, HUD, buttons
│   │   ├── audio/            # Converted audio files
│   │   └── levels/           # Level JSON files (also loadable at runtime)
│   └── favicon.ico
```

### 0.2 Dependencies

```json
{
  "devDependencies": {
    "typescript": "^5.7",
    "vite": "^6.0",
    "@types/node": "^22"
  },
  "dependencies": {
    "phaser": "^3.90.0"
  }
}
```

### 0.3 Phaser game config

```typescript
const config: Phaser.Types.Core.GameConfig = {
  type: Phaser.AUTO,           // WebGL with Canvas fallback
  width: 800,
  height: 600,
  parent: 'game-container',
  pixelArt: true,              // Nearest-neighbor scaling for retro sprites
  scale: {
    mode: Phaser.Scale.FIT,    // Scale to fit browser window
    autoCenter: Phaser.Scale.CENTER_BOTH
  },
  scene: [BootScene, MenuScene, GameScene, HUDScene, EditorScene, HighScoreScene, GameOverScene]
};
```

---

## Phase 1: Asset Conversion

### 1.1 Sprites: BMP → PNG spritesheets

**Why:** BMPs are uncompressed, unsupported by browsers, and loading 143 individual files is slow. PNGs with transparency + spritesheets are the web standard.

**Conversion strategy:**

| Asset Group | Source | Target | Method |
|-------------|--------|--------|--------|
| Player 1 (4 dirs × 5 frames) | 20 BMPs in `images/player1/` | 1 spritesheet PNG + JSON atlas | ImageMagick `montage` + TexturePacker/free-tex-packer |
| Player 2 (4 dirs × 5 frames) | 20 BMPs in `images/player2/` | 1 spritesheet PNG + JSON atlas | Same |
| Enemy (4 dirs × 5 frames) | 20 BMPs in `images/mechant/` | 1 spritesheet PNG + JSON atlas | Same |
| Bomb (4 frames) | 4 BMPs `bombe_0-3.bmp` | 1 spritesheet PNG | Same |
| Explosion (4 frames) | 4 BMPs `explosion_0-3.bmp` | 1 spritesheet PNG | Same |
| Death (5 frames) | 5 BMPs `mort_0-4.bmp` | 1 spritesheet PNG | Same |
| Tiles (wall, floor, door, diamond, 4 bonuses) | 8 individual BMPs | 1 tiles spritesheet PNG | Same |
| UI backgrounds | `jeu.bmp`, `menu_principal.bmp`, `fond_highscores.bmp`, `game_over.bmp`, `pause.bmp` | Individual PNGs (compressed) | `convert X.bmp X.png` |
| Menu buttons (5 × normal + hover) | 10 BMPs | 1 UI spritesheet or individual PNGs | Same |
| Editor UI | 3 BMPs | Individual PNGs | Direct conversion |

**Conversion script** (`scripts/convert_assets.sh`):

```bash
#!/bin/bash
# Requires: ImageMagick (convert, montage), optionally free-tex-packer-cli

SRC="../images"
DST="../web/public/assets"

mkdir -p "$DST/sprites" "$DST/ui" "$DST/audio"

# 1. Handle transparency: original BMPs use magic pink (255,0,255) as transparent color
#    Convert to PNG with alpha channel
convert_with_transparency() {
  convert "$1" -transparent "rgb(255,0,255)" "$2"
}

# 2. Individual tiles → tiles spritesheet
for f in sol caisse incassable end_door diamant bonus_vitesse bonus_explosion bonus_bombes bonus_sante; do
  convert_with_transparency "$SRC/$f.bmp" "$DST/sprites/$f.png"
done

# 3. Player animations → spritesheets (one per player)
for player in player1 player2; do
  for dir in haut bas gauche droite; do
    for i in $(seq 0 4); do
      convert_with_transparency "$SRC/$player/${dir}_${i}.bmp" "/tmp/${player}_${dir}_${i}.png"
    done
  done
  # Montage into single sheet: 5 columns (frames) × 4 rows (directions)
  montage /tmp/${player}_haut_*.png /tmp/${player}_droite_*.png \
          /tmp/${player}_bas_*.png /tmp/${player}_gauche_*.png \
          -tile 5x4 -geometry +0+0 -background none "$DST/sprites/${player}.png"
done

# 4. Enemy → spritesheet
for dir in haut bas gauche droite; do
  for i in $(seq 0 4); do
    convert_with_transparency "$SRC/mechant/${dir}_${i}.bmp" "/tmp/mechant_${dir}_${i}.png"
  done
done
montage /tmp/mechant_haut_*.png /tmp/mechant_droite_*.png \
        /tmp/mechant_bas_*.png /tmp/mechant_gauche_*.png \
        -tile 5x4 -geometry +0+0 -background none "$DST/sprites/mechant.png"

# 5. Bomb, explosion, death → spritesheets
for prefix in bombe explosion; do
  for i in $(seq 0 3); do
    convert_with_transparency "$SRC/${prefix}_${i}.bmp" "/tmp/${prefix}_${i}.png"
  done
  montage /tmp/${prefix}_*.png -tile 4x1 -geometry +0+0 -background none "$DST/sprites/${prefix}.png"
done

for i in $(seq 0 4); do
  convert_with_transparency "$SRC/mort_${i}.bmp" "/tmp/mort_${i}.png"
done
montage /tmp/mort_*.png -tile 5x1 -geometry +0+0 -background none "$DST/sprites/mort.png"

# 6. UI backgrounds (large BMPs → compressed PNGs)
for f in jeu menu_principal fond_highscores game_over pause; do
  convert "$SRC/$f.bmp" -quality 95 "$DST/ui/$f.png"
done

# 7. Menu buttons
for f in un_joueur un_joueur_hover deux_joueurs deux_joueurs_hover \
         highscores highscores_hover editeur editeur_hover \
         quitter quitter_hover; do
  convert_with_transparency "$SRC/$f.bmp" "$DST/ui/$f.png"
done

echo "Asset conversion complete."
```

**Key detail — transparency:** The original BMPs use Allegro's magic pink `rgb(255, 0, 255)` for transparency. ImageMagick's `-transparent` flag handles this. Verify by inspecting a few sprites visually after conversion.

**Sprite dimensions:** All tile sprites are 54×54 px (matching `LAR_CASE_L`). Player/enemy sprites are smaller (~40×40). The conversion must preserve exact pixel dimensions for the grid system to work.

### 1.2 Audio: WAV + MIDI → OGG/MP3

| Source | Target | Method |
|--------|--------|--------|
| 10 WAV files in `sons/` | OGG (primary) + MP3 (fallback) | `ffmpeg -i X.wav X.ogg` + `ffmpeg -i X.wav X.mp3` |
| `musique.mid` (MIDI) | OGG + MP3 | `timidity musique.mid -Ow -o musique.wav` then convert, OR use a SoundFont-based renderer for better quality |

**Why dual format:** Phaser auto-selects OGG (smaller, better quality) or MP3 (Safari fallback) based on browser support.

**MIDI conversion note:** MIDI files contain no audio data — they're instructions. The rendered sound depends entirely on the SoundFont used. For authentic reproduction, use a General MIDI SoundFont (e.g., FluidR3_GM) with TiMidity++ or FluidSynth. Listen to the result and adjust if needed.

```bash
# WAV → OGG + MP3
for f in sons/*.wav; do
  name=$(basename "$f" .wav)
  ffmpeg -i "$f" -c:a libvorbis -q:a 6 "web/public/assets/audio/${name}.ogg"
  ffmpeg -i "$f" -c:a libmp3lame -q:a 2 "web/public/assets/audio/${name}.mp3"
done

# MIDI → WAV → OGG + MP3
timidity sons/musique.mid -Ow -o /tmp/musique.wav
ffmpeg -i /tmp/musique.wav -c:a libvorbis -q:a 6 web/public/assets/audio/musique.ogg
ffmpeg -i /tmp/musique.wav -c:a libmp3lame -q:a 2 web/public/assets/audio/musique.mp3
```

### 1.3 Levels: .niv → JSON

Convert the 25 `.niv` text files into a structured JSON format that's easier to parse in TypeScript.

**Original format** (3 lines per object):
```
0
3
2
```

**Target JSON format:**
```json
{
  "level": 1,
  "timeLimit": { "minutes": 3, "seconds": 0 },
  "objects": [
    { "x": 0, "y": 3, "type": 2 },
    { "x": 0, "y": 4, "type": 60 },
    { "x": 0, "y": 5, "type": 100 }
  ]
}
```

**Conversion script** (`scripts/convert_levels.py`):
```python
import json, sys, os

for i in range(1, 26):
    path = f"niveaux/niveau{i}.niv"
    if not os.path.exists(path):
        continue
    with open(path) as f:
        lines = [l.strip() for l in f if l.strip()]

    objects = []
    time_limit = {"minutes": 3, "seconds": 0}

    for j in range(0, len(lines) - 2, 3):
        x, y, t = int(lines[j]), int(lines[j+1]), int(lines[j+2])
        if t == 1000:
            time_limit = {"minutes": x, "seconds": y}
        else:
            objects.append({"x": x, "y": y, "type": t})

    level_data = {"level": i, "timeLimit": time_limit, "objects": objects}
    out_path = f"web/public/assets/levels/niveau{i}.json"
    os.makedirs(os.path.dirname(out_path), exist_ok=True)
    with open(out_path, "w") as f:
        json.dump(level_data, f, indent=2)

print(f"Converted {i} levels")
```

---

## Phase 2: Core Engine (config + grid + types)

### 2.1 Constants (`src/config.ts`)

Direct port of all C `#define` constants:

```typescript
export const GRID = {
  COLS: 11,          // NB_CASES_X
  ROWS: 10,          // NB_CASES_Y
  CELL_SIZE: 54,     // LAR_CASE_L
  OFFSET_X: 30,      // POS_CASES_X
  OFFSET_Y: 30,      // POS_CASES_Y
  MAX_OBJECTS: 10,   // NB_OBJ_CASE
} as const;

export const OBJ_HALF_SIZE = 25;  // OBJ_LAR — collision half-width

export const FRAME_TIME = 50;     // ms per frame (20 FPS target)

export enum ObjectType {
  // Simple types (static tiles, no allocated state)
  MUR = 1,                    // Destructible wall
  INCASSABLE = 2,             // Indestructible wall
  BONUS_VIE = 21,
  BONUS_VITESSE = 22,
  BONUS_BOMBE_PUISSANCE = 23,
  BONUS_BOMBE_NOMBRE = 24,
  END_DOOR = 60,
  DIAMANT = 70,
  BIENTOT = 80,               // Pending/transition placeholder

  // Complex types (have game objects with state)
  BOMBERMAN = 100,
  BOMBERMAN_MORT = 101,
  MECHANT = 110,
  BOMBE = 140,
  DEFLAGRATION = 141,
}

export enum Direction {
  NONE = 0,
  RIGHT = 1,
  DOWN = 2,
  LEFT = 3,
  UP = 4,
}

export const MAX_PLAYERS = 4;
export const MAX_LEVELS = 25;

export const SCORING = {
  DIAMOND: 100,
  DOOR: 200,
  ENEMY_KILL: 50,
} as const;

export const BOMB_FUSE = 40;          // Frames before detonation
export const EXPLOSION_LIFETIME = 40; // Frames explosion persists
export const EXPLOSION_WALL_BREAK = 31; // Frame at which walls break
export const EXPLOSION_SPREAD = 30;    // Frame at which explosion propagates
export const DEATH_INVINCIBILITY = 50; // Frames of invincibility after respawn
export const BOMB_RECOVERY_INTERVAL = 100; // Frames between bomb recovery
```

### 2.2 Grid System (`src/systems/Grid.ts`)

This replaces the C `carre cases[11][10]` array and the pointer/integer trick.

```typescript
// Each cell holds a mix of static type codes and references to game objects
interface CellEntry {
  type: ObjectType;
  gameObject?: Phaser.GameObjects.Sprite;  // null for simple types
}

class Grid {
  private cells: CellEntry[][][];  // [col][row] → array of entries

  constructor() {
    this.cells = Array.from({ length: GRID.COLS }, () =>
      Array.from({ length: GRID.ROWS }, () => [])
    );
  }

  register(col: number, row: number, entry: CellEntry): boolean { ... }
  unregister(col: number, row: number, entry: CellEntry): void { ... }
  getEntries(col: number, row: number): CellEntry[] { ... }
  hasObstacle(col: number, row: number): boolean { ... }
  clear(): void { ... }
  clearCell(col: number, row: number): void { ... }

  // Coordinate conversions (replaces C inline math)
  pixelToCell(px: number, py: number): { col: number; row: number } { ... }
  cellToPixel(col: number, row: number): { x: number; y: number } { ... }
}
```

**Key change from C:** No more pointer/integer trick. Static tiles (walls, bonuses) are `CellEntry` objects with `type` set and `gameObject` either null (for logic-only entries) or pointing to a static Phaser sprite. Complex objects (players, bombs) always have a `gameObject`.

### 2.3 Collision System (`src/systems/CollisionSystem.ts`)

Faithful port of the original's 4-corner collision check with fallback speeds.

```typescript
class CollisionSystem {
  constructor(private grid: Grid) {}

  /**
   * Attempt to move an object in a direction.
   * Tries full speed, then 4px, then 1px (matching original C behavior).
   * Returns actual displacement or 0 if blocked.
   */
  tryMove(
    x: number, y: number,
    direction: Direction,
    speed: number
  ): { dx: number; dy: number } {
    // Try speeds in order: full, 4, 1
    for (const trySpeed of [speed, Math.min(speed, 4), 1]) {
      const { dx, dy } = this.directionToVector(direction, trySpeed);
      if (this.canMove(x, y, dx, dy)) {
        return { dx, dy };
      }
    }
    // Corner-sliding: try perpendicular nudge
    return this.tryCornerSlide(x, y, direction);
  }

  /**
   * 4-corner check: tests OBJ_HALF_SIZE offsets in movement direction.
   * Direct port of yatil_obstacle_pos() logic.
   */
  private canMove(x: number, y: number, dx: number, dy: number): boolean { ... }
  private tryCornerSlide(x: number, y: number, dir: Direction): { dx: number; dy: number } { ... }
}
```

**Why not use Phaser Arcade Physics:** The original collision model is grid-based with very specific fallback behavior (full → 4px → 1px → corner-slide). Phaser's Arcade physics uses AABB overlap resolution which would change the feel. A custom collision system preserves the exact original gameplay.

---

## Phase 3: Game Objects

### 3.1 Player (`src/objects/Player.ts`)

```typescript
class Player extends Phaser.GameObjects.Sprite {
  // Replaces t_infos struct
  public playerIndex: number;
  public lives: number;
  public bombsDeployed: number;
  public maxBombs: number;
  public bombPower: number;
  public score: number;
  public moveSpeed: number;
  public isDead: boolean;
  public invincibilityTimer: number;

  // Grid position (replaces coord_c)
  public gridCol: number;
  public gridRow: number;

  // Movement state
  public direction: Direction;
  public isMoving: boolean;

  // Key bindings
  private keys: Record<string, Phaser.Input.Keyboard.Key>;

  constructor(scene: GameScene, col: number, row: number, playerIndex: number) {
    super(scene, ...);
    this.setupAnimations();  // 4 directions × 5 frames each
    this.setupKeys();
  }

  update(time: number, delta: number): void {
    if (this.isDead) {
      this.handleDeath();
      return;
    }
    this.handleInput();
    this.handleMovement();
    this.checkDangers();      // Explosion/enemy contact
  }

  placeBomb(): void { ... }
  collectBonus(type: ObjectType): void { ... }
  die(killerPlayerIndex?: number): void { ... }
  respawn(): void { ... }
}
```

**Animation mapping:**
- Original: `anim_joueurs[playerIndex][direction]` → array of BITMAP frames
- Phaser: `this.anims.play(`player${index}_${dirName}`)` using atlas frames

### 3.2 Enemy (`src/objects/Enemy.ts`)

```typescript
class Enemy extends Phaser.GameObjects.Sprite {
  public direction: Direction;
  private moveSpeed = 2;  // 2px per frame (original value)

  update(): void {
    // Port of C enemy AI: move in current direction, rotate on collision
    const moved = this.scene.collisionSystem.tryMove(
      this.x, this.y, this.direction, this.moveSpeed
    );
    if (moved.dx === 0 && moved.dy === 0) {
      this.direction = (this.direction % 4) + 1;  // Rotate: R→D→L→U→R
    } else {
      this.x += moved.dx;
      this.y += moved.dy;
    }
  }
}
```

### 3.3 Bomb (`src/objects/Bomb.ts`)

```typescript
class Bomb extends Phaser.GameObjects.Sprite {
  public fuseTimer: number = BOMB_FUSE;   // 40 frames
  public power: number;                    // Explosion radius
  public ownerIndex: number;               // Player who placed it

  update(): void {
    this.fuseTimer--;
    if (this.fuseTimer <= 0) {
      this.detonate();
    }
  }

  detonate(): void {
    // Replace self with explosion
    // Spawn 4-directional Explosion objects with this.power radius
    this.scene.spawnExplosion(this.gridCol, this.gridRow, this.power, this.ownerIndex);
    this.destroy();
  }
}
```

### 3.4 Explosion (`src/objects/Explosion.ts`)

```typescript
class Explosion extends Phaser.GameObjects.Sprite {
  public lifetime: number = EXPLOSION_LIFETIME;
  public propagationRadius: number;
  public direction: Direction;  // NONE for center, directional for arms
  public ownerIndex: number;
  private hasSpread = false;
  private hasBrokenWalls = false;

  update(): void {
    this.lifetime--;

    // At frame 31: destroy walls in this cell
    if (!this.hasBrokenWalls && this.lifetime <= EXPLOSION_WALL_BREAK) {
      this.hasBrokenWalls = true;
      this.scene.grid.destroyWallAt(this.gridCol, this.gridRow);
    }

    // At frame 30: propagate to adjacent cells
    if (!this.hasSpread && this.lifetime <= EXPLOSION_SPREAD && this.propagationRadius > 0) {
      this.hasSpread = true;
      this.scene.propagateExplosion(this);
    }

    // At frame 0: remove
    if (this.lifetime <= 0) {
      this.destroy();
    }
  }
}
```

**Propagation logic** (in `GameScene`): For each of the 4 cardinal directions, spawn a new `Explosion` at the adjacent cell if it's not blocked by an indestructible wall. Each child explosion has `propagationRadius - 1`. This creates the classic Bomberman cross pattern.

### 3.5 Bonus (`src/objects/Bonus.ts`)

Static sprites placed on the grid. No update logic — collection is handled by the Player when entering a cell (matching the original's `inscrire_objet()` side-effect pattern, but cleaner).

---

## Phase 4: Scenes

### 4.1 BootScene — Asset Loading

```typescript
class BootScene extends Phaser.Scene {
  preload() {
    // Progress bar
    this.load.on('progress', (value: number) => { ... });

    // Spritesheets (with frame dimensions)
    this.load.spritesheet('player1', 'assets/sprites/player1.png',
      { frameWidth: 54, frameHeight: 54 });  // Adjust to actual sprite size
    this.load.spritesheet('player2', 'assets/sprites/player2.png',
      { frameWidth: 54, frameHeight: 54 });
    this.load.spritesheet('mechant', 'assets/sprites/mechant.png',
      { frameWidth: 54, frameHeight: 54 });
    this.load.spritesheet('bombe', 'assets/sprites/bombe.png',
      { frameWidth: 54, frameHeight: 54 });
    this.load.spritesheet('explosion', 'assets/sprites/explosion.png',
      { frameWidth: 54, frameHeight: 54 });
    this.load.spritesheet('mort', 'assets/sprites/mort.png',
      { frameWidth: 54, frameHeight: 54 });

    // Static tile images
    for (const tile of ['sol', 'caisse', 'incassable', 'end_door', 'diamant',
                         'bonus_vitesse', 'bonus_explosion', 'bonus_bombes', 'bonus_sante']) {
      this.load.image(tile, `assets/sprites/${tile}.png`);
    }

    // UI
    this.load.image('menu_bg', 'assets/ui/menu_principal.png');
    this.load.image('game_hud', 'assets/ui/jeu.png');
    this.load.image('game_over', 'assets/ui/game_over.png');
    // ... menu buttons

    // Audio (Phaser auto-selects format)
    this.load.audio('explode', ['assets/audio/explode.ogg', 'assets/audio/explode.mp3']);
    this.load.audio('meurt', ['assets/audio/meurt.ogg', 'assets/audio/meurt.mp3']);
    this.load.audio('diamant', ['assets/audio/diamant.ogg', 'assets/audio/diamant.mp3']);
    this.load.audio('bonus', ['assets/audio/bonus.ogg', 'assets/audio/bonus.mp3']);
    this.load.audio('deflagration', ['assets/audio/deflagration.ogg', 'assets/audio/deflagration.mp3']);
    this.load.audio('musique', ['assets/audio/musique.ogg', 'assets/audio/musique.mp3']);
    // ... other sounds

    // Levels
    for (let i = 1; i <= 25; i++) {
      this.load.json(`level${i}`, `assets/levels/niveau${i}.json`);
    }
  }

  create() {
    this.defineAnimations();  // Register all animation sequences
    this.scene.start('MenuScene');
  }
}
```

### 4.2 MenuScene — Main Menu

Port of `menu_principal.c`. Mouse-driven button selection with hover states.

```typescript
class MenuScene extends Phaser.Scene {
  create() {
    this.add.image(400, 300, 'menu_bg');

    const buttons = [
      { y: 163, key: '1player', action: () => this.startGame(1) },
      { y: 223, key: '2players', action: () => this.startGame(2) },
      { y: 283, key: 'highscores', action: () => this.scene.start('HighScoreScene') },
      { y: 343, key: 'editor', action: () => this.scene.start('EditorScene') },
      { y: 403, key: 'quit', action: () => { /* no-op on web, or show credits */ } },
    ];

    for (const btn of buttons) {
      const img = this.add.image(217, btn.y, btn.key)
        .setInteractive({ useHandCursor: true })
        .on('pointerover', () => img.setTexture(`${btn.key}_hover`))
        .on('pointerout', () => img.setTexture(btn.key))
        .on('pointerdown', btn.action);
    }
  }
}
```

### 4.3 GameScene — Core Gameplay

The main scene. Replaces `main.c`'s game loop + `affichage()` + `give_life()`.

```typescript
class GameScene extends Phaser.Scene {
  public grid!: Grid;
  public collisionSystem!: CollisionSystem;
  public players: Player[] = [];
  public enemies: Enemy[] = [];
  public bombs: Bomb[] = [];
  public explosions: Explosion[] = [];

  private currentLevel = 1;
  private diamondsRemaining = 0;
  private timeLimit = 0;
  private elapsedTime = 0;
  private frameCounter = 0;

  create(data: { numPlayers: number; startLevel?: number }) {
    this.grid = new Grid();
    this.collisionSystem = new CollisionSystem(this.grid);

    // Launch HUD as parallel scene (overlay)
    this.scene.launch('HUDScene', { gameScene: this });

    // Start background music
    this.sound.play('musique', { loop: true, volume: 0.35 });

    this.loadLevel(data.startLevel ?? 1);
  }

  update(time: number, delta: number) {
    this.frameCounter++;

    // Update all game objects (replaces give_life() calls in affichage())
    for (const player of this.players) player.update(time, delta);
    for (const enemy of this.enemies) enemy.update(time, delta);
    for (const bomb of this.bombs) bomb.update(time, delta);
    for (const explosion of this.explosions) explosion.update(time, delta);

    // Bomb recovery (every BOMB_RECOVERY_INTERVAL frames)
    if (this.frameCounter % BOMB_RECOVERY_INTERVAL === 0) {
      for (const player of this.players) player.recoverBomb();
    }

    // Bonus spawning (every 800 frames, matching original)
    if (this.frameCounter % 800 === 0) {
      this.spawnRandomBonus();
    }

    // Time limit check
    this.elapsedTime += delta;
    if (this.timeLimit > 0 && this.elapsedTime >= this.timeLimit * 1000) {
      this.gameOver();
    }

    // Check level completion
    if (this.diamondsRemaining <= 0) {
      // Enable exit door visibility + check if player reached it
    }

    // All players dead check
    if (this.players.every(p => p.lives <= 0)) {
      this.gameOver();
    }
  }

  loadLevel(levelNum: number): void { ... }
  spawnExplosion(col: number, row: number, power: number, owner: number): void { ... }
  propagateExplosion(source: Explosion): void { ... }
  nextLevel(): void { ... }
  gameOver(): void { ... }
}
```

**Frame rate note:** The original runs at 20 FPS with 50ms frames. Phaser runs at 60 FPS by default. Two approaches:

1. **Recommended:** Run at 60 FPS but scale all timers by `delta`. Convert frame-based timers (e.g., bomb fuse = 40 frames at 20 FPS = 2000ms) to millisecond-based timers. This gives smoother animation.
2. **Alternative:** Lock Phaser to 20 FPS via `fps: { target: 20 }` in game config for pixel-perfect original behavior.

**Decision: Use 60 FPS with time-based logic.** The original frame counts translate:
- `BOMB_FUSE = 40 frames × 50ms = 2000ms`
- `EXPLOSION_LIFETIME = 40 frames × 50ms = 2000ms`
- `DEATH_INVINCIBILITY = 50 frames × 50ms = 2500ms`
- `BOMB_RECOVERY_INTERVAL = 100 frames × 50ms = 5000ms`

### 4.4 HUDScene — Overlay

Runs as a parallel scene on top of GameScene. Replaces the HUD rendering in `affichage()`.

```typescript
class HUDScene extends Phaser.Scene {
  create(data: { gameScene: GameScene }) {
    this.add.image(400, 300, 'game_hud');  // Background frame
    // Per-player stats at positions matching original:
    // Player 1: lives at (686, 33), score at (717, 33)
    // Diamond counter at (765, 314)
    // Time display below
  }

  update() {
    // Refresh text values from GameScene state
  }
}
```

### 4.5 EditorScene — Level Editor

Port of `editeur.c`. Mouse-click to place/remove objects, keyboard shortcuts for type selection.

```typescript
class EditorScene extends Phaser.Scene {
  private currentLevel = 1;
  private grid!: Grid;

  create() {
    this.grid = new Grid();
    // Render grid lines
    // Mouse click → cell lookup → show type selector popup
    // F1 = load, F2 = save, F5 = set time, N = clear
    // Right-click = remove
    // Save/load as JSON via download/upload or localStorage
  }
}
```

**Web adaptation:** The original editor saves to disk. For the web version:
- **Save:** Download JSON file or store in `localStorage`
- **Load:** File input dialog or `localStorage`
- **Optional:** URL-encoded level sharing

### 4.6 HighScoreScene

Port of `gestion_highscore.c`. Uses `localStorage` instead of binary files.

```typescript
class HighScoreScene extends Phaser.Scene {
  create() {
    const scores = JSON.parse(localStorage.getItem('highscores_1p') ?? '[]');
    // Render ranked list
    // Name input via HTML overlay or Phaser text input
  }
}
```

---

## Phase 5: Input System

### 5.1 Keyboard Bindings

Direct port of the original keybindings:

| Player | Up | Down | Left | Right | Bomb |
|--------|----|------|------|-------|------|
| 1 | Arrow Up | Arrow Down | Arrow Left | Arrow Right | End |
| 2 | W | S | A | D | E |

**Web adaptation:** Replace `End` key with `Space` for Player 1 (more natural on web). Keep `End` as an alternative.

### 5.2 Gamepad Support (enhancement)

Phaser has built-in gamepad support. Add as optional enhancement:
```typescript
this.input.gamepad?.on('connected', (pad: Phaser.Input.Gamepad.Gamepad) => {
  // Map D-pad to directions, A button to bomb
});
```

### 5.3 Touch/Mobile (optional enhancement)

Virtual D-pad overlay for mobile browsers. Not in original but trivial with Phaser.

---

## Phase 6: Sound System

### 6.1 Sound Manager

Port of `sons.c`. Phaser handles loading and playback natively.

```typescript
// Spatial panning (port of pan_from_case)
function panFromCell(col: number): number {
  // Original: (x * 256) / NB_CASES_X → range 0-256, center at 128
  // Phaser pan: -1.0 (left) to 1.0 (right)
  return (col / GRID.COLS) * 2 - 1;
}

// Play with spatial positioning
function playSpatialSound(scene: Phaser.Scene, key: string, col: number) {
  scene.sound.play(key, { pan: panFromCell(col) });
}
```

### 6.2 Continuous explosion loop

The original plays `deflagration.wav` in a loop while any explosion is active. In Phaser:
```typescript
// In GameScene update:
if (this.explosions.length > 0 && !this.deflagrationLoop?.isPlaying) {
  this.deflagrationLoop = this.sound.play('deflagration', { loop: true });
} else if (this.explosions.length === 0 && this.deflagrationLoop?.isPlaying) {
  this.deflagrationLoop.stop();
}
```

---

## Phase 7: Persistence

### 7.1 High Scores

| Original | Web Version |
|----------|-------------|
| Binary file `highscore-{1\|2}.hs` | `localStorage` key `highscores_{1\|2}p` |
| 20 entries, sorted descending | Same, JSON array |
| Name input via custom text handler | HTML `<input>` overlay or Phaser DOM element |

### 7.2 Level Editor Saves

| Original | Web Version |
|----------|-------------|
| File I/O to `niveaux/niveauN.niv` | `localStorage` or JSON file download |

### 7.3 Game State (optional)

The original has no save/resume. Optionally add `localStorage`-based state persistence for web (save on page unload, resume on return).

---

## Phase 8: Testing & Polish

### 8.1 Gameplay Verification Checklist

For each of these, compare behavior against the original:

- [ ] Player movement: 4 directions, correct speed, wall collision
- [ ] Fallback speed: full → 4px → 1px → corner-slide
- [ ] Bomb placement and fuse countdown (2 seconds)
- [ ] Explosion propagation: 4 directions, blocked by indestructible walls
- [ ] Wall destruction by explosion
- [ ] Chain reactions (bomb detonated by explosion)
- [ ] Player death on explosion/enemy contact
- [ ] Invincibility frames after respawn
- [ ] Power-up reset on death
- [ ] Diamond collection counter
- [ ] Exit door appears when all diamonds collected
- [ ] Level transition on door reach
- [ ] Enemy AI: move in direction, rotate on collision
- [ ] Enemy death by explosion (50 pts to responsible player)
- [ ] All 4 bonus types work correctly
- [ ] Bomb recovery (1 bomb every 5 seconds)
- [ ] Random bonus spawn every 40 seconds
- [ ] Time limit and game over
- [ ] 2-player mode with correct keybindings
- [ ] All 25 levels load and play correctly
- [ ] High score ranking and persistence
- [ ] Sound effects: correct triggers, spatial panning
- [ ] Background music loops

### 8.2 Browser Testing

- Chrome, Firefox, Safari, Edge (latest)
- Mobile Safari, Chrome Android
- Verify audio autoplay policy handling (Phaser handles this, but verify)

### 8.3 Performance

- Target: solid 60 FPS on any modern device
- The game is extremely lightweight (11×10 grid, <100 sprites) — no concerns here
- Verify sprite batching works (Phaser's WebGL renderer handles this automatically)

---

## Implementation Order

| Step | Description | Depends On | Effort |
|------|-------------|------------|--------|
| **0** | Project scaffolding (Vite + TS + Phaser) | — | 1 hour |
| **1a** | Asset conversion scripts (BMP→PNG, WAV→OGG) | — | 2 hours |
| **1b** | Level conversion (.niv→JSON) | — | 30 min |
| **1c** | Run conversions, verify assets visually | 1a, 1b | 1 hour |
| **2** | Constants, types, Grid system | 0 | 2 hours |
| **3a** | BootScene (asset loading, animation definitions) | 1c, 2 | 2 hours |
| **3b** | CollisionSystem (4-corner checks) | 2 | 3 hours |
| **4a** | Player object (movement, input, animation) | 3a, 3b | 4 hours |
| **4b** | GameScene skeleton (level loading, grid rendering) | 3a, 2 | 3 hours |
| **5** | Bomb + Explosion (placement, fuse, propagation, wall destruction) | 4a, 4b | 4 hours |
| **6** | Enemy object (AI, death) | 4b, 3b | 2 hours |
| **7** | Bonus system (spawn, collection, effects) | 4a, 4b | 1 hour |
| **8** | Diamond counting + exit door + level progression | 7 | 2 hours |
| **9** | Player death, respawn, invincibility, power-up reset | 5, 6 | 2 hours |
| **10** | HUD scene (lives, score, diamonds, time, level) | 4b | 2 hours |
| **11** | Sound system (all effects + music + spatial panning) | 3a | 2 hours |
| **12** | Menu scene (buttons, hover, navigation) | 3a | 2 hours |
| **13** | High score scene (localStorage, ranking, name input) | 12 | 2 hours |
| **14** | Game over scene | 12 | 30 min |
| **15** | 2-player mode (keybindings, separate stats) | 4a | 1 hour |
| **16** | Level editor scene | 4b | 4 hours |
| **17** | Full playthrough testing (all 25 levels) | all | 4 hours |
| **18** | Browser compatibility + mobile | all | 2 hours |
| **Total** | | | **~44 hours** |

---

## Key Design Decisions Summary

| Decision | Choice | Rationale |
|----------|--------|-----------|
| **Framework** | Phaser 3.90 (not 4 RC) | Stable, mature, huge ecosystem, 4.0 not yet released |
| **Language** | TypeScript | Type safety for the grid/collision system, IDE support |
| **Build tool** | Vite | Fast HMR for game dev iteration, zero-config for TS |
| **Frame rate** | 60 FPS (time-based) | Smoother than original 20 FPS, convert frame timers to ms |
| **Collision** | Custom (not Arcade Physics) | Preserves exact original feel (fallback speeds, corner-slide) |
| **Grid** | Typed CellEntry[] | Replaces unsafe pointer/integer trick with clean types |
| **Sprites** | PNG spritesheets | Web-native, transparency support, batch rendering |
| **Audio** | OGG + MP3 dual | Cross-browser (OGG for Chrome/FF, MP3 for Safari) |
| **MIDI** | Pre-rendered to OGG | No browser MIDI SoundFont support |
| **Levels** | JSON (converted from .niv) | Easy to parse, validate, and share |
| **Persistence** | localStorage | No server needed, works offline |
| **Deployment** | Static files (Vite build) | Host anywhere: GitHub Pages, Netlify, S3 |

---

## Future Enhancements (not in original, optional)

- **Online multiplayer** via WebSocket (the grid-based design makes state sync straightforward)
- **Touch controls** for mobile
- **Level sharing** via URL-encoded JSON
- **Gamepad support** (Phaser built-in)
- **Responsive scaling** (already handled by `Phaser.Scale.FIT`)
- **PWA offline mode** (service worker + cache)
- **Replay system** (record inputs per frame, deterministic replay)

That covers the complete conversion plan. The core challenge is faithfully porting the collision system and bomb propagation logic — everything else maps cleanly to Phaser's scene/sprite/sound APIs. The asset pipeline (BMP→PNG, MIDI→OGG, .niv→JSON) should be done first as a one-time batch so all subsequent development works with web-native assets.

Sources:
- [Phaser v3.90.0 Download](https://phaser.io/download/stable)
- [Phaser GitHub Releases](https://github.com/phaserjs/phaser/releases)