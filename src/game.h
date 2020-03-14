#ifndef _GAME_
#define GAME

class cGame {
public:
  cGame( int f );
  ~cGame();

private:
  bool game_started;
  bool game_over;
  bool game_draw;
  int flags;
  time_t wait_time;
  usINT suit;
  usINT passto;
  usINT turn;
  usINT player_cards[4][13];
  usINT num_cards[4];
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
  bool Started();
};
#endif
