#include <cstdlib>
#include "../define.h"
#include "../datagrams.h"
#include "../player.h"
#include "../game.h"
#include "play.h"

void cPlay::Execute( cDescriptor &d, cParam &param )
{
  struct cPlayer *player;
  struct cTable *table;

  if (!*param.arguments) return;

  if ((player = d.player) == nullptr) return;

  if ((table = player->table) == nullptr) {
    d.Socket_Write(DGE_PLAYER_NO_TABLE);
    return;
  }

  int chair = table->Chair(d);
  if (chair == PLAYER_NOWHERE) {
    d.Socket_Write(DGE_PLAYER_NOT_SAT);
    return;
  }

  struct cGame *game = table->game;
  if (!game->Started()) {
    d.Socket_Write(DGE_TABLE_NOT_STARTED);
    return;
  }

  if (table->Paused()) {
    d.Socket_Write(DGI_TABLE_PAUSED);
    return;
  }

  if (game->Played(chair) != empty) {
    d.Socket_Write(DGE_TABLE_ALREADY_PLAYED);
    return;
  }

  int card = atoi(param.arguments);

#ifdef DEBUG
  game->Show(chair);
#endif

  if (!game->ValidMove(d, chair, card))
    return;

  table->Adjust_Time_Bank(chair);

  game->Play(*table, card);
}
