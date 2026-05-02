import type { CardDef } from '../types';

// Helper to build cards concisely
const c = (
  id: string, name: string, cost: number, type: CardDef['type'],
  element: CardDef['element'], power: number, desc: string,
  leaderId: string, status?: CardDef['status'], statusDuration?: number
): CardDef => ({ id, name, cost, type, element, power, description: desc, leaderId, status, statusDuration });

export const ALL_CARDS: CardDef[] = [
  // ── kagutsuchi ────────────────────────────────────────────
  c('kagutsuchi_1','炎拳',1,'attack','fire',3,'3ダメージ','kagutsuchi'),
  c('kagutsuchi_2','火炎斬',2,'attack','fire',5,'5ダメージ','kagutsuchi'),
  c('kagutsuchi_3','業火',3,'attack','fire',8,'8ダメージ+バーン2T','kagutsuchi','burn',2),
  c('kagutsuchi_4','炎盾',1,'defense','fire',4,'シールド4','kagutsuchi'),
  c('kagutsuchi_5','炎壁',2,'defense','fire',7,'シールド7','kagutsuchi'),
  c('kagutsuchi_6','炎強化',2,'skill','fire',2,'ATK+2 (2T)','kagutsuchi','strengthen',2),
  c('kagutsuchi_7','灼熱波',3,'skill','fire',4,'相手にバーン3T','kagutsuchi','burn',3),
  c('kagutsuchi_8','炎帝覚醒',4,'special','fire',12,'12ダメージ+バーン3T','kagutsuchi','burn',3),

  // ── ifrit ────────────────────────────────────────────────
  c('ifrit_1','烈火拳',1,'attack','fire',4,'4ダメージ','ifrit'),
  c('ifrit_2','火炎柱',2,'attack','fire',6,'6ダメージ','ifrit'),
  c('ifrit_3','地獄炎',3,'attack','fire',9,'9ダメージ','ifrit'),
  c('ifrit_4','火鎧',1,'defense','fire',3,'シールド3','ifrit'),
  c('ifrit_5','炎の鎧',2,'defense','fire',6,'シールド6','ifrit'),
  c('ifrit_6','炎加速',2,'skill','fire',0,'エネルギー+3','ifrit'),
  c('ifrit_7','炎の怒り',3,'skill','fire',3,'ATK+3 (2T)','ifrit','strengthen',2),
  c('ifrit_8','炎獄解放',4,'special','fire',14,'14ダメージ','ifrit'),

  // ── salamandra ────────────────────────────────────────────
  c('salamandra_1','毒炎',1,'attack','fire',2,'2ダメ+バーン1T','salamandra','burn',1),
  c('salamandra_2','呪炎',2,'attack','fire',4,'4ダメ+バーン2T','salamandra','burn',2),
  c('salamandra_3','焔毒波',3,'attack','fire',5,'5ダメ+バーン3T','salamandra','burn',3),
  c('salamandra_4','炎盾',1,'defense','fire',4,'シールド4','salamandra'),
  c('salamandra_5','焔守護',2,'defense','fire',6,'シールド6','salamandra'),
  c('salamandra_6','毒強化',2,'skill','fire',2,'相手にバーン2T','salamandra','burn',2),
  c('salamandra_7','焔呪縛',3,'skill','fire',3,'相手ATK-2 (2T)','salamandra','weaken',2),
  c('salamandra_8','魔炎解放',4,'special','fire',10,'10ダメ+バーン4T','salamandra','burn',4),

  // ── phoenix ───────────────────────────────────────────────
  c('phoenix_1','火炎羽',1,'attack','fire',3,'3ダメージ','phoenix'),
  c('phoenix_2','炎翼',2,'attack','fire',5,'5ダメージ','phoenix'),
  c('phoenix_3','不死の炎',3,'attack','fire',7,'7ダメージ','phoenix'),
  c('phoenix_4','炎盾',1,'defense','fire',4,'シールド4','phoenix'),
  c('phoenix_5','炎再生',2,'skill','fire',4,'HP4回復','phoenix'),
  c('phoenix_6','再生の炎',3,'skill','fire',6,'HP6回復+シールド3','phoenix'),
  c('phoenix_7','不死鳥の涙',3,'skill','fire',8,'HP8回復','phoenix'),
  c('phoenix_8','炎鳥覚醒',4,'special','fire',10,'10ダメ+HP5回復','phoenix'),

  // ── poseidon ─────────────────────────────────────────────
  c('poseidon_1','水流拳',1,'attack','water',3,'3ダメージ','poseidon'),
  c('poseidon_2','海波',2,'attack','water',5,'5ダメージ','poseidon'),
  c('poseidon_3','津波',3,'attack','water',8,'8ダメージ','poseidon'),
  c('poseidon_4','海盾',1,'defense','water',4,'シールド4','poseidon'),
  c('poseidon_5','水鎧',2,'defense','water',7,'シールド7','poseidon'),
  c('poseidon_6','海流弱化',2,'skill','water',2,'相手ATK-2 (2T)','poseidon','weaken',2),
  c('poseidon_7','深海圧',3,'skill','water',3,'相手ATK-3 (3T)','poseidon','weaken',3),
  c('poseidon_8','海神の怒',4,'special','water',12,'12ダメ+相手ATK-3 (3T)','poseidon','weaken',3),

  // ── killia ────────────────────────────────────────────────
  c('killia_1','氷刃',1,'attack','water',3,'3ダメージ','killia'),
  c('killia_2','氷結斬',2,'attack','water',5,'5ダメ+凍結1T','killia','freeze',1),
  c('killia_3','吹雪',3,'attack','water',7,'7ダメ+凍結1T','killia','freeze',1),
  c('killia_4','氷盾',1,'defense','water',4,'シールド4','killia'),
  c('killia_5','氷壁',2,'defense','water',7,'シールド7','killia'),
  c('killia_6','氷結縛',2,'skill','water',0,'相手凍結1T','killia','freeze',1),
  c('killia_7','絶対零度',3,'skill','water',4,'4ダメ+凍結2T','killia','freeze',2),
  c('killia_8','氷結解放',4,'special','water',11,'11ダメ+凍結2T','killia','freeze',2),

  // ── leviathan ─────────────────────────────────────────────
  c('leviathan_1','触手打',1,'attack','water',2,'2ダメージ','leviathan'),
  c('leviathan_2','深海波',2,'attack','water',4,'4ダメージ','leviathan'),
  c('leviathan_3','大波砕',3,'attack','water',6,'6ダメージ','leviathan'),
  c('leviathan_4','深海鎧',1,'defense','water',5,'シールド5','leviathan'),
  c('leviathan_5','深海壁',2,'defense','water',9,'シールド9','leviathan'),
  c('leviathan_6','深海守護',2,'skill','water',6,'シールド6+DEF+1 (2T)','leviathan'),
  c('leviathan_7','圧縮盾',3,'skill','water',0,'シールド12','leviathan'),
  c('leviathan_8','深海解放',4,'special','water',8,'8ダメ+シールド8','leviathan'),

  // ── aqua ─────────────────────────────────────────────────
  c('aqua_1','水弾',1,'attack','water',2,'2ダメージ','aqua'),
  c('aqua_2','水流撃',2,'attack','water',4,'4ダメージ','aqua'),
  c('aqua_3','水鏡波',3,'attack','water',6,'6ダメージ','aqua'),
  c('aqua_4','水盾',1,'defense','water',3,'シールド3','aqua'),
  c('aqua_5','流水壁',2,'defense','water',5,'シールド5','aqua'),
  c('aqua_6','水流加速',2,'skill','water',0,'カード2枚ドロー','aqua'),
  c('aqua_7','水鏡反射',3,'skill','water',0,'カード3枚ドロー+HP3回復','aqua'),
  c('aqua_8','水鏡解放',4,'special','water',8,'8ダメ+カード3枚ドロー','aqua'),

  // ── raijin ────────────────────────────────────────────────
  c('raijin_1','電撃',1,'attack','thunder',4,'4ダメージ','raijin'),
  c('raijin_2','雷撃',2,'attack','thunder',6,'6ダメージ','raijin'),
  c('raijin_3','落雷',3,'attack','thunder',8,'8ダメ+麻痺1T','raijin','paralyze',1),
  c('raijin_4','雷盾',1,'defense','thunder',3,'シールド3','raijin'),
  c('raijin_5','雷鎧',2,'defense','thunder',5,'シールド5','raijin'),
  c('raijin_6','雷速',2,'skill','thunder',0,'エネルギー+3','raijin'),
  c('raijin_7','連雷',3,'skill','thunder',5,'5ダメ+麻痺1T','raijin','paralyze',1),
  c('raijin_8','雷神解放',4,'special','thunder',13,'13ダメ+麻痺2T','raijin','paralyze',2),

  // ── thunderbird ───────────────────────────────────────────
  c('thunderbird_1','電翼',1,'attack','thunder',3,'3ダメージ','thunderbird'),
  c('thunderbird_2','雷翼斬',2,'attack','thunder',5,'5ダメージ','thunderbird'),
  c('thunderbird_3','嵐翼',3,'attack','thunder',8,'8ダメージ','thunderbird'),
  c('thunderbird_4','電盾',1,'defense','thunder',3,'シールド3','thunderbird'),
  c('thunderbird_5','雷羽盾',2,'defense','thunder',6,'シールド6','thunderbird'),
  c('thunderbird_6','雷加速',2,'skill','thunder',2,'ATK+2 (2T)','thunderbird','strengthen',2),
  c('thunderbird_7','電光乱舞',3,'skill','thunder',3,'3ダメ×2回','thunderbird'),
  c('thunderbird_8','電光解放',4,'special','thunder',12,'12ダメ+ATK+2 (3T)','thunderbird','strengthen',3),

  // ── zeus ─────────────────────────────────────────────────
  c('zeus_1','神撃',1,'attack','thunder',4,'4ダメージ','zeus'),
  c('zeus_2','神雷刃',2,'attack','thunder',6,'6ダメージ','zeus'),
  c('zeus_3','天空撃',3,'attack','thunder',9,'9ダメ+麻痺1T','zeus','paralyze',1),
  c('zeus_4','神盾',1,'defense','thunder',4,'シールド4','zeus'),
  c('zeus_5','神鎧',2,'defense','thunder',7,'シールド7','zeus'),
  c('zeus_6','神の加護',2,'skill','thunder',0,'HP4回復+ATK+1 (2T)','zeus','strengthen',2),
  c('zeus_7','神罰',3,'skill','thunder',6,'6ダメ+相手ATK-2 (2T)','zeus','weaken',2),
  c('zeus_8','神々の怒',4,'special','thunder',14,'14ダメ+麻痺1T','zeus','paralyze',1),

  // ── raika ─────────────────────────────────────────────────
  c('raika_1','電斬',1,'attack','thunder',4,'4ダメージ','raika'),
  c('raika_2','閃光斬',2,'attack','thunder',6,'6ダメージ','raika'),
  c('raika_3','超速斬',3,'attack','thunder',8,'8ダメージ','raika'),
  c('raika_4','電盾',1,'defense','thunder',3,'シールド3','raika'),
  c('raika_5','閃光盾',2,'defense','thunder',5,'シールド5','raika'),
  c('raika_6','超加速',2,'skill','thunder',0,'エネルギー+3','raika'),
  c('raika_7','電光弾',3,'skill','thunder',4,'4ダメ+次攻撃+2','raika'),
  c('raika_8','閃光解放',4,'special','thunder',11,'11ダメ+エネルギー+2','raika'),

  // ── serket ────────────────────────────────────────────────
  c('serket_1','毒針',1,'attack','nature',2,'2ダメ+毒1T','serket','poison',1),
  c('serket_2','毒爪',2,'attack','nature',4,'4ダメ+毒2T','serket','poison',2),
  c('serket_3','猛毒波',3,'attack','nature',5,'5ダメ+毒3T','serket','poison',3),
  c('serket_4','草盾',1,'defense','nature',4,'シールド4','serket'),
  c('serket_5','毒霧壁',2,'defense','nature',4,'シールド4+相手毒1T','serket','poison',1),
  c('serket_6','毒強化',2,'skill','nature',3,'相手に毒2T(毎T3ダメ)','serket','poison',2),
  c('serket_7','猛毒蔓延',3,'skill','nature',0,'相手に毒4T','serket','poison',4),
  c('serket_8','女神の毒',4,'special','nature',8,'8ダメ+毒4T(毎T3ダメ)','serket','poison',4),

  // ── gaia ─────────────────────────────────────────────────
  c('gaia_1','岩撃',1,'attack','nature',2,'2ダメージ','gaia'),
  c('gaia_2','大地撃',2,'attack','nature',3,'3ダメージ','gaia'),
  c('gaia_3','大岩砕',3,'attack','nature',5,'5ダメージ','gaia'),
  c('gaia_4','岩盾',1,'defense','nature',6,'シールド6','gaia'),
  c('gaia_5','大地の壁',2,'defense','nature',10,'シールド10','gaia'),
  c('gaia_6','大地守護',2,'skill','nature',0,'シールド8+DEF+1 (2T)','gaia'),
  c('gaia_7','地盤強化',3,'skill','nature',0,'シールド12','gaia'),
  c('gaia_8','大地解放',4,'special','nature',6,'6ダメ+シールド10','gaia'),

  // ── sylvan ────────────────────────────────────────────────
  c('sylvan_1','草風拳',1,'attack','nature',2,'2ダメージ','sylvan'),
  c('sylvan_2','緑風斬',2,'attack','nature',4,'4ダメージ','sylvan'),
  c('sylvan_3','森の嵐',3,'attack','nature',6,'6ダメージ','sylvan'),
  c('sylvan_4','葉盾',1,'defense','nature',3,'シールド3','sylvan'),
  c('sylvan_5','森壁',2,'defense','nature',5,'シールド5','sylvan'),
  c('sylvan_6','自然治癒',2,'skill','nature',4,'HP4回復','sylvan'),
  c('sylvan_7','森の加護',3,'skill','nature',6,'HP6回復+シールド4','sylvan'),
  c('sylvan_8','緑風解放',4,'special','nature',7,'7ダメ+HP7回復','sylvan'),

  // ── dryas ─────────────────────────────────────────────────
  c('dryas_1','葉刃',1,'attack','nature',2,'2ダメージ','dryas'),
  c('dryas_2','樹木撃',2,'attack','nature',4,'4ダメージ','dryas'),
  c('dryas_3','古木砕',3,'attack','nature',6,'6ダメージ','dryas'),
  c('dryas_4','樹皮盾',1,'defense','nature',4,'シールド4','dryas'),
  c('dryas_5','蒼樹壁',2,'defense','nature',7,'シールド7','dryas'),
  c('dryas_6','樹木強化',2,'skill','nature',2,'ATK+2 (3T)','dryas','strengthen',3),
  c('dryas_7','精霊加護',3,'skill','nature',3,'ATK+3+シールド4','dryas','strengthen',2),
  c('dryas_8','蒼樹解放',4,'special','nature',9,'9ダメ+ATK+2 (3T)','dryas','strengthen',3),

  // ── hades ─────────────────────────────────────────────────
  c('hades_1','死霊撃',1,'attack','dark',3,'3ダメージ','hades'),
  c('hades_2','冥府斬',2,'attack','dark',5,'5ダメージ','hades'),
  c('hades_3','死の波動',3,'attack','dark',8,'8ダメ+呪い2T','hades','curse',2),
  c('hades_4','死盾',1,'defense','dark',4,'シールド4','hades'),
  c('hades_5','冥界壁',2,'defense','dark',7,'シールド7','hades'),
  c('hades_6','死の呪縛',2,'skill','dark',0,'相手に呪い3T','hades','curse',3),
  c('hades_7','冥王弱化',3,'skill','dark',3,'3ダメ+相手ATK-2 (3T)','hades','weaken',3),
  c('hades_8','冥王解放',4,'special','dark',12,'12ダメ+呪い3T','hades','curse',3),

  // ── morgan ────────────────────────────────────────────────
  c('morgan_1','闇拳',1,'attack','dark',3,'3ダメージ','morgan'),
  c('morgan_2','魔術撃',2,'attack','dark',5,'5ダメージ','morgan'),
  c('morgan_3','暗黒波',3,'attack','dark',7,'7ダメージ','morgan'),
  c('morgan_4','闇盾',1,'defense','dark',4,'シールド4','morgan'),
  c('morgan_5','魔術壁',2,'defense','dark',6,'シールド6','morgan'),
  c('morgan_6','闇強化',2,'skill','dark',2,'ATK+2 (2T)','morgan','strengthen',2),
  c('morgan_7','魔力増幅',3,'skill','dark',0,'エネルギー+4','morgan'),
  c('morgan_8','暗黒解放',4,'special','dark',11,'11ダメ+ATK+2 (3T)','morgan','strengthen',3),

  // ── azrael ────────────────────────────────────────────────
  c('azrael_1','死斬',1,'attack','dark',4,'4ダメージ','azrael'),
  c('azrael_2','死神の刃',2,'attack','dark',6,'6ダメージ','azrael'),
  c('azrael_3','死の大鎌',3,'attack','dark',9,'9ダメ+呪い1T','azrael','curse',1),
  c('azrael_4','死盾',1,'defense','dark',3,'シールド3','azrael'),
  c('azrael_5','冥界鎧',2,'defense','dark',6,'シールド6','azrael'),
  c('azrael_6','死の弱化',2,'skill','dark',3,'3ダメ+相手DEF-1 (2T)','azrael','weaken',2),
  c('azrael_7','魂断ち',3,'skill','dark',7,'7ダメ+呪い2T','azrael','curse',2),
  c('azrael_8','死神解放',4,'special','dark',13,'13ダメ+呪い3T','azrael','curse',3),

  // ── kage ─────────────────────────────────────────────────
  c('kage_1','影斬',1,'attack','dark',4,'4ダメージ','kage'),
  c('kage_2','暗殺刃',2,'attack','dark',6,'6ダメージ','kage'),
  c('kage_3','影分裂',3,'attack','dark',8,'8ダメージ','kage'),
  c('kage_4','影盾',1,'defense','dark',3,'シールド3','kage'),
  c('kage_5','暗影壁',2,'defense','dark',5,'シールド5','kage'),
  c('kage_6','影隠れ',2,'skill','dark',0,'次の攻撃を回避','kage','dodge',1),
  c('kage_7','影分身',3,'skill','dark',4,'4ダメ+回避1回','kage','dodge',1),
  c('kage_8','影解放',4,'special','dark',11,'11ダメ+回避1回','kage','dodge',1),

  // ── jeanne ────────────────────────────────────────────────
  c('jeanne_1','聖拳',1,'attack','light',2,'2ダメージ','jeanne'),
  c('jeanne_2','聖剣撃',2,'attack','light',4,'4ダメージ','jeanne'),
  c('jeanne_3','聖光波',3,'attack','light',6,'6ダメージ','jeanne'),
  c('jeanne_4','聖盾',1,'defense','light',5,'シールド5','jeanne'),
  c('jeanne_5','聖壁',2,'defense','light',8,'シールド8','jeanne'),
  c('jeanne_6','祝福',2,'skill','light',5,'HP5回復','jeanne'),
  c('jeanne_7','聖なる加護',3,'skill','light',7,'HP7回復+シールド5','jeanne'),
  c('jeanne_8','聖女解放',4,'special','light',6,'6ダメ+HP8回復+シールド6','jeanne'),

  // ── ra ────────────────────────────────────────────────────
  c('ra_1','太陽拳',1,'attack','light',3,'3ダメージ','ra'),
  c('ra_2','光炎斬',2,'attack','light',5,'5ダメ+バーン1T','ra','burn',1),
  c('ra_3','太陽炎',3,'attack','light',7,'7ダメ+バーン2T','ra','burn',2),
  c('ra_4','光盾',1,'defense','light',4,'シールド4','ra'),
  c('ra_5','光鎧',2,'defense','light',7,'シールド7','ra'),
  c('ra_6','太陽加速',2,'skill','light',2,'ATK+2 (2T)','ra','strengthen',2),
  c('ra_7','灼熱炎',3,'skill','light',5,'5ダメ+バーン3T','ra','burn',3),
  c('ra_8','光神解放',4,'special','light',12,'12ダメ+バーン3T','ra','burn',3),

  // ── michael ───────────────────────────────────────────────
  c('michael_1','天剣',1,'attack','light',3,'3ダメージ','michael'),
  c('michael_2','聖剣斬',2,'attack','light',5,'5ダメージ','michael'),
  c('michael_3','天使の剣',3,'attack','light',8,'8ダメージ','michael'),
  c('michael_4','天盾',1,'defense','light',5,'シールド5','michael'),
  c('michael_5','天使の鎧',2,'defense','light',8,'シールド8','michael'),
  c('michael_6','天の加護',2,'skill','light',5,'HP5回復+ATK+1 (2T)','michael','strengthen',2),
  c('michael_7','聖なる守護',3,'skill','light',0,'シールド10+DEF+1 (2T)','michael'),
  c('michael_8','天使解放',4,'special','light',13,'13ダメ（シールド無視）','michael'),

  // ── byakko ────────────────────────────────────────────────
  c('byakko_1','白爪',1,'attack','light',3,'3ダメージ','byakko'),
  c('byakko_2','虎撃',2,'attack','light',5,'5ダメージ','byakko'),
  c('byakko_3','白虎爪',3,'attack','light',7,'7ダメージ','byakko'),
  c('byakko_4','白虎盾',1,'defense','light',5,'シールド5','byakko'),
  c('byakko_5','守護壁',2,'defense','light',8,'シールド8','byakko'),
  c('byakko_6','白虎守護',2,'skill','light',0,'被ダメ半減 (2T)','byakko','halfDmg',2),
  c('byakko_7','白虎強化',3,'skill','light',3,'3ダメ+シールド5','byakko'),
  c('byakko_8','白虎解放',4,'special','light',10,'10ダメ+シールド8','byakko'),

  // ── fujin ─────────────────────────────────────────────────
  c('fujin_1','風撃',1,'attack','wind',3,'3ダメージ','fujin'),
  c('fujin_2','風刃',2,'attack','wind',5,'5ダメージ','fujin'),
  c('fujin_3','暴風撃',3,'attack','wind',8,'8ダメージ','fujin'),
  c('fujin_4','風盾',1,'defense','wind',3,'シールド3','fujin'),
  c('fujin_5','風壁',2,'defense','wind',5,'シールド5','fujin'),
  c('fujin_6','風加速',2,'skill','wind',0,'エネルギー+3','fujin'),
  c('fujin_7','旋風乱舞',3,'skill','wind',4,'4ダメ×2回','fujin'),
  c('fujin_8','風神解放',4,'special','wind',12,'12ダメ+エネルギー+2','fujin'),

  // ── sylph ─────────────────────────────────────────────────
  c('sylph_1','風爪',1,'attack','wind',3,'3ダメージ','sylph'),
  c('sylph_2','旋風斬',2,'attack','wind',5,'5ダメージ','sylph'),
  c('sylph_3','風の刃',3,'attack','wind',7,'7ダメージ','sylph'),
  c('sylph_4','風盾',1,'defense','wind',3,'シールド3','sylph'),
  c('sylph_5','旋風壁',2,'defense','wind',5,'シールド5+ATK+1 (1T)','sylph','strengthen',1),
  c('sylph_6','風加速',2,'skill','wind',0,'カード2枚ドロー+エネルギー+1','sylph'),
  c('sylph_7','精霊の風',3,'skill','wind',3,'3ダメ+ATK+2 (2T)','sylph','strengthen',2),
  c('sylph_8','旋風解放',4,'special','wind',11,'11ダメ+ATK+2 (2T)','sylph','strengthen',2),

  // ── hurricane ─────────────────────────────────────────────
  c('hurricane_1','嵐撃',1,'attack','wind',4,'4ダメージ','hurricane'),
  c('hurricane_2','台風刃',2,'attack','wind',6,'6ダメージ','hurricane'),
  c('hurricane_3','大嵐波',3,'attack','wind',9,'9ダメージ','hurricane'),
  c('hurricane_4','嵐盾',1,'defense','wind',3,'シールド3','hurricane'),
  c('hurricane_5','嵐壁',2,'defense','wind',5,'シールド5','hurricane'),
  c('hurricane_6','嵐加速',2,'skill','wind',0,'エネルギー+3','hurricane'),
  c('hurricane_7','嵐連撃',3,'skill','wind',5,'5ダメ+次の攻撃+3','hurricane'),
  c('hurricane_8','嵐解放',4,'special','wind',13,'13ダメージ','hurricane'),

  // ── mercurius ─────────────────────────────────────────────
  c('mercurius_1','疾風拳',1,'attack','wind',4,'4ダメージ','mercurius'),
  c('mercurius_2','速斬',2,'attack','wind',6,'6ダメージ','mercurius'),
  c('mercurius_3','神速撃',3,'attack','wind',8,'8ダメージ','mercurius'),
  c('mercurius_4','風盾',1,'defense','wind',3,'シールド3','mercurius'),
  c('mercurius_5','速風壁',2,'defense','wind',5,'シールド5','mercurius'),
  c('mercurius_6','速度加速',2,'skill','wind',0,'エネルギー+4','mercurius'),
  c('mercurius_7','疾風乱舞',3,'skill','wind',3,'3ダメ+エネルギー+2','mercurius'),
  c('mercurius_8','疾風解放',4,'special','wind',11,'11ダメ+エネルギー+3','mercurius'),

  // ── golem ─────────────────────────────────────────────────
  c('golem_1','岩拳',1,'attack','earth',2,'2ダメージ','golem'),
  c('golem_2','岩石撃',2,'attack','earth',3,'3ダメージ','golem'),
  c('golem_3','大岩砕',3,'attack','earth',5,'5ダメージ','golem'),
  c('golem_4','岩石盾',1,'defense','earth',7,'シールド7','golem'),
  c('golem_5','巨石壁',2,'defense','earth',11,'シールド11','golem'),
  c('golem_6','岩石強化',2,'skill','earth',0,'DEF+2 (3T)+シールド5','golem'),
  c('golem_7','岩石守護',3,'skill','earth',0,'シールド12+DEF+2 (2T)','golem'),
  c('golem_8','岩石解放',4,'special','earth',7,'7ダメ+シールド10','golem'),

  // ── atlas ─────────────────────────────────────────────────
  c('atlas_1','大地拳',1,'attack','earth',2,'2ダメージ','atlas'),
  c('atlas_2','天空撃',2,'attack','earth',4,'4ダメージ','atlas'),
  c('atlas_3','天地砕',3,'attack','earth',6,'6ダメージ','atlas'),
  c('atlas_4','大地盾',1,'defense','earth',6,'シールド6','atlas'),
  c('atlas_5','天地壁',2,'defense','earth',10,'シールド10','atlas'),
  c('atlas_6','大地守護',2,'skill','earth',0,'シールド8+DEF+1 (3T)','atlas'),
  c('atlas_7','天地強化',3,'skill','earth',4,'4ダメ+シールド6','atlas'),
  c('atlas_8','大地解放',4,'special','earth',8,'8ダメ+シールド10','atlas'),

  // ── anubis ────────────────────────────────────────────────
  c('anubis_1','砂嵐撃',1,'attack','earth',3,'3ダメージ','anubis'),
  c('anubis_2','砂刃',2,'attack','earth',5,'5ダメージ','anubis'),
  c('anubis_3','砂漠嵐',3,'attack','earth',7,'7ダメージ','anubis'),
  c('anubis_4','砂盾',1,'defense','earth',4,'シールド4','anubis'),
  c('anubis_5','砂壁',2,'defense','earth',7,'シールド7','anubis'),
  c('anubis_6','天秤弱化',2,'skill','earth',3,'3ダメ+相手ATK-2 (2T)','anubis','weaken',2),
  c('anubis_7','砂漠の呪',3,'skill','earth',4,'4ダメ+呪い2T','anubis','curse',2),
  c('anubis_8','審判解放',4,'special','earth',11,'11ダメ+相手ATK-3 (3T)','anubis','weaken',3),

  // ── titan ─────────────────────────────────────────────────
  c('titan_1','巨人拳',1,'attack','earth',2,'2ダメージ','titan'),
  c('titan_2','大岩撃',2,'attack','earth',4,'4ダメージ','titan'),
  c('titan_3','山砕',3,'attack','earth',6,'6ダメージ','titan'),
  c('titan_4','岩鎧',1,'defense','earth',5,'シールド5','titan'),
  c('titan_5','巨岩壁',2,'defense','earth',9,'シールド9','titan'),
  c('titan_6','反撃態勢',2,'skill','earth',0,'カウンター状態(次ダメ×2返し)','titan','counter',1),
  c('titan_7','岩石守護',3,'skill','earth',0,'シールド10+カウンター1回','titan','counter',1),
  c('titan_8','大岩解放',4,'special','earth',10,'10ダメ+シールド8','titan'),
];

export const CARD_MAP = new Map(ALL_CARDS.map(c => [c.id, c]));
