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
   if (fields != 9) {
     Log.Write("SYSERR: Server configuration. Num_Fields: %d", fields);
     return;
   }
   port = atoi(sql.get_row(0));
   nice = atoi(sql.get_row(1));
   wait_select = atoi(sql.get_row(2));
   wait_pass = atoi(sql.get_row(3));
   wait_play = atoi(sql.get_row(4));
   wait_end_turn = atoi(sql.get_row(5));
   wait_end_round = atoi(sql.get_row(6));
   wait_moon = atoi(sql.get_row(7));
   gameover_score = atoi(sql.get_row(8));
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
 wait_select = 10;
 wait_pass = 2;
 wait_play = 2;
 wait_end_turn = 4;
 wait_end_round = 4;
 wait_moon = 10;
 gameover_score = 100;
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
    case OPT_GAMEOVER_SCORE:
	    if (!sql.query("update config set gameover_score = %d;", value))
	      Log.Write("SYSERR: config set failed on gameover_score");
	    gameover_score = value;
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

int cConfig::GameOver_Score()
{
  return gameover_score;
}
