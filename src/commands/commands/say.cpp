#include "../define.h"
#include "../player.h"
#include "../errors.h"
#include "say.h"

void cSay::Execute( cDescriptor &d, cParam &param )
{
  if (!*param.arguments) return;

  struct cPlayer *player = d.player;

  if (player == nullptr) return;

  struct cTable *table = player->table;

  if (table == nullptr) {
    d.Socket_Write(PLAYER_NO_TABLE);
    return;
  }

  table->Say(d, param.arguments);
}