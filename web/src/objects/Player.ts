import { Direction, ObjectType, GRID, OBJ_HALF_SIZE, FRAME_TIME,
  PLAYER_SPEED, BOMB_FUSE_MS, DEFAULT_MAX_BOMBS, DEFAULT_BOMB_POWER,
  DEFAULT_LIVES_1P, DEFAULT_LIVES_2P, DEATH_ANIM_MS, DEATH_INVINCIBILITY_MS,
  SCORING } from '../config';
import { PlayerConfig, CellEntry } from '../types';
import type { GameScene } from '../scenes/GameScene';

export class Player extends Phaser.GameObjects.Sprite {
  public playerIndex: number;
  public lives: number;
  public bombsDeployed = 0;
  public maxBombs: number;
  public bombPower: number;
  public score = 0;
  public moveSpeed: number;
  public isDead = false;
  public invincibilityTimer = 0;
  public direction: Direction = Direction.DOWN;
  public isMoving = false;
  public gridCol = 0;
  public gridRow = 0;

  private gameScene: GameScene;
  private keys: Record<string, Phaser.Input.Keyboard.Key>;
  private deathTimer = 0;
  private cellEntry: CellEntry;
  private spawnCol: number;
  private spawnRow: number;
  private bombRecoveryTimer = 0;

  constructor(
    scene: GameScene,
    col: number,
    row: number,
    playerIndex: number,
    numPlayers: number,
    keyConfig: PlayerConfig
  ) {
    const pos = scene.grid.cellToPixel(col, row);
    const textureKey = `player${playerIndex + 1}`;
    super(scene, pos.x, pos.y, textureKey, 0);

    this.gameScene = scene;
    this.playerIndex = playerIndex;
    this.gridCol = col;
    this.gridRow = row;
    this.spawnCol = col;
    this.spawnRow = row;
    this.lives = numPlayers === 1 ? DEFAULT_LIVES_1P : DEFAULT_LIVES_2P;
    this.maxBombs = DEFAULT_MAX_BOMBS;
    this.bombPower = DEFAULT_BOMB_POWER;
    this.moveSpeed = PLAYER_SPEED;

    this.cellEntry = { type: ObjectType.BOMBERMAN, gameObject: this };
    scene.grid.register(col, row, this.cellEntry);

    // Set up input keys
    const kb = scene.input.keyboard!;
    this.keys = {
      up: kb.addKey(keyConfig.keys.up),
      down: kb.addKey(keyConfig.keys.down),
      left: kb.addKey(keyConfig.keys.left),
      right: kb.addKey(keyConfig.keys.right),
      bomb: kb.addKey(keyConfig.keys.bomb),
    };

    this.setDepth(10);
    scene.add.existing(this);
  }

  update(_time: number, delta: number): void {
    if (this.isDead) {
      this.handleDeath(delta);
      return;
    }

    if (this.invincibilityTimer > 0) {
      this.invincibilityTimer -= delta;
      // Flash effect
      this.setAlpha(Math.sin(this.invincibilityTimer * 0.01) > 0 ? 1 : 0.3);
      if (this.invincibilityTimer <= 0) {
        this.setAlpha(1);
      }
    }

    this.handleInput(delta);
    this.updateGridPosition();
    this.checkPickups();
    this.checkDangers();
  }

  private handleInput(delta: number): void {
    const prevDirection = this.direction;
    let newDirection = Direction.NONE;

    if (this.keys.up.isDown) newDirection = Direction.UP;
    else if (this.keys.down.isDown) newDirection = Direction.DOWN;
    else if (this.keys.left.isDown) newDirection = Direction.LEFT;
    else if (this.keys.right.isDown) newDirection = Direction.RIGHT;

    if (newDirection !== Direction.NONE) {
      this.direction = newDirection;
      this.isMoving = true;

      // Calculate pixel movement for this frame
      const pixelSpeed = this.moveSpeed * (delta / FRAME_TIME);
      const movement = this.gameScene.collisionSystem.tryMove(
        this.x, this.y, this.direction, pixelSpeed
      );

      if (movement.dx !== 0 || movement.dy !== 0) {
        this.x += movement.dx;
        this.y += movement.dy;
      }

      // Play walk animation
      const dirName = this.directionName();
      const animKey = `player${this.playerIndex + 1}_${dirName}`;
      if (!this.anims.isPlaying || this.anims.currentAnim?.key !== animKey) {
        this.anims.play(animKey, true);
      }
    } else {
      this.isMoving = false;
      if (this.anims.isPlaying) {
        this.anims.stop();
      }
    }

    // Bomb placement
    if (Phaser.Input.Keyboard.JustDown(this.keys.bomb)) {
      this.placeBomb();
    }
  }

  private directionName(): string {
    switch (this.direction) {
      case Direction.UP: return 'up';
      case Direction.DOWN: return 'down';
      case Direction.LEFT: return 'left';
      case Direction.RIGHT: return 'right';
      default: return 'down';
    }
  }

  private updateGridPosition(): void {
    const cell = this.gameScene.grid.pixelToCell(this.x, this.y);
    if (cell.col !== this.gridCol || cell.row !== this.gridRow) {
      // Unregister from old cell, register in new
      this.gameScene.grid.unregister(this.gridCol, this.gridRow, this.cellEntry);
      this.gridCol = cell.col;
      this.gridRow = cell.row;
      this.gameScene.grid.register(this.gridCol, this.gridRow, this.cellEntry);
    }
  }

  private checkPickups(): void {
    const entries = this.gameScene.grid.getEntries(this.gridCol, this.gridRow);
    for (const entry of entries) {
      switch (entry.type) {
        case ObjectType.BONUS_VIE:
          this.lives++;
          this.removePickup(entry);
          this.gameScene.playSound('bonus', this.gridCol);
          break;
        case ObjectType.BONUS_VITESSE:
          this.moveSpeed = Math.min(this.moveSpeed + 1, 12);
          this.removePickup(entry);
          this.gameScene.playSound('bonus', this.gridCol);
          break;
        case ObjectType.BONUS_BOMBE_PUISSANCE:
          this.bombPower++;
          this.removePickup(entry);
          this.gameScene.playSound('bonus', this.gridCol);
          break;
        case ObjectType.BONUS_BOMBE_NOMBRE:
          this.maxBombs++;
          this.removePickup(entry);
          this.gameScene.playSound('bonus', this.gridCol);
          break;
        case ObjectType.DIAMANT:
          this.score += SCORING.DIAMOND;
          this.removePickup(entry);
          this.gameScene.collectDiamond();
          this.gameScene.playSound('diamant', this.gridCol);
          break;
        case ObjectType.END_DOOR:
          if (this.gameScene.diamondsRemaining <= 0) {
            this.score += SCORING.DOOR;
            this.gameScene.nextLevel();
          }
          break;
      }
    }
  }

  private removePickup(entry: CellEntry): void {
    this.gameScene.grid.unregister(this.gridCol, this.gridRow, entry);
    if (entry.gameObject) {
      entry.gameObject.destroy();
    }
  }

  private checkDangers(): void {
    if (this.invincibilityTimer > 0) return;

    const entries = this.gameScene.grid.getEntries(this.gridCol, this.gridRow);
    for (const entry of entries) {
      if (entry.type === ObjectType.DEFLAGRATION) {
        this.die();
        return;
      }
      if (entry.type === ObjectType.MECHANT) {
        this.die();
        return;
      }
    }
  }

  placeBomb(): void {
    if (this.bombsDeployed >= this.maxBombs) return;
    // Don't place if there's already a bomb in this cell
    if (this.gameScene.grid.hasType(this.gridCol, this.gridRow, ObjectType.BOMBE)) return;

    this.bombsDeployed++;
    this.gameScene.spawnBomb(this.gridCol, this.gridRow, this.bombPower, this.playerIndex);

  }

  recoverBomb(): void {
    if (this.bombsDeployed > 0) {
      this.bombsDeployed--;
    }
  }

  die(): void {
    if (this.isDead) return;
    this.isDead = true;
    this.lives--;
    this.deathTimer = DEATH_ANIM_MS;

    // Reset bonuses on death
    this.moveSpeed = PLAYER_SPEED;
    this.maxBombs = DEFAULT_MAX_BOMBS;
    this.bombPower = DEFAULT_BOMB_POWER;
    this.bombsDeployed = 0;

    // Update grid entry
    this.cellEntry.type = ObjectType.BOMBERMAN_MORT;

    // Play death animation
    this.anims.play('death', true);
    this.gameScene.playSound('meurt', this.gridCol);
  }

  private handleDeath(delta: number): void {
    this.deathTimer -= delta;
    if (this.deathTimer <= 0) {
      if (this.lives > 0) {
        this.respawn();
      } else {
        // Permanently dead
        this.gameScene.grid.unregister(this.gridCol, this.gridRow, this.cellEntry);
        this.setVisible(false);
        this.setActive(false);
      }
    }
  }

  private respawn(): void {
    // Check if spawn location is safe
    const entries = this.gameScene.grid.getEntries(this.spawnCol, this.spawnRow);
    const hasExplosion = entries.some(e => e.type === ObjectType.DEFLAGRATION);
    const hasEnemy = entries.some(e => e.type === ObjectType.MECHANT);
    if (hasExplosion || hasEnemy) {
      // Wait a bit more
      this.deathTimer = 500;
      return;
    }

    this.isDead = false;
    this.cellEntry.type = ObjectType.BOMBERMAN;

    // Move to spawn
    this.gameScene.grid.unregister(this.gridCol, this.gridRow, this.cellEntry);
    this.gridCol = this.spawnCol;
    this.gridRow = this.spawnRow;
    const pos = this.gameScene.grid.cellToPixel(this.gridCol, this.gridRow);
    this.x = pos.x;
    this.y = pos.y;
    this.gameScene.grid.register(this.gridCol, this.gridRow, this.cellEntry);

    this.invincibilityTimer = DEATH_INVINCIBILITY_MS;
    this.direction = Direction.DOWN;
    this.anims.stop();
    this.setFrame(10); // down-facing frame 0
    this.setVisible(true);
    this.setActive(true);
  }

  cleanup(): void {
    this.gameScene.grid.unregister(this.gridCol, this.gridRow, this.cellEntry);
  }
}
