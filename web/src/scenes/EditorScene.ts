import { GRID, ObjectType } from '../config';
import { Grid } from '../systems/Grid';
import { CellEntry, LevelData } from '../types';

const OBJECT_PALETTE = [
  { type: ObjectType.MUR, label: 'Wall', texture: 'caisse' },
  { type: ObjectType.INCASSABLE, label: 'Solid', texture: 'incassable' },
  { type: ObjectType.DIAMANT, label: 'Diamond', texture: 'diamant' },
  { type: ObjectType.END_DOOR, label: 'Exit', texture: 'end_door' },
  { type: ObjectType.BOMBERMAN, label: 'Player', texture: 'player1' },
  { type: ObjectType.MECHANT, label: 'Enemy', texture: 'mechant' },
  { type: ObjectType.BONUS_VIE, label: 'HP+', texture: 'bonus_sante' },
  { type: ObjectType.BONUS_VITESSE, label: 'Speed', texture: 'bonus_vitesse' },
  { type: ObjectType.BONUS_BOMBE_PUISSANCE, label: 'Power', texture: 'bonus_explosion' },
  { type: ObjectType.BONUS_BOMBE_NOMBRE, label: 'Bombs', texture: 'bonus_bombes' },
];

export class EditorScene extends Phaser.Scene {
  private grid!: Grid;
  private gridData: (number | null)[][] = [];
  private selectedType = 0; // Index into OBJECT_PALETTE
  private currentLevel = 1;
  private tileSprites: (Phaser.GameObjects.Sprite | null)[][] = [];
  private selectorHighlight!: Phaser.GameObjects.Rectangle;

  constructor() {
    super('EditorScene');
  }

  create(): void {
    this.grid = new Grid();
    this.gridData = Array.from({ length: GRID.COLS }, () =>
      Array.from({ length: GRID.ROWS }, () => null)
    );
    this.tileSprites = Array.from({ length: GRID.COLS }, () =>
      Array.from({ length: GRID.ROWS }, () => null)
    );

    // Background
    this.add.rectangle(400, 300, 800, 600, 0x333333);

    // Draw grid
    const graphics = this.add.graphics();
    graphics.lineStyle(1, 0x666666);
    for (let col = 0; col <= GRID.COLS; col++) {
      const x = GRID.OFFSET_X + col * GRID.CELL_SIZE;
      graphics.lineBetween(x, GRID.OFFSET_Y, x, GRID.OFFSET_Y + GRID.ROWS * GRID.CELL_SIZE);
    }
    for (let row = 0; row <= GRID.ROWS; row++) {
      const y = GRID.OFFSET_Y + row * GRID.CELL_SIZE;
      graphics.lineBetween(GRID.OFFSET_X, y, GRID.OFFSET_X + GRID.COLS * GRID.CELL_SIZE, y);
    }

    // Draw floor
    for (let col = 0; col < GRID.COLS; col++) {
      for (let row = 0; row < GRID.ROWS; row++) {
        const pos = this.grid.cellToPixel(col, row);
        this.add.image(pos.x, pos.y, 'sol').setDepth(0);
      }
    }

    // Palette on the right
    this.add.text(640, 20, 'Palette', { fontSize: '16px', color: '#fff', fontFamily: 'monospace' });
    for (let i = 0; i < OBJECT_PALETTE.length; i++) {
      const item = OBJECT_PALETTE[i];
      const y = 50 + i * 45;
      const btn = this.add.sprite(660, y, item.texture, 0)
        .setDisplaySize(36, 36)
        .setInteractive({ useHandCursor: true })
        .on('pointerdown', () => { this.selectedType = i; this.updateSelector(); });
      this.add.text(690, y - 8, item.label, { fontSize: '12px', color: '#ccc', fontFamily: 'monospace' });
    }

    this.selectorHighlight = this.add.rectangle(660, 50, 42, 42)
      .setStrokeStyle(2, 0xffff00)
      .setFillStyle(0, 0);

    // Mouse interaction on grid
    this.input.on('pointerdown', (pointer: Phaser.Input.Pointer) => {
      const col = Math.floor((pointer.x - GRID.OFFSET_X) / GRID.CELL_SIZE);
      const row = Math.floor((pointer.y - GRID.OFFSET_Y) / GRID.CELL_SIZE);
      if (col < 0 || col >= GRID.COLS || row < 0 || row >= GRID.ROWS) return;

      if (pointer.rightButtonDown()) {
        this.removeAt(col, row);
      } else {
        this.placeAt(col, row);
      }
    });

    // Keyboard shortcuts
    this.input.keyboard?.on('keydown-F1', () => this.loadFromStorage());
    this.input.keyboard?.on('keydown-F2', () => this.saveToStorage());
    this.input.keyboard?.on('keydown-N', () => this.clearGrid());
    this.input.keyboard?.on('keydown-ESC', () => this.scene.start('MenuScene'));

    // Instructions
    this.add.text(10, 575, 'Click: place | Right-click: remove | F1: load | F2: save | N: clear | ESC: menu', {
      fontSize: '11px', color: '#888', fontFamily: 'monospace',
    });

    // Try to load existing level
    this.loadFromStorage();
  }

  private updateSelector(): void {
    const y = 50 + this.selectedType * 45;
    this.selectorHighlight.setPosition(660, y);
  }

  private placeAt(col: number, row: number): void {
    this.removeAt(col, row);
    const item = OBJECT_PALETTE[this.selectedType];
    this.gridData[col][row] = item.type;

    const pos = this.grid.cellToPixel(col, row);
    const sprite = this.add.sprite(pos.x, pos.y, item.texture, 0).setDepth(2);
    this.tileSprites[col][row] = sprite;
  }

  private removeAt(col: number, row: number): void {
    this.gridData[col][row] = null;
    const existing = this.tileSprites[col][row];
    if (existing) {
      existing.destroy();
      this.tileSprites[col][row] = null;
    }
  }

  private clearGrid(): void {
    for (let col = 0; col < GRID.COLS; col++) {
      for (let row = 0; row < GRID.ROWS; row++) {
        this.removeAt(col, row);
      }
    }
  }

  private saveToStorage(): void {
    const objects: { x: number; y: number; type: number }[] = [];
    for (let col = 0; col < GRID.COLS; col++) {
      for (let row = 0; row < GRID.ROWS; row++) {
        if (this.gridData[col][row] !== null) {
          objects.push({ x: col, y: row, type: this.gridData[col][row]! });
        }
      }
    }

    const levelData: LevelData = {
      level: this.currentLevel,
      timeLimit: { minutes: 3, seconds: 0 },
      objects,
    };

    localStorage.setItem(`bomberman_editor_level${this.currentLevel}`, JSON.stringify(levelData));

    // Also offer download
    const blob = new Blob([JSON.stringify(levelData, null, 2)], { type: 'application/json' });
    const url = URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = `niveau${this.currentLevel}.json`;
    a.click();
    URL.revokeObjectURL(url);
  }

  private loadFromStorage(): void {
    const saved = localStorage.getItem(`bomberman_editor_level${this.currentLevel}`);
    if (!saved) return;

    this.clearGrid();
    const data: LevelData = JSON.parse(saved);
    for (const obj of data.objects) {
      const paletteIdx = OBJECT_PALETTE.findIndex(p => p.type === obj.type);
      if (paletteIdx >= 0) {
        this.gridData[obj.x][obj.y] = obj.type;
        const pos = this.grid.cellToPixel(obj.x, obj.y);
        const sprite = this.add.sprite(pos.x, pos.y, OBJECT_PALETTE[paletteIdx].texture, 0).setDepth(2);
        this.tileSprites[obj.x][obj.y] = sprite;
      }
    }
  }
}
