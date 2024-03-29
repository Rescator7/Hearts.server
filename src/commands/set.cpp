#include <cstdio>  // sscanf
#include <cstring> // strcmp
#include "../define.h"
#include "../config.h"
#include "../datagrams.h"
#include "set.h"

void cSet::Execute( cDescriptor &d, cParam &param )
{
  if (!*param.arguments) {
    d.Socket_Write("%s port = %d\r\n"
		   "nice = %d\r\n"
		   "wait_select = %d cs\r\n"
                   "wait_pass = %d cs\r\n"
		   "wait_play = %d cs\r\n"
		   "wait_bot = %d cs\r\n"
		   "wait_end_turn = %d cs\r\n"
		   "wait_end_round = %d cs\r\n"
		   "wait_moon = %d cs\r\n"
		   "time_bank = %d cs\r\n"
		   "gameover_score = %d\r\n" 
		   "idleness = %d\r\n", DGI_COMMAND_SET, config.Port(), config.Nice(), 
		                                         config.Wait_Select(),
		                                         config.Wait_Pass(), 
						         config.Wait_Play(), 
							 config.Wait_Bot(),
						         config.Wait_End_Turn(), 
							 config.Wait_End_Round(),
					                 config.Wait_Moon(), 
							 config.Time_Bank(),
							 config.GameOver_Score(),
						         config.Idleness());
    return;
  }

  int ret, value;
  char cmd[20 + 1]; // sscanf (%20s) will require + 1 for NULL terminator

  ret = sscanf(param.arguments, "%20s %d", cmd, &value);

  int opt = OPT_NONE;

  if (ret == 2) {
    if (!strcmp(cmd, "port")) opt = OPT_PORT; else
    if (!strcmp(cmd, "nice")) opt = OPT_NICE; else
    if (!strcmp(cmd, "wait_select")) opt = OPT_WAIT_SELECT; else
    if (!strcmp(cmd, "wait_bot")) opt = OPT_WAIT_BOT; else
    if (!strcmp(cmd, "wait_pass")) opt = OPT_WAIT_PASS; else
    if (!strcmp(cmd, "wait_play")) opt = OPT_WAIT_PLAY; else
    if (!strcmp(cmd, "wait_end_round")) opt = OPT_WAIT_END_ROUND; else
    if (!strcmp(cmd, "wait_end_turn")) opt = OPT_WAIT_END_TURN; else
    if (!strcmp(cmd, "wait_moon")) opt = OPT_WAIT_MOON; else
    if (!strcmp(cmd, "time_bank")) opt = OPT_TIME_BANK; else
    if (!strcmp(cmd, "gameover_score")) opt = OPT_GAMEOVER_SCORE; else 
    if (!strcmp(cmd, "idleness")) opt = OPT_IDLENESS; else
    {
      d.Socket_Write(DGE_COMMAND_WRONG_PARAMETER);
      return;
    } 

    switch (opt) {
      case OPT_PORT: 
	      if ((value < 1024) || (value > 9999)) {
		d.Socket_Write("%s %d %d", DGE_WRONG_VALUE, 1024, 9999);
		return;
              }
	      break;
      case OPT_NICE:
	      if ((value < -20) || (value > 19)) {
		d.Socket_Write("%s %d %d", DGE_WRONG_VALUE, -20, 19);
		return;
              }
	      break;
      case OPT_TIME_BANK:
	      if ((value < 0) || (value > 18000)) {
		d.Socket_Write("%s %d %d", DGE_WRONG_VALUE, 0, 18000);
		return;
	      }
	      break;
      case OPT_WAIT_SELECT:
      case OPT_WAIT_PASS:
      case OPT_WAIT_PLAY:
      case OPT_WAIT_BOT:
      case OPT_WAIT_END_ROUND:
      case OPT_WAIT_END_TURN:
      case OPT_WAIT_MOON: 
	      if ((value < 0) || (value > 2500)) {
		d.Socket_Write("%s %d %d", DGE_WRONG_VALUE, 0, 2500);
		return;
              }
	      break;
      case OPT_GAMEOVER_SCORE: 
	      if ((value < 0) || (value > 1000)) { 
		d.Socket_Write("%s %d %d", DGE_WRONG_VALUE, 0, 1000);
		return;
	      }
	      break;
      case OPT_IDLENESS: 
	      if ((value < 10) || (value > 36000)) {
		d.Socket_Write("%s %d %d", DGE_WRONG_VALUE, 10, 36000);
		return;
              }
	      break;
    }

    config.Set(opt, value);

    d.Socket_Write("%s %s = %d", DGI_COMMAND_SET, cmd, value); 
    return;
  }
  d.Socket_Write(DGE_COMMAND_WRONG_PARAMETER);
}
