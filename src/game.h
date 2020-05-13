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

#define STATE_GAME_STARTED     1
#define STATE_SEND_CARDS       2
#define STATE_WAIT_PASS        3
#define STATE_WAIT_PLAY        4
#define STATE_WAIT_BOT         5
#define STATE_END_TURN         6
#define STATE_END_ROUND        7
#define STATE_SHUFFLE          8
#define STATE_WAIT_ROUND       9
#define STATE_GAME_OVER        10
#define STATE_CORRUPTED        11

#define STATUS_PASSING         0
#define STATUS_PASSED          1
#define STATUS_PLAYING         2
#define STATUS_YOUR_TURN       3
#define STATUS_WAITING         4

const int no_trick_bonus     = 5;
const int omnibus_bonus      = 10;

class cGame {
public:
  cGame( int f );
  ~cGame();

private:
  void Generate_Cards();
  void Sort();
  usINT state;
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
  usINT played[4];

  struct timeval wait_time;
  char str_cards[4][80];    // 3 * 13 + 13 + 1 = 53 == 3 char for code 127 = empty cards * 13 cards + 13 spaces + 1 null char
  bool game_started;
  bool game_over;
  bool game_draw;
  bool passing_over;
  bool heart_broken;
  bool shoot_moon;
  bool moon_add;
  bool jack_diamond;
  bool has_card[4][DECK_SIZE];
  int flags;
  int delay;

public:
  void Start();
  void Wait(int cs_delay);
  void SetState(usINT s);
  void Pass(cTable &table);
  void Play(cTable &table, usINT card);
  void ForcePass(cTable *table, usINT chair);
  void ForcePlay(cTable &table);
  void ResetRound();
  void ResetPlayed();
  void EndRound(cTable &table);
  void EndTurn(cTable &table);
  void Show(usINT chair);
  void SetMoon(bool add);
  usINT Turn();
  usINT State();
  usINT PassTo();
  usINT Cards(usINT player, usINT card);
  usINT Num_Cards(usINT chair);
  usINT PlayerPass(cTable &table, usINT chair, usINT card1, usINT card2, usINT card3);
  usINT ValidMove(usINT chair, usINT card);
  usINT Played(usINT chair);
  usINT CMD_Rank(usINT chair);
  usINT WhoMoon();
  usINT Score(usINT chair);
  usINT HandScore(usINT chair);
  usINT TimeLeft(usINT chair);
  usINT Status(usINT chair);
  char *Str_Cards(usINT chair);
  bool Started();
  bool Passed(usINT chair);
  bool MyTurn(usINT chair);
  bool ValidMove(cDescriptor &d, usINT chair, usINT card);
  bool AdvanceTurn(cTable &table);
  bool Passing();
  bool Ready();
  bool HeartBroken();
  bool WaitOver();
};
#endif
