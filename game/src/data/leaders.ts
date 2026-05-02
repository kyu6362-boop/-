import type { LeaderDef } from '../types';

export const LEADERS: LeaderDef[] = [
  // ── 炎 ──────────────────────────────────────────────────────
  {
    id: 'kagutsuchi', name: '炎帝カグツチ', title: '炎の支配者', element: 'fire',
    hp: 24, atk: 4, def: 0,
    ability: { name: '炎爆発', description: '相手に2ダメージ×3回', cooldown: 4, effect: 'burst_fire', power: 2 },
    deckIds: ['kagutsuchi_1','kagutsuchi_2','kagutsuchi_3','kagutsuchi_4','kagutsuchi_5','kagutsuchi_6','kagutsuchi_7','kagutsuchi_8'],
    color: '#ff4400', emoji: '🔥'
  },
  {
    id: 'ifrit', name: '烈火イフリート', title: '炎獄の番人', element: 'fire',
    hp: 22, atk: 5, def: 0,
    ability: { name: '炎波', description: '相手に8ダメージ', cooldown: 4, effect: 'big_fire', power: 8 },
    deckIds: ['ifrit_1','ifrit_2','ifrit_3','ifrit_4','ifrit_5','ifrit_6','ifrit_7','ifrit_8'],
    color: '#ff2200', emoji: '🌋'
  },
  {
    id: 'salamandra', name: '焔魔女サラマンドラ', title: '不滅の炎使い', element: 'fire',
    hp: 26, atk: 3, def: 1,
    ability: { name: '猛毒炎', description: '3ターン毎ターン3ダメージ', cooldown: 3, effect: 'burn_dot', power: 3 },
    deckIds: ['salamandra_1','salamandra_2','salamandra_3','salamandra_4','salamandra_5','salamandra_6','salamandra_7','salamandra_8'],
    color: '#ff6600', emoji: '🦎'
  },
  {
    id: 'phoenix', name: '紅蓮フェニックス', title: '不死の炎鳥', element: 'fire',
    hp: 20, atk: 3, def: 0,
    ability: { name: '不死再生', description: 'HP10回復（1回限り）', cooldown: 99, effect: 'revive_heal', power: 10 },
    deckIds: ['phoenix_1','phoenix_2','phoenix_3','phoenix_4','phoenix_5','phoenix_6','phoenix_7','phoenix_8'],
    color: '#ff8800', emoji: '🦅'
  },
  // ── 水 ──────────────────────────────────────────────────────
  {
    id: 'poseidon', name: '海神ポセイドン', title: '深海の王', element: 'water',
    hp: 26, atk: 3, def: 1,
    ability: { name: '大波', description: '相手ATK-2 (3ターン)', cooldown: 3, effect: 'weaken_atk', power: 2 },
    deckIds: ['poseidon_1','poseidon_2','poseidon_3','poseidon_4','poseidon_5','poseidon_6','poseidon_7','poseidon_8'],
    color: '#0066ff', emoji: '🔱'
  },
  {
    id: 'killia', name: '氷結キルア', title: '氷の刃', element: 'water',
    hp: 24, atk: 3, def: 1,
    ability: { name: '完全凍結', description: '相手1ターン行動不能', cooldown: 4, effect: 'freeze_turn', power: 1 },
    deckIds: ['killia_1','killia_2','killia_3','killia_4','killia_5','killia_6','killia_7','killia_8'],
    color: '#00aaff', emoji: '❄️'
  },
  {
    id: 'leviathan', name: '深海リヴァイアサン', title: '海淵の怪物', element: 'water',
    hp: 32, atk: 2, def: 2,
    ability: { name: '深海圧', description: 'シールド10獲得', cooldown: 3, effect: 'big_shield', power: 10 },
    deckIds: ['leviathan_1','leviathan_2','leviathan_3','leviathan_4','leviathan_5','leviathan_6','leviathan_7','leviathan_8'],
    color: '#004499', emoji: '🐋'
  },
  {
    id: 'aqua', name: '水鏡アクア', title: '水面の占い師', element: 'water',
    hp: 22, atk: 2, def: 1,
    ability: { name: '水流', description: 'カード3枚ドロー', cooldown: 3, effect: 'draw_cards', power: 3 },
    deckIds: ['aqua_1','aqua_2','aqua_3','aqua_4','aqua_5','aqua_6','aqua_7','aqua_8'],
    color: '#00ccdd', emoji: '💧'
  },
  // ── 雷 ──────────────────────────────────────────────────────
  {
    id: 'raijin', name: '雷神ライジン', title: '天空の怒り', element: 'thunder',
    hp: 22, atk: 5, def: 0,
    ability: { name: '雷鳴', description: '6ダメ+相手麻痺1ターン', cooldown: 4, effect: 'thunder_stun', power: 6 },
    deckIds: ['raijin_1','raijin_2','raijin_3','raijin_4','raijin_5','raijin_6','raijin_7','raijin_8'],
    color: '#ffcc00', emoji: '⚡'
  },
  {
    id: 'thunderbird', name: '電光サンダーバード', title: '嵐を呼ぶ鳥', element: 'thunder',
    hp: 23, atk: 4, def: 0,
    ability: { name: '雷電', description: '次の攻撃ダメージ×2', cooldown: 3, effect: 'double_next', power: 1 },
    deckIds: ['thunderbird_1','thunderbird_2','thunderbird_3','thunderbird_4','thunderbird_5','thunderbird_6','thunderbird_7','thunderbird_8'],
    color: '#ffee00', emoji: '🦅'
  },
  {
    id: 'zeus', name: '嵐ゼウス', title: '神々の王', element: 'thunder',
    hp: 25, atk: 4, def: 0,
    ability: { name: '神雷', description: '5ダメ+相手スタン1ターン', cooldown: 4, effect: 'zeus_bolt', power: 5 },
    deckIds: ['zeus_1','zeus_2','zeus_3','zeus_4','zeus_5','zeus_6','zeus_7','zeus_8'],
    color: '#ddaa00', emoji: '👑'
  },
  {
    id: 'raika', name: '閃光ライカ', title: '電撃の剣士', element: 'thunder',
    hp: 20, atk: 4, def: 0,
    ability: { name: '超加速', description: 'エネルギー+5獲得', cooldown: 3, effect: 'gain_energy', power: 5 },
    deckIds: ['raika_1','raika_2','raika_3','raika_4','raika_5','raika_6','raika_7','raika_8'],
    color: '#aaff00', emoji: '⚔️'
  },
  // ── 自然 ─────────────────────────────────────────────────────
  {
    id: 'serket', name: '森女神セルケト', title: '毒の女神', element: 'nature',
    hp: 27, atk: 2, def: 1,
    ability: { name: '猛毒', description: '相手に毒3ターン(毎T2ダメ)', cooldown: 3, effect: 'poison_dot', power: 2 },
    deckIds: ['serket_1','serket_2','serket_3','serket_4','serket_5','serket_6','serket_7','serket_8'],
    color: '#44aa00', emoji: '🦂'
  },
  {
    id: 'gaia', name: '大地ガイア', title: '不動の大地', element: 'nature',
    hp: 36, atk: 1, def: 3,
    ability: { name: '大地の守', description: 'DEF+3 (2ターン)', cooldown: 3, effect: 'boost_def', power: 3 },
    deckIds: ['gaia_1','gaia_2','gaia_3','gaia_4','gaia_5','gaia_6','gaia_7','gaia_8'],
    color: '#228800', emoji: '🌍'
  },
  {
    id: 'sylvan', name: '緑風シルヴァン', title: '森の癒し手', element: 'nature',
    hp: 28, atk: 2, def: 1,
    ability: { name: '自然治癒', description: 'HP8回復', cooldown: 3, effect: 'big_heal', power: 8 },
    deckIds: ['sylvan_1','sylvan_2','sylvan_3','sylvan_4','sylvan_5','sylvan_6','sylvan_7','sylvan_8'],
    color: '#66cc22', emoji: '🌿'
  },
  {
    id: 'dryas', name: '蒼樹ドリュアス', title: '樹木の精霊', element: 'nature',
    hp: 26, atk: 2, def: 2,
    ability: { name: '強化の葉', description: 'ATK+2 (3ターン)', cooldown: 3, effect: 'boost_atk', power: 2 },
    deckIds: ['dryas_1','dryas_2','dryas_3','dryas_4','dryas_5','dryas_6','dryas_7','dryas_8'],
    color: '#00aa66', emoji: '🌲'
  },
  // ── 闇 ──────────────────────────────────────────────────────
  {
    id: 'hades', name: '冥王ハデス', title: '死の支配者', element: 'dark',
    hp: 25, atk: 3, def: 1,
    ability: { name: '死の呪', description: '相手MaxHP-5', cooldown: 4, effect: 'curse_maxhp', power: 5 },
    deckIds: ['hades_1','hades_2','hades_3','hades_4','hades_5','hades_6','hades_7','hades_8'],
    color: '#8800cc', emoji: '💀'
  },
  {
    id: 'morgan', name: '暗黒モルガン', title: '闇の魔術師', element: 'dark',
    hp: 24, atk: 3, def: 1,
    ability: { name: 'カード奪取', description: '相手の手札1枚を奪う', cooldown: 3, effect: 'steal_card', power: 1 },
    deckIds: ['morgan_1','morgan_2','morgan_3','morgan_4','morgan_5','morgan_6','morgan_7','morgan_8'],
    color: '#6600aa', emoji: '🧙'
  },
  {
    id: 'azrael', name: '死神アズライル', title: '終末の使者', element: 'dark',
    hp: 22, atk: 4, def: 0,
    ability: { name: '死宣告', description: '相手HP<10なら即死', cooldown: 4, effect: 'execute', power: 10 },
    deckIds: ['azrael_1','azrael_2','azrael_3','azrael_4','azrael_5','azrael_6','azrael_7','azrael_8'],
    color: '#440066', emoji: '🌑'
  },
  {
    id: 'kage', name: '影カゲ', title: '暗殺者の影', element: 'dark',
    hp: 21, atk: 4, def: 0,
    ability: { name: '影分身', description: '次の攻撃を1回回避', cooldown: 3, effect: 'dodge_next', power: 1 },
    deckIds: ['kage_1','kage_2','kage_3','kage_4','kage_5','kage_6','kage_7','kage_8'],
    color: '#220044', emoji: '🥷'
  },
  // ── 光 ──────────────────────────────────────────────────────
  {
    id: 'jeanne', name: '聖女ジャンヌ', title: '光の守護聖人', element: 'light',
    hp: 30, atk: 2, def: 2,
    ability: { name: '聖なる回復', description: 'HP10回復+シールド5', cooldown: 3, effect: 'holy_restore', power: 10 },
    deckIds: ['jeanne_1','jeanne_2','jeanne_3','jeanne_4','jeanne_5','jeanne_6','jeanne_7','jeanne_8'],
    color: '#ffaa00', emoji: '⚜️'
  },
  {
    id: 'ra', name: '光神ラー', title: '太陽の化身', element: 'light',
    hp: 25, atk: 4, def: 1,
    ability: { name: '太陽炎', description: '6ダメ+3Tバーン', cooldown: 4, effect: 'solar_burn', power: 6 },
    deckIds: ['ra_1','ra_2','ra_3','ra_4','ra_5','ra_6','ra_7','ra_8'],
    color: '#ffcc00', emoji: '☀️'
  },
  {
    id: 'michael', name: '天使ミカエル', title: '天界の剣士', element: 'light',
    hp: 28, atk: 3, def: 2,
    ability: { name: '聖剣', description: '8ダメ（シールド無視）', cooldown: 4, effect: 'holy_sword', power: 8 },
    deckIds: ['michael_1','michael_2','michael_3','michael_4','michael_5','michael_6','michael_7','michael_8'],
    color: '#ffdd88', emoji: '😇'
  },
  {
    id: 'byakko', name: '白虎ビャッコ', title: '西の守護神', element: 'light',
    hp: 26, atk: 3, def: 2,
    ability: { name: '白虎守護', description: '被ダメ半減 (2ターン)', cooldown: 4, effect: 'half_damage', power: 2 },
    deckIds: ['byakko_1','byakko_2','byakko_3','byakko_4','byakko_5','byakko_6','byakko_7','byakko_8'],
    color: '#ffffaa', emoji: '🐯'
  },
  // ── 風 ──────────────────────────────────────────────────────
  {
    id: 'fujin', name: '風神フウジン', title: '嵐の主', element: 'wind',
    hp: 22, atk: 4, def: 0,
    ability: { name: '暴風', description: '4ダメ×2回', cooldown: 3, effect: 'double_hit', power: 4 },
    deckIds: ['fujin_1','fujin_2','fujin_3','fujin_4','fujin_5','fujin_6','fujin_7','fujin_8'],
    color: '#00ccaa', emoji: '🌪️'
  },
  {
    id: 'sylph', name: '旋風シルフ', title: '風の精霊', element: 'wind',
    hp: 21, atk: 3, def: 1,
    ability: { name: '風の加護', description: 'ドロー3+ATK+1', cooldown: 3, effect: 'wind_grace', power: 1 },
    deckIds: ['sylph_1','sylph_2','sylph_3','sylph_4','sylph_5','sylph_6','sylph_7','sylph_8'],
    color: '#00eebb', emoji: '🌬️'
  },
  {
    id: 'hurricane', name: '嵐ハリケーン', title: '破壊の風', element: 'wind',
    hp: 23, atk: 4, def: 0,
    ability: { name: '大嵐', description: '手札の全カードを3コスト攻撃に変換', cooldown: 4, effect: 'storm_convert', power: 5 },
    deckIds: ['hurricane_1','hurricane_2','hurricane_3','hurricane_4','hurricane_5','hurricane_6','hurricane_7','hurricane_8'],
    color: '#44ddcc', emoji: '🌊'
  },
  {
    id: 'mercurius', name: '疾風メルクリウス', title: '伝令の神', element: 'wind',
    hp: 20, atk: 4, def: 0,
    ability: { name: '瞬速', description: 'エネルギー+5獲得', cooldown: 3, effect: 'gain_energy', power: 5 },
    deckIds: ['mercurius_1','mercurius_2','mercurius_3','mercurius_4','mercurius_5','mercurius_6','mercurius_7','mercurius_8'],
    color: '#88ffee', emoji: '🪶'
  },
  // ── 土 ──────────────────────────────────────────────────────
  {
    id: 'golem', name: '岩石ゴーレム', title: '不動の巨人', element: 'earth',
    hp: 40, atk: 1, def: 3,
    ability: { name: '石化', description: '相手1Tスキップ+自DEF+2', cooldown: 4, effect: 'petrify', power: 2 },
    deckIds: ['golem_1','golem_2','golem_3','golem_4','golem_5','golem_6','golem_7','golem_8'],
    color: '#886600', emoji: '🗿'
  },
  {
    id: 'atlas', name: '大地アトラス', title: '天を支える者', element: 'earth',
    hp: 36, atk: 2, def: 3,
    ability: { name: '大岩投擲', description: 'コスト0で10ダメージ', cooldown: 4, effect: 'free_smash', power: 10 },
    deckIds: ['atlas_1','atlas_2','atlas_3','atlas_4','atlas_5','atlas_6','atlas_7','atlas_8'],
    color: '#aa8800', emoji: '⛰️'
  },
  {
    id: 'anubis', name: '砂漠アヌビス', title: '審判の神', element: 'earth',
    hp: 26, atk: 3, def: 1,
    ability: { name: '天秤審判', description: '双方のHPを平均値に設定', cooldown: 4, effect: 'balance_hp', power: 0 },
    deckIds: ['anubis_1','anubis_2','anubis_3','anubis_4','anubis_5','anubis_6','anubis_7','anubis_8'],
    color: '#cc9900', emoji: '⚖️'
  },
  {
    id: 'titan', name: '大岩タイタン', title: '大地の守護者', element: 'earth',
    hp: 32, atk: 2, def: 2,
    ability: { name: 'カウンター', description: '次に受けたダメージを2倍返し', cooldown: 3, effect: 'counter_dmg', power: 2 },
    deckIds: ['titan_1','titan_2','titan_3','titan_4','titan_5','titan_6','titan_7','titan_8'],
    color: '#bbaa44', emoji: '🏔️'
  },
];

export const LEADER_MAP = new Map(LEADERS.map(l => [l.id, l]));
