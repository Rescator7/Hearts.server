#include "../define.h" 
#include "../player.h"
#include "../datagrams.h"
#include "mute.h"

void cMute::Execute( cDescriptor &d, cParam &param )
{
 struct cPlayer *player = d.player;

 if (player == nullptr) return;

 struct cTable *table = player->table;

 if (table == nullptr) {
   d.Socket_Write(DGE_PLAYER_NO_TABLE);
   return;
 }

 if (table->Muted()) return;

 if (table->PlayerSat(d))  {
   table->Mute();
   table->SendAll("%s", DGE_TABLE_MUTED);
 } else
     d.Socket_Write(DGE_PLAYER_NOT_SAT);
}
