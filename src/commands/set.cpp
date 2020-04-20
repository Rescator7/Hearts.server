#include <cstdio>  // sscanf
#include <cstring> // strcmp
#include "../define.h"
#include "../config.h"
#include "../datagrams.h"
#include "set.h"

void cSet::Execute( cDescriptor &d, cParam &param )
{
  char cmd[20 + 1]; // sscanf (%20s) will require + 1 for NULL terminator
  int ret, value;
 
  if (!*param.arguments) {
    d.Socket_Write("%s port = %d\r\n"
		   "nice = %d\r\n"
		   "wait_select = %d\r\n"
                   "wait_pass = %d\r\n"
		   "wait_play = %d\r\n"
		   "wait_end_turn = %d\r\n"
		   "wait_end_round = %d\r\n"
		   "wait_moon = %d\r\n"
		   "gameover_score = %d\r\n", COMMAND_SET, config.Port(), config.Nice(), config.Wait_Select(), config.Wait_Pass(),
		                              config.Wait_Play(), config.Wait_End_Turn(), config.Wait_End_Round(),
					      config.Wait_Moon(), config.GameOver_Score());

    return;
  }

  ret = sscanf(param.arguments, "%20s %d", cmd, &value);

  if (ret == 2) {
    if (!strcmp(cmd, "port"))
      config.Set(OPT_PORT, value);
    else
    if (!strcmp(cmd, "nice"))
      config.Set(OPT_NICE, value);
    else
    if (!strcmp(cmd, "wait_select"))
      config.Set(OPT_WAIT_SELECT, value);
    else
    if (!strcmp(cmd, "wait_pass"))
      config.Set(OPT_WAIT_PASS, value);
    else
    if (!strcmp(cmd, "wait_play"))
      config.Set(OPT_WAIT_PLAY, value);
    else
    if (!strcmp(cmd, "wait_end_round"))
      config.Set(OPT_WAIT_END_ROUND, value);
    else
    if (!strcmp(cmd, "wait_end_turn"))
      config.Set(OPT_WAIT_END_TURN, value);
    else
    if (!strcmp(cmd, "wait_moon"))
      config.Set(OPT_WAIT_MOON, value);
    else
    if (!strcmp(cmd, "gameover_score"))
      config.Set(OPT_GAMEOVER_SCORE, value);
    else {
      d.Socket_Write(COMMAND_WRONG_PARAMETER);
      return;
    } 
    d.Socket_Write("%s %s = %d", COMMAND_SET, cmd, value); 
    return;
  }
  d.Socket_Write(COMMAND_WRONG_PARAMETER);
}
