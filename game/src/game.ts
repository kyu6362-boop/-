import { Player, Bullet, Asteroid, Particle, Star } from './entities';

type GameState = 'playing' | 'paused' | 'gameover';

export class Game {
  private canvas: HTMLCanvasElement;
  private ctx: CanvasRenderingContext2D;
  private player: Player;
  private bullets: Bullet[] = [];
  private asteroids: Asteroid[] = [];
  private particles: Particle[] = [];
  private stars: Star[] = [];

  private score = 0;
  private highScore = parseInt(localStorage.getItem('asteroidHS') ?? '0');
  private lives = 3;
  private level = 1;
  private state: GameState = 'playing';

  private keys = new Set<string>();
  private spawnTimer = 0;
  private spawnInterval = 180;
  private asteroidSpeed = 2;
  private frameCount = 0;
  private levelUpDisplay = 0;

  readonly W = 480;
  readonly H = 640;

  constructor(canvas: HTMLCanvasElement) {
    this.canvas = canvas;
    this.canvas.width = this.W;
    this.canvas.height = this.H;
    this.ctx = canvas.getContext('2d')!;
    this.player = new Player(this.W, this.H);
    this.spawnStars();
    this.bindInput();
    this.loop();
  }

  private spawnStars() {
    for (let i = 0; i < 80; i++) {
      this.stars.push(new Star(this.W, this.H));
    }
  }

  private bindInput() {
    window.addEventListener('keydown', (e) => {
      this.keys.add(e.code);
      if (e.code === 'KeyP') this.togglePause();
      if (e.code === 'Space') e.preventDefault();
    });
    window.addEventListener('keyup', (e) => this.keys.delete(e.code));
  }

  private togglePause() {
    if (this.state === 'playing') this.state = 'paused';
    else if (this.state === 'paused') this.state = 'playing';
  }

  private spawnAsteroid() {
    const large = this.level >= 5 && Math.random() < 0.3;
    this.asteroids.push(new Asteroid(this.W, this.asteroidSpeed, large));
  }

  private updateDifficulty() {
    const threshold = this.level * 500;
    if (this.score >= threshold) {
      this.level++;
      this.levelUpDisplay = 120;
      this.asteroidSpeed = Math.min(2 + this.level * 0.4, 6);
      this.spawnInterval = Math.max(48, 180 - this.level * 15);
    }
  }

  private checkCollisions() {
    for (const bullet of this.bullets) {
      if (!bullet.active) continue;
      for (const asteroid of this.asteroids) {
        if (!asteroid.active) continue;
        const dx = bullet.x - asteroid.x;
        const dy = bullet.y - asteroid.y;
        if (Math.sqrt(dx * dx + dy * dy) < asteroid.radius + bullet.radius) {
          bullet.active = false;
          const destroyed = asteroid.hit();
          if (destroyed) {
            asteroid.active = false;
            this.score += asteroid.maxHp > 1 ? 150 : 100;
            this.explode(asteroid.x, asteroid.y, '#f84');
          } else {
            this.explode(asteroid.x, asteroid.y, '#fa4', 5);
          }
          break;
        }
      }
    }

    for (const asteroid of this.asteroids) {
      if (!asteroid.active) continue;
      const dx = this.player.x - asteroid.x;
      const dy = this.player.y - asteroid.y;
      if (Math.sqrt(dx * dx + dy * dy) < asteroid.radius + 14) {
        asteroid.active = false;
        this.lives--;
        this.explode(asteroid.x, asteroid.y, '#f44');
        this.explode(this.player.x, this.player.y, '#4af', 20);
        if (this.lives <= 0) this.endGame();
      }
    }
  }

  private explode(x: number, y: number, color: string, count = 15) {
    for (let i = 0; i < count; i++) {
      this.particles.push(new Particle(x, y, color));
    }
  }

  private endGame() {
    this.state = 'gameover';
    if (this.score > this.highScore) {
      this.highScore = this.score;
      localStorage.setItem('asteroidHS', String(this.highScore));
    }
  }

  restart() {
    this.score = 0;
    this.lives = 3;
    this.level = 1;
    this.asteroidSpeed = 2;
    this.spawnInterval = 180;
    this.spawnTimer = 0;
    this.bullets = [];
    this.asteroids = [];
    this.particles = [];
    this.player = new Player(this.W, this.H);
    this.state = 'playing';
  }

  private update() {
    this.frameCount++;

    for (const star of this.stars) star.update();

    if (this.keys.has('ArrowLeft') || this.keys.has('KeyA')) this.player.move(-1, this.W);
    if (this.keys.has('ArrowRight') || this.keys.has('KeyD')) this.player.move(1, this.W);
    if (this.keys.has('Space') && this.player.canShoot()) {
      this.bullets.push(this.player.shoot());
    }
    this.player.update();

    for (const bullet of this.bullets) bullet.update();
    this.bullets = this.bullets.filter(b => b.active && !b.isOffscreen());

    this.spawnTimer++;
    if (this.spawnTimer >= this.spawnInterval) {
      this.spawnTimer = 0;
      this.spawnAsteroid();
    }

    for (const asteroid of this.asteroids) asteroid.update();

    for (const asteroid of this.asteroids) {
      if (asteroid.active && asteroid.isOffscreen(this.H)) {
        asteroid.active = false;
        this.lives--;
        if (this.lives <= 0) this.endGame();
      }
    }
    this.asteroids = this.asteroids.filter(a => a.active);

    this.checkCollisions();

    for (const p of this.particles) p.update();
    this.particles = this.particles.filter(p => !p.isDead());

    if (this.levelUpDisplay > 0) this.levelUpDisplay--;
    this.updateDifficulty();
  }

  private drawHUD() {
    const ctx = this.ctx;
    ctx.fillStyle = 'rgba(0,0,20,0.7)';
    ctx.fillRect(0, 0, this.W, 36);

    ctx.font = 'bold 14px monospace';
    ctx.fillStyle = '#7cf';
    ctx.fillText(`SCORE: ${this.score}`, 10, 22);

    ctx.fillStyle = '#aaa';
    ctx.fillText(`LV ${this.level}`, this.W / 2 - 18, 22);

    ctx.fillStyle = '#f55';
    const hearts = '♥'.repeat(this.lives) + '♡'.repeat(Math.max(0, 3 - this.lives));
    ctx.fillText(hearts, this.W - 80, 22);
  }

  private drawPause() {
    const ctx = this.ctx;
    ctx.fillStyle = 'rgba(0,0,0,0.5)';
    ctx.fillRect(0, 0, this.W, this.H);
    ctx.fillStyle = '#fff';
    ctx.font = 'bold 36px monospace';
    ctx.textAlign = 'center';
    ctx.fillText('PAUSED', this.W / 2, this.H / 2);
    ctx.font = '16px monospace';
    ctx.fillText('Press P to resume', this.W / 2, this.H / 2 + 40);
    ctx.textAlign = 'left';
  }

  private drawGameOver() {
    const ctx = this.ctx;
    ctx.fillStyle = 'rgba(0,0,0,0.75)';
    ctx.fillRect(0, 0, this.W, this.H);

    ctx.textAlign = 'center';
    ctx.fillStyle = '#f55';
    ctx.font = 'bold 48px monospace';
    ctx.fillText('GAME OVER', this.W / 2, this.H / 2 - 80);

    ctx.fillStyle = '#fff';
    ctx.font = 'bold 24px monospace';
    ctx.fillText(`SCORE: ${this.score}`, this.W / 2, this.H / 2 - 30);
    ctx.fillStyle = '#ff0';
    ctx.fillText(`BEST: ${this.highScore}`, this.W / 2, this.H / 2 + 10);

    // Retry button
    const bw = 180, bh = 48, bx = this.W / 2 - bw / 2, by = this.H / 2 + 50;
    ctx.fillStyle = '#4af';
    ctx.beginPath();
    ctx.roundRect(bx, by, bw, bh, 8);
    ctx.fill();
    ctx.fillStyle = '#000';
    ctx.font = 'bold 20px monospace';
    ctx.fillText('RETRY', this.W / 2, by + 30);

    ctx.textAlign = 'left';

    // Click / key to retry
    if (!this._retryListenerAdded) {
      this._retryListenerAdded = true;
      const handler = (e: MouseEvent | KeyboardEvent) => {
        if (e instanceof KeyboardEvent && e.code !== 'Enter' && e.code !== 'Space') return;
        if (e instanceof MouseEvent) {
          const rect = this.canvas.getBoundingClientRect();
          const mx = (e.clientX - rect.left) * (this.W / rect.width);
          const my = (e.clientY - rect.top) * (this.H / rect.height);
          if (mx < bx || mx > bx + bw || my < by || my > by + bh) return;
        }
        this._retryListenerAdded = false;
        window.removeEventListener('click', handler as EventListener);
        window.removeEventListener('keydown', handler as EventListener);
        this.restart();
      };
      window.addEventListener('click', handler as EventListener);
      window.addEventListener('keydown', handler as EventListener);
    }
  }

  private _retryListenerAdded = false;

  private drawLevelUp() {
    if (this.levelUpDisplay <= 0) return;
    const alpha = Math.min(1, this.levelUpDisplay / 30);
    this.ctx.save();
    this.ctx.globalAlpha = alpha;
    this.ctx.fillStyle = '#ff0';
    this.ctx.font = 'bold 32px monospace';
    this.ctx.textAlign = 'center';
    this.ctx.fillText(`LEVEL ${this.level}!`, this.W / 2, this.H / 2);
    this.ctx.textAlign = 'left';
    this.ctx.restore();
  }

  private render() {
    const ctx = this.ctx;
    ctx.fillStyle = '#000010';
    ctx.fillRect(0, 0, this.W, this.H);

    for (const star of this.stars) star.draw(ctx);
    for (const asteroid of this.asteroids) asteroid.draw(ctx);
    for (const bullet of this.bullets) bullet.draw(ctx);
    this.player.draw(ctx);
    for (const p of this.particles) p.draw(ctx);

    this.drawHUD();
    this.drawLevelUp();

    if (this.state === 'paused') this.drawPause();
    if (this.state === 'gameover') this.drawGameOver();
  }

  private loop() {
    if (this.state === 'playing') this.update();
    this.render();
    requestAnimationFrame(() => this.loop());
  }
}
