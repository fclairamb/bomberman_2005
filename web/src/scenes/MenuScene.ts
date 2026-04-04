export class MenuScene extends Phaser.Scene {
  constructor() {
    super('MenuScene');
  }

  create(): void {
    // Background
    this.add.image(400, 300, 'menu_bg');

    const buttons = [
      { y: 163, key: 'un_joueur', action: () => this.startGame(1) },
      { y: 223, key: 'deux_joueurs', action: () => this.startGame(2) },
      { y: 283, key: 'highscores', action: () => this.scene.start('HighScoreScene') },
      { y: 343, key: 'editeur', action: () => this.scene.start('EditorScene') },
      { y: 403, key: 'quitter', action: () => { /* no-op on web */ } },
    ];

    for (const btn of buttons) {
      // Original uses top-left at (217, Y). Buttons are 400x30.
      // Phaser uses center origin, so offset by half dimensions.
      const img = this.add.image(217 + 200, btn.y + 15, btn.key)
        .setInteractive({ useHandCursor: true })
        .on('pointerover', () => img.setTexture(`${btn.key}_hover`))
        .on('pointerout', () => img.setTexture(btn.key))
        .on('pointerdown', btn.action);
    }
  }

  private startGame(numPlayers: number): void {
    this.scene.start('GameScene', { numPlayers, startLevel: 1 });
  }
}
