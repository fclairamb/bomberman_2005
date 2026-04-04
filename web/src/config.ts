export const GRID = {
  COLS: 11,          // NB_CASES_X
  ROWS: 10,          // NB_CASES_Y
  CELL_SIZE: 54,     // LAR_CASE_L
  OFFSET_X: 30,      // POS_CASES_X
  OFFSET_Y: 30,      // POS_CASES_Y
  MAX_OBJECTS: 10,   // NB_OBJ_CASE
} as const;

export const OBJ_HALF_SIZE = 25;  // OBJ_LAR — collision half-width

export const FRAME_TIME = 50;     // ms per frame (20 FPS original)

export enum ObjectType {
  // Simple types (static tiles, no allocated state)
  MUR = 1,                    // Destructible wall (caisse)
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

// Timings converted from frames (at 20 FPS) to milliseconds
export const BOMB_FUSE_MS = 2000;              // 40 frames * 50ms
export const EXPLOSION_LIFETIME_MS = 2000;     // 40 frames * 50ms
export const EXPLOSION_WALL_BREAK_MS = 450;    // at frame 31 from end = 9 frames * 50ms
export const EXPLOSION_SPREAD_MS = 500;        // at frame 30 from end = 10 frames * 50ms
export const DEATH_ANIM_MS = 2500;             // 50 frames * 50ms
export const DEATH_INVINCIBILITY_MS = 2500;    // 50 frames * 50ms
export const BOMB_RECOVERY_MS = 5000;          // 100 frames * 50ms
export const BONUS_SPAWN_MS = 40000;           // 800 frames * 50ms

// Original frame-based values (for reference)
export const BOMB_ANIM_PAUSE = 14;             // frames between bomb animation frames
export const PLAYER_ANIM_PAUSE = 2;            // frames between player animation frames
export const DEATH_ANIM_PAUSE = 10;            // frames between death animation frames

// Movement speeds (pixels per original frame, will be scaled by delta)
export const PLAYER_SPEED = 8;                 // pixels per frame at 20 FPS = 160 px/s
export const ENEMY_SPEED = 2;                  // pixels per frame at 20 FPS = 40 px/s

// Converted to pixels per second for time-based movement
export const PLAYER_SPEED_PPS = PLAYER_SPEED * (1000 / FRAME_TIME);  // 160 px/s
export const ENEMY_SPEED_PPS = ENEMY_SPEED * (1000 / FRAME_TIME);    // 40 px/s

// Player defaults
export const DEFAULT_LIVES_1P = 3;
export const DEFAULT_LIVES_2P = 10;
export const DEFAULT_MAX_BOMBS = 1;
export const DEFAULT_BOMB_POWER = 1;
