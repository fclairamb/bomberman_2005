interface HighScoreEntry {
  name: string;
  score: number;
}

export class HighScoreScene extends Phaser.Scene {
  private scores: HighScoreEntry[] = [];
  private fromGame = false;
  private newScore = 0;

  constructor() {
    super('HighScoreScene');
  }

  create(data?: { score?: number; fromGame?: boolean }): void {
    this.add.image(400, 300, 'highscores_bg');

    this.fromGame = data?.fromGame ?? false;
    this.newScore = data?.score ?? 0;

    // Load scores
    this.scores = JSON.parse(localStorage.getItem('bomberman_highscores') ?? '[]');

    if (this.fromGame && this.newScore > 0) {
      this.promptName();
    } else {
      this.showScores();
    }
  }

  private promptName(): void {
    const textStyle = { fontSize: '20px', color: '#ffffff', fontFamily: 'monospace' };

    this.add.text(400, 100, `Score: ${this.newScore}`, {
      fontSize: '28px', color: '#ffff00', fontFamily: 'monospace',
    }).setOrigin(0.5);

    this.add.text(400, 160, 'Enter your name:', textStyle).setOrigin(0.5);

    // Simple HTML input overlay
    const input = document.createElement('input');
    input.type = 'text';
    input.maxLength = 12;
    input.placeholder = 'Your name';
    input.style.cssText = `
      position: fixed; top: 50%; left: 50%; transform: translate(-50%, -50%);
      font-size: 24px; padding: 10px; text-align: center;
      background: #333; color: #fff; border: 2px solid #fff;
      font-family: monospace; outline: none;
    `;
    document.body.appendChild(input);
    input.focus();

    input.addEventListener('keydown', (e) => {
      if (e.key === 'Enter' && input.value.trim()) {
        const name = input.value.trim();
        document.body.removeChild(input);
        this.addScore(name, this.newScore);
        this.showScores();
      }
    });
  }

  private addScore(name: string, score: number): void {
    this.scores.push({ name, score });
    this.scores.sort((a, b) => b.score - a.score);
    this.scores = this.scores.slice(0, 20); // Keep top 20
    localStorage.setItem('bomberman_highscores', JSON.stringify(this.scores));
  }

  private showScores(): void {
    const textStyle = { fontSize: '16px', color: '#ffffff', fontFamily: 'monospace' };

    this.add.text(400, 60, 'HIGH SCORES', {
      fontSize: '32px', color: '#ffff00', fontFamily: 'monospace',
    }).setOrigin(0.5);

    if (this.scores.length === 0) {
      this.add.text(400, 300, 'No scores yet!', textStyle).setOrigin(0.5);
    } else {
      for (let i = 0; i < Math.min(this.scores.length, 10); i++) {
        const entry = this.scores[i];
        const y = 120 + i * 35;
        const rank = `${(i + 1).toString().padStart(2, ' ')}.`;
        const name = entry.name.padEnd(12);
        const score = entry.score.toString().padStart(8);
        this.add.text(200, y, `${rank} ${name} ${score}`, textStyle);
      }
    }

    this.add.text(400, 520, 'Click to return to menu', {
      fontSize: '14px', color: '#888888', fontFamily: 'monospace',
    }).setOrigin(0.5);

    this.input.once('pointerdown', () => this.scene.start('MenuScene'));
    this.input.keyboard?.once('keydown', () => this.scene.start('MenuScene'));
  }
}
