export class GameOverScene extends Phaser.Scene {
  constructor() {
    super('GameOverScene');
  }

  create(data: { score: number; level: number }): void {
    this.add.image(400, 300, 'game_over_bg');

    const textStyle = { fontSize: '24px', color: '#ffffff', fontFamily: 'monospace' };

    this.add.text(400, 200, 'GAME OVER', {
      fontSize: '48px', color: '#ff0000', fontFamily: 'monospace',
    }).setOrigin(0.5);

    this.add.text(400, 280, `Level: ${data.level}`, textStyle).setOrigin(0.5);
    this.add.text(400, 320, `Score: ${data.score}`, textStyle).setOrigin(0.5);

    this.add.text(400, 420, 'Click to continue', {
      fontSize: '18px', color: '#888888', fontFamily: 'monospace',
    }).setOrigin(0.5);

    this.input.once('pointerdown', () => {
      this.scene.start('HighScoreScene', { score: data.score, fromGame: true });
    });

    this.input.keyboard?.once('keydown', () => {
      this.scene.start('HighScoreScene', { score: data.score, fromGame: true });
    });
  }
}
