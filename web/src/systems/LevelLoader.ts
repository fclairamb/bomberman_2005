import { LevelData } from '../types';

export class LevelLoader {
  static load(scene: Phaser.Scene, levelNum: number): LevelData {
    const data = scene.cache.json.get(`level${levelNum}`);
    if (!data) {
      throw new Error(`Level ${levelNum} not found in cache`);
    }
    return data as LevelData;
  }
}
