#ifndef _GAME_
#define _GAME_

#define QUEEN_SPADE_f          1
#define PERFECT_100_f          1 << 1
#define OMNIBUS_f              1 << 2
#define NO_TRICK_BONUS_f       1 << 3
#define NEW_MOON_f             1 << 4
#define NO_DRAW_f              1 << 5

#define NO_ERROR               0
#define ERROR_CARDS_PASSED     1
#define ERROR_ILLEGAL_CARD     2
#define ERROR_CARD_NOT_FOUND   3
#define ERROR_DOUBLE_CARD      4
#define ERROR_NOT_YOUR_TURN    6
#define ERROR_QUEEN_SPADE      7
#define ERROR_BREAK_HEART      8
#define ERROR_WRONG_SUIT       9
#define ERROR_PLAY_TWO_CLUBS   10

#define STATE_SEND_CARDS       1
#define STATE_WAIT_PASS        2
#define STATE_WAIT_PLAY        3
#define STATE_END_TURN         4
#define STATE_END_ROUND        5
#define STATE_GAME_OVER        6

const int no_trick_bonus     = 5;
const int omnibus_bonus      = 10;
const int GAME_OVER_SCORE    = 100;

class cGame {
public:
  cGame( int f );
  ~cGame();

private:
  bool game_started;
  bool game_over;
  bool game_draw;
  bool passing_over;
  bool heart_broken;
  bool shoot_moon;
  bool moon_add;
  bool has_card[4][DECK_SIZE];
  bool played[4];
  int flags;
  usINT state;
  time_t wait_time;
  usINT delay;
  usINT suit;
  usINT passto;
  usINT turn;
  usINT left_to_play;
  usINT num_cards[4];
  usINT num_passed;
  usINT current_score;
  usINT player_cards[4][13];
  usINT passed_cards[4][3];
  usINT cards_in_suit[4][4];
  usINT hand_score[4];
  usINT score[4];
  usINT won_turn;
  usINT who_moon;
  usINT won_jack_diamond;
  usINT best_card;
  usINT cards_played;
  bool jack_diamond;
  char str_cards[4][80];    // 3 * 13 + 13 + 1 = 53 == 3 char for code 127 = empty cards * 13 cards + 13 spaces + 1 null char
  void Generate_Cards();
  void Sort();

public:
  usINT Turn();
  void Start();
  void Wait(usINT d);
  bool WaitOver();
  char *Str_Cards(usINT player);
  usINT Cards(usINT player, usINT card);
  usINT Num_Cards(usINT player);
  usINT State();
  void SetState(usINT s);
  bool Started();
  usINT PassTo();
  void ForcePass(cTable *table, usINT chair);
  void ForcePlay(cTable &table);
  void Pass(cTable &table);
  usINT PlayerPass(cTable &table, usINT chair, usINT card1, usINT card2, usINT card3);
  bool Passed(usINT pid);
  void ResetRound();
  void Run();
  bool MyTurn(usINT chair);
  bool ValidMove(cDescriptor &d, usINT chair, usINT card);
  usINT ValidMove(usINT chair, usINT card);
  void Play(cTable &table, usINT card);
  bool AdvanceTurn(cTable &table);
  bool Played(usINT chair);
  void ResetPlayed();
  void EndRound(cTable &table);
  bool Passing();
  bool Ready();
  void Show(usINT chair);
  void EndTurn(cTable &table);
  void SetMoon(bool add);
  usINT WhoMoon();
  usINT Score(usINT chair);
};
#endif
