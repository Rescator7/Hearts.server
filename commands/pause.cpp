#include "../define.h"
#include "../player.h"
#include "../table.h"
#include "../game.h"
#include "../config.h"
#include "../datagrams.h"
#include "pause.h"

void cPause::Execute( cDescriptor &d, cParam &param )
{
  struct cPlayer *player = d.player;

  if (player == nullptr) return;

  struct cTable *table = player->table;

  if (table == nullptr) {
    d.Socket_Write(DGE_PLAYER_NO_TABLE);
    return;
  }
 
  struct cGame *game = table->game;

  if (!game->Started()) {
    d.Socket_Write(DGE_TABLE_NOT_STARTED);
    return;
  }

  int pause = table->Paused() ^ 1;

  if (pause)
    table->SendAll(DGI_TABLE_PAUSED);
  else { 
    table->SendAll(DGI_TABLE_UNPAUSED);
    game->Wait(config.Wait_Pass());
  }

  table->Pause(pause);
}
