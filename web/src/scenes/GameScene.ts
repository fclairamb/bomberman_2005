import { GRID, ObjectType, Direction, BONUS_SPAWN_MS, SCORING, MAX_LEVELS } from '../config';
import { GameStartData, CellEntry, PLAYER_KEYS } from '../types';
import { Grid } from '../systems/Grid';
import { CollisionSystem } from '../systems/CollisionSystem';
import { LevelLoader } from '../systems/LevelLoader';
import { Player } from '../objects/Player';
import { Enemy } from '../objects/Enemy';
import { Bomb } from '../objects/Bomb';
import { Explosion } from '../objects/Explosion';

export class GameScene extends Phaser.Scene {
  public grid!: Grid;
  public collisionSystem!: CollisionSystem;
  public players: Player[] = [];
  public enemies: Enemy[] = [];
  public bombs: Bomb[] = [];
  public explosions: Explosion[] = [];

  public currentLevel = 1;
  public diamondsRemaining = 0;
  public numPlayers = 1;

  private timeLimit = 0;       // in seconds
  private elapsedTime = 0;     // in ms
  private bonusSpawnTimer = 0;
  private deflagrationLoop?: Phaser.Sound.BaseSound;
  private isPaused = false;

  // Static tile sprites (for cleanup)
  private tileSprites: Phaser.GameObjects.Sprite[] = [];
  private floorSprites: Phaser.GameObjects.Image[] = [];

  constructor() {
    super('GameScene');
  }

  create(data: GameStartData): void {
    this.grid = new Grid();
    this.collisionSystem = new CollisionSystem(this.grid);
    this.numPlayers = data.numPlayers ?? 1;
    this.currentLevel = data.startLevel ?? 1;

    // Reset state
    this.players = [];
    this.enemies = [];
    this.bombs = [];
    this.explosions = [];
    this.tileSprites = [];
    this.floorSprites = [];
    this.elapsedTime = 0;
    this.bonusSpawnTimer = 0;

    // Launch HUD as parallel scene
    this.scene.launch('HUDScene', { gameScene: this });

    // Draw the game background
    this.add.image(400, 300, 'game_hud').setDepth(0);

    // Pause handling
    this.input.keyboard?.on('keydown-ESC', () => {
      if (this.isPaused) {
        this.resumeGame();
      } else {
        this.pauseGame();
      }
    });

    this.loadLevel(this.currentLevel);
  }

  update(time: number, delta: number): void {
    if (this.isPaused) return;

    // Update all game objects
    for (const player of this.players) {
      if (player.active) player.update(time, delta);
    }
    for (const enemy of [...this.enemies]) {
      if (enemy.active) enemy.update(time, delta);
    }
    for (const bomb of [...this.bombs]) {
      if (bomb.active) bomb.update(time, delta);
    }
    for (const explosion of [...this.explosions]) {
      if (explosion.active) explosion.update(time, delta);
    }

    // Bonus spawning
    this.bonusSpawnTimer += delta;
    if (this.bonusSpawnTimer >= BONUS_SPAWN_MS) {
      this.bonusSpawnTimer = 0;
      this.spawnRandomBonus();
    }

    // Deflagration sound loop
    this.updateDeflagrationSound();

    // Time limit check
    if (this.timeLimit > 0) {
      this.elapsedTime += delta;
      if (this.elapsedTime >= this.timeLimit * 1000) {
        this.gameOver();
        return;
      }
    }

    // All players dead check
    if (this.players.every(p => p.lives <= 0)) {
      this.gameOver();
    }
  }

  loadLevel(levelNum: number): void {
    // Clear existing objects
    this.clearLevel();

    const levelData = LevelLoader.load(this, levelNum);
    this.timeLimit = levelData.timeLimit.minutes * 60 + levelData.timeLimit.seconds;
    this.elapsedTime = 0;
    this.diamondsRemaining = 0;

    // Draw floor tiles everywhere first
    for (let col = 0; col < GRID.COLS; col++) {
      for (let row = 0; row < GRID.ROWS; row++) {
        const pos = this.grid.cellToPixel(col, row);
        const floor = this.add.image(pos.x, pos.y, 'sol').setDepth(1);
        this.floorSprites.push(floor);
      }
    }

    // Place objects from level data
    for (const obj of levelData.objects) {
      this.placeObject(obj.x, obj.y, obj.type);
    }
  }

  private placeObject(col: number, row: number, type: number): void {
    const pos = this.grid.cellToPixel(col, row);

    switch (type) {
      case ObjectType.MUR: {
        const sprite = this.add.sprite(pos.x, pos.y, 'caisse').setDepth(3);
        const entry: CellEntry = { type: ObjectType.MUR, gameObject: sprite };
        this.grid.register(col, row, entry);
        this.tileSprites.push(sprite);
        break;
      }
      case ObjectType.INCASSABLE: {
        const sprite = this.add.sprite(pos.x, pos.y, 'incassable').setDepth(3);
        const entry: CellEntry = { type: ObjectType.INCASSABLE, gameObject: sprite };
        this.grid.register(col, row, entry);
        this.tileSprites.push(sprite);
        break;
      }
      case ObjectType.DIAMANT: {
        const sprite = this.add.sprite(pos.x, pos.y, 'diamant').setDepth(2);
        const entry: CellEntry = { type: ObjectType.DIAMANT, gameObject: sprite };
        this.grid.register(col, row, entry);
        this.tileSprites.push(sprite);
        this.diamondsRemaining++;
        break;
      }
      case ObjectType.END_DOOR: {
        const sprite = this.add.sprite(pos.x, pos.y, 'end_door').setDepth(2);
        sprite.setVisible(false); // Hidden until all diamonds collected
        const entry: CellEntry = { type: ObjectType.END_DOOR, gameObject: sprite };
        this.grid.register(col, row, entry);
        this.tileSprites.push(sprite);
        break;
      }
      case ObjectType.BONUS_VIE:
      case ObjectType.BONUS_VITESSE:
      case ObjectType.BONUS_BOMBE_PUISSANCE:
      case ObjectType.BONUS_BOMBE_NOMBRE: {
        const textures: Record<number, string> = {
          [ObjectType.BONUS_VIE]: 'bonus_sante',
          [ObjectType.BONUS_VITESSE]: 'bonus_vitesse',
          [ObjectType.BONUS_BOMBE_PUISSANCE]: 'bonus_explosion',
          [ObjectType.BONUS_BOMBE_NOMBRE]: 'bonus_bombes',
        };
        const sprite = this.add.sprite(pos.x, pos.y, textures[type]).setDepth(2);
        const entry: CellEntry = { type: type as ObjectType, gameObject: sprite };
        this.grid.register(col, row, entry);
        this.tileSprites.push(sprite);
        break;
      }
      case ObjectType.BOMBERMAN: {
        // Find which player index to assign
        const playerIdx = this.players.length;
        if (playerIdx < this.numPlayers) {
          const keyConfig = PLAYER_KEYS[playerIdx];
          const player = new Player(this, col, row, playerIdx, this.numPlayers, keyConfig);
          this.players.push(player);
        }
        break;
      }
      case ObjectType.MECHANT: {
        const enemy = new Enemy(this, col, row);
        this.enemies.push(enemy);
        break;
      }
    }
  }

  private clearLevel(): void {
    // Cleanup game objects
    for (const p of this.players) p.cleanup();
    for (const e of this.enemies) e.cleanup();
    for (const b of this.bombs) b.cleanup();
    for (const ex of this.explosions) ex.cleanup();

    this.players = [];
    this.enemies = [];
    this.bombs = [];
    this.explosions = [];

    // Destroy tile sprites
    for (const s of this.tileSprites) s.destroy();
    for (const s of this.floorSprites) s.destroy();
    this.tileSprites = [];
    this.floorSprites = [];

    this.grid.clear();
  }

  // === Public methods called by game objects ===

  spawnBomb(col: number, row: number, power: number, ownerIndex: number): void {
    const bomb = new Bomb(this, col, row, power, ownerIndex);
    this.bombs.push(bomb);
    this.playSound('plant', col);
  }

  removeBomb(bomb: Bomb): void {
    const idx = this.bombs.indexOf(bomb);
    if (idx !== -1) this.bombs.splice(idx, 1);
  }

  spawnExplosion(col: number, row: number, power: number, ownerIndex: number): void {
    // Center explosion
    if (this.grid.hasType(col, row, ObjectType.INCASSABLE)) return;

    const explosion = new Explosion(this, col, row, power, Direction.NONE, ownerIndex);
    this.explosions.push(explosion);
  }

  propagateExplosion(source: Explosion): void {
    const directions = source.direction === Direction.NONE
      ? [Direction.UP, Direction.DOWN, Direction.LEFT, Direction.RIGHT]
      : [source.direction]; // Already directional, continue same direction

    for (const dir of directions) {
      let col = source.gridCol;
      let row = source.gridRow;

      // Move one cell in direction
      switch (dir) {
        case Direction.UP: row--; break;
        case Direction.DOWN: row++; break;
        case Direction.LEFT: col--; break;
        case Direction.RIGHT: col++; break;
      }

      if (!this.grid.isValid(col, row)) continue;
      if (this.grid.hasType(col, row, ObjectType.INCASSABLE)) continue;

      // Spawn directional explosion with reduced radius
      const explosion = new Explosion(
        this, col, row,
        source.propagationRadius - 1,
        dir,
        source.ownerIndex
      );
      this.explosions.push(explosion);
    }
  }

  chainDetonateBombs(col: number, row: number): void {
    for (const bomb of this.bombs) {
      if (bomb.gridCol === col && bomb.gridRow === row) {
        bomb.chainDetonate();
      }
    }
  }

  destroyWallAt(col: number, row: number): void {
    const entry = this.grid.removeType(col, row, ObjectType.MUR);
    if (entry?.gameObject) {
      entry.gameObject.destroy();
      const idx = this.tileSprites.indexOf(entry.gameObject as Phaser.GameObjects.Sprite);
      if (idx !== -1) this.tileSprites.splice(idx, 1);
    }
  }

  removeExplosion(explosion: Explosion): void {
    const idx = this.explosions.indexOf(explosion);
    if (idx !== -1) this.explosions.splice(idx, 1);
  }

  collectDiamond(): void {
    this.diamondsRemaining--;
    if (this.diamondsRemaining <= 0) {
      // Show exit doors
      for (let col = 0; col < GRID.COLS; col++) {
        for (let row = 0; row < GRID.ROWS; row++) {
          const entries = this.grid.getEntries(col, row);
          for (const entry of entries) {
            if (entry.type === ObjectType.END_DOOR && entry.gameObject) {
              entry.gameObject.setVisible(true);
            }
          }
        }
      }
    }
  }

  awardEnemyKill(col: number, row: number): void {
    // Award to nearest living player
    let nearestPlayer: Player | null = null;
    let nearestDist = Infinity;

    for (const player of this.players) {
      if (player.isDead || player.lives <= 0) continue;
      const dx = player.gridCol - col;
      const dy = player.gridRow - row;
      const dist = dx * dx + dy * dy;
      if (dist < nearestDist) {
        nearestDist = dist;
        nearestPlayer = player;
      }
    }

    if (nearestPlayer) {
      nearestPlayer.score += SCORING.ENEMY_KILL;
    }
  }

  nextLevel(): void {
    if (this.currentLevel >= MAX_LEVELS) {
      // Game complete!
      this.playSound('victory', 5);
      this.scene.stop('HUDScene');
      this.scene.start('HighScoreScene', {
        score: this.players[0]?.score ?? 0,
        fromGame: true,
      });
      return;
    }

    this.currentLevel++;
    this.playSound('gagne', 5);
    this.loadLevel(this.currentLevel);
  }

  gameOver(): void {
    this.playSound('perdu', 5);
    this.scene.stop('HUDScene');
    this.scene.start('GameOverScene', {
      score: this.players[0]?.score ?? 0,
      level: this.currentLevel,
    });
  }

  playSound(key: string, col: number): void {
    // Spatial panning: -1 (left) to 1 (right)
    const pan = (col / GRID.COLS) * 2 - 1;
    this.sound.play(key, { pan });
  }

  private spawnRandomBonus(): void {
    // Find empty cells
    const emptyCells: { col: number; row: number }[] = [];
    for (let col = 0; col < GRID.COLS; col++) {
      for (let row = 0; row < GRID.ROWS; row++) {
        if (this.grid.getEntries(col, row).length === 0) {
          emptyCells.push({ col, row });
        }
      }
    }

    if (emptyCells.length === 0) return;

    const cell = emptyCells[Math.floor(Math.random() * emptyCells.length)];
    const bonusTypes = [
      ObjectType.BONUS_VIE,
      ObjectType.BONUS_VITESSE,
      ObjectType.BONUS_BOMBE_PUISSANCE,
      ObjectType.BONUS_BOMBE_NOMBRE,
    ];
    const type = bonusTypes[Math.floor(Math.random() * bonusTypes.length)];

    this.placeObject(cell.col, cell.row, type);
    this.playSound('apparition_bonus', cell.col);
  }

  private updateDeflagrationSound(): void {
    if (this.explosions.length > 0) {
      if (!this.deflagrationLoop || !(this.deflagrationLoop as any).isPlaying) {
        this.deflagrationLoop = this.sound.add('deflagration', { loop: true, volume: 0.5 });
        this.deflagrationLoop.play();
      }
    } else {
      if (this.deflagrationLoop && (this.deflagrationLoop as any).isPlaying) {
        this.deflagrationLoop.stop();
        this.deflagrationLoop.destroy();
        this.deflagrationLoop = undefined;
      }
    }
  }

  private pauseGame(): void {
    this.isPaused = true;
    this.scene.pause();
    // Could show a pause overlay
  }

  private resumeGame(): void {
    this.isPaused = false;
    this.scene.resume();
  }

  shutdown(): void {
    this.clearLevel();
    this.deflagrationLoop?.destroy();
  }
}
