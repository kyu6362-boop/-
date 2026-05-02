export type Element = 'fire' | 'water' | 'thunder' | 'nature' | 'dark' | 'light' | 'wind' | 'earth';
export type CardType = 'attack' | 'defense' | 'skill' | 'special';
export type StatusKey = 'burn' | 'freeze' | 'paralyze' | 'poison' | 'curse' | 'shield' | 'regen' | 'strengthen' | 'weaken' | 'dodge' | 'counter' | 'halfDmg' | 'doubleDmg';

export interface StatusEffect {
  key: StatusKey;
  duration: number;
  power?: number;
}

export interface CardDef {
  id: string;
  name: string;
  cost: number;
  type: CardType;
  element: Element;
  power: number;
  status?: StatusKey;
  statusDuration?: number;
  description: string;
  leaderId: string;
}

export interface LeaderAbility {
  name: string;
  description: string;
  cooldown: number;
  effect: string;
  power?: number;
}

export interface LeaderDef {
  id: string;
  name: string;
  title: string;
  element: Element;
  hp: number;
  atk: number;
  def: number;
  ability: LeaderAbility;
  deckIds: string[];
  color: string;
  emoji: string;
}

export interface CombatantState {
  def: LeaderDef;
  hp: number;
  maxHp: number;
  atk: number;
  def_bonus: number;
  shield: number;
  energy: number;
  hand: CardDef[];
  deck: CardDef[];
  discard: CardDef[];
  statuses: StatusEffect[];
  abilityCooldown: number;
  abilityUsed: boolean;
}

export type GamePhase = 'leader-select' | 'battle' | 'result';
export type BattleTurn = 'player' | 'enemy';
