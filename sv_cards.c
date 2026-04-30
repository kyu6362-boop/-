#include "sv_defs.h"

/* ── Spell effect forward declarations ───────────────────────── */
static void fx_fireball      (GameState *g, int pl, int tp, int ti);
static void fx_lightning_bolt(GameState *g, int pl, int tp, int ti);
static void fx_healing_prayer(GameState *g, int pl, int tp, int ti);
static void fx_dark_ritual   (GameState *g, int pl, int tp, int ti);
static void fx_summon_spirits(GameState *g, int pl, int tp, int ti);
static void fx_hellfire      (GameState *g, int pl, int tp, int ti);
static void fx_gale_force    (GameState *g, int pl, int tp, int ti);
static void fx_divine_shield (GameState *g, int pl, int tp, int ti);

/* ── Spirit token (used by Summon Spirits) ───────────────────── */
static const CardDef SPIRIT_TOKEN = {
    0, "Spirit", CT_FOLLOWER, 0, 1, 1, 0, TGT_NONE,
    "Summoned spirit token.", NULL
};

/* ── Card database ───────────────────────────────────────────── */
/*   { id, name, type, cost, atk, def, abilities, target, desc, on_play } */
const CardDef CARD_DB[] = {
    /* Followers */
    {  1, "Goblin",            CT_FOLLOWER, 1, 1, 2, 0,                  TGT_NONE,    "1/2 follower.",                              NULL           },
    {  2, "Wind Fairy",        CT_FOLLOWER, 1, 1, 1, AB_STORM,           TGT_NONE,    "1/1 follower with Storm.",                   NULL           },
    {  3, "Bronze Golem",      CT_FOLLOWER, 2, 1, 4, AB_WARD,            TGT_NONE,    "1/4 follower with Ward.",                    NULL           },
    {  4, "Forest Bat",        CT_FOLLOWER, 2, 2, 1, AB_DRAIN,           TGT_NONE,    "2/1 follower with Drain.",                   NULL           },
    {  5, "Saber-Fang Tiger",  CT_FOLLOWER, 3, 3, 2, AB_RUSH,            TGT_NONE,    "3/2 follower with Rush.",                    NULL           },
    {  6, "Bone Knight",       CT_FOLLOWER, 3, 2, 3, 0,                  TGT_NONE,    "2/3 follower.",                              NULL           },
    {  7, "Ironscale Lizard",  CT_FOLLOWER, 3, 2, 5, AB_WARD,            TGT_NONE,    "2/5 follower with Ward.",                    NULL           },
    {  8, "Shadow Reaper",     CT_FOLLOWER, 4, 3, 3, AB_BANE,            TGT_NONE,    "3/3 follower with Bane.",                    NULL           },
    {  9, "Thunder Hawk",      CT_FOLLOWER, 4, 3, 4, AB_RUSH,            TGT_NONE,    "3/4 follower with Rush.",                    NULL           },
    { 10, "Dark Knight",       CT_FOLLOWER, 4, 4, 3, 0,                  TGT_NONE,    "4/3 follower.",                              NULL           },
    { 11, "Ancient Leviathan", CT_FOLLOWER, 5, 4, 5, AB_WARD,            TGT_NONE,    "4/5 follower with Ward.",                    NULL           },
    { 12, "Storm Dragon",      CT_FOLLOWER, 5, 5, 4, AB_RUSH,            TGT_NONE,    "5/4 follower with Rush.",                    NULL           },
    { 13, "Sacred Guardian",   CT_FOLLOWER, 6, 4, 6, AB_WARD|AB_DRAIN,   TGT_NONE,    "4/6 follower with Ward and Drain.",          NULL           },
    { 14, "Death Wraith",      CT_FOLLOWER, 6, 5, 5, AB_BANE,            TGT_NONE,    "5/5 follower with Bane.",                    NULL           },
    { 15, "Divine Dragon",     CT_FOLLOWER, 8, 7, 7, AB_STORM,           TGT_NONE,    "7/7 follower with Storm.",                   NULL           },
    /* Spells */
    { 16, "Fireball",          CT_SPELL,    2, 0, 0, 0, TGT_ANY,         "Deal 3 damage to any target.",                           fx_fireball     },
    { 17, "Lightning Bolt",    CT_SPELL,    1, 0, 0, 0, TGT_FOLLOWER,    "Deal 2 damage to a follower.",                           fx_lightning_bolt},
    { 18, "Healing Prayer",    CT_SPELL,    2, 0, 0, 0, TGT_NONE,        "Restore 4 HP to your leader.",                           fx_healing_prayer},
    { 19, "Dark Ritual",       CT_SPELL,    3, 0, 0, 0, TGT_FOLLOWER,    "Deal 5 damage to a follower.",                           fx_dark_ritual  },
    { 21, "Summon Spirits",    CT_SPELL,    3, 0, 0, 0, TGT_NONE,        "Summon two 1/1 Spirit followers.",                       fx_summon_spirits},
    { 22, "Hellfire",          CT_SPELL,    5, 0, 0, 0, TGT_NONE,        "Deal 3 damage to all enemy followers.",                  fx_hellfire     },
    { 23, "Gale Force",        CT_SPELL,    2, 0, 0, 0, TGT_FRIENDLY,    "Give a friendly follower +2/+0 and Rush.",               fx_gale_force   },
    { 24, "Divine Shield",     CT_SPELL,    3, 0, 0, 0, TGT_FRIENDLY,    "Give a friendly follower +0/+3 and Ward.",               fx_divine_shield },
};

const int CARD_DB_SIZE = (int)(sizeof(CARD_DB) / sizeof(CARD_DB[0]));

/* ── Default deck (40 cards) ─────────────────────────────────── */
const int DEFAULT_DECK[MAX_DECK] = {
     1,  1,       /*  Goblin x2             */
     2,  2,       /*  Wind Fairy x2         */
     3,  3,       /*  Bronze Golem x2       */
     4,  4,       /*  Forest Bat x2         */
     5,  5,       /*  Saber-Fang Tiger x2   */
     6,  6,       /*  Bone Knight x2        */
     7,  7,       /*  Ironscale Lizard x2   */
     8,  8,       /*  Shadow Reaper x2      */
     9,  9,       /*  Thunder Hawk x2       */
    10, 10,       /*  Dark Knight x2        */
    11, 11,       /*  Ancient Leviathan x2  */
    12, 12,       /*  Storm Dragon x2       */
    13,           /*  Sacred Guardian x1    */
    14,           /*  Death Wraith x1       */
    15,           /*  Divine Dragon x1      */
    16, 16, 16,   /*  Fireball x3           */
    17, 17,       /*  Lightning Bolt x2     */
    18, 18,       /*  Healing Prayer x2     */
    19, 19,       /*  Dark Ritual x2        */
    21,           /*  Summon Spirits x1     */
    22,           /*  Hellfire x1           */
    23,           /*  Gale Force x1         */
    24,           /*  Divine Shield x1      */
};

/* ── Card lookup ─────────────────────────────────────────────── */
const CardDef *card_by_id(int id) {
    for (int i = 0; i < CARD_DB_SIZE; i++)
        if (CARD_DB[i].id == id) return &CARD_DB[i];
    return NULL;
}

/* ── Spell effect implementations ────────────────────────────── */

static void fx_fireball(GameState *g, int pl, int tp, int ti) {
    (void)pl;
    if (ti < 0) {
        printf("  [Fireball] Deals 3 damage to %s's leader.\n",
               g->players[tp].name);
        dmg_leader(g, tp, 3);
    } else if (ti < g->players[tp].board_size) {
        printf("  [Fireball] Deals 3 damage to %s.\n",
               g->players[tp].board[ti].card->name);
        dmg_follower(g, tp, ti, 3, 0);
    }
}

static void fx_lightning_bolt(GameState *g, int pl, int tp, int ti) {
    (void)pl;
    if (ti < 0 || ti >= g->players[tp].board_size) return;
    printf("  [Lightning Bolt] Deals 2 damage to %s.\n",
           g->players[tp].board[ti].card->name);
    dmg_follower(g, tp, ti, 2, 0);
}

static void fx_healing_prayer(GameState *g, int pl, int tp, int ti) {
    (void)tp; (void)ti;
    Player *p = &g->players[pl];
    int amount = 4;
    if (p->hp + amount > LEADER_HP) amount = LEADER_HP - p->hp;
    p->hp += amount;
    printf("  [Healing Prayer] %s restores %d HP. (HP: %d)\n",
           p->name, amount, p->hp);
}

static void fx_dark_ritual(GameState *g, int pl, int tp, int ti) {
    (void)pl;
    if (ti < 0 || ti >= g->players[tp].board_size) return;
    printf("  [Dark Ritual] Deals 5 damage to %s.\n",
           g->players[tp].board[ti].card->name);
    dmg_follower(g, tp, ti, 5, 0);
}

static void fx_summon_spirits(GameState *g, int pl, int tp, int ti) {
    (void)tp; (void)ti;
    Player *p = &g->players[pl];
    int summoned = 0;
    for (int i = 0; i < 2 && p->board_size < MAX_BOARD; i++) {
        Follower f;
        memset(&f, 0, sizeof(f));
        f.card       = &SPIRIT_TOKEN;
        f.atk        = 1;
        f.def        = 1;
        f.abilities  = 0;
        f.can_attack = ATK_NO;
        p->board[p->board_size++] = f;
        summoned++;
    }
    printf("  [Summon Spirits] %s summons %d Spirit(s).\n", p->name, summoned);
}

static void fx_hellfire(GameState *g, int pl, int tp, int ti) {
    (void)tp; (void)ti;
    int opp = 1 - pl;
    printf("  [Hellfire] Deals 3 damage to all of %s's followers.\n",
           g->players[opp].name);
    /* Iterate in reverse so removals don't skip indices */
    for (int i = g->players[opp].board_size - 1; i >= 0; i--)
        dmg_follower(g, opp, i, 3, 0);
}

static void fx_gale_force(GameState *g, int pl, int tp, int ti) {
    (void)tp;
    if (ti < 0 || ti >= g->players[pl].board_size) return;
    Follower *f = &g->players[pl].board[ti];
    f->atk += 2;
    f->abilities |= AB_RUSH;
    if (f->can_attack == ATK_NO) f->can_attack = ATK_RUSH;
    printf("  [Gale Force] %s gains +2/+0 and Rush.\n", f->card->name);
}

static void fx_divine_shield(GameState *g, int pl, int tp, int ti) {
    (void)tp;
    if (ti < 0 || ti >= g->players[pl].board_size) return;
    Follower *f = &g->players[pl].board[ti];
    f->def += 3;
    f->abilities |= AB_WARD;
    printf("  [Divine Shield] %s gains +0/+3 and Ward.\n", f->card->name);
}
