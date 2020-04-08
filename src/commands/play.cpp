#include <stdlib.h>
#include "../define.h"
#include "../datagrams.h"
#include "../player.h"
#include "../game.h"
#include "play.h"


#include <stdio.h>

void cPlay::Execute( cDescriptor &d, cParam &param )
{
  struct cPlayer *player;
  struct cTable *table;
  struct cGame *game;

  if (!*param.arguments) return;

  if ((player = d.player) == nullptr) return;

  if ((table = player->table) == nullptr) {
    d.Socket_Write(PLAYER_NO_TABLE);
    return;
  }

  int chair = table->Chair(d);
  if (chair == PLAYER_NOWHERE) {
    d.Socket_Write(PLAYER_NOT_SAT);
    return;
  }

  game = table->game;
  if (!game->Started()) {
    d.Socket_Write(TABLE_NOT_STARTED);
    return;
  }

  if (game->Played(chair)) {
    d.Socket_Write(TABLE_ALREADY_PLAYED);
    return;
  }

  int card = atoi(param.arguments);
  printf("play: chair: %d %d\r\n", chair, card);
  game->Show(chair);
  if (!game->ValidMove(d, chair, card))
    return;

  game->Play(*table, card);
}
