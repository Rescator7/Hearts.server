#ifndef _GAME_
#define GAME

#define NO_ERROR               0
#define ERROR_CARDS_PASSED     1
#define ERROR_ILLEGAL_CARD     2
#define ERROR_CARD_NOT_FOUND   3
#define ERROR_DOUBLE_CARD      4
#define ERROR_PLAYING          5
#define ERROR_NOT_YOUR_TURN    6
#define ERROR_QUEEN_SPADE      7
#define ERROR_BREAK_HEART      8
#define ERROR_WRONG_SUIT       9

#define STATE_SEND_CARDS       1
#define STATE_WAIT_PASS        2
#define STATE_FORCE_PASS       3
#define STATE_WAIT_TWO_CLUBS   4
#define STATE_FORCE_TWO_CLUBS  5
#define STATE_WAIT_PLAY        6
#define STATE_FORCE_PLAY       7

#define WAIT_SELECT_CARDS      10
#define WAIT_PASSED_CARDS      2
#define WAIT_PLAY_TWO_CLUBS    3 // 5 should be fine for release
#define WAIT_PLAY_CARD         4 // 10 or 15 for release

class cGame {
public:
  cGame( int f );
  ~cGame();

private:
  bool game_started;
  bool game_over;
  bool game_draw;
  bool playing;
  bool heart_broken;
  bool first_card;
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
  usINT won_hand;
  usINT best_card;
  usINT won_jack_diamond;
  bool jack_diamond;
  char str_cards[4][80];    // 3 * 13 + 13 + 1 = 53 == 3 char for code 127 = empty cards * 13 cards + 13 spaces + 1 null char
  void generate_cards();
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
  void ResetPassed();
  void Run();
  bool MyTurn(usINT chair);
  bool ValidMove(cDescriptor &d, usINT chair, usINT card);
  usINT ValidMove(usINT chair, usINT card);
  void Play(usINT chair, usINT card);
  bool AdvanceTurn(cTable &table);
  bool Played(usINT chair);
  void ResetPlayed();
};
#endif
