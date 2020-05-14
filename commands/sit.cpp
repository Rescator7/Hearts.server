#include <cstdio>
#include <cstring>
#include "../define.h"
#include "../datagrams.h"
#include "../player.h"
#include "sit.h"

void cSit::Execute( cDescriptor &d, cParam &param )
{
  struct cPlayer *player = d.player;

  if (player == nullptr) return;

  struct cTable *table = player->table;

  if (table == nullptr) {
    d.Socket_Write(DGE_PLAYER_NO_TABLE);
    return;
  }

  if (!*param.arguments) {
    table->Stand(d, true);
    return;
  }

  usINT chair;

  Direction(param.arguments[0], chair);
  if (chair == PLAYER_NOWHERE) {
    d.Socket_Write(DGE_TABLE_WRONG_CHAIR);
    return;
  }

  table->Sit(d, chair);
}
