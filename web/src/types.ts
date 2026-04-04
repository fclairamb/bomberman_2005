import { ObjectType, Direction } from './config';

export interface LevelData {
  level: number;
  timeLimit: { minutes: number; seconds: number };
  objects: LevelObject[];
}

export interface LevelObject {
  x: number;
  y: number;
  type: number;
}

export interface PlayerConfig {
  playerIndex: number;
  keys: {
    up: string;
    down: string;
    left: string;
    right: string;
    bomb: string;
  };
}

export const PLAYER_KEYS: PlayerConfig[] = [
  {
    playerIndex: 0,
    keys: { up: 'UP', down: 'DOWN', left: 'LEFT', right: 'RIGHT', bomb: 'SPACE' },
  },
  {
    playerIndex: 1,
    keys: { up: 'W', down: 'S', left: 'A', right: 'D', bomb: 'E' },
  },
];

export interface CellEntry {
  type: ObjectType;
  gameObject?: Phaser.GameObjects.Sprite | null;
}

export interface GameStartData {
  numPlayers: number;
  startLevel?: number;
}
