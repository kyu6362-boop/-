import { LEADERS } from '../data/leaders';
import type { LeaderDef, Element } from '../types';

const ELEMENT_LABELS: Record<Element, string> = {
  fire: '🔥 炎', water: '💧 水', thunder: '⚡ 雷', nature: '🌿 自然',
  dark: '🌑 闇', light: '✨ 光', wind: '🌀 風', earth: '🗿 土'
};

const ELEMENT_ORDER: Element[] = ['fire','water','thunder','nature','dark','light','wind','earth'];

export class LeaderSelectScreen {
  private container: HTMLElement;
  private selected: LeaderDef | null = null;
  private filterEl: Element | 'all' = 'all';
  private onStart: (leader: LeaderDef) => void;

  constructor(root: HTMLElement, onStart: (leader: LeaderDef) => void) {
    this.container = root;
    this.onStart = onStart;
    this.render();
  }

  private render() {
    this.container.innerHTML = `
      <div class="ls-screen">
        <h1 class="ls-title">⚔️ 召喚戦記</h1>
        <p class="ls-sub">リーダーを選んでバトル開始</p>
        <div class="ls-filters" id="ls-filters"></div>
        <div class="ls-grid" id="ls-grid"></div>
        <div class="ls-detail" id="ls-detail">← リーダーを選択してください</div>
        <button class="btn-start" id="btn-start" disabled>バトル開始</button>
      </div>
    `;

    this.renderFilters();
    this.renderGrid();

    document.getElementById('btn-start')!.addEventListener('click', () => {
      if (this.selected) this.onStart(this.selected);
    });
  }

  private renderFilters() {
    const el = document.getElementById('ls-filters')!;
    const all = document.createElement('button');
    all.className = 'filter-btn active';
    all.textContent = '全て';
    all.dataset.el = 'all';
    el.appendChild(all);

    ELEMENT_ORDER.forEach(e => {
      const btn = document.createElement('button');
      btn.className = 'filter-btn';
      btn.textContent = ELEMENT_LABELS[e];
      btn.dataset.el = e;
      el.appendChild(btn);
    });

    el.addEventListener('click', (ev) => {
      const btn = (ev.target as HTMLElement).closest('.filter-btn') as HTMLButtonElement;
      if (!btn) return;
      el.querySelectorAll('.filter-btn').forEach(b => b.classList.remove('active'));
      btn.classList.add('active');
      this.filterEl = (btn.dataset.el === 'all' ? 'all' : btn.dataset.el) as Element | 'all';
      this.renderGrid();
    });
  }

  private renderGrid() {
    const el = document.getElementById('ls-grid')!;
    el.innerHTML = '';
    const visible = this.filterEl === 'all'
      ? LEADERS
      : LEADERS.filter(l => l.element === this.filterEl);

    visible.forEach(leader => {
      const card = document.createElement('div');
      card.className = 'ls-card' + (this.selected?.id === leader.id ? ' selected' : '');
      card.style.setProperty('--lc', leader.color);
      card.innerHTML = `
        <div class="ls-card-emoji">${leader.emoji}</div>
        <div class="ls-card-name">${leader.name}</div>
        <div class="ls-card-stats">HP:${leader.hp} ATK:${leader.atk} DEF:${leader.def}</div>
      `;
      card.addEventListener('click', () => this.selectLeader(leader));
      el.appendChild(card);
    });
  }

  private selectLeader(leader: LeaderDef) {
    this.selected = leader;
    this.renderGrid();
    this.renderDetail(leader);
    const btn = document.getElementById('btn-start') as HTMLButtonElement;
    btn.disabled = false;
    btn.style.background = leader.color;
  }

  private renderDetail(l: LeaderDef) {
    const el = document.getElementById('ls-detail')!;
    el.innerHTML = `
      <div class="detail-wrap" style="border-color:${l.color}">
        <span class="detail-emoji">${l.emoji}</span>
        <div class="detail-info">
          <div class="detail-name" style="color:${l.color}">${l.name}</div>
          <div class="detail-title">${l.title} — ${ELEMENT_LABELS[l.element]}</div>
          <div class="detail-stats">HP <b>${l.hp}</b> / ATK <b>${l.atk}</b> / DEF <b>${l.def}</b></div>
          <div class="detail-ability">
            <span class="ability-badge">固有能力</span>
            <b>${l.ability.name}</b>: ${l.ability.description}
            <span class="ability-cd">(CD: ${l.ability.cooldown === 99 ? '1回限り' : l.ability.cooldown + 'T'})</span>
          </div>
        </div>
      </div>
    `;
  }
}
