#include "../define.h"
#include "../player.h"
#include "../datagrams.h"
#include "leave.h"

void cLeave::Execute( cDescriptor &d, cParam &param )
{
  struct cPlayer *player = d.player;

  if (player == nullptr) return;

  struct cTable *table = player->table;

  if (table == nullptr) {
    d.Socket_Write(DGE_PLAYER_NO_TABLE);
    return;
  }

  table->Stand(d, true);
  player->table = nullptr;

  d.Socket_Write("%s %d", DGI_TABLE_LEAVE, table->TableID());
}
