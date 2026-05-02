import './style.css';
import { Game } from './game';

const app = document.querySelector<HTMLDivElement>('#app')!;
const canvas = document.createElement('canvas');
app.appendChild(canvas);

new Game(canvas);
