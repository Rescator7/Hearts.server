#include <cstdio>
#include <cstdlib>
#include "../define.h"
#include "../datagrams.h"
#include "../player.h"
#include "../global.h"
#include "new.h"

void cNew::Execute( cDescriptor & d, cParam & param )
{
 if (server_shutoff) {
   d.Socket_Write(DGE_SERVER_SHUTOFF);
   return;
 }

 struct cPlayer *player = d.player;

 if (player == nullptr) return;

 struct cTable *table = player->table;

 if (table == nullptr) {
   table = new cTable(d, atoi(param.arguments));

   player->table = table;
   table_list->Add(table);   

   d.Socket_Write("%s %d", DGI_PLAYER_CHOOSE_CHAIR, table->TableID());
 } else
     d.Socket_Write(DGE_PLAYER_AT_TABLE);
}
