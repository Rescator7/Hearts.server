#include <stdio.h>
#include "../define.h"
#include "../player.h"
#include "../game.h"
#include "../errors.h"
#include "../global.h"
#include "../commands.h"
#include "join.h"

extern cCommandsStack cmd;

void cJoin::Execute( cDescriptor &d, cParam &param )
{
  struct cPlayer *player = d.player;

  if (player == nullptr) return;

  cTable *table;

  if (player->table != nullptr) {
    d.Socket_Write(PLAYER_AT_TABLE);
    return;
  } 
 
  int id, ret;
  char direction;

  ret = sscanf(param.arguments, "%d %c", &id, &direction);

  if ((table = table_list->Search(id)) == nullptr) {
    d.Socket_Write("%s %d", TABLE_NOT_FOUND, id);
    return;
  }

  if (table->Full()) {
    d.Socket_Write(TABLE_FULL);
    return;
  }

  if (table->game->Started()) {
    d.Socket_Write(TABLE_STARTED);
    return;
  }

  player->table = table;
  d.Socket_Write("%s %d", PLAYER_CHOOSE_CHAIR, table->TableID());
  table->Sat(d);

  if (ret == 2) {
    char buf[7]; 
    sprintf(buf, "sit %c", direction);
    cmd.Process_Command(&d, (char *)&buf);
  }
   
}
