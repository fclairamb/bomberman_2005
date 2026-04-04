import { GRID, OBJ_HALF_SIZE, Direction } from '../config';
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
    // Try speeds in order: full, 4-equiv, 1-equiv (matching original fallback)
    const speeds = [pixelSpeed];
    const fourEquiv = Math.min(pixelSpeed, 4 * pixelSpeed / 8); // scale proportionally
    const oneEquiv = pixelSpeed / 8;
    if (fourEquiv < pixelSpeed) speeds.push(fourEquiv);
    if (oneEquiv < fourEquiv) speeds.push(oneEquiv);

    for (const trySpeed of speeds) {
      const vec = this.directionToVector(direction, trySpeed);
      if (this.canMove(x, y, vec.dx, vec.dy)) {
        return vec;
      }
    }

    // Corner-sliding: try perpendicular nudge
    return this.tryCornerSlide(x, y, direction, pixelSpeed);
  }

  /**
   * 4-corner collision check.
   * Tests if moving by (dx, dy) from (x, y) would collide with obstacles.
   * Uses OBJ_HALF_SIZE offsets like the original yatil_obstacle_pos().
   */
  private canMove(x: number, y: number, dx: number, dy: number): boolean {
    const nx = x + dx;
    const ny = y + dy;
    const half = OBJ_HALF_SIZE;

    // Check all 4 corners of the object's bounding box
    const corners = [
      { px: nx - half, py: ny - half }, // top-left
      { px: nx + half, py: ny - half }, // top-right
      { px: nx - half, py: ny + half }, // bottom-left
      { px: nx + half, py: ny + half }, // bottom-right
    ];

    for (const corner of corners) {
      const cell = this.grid.pixelToCell(corner.px, corner.py);
      if (this.grid.isBlocked(cell.col, cell.row)) {
        return false;
      }
    }
    return true;
  }

  /**
   * Corner-sliding: when blocked moving in a direction, try nudging
   * perpendicular to slide around corners (matching original behavior).
   */
  private tryCornerSlide(
    x: number,
    y: number,
    direction: Direction,
    speed: number
  ): { dx: number; dy: number } {
    const half = OBJ_HALF_SIZE;
    const slideAmount = GRID.CELL_SIZE / 2;

    if (direction === Direction.UP || direction === Direction.DOWN) {
      // Try sliding left or right
      const dy = direction === Direction.UP ? -speed : speed;
      // Check which side is closer to center of nearest cell
      const cell = this.grid.pixelToCell(x, y);
      const cellCenter = this.grid.cellToPixel(cell.col, cell.row);
      const offset = x - cellCenter.x;

      // Try nudging toward cell center
      const nudge = Math.sign(-offset) * Math.min(Math.abs(offset), speed);
      if (nudge !== 0 && this.canMove(x, y, nudge, dy)) {
        return { dx: nudge, dy };
      }
    } else if (direction === Direction.LEFT || direction === Direction.RIGHT) {
      // Try sliding up or down
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
