#ifndef _CONFIG_
#define _CONFIG_

extern class cConfig config;

#define OPT_NONE           0
#define OPT_PORT           1
#define OPT_NICE           2
#define OPT_WAIT_SELECT    3
#define OPT_WAIT_PASS      4
#define OPT_WAIT_PLAY      5
#define OPT_WAIT_BOT       6
#define OPT_WAIT_END_TURN  7
#define OPT_WAIT_END_ROUND 8
#define OPT_WAIT_MOON      9
#define OPT_GAMEOVER_SCORE 10
#define OPT_IDLENESS       11

class cConfig {

public:
  cConfig();
  ~cConfig();

private:
  int port;
  int nice;
  int wait_select;
  int wait_pass;
  int wait_play;
  int wait_bot;
  int wait_end_turn;
  int wait_end_round;
  int wait_moon;
  int gameover_score;
  int idleness;

  void init();

public:
  int Port();
  int Nice();
  int Wait_Select();
  int Wait_Pass();
  int Wait_Play();
  int Wait_Bot();
  int Wait_End_Turn();
  int Wait_End_Round();
  int Wait_Moon();
  int GameOver_Score();
  int Idleness();
  void Set(int opt, int value);
};

#endif // _CONFIG_
