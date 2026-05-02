import './style.css';
import { LeaderSelectScreen } from './ui/LeaderSelect';
import { BattleScreen } from './ui/BattleScreen';
import type { LeaderDef } from './types';

const app = document.getElementById('app')!;

function startLeaderSelect() {
  app.innerHTML = '';
  new LeaderSelectScreen(app, (leader: LeaderDef) => startBattle(leader));
}

function startBattle(leader: LeaderDef) {
  app.innerHTML = '';
  new BattleScreen(app, leader, () => startLeaderSelect());
}

startLeaderSelect();
