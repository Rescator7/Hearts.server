#ifndef _GAME_
#define GAME

#define ERROR_CARDS_PASSED   1
#define ERROR_ILLEGAL_CARD   2
#define ERROR_CARD_NOT_FOUND 3
#define ERROR_DOUBLE_CARD    4
#define ERROR_PLAYING        5

#define STATE_SEND_CARDS       1
#define STATE_WAIT_PASS        2
#define STATE_FORCE_PASS       3
#define STATE_WAIT_TWO_CLUBS   4
#define STATE_FORCE_TWO_CLUBS  5
#define STATE_WAIT_PLAY        6
#define STATE_FORCE_PLAY       7

class cGame {
public:
  cGame( int f );
  ~cGame();

private:
  bool game_started;
  bool game_over;
  bool game_draw;
  bool playing;
  int flags;
  usINT state;
  time_t wait_time;
  usINT suit;
  usINT passto;
  usINT turn;
  usINT num_cards[4];
  usINT num_passed;
  usINT player_cards[4][13];
  usINT passed_cards[4][3];
  char str_cards[4][80];    // 3 * 13 + 13 + 1 = 53 == 3 char for code 127 = empty cards * 13 cards + 13 spaces + 1 null char
  void generate_cards();
  void Sort();

public:
  usINT Turn();
  void Start();
  time_t Wait();
  void Set_Wait(time_t t);
  char *Str_Cards(usINT player);
  usINT Cards(usINT player, usINT card);
  usINT Num_Cards(usINT player);
  usINT State();
  void SetState(usINT s);
  bool Started();
  usINT PassTo();
  void ForcePass(cTable *table, usINT chair);
  void Pass(cTable &table);
  usINT PlayerPass(cTable &table, usINT chair, usINT card1, usINT card2, usINT card3);
  bool Passed(usINT pid);
  void ResetPassed();
  void Run();
};
#endif
