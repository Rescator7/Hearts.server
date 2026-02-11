#include "config.h"
#include "define.h"
#include "sql.h"
#include "log.h"

cConfig::cConfig()
{
 int fields = 0;

 init();

 if (sql.query("select * from config")) {
   fields = sql.Num_Fields();
   if (fields != 12) {
     Log.Write("SYSERR: Server configuration. Num_Fields: %d", fields);
     return;
   }
//   port = atoi(sql.get_row(0));
   port = 5050;
   nice = atoi(sql.get_row(1));
   wait_select = atoi(sql.get_row(2));
   wait_pass = atoi(sql.get_row(3));
   wait_play = atoi(sql.get_row(4));
   wait_bot = atoi(sql.get_row(5));
   wait_end_turn = atoi(sql.get_row(6));
   wait_end_round = atoi(sql.get_row(7));
   wait_moon = atoi(sql.get_row(8));
   time_bank = atoi(sql.get_row(9));
   gameover_score = atoi(sql.get_row(10));
   idleness = atoi(sql.get_row(11));
   Log.Write("Configuration parameters loaded");
 } else {
     Log.Write("SYSERR: Server configuration failed");
   }
}

cConfig::~cConfig()
{
}

void cConfig::init()
{
 port = 4000;
 nice = 2;
 wait_select = 700;
 wait_pass = 700;
 wait_play = 350;
 wait_bot = 25;
 wait_end_turn = 200;
 wait_end_round = 200;
 wait_moon = 800;
 time_bank = 6000;
 gameover_score = 100;
 idleness = 3600;
}

void cConfig::Set(int opt, int value)
{
  switch (opt) {
    case OPT_PORT:
	    if (!sql.query("update config set port = %d;", value))
	      Log.Write("SYSERR: config set failed on port");
	    port = value;
	    break;
    case OPT_NICE:
	    if (!sql.query("update config set nice = %d;", value))
	      Log.Write("SYSERR: config set failed on nice");
	    nice = value;
	    break;
    case OPT_WAIT_SELECT:
	    if (!sql.query("update config set wait_select = %d;", value))
	      Log.Write("SYSERR: config set failed on wait_select");
	    wait_select = value;
	    break;
    case OPT_WAIT_PASS:
	    if (!sql.query("update config set wait_pass = %d;", value))
	      Log.Write("SYSERR: config set failed on wait_pass");
	    wait_pass = value;
	    break;
    case OPT_WAIT_PLAY:
	    if (!sql.query("update config set wait_play = %d;", value))
	      Log.Write("SYSERR: config set failed on wait_play");
	    wait_play = value;
	    break;
    case OPT_WAIT_BOT:
	    if (!sql.query("update config set wait_bot = %d;", value))
	      Log.Write("SYSERR: config set failed on wait_bot");
	    wait_bot = value;
	    break;
    case OPT_WAIT_END_TURN:
	    if (!sql.query("update config set wait_end_turn = %d;", value))
	      Log.Write("SYSERR: config set failed on wait_end_turn");
	    wait_end_turn = value;
	    break;
    case OPT_WAIT_END_ROUND:
	    if (!sql.query("update config set wait_end_round = %d;", value))
	      Log.Write("SYSERR: config set failed on wait_end_round");
	    wait_end_round = value;
	    break;
    case OPT_WAIT_MOON:
	    if (!sql.query("update config set wait_moon = %d;", value))
	      Log.Write("SYSERR: config set failed on wait_moon");
	    wait_moon = value;
	    break;
    case OPT_TIME_BANK:
	    if (!sql.query("update config set time_bank = %d;", value))
	      Log.Write("SYSERR: config set failed on time_bank");
	    time_bank = value;
	    break;
    case OPT_GAMEOVER_SCORE:
	    if (!sql.query("update config set gameover_score = %d;", value))
	      Log.Write("SYSERR: config set failed on gameover_score");
	    gameover_score = value;
	    break;
    case OPT_IDLENESS:
	    if (!sql.query("update config set idleness = %d;", value))
	      Log.Write("SYSERR: config set failed on idleness");
	    idleness = value;
	    break;
  }
}

int cConfig::Port()
{
  return port;
}

int cConfig::Nice()
{
  return nice;
}

int cConfig::Wait_Select()
{
  return wait_select;
}

int cConfig::Wait_Pass()
{
  return wait_pass;
}

int cConfig::Wait_Play()
{
  return wait_play;
}

int cConfig::Wait_End_Turn()
{
  return wait_end_turn;
}

int cConfig::Wait_End_Round()
{
  return wait_end_round;
}

int cConfig::Wait_Moon()
{
  return wait_moon;
}

int cConfig::Time_Bank()
{
  return time_bank;
}

int cConfig::Wait_Bot()
{
  return wait_bot;
}

int cConfig::GameOver_Score()
{
  return gameover_score;
}

int cConfig::Idleness()
{
  return idleness;
}
