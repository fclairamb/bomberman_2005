import { ObjectType, BOMB_FUSE_MS } from '../config';
import { CellEntry } from '../types';
import type { GameScene } from '../scenes/GameScene';

export class Bomb extends Phaser.GameObjects.Sprite {
  public fuseTimer: number;
  public power: number;
  public ownerIndex: number;
  public gridCol: number;
  public gridRow: number;

  private gameScene: GameScene;
  private cellEntry: CellEntry;

  constructor(
    scene: GameScene,
    col: number,
    row: number,
    power: number,
    ownerIndex: number
  ) {
    const pos = scene.grid.cellToPixel(col, row);
    super(scene, pos.x, pos.y, 'bombe', 0);

    this.gameScene = scene;
    this.gridCol = col;
    this.gridRow = row;
    this.power = power;
    this.ownerIndex = ownerIndex;
    this.fuseTimer = BOMB_FUSE_MS;

    this.cellEntry = { type: ObjectType.BOMBE, gameObject: this };
    scene.grid.register(col, row, this.cellEntry);

    this.setDepth(5);
    scene.add.existing(this);

    // Play bomb animation
    this.anims.play('bomb_tick', true);
  }

  update(_time: number, delta: number): void {
    this.fuseTimer -= delta;
    if (this.fuseTimer <= 0) {
      this.detonate();
    }
  }

  detonate(): void {
    // Remove from grid
    this.gameScene.grid.unregister(this.gridCol, this.gridRow, this.cellEntry);

    // Return bomb to owner
    const owner = this.gameScene.players.find(p => p.playerIndex === this.ownerIndex);
    if (owner) {
      owner.recoverBomb();
    }

    // Spawn explosions
    this.gameScene.spawnExplosion(this.gridCol, this.gridRow, this.power, this.ownerIndex);

    // Remove from scene
    this.gameScene.removeBomb(this);
    this.destroy();
  }

  /** Called when hit by another explosion (chain reaction) */
  chainDetonate(): void {
    this.fuseTimer = 0; // Will detonate on next update
  }

  cleanup(): void {
    this.gameScene.grid.unregister(this.gridCol, this.gridRow, this.cellEntry);
  }
}
