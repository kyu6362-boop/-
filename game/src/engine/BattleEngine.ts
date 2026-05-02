import type { CombatantState, CardDef, LeaderDef, StatusKey } from '../types';
import { CARD_MAP } from '../data/cards';

export type BattleEvent =
  | { type: 'damage'; target: 'player' | 'enemy'; amount: number; piercing?: boolean }
  | { type: 'heal'; target: 'player' | 'enemy'; amount: number }
  | { type: 'shield'; target: 'player' | 'enemy'; amount: number }
  | { type: 'status'; target: 'player' | 'enemy'; status: StatusKey; duration: number }
  | { type: 'atk_change'; target: 'player' | 'enemy'; delta: number }
  | { type: 'draw'; target: 'player' | 'enemy'; count: number }
  | { type: 'energy'; target: 'player' | 'enemy'; amount: number }
  | { type: 'message'; text: string }
  | { type: 'turn_end' }
  | { type: 'game_over'; winner: 'player' | 'enemy' };

function shuffle<T>(arr: T[]): T[] {
  for (let i = arr.length - 1; i > 0; i--) {
    const j = Math.floor(Math.random() * (i + 1));
    [arr[i], arr[j]] = [arr[j], arr[i]];
  }
  return arr;
}

function buildDeck(leader: LeaderDef): CardDef[] {
  return shuffle(leader.deckIds.map(id => CARD_MAP.get(id)!).filter(Boolean));
}

export function createCombatant(leader: LeaderDef): CombatantState {
  const deck = buildDeck(leader);
  const hand = deck.splice(0, 5);
  return {
    def: leader,
    hp: leader.hp,
    maxHp: leader.hp,
    atk: leader.atk,
    def_bonus: leader.def,
    shield: 0,
    energy: 5,
    hand,
    deck,
    discard: [],
    statuses: [],
    abilityCooldown: 0,
    abilityUsed: false,
  };
}

export class BattleEngine {
  player: CombatantState;
  enemy: CombatantState;
  turn: 'player' | 'enemy' = 'player';
  turnNumber = 1;
  events: BattleEvent[] = [];
  gameOver = false;

  constructor(playerLeader: LeaderDef, enemyLeader: LeaderDef) {
    this.player = createCombatant(playerLeader);
    this.enemy = createCombatant(enemyLeader);
  }

  private emit(e: BattleEvent) { this.events.push(e); }

  private getCombatant(who: 'player' | 'enemy') {
    return who === 'player' ? this.player : this.enemy;
  }

  private getOpponent(who: 'player' | 'enemy'): 'player' | 'enemy' {
    return who === 'player' ? 'enemy' : 'player';
  }

  private hasStatus(c: CombatantState, key: StatusKey): boolean {
    return c.statuses.some(s => s.key === key && s.duration > 0);
  }

  private addStatus(c: CombatantState, key: StatusKey, duration: number, power?: number) {
    const existing = c.statuses.find(s => s.key === key);
    if (existing) {
      existing.duration = Math.max(existing.duration, duration);
      if (power !== undefined) existing.power = power;
    } else {
      c.statuses.push({ key, duration, power });
    }
  }

  private applyDamage(target: CombatantState, amount: number, piercing = false): number {
    if (!piercing && this.hasStatus(target, 'dodge')) {
      this.removeStatus(target, 'dodge');
      this.emit({ type: 'message', text: `${target.def.name}は攻撃を回避した！` });
      return 0;
    }

    let dmg = amount;

    // Counter: reflect 2× damage
    if (this.hasStatus(target, 'counter')) {
      this.removeStatus(target, 'counter');
      const reflected = dmg * 2;
      const attacker = target === this.player ? this.enemy : this.player;
      this.emit({ type: 'message', text: `${target.def.name}はカウンターで${reflected}ダメージを反射！` });
      this.applyDamage(attacker, reflected, true);
      return 0;
    }

    if (this.hasStatus(target, 'halfDmg')) dmg = Math.ceil(dmg / 2);
    dmg = Math.max(0, dmg - target.def_bonus);

    if (!piercing && target.shield > 0) {
      const absorbed = Math.min(target.shield, dmg);
      target.shield -= absorbed;
      dmg -= absorbed;
    }

    target.hp = Math.max(0, target.hp - dmg);
    return dmg;
  }

  private removeStatus(c: CombatantState, key: StatusKey) {
    const idx = c.statuses.findIndex(s => s.key === key);
    if (idx >= 0) c.statuses.splice(idx, 1);
  }

  drawCard(who: 'player' | 'enemy', count = 1) {
    const c = this.getCombatant(who);
    for (let i = 0; i < count; i++) {
      if (c.hand.length >= 7) break;
      if (c.deck.length === 0) {
        if (c.discard.length === 0) break;
        c.deck = shuffle([...c.discard]);
        c.discard = [];
      }
      c.hand.push(c.deck.shift()!);
    }
  }

  playCard(who: 'player' | 'enemy', cardId: string): boolean {
    const actor = this.getCombatant(who);
    const opp = this.getCombatant(this.getOpponent(who));
    const idx = actor.hand.findIndex(c => c.id === cardId);
    if (idx === -1) return false;
    const card = actor.hand[idx];
    if (actor.energy < card.cost) return false;

    actor.energy -= card.cost;
    actor.hand.splice(idx, 1);
    actor.discard.push(card);

    const atkBonus = actor.atk - actor.def.atk;

    switch (card.type) {
      case 'attack': {
        const base = card.power + Math.max(0, atkBonus);
        const dealt = this.applyDamage(opp, base);
        this.emit({ type: 'damage', target: this.getOpponent(who), amount: dealt });
        break;
      }
      case 'defense':
        actor.shield += card.power;
        this.emit({ type: 'shield', target: who, amount: card.power });
        break;
      case 'skill':
      case 'special': {
        // Skill cards: if power > 0, deal damage; plus status/heal effects
        if (card.power > 0 && card.type === 'special') {
          const base = card.power + Math.max(0, atkBonus);
          const dealt = this.applyDamage(opp, base);
          this.emit({ type: 'damage', target: this.getOpponent(who), amount: dealt });
        } else if (card.power > 0 && card.type === 'skill') {
          // Skills: could be heal (regen keywords) or damage
          const desc = card.description;
          if (desc.includes('HP') && desc.includes('回復')) {
            actor.hp = Math.min(actor.maxHp, actor.hp + card.power);
            this.emit({ type: 'heal', target: who, amount: card.power });
          } else if (desc.includes('シールド') && !desc.includes('ダメ')) {
            actor.shield += card.power;
            this.emit({ type: 'shield', target: who, amount: card.power });
          } else if (desc.includes('ダメ') || desc.includes('damage')) {
            const base = card.power + Math.max(0, atkBonus);
            const dealt = this.applyDamage(opp, base);
            this.emit({ type: 'damage', target: this.getOpponent(who), amount: dealt });
          } else if (desc.includes('エネルギー')) {
            const match = desc.match(/エネルギー\+(\d)/);
            if (match) {
              actor.energy += parseInt(match[1]);
              this.emit({ type: 'energy', target: who, amount: parseInt(match[1]) });
            }
          } else if (desc.includes('ドロー') || desc.includes('カード')) {
            const match = desc.match(/(\d)枚/);
            const n = match ? parseInt(match[1]) : 1;
            this.drawCard(who, n);
            this.emit({ type: 'draw', target: who, count: n });
          } else if (desc.includes('ATK+')) {
            const match = desc.match(/ATK\+(\d)/);
            if (match) {
              actor.atk += parseInt(match[1]);
              if (card.status) this.addStatus(actor, card.status, card.statusDuration ?? 2, parseInt(match[1]));
              this.emit({ type: 'atk_change', target: who, delta: parseInt(match[1]) });
            }
          } else if (desc.includes('ATK-')) {
            const match = desc.match(/ATK-(\d)/);
            if (match) {
              opp.atk = Math.max(0, opp.atk - parseInt(match[1]));
              if (card.status) this.addStatus(opp, card.status, card.statusDuration ?? 2, parseInt(match[1]));
              this.emit({ type: 'atk_change', target: this.getOpponent(who), delta: -parseInt(match[1]) });
            }
          }
        } else if (card.power === 0) {
          // Pure utility skill
          const desc = card.description;
          if (desc.includes('エネルギー+')) {
            const match = desc.match(/エネルギー\+(\d)/);
            if (match) { actor.energy += parseInt(match[1]); }
          } else if (desc.includes('シールド')) {
            const match = desc.match(/シールド(\d+)/);
            if (match) { actor.shield += parseInt(match[1]); this.emit({ type: 'shield', target: who, amount: parseInt(match[1]) }); }
          } else if (desc.includes('カード') && desc.includes('ドロー')) {
            const match = desc.match(/(\d)枚/);
            const n = match ? parseInt(match[1]) : 2;
            this.drawCard(who, n);
          } else if (desc.includes('カウンター')) {
            this.addStatus(actor, 'counter', 1);
          } else if (desc.includes('回避')) {
            this.addStatus(actor, 'dodge', 1);
          } else if (desc.includes('凍結')) {
            const match = desc.match(/凍結(\d)/);
            this.addStatus(opp, 'freeze', match ? parseInt(match[1]) : 1);
          } else if (desc.includes('毒')) {
            const match = desc.match(/毒(\d)/);
            this.addStatus(opp, 'poison', match ? parseInt(match[1]) : 2, 2);
          } else if (desc.includes('DEF+')) {
            const match = desc.match(/DEF\+(\d)/);
            if (match) actor.def_bonus += parseInt(match[1]);
          } else if (desc.includes('被ダメ半減')) {
            this.addStatus(actor, 'halfDmg', card.statusDuration ?? 2);
          }
        }
        break;
      }
    }

    // Apply card's status effect to opponent
    if (card.status && card.statusDuration) {
      const statusTarget = ['strengthen', 'halfDmg', 'counter', 'dodge', 'regen'].includes(card.status) ? actor : opp;
      this.addStatus(statusTarget, card.status, card.statusDuration);
      this.emit({ type: 'status', target: statusTarget === actor ? who : this.getOpponent(who), status: card.status, duration: card.statusDuration });
    }

    this.checkGameOver();
    return true;
  }

  useAbility(who: 'player' | 'enemy'): boolean {
    const actor = this.getCombatant(who);
    const opp = this.getCombatant(this.getOpponent(who));
    const ability = actor.def.ability;

    if (actor.abilityCooldown > 0 || actor.abilityUsed) return false;

    const power = ability.power ?? 0;

    switch (ability.effect) {
      case 'burst_fire':
        for (let i = 0; i < 3; i++) {
          const d = this.applyDamage(opp, power);
          this.emit({ type: 'damage', target: this.getOpponent(who), amount: d });
        }
        break;
      case 'big_fire': case 'free_smash': case 'holy_sword': {
        const d = this.applyDamage(opp, power, ability.effect === 'holy_sword');
        this.emit({ type: 'damage', target: this.getOpponent(who), amount: d });
        break;
      }
      case 'burn_dot': case 'solar_burn':
        this.addStatus(opp, 'burn', 3, power);
        this.emit({ type: 'message', text: `${opp.def.name}がバーン状態になった！` });
        if (ability.effect === 'solar_burn') {
          const d = this.applyDamage(opp, power);
          this.emit({ type: 'damage', target: this.getOpponent(who), amount: d });
        }
        break;
      case 'revive_heal': case 'big_heal': case 'holy_restore':
        actor.hp = Math.min(actor.maxHp, actor.hp + power);
        this.emit({ type: 'heal', target: who, amount: power });
        if (ability.effect === 'holy_restore') {
          actor.shield += 5;
          this.emit({ type: 'shield', target: who, amount: 5 });
        }
        actor.abilityUsed = ability.effect === 'revive_heal';
        break;
      case 'weaken_atk':
        opp.atk = Math.max(0, opp.atk - power);
        this.addStatus(opp, 'weaken', 3, power);
        this.emit({ type: 'atk_change', target: this.getOpponent(who), delta: -power });
        break;
      case 'freeze_turn':
        this.addStatus(opp, 'freeze', 1);
        this.emit({ type: 'message', text: `${opp.def.name}が凍結した！` });
        break;
      case 'big_shield':
        actor.shield += power;
        this.emit({ type: 'shield', target: who, amount: power });
        break;
      case 'draw_cards':
        this.drawCard(who, power);
        this.emit({ type: 'draw', target: who, count: power });
        break;
      case 'thunder_stun': case 'zeus_bolt': {
        const d = this.applyDamage(opp, power);
        this.emit({ type: 'damage', target: this.getOpponent(who), amount: d });
        this.addStatus(opp, 'paralyze', 1);
        this.emit({ type: 'message', text: `${opp.def.name}が麻痺した！` });
        break;
      }
      case 'double_next':
        this.addStatus(actor, 'doubleDmg', 1);
        this.emit({ type: 'message', text: `次の攻撃が2倍になる！` });
        break;
      case 'gain_energy':
        actor.energy += power;
        this.emit({ type: 'energy', target: who, amount: power });
        break;
      case 'poison_dot':
        this.addStatus(opp, 'poison', 3, power);
        this.emit({ type: 'message', text: `${opp.def.name}が毒状態になった！` });
        break;
      case 'boost_def':
        actor.def_bonus += power;
        this.addStatus(actor, 'strengthen', 2);
        this.emit({ type: 'message', text: `${actor.def.name}のDEFが上昇した！` });
        break;
      case 'boost_atk':
        actor.atk += power;
        this.addStatus(actor, 'strengthen', 3);
        this.emit({ type: 'atk_change', target: who, delta: power });
        break;
      case 'curse_maxhp':
        opp.maxHp = Math.max(5, opp.maxHp - power);
        opp.hp = Math.min(opp.hp, opp.maxHp);
        this.emit({ type: 'message', text: `${opp.def.name}の最大HPが${power}減少した！` });
        break;
      case 'steal_card':
        if (opp.hand.length > 0) {
          const stolen = opp.hand.splice(Math.floor(Math.random() * opp.hand.length), 1)[0];
          if (actor.hand.length < 7) actor.hand.push(stolen);
          this.emit({ type: 'message', text: `${stolen.name}を奪った！` });
        }
        break;
      case 'execute':
        if (opp.hp < power) {
          opp.hp = 0;
          this.emit({ type: 'damage', target: this.getOpponent(who), amount: opp.hp });
        } else {
          this.emit({ type: 'message', text: '相手のHPが足りず失敗' });
          actor.abilityCooldown = 0;
          return false;
        }
        break;
      case 'dodge_next':
        this.addStatus(actor, 'dodge', 1);
        this.emit({ type: 'message', text: `${actor.def.name}は次の攻撃を回避する！` });
        break;
      case 'double_hit': {
        const d = this.applyDamage(opp, power);
        this.emit({ type: 'damage', target: this.getOpponent(who), amount: d });
        const d2 = this.applyDamage(opp, power);
        this.emit({ type: 'damage', target: this.getOpponent(who), amount: d2 });
        break;
      }
      case 'wind_grace':
        this.drawCard(who, 3);
        actor.atk += power;
        this.emit({ type: 'draw', target: who, count: 3 });
        this.emit({ type: 'atk_change', target: who, delta: power });
        break;
      case 'storm_convert':
        actor.hand = actor.hand.map(card => ({
          ...card, cost: 3, type: 'attack' as const, power: power,
          description: `${power}ダメージ（嵐変換）`
        }));
        this.emit({ type: 'message', text: '手札が全て嵐攻撃に変換された！' });
        break;
      case 'petrify':
        this.addStatus(opp, 'freeze', 1);
        actor.def_bonus += power;
        this.addStatus(actor, 'strengthen', 2);
        this.emit({ type: 'message', text: `${opp.def.name}が石化！自DEF+${power}` });
        break;
      case 'balance_hp': {
        const avg = Math.floor((actor.hp + opp.hp) / 2);
        actor.hp = Math.min(avg, actor.maxHp);
        opp.hp = Math.min(avg, opp.maxHp);
        this.emit({ type: 'message', text: `両者のHPが${avg}に均された！` });
        break;
      }
      case 'counter_dmg':
        this.addStatus(actor, 'counter', 1);
        this.emit({ type: 'message', text: `${actor.def.name}はカウンター態勢に入った！` });
        break;
    }

    actor.abilityCooldown = ability.cooldown;
    if (ability.effect === 'revive_heal') actor.abilityUsed = true;
    this.checkGameOver();
    return true;
  }

  endTurn(who: 'player' | 'enemy') {
    // Tick status effects on the one whose turn just ended
    this.tickStatuses(this.getCombatant(who));

    // Enemy acts
    if (who === 'player') {
      this.doEnemyTurn();
    }

    // Start next player turn
    if (!this.gameOver) {
      this.startTurn('player');
    }
    this.emit({ type: 'turn_end' });
  }

  private doEnemyTurn() {
    const enemy = this.enemy;
    if (this.hasStatus(enemy, 'freeze') || this.hasStatus(enemy, 'paralyze')) {
      this.tickStatuses(enemy);
      this.emit({ type: 'message', text: `${enemy.def.name}は動けない！` });
      return;
    }

    // Use ability if available and helpful
    if (enemy.abilityCooldown === 0 && !enemy.abilityUsed) {
      const shouldUse =
        enemy.hp < enemy.maxHp * 0.5 ||
        this.turnNumber % 4 === 0 ||
        Math.random() < 0.3;
      if (shouldUse) this.useAbility('enemy');
    }

    // Play cards greedily by cost descending
    let attempts = 0;
    while (enemy.energy > 0 && enemy.hand.length > 0 && !this.gameOver && attempts < 20) {
      attempts++;
      const playable = enemy.hand
        .filter(c => c.cost <= enemy.energy)
        .sort((a, b) => b.cost - a.cost);
      if (playable.length === 0) break;
      this.playCard('enemy', playable[0].id);
    }

    this.tickStatuses(enemy);
    if (enemy.abilityCooldown > 0) enemy.abilityCooldown--;
  }

  private startTurn(who: 'player' | 'enemy') {
    const c = this.getCombatant(who);
    c.energy = 5;
    this.drawCard(who, 2);
    if (c.abilityCooldown > 0) c.abilityCooldown--;
    this.turnNumber++;
  }

  private tickStatuses(c: CombatantState) {
    const toRemove: number[] = [];
    c.statuses.forEach((s, i) => {
      if (s.key === 'burn' || s.key === 'poison') {
        const dmg = s.power ?? 2;
        c.hp = Math.max(0, c.hp - dmg);
        const who: 'player' | 'enemy' = c === this.player ? 'player' : 'enemy';
        this.emit({ type: 'damage', target: who, amount: dmg });
        this.emit({ type: 'message', text: `${c.def.name}が${s.key === 'burn' ? 'バーン' : '毒'}で${dmg}ダメージ！` });
      }
      s.duration--;
      if (s.duration <= 0) {
        // Revert stat changes
        if (s.key === 'strengthen' && c.atk > c.def.atk) {
          c.atk = c.def.atk;
        }
        if (s.key === 'weaken') {
          c.atk = c.def.atk;
        }
        toRemove.push(i);
      }
    });
    for (let i = toRemove.length - 1; i >= 0; i--) c.statuses.splice(toRemove[i], 1);
  }

  private checkGameOver() {
    if (this.player.hp <= 0) {
      this.gameOver = true;
      this.emit({ type: 'game_over', winner: 'enemy' });
    } else if (this.enemy.hp <= 0) {
      this.gameOver = true;
      this.emit({ type: 'game_over', winner: 'player' });
    }
  }

  flushEvents(): BattleEvent[] {
    const e = [...this.events];
    this.events = [];
    return e;
  }
}
