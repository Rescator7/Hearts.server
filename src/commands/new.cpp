#include "../define.h"
#include "../errors.h"
#include "../player.h"
#include "../global.h"
#include "new.h"
#include <stdio.h>
#include <stdlib.h>

void cNew::Execute( cDescriptor & d, cParam & param )
{
 if (!d.player->table) {
   cTable *table = new cTable(d);

   d.player->table = table;
   table->set_flags(atoi(param.arguments));
   table_list->Add(table);   
   d.Socket_Write(PLAYER_CHOOSE_CHAIR);
 } else
     d.Socket_Write(PLAYER_AT_TABLE);
}
