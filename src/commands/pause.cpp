#include "../define.h"
#include "../player.h"
#include "../table.h"
#include "../game.h"
#include "../datagrams.h"
#include "pause.h"

void cPause::Execute( cDescriptor &d, cParam &param )
{
  struct cPlayer *player = d.player;

  if (player == nullptr) return;

  struct cTable *table = player->table;

  if (table == nullptr) {
    d.Socket_Write(PLAYER_NO_TABLE);
    return;
  }
 
  struct cGame *game = table->game;

  if (!game->Started()) {
    d.Socket_Write(TABLE_NOT_STARTED);
    return;
  }

  int pause = table->Paused() ^ 1;

  table->SendAll(pause ? TABLE_PAUSED : TABLE_UNPAUSED);
  table->Pause(pause);
}
