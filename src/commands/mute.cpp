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
   d.Socket_Write(PLAYER_NO_TABLE);
   return;
 }

 if (table->Muted()) return;

 if (table->PlayerSat(d))  {
   table->Mute();
   table->SendAll("%s", TABLE_MUTED);
 } else
     d.Socket_Write(PLAYER_NOT_SAT);
}
