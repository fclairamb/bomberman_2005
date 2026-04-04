import { GRID, ObjectType } from '../config';
import { CellEntry } from '../types';

export class Grid {
  private cells: CellEntry[][][]; // [col][row] → array of entries

  constructor() {
    this.cells = Array.from({ length: GRID.COLS }, () =>
      Array.from({ length: GRID.ROWS }, () => [])
    );
  }

  register(col: number, row: number, entry: CellEntry): boolean {
    if (!this.isValid(col, row)) return false;
    const cell = this.cells[col][row];
    if (cell.length >= GRID.MAX_OBJECTS) return false;
    cell.push(entry);
    return true;
  }

  unregister(col: number, row: number, entry: CellEntry): void {
    if (!this.isValid(col, row)) return;
    const cell = this.cells[col][row];
    const idx = cell.indexOf(entry);
    if (idx !== -1) cell.splice(idx, 1);
  }

  unregisterByObject(col: number, row: number, gameObject: Phaser.GameObjects.Sprite): void {
    if (!this.isValid(col, row)) return;
    const cell = this.cells[col][row];
    const idx = cell.findIndex(e => e.gameObject === gameObject);
    if (idx !== -1) cell.splice(idx, 1);
  }

  getEntries(col: number, row: number): CellEntry[] {
    if (!this.isValid(col, row)) return [];
    return this.cells[col][row];
  }

  hasType(col: number, row: number, type: ObjectType): boolean {
    return this.getEntries(col, row).some(e => e.type === type);
  }

  hasObstacle(col: number, row: number): boolean {
    if (!this.isValid(col, row)) return true; // Out of bounds = obstacle
    return this.getEntries(col, row).some(e =>
      e.type === ObjectType.MUR ||
      e.type === ObjectType.INCASSABLE ||
      e.type === ObjectType.BOMBE ||
      e.type === ObjectType.BIENTOT
    );
  }

  /** Check if a cell blocks movement (including other players for collision) */
  isBlocked(col: number, row: number): boolean {
    if (!this.isValid(col, row)) return true;
    return this.getEntries(col, row).some(e =>
      e.type === ObjectType.MUR ||
      e.type === ObjectType.INCASSABLE ||
      e.type === ObjectType.BOMBE ||
      e.type === ObjectType.BIENTOT
    );
  }

  removeType(col: number, row: number, type: ObjectType): CellEntry | null {
    if (!this.isValid(col, row)) return null;
    const cell = this.cells[col][row];
    const idx = cell.findIndex(e => e.type === type);
    if (idx !== -1) {
      const [removed] = cell.splice(idx, 1);
      return removed;
    }
    return null;
  }

  removeAllOfType(col: number, row: number, type: ObjectType): CellEntry[] {
    if (!this.isValid(col, row)) return [];
    const cell = this.cells[col][row];
    const removed: CellEntry[] = [];
    for (let i = cell.length - 1; i >= 0; i--) {
      if (cell[i].type === type) {
        removed.push(cell.splice(i, 1)[0]);
      }
    }
    return removed;
  }

  clear(): void {
    for (let col = 0; col < GRID.COLS; col++) {
      for (let row = 0; row < GRID.ROWS; row++) {
        this.cells[col][row] = [];
      }
    }
  }

  clearCell(col: number, row: number): void {
    if (this.isValid(col, row)) {
      this.cells[col][row] = [];
    }
  }

  isValid(col: number, row: number): boolean {
    return col >= 0 && col < GRID.COLS && row >= 0 && row < GRID.ROWS;
  }

  /** Convert pixel position to grid cell */
  pixelToCell(px: number, py: number): { col: number; row: number } {
    return {
      col: Math.floor((px - GRID.OFFSET_X) / GRID.CELL_SIZE),
      row: Math.floor((py - GRID.OFFSET_Y) / GRID.CELL_SIZE),
    };
  }

  /** Convert grid cell to pixel center position */
  cellToPixel(col: number, row: number): { x: number; y: number } {
    return {
      x: GRID.OFFSET_X + col * GRID.CELL_SIZE + GRID.CELL_SIZE / 2,
      y: GRID.OFFSET_Y + row * GRID.CELL_SIZE + GRID.CELL_SIZE / 2,
    };
  }
}
