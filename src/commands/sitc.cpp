#include <stdio.h>
#include <string.h>
#include "../define.h"
#include "../errors.h"
#include "../player.h"
#include "sitc.h"

void cSitc::Execute( cDescriptor & d, cParam & param )
{
  unsigned int player_position;

  if (!param.arguments[0] || param.arguments[1]) {
    d.Socket_Write(TABLE_WRONG_CHAIR);
    return;
  }

  switch (param.arguments[0]) {
    case 'n': player_position = PLAYER_NORTH; break; 
    case 's': player_position = PLAYER_SOUTH; break;
    case 'w': player_position = PLAYER_WEST; break;
    case 'e': player_position = PLAYER_EAST; break;
    default : d.Socket_Write(TABLE_WRONG_CHAIR);
	      return;
  }

  if (!d.player->table)
    d.Socket_Write(PLAYER_NO_TABLE);
  else
    d.player->table->sit_player(d, player_position);
}
