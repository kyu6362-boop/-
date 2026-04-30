#ifndef SV_DEFS_H
#define SV_DEFS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ── Game constants ──────────────────────────────────────────── */
#define MAX_HAND    9
#define MAX_BOARD   5
#define MAX_DECK    40
#define LEADER_HP   20
#define MAX_PP      10

/* ── Ability bitmask ─────────────────────────────────────────── */
#define AB_RUSH   (1u << 0)  /* Attack followers on play turn    */
#define AB_STORM  (1u << 1)  /* Attack anything on play turn     */
#define AB_WARD   (1u << 2)  /* Must be targeted before others   */
#define AB_DRAIN  (1u << 3)  /* Heals leader by damage dealt     */
#define AB_BANE   (1u << 4)  /* Destroys any follower it damages */

/* ── can_attack values ───────────────────────────────────────── */
#define ATK_NO      0  /* Summoning sickness                    */
#define ATK_RUSH    1  /* Can attack followers only (Rush mode) */
#define ATK_FULL    2  /* Can attack anything                   */

/* ── Spell target type ───────────────────────────────────────── */
typedef enum {
    TGT_NONE,      /* No target required (e.g. AoE, self-heal) */
    TGT_ANY,       /* Any character (follower or leader)        */
    TGT_FOLLOWER,  /* Any follower                              */
    TGT_FRIENDLY   /* A friendly follower                       */
} TargetType;

typedef enum { CT_FOLLOWER, CT_SPELL } CardType;

/* Forward declaration needed by on_play callbacks */
typedef struct GameState GameState;

/* ── Static card definition (lives in CARD_DB array) ─────────── */
typedef struct {
    int        id;
    const char *name;
    CardType   type;
    int        cost;
    int        atk;       /* base ATK (followers only) */
    int        def;       /* base DEF (followers only) */
    unsigned   abilities;
    TargetType target_type;
    const char *desc;
    void (*on_play)(GameState *g, int player, int tgt_p, int tgt_i);
} CardDef;

/* ── Follower on the board ───────────────────────────────────── */
typedef struct {
    const CardDef *card;
    int      atk;
    int      def;
    unsigned abilities;
    int      can_attack;  /* ATK_NO / ATK_RUSH / ATK_FULL */
    int      evolved;
} Follower;

/* ── Player state ────────────────────────────────────────────── */
typedef struct {
    char     name[32];
    int      hp;
    int      pp;
    int      pp_max;
    int      evolve_pts;
    const CardDef *hand[MAX_HAND];
    int      hand_size;
    const CardDef *deck[MAX_DECK];
    int      deck_size;
    Follower board[MAX_BOARD];
    int      board_size;
} Player;

/* ── Full game state ─────────────────────────────────────────── */
struct GameState {
    Player players[2];
    int    active;   /* 0 or 1: whose turn */
    int    turn;     /* overall turn counter */
    int    winner;   /* -1 = ongoing, 0/1 = winner index */
};

/* ── Card database (defined in sv_cards.c) ───────────────────── */
extern const CardDef CARD_DB[];
extern const int     CARD_DB_SIZE;
extern const int     DEFAULT_DECK[MAX_DECK];

const CardDef *card_by_id(int id);

/* ── Game lifecycle ──────────────────────────────────────────── */
void game_init(GameState *g, const char *p1, const char *p2,
               const int *deck1, int d1, const int *deck2, int d2);
void game_start_turn(GameState *g);
void game_end_turn(GameState *g);
int  game_check_winner(GameState *g);

/* ── Player actions (return 0=ok, -1=invalid) ────────────────── */
int  game_play_card(GameState *g, int hand_i, int tgt_p, int tgt_i);
int  game_attack(GameState *g, int atk_i, int tgt_p, int tgt_i);
int  game_evolve(GameState *g, int board_i);

/* ── Internal helpers (also used by UI / AI) ─────────────────── */
Player *active_pl(GameState *g);
Player *opp_pl(GameState *g);
int     has_ward(const Player *p);
void    remove_follower(Player *p, int i);
void    dmg_leader(GameState *g, int pi, int dmg);
void    dmg_follower(GameState *g, int pi, int bi, int dmg, int is_bane);
void    player_draw(Player *p, int n);
void    deck_shuffle(Player *p);

/* ── UI helpers (defined in sv_main.c) ───────────────────────── */
void print_abilities(unsigned ab);
void print_board(const Player *p, const char *label);
void print_hand(const Player *p);
void print_game_state(const GameState *g);

/* ── AI (defined in sv_main.c) ───────────────────────────────── */
void ai_turn(GameState *g);

#endif /* SV_DEFS_H */
