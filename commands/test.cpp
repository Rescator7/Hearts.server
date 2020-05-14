#include <cstdlib>
#include "../define.h"
#include "../datagrams.h"
#include "../player.h"
#include "../game.h"
#include "test.h"

void cTest::Execute( cDescriptor &d, cParam &param )
{
  struct cPlayer *player = d.player;

  if (player == nullptr) return;

  struct cTable *table = player->table;

  if (table == nullptr) {
    d.Socket_Write(DGE_PLAYER_NO_TABLE);
    return;
  }

  int chair = table->Chair(d);

  table->Pause(true);
  d.Socket_Write("%s %d %d", DGI_TABLE_SHOOT_MOON, chair, atoi(param.arguments));
}
