#include "sv_defs.h"

/* ════════════════════════════════════════════════════════════════
   Display helpers
   ════════════════════════════════════════════════════════════════ */

void print_abilities(unsigned ab) {
    if (!ab) return;
    if (ab & AB_RUSH)  printf(" [Rush]");
    if (ab & AB_STORM) printf(" [Storm]");
    if (ab & AB_WARD)  printf(" [Ward]");
    if (ab & AB_DRAIN) printf(" [Drain]");
    if (ab & AB_BANE)  printf(" [Bane]");
}

void print_board(const Player *p, const char *label) {
    printf("  %s board (%d/%d):\n", label, p->board_size, MAX_BOARD);
    if (p->board_size == 0) {
        printf("    (empty)\n");
        return;
    }
    for (int i = 0; i < p->board_size; i++) {
        const Follower *f = &p->board[i];
        const char *atk_str =
            f->can_attack == ATK_FULL ? "ready" :
            f->can_attack == ATK_RUSH ? "rush"  : "sick";
        printf("    [%d] %s (%d/%d)%s%s\n",
               i, f->card->name, f->atk, f->def,
               f->evolved ? " *evolved*" : "",
               f->can_attack ? "" : " (summoning sickness)");
        if (f->abilities) { printf("        "); print_abilities(f->abilities); printf("\n"); }
        (void)atk_str;
    }
}

void print_hand(const Player *p) {
    printf("  Hand (%d/%d):\n", p->hand_size, MAX_HAND);
    for (int i = 0; i < p->hand_size; i++) {
        const CardDef *c = p->hand[i];
        if (c->type == CT_FOLLOWER) {
            printf("    [%d] %s  cost:%d  %d/%d",
                   i, c->name, c->cost, c->atk, c->def);
            print_abilities(c->abilities);
        } else {
            printf("    [%d] %s  cost:%d  (Spell) %s",
                   i, c->name, c->cost, c->desc);
        }
        printf("\n");
    }
}

void print_card_db(void) {
    printf("\n=== All Available Cards ===\n");
    for (int i = 0; i < CARD_DB_SIZE; i++) {
        const CardDef *c = &CARD_DB[i];
        if (c->type == CT_FOLLOWER)
            printf("  [%2d] %-22s  cost:%-2d  %d/%d", c->id, c->name, c->cost, c->atk, c->def);
        else
            printf("  [%2d] %-22s  cost:%-2d  (Spell)", c->id, c->name, c->cost);
        print_abilities(c->abilities);
        printf("  - %s\n", c->desc);
    }
}

void print_game_state(const GameState *g) {
    const Player *opp = &g->players[1 - g->active];
    const Player *me  = &g->players[g->active];

    printf("\n──────────────────────────────────────────────\n");
    printf("  %s  HP: %d/%d  PP: %d/%d  Evolve: %d  Deck: %d\n",
           opp->name, opp->hp, LEADER_HP,
           opp->pp, opp->pp_max, opp->evolve_pts, opp->deck_size);
    printf("  Hand: %d cards\n", opp->hand_size);
    print_board(opp, "Opponent");
    printf("  ──────────────────────────────────────────\n");
    print_board(me, "Your");
    print_hand(me);
    printf("  HP: %d/%d  PP: %d/%d  Evolve: %d  Deck: %d\n",
           me->hp, LEADER_HP, me->pp, me->pp_max, me->evolve_pts, me->deck_size);
    printf("──────────────────────────────────────────────\n");
}

static void print_help(void) {
    printf("\nCommands:\n");
    printf("  p <hand_idx> [tgt_p tgt_i]  Play card (tgt_p=0/1, tgt_i=-1=leader)\n");
    printf("  a <my_idx> <tgt_p> <tgt_i>  Attack (tgt_p=opponent, tgt_i=-1=leader)\n");
    printf("  e <my_idx>                   Evolve follower\n");
    printf("  end                          End your turn\n");
    printf("  s                            Show game state\n");
    printf("  cards                        List all cards\n");
    printf("  help                         Show this help\n");
    printf("\nNotes:\n");
    printf("  tgt_p: 0=%s, 1=%s\n", "Player 1", "Player 2");
    printf("  Ward followers must be attacked before others or the leader.\n");
    printf("  Rush followers can only attack enemy followers on their first turn.\n");
}

/* ════════════════════════════════════════════════════════════════
   AI
   ════════════════════════════════════════════════════════════════ */

void ai_turn(GameState *g) {
    int ai_idx  = g->active;
    int opp_idx = 1 - ai_idx;
    Player *ai  = &g->players[ai_idx];
    Player *opp = &g->players[opp_idx];

    printf("\n  [AI %s is thinking...]\n", ai->name);

    /* ── Play cards (greedy: highest cost first) ── */
    int changed = 1;
    while (changed && g->winner < 0) {
        changed = 0;
        int best_i = -1, best_cost = -1;

        for (int i = 0; i < ai->hand_size; i++) {
            const CardDef *c = ai->hand[i];
            if (c->cost > ai->pp) continue;
            if (c->type == CT_FOLLOWER && ai->board_size >= MAX_BOARD) continue;
            /* Skip spells that need a follower target if no valid target */
            if (c->type == CT_SPELL) {
                if (c->target_type == TGT_FOLLOWER && opp->board_size == 0) continue;
                if (c->target_type == TGT_FRIENDLY && ai->board_size == 0) continue;
            }
            if (c->cost > best_cost) { best_cost = c->cost; best_i = i; }
        }

        if (best_i < 0) break;

        const CardDef *c = ai->hand[best_i];
        int tgt_p = -1, tgt_i = -1;

        if (c->type == CT_SPELL) {
            switch (c->target_type) {
            case TGT_NONE:
                tgt_p = ai_idx; tgt_i = -1; break;
            case TGT_ANY:
                tgt_p = opp_idx;
                tgt_i = (opp->board_size > 0) ? 0 : -1;
                break;
            case TGT_FOLLOWER:
                tgt_p = opp_idx;
                /* Pick the follower with highest DEF that we can kill, else first */
                tgt_i = 0;
                break;
            case TGT_FRIENDLY:
                tgt_p = ai_idx;
                tgt_i = 0;
                break;
            }
        }

        if (game_play_card(g, best_i, tgt_p, tgt_i) == 0) changed = 1;
    }

    if (g->winner >= 0) return;

    /* ── Attack with all followers ── */
    int loop = 1;
    while (loop && g->winner < 0) {
        loop = 0;
        for (int i = 0; i < ai->board_size; i++) {
            Follower *f = &ai->board[i];
            if (f->can_attack == ATK_NO) continue;

            /* Find Ward follower on opponent's side */
            int ward_i = -1;
            for (int j = 0; j < opp->board_size; j++) {
                if (opp->board[j].abilities & AB_WARD) { ward_i = j; break; }
            }

            int tgt_i;
            if (ward_i >= 0) {
                tgt_i = ward_i;                       /* Must attack Ward */
            } else if (f->can_attack < ATK_FULL && opp->board_size > 0) {
                tgt_i = 0;                            /* Rush: must hit follower */
            } else if (f->can_attack < ATK_FULL) {
                continue;                             /* Rush, nothing to hit */
            } else if (opp->board_size == 0) {
                tgt_i = -1;                           /* Hit leader */
            } else {
                /* Prefer a follower we can kill; otherwise hit leader */
                tgt_i = -1;
                for (int j = 0; j < opp->board_size; j++) {
                    if (f->atk >= opp->board[j].def) { tgt_i = j; break; }
                }
            }

            if (game_attack(g, i, opp_idx, tgt_i) == 0) {
                loop = 1;
                break; /* Board may have changed; restart loop */
            }
        }
    }
    /* Caller (main loop) handles game_end_turn */
}

/* ════════════════════════════════════════════════════════════════
   Human turn
   ════════════════════════════════════════════════════════════════ */

static int human_turn(GameState *g) {
    char buf[128];
    while (g->winner < 0) {
        printf("\n> ");
        fflush(stdout);
        if (!fgets(buf, sizeof(buf), stdin)) return 0;

        /* Strip newline */
        char *nl = strchr(buf, '\n');
        if (nl) *nl = '\0';

        if (strcmp(buf, "end") == 0) {
            return 1;
        } else if (strcmp(buf, "s") == 0) {
            print_game_state(g);
        } else if (strcmp(buf, "help") == 0) {
            print_help();
        } else if (strcmp(buf, "cards") == 0) {
            print_card_db();
        } else if (buf[0] == 'p') {
            int hand_i, tgt_p = -1, tgt_i = -1;
            int n = sscanf(buf + 1, "%d %d %d", &hand_i, &tgt_p, &tgt_i);
            if (n < 1) { printf("  Usage: p <hand_idx> [tgt_p tgt_i]\n"); continue; }
            if (game_play_card(g, hand_i, tgt_p, tgt_i) < 0)
                printf("  Invalid play.\n");
        } else if (buf[0] == 'a') {
            int atk_i, tp, ti;
            if (sscanf(buf + 1, "%d %d %d", &atk_i, &tp, &ti) < 3) {
                printf("  Usage: a <my_idx> <tgt_p> <tgt_i>  (tgt_i=-1 for leader)\n");
                continue;
            }
            if (game_attack(g, atk_i, tp, ti) < 0)
                printf("  Invalid attack.\n");
        } else if (buf[0] == 'e') {
            int bi;
            if (sscanf(buf + 1, "%d", &bi) < 1) {
                printf("  Usage: e <board_idx>\n"); continue;
            }
            if (game_evolve(g, bi) < 0)
                printf("  Invalid evolve.\n");
        } else if (buf[0] != '\0') {
            printf("  Unknown command. Type 'help' for help.\n");
        }
    }
    return 0;
}

/* ════════════════════════════════════════════════════════════════
   Main
   ════════════════════════════════════════════════════════════════ */

int main(void) {
    printf("╔════════════════════════════════════╗\n");
    printf("║      Shadowverse Battle Game       ║\n");
    printf("╚════════════════════════════════════╝\n\n");

    /* ── Mode selection ── */
    printf("Mode: (1) Player vs Player  (2) Player vs AI\n> ");
    int mode = 1;
    char buf[64];
    if (fgets(buf, sizeof(buf), stdin))
        mode = (buf[0] == '2') ? 2 : 1;

    /* ── Player names ── */
    char p1name[32] = "Player1";
    char p2name[32] = "Player2";
    printf("Player 1 name (Enter = Player1): ");
    if (fgets(buf, sizeof(buf), stdin) && buf[0] != '\n')
        sscanf(buf, "%31s", p1name);
    if (mode == 2) {
        strcpy(p2name, "AI");
    } else {
        printf("Player 2 name (Enter = Player2): ");
        if (fgets(buf, sizeof(buf), stdin) && buf[0] != '\n')
            sscanf(buf, "%31s", p2name);
    }

    /* ── Init game ── */
    GameState g;
    game_init(&g, p1name, p2name,
              DEFAULT_DECK, MAX_DECK,
              DEFAULT_DECK, MAX_DECK);

    printf("\nGame start! Both players draw 3 cards.\n");
    printf("Player 1 (%s) goes first with 2 evolve points.\n", p1name);
    printf("Player 2 (%s) goes second with 3 evolve points.\n", p2name);
    print_help();

    /* ── Main game loop ── */
    while (g.winner < 0) {
        game_start_turn(&g);
        if (g.winner >= 0) break;

        int is_ai = (mode == 2 && g.active == 1);

        if (!is_ai) {
            print_game_state(&g);
            human_turn(&g);
        } else {
            ai_turn(&g);
        }
        if (g.winner < 0) game_end_turn(&g);
    }

    /* ── Game over ── */
    printf("\n╔════════════════════════════════════╗\n");
    printf("║  GAME OVER                         ║\n");
    if (g.winner >= 0)
        printf("║  Winner: %-26s  ║\n", g.players[g.winner].name);
    printf("╚════════════════════════════════════╝\n");
    printf("Final HP:  %s: %d  │  %s: %d\n",
           g.players[0].name, g.players[0].hp,
           g.players[1].name, g.players[1].hp);

    return 0;
}
