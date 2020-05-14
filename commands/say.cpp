#include "../define.h"
#include "../player.h"
#include "../datagrams.h"
#include "say.h"

void cSay::Execute( cDescriptor &d, cParam &param )
{
  if (!*param.arguments) return;

  struct cPlayer *player = d.player;

  if (player == nullptr) return;

  struct cTable *table = player->table;

  if (table == nullptr) {
    d.Socket_Write(DGE_PLAYER_NO_TABLE);
    return;
  }

  if (table->Chair(d) == PLAYER_NOWHERE) {
    d.Socket_Write(DGE_PLAYER_NOT_SAT);
    return;
  }

  table->Say(d, param.arguments);
}
