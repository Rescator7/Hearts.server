#include "../define.h"
#include "../datagrams.h"
#include "../player.h"
#include "../game.h"
#include "moon.h"

void cMoon::Execute( cDescriptor &d, cParam &param )
{
  struct cPlayer *player = d.player;

  if (player == nullptr) return;

  struct cTable *table = player->table;

  if (table == nullptr) {
    d.Socket_Write(PLAYER_NO_TABLE);
    return;
  }

  usINT chair = table->Chair(d);
  if (chair == PLAYER_NOWHERE) {
    d.Socket_Write(PLAYER_NOT_SAT);
    return;
  }

  struct cGame *game = table->game;

  if (game->WhoMoon() != chair) {
    d.Socket_Write(PLAYER_NOT_MOON);
    return;
  }

  if (*param.arguments == '-')
    game->SetMoon(false);
  
  game->Wait(0);
}
