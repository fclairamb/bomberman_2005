import { Direction, ObjectType, GRID, ENEMY_SPEED, FRAME_TIME, SCORING } from '../config';
import { CellEntry } from '../types';
import type { GameScene } from '../scenes/GameScene';

export class Enemy extends Phaser.GameObjects.Sprite {
  public direction: Direction = Direction.UP;
  public gridCol: number;
  public gridRow: number;
  public isAlive = true;

  private gameScene: GameScene;
  private cellEntry: CellEntry;
  private moveSpeed = ENEMY_SPEED;

  constructor(scene: GameScene, col: number, row: number) {
    const pos = scene.grid.cellToPixel(col, row);
    super(scene, pos.x, pos.y, 'mechant', 0);

    this.gameScene = scene;
    this.gridCol = col;
    this.gridRow = row;

    this.cellEntry = { type: ObjectType.MECHANT, gameObject: this };
    scene.grid.register(col, row, this.cellEntry);

    this.setDepth(9);
    scene.add.existing(this);

    // Start walking animation
    this.anims.play('mechant_up', true);
  }

  update(_time: number, delta: number): void {
    if (!this.isAlive) return;

    // Check if standing in explosion
    const entries = this.gameScene.grid.getEntries(this.gridCol, this.gridRow);
    for (const entry of entries) {
      if (entry.type === ObjectType.DEFLAGRATION) {
        this.die();
        return;
      }
    }

    const pixelSpeed = this.moveSpeed * (delta / FRAME_TIME);
    const movement = this.gameScene.collisionSystem.tryMove(
      this.x, this.y, this.direction, pixelSpeed
    );

    if (movement.dx === 0 && movement.dy === 0) {
      // Rotate direction: UP→RIGHT→DOWN→LEFT→UP (matching original: direction%4 + 1)
      this.direction = (this.direction % 4) + 1;
    } else {
      this.x += movement.dx;
      this.y += movement.dy;
    }

    // Update animation based on direction
    const dirName = this.directionName();
    const animKey = `mechant_${dirName}`;
    if (!this.anims.isPlaying || this.anims.currentAnim?.key !== animKey) {
      this.anims.play(animKey, true);
    }

    this.updateGridPosition();
  }

  private directionName(): string {
    switch (this.direction) {
      case Direction.UP: return 'up';
      case Direction.DOWN: return 'down';
      case Direction.LEFT: return 'left';
      case Direction.RIGHT: return 'right';
      default: return 'up';
    }
  }

  private updateGridPosition(): void {
    const cell = this.gameScene.grid.pixelToCell(this.x, this.y);
    if (cell.col !== this.gridCol || cell.row !== this.gridRow) {
      this.gameScene.grid.unregister(this.gridCol, this.gridRow, this.cellEntry);
      this.gridCol = cell.col;
      this.gridRow = cell.row;
      this.gameScene.grid.register(this.gridCol, this.gridRow, this.cellEntry);
    }
  }

  die(): void {
    if (!this.isAlive) return;
    this.isAlive = false;
    this.gameScene.grid.unregister(this.gridCol, this.gridRow, this.cellEntry);
    this.gameScene.playSound('mort_mechant', this.gridCol);

    // Award score to nearest player (simplified from original)
    this.gameScene.awardEnemyKill(this.gridCol, this.gridRow);

    this.destroy();
  }

  cleanup(): void {
    this.gameScene.grid.unregister(this.gridCol, this.gridRow, this.cellEntry);
  }
}
