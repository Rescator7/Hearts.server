#include "../define.h"
#include "../datagrams.h"
#include "../player.h"
#include "stats.h"

void cStats::Execute( cDescriptor &d, cParam &param )
{
  struct cPlayer *player = d.player;

  if (player == nullptr) return;

  d.Socket_Write("%s %d %d %d %d", GAME_STATS, player->First(), player->Second(), player->Third(), player->Fourth());
}
