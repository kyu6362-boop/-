export class Player {
  x: number;
  y: number;
  width = 36;
  height = 32;
  speed = 5;
  shootCooldown = 0;
  readonly shootDelay = 15;

  constructor(canvasWidth: number, canvasHeight: number) {
    this.x = canvasWidth / 2;
    this.y = canvasHeight - 60;
  }

  move(dx: number, canvasWidth: number) {
    this.x = Math.max(this.width / 2, Math.min(canvasWidth - this.width / 2, this.x + dx * this.speed));
  }

  update() {
    if (this.shootCooldown > 0) this.shootCooldown--;
  }

  canShoot(): boolean {
    return this.shootCooldown === 0;
  }

  shoot(): Bullet {
    this.shootCooldown = this.shootDelay;
    return new Bullet(this.x, this.y - this.height / 2);
  }

  draw(ctx: CanvasRenderingContext2D) {
    ctx.save();
    ctx.translate(this.x, this.y);

    // Engine glow
    ctx.shadowColor = '#4af';
    ctx.shadowBlur = 12;
    ctx.fillStyle = '#4af';
    ctx.fillRect(-4, 10, 8, 10);

    ctx.shadowBlur = 0;

    // Ship body
    ctx.fillStyle = '#7cf';
    ctx.beginPath();
    ctx.moveTo(0, -this.height / 2);
    ctx.lineTo(this.width / 2, this.height / 2);
    ctx.lineTo(this.width / 4, this.height / 3);
    ctx.lineTo(-this.width / 4, this.height / 3);
    ctx.lineTo(-this.width / 2, this.height / 2);
    ctx.closePath();
    ctx.fill();

    // Cockpit
    ctx.fillStyle = '#aef';
    ctx.beginPath();
    ctx.ellipse(0, -4, 6, 9, 0, 0, Math.PI * 2);
    ctx.fill();

    ctx.restore();
  }
}

export class Bullet {
  x: number;
  y: number;
  speed = 10;
  radius = 3;
  active = true;

  constructor(x: number, y: number) {
    this.x = x;
    this.y = y;
  }

  update() {
    this.y -= this.speed;
  }

  isOffscreen(): boolean {
    return this.y < -this.radius;
  }

  draw(ctx: CanvasRenderingContext2D) {
    ctx.save();
    ctx.shadowColor = '#ff0';
    ctx.shadowBlur = 8;
    ctx.fillStyle = '#ff0';
    ctx.fillRect(this.x - 2, this.y - 8, 4, 16);
    ctx.fillStyle = '#fff';
    ctx.fillRect(this.x - 1, this.y - 6, 2, 12);
    ctx.restore();
  }
}

export class Asteroid {
  x: number;
  y: number;
  radius: number;
  speed: number;
  active = true;
  hp: number;
  maxHp: number;
  vertices: { x: number; y: number }[];
  rotation = 0;
  rotationSpeed: number;

  constructor(canvasWidth: number, speed: number, large = false) {
    this.radius = large ? 32 : 18 + Math.random() * 10;
    this.x = this.radius + Math.random() * (canvasWidth - this.radius * 2);
    this.y = -this.radius;
    this.speed = speed;
    this.maxHp = large ? 2 : 1;
    this.hp = this.maxHp;
    this.rotationSpeed = (Math.random() - 0.5) * 0.05;
    this.vertices = this.buildVertices();
  }

  private buildVertices() {
    const count = 8 + Math.floor(Math.random() * 4);
    const verts: { x: number; y: number }[] = [];
    for (let i = 0; i < count; i++) {
      const angle = (i / count) * Math.PI * 2;
      const r = this.radius * (0.65 + Math.random() * 0.35);
      verts.push({ x: Math.cos(angle) * r, y: Math.sin(angle) * r });
    }
    return verts;
  }

  update() {
    this.y += this.speed;
    this.rotation += this.rotationSpeed;
  }

  isOffscreen(canvasHeight: number): boolean {
    return this.y > canvasHeight + this.radius;
  }

  hit(): boolean {
    this.hp--;
    return this.hp <= 0;
  }

  draw(ctx: CanvasRenderingContext2D) {
    ctx.save();
    ctx.translate(this.x, this.y);
    ctx.rotate(this.rotation);

    const damaged = this.hp < this.maxHp;
    ctx.strokeStyle = damaged ? '#f84' : '#aaa';
    ctx.fillStyle = damaged ? '#542' : '#444';
    ctx.lineWidth = 2;
    ctx.shadowColor = damaged ? '#f84' : '#888';
    ctx.shadowBlur = damaged ? 6 : 3;

    ctx.beginPath();
    ctx.moveTo(this.vertices[0].x, this.vertices[0].y);
    for (let i = 1; i < this.vertices.length; i++) {
      ctx.lineTo(this.vertices[i].x, this.vertices[i].y);
    }
    ctx.closePath();
    ctx.fill();
    ctx.stroke();

    ctx.restore();
  }
}

export class Particle {
  x: number;
  y: number;
  vx: number;
  vy: number;
  life: number;
  maxLife: number;
  color: string;
  radius: number;

  constructor(x: number, y: number, color: string) {
    this.x = x;
    this.y = y;
    const angle = Math.random() * Math.PI * 2;
    const speed = 1 + Math.random() * 4;
    this.vx = Math.cos(angle) * speed;
    this.vy = Math.sin(angle) * speed;
    this.maxLife = 20 + Math.floor(Math.random() * 20);
    this.life = this.maxLife;
    this.color = color;
    this.radius = 2 + Math.random() * 3;
  }

  update() {
    this.x += this.vx;
    this.y += this.vy;
    this.vy += 0.05;
    this.life--;
  }

  isDead(): boolean {
    return this.life <= 0;
  }

  draw(ctx: CanvasRenderingContext2D) {
    const alpha = this.life / this.maxLife;
    ctx.save();
    ctx.globalAlpha = alpha;
    ctx.fillStyle = this.color;
    ctx.beginPath();
    ctx.arc(this.x, this.y, this.radius * alpha, 0, Math.PI * 2);
    ctx.fill();
    ctx.restore();
  }
}

export class Star {
  x: number;
  y: number;
  speed: number;
  brightness: number;
  canvasHeight: number;

  constructor(canvasWidth: number, canvasHeight: number) {
    this.canvasHeight = canvasHeight;
    this.x = Math.random() * canvasWidth;
    this.y = Math.random() * canvasHeight;
    this.speed = 0.3 + Math.random() * 1.2;
    this.brightness = 0.3 + Math.random() * 0.7;
  }

  update() {
    this.y += this.speed;
    if (this.y > this.canvasHeight) this.y = 0;
  }

  draw(ctx: CanvasRenderingContext2D) {
    ctx.save();
    ctx.globalAlpha = this.brightness;
    ctx.fillStyle = '#fff';
    ctx.fillRect(this.x, this.y, 1.5, 1.5);
    ctx.restore();
  }
}
