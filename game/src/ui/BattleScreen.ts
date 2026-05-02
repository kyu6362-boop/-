import { BattleEngine } from '../engine/BattleEngine';
import type { CombatantState, CardDef, StatusKey } from '../types';
import { LEADERS } from '../data/leaders';

const STATUS_LABELS: Partial<Record<StatusKey, string>> = {
  burn: '🔥燃焼', freeze: '❄️凍結', paralyze: '⚡麻痺', poison: '☠️毒',
  curse: '💀呪', shield: '🛡', regen: '💚再生', strengthen: '💪強化',
  weaken: '👇弱体', dodge: '💨回避', counter: '↩️反撃', halfDmg: '🔰半減', doubleDmg: '⚡倍撃'
};

export class BattleScreen {
  private container: HTMLElement;
  private engine: BattleEngine;
  private onEnd: (win: boolean) => void;
  private log: string[] = [];
  private animating = false;

  constructor(root: HTMLElement, playerLeader: import('../types').LeaderDef, onEnd: (win: boolean) => void) {
    this.container = root;
    this.onEnd = onEnd;
    const enemyLeader = LEADERS.filter(l => l.id !== playerLeader.id)[Math.floor(Math.random() * 31)];
    this.engine = new BattleEngine(playerLeader, enemyLeader);
    this.render();
  }

  private render() {
    this.container.innerHTML = `
      <div class="battle-screen">
        <div class="battle-top">
          <div class="combatant enemy-side" id="enemy-area"></div>
          <div class="battle-log" id="battle-log"><p>バトル開始！</p></div>
        </div>
        <div class="battle-mid">
          <div class="enemy-hand" id="enemy-hand"></div>
        </div>
        <div class="battle-bottom">
          <div class="player-hand" id="player-hand"></div>
          <div class="combatant player-side" id="player-area"></div>
          <div class="battle-actions">
            <button class="btn-ability" id="btn-ability">⚡ 能力発動</button>
            <button class="btn-end-turn" id="btn-end-turn">→ ターン終了</button>
          </div>
        </div>
      </div>
    `;

    document.getElementById('btn-end-turn')!.addEventListener('click', () => this.endTurn());
    document.getElementById('btn-ability')!.addEventListener('click', () => this.useAbility());

    this.updateUI();
  }

  private updateUI() {
    this.renderCombatant('enemy-area', this.engine.enemy, false);
    this.renderCombatant('player-area', this.engine.player, true);
    this.renderHand();
    this.renderEnemyHand();
    this.updateAbilityBtn();
  }

  private renderCombatant(id: string, c: CombatantState, isPlayer: boolean) {
    const el = document.getElementById(id)!;
    const hpPct = Math.max(0, (c.hp / c.maxHp) * 100);
    const hpColor = hpPct > 50 ? '#44cc44' : hpPct > 25 ? '#ffaa00' : '#ff4444';
    const statuses = c.statuses.map(s => `<span class="status-badge">${STATUS_LABELS[s.key] ?? s.key}${s.duration > 0 ? s.duration : ''}</span>`).join('');
    el.innerHTML = `
      <div class="combatant-header" style="color:${c.def.color}">
        <span class="cbt-emoji">${c.def.emoji}</span>
        <span class="cbt-name">${c.def.name}</span>
        ${isPlayer ? `<span class="cbt-label">あなた</span>` : `<span class="cbt-label">敵</span>`}
      </div>
      <div class="hp-bar-wrap">
        <div class="hp-bar" style="width:${hpPct}%;background:${hpColor}"></div>
        <span class="hp-text">HP ${c.hp}/${c.maxHp}</span>
      </div>
      <div class="cbt-stats">
        ATK:<b>${c.atk}</b> DEF:<b>${c.def_bonus}</b>
        ${c.shield > 0 ? `🛡<b>${c.shield}</b>` : ''}
        ⚡<b>${c.energy}</b>
      </div>
      <div class="status-row">${statuses}</div>
    `;
  }

  private renderHand() {
    const el = document.getElementById('player-hand')!;
    const p = this.engine.player;
    el.innerHTML = '';
    if (p.hand.length === 0) {
      el.innerHTML = '<div class="empty-hand">手札なし</div>';
      return;
    }
    p.hand.forEach(card => {
      const div = document.createElement('div');
      const canPlay = card.cost <= p.energy && !this.animating && !this.engine.gameOver;
      div.className = `card-item ${canPlay ? 'playable' : 'disabled'}`;
      div.style.setProperty('--cc', this.elementColor(card.element));
      div.innerHTML = `
        <div class="card-cost">⚡${card.cost}</div>
        <div class="card-type-badge">${this.typeLabel(card.type)}</div>
        <div class="card-name">${card.name}</div>
        <div class="card-power">${card.power > 0 ? (card.type === 'defense' ? `🛡${card.power}` : `⚔️${card.power}`) : ''}</div>
        <div class="card-desc">${card.description}</div>
      `;
      if (canPlay) {
        div.addEventListener('click', () => this.playCard(card));
      }
      el.appendChild(div);
    });
  }

  private renderEnemyHand() {
    const el = document.getElementById('enemy-hand')!;
    el.innerHTML = '';
    const n = this.engine.enemy.hand.length;
    for (let i = 0; i < n; i++) {
      const div = document.createElement('div');
      div.className = 'card-back';
      div.textContent = '🂠';
      el.appendChild(div);
    }
  }

  private updateAbilityBtn() {
    const btn = document.getElementById('btn-ability') as HTMLButtonElement;
    const p = this.engine.player;
    const cd = p.abilityCooldown;
    const used = p.abilityUsed;
    btn.disabled = cd > 0 || used || this.engine.gameOver;
    btn.textContent = used ? '使用済み' : cd > 0 ? `⚡ CD: ${cd}T` : `⚡ ${p.def.ability.name}`;
    btn.style.opacity = (cd > 0 || used) ? '0.5' : '1';
  }

  private playCard(card: CardDef) {
    if (this.animating || this.engine.gameOver) return;
    this.engine.playCard('player', card.id);
    this.processEvents();
    this.updateUI();
  }

  private useAbility() {
    if (this.animating || this.engine.gameOver) return;
    const ok = this.engine.useAbility('player');
    if (!ok) return;
    this.processEvents();
    this.updateUI();
  }

  private endTurn() {
    if (this.animating || this.engine.gameOver) return;
    this.setActionsEnabled(false);
    this.engine.endTurn('player');
    this.processEvents();
    this.updateUI();
    this.setActionsEnabled(true);
  }

  private setActionsEnabled(on: boolean) {
    const btns = this.container.querySelectorAll<HTMLButtonElement>('.btn-ability, .btn-end-turn, .card-item');
    btns.forEach(b => b.disabled = !on);
  }

  private processEvents() {
    const events = this.engine.flushEvents();
    for (const ev of events) {
      if (ev.type === 'message') {
        this.addLog(ev.text);
      } else if (ev.type === 'damage') {
        const who = ev.target === 'player' ? this.engine.player.def.name : this.engine.enemy.def.name;
        if (ev.amount > 0) this.addLog(`${who} に ${ev.amount} ダメージ！`);
      } else if (ev.type === 'heal') {
        const who = ev.target === 'player' ? this.engine.player.def.name : this.engine.enemy.def.name;
        this.addLog(`${who} が ${ev.amount} HP 回復！`);
      } else if (ev.type === 'game_over') {
        setTimeout(() => this.showResult(ev.winner === 'player'), 400);
      }
    }
  }

  private addLog(text: string) {
    this.log.unshift(text);
    if (this.log.length > 12) this.log.pop();
    const el = document.getElementById('battle-log');
    if (el) el.innerHTML = this.log.map(t => `<p>${t}</p>`).join('');
  }

  private showResult(win: boolean) {
    this.container.innerHTML = `
      <div class="result-screen">
        <div class="result-icon">${win ? '🏆' : '💀'}</div>
        <h2 class="result-title" style="color:${win ? '#ffcc00' : '#ff4444'}">${win ? '勝利！' : '敗北...'}</h2>
        <p class="result-sub">${win
          ? `${this.engine.enemy.def.name} を倒した！`
          : `${this.engine.player.def.name} が敗れた...`
        }</p>
        <button class="btn-retry">もう一度プレイ</button>
      </div>
    `;
    this.container.querySelector('.btn-retry')!.addEventListener('click', () => this.onEnd(win));
  }

  private elementColor(el: string): string {
    const map: Record<string, string> = {
      fire:'#ff4400', water:'#0088ff', thunder:'#ffdd00', nature:'#44aa00',
      dark:'#8800cc', light:'#ffaa00', wind:'#00ccaa', earth:'#886600'
    };
    return map[el] ?? '#888';
  }

  private typeLabel(t: string): string {
    return { attack:'攻', defense:'防', skill:'技', special:'奥' }[t] ?? t;
  }
}
