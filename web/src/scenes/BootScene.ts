export class BootScene extends Phaser.Scene {
  constructor() {
    super('BootScene');
  }

  preload(): void {
    // Progress bar
    const width = this.cameras.main.width;
    const height = this.cameras.main.height;
    const progressBar = this.add.graphics();
    const progressBox = this.add.graphics();
    progressBox.fillStyle(0x222222, 0.8);
    progressBox.fillRect(width / 2 - 160, height / 2 - 25, 320, 50);

    const loadingText = this.add.text(width / 2, height / 2 - 50, 'Loading...', {
      fontSize: '20px',
      color: '#ffffff',
    }).setOrigin(0.5);

    this.load.on('progress', (value: number) => {
      progressBar.clear();
      progressBar.fillStyle(0xffffff, 1);
      progressBar.fillRect(width / 2 - 150, height / 2 - 15, 300 * value, 30);
    });

    this.load.on('complete', () => {
      progressBar.destroy();
      progressBox.destroy();
      loadingText.destroy();
    });

    // === SPRITESHEETS ===

    // Player spritesheets: 5 cols x 4 rows, each frame 32x54
    this.load.spritesheet('player1', 'assets/sprites/player1.png', {
      frameWidth: 32, frameHeight: 54,
    });
    this.load.spritesheet('player2', 'assets/sprites/player2.png', {
      frameWidth: 32, frameHeight: 54,
    });

    // Enemy spritesheet: same layout as players
    this.load.spritesheet('mechant', 'assets/sprites/mechant.png', {
      frameWidth: 32, frameHeight: 54,
    });

    // Bomb: 4 frames, each 54x54
    this.load.spritesheet('bombe', 'assets/sprites/bombe.png', {
      frameWidth: 54, frameHeight: 54,
    });

    // Explosion: 4 frames, each 54x54
    this.load.spritesheet('explosion', 'assets/sprites/explosion.png', {
      frameWidth: 54, frameHeight: 54,
    });

    // Death: 5 frames, each 32x54 (padded from 32x50)
    this.load.spritesheet('mort', 'assets/sprites/mort.png', {
      frameWidth: 32, frameHeight: 54,
    });

    // === STATIC TILE IMAGES ===
    const tiles = ['sol', 'caisse', 'incassable', 'end_door', 'diamant',
      'bonus_vitesse', 'bonus_explosion', 'bonus_bombes', 'bonus_sante'];
    for (const tile of tiles) {
      this.load.image(tile, `assets/sprites/${tile}.png`);
    }

    // === UI ===
    this.load.image('menu_bg', 'assets/ui/menu_principal.png');
    this.load.image('game_hud', 'assets/ui/jeu.png');
    this.load.image('game_over_bg', 'assets/ui/game_over.png');
    this.load.image('highscores_bg', 'assets/ui/fond_highscores.png');
    this.load.image('pause_bg', 'assets/ui/pause.png');

    // Menu buttons
    const buttons = ['un_joueur', 'deux_joueurs', 'highscores', 'editeur', 'quitter'];
    for (const btn of buttons) {
      this.load.image(btn, `assets/ui/${btn}.png`);
      this.load.image(`${btn}_hover`, `assets/ui/${btn}_hover.png`);
    }

    // === AUDIO ===
    const sounds = [
      'apparition_bonus', 'bonus', 'deflagration', 'diamant', 'explode',
      'gagne', 'meurt', 'mort_mechant', 'perdu', 'piege', 'plant',
      'victory', 'yessir',
    ];
    for (const snd of sounds) {
      this.load.audio(snd, [`assets/audio/${snd}.mp3`]);
    }

    // === LEVELS ===
    for (let i = 1; i <= 25; i++) {
      this.load.json(`level${i}`, `assets/levels/niveau${i}.json`);
    }
  }

  create(): void {
    this.defineAnimations();
    this.scene.start('MenuScene');
  }

  private defineAnimations(): void {
    // Player animations (spritesheet layout: rows = up, right, down, left)
    // Row 0 = up (frames 0-4), Row 1 = right (5-9), Row 2 = down (10-14), Row 3 = left (15-19)
    const directions = [
      { name: 'up', startRow: 0 },
      { name: 'right', startRow: 1 },
      { name: 'down', startRow: 2 },
      { name: 'left', startRow: 3 },
    ];

    for (const playerKey of ['player1', 'player2']) {
      for (const dir of directions) {
        const start = dir.startRow * 5;
        this.anims.create({
          key: `${playerKey}_${dir.name}`,
          frames: this.anims.generateFrameNumbers(playerKey, { start, end: start + 4 }),
          frameRate: 10,  // ~100ms per frame (original: 2 frame pause at 20fps)
          repeat: -1,
        });
      }
    }

    // Enemy animations (same layout as players)
    for (const dir of directions) {
      const start = dir.startRow * 5;
      this.anims.create({
        key: `mechant_${dir.name}`,
        frames: this.anims.generateFrameNumbers('mechant', { start, end: start + 4 }),
        frameRate: 10,
        repeat: -1,
      });
    }

    // Bomb animation (4 frames, slower)
    this.anims.create({
      key: 'bomb_tick',
      frames: this.anims.generateFrameNumbers('bombe', { start: 0, end: 3 }),
      frameRate: 1.4,  // ~700ms per frame (original: 14 frame pause at 20fps)
      repeat: -1,
    });

    // Explosion animation (4 frames)
    this.anims.create({
      key: 'explosion_anim',
      frames: this.anims.generateFrameNumbers('explosion', { start: 0, end: 3 }),
      frameRate: 8,
      repeat: -1,
    });

    // Death animation (5 frames)
    this.anims.create({
      key: 'death',
      frames: this.anims.generateFrameNumbers('mort', { start: 0, end: 4 }),
      frameRate: 2,  // ~500ms per frame (original: 10 frame pause at 20fps)
      repeat: 0,
    });
  }
}
