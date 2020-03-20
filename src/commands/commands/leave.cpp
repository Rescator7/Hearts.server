#include "../define.h"
#include "../player.h"
#include "../errors.h"
#include "leave.h"

void cLeave::Execute( cDescriptor &d, cParam &param )
{
  struct cPlayer *player = d.player;

  if (player == nullptr) return;

  struct cTable *table = player->table;

  if (table == nullptr) {
    d.Socket_Write(PLAYER_NO_TABLE);
    return;
  }

  int id = table->TableID();

  table->Stand(d);
  player->table = nullptr;

  d.Socket_Write("%s %d", TABLE_LEAVE, id);
}
