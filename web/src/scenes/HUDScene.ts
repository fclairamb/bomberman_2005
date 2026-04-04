import { GRID } from '../config';
import type { GameScene } from './GameScene';

export class HUDScene extends Phaser.Scene {
  private gameScene!: GameScene;
  private livesTexts: Phaser.GameObjects.Text[] = [];
  private scoreTexts: Phaser.GameObjects.Text[] = [];
  private diamondText!: Phaser.GameObjects.Text;
  private timeText!: Phaser.GameObjects.Text;
  private levelText!: Phaser.GameObjects.Text;

  constructor() {
    super('HUDScene');
  }

  create(data: { gameScene: GameScene }): void {
    this.gameScene = data.gameScene;

    // HUD area is to the right of the game grid
    // Grid ends at OFFSET_X + COLS * CELL_SIZE = 30 + 11*54 = 624
    const hudX = 650;
    const textStyle = { fontSize: '16px', color: '#ffffff', fontFamily: 'monospace' };

    // Level indicator
    this.levelText = this.add.text(hudX, 30, '', { ...textStyle, fontSize: '20px' });

    // Player stats
    for (let i = 0; i < 2; i++) {
      const y = 80 + i * 80;
      this.add.text(hudX, y, `P${i + 1}`, { ...textStyle, color: '#ffff00' });
      this.livesTexts.push(this.add.text(hudX, y + 20, '', textStyle));
      this.scoreTexts.push(this.add.text(hudX, y + 40, '', textStyle));
    }

    // Diamond counter
    this.add.text(hudX, 260, 'Diamonds', { ...textStyle, color: '#00ffff' });
    this.diamondText = this.add.text(hudX, 280, '', textStyle);

    // Time
    this.add.text(hudX, 320, 'Time', { ...textStyle, color: '#ff8800' });
    this.timeText = this.add.text(hudX, 340, '', textStyle);
  }

  update(): void {
    if (!this.gameScene) return;

    this.levelText.setText(`Level ${this.gameScene.currentLevel}`);
    this.diamondText.setText(`${this.gameScene.diamondsRemaining}`);

    // Time display
    if (this.gameScene['timeLimit'] > 0) {
      const remaining = Math.max(0,
        this.gameScene['timeLimit'] - (this.gameScene['elapsedTime'] / 1000)
      );
      const min = Math.floor(remaining / 60);
      const sec = Math.floor(remaining % 60);
      this.timeText.setText(`${min}:${sec.toString().padStart(2, '0')}`);
    } else {
      this.timeText.setText('--:--');
    }

    // Player stats
    for (let i = 0; i < this.gameScene.players.length; i++) {
      const player = this.gameScene.players[i];
      if (this.livesTexts[i]) {
        this.livesTexts[i].setText(`Lives: ${player.lives}`);
      }
      if (this.scoreTexts[i]) {
        this.scoreTexts[i].setText(`Score: ${player.score}`);
      }
    }
  }
}
