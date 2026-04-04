import { GRID, OBJ_HALF_SIZE, Direction, ObjectType } from '../config';
import { Grid } from './Grid';

export class CollisionSystem {
  constructor(private grid: Grid) {}

  /**
   * Attempt to move an object. Tries full speed, then 4px/frame-equiv, then 1px/frame-equiv.
   * Returns actual pixel displacement or {0,0} if fully blocked.
   * pixelSpeed is already delta-scaled (pixels to move this frame).
   */
  tryMove(
    x: number,
    y: number,
    direction: Direction,
    pixelSpeed: number
  ): { dx: number; dy: number } {
    const speeds = [pixelSpeed];
    const fourEquiv = Math.min(pixelSpeed, 4 * pixelSpeed / 8);
    const oneEquiv = pixelSpeed / 8;
    if (fourEquiv < pixelSpeed) speeds.push(fourEquiv);
    if (oneEquiv < fourEquiv) speeds.push(oneEquiv);

    for (const trySpeed of speeds) {
      const vec = this.directionToVector(direction, trySpeed);
      if (this.canMove(x, y, vec.dx, vec.dy)) {
        return vec;
      }
    }

    return this.tryCornerSlide(x, y, direction, pixelSpeed);
  }

  private canMove(x: number, y: number, dx: number, dy: number): boolean {
    const nx = x + dx;
    const ny = y + dy;
    const half = OBJ_HALF_SIZE;

    const corners = [
      { px: nx - half, py: ny - half },
      { px: nx + half, py: ny - half },
      { px: nx - half, py: ny + half },
      { px: nx + half, py: ny + half },
    ];

    for (const corner of corners) {
      const cell = this.grid.pixelToCell(corner.px, corner.py);
      if (this.isCellBlocked(cell.col, cell.row)) {
        return false;
      }
    }
    return true;
  }

  private isCellBlocked(col: number, row: number): boolean {
    if (!this.grid.isValid(col, row)) return true;

    const entries = this.grid.getEntries(col, row);
    for (const e of entries) {
      if (e.type === ObjectType.MUR ||
          e.type === ObjectType.INCASSABLE ||
          e.type === ObjectType.BIENTOT) {
        return true;
      }
      // Bombs are non-blocking — players and enemies can walk through them
    }
    return false;
  }

  private tryCornerSlide(
    x: number,
    y: number,
    direction: Direction,
    speed: number
  ): { dx: number; dy: number } {
    if (direction === Direction.UP || direction === Direction.DOWN) {
      const dy = direction === Direction.UP ? -speed : speed;
      const cell = this.grid.pixelToCell(x, y);
      const cellCenter = this.grid.cellToPixel(cell.col, cell.row);
      const offset = x - cellCenter.x;

      const nudge = Math.sign(-offset) * Math.min(Math.abs(offset), speed);
      if (nudge !== 0 && this.canMove(x, y, nudge, dy)) {
        return { dx: nudge, dy };
      }
    } else if (direction === Direction.LEFT || direction === Direction.RIGHT) {
      const dx = direction === Direction.LEFT ? -speed : speed;
      const cell = this.grid.pixelToCell(x, y);
      const cellCenter = this.grid.cellToPixel(cell.col, cell.row);
      const offset = y - cellCenter.y;

      const nudge = Math.sign(-offset) * Math.min(Math.abs(offset), speed);
      if (nudge !== 0 && this.canMove(x, y, dx, nudge)) {
        return { dx, dy: nudge };
      }
    }

    return { dx: 0, dy: 0 };
  }

  directionToVector(direction: Direction, speed: number): { dx: number; dy: number } {
    switch (direction) {
      case Direction.UP: return { dx: 0, dy: -speed };
      case Direction.DOWN: return { dx: 0, dy: speed };
      case Direction.LEFT: return { dx: -speed, dy: 0 };
      case Direction.RIGHT: return { dx: speed, dy: 0 };
      default: return { dx: 0, dy: 0 };
    }
  }
}
