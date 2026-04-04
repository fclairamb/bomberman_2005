import { ObjectType, Direction, EXPLOSION_LIFETIME_MS, EXPLOSION_WALL_BREAK_MS,
  EXPLOSION_SPREAD_MS } from '../config';
import { CellEntry } from '../types';
import type { GameScene } from '../scenes/GameScene';

export class Explosion extends Phaser.GameObjects.Sprite {
  public lifetime: number;
  public propagationRadius: number;
  public direction: Direction;
  public ownerIndex: number;
  public gridCol: number;
  public gridRow: number;

  private gameScene: GameScene;
  private cellEntry: CellEntry;
  private hasSpread = false;
  private hasBrokenWalls = false;
  private elapsed = 0;

  constructor(
    scene: GameScene,
    col: number,
    row: number,
    radius: number,
    direction: Direction,
    ownerIndex: number
  ) {
    const pos = scene.grid.cellToPixel(col, row);
    super(scene, pos.x, pos.y, 'explosion', 0);

    this.gameScene = scene;
    this.gridCol = col;
    this.gridRow = row;
    this.propagationRadius = radius;
    this.direction = direction;
    this.ownerIndex = ownerIndex;
    this.lifetime = EXPLOSION_LIFETIME_MS;

    this.cellEntry = { type: ObjectType.DEFLAGRATION, gameObject: this };
    scene.grid.register(col, row, this.cellEntry);

    this.setDepth(15);
    scene.add.existing(this);

    // Play explosion animation
    this.anims.play('explosion_anim', true);

    // Play sound
    scene.playSound('explode', col);

    // Trigger chain reactions on bombs in this cell
    this.gameScene.chainDetonateBombs(col, row);
  }

  update(_time: number, delta: number): void {
    this.elapsed += delta;

    // At EXPLOSION_WALL_BREAK_MS elapsed: destroy walls
    if (!this.hasBrokenWalls && this.elapsed >= EXPLOSION_WALL_BREAK_MS) {
      this.hasBrokenWalls = true;
      this.gameScene.destroyWallAt(this.gridCol, this.gridRow);
    }

    // At EXPLOSION_SPREAD_MS elapsed: propagate
    if (!this.hasSpread && this.elapsed >= EXPLOSION_SPREAD_MS && this.propagationRadius > 0) {
      this.hasSpread = true;
      this.gameScene.propagateExplosion(this);
    }

    // At lifetime: remove
    if (this.elapsed >= this.lifetime) {
      this.gameScene.grid.unregister(this.gridCol, this.gridRow, this.cellEntry);
      this.gameScene.removeExplosion(this);
      this.destroy();
    }
  }

  cleanup(): void {
    this.gameScene.grid.unregister(this.gridCol, this.gridRow, this.cellEntry);
  }
}
