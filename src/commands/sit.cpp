#include <stdio.h>
#include <string.h>
#include "../define.h"
#include "../errors.h"
#include "../player.h"
#include "sit.h"

void cSit::Execute( cDescriptor &d, cParam &param )
{
  struct cPlayer *player = d.player;

  if (player == nullptr) return;

  struct cTable *table = player->table;

  if (table == nullptr) {
    d.Socket_Write(PLAYER_NO_TABLE);
    return;
  }

  if (!param.arguments[0]) {
    table->Stand(d);
    return;
  }

  unsigned int chair;

  switch (param.arguments[0]) {
    case 'n': chair = PLAYER_NORTH; break; 
    case 's': chair = PLAYER_SOUTH; break;
    case 'w': chair = PLAYER_WEST; break;
    case 'e': chair = PLAYER_EAST; break;
    default : d.Socket_Write(TABLE_WRONG_CHAIR);
	      return;
  }

  table->Sit(d, chair);
}