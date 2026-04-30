#include "sv_defs.h"

/* ── Convenience accessors ───────────────────────────────────── */
Player *active_pl(GameState *g) { return &g->players[g->active]; }
Player *opp_pl(GameState *g)    { return &g->players[1 - g->active]; }

int has_ward(const Player *p) {
    for (int i = 0; i < p->board_size; i++)
        if (p->board[i].abilities & AB_WARD) return 1;
    return 0;
}

/* ── Deck helpers ────────────────────────────────────────────── */
void deck_shuffle(Player *p) {
    for (int i = p->deck_size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        const CardDef *tmp = p->deck[i];
        p->deck[i]  = p->deck[j];
        p->deck[j]  = tmp;
    }
}

void player_draw(Player *p, int n) {
    for (int i = 0; i < n; i++) {
        if (p->deck_size == 0) {
            printf("  %s has no cards left to draw!\n", p->name);
            continue;
        }
        if (p->hand_size >= MAX_HAND) {
            printf("  %s's hand is full! Card burned.\n", p->name);
            p->deck_size--;
            continue;
        }
        p->hand[p->hand_size++] = p->deck[--p->deck_size];
    }
}

/* ── Damage helpers ──────────────────────────────────────────── */
void dmg_leader(GameState *g, int pi, int dmg) {
    g->players[pi].hp -= dmg;
    if (g->players[pi].hp < 0) g->players[pi].hp = 0;
    game_check_winner(g);
}

/* is_bane: treat as Bane hit (destroy even if DEF > 0 after damage) */
void dmg_follower(GameState *g, int pi, int bi, int dmg, int is_bane) {
    if (bi < 0 || bi >= g->players[pi].board_size) return;
    Follower *f = &g->players[pi].board[bi];
    f->def -= dmg;
    if (is_bane && dmg > 0) f->def = 0;
    if (f->def <= 0) {
        printf("  %s's %s is destroyed!\n",
               g->players[pi].name, f->card->name);
        remove_follower(&g->players[pi], bi);
    }
}

void remove_follower(Player *p, int i) {
    for (int j = i; j < p->board_size - 1; j++)
        p->board[j] = p->board[j + 1];
    p->board_size--;
}

/* ── Game lifecycle ──────────────────────────────────────────── */
void game_init(GameState *g,
               const char *p1, const char *p2,
               const int *deck1, int d1,
               const int *deck2, int d2) {
    memset(g, 0, sizeof(*g));
    g->winner = -1;
    g->active = 0;
    g->turn   = 0;
    srand((unsigned)time(NULL));

    for (int pi = 0; pi < 2; pi++) {
        Player *p = &g->players[pi];
        strncpy(p->name, pi == 0 ? p1 : p2, (int)sizeof(p->name) - 1);
        p->hp         = LEADER_HP;
        p->pp         = 0;
        p->pp_max     = 0;
        p->evolve_pts = (pi == 0) ? 2 : 3;  /* P2 gets 1 extra evolve */
        p->hand_size  = 0;
        p->board_size = 0;
        p->deck_size  = 0;

        const int *deck = (pi == 0) ? deck1 : deck2;
        int   dsz  = (pi == 0) ? d1    : d2;
        if (dsz > MAX_DECK) dsz = MAX_DECK;
        for (int i = 0; i < dsz; i++) {
            const CardDef *c = card_by_id(deck[i]);
            if (c) p->deck[p->deck_size++] = c;
        }
        deck_shuffle(p);
    }

    /* Both players draw 3 opening cards */
    player_draw(&g->players[0], 3);
    player_draw(&g->players[1], 3);
}

void game_start_turn(GameState *g) {
    g->turn++;
    Player *p = active_pl(g);

    /* Gain 1 PP (max 10) */
    if (p->pp_max < MAX_PP) p->pp_max++;
    p->pp = p->pp_max;

    /* Refresh all existing followers to full-attack */
    for (int i = 0; i < p->board_size; i++)
        p->board[i].can_attack = ATK_FULL;

    /* Draw 1 card */
    player_draw(p, 1);

    printf("\n╔══════════════════════════════════════════╗\n");
    printf("║  Turn %-3d  │  %s's turn\n", g->turn, p->name);
    printf("╚══════════════════════════════════════════╝\n");
    printf("  HP: %d/%d  │  PP: %d/%d  │  Evolve: %d\n",
           p->hp, LEADER_HP, p->pp, p->pp_max, p->evolve_pts);
}

void game_end_turn(GameState *g) {
    if (g->winner >= 0) return;
    g->active = 1 - g->active;
}

int game_check_winner(GameState *g) {
    if (g->winner >= 0) return g->winner;
    for (int i = 0; i < 2; i++) {
        if (g->players[i].hp <= 0) {
            g->winner = 1 - i;
            return g->winner;
        }
    }
    return -1;
}

/* ── Play card ───────────────────────────────────────────────── */
int game_play_card(GameState *g, int hand_i, int tgt_p, int tgt_i) {
    Player *p = active_pl(g);

    if (hand_i < 0 || hand_i >= p->hand_size) return -1;
    const CardDef *card = p->hand[hand_i];

    if (p->pp < card->cost) return -1;
    if (card->type == CT_FOLLOWER && p->board_size >= MAX_BOARD) return -1;

    /* Validate spell targets */
    if (card->type == CT_SPELL) {
        switch (card->target_type) {
        case TGT_FOLLOWER:
            if (tgt_i < 0 || tgt_i >= g->players[tgt_p].board_size) return -1;
            break;
        case TGT_ANY:
            if (tgt_i >= 0 && tgt_i >= g->players[tgt_p].board_size) return -1;
            break;
        case TGT_FRIENDLY:
            if (tgt_p != g->active) return -1;
            if (tgt_i < 0 || tgt_i >= p->board_size) return -1;
            break;
        case TGT_NONE:
            break;
        }
    }

    /* Deduct PP and remove from hand */
    p->pp -= card->cost;
    for (int i = hand_i; i < p->hand_size - 1; i++)
        p->hand[i] = p->hand[i + 1];
    p->hand_size--;

    printf("\n  %s plays [%s]", p->name, card->name);

    if (card->type == CT_FOLLOWER) {
        Follower f;
        memset(&f, 0, sizeof(f));
        f.card      = card;
        f.atk       = card->atk;
        f.def       = card->def;
        f.abilities = card->abilities;
        f.evolved   = 0;
        /* Determine initial attack readiness */
        if      (card->abilities & AB_STORM) f.can_attack = ATK_FULL;
        else if (card->abilities & AB_RUSH)  f.can_attack = ATK_RUSH;
        else                                 f.can_attack = ATK_NO;

        p->board[p->board_size++] = f;
        printf(" (%d/%d)", card->atk, card->def);
        print_abilities(card->abilities);
        printf("\n");
    } else {
        printf("\n");
        if (card->on_play)
            card->on_play(g, g->active, tgt_p, tgt_i);
    }

    game_check_winner(g);
    return 0;
}

/* ── Attack ──────────────────────────────────────────────────── */
int game_attack(GameState *g, int atk_i, int tgt_p, int tgt_i) {
    Player *ap = active_pl(g);
    int opp_idx = 1 - g->active;

    if (atk_i < 0 || atk_i >= ap->board_size)    return -1;
    Follower *atk = &ap->board[atk_i];
    if (atk->can_attack == ATK_NO)               return -1;
    if (tgt_p != opp_idx)                        return -1;  /* Must target opponent */
    if (tgt_i == -1 && atk->can_attack < ATK_FULL) return -1; /* Rush: followers only */

    Player *dp = &g->players[tgt_p];

    /* Ward enforcement */
    if (tgt_i == -1 && has_ward(dp)) return -1;
    if (tgt_i >= 0 && has_ward(dp) && !(dp->board[tgt_i].abilities & AB_WARD)) return -1;

    if (tgt_i >= dp->board_size) return -1;

    atk->can_attack = ATK_NO;  /* Used this turn */

    if (tgt_i < 0) {
        /* ── Attack leader ── */
        printf("\n  %s's [%s] attacks %s's leader for %d!\n",
               ap->name, atk->card->name, dp->name, atk->atk);
        if (atk->abilities & AB_DRAIN) {
            int heal = atk->atk;
            ap->hp += heal;
            if (ap->hp > LEADER_HP) ap->hp = LEADER_HP;
            printf("  Drain: %s restores %d HP. (HP: %d)\n",
                   ap->name, heal, ap->hp);
        }
        dmg_leader(g, tgt_p, atk->atk);
    } else {
        /* ── Attack follower ── */
        Follower *def = &dp->board[tgt_i];
        printf("\n  %s's [%s] (%d/%d) attacks %s's [%s] (%d/%d)!\n",
               ap->name, atk->card->name, atk->atk, atk->def,
               dp->name, def->card->name, def->atk, def->def);

        int atk_dmg = atk->atk;
        int def_dmg = def->atk;
        int atk_bane = (atk->abilities & AB_BANE) ? 1 : 0;
        int def_bane = (def->abilities & AB_BANE) ? 1 : 0;

        /* Apply damage to both sides */
        def->def -= atk_dmg;
        atk->def -= def_dmg;

        /* Bane: destroy any follower it damages */
        if (atk_bane && atk_dmg > 0) def->def = 0;
        if (def_bane && def_dmg > 0) atk->def = 0;

        /* Drain: heal leader by damage dealt */
        if (atk->abilities & AB_DRAIN) {
            int heal = atk_dmg;
            ap->hp += heal;
            if (ap->hp > LEADER_HP) ap->hp = LEADER_HP;
            printf("  Drain: %s restores %d HP. (HP: %d)\n",
                   ap->name, heal, ap->hp);
        }

        /* Check deaths (check defender first: it doesn't shift attacker's index) */
        if (def->def <= 0) {
            printf("  %s's [%s] is destroyed!\n", dp->name, def->card->name);
            remove_follower(dp, tgt_i);
        }
        /* Re-check attacker pointer (still valid: different board) */
        if (atk->def <= 0) {
            printf("  %s's [%s] is destroyed!\n", ap->name, atk->card->name);
            remove_follower(ap, atk_i);
        }
    }

    game_check_winner(g);
    return 0;
}

/* ── Evolve ──────────────────────────────────────────────────── */
int game_evolve(GameState *g, int board_i) {
    Player *p = active_pl(g);

    if (p->evolve_pts <= 0)                         return -1;
    if (board_i < 0 || board_i >= p->board_size)    return -1;

    Follower *f = &p->board[board_i];
    if (f->evolved) return -1;

    f->atk += 2;
    f->def += 2;
    f->evolved = 1;
    p->evolve_pts--;

    /* Evolution grants Rush (attack followers immediately) */
    if (f->abilities & AB_STORM) {
        f->can_attack = ATK_FULL;
    } else {
        f->abilities |= AB_RUSH;
        f->can_attack = ATK_RUSH;
    }

    printf("\n  %s evolves [%s]! Now %d/%d",
           p->name, f->card->name, f->atk, f->def);
    print_abilities(f->abilities);
    printf("  (Evolve pts remaining: %d)\n", p->evolve_pts);
    return 0;
}
