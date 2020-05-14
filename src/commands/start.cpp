#include <cstdio>
#include "../define.h"
#include "../datagrams.h"
#include "../player.h"
#include "../game.h"
#include "../global.h"
#include "start.h"

void cStart::Execute( cDescriptor &d, cParam &param )
{
  struct cPlayer *player = d.player;

  if (player == nullptr) return;

  struct cTable *table = player->table; 
  
  if (table == nullptr) {
    d.Socket_Write(DGE_PLAYER_NO_TABLE);
    return;
  }

  int chair = table->Chair(d);
  if (chair == PLAYER_NOWHERE) {
    d.Socket_Write(DGE_PLAYER_NOT_SAT);
    return;
  }

  struct cGame *game = table->game;
  if (game->Started()) {
    d.Socket_Write(DGE_TABLE_STARTED);
    return;
  }

  if (!table->Owner(d)) {
    d.Socket_Write(DGE_TABLE_NOT_OWNER);
    return;
  }

  table->Bot();
  game->Start();
}
