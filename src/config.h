#ifndef _CONFIG_
#define _CONFIG_

extern class cConfig config;

#define OPT_PORT           0
#define OPT_NICE           1
#define OPT_WAIT_SELECT    2
#define OPT_WAIT_PASS      3
#define OPT_WAIT_PLAY      4
#define OPT_WAIT_END_TURN  5
#define OPT_WAIT_END_ROUND 6
#define OPT_WAIT_MOON      7

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
  int wait_end_turn;
  int wait_end_round;
  int wait_moon;

  void init();

public:
  int Port();
  int Nice();
  int Wait_Select();
  int Wait_Pass();
  int Wait_Play();
  int Wait_End_Turn();
  int Wait_End_Round();
  int Wait_Moon();
  void Set(int opt, int value);
};

#endif // _CONFIG_
