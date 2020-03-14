#include <stdio.h>
#include "../define.h"
#include "../player.h"
#include "../errors.h"
#include "../global.h"
#include "../commands.h"
#include "join.h"

extern cCommandsStack cmd;

void cJoin::Execute( cDescriptor &d, cParam &param )
{
 cTable *table;

 if (d.player->table != nullptr) {
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

 d.player->table = table;
 d.Socket_Write("%s %d", PLAYER_CHOOSE_CHAIR, table->TableID());
 table->Sat(d);

 if (ret == 2) {
   char buf[7]; 
   sprintf(buf, "sit %c", direction);
   cmd.Process_Command(&d, (char *)&buf);
 }
   
}
