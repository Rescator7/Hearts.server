#include <stdlib.h>
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
    d.Socket_Write(PLAYER_NO_TABLE);
    return;
  }

  int chair = table->Chair(d);

  d.Socket_Write("%s %d %d", TABLE_SHOOT_MOON, chair, atoi(param.arguments));
}
