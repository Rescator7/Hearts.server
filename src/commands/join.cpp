#include <cstdio>
#include "../define.h"
#include "../player.h"
#include "../game.h"
#include "../datagrams.h"
#include "../global.h"
#include "../commands.h"
#include "join.h"

void cJoin::Execute( cDescriptor &d, cParam &param )
{
  struct cPlayer *player = d.player;

  if (player == nullptr) return;

  if (!*param.arguments) return;

  if (player->table != nullptr) {
    d.Socket_Write(DGE_PLAYER_AT_TABLE);
    return;
  } 
 
  int id, ret;
  char direction;

  if (!(ret = sscanf(param.arguments, "%d %c", &id, &direction))) return;

  cTable *table;

  if ((table = table_list->Search(id)) == nullptr) {
    d.Socket_Write("%s %d", DGE_TABLE_NOT_FOUND, id);
    return;
  }

  if (table->Full()) {
    d.Socket_Write(DGE_TABLE_FULL);
    return;
  }

  struct cGame *game = table->game;

  if (game->Started()) {
    d.Socket_Write(DGE_TABLE_STARTED);
    return;
  }

  int chair = PLAYER_NOWHERE;
  if (ret == 2) {
    Direction(direction, chair);
    if (chair == PLAYER_NOWHERE) {
      d.Socket_Write(DGE_TABLE_WRONG_CHAIR);
      return;
    }
  }

  player->table = table;

  d.Socket_Write("%s %d", DGI_PLAYER_CHOOSE_CHAIR, table->TableID());

  table->Sat(d);

  bool sat = false;
  if (chair != PLAYER_NOWHERE)
    sat = table->Sit(d, chair);

  if (!sat)
    table->AutoSit(d);
}
